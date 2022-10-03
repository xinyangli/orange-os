#ifndef ORANGE_OS_MM_TYPES
#define ORANGE_OS_MM_TYPES
#include "stddef.h"

/* Memory types */
typedef void *VM;
typedef void *PM;

typedef struct {
  unsigned int *map;
  int size; /* size of bitmap array */
} free_area_t;

#endif //ORANGE_OS_MM_TYPES