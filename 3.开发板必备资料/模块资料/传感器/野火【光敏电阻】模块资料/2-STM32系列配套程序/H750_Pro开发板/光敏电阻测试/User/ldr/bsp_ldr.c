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
  * ʵ��ƽ̨:Ұ�� STM32H750 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "./ldr/bsp_ldr.h" 
#include "./led/bsp_led.h"

/**
  * @brief  ���ù����õ���I/O��
  * @param  ��
  * @retval ��
  */
void LDR_GPIO_Config(void)
{
	
/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	/*�����˿ڵ�ʱ��*/
	LDR_GPIO_CLK_ENABLE();
	
	//ѡ�����������
	GPIO_InitStruct.Pin = LDR_PIN; 
	
  /*�������ŵ��������Ϊ�������*/
	GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;  
  
 	/*��������Ϊ����ģʽ*/
	GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
	
	/*������������Ϊ���� */   
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; 

	/*���ÿ⺯����ʹ���������õ�GPIO_InitStructure��ʼ��GPIO*/
	HAL_GPIO_Init(LDR_GPIO_PORT, &GPIO_InitStruct);	
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
	if(HAL_GPIO_ReadPin(GPIOx,GPIO_Pin) == 1 )  
	{	  
		return LDR_OFF;    // �޹�
	}
	else
		return LDR_ON;    // �й�
}
/*********************************************END OF FILE**********************/
