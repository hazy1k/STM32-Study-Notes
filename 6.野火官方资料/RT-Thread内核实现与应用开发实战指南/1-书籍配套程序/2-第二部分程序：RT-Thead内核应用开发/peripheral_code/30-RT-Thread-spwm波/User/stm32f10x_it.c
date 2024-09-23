/* ���ļ�ͳһ���ڴ���жϷ����� */
#include "stm32f10x_it.h"
#include "./spwm/bsp_spwm.h"

extern uint16_t indexWave[];

extern __IO uint32_t rgb_color;

/* �������жϷ����� */
void BRE_TIMx_IRQHandler(void)
{	
	static uint16_t pwm_index = 0;			//����PWM���
	static uint16_t period_cnt = 0;		//���ڼ���������
	static uint16_t amplitude_cnt = 0;	//���ڼ����ֵ�ȼ�

	if (TIM_GetITStatus(BRE_TIMx, TIM_IT_Update) != RESET)	//TIM_IT_Update
 	{		
			amplitude_cnt++;
				
			//ÿ��PWM���е�ÿ��Ԫ����AMPLITUDE_CLASS���ȼ���
		  //ÿ����һ�������һ�����壬��PWM���е�Ԫ�ض�ʹ��һ��
		  //ʹ��256�Σ�����RGB��ɫ��������ͨ�����
			if(amplitude_cnt > (AMPLITUDE_CLASS-1))		 						
			{		
				period_cnt++;
				
				//ÿ��PWM���е�ÿ��Ԫ��ʹ��period_class��
				if(period_cnt > period_class)
				{				

					pwm_index++;												//��־PWM��ָ����һ��Ԫ��
				
					//��PWM���ѵ����β������ָ���ͷ
					if( pwm_index >=  POINT_NUM)			
					{
						pwm_index=0;								
					}
					
					period_cnt = 0;											//�������ڼ�����־
				}
				
				amplitude_cnt=0;											//���÷�ֵ������־
			}
			else
			{	
					//ÿ��PWM���е�ÿ��Ԫ����AMPLITUDE_CLASS���ȼ���
					//ÿ����һ�������һ�����壬��PWM���е�Ԫ�ض�ʹ��һ��

					//����RGB��ɫ����ֵ�����ø���ͨ���Ƿ������ǰ��PWM��Ԫ�ر�ʾ������
					//��
					if(((rgb_color&0xFF0000)>>16) >= amplitude_cnt)				
						BRE_TIMx->BRE_RED_CCRx = indexWave[pwm_index];	//����PWM���޸Ķ�ʱ���ıȽϼĴ���ֵ
					else
						BRE_TIMx->BRE_RED_CCRx = 0;		//�ȽϼĴ���ֵΪ0��ͨ������ߵ�ƽ����ͨ��LED����
					
					//��
					if(((rgb_color&0x00FF00)>>8) >= amplitude_cnt)				
						BRE_TIMx->BRE_GREEN_CCRx = indexWave[pwm_index];	//����PWM���޸Ķ�ʱ���ıȽϼĴ���ֵ
					else
						BRE_TIMx->BRE_GREEN_CCRx = 0;	//�ȽϼĴ���ֵΪ0��ͨ������ߵ�ƽ����ͨ��LED����
					
					//��
					if((rgb_color&0x0000FF) >= amplitude_cnt)				
						BRE_TIMx->BRE_BLUE_CCRx = indexWave[pwm_index];	//����PWM���޸Ķ�ʱ���ıȽϼĴ���ֵ
					else
						BRE_TIMx->BRE_BLUE_CCRx = 0;		//�ȽϼĴ���ֵΪ0��ͨ������ߵ�ƽ����ͨ��LED����	

			}					
		
		TIM_ClearITPendingBit (BRE_TIMx, TIM_IT_Update);	//����Ҫ����жϱ�־λ
	}
}
