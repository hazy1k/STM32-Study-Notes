#include "stm32f10x.h"
#include "usart.h"
#include "ADC.h"
#include "SysTick.h"

extern __IO uint16_t ADC_ConvertedValue;
float ADC_Result;

int main(void)
{
    SysTick_Init();
    USART_Config();
    ADCx_Init();
    while (1)
    {
        // ���ADC�Ƿ����ת��
        if(ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC))
        {
            // ���DMA���ã�ֱ�Ӵ��ڴ������ȡֵ
            ADC_ConvertedValue = ADC_GetConversionValue(ADCx); // ��ȡת�����
            ADC_ClearFlag(ADCx, ADC_FLAG_EOC);  // �����־λ
            ADC_Result = (float)ADC_ConvertedValue / 4096 * 3.3;
            printf("\r\n ADC�ɼ�����ֵ = 0x%04X \r\n", ADC_ConvertedValue);
            printf("\r\n ����õ��ĵ�ѹֵ = %f V \r\n", ADC_Result);
			Delay_ms(1000); 
        }
        Delay_ms(2000);
    }
}
