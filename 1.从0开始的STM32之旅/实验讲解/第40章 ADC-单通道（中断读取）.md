# 第40章 ADC-单通道（中断读取）

## 1. 硬件设计

开发板板载一个贴片滑动变阻器，电路设计见图 [开发板电位器部分原理图](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/ADC.html#id17) 。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/ADC006.png)

贴片滑动变阻器的动触点通过连接至STM32芯片的ADC通道引脚。当我们使用旋转滑动变阻器调节旋钮时， 其动触点电压也会随之改变，电压变化范围为0~3.3V，亦是开发板默认的ADC电压采集范围。

## 2. 软件设计

### 2.1 编程目标

1) 初始ADC用到的GPIO；

2) 设置ADC的工作参数并初始化；

3) 设置ADC工作时钟；

4) 设置ADC转换通道顺序及采样时间；

5) 配置使能ADC转换完成中断，在中断内读取转换完数据；

6) 使能ADC；

7) 使能软件触发ADC转换。

ADC转换结果数据使用中断方式读取，这里没有使用DMA进行数据传输。

### 2.2 代码分析

- ADC宏定义

```c
// ADC 编号选择
// 可以是 ADC1/2，如果使用ADC3，中断相关的要改成ADC3的
#define    ADC_APBxClock_FUN             RCC_APB2PeriphClockCmd
#define    ADCx                          ADC2
#define    ADC_CLK                       RCC_APB2Periph_ADC2

// ADC GPIO宏定义
// 注意：用作ADC采集的IO必须没有复用，否则采集电压会有影响
#define    ADC_GPIO_APBxClock_FUN        RCC_APB2PeriphClockCmd
#define    ADC_GPIO_CLK                  RCC_APB2Periph_GPIOC  
#define    ADC_PORT                      GPIOC
#define    ADC_PIN                       GPIO_Pin_1
// ADC 通道宏定义
#define    ADC_CHANNEL                   ADC_Channel_11

// ADC 中断相关宏定义
#define    ADC_IRQ                       ADC1_2_IRQn
#define    ADC_IRQHandler                ADC1_2_IRQHandler
```

使用宏定义引脚信息方便硬件电路改动时程序移植。

- ADC GPIO初始化函数

```c
static void ADCx_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 打开 ADC IO端口时钟
    ADC_GPIO_APBxClock_FUN ( ADC_GPIO_CLK, ENABLE );

    // 配置 ADC IO 引脚模式
    // 必须为模拟输入
    GPIO_InitStructure.GPIO_Pin = ADC_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;

    // 初始化 ADC IO
    GPIO_Init(ADC_PORT, &GPIO_InitStructure);
}
```

使用到GPIO时候都必须开启对应的GPIO时钟，GPIO用于AD转换功能必须配置为模拟输入模式。

- 配置ADC工作模式

```c
static void ADCx_Mode_Config(void)
{
    ADC_InitTypeDef ADC_InitStructure;

    // 打开ADC时钟
    ADC_APBxClock_FUN ( ADC_CLK, ENABLE );

    // ADC 模式配置
    // 只使用一个ADC，属于独立模式
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;

    // 禁止扫描模式，多通道才要，单通道不需要
    ADC_InitStructure.ADC_ScanConvMode = DISABLE ;

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

    // 配置ADC时钟Ｎ狿CLK2的8分频，即9MHz
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    // 配置 ADC 通道转换顺序为1，第一个转换，采样时间为55.5个时钟周期
    ADC_RegularChannelConfig(ADCx, ADC_CHANNEL, 1,
                            ADC_SampleTime_55Cycles5);

    // ADC 转换结束产生中断，在中断服务程序中读取转换值
    ADC_ITConfig(ADCx, ADC_IT_EOC, ENABLE);

    // 开启ADC ，并开始转换
    ADC_Cmd(ADCx, ENABLE);

    // 初始化ADC 校准寄存器
    ADC_ResetCalibration(ADCx);
    // 等待校准寄存器初始化完成
    while (ADC_GetResetCalibrationStatus(ADCx));

    // ADC开始校准
    ADC_StartCalibration(ADCx);
    // 等待校准完成
    while (ADC_GetCalibrationStatus(ADCx));

    // 由于没有采用外部触发，所以使用软件触发ADC转换
    ADC_SoftwareStartConvCmd(ADCx, ENABLE);
}
```

首先，定义一个ADC初始化结构体ADC_InitTypeDef，用来配置ADC具体的工作模式。然后调用RCC_APB2PeriphClockCmd()开启ADC时钟。

ADC工作参数具体配置为：独立模式、单通道采集不需要扫描、启动连续转换、使用内部软件触发无需外部触发事件、使用右对齐数据格式、 转换通道为1，并调用ADC_Init函数完成ADC1工作环境配置。

RCC_ADCCLKConfig()函数用来配置ADC的工作时钟，接收一个参数，设置的是PCLK2的分频系数，ADC的时钟最大不能超过14M。

ADC_RegularChannelConfig函数用来绑定ADC通道的转换顺序和时间。它接收4个形参，第一个形参选择ADC外设，可为ADC1、ADC2或ADC3； 第二个形参通道选择，总共可选18个通道；第三个形参为通道的转换顺序，可选为1到16；第四个形参为采样周期选择，采样周期越短， ADC转换数据输出周期就越短但数据精度也越低，采样周期越长，ADC转换数据输出周期就越长同时数据精度越高。

利用ADC转换完成中断可以非常方便的保证我们读取到的数据是转换完成后的数据而不用担心该数据可能是ADC正在转换时“不稳定”的数据。 我们使用ADC_ITConfig函数使能ADC转换完成中断，并在中断服务函数中读取转换结果数据。

最后，在ADC校准之后调用ADC_SoftwareStartConvCmd函数进行软件触发ADC开始转换。

- ADC中断配置

```c
static void ADC_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);
}
```

我们使能了ADC转换完成中断，需要配置中断源和中断优先级。

- 中断服务函数

```c
void ADC_IRQHandler(void)
{
    if (ADC_GetITStatus(ADCx,ADC_IT_EOC)==SET) {
        // 读取ADC的转换值
        ADC_ConvertedValue = ADC_GetConversionValue(ADCx);

    }
    ADC_ClearITPendingBit(ADCx,ADC_IT_EOC);

}
```

中断服务函数一般定义在stm32f10x_it.c文件内，我们使能了ADC转换完成中断，在ADC转换完成后就会进入中断服务函数， 我们在中断服务函数内直接读取ADC转换结果保存在变量ADC_ConvertedValue(在main.c中定义)中。

ADC_GetConversionValue函数是获取ADC转换结果值的库函数，只有一个形参为ADC外设，可选为ADC1、ADC2或ADC3， 该函数还返回一个16位的ADC转换结果值。

- 主函数

```c
int main(void)
{
    // 配置串口
    USART_Config();

    // ADC 初始化
    ADCx_Init();

    printf("\r\n ----这是一个ADC实验(DMA传输)----\r\n");

    while (1)
    {
        ADC_ConvertedValueLocal =(float) ADC_ConvertedValue/4096*3.3;

        printf("\r\n The current AD value = 0x%04X \r\n",
            ADC_ConvertedValue);
        printf("\r\n The current AD value = %f V \r\n",
            ADC_ConvertedValueLocal);
        printf("\r\n\r\n");

        Delay(0xffffee);
    }
}
```

主函数先调用USART_Config函数配置调试串口相关参数，函数定义在bsp_debug_usart.c文件中。

接下来调用ADCx _Init函数进行ADC初始化配置并启动ADC。ADCx_Init函数是定义在bsp_adc.c文件中， 它只是简单的分别调用ADC_GPIO_Config ()、ADC_Mode_Config ()和Rheostat_ADC_NVIC_Config()。

Delay函数只是一个简单的延时函数。

在ADC中断服务函数中我们把AD转换结果保存在变量ADC_ConvertedValue中，根据我们之前的分析可以非常清楚的计算出对应的电位器动触点的电压值。

最后就是把相关数据打印至串口调试助手。
