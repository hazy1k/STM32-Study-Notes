/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   �߼����ƶ�ʱ��DMA
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 ָ���� ������
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include "./tim/bsp_advance_tim.h"
#include "./key/bsp_key.h" 

extern uint16_t aSRC_Buffer[3];
/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	/* ��ʼ������GPIO */
  Key_GPIO_Config();
	
  /* ��ʼ���߼����ƶ�ʱ��������PWMģʽ */
	TIMx_Configuration();
  
  while(1)
  {       
		/* ɨ��KEY1 */
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
		{
			aSRC_Buffer[0]=1000;
			aSRC_Buffer[1]=1000;
			aSRC_Buffer[2]=100;
		}   
    /* ɨ��KEY2 */
    if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
		{
			aSRC_Buffer[0]=300;
			aSRC_Buffer[1]=300;
			aSRC_Buffer[2]=100;
		}   
  }
}
/*********************************************END OF FILE**********************/

