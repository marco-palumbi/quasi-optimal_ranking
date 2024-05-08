/**
	utils.h
	
	Authors:
		Alessandro Budroni, May 2019
*/

#ifndef LEHMER_GMP_H
#define LEHMER_GMP_H

#include <stdio.h>
#include <stdlib.h>

#include "permutations.h"
#include "config.h"
#include "gmp.h"

void lehmer_encode(const perm_t in_p, mpz_t code);
void lehmer_decode(mpz_t code, perm_t out_p);

#endif