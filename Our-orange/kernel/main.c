
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
							main.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
													Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

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
#include "proto.h"

/*****************************************************************************
 *                               kernel_main
 *****************************************************************************/
/**
 * jmp from kernel.asm::_start.
 *
 *****************************************************************************/
PUBLIC int kernel_main()
{
	disp_str("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
			 "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	int i, j, eflags, prio;
	u8 rpl;
	u8 priv; /* privilege */

	struct task *t;
	struct proc *p = proc_table;

	char *stk = task_stack + STACK_SIZE_TOTAL;

	for (i = 0; i < NR_TASKS + NR_PROCS; i++, p++, t++)
	{
		if (i >= NR_TASKS + NR_NATIVE_PROCS)
		{
			p->p_flags = FREE_SLOT;
			continue;
		}

		if (i < NR_TASKS)
		{ /* TASK */
			t = task_table + i;
			priv = PRIVILEGE_TASK;
			rpl = RPL_TASK;
			eflags = 0x1202; /* IF=1, IOPL=1, bit 2 is always 1 */
			prio = 15;
		}
		else
		{ /* USER PROC */
			t = user_proc_table + (i - NR_TASKS);
			priv = PRIVILEGE_USER;
			rpl = RPL_USER;
			eflags = 0x202; /* IF=1, bit 2 is always 1 */
			prio = 5;
		}

		strcpy(p->name, t->name); /* name of the process */
		p->p_parent = NO_TASK;

		if (strcmp(t->name, "INIT") != 0)
		{
			p->ldts[INDEX_LDT_C] = gdt[SELECTOR_KERNEL_CS >> 3];
			p->ldts[INDEX_LDT_RW] = gdt[SELECTOR_KERNEL_DS >> 3];

			/* change the DPLs */
			p->ldts[INDEX_LDT_C].attr1 = DA_C | priv << 5;
			p->ldts[INDEX_LDT_RW].attr1 = DA_DRW | priv << 5;
		}
		else
		{ /* INIT process */
			unsigned int k_base;
			unsigned int k_limit;
			int ret = get_kernel_map(&k_base, &k_limit);
			assert(ret == 0);
			init_desc(&p->ldts[INDEX_LDT_C],
					  0, /* bytes before the entry point
						  * are useless (wasted) for the
						  * INIT process, doesn't matter
						  */
					  (k_base + k_limit) >> LIMIT_4K_SHIFT,
					  DA_32 | DA_LIMIT_4K | DA_C | priv << 5);

			init_desc(&p->ldts[INDEX_LDT_RW],
					  0, /* bytes before the entry point
						  * are useless (wasted) for the
						  * INIT process, doesn't matter
						  */
					  (k_base + k_limit) >> LIMIT_4K_SHIFT,
					  DA_32 | DA_LIMIT_4K | DA_DRW | priv << 5);
		}

		p->regs.cs = INDEX_LDT_C << 3 | SA_TIL | rpl;
		p->regs.ds =
			p->regs.es =
				p->regs.fs =
					p->regs.ss = INDEX_LDT_RW << 3 | SA_TIL | rpl;
		p->regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl;
		p->regs.eip = (u32)t->initial_eip;
		p->regs.esp = (u32)stk;
		p->regs.eflags = eflags;

		p->ticks = p->priority = prio;

		p->p_flags = 0;
		p->p_msg = 0;
		p->p_recvfrom = NO_TASK;
		p->p_sendto = NO_TASK;
		p->has_int_msg = 0;
		p->q_sending = 0;
		p->next_sending = 0;

		for (j = 0; j < NR_FILES; j++)
			p->filp[j] = 0;

		stk -= t->stacksize;
	}

	k_reenter = 0;
	ticks = 0;

	p_proc_ready = proc_table;

	init_clock();
	init_keyboard();

	restart();

	while (1)
	{
	}
}

/*****************************************************************************
 *                                get_ticks
 *****************************************************************************/
PUBLIC int get_ticks()
{
	MESSAGE msg;
	reset_msg(&msg);
	msg.type = GET_TICKS;
	send_recv(BOTH, TASK_SYS, &msg);
	return msg.RETVAL;
}

/**
 * @struct posix_tar_header
 * Borrowed from GNU `tar'
 */
struct posix_tar_header
{						/* byte offset */
	char name[100];		/*   0 */
	char mode[8];		/* 100 */
	char uid[8];		/* 108 */
	char gid[8];		/* 116 */
	char size[12];		/* 124 */
	char mtime[12];		/* 136 */
	char chksum[8];		/* 148 */
	char typeflag;		/* 156 */
	char linkname[100]; /* 157 */
	char magic[6];		/* 257 */
	char version[2];	/* 263 */
	char uname[32];		/* 265 */
	char gname[32];		/* 297 */
	char devmajor[8];	/* 329 */
	char devminor[8];	/* 337 */
	char prefix[155];	/* 345 */
						/* 500 */
};

/*****************************************************************************
 *                                untar
 *****************************************************************************/
/**
 * Extract the tar file and store them.
 *
 * @param filename The tar file.
 *****************************************************************************/
void untar(const char *filename)
{
	printf("[extract `%s'\n", filename);
	int fd = open(filename, O_RDWR);
	assert(fd != -1);

	char buf[SECTOR_SIZE * 16];
	int chunk = sizeof(buf);

	while (1)
	{
		read(fd, buf, SECTOR_SIZE);
		if (buf[0] == 0)
			break;

		struct posix_tar_header *phdr = (struct posix_tar_header *)buf;

		/* calculate the file size */
		char *p = phdr->size;
		int f_len = 0;
		while (*p)
			f_len = (f_len * 8) + (*p++ - '0'); /* octal */

		int bytes_left = f_len;
		int k;
		int fdout = open(phdr->name, O_CREAT | O_RDWR);
		if (fdout == -1)
		{
			printf("    failed to extract file: %s\n", phdr->name);
			printf(" aborted]");
			return;
		}
		printf("    %s (%d bytes)\n", phdr->name, f_len);
		memcpy(check_table[check_index].filename, phdr->name, strlen(phdr->name));
        check_table[check_index].filename[strlen(phdr->name)] = '\0';
        check_table[check_index].checksum = 0;
		while (bytes_left)
		{
			int iobytes = min(chunk, bytes_left);
			read(fd, buf,
				 ((iobytes - 1) / SECTOR_SIZE + 1) * SECTOR_SIZE);
			for(k = 0; k < iobytes; k++)
			{
				check_table[check_index].checksum ^= buf[k] & 0xFF;
			}
			write(fdout, buf, iobytes);
			bytes_left -= iobytes;
		}
		printf("check_table[%d] -- filename:%s checksum:%d\n",
            check_index, check_table[check_index].filename, check_table[check_index].checksum);
        check_index++;
		close(fdout);
	}

	close(fd);

	printf(" done]\n");
}

/*****************************************************************************
 *                                show_proc_table
 *****************************************************************************/
/**
 * print all the proc info
 *****************************************************************************/
int show_proc_table()
{
	struct proc *p = proc_table;
	int i = 0, flag = 1;
	for (i = 0; i < NR_TASKS + NR_PROCS && flag; i++, p++)
	{
		printf("[%2d] proc_name:%s ", i, p->name);
		printf("ticks:%d ", p->ticks);
		printf("status:");
		if (p->p_flags == 0)
			printf("READY...");
		else if (p->p_flags & SENDING)
			printf("SENDING...");
		else if (p->p_flags & RECEIVING)
			printf("RECEIVING...");
		else if (p->p_flags & HANGING)
			printf("HANGING...");
		else if (p->p_flags & FREE_SLOT)
		{
			printf("FREE_SLOT...");
			flag = 0; // jump out the loop
		}
		else
		{
			printf("NOT RECOGNIZED FLAG");
		}
		printf("\n");
	}
	return 0;
}

/*****************************************************************************
 *                                shabby_shell
 *****************************************************************************/
/**
 * A very very simple shell.
 *
 * @param tty_name  TTY file name.
 *****************************************************************************/
void shabby_shell(const char *tty_name)
{
	int fd_stdin = open(tty_name, O_RDWR);
	assert(fd_stdin == 0);
	int fd_stdout = open(tty_name, O_RDWR);
	assert(fd_stdout == 1);

	char rdbuf[128];
	int pid;

	while (1)
	{
		write(1, "$ ", 2);
		int r = read(0, rdbuf, 70);
		rdbuf[r] = 0;

		int argc = 0;
		char *argv[PROC_ORIGIN_STACK];
		char *p = rdbuf;
		char *s;
		int word = 0;
		char ch;
		do
		{
			ch = *p;
			if (*p != ' ' && *p != 0 && !word)
			{
				s = p;
				word = 1;
			}
			if ((*p == ' ' || *p == 0) && word)
			{
				word = 0;
				argv[argc++] = s;
				*p = 0;
			}
			p++;
		} while (ch);
		argv[argc] = 0;

		char *child_argv[10];
		int i, index = 0, cnt = 0;
		for (i = 0; i < argc + 1; i++)
		{
			if (strcmp(argv[i], "&") == 0 || i == argc)
			{ // 当遇到&或者扫描到末尾则执行一次命令
				child_argv[index] = 0;
				if (strcmp(child_argv[0], "show_proc_table") == 0)
				{
					show_proc_table();
				}
				else
				{
					int fd = open(child_argv[0], O_RDWR);
					if (fd == -1)
					{
						if (rdbuf[0])
						{
							write(1, "{", 1);
							write(1, child_argv[0], strlen(child_argv[0]));
							write(1, "}\n", 2);
						}
					}
					else
					{
						close(fd);
						cnt += 1; // 每创建一个子进程cnt就加1
						pid = fork();
						milli_delay(1000);
						if (pid == 0)
						{ /* child */
							execv(child_argv[0], child_argv);
							break;
						}
					}
				}
				index = 0;
			}
			else
			{
				child_argv[index++] = argv[i];
			}
		}
		for (i = 0; i < cnt; i++)
		{ // 等待所有子进程
			int status;
			wait(&status);
		}
	}

	close(1);
	close(0);
}

/*****************************************************************************
 *                                Init
 *****************************************************************************/
/**
 * The hen.
 *
 *****************************************************************************/
void Init()
{
	int fd_stdin = open("/dev_tty0", O_RDWR);
	assert(fd_stdin == 0);
	int fd_stdout = open("/dev_tty0", O_RDWR);
	assert(fd_stdout == 1);

	printf("Init() is running ...\n");

	/* extract `cmd.tar' */
	untar("/cmd.tar");

	char *tty_list[] = {"/dev_tty1", "/dev_tty2"};

	int i;
	for (i = 0; i < sizeof(tty_list) / sizeof(tty_list[0]); i++)
	{
		int pid = fork();
		if (pid != 0)
		{ /* parent process */
			printf("[parent is running, child pid:%d]\n", pid);
		}
		else
		{ /* child process */
			printf("[child is running, pid:%d]\n", getpid());
			close(fd_stdin);
			close(fd_stdout);

			shabby_shell(tty_list[i]);
			assert(0);
		}
	}

	while (1)
	{
		int s;
		int child = wait(&s);
		printf("child (%d) exited with status: %d.\n", child, s);
	}

	assert(0);
}

/*======================================================================*
							   TestA
 *======================================================================*/
void TestA()
{
	for (;;)
	{
		milli_delay(1000);
		// printf("A\n");
	}
}

/*======================================================================*
							   TestB
 *======================================================================*/
void TestB()
{
	for (;;)
		;
}

/*======================================================================*
							   TestB
 *======================================================================*/
void TestC()
{
	for (;;)
		;
}

/*****************************************************************************
 *                                panic
 *****************************************************************************/
PUBLIC void panic(const char *fmt, ...)
{
	int i;
	char buf[256];

	/* 4 is the size of fmt in the stack */
	va_list arg = (va_list)((char *)&fmt + 4);

	i = vsprintf(buf, fmt, arg);

	printl("%c !!panic!! %s", MAG_CH_PANIC, buf);

	/* should never arrive here */
	__asm__ __volatile__("ud2");
}
