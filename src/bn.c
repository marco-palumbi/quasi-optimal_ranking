/*

Big number library - arithmetic on multiple-precision unsigned integers.

This library is an implementation of arithmetic on arbitrarily large integers.

The difference between this and other implementations, is that the data structure
has optimal memory utilization (i.e. a 1024 bit integer takes up 128 bytes RAM),
and all memory is allocated statically: no dynamic allocation for better or worse.

Primary goals are correctness, clarity of code and clean, portable implementation.
Secondary goal is a memory footprint small enough to make it suitable for use in
embedded applications.


The current state is correct functionality and adequate performance.
There may well be room for performance-optimizations and improvements.

*/

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "bn.h"



/* Functions for shifting number in-place. */
static void _lshift_one_bit(struct bn* a);
static void _rshift_one_bit(struct bn* a);
static void _lshift_word(struct bn* a, int nwords);
static void _rshift_word(struct bn* a, int nwords);



/* Public / Exported functions. */
void bignum_init(struct bn* n)
{
    require(n, "n is null");

    int i;
    for (i = 0; i < BN_ARRAY_SIZE; ++i)
    {
        n->array[i] = 0;
    }
}


void bignum_from_int(struct bn* n, DTYPE_TMP i)
{
    require(n, "n is null");

    bignum_init(n);

    /* Endianness issue if machine is not little-endian? */
#ifdef WORD_SIZE
#if (WORD_SIZE == 1)
    n->array[0] = (i & 0x000000ff);
    n->array[1] = (i & 0x0000ff00) >> 8;
    n->array[2] = (i & 0x00ff0000) >> 16;
    n->array[3] = (i & 0xff000000) >> 24;
#elif (WORD_SIZE == 2)
    n->array[0] = (i & 0x0000ffff);
    n->array[1] = (i & 0xffff0000) >> 16;
#elif (WORD_SIZE == 4)
    n->array[0] = i;
    DTYPE_TMP num_32 = 32;
    DTYPE_TMP tmp = i >> num_32; /* bit-shift with U64 operands to force 64-bit results */
    n->array[1] = tmp;
#endif
#endif
}


int bignum_to_int(struct bn* n)
{
    require(n, "n is null");

    int ret = 0;

    /* Endianness issue if machine is not little-endian? */
#if (WORD_SIZE == 1)
    ret += n->array[0];
    ret += n->array[1] << 8;
    ret += n->array[2] << 16;
    ret += n->array[3] << 24;
#elif (WORD_SIZE == 2)
    ret += n->array[0];
    ret += n->array[1] << 16;
#elif (WORD_SIZE == 4)
    ret += (int)n->array[0];
#endif

    return ret;
}

void bignum_add(const struct bn* a, const struct bn* b, struct bn* c)
{
    require(a, "a is null");
    require(b, "b is null");
    require(c, "c is null");

    int carry = 0;
    int i;
    for (i = 0; i < BN_ARRAY_SIZE; ++i)
    {
        DTYPE_TMP tmp;
        tmp = (DTYPE_TMP)a->array[i] + b->array[i] + carry;
        carry = (tmp > MAX_VAL);
        c->array[i] = (tmp & MAX_VAL);
    }
}


void bignum_sub(const struct bn* a, const struct bn* b, struct bn* c)
{
    require(a, "a is null");
    require(b, "b is null");
    require(c, "c is null");

    int borrow = 0;
    int i;
    for (i = 0; i < BN_ARRAY_SIZE; ++i)
    {
        DTYPE_TMP res;
        DTYPE_TMP tmp1;
        DTYPE_TMP tmp2;
        tmp1 = (DTYPE_TMP)a->array[i] + (MAX_VAL + 1); /* + number_base */
        tmp2 = (DTYPE_TMP)b->array[i] + borrow;
        res = (tmp1 - tmp2);
        c->array[i] = (DTYPE)(res & MAX_VAL); /* "modulo number_base" == "% (number_base - 1)" if number_base is 2^N */
        borrow = (res <= MAX_VAL);
    }
}


void bignum_mul(const struct bn* a, const struct bn* b, struct bn* c)
{
    require(a, "a is null");
    require(b, "b is null");
    require(c, "c is null");

    bignum_init(c);

    if (bignum_is_zero(a)) {return;}
    if (bignum_is_zero(b)) {return;}

    struct bn tmp;
    struct bn max_val;
    bignum_from_int(&max_val, MAX_VAL);

    struct bn a1;
    DTYPE_TMP a0;
    struct bn b1;
    DTYPE_TMP b0;
    struct bn z0;
    struct bn z1;
    struct bn z2;

    bignum_rshift(a, &a1, WORD_SIZE*8);
    bignum_rshift(b, &b1, WORD_SIZE*8);
    a0 = (DTYPE_TMP)(a->array[0]);
    b0 = (DTYPE_TMP)(b->array[0]);
    DTYPE_TMP z0_small = a0*b0;

    bignum_init(&z0);
    z0.array[0] = z0_small & MAX_VAL;
    z0.array[1] = (z0_small & (MAX_VAL << (WORD_SIZE*8))) >> (WORD_SIZE*8);
    if (bignum_is_zero(&a1) && bignum_is_zero(&b1)) {
        bignum_assign(c, &z0);
        return;
    }
    bignum_mul(&a1, &b1, &z2);
    struct bn a0_big;
    struct bn b0_big;
    bignum_from_int(&a0_big, a0);
    bignum_from_int(&b0_big, b0);

    struct bn tmpa;
    struct bn tmpb;
    struct bn tmpz1;
    bignum_add(&a1, &a0_big, &tmpa);
    bignum_add(&b1, &b0_big, &tmpb);
    bignum_mul(&tmpa, &tmpb, &tmpz1);
    bignum_sub(&tmpz1, &z2, &tmpa);
    bignum_sub(&tmpa, &z0, &z1);

    bignum_lshift(&z2, &tmp, WORD_SIZE*8);
    bignum_add(&tmp, &z1, c);
    bignum_lshift(c, &tmp, WORD_SIZE*8);
    bignum_add(&tmp, &z0, c);
}


void bignum_div(const struct bn* a, const struct bn* b, struct bn* c)
{
    require(a, "a is null");
    require(b, "b is null");
    require(c, "c is null");

    struct bn current;
    struct bn denom;
    struct bn tmp;

    bignum_from_int(&current, 1);               // int current = 1;
    bignum_assign(&denom, b);                   // denom = b
    bignum_assign(&tmp, a);                     // tmp   = a

    const DTYPE_TMP half_max = 1 + (DTYPE_TMP)(MAX_VAL / 2);
    bool overflow = false;
    while (bignum_cmp(&denom, a) != LARGER)     // while (denom <= a) {
    {
        if (denom.array[BN_ARRAY_SIZE - 1] >= half_max)
        {
            overflow = true;
            break;
        }
        _lshift_one_bit(&current);                //   current <<= 1;
        _lshift_one_bit(&denom);                  //   denom <<= 1;
    }
    if (!overflow)
    {
        _rshift_one_bit(&denom);                  // denom >>= 1;
        _rshift_one_bit(&current);                // current >>= 1;
    }
    bignum_init(c);                             // int answer = 0;

    while (!bignum_is_zero(&current))           // while (current != 0)
    {
        if (bignum_cmp(&tmp, &denom) != SMALLER)  //   if (dividend >= denom)
        {
            bignum_sub(&tmp, &denom, &tmp);         //     dividend -= denom;
            bignum_or(c, &current, c);              //     answer |= current;
        }
        _rshift_one_bit(&current);                //   current >>= 1;
        _rshift_one_bit(&denom);                  //   denom >>= 1;
    }                                           // return answer;
}


void bignum_lshift(const struct bn* a, struct bn* b, int nbits)
{
    require(a, "a is null");
    require(b, "b is null");
    require(nbits >= 0, "no negative shifts");

    bignum_assign(b, a);
    /* Handle shift in multiples of word-size */
    const int nbits_pr_word = (WORD_SIZE * 8);
    int nwords = nbits / nbits_pr_word;
    if (nwords != 0)
    {
        _lshift_word(b, nwords);
        nbits -= (nwords * nbits_pr_word);
    }

    if (nbits != 0)
    {
        int i;
        for (i = (BN_ARRAY_SIZE - 1); i > 0; --i)
        {
            b->array[i] = (b->array[i] << nbits) | (b->array[i - 1] >> ((8 * WORD_SIZE) - nbits));
        }
        b->array[i] <<= nbits;
    }
}


void bignum_rshift(const struct bn* a, struct bn* b, int nbits)
{
    require(a, "a is null");
    require(b, "b is null");
    require(nbits >= 0, "no negative shifts");

    bignum_assign(b, a);
    /* Handle shift in multiples of word-size */
    const int nbits_pr_word = (WORD_SIZE * 8);
    int nwords = nbits / nbits_pr_word;
    if (nwords != 0)
    {
        _rshift_word(b, nwords);
        nbits -= (nwords * nbits_pr_word);
    }

    if (nbits != 0)
    {
        int i;
        for (i = 0; i < (BN_ARRAY_SIZE - 1); ++i)
        {
            b->array[i] = (b->array[i] >> nbits) | (b->array[i + 1] << ((8 * WORD_SIZE) - nbits));
        }
        b->array[i] >>= nbits;
    }

}


void bignum_mod(const struct bn* a, const struct bn* b, struct bn* c)
{
    /*
      Take divmod and throw away div part
    */
    require(a, "a is null");
    require(b, "b is null");
    require(c, "c is null");

    struct bn tmp;

    bignum_divmod(a,b,&tmp,c);
}

void bignum_divmod(const struct bn* a, const struct bn* b, struct bn* c, struct bn* d)
{
    /*
      Puts a%b in d
      and a/b in c

      mod(a,b) = a - ((a / b) * b)

      example:
        mod(8, 3) = 8 - ((8 / 3) * 3) = 2
    */
    require(a, "a is null");
    require(b, "b is null");
    require(c, "c is null");

    struct bn tmp;

    /* c = (a / b) */
    bignum_div(a, b, c);

    /* tmp = (c * b) */
    bignum_mul(c, b, &tmp);

    /* c = a - tmp */
    bignum_sub(a, &tmp, d);
}

void bignum_or(struct bn* a, struct bn* b, struct bn* c)
{
    require(a, "a is null");
    require(b, "b is null");
    require(c, "c is null");

    int i;
    for (i = 0; i < BN_ARRAY_SIZE; ++i)
    {
        c->array[i] = (a->array[i] | b->array[i]);
    }
}

int bignum_cmp(const struct bn *a, const struct bn *b)
{
    require(a, "a is null");
    require(b, "b is null");

    int i = BN_ARRAY_SIZE;
    do
    {
        i -= 1; /* Decrement first, to start with last array element */
        if (a->array[i] > b->array[i])
        {
            return LARGER;
        }
        else if (a->array[i] < b->array[i])
        {
            return SMALLER;
        }
    }
    while (i != 0);

    return EQUAL;
}


int bignum_is_zero(const struct bn* n)
{
    require(n, "n is null");

    int i;
    for (i = 0; i < BN_ARRAY_SIZE; ++i)
    {
        if (n->array[i])
        {
            return 0;
        }
    }

    return 1;
}

void bignum_assign(struct bn* dst, const struct bn* src)
{
    require(dst, "dst is null");
    require(src, "src is null");

    int i;
    for (i = 0; i < BN_ARRAY_SIZE; ++i)
    {
        dst->array[i] = src->array[i];
    }
}


/* Private / Static functions. */
static void _rshift_word(struct bn* a, int nwords)
{
    /* Naive method: */
    require(a, "a is null");
    require(nwords >= 0, "no negative shifts");

    int i;
    if (nwords >= BN_ARRAY_SIZE)
    {
        for (i = 0; i < BN_ARRAY_SIZE; ++i)
        {
            a->array[i] = 0;
        }
        return;
    }

    for (i = 0; i < BN_ARRAY_SIZE - nwords; ++i)
    {
        a->array[i] = a->array[i + nwords];
    }
    for (; i < BN_ARRAY_SIZE; ++i)
    {
        a->array[i] = 0;
    }
}


static void _lshift_word(struct bn* a, int nwords)
{
    require(a, "a is null");
    require(nwords >= 0, "no negative shifts");

    int i;
    /* Shift whole words */
    for (i = (BN_ARRAY_SIZE - 1); i >= nwords; --i)
    {
        a->array[i] = a->array[i - nwords];
    }
    /* Zero pad shifted words. */
    for (; i >= 0; --i)
    {
        a->array[i] = 0;
    }
}


static void _lshift_one_bit(struct bn* a)
{
    require(a, "a is null");

    int i;
    for (i = (BN_ARRAY_SIZE - 1); i > 0; --i)
    {
        a->array[i] = (a->array[i] << 1) | (a->array[i - 1] >> ((8 * WORD_SIZE) - 1));
    }
    a->array[0] <<= 1;
}


static void _rshift_one_bit(struct bn* a)
{
    require(a, "a is null");

    int i;
    for (i = 0; i < (BN_ARRAY_SIZE - 1); ++i)
    {
        a->array[i] = (a->array[i] >> 1) | (a->array[i + 1] << ((8 * WORD_SIZE) - 1));
    }
    a->array[BN_ARRAY_SIZE - 1] >>= 1;
}