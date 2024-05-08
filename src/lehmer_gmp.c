/**
	utils.h
	
	Authors:
		Alessandro Budroni, May 2019
*/

#include "lehmer_gmp.h"
#include "factorial_tables_gmp.h"
#define GMP_PARAM_N1_FACTORIAL "2nCw64C9IVjZrkOWbyzvBzVD6yGgVEfyjgxGyjc9GRbWRqxGya9YVk3hNIr3EcYC00"

#ifndef USE_FACTORIAL
#define USE_FACTORIAL 0
#endif

void lehmer_encode(const perm_t in_p, mpz_t code) {

    mpz_t mul;
    mpz_init(mul);

    mpz_set_si(code, 0);


    for (int i = 0; i < PARAM_N1; ++i) {

        int count = 0;

        for (int j = i+1; j < PARAM_N1; ++j) {
            if (in_p[j] < in_p[i]){
                count++;
            }
        }
        // bignum for computing the rank
        mpz_mul_ui(mul, code, PARAM_N1-i);
        mpz_add_ui(code, mul, count);
    }
    mpz_clear(mul);
}

void lehmer_decode(mpz_t code, perm_t out_p) {

    mpz_t tmp, tmp2, fact;
    mpz_init(tmp);
    mpz_init(tmp2);
    mpz_init(fact);

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
    mpz_clears(tmp,tmp2,fact, NULL);
}