/**
  ******************************************************************************
  * @file    bsp_irda.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   红外遥控器接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32H743 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
#include "./systick/bsp_SysTick.h"
#include "./IrDa/bsp_irda.h"  
#include "./led/bsp_led.h"


uint32_t frame_data=0;    /* 一帧数据缓存 */
uint8_t  frame_cnt=0;
uint8_t  frame_flag=0;    /* 一帧数据接收完成标志 */

 
/* 初始化红外接收头1838用到的IO */
void IrDa_Init(void)
{
	  GPIO_InitTypeDef  GPIO_InitStruct;

    IRDA_GPIO_CLK();
	  /* 配置引脚 */
    GPIO_InitStruct.Pin = IRDA_GPIO_PIN;
    /* 设置引脚为输入模式 */ 
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;	    		
    /* 设置引脚不上拉也不下拉 */
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    /* 使用上面的结构体初始化 */
    HAL_GPIO_Init(IRDA_GPIO_PORT, &GPIO_InitStruct); 
    /*配置中断优先级*/
    HAL_NVIC_SetPriority(IRDA_EXTI_IRQN,1,3);
    /* 使能中断 */
    HAL_NVIC_EnableIRQ(IRDA_EXTI_IRQN);
	
}

/* 获取高电平的时间 */
uint8_t Get_Pulse_Time(void)
{
  uint8_t time = 0;
  while( IrDa_DATA_IN() )
  {
    time ++;
    Delay_us(2);     // 延时 20us
    if(time == 250)
      return time;   // 超时溢出   
  }
  return time;
}

/*
 * 帧数据有4个字节，第一个字节是遥控的ID，第二个字节是第一个字节的反码
 * 第三个数据是遥控的真正的键值，第四个字节是第三个字节的反码
 */
uint8_t IrDa_Process(void)
{
  uint8_t first_byte, sec_byte, tir_byte, fou_byte;  
  
  first_byte = frame_data >> 24;
  sec_byte = (frame_data>>16) & 0xff;
  tir_byte = frame_data >> 8;
  fou_byte = frame_data;
  
  /* 记得清标志位 */
  frame_flag = 0;
  
  if( (first_byte==(uint8_t)~sec_byte) && (first_byte==IRDA_ID) )
  {
    if( tir_byte == (uint8_t)~fou_byte )
      return tir_byte;
  }
  
  return 0;   /* 错误返回 */
}

/*********************************************END OF FILE**********************/
