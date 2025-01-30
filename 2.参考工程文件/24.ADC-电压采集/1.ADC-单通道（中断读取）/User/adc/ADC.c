#include "ADC.h"

__IO uint16_t ADC_ConvertedValue; // ȫ�ֱ��������ADCת��ֵ

static void ADC_NVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

static void ADC_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_GPIO_APBxClock(ADC_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = ADC_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; // ��ͨ������ģʽ
	GPIO_Init(ADC_GPIO_PORT, &GPIO_InitStructure);
}

static void ADC_Mode_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	ADC_APBxClock(ADC_CLK, ENABLE);
	/* ADCģʽ���� */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; // ����ģʽ��ֻ�õ���һ��ADCͨ��
	ADC_InitStructure.ADC_ScanConvMode = DISABLE; // ��ʹ��ɨ��ģʽ����ͨ����ʹ��
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; // ����ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // ��ֹ�ⲿ������ʹ���������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; // �����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1; // ��ͨ��ģʽ
	ADC_Init(ADCx, &ADC_InitStructure);
	/* ADCʱ������ */
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); // ����ADCʱ��ΪPLCK2��8��Ƶ����72/8=9MHz
	ADC_RegularChannelConfig(ADCx, ADC_CHANNEL, 1, ADC_SampleTime_55Cycles5); // ����ADCͨ��������ʱ��Ϊ55.5��ʱ������
	/* ADC�ж����� */
	ADC_ITConfig(ADCx, ADC_IT_EOC, ENABLE); // ת���������ʱ�����жϣ��ڷ������м���ת��ֵ
	ADC_Cmd(ADCx, ENABLE); // ʹ��ADC
	ADC_ResetCalibration(ADCx); // ��λУ׼�Ĵ���
	while(ADC_GetResetCalibrationStatus(ADCx)); // �ȴ�У׼���
	ADC_StartCalibration(ADCx); // ��ʼУ׼
	while(ADC_GetCalibrationStatus(ADCx)); // �ȴ�У׼���
	ADC_SoftwareStartConvCmd(ADCx, ENABLE); // �������ת��
}

void ADCx_Init(void)
{
	ADC_NVIC_Init();
	ADC_GPIO_Init();
	ADC_Mode_Init();
}
