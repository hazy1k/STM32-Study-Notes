# 第四十三章 ADC-双ADC（同步规则）

AD转换包括采样阶段和转换阶段，在采样阶段才对通道数据进行采集；而在转换阶段只是将采集到的数据进行转换为数字量输出，此刻通道数据变化不会改变转换结果。

独立模式的ADC采集需要在一个通道采集并且转换完成后才会进行下一个通道的采集。而双重ADC的机制就是使用两个ADC同时采样一个或者多个通道。 双重ADC模式较独立模式一个最大的优势就是提高了采样率，弥补了单个ADC采样不够快的缺点。

启用双ADC模式的时候，通过配置ADC_CR1寄存器的DUALMOD[3:0]位，可以有几种不同的模式， 具体见表格：

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/ADC01.png)

这里我们选取同步规则模式来作为实验讲解。同步规则模式是ADC1和ADC2同时转换一个规则通道组，ADC1是主，ADC2是从， ADC1转换的结果放在ADC1_DR的低16位，ADC2转换的结果放在ADC1_DR的高十六位。并且必须开启DMA功能。

外部触发来自ADC1的规则组多路开关(由ADC1_CR2寄存器的EXTSEL[2:0]选择)， 它同时给ADC2提供同步触发。 为了简单起见，ADC1跟ADC2我们选择软件触发。

为了实验的简单起见，实验中我们选取ADC1和ADC2各采集一个通道 。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/ADC007.png)

## 1. 硬件设计

开发板用引出了6路ADC，实验中选取其中的两路（根据软件配置来选择）接开发板中的电位器/GND/3V3来获取模拟信号即可做实验。

## 2. 软件设计

### 2.1 编程目标

1) 初始化ADC GPIO；

2) 初始化DMA配置；

3) 初始化ADC参数；

4) 读取ADC采集的数据，并打印出来校正；

### 2.2 代码分析

- ADC宏定义

```c
// 双模式时，ADC1和ADC2转换的数据都存放在ADC1的数据寄存器，
// ADC1的在低十六位，ADC2的在高十六位

// 双ADC模式的第一个ADC，必须是ADC1
#define ADCx_1                    ADC1
#define ADCx_1_APBxClock_FUN      RCC_APB2PeriphClockCmd
#define ADCx_1_CLK                RCC_APB2Periph_ADC1
#define ADCx_1_GPIO_APBxClock_FUN RCC_APB2PeriphClockCmd
#define ADCx_1_GPIO_CLK           RCC_APB2Periph_GPIOC  
#define ADCx_1_PORT               GPIOC
#define ADCx_1_PIN                GPIO_Pin_1
#define ADCx_1_CHANNEL            ADC_Channel_11

// 双ADC模式的第二个ADC，必须是ADC2
#define ADCx_2                    ADC2
#define ADCx_2_APBxClock_FUN      RCC_APB2PeriphClockCmd
#define ADCx_2_CLK                RCC_APB2Periph_ADC2
#define ADCx_2_GPIO_APBxClock_FUN RCC_APB2PeriphClockCmd
#define ADCx_2_GPIO_CLK           RCC_APB2Periph_GPIOC  
#define ADCx_2_PORT               GPIOC
#define ADCx_2_PIN                GPIO_Pin_4
#define ADCx_2_CHANNEL            ADC_Channel_14

// ADC通道
#define NOFCHANEL                 1
// ADC1 对应 DMA1通道1，ADC3对应DMA2通道5，ADC2没有DMA功能
#define ADC_DMA_CHANNEL           DMA1_Channel1
```

ADC1和ADC2的通道由宏来定义，当硬件环境改变时，方便修改。

- ADC GPIO初始化函数

```c
static void ADCx_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    // ADCx_1 GPIO 初始化
    ADCx_1_GPIO_APBxClock_FUN(ADCx_1_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = ADCx_1_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; // 模拟输入模式
    GPIO_Init(ADCx_1_PORT, &GPIO_InitStructure);
    // ADCx_2 GPIO 初始化
    ADCx_1_GPIO_APBxClock_FUN(ADCx_2_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = ADCx_2_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(ADCx_2_PORT, &GPIO_InitStructure);
}
```

使用到GPIO时候都必须开启对应的GPIO时钟，GPIO用于AD转换功能必须配置为模拟输入模式。

- 配置双重ADC规则同步模式

```c
static void ADCx_Mode_Config(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;

    // 打开DMA时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    // 打开ADC时钟
    ADCx_1_APBxClock_FUN ( ADCx_1_CLK, ENABLE );
    ADCx_2_APBxClock_FUN ( ADCx_2_CLK, ENABLE );

    /* ------------------DMA模式配置---------------- */
    // 复位DMA控制器
    DMA_DeInit(ADC_DMA_CHANNEL);
    // 配置 DMA 初始化结构体
    // 外设基址为：ADC 数据寄存器地址
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(ADCx_1->DR));	
    // 存储器地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_ConvertedValue;
    // 数据源来自外设
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    // 缓冲区大小，应该等于数据目的地的大小
    DMA_InitStructure.DMA_BufferSize = NOFCHANEL;
    // 外设寄存器只有一个，地址不用递增
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    // 存储器地址递增
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    // 外设数据大小
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    // 内存数据大小，跟外设数据大小相同
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
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
    /* ----------------ADCx_1 模式配置--------------------- */
    // 双ADC的规则同步
    ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;
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
    ADC_Init(ADCx_1, &ADC_InitStructure);
    // 配置ADC时钟为PCLK2的8分频，即9MHz
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);
    // 配置ADC 通道的转换顺序和采样时间
    ADC_RegularChannelConfig(ADCx_1, ADCx_1_CHANNEL, 1, ADC_SampleTime_239Cycles5);
    // 使能ADC DMA 请求
    ADC_DMACmd(ADCx_1, ENABLE);
    /* ----------------ADCx_2 模式配置--------------------- */
    // 双ADC的规则同步
    ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;
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
    ADC_Init(ADCx_2, &ADC_InitStructure);
    // 配置ADC时钟Ｎ狿CLK2的8分频，即9MHz
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);
    // 配置ADC 通道的转换顺序和采样时间
    ADC_RegularChannelConfig(ADCx_2, ADCx_2_CHANNEL, 1, ADC_SampleTime_239Cycles5);
    // 使能ADCx_2的外部触发转换
    ADC_ExternalTrigConvCmd(ADC2, ENABLE);
    /* ----------------ADCx_1 校准--------------------- */
    // 开启ADC ，并开始转换
    ADC_Cmd(ADCx_1, ENABLE);
    // 初始化ADC 校准寄存器
    ADC_ResetCalibration(ADCx_1);
    // 等待校准寄存器初始化完成
    while(ADC_GetResetCalibrationStatus(ADCx_1));
    // ADC开始校准
    ADC_StartCalibration(ADCx_1);
    // 等待校准完成
    while(ADC_GetCalibrationStatus(ADCx_1));
    /* ----------------ADCx_2 校准--------------------- */
    // 开启ADC ，并开始转换
    ADC_Cmd(ADCx_2, ENABLE);
    // 初始化ADC 校准寄存器
    ADC_ResetCalibration(ADCx_2);
    // 等待校准寄存器初始化完成
    while (ADC_GetResetCalibrationStatus(ADCx_2));
    // ADC开始校准
    ADC_StartCalibration(ADCx_2);
    // 等待校准完成
    while(ADC_GetCalibrationStatus(ADCx_2));
    // 由于没有采用外部触发，所以使用软件触发ADC转换
    ADC_SoftwareStartConvCmd(ADCx_1, ENABLE);
}
```

ADCx_Mode_Config()与独立模式多通道配置基本一样，只是有几点需要注意：

ADC工作模式要设置为同步规则模式；两个ADC的通道的采样时间需要一致；ADC1设置为软件触发； ADC2设置为外部触发。其他的基本一样，看代码注释理解即可。

- 主函数

```c
int main(void)
{        
    uint16_t temp0=0 ,temp1=0;
    // 配置串口
    USART_Config();
    // ADC 初始化
    ADCx_Init();
    printf("\r\n ----这是一个双ADC规则同步采集实验----\r\n");
    while (1)
    {    
        // 取出ADC1数据寄存器的高16位，这个是ADC2的转换数据
        temp0 = (ADC_ConvertedValue[0]&0XFFFF0000) >> 16;
        // 取出ADC1数据寄存器的低16位，这个是ADC1的转换数据
        temp1 = (ADC_ConvertedValue[0]&0XFFFF);    

        ADC_ConvertedValueLocal[0] =(float) temp0/4096*3.3;
        ADC_ConvertedValueLocal[1] =(float) temp1/4096*3.3;

        printf("\r\n ADCx_1 value = %f V \r\n",
                ADC_ConvertedValueLocal[1]);
        printf("\r\n ADCx_2 value = %f V \r\n",
                ADC_ConvertedValueLocal[0]);

        printf("\r\n\r\n");
        Delay(0xffffee);          
    }
}
```

配置好串口，初始化好ADC，然后把ADC1和ADC2采集的数据分离出来，最后调用printf函数打印到电脑的串口调试助手。

## 3. 小结

相比单通道，双ADC采用同步规则，提高采集精度，ADC1软件触发，ADC2外部触发

1. **初始化ADC**：
   配置两个ADC（ADC1和ADC2），设置为同步工作模式。

2. **设置规则通道**：
   配置两个ADC的规则通道，使其共享相同的输入通道。

3. **配置DMA**：
   使用DMA将两个ADC的采样结果同时存储，以提高效率。

```c
void ADC_Dual_Init(void) {
    // 省略时钟和GPIO初始化部分

    ADC_InitTypeDef ADC_InitStruct;
    
    // 初始化ADC1
    ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStruct.ADC_ScanConvMode = ENABLE;
    ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStruct.ADC_NbrOfChannel = 2; // 两个通道
    ADC_Init(ADC1, &ADC_InitStruct);

    // 初始化ADC2
    ADC_Init(ADC2, &ADC_InitStruct);

    // 配置ADC通道
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_1_5Cycles);
    ADC_RegularChannelConfig(ADC2, ADC_Channel_1, 1, ADC_SampleTime_1_5Cycles);

    // 启用ADC
    ADC_Cmd(ADC1, ENABLE);
    ADC_Cmd(ADC2, ENABLE);
    
    // 启动ADC转换
    ADC_StartOfConversion(ADC1);
    ADC_StartOfConversion(ADC2);
}

// 在主循环中读取数据
void Read_ADC_Dual(void) {
    if (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) != RESET) {
        uint16_t value1 = ADC_GetConversionValue(ADC1);
        uint16_t value2 = ADC_GetConversionValue(ADC2);
        // 处理数据
    }
}
```

---

2024.9.22 第一次修订，后期不再维护


