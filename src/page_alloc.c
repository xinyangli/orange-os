#include "types.h"
#include "mm.h"

free_area_t free_area[MAX_ORDER]; /* array of free space bitmap */

PM alloc_pages(unsigned int order) {
  unsigned long pos = 0; /* position of available bit in bitmap*/
  unsigned int corder = order; /* current order to search */
  PM paddr = NULL;
  while(true) {
    pos = 0;
    for(int i = 0; i < free_area[corder].size; i++) {
      if (free_area[corder].map[i] == 0) {
        pos += 32;
      } else {
        pos += clz(free_area[corder].map[i]);
      }
    }

    if (pos == free_area[corder].size * 32) {
      /* no available space in request order */
      /* find in higher order and split */
      ++corder;
      continue;
    }

    SET_BIT(free_area[corder].map, pos);
    for(--corder; corder > order; --corder) {
      pos <<= 1;
      CLEAR_BIT(free_area[corder].map, pos ^ 1); /* set buddy to free */
    }
    return (PM)(pos << LPAGE_SIZE << order);
  }
}

void free_pages(PM paddr, unsigned int order) {
  unsigned int corder = order;
  unsigned long pos = (unsigned long) paddr >> LPAGE_SIZE >> order;
  while(corder < MAX_ORDER) {
    if (TEST_BIT(free_area[corder].map, pos ^ 1)) {
      CLEAR_BIT(free_area[corder].map, pos);
      /* buddy is occupied, stop */
      break;
    }
    /* buddy is free */
    SET_BIT(free_area[corder].map, pos);
    ++corder; pos >>= 1;
  }
}
