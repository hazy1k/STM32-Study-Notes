/**
  ******************************************************************************
  * @file    bsp_beep.c
  * @author  fire
  * @version V3.0
  * @date    2015-xx-xx
  * @brief   ������Ӧ�ú����ӿ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� IOT STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://fire-stm32.taobao.com
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
		RCC_APB2PeriphClockCmd( BEEP_GPIO_CLK, ENABLE); 

		/*ѡ��Ҫ���Ʒ�������GPIO*/															   
		GPIO_InitStructure.GPIO_Pin = BEEP_GPIO_PIN;	

		/*����GPIOģʽΪͨ���������*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

		/*����GPIO����Ϊ50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

		/*���ÿ⺯������ʼ�����Ʒ�������GPIO*/
		GPIO_Init(BEEP_GPIO_PORT, &GPIO_InitStructure);			 
    
    /* �رշ�����*/
		GPIO_ResetBits(BEEP_GPIO_PORT, BEEP_GPIO_PIN);	 
}
/*********************************************END OF FILE**********************/
