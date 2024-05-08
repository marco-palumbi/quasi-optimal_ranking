






#include "../src/parsing_permutations.h"
#include "test_utils.h"
#include <string.h>

int main(int argc, char const *argv[])
{

    hal_setup(CLOCK_BENCHMARK);

    perm_t p, q;
    uint8_t p_code[PARAM_N1];

    unsigned long long cycles_tot_encode, cycles_tot_decode, cycles1, cycles2;
    double start, end;
    cycles_tot_encode = 0;
    double time_taken_encode = 0;
    cycles_tot_decode = 0;
    double time_taken_decode = 0;

    printf("Start test <%s> PARAM_N1=%d\n", TARGET_NAME, PARAM_N1);
    for (int i = 0; i < N_ITERATIONS; ++i) {
        perm_sample(p);

        start = (double) clock();
        cycles1 = cpucycles();

        memset(p_code, 0, sizeof(p_code));
        sig_perk_perm_encode(p, p_code, 0);
        cycles2 = cpucycles();
        end = (double)clock();

        time_taken_encode += (end - start) / ((double) CLOCKS_PER_SEC);
        cycles_tot_encode += cycles2 - cycles1;

        start = (double) clock();
        cycles1 = cpucycles();
        sig_perk_perm_decode(p_code, 0, q);
        cycles2 = cpucycles();
        end = (double)clock();

        time_taken_decode += (end - start) / ((double) CLOCKS_PER_SEC);
        cycles_tot_decode += cycles2 - cycles1;

        for (int j = 0; j < PARAM_N1; ++j) {
            if(p[j]!=q[j]) {
                printf("failed\n");

                perm_print(p);
                perm_print(q);
                break;
            }
        }
    }

    printf("Encoding - Size %d, Iterations %d: %10lld ", PARAM_N1, N_ITERATIONS, cycles_tot_encode/N_ITERATIONS);
    printf("cycles");
    printf("\n");
    printf("Time taken %lf\n\n", time_taken_encode/N_ITERATIONS);

    printf("Decoding - Size %d, Iterations %d: %10lld ", PARAM_N1, N_ITERATIONS, cycles_tot_decode/N_ITERATIONS);
    printf("cycles");
    printf("\n");
    printf("Time taken %lf\n\n", time_taken_decode/N_ITERATIONS);

	return 0;
}

// code to generate the precomputation tables[
//    struct bn num;
//    for (int i = 0; i < 80; ++i) {
//        bignum_from_int(&num, i);
//        bignum_factorial(&num, &p_code);
//        printf("{");
//        for (int j = 0; j < BN_ARRAY_SIZE; ++j) {
//            printf("%d,", p_code.array[j]);
//        }
//        printf("},\n");
//    }
//
