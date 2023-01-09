#ifndef ORANGE_OS_INCLUDE_MM_H
#define ORANGE_OS_INCLUDE_MM_H
#include "types.h"
#include "list.h"

#define MAX_ORDER 10
#define PAGE_SIZE 4096

/* log2(PAGE_SIZE) */
#define LPAGE_SIZE 10

typedef struct {
    u32 *map;
    int size;       /* size of bitmap array */
} free_area_t;

#define SET_BIT(bmap, bit) \
    ((unsigned int *)(bmap))[(bit) / 32] |= (0x80000000U >> ((bit) % 32))
#define CLEAR_BIT(bmap, bit) \
    ((unsigned int*)(bmap))[(bit) / 32] &= ~(0x80000000U >> ((bit) % 32))
#define TEST_BIT(bmap, bit) \
    (((unsigned int *)(bmap))[(bit) / 32] & (0x80000000U >> ((bit) % 32)))

u32 seg2phys(u16 seg);

#endif //ORANGE_OS_INCLUDE_MM_H
