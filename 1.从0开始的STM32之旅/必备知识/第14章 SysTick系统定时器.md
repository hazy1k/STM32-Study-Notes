# 第十四章 SysTick系统定时器

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

## 3. SysTick定时实验

    实验目的：利用SysTick产生1s的时基，LED以1s的频率闪烁

### 3.1 硬件设计

    SysTick属于单片机内部的外设，不需要额外的硬件电路，剩下的只需一个LED灯即可。

### 3.2 软件设计

    这里只讲解核心的部分代码，有些变量的设置，头文件的包含等并没有涉及到，完整的代码请参考本章配套的工程。 我们创建了两个文件：bsp_SysTick.c和bsp_ SysTick.h文件用来存放SysTick驱动程序及相关宏定义， 中断服务函数放在stm32f10x_it.c文件中。

#### 3.2.1 编程要点

- 设置重装寄存器的值

- 清楚当前数值寄存器的值

- 配置控制于状态寄存器

#### 3.2.2 代码分析

- SysTick配置库函数

    SysTick 属于内核的外设，有关的寄存器定义和库函数都在内核相关的库文件core_cm3.h中。

```c
__STATIC_INLINE uint32_t SysTick_Config(uint32_t ticks)
{
    // 不可能的重装载值，超出范围
    if ((ticks - 1UL) > SysTick_LOAD_RELOAD_Msk) {
        return (1UL);
    }

    // 设置重装载寄存器
    SysTick->LOAD  = (uint32_t)(ticks - 1UL);

    // 设置中断优先级
    NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);

    // 设置当前数值寄存器
    SysTick->VAL   = 0UL;

    // 设置系统定时器的时钟源为AHBCLK=72M
    // 使能系统定时器中断
    // 使能定时器
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                    SysTick_CTRL_TICKINT_Msk   |
                    SysTick_CTRL_ENABLE_Msk;
    return (0UL);
}
```

    用固件库编程的时候我们只需要调用库函数SysTick_Config()即可，形参ticks用来设置重装载寄存器的值， 最大不能超过重装载寄存器的值224，当重装载寄存器的值递减到0的时候产生中断，然后重装载寄存器的值又重新装载往下递减计数， 以此循环往复。紧随其后设置好中断优先级，最后配置系统定时器的时钟等于AHBCLK=72M，使能定时器和定时器中断，这样系统定时器就配置好了，一个库函数搞定。

    SysTick_Config()库函数主要配置了SysTick中的三个寄存器：LOAD、VAL和CTRL，有关具体的部分看代码注释即可。

- 配置SysTick中断优先级

    在SysTick_Config()库函数还调用了固件库函数NVIC_SetPriority()来配置系统定时器的中断优先级，该库函数也在core_m3.h中定义，原型如下：

```c
__STATIC_INLINE void NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority)
{
    if ((int32_t)IRQn < 0) {
        SCB->SHP[(((uint32_t)(int32_t)IRQn) & 0xFUL)-4UL] =
        (uint8_t)((priority << (8 - __NVIC_PRIO_BITS)) & (uint32_t)0xFFUL);
    } else {
        NVIC->IP[((uint32_t)(int32_t)IRQn)] =
        (uint8_t)((priority << (8 - __NVIC_PRIO_BITS)) & (uint32_t)0xFFUL);
    }
}
```

    函数首先先判断形参IRQn的大小，如果是小于0，则表示这个是系统异常，系统异常的优先级由内核外设SCB的寄存器SHPRx控制， 如果大于0则是外部中断，外部中断的优先级由内核外设NVIC中的IPx寄存器控制。

    因为SysTick属于内核外设，跟普通外设的中断优先级有些区别，并没有抢占优先级和子优先级的说法。在STM32F103中， 内核外设的中断优先级由内核SCB这个外设的寄存器：SHPRx（x=1.2.3）来配置。

    SPRH1-SPRH3是一个32位的寄存器，但是只能通过字节访问，每8个字段控制着一个内核外设的中断优先级的配置。在STM32F103中， 只有位7:4这高四位有效，低四位没有用到，所以内核外设的中断优先级可编程为：0~15，只有16个可编程优先级，数值越小，优先级越高。 如果软件优先级配置相同，那就根据他们在中断向量表里面的位置编号来决定优先级大小，编号越小，优先级越高。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SysTic06.png)

    如果要修改内核外设的优先级，只需要修改下面三个寄存器对应的某个字段即可。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SysTic003.jpeg)

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SysTic003.jpeg)

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SysTic004.png)

    在系统定时器中，配置优先级为(1UL << __NVIC_PRIO_BITS) - 1UL)， 其中宏__NVIC_PRIO_BITS为4，那计算结果就等于15， 可以看出系统定时器此时设置的优先级在内核外设中是最低的，如果要修改优先级则修改这个值即可，范围为：0~15。

```c
// 设置系统定时器中断优先级
NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);
```

    但是，问题来了，刚刚我们只是学习了内核的外设的优先级配置。如果我同时使用了systick和片上外设呢？而且片上外设也刚好需要使用中断， 那systick的中断优先级跟外设的中断优先级怎么设置？会不会因为systick是内核里面的外设，所以它的中断优先级就一定比内核之外的外设的优先级高？

    从《STM32中断应用概览》这章我们知道，外设在设置中断优先级的时候，首先要分组，然后设置抢占优先级和子优先级。 而systick这类内核的外设在配置的时候，只需要配置一个寄存器即可，取值范围为0~15。既然配置方法不同，那如何区分两者的优先级？下面举例说明。

    比如配置一个外设的中断优先级分组为2，抢占优先级为1，子优先级也为1，systick的优先级为固件库默认配置的15。 当我们比较内核外设和片上外设的中断优先级的时候，我们只需要抓住NVIC的中断优先级分组不仅对片上外设有效，同样对内核的外设也有效。 我们把systick的优先级15转换成二进制值就是1111(0b)，又因为NVIC的优先级分组2，那么前两位的11(0b)就是3，后两位的11(0b)也是3。 无论从抢占还是子优先级都比我们设定的外设的优先级低。如果当两个的软件优先级都配置成一样，那么就比较他们在中断向量表中的硬件编号，编号越小，优先级越高。

- SysTick初始化函数

```c
// 启动系统滴答定时器 SysTick
void SysTick_Init(void)
{
    /* SystemFrequency / 1000    1ms中断一次
    * SystemFrequency / 100000  10us中断一次
    * SystemFrequency / 1000000 1us中断一次
    */
    if (SysTick_Config(SystemCoreClock / 100000)) // 1us中断一次 
    {
        /* Capture error */
        while (1);
    }
}
```

    SysTick初始化函数由用户编写，里面调用了SysTick_Config()这个固件库函数， 通过设置该固件库函数的形参，就决定了系统定时器经过多少时间就产生一次中断。

- SysTick定时函数

    现在我们定义一个微秒级别的延时函数，形参为nTime，当用这个形参乘以中断时间TINT就得出我们需要的延时时间， 其中TINT我们已经设置好为10us。关于这个函数的具体调用看注释即可。

```c
/**
* @brief   us延时程序,10us为一个单位
* @param
*   @arg nTime: Delay_us( 1 ) 则实现的延时为 1 * 10us = 10us
* @retval  无
*/
void Delay_us(__IO u32 nTime)
{
    TimingDelay = nTime;

    while (TimingDelay != 0);
}
```

    函数Delay_us()中我们等待TimingDelay为0，当TimingDelay为0的时候表示延时时间到。变量TimingDelay在中断函数中递减， 即SysTick每进一次中断即10us的时间TimingDelay递减一次。

- SysTick中断服务函数

```c
void SysTick_Handler(void)
{
    TimingDelay_Decrement();
}
```

    中断服务函数调用了另外一个函数TimingDelay_Decrement()，原型如下：

```c
/**
* @brief  获取节拍程序
* @param  无
* @retval 无
* @attention  在 SysTick 中断函数 SysTick_Handler()调用
*/
void TimingDelay_Decrement(void)
{
    if (TimingDelay != 0x00) {
        TimingDelay--;
    }
}
```

    TimingDelay的值等于延时函数中传进去的nTime的值，比如nTime=100000，则延时的时间等于100000*10us=1s。

- 主函数

```c
int main(void)
{
    /* LED 端口初始化 */
    LED_GPIO_Config();

    /* 配置SysTick 为10us中断一次,时间到后触发定时中断，
    *进入stm32fxx_it.c文件的SysTick_Handler处理，通过数中断次数计时
    */
    SysTick_Init();

    while (1) {

        LED_ON;
        Delay_us(100000);   // 10000 * 10us = 1000ms

        LED2_ON;
        Delay_us(100000);   // 10000 * 10us = 1000ms

        LED3_ON;
        Delay_us(100000);   // 10000 * 10us = 1000ms
    }
}
```

    主函数中初始化了LED和SysTick，然后在一个while循环中以1s的频率让LED闪烁。

## 4. 小结

    这一章主要就是学习SysTick系统计时器，实验通过简单延时实现LED闪烁

    更多内容可以参考：[超详细！必看！！STM32-系统滴答SysTick_stm32 systick-CSDN博客](https://blog.csdn.net/qq_48361010/article/details/134353735)

    [STM32入门：Systick（嘀嗒定时器）学习-CSDN博客](https://blog.csdn.net/qq_36553707/article/details/112095345#:~:text=%E4%B8%80%E3%80%81Systick%20%E4%BB%8B%E7%BB%8D%20Systick%20%E6%98%AF%20STM32%20%E7%9A%84%E4%B8%80%E4%B8%AA%E7%B3%BB%E7%BB%9F%E5%AE%9A%E6%97%B6%E5%99%A8%EF%BC%8C%E5%8F%88%E5%90%8D%E7%B3%BB%E7%BB%9F%E5%98%80%E5%97%92%E5%AE%9A%E6%97%B6%E5%99%A8%EF%BC%8C%E6%98%AF%E4%B8%80%E4%B8%AA%2024%20%E4%BD%8D%E7%9A%84%E5%80%92%E8%AE%A1%E6%95%B0%E5%AE%9A%E6%97%B6%E5%99%A8%EF%BC%8C%E5%BD%93%E8%AE%A1%E6%95%B0%E5%88%B0,RELOAD%20%E5%AF%84%E5%AD%98%E5%99%A8%20%E4%B8%AD%E8%87%AA%E5%8A%A8%E9%87%8D%E8%A3%85%E8%BD%BD%E5%AE%9A%E6%97%B6%E5%88%9D%E5%80%BC%EF%BC%8C%E5%BC%80%E5%A7%8B%E6%96%B0%E4%B8%80%E8%BD%AE%E8%AE%A1%E6%95%B0%E3%80%82%20Systick%20%E7%9A%84%E4%BF%A1%E5%8F%B7%E6%9D%A5%E6%BA%90%E4%BA%8E%E7%B3%BB%E7%BB%9F%E6%97%B6%E9%92%9F%EF%BC%8C%E4%B8%8D%E5%88%86%E9%A2%91%E4%B8%BA%2072MHz%EF%BC%8C8%20%E5%88%86%E9%A2%91%E4%B8%BA%209MHz%EF%BC%8C%E4%BB%8E%E4%B8%8B%E5%9B%BE%E7%9A%84%E6%97%B6%E9%92%9F%E6%A0%91%E5%B0%B1%E5%8F%AF%E4%BB%A5%E7%9C%8B%E5%87%BA%E6%9D%A5%E3%80%82)


