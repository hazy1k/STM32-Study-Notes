# 第五十六章 PWR-待机模式实验

## 1. 硬件设计

本实验中的硬件与睡眠模式、停止模式中的一致，主要使用到了按键、LED彩灯以及使用串口输出调试信息。要强调的是， 由于WKUP引脚(PA0)必须使用上升沿才能唤醒待机状态的系统，所以我们硬件设计的PA0引脚连接到按键KEY1，且按下按键的时候会在PA0引脚产生上升沿， 从而可实现唤醒的功能，按键的具体电路请查看配套的原理图。

## 2. 软件设计

### 2.1 编程目标

- 清除WUF标志位；

- 使能WKUP唤醒功能；

- 进入待机状态。

### 2.2 代码分析

首先看看主函数

```c
int main(void)
{    
    // 使能电源管理单元的时钟,必须要使能时钟才能进入待机模式
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    LED_GPIO_Config();    
    USART_Config();            

    // 初始化按键，不需要中断,仅初始化KEY2即可，只用于唤醒的PA0引脚不需要这样初始化
    Key_GPIO_Config();   

    printf("\r\n 实验说明：\r\n");
    printf("\r\n 1.本程序中，绿灯表示本次复位是上电或引脚复位，红灯表示即将进入待机状态，蓝灯表示本次是待机唤醒的复位\r\n");
    printf("\r\n 2.长按KEY2按键后，会进入待机模式\r\n");
    printf("\r\n 3.在待机模式下，按KEY1按键可唤醒，唤醒后系统会进行复位，程序从头开始执行\r\n");
    printf("\r\n 4.可通过检测WU标志位确定复位来源\r\n");
    printf("\r\n 5.在待机状态下，DAP下载器无法给STM32下载程序，需要唤醒后才能下载");

    // 检测复位来源
    if(PWR_GetFlagStatus(PWR_FLAG_WU) == SET)
    {
        LED_BLUE;
        printf("\r\n 待机唤醒复位 \r\n");
    }
    else
    {
        LED_GREEN;
        printf("\r\n 非待机唤醒复位 \r\n");
    }

  while(1)
  {            
        // K2 按键长按进入待机模式
        if(KEY2_LongPress()) // 长按KEY2
        {
            printf("\r\n 即将进入待机模式，进入待机模式后可按KEY1唤醒，唤醒后会进行复位，程序从头开始执行\r\n");
            LED_RED;    
            Delay(0xFFFFFF);
            // 清除WU状态位
            PWR_ClearFlag(PWR_FLAG_WU);
            // 使能WKUP引脚的唤醒功能 ，使能PA0
            PWR_WakeUpPinCmd(ENABLE);    
            // 进入待机模式 
            PWR_EnterSTANDBYMode();
        }
  }
}
```

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/Power008.png)

(1) 程序首先使用库函数RCC_APB1PeriphClockCmd和参数RCC_APB1Periph_PWR初始化了电源管理外设的时钟， 要先使能该时钟，后面才能正常使用命令进入待机状态和唤醒。

(2) 接着初始化LED灯及串口以便用于指示芯片的运行状态，由于待机模式唤醒使用WKUP引脚并不需要特别的引脚初始化， 所以我们调用的按键初始化函数Key_GPIO_Config它的内部只初始化了KEY2按键，而且是普通的输入模式，对唤醒用的PA0引脚可以不初始化。 当然，如果不初始化PA0的话，在正常运行模式中KEY1按键是不能正常运行的，我们这里只是强调待机模式的WKUP唤醒不需要中断， 也不需要像按键那样初始化。本工程中使用的Key_GPIO_Config函数定义如下：

```c
void Key_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    // 开启按键GPIO口的时钟
    RCC_AHB1PeriphClockCmd(KEY2_GPIO_CLK,ENABLE);
    // 设置引脚为输入模式
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    // 设置引脚不上拉也不下拉
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    // 选择按键的引脚
    GPIO_InitStructure.GPIO_Pin = KEY2_GPIO_PIN;
    // 使用上面的结构体初始化按键
    GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure);
}
```

(3) 使用库函数PWR_GetFlagStatus检测PWR_FLAG_WU标志位，当这个标志位为SET状态的时候，表示本次系统是从待机模式唤醒的复位， 否则可能是上电复位。我们利用这个区分两种复位形式，分别使用蓝色LED灯或绿色LED灯来指示。

(4) 在while循环中，使用自定义的函数KEY2_LongPress来检测KEY2按键是否被长时间按下，若长时间按下则进入待机模式， 否则继续while循环。

```c
// 用于检测按键是否被长时间按下
static uint8_t KEY2_LongPress(void)
{            
    uint8_t downCnt = 0; // 记录按下的次数
    uint8_t upCnt = 0;   // 记录松开的次数            

    while(1)                                                                                                    
    {    
        Delay(0x2FFFF);                                                                                
        if(GPIO_ReadInputDataBit(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == SET) // 检测到按下按键
        {
            downCnt++; // 记录按下次数
            upCnt =0 ; // 清除按键释放记录
            if(downCnt >= 100) // 按下时间足够
            {
                return 1; // 长按                                                                                
            }
        }
        else 
        {
            upCnt++; // 记录释放次数
            if(upCnt>5) // 连续检测到释放超过5次
            {
                return 0; // 按下时间太短，不是按键长按操作
            }
        } 
    }
}
```

(5) 检测到KEY2按键被长时间按下，要进入待机模式。在使用库函数PWR_EnterSTANDBYMode发送待机命令前， 要先使用库函数PWR_ClearFlag清除PWR_FLAG_WU标志位，并且使用库函数PWR_WakeUpPinCmd使能WKUP唤醒功能，这样进入待机模式后才能使用WKUP唤醒。

(6) 在进入待机模式前我们控制了LED彩灯为红色， 但在待机状态时，由于I/O口会处于高阻态，所以LED灯会熄灭。

(7) 按下KEY1按键，会使PA0引脚产生一个上升沿， 从而唤醒系统。

(8) 系统唤醒后会进行复位，从头开始执行上述过程，与第一次上电时不同的是， 这样的复位会使PWR_FLAG_WU标志位改为SET状态，所以这个时候LED彩灯会亮蓝色。

## 3. 小结

### 实验目标

- 理解STM32的待机模式及其工作原理。
- 通过LED和按键观察待机模式的效果。

### 实验步骤

#### 1. 硬件连接

- 将LED连接到一个GPIO引脚（例如PA5）。
- 将按键连接到另一个GPIO引脚（例如PC13），确保其配置为外部中断。
- 使用适当的电阻连接LED，以控制电流。

#### 2. 软件配置

1. **创建新项目**：在STM32的开发环境中创建一个新项目，选择对应的STM32芯片。
2. **配置GPIO**：
   - 将PA5设置为输出模式，用于控制LED。
   - 将PC13设置为输入模式，并配置为外部中断（EXTI）触发，选择下降沿触发（按键按下）。

#### 3. 编写代码

```c
#include "stm32f4xx.h"  // STM32F4标准固件库头文件

// 定义LED和按键的引脚
#define LED_PIN GPIO_Pin_5
#define LED_PORT GPIOA
#define BUTTON_PIN GPIO_Pin_13
#define BUTTON_PORT GPIOC

void GPIO_Config(void);
void System_Clock_Config(void);
void Enter_Standby_Mode(void);

int main(void)
{
    // 初始化系统时钟
    System_Clock_Config();
    // 配置GPIO
    GPIO_Config();
    // 无限循环
    while (1)
    {
        // 点亮LED，表示系统将进入待机模式
        GPIO_SetBits(LED_PORT, LED_PIN);
        Delay(1000); // 等待1秒
        // 关闭LED，准备进入待机模式
        GPIO_ResetBits(LED_PORT, LED_PIN);
        // 进入待机模式
        Enter_Standby_Mode();
        // 唤醒后再次点亮LED，表示系统已唤醒
        GPIO_SetBits(LED_PORT, LED_PIN);
        Delay(1000); // 等待1秒
    }
}

// 按键中断回调函数
void EXTI15_10_IRQHandler(void)
{
    // 检查按键引脚是否触发中断
    if (EXTI_GetITStatus(EXTI_Line13) != RESET)
    {
        // 清除中断标志位
        EXTI_ClearITPendingBit(EXTI_Line13);
    }
}

// 进入待机模式的函数
void Enter_Standby_Mode(void)
{
    // 使能PWR时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    // 设置待机模式
    PWR_EnterSTANDBYMode();
}

// GPIO配置函数
void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    // 使能GPIO时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    
    // 配置LED引脚为输出模式
    GPIO_InitStruct.GPIO_Pin = LED_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(LED_PORT, &GPIO_InitStruct);

    // 配置按键引脚为输入模式并使能外部中断
    GPIO_InitStruct.GPIO_Pin = BUTTON_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN; // 下拉
    GPIO_Init(BUTTON_PORT, &GPIO_InitStruct);
    
    // 配置外部中断
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource13);
    
    EXTI_InitTypeDef EXTI_InitStruct;
    EXTI_InitStruct.EXTI_Line = EXTI_Line13; // 选择按键引脚
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt; // 中断模式
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling; // 下降沿触发
    EXTI_InitStruct.EXTI_LineCmd = ENABLE; // 使能中断
    EXTI_Init(&EXTI_InitStruct);
}

// 系统时钟配置函数
void System_Clock_Config(void)
{
    // 配置系统时钟的代码（生成的代码）
    // 例如设置HSE作为系统时钟，配置PLL等
}

// 延时函数，简化处理
void Delay(uint32_t time)
{
    while (time--);
}
```

---

2024.9.25 第一次修订，后期不再维护
