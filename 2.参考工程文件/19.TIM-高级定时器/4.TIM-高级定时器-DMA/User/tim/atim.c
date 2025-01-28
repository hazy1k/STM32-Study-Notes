#include "atim.h"

uint16_t ATIM_Buffer[ATIM_DMA_BufferSize] = {700, 700, 100};

static void ATIM_GPIO_Init(void)
{
  // CH1 GPIO Configuration
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(ATIM_CH1_GPIO_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = ATIM_CH1_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(ATIM_CH1_GPIO_PORT, &GPIO_InitStructure);
  // DMA Configuration
  DMA_InitTypeDef DMA_InitStructure;
  RCC_AHBPeriphClockCmd(ATIM_DMA_CLK, ENABLE);
  DMA_DeInit(ATIM_DMA_Channel);
  // DMA����
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(ATIM_DMA_ADDRESS); // ����DMAԴ��ַ��TIM��CCR�Ĵ���
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ATIM_Buffer; // ����DMAĿ�ĵ�ַ��ATIM_Buffer
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; // ����DMA���䷽�򣺴��ڴ浽����
  DMA_InitStructure.DMA_BufferSize = ATIM_DMA_BufferSize; // ����DMA�������ݳ���
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(ATIM_DMA_Channel, &DMA_InitStructure);
}

static void ATIM_Mode_Init(void)
{
  // ʱ���ṹ��ʼ��
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  RCC_APB2PeriphClockCmd(ATIM_CLK, ENABLE);
  TIM_TimeBaseStructure.TIM_Period = 1000-1;
  TIM_TimeBaseStructure.TIM_Prescaler = 7200-1;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(ATIMx, &TIM_TimeBaseStructure);
  // ���PWMģʽ����
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC1Init(ATIMx, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(ATIMx, TIM_OCPreload_Enable);
  TIM_Cmd(ATIMx, ENABLE);
  // DMAʹ��
  DMA_Cmd(ATIM_DMA_Channel, ENABLE);
  TIM_DMACmd(ATIMx, TIM_DMA_CC1, ENABLE);
  TIM_CtrlPWMOutputs(ATIMx, ENABLE); // ʹ��PWM���
}

void ATIMx_Init(void)
{
  ATIM_GPIO_Init();
  ATIM_Mode_Init();
}
