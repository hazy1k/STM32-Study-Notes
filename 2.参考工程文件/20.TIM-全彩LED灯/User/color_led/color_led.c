#include "color_led.h"

static void TIM_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(TIM_GPIO_CLK, ENABLE);
	GPIO_REMAP_FUN(); // ����ӳ��GPIO
	// ��
	GPIO_InitStructure.GPIO_Pin = RED_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RED_GPIO_PORT, &GPIO_InitStructure);
	// ��
	GPIO_InitStructure.GPIO_Pin = GREEN_GPIO_PIN;
	GPIO_Init(GREEN_GPIO_PORT, &GPIO_InitStructure);
	// ��
	GPIO_InitStructure.GPIO_Pin = BLUE_GPIO_PIN;
	GPIO_Init(BLUE_GPIO_PORT, &GPIO_InitStructure);
}

static void TIM_Mode_Init(void)
{
	TIM_APBxClock(TIM_CLK, ENABLE);
	// ʱ���ṹ��ʼ��
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = 255;
	TIM_TimeBaseStructure.TIM_Prescaler = 4000;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);
	// PWMģʽ��ʼ��
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	RED_OCxInit(TIMx, &TIM_OCInitStructure); // TIM3_CH2��ʼ��
	RED_OCxPreloadConfig(TIMx, TIM_OCPreload_Enable); // TIM3_CH2Ԥװ��ʹ��
	GREEN_OCxInit(TIMx, &TIM_OCInitStructure); // TIM3_CH3��ʼ��
	GREEN_OCxPreloadConfig(TIMx, TIM_OCPreload_Enable); // TIM3_CH3Ԥװ��ʹ��
	BLUE_OCxInit(TIMx, &TIM_OCInitStructure); // TIM3_CH4��ʼ��
	BLUE_OCxPreloadConfig(TIMx, TIM_OCPreload_Enable); // TIM3_CH4Ԥװ��ʹ��
	TIM_ARRPreloadConfig(TIMx, ENABLE); // ʹ����װ�ؼĴ���
	TIM_Cmd(TIMx, ENABLE); 
}

void TIMx_Init(void)
{
	TIM_GPIO_Init();
	TIM_Mode_Init();
}

// ����RGB��ɫֵ
void SetRGBColor(uint32_t rgb)
{
	TIMx->RED_CCRx   = (uint8_t)(rgb >> 16);// R
	TIMx->GREEN_CCRx = (uint8_t)(rgb >> 8); // G
	TIMx->BLUE_CCRx  = (uint8_t)(rgb);      // B
}
// ����RGB��ɫֵ
void SetColorValue(uint8_t r, uint8_t g, uint8_t b)
{
	TIMx->RED_CCRx   = r;
	TIMx->GREEN_CCRx = g;
	TIMx->BLUE_CCRx  = b;
}
 
void LED_Close(void) // ֹͣPWM���
{
	SetColorValue(0,0,0);  // �ر�LED
	TIM_Cmd(TIMx, DISABLE);// �رն�ʱ��
	TIM_APBxClock(TIM_CLK, DISABLE);
	GPIO_SetBits(RED_GPIO_PORT, RED_GPIO_PIN);
	GPIO_SetBits(GREEN_GPIO_PORT, GREEN_GPIO_PIN);
	GPIO_SetBits(BLUE_GPIO_PORT, BLUE_GPIO_PIN);
}
