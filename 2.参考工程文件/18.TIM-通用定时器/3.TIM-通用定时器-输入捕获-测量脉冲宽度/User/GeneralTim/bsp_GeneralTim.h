#ifndef __BSP_GENERALTIME_H
#define __BSP_GENERALTIME_H


#include "stm32f10x.h"

/************通用定时器TIM参数定义，只限TIM2、3、4、5************/
// 当使用不同的定时器的时候，对应的GPIO是不一样的，这点要注意
// 我们这里默认使用TIM5
#define            GENERAL_TIM                   TIM5                   // 通用定时器选择-TIM5
#define            GENERAL_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd // 使能时钟
#define            GENERAL_TIM_CLK               RCC_APB1Periph_TIM5    // 时钟选择
#define            GENERAL_TIM_PERIOD            0XFFFF                 // 自动重装载值
#define            GENERAL_TIM_PSC               (72-1)                 // 时钟预分频系数

// TIM 输入捕获通道GPIO相关宏定义
#define            GENERAL_TIM_CH1_GPIO_CLK      RCC_APB2Periph_GPIOA  // 通道1 GPIO时钟
#define            GENERAL_TIM_CH1_PORT          GPIOA                  // 通道1 GPIO端口
#define            GENERAL_TIM_CH1_PIN           GPIO_Pin_0             // 通道1 GPIO引脚
#define            GENERAL_TIM_CHANNEL_x         TIM_Channel_1          // 通道选择

// 中断相关宏定义
#define            GENERAL_TIM_IT_CCx            TIM_IT_CC1             // 输入捕获中断
#define            GENERAL_TIM_IRQ               TIM5_IRQn              // 通用定时器中断
#define            GENERAL_TIM_INT_FUN           TIM5_IRQHandler        // 通用定时器中断服务函数

// 获取捕获寄存器值函数宏定义
#define            GENERAL_TIM_GetCapturex_FUN                 TIM_GetCapture1
// 捕获信号极性函数宏定义
#define            GENERAL_TIM_OCxPolarityConfig_FUN           TIM_OC1PolarityConfig

// 测量的起始边沿
#define            GENERAL_TIM_STRAT_ICPolarity                TIM_ICPolarity_Rising
// 测量的结束边沿
#define            GENERAL_TIM_END_ICPolarity                  TIM_ICPolarity_Falling

// 定时器输入捕获用户自定义变量结构体声明
typedef struct
{   
	uint8_t   Capture_FinishFlag;   // 捕获结束标志位
	uint8_t   Capture_StartFlag;    // 捕获开始标志位
	uint16_t  Capture_CcrValue;     // 捕获寄存器的值
	uint16_t  Capture_Period;       // 自动重装载寄存器更新标志 
}TIM_ICUserValueTypeDef;

extern TIM_ICUserValueTypeDef TIM_ICUserValueStructure;

void GENERAL_TIM_Init(void);

#endif