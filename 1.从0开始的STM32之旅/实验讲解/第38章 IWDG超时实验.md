# 第三十八章 IWDG超时实验

## 1. 硬件设计

1. IWDG一个

2. 按键一个

3. LED三个

IWDG属于单片机内部资源，不需要外部电路，需要一个外部的按键和LED，在 1s 的时间内通过按键来不断的喂狗，如果喂狗失败，红灯闪烁。如果一直喂狗成功，则绿灯常亮。

## 2. 软件设计

- IWDG配置函数

```c
void IWDG_Config(uint8_t prv ,uint16_t rlv)
{
    // 使能 预分频寄存器PR和重装载寄存器RLR可写
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    // 设置预分频器值
    IWDG_SetPrescaler( prv );

    // 设置重装载寄存器值
    IWDG_SetReload( rlv );

    // 把重装载寄存器的值放到计数器中
    IWDG_ReloadCounter();

    // 使能 IWDG
    IWDG_Enable();
}
```

IWDG配置函数有两个形参，prv用来设置预分频的值，取值可以是：

```c
/*
*     @arg IWDG_Prescaler_4:    IWDG prescaler set to 4
*     @arg IWDG_Prescaler_8:    IWDG prescaler set to 8
*     @arg IWDG_Prescaler_16:   IWDG prescaler set to 16
*     @arg IWDG_Prescaler_32:   IWDG prescaler set to 32
*     @arg IWDG_Prescaler_64:   IWDG prescaler set to 64
*     @arg IWDG_Prescaler_128:  IWDG prescaler set to 128
*     @arg IWDG_Prescaler_256:  IWDG prescaler set to 256
*/
```

这些宏在stm32f10x_iwdg.h中定义，宏展开是8位的16进制数，具体作用是配置配置预分频寄存器IWDG_PR，获得各种分频系数。 形参rlv用来设置重装载寄存器IWDG_RLR的值，取值范围为0~0XFFF。溢出时间Tout = prv/40 * rlv(s)， prv可以是[4,8,16,32,64,128,256]。如果我们需要设置1s的超时溢出，prv可以取IWDG_Prescaler_64，rlv取625， 即调用:IWDG_Config(IWDG_Prescaler_64 ,625)。Tout=64/40*625=1s。

- 喂狗函数

```c
void IWDG_Feed(void)
{
    // 把重装载寄存器的值放到计数器中，喂狗，防止IWDG复位
    // 当计数器的值减到0的时候会产生系统复位
    IWDG_ReloadCounter();
}
```

- 主函数

```c
int main(void)
{	
  // 配置LED GPIO，并关闭LED
  LED_GPIO_Config();	
  Delay(0X8FFFFF);
  // 检查是否为独立看门狗复位
  if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
  {
    // 独立看门狗复位
    //  亮红灯 
    LED_RED;

    // 清除标志
    RCC_ClearFlag();
		
		/*如果一直不喂狗，会一直复位，加上前面的延时，会看到红灯闪烁
		在1s 时间内喂狗的话，则会持续亮绿灯*/
  }
  else
  {
    // 不是独立看门狗复位(可能为上电复位或者手动按键复位之类的) 
    // 亮蓝灯 
    LED_BLUE;
  }
	// 配置按键GPIO
	Key_GPIO_Config();
	// IWDG 1s 超时溢出
	IWDG_Config(IWDG_Prescaler_64, 625);
	/*
	while部分是我们在项目中具体需要写的代码，这部分的程序可以用独立看门狗来监控,如果我们知道这部分代码的执行时间，比如是500ms，那么我们可以设置独立看门狗的
	溢出时间是600ms，比500ms多一点，如果要被监控的程序没有跑飞正常执行的话，那么执行完毕之后就会执行喂狗的程序，如果程序跑飞了那程序就会超时，到达不了喂狗
	的程序,此时就会产生系统复位。但是也不排除程序跑飞了又跑回来了，刚好喂狗了，歪打正着。所以要想更精确的监控程序，可以使用窗口看门狗，窗口看门狗规定必须在
	规定的窗口时间内喂狗。
	*/
	while(1)                        
	{	
	// 这里添加需要被监控的代码，如果有就去掉按键模拟喂狗，把按键扫描程序去掉
		if(Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PING) == KEY_ON  )
		{
			// 喂狗，如果不喂狗，系统则会复位，复位后亮红灯，如果在1s
			// 时间内准时喂狗的话，则会亮绿灯
			IWDG_Feed(); // 喂狗
			//喂狗后亮绿灯
			LED_GREEN;
		}   
	}
}
```

主函数中我们初始化好LED和按键相关的配置，设置IWDG 1s超时溢出之后，进入while死循环，通过按键来喂狗，如果喂狗成功， 则亮绿灯，如果喂狗失败的话，系统重启，程序重新执行，当执行到RCC_GetFlagStatus函数的时候，则会检测到是IWDG复位， 然后让红灯亮。如果喂狗一直失败的话，则会一直产生系统复位，加上前面延时的效果，则会看到红灯一直闪烁。

我们这里是通过按键来模拟一个喂狗程序，真正的项目中则不是这样使用。while部分是我们在项目中具体需要写的代码， 这部分的程序可以用独立看门狗来监控，如果我们知道这部分代码的执行时间，比如是500ms，那么我们可以设置独立看门狗的溢出时间是510ms， 比500ms多一点，如果要被监控的程序没有跑飞正常执行的话，那么执行完毕之后就会执行喂狗的程序，如果程序跑飞了那程序就会超时， 到达不了喂狗的程序，此时就会产生系统复位，但是也不排除程序跑飞了又跑回来了，刚好喂狗了，歪打正着。所以要想更精确的监控程序， 可以使用窗口看门狗，窗口看门狗规定必须在规定的窗口时间内喂狗，早了不行，晚了也不行。


