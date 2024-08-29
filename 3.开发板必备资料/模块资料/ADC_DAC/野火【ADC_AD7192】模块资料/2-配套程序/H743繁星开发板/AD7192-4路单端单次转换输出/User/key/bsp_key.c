/**
  ******************************************************************************
  * @file    bsp_key.c
  * @author  fire
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   ����Ӧ��bsp��ɨ��ģʽ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 H743 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 

#include ".\key\bsp_key.h" 

/**
  * @brief  ���ð����õ���I/O��
  * @param  ��
  * @retval ��
  */

void Key_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    /*��������GPIO�ڵ�ʱ��*/
    KEY1_GPIO_CLK_ENABLE();
    KEY2_GPIO_CLK_ENABLE();
		KEY3_GPIO_CLK_ENABLE();
		KEY4_GPIO_CLK_ENABLE();
		KEY5_GPIO_CLK_ENABLE();
    /*ѡ�񰴼�������*/	
    GPIO_InitStructure.Pin = KEY1_PIN; 
    /*��������Ϊ����ģʽ*/
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT; 
    /*�������Ų�����Ҳ������*/
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    /*ʹ������Ľṹ���ʼ������*/
    HAL_GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);
    /*ѡ�񰴼�������*/
    GPIO_InitStructure.Pin = KEY2_PIN; 
    /*ʹ������Ľṹ���ʼ������*/
    HAL_GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure);
	
		GPIO_InitStructure.Pin = KEY3_PIN; 
		HAL_GPIO_Init(KEY3_GPIO_PORT, &GPIO_InitStructure);
		
		GPIO_InitStructure.Pin = KEY4_PIN; 
		HAL_GPIO_Init(KEY4_GPIO_PORT, &GPIO_InitStructure);
		
		GPIO_InitStructure.Pin = KEY5_PIN; 
		HAL_GPIO_Init(KEY5_GPIO_PORT, &GPIO_InitStructure);
}



/**
  * @brief   ����Ƿ��а�������
  * @param   ����Ķ˿ںͶ˿�λ
  *		@arg GPIOx: x�����ǣ�A...G�� 
  *		@arg GPIO_PIN ������GPIO_PIN_x��x������1...16��
  * @retval  ������״̬
  *		@arg KEY_ON:��������
  *		@arg KEY_OFF:����û����
  */

uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
{			
	/*����Ƿ��а������� */
	if(HAL_GPIO_ReadPin(GPIOx,GPIO_Pin) == KEY_ON )  
	{	 
		/*�ȴ������ͷ� */
		while(HAL_GPIO_ReadPin(GPIOx,GPIO_Pin) == KEY_ON);   
		return 	KEY_ON;	 
	}
	else
		return KEY_OFF;
}

/*********************************************END OF FILE**********************/

