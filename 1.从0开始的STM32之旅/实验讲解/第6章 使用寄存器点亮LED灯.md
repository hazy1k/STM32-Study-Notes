# 第六章 使用寄存器点亮LED灯

## 1. 硬件设计

    在本教程中STM32芯片与LED灯的连接见图 [LED灯电路连接图](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/LED_register.html#id21) ， 这是一个RGB灯，里面由红蓝绿三个小灯构成，使用PWM控制时可以混合成256不同的颜色。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LEDreg009.png)

    由图可知，3个LED灯的阳极引出连接到3.3V电源，阴极通过限流电阻引入至STM32的3个GPIO引脚中，所以我们需要控制这三个引脚输出高低电平，就可以控制LED亮灭。

    我们的目标就是把GPIO的引脚设置成推挽输出模式并且默认下拉，输出低电平，如此LED就亮

## 2. 前期配置

    主要是一些启动文件的配置，配置好了以后我们以后只需复制工程，不用再重新配置，所以虽然这一章是点亮LED灯，但是还是有很多内容

### 2.1 启动文件

    名为“startup_stm32f10x_hd.s”的文件，它里边使用汇编语言写好了基本程序，当STM32芯片上电启动的时候，首先会执行这里的汇编程序， 从而建立起C语言的运行环境，所以我们把这个文件称为启动文件。

    startup_stm32f10x_hd.s文件由官方提供，一般有需要也是在官方的基础上修改，不会自己完全重写。该文件从 ST 固件库里面找到， 找到该文件后把启动文件添加到工程里面即可。不同型号的芯片以及不同编译环境下使用的汇编文件是不一样的，但功能相同。

    对于启动文件这部分我们主要总结它的功能，不详解讲解里面的代码，其功能如下：

> - 初始化堆栈指针SP;
> 
> - 初始化程序计数器指针PC;
> 
> - 设置堆、栈的大小;
> 
> - 初始化中断向量表;
> 
> - 配置外部SRAM作为数据存储器（这个由用户配置，一般的开发板可没有外部SRAM）;
> 
> - 调用SystemIni() 函数配置STM32的系统时钟。
> 
> - 设置C库的分支入口“__main”（最终用来调用main函数）;

    前面的我们现在无需理解，主要是后面两点，在启动文件中有一段复位后立即执行的程序，代码见 [代码清单:点亮LED-2](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/LED_register.html#led-2) 。 在实际工程中阅读时，可使用编辑器的搜索(Ctrl+F)功能查找这段代码在文件中的位置，搜索Reset_Handler即可找到。

```nasm
;代码清单:点亮LED-2复位后执行的程序

;Reset handler
Reset_Handler    PROC
                EXPORT  Reset_Handler     [WEAK]
    IMPORT  SystemInit
    IMPORT  __main

        LDR     R0, =SystemInit
        BLX     R0
        LDR     R0, =__main
        BX      R0
        ENDP
```

下面我们来解释一下这个代码：

    开头的是程序注释，在汇编里面注释用的是“;”，相当于 C 语言的“//”注释符

    第二行是定义了一个子程序：Reset_Handler。PROC 是子程序定义伪指令。这里就相当于C语言里定义了一个函数，函数名为Reset_Handler。

    第三行 EXPORT 表示 Reset_Handler 这个子程序可供其他模块调用。相当于C语言的函数声明。关键字[WEAK] 表示弱定义， 如果编译器发现在别处定义了同名的函数，则在链接时用别处的地址进行链接，如果其它地方没有定义，编译器也不报错，以此处地址进行链接。

    第四行和第五行 IMPORT 说明 SystemInit 和__main 这两个标号在其他文件，在链接的时候需要到其他文件去寻找。 相当于C语言中，从其它文件引入函数声明。以便下面对外部函数进行调用。

    SystemInit 需要由我们自己实现，即我们要编写一个具有该名称的函数，用来初始化 STM32 芯片的时钟，一般包括初始化AHB、APB等各总线的时钟， 需要经过一系列的配置STM32才能达到稳定运行的状态。其实这个函数在固件库里面有提供，官方已经为我们写好。

    __main 其实不是我们定义的(不要与C语言中的main函数混淆)，这是一个C库函数，当编译器编译时，只要遇到这个标号就会定义这个函数， 该函数的主要功能是：负责初始化栈、堆，配置系统环境，并在函数的最后调用用户编写的 main 函数，从此来到 C 的世界。

    第六行把 SystemInit 的地址加载到寄存器 R0。

    第七行程序跳转到 R0 中的地址执行程序，即执行SystemInit函数的内容。

    第八行把__main 的地址加载到寄存器 R0。

    第九行程序跳转到 R0 中的地址执行程序，即执行__main函数，执行完毕之后就去到我们熟知的 C 世界，进入main函数。

    第十行表示子程序的结束。

    总之，看完这段代码后，了解到如下内容即可：我们需要在外部定义一个SystemInit函数设置STM32的时钟；STM32上电后，会执行SystemInit函数，最后执行我们C语言中的main函数。

### 2.2 stm32f10x.h文件

    看完上个启动文件，我们会想到接下来写Systemlnit和main函数吧？别着急，定义好了SystemInit函数和main我们又能写什么内容？连接LED灯的GPIO引脚， 是要通过读写寄存器来控制的，就这样空着手，如何控制寄存器。我们知道寄存器就是给一个已经分配好地址的特殊的内存空间取的一个别名， 这个特殊的内存空间可以通过指针来操作。在编程之前我们要先实现寄存器映射，有关寄存器映射的代码都统一写在stm32f10x.h文件中

```c
// 代码清单:点亮LED-3 外设地址定义


/*片上外设基地址  */
#define PERIPH_BASE         ((unsigned int)0x40000000)

/*总线基地址，GPIO都挂载到APB2上 */
#define APB2PERIPH_BASE     (PERIPH_BASE + 0x10000)
/* AHB总线基地址 */
#define AHBPERIPH_BASE      (PERIPH_BASE + 0x20000)

/*GPIOB外设基地址*/
#define GPIOB_BASE          (APB2PERIPH_BASE + 0x0C00)

/* GPIOB寄存器地址,强制转换成指针 */
#define GPIOB_CRL           *(unsigned int*)(GPIOB_BASE+0x00)
#define GPIOB_CRH           *(unsigned int*)(GPIOB_BASE+0x04)
#define GPIOB_IDR           *(unsigned int*)(GPIOB_BASE+0x08)
#define GPIOB_ODR           *(unsigned int*)(GPIOB_BASE+0x0C)
#define GPIOB_BSRR          *(unsigned int*)(GPIOB_BASE+0x10)
#define GPIOB_BRR           *(unsigned int*)(GPIOB_BASE+0x14)
#define GPIOB_LCKR          *(unsigned int*)(GPIOB_BASE+0x18)

/*RCC外设基地址*/
#define RCC_BASE           (AHBPERIPH_BASE + 0x1000)
/*RCC的AHB1时钟使能寄存器地址,强制转换成指针*/
#define RCC_APB2ENR        *(unsigned int*)(RCC_BASE+0x18)
```

    GPIO外设的地址跟前面章节讲解的相同，不过此处把寄存器的地址值都直接强制转换成了指针，方便使用。 代码的最后两段是RCC外设寄存器的地址定义，RCC外设是用来设置时钟的，以后我们会详细分析，本实验中只要了解到使用GPIO外设必须开启它的时钟即可。

### 2.3 main文件

    把启动文件放入我们的工程文件夹里，下面就可以正式开始写我们的函数了-main函数

```c
// 示例文件 

#include "stm32f10x.h"                  // Device header

int main(void)
{
    while(1)
    {

    }
}
```

## 3. 配置GPIO模式

    首先我们需要把连接到LED的GPIO引脚PB0配置成输出模式，即配置GPIO的端口配置低寄存器CRL，见图 [GPIO端口控制低寄存器CRL](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/LED_register.html#gpiocrl) 。 CRL中包含0-7号引脚，每个引脚占用4个寄存器位。MODE位用来配置输出的速度，CNF位用来配置各种输入输出模式。在这里我们把PB0配置为通用推挽输出， 输出的速度为10M，具体见 [代码清单:点亮LED-5](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/LED_register.html#led-5) 。

```c
// 代码清单:点亮LED-5 配置输出模式

// 清空控制PB0的端口位
GPIOB_CRL &= ~(0x0F << (4*0));
// 配置PB0为通用推挽输出，速度为10M
GPIOB_CRL |= (1<<4*0);
```

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LEDreg010.png)

下面解释一下这段代码：

    在代码中，我们先把控制PB0的端口位清0，然后再向它赋值“0001 b”，从而使GPIOB0引脚设置成输出模式，速度为10M。

    代码中使用了 **&=~**、**|=** 这种操作方法是为了避免影响到寄存器中的其它位，因为寄存器不能按位读写，假如我们直接给CRL寄存器赋值：

```c
GPIOB_CRL = 0x0000001;
```

    这时CRL的的低4位被设置成“0001”输出模式，但其它GPIO引脚就有意见了，因为其它引脚的MODER位都已被设置成输入模式。

可能有些人C语言基础不是很好，逐步分析一下这个代码吧：

```c
GPIOB_CRL 是一个GPIO的地位寄存器，用来控制GPIOB端口的配置
0x0F是一个十六进制数转换成二进制数为00001111
<<(4*0)相当于没有移动
&= ~ 结合起来的作用是将某个变量的特定位清零，意思就是将GPIOB_CRL 中控制 PB0 的端口位清零。
所以这里的操作可以简单理解为将GPIOB_CRL 的低 4 位清零，对应着 PB0 引脚的配置位。
```

```c
1<<4*0将值 1 左移 4*0 位，结果仍然是 1
然后使用按位或操作符 | 将这个值设置到 GPIOB_CRL 的相应位置上
这样就将 PB0 配置为通用推挽输出模式。
```

### 3.1 控制引脚输出电平

    在输出模式时，对端口位设置/清除寄存器BSRR寄存器、端口位清除寄存器BRR和ODR寄存器写入参数即可控制引脚的电平状态， 其中操作BSRR和BRR最终影响的都是ODR寄存器，然后再通过ODR寄存器的输出来控制GPIO。为了一步到位，我们在这里直接操作ODR寄存器来控制GPIO的电平。 具体见 [代码清单:点亮LED-6](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/LED_register.html#led-6) 。

```c
// 代码清单:点亮LED-6 控制引脚输出电平

// PB0输出低电平
GPIOB_ODR &= ~(1<<0);
```

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LEDreg011.png)

### 3.2 开启外设时钟

    设置完GPIO的引脚，控制电平输出，以为现在总算可以点亮 LED 了吧，其实还差最后一步。由于STM32的 外设很多，为了降低功耗， 每个外设都对应着一个时钟，在芯片刚上电的时候这些时钟都是被关闭的，如果想要外设工作，必须把相应的时钟打开。

    STM32 的所有外设的时钟由一个专门的外设来管理，叫 RCC（reset and clockcontrol）， RCC 在《 STM32 中文参考手册》的第六章。 关于RCC外设中的时钟部分，我们在后面的章节《RCC—使用HSE/HIS配置》中有详细的讲解，这里我们暂时先了解下。

    所有的 GPIO都挂载到 APB2 总线上，具体的时钟由APB2外设时钟使能寄存器(RCC_ APB2ENR)来控制， 具体见 [代码清单:点亮LED-7](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/LED_register.html#led-7) 。

```c
// 代码清单:点亮LED-7 开启端口时钟

// 开启 GPIOB 端口 时钟
RCC_APB2ENR |= (1<<3);
```

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LEDreg012.png)

## 4. 点亮LED

    开启时钟，配置引脚模式，控制电平，经过这三步，我们总算可以控制一个 LED了。现在我们完整组织下用 STM32 控制一个 LED 的代码， 见 [代码清单:点亮LED-8](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/LED_register.html#led-8) 。

```c
// 代码清单:点亮LED-8 main文件中控制LED灯的代码

int main(void)
{
    // 开启GPIOB端口时钟
    RCC_APB2ENR |= (1<<3);

    // 清空控制PB0的端口位
    GPIOB_CRL &= ~( 0x0F<< (4*0));
    // 配置PB0为通用推挽输出，速度为10M
    GPIOB_CRL |= (1<<4*0);

    // 使PB0输出为低电平
    GPIOB_ODR |= (0<<0);

    while(1)
    {

    }
}
```

## 5. 实验现象

    开发板上的LED成功点亮：

![](https://img.picgo.net/2024/05/28/IMG_20240528_2105052fc813d4a7ab59ed.jpg)
