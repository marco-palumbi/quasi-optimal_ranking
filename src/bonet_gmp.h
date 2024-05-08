//
// Created by Alessandro Budroni on 24/08/2023.
//

#ifndef RANK_UNRANK_BONET_GMP_H
#define RANK_UNRANK_BONET_GMP_H

#include <stdio.h>
#include <stdlib.h>

#include "permutations.h"
#include "config.h"
#include "gmp.h"

void bonet_encode(const perm_t in_p, mpz_t code);
void bonet_decode(mpz_t code, perm_t out_p);

#endif //RANK_UNRANK_BONET_GMP_H
