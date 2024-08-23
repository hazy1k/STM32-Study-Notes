# 第三十四章 TIM-全彩LED灯

## 1. 导入

全彩LED灯，实质上是一种把红、绿、蓝单色发光体集成到小面积区域中的LED灯，控制时对这三种颜色的灯管输出不同的光照强度， 即可混合得到不同的颜色，其混色原理与光的三原色混合原理一致。

例如，若红绿蓝灯都能控制输出光照强度为[0:255]种等级，那么该灯可混合得到使用RGB888表示的所有颜色(包括以RGB三个灯管都全灭所表示的纯黑色)。

## 2. 全彩LED灯控制原理

本教程配套开发板中的RGB灯就是一种全彩LED灯，前面介绍LED基本控制原理的时候，只能控制RGB三色灯的亮灭，即RGB每盏灯有[0:1]两种等级，因此只能组合出8种颜色。

要使用STM32控制LED灯输出多种亮度等级，可以通过控制输出脉冲的占空比来实现。见图：

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/COLORT002.jpeg)

示例图中列出了周期相同而占空比分别为100%、80%、50和20%的脉冲波形，假如利用这样的脉冲控制LED灯，即可控制LED灯亮灭时间长度的比例。 若提高脉冲的频率，LED灯将会高频率进行开关切换，由于视觉暂留效应，人眼看不到LED灯的开关导致的闪烁现象， 而是感觉到使用不同占空比的脉冲控制LED灯时的亮度差别。即单个控制周期内，LED灯亮的平均时间越长，亮度就越高，反之越暗。

把脉冲信号占空比分成256个等级，即可用于控制LED灯输出256种亮度，使用三种这样的信号控制RGB灯即可得到256*256*256种颜色混合的效果。 而要控制占空比，直接使用STM32定时器的PWM功能即可。

## 3. 硬件设计

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/COLORT004.jpeg)

本实验中的RGB灯使用阴极分别连接到了PB5、PB0及PB1，它们分别是定时器TIM3的通道2、3、4，其中PB5用于定时器输出通道时，需要使用重定义功能。

## 4. 软件设计

### 4.1 编程目标

1. 初始化RGB灯使用的GPIO；
2) 配置定时器输出PWM脉冲；

3) 编写修改PWM脉冲占空比大小的函数；

4) 测试配置的定时器脉冲控制周期是否会导致LED灯明显闪烁；

### 4.2 代码分析

- 定时器参数宏定义

```c
// 定时器配置
#define   COLOR_TIMx                    TIM3                                        // 选择使用的定时器
#define   COLOR_TIM_APBxClock_FUN       RCC_APB1PeriphClockCmd                      // 使能定时器时钟
#define   COLOR_TIM_CLK                 RCC_APB1Periph_TIM3                         // 定时器时钟
#define   COLOR_TIM_GPIO_CLK            (RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO)  // 定时器GPIO时钟
```

- 三种灯及通道宏定义

```c
//红灯的引脚需要重映射
#define   COLOR_GPIO_REMAP_FUN()        GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);                 
// 红灯
#define  COLOR_RED_TIM_LED_PORT               GPIOB
#define  COLOR_RED_TIM_LED_PIN                GPIO_Pin_5
#define  COLOR_RED_TIM_OCxInit                TIM_OC2Init          // 通道初始化函数
#define  COLOR_RED_TIM_OCxPreloadConfig       TIM_OC2PreloadConfig // 通道重载配置函数
//通道比较寄存器，以TIMx->CCRx方式可访问该寄存器，设置新的比较值，控制占空比
//以宏封装后，使用这种形式：COLOR_TIMx->COLOR_RED_CCRx ，可访问该通道的比较寄存器
#define  COLOR_RED_CCRx                       CCR2        

// 绿灯
#define  COLOR_GREEN_TIM_LED_PORT             GPIOB
#define  COLOR_GREEN_TIM_LED_PIN              GPIO_Pin_0
#define  COLOR_GREEN_TIM_OCxInit              TIM_OC3Init          // 通道初始化函数
#define  COLOR_GREEN_TIM_OCxPreloadConfig     TIM_OC3PreloadConfig // 通道重载配置函数
//通道比较寄存器，以TIMx->CCRx方式可访问该寄存器，设置新的比较值，控制占空比
//以宏封装后，使用这种形式：COLOR_TIMx->COLOR_GREEN_CCRx ，可访问该通道的比较寄存器
#define  COLOR_GREEN_CCRx                     CCR3

// 蓝灯
#define   COLOR_BLUE_TIM_LED_PORT             GPIOB
#define   COLOR_BLUE_TIM_LED_PIN              GPIO_Pin_1
#define   COLOR_BLUE_TIM_OCxInit              TIM_OC4Init          // 通道初始化函数
#define   COLOR_BLUE_TIM_OCxPreloadConfig     TIM_OC4PreloadConfig  // 通道重载配置函数
//通道比较寄存器，以TIMx->CCRx方式可访问该寄存器，设置新的比较值，控制占空比
//以宏封装后，使用这种形式：COLOR_TIMx->COLOR_BLUE_CCRx ，可访问该通道的比较寄存器
#define   COLOR_BLUE_CCRx                      CCR4
```

这些宏定义非常丰富，包括使用的定时器编号COLOR_TIMx、定时器时钟使能库函数COLOR_TIM_APBxClock_FUN和引脚重映射操作COLOR_GPIO_REMAP_FUN。

控制各个LED灯时，每个颜色占用一个通道，这些与通道相关的也使用宏封装起来了，如：端口号COLOR_xxx_TIM_LED_PORT、 引脚号COLOR_xxx_TIM_LED_PIN、通道初始化库函数COLOR_xxx_TIM_OCxInit、 通道重载配置库函数COLOR_xxx_TIM_OCxPreloadConfig以及通道对应的比较寄存器名COLOR_xxx_CCRx。其中xxx宏中的xxx指RED、GREEN和BLUE三种颜色。

由于部分操作使用宏封装后不够直观，此处着重强调一下与通道相关的寄存器宏。为方便修改定时器某通道输出脉冲的占空比， 初始化定时器后，可以直接使用形如“TIM3->CCR2=0xFFFFFF”的代码修改定时器TIM3通道2的比较寄存器中的数值，使用本工程中的宏封装后， 形式改为“COLOR_TIMx->COLOR_RED_CCRx=0xFFFFFF”。

- TIM GPIO初始化

```c
// 配置COLOR_TIMx复用输出PWM时用到的I/O
static void COLOR_TIMx_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(COLOR_TIM_GPIO_CLK, ENABLE); // 使能GPIO时钟
  COLOR_GPIO_REMAP_FUN(); // 颜色LED引脚复用设置

  // 配置LED灯用到的引脚
  //红
  GPIO_InitStructure.GPIO_Pin =  COLOR_RED_TIM_LED_PIN ;    
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;            // 复用推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(COLOR_RED_TIM_LED_PORT, &GPIO_InitStructure);

  // 绿
  GPIO_InitStructure.GPIO_Pin =  COLOR_GREEN_TIM_LED_PIN ;
  GPIO_Init(COLOR_GREEN_TIM_LED_PORT, &GPIO_InitStructure);

  // 蓝
  GPIO_InitStructure.GPIO_Pin =  COLOR_BLUE_TIM_LED_PIN ;
  GPIO_Init(COLOR_BLUE_TIM_LED_PORT, &GPIO_InitStructure);
}
```

与LED灯的基本控制不同，由于本实验直接使用定时器输出通道的脉冲信号控制LED灯，此处代码把GPIO相关的引脚都配置成了复用推挽输出模式。 其中由于红灯使用的引脚需要用到第二功能，本代码使用COLOR_GPIO_REMAP_FUN()进行了该引脚的功能重定义操作。

- 配置PWM信号和定时器模式

```c
// 配置COLOR_TIMx输出的PWM信号的模式，如周期、极性
static void COLOR_TIMx_Mode_Config(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;                                                                                
  COLOR_TIM_APBxClock_FUN(COLOR_TIM_CLK, ENABLE); // 使能COLOR_TIMx时钟

  // 基本定时器配置         
  TIM_TimeBaseStructure.TIM_Period = 255; // 当定时器从0计数到255，即为256次，为一个定时周期
  TIM_TimeBaseStructure.TIM_Prescaler = 4000;    // 设置预分频
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 设置时钟分频系数：不分频(这里用不到)
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
  TIM_TimeBaseInit(COLOR_TIMx, &TIM_TimeBaseStructure); // 初始化基本定时器

  // PWM模式配置
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                        // 配置为PWM模式1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;    // 使能输出
  TIM_OCInitStructure.TIM_Pulse = 0;                                                      // 设置初始PWM脉冲宽度为0    
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;        // 当定时器计数值小于CCR_Val时为低电平

    // 使能通道和预装载
  COLOR_RED_TIM_OCxInit(COLOR_TIMx, &TIM_OCInitStructure);                                 
  COLOR_RED_TIM_OCxPreloadConfig(COLOR_TIMx, TIM_OCPreload_Enable);                        

    // 使能通道和预装载
  COLOR_GREEN_TIM_OCxInit(COLOR_TIMx, &TIM_OCInitStructure);                                         
  COLOR_GREEN_TIM_OCxPreloadConfig(COLOR_TIMx, TIM_OCPreload_Enable);                        

    // 使能通道和预装载
  COLOR_BLUE_TIM_OCxInit(COLOR_TIMx, &TIM_OCInitStructure);                                         
  COLOR_BLUE_TIM_OCxPreloadConfig(COLOR_TIMx, TIM_OCPreload_Enable);                        

  TIM_ARRPreloadConfig(COLOR_TIMx, ENABLE); // 使能COLOR_TIMx重载寄存器ARR
  TIM_Cmd(COLOR_TIMx, ENABLE); // 使能定时器        
```

本配置初始化了控制RGB灯用的定时器，它被配置为向上计数，TIM_Period被配置为255，即定时器每个时钟周期计数器加1，计数至255时溢出， 从0开始重新计数；而代码中的PWM通道配置，当计数器的值小于输出比较寄存器CCRx的值时，PWM通道输出低电平，点亮LED灯。 所以上述代码配置把输出脉冲的单个周期分成了256份（注意区分定时器的时钟周期和输出脉冲周期）， 而输出比较寄存器CCRx配置的值即该脉冲周期内LED灯点亮的时间份数，所以修改CCRx的值，即可控制输出[0:255]种亮度等级。

关于定时器中的TIM_Prescaler分频配置，只要让它设置使得PWM控制脉冲的频率足够高，让人看不出LED灯闪烁即可，您可以亲自修改使用其它参数测试。

- 呼吸灯初始化

```c
// COLOR_TIMx 呼吸灯初始化
void COLOR_TIMx_LED_Init(void)
{
    COLOR_TIMx_GPIO_Config();
    COLOR_TIMx_Mode_Config();    
}
```

- 设置彩灯颜色

```c
// 设置RGB LED的颜色
void SetRGBColor(uint32_t rgb)
{
    // 根据颜色值修改定时器的比较寄存器值
    COLOR_TIMx->COLOR_RED_CCRx = (uint8_t)(rgb>>16);    //R
    COLOR_TIMx->COLOR_GREEN_CCRx = (uint8_t)(rgb>>8);    //G     
    COLOR_TIMx->COLOR_BLUE_CCRx = (uint8_t)rgb;                //B
}


// 设置RGB LED的颜色
void SetColorValue(uint8_t r,uint8_t g,uint8_t b)
{
    //根据颜色值修改定时器的比较寄存器值
    COLOR_TIMx->COLOR_RED_CCRx = r;    
    COLOR_TIMx->COLOR_GREEN_CCRx = g;            
    COLOR_TIMx->COLOR_BLUE_CCRx = b;    
}
```

- 停止PWM输出

```c
//停止pwm输出
void COLOR_TIMx_LED_Close(void)
{
    SetColorValue(0,0,0); // 关闭LED灯
    TIM_Cmd(COLOR_TIMx, DISABLE); // 失能定时器                        
    COLOR_TIM_APBxClock_FUN(COLOR_TIM_CLK, DISABLE); // 失能定时器时钟
    //关闭LED灯
    GPIO_SetBits(COLOR_RED_TIM_LED_PORT,COLOR_RED_TIM_LED_PIN);
    GPIO_SetBits(COLOR_GREEN_TIM_LED_PORT,COLOR_GREEN_TIM_LED_PIN);
    GPIO_SetBits(COLOR_BLUE_TIM_LED_PORT,COLOR_BLUE_TIM_LED_PIN);
}
```

- 主函数

```c
int main(void)
{            
    COLOR_TIMx_LED_Init();

    while(1)
    {
        // 显示各种颜色
        SetRGBColor(0x8080ff);
        SOFT_DELAY();

        SetRGBColor(0xff8000);
        SOFT_DELAY();

        SetRGBColor(0xffc90e);
        SOFT_DELAY();

        SetColorValue(181,230,29);
        SOFT_DELAY();

        SetColorValue(255,128,64);
        SOFT_DELAY();        

    }        
}
```

main函数中直接调用了COLOR_TIMx_LED_Init函数，而该函数内部又直接调用了前面讲解的GPIO和PWM配置函数：COLOR_TIMx_GPIO_Config和COLOR_TIMx_Mode_Config。 初始化完成后，在while循环中调用SetRGBColor和SetColorValue切换RGB灯显示的颜色。


