#include "./adc/bsp_adc.h"
#include ".\motor_control\bsp_motor_control.h"
#include "./led/bsp_led.h" 
#include "./usart/bsp_debug_usart.h"

__IO uint16_t ADC_ConvertedValue;
DMA_HandleTypeDef DMA_Init_Handle;
ADC_HandleTypeDef ADC_Handle;

static int16_t adc_buff[ADC_NUM_MAX];    // ��ѹ�ɼ�������
static int16_t vbus_adc_mean = 0;        // ��Դ��ѹ ACD �������ƽ��ֵ
static uint32_t adc_mean_sum = 0;        // ƽ��ֵ�ۼ�
static uint32_t adc_mean_count = 0;      // �ۼӼ���

/**
  * @brief  ADC ͨ�����ų�ʼ��
  * @param  ��
  * @retval ��
  */
static void ADC_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    // ʹ�� GPIO ʱ��
    CURR_ADC_GPIO_CLK_ENABLE();
    VBUS_GPIO_CLK_ENABLE();
    // ���� IO
    GPIO_InitStructure.Pin = CURR_ADC_GPIO_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;	    
    GPIO_InitStructure.Pull = GPIO_NOPULL ; //������������
    HAL_GPIO_Init(CURR_ADC_GPIO_PORT, &GPIO_InitStructure);	

    GPIO_InitStructure.Pin = VBUS_GPIO_PIN;
    HAL_GPIO_Init(VBUS_GPIO_PORT, &GPIO_InitStructure);	
}

void adc_dma_init(void)
{
    // ------------------DMA Init �ṹ����� ��ʼ��--------------------------
    // ADC1ʹ��DMA1��ͨ��1��������ֲ�̶�����
    // ����DMAʱ��
    CURR_ADC_DMA_CLK_ENABLE();
    // ���ݴ���ͨ��
    DMA_Init_Handle.Instance = CURR_ADC_DMA_STREAM;
    // ���ݴ��䷽��Ϊ���赽�洢��	
    DMA_Init_Handle.Init.Direction = DMA_PERIPH_TO_MEMORY;
    // ����Ĵ���ֻ��һ������ַ���õ���
    DMA_Init_Handle.Init.PeriphInc = DMA_PINC_DISABLE;
    // �洢����ַ�̶�
    DMA_Init_Handle.Init.MemInc = DMA_MINC_ENABLE;
    // �������ݴ�СΪ���֣��������ֽ�
    DMA_Init_Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    //	�洢�����ݴ�СҲΪ���֣����������ݴ�С��ͬ
    DMA_Init_Handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;	
    // ѭ������ģʽ
    DMA_Init_Handle.Init.Mode = DMA_CIRCULAR;
    // DMA ����ͨ�����ȼ�Ϊ�ߣ���ʹ��һ��DMAͨ��ʱ�����ȼ����ò�Ӱ��
    DMA_Init_Handle.Init.Priority = DMA_PRIORITY_HIGH;
    //��ʼ��DMA
    HAL_DMA_Init(&DMA_Init_Handle); 

    __HAL_LINKDMA(&ADC_Handle,DMA_Handle,DMA_Init_Handle);
}

/**
  * @brief  ADC �� DMA ��ʼ��
  * @param  ��
  * @retval ��
  */
static void ADC_Mode_Config(void)
{
    // ����ADCʱ��
    CURR_ADC_CLK_ENABLE();

    // -------------------ADC Init �ṹ�� ���� ��ʼ��------------------------
    // ADC1
    ADC_Handle.Instance = CURR_ADC;
    // ��ֹɨ��ģʽ����ͨ���ɼ�����Ҫ	
    ADC_Handle.Init.ScanConvMode = ENABLE; 
    // ����ת��	
    ADC_Handle.Init.ContinuousConvMode = ENABLE;
    // ������ת��	
    ADC_Handle.Init.DiscontinuousConvMode = DISABLE;
    // ������ת������
    ADC_Handle.Init.NbrOfDiscConversion   = 0;
    //ʹ���������
    ADC_Handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    //���������
    ADC_Handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    //ת��ͨ�� 2��
    ADC_Handle.Init.NbrOfConversion = 2;
    // ��ʼ��ADC	                          
    HAL_ADC_Init(&ADC_Handle);
		
    //---------------------------------------------------------------------------
    ADC_ChannelConfTypeDef ADC_Config;
    
    ADC_Config.Channel      = CURR_ADC_CHANNEL;
    ADC_Config.Rank         = 1;
    // ����ʱ����	
    ADC_Config.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
    // ���� ADC ͨ��ת��˳��Ϊ1����һ��ת��������ʱ��Ϊ3��ʱ������
    HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);
    
    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
    */
    ADC_Config.Channel = VBUS_ADC_CHANNEL;
    ADC_Config.Rank = 2;
    // ����ʱ����	
    ADC_Config.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
    if (HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config) != HAL_OK)
    {
      while(1);
    }
    
    // �����ж����ȼ����ú�ʹ���ж�����
    HAL_NVIC_SetPriority(ADC_DMA_IRQ, 1, 1);
    HAL_NVIC_EnableIRQ(ADC_DMA_IRQ);

    HAL_ADC_Start_DMA(&ADC_Handle, (uint32_t*)&adc_buff, ADC_NUM_MAX);
}

/**
  * @brief  �����ɼ���ʼ��
  * @param  ��
  * @retval ��
  */
void ADC_Init(void)
{
	ADC_GPIO_Config();
  adc_dma_init();
	ADC_Mode_Config();
}

/**
  * @brief  ����ת���ڷ�����ģʽ����ɻص�
  * @param  hadc: ADC  ���.
  * @retval ��
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
  int32_t adc_mean = 0;

  HAL_ADC_Stop_DMA(hadc);       // ֹͣ ADC ������������һ�������ڼ�������
  
  /* �������ͨ��������ƽ��ֵ */
  for(uint32_t count = 0; count < ADC_NUM_MAX; count+=2)
  {
    adc_mean += (int32_t)adc_buff[count];
  }
  
  adc_mean_sum += adc_mean / (ADC_NUM_MAX / 2);    // �ۼӵ�ѹ
  adc_mean_count++;
  
#if 1
  
  adc_mean = 0;
  
  /* �����ѹͨ��������ƽ��ֵ */
  for(uint32_t count = 1; count < ADC_NUM_MAX; count+=2)
  {
    adc_mean += (int32_t)adc_buff[count];
  }
  
  vbus_adc_mean = adc_mean / (ADC_NUM_MAX / 2);    // ����ƽ��ֵ
  
#else
  vbus_adc_mean = adc_buff[1];
#endif
  
  HAL_ADC_Start_DMA(&ADC_Handle, (uint32_t*)&adc_buff, ADC_NUM_MAX);    // ��ʼ ADC ����
}

/**
  * @brief  ��ȡ����ֵ
  * @param  ��
  * @retval ת���õ��ĵ���ֵ
  */
int32_t get_curr_val(void)
{
  static uint8_t flag = 0;
  static uint32_t adc_offset = 0;    // ƫ�õ�ѹ
  int16_t curr_adc_mean = 0;         // ���� ACD �������ƽ��ֵ
  
  curr_adc_mean = adc_mean_sum / adc_mean_count;    // ����ƽ��ֵ
  

    adc_mean_count = 0;
    adc_mean_sum = 0;
    
    if (flag < 17)
    {
      adc_offset = curr_adc_mean;    // ��μ�¼ƫ�õ�ѹ����ϵͳ�ȶ�ƫ�õ�ѹ��Ϊ��Чֵ
      flag += 1;
    }
    if(curr_adc_mean>=adc_offset)
	{
		curr_adc_mean -= adc_offset;                     // ��ȥƫ�õ�ѹ
	}else
	{
		curr_adc_mean=0;
	}


  float vdc = GET_ADC_VDC_VAL(curr_adc_mean);      // ��ȡ��ѹֵ
  
  return GET_ADC_CURR_VAL(vdc);
}

/**
  * @brief  ��ȡ��Դ��ѹֵ
  * @param  ��
  * @retval ת���õ��ĵ���ֵ
  */
float get_vbus_val(void)
{
  float vdc = GET_ADC_VDC_VAL(vbus_adc_mean);      // ��ȡ��ѹֵ
  return GET_VBUS_VAL(vdc);
}

/*********************************** END OF FILE *********************************************/
