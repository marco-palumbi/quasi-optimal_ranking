//
// Created by Alessandro Budroni on 24/08/2023.
//

#ifndef RANK_UNRANK_BONET_BN_H
#define RANK_UNRANK_BONET_BN_H

#include <stdio.h>
#include <stdlib.h>

#include "bn.h"
#include "permutations.h"
#include "config.h"

void bonet_encode(const perm_t in_p, struct bn* code);
void bonet_decode(struct bn* code, perm_t out_p);

#endif //RANK_UNRANK_BONET_BN_H
