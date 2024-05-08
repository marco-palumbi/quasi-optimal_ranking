//
// Created by Alessandro Budroni on 23/08/2023.
//

#ifndef LEHMER_C_PERMUTATIONS_H
#define LEHMER_C_PERMUTATIONS_H

#include "config.h"
#include "stdlib.h"
#include <stdint.h>

typedef uint8_t perm_t[PARAM_N1];

void perm_sample(perm_t p);
void perm_print(const perm_t p);

#endif //LEHMER_C_PERMUTATIONS_H
