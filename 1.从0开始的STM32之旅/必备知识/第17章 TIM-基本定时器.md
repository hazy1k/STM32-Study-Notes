# 第十七章 TIM-基本定时器

## 1. 定时器分类

STM32F1 系列中，除了互联型的产品，共有 8个定时器，分为基本定时器，通用定时器和高级定时器。 基本定时器 TIM6 和 TIM7 是一个 16位的只能向上计数的定时器，只能定时，没有外部 IO。 通用定时器 TIM2/3/4/5是一个 16位的可以向上/下计数的定时器，可以定时，可以输出比较，可以输入捕捉， 每个定时器有四个外部IO。高级定时器 TIM1/8是一个 16位的可以向上/下计数的定时器，可以定时，可以输出比较， 可以输入捕捉，还可以有三相电机互补输出信号，每个定时器有8 个外部 IO。更加具体的分类详情见图 [定时器分类](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/TIM_basic.html#id2) 。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/TIMbas002.png)

## 2. 基本定时器功能框图讲解

基本定时器的核心是时基，不仅基本定时器有，通用定时器和高级定时器也有。学习定时器时，我们先从简单的基本定时器学起， 到了后面的通用和高级定时器的学习中，我们直接跳过时基部分的讲解即可。 基本定时器的功能框图见图 [基本定时器功能框图](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/TIM_basic.html#id4) 。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/TIMbas003.png)

### 2.1 时钟源

定时器时钟 TIMxCLK，即内部时钟 CK_INT，经 APB1预分频器后分频提供，如果APB1 预分频系数等于 1，则频率不变， 否则频率乘以 2，库函数中 APB1预分频的系数是2，即 PCLK1=36M，所以定时器时钟 TIMxCLK=36*2=72M。

### 2.2 计时器时钟

定时器时钟经过 PSC 预分频器之后，即 CK_CNT，用来驱动计数器计数。PSC是一个16 位的预分频器， 可以对定时器时钟 TIMxCLK 进行 1~65536之间的任何一个数进行分频。具体计算方式为：CK_CNT=TIMxCLK/(PSC+1)。

### 2.3 计数器

计数器 CNT 是一个 16 位的计数器，只能往上计数，最大计数值为65535。当计数达到自动重装载寄存器的时候产生更新事件，并清零从头开始计数。

### 2.4 自动重装载寄存器

自动重装载寄存器 ARR 是一个 16位的寄存器，这里面装着计数器能计数的最大数值。当计数到这个值的时候，如果使能了中断的话，定时器就产生溢出中断。

### 2.5 定时时间的计算

定时器的定时时间等于计数器的中断周期乘以中断的次数。计数器在 CK_CNT的驱动下，计一个数的时间则是 CK_CLK的倒数， 等于：1/（TIMxCLK/(PSC+1)），产生一次中断的时间则等于：1/（CK_CLK * ARR）。如果在中断服务程序里面设置一个变量time， 用来记录中断的次数，那么就可以计算出我们需要的定时时间等于：1/CK_CLK* (ARR+1)*time。

## 3. 定时器初始化结构体详解

在标准库函数头文件stm32f10x_tim.h中对定时器外设建立了四个初始化结构体，基本定时器只用到其中一个即TIM_TimeBaseInitTypeDef， 具体的见 [代码清单:基本定时器-1](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/TIM_basic.html#id11) ，其他三个我们在高级定时器章节讲解。

```c
typedef struct {
    uint16_t TIM_Prescaler;          // 预分频器
    uint16_t TIM_CounterMode;        // 计数模式
    uint32_t TIM_Period;             // 定时器周期
    uint16_t TIM_ClockDivision;      // 时钟分频
    uint8_t TIM_RepetitionCounter;   // 重复计算器
} TIM_TimeBaseInitTypeDef;
```

(1) TIM_Prescaler： 定时器预分频器设置，时钟源经该预分频器才是定时器时钟，它设定TIMx_PSC寄存器的值。可设置范围为0至65535，实现1至65536分频。

(2) TIM_CounterMode： 定时器计数方式，可是在为向上计数、向下计数以及三种中心对齐模式。基本定时器只能是向上计数，即TIMx_CNT只能从0开始递增，并且无需初始化。

(3) TIM_Period： 定时器周期，实际就是设定自动重载寄存器的值，在事件生成时更新到影子寄存器。可设置范围为0至65535。

(4) TIM_ClockDivision： 时钟分频，设置定时器时钟CK_INT频率与数字滤波器采样时钟频率分频比，基本定时器没有此功能，不用设置。

(5) TIM_RepetitionCounter： 重复计数器，属于高级控制寄存器专用寄存器位，利用它可以非常容易控制输出PWM的个数。这里不用设置。

虽然定时器基本初始化结构体有5个成员，但对于基本定时器只需设置其中两个就可以，想想使用基本定时器就是简单。
