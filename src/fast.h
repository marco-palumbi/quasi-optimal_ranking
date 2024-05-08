//
// Created by Alessandro Budroni on 24/08/2023.
//

#ifndef RANK_UNRANK_fast_H
#define RANK_UNRANK_fast_H

#include <stdio.h>
#include <stdlib.h>

#include "permutations.h"
#include "config.h"

void fast_encode_2perm(const perm_t in_p[2], uint8_t *code);
void fast_decode_2perm(uint8_t *code, perm_t out_p[2]);

#endif //RANK_UNRANK_fast_H
