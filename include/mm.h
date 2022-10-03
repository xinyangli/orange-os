#ifndef ORANGE_OS_INCLUDE_MM_H
#define ORANGE_OS_INCLUDE_MM_H
#include "types.h"

#define MAX_ORDER 10
#define PAGE_SIZE 4096

/* log2(PAGE_SIZE) */
#define LPAGE_SIZE 10

#define SET_BIT(bmap, bit) \
    ((unsigned int *)(bmap))[(bit) / 32] |= (0x80000000U >> ((bit) % 32))
#define CLEAR_BIT(bmap, bit) \
    ((unsigned int*)(bmap))[(bit) / 32] &= ~(0x80000000U >> ((bit) % 32))
#define TEST_BIT(bmap, bit) \
    (((unsigned int *)(bmap))[(bit) / 32] & (0x80000000U >> ((bit) % 32)))

int clz(unsigned int x) {
  return __builtin_clz(x);
}

VM alloc_pages(unsigned int order);

#endif //ORANGE_OS_INCLUDE_MM_H
