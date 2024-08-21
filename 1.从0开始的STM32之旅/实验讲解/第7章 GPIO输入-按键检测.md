# 第七章 GPIO输入-按键控制LED

## 1. 硬件设计

    本章我们用到的硬件是-机械按键开关。在51单片机里我们已经学过了。此出再强调一下消抖。

    按键机械触点断开、闭合时，由于触点的弹性作用，按键开关不会马上稳定接通或一下子断开， 使用按键时会产生带波纹信号，需要用软件消抖处理滤波，不方便输入检测。本实验板连接的按键带硬件消抖功能， 见图 ，它利用电容充放电的延时，消除了波纹，从而简化软件的处理，软件只需要直接检测引脚的电平即可。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/GPIOin002.jpg)

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/GPIOin003.png)

    从按键的原理可知，在按键没有被按下的时候，GPIO引脚的输入状态为低电平（按键所在的电路不通，引脚接地）当按键按下时，GPIO引脚的输入状态为高电平（按键电路导通，引脚接到电源）所为我们只需要检测引脚的输入电平，就可以判断按键是否按下。（按下1，没有按下0）

## 2. 软件设计

### 2.1 编程目的

1. 使能GPIO端口时钟；

2. 初始化GPIO目标引脚为输入模式(浮空输入)；

3. 编写简单测试程序，检测按键的状态，实现按键控制LED灯。

### 2.2 按键宏定义

    在编写按键驱动时，我们同样需要功能模块化。我们把按键检测引脚相关的宏定义到“key.h”文件中。

```c
// 按键检测引脚相关的宏

//  引脚定义
#define KEY1_GPIO_CLK  RCC_APB2Periph_GPIOA // 时钟
#define KEY1_GPIO_PORT GPIOA // 端口
#define KEY1_GPIO_PIN  GPIO_Pin_0 // 引脚

#define KEY2_GPIO_CLK  RCC_APB2Periph_GPIOC // 时钟
#define KEY2_GPIO_PORT GPIOC // 端口
#define KEY2_GPIO_PIN  GPIO_Pin_13 // 引脚
```

    以上代码根据按键的硬件连接，把检测按键输入的GPIO端口、GPIO引脚号以及GPIO端口时钟封装起来了，方便后面直接使用。

### 2.3 按键GPIO初始化宏定义

    下面利用我们刚刚定义的宏，编写按键的初始化函数

```c
// 按键GPIO初始化函数

void Key_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /*开启按键端口的时钟*/
    RCC_APB2PeriphClockCmd(KEY1_GPIO_CLK|KEY2_GPIO_CLK,ENABLE);

    // 选择按键的引脚
    GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_PIN; 
    // 设置按键的引脚为浮空输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
    // 使用结构体初始化按键
    GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);

    // 选择按键的引脚
    GPIO_InitStructure.GPIO_Pin = KEY2_GPIO_PIN; 
    // 设置按键的引脚为浮空输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
    // 使用结构体初始化按键
    GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure);    
}
```

同为GPIO的初始化函数，初始化的流程与“LED GPIO初始化函数”章节中的类似，主要区别是引脚的模式。函数执行流程如下：

1. 使用GPIO_InitTypeDef定义GPIO初始化结构体变量，以便下面用于存储GPIO配置。

2. 调用库函数RCC_APB2PeriphClockCmd来使能按键的GPIO端口时钟，调用时我们使用“|”操作同时配置两个按键的时钟。

3. 向GPIO初始化结构体赋值，把引脚初始化成浮空输入模式，其中的GPIO_Pin使用宏“KEYx_GPIO_PIN”来赋值，使函数的实现方便移植。 由于引脚的默认电平受按键电路影响，所以设置成浮空输入。

4. 使用以上初始化结构体的配置，调用GPIO_Init函数向寄存器写入参数，完成GPIO的初始化， 这里的GPIO端口使用“KEYx_GPIO_PORT”宏来赋值，也是为了程序移植方便。

5. 使用同样的初始化结构体，只修改控制的引脚和端口，初始化其它按键检测时使用的GPIO引脚。

### 2.4 检测按键的状态

    初始化按键后，我们就可以通过检测对应引脚的电平来判断按键状态了

```c
// 按键检测函数
uint8_t Key_Scan(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) // 函数参数：端口号，引脚
{            
    /*检测是否有按键按下 */
    if(GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == KEY_ON ) // 库函数：GPIO_ReadInputDataBit，用于获取引脚状态
    {     
        /*等待按键释放 */
        while(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON);   
        return KEY_ON;     
    }
    else
        return KEY_OFF;
}
```

    在这里我们定义了一个Key_Scan函数用于扫描按键状态。GPIO引脚的输入电平可通过读取IDR寄存器对应的数据位来感知， 而STM32标准库提供了库函数GPIO_ReadInputDataBit来获取位状态，该函数输入GPIO端口及引脚号，函数返回该引脚的电平状态，高电平返回1， 低电平返回0。

    Key_Scan函数中以GPIO_ReadInputDataBit的返回值与自定义的宏“KEY_ON”对比，若检测到按键按下，则使用while循环持续检测按键状态， 直到按键释放，按键释放后Key_Scan函数返回一个“KEY_ON”值；若没有检测到按键按下，则函数直接返回“KEY_OFF”。 

    若按键的硬件没有做消抖处理，需要在这个Key_Scan函数中做软件滤波，防止波纹抖动引起误触发。（我们的硬件已经做了处理所以不需要，软件处理就是添加一个延时函数，可以参考51的按键实验）

### 2.5 LED初始化

```c
#include "./led/bsp_led.h"   

// LED初始化函数
void LED_GPIO_Config(void)
{        
        /*定义一个GPIO_InitTypeDef类型的结构体*/
        GPIO_InitTypeDef GPIO_InitStructure;

        /*开启LED相关的GPIO外设时钟*/
        RCC_APB2PeriphClockCmd( LED1_GPIO_CLK | LED2_GPIO_CLK | LED3_GPIO_CLK, ENABLE);
        /*选择要控制的GPIO引脚*/
        GPIO_InitStructure.GPIO_Pin = LED1_GPIO_PIN;    

        /*设置引脚模式为通用推挽输出*/
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

        /*设置引脚速率为50MHz */   
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

        /*调用库函数，初始化GPIO*/
        GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure);    

        /*选择要控制的GPIO引脚*/
        GPIO_InitStructure.GPIO_Pin = LED2_GPIO_PIN;

        /*调用库函数，初始化GPIO*/
        GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStructure);

        /*选择要控制的GPIO引脚*/
        GPIO_InitStructure.GPIO_Pin = LED3_GPIO_PIN;

        /*调用库函数，初始化GPIOF*/
        GPIO_Init(LED3_GPIO_PORT, &GPIO_InitStructure);

        /* 关闭所有led灯    */
        GPIO_SetBits(LED1_GPIO_PORT, LED1_GPIO_PIN);

        /* 关闭所有led灯    */
        GPIO_SetBits(LED2_GPIO_PORT, LED2_GPIO_PIN);     

    /* 关闭所有led灯    */
        GPIO_SetBits(LED3_GPIO_PORT, LED3_GPIO_PIN);
}
```

    上一章我们已经详细解释过了，这里不在赘述

### 2.6 主函数

```c
// 按键检测主函数

int main(void)
{
    /* LED 端口初始化 */
    LED_GPIO_Config();

    /*初始化按键*/
    Key_GPIO_Config();

    /* 轮询按键状态，若按键按下则反转LED */
    while (1) 
    {
        if (Key_Scan(KEY1_GPIO_PORT, KEY1_PIN) == KEY_ON) 
        {
            /*LED1反转*/
            LED1_TOGGLE;
        }

        if (Key_Scan(KEY2_GPIO_PORT, KEY2_PIN) == KEY_ON) 
        {
            /*LED2反转*/
            LED2_TOGGLE;
        }
    }
}
```

## 3. 小结

    写入程序后，初始LED关，按下key1，LED亮（红）再按下关闭，key2--LED（绿），有趣的是你可以把两个灯同时打开就能得到混合颜色，至于为什么显示这种颜色，上一章已经说过或者参考led函数的头文件。

    代码不难而且注释也写得很详细了，主要就是一个配置问题和一个扫描函数，led可以照搬

    扫描按键函数中我们使用了一个新的库函数-GPIO_ReadInputDataBit，下面我们详细解释一下（以后一旦我们遇到新函数，都尽量解释一下吧，慢慢见多识广了）：

在 STM32 标准外设库中，`GPIO_ReadInputDataBit` 函数的原型通常如下：

```c
BitAction GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, GPIO_PinState GPIO_Pin);
```

参数说明：

1. **`GPIOx`**:
   
   - 类型：`GPIO_TypeDef*`
   - 描述：指定要读取的 GPIO 端口。`GPIOx` 是一个指向 GPIO 端口寄存器的指针，如 `GPIOA`, `GPIOB`, `GPIOC` 等。
   
   例如，如果你传入 `GPIOA`，函数将读取 GPIOA 端口的引脚状态。

2. **`GPIO_Pin`**:
   
   - 类型：`GPIO_PinState`
   - 描述：指定要读取的 GPIO 引脚。这个参数是一个掩码值，用于选择特定的引脚。例如，`GPIO_Pin_0` 表示第 0 号引脚，`GPIO_Pin_1` 表示第 1 号引脚，依此类推。

返回值：

- **`Bit_SET`**：表示指定的 GPIO 引脚为高电平（逻辑 1）。
- **`Bit_RESET`**：表示指定的 GPIO 引脚为低电平（逻辑 0）。

函数作用：

`GPIO_ReadInputDataBit` 函数的主要作用是读取一个 GPIO 引脚的电平状态，这在需要根据引脚的电平状态来做决策时非常有用。例如，读取一个按钮的状态或传感器的输出。

假设你要读取 GPIOA 端口的第 5 号引脚的电平状态，你可以使用 `GPIO_ReadInputDataBit` 函数来完成。下面是一个简单的示例代码：

```c
#include "stm32f10x.h"

// 假设你已经配置好了 GPIOA 的第 5 号引脚为输入模式

int main(void)
{
    // 初始化 GPIOA，第 5 号引脚为输入模式
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    while (1)
    {
        // 读取 GPIOA 的第 5 号引脚的电平状态
        BitAction pinState = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5);
        
        if (pinState == Bit_SET)
        {
            // 引脚为高电平，执行相关操作
        }
        else
        {
            // 引脚为低电平，执行相关操作
        }
        
        // 延迟或其他操作
    }
}

```

在这个示例中：

1. **GPIOA** 的第 5 号引脚被配置为浮空输入模式。
2. `GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5)` 用于读取该引脚的电平状态。
3. 根据返回的电平状态（`Bit_SET` 或 `Bit_RESET`），程序可以执行不同的操作。

---

2024.7.22 第一次修订

2024.8.20 第二次修订，后期不再维护
