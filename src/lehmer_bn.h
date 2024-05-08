/**
	utils.h
	
	Authors:
		Alessandro Budroni, May 2019
*/

#ifndef RANK_UNRANK_LEHMER_BN_H
#define RANK_UNRANK_LEHMER_BN_H

#include <stdio.h>
#include <stdlib.h>

#include "bn.h"
#include "permutations.h"
#include "config.h"

void lehmer_encode(const perm_t in_p, struct bn* code);
void lehmer_decode(struct bn* code, perm_t out_p);

#endif