
#include "stm32f10x.h"
#include "bsp_SysTick.h"
#include "bsp_led.h"

/*
 * t : ��ʱʱ�� 
 * Ticks : ���ٸ�ʱ�����ڲ���һ���ж� 
 * f : ʱ��Ƶ�� 72000000
 * t = Ticks * 1/f = (72000000/100000) * (1/72000000) = 10us 
 */ 

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();

	/* ����SysTick Ϊ10us�ж�һ�� */
	SysTick_Init();

//	for(;;)
//	{

//		LED1( ON ); 
//	    Delay_us(100000);    	// 100000 * 10us = 1000ms
//		//Delay_ms(100);
//		LED1( OFF );
//	  
//		LED2( ON );
//	    Delay_us(100000);		// 100000 * 10us = 1000ms
//		//Delay_ms(100);
//		LED2( OFF );
//	
//		LED3( ON );
//	    Delay_us(100000);		// 100000 * 10us = 1000ms
//		//Delay_ms(100);
//		LED3( OFF );
//	}     

	for(;;)
	{

		LED1( ON ); 
		SysTick_Delay_Ms( 1000 );
		LED1( OFF );
	  
		LED2( ON );
		SysTick_Delay_Ms( 1000 );
		LED2( OFF );
	
		LED3( ON );
		SysTick_Delay_Ms( 1000 );
		LED3( OFF );
	} 
	
}





//  ============================================================================================================================
//    NVIC_PriorityGroup   | NVIC_IRQChannelPreemptionPriority | NVIC_IRQChannelSubPriority  | Description
//  ============================================================================================================================
//   NVIC_PriorityGroup_0  |                0                  |            0-15             |   0 bits for 
//                         |                                   |                             |   4 bits for subpriority
//  ----------------------------------------------------------------------------------------------------------------------------
//   NVIC_PriorityGroup_1  |                0-1                |            0-7              |   1 bits for pre-emption priority
//                         |                                   |                             |   3 bits for subpriority
//  ----------------------------------------------------------------------------------------------------------------------------    
//   NVIC_PriorityGroup_2  |                0-3                |            0-3              |   2 bits for pre-emption priority
//                         |                                   |                             |   2 bits for subpriority
//  ----------------------------------------------------------------------------------------------------------------------------    
//   NVIC_PriorityGroup_3  |                0-7                |            0-1              |   3 bits for pre-emption priority
//                         |                                   |                             |   1 bits for subpriority
//  ----------------------------------------------------------------------------------------------------------------------------    
//   NVIC_PriorityGroup_4  |                0-15               |            0                |   4 bits for pre-emption priority
//                         |                                   |                             |   0 bits for subpriority                       
//  ============================================================================================================================












/*********************************************END OF FILE**********************/
