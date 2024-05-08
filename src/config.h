/**
	config.h
	
	Authors:
		Alessandro Budroni, May 2019
*/

#ifndef CONFIG_H
#define CONFIG_H

/* config variables */

#ifndef PARAM_N1
#define PARAM_N1 79
#endif


#if (PARAM_N1 == 79)

#define PARAM_K1 7

#elif (PARAM_N1 == 83)

#define PARAM_K1 7

#elif (PARAM_N1 == 112)

#define PARAM_K1 7

#elif (PARAM_N1 == 116)

#define PARAM_K1 7

#elif (PARAM_N1 == 146)

#define PARAM_K1 8

#elif (PARAM_N1 == 150)

#define PARAM_K1 8

#else
#error PARAM_N1 not supported
#endif

#define PARAM_2toK1 (1 << PARAM_K1)
#define PARAM_LEN_T ((1 << (PARAM_K1 + 1)) -1)

#endif