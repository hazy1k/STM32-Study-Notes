/**
  ******************************************************************************
  * @file    bsp_led.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   �����������ӿ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F429 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./beep/bsp_beep.h"   

 /**
  * @brief  ��ʼ�����Ʒ�������IO
  * @param  ��
  * @retval ��
  */
void BEEP_GPIO_Config(void)
{		
		/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
		GPIO_InitTypeDef GPIO_InitStructure;

		/*�������Ʒ�������GPIO�Ķ˿�ʱ��*/
		RCC_AHB1PeriphClockCmd( BEEP_GPIO_CLK, ENABLE);   

		/*ѡ��Ҫ���Ʒ�������GPIO*/															   
		GPIO_InitStructure.GPIO_Pin = BEEP_GPIO_PIN;	

		/*����GPIOģʽΪͨ���������*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
	
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;

		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;

		/*����GPIO����Ϊ50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

		/*���ÿ⺯������ʼ�����Ʒ�������GPIO*/
		GPIO_Init(BEEP_GPIO_PORT, &GPIO_InitStructure);			 
    
    /* �رշ�����*/
		GPIO_ResetBits(BEEP_GPIO_PORT, BEEP_GPIO_PIN);	 
}
/*********************************************END OF FILE**********************/
