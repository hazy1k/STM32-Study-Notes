# 第二章 GPIO介绍及应用

## 1. GPIO简介

GPIO 是通用输入输出端口的简称，简单来说就是 STM32 可控制的引脚， STM32 芯片的 GPIO 引脚与外部设备连接起来，从而实现与外部通讯、控制以及数据采集的功能。 STM32 芯片的 GPIO被分成很多组，每组有 16 个引脚，如型号为 STM32F103VET6 型号的芯片有 GPIOA、 GPIOB、GPIOC 至 GPIOE 共 5 组 GPIO，芯片一共 100 个引脚，其中 GPIO 就占了一大部分，所有的 GPIO引脚都有基本的输入输出功能。

最基本的输出功能是由 STM32 控制引脚输出高、低电平，实现开关控制，如把 GPIO 引脚接入到 LED 灯，那就可以控制 LED 灯的亮灭，引脚接入到继电器或三极管，那就可以通过继电器或三极管控制外部大功率电路的通断。

最基本的输入功能是检测外部输入电平，如把 GPIO 引脚连接到按键，通过电平高低区分按键是否被按下。

## 2. GPIO工作模式

总结一下，由 GPIO 的结构决定了 GPIO 可以配置成以下模式：

```c
typedef enum
{
    GPIO_Mode_AIN = 0x0, // 模拟输入
    GPIO_Mode_IN_FLOATING = 0x04, // 浮空输入
    GPIO_Mode_IPD = 0x28, // 下拉输入
    GPIO_Mode_IPU = 0x48, // 上拉输入
    GPIO_Mode_Out_OD = 0x14, // 开漏输出    
    GPIO_Mode_Out_PP = 0x10, // 推挽输出
    GPIO_Mode_AF_OD = 0x1C, // 复用开漏输出
    GPIO_Mode_AF_PP = 0x18 // 复用推挽输出
} GPIOMode_TypeDef;
```

在标准库中， GPIO 总共有 8 种细分的工作模式，稍加整理可以大致归类为以下三类：

### 2.2.1 输入模式（模拟/浮空/上拉/下拉）

在输入模式时，施密特触发器打开，输出被禁止，可通过输入数据寄存器 GPIOx_IDR 读取 I/O 状态。其中输入模式，可设置为上拉、下拉、浮空和模拟输入四种。上拉和下拉输入很好理解，默认的电平由上拉或者下拉决定。浮空输入的电平是不确定的，完全由外部的输入决定，一般接按键的时候用的是这个模式。模拟输入则用于 ADC 采集。

### 2.2.2 输出模式（推挽/开漏）

在输出模式中，推挽模式时双 MOS 管以轮流方式工作，输出数据寄存器 GPIOx_ODR 可控制 I/O输出高低电平。开漏模式时，只有 N-MOS 管工作，输出数据寄存器可控制 I/O 输出高阻态或低电平。输出速度可配置，有 2MHz10MHz50MHz 的选项。此处的输出速度即 I/O 支持的高低电平状态最高切换频率，支持的频率越高，功耗越大，如果功耗要求不严格，把速度设置成最大即可

在输出模式时施密特触发器是打开的，即输入可用，通过输入数据寄存器 GPIOx_IDR 可读取 I/O的实际状态。

### 2.2.3 复用功能（推挽/开漏）

复用功能模式中，输出使能，输出速度可配置，可工作在开漏及推挽模式，但是输出信号源于其它外设，输出数据寄存器 GPIOx_ODR 无效；输入可用，通过输入数据寄存器可获取 I/O 实际状态，但一般直接用外设的寄存器来获取该数据信号。

## 3. GPIO使用示例

### 3.1 GPIO输出-点亮LED

![屏幕截图 2025-05-26 213933.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/05/26-21-39-42-屏幕截图%202025-05-26%20213933.png)

#### 3.1.1 led相关参数宏定义

```c
#ifndef __LED_H__
#define __LED_H__

#include "stm32f10x.h"

#define LED_GPIO_Port GPIOB
#define LED_GPIO_CLK RCC_APB2Periph_GPIOB
#define RED_LED_Pin GPIO_Pin_5
#define GREEN_LED_Pin GPIO_Pin_0
#define BLUE_LED_Pin GPIO_Pin_1

#define LED_ON(x)    GPIO_SetBits(LED_GPIO_Port, (x))
#define LED_OFF(x)   GPIO_ResetBits(LED_GPIO_Port, (x))

#endif /* __LED_H__ */
```

#### 3.1.2 GPIO初始化

```c
#include "led.h"

void BSP_LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(LED_GPIO_CLK, ENABLE); // 打开GPIO时钟

    // 初始化红色 LED 引脚
    GPIO_InitStructure.GPIO_Pin = RED_LED_Pin; // 选择引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 输出速度
    GPIO_Init(LED_GPIO_Port, &GPIO_InitStructure);

    // 初始化绿色 LED 引脚
    GPIO_InitStructure.GPIO_Pin = GREEN_LED_Pin;
    GPIO_Init(LED_GPIO_Port, &GPIO_InitStructure);

    // 初始化蓝色 LED 引脚
    GPIO_InitStructure.GPIO_Pin = BLUE_LED_Pin;
    GPIO_Init(LED_GPIO_Port, &GPIO_InitStructure);

    // 初始LED全灭
    LED_OFF(RED_LED_Pin);
    LED_OFF(GREEN_LED_Pin);
    LED_OFF(BLUE_LED_Pin);
}
```

#### 3.1.3 主函数测试

```c
#include "stm32f10x.h"
#include "led.h"

#define led_delay delay(0x0FFFFF);

void delay(__IO uint32_t nCount)
{
    for(; nCount != 0; nCount--);
}

int main()
{
    BSP_LED_Init();
    while(1)
    {
        LED_ON(RED_LED_Pin);
        led_delay;
        LED_OFF(RED_LED_Pin);
        led_delay;
        LED_ON(GREEN_LED_Pin);
        led_delay;
        LED_OFF(GREEN_LED_Pin);
        led_delay;
        LED_ON(BLUE_LED_Pin);
        led_delay;
        LED_OFF(BLUE_LED_Pin);
        led_delay;
    }
}
```

### 3.2 GPIO输出-控制蜂鸣器

### 3.3 GPIO输入-按键控制LED
