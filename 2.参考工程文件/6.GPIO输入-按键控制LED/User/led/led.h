#ifndef __LED_H__
#define __LED_H__
#include "stm32f10x.h"

// LED初始化函数
void LED_Init(void);

// LED引脚宏定义
#define LED1_GPIO_PORT GPIOB
#define LED1_GPIO_PIN GPIO_Pin_5
#define LED1_GPIO_CLK RCC_APB2Periph_GPIOB
#define LED2_GPIO_PORT GPIOB
#define LED2_GPIO_PIN GPIO_Pin_0
#define LED2_GPIO_CLK RCC_APB2Periph_GPIOB
#define LED3_GPIO_PORT GPIOB
#define LED3_GPIO_PIN GPIO_Pin_1
#define LED3_GPIO_CLK RCC_APB2Periph_GPIOB

// 控制LED宏定义
#define LED1_ON() GPIO_SetBits(LED1_GPIO_PORT, LED1_GPIO_PIN)
#define LED1_OFF() GPIO_ResetBits(LED1_GPIO_PORT, LED1_GPIO_PIN)
#define LED2_ON() GPIO_SetBits(LED2_GPIO_PORT, LED2_GPIO_PIN)
#define LED2_OFF() GPIO_ResetBits(LED2_GPIO_PORT, LED2_GPIO_PIN)
#define LED3_ON() GPIO_SetBits(LED3_GPIO_PORT, LED3_GPIO_PIN)
#define LED3_OFF() GPIO_ResetBits(LED3_GPIO_PORT, LED3_GPIO_PIN)

// 基本混色
#define LED_RED()   {LED1_ON(); LED2_OFF(); LED3_OFF();}
#define LED_GREEN() {LED1_OFF(); LED2_ON(); LED3_OFF();}
#define LED_BLUE()  {LED1_OFF(); LED2_OFF(); LED3_ON();}
#define LED_YELLOW() {LED1_ON(); LED2_ON(); LED3_OFF();}
#define LED_PURPLE() {LED1_ON(); LED2_OFF(); LED3_ON();}
#define LED_CYAN()  {LED1_OFF(); LED2_ON(); LED3_ON();}
#define LED_WHITE() {LED1_ON(); LED2_ON(); LED3_ON();}
#define LED_RGBOFF() {LED1_OFF(); LED2_OFF(); LED3_OFF();}

#endif 
