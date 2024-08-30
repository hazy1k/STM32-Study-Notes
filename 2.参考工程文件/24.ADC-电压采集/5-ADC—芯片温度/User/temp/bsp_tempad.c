//�¶ȴ��������ڲ���ADC1_IN16����ͨ�������ӣ�
#include "./temp/bsp_tempad.h"

//ADC1_BASE��ַ��0x4000 0000+0x2400 ��ADC1�Ļ���ַ�������Ĺ������ݼĴ�����ƫ�Ƶ�ַ�ǣ�0x4c
#define ADC1_DR_Address    ((u32)ADC1_BASE+0x4c)

__IO u16 ADC_ConvertedValue;

// ADC1 GPIO����
static void ADC1_GPIO_Config(void)		       
{
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);   // DMA1ʱ��ʹ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); // ADC1ʱ��ʹ��

}

//����ADC1�Ĺ���ģʽΪDMAģʽ
static void ADC1_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;

  // DMA1ͨ��1����
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;		             // �������ַ
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;	         // ADת��ֵ����ŵ��ڴ����ַ	�����Ǹ�����ַ��
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                         // ������Ϊ���ݴ������Դ	
  DMA_InitStructure.DMA_BufferSize = 1;                                      // ����ָ��DMAͨ�� DMA����Ĵ�С
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;           // �����ַ�Ĵ�������
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;			             // �ڴ��ַ�Ĵ�������
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;// ���ݿ��Ϊ16λ
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;        // ���ݿ��Ϊ16λ
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;		                         // ������ѭ��ģʽ��
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;	                       // �����ȼ�
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;		                           // û������Ϊ�ڴ浽�ڴ�Ĵ���
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);                               // DMA1ͨ��1��ʼ��
  DMA_Cmd(DMA1_Channel1, ENABLE);											                       // ʹ��DMA1ͨ��1

  // ADC1����
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                  // ��������ģʽ
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;			                  // ��ͨ��
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;			            // ����ת��
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // �������������
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;					    // �����Ҷ���
  ADC_InitStructure.ADC_NbrOfChannel = 1;				                      // ��һ��ͨ��ת��
  ADC_Init(ADC1, &ADC_InitStructure);                                 // ADC1��ʼ��
	
  /*����ADCʱ�ӣ�ΪPCLK2��8��Ƶ����9Hz*/
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 
 
  //���ò���ͨ��IN16, ���ò���ʱ��
  ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_239Cycles5);	

  //ʹ���¶ȴ��������ڲ��ο���ѹ   
  ADC_TempSensorVrefintCmd(ENABLE);                                    
	  
  ADC_DMACmd(ADC1, ENABLE); // ʹ��DMAģʽ
  ADC_Cmd(ADC1, ENABLE); // ʹ��ADC1   
  ADC_ResetCalibration(ADC1); // ��λУ׼�Ĵ���			                              
  while(ADC_GetResetCalibrationStatus(ADC1));  // �ȴ�У׼����                        
  ADC_StartCalibration(ADC1); // ��ʼУ׼				                        
  while(ADC_GetCalibrationStatus(ADC1)); // �ȴ�У׼����		  
  ADC_SoftwareStartConvCmd(ADC1, ENABLE); // ����ADCת��
}												

void Temp_ADC1_Init(void)
{
	ADC1_GPIO_Config();
	ADC1_Mode_Config();
}
