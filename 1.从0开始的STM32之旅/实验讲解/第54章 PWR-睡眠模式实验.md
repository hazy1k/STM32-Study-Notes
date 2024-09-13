# 第五十四章 PWR-睡眠模式实验

## 1. 硬件设计

实验中的硬件主要使用到了按键、LED彩灯以及使用串口输出调试信息， 这些硬件都与前面相应实验中的一致，涉及到硬件设计的可参考原理图或前面章节中的内容。

## 2. 软件设计

### 2.1 编程目标

1. 初始化用于唤醒的中断按键

2. 进入睡眠状态

3. 使用按键中断唤醒芯片

### 2.2 代码分析

- NVIC中断配置

```c
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  // 配置NVIC为优先级组1
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  // 配置中断源：按键1 
  NVIC_InitStructure.NVIC_IRQChannel = KEY1_INT_EXTI_IRQ;
  // 配置抢占优先级 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  // 配置子优先级 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  // 使能中断通道 
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  // 配置中断源：按键2，其他使用上面相关配置  
  NVIC_InitStructure.NVIC_IRQChannel = KEY2_INT_EXTI_IRQ;
  NVIC_Init(&NVIC_InitStructure);
}
```

- 中断服务函数

```c
void KEY1_IRQHandler(void)
{
  //确保是否产生了EXTI Line中断
	if(EXTI_GetITStatus(KEY1_INT_EXTI_LINE) != RESET) 
	{
		LED_BLUE;		
		printf("\r\n KEY1 按键中断唤醒 \r\n");    
		EXTI_ClearITPendingBit(KEY1_INT_EXTI_LINE);     
	}  
}

void KEY2_IRQHandler(void)
{
  //确保是否产生了EXTI Line中断
	if(EXTI_GetITStatus(KEY2_INT_EXTI_LINE) != RESET) 
	{
		LED_BLUE;
		printf("\r\n KEY2 按键中断唤醒 \r\n");   
    //清除中断标志位
		EXTI_ClearITPendingBit(KEY2_INT_EXTI_LINE);     
	}  
}
```

- EXTI配置

```c
// 配置 IO为EXTI中断口，并设置中断优先级
void EXTI_Key_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;

	// 开启按键GPIO口的时钟
	RCC_APB2PeriphClockCmd(KEY1_INT_GPIO_CLK,ENABLE);			
	// 配置 NVIC 中断
	NVIC_Configuration();
	
/*--------------------------KEY1配置-----------------------------*/
	// 选择按键用到的GPIO 	
  GPIO_InitStructure.GPIO_Pin = KEY1_INT_GPIO_PIN;
  // 配置为浮空输入	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(KEY1_INT_GPIO_PORT, &GPIO_InitStructure);

	// 选择EXTI的信号源 
  GPIO_EXTILineConfig(KEY1_INT_EXTI_PORTSOURCE, KEY1_INT_EXTI_PINSOURCE); 
  EXTI_InitStructure.EXTI_Line = KEY1_INT_EXTI_LINE;
	
	// EXTI为中断模式
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	// 上升沿中断 
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  // 使能中断 	
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
	
  /*--------------------------KEY2配置-----------------------------*/
	// 选择按键用到的GPIO 	
  GPIO_InitStructure.GPIO_Pin = KEY2_INT_GPIO_PIN;
  // 配置为浮空输入 	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(KEY2_INT_GPIO_PORT, &GPIO_InitStructure);

	// 选择EXTI的信号源 
  GPIO_EXTILineConfig(KEY2_INT_EXTI_PORTSOURCE, KEY2_INT_EXTI_PINSOURCE); 
  EXTI_InitStructure.EXTI_Line = KEY2_INT_EXTI_LINE;
	
	// EXTI为中断模式 
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	// 下降沿中断 
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  // 使能中断 	
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
}
```

- 主函数

```c
int main(void)
{ 
	// 初始化LED
	LED_GPIO_Config();
    // 初始化USART1
    USART_Config();		
	// 初始化按键为中断模式，按下中断后会进入中断服务函数
	EXTI_Key_Config(); 
	
  	printf("\r\n 野火STM32 睡眠模式例程\r\n");
	printf("\r\n 实验说明：\r\n");
	printf("\r\n 1.本程序中，绿灯表示STM32正常运行，红灯表示睡眠状态，蓝灯表示刚从睡眠状态被唤醒\r\n");
	printf("\r\n 2.程序运行一段时间后自动进入睡眠状态，在睡眠状态下，可使用KEY1或KEY2唤醒\r\n");
	printf("\r\n 3.本实验执行这样一个循环：\r\n ------》亮绿灯(正常运行)->亮红灯(睡眠模式)->按KEY1或KEY2唤醒->亮蓝灯(刚被唤醒)-----》\r\n");
	printf("\r\n 4.在睡眠状态下，DAP下载器无法给STM32下载程序，\r\n 可按KEY1、KEY2唤醒后下载，\r\n 或按复位键使芯片处于复位状态，然后在电脑上点击下载按钮，再释放复位按键，即可下载\r\n");

  	while(1)
  	{	
		printf("\r\n STM32正常运行，亮绿灯\r\n");
		LED_GREEN;	
		Delay(0x3FFFFF);
		// 任务执行完毕，进入睡眠降低功耗
		printf("\r\n 进入睡眠模式，按KEY1或KEY2按键可唤醒\r\n");
		// 使用红灯指示，进入睡眠状态
		LED_RED;
		//进入睡眠模式
		__WFI(); // WFI指令进入睡眠
		
		// 等待中断唤醒  K1或K2按键中断	
		// 被唤醒，亮蓝灯指示
		LED_BLUE;	
		Delay(0x1FFFFF);		
			
		printf("\r\n 已退出睡眠模式\r\n");
  }
}
```

这个main函数的执行流程见图：

<img src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/Power006.png" title="" alt="" width="501">

1. 程序中首先初始化了LED灯及串口以便用于指示芯片的运行状态， 并且把实验板上的两个按键都初始化成了中断模式，以便当系统进入睡眠模式的时候可以通过按键来唤醒。这些硬件的初始化过程都跟前面章节中的一模一样。

2. 初始化完成后使用LED及串口表示运行状态，在本实验中， LED彩灯为绿色时表示正常运行，红灯时表示睡眠状态，蓝灯时表示刚从睡眠状态中被唤醒。

3. 程序执行一段时间后，直接使用WFI指令进入睡眠模式， 由于WFI睡眠模式可以使用任意中断唤醒，所以我们可以使用按键中断唤醒。在实际应用中，也可以把它改成串口中断、定时器中断等。

4. 当系统进入停止状态后，我们按下实验板上的KEY1或KEY2按键， 即可使系统回到正常运行的状态，当执行完中断服务函数后，会继续执行WFI指令后的代码。


