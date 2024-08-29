# 第二十六章 RTC-实时时钟

## 1. RTC实时时钟简介

STM32的RTC外设（Real Time Clock），实质是一个掉电后还继续运行的定时器。从定时器的角度来说，相对于通用定时器TIM外设，它十分简单， 只有很纯粹的计时和触发中断的功能；但从掉电还继续运行的角度来说，它却是STM32中唯一一个具有如此强大功能的外设。 所以RTC外设的复杂之处并不在于它的定时功能，而在于它掉电还继续运行的特性。

以上所说的掉电，是指主电源V<sub>DD断开的情况，为了RTC外设掉电继续运行，必须接上锂电池给STM32的RTC、 备份发卡通过V<sub>BAT引脚供电。当主电源V<sub>DD有效时，由V<sub>DD给RTC外设供电； 而当V<sub>DD掉电后，由V<sub>BAT给RTC外设供电。但无论由什么电源供电，RTC中的数据都保存在属于RTC的备份域中， 若主电源V<sub>DD和V<sub>BAT都掉电，那么备份域中保存的所有数据将丢失。备份域除了RTC模块的寄存器， 还有42个16位的寄存器可以在V<sub>DD掉电的情况下保存用户程序的数据，系统复位或电源复位时，这些数据也不会被复位。

从RTC的定时器特性来说，它是一个32位的计数器，只能向上计数。它使用的时钟源有三种，分别为高速外部时钟的128分频（HSE/128）、 低速内部时钟LSI以及低速外部时钟LSE；使HSE分频时钟或LSI的话，在主电源V<sub>DD掉电的情况下，这两个时钟来源都会受到影响， 因此没法保证RTC正常工作。因此RTC一般使用低速外部时钟LSE，在设计中，频率通常为实时时钟模块中常用的32.768KHz， 这是因为32768 = 2的15次方，分频容易实现，所以它被广泛应用到RTC模块。在主电源VDD有效的情况下(待机)， RTC还可以配置闹钟事件使STM32退出待机模式。

## 2. RTC外设框图剖析

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/RTC002.png)

框图中浅灰色的部分都是属于备份域的，在V<sub>DD掉电时可在V<sub>BAT的驱动下继续运行。 这部分仅包括RTC的分频器，计数器，和闹钟控制器。若V<sub>DD电源有效，RTC可以触发RTC_Second(秒中断)、 RTC_Overflow(溢出事件)和RTC_Alarm(闹钟中断)。从结构图可以分析到，其中的定时器溢出事件无法被配置为中断。 若STM32原本处于待机状态，可由闹钟事件或WKUP事件(外部唤醒事件，属于EXTI模块，不属于RTC)使它退出待机模式。 闹钟事件是在计数器RTC_CNT的值等于闹钟寄存器RTC_ALR的值时触发的。

在备份域中所有寄存器都是16位的， RTC控制相关的寄存器也不例外。它的计数器RTC_CNT的32位由RTC_CNTL和RTC_CNTH两个寄存器组成，分别保存定时计数值的低16位和高16位。 在配置RTC模块的时钟时，通常把输入的32768Hz的RTCCLK进行32768分频得到实际驱动计数器的时钟 TR_CLK =RTCCLK/32768= 1 Hz， 计时周期为1秒，计时器在TR_CLK的驱动下计数，即每秒计数器RTC_CNT的值加1。

由于备份域的存在，使得RTC核具有了完全独立于APB1接口的特性， 也因此对RTC寄存器的访问要遵守一定的规则。

系统复位后，默认禁止访问后备寄存器和RTC，防止对后备区域(BKP)的意外写操作。 执行以下操作使能对后备寄存器和RTC的访问：

(1) 设置RCC_APB1ENR寄存器的PWREN和BKPEN位来使能电源和后备接口时钟。

(2) 设置PWR_CR寄存器的DBP位使能对后备寄存器和RTC的访问。

设置后备寄存器为可访问后，在第一次通过APB1接口访问RTC时，因为时钟频率的差异，所以必须等待APB1与RTC外设同步， 确保被读取出来的RTC寄存器值是正确的。若在同步之后，一直没有关闭APB1的RTC外设接口，就不需要再次同步了。

如果内核要对RTC寄存器进行任何的写操作，在内核发出写指令后，RTC模块在3个RTCCLK时钟之后，才开始正式的写RTC寄存器操作。 由于RTCCLK的频率比内核主频低得多，所以每次操作后必须要检查RTC关闭操作标志位RTOFF，当这个标志被置1时，写操作才正式完成。

## 3. UNIX时间戳

在使用RTC外设前，还需要引入UNIX时间戳的概念。

如果从现在起，把计数器RTC_CNT的计数值置0，然后每秒加1， RTC_CNT什么时候会溢出呢？由于RTC_CNT是32位寄存器， 可存储的最大值为(2^32-1)，即这样计时的话，在232秒后溢出，即它将在今后的136年时溢出：

N = 2^32/365/24/60/60 ≈136年

假如某个时刻读取到计数器的数值为X = 60*60*24*2，即两天时间的秒数，而假设又知道计数器是在2011年1月1日的0时0分0秒置0的， 那么就可以根据计数器的这个相对时间数值，计算得这个X时刻是2011年1月3日的0时0分0秒了。而计数器则会在(2011+136)年左右溢出， 也就是说到了（2011+136）年时，如果我们还在使用这个计数器提供时间的话就会出现问题。在这个例子中，定时器被置0的这个时间被称为计时元年， 相对计时元年经过的秒数称为时间戳，也就是计数器中的值。

大多数操作系统都是利用时间戳和计时元年来计算当前时间的，而这个时间戳和计时元年大家都取了同一个标准——UNIX时间戳和UNIX计时元年。 UNIX计时元年被设置为格林威治时间1970年1月1日0时0分0秒，大概是为了纪念UNIX的诞生的时代吧， 而UNIX时间戳即为当前时间相对于UNIX计时元年经过的秒数。因为unix时间戳主要用来表示当前时间或者和电脑有关的日志时间（如文件创立时间，log发生时间等）， 考虑到所有电脑文件不可能在1970年前创立，所以用unix时间戳很少用来表示1970前的时间。

在这个计时系统中，使用的是有符号的32位整型变量来保存UNIX时间戳的，即实际可用计数位数比我们上面例子中的少了一位， 少了这一位，UNIX计时元年也相对提前了，这个计时方法在2038年1月19日03时14分07秒将会发生溢出，这个时间离我们并不远。 由于UNIX时间戳被广泛应用到各种系统中，溢出可能会导致系统发生严重错误，届时，很可能会重演一次“千年虫”的问题，所以在设计预期寿命较长的设备需要注意。

## 4. 与RTC控制相关的库函数

### 4.1 等待时钟同步和操作完成

RTC区域的时钟比APB时钟慢，访问前需要进行时钟同步，只要调用库函数RTC_WaitForSynchro即可，而如果修改了RTC的寄存器， 又需要调用RTC_WaitForLastTask函数确保数据已写入

```c
/**
* @brief  等待RTC寄存器与APB时钟同步 (RTC_CNT, RTC_ALR and RTC_PRL)
* @note   在APB时钟复位或停止后，在对RTC寄存器的任何操作前，必须调用本函数
* @param  None
* @retval None
*/
void RTC_WaitForSynchro(void)
{
    /* 清除 RSF 寄存器位 */
    RTC->CRL &= (uint16_t)~RTC_FLAG_RSF;
    /* 等待至 RSF 寄存器位为SET */
    while ((RTC->CRL & RTC_FLAG_RSF) == (uint16_t)RESET) {
    }
}

/**
* @brief  等待上一次对 RTC寄存器的操作完成
* @note   修改RTC寄存器后，必须调用本函数
* @param  None
* @retval None
*/
void RTC_WaitForLastTask(void)
{
    /* 等待至 RTOFF 寄存器位为SET*/
    while ((RTC->CRL & RTC_FLAG_RTOFF) == (uint16_t)RESET) {
    }
}
```

这两个库函数主要通过while循环检测RTC控制寄存器的RSF和RTOFF位实现等待功能。

### 4.2 使能备份域及RTC配置

默认情况下，RTC所属的备份域禁止访问，可使用库函数PWR_BackupAccessCmd使能访问

```c
/**
* @brief  使能对 RTC 和 backup 寄存器的访问.
* @param   ENABLE 或 DISABLE.
* @retval None
*/
void PWR_BackupAccessCmd(FunctionalState NewState)
{
    *(__IO uint32_t *) CR_DBP_BB = (uint32_t)NewState;
}
```

该函数通过PWR_CR寄存器的DBP位使能访问，使能后才可以访问RTC相关的寄存器，然而若希望修改RTC的寄存器， 还需要进一步使能RTC控制寄存器的CNF位使能寄存器配置

```c
/**
* @brief  进入 RTC 配置模式 .
* @param  None
* @retval None
*/
void RTC_EnterConfigMode(void)
{
    /* 设置 CNF 位进入配置模式 */
    RTC->CRL |= RTC_CRL_CNF;
}

/**
* @brief  退出 RTC 配置模式 .
* @param  None
* @retval None
*/
void RTC_ExitConfigMode(void)
{
    /* 清空  CNF 位退出配置模式 */
    RTC->CRL &= (uint16_t)~((uint16_t)RTC_CRL_CNF);
}
```

这两个库函数分别提供了进入和退出RTC寄存器的配置模式，一般情况下它们由库函数调用。

### 4.3 设置RTC时钟分频

使用RCC相关的库函数选择RTC使用的时钟后，可以使用库RTC_SetPrescaler进行分频， 一般会把RTC时钟分频得到1Hz的时钟

```c
/**
* @brief  设置RTC分频配置
* @param  PrescalerValue: RTC 分频值.
* @retval None
*/
void RTC_SetPrescaler(uint32_t PrescalerValue)
{
    RTC_EnterConfigMode();
    /* 设置 RTC 分频值的 MSB  */
    RTC->PRLH = (PrescalerValue & PRLH_MSB_MASK) >> 16;
    /* 设置 RTC 分频值的 LSB  */
    RTC->PRLL = (PrescalerValue & RTC_LSB_MASK);
    RTC_ExitConfigMode();
}
```

在函数中，使用RTC_EnterConfigMode和RTC_ExitConfigMode进入和退出RTC寄存器配置模式， 配置时把函数参数PrescalerValue写入到RTC的PRLH和PRLL寄存器中。

### 4.4 设置、获取RTC计数器及闹钟

RTC外设中最重要的就是计数器以及闹钟寄存器了，它们可以使用RTC_SetCounter、 RTC_GetCounter以及RTC_SetAlarm库函数操作

```c
/**
* @brief  设置 RTC 计数器的值 .
* @param  CounterValue: 要设置的RTC计数器值.
* @retval None
*/
void RTC_SetCounter(uint32_t CounterValue)
{
    RTC_EnterConfigMode();
    /* 设置 RTC 计数器的 MSB  */
    RTC->CNTH = CounterValue >> 16;
    /* 设置 RTC 计数器的 LSB  */
    RTC->CNTL = (CounterValue & RTC_LSB_MASK);
    RTC_ExitConfigMode();
}

/**
* @brief  获取 RTC 计数器的值 .
* @param  None
* @retval 返回RTC计数器的值
*/
uint32_t RTC_GetCounter(void)
{
    uint16_t tmp = 0;
    tmp = RTC->CNTL;
    return (((uint32_t)RTC->CNTH << 16 ) | tmp) ;
}

/**
* @brief  设置 RTC 闹钟的值 .
* @param  AlarmValue: 要设置的RTC闹钟值.
* @retval None
*/
void RTC_SetAlarm(uint32_t AlarmValue)
{
    RTC_EnterConfigMode();
    /* 设置 RTC 闹钟的 MSB  */
    RTC->ALRH = AlarmValue >> 16;
    /* 设置 RTC 闹钟的 LSB  */
    RTC->ALRL = (AlarmValue & RTC_LSB_MASK);
    RTC_ExitConfigMode();
}
```

利用RTC_SetCounter可以向RTC的计数器写入新数值，通常这些数值被设置为时间戳以更新时间。

RTC_GetCounter函数则用于在RTC正常运行时获取当前计数器的值以获取当前时间。

RTC_SetAlarm函数用于配置闹钟时间，当计数器的值与闹钟寄存器的值相等时， 可产生闹钟事件或中断，该事件可以把睡眠、停止和待机模式的STM32芯片唤醒。
