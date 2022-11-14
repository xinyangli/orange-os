target remote localhost:1234
layout asm
symbol-file okernel
directory kernel

break 0x1203bc
break __handler_clock
