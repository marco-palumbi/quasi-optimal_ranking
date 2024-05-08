#ifndef DIVIDE_CT_H
#define DIVIDE_CT_H

#include <stdint.h>
#include "config.h"

#define MAX_DIV 150

extern uint32_t m_tbl[MAX_DIV + 1];
extern uint32_t s_tbl[MAX_DIV + 1];

static inline uint32_t div_ct(uint32_t num, uint32_t divisor) {
    return (((uint64_t)num * m_tbl[divisor]) >> 32) >> s_tbl[divisor];
}

#endif
