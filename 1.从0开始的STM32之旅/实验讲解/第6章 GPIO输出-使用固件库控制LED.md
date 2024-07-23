# 第六章 GPIO输出—使用固件库控制LED

## 1. 硬件设计

    在开发板上面与LED的连接见图 [LED硬件原理图](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/GPIO_output.html#led) ，这是一个RGB灯，里面由红蓝绿三个小灯构成， 使用PWM控制时可以混合成256不同的颜色。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/GPIOou002.png)

## 2. 软件设计

    为了使工程更加有条理和方便迁移，我们把LED灯控制相关的代码分别独立存储。在“工程模板”之上新建一个文件夹“led”，这个文件里我们用来存放“led.c”和“led.h”

### 2.1 编程目的

1. 使能GPIO端口时钟；

2. 初始化GPIO目标引脚为推挽输出模式；

3. 编写简单测试程序，控制GPIO引脚输出高、低电平。

### 2.2 代码分析

#### 2.2.1 LED灯脚宏定义

```c
// LED控制引脚相关的宏

// R-红色
#define LED1_GPIO_PORT      GPIOB
#define LED1_GPIO_CLK       RCC_APB2Periph_GPIOB
#define LED1_GPIO_PIN       GPIO_Pin_5
// G-绿色
#define LED2_GPIO_PORT      GPIOB
#define LED2_GPIO_CLK       RCC_APB2Periph_GPIOB
#define LED2_GPIO_PIN       GPIO_Pin_0
// B-蓝色
#define LED3_GPIO_PORT      GPIOB
#define LED3_GPIO_CLK       RCC_APB2Periph_GPIOB
#define LED3_GPIO_PIN       GPIO_Pin_1
```

    以上代码分别把控制LED灯的GPIO端口、GPIO引脚号以及GPIO端口时钟封装起来了。在实际控制的时候我们就直接用这些宏，以达到应用代码硬件无关的效果。

    其中的GPIO时钟宏“RCC_APB2Periph_GPIOB”是STM32标准库定义的GPIO端口时钟相关的宏，它的作用与“GPIO_Pin_x”这类宏类似，是用于指示寄存器位的， 方便库函数使用，下面初始化GPIO时钟的时候可以看到它的用法。

#### 2.2.2 控制LED灯亮灭状态的宏定义

      为了方便控制LED灯，我们把LED灯常用的亮、灭及状态反转的控制也直接定义成宏：

```c
// 控制LED亮灭的宏

/* 直接操作寄存器的方法控制IO */
#define digitalHi(p,i)       {p->BSRR=i;}    // 输出为高电平
#define digitalLo(p,i)       {p->BRR=i;}     // 输出低电平
#define digitalToggle(p,i)   {p->ODR ^=i;}   // 输出反转状态


/* 定义控制IO的宏 */
#define LED1_TOGGLE       digitalToggle(LED1_GPIO_PORT,LED1_GPIO_PIN) // 状态反转
#define LED1_OFF          digitalHi(LED1_GPIO_PORT,LED1_GPIO_PIN) // 关闭-高电平
#define LED1_ON           digitalLo(LED1_GPIO_PORT,LED1_GPIO_PIN) // 打开-低电平

#define LED2_TOGGLE       digitalToggle(LED2_GPIO_PORT,LED2_GPIO_PIN)
#define LED2_OFF          digitalHi(LED2_GPIO_PORT,LED2_GPIO_PIN)
#define LED2_ON           digitalLo(LED2_GPIO_PORT,LED2_GPIO_PIN)

#define LED3_TOGGLE       digitalToggle(LED2_GPIO_PORT,LED3_GPIO_PIN)
#define LED3_OFF          digitalHi(LED2_GPIO_PORT,LED3_GPIO_PIN)
#define LED3_ON           digitalLo(LED2_GPIO_PORT,LED3_GPIO_PIN)

/* 基本混色，后面高级用法使用PWM可混出全彩颜色,且效果更好 */

//红
#define LED_RED LED1_ON; LED2_OFF; LED3_OFF

//绿
#define LED_GREEN LED1_OFF; LED2_ON; LED3_OFF

//蓝
#define LED_BLUE LED1_OFF; LED2_OFF; LED3_ON

//黄(红+绿)
#define LED_YELLOW LED1_ON; LED2_ON; LED3_OFF

//紫(红+蓝)
#define LED_PURPLE LED1_ON; LED2_OFF; LED3_ON

//青(绿+蓝)
#define LED_CYAN LED1_OFF; LED2_ON; LED3_ON

//白(红+绿+蓝)
#define LED_WHITE LED1_ON; LED2_ON; LED3_ON

//黑(全部关闭)
#define LED_RGBOFF LED1_OFF; LED2_OFF; LED3_OFF
```

    这部分宏控制LED亮灭的操作是直接向BSRR、BRR和ODR这三个寄存器写入控制指令来实现的，对BSRR写1输出高电平， 对BRR写1输出低电平，对ODR寄存器某位进行异或操作可反转位的状态。

    RGB彩灯可以实现混色，如最后一段代码我们控制红灯和绿灯亮而蓝灯灭，可混出黄色效果。

### 2.2.3 LED GPIO初始化函数

    利用上面的宏，编写LED灯的初始化函数：

```c
// 代码清单:GPIO输出-3 LED GPIO初始化函数

void LED_GPIO_Config(void)
{
    /*定义一个GPIO_InitTypeDef类型的结构体*/
    GPIO_InitTypeDef GPIO_InitStructure;

    /*开启LED相关的GPIO外设时钟*/
    RCC_APB2PeriphClockCmd( LED1_GPIO_CLK | LED2_GPIO_CLK | LED3_GPIO_CLK, ENABLE);
    /*选择要控制的GPIO引脚*/
    GPIO_InitStructure.GPIO_Pin = LED1_GPIO_PIN;
    /*设置引脚模式为通用推挽输出*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    /*设置引脚速率为50MHz */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    /*调用库函数，初始化GPIO*/
    GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure);
    /*选择要控制的GPIO引脚*/
    GPIO_InitStructure.GPIO_Pin = LED2_GPIO_PIN;
    /*调用库函数，初始化GPIO*/
    GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStructure);
    /*选择要控制的GPIO引脚*/
    GPIO_InitStructure.GPIO_Pin = LED3_GPIO_PIN;
    /*调用库函数，初始化GPIOF*/
    GPIO_Init(LED3_GPIO_PORT, &GPIO_InitStructure);
    /* 关闭所有led灯   */
    GPIO_SetBits(LED1_GPIO_PORT, LED1_GPIO_PIN);
    /* 关闭所有led灯   */
    GPIO_SetBits(LED2_GPIO_PORT, LED2_GPIO_PIN);
    /* 关闭所有led灯   */
    GPIO_SetBits(LED3_GPIO_PORT, LED3_GPIO_PIN);
}
```

函数执行流程如下：

1. 使用GPIO_InitTypeDef定义GPIO初始化结构体变量，以便下面用于存储GPIO配置。

2. 调用库函数RCC_APB2PeriphClockCmd来使能LED灯的GPIO端口时钟，在前面的章节中我们是直接向RCC寄存器赋值来使能时钟的， 不如这样直观。该函数有两个输入参数，第一个参数用于指示要配置的时钟，如本例中的“RCC_APB2Periph_GPIOB”， 应用时我们使用“|”操作同时配置3个LED灯的时钟；函数的第二个参数用于设置状态，可输入“Disable”关闭或“Enable”使能时钟。

3. 向GPIO初始化结构体赋值，把引脚初始化成推挽输出模式，其中的GPIO_Pin使用宏“LEDx_GPIO_PIN”来赋值，使函数的实现方便移植。

4. 使用以上初始化结构体的配置，调用GPIO_Init函数向寄存器写入参数，完成GPIO的初始化， 这里的GPIO端口使用“LEDx_GPIO_PORT”宏来赋值，也是为了程序移植方便。

5. 使用同样的初始化结构体，只修改控制的引脚和端口，初始化其它LED灯使用的GPIO引脚。

6. 使用宏控制RGB灯默认关闭。

#### 2.2.4 主函数

    编写完LED灯的控制函数后，就可以在main函数中测试了

```c
#include "stm32f10x.h"
#include "led.h"

#define SOFT_DELAY Delay(0x0FFFFF);

void Delay(__IO u32 nCount);

int main(void)
{
    /* LED 端口初始化 */
    LED_GPIO_Config();

    while (1)
    {
        LED1_ON; // 亮
        SOFT_DELAY; // 延时
        LED1_OFF; // 灭

        LED2_ON;             
        SOFT_DELAY;
        LED2_OFF;          

        LED3_ON;             
        SOFT_DELAY;
        LED3_OFF;         

        /*轮流显示 红绿蓝黄紫青白 颜色*/
        LED_RED;
        SOFT_DELAY;

        LED_GREEN;
        SOFT_DELAY;

        LED_BLUE;
        SOFT_DELAY;

        LED_YELLOW;
        SOFT_DELAY;

        LED_PURPLE;
        SOFT_DELAY;

        LED_CYAN;
        SOFT_DELAY;

        LED_WHITE;
        SOFT_DELAY;

        LED_RGBOFF;
        SOFT_DELAY;
    }
}

void Delay(__IO uint32_t nCount)     //简单的延时函数
{
    for (; nCount != 0; nCount--);
}
```

    在main函数中，调用我们前面定义的LED_GPIO_Config初始化好LED的控制引脚，然后直接调用各种控制LED灯亮灭的宏来实现LED灯的控制。

## 3. 小结

    前期写功能实现的代码倒是不难，关键是如何配置和初始化才是难点，虽然每个配置我们都已经讲了，但是既然是小结，在此重新总结一下：

- 首先就是写LED的头文件了，在这个头文件里写什么呢：主要就是定义各种宏-方便后面调试

- 首先就是定义LED1~3的宏，例如：

```c
// LED1对应颜色为红色
#define LED1_GPIO_PORT        GPIOB                 // 用LED1_GPIO_PORT代替GPIOB，输出输入端口     
#define LED1_GPIO_CLK         RCC_APB2Periph_GPIOB // 用LED1_GPIO_CLK代替RCC_APB2Periph_GPIOB，端口时钟
#define LED1_GPIO_PIN         GPIO_Pin_5             // 用LED1_GPIO_PIN代表GPIO_Pin_5，接的哪个引脚    
```

- 接着比较重要的就是定义方法控制IO口实现输出高低电平：

```c
// 前面我们就知道对BSRR写1输出高电平， 对BRR写1输出低电平，对ODR寄存器某位进行异或操作可反转位的状态。
// 所以看注释应该能看懂吧
#define    digitalHi(p, i)         {p->BSRR = i;} // 高电平            
#define    digitalLo(p, i)         {p->BRR  = i;} // 低电平
#define    digitalToggle(p, i)     {p->ODR  ^= i;}// 反转

// 为了更加方便接下来控制LED所以还需定义控制IO的宏
// 这个都不用注释都能看懂
#define LED1_TOGGLE         digitalToggle(LED1_GPIO_PORT, LED1_GPIO_PIN) 
#define LED1_OFF            digitalHi(LED1_GPIO_PORT, LED1_GPIO_PIN)     
#define LED1_ON             digitalLo(LED1_GPIO_PORT, LED1_GPIO_PIN)     

#define LED2_TOGGLE         digitalToggle(LED2_GPIO_PORT, LED2_GPIO_PIN)
#define LED2_OFF            digitalHi(LED2_GPIO_PORT, LED2_GPIO_PIN)
#define LED2_ON             digitalLo(LED2_GPIO_PORT, LED2_GPIO_PIN)

#define LED3_TOGGLE         digitalToggle(LED3_GPIO_PORT, LED3_GPIO_PIN)
#define LED3_OFF            digitalHi(LED3_GPIO_PORT, LED3_GPIO_PIN)
#define LED3_ON             digitalLo(LED3_GPIO_PORT, LED3_GPIO_PIN)
```

- 后面就是多种LED混合了，可有可无

- 开始写LCD.c文件，这个主要就是定义一个结构体，把LED1~3都封装到里面，还配置了推挽输出模式和50MHz速度，就拿LED1举例吧：

```c
/*选择要控制的GPIO引脚*/
GPIO_InitStructure.GPIO_Pin = LED1_GPIO_PIN;

/*调用库函数，初始化GPIO，此时的GPIOB控制的就是LED1的引脚啦*/
GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure);    
```

- 主函数很简单，没什么可说的，至此工程完成

---

2024.7.22第一次修订
