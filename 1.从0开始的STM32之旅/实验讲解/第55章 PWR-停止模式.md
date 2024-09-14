# 第五十五章 PWR-停止模式

## 1. 硬件设计

本实验中的硬件与睡眠模式中的一致，主要使用到了按键、LED彩灯以及使用串口输出调试信息。

## 2. 软件设计

### 2.1 编程目标

- 初始化用于唤醒的中断按键；

- 选择电压调节器的工作模式并进入停止状态；

- 使用按键中断唤醒芯片；

- 重启HSE时钟，使系统完全恢复停止前的状态。

### 2.2 代码分析

- 重启HSE时钟

与睡眠模式不一样，系统从停止模式被唤醒时，是使用HSI作为系统时钟的，在STM32F103中，HSI时钟一般为8MHz，与我们常用的72MHz相关太远， 它会影响各种外设的工作频率。所以在系统从停止模式唤醒后，若希望各种外设恢复正常的工作状态，就要恢复停止模式前使用的系统时钟， 本实验中定义了一个SYSCLKConfig_STOP函数，用于恢复系统时钟

```c
// 停机唤醒后配置系统时钟: 使能 HSE, PLL
// 并且选择PLL作为系统时钟.
static void SYSCLKConfig_STOP(void)
{
  // 使能 HSE
  RCC_HSEConfig(RCC_HSE_ON);
  // 等待 HSE 准备就绪
  while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)
  {

  }
  // 使能 PLL 
  RCC_PLLCmd(ENABLE);
  // 等待 PLL 准备就绪
  while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
  {

  }
  // 选择PLL作为系统时钟源
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  // 等待PLL被选择为系统时钟源 
  while (RCC_GetSYSCLKSource() != 0x08)
  {

  }
}
```

- 主函数

停止模式实验的main函数流程与睡眠模式的类似，主要是调用指令方式的不同及唤醒后增加了恢复时钟的操作

```c
int main(void)
{	

	RCC_ClocksTypeDef clock_status_wakeup,clock_status_config;
	uint8_t clock_source_wakeup,clock_source_config; 
	
	LED_GPIO_Config();	
    USART_Config();		
	
	// 初始化按键中断，按下按键后会进入中断服务程序
	EXTI_Key_Config(); 
	printf("\r\n 实验说明：\r\n");
	printf("\r\n 1.本程序中，绿灯表示STM32正常运行，红灯表示停止状态，蓝灯表示刚从停止状态被唤醒\r\n");
	printf("\r\n 2.在停止状态下，可使用KEY1或KEY2唤醒\r\n");
	printf("\r\n 3.本实验执行这样一个循环：\r\n ------》亮绿灯(正常运行)->亮红灯(停止模式)->按KEY1或KEY2唤醒->亮蓝灯(刚被唤醒)-----》\r\n");
	printf("\r\n 4.在停止状态下，DAP下载器无法给STM32下载程序，\r\n 可按KEY1、KEY2唤醒后下载，\r\n 或按复位键使芯片处于复位状态，然后在电脑上点击下载按钮，再释放复位按键，即可下载\r\n");

  while(1)
  {	
	printf("\r\n STM32正常运行，亮绿灯\r\n");
	LED_GREEN;	
	Delay(0x3FFFFF);
		
	// 任务执行完毕，进入停止降低功耗
	printf("\r\n 进入停止模式，按KEY1或KEY2按键可唤醒\r\n");
	//使用红灯指示，进入停止状态
	LED_RED;
		
	// 进入停止模式，设置电压调节器为低功耗模式，等待中断唤醒 
	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);	
		
	// 等待中断唤醒  K1或K2按键中断	
	// 获取刚被唤醒时的时钟状态	
	// 时钟源
	clock_source_wakeup = RCC_GetSYSCLKSource ();
	// 时钟频率
	RCC_GetClocksFreq(&clock_status_wakeup);	
	//从停止模式下被唤醒后使用的是HSI时钟，此处重启HSE时钟,使用PLLCLK
	SYSCLKConfig_STOP();
		
	// 获取重新配置后的时钟状态	
	// 时钟源
	clock_source_config = RCC_GetSYSCLKSource ();
	// 时钟频率
	RCC_GetClocksFreq(&clock_status_config);
	
	// 因为刚唤醒的时候使用的是HSI时钟，会影响串口波特率，输出不对，所以在重新配置时钟源后才使用串口输出。
	printf("\r\n重新配置后的时钟状态：\r\n");
	printf(" SYSCLK频率:%d,\r\n HCLK频率:%d,\r\n PCLK1频率:%d,\r\n PCLK2频率:%d,\r\n 时钟源:%d (0表示HSI，8表示PLLCLK)\n", 
			clock_status_config.SYSCLK_Frequency, 
			clock_status_config.HCLK_Frequency, 
			clock_status_config.PCLK1_Frequency, 
			clock_status_config.PCLK2_Frequency, 
			clock_source_config);
			
	printf("\r\n刚唤醒的时钟状态：\r\n");	
	printf(" SYSCLK频率:%d,\r\n HCLK频率:%d,\r\n PCLK1频率:%d,\r\n PCLK2频率:%d,\r\n 时钟源:%d (0表示HSI，8表示PLLCLK)\n", 
			clock_status_wakeup.SYSCLK_Frequency, 
			clock_status_wakeup.HCLK_Frequency, 
			clock_status_wakeup.PCLK1_Frequency, 
			clock_status_wakeup.PCLK2_Frequency, 
			clock_source_wakeup);
		
	//指示灯
	LED_BLUE;	
	Delay(0x1FFFFF);		
	printf("\r\n 已退出停止模式\r\n");
  }
}
```

<img src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/Power007.png" title="" alt="" width="546">

(1) 程序中首先初始化了LED灯及串口以便用于指示芯片的运行状态，并且把实验板上的两个按键都初始化成了中断模式， 以便当系统进入停止模式的时候可以通过按键来唤醒。这些硬件的初始化过程都跟前面章节中的一模一样。

(2) 初始化完成后使用LED及串口表示运行状态，在本实验中，LED彩灯为绿色时表示正常运行，红灯时表示停止状态， 蓝灯时表示刚从停止状态中被唤醒。在停止模式下，I/O口会保持停止前的状态，所以LED彩灯在停止模式时也会保持亮红灯。

(3) 程序执行一段时间后，调用库函数PWR_EnterSTOPMode把调压器设置在低功耗模式，并使用WFI指令进入停止状态。 由于WFI停止模式可以使用任意EXTI的中断唤醒，所以我们可以使用按键中断唤醒。

(4) 当系统进入睡眠状态后，我们按下实验板上的KEY1或KEY2按键，即可唤醒系统，当执行完中断服务函数后， 会继续执行WFI指令(即PWR_EnterSTOPMode函数)后的代码。

(5) 为了更清晰地展示停止模式的影响，在刚唤醒后，我们调用了库函数RCC_GetSYSCLKSource以及RCC_GetClocksFreq获取刚唤醒后的系统的时钟源以及时钟频率， 在使用SYSCLKConfig_STOP恢复时钟后，我们再次获取这些时状态，最后再通过串口打印出来。

(6) 通过串口调试信息我们会知道刚唤醒时系统时钟使用的是HSI时钟，频率为8MHz ，恢复后的系统时钟采用HSE倍频后的PLL时钟，时钟频率为72MHz。


