#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"

#define ON 1
#define OFF 0

// 宏定义led相关参数
#define LED1_GPIO_PORT GPIOB
#define LED1_GPIO_CLK RCC_APB2Periph_GPIOB
#define LED1_GPIO_PIN GPIO_Pin_5 // PB5
#define LED2_GPIO_PORT GPIOB
#define LED2_GPIO_CLK RCC_APB2Periph_GPIOB
#define LED2_GPIO_PIN GPIO_Pin_0 // PB0
#define LED3_GPIO_PORT GPIOB
#define LED3_GPIO_CLK RCC_APB2Periph_GPIOB
#define LED3_GPIO_PIN GPIO_Pin_1 // PB1
#define LED_GPIO_Mode  GPIO_Mode_Out_PP // 推挽输出模式

// LED控制宏定义
#define LED1(a) do { \
                    if(a) \
                        GPIO_SetBits(LED1_GPIO_PORT, LED1_GPIO_PIN); \
                    else \
                        GPIO_ResetBits(LED1_GPIO_PORT, LED1_GPIO_PIN); \
                } while(0)

#define LED2(a) do { \
                    if(a) \
                        GPIO_SetBits(LED2_GPIO_PORT, LED2_GPIO_PIN); \
                    else \
                        GPIO_ResetBits(LED2_GPIO_PORT, LED2_GPIO_PIN); \
                } while(0)

#define LED3(a) do { \
                    if(a) \
                        GPIO_SetBits(LED3_GPIO_PORT, LED3_GPIO_PIN); \
                    else \
                        GPIO_ResetBits(LED3_GPIO_PORT, LED3_GPIO_PIN); \
                } while(0)

/* 直接操作寄存器的方法控制IO */
#define digitalHi(p,i)       {p->BSRR=i;}    // 输出为高电平
#define digitalLo(p,i)       {p->BRR=i;}     // 输出低电平
#define digitalToggle(p,i)   {p->ODR ^=i;}   // 输出反转状态

/* 定义控制IO的宏 */
#define LED1_TOGGLE       digitalToggle(LED1_GPIO_PORT,LED1_GPIO_PIN) // 状态反转
#define LED1_OFF          digitalHi(LED1_GPIO_PORT,LED1_GPIO_PIN) // 关闭-高电平
#define LED1_ON           digitalLo(LED1_GPIO_PORT,LED1_GPIO_PIN) // 打开-低电平
#define LED2_TOGGLE       digitalToggle(LED2_GPIO_PORT,LED2_GPIO_PIN)
#define LED2_OFF          digitalHi(LED2_GPIO_PORT,LED2_GPIO_PIN)
#define LED2_ON           digitalLo(LED2_GPIO_PORT,LED2_GPIO_PIN)
#define LED3_TOGGLE       digitalToggle(LED2_GPIO_PORT,LED3_GPIO_PIN)
#define LED3_OFF          digitalHi(LED2_GPIO_PORT,LED3_GPIO_PIN)
#define LED3_ON           digitalLo(LED2_GPIO_PORT,LED3_GPIO_PIN)

// 红
#define LED_RED LED1_ON; LED2_OFF; LED3_OFF
// 绿
#define LED_GREEN LED1_OFF; LED2_ON; LED3_OFF
// 蓝
#define LED_BLUE LED1_OFF; LED2_OFF; LED3_ON
// 黄(红+绿)
#define LED_YELLOW LED1_ON; LED2_ON; LED3_OFF
// 紫(红+蓝)
#define LED_PURPLE LED1_ON; LED2_OFF; LED3_ON
// 青(绿+蓝)
#define LED_CYAN LED1_OFF; LED2_ON; LED3_ON
// 白(红+绿+蓝)
#define LED_WHITE LED1_ON; LED2_ON; LED3_ON
// 黑(全部关闭)
#define LED_RGBOFF LED1_OFF; LED2_OFF; LED3_OFF

// 函数声明
void LED_GPIO_Init(void);

#endif
