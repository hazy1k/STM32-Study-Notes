# 第四十四章 ADC-芯片温度

## 1. 硬件设计

使用ADC的内部温度通道获取芯片温度

通过ADC1的通道16获取芯片内部的温度，把获取到的温度通过串口打印到电脑的串口调试助手显示。

均使用芯片资源，无需其他硬件设计

## 2. 软件设计

- 温度参数宏定义

```c
// 对于12位的ADC，3.3V的ADC值为0xfff,温度为25度时对应的电压值为1.43V即0x6EE
#define V25  0x6EE     
// 斜率 每摄氏度4.3mV 对应每摄氏度0x05
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
// 配置ADC1的工作模式为DMA模式
static void ADC1_Mode_Config(void)
{
  DMA_InitTypeDef DMA_InitStructure;
  ADC_InitTypeDef ADC_InitStructure;
  // DMA1通道1配置
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;                // 外设基地址
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;           // AD转换值所存放的内存基地址（就是给个地址）
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                         // 外设作为数据传输的来源    
  DMA_InitStructure.DMA_BufferSize = 1;                                      // 定义指定DMA通道 DMA缓存的大小
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;           // 外设地址寄存器不变
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;                   // 内存地址寄存器不变
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;// 数据宽度为16位
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;        // 数据宽度为16位
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                            // 工作在循环模式下
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;                        // 高优先级
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                               // 没有设置为内存到内存的传输
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);                               // DMA1通道1初始化
  DMA_Cmd(DMA1_Channel1, ENABLE);                                            // 使能DMA1通道1
  // ADC1配置
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                 // 独立工作模式
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;                       // 多通道
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                 // 连续转换
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;// 由软件触发启动
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;             // 数据右对齐
  ADC_InitStructure.ADC_NbrOfChannel = 1;                            // 仅一个通道转换
  ADC_Init(ADC1, &ADC_InitStructure);                                // ADC1初始化
  //配置ADC时钟，为PCLK2的8分频，即9Hz
  RCC_ADCCLKConfig(RCC_PCLK2_Div8); 
  //设置采样通道IN16, 设置采样时间
  ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_239Cycles5);    
  //使能温度传感器和内部参考电压   
  ADC_TempSensorVrefintCmd(ENABLE);                                    
  ADC_DMACmd(ADC1, ENABLE);  // 使能DMA模式
  ADC_Cmd(ADC1, ENABLE);     // 使能ADC1   
  ADC_ResetCalibration(ADC1);// 复位校准寄存器                                          
  while(ADC_GetResetCalibrationStatus(ADC1));  // 等待校准结束                        
  ADC_StartCalibration(ADC1); // 开始校准                                        
  while(ADC_GetCalibrationStatus(ADC1)); // 等待校准结束          
  ADC_SoftwareStartConvCmd(ADC1, ENABLE); // 启动ADC转换
}                                                
```

- 主函数

```c
// 通过ADC1通道16获取芯片内部温度，并通过串口打印到电脑串口调试助手显示
#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./temp/bsp_tempad.h"
// ADC1转换的电压值通过MDA方式传到sram
extern __IO u16 ADC_ConvertedValue;
//计算后的温度值
u16 Current_Temp;     

void Delay(__IO u32 nCount)
{
  for(; nCount != 0; nCount--);
} 

int main(void)
{         
    USART_Config();
    Temp_ADC1_Init();
    printf("\r\n 这是一个内部温度传感器实验 \r\n");
    printf( "\r\n Print current Temperature  \r\n");    
  while (1)
  { 
    Delay(0xffffee);      // 延时 
    // 计算方法1                                                                       
     // Current_Temp= (1.43- ADC_ConvertedValue*3.3/4096)*1000 / 4.3+ 25 ;
    //计算方法2
    Current_Temp = (V25-ADC_ConvertedValue)/AVG_SLOPE+25;    
    //10进制显示
    printf("\r\n The IC current tem= %3d ℃\r\n", Current_Temp);          
    //16进制显示                          
     // printf("\r\n The current temperature= %04x \r\n", Current_Temp);         
  }
}
```

## 3. 小结

### 实验步骤

#### 1. 硬件准备

- STM32F103系列开发板
- USB连接线

#### 2. 软件准备

- 开发环境: Keil, IAR 或其他支持STM32的IDE

#### 3. 项目配置

1. **引入标准库**: 确保你已引入STM32标准外设库。
2. **配置ADC和DMA**: 通过寄存器设置来配置ADC和DMA。

### 代码示例

```c
#include "stm32f10x.h" // 引入STM32标准库头文件

#define BUFFER_SIZE 10 // 定义DMA缓冲区大小
uint16_t temperatureBuffer[BUFFER_SIZE]; // 温度数据缓冲区

// 系统时钟配置
void RCC_Configuration(void) {
    // 使能ADC1的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    // 使能DMA1的时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}

// GPIO配置
void GPIO_Configuration(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; // 设置ADC输入引脚（PA0）
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; // 设置为模拟输入模式
    GPIO_Init(GPIOA, &GPIO_InitStructure); // 初始化GPIOA
}

// ADC配置
void ADC_Configuration(void) {
    ADC_InitTypeDef ADC_InitStructure; // ADC初始化结构体
    ADC_CommonInitTypeDef ADC_CommonInitStructure; // ADC公共初始化结构体

    // ADC工作模式配置
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent; // 独立模式
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2; // 时钟分频
    ADC_CommonInit(&ADC_CommonInitStructure); // 进行ADC公共配置

    // ADC具体初始化配置
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b; // 12位分辨率
    ADC_InitStructure.ADC_ScanConvMode = DISABLE; // 禁用扫描转换模式
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; // 启用连续转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 无外部触发
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; // 数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 1; // 通道数量为1
    ADC_Init(ADC1, &ADC_InitStructure); // 初始化ADC1

    // 启用内部温度传感器和参考电压
    ADC_TempSensorVrefintCmd(ENABLE); 
    ADC_Cmd(ADC1, ENABLE); // 启动ADC1
}

// DMA配置
void DMA_Configuration(void) {
    DMA_InitTypeDef DMA_InitStructure; // DMA初始化结构体
    // 设置DMA的外设基地址（ADC数据寄存器地址）
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR; 
    // 设置DMA的内存基地址（数据缓冲区）
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)temperatureBuffer; 
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; // 设置DMA传输方向为外设到内存
    DMA_InitStructure.DMA_BufferSize = BUFFER_SIZE; // 设置DMA缓冲区大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 禁用外设地址递增
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; // 启用内存地址递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // 外设数据大小为半字（16位）
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; // 内存数据大小为半字（16位）
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; // 设置为循环模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High; // 设置DMA优先级为高
    DMA_Init(DMA1_Channel1, &DMA_InitStructure); // 初始化DMA1通道1
    DMA_Cmd(DMA1_Channel1, ENABLE); // 启用DMA1通道1
}

// 启动ADC转换
void ADC_Start(void) {
    ADC_SoftwareStartConvCmd(ADC1, ENABLE); // 软件启动ADC转换
}

// 主函数
int main(void) {
    RCC_Configuration(); // 调用时钟配置函数
    GPIO_Configuration(); // 调用GPIO配置函数
    DMA_Configuration(); // 调用DMA配置函数
    ADC_Configuration(); // 调用ADC配置函数
    ADC_Start(); // 启动ADC转换

    while (1) {
        // 在这里处理温度数据
        // 例如，可以读取temperatureBuffer中的数据并进行处理
    }
}

// DMA中断处理程序
void DMA1_Channel1_IRQHandler(void) {
    // 在这里处理DMA中断
    // 例如，可以处理接收到的数据
}
```

### 代码解释

1. **库文件**: 包含了STM32标准库的必要头文件。
2. **缓冲区定义**: 定义了一个数组用于存储温度数据。
3. **时钟配置**: 使能ADC和DMA的时钟。
4. **GPIO配置**: 设置PA0引脚为模拟输入，连接到ADC。
5. **ADC配置**: 设置ADC的分辨率、转换模式等参数。
6. **DMA配置**: 设置DMA的工作模式、数据方向等。
7. **启动ADC**: 软件启动ADC转换。
8. **主循环**: 这里可以处理获取到的温度数据。
9. **中断处理**: 可以在DMA中断处理函数中进行数据处理。

---

2024.9.21 第一次修订，后期不再维护
