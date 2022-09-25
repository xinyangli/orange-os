# <center>武汉大学国家网络安全学院教学实验报告</center>

|   课程名称   |  操作系统设计与实践  |   实验日期   | 2022.9.24 |
| :----------: | :------------------: | :----------: | :-------: |
| **实验名称** | **保护模式工作机理** | **实验周次** | **第2周** |
|   **姓名**   |       **学号**       |   **专业**   | **班级**  |
|    李心杨    |     202030281022     |   信息安全   |     2     |
|    王宇骥    |    2020302181008     |   信息安全   |     2     |
|    林锟扬    |    2020302181032     |   信息安全   |     2     |
|    郑炳捷    |    2020302181024     |   信息安全   |     2     |

#### 一、实验目的及实验内容

##### 实验目的

1. 理解x86架构下的段式内存管理。
2. 掌握实模式和保护模式下段式寻址的组织方式、关键数据结构、代码组织方式。
3. 掌握实模式与保护模式的切换。
4. 掌握特权级的概念，以及不同特权之间的转移。
5. 了解调用门、任务门的基本概念。

##### 实验内容

1. 认真阅读章节资料，掌握什么是保护模式，弄清关键数据结构：GDT、descriptor、selector、GDTR及其之间关系，阅读pm.inc文件中数据结构以及含义，写出对宏Descriptor的分析。
2. 调试代码，/a/ 掌握从实模式到保护模式的基本方法，画出代码流程图，如果代码/a/中，第71行有dword前缀和没有前缀，编译出来的代码有区别么，为什么，请调试截图。
3. 调试代码，/b/，掌握GDT的构造与切换，从保护模式切换回实模式方法。
4. 调试代码，/c/，掌握LDT切换。
5. 调试代码，/d/掌握一致代码段、非一致代码段、数据段的权限访问规则，掌握CPL、DPL、RPL之间关系，以及段间切换的基本方法。
6. 调试代码，/e/掌握利用调用门进行特权级变换的转移的基本方法。

#### 二、实验环境及实验步骤

 ##### 实验环境

Ubuntu 14.04；bochs 2.7 .

##### 概念分析

- **保护模式**：相比于实模式，保护模式能够支持多任务，支持优先级，并且拥有更强大的寻址能力。

  - GDT：由多个描述符(Descriptor)组成，提供段式存储机制。
  - Descriptor：每一个Descriptor定义一个段，其结构如下：
  
  ![image-20210912110635291](https://img-blog.csdnimg.cn/img_convert/e5ad29915579458760da68d3c17b2064.png)
  
  ​		段基址：共32位，存放在不连续的四个字节中，用于寻址。
  
  ​		段界限：共20位，表示的是段边界的扩展最值。
  
  ​		其余是相关属性位，用这些额外的属性来提高安全性。
  
  - Selector：给出描述符在GDT/LDT的索引号、GDT/LDT标志以及特权级。
  
  <img src="osfs03-1.asset/selector.png" alt="selector" style="zoom: 120%;" />
  
   - GDTR寄存器：保存GDT的起始地址和界限。
  
  ![GDTR](osfs03-1.asset/GDTR.png)
  
- 寻址过程中的各个数据结构的**关系**：① 先从GDTR寄存器中获得GDT基址。② 在GDT中根据Selector确定Descriptor。③ Descriptor给出了段的基址，再根据程序中给出的偏移地址得到最终的线性地址。 ④ 访存。

![QQ图片20210910104225](https://img-blog.csdnimg.cn/img_convert/ed70d022c9583a3a8b7f0b2af97f17ba.png)

##### 代码/a/：从实模式到保护模式

- 代码流程图

![exp2](osfs03-1.asset/exp2.png)

- 运行源码程序

  这里我们采用DOS执行COM文件的方法来运行程序。

  - 按照参考书教程，准备freedos.img和pm.img，并修改bochsrc。

  - 编译pmtest1.asm，并将pmtest1.com复制到虚拟软盘pm.img上。

    ```
    nasm pmtest1.asm -o pmtest1.com
    sudo mount -o loop pm.img /mnt/floppy
    sudo cp pmtest1.com /mnt/floppy
    sudo umount /mnt/floppy
    ```

  - 在FreeDos中运行程序。

  ![image-20220923100921760](osfs03-1.asset/image-20220923100921760.png)

  ​	可以看到在bochs窗口右侧中出现红色的P，说明程序正确运行。

- 关键步骤解释

  - 加载GDTR：前文中已经给出了GDTR的结构，高4字节是GDT基地址，低2字节是GDT的界限。为了加载GDTR，需要首先设置好这6字节的内容。然后利用lgdt指令完成加载即可。

    相关代码如下。

  ```
  ; 为加载 GDTR 作准备
  	xor	eax, eax
  	mov	ax, ds
  	shl	eax, 4
  	add	eax, LABEL_GDT		; eax <- gdt 基地址
  	mov	dword [GdtPtr + 2], eax	; [GdtPtr + 2] <- gdt 基地址
  
  	; 加载 GDTR
  	lgdt	[GdtPtr]
  ```

  - 关中断&打开地址线A20

    保护模式和实模式的中断处理机制不同，所以在切换时需要进行关中断操作。另外，保护模式有更强的访问内存的能力，这要求将A20打开以访问全部的内存。

    相关代码如下。

    ```
    ; 关中断
    	cli
    
    ; 打开地址线A20
    in	al, 92h
    or	al, 00000010b
    out	92h, al
    ```

  - 切换到保护模式

    cr0寄存器的第0位是PE位，将该位置为1，CPU就能够运行于保护模式了。

    相关代码如下。

    ```
    ; 准备切换到保护模式
    mov	eax, cr0
    or	eax, 1
    mov	cr0, eax
    ```

- 程序修改

  将源代码中jmp dword SelectorCode32:0一句中的dword删除，重新编译并运行。结果发现程序仍然能够正常运行，窗口中仍然能够显示p。

  ![image-20220923111144854](osfs03-1.asset/image-20220923111144854.png)

  对源程序和修改程序的对应二进制文件进行反汇编，对比如下。

  ![image-20220923111411120](images\image-20220923111411120.png)

​		不加dword时，地址被解释为16位，即word。在该程序中，16位仍然能够完成正确的跳转。但是这个是不严谨的，在其他情形下，当目标跳转范围较大时，只有dword才能实现正确跳转。

##### 代码/b/：GDT的构造与切换，从保护模式切换为实模式

阅读代码/b/，观察GDT的结构与以及保护模式切换为实模式的方法。

- GDT结构分析

  SECTION.gdt中存放GDT的整个结构。首先是对各个Descriptor的定义。

  ![image-20220923153410680](osfs03-1.asset/image-20220923153410680.png)

  每个Descriptor遵循如下的定义形式：

  | ; usage: Descriptor Base, Limit, Attr                    |
  | -------------------------------------------------------- |
  | ;    Base: dd                                            |
  | ;    Limit: dd (low 20 bits available)                   |
  | ;    Attr: dw (lower 4 bits of higher byte are always 0) |

  然后定义了GdtLen和GdtPtr。最后设置好每一段对应的选择子。

  ![image-20220923153710475](osfs03-1.asset/image-20220923153710475.png)

- 保护模式切换为实模式的代码阐述

  在代码/a/中，已经讨论过实模式切换为保护模式的步骤，这里我们主要关注从保护模式切换为实模式。

  切换为实模式的代码如下。

  ```
  ; 16 位代码段. 由 32 位代码段跳入, 跳出后到实模式
  [SECTION .s16code]
  ALIGN	32
  [BITS	16]
  LABEL_SEG_CODE16:
  	; 跳回实模式:
  	mov	ax, SelectorNormal
  	mov	ds, ax
  	mov	es, ax
  	mov	fs, ax
  	mov	gs, ax
  	mov	ss, ax
  
  	mov	eax, cr0
  	and	al, 11111110b
  	mov	cr0, eax
  
  LABEL_GO_BACK_TO_REAL:
  	jmp	0:LABEL_REAL_ENTRY	; 段地址会在程序开始处被设置成正确的值
  
  Code16Len	equ	$ - LABEL_SEG_CODE16
  
  ; END of [SECTION .s16code]
  ```

  SelectorNormal是一个选择子，它指向Normal描述符。在准备从保护模式切换回实模式前，需要加载一个合适的描述符选择子到有关段寄存器，使得对应段描述符告诉缓冲寄存器包含合适的段界限和属性。Normal描述符就是为了实现这一点。然后将cr0的PE位置为0，最终跳转到REAL_ENTRY段。注意，表面上是jmp 0:LABEL_REAL_ENTRY，但是在程序前面已经对这条指令进行了修改：

  ![image-20220923161116754](osfs03-1.asset/image-20220923161116754.png)

  所以实际上，这条指令在执行时变成了jmp cs_real_mode:LABEL_REAL_ENTRY。这样就能够正确跳转到目标位置。

  LABEL_REAL_ENTRY段的代码如下。主要步骤是：完成关A20，开中断操作，这些与之前从实模式跳转到保护模式的操作是互逆的。最终调用21h中断，返回DOS模式。

  <img src="osfs03-1.asset/image-20220923161245709.png" style="zoom:60%;" />

- 程序运行

  对asm文件编译，并拷贝到磁盘中。在DOS模式中运行该程序。结果如下图：

  ![image-20220923161703295](osfs03-1.asset/image-20220923161703295.png)

  程序运行正确，最终程序运行完毕后，又回到了实模式下的DOS。

##### 代码/d/：一致代码段、非一致代码段、数据段的权限访问规则，CPL、DPL、RPL 之间的关系，以及段间切换的基本方法

- 三类段间的权限访问规则
  
  |                | 特权级低到高 | 特权级高到低 | 相同特权级之间 |
  | ---------      | ----------- | ----------- | --------- |
  | **一致代码段**  |    允许     |    不允许   |   允许     |
  | **非一致代码段** | 不允许  | 不允许 |  允许 |
  | **数据段**  |  不允许 |  允许  |  允许 |

- CPL、DPL 与 RPL
  
  - 三者功能的基本描述：

    CPL、DPL、RPL 都是段访问机制中的特权级表示位。它的值是 0 ~ 3，描述了特权级的高低。（0 为最高，3 为最低）

    其中，**CPL** (Current Privilege Level) 描述当前执行程序或者任务的特权级。存储在 CS 和 SS 的第 0 位和第 1 位。

    **DPL** (Descriptor Privilege Level) 描述段或者门的特权级。存储在描述符的 DPL 字段中。

    **RPL** (Request Privilege Level) 描述选择子的权限级。存储在选择子的第 0 位和第 1 位。

  - 在权限访问规则中的作用：

    我们不难想到，前面所谓的“特权级由低到高”等说法，体现在这里，就是对 CPL 和 DPL 的值进行比较。
    
    通常情况下，CPL 会随着当前代码段权限的变化而变化。特殊的，当程序转移到一致代码段时，CPL 保持不变。

    DPL 则是对试图访问段的程序的 CPL 或 RPL 进行了限制。这一限制在不同类型的段下有不同的体现：

    | 段类型 | 限制要求 |
    | --- | --- |
    | 数据段、调用门 | $CPL \leq DPL$ |
    | 非一致代码段（不使用调用门） | $CPL = DPL$ |
    | 一致代码段与门调用的非一致代码段 | $CPL \geq DPL$ |


    而 RPL 则是在考虑了调用机制后对这一规则的补充。
    
    > 具体的，当程序从低特权级的代码段转移到高特权级的非一致代码段（比如通过系统调用）时，CPL 会改变，当前权限级会提高。  
    > 如果没有 RPL，应用程序就可以借助系统调用的高权限，来非法地访问自己权限不足以访问的段。  
    > 为了避免这一现象，操作系统会在被调用过程接收到从调用过程传来的选择子时，将调用者的 CPL 存储在这个选择子的 RPL 中。综合 RPL 和 CPL，来决定是否有访问段的权限。

- 代码阅读
  
  本节的代码 pmtest4.asm，给出了同权限级的段间转移示例。相比前面，它重要的部分如下：

  首先，是门描述符和选择子的定义。不难发现，它的定义方式与段描述符的方法类似。通过 pm.inc 中的宏 Gate，可以将门的四个部分：目标选择子、偏移、DCount、属性方便地格式化为规定的形式。
  
  代码段中的 LABEL_DESC_CODE_DEST，则是被调用段的描述符。DA_C 和 DA_32 两个参数，表示它是个非一致的 32 位代码段。

  ```x86asm
  [SECTION .gdt]
  ...
  LABEL_DESC_CODE_DEST: Descriptor 0,SegCodeDestLen-1, DA_C+DA_32; 非一致代码段,32
  ...
  LABEL_CALL_GATE_TEST: Gate SelectorCodeDest,   0,     0, DA_386CGate+DA_DPL0
  ...
  SelectorCodeDest	equ	LABEL_DESC_CODE_DEST	- LABEL_GDT
  ...
  SelectorCallGateTest	equ	LABEL_CALL_GATE_TEST	- LABEL_GDT
  ...
  ```

  接下来是被调用代码段的描述符初始化。类似与前面其它的段描述符初始化。

  ```x86asm
  [SECTION .s16]
  ...
  ; 初始化测试调用门的代码段描述符
	xor	eax, eax
	mov	ax, cs
	shl	eax, 4
	add	eax, LABEL_SEG_CODE_DEST
	mov	word [LABEL_DESC_CODE_DEST + 2], ax
	shr	eax, 16
	mov	byte [LABEL_DESC_CODE_DEST + 4], al
	mov	byte [LABEL_DESC_CODE_DEST + 7], ah
  ...
  ```

  通过 call 指令，我们可以对门进行调用。由于此处都是 ring 0，满足非一致代码段的权限访问要求，因此直接访问代码段和调用门都是可行的。

  ```x86asm
  [SECTION .s32]; 32 位代码段. 由实模式跳入.
  ...
  ; 测试调用门（无特权级变换），将打印字母 'C'
	call	SelectorCallGateTest:0
	;call	SelectorCodeDest:0
  ...

  [SECTION .sdest]; 调用门目标段
  [BITS	32]
  LABEL_SEG_CODE_DEST:
  	;jmp	$
  	mov	ax, SelectorVideo
  	mov	gs, ax			; 视频段选择子(目的)
  	mov	edi, (80 * 12 + 0) * 2	; 屏幕第 12 行, 第 0 列。
  	mov	ah, 0Ch			; 0000: 黑底    1100: 红字
  	mov	al, 'C'
  	mov	[gs:edi], ax
  	retf
  SegCodeDestLen	equ	$ - LABEL_SEG_CODE_DEST
  ; END of [SECTION .sdest]
  ...
  ```

  该代码段会在屏幕第 12 行第 0 列显示一个字母 C。它的运行结果如下，符合我们的预期：

  ![](osfs03-1.asset/debug-d.png)


#### 三、实验过程分析与故障记录

- 在对代码/a/进行反汇编时，发现LABEL_SEG_CODE32段的代码与源码有一定的差异。源码中32位代码段如下：

  ```
  LABEL_SEG_CODE32:
  	mov	ax, SelectorVideo
  	mov	gs, ax			; 视频段选择子(目的)
  
  	mov	edi, (80 * 11 + 79) * 2	; 屏幕第 11 行, 第 79 列。
  	mov	ah, 0Ch			; 0000: 黑底    1100: 红字
  	mov	al, 'P'
  	mov	[gs:edi], ax
  
  	; 到此停止
  	jmp	$
  ```

  反汇编后：

  ![image-20220923151238759](osfs03-1.asset/image-20220923151238759.png)

  在 bochs 中进行调试对比，我们不难发现，反汇编程序把对齐用的三个字节的 0 当成了指令。

  ![](osfs03-1.asset/diff-ndisasm.png)

  我们联想到是反汇编默认采用的是16位反汇编。于是修改ndisasm的参数，令其以32位进行反汇编。结果如下：

  ![image-20220923151828913](osfs03-1.asset/image-20220923151828913.png)

  此时能够与源码对应上。

- 在尝试将 pmtest1.asm 作为引导程序运行时出现故障：

  ![](osfs03-1.asset/error-1.png)

  原因是 pmtest1.asm 与实验一的代码不同，它不包含填充到 510 字节和写入 0xaa55 的部分。而且由于它有很多的 section，直接使用 `times 510-($-$$)` 是不行的，因为 \$\$ 针对的是当前段。

  因此，我们需要使用指令来写入 0xaa55。

  ```bash
  echo -ne "\x55\xaa" | dd of=a.img seek=510 bs=1 count=2 conv=notrunc
  ```

  通过 `-e`，来让 echo 输出特殊的转义字符到标准输出中，并作为 dd 的标准输入写入 a.img 的特定位置。

  这样做直接在 bash 或 zsh 下运行时是可行的。但当我们将其写入 makefile 文件中后，却发现无法正常写入：

  ![](osfs03-1.asset/error-2.png)

  它写入了莫名其妙的 0x6e2d。

  经过查表发现，0x6e2d 其实是字符串 "-n" 的 16 进制表示。通过查阅资料得知，make 默认使用 sh 来执行指令，而在高版本 ubuntu 中，为了加快脚本执行的速度，/bin/sh 默认使用的是 dash 而不是 bash。dash 并不支持 -ne 这样的参数。

  在 Makefile 中加上一行声明 shell 即可。

  ```makefile
  SHELL=/bin/bash
  ```

- 

#### 四、实验结果总结

#### 五、个人贡献与体会

#### 六、教师评价

<table>
    <tr>
        <td colspan="3"> 教师评语 </td>
    </tr>
    <tr>
        <td colspan="3"> </br></br></br> </td>
    </tr>
    <tr align="center">
        <td colspan="3"> 教师评分</td>
    </tr>
	<tr align="center">
        <td>姓名</td>
        <td>学号</td>
        <td>分数</td>
    </tr>
    <tr align="center">
        <td>李心杨</td>
        <td>2020302181022</td>
        <td></td>
    </tr>
    <tr align="center">
        <td>王宇骥</td>
        <td>2020302181008</td>
        <td></td>
    </tr>
    <tr align="center">
        <td>林锟扬</td>
        <td>2020302181032</td>
        <td></td>
    </tr>
    <tr align="center">
        <td>郑炳捷</td>
        <td>2020302181024</td>
        <td></td>
    </tr>
	<tr>
        <td colspan="3">教师签名 </br> </br> </td>
	</tr>

