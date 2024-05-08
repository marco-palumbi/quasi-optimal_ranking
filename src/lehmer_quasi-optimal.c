
/**
 * @file parsing_permutations.c
 * @brief Implementation of parsing functions
 */

#include "parsing_permutations.h"
// #include "parameters.h"

#include "divide_ct.h"
// #include "data_structures.h"

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
#define MIN(a, b) ((a) < (b) ? a : b)

static unsigned store_n_bit_in_bytearray(uint8_t bytearray[], unsigned bit_index, uint32_t val, unsigned n) {
    unsigned remaining = n;
    while (remaining > 0) {
        const unsigned pos_bit = bit_index % 8;
        const unsigned pos_byte = bit_index / 8;
        const unsigned free_bits = 8 - pos_bit;
        const unsigned n1 = MIN(free_bits, remaining);

        if (pos_bit == 0) {
            bytearray[pos_byte] = val;
        } else {
            bytearray[pos_byte] |= val << pos_bit;
        }

        val >>= n1;
        remaining -= n1;
        bit_index += n1;
    }
    return bit_index;
}

static unsigned load_n_bit_from_bytearray(uint32_t *val, unsigned n, const uint8_t bytearray[], unsigned bit_index) {
    unsigned remaining = n;
    *val = 0;
    while (remaining > 0) {
        const unsigned last_bit = (bit_index + remaining - 1) % 8;
        const unsigned pos_byte = (bit_index + remaining - 1) / 8;
        const unsigned available_bits = last_bit + 1;
        const unsigned n1 = MIN(available_bits, remaining);
        *val <<= n1;

        *val |= (bytearray[pos_byte] >> (available_bits - n1)) & ((1U << n1) - 1);

        remaining -= n1;
    }
    return bit_index + n;
}

static unsigned lehmer_encode_block(uint8_t bytearay[], unsigned bit_index, unsigned k, const perm_t in_p) {
    uint32_t block = 0;
    for (unsigned i = j_k[k] - 1; i > j_k[k-1] - 1; i--) {
        int count = 0;

        for (int j = (PARAM_N1 - i); j < PARAM_N1; ++j) {
            if (in_p[j] < in_p[PARAM_N1 - 1 - i]) {
                count++;
            }
        }
        block = count + ((i + 1) * block);
    }

    return store_n_bit_in_bytearray(bytearay, bit_index, block, bit_per_block[k]);
}

static int unpack_index_block(perm_t d_i, unsigned k, const uint8_t bytearray[], unsigned bit_index) {
    uint32_t block = 0;

    unsigned idx = load_n_bit_from_bytearray(&block, bit_per_block[k], bytearray, bit_index);
    if (block >= max_per_block[k]){
        return -1;
    }
    unsigned i;
    for (i = j_k[k-1] + 1; i < j_k[k]; i++) {
        uint32_t reminder = div_ct(block, i);
        d_i[PARAM_N1 - i] = block - reminder * i;
        block = reminder;
    }
    d_i[PARAM_N1 - i] = block;

    return idx;
}


unsigned sig_perk_perm_encode(const perm_t in_p, uint8_t* code, unsigned idx) {

    for (unsigned i = 1; i < n_i_k; i++) {
        idx = lehmer_encode_block(code, idx, i, in_p);
    }
    return idx;
}

int sig_perk_perm_decode(const uint8_t* code, int idx, perm_t out_p) {

    out_p[PARAM_N1 - 1] = 0;
    for (unsigned i = 1; i < n_i_k; i++) {
        idx = unpack_index_block(out_p, i, code, idx);
        if (idx < 0){
            return -1;
        }
    }

    uint8_t used[PARAM_N1] = {0};
    for (int i = 0; i < PARAM_N1; ++i) {
        int count = 0;
        for (int j = 0; j < PARAM_N1; ++j) {
            if (!used[j])
                count += 1;
            if (count == out_p[i] + 1) {
                out_p[i] = j;
                used[j] = 1;
                break;
            }
        }
    }
    return idx;
}

#if 0
void sig_perk_signature_perm_to_bytes(uint8_t* sb, const perk_signature_t* signature) {
    unsigned idx = 0;

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
