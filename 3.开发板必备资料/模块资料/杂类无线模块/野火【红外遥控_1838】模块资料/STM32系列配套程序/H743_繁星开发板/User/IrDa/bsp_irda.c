/**
  ******************************************************************************
  * @file    bsp_irda.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   ����ң�����ӿ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32H743 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
#include "./systick/bsp_SysTick.h"
#include "./IrDa/bsp_irda.h"  
#include "./led/bsp_led.h"


uint32_t frame_data=0;    /* һ֡���ݻ��� */
uint8_t  frame_cnt=0;
uint8_t  frame_flag=0;    /* һ֡���ݽ�����ɱ�־ */

 
/* ��ʼ���������ͷ1838�õ���IO */
void IrDa_Init(void)
{
	  GPIO_InitTypeDef  GPIO_InitStruct;

    IRDA_GPIO_CLK();
	  /* �������� */
    GPIO_InitStruct.Pin = IRDA_GPIO_PIN;
    /* ��������Ϊ����ģʽ */ 
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;	    		
    /* �������Ų�����Ҳ������ */
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    /* ʹ������Ľṹ���ʼ�� */
    HAL_GPIO_Init(IRDA_GPIO_PORT, &GPIO_InitStruct); 
    /*�����ж����ȼ�*/
    HAL_NVIC_SetPriority(IRDA_EXTI_IRQN,1,3);
    /* ʹ���ж� */
    HAL_NVIC_EnableIRQ(IRDA_EXTI_IRQN);
	
}

/* ��ȡ�ߵ�ƽ��ʱ�� */
uint8_t Get_Pulse_Time(void)
{
  uint8_t time = 0;
  while( IrDa_DATA_IN() )
  {
    time ++;
    Delay_us(2);     // ��ʱ 20us
    if(time == 250)
      return time;   // ��ʱ���   
  }
  return time;
}

/*
 * ֡������4���ֽڣ���һ���ֽ���ң�ص�ID���ڶ����ֽ��ǵ�һ���ֽڵķ���
 * ������������ң�ص������ļ�ֵ�����ĸ��ֽ��ǵ������ֽڵķ���
 */
uint8_t IrDa_Process(void)
{
  uint8_t first_byte, sec_byte, tir_byte, fou_byte;  
  
  first_byte = frame_data >> 24;
  sec_byte = (frame_data>>16) & 0xff;
  tir_byte = frame_data >> 8;
  fou_byte = frame_data;
  
  /* �ǵ����־λ */
  frame_flag = 0;
  
  if( (first_byte==(uint8_t)~sec_byte) && (first_byte==IRDA_ID) )
  {
    if( tir_byte == (uint8_t)~fou_byte )
      return tir_byte;
  }
  
  return 0;   /* ���󷵻� */
}

/*********************************************END OF FILE**********************/
