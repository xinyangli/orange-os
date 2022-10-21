# ORANGE-OS

## 运行

使用 `make` 进行编译，执行后会在根目录创建引导扇区 `dbr.img`、 Loader `loader.bin` 及 内核文件 `okernel`。

使用 `scripts/mkfd.sh` 可以创建一个新的 FAT12 软盘镜像，并将上述文件拷贝进去。

在根目录下添加 `.bochsrc` 文件，然后执行 `bochs`。
