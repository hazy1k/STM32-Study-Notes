/**
  ******************************************************************************
  * @file    bsp_SysTick.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   SysTick ϵͳ�δ�ʱ��10us�жϺ�����,�ж�ʱ����������ã�
  *          ���õ��� 1us 10us 1ms �жϡ�     
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��STM32 F103-�Ե� ������
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
	
#include "bsp_SysTick.h"




/**
  * @brief  ����ϵͳ�δ�ʱ�� SysTick
  * @param  ��
  * @retval ��
  */
void SysTick_Init( void )
{
	/* SystemFrequency / 1000    1ms�ж�һ��
	 * SystemFrequency / 100000	 10us�ж�һ��
	 * SystemFrequency / 1000000 1us�ж�һ��
	 */
	if ( SysTick_Config(SystemCoreClock / 100) )	// ST3.5.0��汾
	{ 
		/* Capture error */ 
		while (1);
	}
  
  // �رյδ�ʱ��  
	//SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
	
  // ʹ�ܵδ�ʱ��  10ms�ж�һ��
	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;
  
}







/*********************************************END OF FILE**********************/
