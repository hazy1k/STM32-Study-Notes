/**
  ******************************************************************************
  * @file    bsp_irda.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ����ң�����ӿ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F429 STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include "./IrDa/bsp_irda.h"
#include "./systick/bsp_SysTick.h"
#include "./led/bsp_led.h"

uint32_t frame_data=0;    /* һ֡���ݻ��� */
uint8_t  frame_cnt=0;
uint8_t  frame_flag=0;    /* һ֡���ݽ�����ɱ�־ */

 /**
  * @brief  ����Ƕ�������жϿ�����NVIC
  * @param  ��
  * @retval ��
  */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* ����P[A|B|C|D|E]11Ϊ�ж�Դ */
  NVIC_InitStructure.NVIC_IRQChannel = IRDA_EXTI_IRQN;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

 
/* ��ʼ���������ͷ1838�õ���IO */
void IrDa_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;

	/* config the extiline clock and AFIO clock */
	RCC_AHB1PeriphClockCmd(IRDA_GPIO_CLK ,ENABLE);
							
  /* ʹ�� SYSCFG ʱ�� ��ʹ��GPIO�ⲿ�ж�ʱ����ʹ��SYSCFGʱ��*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	/* config the NVIC */
	NVIC_Configuration();

	/* EXTI line gpio config */	
  GPIO_InitStructure.GPIO_Pin = IRDA_GPIO_PIN;       
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;	
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
  GPIO_Init(IRDA_GPIO_PORT, &GPIO_InitStructure);

	/* EXTI line mode config */
  SYSCFG_EXTILineConfig(IRDA_GPIO_PORT_SOURCE,IRDA_GPIO_PIN_SOURCE);
  EXTI_InitStructure.EXTI_Line = IRDA_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½����ж�
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);  
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
    if( tir_byte == (u8)~fou_byte )
      return tir_byte;
  }
  
  return 0;   /* ���󷵻� */
}

/*********************************************END OF FILE**********************/
