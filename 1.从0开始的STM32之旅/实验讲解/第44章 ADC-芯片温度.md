# 第四十四章 ADC-芯片温度

## 1. 硬件设计

使用ADC的内部温度通道获取芯片温度

通过ADC1的通道16获取芯片内部的温度，把获取到的温度通过串口打印到电脑的串口调试助手显示。

均使用芯片资源，无需其他硬件设计

## 2. 软件设计

- 温度参数宏定义

```c
//对于12位的ADC，3.3V的ADC值为0xfff,温度为25度时对应的电压值为1.43V即0x6EE
#define V25  0x6EE	 

//斜率 每摄氏度4.3mV 对应每摄氏度0x05
#define AVG_SLOPE 0x05 
```

- ADC1 GPIO配置

```c
// ADC1 GPIO配置
static void ADC1_GPIO_Config(void)		       
{
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);   // DMA1时钟使能
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); // ADC1时钟使能

}
```

- 配置ADC1的工作模式为DMA模式

```c
//配置ADC1的工作模式为DMA模式
static void ADC1_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;

  // DMA1通道1配置
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;		             // 外设基地址
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;	         // AD转换值所存放的内存基地址	（就是给个地址）
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                         // 外设作为数据传输的来源	
  DMA_InitStructure.DMA_BufferSize = 1;                                      // 定义指定DMA通道 DMA缓存的大小
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;           // 外设地址寄存器不变
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;			             // 内存地址寄存器不变
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;// 数据宽度为16位
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;        // 数据宽度为16位
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;		                         // 工作在循环模式下
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;	                       // 高优先级
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;		                           // 没有设置为内存到内存的传输
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);                               // DMA1通道1初始化
  DMA_Cmd(DMA1_Channel1, ENABLE);											                       // 使能DMA1通道1

  // ADC1配置
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                  // 独立工作模式
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;			                  // 多通道
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;			            // 连续转换
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 由软件触发启动
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;					    // 数据右对齐
  ADC_InitStructure.ADC_NbrOfChannel = 1;				                      // 仅一个通道转换
  ADC_Init(ADC1, &ADC_InitStructure);                                 // ADC1初始化
	
  /*配置ADC时钟，为PCLK2的8分频，即9Hz*/
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 
 
  //设置采样通道IN16, 设置采样时间
  ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_239Cycles5);	

  //使能温度传感器和内部参考电压   
  ADC_TempSensorVrefintCmd(ENABLE);                                    
	  
  ADC_DMACmd(ADC1, ENABLE); // 使能DMA模式
  ADC_Cmd(ADC1, ENABLE); // 使能ADC1   
  ADC_ResetCalibration(ADC1); // 复位校准寄存器			                              
  while(ADC_GetResetCalibrationStatus(ADC1));  // 等待校准结束                        
  ADC_StartCalibration(ADC1); // 开始校准				                        
  while(ADC_GetCalibrationStatus(ADC1)); // 等待校准结束		  
  ADC_SoftwareStartConvCmd(ADC1, ENABLE); // 启动ADC转换
}												
```

- 主函数

```c

```
