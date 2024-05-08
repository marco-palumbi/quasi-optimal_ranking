//
// Created by Alessandro Budroni on 24/08/2023.
//

#include "bonet_gmp.h"
#include "factorial_tables_gmp.h"
#define GMP_PARAM_N1_FACTORIAL "2nCw64C9IVjZrkOWbyzvBzVD6yGgVEfyjgxGyjc9GRbWRqxGya9YVk3hNIr3EcYC00"

#ifndef USE_FACTORIAL
#define USE_FACTORIAL 0
#endif

void bonet_encode(const perm_t in_p, mpz_t code) {

    mpz_t mul;
    mpz_init(mul);

    uint8_t T[PARAM_LEN_T] = {0};
    mpz_set_si(code, 0);
    for (int i = 0; i < PARAM_N1; ++i) {
        uint8_t ctr = in_p[i];
        uint16_t node = PARAM_2toK1 + in_p[i];
        for (int j = 0; j < PARAM_K1; ++j) {
            if (node & 0x1) {
                ctr -= T[(node >> 1) << 1];
            }
            T[node] += 1;
            node = node >> 1;
        }
        T[node] += 1;
        // bignum for computing the rank
        mpz_mul_ui(mul, code, PARAM_N1-i);
        mpz_add_ui(code, mul, ctr);
    }
    mpz_clear(mul);
}

void bonet_decode(mpz_t code, perm_t out_p) {

    mpz_t tmp, tmp2, fact;
    mpz_inits(tmp,tmp2,fact, NULL);

    mpz_set_str(fact, factorial[PARAM_N1], 62);
    // mpz_set_str(fact, GMP_PARAM_N1_FACTORIAL, 62);
    if (mpz_cmp(fact, code) < 1) {
        mpz_clears(tmp,tmp2,fact, NULL);
        printf("ERROR\n");
        return;
    }


#if (USE_FACTORIAL == 1)
    mpz_mod(tmp, code, fact);
    mpz_set_str(fact, factorial[PARAM_N1 -1], 62);
    mpz_divmod(tmp, tmp2, tmp, fact);
    out_p[0] = mpz_get_si(tmp);

    for (int i = 1; i < PARAM_N1 -1; ++i) {
        mpz_set_str(fact, factorial[PARAM_N1 -i -1], 62);
        mpz_divmod(tmp, tmp2, tmp2, fact);
        out_p[i] = mpz_get_si(tmp);
    }

    mpz_set_str(fact, factorial[0], 62);
    mpz_div(tmp, tmp2, fact);
    out_p[PARAM_N1-1] = mpz_get_si(tmp);
#else
    out_p[PARAM_N1 - 1] = 0;
    for (int i = 2; i < PARAM_N1; i++) {
        mpz_divmod_ui(code, tmp, code, i);
        out_p[PARAM_N1 - i] = mpz_get_si(tmp);
    }
    out_p[0] = mpz_get_si(code);
#endif


    uint8_t T[PARAM_LEN_T] = {0};
    for (int i = 0; i <= PARAM_K1; ++i) {
        int pw = (1 << i);
        for (int j = 0; j < pw; ++j) {
            T[(pw) +j -1] = 1 << (PARAM_K1-i);
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
    mpz_clears(tmp,tmp2,fact, NULL);
}