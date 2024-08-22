# 第九章 SysTick系统定时器

## 1. 导入

    SysTick—系统定时器是属于CM3内核中的一个外设，内嵌在NVIC中。系统定时器是一个24bit的向下递减的计数器， 计数器每计数一次的时间为1/SYSCLK，一般我们设置系统时钟SYSCLK等于72M。当重装载数值寄存器的值递减到0的时候，系统定时器就产生一次中断，以此循环往复。

    因为SysTick是属于CM3内核的外设，所以所有基于CM3内核的单片机都具有这个系统定时器，使得软件在CM3单片机中可以很容易的移植。 系统定时器一般用于操作系统，用于产生时基，维持操作系统的心跳。

## 2. SysTick寄存器

    SysTick—系统定时器有4个寄存器，简要介绍如下。在使用SysTick产生定时的时候，只需要配置前三个寄存器，最后一个校准寄存器不需要使用。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SysTic01.png)

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SysTic02.png)

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SysTic03.png)

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SysTic04.png)

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SysTic05.png)

具体运用可以参考本章的例程

---

2024.8.22 第一次修订，后期不再维护
