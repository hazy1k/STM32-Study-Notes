/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   �������ԣ��ж�ģʽ/EXTIģʽ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_exti.h" 


/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */ 
int main(void)
{
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();
	  	
	/* ��ʼ��EXTI�жϣ����°����ᴥ���жϣ�
  *  �����жϻ����stm32f4xx_it.c�ļ��еĺ���
	*  KEY1_IRQHandler��KEY2_IRQHandler�������жϣ���תLED�ơ�
	*/
	EXTI_Key_Config(); 
	
	/* �ȴ��жϣ�����ʹ���жϷ�ʽ��CPU������ѯ���� */
	while(1)                            
	{
	}
}
/*********************************************END OF FILE**********************/
