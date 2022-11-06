#ifndef ORANGE_OS_MM_TYPES
#define ORANGE_OS_MM_TYPES
#include "stddef.h"

/* Memory types */
typedef void *VM;
typedef void *PM;

typedef struct {
    unsigned int *map;
    int size;       /* size of bitmap array */
} free_area_t;

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef int i32;
typedef short i16;
typedef char i8;

#endif // ORANGE_OS_MM_TY