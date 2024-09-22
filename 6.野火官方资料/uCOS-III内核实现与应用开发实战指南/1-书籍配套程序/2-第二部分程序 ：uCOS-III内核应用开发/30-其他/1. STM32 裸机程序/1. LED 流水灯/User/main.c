/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ����led
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
	
#include "stm32f10x.h"
#include "bsp_led.h"



static void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}


/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main ( void )
{	
	LED_Init ();	          //��ʼ�� LED
 

	while ( 1 )
	{
		macLED1_ON ();			  // ��
		Delay ( 0x0FFFFF );
		macLED1_OFF ();		  // ��

		macLED2_ON ();			  // ��
		Delay ( 0x0FFFFF );
		macLED2_OFF ();		  // ��

		macLED3_ON ();			  // ��
		Delay ( 0x0FFFFF );
		macLED3_OFF ();		  // ��	   
		
	}

}

/*********************************************END OF FILE**********************/
