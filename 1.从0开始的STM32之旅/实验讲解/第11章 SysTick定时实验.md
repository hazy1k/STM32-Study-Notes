# 第十一章 SysTick定时实验

## 1. 硬件设计

    SysTick属于单片机内部的外设，不需要额外的硬件电路，剩下的只需一个LED灯即可。

## 2. 软件设计

### 2.1 编程概要

1. 设置重装寄存器的值

2. 清除当前数值寄存器的值

3. 配置控制与状态寄存器

### 2.2 代码分析

#### 2.2.1 SysTick配置库函数

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
    SysTick->VAL = 0UL;

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

当然初次学习系统定时器配置，我们有必要将得详细一些：

在 STM32 微控制器中，`SysTick_Config()` 函数用于配置 SysTick 定时器。这是一个常用的函数来设置定时器以生成系统节拍中断（通常用于操作系统的节拍定时器或其他定时任务）：

1. **检查重装载值的有效性**：

```c
if ((ticks - 1UL) > SysTick_LOAD_RELOAD_Msk) {
    return (1UL);
}

```

- 这部分代码检查传入的 `ticks` 值是否在有效范围内。`SysTick_LOAD_RELOAD_Msk` 是一个掩码，定义了重装载寄存器 `LOAD` 的最大值。若 `ticks - 1` 超出这个范围，则返回 `1UL`，表示配置失败。
2. **设置重装载寄存器**：

```c
SysTick->LOAD = (uint32_t)(ticks - 1UL);
```

- 这行代码将 `ticks` 设定为 SysTick 的重装载值。定时器将计数到这个值，然后触发一个中断，并重新开始计数。
3. **设置中断优先级**：

```c
NVIC_SetPriority(SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);
```

- 设置 SysTick 中断的优先级为最低。`__NVIC_PRIO_BITS` 是优先级的位数， `(1UL << __NVIC_PRIO_BITS) - 1UL` 计算出优先级的最大值。
4. **初始化当前值寄存器**：

```c
SysTick->VAL = 0UL;
```

- 将当前值寄存器 `VAL` 清零，确保计数从 `LOAD` 的值开始。
5. **配置和使能 SysTick 定时器**：

```c
SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                SysTick_CTRL_TICKINT_Msk   |
                SysTick_CTRL_ENABLE_Msk;
```

- `SysTick_CTRL_CLKSOURCE_Msk`：选择定时器的时钟源，通常是系统时钟或 AHB 时钟。
- `SysTick_CTRL_TICKINT_Msk`：使能 SysTick 定时器中断。
- `SysTick_CTRL_ENABLE_Msk`：使能 SysTick 定时器。

#### 2.2.2 配置SysTick中断优先级

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

SPRH1-SPRH3是一个32位的寄存器，但是只能通过字节访问，每8个字段控制着一个内核外设的中断优先级的配置。在STM32F103中， 只有位7:4这高四位有效，低四位没有用到，所以内核外设的中断优先级可编程为：0~15，只有16个可编程优先级，数值越小，优先级越高。 如果软件优先级配置相同，那就根据他们在中断向量表里面的位置编号来决定优先级大小，编号越小，优先级越高。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SysTic06.png)

如果要修改内核外设的优先级，只需要修改下面三个寄存器对应的某个字段即可。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SysTic002.png)

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SysTic003.jpeg)

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/SysTic004.png)

在系统定时器中，配置优先级为(1UL << __NVIC_PRIO_BITS) - 1UL)， 其中宏__NVIC_PRIO_BITS为4，那计算结果就等于15， 可以看出系统定时器此时设置的优先级在内核外设中是最低的，如果要修改优先级则修改这个值即可，范围为：0~15。

```c
// 设置系统定时器中断优先级
NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);
```

但是，问题来了，刚刚我们只是学习了内核的外设的优先级配置。如果我同时使用了systick和片上外设呢？而且片上外设也刚好需要使用中断， 那systick的中断优先级跟外设的中断优先级怎么设置？会不会因为systick是内核里面的外设，所以它的中断优先级就一定比内核之外的外设的优先级高？

我们知道，外设在设置中断优先级的时候，首先要分组，然后设置抢占优先级和子优先级。 而systick这类内核的外设在配置的时候，只需要配置一个寄存器即可，取值范围为0~15。既然配置方法不同，那如何区分两者的优先级？下面举例说明。

比如配置一个外设的中断优先级分组为2，抢占优先级为1，子优先级也为1，systick的优先级为固件库默认配置的15。 当我们比较内核外设和片上外设的中断优先级的时候，我们只需要抓住NVIC的中断优先级分组不仅对片上外设有效，同样对内核的外设也有效。 我们把systick的优先级15转换成二进制值就是1111(0b)，又因为NVIC的优先级分组2，那么前两位的11(0b)就是3，后两位的11(0b)也是3。 无论从抢占还是子优先级都比我们设定的外设的优先级低。如果当两个的软件优先级都配置成一样，那么就比较他们在中断向量表中的硬件编号，编号越小，优先级越高。

#### 2.2.3 SysTick初始化函数

```c
void SysTick_Init(void)
{
    /* SystemFrequency / 1000    1ms中断一次
    * SystemFrequency / 100000  10us中断一次
    * SystemFrequency / 1000000 1us中断一次
    */
    if (SysTick_Config(SystemCoreClock / 100000)) 
    {
        while (1);
    }
}
```

SysTick初始化函数由用户编写，里面调用了SysTick_Config()这个固件库函数， 通过设置该固件库函数的形参，就决定了系统定时器经过多少时间就产生一次中断。

#### 2.2.4 SysTick中断时间的计算

SysTick定时器的计数器是向下递减计数的，计数一次的时间T<sub>DEC=1/CLK<sub>AHB， 当重装载寄存器中的值VALUE<sub>LOAD减到0的时候，产生中断， 可知中断一次的时间TINT=VALUE<sub>LOAD * T<sub>DEC= VALUE<sub>LOAD/CLK<sub>AHB， 其中CLK<sub>AHB =72MHZ。如果设置VALUE<sub>LOAD为72， 那中断一次的时间T<sub>INT=72/72M=1us。 不过1us的中断没啥意义，整个程序的重心都花在进出中断上了，根本没有时间处理其他的任务。

```c
SysTick_Config(SystemCoreClock / 100000)
```

SysTick_Config（）的形我们配置为SystemCoreClock / 100000=72M/100000=720， 从刚刚分析我们知道这个形参的值最终是写到重装载寄存器LOAD中的， 从而可知我们现在把SysTick定时器中断一次的时间T<sub>INT=720/72M=10us。

#### 2.2.5 SysTick定时时间的计算

当设置好中断时间T<sub>INT后，我们可以设置一个变量t，用来记录进入中断的次数， 那么变量t乘以中断的时间T<sub>INT就可以计算出需要定时的时间。

#### 2.2.6 SysTick定时函数

现在我们定义一个微秒级别的延时函数，形参为nTime，当用这个形参乘以中断时间T<sub>INT就得出我们需要的延时时间， 其中T<sub>INT我们已经设置好为10us。关于这个函数的具体调用看注释即可。

```c
void Delay_us(__IO u32 nTime)
{
    TimingDelay = nTime;

    while (TimingDelay != 0);
}
```

函数Delay_us()中我们等待TimingDelay为0，当TimingDelay为0的时候表示延时时间到。变量TimingDelay在中断函数中递减， 即SysTick每进一次中断即10us的时间TimingDelay递减一次。

#### 2.2.7 SysTick中断服务函数

```c
void SysTick_Handler(void)
{
    TimingDelay_Decrement();
}
```

中断复位函数调用了另外一个函数TimingDelay_Decrement()，原型如下：

```c
// 获取节拍程序
void TimingDelay_Decrement(void)
{
    if (TimingDelay != 0x00) // 如果计数器不为0
    { 
        TimingDelay--; // 计数器减1
    }
}
```

TimingDelay的值等于延时函数中传进去的nTime的值，比如nTime=100000，则延时的时间等于100000*10us=1s。

#### 2.2.8 主函数

```c
int main(void)
{
    LED_GPIO_Config();
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

函数中初始化了LED和SysTick，然后在一个while循环中以1s的频率让LED闪烁

## 3. 小结

    这一章还是比较简单的，首先我们应该知道为什么要使用SysTick定时器-MCU的定时器资源有限，这样可以节省MCU资源，不用浪费一个定时器，点滴计时器就好像单片机的心脏一样，会根据固定的时间频率跳动。

    稍微总结一下吧：SysTick就是一个定时器，不过只能向下计时，比如你给他1个初值100s，那么100s后它就会产生一次中断，主要是学会SysTick_Config()这个库函数，我们可以举个例子：

```c
#include "stm32f4xx.h"  // 根据你的 STM32 系列选择适当的头文件

volatile uint32_t delay;

void TimingDelay_Decrement(void) {
    if (delay != 0) {
        delay--;
    }
}

int main(void) {
    // 假设系统时钟为 72 MHz
    #define SYSTEM_CLOCK_FREQ 72000000
    // 设置 SysTick 以每 1 毫秒产生一次中断
    #define TICK_FREQ 1000
    uint32_t ticks = SYSTEM_CLOCK_FREQ / TICK_FREQ;

    // 配置 SysTick
    if (SysTick_Config(ticks)) {
        // 初始化失败，可能是 ticks 值超出范围
        while (1);
    }

    // 初始化延时计数器
    delay = 500;

    while (1) {
        if (delay == 0) {
            // 执行任务
            delay = 500; // 重置延时计数器
        }
    }
}

void SysTick_Handler(void) {
    TimingDelay_Decrement();
}

```

- **定义时钟频率和节拍频率**： `SYSTEM_CLOCK_FREQ` 定义了系统时钟频率（72 MHz），`TICK_FREQ` 定义了每秒钟的节拍数（1000，即每毫秒一次）。
- **计算 `ticks` 值**： `ticks` 是 SysTick 计数器的重载值，通过将系统时钟频率除以节拍频率计算得出。
- **配置 SysTick 定时器**： `SysTick_Config(ticks)` 配置 SysTick 定时器，如果配置失败，程序会进入死循环。
- **设置初始延时**： `delay` 初始化为 500，用于表示 500 毫秒的延时。
- **主循环**： 主循环检查 `delay` 是否为零，如果是，则执行任务并重置 `delay`。

---

2024.8.22 第一次修订，后期不再维护
