/* ���ļ�ͳһ���ڴ���жϷ����� */
#include "stm32f10x_it.h"

#include "board.h" 


extern __IO uint16_t ADC_ConvertedValue;

void ADC_IRQHandler(void)
{	
	if (ADC_GetITStatus(ADCx,ADC_IT_EOC)==SET) 
	{
		// ��ȡADC��ת��ֵ
		ADC_ConvertedValue = ADC_GetConversionValue(ADCx);
	}
	ADC_ClearITPendingBit(ADCx,ADC_IT_EOC);
}




