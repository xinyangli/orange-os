#include "asm.h"
#include "loader.h"
#include "klib.h"

#define SECTSIZE 512
#define CLUSSIZE 4096
static fat_superblock_t fat_superblock;
static u8 img_buf[CLUSSIZE], fat_buf[SECTSIZE];
static int fat_sector = 0; /* fat_buf中保存的fat表为fat的第fat_sector扇区 */
static int fat_offset, data_offset;

void read_sect(void *dst, u32 offset, int cnt);
void load_dbr();
void load_kernel(u32 start_clus, void *kernel_addr);

__attribute__((noreturn)) int main() {
    char s[] = {'Y', 0xF, 'e', 0xF, 's', 0xF};
    char buf[18] = {0};
    fat_superblock_t *sb = &fat_superblock;
    __builtin_memcpy((void *)0xB8000, s, 6);
    load_dbr();
    itohstr(data_offset, buf);
    __builtin_memcpy((void *)0xB8010, buf, 18);
}

void wait_disk(void) {
    // Wait for disk ready.
    while ((inb(0x1F7) & 0xC0) != 0x40)
        ;
}

// Read a single sector at offset into dst.
void read_sect(void *dst, u32 offset, int cnt) {
    // Issue command.
    outb(0x1F2, cnt); // count = 1
    outb(0x1F3, offset);
    outb(0x1F4, offset >> 8);
    outb(0x1F5, offset >> 16);
    outb(0x1F6, (offset >> 24) | 0xE0);
    outb(0x1F7, 0x20); // cmd 0x20 - read sectors

    // Read data.
    wait_disk();
    insl(0x1F0, dst, SECTSIZE / 4);
}

void load_dbr() {
    fat_superblock_t *sb = &fat_superblock;
    read_sect(img_buf, 0, 1);
    sb->size_per_sector = *(u16 *)(img_buf + 0x0B);
    sb->sectors_per_cluster = *(u8 *)(img_buf + 0x0D);
    sb->reserved_sectors_num = *(u16 *)(img_buf + 0x0E);
    sb->FATs_num = *(u8 *)(img_buf + 0x10);
    sb->sectors_num = *(u32 *)(img_buf + 0x20);
    sb->sectors_per_FAT = *(u32 *)(img_buf + 0x24);
    sb->root_clus = *(u32 *)(img_buf + 0x2C);
    fat_offset = sb->reserved_sectors_num * sb->size_per_sector;
    data_offset =
        fat_offset + (sb->FATs_num * sb->sectors_per_FAT -
                      2 * sb->sectors_per_cluster) * /* 簇编号从2开始 */
                         sb->size_per_sector;
}

inline void read_clus(u32 cluster, u8 *buf) {
    fat_superblock_t *sb = &fat_superblock;
    u32 cluster_size = sb->sectors_per_cluster * sb->size_per_sector;
    read_sect(buf, data_offset + cluster * cluster_size,
              sb->sectors_per_cluster);
}

inline u32 next_clus(u32 cluster) {
    u32 nxt = 0x0FFFFFFFU;
    if (fat_sector != cluster / (SECTSIZE / 4)) {
        read_sect(fat_buf, fat_offset + cluster, 1);
        fat_sector = cluster / (SECTSIZE / 4);
    }
    nxt = fat_buf[fat_offset % (SECTSIZE / 4)];
    return nxt;
}

void load_kernel(u32 start_clus, void *kernel_addr) {
    u32 clus = start_clus;
    do {
        read_sect(kernel_addr, data_offset + start_clus * CLUSSIZE,
                fat_superblock.sectors_per_cluster);
        kernel_addr += CLUSSIZE;
        clus = next_clus(clus);
    } while ((clus & 0x0FFFFFFFu) >= 0x0FFFFFF8u);
}