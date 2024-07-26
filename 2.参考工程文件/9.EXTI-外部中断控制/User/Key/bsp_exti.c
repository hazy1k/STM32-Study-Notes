#include "bsp_exti.h"

// 配置嵌套向量中断控制器NVIC
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  // 1.配置NVIC为优先级组1
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); // 使用库函数misc.C中的函数-NVIC_PriorityGroupConfig()配置NVIC为优先级组1
  
  // 2.配置中断源：按键1
  NVIC_InitStructure.NVIC_IRQChannel = KEY1_INT_EXTI_IRQ; // 实际上是使用的是EXTI_IRQn

  // 3.配置中断子优先级：1
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;

  // 4.配置子优先级：1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;

  // 5.使能中断通道
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  // 6.配置中断源：按键2，其他使用上面相关配置
  NVIC_InitStructure.NVIC_IRQChannel = KEY2_INT_EXTI_IRQ;
  NVIC_Init(&NVIC_InitStructure);
}

// 配置IO为EXTI中断口，并设置中断优先级
void EXTI_Key_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;  // 定义GPIO初始化结构体变量
	EXTI_InitTypeDef EXTI_InitStructure; // 定义EXTI初始化结构体变量

	// 使能GPIO时钟
	RCC_APB2PeriphClockCmd(KEY1_INT_GPIO_CLK, ENABLE);
  RCC_APB2PeriphClockCmd(KEY2_INT_GPIO_CLK, ENABLE);
												
	// 配置NVIC中断
	NVIC_Configuration();
	
/*--------------------------KEY1配置-----------------------------*/
	// 1.选择按键用到的GPIO引脚
  GPIO_InitStructure.GPIO_Pin = KEY1_INT_GPIO_PIN;

  // 2.配置为浮空输入
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(KEY1_INT_GPIO_PORT, &GPIO_InitStructure);

	// 3.选择EXTI的信号源
  GPIO_EXTILineConfig(KEY1_INT_EXTI_PORTSOURCE, KEY1_INT_EXTI_PINSOURCE); 
  EXTI_InitStructure.EXTI_Line = KEY1_INT_EXTI_LINE;
	
	// 4.配置EXTI为中断模式
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;

	// 5.上升沿中断
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;

  // 6.使能中断	
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
	
  /*--------------------------KEY2配置-----------------------------*/
	/* 选择按键用到的GPIO */	
  GPIO_InitStructure.GPIO_Pin = KEY2_INT_GPIO_PIN;
  /* 配置为浮空输入 */	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(KEY2_INT_GPIO_PORT, &GPIO_InitStructure);

	/* 选择EXTI的信号源 */
  GPIO_EXTILineConfig(KEY2_INT_EXTI_PORTSOURCE, KEY2_INT_EXTI_PINSOURCE); 
  EXTI_InitStructure.EXTI_Line = KEY2_INT_EXTI_LINE;
	
	/* EXTI为中断模式 */
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	/* 下降沿中断 */
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  /* 使能中断 */	
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
}
