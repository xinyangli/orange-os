/*************************************************************************//**
 *****************************************************************************
 * @file   list.c
 * @brief
 *****************************************************************************
 *****************************************************************************/

#include "type.h"
#include "stdio.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "fs.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "keyboard.h"
#include "proto.h"

PUBLIC int do_list(){
    struct inode* dir_inode;
    char filename[MAX_PATH];
    char* dir = fs_msg.BUF2;
    int pointer = 0;
    memset(filename, 0, MAX_FILENAME_LEN);
    if (strip_path(filename, dir, &dir_inode) != 0) {
        return 0;
    }

    int dir_blk0_nr = dir_inode->i_start_sect;
    int nr_dir_blks = (dir_inode->i_size + SECTOR_SIZE - 1) / SECTOR_SIZE;
    struct dir_entry* pde;
    int i, j;
    for (i = 0; i < nr_dir_blks; i++) {
        RD_SECT(dir_inode->i_dev, dir_blk0_nr + i);
        pde = (struct dir_entry*)fsbuf;
        for (j = 0; j < SECTOR_SIZE / DIR_ENTRY_SIZE; j++, pde++) {
            if (!strlen(pde->name))
                continue;
            dir[pointer] = ' ';
            pointer += 1;
            memcpy(dir + pointer, pde->name, strlen(pde->name));
            pointer += strlen(pde->name);
        }
    }
    dir[pointer]='\0';
    return 0;
}
