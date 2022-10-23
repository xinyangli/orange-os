#ifndef ORANGE_OS_LOADER_H
#define ORANGE_OS_LOADER_H
#include "types.h"
typedef struct fat_superblock_t {
    u16 size_per_sector;
    u8 sectors_per_cluster;
    u16 reserved_sectors_num;
    u8 FATs_num;
    u32 sectors_num;
    u32 sectors_per_FAT;
    u32 root_clus;
} fat_superblock_t;

typedef struct fat_entry_t {
    char *name; /* 短目录项名 或 长目录项名 */
    int n_len;  /* name 的长度，通过n_len大小可以判断是长文件名还是短文件名 */
    u8 attr;    /* 从目录项中直接读取的属性 */
    union {
        /* 短目录项中文件/目录起始簇号，长目录项中序数项 */
        u32 i_first;
        u8 ordinal;
    };
    u32 size; /* 文件大小 */
} fat_entry_t;
#endif // ORANGE_OS_LOADER_H