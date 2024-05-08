/**
	main.c
	This is an example

	Authors:
		- Alessandro Budroni, May 2019
*/

#include "../src/lehmer_bn.h"
#include "test_utils.h"

int main(int argc, char const *argv[])
{

    hal_setup(CLOCK_BENCHMARK);

    perm_t p, q;
    struct bn p_code;

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
        bignum_init(&p_code);
        lehmer_encode(p, &p_code);
        cycles2 = cpucycles();
        end = (double)clock();

        time_taken_encode += (end - start) / ((double) CLOCKS_PER_SEC);
        cycles_tot_encode += cycles2 - cycles1;

        start = (double) clock();
        cycles1 = cpucycles();
        lehmer_decode(&p_code, q);
        cycles2 = cpucycles();
        end = (double)clock();

        time_taken_decode += (end - start) / ((double) CLOCKS_PER_SEC);
        cycles_tot_decode += cycles2 - cycles1;

        for (int j = 0; j < PARAM_N1; ++j) {
            if(p[j]!=q[j]) {
                printf("failed\n");
                printf("code = %d \n", bignum_to_int(&p_code));
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
