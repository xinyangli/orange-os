# ORANGE-OS

## 运行

使用 `make` 进行编译，执行后会在根目录创建引导扇区 `dbr.img`、 Loader `loader.bin` 及 内核文件 `okernel`。

使用 `scripts/mkfd.sh` 可以创建一个新的 FAT12 软盘镜像 a.img 和一个新的 FAT32 硬盘镜像 c.img，并将Loader拷贝到 a.img 中，将 `okernel` 拷贝到 c.img 中。

执行 `make bochs` 可以直接使用bochs运行项目。（可能需要对 .bochsrc做调整）
