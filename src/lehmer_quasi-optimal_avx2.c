/**
 * @file parsing_permutations_avx2.c
 * @brief Implementation of parsing functions
 */

#include "parsing_permutations.h"
#include "immintrin.h"
#include "divide_ct.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define UNROLL

#if (PARAM_N1 == 79)
static const unsigned j_k[] = {1, 6, 11, 15, 20, 24, 29, 34, 39, 44, 49, 54, 59, 64, 69, 74, 79};
static const unsigned bit_per_block[] = {0, 10, 16, 15, 21, 18, 24, 25, 27, 27, 28, 29, 30, 30, 31, 31, 32};
static const unsigned max_per_block[] = {0, 720, 55440, 32760, 1860480, 255024, 14250600, 33390720, 69090840, 130320960, 228826080, 379501200, 600766320, 914941440, 1348621560, 1933051680, 2704501800};
#elif (PARAM_N1 == 83)
static const unsigned j_k[] = {1, 6, 11, 16, 21, 26, 32, 38, 43, 48, 53, 58, 63, 68, 73, 78, 83};
static const unsigned bit_per_block[] = {0, 10, 16, 19, 22, 23, 30, 31, 27, 28, 29, 30, 30, 31, 31, 32, 32};
static const unsigned max_per_block[] = {0, 720, 55440, 524160, 2441880, 7893600, 652458240, 1987690320, 115511760, 205476480, 344362200, 549853920, 843461640, 1250895360, 1802440080, 2533330800, 3484127520};
#elif (PARAM_N1 == 112)
static const unsigned j_k[] = {1, 6, 11, 16, 21, 26, 31, 36, 41, 45, 50, 55, 60, 65, 70, 75, 80, 84, 88, 92, 96, 100, 104, 108, 112};
static const unsigned bit_per_block[] = {0, 10, 16, 19, 22, 23, 25, 26, 27, 22, 28, 29, 30, 30, 31, 31, 32, 26, 26, 26, 27, 27, 27, 27, 28};
static const unsigned max_per_block[] = {0, 720, 55440, 524160, 2441880, 7893600, 20389320, 45239040, 89927760, 3575880, 254251200, 417451320, 655381440, 991186560, 1452361680, 2071126800, 2884801920, 46308024, 55965360, 67059720, 79727040, 94109400, 110355024, 128618280, 149059680};
#elif (PARAM_N1 == 116)
static const unsigned j_k[] = {1, 6, 11, 16, 21, 27, 33, 39, 44, 49, 54, 59, 64, 69, 74, 79, 84, 88, 92, 96, 100, 104, 108, 112, 116};
static const unsigned bit_per_block[] = {0, 10, 16, 19, 22, 28, 30, 32, 27, 28, 29, 30, 30, 31, 31, 32, 32, 26, 26, 27, 27, 27, 27, 28, 28};
static const unsigned max_per_block[] = {0, 720, 55440, 524160, 2441880, 213127200, 797448960, 2349088560, 130320960, 228826080, 379501200, 600766320, 914941440, 1348621560, 1933051680, 2704501800, 3704641920, 55965360, 67059720, 79727040, 94109400, 110355024, 128618280, 149059680, 171845880};
#elif (PARAM_N1 == 146)
static const unsigned j_k[] = {1, 6, 11, 16, 21, 26, 31, 36, 41, 46, 51, 56, 61, 66, 71, 76, 81, 86, 90, 94, 98, 102, 106, 110, 114, 118, 122, 126, 130, 134, 138, 142, 146};
static const unsigned bit_per_block[] = {0, 10, 16, 19, 22, 23, 25, 26, 27, 28, 29, 29, 30, 30, 31, 32, 32, 32, 26, 27, 27, 27, 27, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29};
static const unsigned max_per_block[] = {0, 720, 55440, 524160, 2441880, 7893600, 20389320, 45239040, 89927760, 164490480, 281887200, 458377920, 713897640, 1072431360, 1562389080, 2216980800, 3074591520, 4179156240, 61324560, 73188024, 86694720, 101989800, 119224560, 138556440, 160149024, 184172040, 210801360, 240219000, 272613120, 308178024, 347114160, 389628120, 435932640};
#elif (PARAM_N1 == 150)
static const unsigned j_k[] = {1, 6, 12, 18, 23, 29, 35, 41, 46, 51, 56, 61, 66, 71, 76, 81, 86, 90, 94, 98, 102, 106, 110, 114, 118, 122, 126, 130, 134, 138, 142, 146, 150};
static const unsigned bit_per_block[] = {0, 10, 20, 24, 22, 29, 31, 32, 28, 29, 29, 30, 30, 31, 32, 32, 32, 26, 27, 27, 27, 27, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29};
static const unsigned max_per_block[] = {0, 720, 665280, 13366080, 4037880, 342014400, 1168675200, 3237399360, 164490480, 281887200, 458377920, 713897640, 1072431360, 1562389080, 2216980800, 3074591520, 4179156240, 61324560, 73188024, 86694720, 101989800, 119224560, 138556440, 160149024, 184172040, 210801360, 240219000, 272613120, 308178024, 347114160, 389628120, 435932640, 486246600};
#else
#error PARAM_N1 not supported
#endif
static const unsigned n_i_k = sizeof(j_k) / sizeof(*j_k);

static unsigned store_n_bit_in_bytearray(uint8_t bytearray[], unsigned bit_index, uint32_t val, unsigned n) {
    uint64_t *array_64 = (uint64_t *)bytearray;

    if ((bit_index % 64) + n < 64) {
        uint64_t res;
        memcpy(&res, &array_64[bit_index / 64], sizeof(res));

        res |= (uint64_t)val << (bit_index % 64);
        
        memcpy(&array_64[bit_index / 64], &res, sizeof(res));
    }
    else {
        const unsigned n1 = 64 - (bit_index % 64);
        uint64_t res[2];

        uint64_t val1 = (uint64_t)val & ((UINT64_C(1) << n1) - 1);
        uint64_t val2 = (uint64_t)val >> n1;

        memcpy(res, &array_64[bit_index / 64], sizeof(res));

        res[0] |= val1 << (bit_index % 64);
        res[1] |= val2;

        memcpy(&array_64[bit_index / 64], res, sizeof(res));
    }

    return bit_index + n;
}

static unsigned load_n_bit_from_bytearray(uint32_t *val, unsigned n, const uint8_t bytearray[], unsigned bit_index) {
    uint64_t *array_64 = (uint64_t *)bytearray;

    if ((bit_index % 64) + n < 64) {
        uint64_t in;

        memcpy(&in, &array_64[bit_index / 64], sizeof(in));
        *val = (in >> (bit_index % 64)) & ((UINT64_C(1) << n) - 1);
    }
    else {
        const unsigned n1 = 64 - (bit_index % 64);
        uint64_t in[2];

        memcpy(in, &array_64[bit_index / 64], sizeof(in));

        *val = (in[0] >> (bit_index % 64)) & ((UINT64_C(1) << n1) - 1);
        *val |= (in[1] & ((UINT64_C(1) << (n - n1)) - 1)) << n1;
    }

    return bit_index + n;
}

#if !defined(__clang__) && defined(__GNUC__) && __GNUC__ > 11 && defined(__OPTIMIZE__)
#define AVX2_blend(a, b, mask)                                     \
    ({                                                             \
        __m256i res;                                               \
        asm("vpblendd $" #mask ", %1, %0, %0" : "+x"(a) : "x"(b)); \
        res;                                                       \
    })
#else
#define AVX2_blend(a, b, mask) _mm256_blend_epi32(a, b, mask)
#endif

#ifdef UNROLL
#if PARAM_N1 == 79
static unsigned pack_indices_79(uint8_t bytearray[], uint8_t *d_i, unsigned idx) {
    __m256i vblock[2];
    __m256i vFF = _mm256_set1_epi32(0xFF), vd_i, vj, vtmp;
    uint32_t *block = (uint32_t *)vblock;
    unsigned i, j;

    vblock[0] = _mm256_setzero_si256();
    vj = _mm256_loadu_si256((__m256i *)&j_k[1]);

    for (j = 1; j <= 4; j++) {
        vblock[0] = _mm256_mullo_epi32(vj, vblock[0]);
        vj = _mm256_sub_epi32(vj, _mm256_set1_epi32(1));

        vd_i = _mm256_and_si256(_mm256_i32gather_epi32((const int *)d_i, vj, 1), vFF);
        vblock[0] = _mm256_add_epi32(vblock[0], vd_i);
    }

    vtmp = _mm256_mullo_epi32(vj, vblock[0]);
    vj = _mm256_sub_epi32(vj, _mm256_set1_epi32(1));

    vd_i = _mm256_and_si256(_mm256_i32gather_epi32((const int *)d_i, vj, 1), vFF);
    vtmp = _mm256_add_epi32(vtmp, vd_i);
    vblock[0] = AVX2_blend(vblock[0], vtmp, 0xEB);

    //

    vblock[1] = _mm256_setzero_si256();
    vj = _mm256_loadu_si256((__m256i *)&j_k[9]);

    for (j = 1; j <= 5; j++) {
        vblock[1] = _mm256_mullo_epi32(vj, vblock[1]);
        vj = _mm256_sub_epi32(vj, _mm256_set1_epi32(1));

        vd_i = _mm256_and_si256(_mm256_i32gather_epi32((const int *)d_i, vj, 1), vFF);
        vblock[1] = _mm256_add_epi32(vblock[1], vd_i);
    }

    for (i = 1; i < n_i_k; i++) {
        idx = store_n_bit_in_bytearray(bytearray, idx, block[i - 1], bit_per_block[i]);
    }
    return idx;
}
#elif PARAM_N1 == 83
static unsigned pack_indices_83(uint8_t bytearray[], uint8_t *d_i, unsigned idx) {
    __m256i vblock[2];
    __m256i vFF = _mm256_set1_epi32(0xFF), vd_i, vj, vtmp;
    uint32_t *block = (uint32_t *)vblock;
    unsigned i, j;

    vblock[0] = _mm256_setzero_si256();
    vj = _mm256_loadu_si256((__m256i *)&j_k[1]);

    for (j = 1; j <= 5; j++) {
        vblock[0] = _mm256_mullo_epi32(vj, vblock[0]);
        vj = _mm256_sub_epi32(vj, _mm256_set1_epi32(1));

        vd_i = _mm256_and_si256(_mm256_i32gather_epi32((const int *)d_i, vj, 1), vFF);
        vblock[0] = _mm256_add_epi32(vblock[0], vd_i);
    }

    vtmp = _mm256_mullo_epi32(vj, vblock[0]);
    vj = _mm256_sub_epi32(vj, _mm256_set1_epi32(1));

    vd_i = _mm256_and_si256(_mm256_i32gather_epi32((const int *)d_i, vj, 1), vFF);
    vtmp = _mm256_add_epi32(vtmp, vd_i);
    vblock[0] = AVX2_blend(vblock[0], vtmp, 0x60);

    //

    vblock[1] = _mm256_setzero_si256();
    vj = _mm256_loadu_si256((__m256i *)&j_k[9]);

    for (j = 1; j <= 5; j++) {
        vblock[1] = _mm256_mullo_epi32(vj, vblock[1]);
        vj = _mm256_sub_epi32(vj, _mm256_set1_epi32(1));

        vd_i = _mm256_and_si256(_mm256_i32gather_epi32((const int *)d_i, vj, 1), vFF);
        vblock[1] = _mm256_add_epi32(vblock[1], vd_i);
    }

    for (i = 1; i < n_i_k; i++) {
        idx = store_n_bit_in_bytearray(bytearray, idx, block[i - 1], bit_per_block[i]);
    }
    return idx;
}
#elif PARAM_N1 == 112
static unsigned pack_indices_112(uint8_t bytearray[], uint8_t *d_i, unsigned idx) {
    __m256i vblock[3];
    __m256i vFF = _mm256_set1_epi32(0xFF), vd_i, vj, vtmp;
    uint32_t *block = (uint32_t *)vblock;
    unsigned i, j;

    vblock[0] = _mm256_setzero_si256();
    vj = _mm256_loadu_si256((__m256i *)&j_k[1]);

    for (j = 1; j <= 5; j++) {
        vblock[0] = _mm256_mullo_epi32(vj, vblock[0]);
        vj = _mm256_sub_epi32(vj, _mm256_set1_epi32(1));

        vd_i = _mm256_and_si256(_mm256_i32gather_epi32((const int *)d_i, vj, 1), vFF);
        vblock[0] = _mm256_add_epi32(vblock[0], vd_i);
    }

    //

    vblock[1] = _mm256_setzero_si256();
    vj = _mm256_loadu_si256((__m256i *)&j_k[9]);

    for (j = 1; j <= 4; j++) {
        vblock[1] = _mm256_mullo_epi32(vj, vblock[1]);
        vj = _mm256_sub_epi32(vj, _mm256_set1_epi32(1));

        vd_i = _mm256_and_si256(_mm256_i32gather_epi32((const int *)d_i, vj, 1), vFF);
        vblock[1] = _mm256_add_epi32(vblock[1], vd_i);
    }

    vtmp = _mm256_mullo_epi32(vj, vblock[1]);
    vj = _mm256_sub_epi32(vj, _mm256_set1_epi32(1));

    vd_i = _mm256_and_si256(_mm256_i32gather_epi32((const int *)d_i, vj, 1), vFF);
    vtmp = _mm256_add_epi32(vtmp, vd_i);
    vblock[1] = AVX2_blend(vblock[1], vtmp, 0xFE);

    //

    vblock[2] = _mm256_setzero_si256();
    vj = _mm256_loadu_si256((__m256i *)&j_k[17]);

    for (j = 1; j <= 4; j++) {
        vblock[2] = _mm256_mullo_epi32(vj, vblock[2]);
        vj = _mm256_sub_epi32(vj, _mm256_set1_epi32(1));

        vd_i = _mm256_and_si256(_mm256_i32gather_epi32((const int *)d_i, vj, 1), vFF);
        vblock[2] = _mm256_add_epi32(vblock[2], vd_i);
    }

    for (i = 1; i < n_i_k; i++) {
        idx = store_n_bit_in_bytearray(bytearray, idx, block[i - 1], bit_per_block[i]);
    }
    return idx;
}
#elif PARAM_N1 == 116
static unsigned pack_indices_116(uint8_t bytearray[], uint8_t *d_i, unsigned idx) {
    __m256i vblock[3];
    __m256i vFF = _mm256_set1_epi32(0xFF), vd_i, vj, vtmp;
    uint32_t *block = (uint32_t *)vblock;
    unsigned i, j;

    vblock[0] = _mm256_setzero_si256();
    vj = _mm256_loadu_si256((__m256i *)&j_k[1]);

    for (j = 1; j <= 5; j++) {
        vblock[0] = _mm256_mullo_epi32(vj, vblock[0]);
        vj = _mm256_sub_epi32(vj, _mm256_set1_epi32(1));

        vd_i = _mm256_and_si256(_mm256_i32gather_epi32((const int *)d_i, vj, 1), vFF);
        vblock[0] = _mm256_add_epi32(vblock[0], vd_i);
    }

    vtmp = _mm256_mullo_epi32(vj, vblock[0]);
    vj = _mm256_sub_epi32(vj, _mm256_set1_epi32(1));

    vd_i = _mm256_and_si256(_mm256_i32gather_epi32((const int *)d_i, vj, 1), vFF);
    vtmp = _mm256_add_epi32(vtmp, vd_i);
    vblock[0] = AVX2_blend(vblock[0], vtmp, 0x70);

    //

    vblock[1] = _mm256_setzero_si256();
    vj = _mm256_loadu_si256((__m256i *)&j_k[9]);

    for (j = 1; j <= 5; j++) {
        vblock[1] = _mm256_mullo_epi32(vj, vblock[1]);
        vj = _mm256_sub_epi32(vj, _mm256_set1_epi32(1));

        vd_i = _mm256_and_si256(_mm256_i32gather_epi32((const int *)d_i, vj, 1), vFF);
        vblock[1] = _mm256_add_epi32(vblock[1], vd_i);
    }

    //

    vblock[2] = _mm256_setzero_si256();
    vj = _mm256_loadu_si256((__m256i *)&j_k[17]);

    for (j = 1; j <= 4; j++) {
        vblock[2] = _mm256_mullo_epi32(vj, vblock[2]);
        vj = _mm256_sub_epi32(vj, _mm256_set1_epi32(1));

        vd_i = _mm256_and_si256(_mm256_i32gather_epi32((const int *)d_i, vj, 1), vFF);
        vblock[2] = _mm256_add_epi32(vblock[2], vd_i);
    }

    for (i = 1; i < n_i_k; i++) {
        idx = store_n_bit_in_bytearray(bytearray, idx, block[i - 1], bit_per_block[i]);
    }
    return idx;
}
#elif PARAM_N1 == 146
static unsigned pack_indices_146(uint8_t bytearray[], uint8_t *d_i, unsigned idx) {
    __m256i vblock[4];
    __m256i vFF = _mm256_set1_epi32(0xFF), vd_i, vj, vtmp;
    uint32_t *block = (uint32_t *)vblock;
    unsigned i, j;

    vblock[0] = _mm256_setzero_si256();
    vj = _mm256_loadu_si256((__m256i *)&j_k[1]);

    for (j = 1; j <= 5; j++) {
        vblock[0] = _mm256_mullo_epi32(vj, vblock[0]);
        vj = _mm256_sub_epi32(vj, _mm256_set1_epi32(1));

        vd_i = _mm256_and_si256(_mm256_i32gather_epi32((const int *)d_i, vj, 1), vFF);
        vblock[0] = _mm256_add_epi32(vblock[0], vd_i);
    }

    //

    vblock[1] = _mm256_setzero_si256();
    vj = _mm256_loadu_si256((__m256i *)&j_k[9]);

    for (j = 1; j <= 5; j++) {
        vblock[1] = _mm256_mullo_epi32(vj, vblock[1]);
        vj = _mm256_sub_epi32(vj, _mm256_set1_epi32(1));

        vd_i = _mm256_and_si256(_mm256_i32gather_epi32((const int *)d_i, vj, 1), vFF);
        vblock[1] = _mm256_add_epi32(vblock[1], vd_i);
    }

    //

    vblock[2] = _mm256_setzero_si256();
    vj = _mm256_loadu_si256((__m256i *)&j_k[17]);

    for (j = 1; j <= 4; j++) {
        vblock[2] = _mm256_mullo_epi32(vj, vblock[2]);
        vj = _mm256_sub_epi32(vj, _mm256_set1_epi32(1));

        vd_i = _mm256_and_si256(_mm256_i32gather_epi32((const int *)d_i, vj, 1), vFF);
        vblock[2] = _mm256_add_epi32(vblock[2], vd_i);
    }

    vtmp = _mm256_mullo_epi32(vj, vblock[2]);
    vj = _mm256_sub_epi32(vj, _mm256_set1_epi32(1));

    vd_i = _mm256_and_si256(_mm256_i32gather_epi32((const int *)d_i, vj, 1), vFF);
    vtmp = _mm256_add_epi32(vtmp, vd_i);
    vblock[2] = AVX2_blend(vblock[2], vtmp, 0x01);

    //

    vblock[3] = _mm256_setzero_si256();
    vj = _mm256_loadu_si256((__m256i *)&j_k[25]);

    for (j = 1; j <= 4; j++) {
        vblock[3] = _mm256_mullo_epi32(vj, vblock[3]);
        vj = _mm256_sub_epi32(vj, _mm256_set1_epi32(1));

        vd_i = _mm256_and_si256(_mm256_i32gather_epi32((const int *)d_i, vj, 1), vFF);
        vblock[3] = _mm256_add_epi32(vblock[3], vd_i);
    }

    for (i = 1; i < n_i_k; i++) {
        idx = store_n_bit_in_bytearray(bytearray, idx, block[i - 1], bit_per_block[i]);
    }
    return idx;
}
#elif PARAM_N1 == 150
static unsigned pack_indices_150(uint8_t bytearray[], uint8_t *d_i, unsigned idx) {
    __m256i vblock[4];
    __m256i vFF = _mm256_set1_epi32(0xFF), vd_i, vj, vtmp;
    uint32_t *block = (uint32_t *)vblock;
    unsigned i, j;

    vblock[0] = _mm256_setzero_si256();
    vj = _mm256_loadu_si256((__m256i *)&j_k[1]);

    for (j = 1; j <= 5; j++) {
        vblock[0] = _mm256_mullo_epi32(vj, vblock[0]);
        vj = _mm256_sub_epi32(vj, _mm256_set1_epi32(1));

        vd_i = _mm256_and_si256(_mm256_i32gather_epi32((const int *)d_i, vj, 1), vFF);
        vblock[0] = _mm256_add_epi32(vblock[0], vd_i);
    }

    vtmp = _mm256_mullo_epi32(vj, vblock[0]);
    vj = _mm256_sub_epi32(vj, _mm256_set1_epi32(1));

    vd_i = _mm256_and_si256(_mm256_i32gather_epi32((const int *)d_i, vj, 1), vFF);
    vtmp = _mm256_add_epi32(vtmp, vd_i);
    vblock[0] = AVX2_blend(vblock[0], vtmp, 0x76);

    //

    vblock[1] = _mm256_setzero_si256();
    vj = _mm256_loadu_si256((__m256i *)&j_k[9]);

    for (j = 1; j <= 5; j++) {
        vblock[1] = _mm256_mullo_epi32(vj, vblock[1]);
        vj = _mm256_sub_epi32(vj, _mm256_set1_epi32(1));

        vd_i = _mm256_and_si256(_mm256_i32gather_epi32((const int *)d_i, vj, 1), vFF);
        vblock[1] = _mm256_add_epi32(vblock[1], vd_i);
    }

    //

    vblock[2] = _mm256_setzero_si256();
    vj = _mm256_loadu_si256((__m256i *)&j_k[17]);

    for (j = 1; j <= 4; j++) {
        vblock[2] = _mm256_mullo_epi32(vj, vblock[2]);
        vj = _mm256_sub_epi32(vj, _mm256_set1_epi32(1));

        vd_i = _mm256_and_si256(_mm256_i32gather_epi32((const int *)d_i, vj, 1), vFF);
        vblock[2] = _mm256_add_epi32(vblock[2], vd_i);
    }

    //

    vblock[3] = _mm256_setzero_si256();
    vj = _mm256_loadu_si256((__m256i *)&j_k[25]);

    for (j = 1; j <= 4; j++) {
        vblock[3] = _mm256_mullo_epi32(vj, vblock[3]);
        vj = _mm256_sub_epi32(vj, _mm256_set1_epi32(1));

        vd_i = _mm256_and_si256(_mm256_i32gather_epi32((const int *)d_i, vj, 1), vFF);
        vblock[3] = _mm256_add_epi32(vblock[3], vd_i);
    }

    for (i = 1; i < n_i_k; i++) {
        idx = store_n_bit_in_bytearray(bytearray, idx, block[i - 1], bit_per_block[i]);
    }
    return idx;
}
#else
#error PARAM_N1 not supported
#endif

#else
static unsigned pack_indices(uint8_t bytearray[], uint8_t *d_i, unsigned idx) {
    uint32_t block[n_i_k - 1];
    unsigned i, j;
    memset(block, 0, sizeof(block));

    for (i = 1; i <= 8; i++) {
        for (j = j_k[i] - 1; j > j_k[i - 1] - 1; j--) {
            block[i - 1] = d_i[j] + ((j + 1) * block[i - 1]);
        }
    }

    for (; i <= 16; i++) {
        for (j = j_k[i] - 1; j > j_k[i - 1] - 1; j--) {
            block[i - 1] = d_i[j] + ((j + 1) * block[i - 1]);
        }
    }

#if PARAM_N1 >= 112
    for (; i <= 24; i++) {
        for (j = j_k[i] - 1; j > j_k[i - 1] - 1; j--) {
            block[i - 1] = d_i[j] + ((j + 1) * block[i - 1]);
        }
    }

#if PARAM_N1 >= 146
    for (; i <= 32; i++) {
        for (j = j_k[i] - 1; j > j_k[i - 1] - 1; j--) {
            block[i - 1] = d_i[j] + ((j + 1) * block[i - 1]);
        }
    }
#endif  // PARAM_N1 >= 146
#endif  // PARAM_N1 >= 112

    for (i = 1; i < n_i_k; i++) {
        idx = store_n_bit_in_bytearray(bytearray, idx, block[i - 1], bit_per_block[i]);
    }
    return idx;
}
#endif  // UNROLL

extern uint32_t m_tbl[MAX_DIV + 1];
extern uint32_t s_tbl[MAX_DIV + 1];

#ifdef UNROLL
#if PARAM_N1 == 79
static int unpack_indices_79(perm_t d_i, const uint8_t bytearray[], int idx) {
    __m256i vblock[2], vj, vtmp, vrem, vm_tbl, vs_tbl, ve, vo;
    uint32_t *block = (uint32_t *)vblock;
    uint32_t *jj = (uint32_t *)&vj;
    uint32_t *tmp = (uint32_t *)&vtmp;
    unsigned i, j;
    memset(block, 0, sizeof(vblock));

    for (i = 1; i < n_i_k; i++) {
        idx = load_n_bit_from_bytearray(&block[i - 1], bit_per_block[i], bytearray, idx);
        if (block[i - 1] >= max_per_block[i]){
            return -1;
        }
    }

    vj = _mm256_loadu_si256((__m256i *)&j_k[0]);

    for (j = 1; j <= 3; j++) {
        vj = _mm256_add_epi32(vj, _mm256_set1_epi32(1));

        vm_tbl = _mm256_i32gather_epi32((const int *)m_tbl, vj, 4);
        vs_tbl = _mm256_i32gather_epi32((const int *)s_tbl, vj, 4);

        ve = _mm256_mul_epu32(vblock[0], vm_tbl);
        vo = _mm256_mul_epu32(_mm256_srli_epi64(vblock[0], 32), _mm256_srli_epi64(vm_tbl, 32));
        ve = _mm256_srli_epi64(ve, 32);
        vrem = AVX2_blend(vo, ve, 0x55);

        vrem = _mm256_srlv_epi32(vrem, vs_tbl);

        vtmp = _mm256_sub_epi32(vblock[0], _mm256_mullo_epi32(vrem, vj));
        vblock[0] = vrem;

        for (i = 0; i < 8; i++) {
            d_i[PARAM_N1 - jj[i]] = tmp[i];
        }
    }

    vj = _mm256_add_epi32(vj, _mm256_set1_epi32(1));

    vm_tbl = _mm256_i32gather_epi32((const int *)m_tbl, vj, 4);
    vs_tbl = _mm256_i32gather_epi32((const int *)s_tbl, vj, 4);

    ve = _mm256_mul_epu32(vblock[0], vm_tbl);
    vo = _mm256_mul_epu32(_mm256_srli_epi64(vblock[0], 32), _mm256_srli_epi64(vm_tbl, 32));
    ve = _mm256_srli_epi64(ve, 32);
    vrem = AVX2_blend(vo, ve, 0x55);

    vrem = _mm256_srlv_epi32(vrem, vs_tbl);

    vtmp = _mm256_sub_epi32(vblock[0], _mm256_mullo_epi32(vrem, vj));
    vblock[0] = AVX2_blend(vblock[0], vrem, 0xEB);

    d_i[PARAM_N1 - jj[0]] = tmp[0];
    d_i[PARAM_N1 - jj[1]] = tmp[1];
    d_i[PARAM_N1 - jj[3]] = tmp[3];
    d_i[PARAM_N1 - jj[5]] = tmp[5];
    d_i[PARAM_N1 - jj[6]] = tmp[6];
    d_i[PARAM_N1 - jj[7]] = tmp[7];

    for (i = 0; i < 8; i++) {
        d_i[PARAM_N1 - j_k[i + 1]] = block[i];
    }

    //

    vj = _mm256_loadu_si256((__m256i *)&j_k[8]);

    for (j = 1; j <= 4; j++) {
        vj = _mm256_add_epi32(vj, _mm256_set1_epi32(1));

        vm_tbl = _mm256_i32gather_epi32((const int *)m_tbl, vj, 4);
        vs_tbl = _mm256_i32gather_epi32((const int *)s_tbl, vj, 4);

        ve = _mm256_mul_epu32(vblock[1], vm_tbl);
        vo = _mm256_mul_epu32(_mm256_srli_epi64(vblock[1], 32), _mm256_srli_epi64(vm_tbl, 32));
        ve = _mm256_srli_epi64(ve, 32);
        vrem = AVX2_blend(vo, ve, 0x55);

        vrem = _mm256_srlv_epi32(vrem, vs_tbl);

        vtmp = _mm256_sub_epi32(vblock[1], _mm256_mullo_epi32(vrem, vj));
        vblock[1] = vrem;

        for (i = 0; i < 8; i++) {
            d_i[PARAM_N1 - jj[i]] = tmp[i];
        }
    }

    for (i = 0; i < 8; i++) {
        d_i[PARAM_N1 - j_k[i + 1 + 8]] = block[i + 8];
    }
    return idx;
}
#elif PARAM_N1 == 83
static unsigned unpack_indices_83(perm_t d_i, const uint8_t bytearray[], unsigned idx) {
    __m256i vblock[2], vj, vtmp, vrem, vm_tbl, vs_tbl, ve, vo;
    uint32_t *block = (uint32_t *)vblock;
    uint32_t *jj = (uint32_t *)&vj;
    uint32_t *tmp = (uint32_t *)&vtmp;
    unsigned i, j;
    memset(block, 0, sizeof(vblock));

    for (i = 1; i < n_i_k; i++) {
        idx = load_n_bit_from_bytearray(&block[i - 1], bit_per_block[i], bytearray, idx);
        if (block[i - 1] >= max_per_block[i]){
            return -1;
        }
    }

    vj = _mm256_loadu_si256((__m256i *)&j_k[0]);

    for (j = 1; j <= 4; j++) {
        vj = _mm256_add_epi32(vj, _mm256_set1_epi32(1));

        vm_tbl = _mm256_i32gather_epi32((const int *)m_tbl, vj, 4);
        vs_tbl = _mm256_i32gather_epi32((const int *)s_tbl, vj, 4);

        ve = _mm256_mul_epu32(vblock[0], vm_tbl);
        vo = _mm256_mul_epu32(_mm256_srli_epi64(vblock[0], 32), _mm256_srli_epi64(vm_tbl, 32));
        ve = _mm256_srli_epi64(ve, 32);
        vrem = AVX2_blend(vo, ve, 0x55);

        vrem = _mm256_srlv_epi32(vrem, vs_tbl);

        vtmp = _mm256_sub_epi32(vblock[0], _mm256_mullo_epi32(vrem, vj));
        vblock[0] = vrem;

        for (i = 0; i < 8; i++) {
            d_i[PARAM_N1 - jj[i]] = tmp[i];
        }
    }

    uint32_t reminder = div_ct(block[5], 31);
    d_i[PARAM_N1 - 31] = block[5] - reminder * 31;
    block[5] = reminder;

    reminder = div_ct(block[6], 37);
    d_i[PARAM_N1 - 37] = block[6] - reminder * 37;
    block[6] = reminder;

    for (i = 0; i < 8; i++) {
        d_i[PARAM_N1 - j_k[i + 1]] = block[i];
    }

    //

    vj = _mm256_loadu_si256((__m256i *)&j_k[8]);

    for (j = 1; j <= 4; j++) {
        vj = _mm256_add_epi32(vj, _mm256_set1_epi32(1));

        vm_tbl = _mm256_i32gather_epi32((const int *)m_tbl, vj, 4);
        vs_tbl = _mm256_i32gather_epi32((const int *)s_tbl, vj, 4);

        ve = _mm256_mul_epu32(vblock[1], vm_tbl);
        vo = _mm256_mul_epu32(_mm256_srli_epi64(vblock[1], 32), _mm256_srli_epi64(vm_tbl, 32));
        ve = _mm256_srli_epi64(ve, 32);
        vrem = AVX2_blend(vo, ve, 0x55);

        vrem = _mm256_srlv_epi32(vrem, vs_tbl);

        vtmp = _mm256_sub_epi32(vblock[1], _mm256_mullo_epi32(vrem, vj));
        vblock[1] = vrem;

        for (i = 0; i < 8; i++) {
            d_i[PARAM_N1 - jj[i]] = tmp[i];
        }
    }

    for (i = 0; i < 8; i++) {
        d_i[PARAM_N1 - j_k[i + 1 + 8]] = block[i + 8];
    }
    return idx;
}

#elif PARAM_N1 == 112

static int unpack_indices_112(perm_t d_i, const uint8_t bytearray[], int idx) {
    __m256i vblock[3], vj, vtmp, vrem, vm_tbl, vs_tbl, ve, vo;
    uint32_t *block = (uint32_t *)vblock;
    uint32_t *jj = (uint32_t *)&vj;
    uint32_t *tmp = (uint32_t *)&vtmp;
    unsigned i, j;
    memset(block, 0, sizeof(vblock));

    for (i = 1; i < n_i_k; i++) {
        idx = load_n_bit_from_bytearray(&block[i - 1], bit_per_block[i], bytearray, idx);
        if (block[i - 1] >= max_per_block[i]){
            return -1;
        }
    }

    vj = _mm256_loadu_si256((__m256i *)&j_k[0]);

    for (j = 1; j <= 4; j++) {
        vj = _mm256_add_epi32(vj, _mm256_set1_epi32(1));

        vm_tbl = _mm256_i32gather_epi32((const int *)m_tbl, vj, 4);
        vs_tbl = _mm256_i32gather_epi32((const int *)s_tbl, vj, 4);

        ve = _mm256_mul_epu32(vblock[0], vm_tbl);
        vo = _mm256_mul_epu32(_mm256_srli_epi64(vblock[0], 32), _mm256_srli_epi64(vm_tbl, 32));
        ve = _mm256_srli_epi64(ve, 32);
        vrem = AVX2_blend(vo, ve, 0x55);

        vrem = _mm256_srlv_epi32(vrem, vs_tbl);

        vtmp = _mm256_sub_epi32(vblock[0], _mm256_mullo_epi32(vrem, vj));
        vblock[0] = vrem;

        for (i = 0; i < 8; i++) {
            d_i[PARAM_N1 - jj[i]] = tmp[i];
        }
    }

    for (i = 0; i < 8; i++) {
        d_i[PARAM_N1 - j_k[i + 1]] = block[i];
    }

    //

    vj = _mm256_loadu_si256((__m256i *)&j_k[8]);

    for (j = 1; j <= 3; j++) {
        vj = _mm256_add_epi32(vj, _mm256_set1_epi32(1));

        vm_tbl = _mm256_i32gather_epi32((const int *)m_tbl, vj, 4);
        vs_tbl = _mm256_i32gather_epi32((const int *)s_tbl, vj, 4);

        ve = _mm256_mul_epu32(vblock[1], vm_tbl);
        vo = _mm256_mul_epu32(_mm256_srli_epi64(vblock[1], 32), _mm256_srli_epi64(vm_tbl, 32));
        ve = _mm256_srli_epi64(ve, 32);
        vrem = AVX2_blend(vo, ve, 0x55);

        vrem = _mm256_srlv_epi32(vrem, vs_tbl);

        vtmp = _mm256_sub_epi32(vblock[1], _mm256_mullo_epi32(vrem, vj));
        vblock[1] = vrem;

        for (i = 0; i < 8; i++) {
            d_i[PARAM_N1 - jj[i]] = tmp[i];
        }
    }

    vj = _mm256_add_epi32(vj, _mm256_set1_epi32(1));

    vm_tbl = _mm256_i32gather_epi32((const int *)m_tbl, vj, 4);
    vs_tbl = _mm256_i32gather_epi32((const int *)s_tbl, vj, 4);

    ve = _mm256_mul_epu32(vblock[1], vm_tbl);
    vo = _mm256_mul_epu32(_mm256_srli_epi64(vblock[1], 32), _mm256_srli_epi64(vm_tbl, 32));
    ve = _mm256_srli_epi64(ve, 32);
    vrem = AVX2_blend(vo, ve, 0x55);

    vrem = _mm256_srlv_epi32(vrem, vs_tbl);

    vtmp = _mm256_sub_epi32(vblock[1], _mm256_mullo_epi32(vrem, vj));
    vblock[1] = AVX2_blend(vblock[1], vrem, 0xFE);
    // vblock[1] = AVX2_blend(vblock[1], vrem, 0x7E);

    d_i[PARAM_N1 - jj[1]] = tmp[1];
    d_i[PARAM_N1 - jj[2]] = tmp[2];
    d_i[PARAM_N1 - jj[3]] = tmp[3];
    d_i[PARAM_N1 - jj[4]] = tmp[4];
    d_i[PARAM_N1 - jj[5]] = tmp[5];
    d_i[PARAM_N1 - jj[6]] = tmp[6];
    d_i[PARAM_N1 - jj[7]] = tmp[7];

    for (i = 0; i < 8; i++) {
        d_i[PARAM_N1 - j_k[i + 1 + 8]] = block[i + 8];
    }

    //

    vj = _mm256_loadu_si256((__m256i *)&j_k[16]);

    for (j = 1; j <= 3; j++) {
        vj = _mm256_add_epi32(vj, _mm256_set1_epi32(1));

        vm_tbl = _mm256_i32gather_epi32((const int *)m_tbl, vj, 4);
        vs_tbl = _mm256_i32gather_epi32((const int *)s_tbl, vj, 4);

        ve = _mm256_mul_epu32(vblock[2], vm_tbl);
        vo = _mm256_mul_epu32(_mm256_srli_epi64(vblock[2], 32), _mm256_srli_epi64(vm_tbl, 32));
        ve = _mm256_srli_epi64(ve, 32);
        vrem = AVX2_blend(vo, ve, 0x55);

        vrem = _mm256_srlv_epi32(vrem, vs_tbl);

        vtmp = _mm256_sub_epi32(vblock[2], _mm256_mullo_epi32(vrem, vj));
        vblock[2] = vrem;

        for (i = 0; i < 8; i++) {
            d_i[PARAM_N1 - jj[i]] = tmp[i];
        }
    }

    for (i = 0; i < 8; i++) {
        d_i[PARAM_N1 - j_k[i + 1 + 16]] = block[i + 16];
    }
    return idx;
}

#elif PARAM_N1 == 116

static unsigned unpack_indices_116(perm_t d_i, const uint8_t bytearray[], unsigned idx) {
    __m256i vblock[3], vj, vtmp, vrem, vm_tbl, vs_tbl, ve, vo;
    uint32_t *block = (uint32_t *)vblock;
    uint32_t *jj = (uint32_t *)&vj;
    uint32_t *tmp = (uint32_t *)&vtmp;
    unsigned i, j;
    memset(block, 0, sizeof(vblock));

    for (i = 1; i < n_i_k; i++) {
        idx = load_n_bit_from_bytearray(&block[i - 1], bit_per_block[i], bytearray, idx);
        if (block[i - 1] >= max_per_block[i]){
            return -1;
        }
    }

    vj = _mm256_loadu_si256((__m256i *)&j_k[0]);

    for (j = 1; j <= 4; j++) {
        vj = _mm256_add_epi32(vj, _mm256_set1_epi32(1));

        vm_tbl = _mm256_i32gather_epi32((const int *)m_tbl, vj, 4);
        vs_tbl = _mm256_i32gather_epi32((const int *)s_tbl, vj, 4);

        ve = _mm256_mul_epu32(vblock[0], vm_tbl);
        vo = _mm256_mul_epu32(_mm256_srli_epi64(vblock[0], 32), _mm256_srli_epi64(vm_tbl, 32));
        ve = _mm256_srli_epi64(ve, 32);
        vrem = AVX2_blend(vo, ve, 0x55);

        vrem = _mm256_srlv_epi32(vrem, vs_tbl);

        vtmp = _mm256_sub_epi32(vblock[0], _mm256_mullo_epi32(vrem, vj));
        vblock[0] = vrem;

        for (i = 0; i < 8; i++) {
            d_i[PARAM_N1 - jj[i]] = tmp[i];
        }
    }

    uint32_t reminder = div_ct(block[4], 26);
    d_i[PARAM_N1 - 26] = block[4] - reminder * 26;
    block[4] = reminder;

    reminder = div_ct(block[5], 32);
    d_i[PARAM_N1 - 32] = block[5] - reminder * 32;
    block[5] = reminder;

    reminder = div_ct(block[6], 38);
    d_i[PARAM_N1 - 38] = block[6] - reminder * 38;
    block[6] = reminder;

    for (i = 0; i < 8; i++) {
        d_i[PARAM_N1 - j_k[i + 1]] = block[i];
    }

    //

    vj = _mm256_loadu_si256((__m256i *)&j_k[8]);

    for (j = 1; j <= 4; j++) {
        vj = _mm256_add_epi32(vj, _mm256_set1_epi32(1));

        vm_tbl = _mm256_i32gather_epi32((const int *)m_tbl, vj, 4);
        vs_tbl = _mm256_i32gather_epi32((const int *)s_tbl, vj, 4);

        ve = _mm256_mul_epu32(vblock[1], vm_tbl);
        vo = _mm256_mul_epu32(_mm256_srli_epi64(vblock[1], 32), _mm256_srli_epi64(vm_tbl, 32));
        ve = _mm256_srli_epi64(ve, 32);
        vrem = AVX2_blend(vo, ve, 0x55);

        vrem = _mm256_srlv_epi32(vrem, vs_tbl);

        vtmp = _mm256_sub_epi32(vblock[1], _mm256_mullo_epi32(vrem, vj));
        vblock[1] = vrem;

        for (i = 0; i < 8; i++) {
            d_i[PARAM_N1 - jj[i]] = tmp[i];
        }
    }

    for (i = 0; i < 8; i++) {
        d_i[PARAM_N1 - j_k[i + 1 + 8]] = block[i + 8];
    }

    //

    vj = _mm256_loadu_si256((__m256i *)&j_k[16]);

    for (j = 1; j <= 3; j++) {
        vj = _mm256_add_epi32(vj, _mm256_set1_epi32(1));

        vm_tbl = _mm256_i32gather_epi32((const int *)m_tbl, vj, 4);
        vs_tbl = _mm256_i32gather_epi32((const int *)s_tbl, vj, 4);

        ve = _mm256_mul_epu32(vblock[2], vm_tbl);
        vo = _mm256_mul_epu32(_mm256_srli_epi64(vblock[2], 32), _mm256_srli_epi64(vm_tbl, 32));
        ve = _mm256_srli_epi64(ve, 32);
        vrem = AVX2_blend(vo, ve, 0x55);

        vrem = _mm256_srlv_epi32(vrem, vs_tbl);

        vtmp = _mm256_sub_epi32(vblock[2], _mm256_mullo_epi32(vrem, vj));
        vblock[2] = vrem;

        for (i = 0; i < 8; i++) {
            d_i[PARAM_N1 - jj[i]] = tmp[i];
        }
    }

    for (i = 0; i < 8; i++) {
        d_i[PARAM_N1 - j_k[i + 1 + 16]] = block[i + 16];
    }
    return idx;
}

#elif PARAM_N1 == 146

static unsigned unpack_indices_146(perm_t d_i, const uint8_t bytearray[], unsigned idx) {
    __m256i vblock[4], vj, vtmp, vrem, vm_tbl, vs_tbl, ve, vo;
    uint32_t *block = (uint32_t *)vblock;
    uint32_t *jj = (uint32_t *)&vj;
    uint32_t *tmp = (uint32_t *)&vtmp;
    unsigned i, j;
    memset(block, 0, sizeof(vblock));

    for (i = 1; i < n_i_k; i++) {
        idx = load_n_bit_from_bytearray(&block[i - 1], bit_per_block[i], bytearray, idx);
        if (block[i - 1] >= max_per_block[i]){
            return -1;
        }
    }

    vj = _mm256_loadu_si256((__m256i *)&j_k[0]);

    for (j = 1; j <= 4; j++) {
        vj = _mm256_add_epi32(vj, _mm256_set1_epi32(1));

        vm_tbl = _mm256_i32gather_epi32((const int *)m_tbl, vj, 4);
        vs_tbl = _mm256_i32gather_epi32((const int *)s_tbl, vj, 4);

        ve = _mm256_mul_epu32(vblock[0], vm_tbl);
        vo = _mm256_mul_epu32(_mm256_srli_epi64(vblock[0], 32), _mm256_srli_epi64(vm_tbl, 32));
        ve = _mm256_srli_epi64(ve, 32);
        vrem = AVX2_blend(vo, ve, 0x55);

        vrem = _mm256_srlv_epi32(vrem, vs_tbl);

        vtmp = _mm256_sub_epi32(vblock[0], _mm256_mullo_epi32(vrem, vj));
        vblock[0] = vrem;

        for (i = 0; i < 8; i++) {
            d_i[PARAM_N1 - jj[i]] = tmp[i];
        }
    }

    for (i = 0; i < 8; i++) {
        d_i[PARAM_N1 - j_k[i + 1]] = block[i];
    }

    //

    vj = _mm256_loadu_si256((__m256i *)&j_k[8]);

    for (j = 1; j <= 4; j++) {
        vj = _mm256_add_epi32(vj, _mm256_set1_epi32(1));

        vm_tbl = _mm256_i32gather_epi32((const int *)m_tbl, vj, 4);
        vs_tbl = _mm256_i32gather_epi32((const int *)s_tbl, vj, 4);

        ve = _mm256_mul_epu32(vblock[1], vm_tbl);
        vo = _mm256_mul_epu32(_mm256_srli_epi64(vblock[1], 32), _mm256_srli_epi64(vm_tbl, 32));
        ve = _mm256_srli_epi64(ve, 32);
        vrem = AVX2_blend(vo, ve, 0x55);

        vrem = _mm256_srlv_epi32(vrem, vs_tbl);

        vtmp = _mm256_sub_epi32(vblock[1], _mm256_mullo_epi32(vrem, vj));
        vblock[1] = vrem;

        for (i = 0; i < 8; i++) {
            d_i[PARAM_N1 - jj[i]] = tmp[i];
        }
    }

    for (i = 0; i < 8; i++) {
        d_i[PARAM_N1 - j_k[i + 1 + 8]] = block[i + 8];
    }

    //

    vj = _mm256_loadu_si256((__m256i *)&j_k[16]);

    for (j = 1; j <= 3; j++) {
        vj = _mm256_add_epi32(vj, _mm256_set1_epi32(1));

        vm_tbl = _mm256_i32gather_epi32((const int *)m_tbl, vj, 4);
        vs_tbl = _mm256_i32gather_epi32((const int *)s_tbl, vj, 4);

        ve = _mm256_mul_epu32(vblock[2], vm_tbl);
        vo = _mm256_mul_epu32(_mm256_srli_epi64(vblock[2], 32), _mm256_srli_epi64(vm_tbl, 32));
        ve = _mm256_srli_epi64(ve, 32);
        vrem = AVX2_blend(vo, ve, 0x55);

        vrem = _mm256_srlv_epi32(vrem, vs_tbl);

        vtmp = _mm256_sub_epi32(vblock[2], _mm256_mullo_epi32(vrem, vj));
        vblock[2] = vrem;

        for (i = 0; i < 8; i++) {
            d_i[PARAM_N1 - jj[i]] = tmp[i];
        }
    }

    uint32_t reminder = div_ct(block[16], 85);
    d_i[PARAM_N1 - 85] = block[16] - reminder * 85;
    block[16] = reminder;

    for (i = 0; i < 8; i++) {
        d_i[PARAM_N1 - j_k[i + 1 + 16]] = block[i + 16];
    }

    //

    vj = _mm256_loadu_si256((__m256i *)&j_k[24]);

    for (j = 1; j <= 3; j++) {
        vj = _mm256_add_epi32(vj, _mm256_set1_epi32(1));

        vm_tbl = _mm256_i32gather_epi32((const int *)m_tbl, vj, 4);
        vs_tbl = _mm256_i32gather_epi32((const int *)s_tbl, vj, 4);

        ve = _mm256_mul_epu32(vblock[3], vm_tbl);
        vo = _mm256_mul_epu32(_mm256_srli_epi64(vblock[3], 32), _mm256_srli_epi64(vm_tbl, 32));
        ve = _mm256_srli_epi64(ve, 32);
        vrem = AVX2_blend(vo, ve, 0x55);

        vrem = _mm256_srlv_epi32(vrem, vs_tbl);

        vtmp = _mm256_sub_epi32(vblock[3], _mm256_mullo_epi32(vrem, vj));
        vblock[3] = vrem;

        for (i = 0; i < 8; i++) {
            d_i[PARAM_N1 - jj[i]] = tmp[i];
        }
    }

    for (i = 0; i < 8; i++) {
        d_i[PARAM_N1 - j_k[i + 1 + 24]] = block[i + 24];
    }
    return idx;
}

#elif PARAM_N1 == 150

static unsigned unpack_indices_150(perm_t d_i, const uint8_t bytearray[], unsigned idx) {
    __m256i vblock[4], vj, vtmp, vrem, vm_tbl, vs_tbl, ve, vo;
    uint32_t *block = (uint32_t *)vblock;
    uint32_t *jj = (uint32_t *)&vj;
    uint32_t *tmp = (uint32_t *)&vtmp;
    unsigned i, j;
    memset(block, 0, sizeof(vblock));

    for (i = 1; i < n_i_k; i++) {
        idx = load_n_bit_from_bytearray(&block[i - 1], bit_per_block[i], bytearray, idx);
        if (block[i - 1] >= max_per_block[i]){
            return -1;
        }
    }

    vj = _mm256_loadu_si256((__m256i *)&j_k[0]);

    for (j = 1; j <= 4; j++) {
        vj = _mm256_add_epi32(vj, _mm256_set1_epi32(1));

        vm_tbl = _mm256_i32gather_epi32((const int *)m_tbl, vj, 4);
        vs_tbl = _mm256_i32gather_epi32((const int *)s_tbl, vj, 4);

        ve = _mm256_mul_epu32(vblock[0], vm_tbl);
        vo = _mm256_mul_epu32(_mm256_srli_epi64(vblock[0], 32), _mm256_srli_epi64(vm_tbl, 32));
        ve = _mm256_srli_epi64(ve, 32);
        vrem = AVX2_blend(vo, ve, 0x55);

        vrem = _mm256_srlv_epi32(vrem, vs_tbl);

        vtmp = _mm256_sub_epi32(vblock[0], _mm256_mullo_epi32(vrem, vj));
        vblock[0] = vrem;

        for (i = 0; i < 8; i++) {
            d_i[PARAM_N1 - jj[i]] = tmp[i];
        }
    }

    vj = _mm256_add_epi32(vj, _mm256_set1_epi32(1));

    vm_tbl = _mm256_i32gather_epi32((const int *)m_tbl, vj, 4);
    vs_tbl = _mm256_i32gather_epi32((const int *)s_tbl, vj, 4);

    ve = _mm256_mul_epu32(vblock[0], vm_tbl);
    vo = _mm256_mul_epu32(_mm256_srli_epi64(vblock[0], 32), _mm256_srli_epi64(vm_tbl, 32));
    ve = _mm256_srli_epi64(ve, 32);
    vrem = AVX2_blend(vo, ve, 0x55);

    vrem = _mm256_srlv_epi32(vrem, vs_tbl);

    vtmp = _mm256_sub_epi32(vblock[0], _mm256_mullo_epi32(vrem, vj));
    vblock[0] = AVX2_blend(vblock[0], vrem, 0x76);

    d_i[PARAM_N1 - jj[1]] = tmp[1];
    d_i[PARAM_N1 - jj[2]] = tmp[2];
    d_i[PARAM_N1 - jj[4]] = tmp[4];
    d_i[PARAM_N1 - jj[5]] = tmp[5];
    d_i[PARAM_N1 - jj[6]] = tmp[6];

    for (i = 0; i < 8; i++) {
        d_i[PARAM_N1 - j_k[i + 1]] = block[i];
    }

    //

    vj = _mm256_loadu_si256((__m256i *)&j_k[8]);

    for (j = 1; j <= 4; j++) {
        vj = _mm256_add_epi32(vj, _mm256_set1_epi32(1));

        vm_tbl = _mm256_i32gather_epi32((const int *)m_tbl, vj, 4);
        vs_tbl = _mm256_i32gather_epi32((const int *)s_tbl, vj, 4);

        ve = _mm256_mul_epu32(vblock[1], vm_tbl);
        vo = _mm256_mul_epu32(_mm256_srli_epi64(vblock[1], 32), _mm256_srli_epi64(vm_tbl, 32));
        ve = _mm256_srli_epi64(ve, 32);
        vrem = AVX2_blend(vo, ve, 0x55);

        vrem = _mm256_srlv_epi32(vrem, vs_tbl);

        vtmp = _mm256_sub_epi32(vblock[1], _mm256_mullo_epi32(vrem, vj));
        vblock[1] = vrem;

        for (i = 0; i < 8; i++) {
            d_i[PARAM_N1 - jj[i]] = tmp[i];
        }
    }

    for (i = 0; i < 8; i++) {
        d_i[PARAM_N1 - j_k[i + 1 + 8]] = block[i + 8];
    }

    //

    vj = _mm256_loadu_si256((__m256i *)&j_k[16]);

    for (j = 1; j <= 3; j++) {
        vj = _mm256_add_epi32(vj, _mm256_set1_epi32(1));

        vm_tbl = _mm256_i32gather_epi32((const int *)m_tbl, vj, 4);
        vs_tbl = _mm256_i32gather_epi32((const int *)s_tbl, vj, 4);

        ve = _mm256_mul_epu32(vblock[2], vm_tbl);
        vo = _mm256_mul_epu32(_mm256_srli_epi64(vblock[2], 32), _mm256_srli_epi64(vm_tbl, 32));
        ve = _mm256_srli_epi64(ve, 32);
        vrem = AVX2_blend(vo, ve, 0x55);

        vrem = _mm256_srlv_epi32(vrem, vs_tbl);

        vtmp = _mm256_sub_epi32(vblock[2], _mm256_mullo_epi32(vrem, vj));
        vblock[2] = vrem;

        for (i = 0; i < 8; i++) {
            d_i[PARAM_N1 - jj[i]] = tmp[i];
        }
    }

    for (i = 0; i < 8; i++) {
        d_i[PARAM_N1 - j_k[i + 1 + 16]] = block[i + 16];
    }

    //

    vj = _mm256_loadu_si256((__m256i *)&j_k[24]);

    for (j = 1; j <= 3; j++) {
        vj = _mm256_add_epi32(vj, _mm256_set1_epi32(1));

        vm_tbl = _mm256_i32gather_epi32((const int *)m_tbl, vj, 4);
        vs_tbl = _mm256_i32gather_epi32((const int *)s_tbl, vj, 4);

        ve = _mm256_mul_epu32(vblock[3], vm_tbl);
        vo = _mm256_mul_epu32(_mm256_srli_epi64(vblock[3], 32), _mm256_srli_epi64(vm_tbl, 32));
        ve = _mm256_srli_epi64(ve, 32);
        vrem = AVX2_blend(vo, ve, 0x55);

        vrem = _mm256_srlv_epi32(vrem, vs_tbl);

        vtmp = _mm256_sub_epi32(vblock[3], _mm256_mullo_epi32(vrem, vj));
        vblock[3] = vrem;

        for (i = 0; i < 8; i++) {
            d_i[PARAM_N1 - jj[i]] = tmp[i];
        }
    }

    for (i = 0; i < 8; i++) {
        d_i[PARAM_N1 - j_k[i + 1 + 24]] = block[i + 24];
    }
    return idx;
}
#endif
#else  // UNROLL

static unsigned unpack_indices(perm_t d_i, const uint8_t bytearray[], unsigned idx) {
    uint32_t block[n_i_k - 1];
    unsigned i, j;
    memset(block, 0, sizeof(block));

    for (i = 1; i < n_i_k; i++) {
        idx = load_n_bit_from_bytearray(&block[i - 1], bit_per_block[i], bytearray, idx);
        if (block[i - 1] >= max_per_block[i]){
            return -1;
        }
    }

    for (i = 1; i <= 8; i++) {
        for (j = j_k[i - 1] + 1; j < j_k[i]; j++) {
            uint32_t reminder = div_ct(block[i - 1], j);
            d_i[PARAM_N1 - j] = block[i - 1] - reminder * j;
            block[i - 1] = reminder;
        }
        d_i[PARAM_N1 - j] = block[i - 1];
    }

    for (; i <= 16; i++) {
        for (j = j_k[i - 1] + 1; j < j_k[i]; j++) {
            uint32_t reminder = div_ct(block[i - 1], j);
            d_i[PARAM_N1 - j] = block[i - 1] - reminder * j;
            block[i - 1] = reminder;
        }
        d_i[PARAM_N1 - j] = block[i - 1];
    }

#if PARAM_N1 >= 112
    for (; i <= 24; i++) {
        for (j = j_k[i - 1] + 1; j < j_k[i]; j++) {
            uint32_t reminder = div_ct(block[i - 1], j);
            d_i[PARAM_N1 - j] = block[i - 1] - reminder * j;
            block[i - 1] = reminder;
        }
        d_i[PARAM_N1 - j] = block[i - 1];
    }

#if PARAM_N1 >= 146
    for (; i <= 32; i++) {
        for (j = j_k[i - 1] + 1; j < j_k[i]; j++) {
            uint32_t reminder = div_ct(block[i - 1], j);
            d_i[PARAM_N1 - j] = block[i - 1] - reminder * j;
            block[i - 1] = reminder;
        }
        d_i[PARAM_N1 - j] = block[i - 1];
    }
#endif  // PARAM_N1 >= 146
#endif  // PARAM_N1 >= 112
    return idx;
}
#endif //UNROLL

static __m256i AVX2_cmpgt(__m256i a, __m256i b) {
    // https://stackoverflow.com/a/24234695/523079
#if PARAM_N1 < 128
    return _mm256_cmpgt_epi8(a, b);
#else
    return _mm256_cmpgt_epi8(_mm256_xor_si256(a, _mm256_set1_epi8(-128)), _mm256_xor_si256(b, _mm256_set1_epi8(-128)));
#endif
}

static __m256i AVX2_cmpge(__m256i a, __m256i b) {
    // https://stackoverflow.com/a/28383095/523079
    return _mm256_cmpeq_epi8(_mm256_max_epu8(a, b), a);
}

#define PARAM_N1_32 ((PARAM_N1 + 31) / 32)

unsigned sig_perk_perm_encode(const perm_t in_p, uint8_t *code, unsigned idx) {
    __m256i vd_i[PARAM_N1_32], vin_p[PARAM_N1_32], vmask, vtmp, vcmp;
    const __m256i valign_mask1 = _mm256_set_epi64x(-1, -1, 0, 0);
    const __m256i valign_mask2 = _mm256_set_epi64x(-1, -1, -1, -1);
    uint8_t *d_i = (uint8_t *)vd_i;
    uint8_t *iin_p = (uint8_t *)vin_p;
    unsigned i, j;

    for (i = 0; i < PARAM_N1_32; i++) {
        vd_i[i] = _mm256_setzero_si256();
    }

    vin_p[0] = _mm256_setzero_si256();
    memcpy(&iin_p[32 * PARAM_N1_32 - PARAM_N1], in_p, PARAM_N1);

#ifndef UNROLL
    unsigned k;

    vmask = _mm256_setzero_si256();

    for (i = 1; i <= 16; i++) {
        vmask = _mm256_alignr_epi8(valign_mask1, vmask, 1);
        vcmp = _mm256_set1_epi8(in_p[PARAM_N1 - 1 - i]);

        vtmp = AVX2_cmpgt(vcmp, vin_p[PARAM_N1_32 - 1]);
        vtmp = _mm256_and_si256(vtmp, vmask);
        d_i[i] += _mm_popcnt_u32(_mm256_movemask_epi8(vtmp));
    }

    for (; i <= 32; i++) {
        vmask = _mm256_alignr_epi8(valign_mask2, vmask, 1);
        vcmp = _mm256_set1_epi8(in_p[PARAM_N1 - 1 - i]);

        vtmp = AVX2_cmpgt(vcmp, vin_p[PARAM_N1_32 - 1]);
        vtmp = _mm256_and_si256(vtmp, vmask);
        d_i[i] += _mm_popcnt_u32(_mm256_movemask_epi8(vtmp));
    }

    for (k = 1; k < PARAM_N1_32 - 1; k++) {
        vmask = _mm256_setzero_si256();

        for (; i < 32 * k + 16; i++) {
            vmask = _mm256_alignr_epi8(valign_mask1, vmask, 1);
            vcmp = _mm256_set1_epi8(in_p[PARAM_N1 - 1 - i]);

            vtmp = AVX2_cmpgt(vcmp, vin_p[PARAM_N1_32 - 1 - k]);
            vtmp = _mm256_and_si256(vtmp, vmask);
            d_i[i] += _mm_popcnt_u32(_mm256_movemask_epi8(vtmp));

            for (j = PARAM_N1_32 - k; j < PARAM_N1_32; ++j) {
                vtmp = AVX2_cmpgt(vcmp, vin_p[j]);
                d_i[i] += _mm_popcnt_u32(_mm256_movemask_epi8(vtmp));
            }
        }

        for (; i < 32 * k + 32; i++) {
            vmask = _mm256_alignr_epi8(valign_mask2, vmask, 1);
            vcmp = _mm256_set1_epi8(in_p[PARAM_N1 - 1 - i]);

            vtmp = AVX2_cmpgt(vcmp, vin_p[PARAM_N1_32 - 1 - k]);
            vtmp = _mm256_and_si256(vtmp, vmask);
            d_i[i] += _mm_popcnt_u32(_mm256_movemask_epi8(vtmp));

            for (j = PARAM_N1_32 - k; j < PARAM_N1_32; ++j) {
                vtmp = AVX2_cmpgt(vcmp, vin_p[j]);
                d_i[i] += _mm_popcnt_u32(_mm256_movemask_epi8(vtmp));
            }
        }
    }

    vmask = _mm256_setzero_si256();

#if PARAM_N1 % 32 > 16
    for (; i < 32 * k + 16; i++) {
#else
    for (; i < PARAM_N1; i++) {
#endif
        vmask = _mm256_alignr_epi8(valign_mask1, vmask, 1);
        vcmp = _mm256_set1_epi8(in_p[PARAM_N1 - 1 - i]);

        vtmp = AVX2_cmpgt(vcmp, vin_p[PARAM_N1_32 - 1 - k]);
        vtmp = _mm256_and_si256(vtmp, vmask);
        d_i[i] += _mm_popcnt_u32(_mm256_movemask_epi8(vtmp));

        for (j = PARAM_N1_32 - k; j < PARAM_N1_32; ++j) {
            vtmp = AVX2_cmpgt(vcmp, vin_p[j]);
            d_i[i] += _mm_popcnt_u32(_mm256_movemask_epi8(vtmp));
        }
    }

#if PARAM_N1 % 32 > 16
    for (; i < PARAM_N1; i++) {
        vmask = _mm256_alignr_epi8(valign_mask2, vmask, 1);
        vcmp = _mm256_set1_epi8(in_p[PARAM_N1 - 1 - i]);

        vtmp = AVX2_cmpgt(vcmp, vin_p[PARAM_N1_32 - 1 - k]);
        vtmp = _mm256_and_si256(vtmp, vmask);
        d_i[i] += _mm_popcnt_u32(_mm256_movemask_epi8(vtmp));

        for (j = PARAM_N1_32 - k; j < PARAM_N1_32; ++j) {
            vtmp = AVX2_cmpgt(vcmp, vin_p[j]);
            d_i[i] += _mm_popcnt_u32(_mm256_movemask_epi8(vtmp));
        }
    }
#endif
#else
    vmask = _mm256_setzero_si256();

    for (i = 1; i <= 16; i++) {
        vmask = _mm256_alignr_epi8(valign_mask1, vmask, 1);
        vcmp = _mm256_set1_epi8(in_p[PARAM_N1 - 1 - i]);

        vtmp = AVX2_cmpgt(vcmp, vin_p[PARAM_N1_32 - 1]);
        vtmp = _mm256_and_si256(vtmp, vmask);
        d_i[i] += _mm_popcnt_u32(_mm256_movemask_epi8(vtmp));
    }

    for (; i <= 32; i++) {
        vmask = _mm256_alignr_epi8(valign_mask2, vmask, 1);
        vcmp = _mm256_set1_epi8(in_p[PARAM_N1 - 1 - i]);

        vtmp = AVX2_cmpgt(vcmp, vin_p[PARAM_N1_32 - 1]);
        vtmp = _mm256_and_si256(vtmp, vmask);
        d_i[i] += _mm_popcnt_u32(_mm256_movemask_epi8(vtmp));
    }

    vmask = _mm256_setzero_si256();

    for (; i <= 48; i++) {
        vmask = _mm256_alignr_epi8(valign_mask1, vmask, 1);
        vcmp = _mm256_set1_epi8(in_p[PARAM_N1 - 1 - i]);

        vtmp = AVX2_cmpgt(vcmp, vin_p[PARAM_N1_32 - 2]);
        vtmp = _mm256_and_si256(vtmp, vmask);
        d_i[i] += _mm_popcnt_u32(_mm256_movemask_epi8(vtmp));

        vtmp = AVX2_cmpgt(vcmp, vin_p[PARAM_N1_32 - 1]);
        d_i[i] += _mm_popcnt_u32(_mm256_movemask_epi8(vtmp));
    }

    for (; i <= 64; i++) {
        vmask = _mm256_alignr_epi8(valign_mask2, vmask, 1);
        vcmp = _mm256_set1_epi8(in_p[PARAM_N1 - 1 - i]);

        vtmp = AVX2_cmpgt(vcmp, vin_p[PARAM_N1_32 - 2]);
        vtmp = _mm256_and_si256(vtmp, vmask);
        d_i[i] += _mm_popcnt_u32(_mm256_movemask_epi8(vtmp));

        vtmp = AVX2_cmpgt(vcmp, vin_p[PARAM_N1_32 - 1]);
        d_i[i] += _mm_popcnt_u32(_mm256_movemask_epi8(vtmp));
    }

    vmask = _mm256_setzero_si256();

#if PARAM_N1 == 79
    for (; i < PARAM_N1; i++) {
        vmask = _mm256_alignr_epi8(valign_mask1, vmask, 1);
        vcmp = _mm256_set1_epi8(in_p[PARAM_N1 - 1 - i]);

        vtmp = AVX2_cmpgt(vcmp, vin_p[0]);
        vtmp = _mm256_and_si256(vtmp, vmask);
        d_i[i] += _mm_popcnt_u32(_mm256_movemask_epi8(vtmp));

        for (j = 1; j < PARAM_N1_32; ++j) {
            vtmp = AVX2_cmpgt(vcmp, vin_p[j]);
            d_i[i] += _mm_popcnt_u32(_mm256_movemask_epi8(vtmp));
        }
    }
#else
    for (; i <= 80; i++) {
        vmask = _mm256_alignr_epi8(valign_mask1, vmask, 1);
        vcmp = _mm256_set1_epi8(in_p[PARAM_N1 - 1 - i]);

        vtmp = AVX2_cmpgt(vcmp, vin_p[PARAM_N1_32 - 3]);
        vtmp = _mm256_and_si256(vtmp, vmask);
        d_i[i] += _mm_popcnt_u32(_mm256_movemask_epi8(vtmp));

        for (j = PARAM_N1_32 - 2; j < PARAM_N1_32; ++j) {
            vtmp = AVX2_cmpgt(vcmp, vin_p[j]);
            d_i[i] += _mm_popcnt_u32(_mm256_movemask_epi8(vtmp));
        }
    }

#if PARAM_N1 == 83
    for (; i < PARAM_N1; i++) {
        vmask = _mm256_alignr_epi8(valign_mask2, vmask, 1);
        vcmp = _mm256_set1_epi8(in_p[PARAM_N1 - 1 - i]);

        vtmp = AVX2_cmpgt(vcmp, vin_p[0]);
        vtmp = _mm256_and_si256(vtmp, vmask);
        d_i[i] += _mm_popcnt_u32(_mm256_movemask_epi8(vtmp));

        for (j = 1; j < PARAM_N1_32; ++j) {
            vtmp = AVX2_cmpgt(vcmp, vin_p[j]);
            d_i[i] += _mm_popcnt_u32(_mm256_movemask_epi8(vtmp));
        }
    }
#else
    for (; i <= 96; i++) {
        vmask = _mm256_alignr_epi8(valign_mask2, vmask, 1);
        vcmp = _mm256_set1_epi8(in_p[PARAM_N1 - 1 - i]);

        vtmp = AVX2_cmpgt(vcmp, vin_p[PARAM_N1_32 - 3]);
        vtmp = _mm256_and_si256(vtmp, vmask);
        d_i[i] += _mm_popcnt_u32(_mm256_movemask_epi8(vtmp));

        for (j = PARAM_N1_32 - 2; j < PARAM_N1_32; ++j) {
            vtmp = AVX2_cmpgt(vcmp, vin_p[j]);
            d_i[i] += _mm_popcnt_u32(_mm256_movemask_epi8(vtmp));
        }
    }

    vmask = _mm256_setzero_si256();

#if PARAM_N1 == 112
    for (; i < PARAM_N1; i++) {
        vmask = _mm256_alignr_epi8(valign_mask1, vmask, 1);
        vcmp = _mm256_set1_epi8(in_p[PARAM_N1 - 1 - i]);

        vtmp = AVX2_cmpgt(vcmp, vin_p[PARAM_N1_32 - 4]);
        vtmp = _mm256_and_si256(vtmp, vmask);
        d_i[i] += _mm_popcnt_u32(_mm256_movemask_epi8(vtmp));

        for (j = PARAM_N1_32 - 3; j < PARAM_N1_32; ++j) {
            vtmp = AVX2_cmpgt(vcmp, vin_p[j]);
            d_i[i] += _mm_popcnt_u32(_mm256_movemask_epi8(vtmp));
        }
    }
#else
    for (; i <= 112; i++) {
        vmask = _mm256_alignr_epi8(valign_mask1, vmask, 1);
        vcmp = _mm256_set1_epi8(in_p[PARAM_N1 - 1 - i]);

        vtmp = AVX2_cmpgt(vcmp, vin_p[PARAM_N1_32 - 4]);
        vtmp = _mm256_and_si256(vtmp, vmask);
        d_i[i] += _mm_popcnt_u32(_mm256_movemask_epi8(vtmp));

        for (j = PARAM_N1_32 - 3; j < PARAM_N1_32; ++j) {
            vtmp = AVX2_cmpgt(vcmp, vin_p[j]);
            d_i[i] += _mm_popcnt_u32(_mm256_movemask_epi8(vtmp));
        }
    }

#if PARAM_N1 == 116
    for (; i < PARAM_N1; i++) {
        vmask = _mm256_alignr_epi8(valign_mask2, vmask, 1);
        vcmp = _mm256_set1_epi8(in_p[PARAM_N1 - 1 - i]);

        vtmp = AVX2_cmpgt(vcmp, vin_p[PARAM_N1_32 - 4]);
        vtmp = _mm256_and_si256(vtmp, vmask);
        d_i[i] += _mm_popcnt_u32(_mm256_movemask_epi8(vtmp));

        for (j = PARAM_N1_32 - 3; j < PARAM_N1_32; ++j) {
            vtmp = AVX2_cmpgt(vcmp, vin_p[j]);
            d_i[i] += _mm_popcnt_u32(_mm256_movemask_epi8(vtmp));
        }
    }
#else
    for (; i <= 128; i++) {
        vmask = _mm256_alignr_epi8(valign_mask2, vmask, 1);
        vcmp = _mm256_set1_epi8(in_p[PARAM_N1 - 1 - i]);

        vtmp = AVX2_cmpgt(vcmp, vin_p[PARAM_N1_32 - 4]);
        vtmp = _mm256_and_si256(vtmp, vmask);
        d_i[i] += _mm_popcnt_u32(_mm256_movemask_epi8(vtmp));

        for (j = PARAM_N1_32 - 3; j < PARAM_N1_32; ++j) {
            vtmp = AVX2_cmpgt(vcmp, vin_p[j]);
            d_i[i] += _mm_popcnt_u32(_mm256_movemask_epi8(vtmp));
        }
    }

    vmask = _mm256_setzero_si256();

    for (; i <= 144; i++) {
        vmask = _mm256_alignr_epi8(valign_mask1, vmask, 1);
        vcmp = _mm256_set1_epi8(in_p[PARAM_N1 - 1 - i]);

        vtmp = AVX2_cmpgt(vcmp, vin_p[PARAM_N1_32 - 5]);
        vtmp = _mm256_and_si256(vtmp, vmask);
        d_i[i] += _mm_popcnt_u32(_mm256_movemask_epi8(vtmp));

        for (j = PARAM_N1_32 - 4; j < PARAM_N1_32; ++j) {
            vtmp = AVX2_cmpgt(vcmp, vin_p[j]);
            d_i[i] += _mm_popcnt_u32(_mm256_movemask_epi8(vtmp));
        }
    }

    for (; i < PARAM_N1; i++) {
        vmask = _mm256_alignr_epi8(valign_mask2, vmask, 1);
        vcmp = _mm256_set1_epi8(in_p[PARAM_N1 - 1 - i]);

        vtmp = AVX2_cmpgt(vcmp, vin_p[PARAM_N1_32 - 5]);
        vtmp = _mm256_and_si256(vtmp, vmask);
        d_i[i] += _mm_popcnt_u32(_mm256_movemask_epi8(vtmp));

        for (j = PARAM_N1_32 - 4; j < PARAM_N1_32; ++j) {
            vtmp = AVX2_cmpgt(vcmp, vin_p[j]);
            d_i[i] += _mm_popcnt_u32(_mm256_movemask_epi8(vtmp));
        }
    }
#endif  // PARAM_N1 == 116
#endif  // PARAM_N1 == 112
#endif  // PARAM_N1 == 83
#endif  // PARAM_N1 == 79
#endif  // UNROLL

#ifdef UNROLL
#if PARAM_N1 == 79
    idx = pack_indices_79(code, d_i, idx);
#elif PARAM_N1 == 83
    idx = pack_indices_83(code, d_i, idx);
#elif PARAM_N1 == 112
    idx = pack_indices_112(code, d_i, idx);
#elif PARAM_N1 == 116
    idx = pack_indices_116(code, d_i, idx);
#elif PARAM_N1 == 146
    idx = pack_indices_146(code, d_i, idx);
#elif PARAM_N1 == 150
    idx = pack_indices_150(code, d_i, idx);;
#endif
#else
    idx = pack_indices(code, d_i, idx);
#endif
    return idx;
}

static void incr_ge_mask(__m256i *vout_p_j, __m256i vcmp, __m256i vmask) {
    __m256i vres;

    vres = AVX2_cmpgt(vcmp, *vout_p_j);
    vres = _mm256_andnot_si256(vres, vmask);
    *vout_p_j = _mm256_sub_epi8(*vout_p_j, vres);
}

static void incr_ge(__m256i *vout_p_j, __m256i vcmp) {
    __m256i vres;

    vres = AVX2_cmpge(*vout_p_j, vcmp);
    *vout_p_j = _mm256_sub_epi8(*vout_p_j, vres);
}

int sig_perk_perm_decode(const uint8_t *code, int idx, perm_t out_p) {
    __m256i vout_p[PARAM_N1_32];
    __m256i vmask = _mm256_setzero_si256();
    __m256i vcmp;
    const __m256i valign_mask1 = _mm256_set_epi64x(-1, -1, 0, 0);
    const __m256i valign_mask2 = _mm256_set_epi64x(-1, -1, -1, -1);
    uint8_t *mask = (uint8_t *)&vmask;
    uint8_t *outp = (uint8_t *)&vout_p;
    unsigned i, j;

    mask[31] = 0xFF;

    outp[32 * PARAM_N1_32 - 1] = 0;

#ifdef UNROLL
#if PARAM_N1 == 79
    idx = unpack_indices_79(&outp[32 * PARAM_N1_32 - PARAM_N1], code, idx);
#elif PARAM_N1 == 83
    idx = unpack_indices_83(&outp[32 * PARAM_N1_32 - PARAM_N1], code, idx);
#elif PARAM_N1 == 112
    idx = unpack_indices_112(&outp[32 * PARAM_N1_32 - PARAM_N1], code, idx);
#elif PARAM_N1 == 116
    idx = unpack_indices_116(&outp[32 * PARAM_N1_32 - PARAM_N1], code, idx);
#elif PARAM_N1 == 146
    idx = unpack_indices_146(&outp[32 * PARAM_N1_32 - PARAM_N1], code, idx);
#elif PARAM_N1 == 150
    idx = unpack_indices_150(&outp[32 * PARAM_N1_32 - PARAM_N1], code, idx);
#endif
#else
    idx = unpack_indices(&outp[32 * PARAM_N1_32 - PARAM_N1], code, idx);
#endif
    if (idx < 0){
        return -1;
    }

#ifndef UNROLL
    unsigned k;

    i = 1;

    for (k = 0; k < PARAM_N1_32 - 1; k++) {
        for (; i < 32 * k + 16; i++) {
            vcmp = _mm256_set1_epi8(outp[32 * PARAM_N1_32 - 1 - i]);

            incr_ge_mask(&vout_p[PARAM_N1_32 - k - 1], vcmp, vmask);

            for (j = PARAM_N1_32 - k; j < PARAM_N1_32; j++) {
                incr_ge(&vout_p[j], vcmp);
            }

            vmask = _mm256_alignr_epi8(valign_mask1, vmask, 1);
        }

        for (; i < 32 * k + 32; i++) {
            vcmp = _mm256_set1_epi8(outp[32 * PARAM_N1_32 - 1 - i]);

            incr_ge_mask(&vout_p[PARAM_N1_32 - k - 1], vcmp, vmask);

            for (j = PARAM_N1_32 - k; j < PARAM_N1_32; j++) {
                incr_ge(&vout_p[j], vcmp);
            }

            vmask = _mm256_alignr_epi8(valign_mask2, vmask, 1);
        }

        vmask = _mm256_setzero_si256();
    }

#if PARAM_N1 % 32 > 16
    for (; i < 32 * k + 16; i++) {
#else
    for (; i < PARAM_N1; i++) {
#endif
        vcmp = _mm256_set1_epi8(outp[32 * PARAM_N1_32 - 1 - i]);

        incr_ge_mask(&vout_p[PARAM_N1_32 - k - 1], vcmp, vmask);

        for (j = PARAM_N1_32 - k; j < PARAM_N1_32; j++) {
            incr_ge(&vout_p[j], vcmp);
        }

        vmask = _mm256_alignr_epi8(valign_mask1, vmask, 1);
    }

#if PARAM_N1 % 32 > 16
    for (; i < PARAM_N1; i++) {
        vcmp = _mm256_set1_epi8(outp[32 * PARAM_N1_32 - 1 - i]);

        incr_ge_mask(&vout_p[PARAM_N1_32 - k - 1], vcmp, vmask);

        for (j = PARAM_N1_32 - k; j < PARAM_N1_32; j++) {
            incr_ge(&vout_p[j], vcmp);
        }

        vmask = _mm256_alignr_epi8(valign_mask2, vmask, 1);
    }
#endif
#else
    for (i = 1; i < 16; i++) {
        vcmp = _mm256_set1_epi8(outp[32 * PARAM_N1_32 - 1 - i]);

        incr_ge_mask(&vout_p[PARAM_N1_32 - 1], vcmp, vmask);

        vmask = _mm256_alignr_epi8(valign_mask1, vmask, 1);
    }

    for (; i < 32; i++) {
        vcmp = _mm256_set1_epi8(outp[32 * PARAM_N1_32 - 1 - i]);

        incr_ge_mask(&vout_p[PARAM_N1_32 - 1], vcmp, vmask);

        vmask = _mm256_alignr_epi8(valign_mask2, vmask, 1);
    }

    vmask = _mm256_setzero_si256();

    for (; i < 48; i++) {
        vcmp = _mm256_set1_epi8(outp[32 * PARAM_N1_32 - 1 - i]);

        incr_ge_mask(&vout_p[PARAM_N1_32 - 2], vcmp, vmask);
        incr_ge(&vout_p[PARAM_N1_32 - 1], vcmp);

        vmask = _mm256_alignr_epi8(valign_mask1, vmask, 1);
    }

    for (; i < 64; i++) {
        vcmp = _mm256_set1_epi8(outp[32 * PARAM_N1_32 - 1 - i]);

        incr_ge_mask(&vout_p[PARAM_N1_32 - 2], vcmp, vmask);
        incr_ge(&vout_p[PARAM_N1_32 - 1], vcmp);

        vmask = _mm256_alignr_epi8(valign_mask2, vmask, 1);
    }

    vmask = _mm256_setzero_si256();

#if PARAM_N1 == 79
    for (; i < PARAM_N1; i++) {
#else
        for (; i < 80; i++) {
#endif  // PARAM_N1 == 79
        vcmp = _mm256_set1_epi8(outp[32 * PARAM_N1_32 - 1 - i]);

        incr_ge_mask(&vout_p[PARAM_N1_32 - 3], vcmp, vmask);

        for (j = PARAM_N1_32 - 2; j < PARAM_N1_32; j++) {
            incr_ge(&vout_p[j], vcmp);
        }

        vmask = _mm256_alignr_epi8(valign_mask1, vmask, 1);
    }

#if PARAM_N1 > 79
    #if PARAM_N1 == 83
    for (; i < PARAM_N1; i++) {
#else
    for (; i < 96; i++) {
#endif  // PARAM_N1 == 83
        vcmp = _mm256_set1_epi8(outp[32 * PARAM_N1_32 - 1 - i]);

        incr_ge_mask(&vout_p[PARAM_N1_32 - 3], vcmp, vmask);

        for (j = PARAM_N1_32 - 2; j < PARAM_N1_32; j++) {
            incr_ge(&vout_p[j], vcmp);
        }

        vmask = _mm256_alignr_epi8(valign_mask2, vmask, 1);
    }

#if PARAM_N1 > 83
    vmask = _mm256_setzero_si256();

    // Since one of the parameters is 112, the include guard is not necessary here
    for (; i < 112; i++) {
        vcmp = _mm256_set1_epi8(outp[32 * PARAM_N1_32 - 1 - i]);

        incr_ge_mask(&vout_p[PARAM_N1_32 - 4], vcmp, vmask);

        for (j = PARAM_N1_32 - 3; j < PARAM_N1_32; j++) {
            incr_ge(&vout_p[j], vcmp);
        }

        vmask = _mm256_alignr_epi8(valign_mask1, vmask, 1);
    }

#if PARAM_N1 > 112
#if PARAM_N1 == 116
    for (; i < PARAM_N1; i++) {
#else
    for (; i < 128; i++) {
#endif  // PARAM_N1 == 116
        vcmp = _mm256_set1_epi8(outp[32 * PARAM_N1_32 - 1 - i]);

        incr_ge_mask(&vout_p[PARAM_N1_32 - 4], vcmp, vmask);

        for (j = PARAM_N1_32 - 3; j < PARAM_N1_32; j++) {
            incr_ge(&vout_p[j], vcmp);
        }

        vmask = _mm256_alignr_epi8(valign_mask2, vmask, 1);
    }

#if PARAM_N1 > 116
    vmask = _mm256_setzero_si256();

    for (; i < 144; i++) {
        vcmp = _mm256_set1_epi8(outp[32 * PARAM_N1_32 - 1 - i]);

        incr_ge_mask(&vout_p[PARAM_N1_32 - 5], vcmp, vmask);

        for (j = PARAM_N1_32 - 4; j < PARAM_N1_32; j++) {
            incr_ge(&vout_p[j], vcmp);
        }

        vmask = _mm256_alignr_epi8(valign_mask1, vmask, 1);
    }

    for (; i < PARAM_N1; i++) {
        vcmp = _mm256_set1_epi8(outp[32 * PARAM_N1_32 - 1 - i]);

        incr_ge_mask(&vout_p[PARAM_N1_32 - 5], vcmp, vmask);

        for (j = PARAM_N1_32 - 4; j < PARAM_N1_32; j++) {
            incr_ge(&vout_p[j], vcmp);
        }

        vmask = _mm256_alignr_epi8(valign_mask2, vmask, 1);
    }
#endif  // PARAM_N1 > 116
#endif  // PARAM_N1 > 112
#endif  // PARAM_N1 > 83
#endif  // PARAM_N1 > 79
#endif

    memcpy(out_p, &outp[32 * PARAM_N1_32 - PARAM_N1], PARAM_N1);
    return idx;
}

#if 0
void sig_perk_signature_perm_to_bytes(uint8_t* sb, const perk_signature_t* signature) {
    unsigned idx = 0;

    memset(sb, 0, ((PARAM_PERM_COMPRESSION_BITS * PARAM_TAU)+ 7) / 8);

    for (int i = 0; i < PARAM_TAU; i++) {
        idx = sig_perk_perm_encode(signature->responses[i].z2_pi, sb, idx);
    }
}

int sig_perk_signature_perm_from_bytes(perk_signature_t* signature, const uint8_t* sb) {
    int idx = 0;
    for (int i = 0; i < PARAM_TAU; i++) {
        idx = sig_perk_perm_decode(sb, idx, signature->responses[i].z2_pi);
        if (idx < 0) {
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}
#endif
