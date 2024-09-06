/**
  ******************************************************************************
  * @file    bsp_exti.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   �����������ж�����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32H750 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./mpu6050/bsp_mpu_exti.h"

 /**
  * @brief  �����жϿڣ��������ж����ȼ�
  * @param  ��
  * @retval ��
  */
void EXTI_MPU_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; 

    /*����GPIO�ڵ�ʱ��*/
    MPU_INT_GPIO_CLK_ENABLE();

    /* ѡ���жϵ����� */ 
    GPIO_InitStructure.Pin = MPU_INT_GPIO_PIN;
    /* ��������Ϊ����ģʽ */ 
    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;	    		
    /* �������Ų�����Ҳ������ */
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    /* ʹ������Ľṹ���ʼ������ */
    HAL_GPIO_Init(MPU_INT_GPIO_PORT, &GPIO_InitStructure); 
    /* ���� EXTI �ж�Դ ��key1 ���š������ж����ȼ�*/
    HAL_NVIC_SetPriority(MPU_INT_EXTI_IRQ, 1, 1);
    /* ʹ���ж� */
    HAL_NVIC_EnableIRQ(MPU_INT_EXTI_IRQ);
}
/*********************************************END OF FILE**********************/
