#include "./stepper/bsp_stepper_init.h"
#include "./led/bsp_led.h"   
#include "stm32f1xx.h"


TIM_HandleTypeDef TIM_TimeBaseStructure;
/**
  * @brief  ͨ�ö�ʱ�� TIMx,x[6,7]�ж����ȼ�����
  * @param  ��
  * @retval ��
  */
static void TIMx_NVIC_Configuration(void)
{
  /* �����ж����� */
  HAL_NVIC_SetPriority(MOTOR_PUL_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(MOTOR_PUL_IRQn);
}

/*
 * ע�⣺TIM_TimeBaseInitTypeDef�ṹ��������5����Ա��TIM6��TIM7�ļĴ�������ֻ��
 * TIM_Prescaler��TIM_Period������ʹ��TIM6��TIM7��ʱ��ֻ���ʼ����������Ա���ɣ�
 * ����������Ա��ͨ�ö�ʱ���͸߼���ʱ������.
 *-----------------------------------------------------------------------------
 * TIM_Prescaler         ����
 * TIM_CounterMode       TIMx,x[6,7]û�У��������У�ͨ�ö�ʱ����
 * TIM_Period            ����
 * TIM_ClockDivision     TIMx,x[6,7]û�У���������(ͨ�ö�ʱ��)
 * TIM_RepetitionCounter TIMx,x[1,8]����(�߼���ʱ��)
 *-----------------------------------------------------------------------------
 */
static void TIM_Mode_Config(void)
{

  MOTOR_PUL_CLK_ENABLE();

  TIM_TimeBaseStructure.Instance = MOTOR_PUL_TIM;
  /* �ۼ� TIM_Period�������һ�����»����ж�*/    
  //����ʱ����0������4999����Ϊ5000�Σ�Ϊһ����ʱ����
  TIM_TimeBaseStructure.Init.Period = 300-1;  
  // ͨ�ÿ��ƶ�ʱ��ʱ��ԴTIMxCLK = HCLK/2=84MHz 
  // �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_Prescaler+1)=1MHz
  TIM_TimeBaseStructure.Init.Prescaler = 84-1;
  // ������ʽ
  TIM_TimeBaseStructure.Init.CounterMode=TIM_COUNTERMODE_UP;
  // ����ʱ�ӷ�Ƶ
  TIM_TimeBaseStructure.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
  // ��ʼ����ʱ��TIMx, x[2,5] [9,14]
  HAL_TIM_Base_Init(&TIM_TimeBaseStructure);

  // ������ʱ�������ж�
  HAL_TIM_Base_Start_IT(&TIM_TimeBaseStructure);  
}

/**
  * @brief  ��ʼ��ͨ�ö�ʱ����ʱ
  * @param  ��
  * @retval ��
  */
void TIMx_Configuration(void)
{
  TIMx_NVIC_Configuration();  
  
  TIM_Mode_Config();
}

/**
  * @brief  ���ų�ʼ��
  * @retval ��
  */
void stepper_Init()
{
  /*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
  GPIO_InitTypeDef  GPIO_InitStruct;

  /*����Motor��ص�GPIO����ʱ��*/
  MOTOR_DIR_GPIO_CLK_ENABLE();
  MOTOR_PUL_GPIO_CLK_ENABLE();
  MOTOR_EN_GPIO_CLK_ENABLE();

  /*ѡ��Ҫ���Ƶ�GPIO����*/                                 
  GPIO_InitStruct.Pin = MOTOR_DIR_PIN;  

  /*�������ŵ��������Ϊ�������*/
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;  

  GPIO_InitStruct.Pull =GPIO_PULLUP;

  /*������������Ϊ���� */   
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

  /*Motor �������� ��ʼ��*/
  HAL_GPIO_Init(MOTOR_DIR_GPIO_PORT, &GPIO_InitStruct); 

  /*Motor �������� ��ʼ��*/
  GPIO_InitStruct.Pin = MOTOR_PUL_PIN;  
  HAL_GPIO_Init(MOTOR_PUL_GPIO_PORT, &GPIO_InitStruct); 

  /*Motor ʹ������ ��ʼ��*/
  GPIO_InitStruct.Pin = MOTOR_EN_PIN; 
  HAL_GPIO_Init(MOTOR_EN_GPIO_PORT, &GPIO_InitStruct);  

  /*�ص�ʹ��*/
  MOTOR_EN(LOW);
  /*��ʼ����ʱ��*/
  TIMx_Configuration();
        
}

/**
  * @brief  ��ʱ���жϺ���
  * @note   ��
  * @retval ��
  */
void MOTOR_PUL_IRQHandler (void)
{
  HAL_TIM_IRQHandler(&TIM_TimeBaseStructure);   
}
/**
  * @brief  �ص�����
  * @note   ��
  * @retval ��
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim==(&TIM_TimeBaseStructure))
  {
    MOTOR_PUL_T();//��תIO�ڴﵽ�����Ч��
  }
}
















