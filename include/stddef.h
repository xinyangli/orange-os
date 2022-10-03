#ifndef ORANGE_OS_STDDEF_H
#define ORANGE_OS_STDDEF_H
typedef unsigned long size_t;
#define NULL ((void *)0)

#define bool _Bool
enum {
  false = 0,
  true = 1
};

#endif //ORANGE_OS_STDDEF_H