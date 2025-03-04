#ifndef __LED_H
#define __LED_H
#include "stm32f10x.h"

// LED��ʼ������
void LED_Init(void);

// LED���ź궨��(LED1-��ɫ, LED2-��ɫ, LED3-��ɫ)
#define LED1_GPIO_PORT GPIOB
#define LED1_GPIO_PIN GPIO_Pin_5
#define LED1_GPIO_CLK RCC_APB2Periph_GPIOB
#define LED2_GPIO_PORT GPIOB
#define LED2_GPIO_PIN GPIO_Pin_0
#define LED2_GPIO_CLK RCC_APB2Periph_GPIOB
#define LED3_GPIO_PORT GPIOB
#define LED3_GPIO_PIN GPIO_Pin_1
#define LED3_GPIO_CLK RCC_APB2Periph_GPIOB

// ����LED�궨��
#define LED1_ON() GPIO_ResetBits(LED1_GPIO_PORT, LED1_GPIO_PIN) // �͵�ƽ����
#define LED1_OFF() GPIO_SetBits(LED1_GPIO_PORT, LED1_GPIO_PIN)  // �ߵ�ƽϨ��
#define LED1_TOGGLE() GPIO_ToggleBits(LED1_GPIO_PORT, LED1_GPIO_PIN) // ��ת��ǰ״̬
#define LED2_ON() GPIO_ResetBits(LED2_GPIO_PORT, LED2_GPIO_PIN)
#define LED2_OFF() GPIO_SetBits(LED2_GPIO_PORT, LED2_GPIO_PIN)
#define LED2_TOGGLE() GPIO_ToggleBits(LED2_GPIO_PORT, LED2_GPIO_PIN)
#define LED3_ON() GPIO_ResetBits(LED3_GPIO_PORT, LED3_GPIO_PIN)
#define LED3_OFF() GPIO_SetBits(LED3_GPIO_PORT, LED3_GPIO_PIN)
#define LED3_TOGGLE() GPIO_ToggleBits(LED3_GPIO_PORT, LED3_GPIO_PIN)

// ������ɫ
#define LED_RED()   {LED1_ON(); LED2_OFF(); LED3_OFF();}
#define LED_GREEN() {LED1_OFF(); LED2_ON(); LED3_OFF();}
#define LED_BLUE()  {LED1_OFF(); LED2_OFF(); LED3_ON();}
#define LED_YELLOW() {LED1_ON(); LED2_ON(); LED3_OFF();}
#define LED_PURPLE() {LED1_ON(); LED2_OFF(); LED3_ON();}
#define LED_CYAN()  {LED1_OFF(); LED2_ON(); LED3_ON();}
#define LED_WHITE() {LED1_ON(); LED2_ON(); LED3_ON();}
#define LED_RGBOFF() {LED1_OFF(); LED2_OFF(); LED3_OFF();}

#endif /* __LED_H */
