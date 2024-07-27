# 第十一章 SysTick定时实验

## 1. 硬件设计

    SysTick属于单片机内部的外设，不需要额外的硬件电路，剩下的只需一个LED灯即可。

## 2. 软件设计

1. SysTick初始化函数

```c
// 点滴定时器初始化函数
void SysTick_Init(void)
{
    /* 
    SystemFrequency / 1000    1ms中断一次
    SystemFrequency / 100000     10us中断一次
    SystemFrequency / 1000000 1us中断一次
    */
//    if (SysTick_Config(SystemFrequency / 100000))    // ST3.0.0库版本
    if (SysTick_Config(SystemCoreClock / 100000))    // ST3.5.0库版本
    {  
        while (1);
    }
}
```

SysTick初始化函数由用户编写，里面调用了SysTick_Config()这个固件库函数， 通过设置该固件库函数的形参，就决定了系统定时器经过多少时间就产生一次中断。

2. SysTick定时函数

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

3. SysTick中断服务函数

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

4. 主函数

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

函数中初始化了LED和SysTick，然后在一个while循环中以1s的频率让LED闪烁

---

上面的实验，我们是使用了中断，而且经过多个函数的调用，还使用了全局变量，理解起来挺费劲的，其实还有另外一种更简洁的写法。 我们知道，systick的counter从reload值往下递减到0的时候，CTRL寄存器的位16:countflag会置1，且读取该位的值可清0， 所有我们可以使用软件查询的方法来实现延时。我敢肯定这样的写法， 初学者肯定会更喜欢，因为它直接，套路浅。

```c
void SysTick_Delay_Us( __IO uint32_t us) // us级延时
{
    uint32_t i;
    SysTick_Config(SystemCoreClock / 1000000);

    for(i = 0; i < us; i++)
    {
        // 当计数器的值减小到0的时候，CRTL寄存器的位16会置1    
        while( !((SysTick->CTRL)&(1<<16)) );
    }
    // 关闭SysTick定时器
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}
```

```c
void SysTick_Delay_Ms( __IO uint32_t ms) // ms级延时
{
    uint32_t i;    
    SysTick_Config(SystemCoreClock / 1000);

    for(i = 0; i < ms; i++)
    {
        // 当计数器的值减小到0的时候，CRTL寄存器的位16会置1
        // 当置1时，读取该位会清0
        while( !((SysTick->CTRL)&(1<<16)) );
    }

    // 关闭SysTick定时器
    SysTick->CTRL &=~ SysTick_CTRL_ENABLE_Msk;
}
```

## 3. 小结

    这一章还是比较简单的，首先我们应该知道为什么要使用SysTick定时器-MCU的定时器资源有限，这样可以节省MCU资源，不用浪费一个定时器，点滴计时器就好像单片机的心脏一样，会根据固定的时间频率跳动
