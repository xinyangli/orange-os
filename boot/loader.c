#include "asm.h"
#include "loader.h"
#include "elf.h"

#define SECTSIZE 512
#define CLUSSIZE 512
static fat_superblock_t fat_superblock;
static u8 img_buf[CLUSSIZE], fat_buf[SECTSIZE];
static int fat_sector = 0; /* fat_buf中保存的fat表为fat的第fat_sector扇区 */
static int fat_offset, data_sector;
struct elfhdr *elf;

static void read_sect(void *dst, u32 offset, int cnt);
static u32 next_clus(u32 clus);
static void load_dbr();
static void load_kernel(u32 start_clus, void *kernel_addr);

static inline void itohstr(int num, char *buf);
static inline int strncmp(const char *s1, const char *s2, size_t n);

static void memset(void *dst, u8 c, size_t siz) {
    // TODO: Optimize performance
    for(int i = 0; i < siz; i++) {
        *(u8*)dst = c;
    }
}
void* memcpy(void *dst, void *src, size_t siz);

__attribute__((noreturn)) void main() {
    char s[] = {'Y', 0xF, 'e', 0xF, 's', 0xF};
    char buf[18] = {0};
    fat_superblock_t *sb = &fat_superblock;
    void (*entry)(void) = NULL;
    __builtin_memcpy((void *)0xB8000, s, 6);
    outb(0x1F6, 0xE0);
    load_dbr();
    itohstr(data_sector, buf);
    __builtin_memcpy((void *)0xB8010, buf, 18);

    read_sect(fat_buf, sb->reserved_sectors_num + fat_sector, 1);
    /* Parse root cluster */
    u32 clus = sb->root_clus;
    do {
        read_sect(img_buf, data_sector + clus * sb->sectors_per_cluster,
                  sb->sectors_per_cluster);
        for (u8 *p_buf = img_buf; p_buf < img_buf + CLUSSIZE; p_buf += 0x20) {
            if (strncmp("OKERNEL     ", (char *)p_buf, 12) == 0) {
                u32 start_clus =
                    *(u16 *)(p_buf + 0x14) << 16 | *(u16 *)(p_buf + 0x1A);
                load_kernel(start_clus, (void *)0x100000);
                /* Jump to kernel */
                entry = (void (*) (void))(elf->entry);
                entry();
            }
        }
        clus = next_clus(clus);
    } while ((clus & 0x0FFFFFFFu) >= 0x0FFFFFF8u);
    while (1)
        ;
}

void wait_disk(void) {
    // Wait for disk ready.
    while ((inb(0x1F7) & 0xC0) != 0x40)
        ;
}

// Read a single sector at offset into dst.
static void read_sect(void *dst, u32 offset, int cnt) {
    // Issue command.
    while (cnt--) {
        wait_disk();
        outb(0x1F2, 1); // count = 1
        outb(0x1F3, offset);
        outb(0x1F4, offset >> 8);
        outb(0x1F5, offset >> 16);
        outb(0x1F6, (offset >> 24) | 0xE0);
        outb(0x1F7, 0x20); // cmd 0x20 - read sectors

        // Read data.
        wait_disk();
        insl(0x1F0, dst, SECTSIZE / 4);
        dst += SECTSIZE;
    }
}

static void load_dbr() {
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
    data_sector = sb->reserved_sectors_num +
                  (sb->FATs_num * sb->sectors_per_FAT -
                   2 * sb->sectors_per_cluster); /* 簇编号从2开始 */
}

static inline void read_clus(u32 cluster, u8 *buf) {
    fat_superblock_t *sb = &fat_superblock;
    read_sect(buf, data_sector + cluster * sb->sectors_per_cluster,
              sb->sectors_per_cluster);
}

static inline u32 next_clus(u32 cluster) {
    u32 nxt = 0x0FFFFFFFU;
    fat_superblock_t *sb = &fat_superblock;
    if (fat_sector != cluster / (SECTSIZE / 4)) {
        fat_sector = cluster / (SECTSIZE / 4);
        read_sect(
            fat_buf,
            sb->reserved_sectors_num + fat_sector, 1);
    }
    nxt = *(((u32 *)fat_buf) + cluster % (SECTSIZE / 4));
    return nxt;
}

static void load_kernel(u32 start_clus, void *kernel_addr) {
    u32 clus = start_clus;
    void *addr = kernel_addr;
    struct proghdr *ph, *ph_end;
    u8 *pa;
    do {
        read_sect(addr,
                  data_sector + clus * fat_superblock.sectors_per_cluster,
                  fat_superblock.sectors_per_cluster);
        addr += CLUSSIZE;
        clus = next_clus(clus);
    } while ((clus & 0x0FFFFFFFu) < 0x0FFFFFF8u);
    
    elf = (struct elfhdr *)(kernel_addr);
    /* Copy elf sections to memory position */
    ph = (struct proghdr*) ((u8 *) elf + elf->phoff);
    ph_end = ph + elf->phnum;
    for(; ph < ph_end; ph++) {
        pa = (u8 *)ph->paddr;
        if (ph->type == ELF_PROG_LOAD) {
            memcpy(pa, kernel_addr + ph->off, ph->filesz);
            memset(pa + ph->filesz, 0, ph->memsz - ph->filesz);
        }
    }
}


static inline void itohstr(int num, char *buf) {
    for(int i = 15; i >= 0; i -= 2) {
        buf[i] = 0xF;
        buf[i-1] = num % 16;
        buf[i-1] += buf[i-1] > 9 ? 'A' - 10 : '0';
        num /= 16;
    }
    buf[16] = '$';
    buf[17] = 0xF;
}

static inline int strncmp(const char *s1, const char *s2, size_t n) {
    int i_s1 = 0, i_s2 = 0;
    while (n && s1[i_s1] != '\0' && s2[i_s2] != '\0' && s1[i_s1] == s2[i_s2]) {
        --n; ++i_s1; ++i_s2;
    }
    if (n == 0 || s1[i_s1] == s2[i_s2]) {
        /* Same str */
        return 0;
    } else {
        return -1;
    }
}
