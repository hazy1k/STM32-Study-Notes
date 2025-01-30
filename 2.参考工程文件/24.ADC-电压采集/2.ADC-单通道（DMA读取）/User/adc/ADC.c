#include "ADC.h"

__IO uint16_t ADC_ConvertedValue;

static void ADC_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_GPIO_APBxClock(ADC_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = ADC_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; // ģ������ģʽ
	GPIO_Init(ADC_GPIO_PORT, &GPIO_InitStructure);
}

static void ADC_Mode_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	/* ʱ������ */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	ADC_APBxClock(ADC_CLK, ENABLE);
	/* DAM���� */
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(ADCx->DR)); // �����ַΪADC���ݼĴ�����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue; // �ڴ��ַΪADC_ConvertedValue������ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; // ����Դ�������裬Ҳ����ADC
	DMA_InitStructure.DMA_BufferSize = 1; // һ��ֻ����һ������
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // �����ַ������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable; // �ڴ��ַ������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStructure);
	DMA_Cmd(ADC_DMA_CHANNEL, ENABLE);
	/* ADC���� */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADCx, &ADC_InitStructure);
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); // ����ADCʱ��ΪSYSCLK/8 = 72/8 = 9MHz
	ADC_RegularChannelConfig(ADCx, ADC_CHANNEL, 1, ADC_SampleTime_55Cycles5); // ����ADCͨ����ѡ��ADC_CHANNEL��ת��ʱ��Ϊ55��ʱ������
	ADC_Cmd(ADCx, ENABLE);
	ADC_ResetCalibration(ADCx);
	while(ADC_GetResetCalibrationStatus(ADCx));
	ADC_StartCalibration(ADCx);
	while(ADC_GetCalibrationStatus(ADCx));
	ADC_SoftwareStartConvCmd(ADCx, ENABLE);
}

void ADCx_Init(void)
{
	ADC_GPIO_Init();
	ADC_Mode_Init();
}
