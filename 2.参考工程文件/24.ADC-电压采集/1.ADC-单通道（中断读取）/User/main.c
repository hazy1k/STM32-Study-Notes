#include "stm32f10x.h"
#include "usart.h"
#include "ADC.h"
#include "SysTick.h"

extern __IO uint16_t ADC_ConvertedValue;
float ADC_Result;

int main(void)
{
	USART_Config();
	SysTick_Init();
	ADCx_Init();
	while(1)
	{
		ADC_Result = (float)ADC_ConvertedValue/4096*3.3;
		printf("�ɼ�����ADCֵΪ = 0x%04x \r\n", ADC_ConvertedValue);
		printf("����õ���ѹֵ = %.2f V \r\n", ADC_Result);
		Delay_ms(2000);
	}
}
