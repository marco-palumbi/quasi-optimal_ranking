/**
	utils.h
	
	Authors:
		Alessandro Budroni, May 2019
*/

#include "lehmer_bn.h"
#include "factorial_tables_bn.h"

#ifndef USE_FACTORIAL
#define USE_FACTORIAL 1
#endif

void lehmer_encode(const perm_t in_p, struct bn* code) {

    bignum_from_int(code, 0);

    struct bn tmp, mul;

    for (int i = 0; i < PARAM_N1; ++i) {

        int count = 0;

        for (int j = i+1; j < PARAM_N1; ++j) {
            if (in_p[j] < in_p[i]){
                count++;
            }
        }
#if (USE_FACTORIAL == 1)
        bignum_from_int(&tmp, count);
        bignum_mul(&factorial[PARAM_N1 -i -1], &tmp, &mul);
        bignum_add(code, &mul, code);
#else
        // bignum for computing the rank
        bignum_from_int(&tmp, PARAM_N1-i);
        bignum_mul(code, &tmp, &mul);
        bignum_from_int(&tmp, count);
        bignum_add(&mul, &tmp, code);
#endif
    }
}

void lehmer_decode(struct bn* code, perm_t out_p) {

    struct bn tmp, tmp2, tmp3;

#if (USE_FACTORIAL == 1)
    bignum_mod(code, &factorial[PARAM_N1], &tmp);
    bignum_assign(&tmp3, &tmp);
    bignum_divmod(&tmp3, &factorial[PARAM_N1 -1], &tmp, &tmp2);
    out_p[0] = bignum_to_int(&tmp);

    for (int i = 1; i < PARAM_N1-1; ++i) {
        bignum_assign(&tmp3, &tmp2);
        bignum_divmod(&tmp3, &factorial[PARAM_N1 -i -1], &tmp, &tmp2);
        out_p[i] = bignum_to_int(&tmp);
    }

    bignum_div(&tmp2, &factorial[0], &tmp);
    out_p[PARAM_N1-1] = bignum_to_int(&tmp);
#else
    out_p[PARAM_N1 - 1] = 0;
    for (int i = 2; i < PARAM_N1; i++) {
        bignum_from_int(&tmp3, i);
        bignum_divmod(code, &tmp3, &tmp2, &tmp);
        bignum_assign(code, &tmp2);
        out_p[PARAM_N1 - i] = bignum_to_int(&tmp);
    }
    out_p[0] = bignum_to_int(code);
#endif

    uint8_t used[PARAM_N1] = {0};
    for (int i = 0; i < PARAM_N1; ++i) {
        int count = 0;
        for (int j = 0; j < PARAM_N1; ++j) {
            if (!used[j])
                count += 1;
            if (count == out_p[i] + 1) {
                out_p[i] = j;
                used[j] = 1;
                break;
            }
        }
    }
}