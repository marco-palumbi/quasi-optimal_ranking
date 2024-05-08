






#include "../src/fast.h"
#include "test_utils.h"

int main(int argc, char const *argv[])
{

    hal_setup(CLOCK_BENCHMARK);

    perm_t p[2], q[2];
    uint8_t p_code[PARAM_N1 * 2];

    unsigned long long cycles_tot_encode, cycles_tot_decode, cycles1, cycles2;
    double start, end;
    cycles_tot_encode = 0;
    double time_taken_encode = 0;
    cycles_tot_decode = 0;
    double time_taken_decode = 0;

    printf("Start test <%s> PARAM_N1=%d\n", TARGET_NAME, PARAM_N1);
    for (int i = 0; i < N_ITERATIONS; ++i) {
        perm_sample(p[0]);
        perm_sample(p[1]);

        start = (double) clock();
        cycles1 = cpucycles();

        fast_encode_2perm(p, p_code);
        cycles2 = cpucycles();
        end = (double)clock();

        time_taken_encode += (end - start) / ((double) CLOCKS_PER_SEC);
        cycles_tot_encode += cycles2 - cycles1;

        start = (double) clock();
        cycles1 = cpucycles();
        fast_decode_2perm(p_code, q);
        cycles2 = cpucycles();
        end = (double)clock();

        time_taken_decode += (end - start) / ((double) CLOCKS_PER_SEC);
        cycles_tot_decode += cycles2 - cycles1;

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < PARAM_N1; ++j) {
                if(p[i][j]!=q[i][j]) {
                    printf("failed\n");

                    perm_print(p[i]);
                    perm_print(q[i]);
                    break;
                }
            }
        }
    }

    printf("Encoding - Size %d, Iterations %d: %10lld ", PARAM_N1, N_ITERATIONS, cycles_tot_encode/N_ITERATIONS/2);
    printf("cycles");
    printf("\n");
    printf("Time taken %lf\n\n", time_taken_encode/N_ITERATIONS/2);

    printf("Decoding - Size %d, Iterations %d: %10lld ", PARAM_N1, N_ITERATIONS, cycles_tot_decode/N_ITERATIONS/2);
    printf("cycles");
    printf("\n");
    printf("Time taken %lf\n\n", time_taken_decode/N_ITERATIONS/2);

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
