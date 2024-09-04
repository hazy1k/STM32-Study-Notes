/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ʹ���ⲿ�ж�EXTI��ⰴ�������Ʋʵơ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32 F429 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "./led/bsp_led.h"
#include "./sr501/bsp_exti.h"

	/// ����ȷ����ʱ
void Delay(__IO u32 nCount)
{
	for(; nCount != 0; nCount--);
} 



/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();
  	
	/* ��ʼ��EXTI�жϣ���Ӧ��������Դ�ᴥ���ж�*/
	EXTI_SR501_Config(); 
	
	/* �ȴ��жϣ�����ʹ���жϷ�ʽ��CPU������ѯ���� */
	while(1)                            
	{
		if( GPIO_ReadInputDataBit(SR501_INT_GPIO_PORT,SR501_INT_GPIO_PIN)  == 0 )
		{			
			GPIO_SetBits(LED1_GPIO_PORT, LED1_PIN);
			
		}
	
		Delay(0xFFF);
	}
}



/*********************************************END OF FILE**********************/

