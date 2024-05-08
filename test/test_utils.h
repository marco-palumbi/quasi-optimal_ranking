//
// Created by Alessandro Budroni on 24/08/2023.
//

#ifndef RANK_UNRANK_TEST_UTILS_H
#define RANK_UNRANK_TEST_UTILS_H

#include <stdlib.h>
#include <time.h>
#include "hal.h"

#ifndef N_ITERATIONS
#define N_ITERATIONS 1000
#endif

#ifndef TARGET_NAME
#define TARGET_NAME ""
#endif

#define cpucycles() PLATFORM_CLOCK()

#endif // RANK_UNRANK_TEST_UTILS_H
