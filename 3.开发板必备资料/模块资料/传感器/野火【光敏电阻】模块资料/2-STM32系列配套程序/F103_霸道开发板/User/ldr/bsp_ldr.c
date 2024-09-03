/**
  ******************************************************************************
  * @file    bsp_photoresistors.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   ����ģ�����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-MINI STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "./ldr/bsp_ldr.h" 
#include "bsp_led.h"

/**
  * @brief  ���ù����õ���I/O��
  * @param  ��
  * @retval ��
  */
void LDR_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/*�����˿ڵ�ʱ��*/
	RCC_APB2PeriphClockCmd(LDR_GPIO_CLK,ENABLE);
	
	//ѡ�����������
	GPIO_InitStructure.GPIO_Pin = LDR_GPIO_PIN; 
	// ���ù������������Ϊ��������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 
	GPIO_Init(LDR_GPIO_PORT, &GPIO_InitStructure);
}

 /*
 * �������������������
 * ����  �������������״̬
 * ����  ��GPIOx��x ������ A��B��C��D���� E
 *		     GPIO_Pin������ȡ�Ķ˿�λ 	
 * ���  ��LDR_ON���й�   LDR_OFF���޹�
 */
uint8_t LDR_Test(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
{			
	/*����������״̬ */
	if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == 1 )  
	{	  
		return LDR_OFF;    // �޹�
	}
	else
		return LDR_ON;    // �й�
}
/*********************************************END OF FILE**********************/
