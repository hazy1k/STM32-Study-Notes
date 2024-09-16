# 第五十七章 PWR-PVD电源监控实验

## 1. 硬件设计

本实验中使用PVD监控STM32芯片的VDD引脚，当监测到供电电压低于阈值时会产生PVD中断，系统进入中断服务函数进入紧急处理过程。 所以进行这个实验时需要使用一个可调的电压源给实验板供电，改变给STM32芯片的供电电压，为此我们需要先了解实验板的电源供电系统：

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/Power005.jpeg)

整个电源供电系统主要分为以下五部分：

(1) 6-12V的DC电源供电系统，这部分使用DC电源接口引入6-12V的电源， 经过RT7272进行电压转换成5V电源，再与第二部分的“5V_USB”电源线连接在一起。

(2) 第二部分使用USB接口，使用USB线从外部引入5V电源， 引入的电源经过电源开关及保险丝连接到“5V”电源线。

(3) 第三部分的是电源开关及保险丝，即当我们的实验板使用DC电源或“5V_USB”线供电时， 可用电源开关控制通断，保险丝也会起保护作用。

(4) “5V”电源线遍布整个板子，板子上各个位置引出的标有“5V”丝印的排针都与这个电源线直接相连。 5V电源线给板子上的某些工作电压为5V的芯片供电。5V电源还经过LDO稳压芯片，输出3.3V电源连接到“3.3V”电源线。

(5) 同样地，“3.3V”电源线也遍布整个板子，各个引出的标有“3.3V”丝印的排针都与它直接相连， 3.3V电源给工作电压为3.3V的各种芯片供电。STM32芯片的VDD引脚就是直接与这个3.3V电源相连的，所以通过STM32的PVD监控的就是这个“3.3V”电源线的电压。

当我们进行这个PVD实验时，为方便改变“3.3V”电源线的电压，我们可以把可调电源通过实验板上引出的“5V”及“GND” 排针给实验板供电， 当可调电源电压降低时，LDO在“3.3V”电源线的供电电压会随之降低，即STM32的PVD监控的VDD引脚电压会降低， 这样我们就可以模拟VDD电压下降的实验条件，对PVD进行测试了。不过，由于这样供电不经过保险丝， 所以在调节电压的时候要小心，不要给它供电远高于5V，否则可能会烧坏实验板上的芯片。

## 2. 软件设计

### 2.1 编程目标

- 初始化PVD中断；

- 设置PVD电压监控等级并使能PVD；

- 编写PVD中断服务函数，处理紧急任务。

### 2.2 代码分析

- 初始化PVD

```c
// 配置PCD
void PVD_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;

  // 使能 PWR 时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  // 使能 PVD 中断
  NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
      
  // 配置 EXTI16线(PVD 输出) 来产生上升下降沿中断
  EXTI_ClearITPendingBit(EXTI_Line16); // 清除中断标志位
  EXTI_InitStructure.EXTI_Line = EXTI_Line16; // 选择 EXTI 线
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; // 中断模式
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; // 上升沿和下降沿触发中断
  EXTI_InitStructure.EXTI_LineCmd = ENABLE; // 使能中断线
  EXTI_Init(&EXTI_InitStructure);

  // 配置PVD级别PWR_PVDLevel_2V6 (PVD检测电压的阈值为2.6V，VDD电压低于2.6V时产生PVD中断)
	// 具体级别根据自己的实际应用要求配置
  PWR_PVDLevelConfig(PWR_PVDLevel_2V6); // 配置PVD检测电压阈值

  // 使能PVD输出
  PWR_PVDCmd(ENABLE);
}
```

在这段代码中，执行的流程如下：

1. 配置PVD的中断优先级。 由于电压下降是非常危急的状态，所以请尽量把它配置成最高优先级。

2. 配置了EXTI16线的中断源， 设置EXTI16是因为PVD中断是通过EXTI16产生中断的(GPIO的中断是EXTI0-EXTI15)。

3. 使用库函数PWR_PVDLevelConfig设置PVD监控的电压阈值等级， 各个阈值等级表示的电压值请查阅表 [PVD的阈值等级](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/Power.html#id4) 或STM32的数据手册。

4. 最后使用库函数PWR_PVDCmd使能PVD功能。
- PVD中断服务函数

```c
void PVD_IRQHandler(void)
{
		// 检测是否产生了PVD警告信号
		if(PWR_GetFlagStatus (PWR_FLAG_PVDO)==SET)			
		{
			// 亮红灯，实际应用中应进入紧急状态处理
			LED_RED; 	
	  }
    // 清除中断信号
    EXTI_ClearITPendingBit(EXTI_Line16);
}
```

- 主函数

```c

```

本电源监控实验的main函数执行流程比较简单，仅调用了PVD_Config配置监控功能，当VDD供电电压正常时，板子亮绿灯， 当电压低于阈值时，会跳转到中断服务函数中， 板子亮红灯


