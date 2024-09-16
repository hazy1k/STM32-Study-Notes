// TIM��ͨ��-����-���ݰ������ Ӧ��
#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_tpad.h"  
#include "bsp_SysTick.h"
#include "bsp_beep.h"

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{
	//uint16_t temp;
	/* led �˿����� */ 
	LED_GPIO_Config();
	
	/* ��������ʼ�� */	
	Beep_Init();

	/* ���ڳ�ʼ�� */
	USART_Config();
	printf ( "\r\nҰ��STM32 ���벶����ݰ������ʵ��\r\n" );
	printf ( "\r\n�������ݰ����������������\r\n" );
	
//	TPAD_TIM_Init();
//	while(1)
//  {
//		temp = TPAD_Get_Val();
//		printf("���ݰ���Ĭ�ϳ��ʱ��Ϊ: %d us\n",temp);
//		SysTick_Delay_Ms(100);
//	}
	
	// ��ʼ�����ݰ���
	while( TPAD_Init() );

	while(1)
  {
		if( TPAD_Scan() == TPAD_ON )
    {
      BEEP_ON();
			SysTick_Delay_Ms(25);
			BEEP_OFF();
	  }
	}
}
/*********************************************END OF FILE**********************/
