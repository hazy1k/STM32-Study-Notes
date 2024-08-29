# 第四十二章 ADC-多通道（DMA读取）

## 1. 硬件设计

开发板已通过排针接口把部分ADC通道引脚引出，其中电位器通过跳线帽默认接了一个ADC的IO， 其他的ADC IO 在做实验的时候可以用杜邦线连接到开发板中的GND或者3V3来获取模拟信号。要注意的是， 用来做ADC输入的IO不能被复用，否则会导致采集到的信号不准确。

## 2. 软件设计

### 2.1 编程目标

1) 初始化ADC GPIO；

2) 初始化ADC工作参数；

3) 配置DMA工作参数；

4) 读取ADC采集的数据；

ADC转换结果数据使用DMA方式传输至指定的存储区，这样取代单通道实验使用中断服务的读取方法。 实际上，多通道ADC采集一般使用DMA数据传输方式更加高效方便。

### 2.2 代码分析

- ADC宏定义

```c
// 注意：用作ADC采集的IO必须没有复用，否则采集电压会有影响
/********************ADC1输入通道（引脚）配置**************************/
#define    ADC_APBxClock_FUN             RCC_APB2PeriphClockCmd
#define    ADC_CLK                       RCC_APB2Periph_ADC1

#define    ADC_GPIO_APBxClock_FUN        RCC_APB2PeriphClockCmd
#define    ADC_GPIO_CLK                  RCC_APB2Periph_GPIOC  
#define    ADC_PORT                      GPIOC

// 注意
// 1-PC0 在霸道里面接的是蜂鸣器，默认被拉低
// 2-PC0 在指南者里面接的是SPI FLASH的 片选，默认被拉高
// 所以 PC0 做 ADC 转换通道的时候，结果可能会有误差

// 转换通道个数
#define    NOFCHANEL					6

#define    ADC_PIN1                     GPIO_Pin_0
#define    ADC_CHANNEL1                 ADC_Channel_10

#define    ADC_PIN2                     GPIO_Pin_1
#define    ADC_CHANNEL2                 ADC_Channel_11

#define    ADC_PIN3                     GPIO_Pin_2
#define    ADC_CHANNEL3                 ADC_Channel_12

#define    ADC_PIN4                     GPIO_Pin_3
#define    ADC_CHANNEL4                 ADC_Channel_13

#define    ADC_PIN5                     GPIO_Pin_4
#define    ADC_CHANNEL5                 ADC_Channel_14

#define    ADC_PIN6                     GPIO_Pin_5
#define    ADC_CHANNEL6                 ADC_Channel_15

// ADC1 对应 DMA1通道1，ADC3对应DMA2通道5，ADC2没有DMA功能
#define    ADC_x                         ADC1
#define    ADC_DMA_CHANNEL               DMA1_Channel1
#define    ADC_DMA_CLK                   RCC_AHBPeriph_DMA1
```

定义NOFCHANEL个通道进行多通道ADC实验，并且定义DMA相关配置。

- GPIO初始化函数

```c
// ADC GPIO 初始化
static void ADCx_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	// 打开 ADC IO端口时钟
	ADC_GPIO_APBxClock_FUN(ADC_GPIO_CLK, ENABLE );
	// 配置 ADC IO 引脚模式
	GPIO_InitStructure.GPIO_Pin = ADC_PIN1|	ADC_PIN2|ADC_PIN3|ADC_PIN4|ADC_PIN5|ADC_PIN6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; // 模拟输入模式
	// 初始化 ADC IO
	GPIO_Init(ADC_PORT, &GPIO_InitStructure);				
}
```

使用到GPIO时候都必须开启对应的GPIO时钟，GPIO用于AD转换功能必须配置为模拟输入模式。

- 配置ADC工作模式

```c
// 配置ADC模式
static void ADCx_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	// 打开DMA时钟
	RCC_AHBPeriphClockCmd(ADC_DMA_CLK, ENABLE);
	// 打开ADC时钟
	ADC_APBxClock_FUN ( ADC_CLK, ENABLE );
	
	// 复位DMA控制器
	DMA_DeInit(ADC_DMA_CHANNEL);
	
	// 配置 DMA 初始化结构体
	// 外设基址为：ADC 数据寄存器地址
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(ADC_x->DR));
	// 存储器地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ADC_ConvertedValue;
	// 数据源来自外设
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	// 缓冲区大小，应该等于数据目的地的大小
	DMA_InitStructure.DMA_BufferSize = NOFCHANEL;
	// 外设寄存器只有一个，地址不用递增
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	// 存储器地址递增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
	// 外设数据大小为半字，即两个字节
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	// 内存数据大小也为半字，跟外设数据大小相同
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	// 循环传输模式
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	
	// DMA 传输通道优先级为高，当使用一个DMA通道时，优先级设置不影响
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	// 禁止存储器到存储器模式，因为是从外设到存储器
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	
	// 初始化DMA
	DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStructure);
	// 使能 DMA 通道
	DMA_Cmd(ADC_DMA_CHANNEL , ENABLE);

	// ADC 模式配置
	// 只使用一个ADC，属于单模式
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	// 扫描模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE ; 
	// 连续转换模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	// 不用外部触发转换，软件开启即可
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	// 转换结果右对齐
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	// 转换通道个数
	ADC_InitStructure.ADC_NbrOfChannel = NOFCHANEL;	
	// 初始化ADC
	ADC_Init(ADC_x, &ADC_InitStructure);
	// 配置ADC时钟Ｎ狿CLK2的8分频，即9MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 
	
	// 配置ADC 通道的转换顺序和采样时间
	ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL1, 1, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL2, 2, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL3, 3, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL4, 4, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL5, 5, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL6, 6, ADC_SampleTime_55Cycles5);
	
	// 使能ADC DMA 请求
	ADC_DMACmd(ADC_x, ENABLE);
	// 开启ADC ，并开始转换
	ADC_Cmd(ADC_x, ENABLE);
	// 初始化ADC 校准寄存器  
	ADC_ResetCalibration(ADC_x);
	// 等待校准寄存器初始化完成
	while(ADC_GetResetCalibrationStatus(ADC_x));
	// ADC开始校准
	ADC_StartCalibration(ADC_x);
	// 等待校准完成
	while(ADC_GetCalibrationStatus(ADC_x));
	// 由于没有采用外部触发，所以使用软件触发ADC转换 
	ADC_SoftwareStartConvCmd(ADC_x, ENABLE);
}
```

ADCx_Mode_Config函数主要做了两个工作，一个是配置ADC的工作参数，另外一个是配置DMA的工作参数。

ADC的工作参数具体如下：打开ADC外设时钟；因为只是使用一个ADC，所有模式配置为独立模式；多通道采集，开启扫描模式； 需要不断的采集外部的模拟数据，所有使能连续转换模式；不使用外部触发转换信号；转换结果右对齐；设置需要转换的通道的个数， 最后调用ADC_Init()函数把这些参数写入ADC的寄存器完成配置。因为是多通道采集， 所以调用ADC_RegularChannelConfig()函数设置每个通道的转换顺序和采样实际。

DMA的工作参数具体如下：我们是把ADC采集到的数据通过DMA传输到存储器上，则外设地址为ADC的数据寄存器；存储器的地址是我们定义的用来存放ADC数据的数组的地址； 传输方向为外设到存储器；缓冲区大小等于等于我们定义的存储ADC数据的数组大小；所有通道转换的数据都放在一个数据寄存器中，则外设地址不变； 采集存储的数据有多个，则存储器地址递增；外设和存储器单位均为两个字节；开启循环传输模式；只有一个DMA通道工作，优先级随便设置；禁用存储器到存储器模式， 最后最后调用DMA_Init()函数把这些参数写入DMA的寄存器完成配置。

完成配置之后则使能ADC和DMA，开启软件触发，让ADC开始采集数据。

- 主函数

```c
int main(void)
{
    // 配置串口
    USART_Config();

    // ADC 初始化
    ADCx_Init();

    printf("\r\n ----这是一个ADC多通道采集实验----\r\n");

    while (1)
    {

        ADC_ConvertedValueLocal[0] =(float)
                                    ADC_ConvertedValue[0]/4096*3.3;
        ADC_ConvertedValueLocal[1] =(float)
                                    ADC_ConvertedValue[1]/4096*3.3;
        ADC_ConvertedValueLocal[2] =(float)
                                    ADC_ConvertedValue[2]/4096*3.3;
        ADC_ConvertedValueLocal[3] =(float)
                                    ADC_ConvertedValue[3]/4096*3.3;
        ADC_ConvertedValueLocal[4] =(float)
                                    ADC_ConvertedValue[4]/4096*3.3;

        printf("\r\n CH1 value = %f V \r\n",ADC_ConvertedValueLocal[0]);
        printf("\r\n CH2 value = %f V \r\n",ADC_ConvertedValueLocal[1]);
        printf("\r\n CH3 value = %f V \r\n",ADC_ConvertedValueLocal[2]);
        printf("\r\n CH2 value = %f V \r\n",ADC_ConvertedValueLocal[3]);
        printf("\r\n CH3 value = %f V \r\n",ADC_ConvertedValueLocal[4]);

        printf("\r\n\r\n");
        Delay(0xffffee);

    }
}
```

主函数中我们配置好串口，初始化好ADC之后，把采集到的电压经过转换之后通过串口打印到电脑的调试助手显示， 要注意的是在做实验时需要给每个ADC通道提供模拟电源，可以用杜邦线从开发板的GND或者3V3取信号来做实验。


