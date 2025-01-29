# 第41章 ADC-单通道（DMA读取）

## 1. 硬件设计

和单通道(中断读取)的实验差不多，差别在于ADC转换结果数据使用DMA方式传输到指定的存储区，取代单通道使用中断的读取方法

## 2. 软件分析

- ADC GOIO及通道宏定义

```c
/********************ADC1输入通道（引脚）配置**************************/
#define ADCx                   ADC1
#define ADC_APBxClock_FUN      RCC_APB2PeriphClockCmd
#define ADC_CLK                RCC_APB2Periph_ADC1
#define ADC_GPIO_APBxClock_FUN RCC_APB2PeriphClockCmd
#define ADC_GPIO_CLK           RCC_APB2Periph_GPIOC  
#define ADC_PORT               GPIOC
// PC1-通道11 独立IO
#define ADC_PIN                GPIO_Pin_1
#define ADC_CHANNEL            ADC_Channel_11
#define ADC_DMA_CHANNEL        DMA1_Channel1
```

- ADC GPIO初始化

```c
// ADC GPIO 初始化
static void ADCx_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    // 打开 ADC IO端口时钟
    ADC_GPIO_APBxClock_FUN(ADC_GPIO_CLK, ENABLE );
    // 配置 ADC IO 引脚模式
    GPIO_InitStructure.GPIO_Pin = ADC_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; // 模拟输入模式
    // 初始化 ADC IO
    GPIO_Init(ADC_PORT, &GPIO_InitStructure);                
}
```

- 配置DMA工作

```c
// 打开DMA时钟
RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
// 复位DMA控制器
DMA_DeInit(ADC_DMA_CHANNEL);
// 配置 DMA 初始化结构体
// 外设基址为：ADC 数据寄存器地址
DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(ADCx->DR));
// 存储器地址，实际上就是一个内部SRAM的变量
DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue;
// 数据源来自外设
DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
// 缓冲区大小为1，缓冲区的大小应该等于存储器的大小
DMA_InitStructure.DMA_BufferSize = 1;
// 外设寄存器只有一个，地址不用递增
DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
// 存储器地址固定
DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable; 
// 外设数据大小为半字，即两个字节
DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
// 存储器数据大小也为半字，跟外设数据大小相同
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
DMA_Cmd(ADC_DMA_CHANNEL, ENABLE);
```

- ADC模式配置

```c
// ADC 模式配置
// 只使用一个ADC，属于单模式
ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
// 禁止扫描模式，多通道才要，单通道不需要
ADC_InitStructure.ADC_ScanConvMode = DISABLE; 
// 连续转换模式
ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
// 不用外部触发转换，软件开启即可
ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
// 转换结果右对齐
ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
// 转换通道1个
ADC_InitStructure.ADC_NbrOfChannel = 1;    
// 初始化ADC
ADC_Init(ADCx, &ADC_InitStructure);
// 配置ADC时钟为PCLK2的8分频，即9MHz
RCC_ADCCLKConfig(RCC_PCLK2_Div8); 
// 配置 ADC 通道转换顺序为1，第一个转换，采样时间为55.5个时钟周期
ADC_RegularChannelConfig(ADCx, ADC_CHANNEL, 1, ADC_SampleTime_55Cycles5);
// 使能ADC DMA 请求
ADC_DMACmd(ADCx, ENABLE);
// 开启ADC ，并开始转换
ADC_Cmd(ADCx, ENABLE);
// 初始化ADC 校准寄存器  
ADC_ResetCalibration(ADCx);
// 等待校准寄存器初始化完成
while(ADC_GetResetCalibrationStatus(ADCx));
// ADC开始校准
ADC_StartCalibration(ADCx);
// 等待校准完成
while(ADC_GetCalibrationStatus(ADCx));
// 由于没有采用外部触发，所以使用软件触发ADC转换 
ADC_SoftwareStartConvCmd(ADCx, ENABLE);
```

- 主函数

```c
int main(void)
{    
    // 配置串口
    USART_Config();    
    // ADC 初始化
    ADCx_Init();
    printf("\r\n ----这是一个ADC单通道DMA读取实验----\r\n");
    while (1)
    {
        ADC_ConvertedValueLocal =(float) ADC_ConvertedValue/4096*3.3; // 读取转换的AD值
        printf("\r\n The current AD value = 0x%04X \r\n", ADC_ConvertedValue); 
        printf("\r\n The current AD value = %f V \r\n",ADC_ConvertedValueLocal); 
        Delay(0xffffee);  
    }
}
```

## 3. 小结

这一章主要就增加了一个DMA传输嘛，DMA我们前面就已经学习过了，所以还算比较简单的

### 实验目标

使用 STM32 标准库通过 DMA 读取电位器的电压值。

### 硬件需求

与之前相同：

1. STM32 开发板（如 STM32F103）
2. 电位器
3. 连接线
4. 电源

### 硬件连接

- 电位器的一个引脚接到 VCC（如 3.3V），另一个引脚接到 GND，中间引脚接到 ADC 输入通道（如 PA0）。

### 软件设计

1. **项目设置**
   
   - 在 STM32 标准库中创建新项目，配置 ADC 模块和 DMA。

2. **代码实现**

#### 主程序代码

```c
#include "stm32f10x.h"

#define ADC_BUFFER_SIZE  10
volatile uint16_t adc_buffer[ADC_BUFFER_SIZE];

void ADC1_Init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;  // PA0
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;  // 模拟输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;  // 开启连续转换
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_Cmd(ADC1, ENABLE);  // 启动 ADC

    // DMA 初始化
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  // 使能 DMA 时钟

    DMA_InitTypeDef DMA_InitStructure;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)adc_buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  // 从外设到内存
    DMA_InitStructure.DMA_BufferSize = ADC_BUFFER_SIZE;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  // 循环模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    DMA_Cmd(DMA1_Channel1, ENABLE);  // 启动 DMA

    // 启动 ADC
    ADC_DMACmd(ADC1, ENABLE);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);  // 启动软件转换
}

int main(void) {
    SystemInit();
    ADC1_Init();
    
    while (1) {
        // 在这里可以处理 adc_buffer 中的数据
        for (int i = 0; i < ADC_BUFFER_SIZE; i++) {
            // 处理 adc_buffer[i]
        }
        // 添加适当的延时
    }
}
```

### 实验步骤

1. 在 STM32 标准库中创建项目，并配置 ADC1 和 DMA。
2. 将代码复制到主程序文件中。
3. 连接电位器，启动程序，观察 `adc_buffer` 中的数据变化。

---

2024.9.20 第一次修订，后期不再维护
