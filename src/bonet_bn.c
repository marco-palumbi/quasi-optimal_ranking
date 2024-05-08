//
// Created by Alessandro Budroni on 24/08/2023.
//

#include "bonet_bn.h"
#include "factorial_tables_bn.h"

#ifndef USE_FACTORIAL
#define USE_FACTORIAL 1
#endif

void bonet_encode(const perm_t p, struct bn* code) {

    bignum_from_int(code, 0);

    struct bn tmp, mul;

    uint8_t T[PARAM_LEN_T] = {0};
    for (int i = 0; i < PARAM_N1; ++i) {
        uint8_t ctr = p[i];
        uint16_t node = PARAM_2toK1 + p[i];
        for (int j = 0; j < PARAM_K1; ++j) {
            if (node & 0x1) {
                ctr -= T[(node >> 1) << 1];
            }
            T[node] += 1;
            node = node >> 1;
        }
        T[node] += 1;
#if (USE_FACTORIAL == 1)
        bignum_from_int(&tmp, ctr);
        bignum_mul(&factorial[PARAM_N1 -i -1], &tmp, &mul);
        bignum_add(code, &mul, code);
#else
        // bignum for computing the rank
        bignum_from_int(&tmp, PARAM_N1-i);
        bignum_mul(code, &tmp, &mul);
        bignum_from_int(&tmp, ctr);
        bignum_add(&mul, &tmp, code);
#endif
    }
}

void bonet_decode(struct bn* code, perm_t out_p) {

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

    uint8_t T[PARAM_LEN_T] = {0};
    for (int i = 0; i <= PARAM_K1; ++i) {
        for (int j = 0; j < (1 << i); ++j) {
            T[((1 << i)) +j -1] = 1 << (PARAM_K1-i);
        }
    }

    for (int i = 0; i < PARAM_N1; ++i) {
        int digit = out_p[i];
        uint16_t node = 1;
        for (int j = 0; j < PARAM_K1; ++j) {
            T[node] -= 1;
            node <<= 1;
            if (digit >= T[node]){
                digit -= T[node];
                node += 1;
            }
        }
        T[node] = 0;
        out_p[i] = node - (1 << PARAM_K1);
    }
}


