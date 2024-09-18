#include "./adc/bsp_adc.h"

__IO uint16_t ADC_ConvertedValue[NOFCHANEL]={0,0,0,0};
DMA_HandleTypeDef hdma_adcx;
ADC_HandleTypeDef ADC_Handle;
ADC_ChannelConfTypeDef ADC_Config;

static void Rheostat_ADC_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RHEOSTAT_ADC_CLK_ENABLE(); 
    // ʹ�� GPIO ʱ��
    RHEOSTAT_ADC_GPIO_CLK_ENABLE();
          
    // ���� IO
    GPIO_InitStructure.Pin =ADC_PIN1|
                            ADC_PIN2|
                            ADC_PIN3|
                            ADC_PIN4|
                            ADC_PIN5|
                            ADC_PIN6;
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;	    
    GPIO_InitStructure.Pull = GPIO_NOPULL ; //������������
    HAL_GPIO_Init(RHEOSTAT_ADC_GPIO_PORT, &GPIO_InitStructure);		
}

static void Rheostat_ADC_Mode_Config(void)
{
    // ------------------DMA Init �ṹ����� ��ʼ��--------------------------
    // ����DMAʱ��
    RHEOSTAT_ADC_DMA_CLK_ENABLE();
    // ���ݴ���ͨ��
     hdma_adcx.Instance = RHEOSTAT_ADC_DMA_STREAM;
  
     hdma_adcx.Init.Direction=DMA_PERIPH_TO_MEMORY;;            //�洢��������
     hdma_adcx.Init.PeriphInc=DMA_PINC_DISABLE;                 //���������ģʽ
     hdma_adcx.Init.MemInc=DMA_MINC_ENABLE;                     //�洢������ģʽ 
     hdma_adcx.Init.PeriphDataAlignment=DMA_PDATAALIGN_HALFWORD;//�������ݳ���:16λ
     hdma_adcx.Init.MemDataAlignment=DMA_MDATAALIGN_HALFWORD;   //�洢�����ݳ���:16λ
     hdma_adcx.Init.Mode= DMA_CIRCULAR;                         //������ͨģʽ
     hdma_adcx.Init.Priority=DMA_PRIORITY_MEDIUM;               //�е����ȼ�

     HAL_DMA_Init(&hdma_adcx); 

    __HAL_LINKDMA( &ADC_Handle,DMA_Handle,hdma_adcx);
  
   //---------------------------------------------------------------------------
    RCC_PeriphCLKInitTypeDef ADC_CLKInit;
    // ����ADCʱ��
    ADC_CLKInit.PeriphClockSelection=RCC_PERIPHCLK_ADC;			//ADC����ʱ��
    ADC_CLKInit.AdcClockSelection=RCC_ADCPCLK2_DIV6;			  //��Ƶ����6ʱ��Ϊ72M/6=12MHz
    HAL_RCCEx_PeriphCLKConfig(&ADC_CLKInit);					      //����ADCʱ��
   
    ADC_Handle.Instance=RHEOSTAT_ADC;
    ADC_Handle.Init.DataAlign=ADC_DATAALIGN_RIGHT;             //�Ҷ���
    ADC_Handle.Init.ScanConvMode=ENABLE;                      //��ɨ��ģʽ
    ADC_Handle.Init.ContinuousConvMode=ENABLE;                 //����ת��
    ADC_Handle.Init.NbrOfConversion=NOFCHANEL	;                         //1��ת���ڹ��������� Ҳ����ֻת����������1 
    ADC_Handle.Init.DiscontinuousConvMode=DISABLE;             //��ֹ����������ģʽ
    ADC_Handle.Init.NbrOfDiscConversion=0;                     //����������ͨ����Ϊ0
    ADC_Handle.Init.ExternalTrigConv=ADC_SOFTWARE_START;       //�������
    HAL_ADC_Init(&ADC_Handle);                                 //��ʼ�� 
 
 //---------------------------------------------------------------------------
    ADC_Config.Channel      = ADC_CHANNEL1;
    ADC_Config.Rank         = 1;
    // ����ʱ����	
    ADC_Config.SamplingTime = ADC_SAMPLETIME_55CYCLES_5 ;
    // ���� ADC ͨ��ת��˳��Ϊ1����һ��ת��������ʱ��Ϊ3��ʱ������
    HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);
    
 //---------------------------------------------------------------------------
    ADC_Config.Channel      = ADC_CHANNEL2;
    ADC_Config.Rank         = 2;
    // ���� ADC ͨ��ת��˳��Ϊ1����һ��ת��������ʱ��Ϊ3��ʱ������
    HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);   
    
  //---------------------------------------------------------------------------
    ADC_Config.Channel      = ADC_CHANNEL3;
    ADC_Config.Rank         = 3;
    // ���� ADC ͨ��ת��˳��Ϊ1����һ��ת��������ʱ��Ϊ3��ʱ������
    HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);   
//---------------------------------------------------------------------------
    ADC_Config.Channel      = ADC_CHANNEL4;
    ADC_Config.Rank         = 4;
    // ���� ADC ͨ��ת��˳��Ϊ1����һ��ת��������ʱ��Ϊ3��ʱ������
    HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);   
//---------------------------------------------------------------------------
    ADC_Config.Channel      = ADC_CHANNEL5;
    ADC_Config.Rank         = 5;
    // ���� ADC ͨ��ת��˳��Ϊ1����һ��ת��������ʱ��Ϊ3��ʱ������
    HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);   
//---------------------------------------------------------------------------
    ADC_Config.Channel      = ADC_CHANNEL6;
    ADC_Config.Rank         = 6;
    // ���� ADC ͨ��ת��˳��Ϊ1����һ��ת��������ʱ��Ϊ3��ʱ������
    HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);       
    
 
    HAL_ADC_Start_DMA(&ADC_Handle, (uint32_t*)&ADC_ConvertedValue, 1);
}



void Rheostat_Init(void)
{
	Rheostat_ADC_GPIO_Config();
	Rheostat_ADC_Mode_Config();
}

