# 第三十三章 高级定时器-PWM输入捕获实验

## 1. 导入

普通的输入捕获可以使用定时器的四个通道， 一路捕获占用一个捕获寄存器，而PWM输入则只能使用两个通道，即通道1和通道2，且一路PWM输入要占用两个捕获寄存器，一个用于捕获周期， 一个用于捕获占空比。在本节实验中，我们用通用定时器产生一路PWM信号，然后用高级定时器的通道1或者通道2来捕获。

## 2. 硬件设计

实验中用到两个引脚，一个是通用定时器TIM3的通道1，即PA6，用于输出PWM信号，另一个是高级控制定时器TIM1的通道1， 即PA8，用于PWM输入捕获，实验中直接用一根杜邦线短接即可PA6和PA8即可，同时可用示波器监控PA6的波形，看看实验捕获的数据是否正确。

## 3. 软件设计

### 3.1 编程目的

1. 通用定时器产生PWM配置

2. 高级定时器PWM输入配置

3. 编写中断服务程序，计算测量的频率和占空比，并打印出来比较

编程的要点主要分成两部分，一个是通用定时器的PWM信号输出，另一个是PWM信号输入捕获。

### 3.2 代码分析

- 通用定时器宏定义

```c
/************通用定时器TIM参数定义，只限TIM2、3、4、5************/
// 当使用不同的定时器的时候，对应的GPIO是不一样的，这点要注意
// 我们这里默认使用TIM3

#define            GENERAL_TIM                   TIM3
#define            GENERAL_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            GENERAL_TIM_CLK               RCC_APB1Periph_TIM3
// 输出PWM的频率为 72M/{ (ARR+1)*(PSC+1) }
#define            GENERAL_TIM_PERIOD            (10-1)
#define            GENERAL_TIM_PSC               (72-1)

#define            GENERAL_TIM_CCR1              5

// TIM3 输出比较通道1
#define            GENERAL_TIM_CH1_GPIO_CLK      RCC_APB2Periph_GPIOA
#define            GENERAL_TIM_CH1_PORT          GPIOA
#define            GENERAL_TIM_CH1_PIN           GPIO_Pin_6

```

使用宏定义非常方便程序升级、移植。通过上面的宏，我们可以算出PWM信号的频率F为：72M/( 10*72 )=100KHZ， 占空比为GENERAL_TIM_CCR1/ （GENERAL_TIM_PERIOD+1）= 50%。

- 通用定时器PWM输出

```c
static void GENERAL_TIM_Mode_Config(void)
{
    // 开启定时器时钟,即内部时钟CK_INT=72M
    GENERAL_TIM_APBxClock_FUN(GENERAL_TIM_CLK,ENABLE);

    /*--------------------时基结构体初始化-------------------------*/
    // 配置周期，这里配置为100K

    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    // 自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
    TIM_TimeBaseStructure.TIM_Period=GENERAL_TIM_PERIOD;
    // 驱动CNT计数器的时钟 = Fck_int/(psc+1)
    TIM_TimeBaseStructure.TIM_Prescaler= GENERAL_TIM_PSC;
    // 时钟分频因子 ，配置死区时间时需要用到
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    // 计数器计数模式，设置为向上计数
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
    // 重复计数器的值，没用到不用管
    TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
    // 初始化定时器
    TIM_TimeBaseInit(GENERAL_TIM, &TIM_TimeBaseStructure);

    /*--------------------输出比较结构体初始化-------------------*/
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    // 配置为PWM模式1
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    // 输出使能
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    // 输出通道电平极性配置
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    // 输出比较通道 1
    TIM_OCInitStructure.TIM_Pulse = GENERAL_TIM_CCR1;
    TIM_OC1Init(GENERAL_TIM, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(GENERAL_TIM, TIM_OCPreload_Enable);

    // 使能计数器
    TIM_Cmd(GENERAL_TIM, ENABLE);
}
```

GENERAL_TIM_Mode_Config()函数中初始化了两个结构体，有关这两个结构体成员的具体含义可参考“定时器初始化结构体详解”小节，剩下的程序参考注释阅读即可。

如果需要修改PWM的周期和占空比，修改头文件里面的GENERAL_TIM_PERIOD、GENERAL_TIM_PSC和GENERAL_TIM_CCR1这三个宏即可。 PWM信号的频率的计算公式为：F =TIM_CLK/{(ARR+1)*(PSC+1)}，其中TIM_CLK等于72MHZ，ARR即自动重装载寄存器的值， 对应GENERAL_TIM_PERIOD这个宏，PSC即计数器时钟的分频因子，对应GENERAL_TIM_PSC这个宏。

- 通用定时器初始化

```c
void GENERAL_TIM_Init(void)
{
    GENERAL_TIM_GPIO_Config();
    GENERAL_TIM_Mode_Config();
}
```

当调用函数GENERAL_TIM_Init()之后，相应的引脚就会输出PWM信号。

- 高级定时器宏定义

```c
#define            ADVANCE_TIM                   TIM1
#define            ADVANCE_TIM_APBxClock_FUN     RCC_APB2PeriphClockCmd
#define            ADVANCE_TIM_CLK               RCC_APB2Periph_TIM1

// 输入捕获能捕获到的最小的频率为 72M/{ (ARR+1)*(PSC+1) }
#define            ADVANCE_TIM_Period            (1000-1)
#define            ADVANCE_TIM_Prescaler         (72-1)

// 中断相关宏定义
#define            ADVANCE_TIM_IRQ               TIM1_CC_IRQn
#define            ADVANCE_TIM_IRQHandler        TIM1_CC_IRQHandler

// TIM1 输入捕获通道1
#define            ADVANCE_TIM_CH1_GPIO_CLK      RCC_APB2Periph_GPIOA
#define            ADVANCE_TIM_CH1_PORT          GPIOA
#define            ADVANCE_TIM_CH1_PIN           GPIO_Pin_8

#define            ADVANCE_TIM_IC1PWM_CHANNEL    TIM_Channel_1
```

在上面的宏定义里面，我们可以算出计数器的计数周期为T=72M/(1000*72)=1MS，这个是定时器在不溢出的情况下的最大计数周期， 也就是说周期小于1ms的PWM信号都可以被捕获到，转换成频率就是能捕获到的最小的频率为1KHZ。 所以我们要根据捕获的PWM信号来调节ADVANCE_TIM_PERIOD和ADVANCE_TIM_PSC这两个宏。

- 高级定时器PWM输入模式

```c
static void ADVANCE_TIM_Mode_Config(void)
{
    // 开启定时器时钟,即内部时钟CK_INT=72M
    ADVANCE_TIM_APBxClock_FUN(ADVANCE_TIM_CLK,ENABLE);

    /*--------------------时基结构体初始化-------------------------*/
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    // 自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
    TIM_TimeBaseStructure.TIM_Period=ADVANCE_TIM_PERIOD;
    // 驱动CNT计数器的时钟 = Fck_int/(psc+1)
    TIM_TimeBaseStructure.TIM_Prescaler= ADVANCE_TIM_PSC;
    // 时钟分频因子 ，配置死区时间时需要用到
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    // 计数器计数模式，设置为向上计数
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
    // 重复计数器的值，没用到不用管
    TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
    // 初始化定时器
    TIM_TimeBaseInit(ADVANCE_TIM, &TIM_TimeBaseStructure);

    /*--------------------输入捕获结构体初始化-------------------*/
    // 使用PWM输入模式时，需要占用两个捕获寄存器，一个测周期，另外一个测占空比

    TIM_ICInitTypeDef  TIM_ICInitStructure;
    // 捕获通道IC1配置
    // 选择捕获通道
    TIM_ICInitStructure.TIM_Channel = ADVANCE_TIM_IC1PWM_CHANNEL;
    // 设置捕获的边沿
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    // 设置捕获通道的信号来自于哪个输入通道，有直连和非直连两种
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    // 1分频，即捕获信号的每个有效边沿都捕获
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    // 不滤波
    TIM_ICInitStructure.TIM_ICFilter = 0x0;
    // 初始化PWM输入模式
    TIM_PWMIConfig(ADVANCE_TIM, &TIM_ICInitStructure);

    // 当工作做PWM输入模式时,只需要设置触发信号的那一路即可（用于测量周期）
    // 另外一路（用于测量占空比）会由硬件自带设置，不需要再配置

    // 捕获通道IC2配置
    //  TIM_ICInitStructure.TIM_Channel = ADVANCE_TIM_IC1PWM_CHANNEL;
    //  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
    //  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_IndirectTI;
    //  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    //  TIM_ICInitStructure.TIM_ICFilter = 0x0;
    //  TIM_PWMIConfig(ADVANCE_TIM, &TIM_ICInitStructure);

    // 选择输入捕获的触发信号
    TIM_SelectInputTrigger(ADVANCE_TIM, TIM_TS_TI1FP1);

    // 选择从模式: 复位模式
    // PWM输入模式时,从模式必须工作在复位模式，当捕获开始时,计数器CNT会被复位
    TIM_SelectSlaveMode(ADVANCE_TIM, TIM_SlaveMode_Reset);
    TIM_SelectMasterSlaveMode(ADVANCE_TIM,TIM_MasterSlaveMode_Enable);

    // 使能捕获中断,这个中断针对的是主捕获通道（测量周期那个）
    TIM_ITConfig(ADVANCE_TIM, TIM_IT_CC1, ENABLE);
    // 清除中断标志位
    TIM_ClearITPendingBit(ADVANCE_TIM, TIM_IT_CC1);

    // 使能高级控制定时器，计数器开始计数
    TIM_Cmd(ADVANCE_TIM, ENABLE);
}
```

ADVANCE_TIM_Mode_Config()函数中初始化了两个结构体，有关这两个结构体成员的具体含义可参考“定时器初始化结构体详解”小节，剩下的程序参考注释阅读即可。

因为是PWM输入模式，只能使用通道1和通道2 ，假如我们使用的是通道1，即TI1，输入的PWM信号会被分成两路，分别是TI1FP1和TI1FP2， 两路都可以是触发信号。如果选择TI1FP1为触发信号，那么IC1捕获到的是PWM信号的周期， IC2捕获到的是占空比， 这种输入通道TI和捕获通道IC的映射关系叫直连，输入捕获结构体的TIM_ICSelection要配置为TIM_ICSelection_DirectTI。如果选择TI1FP2为触发信号， 则IC2捕获到的是周期，IC1捕获到的是占空比，这种输入通道TI和捕获通道IC的映射关系叫非直连， 输入捕获结构体的TIM_ICSelection要配置为TIM_ICSelection_IndirectTI。 有关输入通道TI和捕获通道IC的具体映射关系见图 [输入通道TI和捕获通道IC的映射图](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/TIM_senior.html#tiic) ，有直连和非直连两种。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/TIMsen012.png)

- 高级定时器中断优先级

```c
static void ADVANCE_TIM_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    // 设置中断组为0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    // 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = ADVANCE_TIM_IRQ;
    // 设置抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    // 设置子优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
```

因为只有一个中断源，优先级可以随便配置。

- 高级定时器中断服务函数

```c
void ADVANCE_TIM_IRQHandler(void)
{
    /* 清除中断标志位 */
    TIM_ClearITPendingBit(ADVANCE_TIM, TIM_IT_CC1);

    /* 获取输入捕获值 */
    IC1Value = TIM_GetCapture1(ADVANCE_TIM);
    IC2Value = TIM_GetCapture2(ADVANCE_TIM);

    // 注意：捕获寄存器CCR1和CCR2的值在计算占空比和频率的时候必须加1
    if (IC1Value != 0) {
        /* 占空比计算 */
        DutyCycle = (float)((IC2Value+1) * 100) / (IC1Value+1);

        /* 频率计算 */
        Frequency = (72000000/(ADVANCE_TIM_PSC+1))/(float)(IC1Value+1);
        printf("占空比：%0.2f%%   频率：%0.2fHz\n",DutyCycle,Frequency);
    } else {
        DutyCycle = 0;
        Frequency = 0;
    }
}
```

当捕获到PWM信号的第一个上升沿时，产生中断，计数器被复位，锁存到捕获寄存器IC1和IC2的值都为0。当下降沿到来时， IC2会捕获，对应的是占空比，但是会产生中断。当捕获到第二个下降沿时，IC1会捕获，对应的是周期，而且会再次进入中断， 这个时间就可以根据IC1和IC2的值计算出频率和占空比。有关PWM输入的时序见图 [PWM输入模式时序图](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/book/TIM_senior.html#id83) 。

中断复位函数中，我们获取输入捕获寄存器CCR1和CCR2寄存器中的值，当CCR1的值不为0时，说明有效捕获到了一个周期， 然后计算出频率和占空比。在计算的时候CCR1和CCR2的值都必须要加1，因为计数器是从0开始计数的。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/TIMsen013.png)

- 主函数

```c
int main(void)
{
    /* 串口初始化 */
    USART_Config();

    /* 通用定时器初始化，用于生成PWM信号 */
    GENERAL_TIM_Init();

    /* 高级定时器初始化 ，用户捕获PWM信号*/
    ADVANCE_TIM_Init();

    while (1) {
    }
}
```

main函数非常简单，通用定时器初始化完之后用于输出PWM信号，高级定时器初始化完之后用于捕获通用定时器输出的PWM信号。
