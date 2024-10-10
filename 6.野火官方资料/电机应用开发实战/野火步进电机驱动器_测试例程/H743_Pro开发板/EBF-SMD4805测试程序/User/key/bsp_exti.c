/**
  ******************************************************************************
  * @file    bsp_exti.c
  * @author  fire
  * @version V1.0
  * @date    2016-xx-xx
  * @brief   I/O���ж�Ӧ��bsp
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32 H750 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./key/bsp_exti.h"


 /**
  * @brief  ���� PA0 Ϊ���жϿڣ��������ж����ȼ�
  * @param  ��
  * @retval ��
  */
void EXTI_Key_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 

	/*��������GPIO�ڵ�ʱ��*/
	KEY1_INT_GPIO_CLK_ENABLE();
	KEY2_INT_GPIO_CLK_ENABLE();

	/* ѡ�񰴼�1������ */ 
	GPIO_InitStructure.Pin = KEY1_INT_GPIO_PIN;
	/* ��������Ϊ����ģʽ */ 
	GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;	    		
	/* �������Ų�����Ҳ������ */
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	/* ʹ������Ľṹ���ʼ������ */
	HAL_GPIO_Init(KEY1_INT_GPIO_PORT, &GPIO_InitStructure); 
	/* H750XBH6��PA0��PA0_C����EXTI0�ⲿ�ж�Դ���������������ĺ��̣�
	   ͨ��һ��ģ�⿪�����ӵ�Ƭ�����裬Ϊ�˱�����ţ�ʹ��ʱ��Ҫ�л� */
  /* PA0 ------> EXTI0  */
	HAL_SYSCFG_AnalogSwitchConfig(SYSCFG_SWITCH_PA0, SYSCFG_SWITCH_PA0_CLOSE);
	/* ���� EXTI �ж�Դ ��key1 ���š������ж����ȼ�*/
	HAL_NVIC_SetPriority(KEY1_INT_EXTI_IRQ, 0, 0);
	/* ʹ���ж� */
	HAL_NVIC_EnableIRQ(KEY1_INT_EXTI_IRQ);

	/* ѡ�񰴼�2������ */ 
	GPIO_InitStructure.Pin = KEY2_INT_GPIO_PIN;  
	/* ����������������ͬ */
	HAL_GPIO_Init(KEY2_INT_GPIO_PORT, &GPIO_InitStructure);      
	/* ���� EXTI �ж�Դ ��key1 ���š������ж����ȼ�*/
	HAL_NVIC_SetPriority(KEY2_INT_EXTI_IRQ, 0, 0);
	/* ʹ���ж� */
	HAL_NVIC_EnableIRQ(KEY2_INT_EXTI_IRQ);
}
/*********************************************END OF FILE**********************/
