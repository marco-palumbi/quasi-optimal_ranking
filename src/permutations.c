//
// Created by Alessandro Budroni on 23/08/2023.
//

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "hal.h"

#include "permutations.h"

#if 0 //use hw_randombytes
void get_seed(uint8_t *seed)
{
    // get seed for RNG
    int randomData = open("/dev/urandom", O_RDONLY);
    if(randomData  < 0){
        printf("Could not open /dev/urandom\n");
        exit(EXIT_FAILURE);
    }
    int ret;
    ret = read(randomData, seed, 1);
    if(ret  < 0){
        printf("Could not read from /dev/urandom\n");
        exit(EXIT_FAILURE);
    }
    close(randomData);
}
#endif

void perm_sample(perm_t p) {

    int seed;
    // get_seed(&seed);
    hw_randombytes((uint8_t *)&seed, sizeof(seed));
    srand(seed);

    uint8_t buffer[PARAM_N1];
    for (int i = PARAM_N1-1; i >= 0; i--) {
        buffer[i] = i + (rand() % (PARAM_N1 -i));
        p[i] = p[buffer[i]];
        p[buffer[i]] = i;
    }
}

void perm_print(const perm_t p) {
    printf("[");
    for (int i = 0; i < PARAM_N1-1; ++i) {
        printf("%d,", p[i]);
    }
    printf("%d", p[PARAM_N1-1]);
    printf("]\n");
}