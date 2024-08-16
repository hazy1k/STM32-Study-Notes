// TIM��ͨ��-����-�������� Ӧ��
#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_GeneralTim.h"  

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{
	uint32_t time;
	
	// TIM ������������ʱ��
	uint32_t TIM_PscCLK = 72000000 / (GENERAL_TIM_PSC+1);
	
	/* ���ڳ�ʼ�� */
	USART_Config();
	
	/* ��ʱ����ʼ�� */
	GENERAL_TIM_Init();
	
	printf ( "\r\nҰ�� STM32 ���벶��ʵ��\r\n" );
	printf ( "\r\n����K1������K1���µ�ʱ��\r\n" );
	
	while ( 1 )
	{
		if(TIM_ICUserValueStructure.Capture_FinishFlag == 1)
		{
			// ����ߵ�ƽʱ��ļ�������ֵ
			time = TIM_ICUserValueStructure.Capture_Period * (GENERAL_TIM_PERIOD+1) + 
			       (TIM_ICUserValueStructure.Capture_CcrValue+1);
			
			// ��ӡ�ߵ�ƽ����ʱ��
			printf ( "\r\n��øߵ�ƽ����ʱ�䣺%d.%d s\r\n",time/TIM_PscCLK,time%TIM_PscCLK );
			
			TIM_ICUserValueStructure.Capture_FinishFlag = 0;			
		}		
	}
}
/*********************************************END OF FILE**********************/
