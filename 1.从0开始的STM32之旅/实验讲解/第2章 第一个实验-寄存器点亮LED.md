# 第二章 我们的第一个实验-用寄存器点亮LED

## 1. 硬件设计

    在本教程中STM32芯片与LED灯的连接见图， 这是一个RGB灯，里面由红蓝绿三个小灯构成，使用PWM控制时可以混合成256不同的颜色。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LEDreg009.png)

    由图可知，3个LED灯的阳极引出连接到3.3V电源，阴极通过限流电阻引入至STM32的3个GPIO引脚中，所以我们需要控制这三个引脚输出高低电平，就可以控制LED亮灭。 

    我们的目标就是把GPIO的引脚设置成推挽输出模式并且默认下拉，输出低电平，如此LED就亮

## 2. 前期配置

    主要是一些启动文件的配置，配置好了以后我们以后只需复制工程，不用再重新配置，所以虽然这一章是点亮LED灯，但是还是有很多内容

### 2.1 启动文件

    名为“startup_stm32f10x_hd.s”的文件，它里边使用汇编语言写好了基本程序，当STM32芯片上电启动的时候，首先会执行这里的汇编程序， 从而建立起C语言的运行环境，所以我们把这个文件称为启动文件。

    startup_stm32f10x_hd.s文件由官方提供，一般有需要也是在官方的基础上修改，不会自己完全重写。该文件从 ST 固件库里面找到， 找到该文件后把启动文件添加到工程里面即可。不同型号的芯片以及不同编译环境下使用的汇编文件是不一样的，但功能相同。

### 2.2 stm32f10x.h文件

    如何控制寄存器。我们知道寄存器就是给一个已经分配好地址的特殊的内存空间取的一个别名， 这个特殊的内存空间可以通过指针来操作。在编程之前我们要先实现寄存器映射，有关寄存器映射的代码都统一写在stm32f10x.h文件中，这个我们当然可以直接写或者直接拿官方的，这个实验比较简单，我们就自己写吧

```c
// 前期准备-外设地址定义
/*片上外设基地址  */
#define PERIPH_BASE         ((unsigned int)0x40000000)
/*总线基地址，GPIO都挂载到APB2上 */
#define APB2PERIPH_BASE     (PERIPH_BASE + 0x10000) // 地址偏移
/* AHB总线基地址 */
#define AHBPERIPH_BASE      (PERIPH_BASE + 0x20000) // 地址偏移
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
#include "stm32f10x.h"

int main(void)
{
    while(1)
    {

    }
}
```

## 3. 配置GPIO模式

    首先我们需要把连接到LED的GPIO引脚PB0配置成输出模式，即配置GPIO的端口配置低寄存器CRL，见图GPIO端口控制低寄存器CRL，CRL中包含0-7号引脚，每个引脚占用4个寄存器位。MODE位用来配置输出的速度，CNF位用来配置各种输入输出模式。在这里我们把PB0配置为通用推挽输出， 输出的速度为10M

```c
// 配置输出模式
// 清空控制PB0的端口位
GPIOB_CRL &= ~(0x0F << (4*0)); // 0000 1111->0000 0000
// 配置PB0为通用推挽输出，速度为10M
GPIOB_CRL |= (1<<4*0); // 0000 0001 -> 0000 0001
```

    可能刚开始有点看不懂代码，没问题，下面我细致讲解一下：

- `GPIOB_CRL` 是一个用于配置 GPIOB 端口位的寄存器。在STM32F10X系列中，每个GPIO端口（比如GPIOB）都有控制寄存器（比如GPIOB_CRL和GPIOB_CRH）用于配置对应端口位的输入、输出模式、速率等。

- `&=` 是位与运算符，用来将 GPIOB_CRL 寄存器中控制 PB0 端口位的配置清空。（&= 是如何清空的涉及C语言基础和二进制）
  
  - `(0x0F << (4*0))` 会生成一个二进制数，其低四位为1，其余位为0，这个数的二进制表示是0000 1111。在这个代码中，`(4*0)` 实际上就是0，所以左移0位并没有改变原数。
  
  - `~(0x0F << (4*0))` 对刚才生成的二进制数取反，得到了一个值为1111 0000的二进制数，用来清除特定端口位的配置。
  
  - `GPIOB_CRL &= ~(0x0F << (4*0))` 通过按位与操作符 `&`，将 GPIOB_CRL 寄存器中控制 PB0 端口位的配置与之前生成的用来清除配置的二进制数进行按位与操作。这样就实现了将对应端口位的配置清空，使其恢复到默认状态。

- `GPIOB_CRL |= (1<<4*0)`：`|=` 是按位或赋值运算符，它将 GPIOB_CRL 寄存器中对应位设置为 1。在这里，`(1<<4*0)` 的结果是 `1`，所以这个操作的效果就是将 GPIOB_CRL 寄存器中控制 PB0 引脚的相应位设置为 1，从而配置 PB0 引脚为输出模式。（CRL第四位配置为0001，即可设置GPIO端口0为输出模式，输出为10M）

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LEDreg010.png)

简化解释代码：

    在代码中，我们先把控制PB0的端口位清0，然后再向它赋值“0000 0001”，从而使GPIOB0引脚设置成输出模式，速度为10M。

    代码中使用了 **&=~**、**|=** 这种操作方法是为了避免影响到寄存器中的其它位，因为寄存器不能按位读写，假如我们直接给CRL寄存器赋值：

```c
GPIOB_CRL = 0x0000001;
```

    这时CRL的的低4位被设置成“0001”输出模式，但其它GPIO引脚就有意见了，因为其它引脚的MODER位都已被设置成输入模式。

### 3.1 控制引脚输出电平

    在输出模式时，对端口位设置/清除寄存器BSRR寄存器、端口位清除寄存器BRR和ODR寄存器写入参数即可控制引脚的电平状态， 其中操作BSRR和BRR最终影响的都是ODR寄存器，然后再通过ODR寄存器的输出来控制GPIO。为了一步到位，我们在这里直接操作ODR寄存器来控制GPIO的电平。

```c
// 控制引脚输出电平
// PB0输出低电平
GPIOB_ODR &= ~(1<<0);
```

1. `(1<<0)`：这是一个位运算，将数字 `1` 左移了 `0` 位，结果仍然是 `1`。这里的目的是创建一个掩码，用来设置为要修改的引脚的状态。

2. `~(1<<0)`：`~` 是按位取反运算符，它会将 `1<<0` 的结果进行按位取反，得到一个只有对应位为 0 的结果。

3. `GPIOB_ODR &= ~(1<<0)`：`&=` 是按位与并赋值运算符，它将 GPIOB_ODR 寄存器中的对应位与之前创建的掩码进行按位与操作，并将结果重新赋值给 GPIOB_ODR 寄存器。这个操作的效果就是将 PB0 引脚的输出状态清零，即设置为低电平。（我们学过51，知道LED低电平点亮）

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LEDreg011.png)

### 3.2 开启外设时钟

    设置完GPIO的引脚，控制电平输出，以为现在总算可以点亮 LED 了吧，其实还差最后一步。由于STM32的 外设很多，为了降低功耗， 每个外设都对应着一个时钟，在芯片刚上电的时候这些时钟都是被关闭的，如果想要外设工作，必须把相应的时钟打开。

    STM32 的所有外设的时钟由一个专门的外设来管理，叫 RCC（这个又是一大重点，我们后面会讲解的，现在只需了解即可）

    所有的 GPIO都挂载到 APB2 总线上，具体的时钟由APB2外设时钟使能寄存器(RCC_ APB2ENR)来控制。

```c
// 开启端口时钟

// 开启 GPIOB 端口时钟
RCC_APB2ENR |= (1<<3); // 0000 0001 -> 0000 1000
```

- `RCC_APB2ENR` 是 RCC 寄存器中的一个寄存器，它用于控制 APB2 总线上的外设时钟。在STM32系列微控制器中，GPIOB对应APB2总线上的外设，因此启用GPIOB时钟的位通常在 `RCC_APB2ENR` 寄存器中。

- `(1<<3)` 将数字1左移3位，得到一个二进制数 `00001000`，对应了 GPIOB 的时钟使能位。

- `|=` 运算符执行按位或运算并赋值，将 GPIOB 的时钟使能位置为1。

因此，执行这行代码后，GPIOB 的时钟就会被打开，允许你正常地使用 GPIOB 外设。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/LEDreg012.png)

## 4. 点亮LED

    开启时钟，配置引脚模式，控制电平，经过这三步，我们总算可以控制一个 LED了：

```c
// main文件中控制LED灯的代码

int main(void)
{
    // 首先开启GPIOB端口时钟
    RCC_APB2ENR |= (1<<3); // GPIOB
    // 首先清空控制PB0的端口位再写入
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

## 5. 小结

    这一章的实验还是比较简单的，毕竟是点亮一个LED灯，但还是比51难以理解的，我们不妨和51比较一下：

- 51怎么实现点亮LED呢？我们只需要控制一个引脚输出为低电平就行了，然后引脚和LED一连就行了

- 但是32我们先要把对应的时钟打开（RCC）

- 然后还要清空端口，配置模式（我们这里是推挽输出速度10M）

- 使PB0输出低电平，大功告成

- 殊途同归还是要使GPIO输出为低电平（只不过比51多了一些步骤就是了）

毕竟是我们自己写的库函数，而且还是直接操作寄存器，后面我们借助官方库的力量进行重写，那时候就简单不少了（提示：结构体）

---

2024.7.20第一次修订

2024.8.18 第二次修订，后期不再维护

2024.12.8 修补部分内容
