# 第九章 GPIO-位带操作

## 1. 位带简介

    位操作就是单独的对一个比特位读和写，在51单片机里我们见到过很多次了（sbit实现位定义）但是STM32没有这样的关键字，通过访问位带别名区来实现。

    在STM32中，有两个地方实现了位带，一个是SRAM区的最低1MB空间，另一个是外设区最低1MB空间。 这两个1MB的空间除了可以像正常的RAM一样操作外，他们还有自己的位带别名区，位带别名区把这1MB的空间的每一个位膨胀成一个32位的字， 当访问位带别名区的这些字时，就可以达到访问位带区某个比特位的目的。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/GPIObi002.png)

### 1.1 外设位带区

    外设外带区的地址为：0X40000000~0X40100000，大小为1MB，这1MB的大小在103系列大/中/小容量型号的单片机中包含了片上外设的全部寄存器， 这些寄存器的地址为：0X40000000~0X40029FFF。外设位带区经过膨胀后的位带别名区地址为：0X42000000~0X43FFFFFF， 这个地址仍然在CM3 片上外设的地址空间中。在103系列大/中小容量型号的单片机里面，0X40030000~0X4FFFFFFF属于保留地址， 膨胀后的32MB位带别名区刚好就落到这个地址范围内，不会跟片上外设的其他寄存器地址重合。

    STM32的全部寄存器都可以通过访问位带别名区的方式来达到访问原始寄存器比特位的效果，这比51单片机强大很多。 因为51单片机里面并不是所有的寄存器都是可以比特位操作，有些寄存器还是得字节操作，比如SBUF。

### 1.2 SRAM位带区

    SRAM的位带区的地址为：0X2000 0000~X2010 0000，大小为1MB，经过膨胀后的位带别名区地址为：0X2200 0000~0X23FF FFFF， 大小为32MB。操作SRAM的比特位这个用得很少。

### 1.3 位带区和位带别名区地址转换

    位带区的一个比特位经过膨胀之后，虽然变大到4个字节，但是还是LSB才有效。有人会问这不是浪费空间吗， 要知道STM32的系统总线是32位的，按照4个字节访问的时候是最快的，所以膨胀成4个字节来访问是最高效的。

    我们可以通过指针的形式访问位带别名区地址从而达到操作位带区比特位的效果。那这两个地址直接如何转换，我们简单介绍一下。

#### 1.3.1 外设位带别名区地址

    对于片上外设位带区的某个比特，记它所在字节的地址为 A,位序号为 n(0<=n<=31)(n的范围根据具体寄存器能控制的位决定)，则该比特在别名区的地址为：

```c
AliasAddr= =0x42000000+ (A-0x40000000)*8*4 +n*4
```

    0X42000000是外设位带别名区的起始地址，0x40000000是外设位带区的起始地址，（A-0x40000000）表示该比特前面有多少个字节， 一个字节有8位，所以*8，一个位膨胀后是4个字节，所以*4，n表示该比特在A地址的序号，因为一个位经过膨胀后是四个字节，所以也*4。

#### 1.3.2 SRAM位带别名区地址

    对于SRAM位带区的某个比特，记它所在字节的地址为 A,位序号为 n(0<=n<=31)(n的范围根据具体寄存器能控制的位决定)，则该比特在别名区的地址为：

```c
AliasAddr= =0x22000000+ (A-0x20000000)*8*4 +n*4
```

    公式分析同上。

#### 1.3.3 统一公式

    为了方便操作，我们可以把这两个公式合并成一个公式，把“位带地址+位序号”转换成别名区地址统一成一个宏。

```c
// 把“位带地址+位序号”转换成别名地址的宏
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x02000000+((addr & 0x00FFFFFF)<<5)+(bitnum<<2))
```

    addr & 0xF0000000是为了区别SRAM还是外设，实际效果就是取出4或者2，如果是外设， 则取出的是4，+0X02000000之后就等于0X42000000，0X42000000是外设别名区的起始地址。 如果是SRAM，则取出的是2，+0X02000000之后就等于0X22000000，0X22000000是SRAM别名区的起始地址。

    addr & 0x00FFFFFF 屏蔽了高三位，相当于减去0X20000000或者0X40000000，但是为什么是屏蔽高三位？ 因为外设的最高地址是：0X20100000，跟起始地址0X20000000相减的时候，总是低5位才有效， 所以干脆就把高三位屏蔽掉来达到减去起始地址的效果，具体屏蔽掉多少位跟最高地址有关。SRAM同理分析即可。 <<5相当于*8*4，<<2相当于*4，这两个我们在上面分析过。

    最后我们就可以通过指针的形式操作这些位带别名区地址，最终实现位带区的比特位操作。

```c
// 把一个地址转换成一个指针
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr))

// 把位带别名区地址转换成指针
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))
```

## 2. 软件设计

    外设的位带区，覆盖了全部的片上外设的寄存器，我们可以通过宏为每个寄存器的位都定义一个位带别名地址，从而实现位操作。 但这个在实际项目中不是很现实，也很少人会这么做，我们在这里仅仅演示下GPIO中ODR和IDR这两个寄存器的位操作。

    从手册中我们可以知道ODR和IDR这两个寄存器对应GPIO基址的偏移是12和8，我们先实现这两个寄存器的地址映射，其中GPIOx_BASE在库函数里面有定义。

### 2.1 GPIO寄存器映射

```c
// 代码清单:位带操作-1 GPIO ODR 和 IDR 寄存器映射

// GPIO ODR 和 IDR 寄存器地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C

#define GPIOA_IDR_Addr    (GPIOA_BASE+8)  //0x40010808
#define GPIOB_IDR_Addr    (GPIOB_BASE+8)  //0x40010C08
#define GPIOC_IDR_Addr    (GPIOC_BASE+8)  //0x40011008
#define GPIOD_IDR_Addr    (GPIOD_BASE+8)  //0x40011408
#define GPIOE_IDR_Addr    (GPIOE_BASE+8)  //0x40011808
#define GPIOF_IDR_Addr    (GPIOF_BASE+8)  //0x40011A08
#define GPIOG_IDR_Addr    (GPIOG_BASE+8)  //0x40011E08
```

    现在我们就可以用位操作的方法来控制GPIO的输入和输出了，其中宏参数n表示具体是哪一个IO口，n(0,1,2…15)。 这里面包含了端口A~G ，并不是每个单片机型号都有这么多端口，使用这部分代码时，要查看你的单片机型号，如果是64pin的则最多只能使用到C端口。

### 2.2 GPIO位操作

```c
// 代码清单:位带操作-2 GPIO 输入输出位操作

// 单独操作 GPIO的某一个IO口，n(0,1,2...15),n表示具体是哪一个IO口
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入
```

### 2.3 主函数

    该工程我们直接从LED-库函数 操作移植过来，有关LED GPIO 初始化和软件延时等函数我们直接用， 修改的是控制GPIO输出的部分改成了位操作。该实验我们让IO口输出高低电平来控制LED的亮灭，负逻辑点亮。具体使用哪一个IO和点亮方式由硬件平台决定。

```c
int main(void)
{
    // 程序来到main函数之前，启动文件：statup_stm32f10x_hd.s已经调用
    // SystemInit()函数把系统时钟初始化成72MHZ
    // SystemInit()在system_stm32f10x.c中定义
    // 如果用户想修改系统时钟，可自行编写程序修改

    LED_GPIO_Config();

    while(1) 
    {
        // PB0 = 0,点亮LED
        PBout(0)= 0;
        SOFT_Delay(0x0FFFFF);

        // PB1 = 1,熄灭LED
        PBout(0)= 1;
        SOFT_Delay(0x0FFFFF);
    }
}
```
