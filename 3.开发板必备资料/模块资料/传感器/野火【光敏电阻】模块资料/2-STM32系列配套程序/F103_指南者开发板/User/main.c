/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   ���Թ���
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
#include "./ldr/bsp_ldr.h" 

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */ 
int main(void)
{	
	/* LED�˿ڳ�ʼ�� */
	LED_GPIO_Config();
	LED2_OFF;

	/* �����˿ڳ�ʼ�� */
	LDR_GPIO_Config();
	
	/* ��ѯ����״̬�����й�������� */
	while(1)
	{	   
		if (LDR_Test(LDR_GPIO_PORT,LDR_GPIO_PIN) == LDR_ON)
    {
      LED2_ON;    // �й����
    }
    else
      LED2_OFF;   // �޹����
	}
}
/*********************************************END OF FILE**********************/
