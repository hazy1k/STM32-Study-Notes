/* ���ļ�ͳһ���ڴ���жϷ����� */
#include "stm32f10x_it.h"
#include "board.h" 
#include "rtthread.h"

/*
*************************************************************************
*                               ����
*************************************************************************
*/

extern volatile uint32_t time_num;
/* �����ź������ƿ� */
extern rt_sem_t test_sem;


void GENERAL_TIM_INT_FUN(void)
{
	// ��Ҫ��������źŵ����ڴ��ڶ�ʱ�������ʱʱ����ʱ���ͻ���������������ж�
	// ���ʱ��������Ҫ�������Ķ�ʱ���ڼӵ������źŵ�ʱ������ȥ
	if ( TIM_GetITStatus ( GENERAL_TIM, TIM_IT_Update) != RESET )               
	{	
		TIM_ICUserValueStructure.Capture_Period ++;		
		TIM_ClearITPendingBit ( GENERAL_TIM, TIM_FLAG_Update ); 		
	}

	// �����ز����ж�
	if ( TIM_GetITStatus (GENERAL_TIM, GENERAL_TIM_IT_CCx ) != RESET)
	{
		// ��һ�β���
		if ( TIM_ICUserValueStructure.Capture_StartFlag == 0 )
		{
			// ��������0
			TIM_SetCounter ( GENERAL_TIM, 0 );
			// �Զ���װ�ؼĴ������±�־��0
			TIM_ICUserValueStructure.Capture_Period = 0;
      // �沶��ȽϼĴ�����ֵ�ı�����ֵ��0			
			TIM_ICUserValueStructure.Capture_CcrValue = 0;

			// ����һ�β���������֮�󣬾ͰѲ����������Ϊ�½���
			GENERAL_TIM_OCxPolarityConfig_FUN(GENERAL_TIM, TIM_ICPolarity_Falling);
      // ��ʼ�����׼��1			
			TIM_ICUserValueStructure.Capture_StartFlag = 1;			
		}
		// �½��ز����ж�
		else // �ڶ��β���
		{
			// ��ȡ����ȽϼĴ�����ֵ�����ֵ���ǲ��񵽵ĸߵ�ƽ��ʱ���ֵ
			TIM_ICUserValueStructure.Capture_CcrValue = 
			GENERAL_TIM_GetCapturex_FUN (GENERAL_TIM);

			// ���ڶ��β����½���֮�󣬾ͰѲ����������Ϊ�����أ��ÿ����µ�һ�ֲ���
			GENERAL_TIM_OCxPolarityConfig_FUN(GENERAL_TIM, TIM_ICPolarity_Rising);
      // ��ʼ�����־��0		
			TIM_ICUserValueStructure.Capture_StartFlag = 0;
      // ������ɱ�־��1			
			TIM_ICUserValueStructure.Capture_FinishFlag = 1;		
			
			rt_sem_release(test_sem);   //�ͷŶ�ֵ�ź���
		}

		TIM_ClearITPendingBit (GENERAL_TIM,GENERAL_TIM_IT_CCx);	    
	}		
}

