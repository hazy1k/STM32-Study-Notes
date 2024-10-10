/**
  ******************************************************************************
  * @file    bsp_motor_control.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   编码器接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F407 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "./Encoder/bsp_encoder.h"

/* 定时器溢出次数 */
__IO int16_t Encoder_Overflow_Count = 0;

TIM_TimeBaseInitTypeDef TIM_EncoderHandle;

/**
  * @brief  编码器接口引脚初始化
  * @param  无
  * @retval 无
  */
static void Encoder_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  /* 定时器通道引脚端口时钟使能 */
  ENCODER_TIM_CH1_GPIO_CLK_ENABLE();
  ENCODER_TIM_CH2_GPIO_CLK_ENABLE();
  
  /**TIM3 GPIO Configuration    
  PC6     ------> TIM3_CH1
  PC7     ------> TIM3_CH2 
  */
  /* 设置输入类型 */
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  /* 设置上拉 */
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  /* 设置引脚速率 */
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
  
  /* 选择要控制的GPIO引脚 */	
  GPIO_InitStruct.GPIO_Pin = ENCODER_TIM_CH1_PIN;
  /* 调用库函数，使用上面配置的GPIO_InitStructure初始化GPIO */
  GPIO_Init(ENCODER_TIM_CH1_GPIO_PORT, &GPIO_InitStruct);
  GPIO_PinAFConfig(ENCODER_TIM_CH1_GPIO_PORT, ENCODER_TIM_CH1_GPIO_AF, GPIO_AF_TIM3);
  /* 选择要控制的GPIO引脚 */	
  GPIO_InitStruct.GPIO_Pin = ENCODER_TIM_CH2_PIN;
  /* 调用库函数，使用上面配置的GPIO_InitStructure初始化GPIO */
  GPIO_Init(ENCODER_TIM_CH2_GPIO_PORT, &GPIO_InitStruct);
  GPIO_PinAFConfig(ENCODER_TIM_CH1_GPIO_PORT, ENCODER_TIM_CH2_GPIO_AF, GPIO_AF_TIM3);
}

/**
  * @brief  配置TIMx编码器模式
  * @param  无
  * @retval 无
  */
static void TIM_Encoder_Init(void)
{ 
  TIM_ICInitTypeDef Encoder_ConfigStructure = {0};
  
  /* 使能编码器接口时钟 */
  ENCODER_TIM_CLK_ENABLE();
  
  /* 定时器初始化设置 */
  TIM_EncoderHandle.TIM_Prescaler = ENCODER_TIM_PRESCALER;
  TIM_EncoderHandle.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_EncoderHandle.TIM_Period = ENCODER_TIM_PERIOD;
  TIM_EncoderHandle.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInit(ENCODER_TIM, &TIM_EncoderHandle); 
  
  TIM_EncoderInterfaceConfig(ENCODER_TIM, ENCODER_MODE,
                             TIM_ICPolarity_Rising ,TIM_ICPolarity_Rising);	//配置编码器模式触发源和极性
  /* 编码器接口通道1设置 */
  Encoder_ConfigStructure.TIM_Channel = TIM_Channel_1;
  Encoder_ConfigStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  Encoder_ConfigStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  Encoder_ConfigStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  Encoder_ConfigStructure.TIM_ICFilter = 0;
  /* 初始化编码器接口 */
  TIM_ICInit(ENCODER_TIM, &Encoder_ConfigStructure);
  
  /* 编码器接口通道2设置 */
  Encoder_ConfigStructure.TIM_Channel = TIM_Channel_2;
  /* 初始化编码器接口 */
  TIM_ICInit(ENCODER_TIM, &Encoder_ConfigStructure);
  
  NVIC_InitTypeDef NVIC_InitStructure; 
  // 设置中断组为0
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
  // 设置中断来源
  NVIC_InitStructure.NVIC_IRQChannel = ENCODER_TIM_IRQn; 	
  // 设置抢占优先级
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	 
  // 设置子优先级
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;	
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;   
  
  /* 清零计数器 */
  ENCODER_TIM->CNT = 0;

  /* 设置更新事件请求源为：计数器溢出 */
  TIM_UpdateRequestConfig(ENCODER_TIM,TIM_UpdateSource_Regular);
  /* 使能定时器的更新事件中断 */
  TIM_ITConfig(ENCODER_TIM, TIM_IT_Update, ENABLE);
  
  NVIC_Init(&NVIC_InitStructure);
  
  /* 使能编码器接口 */
  // 使能计数器
	TIM_Cmd(ENCODER_TIM, ENABLE);
}

/**
  * @brief  编码器接口初始化
  * @param  无
  * @retval 无
  */
void Encoder_Init(void)
{
  Encoder_GPIO_Init();    /* 引脚初始化 */
  TIM_Encoder_Init();     /* 配置编码器接口 */
}

/*********************************************END OF FILE**********************/

