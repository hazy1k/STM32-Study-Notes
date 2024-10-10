#ifndef __BASIC_TIM_H
#define	__BASIC_TIM_H

#include "stm32f7xx.h"

#define CW  0
#define CCW 1

#define TRUE 1
#define FALSE 0

#define Pulse_width 20

//系统状态
typedef struct{
  //当步进电机正在运行时，值为1
  unsigned char running:1;
  //当串口接收到数据时，值为1
  unsigned char cmd:1;
  //当驱动器正常输出时,值为1
  unsigned char out_ena:1;
} GLOBAL_FLAGS;

extern __IO GLOBAL_FLAGS status;
extern int stepPosition;

#define T1_FREQ 1000000     //定时器频率
#define FSPR    200         //步进电机单圈步数
#define SPR     (FSPR*2)  //100细分的步数
// 数学常数。 用于SMD_Move函数的简化计算
#define ALPHA (2*3.14159/SPR)                    // 2*pi/spr
#define A_T_x100 ((long)(ALPHA*T1_FREQ*100))     // (ALPHA / T1_FREQ)*100
#define T1_FREQ_148 ((int)((T1_FREQ*0.676)/100)) // divided by 100 and scaled by 0.676
#define A_SQ (long)(ALPHA*2*10000000000)         // 
#define A_x20000 (int)(ALPHA*20000)              // ALPHA*20000
    
//速度曲线状态
#define STOP  0
#define ACCEL 1
#define DECEL 2
#define RUN   3

typedef struct {
  //电机运行状态
  unsigned char run_state : 3;
  //电机运行方向
  unsigned char dir : 1;
  //下一个脉冲延时周期，启动时为加速度速率
  unsigned int step_delay;
  //开始减速的位置
  unsigned int decel_start;
  //减速距离
  signed int decel_val;
  //最小延时（即最大速度）
  signed int min_delay;
  //加速或者减速计数器
  signed int accel_count;
} speedRampData;

//TIM通道CH1的引脚宏定义
#define  SMD_PULSE_GPIO_CLK()             __GPIOC_CLK_ENABLE()
#define  SMD_PULSE_GPIO_PORT              GPIOC
#define  SMD_PULSE_GPIO_PIN               GPIO_PIN_9


//高级定时器宏定义
#define SMD_PULSE_TIM           		        TIM3
#define ADVANCE_TIM_CLK_ENABLE()       		__TIM3_CLK_ENABLE()
#define ADVANCE_TIM_CH                    TIM_CHANNEL_4

// 定时器中断
#define            SMD_PULSE_TIM_IRQ                TIM3_IRQn
#define            SMD_PULSE_TIM_IRQHandler         TIM3_IRQHandler

// PWM 信号的频率 F = TIM_CLK/{(ARR+1)*(PSC+1)}
#define            SMD_PULSE_TIM_PERIOD             (10)
#define            SMD_PULSE_TIM_PSC                (109)

// 步进电机方向控制
#define            SMD_DIR_GPIO_CLK()               __GPIOD_CLK_ENABLE()
#define            SMD_DIR_PORT                   GPIOD
#define            SMD_DIR_PIN                    GPIO_PIN_11

// 步进电机输出使能引脚
#define            SMD_ENA_GPIO_CLK()               __GPIOD_CLK_ENABLE()
#define            SMD_ENA_PORT                     GPIOD
#define            SMD_ENA_PIN                      GPIO_PIN_12


#define DIR(a)	if (a == CW)	\
					HAL_GPIO_WritePin(SMD_DIR_PORT,SMD_DIR_PIN,GPIO_PIN_RESET);\
					else		\
					HAL_GPIO_WritePin(SMD_DIR_PORT,SMD_DIR_PIN,GPIO_PIN_SET)
                    
/**************************函数声明********************************/

void SMD_Init(void);
void SMD_ENA(FunctionalState NewState);
void SMD_Move(signed int step, unsigned int accel, unsigned int decel, unsigned int speed);

void TIM_Advance_Init(void);

#endif /* __BASIC_TIM_H */

