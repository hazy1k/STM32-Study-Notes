# 第十五章 ATIM介绍及应用

## 1. 高级定时器简介

高级控制定时器 (TIM1 和 TIM8) 和通用定时器在基本定时器的基础上引入了外部引脚，可以实现输入捕获和输出比较功能。高级控制定时器时基单元包含一个 16 位自动重装载寄存器 ARR，一个 16 位的计数器 CNT，可向上/下计数，一个 16 位可编程预分频器 PSC，预分频器时钟源有多种可选，有内部的时钟、外部时钟。还有一个 8 位的重复计数器 RCR，这样最高可实现 40 位的可编程定时。

![屏幕截图 2025-07-07 234122.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/07/07-23-41-29-屏幕截图%202025-07-07%20234122.png)

## 2. 定时器初始化结构体

### 2.1 TIM_TimeBaseInitTypeDef

时基结构体 TIM_TimeBaseInitTypeDef 用于定时器基础参数设置，与 TIM_TimeBaseInit 函数配合使用完成配置。

```c
typedef struct {
    uint16_t TIM_Prescaler; // 预分频器
    uint16_t TIM_CounterMode; // 计数模式
    uint32_t TIM_Period; // 定时器周期
    uint16_t TIM_ClockDivision; // 时钟分频
    uint8_t TIM_RepetitionCounter; // 重复计算器
}TIM_TimeBaseInitTypeDef;
```

1. TIM_Prescaler：定时器预分频器设置，时钟源经该预分频器才是定时器计数时钟 CK_CNT，它设定 PSC 寄存器的值。计算公式为：计数器时钟频率 (fCK_CNT) 等于 fCK_PSC / (PSC[15:0] + 1)，可实现 1 至 65536 分频。

2. TIM_CounterMode：定时器计数方式，可设置为向上计数、向下计数以及中心对齐。高级控制定时器允许选择任意一种。

3. TIM_Period：定时器周期，实际就是设定自动重载寄存器 ARR 的值， ARR 为要装载到实际自动重载寄存器（即影子寄存器）的值，可设置范围为 0 至 65535。

4. TIM_ClockDivision：时钟分频，设置定时器时钟 CK_INT 频率与死区发生器以及数字滤波器采样时钟频率分频比。可以选择 1、 2、 4 分频。

5. TIM_RepetitionCounter：重复计数器，只有 8 位，只存在于高级定时器。

### 2.2 TIM_OCInitTypeDef

输出比较结构体 TIM_OCInitTypeDef 用于输出比较模式，与 TIM_OCxInit 函数配合使用完成指定定时器输出通道初始化配置。高级控制定时器有四个定时器通道，使用时都必须单独设置。

```c
typedef struct {
    uint16_t TIM_OCMode; // 比较输出模式
    uint16_t TIM_OutputState; // 比较输出使能
    uint16_t TIM_OutputNState; // 比较互补输出使能
    uint32_t TIM_Pulse; // 脉冲宽度
    uint16_t TIM_OCPolarity; // 输出极性
    uint16_t TIM_OCNPolarity; // 互补输出极性
    uint16_t TIM_OCIdleState; // 空闲状态下比较输出状态
    uint16_t TIM_OCNIdleState; // 空闲状态下比较互补输出状态
} TIM_OCInitTypeDef;
```

1. TIM_OCMode：比较输出模式选择，总共有八种，常用的为 PWM1/PWM2。它设定 CCMRx 寄存器 OCxM[2:0] 位的值

2. TIM_OutputState：比较输出使能，决定最终的输出比较信号 OCx 是否通过外部引脚输出。它设定 TIMx_CCER 寄存器 CCxE/CCxNE 位的值。

3. TIM_OutputNState: 比较互补输出使能，决定 OCx 的互补信号 OCxN 是否通过外部引脚输出。它设定 CCER 寄存器 CCxNE 位的值。

4. TIM_Pulse：比较输出脉冲宽度，实际设定比较寄存器 CCR 的值，决定脉冲宽度。可设置范围为 0 至 65535。

5. TIM_OCPolarity：比较输出极性，可选 OCx 为高电平有效或低电平有效。它决定着定时器通道有效电平。它设定 CCER 寄存器的 CCxP 位的值。

6. TIM_OCNPolarity：比较互补输出极性，可选 OCxN 为高电平有效或低电平有效。它设定TIMx_CCER 寄存器的 CCxNP 位的值。

7. TIM_OCIdleState：空闲状态时通道输出电平设置，可选输出 1 或输出 0，即在空闲状态(BDTR_MOE 位为 0) 时，经过死区时间后定时器通道输出高电平或低电平。它设定 CR2 寄存器的 OISx 位的值。

8. TIM_OCNIdleState：空闲状态时互补通道输出电平设置，可选输出 1 或输出 0，即在空闲状态(BDTR_MOE 位为 0) 时，经过死区时间后定时器互补通道输出高电平或低电平，设定值必须与TIM_OCIdleState 相反。它设定是 CR2 寄存器的 OISxN 位的值。

### 2.3 TIM_ICInitTypeDef

输入捕获结构体 TIM_ICInitTypeDef 用于输入捕获模式，与 TIM_ICInit 函数配合使用完成定时器输入通道初始化配置。如果使用 PWM 输入模式需要与 TIM_PWMIConfig 函数配合使用完成定时器输入通道初始化配置。

```c
typedef struct {
    uint16_t TIM_Channel; // 输入通道选择
    uint16_t TIM_ICPolarity; // 输入捕获触发选择
    uint16_t TIM_ICSelection; // 输入捕获选择
    uint16_t TIM_ICPrescaler; // 输入捕获预分频器
    uint16_t TIM_ICFilter; // 输入捕获滤波器
}TIM_ICInitTypeDef;
```

1. TIM_Channel：捕获通道 ICx 选择，可选 TIM_Channel_1、 TIM_Channel_2、 TIM_Channel_3 或TIM_Channel_4 四个通道。它设定 CCMRx 寄存器 CCxS 位的值。

2. TIM_ICPolarity：输入捕获边沿触发选择，可选上升沿触发、下降沿触发或边沿跳变触发。它设定 CCER 寄存器 CCxP 位和 CCxNP 位的值。

3. TIM_ICSelection：输入通道选择，捕获通道 ICx 的信号可来自三个输入通道，分别为TIM_ICSelection_DirectTI、 TIM_ICSelection_IndirectTI 或 TIM_ICSelection_TRC，具体的区别见图输入通道与捕获通道 IC 的映射图 。如果是普通的输入捕获， 4 个通道都可以使用，如果是 PWM输入则只能使用通道 1 和通道 2。它设定 CCRMx 寄存器的 CCxS[1:0] 位的值。

![屏幕截图 2025-07-07 235406.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/07/07-23-54-13-屏幕截图%202025-07-07%20235406.png)

4. TIM_ICPrescaler：输入捕获通道预分频器，可设置 1、 2、 4、 8 分频，它设定 CCMRx 寄存器的 ICxPSC[1:0] 位的值。如果需要捕获输入信号的每个有效边沿，则设置 1 分频即可。

5. TIM_ICFilter：输入捕获滤波器设置，可选设置 0x0 至 0x0F。它设定 CCMRx 寄存器 ICxF[3:0]位的值。一般我们不使用滤波器，即设置为 0。

### 2.4 TIM_BDTRInitTypeDef

断路和死区结构体 TIM_BDTRInitTypeDef 用于断路和死区参数的设置，属于高级定时器专用，用于配置断路时通道输出状态，以及死区时间。它与 TIM_BDTRConfig 函数配置使用完成参数配置。

```c
typedef struct {
    uint16_t TIM_OSSRState; // 运行模式下的关闭状态选择
    uint16_t TIM_OSSIState; // 空闲模式下的关闭状态选择
    uint16_t TIM_LOCKLevel; // 锁定配置
    uint16_t TIM_DeadTime; // 死区时间
    uint16_t TIM_Break; // 断路输入使能控制
    uint16_t TIM_BreakPolarity; // 断路输入极性
    uint16_t TIM_AutomaticOutput; // 自动输出使能
}TIM_BDTRInitTypeDef;
```

1. TIM_OSSRState：运行模式下的关闭状态选择，它设定 BDTR 寄存器 OSSR 位的值。

2. TIM_OSSIState：空闲模式下的关闭状态选择，它设定 BDTR 寄存器 OSSI 位的值。

3. TIM_LOCKLevel：锁定级别配置， BDTR 寄存器 LOCK[1:0] 位的值。

4. TIM_DeadTime：配置死区发生器，定义死区持续时间，可选设置范围为 0x0 至 0xFF。它设定BDTR 寄存器 DTG[7:0] 位的值。

5. TIM_Break：断路输入功能选择，可选使能或禁止。它设定 BDTR 寄存器 BKE 位的值。

6. TIM_BreakPolarity：断路输入通道 BRK 极性选择，可选高电平有效或低电平有效。它设定BDTR 寄存器 BKP 位的值。

7. TIM_AutomaticOutput：自动输出使能，可选使能或禁止，它设定 BDTR 寄存器 AOE 位的值。

## 3. 高级定时器使用示例

### 3.1 PWM互补输出

输出比较模式比较多，这里我们以 PWM 输出为例讲解，并通过示波器来观察波形。实验中不仅在主输出通道输出波形，还在互补通道输出与主通道互补的的波形，并且添加了断路和死区功能。

使用高级定时器 TIM1 的通道 1及其互补通道作为本实验的波形输出通道，对应选择 PA8 和 PB13 引脚。将示波器的两个输入通道分别与 PA8 和 PB13 引脚连接，用于观察波形

为增加断路功能，需要用到 TIM1_BKIN 引脚，这里选择 PB12 引脚。程序我们设置该引脚为高电平有效，当 BKIN 引脚被置高电平的时候，两路互补的 PWM 输出就被停止，就好像是刹车一样。

#### 3.1.1 计算死区时间

```c
// 计算死区时间 (单位: ns)
static uint32_t calc_deadtime(uint8_t dt)
{
    // 系统时钟频率 (Hz)
    uint32_t system_clk = SystemCoreClock;
    // 定时器时钟频率 (Hz)
    uint32_t tim_clk = system_clk / (TIM1->PSC + 1);
    // 基本时间单位 (ns)
    float t_dts = 1000000000.0f / tim_clk;
    return (uint32_t)(dt * t_dts);
}
```

#### 3.1.2 TIM GPIO初始化

```c
static void ATIM_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // PA8: TIM1_CH1 (PWM主输出)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // PB13: TIM1_CH1N (互补输出)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // PB12: 刹车输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;  // 输入上拉模式
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}
```

#### 3.1.3 TIM工作模式配置

```c
void ATIM_Mode_Init(void)
{
    ATIM_GPIO_Init();
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    
    /* 时基单元配置 */
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 7;          // ARR = 7
    TIM_TimeBaseStructure.TIM_Prescaler = 8;        // PSC = 8
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
    
    /* 输出比较配置 */
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 4;  // 50%占空比
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    
    /* 刹车和死区配置 */
    TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
    TIM_BDTRStructInit(&TIM_BDTRInitStructure);
    TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
    TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
    TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
    TIM_BDTRInitStructure.TIM_DeadTime = 11;  // 死区时间值
    TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
    TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
    TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
    TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);
    
    // 使能定时器
    TIM_Cmd(TIM1, ENABLE);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    
    // 清除中断标志
    TIM_ClearFlag(TIM1, TIM_FLAG_Update | TIM_FLAG_Break);
}
```

#### 3.1.4 TIM调试信息

```c
void TIM1_DebugInfo(void)
{
    // 计算实际输出频率和占空比
    uint32_t system_clk = SystemCoreClock;
    uint32_t tim_clk = system_clk / (TIM1->PSC + 1);
    float output_freq = (float)tim_clk / (TIM1->ARR + 1);
    float duty_cycle = (TIM1->CCR1 * 100.0f) / (TIM1->ARR + 1);
    uint32_t deadtime_ns = calc_deadtime(TIM1->BDTR & 0xFF);
    
    printf("\r\n===== TIM1 Configuration =====\r\n");
    printf("System Clock: %u Hz\r\n", system_clk);
    printf("TIM1 Clock: %u Hz\r\n", tim_clk);
    printf("PSC: %u\r\n", TIM1->PSC);
    printf("ARR: %u\r\n", TIM1->ARR);
    printf("CCR1: %u\r\n", TIM1->CCR1);
    printf("Output Freq: %.1f Hz\r\n", output_freq);
    printf("Duty Cycle: %.1f%%\r\n", duty_cycle);
    printf("Dead Time: %u ns\r\n", deadtime_ns);
    printf("Break Input (PB12): %s\r\n", 
           GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) ? "High" : "Low");
    printf("Break Flag: %s\r\n", 
           TIM_GetFlagStatus(TIM1, TIM_FLAG_Break) ? "Active" : "Inactive");
    printf("MOE Status: %s\r\n", 
           (TIM1->BDTR & TIM_BDTR_MOE) ? "Enabled" : "Disabled");
    printf("TIM1 Status: %s\r\n", 
           (TIM1->CR1 & TIM_CR1_CEN) ? "Enabled" : "Disabled");
    printf("BDTR Register: 0x%04X\r\n", TIM1->BDTR);
    printf("=============================\r\n");
}

```

#### 3.1.5 主函数测试

```c
#include "stm32f10x.h"
#include "tim.h"
#include "systick.h"
#include "usart.h"
#include <stdio.h>

int main(void)
{   
    SysTick_Init();
    USARTx_Init(115200);
    printf("\r\nSystem Booted\r\n");
    printf("TIM1 Advanced Timer Demo\r\n");
    printf("For break function test:\r\n");
    printf("  - Connect PB12 to 3.3V to trigger break\r\n");
    printf("  - Disconnect PB12 to clear break flag\r\n");
    ATIM_Mode_Init();
    TIM1_DebugInfo();
    while(1) {
        printf("\r\n===== Main Menu =====\r\n");
        printf("1. Read TIM1 Status\r\n");
        printf("2. Test Break Function (connect PB12 to 3.3V)\r\n");
        printf("3. Reset Break Flag (disconnect PB12 from 3.3V)\r\n");
        printf("4. Re-enable PWM Output\r\n");
        printf("Enter choice: ");
        char choice = USART_ReceiveChar();
        printf("\r\nSelected: %c\r\n", choice);

        switch(choice) {
            case '1':  // 读取状态
                TIM1_DebugInfo();
                break;

            case '2':  // 测试刹车功能
                printf("Testing Break Function...\r\n");
                printf("Current PB12 state: %s\r\n", 
                       GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) ? "HIGH" : "LOW");
                
                if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)) {
                    printf("PB12 is already HIGH. Break should be active.\r\n");
                } else {
                    printf("Please connect PB12 to 3.3V to trigger break...\r\n");
                    // 等待用户操作
                    uint32_t start = SysTick_GetTick();
                    while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) && 
                          (SysTick_GetTick() - start < 5000)) {
                        // 等待PB12变高或超时(5秒)
                    }
                    
                    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)) {
                        printf("PB12 is now HIGH. Break should be active.\r\n");
                    } else {
                        printf("Timeout. PB12 still LOW.\r\n");
                    }
                }
                
                // 检查刹车状态
                if(TIM_GetFlagStatus(TIM1, TIM_FLAG_Break)) {
                    printf("Break Event Detected!\r\n");
                } else {
                    printf("No Break Event Detected! Check PB12 connection.\r\n");
                }
                
                TIM1_DebugInfo();
                break;
                
            case '3':  // 清除刹车标志
                printf("Clearing Break Flag...\r\n");
                printf("Current PB12 state: %s\r\n", 
                       GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) ? "HIGH" : "LOW");
                
                if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)) {
                    printf("PB12 is still HIGH. Please disconnect PB12 from 3.3V...\r\n");
                    // 等待用户操作
                    uint32_t start = SysTick_GetTick();
                    while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) && 
                          (SysTick_GetTick() - start < 5000)) {
                        // 等待PB12变低或超时(5秒)
                    }
                }
                
                if(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)) {
                    TIM_ClearFlag(TIM1, TIM_FLAG_Break);
                    Delay_ms(10);  // 稍等片刻让标志位更新
                    
                    if(TIM_GetFlagStatus(TIM1, TIM_FLAG_Break) == RESET) {
                        printf("Break flag cleared.\r\n");
                    } else {
                        printf("Failed to clear break flag!\r\n");
                    }
                } else {
                    printf("PB12 still HIGH. Cannot clear break flag.\r\n");
                }
                
                TIM1_DebugInfo();
                break;
                
            case '4':  // 重新使能PWM输出
                printf("Re-enabling PWM Output...\r\n");
                
                // 确保刹车标志已清除
                if(TIM_GetFlagStatus(TIM1, TIM_FLAG_Break)) {
                    printf("Break flag still active! Clear it first.\r\n");
                    TIM1_DebugInfo();
                    break;
                }
                
                // 重新配置BDTR（确保所有配置正确）
                TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
                TIM_BDTRStructInit(&TIM_BDTRInitStructure);
                TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
                TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
                TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
                TIM_BDTRInitStructure.TIM_DeadTime = 11;
                TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
                TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
                TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
                TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);
                
                // 使能主输出
                TIM1->BDTR |= TIM_BDTR_MOE;
                TIM_CtrlPWMOutputs(TIM1, ENABLE);
                
                printf("PWM output re-enabled.\r\n");
                TIM1_DebugInfo();
                break;
                
            default:
                printf("Invalid choice!\r\n");
        }
        Delay_ms(1000);
    }
}

```

### 3.2 脉宽测量输入
