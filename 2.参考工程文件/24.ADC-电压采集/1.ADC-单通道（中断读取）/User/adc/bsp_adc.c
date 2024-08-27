#include "bsp_adc.h"

__IO uint16_t ADC_ConvertedValue;

// ADC GPIO ��ʼ��
static void ADCx_GPIO_Config(void)
{
	// 1.����һ��GPIO_InitTypeDef�ṹ�����
	GPIO_InitTypeDef GPIO_InitStructure;
	// 2.�� ADC IO�˿�ʱ��
	ADC_GPIO_APBxClock_FUN(ADC_GPIO_CLK, ENABLE);
	
	// 3.���� ADC IO ����ģʽ
	// ����Ϊģ������
	GPIO_InitStructure.GPIO_Pin = ADC_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	
	// 4.��ʼ�� ADC IO
	GPIO_Init(ADC_PORT, &GPIO_InitStructure);				
}

// ����ADCģʽ
static void ADCx_Mode_Config(void)
{
	ADC_InitTypeDef ADC_InitStructure;	

	// ��ADCʱ��
	ADC_APBxClock_FUN(ADC_CLK, ENABLE );
	// ADC ģʽ����
	// ֻʹ��һ��ADC�����ڶ���ģʽ
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	// ��ֹɨ��ģʽ����ͨ����Ҫ����ͨ������Ҫ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE ; 
	// ����ת��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	// �����ⲿ����ת���������������
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	// ת������Ҷ���
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	// ת��ͨ��1��
	ADC_InitStructure.ADC_NbrOfChannel = 1;	
	// ��ʼ��ADC
	ADC_Init(ADCx, &ADC_InitStructure);
	// ����ADCʱ��ΪPCLK2��8��Ƶ����9MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 
	// ���� ADC ͨ��ת��˳��Ͳ���ʱ��
	ADC_RegularChannelConfig(ADCx, ADC_CHANNEL, 1, ADC_SampleTime_55Cycles5); // ����������ADCx��ͨ���ţ�ͨ���飬����ʱ��
	// ADC ת�����������жϣ����жϷ�������ж�ȡת��ֵ
	ADC_ITConfig(ADCx, ADC_IT_EOC, ENABLE);
	// ����ADC ������ʼת��
	ADC_Cmd(ADCx, ENABLE);
	// ��ʼ��ADC У׼�Ĵ���  
	ADC_ResetCalibration(ADCx);
	// �ȴ�У׼�Ĵ�����ʼ�����
	while(ADC_GetResetCalibrationStatus(ADCx));
	// ADC��ʼУ׼
	ADC_StartCalibration(ADCx);
	// �ȴ�У׼���
	while(ADC_GetCalibrationStatus(ADCx));
	// ����û�в����ⲿ����������ʹ���������ADCת�� 
	ADC_SoftwareStartConvCmd(ADCx, ENABLE);
}

// ����ADC�ж�
static void ADC_NVIC_Config(void)
{
  	NVIC_InitTypeDef NVIC_InitStructure;
	// ���ȼ�����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  	// �����ж����ȼ�
  	NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQ;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
}


/**
  * @brief  ADC��ʼ��
  * @param  ��
  * @retval ��
  */
void ADCx_Init(void)
{
	ADCx_GPIO_Config();
	ADCx_Mode_Config();
	ADC_NVIC_Config();
}
/*********************************************END OF FILE**********************/
