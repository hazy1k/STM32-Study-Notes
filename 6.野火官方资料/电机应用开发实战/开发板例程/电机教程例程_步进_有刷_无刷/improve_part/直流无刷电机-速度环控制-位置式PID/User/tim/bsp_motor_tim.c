/**
  ******************************************************************************
  * @file    bsp_motor_tim.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   PWM�������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32 F103 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./tim/bsp_motor_tim.h"

TIM_HandleTypeDef  htimx_bldcm;
TIM_OC_InitTypeDef TIM_OCInitStructure;

/* ������������ض�ʱ����ʼ�� */
TIM_HandleTypeDef htimx_hall;

static uint16_t bldcm_pulse = 0;

static motor_rotate_t motor_drive = {0};    // ��������������ṹ��

static void update_speed_dir(uint8_t dir_in);

/**
  * @brief  ����TIM�������PWMʱ�õ���I/O
  * @param  ��
  * @retval ��
  */
static void TIMx_GPIO_Config(void) 
{
  /*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
  GPIO_InitTypeDef GPIO_InitStructure;

  /*������ʱ����ص�GPIO����ʱ��*/
  MOTOR_OCPWM1_GPIO_CLK_ENABLE();
  MOTOR_OCNPWM1_GPIO_CLK_ENABLE();
  MOTOR_OCPWM2_GPIO_CLK_ENABLE();
  MOTOR_OCNPWM2_GPIO_CLK_ENABLE();
  MOTOR_OCPWM3_GPIO_CLK_ENABLE();
  MOTOR_OCNPWM3_GPIO_CLK_ENABLE();
	
  /* ��ʱ������������ӳ�� */			
	MOTOR_OCPWM1_AF_ENABLE();
	
  /* ��ʱ���������ų�ʼ�� */															   
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;   // �������ģʽ

  GPIO_InitStructure.Pin = MOTOR_OCNPWM1_PIN;
  HAL_GPIO_Init(MOTOR_OCNPWM1_GPIO_PORT, &GPIO_InitStructure);	

  GPIO_InitStructure.Pin = MOTOR_OCNPWM2_PIN;	
  HAL_GPIO_Init(MOTOR_OCNPWM2_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.Pin = MOTOR_OCNPWM3_PIN;	
  HAL_GPIO_Init(MOTOR_OCNPWM3_GPIO_PORT, &GPIO_InitStructure);	

  /* ͨ�� 1 */
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;  

  GPIO_InitStructure.Pin = MOTOR_OCPWM1_PIN;
  HAL_GPIO_Init(MOTOR_OCPWM1_GPIO_PORT, &GPIO_InitStructure);
  /* ͨ�� 2 */
  GPIO_InitStructure.Pin = MOTOR_OCPWM2_PIN;	
  HAL_GPIO_Init(MOTOR_OCPWM2_GPIO_PORT, &GPIO_InitStructure);

  /* ͨ�� 3 */
  GPIO_InitStructure.Pin = MOTOR_OCPWM3_PIN;	
  HAL_GPIO_Init(MOTOR_OCPWM3_GPIO_PORT, &GPIO_InitStructure);
}

/*
 * ע�⣺TIM_TimeBaseInitTypeDef�ṹ��������5����Ա��TIM6��TIM7�ļĴ�������ֻ��
 * TIM_Prescaler��TIM_Period������ʹ��TIM6��TIM7��ʱ��ֻ���ʼ����������Ա���ɣ�
 * ����������Ա��ͨ�ö�ʱ���͸߼���ʱ������.
 *-----------------------------------------------------------------------------
 * TIM_Prescaler         ����
 * TIM_CounterMode			 TIMx,x[6,7]û�У��������У�������ʱ����
 * TIM_Period            ����
 * TIM_ClockDivision     TIMx,x[6,7]û�У���������(������ʱ��)
 * TIM_RepetitionCounter TIMx,x[1,8]����(�߼���ʱ��)
 *-----------------------------------------------------------------------------
 */
static void TIM_Mode_Config(void)
{
  // ����TIMx_CLK,x[1,8] 
  MOTOR_TIM_CLK_ENABLE(); 
  /* ���嶨ʱ���ľ����ȷ����ʱ���Ĵ����Ļ���ַ*/
  htimx_bldcm.Instance = MOTOR_TIM;
  /* �ۼ� TIM_Period�������һ�����»����ж�*/		
  //����ʱ����0������PWM_PERIOD_COUNT����Ϊ2400�Σ�Ϊһ����ʱ����
  htimx_bldcm.Init.Period = PWM_PERIOD_COUNT - 1;
  // �߼����ƶ�ʱ��ʱ��ԴTIMxCLK = HCLK=72MHz 
  // �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_Prescaler+1)/(TIM_Period)=15KHz
  htimx_bldcm.Init.Prescaler = PWM_PRESCALER_COUNT - 1;	
  // ����ʱ�ӷ�Ƶ
  htimx_bldcm.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
  // ������ʽ
  htimx_bldcm.Init.CounterMode=TIM_COUNTERMODE_UP;
  // �ظ�������
  htimx_bldcm.Init.RepetitionCounter=0;	
  // ��ʼ����ʱ��TIMx, x[1,8]
  HAL_TIM_PWM_Init(&htimx_bldcm);

  /*PWMģʽ����*/
  //����ΪPWMģʽ1
  TIM_OCInitStructure.OCMode = TIM_OCMODE_PWM1;
  TIM_OCInitStructure.Pulse = 0;                         // Ĭ�ϱ���Ҫ��ʼΪ0
  TIM_OCInitStructure.OCPolarity = TIM_OCPOLARITY_HIGH;
  TIM_OCInitStructure.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  TIM_OCInitStructure.OCIdleState = TIM_OCIDLESTATE_SET;
  TIM_OCInitStructure.OCNIdleState = TIM_OCNIDLESTATE_RESET;

  HAL_TIM_PWM_ConfigChannel(&htimx_bldcm,&TIM_OCInitStructure,TIM_CHANNEL_1);    // ��ʼ��ͨ�� 1 ��� PWM 
  HAL_TIM_PWM_ConfigChannel(&htimx_bldcm,&TIM_OCInitStructure,TIM_CHANNEL_2);    // ��ʼ��ͨ�� 2 ��� PWM
  HAL_TIM_PWM_ConfigChannel(&htimx_bldcm,&TIM_OCInitStructure,TIM_CHANNEL_3);    // ��ʼ��ͨ�� 3 ��� PWM
  
  /* ���ô���Դ */
  HAL_TIMEx_ConfigCommutEvent(&htimx_bldcm, TIM_COM_TS_ITRx, TIM_COMMUTATION_SOFTWARE);

  /* ������ʱ��ͨ��1���PWM */
  HAL_TIM_PWM_Start(&htimx_bldcm,TIM_CHANNEL_1);

  /* ������ʱ��ͨ��2���PWM */
  HAL_TIM_PWM_Start(&htimx_bldcm,TIM_CHANNEL_2);

  /* ������ʱ��ͨ��3���PWM */
  HAL_TIM_PWM_Start(&htimx_bldcm,TIM_CHANNEL_3);
}

/**
  * @brief  ֹͣpwm���
  * @param  ��
  * @retval ��
  */
void stop_pwm_output(void)
{
  /* �رն�ʱ��ͨ��1���PWM */
  __HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_1, 0);

  /* �رն�ʱ��ͨ��2���PWM */
  __HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_2, 0);
  
  /* �رն�ʱ��ͨ��3���PWM */
  __HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_3, 0);
  
  HAL_GPIO_WritePin(MOTOR_OCNPWM1_GPIO_PORT, MOTOR_OCNPWM1_PIN, GPIO_PIN_RESET);    // �ر����ű�
  HAL_GPIO_WritePin(MOTOR_OCNPWM2_GPIO_PORT, MOTOR_OCNPWM2_PIN, GPIO_PIN_RESET);    // �ر����ű�
  HAL_GPIO_WritePin(MOTOR_OCNPWM3_GPIO_PORT, MOTOR_OCNPWM3_PIN, GPIO_PIN_RESET);    // �ر����ű�
}

/**
  * @brief  ����pwm�����ռ�ձ�
  * @param  pulse:Ҫ���õ�ռ�ձ�
  * @retval ��
  */
void set_pwm_pulse(uint16_t pulse)
{
  /* ���ö�ʱ��ͨ����� PWM ��ռ�ձ� */
	bldcm_pulse = pulse;
	
	if (motor_drive.enable_flag)
		HAL_TIM_TriggerCallback(NULL);   // ִ��һ�λ���
}

/**
  * @brief  ��ʼ���߼����ƶ�ʱ��
  * @param  ��
  * @retval ��
  */
void PWM_TIMx_Configuration(void)
{
	TIMx_GPIO_Config();
	TIM_Mode_Config();
}

/**
  * @brief  �������������ų�ʼ��
  * @param  ��
  * @retval ��
  */
static void hall_gpio_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  HALL_INPUTU_GPIO_CLK_ENABLE();
  HALL_INPUTV_GPIO_CLK_ENABLE();
  HALL_INPUTW_GPIO_CLK_ENABLE();
  HALL_INPUT_AF_ENABLE();//��ʱ��4������Ҫ��ӳ��
	
  /* ��ʱ��ͨ�� 1 ���ų�ʼ�� */
  GPIO_InitStruct.Pin = HALL_INPUTU_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(HALL_INPUTU_GPIO_PORT, &GPIO_InitStruct);
  
  /* ��ʱ��ͨ�� 2 ���ų�ʼ�� */
  GPIO_InitStruct.Pin = HALL_INPUTV_PIN;
  HAL_GPIO_Init(HALL_INPUTV_GPIO_PORT, &GPIO_InitStruct);
  
  /* ��ʱ��ͨ�� 3 ���ų�ʼ�� */
  GPIO_InitStruct.Pin = HALL_INPUTW_PIN;
  HAL_GPIO_Init(HALL_INPUTW_GPIO_PORT, &GPIO_InitStruct);
}

/**
  * @brief  ������������ʱ����ʼ��
  * @param  ��
  * @retval ��
  */
static void hall_tim_init(void)
{
  TIM_HallSensor_InitTypeDef  hall_sensor_cfg;  
  
  /* ������ʱ������ʱ��ʹ�� */
  HALL_TIM_CLK_ENABLE();
  
  /* ��ʱ�������������� */
  htimx_hall.Instance = HALL_TIM;
  htimx_hall.Init.Prescaler = HALL_PRESCALER_COUNT - 1;       // Ԥ��Ƶ
  htimx_hall.Init.CounterMode = TIM_COUNTERMODE_UP;           // ���ϼ���
  htimx_hall.Init.Period = HALL_PERIOD_COUNT - 1;             // ��������
  htimx_hall.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;     // ʱ�ӷ�Ƶ
  
  hall_sensor_cfg.IC1Prescaler = TIM_ICPSC_DIV1;            // ���벶���Ƶ
  hall_sensor_cfg.IC1Polarity = TIM_ICPOLARITY_BOTHEDGE;    // ���벶����
  hall_sensor_cfg.IC1Filter = 10;                           // �����˲�
  hall_sensor_cfg.Commutation_Delay = 0U;                   // ��ʹ���ӳٴ���
  HAL_TIMEx_HallSensor_Init(&htimx_hall, &hall_sensor_cfg);
  
  HAL_NVIC_SetPriority(HALL_TIM_IRQn, 0, 0);    // �����ж����ȼ�
  HAL_NVIC_EnableIRQ(HALL_TIM_IRQn);            // ʹ���ж�
}

/**
  * @brief  ʹ�ܻ���������
  * @param  ��
  * @retval ��
  */
void hall_enable(void)
{
  /* ʹ�ܻ����������ӿ� */
  __HAL_TIM_ENABLE_IT(&htimx_hall, TIM_IT_TRIGGER);
  __HAL_TIM_ENABLE_IT(&htimx_hall, TIM_IT_UPDATE);
  
  HAL_TIMEx_HallSensor_Start(&htimx_hall);

  LED1_OFF;
  
  HAL_TIM_TriggerCallback(&htimx_hall);   // ִ��һ�λ���
	
	motor_drive.enable_flag = 1;
}

/**
  * @brief  ���û���������
  * @param  ��
  * @retval ��
  */
void hall_disable(void)
{
  /* ���û����������ӿ� */
  __HAL_TIM_DISABLE_IT(&htimx_hall, TIM_IT_TRIGGER);
  __HAL_TIM_DISABLE_IT(&htimx_hall, TIM_IT_UPDATE);
  HAL_TIMEx_HallSensor_Stop(&htimx_hall);
	motor_drive.enable_flag = 0;
  motor_drive.speed = 0;
}

uint8_t get_hall_state(void)
{
  uint8_t state = 0;
  
#if 1
  /* ��ȡ���������� U ��״̬ */
  if(HAL_GPIO_ReadPin(HALL_INPUTU_GPIO_PORT, HALL_INPUTU_PIN) != GPIO_PIN_RESET)
  {
    state |= 0x01U << 0;
  }
  
  /* ��ȡ���������� V ��״̬ */
  if(HAL_GPIO_ReadPin(HALL_INPUTV_GPIO_PORT, HALL_INPUTV_PIN) != GPIO_PIN_RESET)
  {
    state |= 0x01U << 1;
  }
  
  /* ��ȡ���������� W ��״̬ */
  if(HAL_GPIO_ReadPin(HALL_INPUTW_GPIO_PORT, HALL_INPUTW_PIN) != GPIO_PIN_RESET)
  {
    state |= 0x01U << 2;
  }
#else
  state = (GPIOH->IDR >> 10) & 7;    // �� 3 ��������������״̬
#endif

  return state;    // ���ش�����״̬
}

/**
  * @brief  ��ʼ��������������ʱ��
  * @param  ��
  * @retval ��
  */
void hall_tim_config(void)
{
	hall_gpio_init();	    // ��ʼ������
	hall_tim_init();      // ��ʼ����ʱ��
}

static uint8_t count = 0;
static void update_motor_speed(uint8_t dir_in, uint32_t time)
{
  int speed_temp = 0;
  static int flag = 0;
  float f = 0;

  /* �����ٶȣ�
     ���ÿתһȦ����12�����壬(1.0/(72000000.0/128.0)Ϊ�����������ڣ�(1.0/(72000000.0/128.0) * time)Ϊʱ�䳤��
  */

  if (time == 0)
    motor_drive.speed_group[count++] = 0;
  else
  {
    f = (1.0f / (72000000.0f / HALL_PRESCALER_COUNT) * time);
    f = (1.0f / 12.0f) / (f  / 60.0f);
    motor_drive.speed_group[count++] = f;
  }
	update_speed_dir(dir_in);
//	motor_drive.speed = motor_drive.speed_group[count-1];
  if (count >= SPEED_FILTER_NUM)
  {
    flag = 1;
    count = 0;
  }
//	return ;
  speed_temp = 0;
	
  /* ����� SPEED_FILTER_NUM �ε��ٶ�ƽ��ֵ���˲��� */
  if (flag)
  {
    for (uint8_t c=0; c<SPEED_FILTER_NUM; c++)
    {
      speed_temp += motor_drive.speed_group[c];
    }

    motor_drive.speed = speed_temp/ SPEED_FILTER_NUM;
  }
  else
  {
    for (uint8_t c=0; c<count; c++)
    {
      speed_temp += motor_drive.speed_group[c];
    }

    motor_drive.speed = speed_temp / count;
  }

}

/**
  * @brief  ��ȡ���ת��
  * @param  time:��ȡ��ʱ����
  * @retval ���ص��ת��
  */
float get_motor_speed(void)
{
  return motor_drive.speed;
}

/**
  * @brief  ���µ��ʵ���ٶȷ���
  * @param  dir_in������ֵ
  * @retval ��
  */
static void update_speed_dir(uint8_t dir_in)
{
  uint8_t step[6] = {1, 3, 2, 6, 4, 5};

  static uint8_t num_old = 0;
  uint8_t step_loc = 0;    // ��¼��ǰ����λ��
  int8_t dir = 1;
  
  for (step_loc=0; step_loc<6; step_loc++)
  {
    if (step[step_loc] == dir_in)    // �ҵ���ǰ������λ��
    {
      break;
    }
  }
  
  /* �˵㴦�� */
  if (step_loc == 0)
  {
    if (num_old == 1)
    {
      dir = 1;
    }
    else if (num_old == 5)
    {
      dir = -1;
    }
  }
  /* �˵㴦�� */
  else if (step_loc == 5)
  {
    if (num_old == 0)
    {
      dir = 1;
    }
    else if (num_old == 4)
    {
      dir = -1;
    }
  }
  else if (step_loc > num_old)
  {
    dir = -1;
  }
  else if (step_loc < num_old)
  {
    dir = 1;
  }
  
  num_old = step_loc;
//  motor_drive.speed *= dir;;
	motor_drive.speed_group[count-1]*= dir;
}

/**
  * @brief  ���������������ص�����
  * @param  htim:��ʱ�����
  * @retval ��
  */
void HAL_TIM_TriggerCallback(TIM_HandleTypeDef *htim)
{
  /* ��ȡ��������������״̬,��Ϊ��������� */
  uint8_t step = 0;
  step = get_hall_state();

	if (htim == &htimx_hall)   // �ж��Ƿ��ɴ����жϲ���
  {
    update_motor_speed(step, __HAL_TIM_GET_COMPARE(htim,TIM_CHANNEL_1));
    motor_drive.timeout = 0;
  }
	
	if(get_bldcm_direction() == MOTOR_FWD)
		{
			switch(step)
			{
				case 1:    /* U+ W- */
					__HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_2, 0);                            // ͨ�� 2 ����Ϊ 0
					HAL_GPIO_WritePin(MOTOR_OCNPWM2_GPIO_PORT, MOTOR_OCNPWM2_PIN, GPIO_PIN_RESET);    // �ر����ű�
				
					__HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_3, 0);                            // ͨ�� 3 ����Ϊ 0
					HAL_GPIO_WritePin(MOTOR_OCNPWM1_GPIO_PORT, MOTOR_OCNPWM1_PIN, GPIO_PIN_RESET);    // �ر����ű�

					__HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_1, bldcm_pulse);                  // ͨ�� 1 ���õ�ռ�ձ�
					HAL_GPIO_WritePin(MOTOR_OCNPWM3_GPIO_PORT, MOTOR_OCNPWM3_PIN, GPIO_PIN_SET);      // �������ű�
					break;
				
				case 2:     /* V+ U- */
					__HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_3, 0);                            // ͨ�� 3 ����Ϊ 0
					HAL_GPIO_WritePin(MOTOR_OCNPWM3_GPIO_PORT, MOTOR_OCNPWM3_PIN, GPIO_PIN_RESET);    // �ر����ű�

					__HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_1, 0);                            // ͨ�� 1 ����Ϊ 0
					HAL_GPIO_WritePin(MOTOR_OCNPWM2_GPIO_PORT, MOTOR_OCNPWM2_PIN, GPIO_PIN_RESET);    // �ر����ű�
				
					__HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_2, bldcm_pulse);                  // ͨ�� 2 ���õ�ռ�ձ�
					HAL_GPIO_WritePin(MOTOR_OCNPWM1_GPIO_PORT, MOTOR_OCNPWM1_PIN, GPIO_PIN_SET);      // �������ű�
				
					break;
				
				case 3:    /* V+ W- */
					__HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_1, 0);                            // ͨ�� 1 ����Ϊ 0
					HAL_GPIO_WritePin(MOTOR_OCNPWM1_GPIO_PORT, MOTOR_OCNPWM1_PIN, GPIO_PIN_RESET);    // �ر����ű�

					__HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_3, 0);                            // ͨ�� 3 ����Ϊ 0
					HAL_GPIO_WritePin(MOTOR_OCNPWM2_GPIO_PORT, MOTOR_OCNPWM2_PIN, GPIO_PIN_RESET);    // �ر����ű�
					
					__HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_2, bldcm_pulse);                  // ͨ�� 2 ���õ�ռ�ձ�
					HAL_GPIO_WritePin(MOTOR_OCNPWM3_GPIO_PORT, MOTOR_OCNPWM3_PIN, GPIO_PIN_SET);      // �������ű�
					break;
				
				case 4:     /* W+ V- */
					__HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_1, 0);                            // ͨ�� 1 ����Ϊ 0
					HAL_GPIO_WritePin(MOTOR_OCNPWM1_GPIO_PORT, MOTOR_OCNPWM1_PIN, GPIO_PIN_RESET);    // �ر����ű�

					__HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_2, 0);                            // ͨ�� 2 ����Ϊ 0
					HAL_GPIO_WritePin(MOTOR_OCNPWM3_GPIO_PORT, MOTOR_OCNPWM3_PIN, GPIO_PIN_RESET);    // �ر����ű�
		 
					__HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_3, bldcm_pulse);                  // ͨ�� 3 ���õ�ռ�ձ�
					HAL_GPIO_WritePin(MOTOR_OCNPWM2_GPIO_PORT, MOTOR_OCNPWM2_PIN, GPIO_PIN_SET);      // �������ű� 
					break;
				
				case 5:     /* U+  V -*/
					__HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_3, 0);                            // ͨ�� 3 ����Ϊ 0
					HAL_GPIO_WritePin(MOTOR_OCNPWM3_GPIO_PORT, MOTOR_OCNPWM3_PIN, GPIO_PIN_RESET);    // �ر����ű�
				
					__HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_2, 0);                            // ͨ�� 2 ����Ϊ 0
					HAL_GPIO_WritePin(MOTOR_OCNPWM1_GPIO_PORT, MOTOR_OCNPWM1_PIN, GPIO_PIN_RESET);    // �ر����ű�
				
					__HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_1, bldcm_pulse);                  // ͨ�� 1 ���õ�ռ�ձ�
					HAL_GPIO_WritePin(MOTOR_OCNPWM2_GPIO_PORT, MOTOR_OCNPWM2_PIN, GPIO_PIN_SET);      // �������ű�
					break;
				
				case 6:     /* W+ U- */
					__HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_2, 0);                            // ͨ�� 2 ����Ϊ 0
					HAL_GPIO_WritePin(MOTOR_OCNPWM2_GPIO_PORT, MOTOR_OCNPWM2_PIN, GPIO_PIN_RESET);    // �ر����ű�
				
					__HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_1, 0);                            // ͨ�� 1 ����Ϊ 0
					HAL_GPIO_WritePin(MOTOR_OCNPWM3_GPIO_PORT, MOTOR_OCNPWM3_PIN, GPIO_PIN_RESET);    // �ر����ű�
				
					__HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_3, bldcm_pulse);                  // ͨ�� 3 ���õ�ռ�ձ�
					HAL_GPIO_WritePin(MOTOR_OCNPWM1_GPIO_PORT, MOTOR_OCNPWM1_PIN, GPIO_PIN_SET);      // �������ű�
					break;
			}
		}
		else
		{
			switch(step)
			{
				case 1:   /* W+ U- */
					__HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_2, 0);                            // ͨ�� 2 ����Ϊ 0
					HAL_GPIO_WritePin(MOTOR_OCNPWM2_GPIO_PORT, MOTOR_OCNPWM2_PIN, GPIO_PIN_RESET);    // �ر����ű�
				
					__HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_1, 0);                            // ͨ�� 1 ����Ϊ 0
					HAL_GPIO_WritePin(MOTOR_OCNPWM3_GPIO_PORT, MOTOR_OCNPWM3_PIN, GPIO_PIN_RESET);    // �ر����ű�
				
					__HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_3, bldcm_pulse);                  // ͨ�� 3 ���õ�ռ�ձ�
					HAL_GPIO_WritePin(MOTOR_OCNPWM1_GPIO_PORT, MOTOR_OCNPWM1_PIN, GPIO_PIN_SET);      // �������ű�
					break;
				
				case 2:    /* U+  V -*/
					__HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_3, 0);                            // ͨ�� 3 ����Ϊ 0
					HAL_GPIO_WritePin(MOTOR_OCNPWM3_GPIO_PORT, MOTOR_OCNPWM3_PIN, GPIO_PIN_RESET);    // �ر����ű�
				
					__HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_2, 0);                            // ͨ�� 2 ����Ϊ 0
					HAL_GPIO_WritePin(MOTOR_OCNPWM1_GPIO_PORT, MOTOR_OCNPWM1_PIN, GPIO_PIN_RESET);    // �ر����ű�
				
					__HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_1, bldcm_pulse);                  // ͨ�� 1 ���õ�ռ�ձ�
					HAL_GPIO_WritePin(MOTOR_OCNPWM2_GPIO_PORT, MOTOR_OCNPWM2_PIN, GPIO_PIN_SET);      // �������ű�
					break;
				
				case 3:   /* W+ V- */
					__HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_1, 0);                            // ͨ�� 1 ����Ϊ 0
					HAL_GPIO_WritePin(MOTOR_OCNPWM1_GPIO_PORT, MOTOR_OCNPWM1_PIN, GPIO_PIN_RESET);    // �ر����ű�

					__HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_2, 0);                            // ͨ�� 2 ����Ϊ 0
					HAL_GPIO_WritePin(MOTOR_OCNPWM3_GPIO_PORT, MOTOR_OCNPWM3_PIN, GPIO_PIN_RESET);    // �ر����ű�
		 
					__HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_3, bldcm_pulse);                  // ͨ�� 3 ���õ�ռ�ձ�
					HAL_GPIO_WritePin(MOTOR_OCNPWM2_GPIO_PORT, MOTOR_OCNPWM2_PIN, GPIO_PIN_SET);      // �������ű�        

					break;
				
				case 4:    /* V+ W- */
					__HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_1, 0);                            // ͨ�� 1 ����Ϊ 0
					HAL_GPIO_WritePin(MOTOR_OCNPWM1_GPIO_PORT, MOTOR_OCNPWM1_PIN, GPIO_PIN_RESET);    // �ر����ű�

					__HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_3, 0);                            // ͨ�� 3 ����Ϊ 0
					HAL_GPIO_WritePin(MOTOR_OCNPWM2_GPIO_PORT, MOTOR_OCNPWM2_PIN, GPIO_PIN_RESET);    // �ر����ű�
					
					__HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_2, bldcm_pulse);                  // ͨ�� 2 ���õ�ռ�ձ�
					HAL_GPIO_WritePin(MOTOR_OCNPWM3_GPIO_PORT, MOTOR_OCNPWM3_PIN, GPIO_PIN_SET);      // �������ű�
					break;
				
				case 5:    /* V+ U- */
					__HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_3, 0);                            // ͨ�� 3 ����Ϊ 0
					HAL_GPIO_WritePin(MOTOR_OCNPWM3_GPIO_PORT, MOTOR_OCNPWM3_PIN, GPIO_PIN_RESET);    // �ر����ű�

					__HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_1, 0);                            // ͨ�� 1 ����Ϊ 0
					HAL_GPIO_WritePin(MOTOR_OCNPWM2_GPIO_PORT, MOTOR_OCNPWM2_PIN, GPIO_PIN_RESET);    // �ر����ű�
				
					__HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_2, bldcm_pulse);                  // ͨ�� 2 ���õ�ռ�ձ�
					HAL_GPIO_WritePin(MOTOR_OCNPWM1_GPIO_PORT, MOTOR_OCNPWM1_PIN, GPIO_PIN_SET);      // �������ű�
					break;
				
				case 6:    /* U+ W- */
					__HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_2, 0);                            // ͨ�� 2 ����Ϊ 0
					HAL_GPIO_WritePin(MOTOR_OCNPWM2_GPIO_PORT, MOTOR_OCNPWM2_PIN, GPIO_PIN_RESET);    // �ر����ű�
				
					__HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_3, 0);                            // ͨ�� 3 ����Ϊ 0
					HAL_GPIO_WritePin(MOTOR_OCNPWM1_GPIO_PORT, MOTOR_OCNPWM1_PIN, GPIO_PIN_RESET);    // �ر����ű�

					__HAL_TIM_SET_COMPARE(&htimx_bldcm, TIM_CHANNEL_1, bldcm_pulse);                  // ͨ�� 1 ���õ�ռ�ձ�
					HAL_GPIO_WritePin(MOTOR_OCNPWM3_GPIO_PORT, MOTOR_OCNPWM3_PIN, GPIO_PIN_SET);      // �������ű�
					break;
			}
		}
		HAL_TIM_GenerateEvent(&htimx_bldcm, TIM_EVENTSOURCE_COM);    // ������������¼�����ʱ�Ž�����д��	

}

/**
  * @brief  ��ʱ�������жϻص�����
  * @param  htim:��ʱ�����
  * @retval ��
  */
int32_t speed_actual = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim == (&htimx_hall))
  {
    if (motor_drive.timeout++ > 100)    // ��һ���ڲ��������ж�ǰ����������û�в���ֵ
    {
      printf("��ת��ʱ\r\n");
      motor_drive.timeout = 0;
      
      LED1_ON;     // ����LED1��ʾ��ת��ʱֹͣ
      
      /* ��ת��ʱֹͣ PWM ��� */
      hall_disable();       // ���û����������ӿ�
      stop_pwm_output();    // ֹͣ PWM ���
      set_bldcm_disable();
      motor_drive.speed = 0;
    }
  }
  else if(htim == (&TIM_TimeBaseStructure))
  {
      bldcm_pid_control();
  }
}
/*********************************************END OF FILE**********************/
