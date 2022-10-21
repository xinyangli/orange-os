# 文件内容简介

## 宏定义、函数库

### lib.inc

包含在保护模式 32 位代码段内，使用了栈和数据段。用于显示数字和字符串。

### pm.inc

一些描述符相关的宏定义。

### utils.inc

将一些常用的代码定义成了宏

### s8259a.inc

定义了对 8259a 的操作，中断向量表的初始化和恢复。

### handler.inc

包含在保护模式 32 位代码段内，使用了栈和数据段。作为中断处理程序。

## 主程序

### int.asm

写了个计时器中断。效果是屏幕上出现一个在 A 到 Z 间循环跳动的红色字符。在循环结束屏蔽中断后，该字符会停止跳动。

### int2.asm

在 int.asm 的基础上，加上了键盘中断。效果是按下和松开键盘时，屏幕上出现一个在 A 到 Z 间循环跳动的红色字符。

### int3.asm

结合大一下学期的知识，修改实模式中断向量表，在实模式下实现了上面的键盘中断。

### int4.asm

区别于 int3.asm，同样修改了中断向量表，但在自定义功能完成后，将控制权交还给原本的处理程序，由原本的处理程序继续完成中断。

效果是，程序退出后，在 freedos 下打字，除了像往常一样能正常输入外，还能在右上角看到一个会动的红色字符。