/**
  ******************************************************************************
  * @file    bsp_basic_tim.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   �����������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32F767 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./SMD/MicroStepDriver.h"
#include "./led/bsp_led.h" 
#include <stdio.h>
#include <math.h>
extern unsigned char UART_RxBuffer[];
TIM_HandleTypeDef TIM_Handle;
TIM_OC_InitTypeDef sConfig;
TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;

__IO uint16_t ChannelPulse = 4999;

//ϵͳ�Ӽ��ٲ���
speedRampData srd;
//��¼���������λ��
int stepPosition = 0;
//ϵͳ���������״̬
__IO GLOBAL_FLAGS status = {FALSE, FALSE,TRUE};
 /**
  * @brief  �߼���ʱ�� TIMx,x=[1,8]ͨ�����ų�ʼ��
  * @param  ��
  * @retval ��
  */
static void TIMx_GPIO_Configuration(void)
{
	  GPIO_InitTypeDef GPIO_Initure;
     SMD_PULSE_GPIO_CLK();			//����GPIOBʱ��
	   SMD_DIR_GPIO_CLK();
	   SMD_ENA_GPIO_CLK();
	
    GPIO_Initure.Pin=SMD_PULSE_GPIO_PIN;    //PC9
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;  	  //�����������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;     //����
	  GPIO_Initure.Alternate=GPIO_AF2_TIM3;	//PC9����ΪTIM3_CH4
    HAL_GPIO_Init(SMD_PULSE_GPIO_PORT,&GPIO_Initure);
	
	 //���������� GPIO ��ʼ��
    GPIO_Initure.Pin =  SMD_DIR_PIN;
	  GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; 
	  GPIO_Initure.Pull  = GPIO_PULLUP;
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SMD_DIR_PORT, &GPIO_Initure);
    HAL_GPIO_WritePin(SMD_DIR_PORT,SMD_DIR_PIN,GPIO_PIN_RESET);
    
    //���ʹ����� GPIO ��ʼ��
    GPIO_Initure.Pin =  SMD_ENA_PIN;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; 
	  GPIO_Initure.Pull  = GPIO_PULLUP;
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SMD_ENA_PORT, &GPIO_Initure);
    HAL_GPIO_WritePin(SMD_ENA_PORT,SMD_ENA_PIN,GPIO_PIN_SET);
}  
/**

  * @brief  ����������ֹͣ

  * @param  NewState��ʹ�ܻ��߽�ֹ

  * @retval ��

  */
void SMD_ENA(FunctionalState NewState)
{
    if(NewState)
    {
      //ENAʧ�ܣ���ֹ���������
			HAL_GPIO_WritePin(SMD_ENA_PORT,SMD_ENA_PIN,GPIO_PIN_RESET);
      //����ֹͣ��־λΪ��
      status.out_ena = FALSE; 
      printf("\n\r��������ֹ������ѻ�״̬����ʱ���Ϊ�ޱ�������״̬�������ֶ���ת���");        
    }
    else
    {
      //ENAʹ��
     		HAL_GPIO_WritePin(SMD_ENA_PORT,SMD_ENA_PIN,GPIO_PIN_SET);
      //����ֹͣ��־λΪ��
      status.out_ena = TRUE; 
      printf("\n\r�������ָ����У���ʱ���Ϊ��������״̬����ʱ����ָ������������Ƶ��");         
    }
    
}
/*! \brief �Ը����Ĳ����ƶ��������
 *  ͨ��������ٵ�����ٶȣ��Ը����Ĳ�����ʼ����
 *  ������ٶȺͼ��ٶȺ�С������������ƶ���������û�ﵽ����ٶȾ�Ҫ��ʼ����
 *  \param step   �ƶ��Ĳ��� (����Ϊ˳ʱ�룬����Ϊ��ʱ��).
 *  \param accel  ���ٶ�,���ȡֵΪ100��ʵ��ֵΪ100*0.01*rad/sec^2=1rad/sec^2
 *  \param decel  ���ٶ�,���ȡֵΪ100��ʵ��ֵΪ100*0.01*rad/sec^2=1rad/sec^2
 *  \param speed  ����ٶ�,���ȡֵΪ100��ʵ��ֵΪ100*0.01*rad/sec=1rad/sec
 */
void SMD_Move(signed int step, unsigned int accel, unsigned int decel, unsigned int speed)
{
    //�ﵽ����ٶ�ʱ�Ĳ���.
    unsigned int max_s_lim;
    //���뿪ʼ���ٵĲ���(�����û���ٵ�������ٶ�ʱ)��
    unsigned int accel_lim;

    // ���ݲ������������жϷ���
    if(step < 0)//��ʱ��
    {
        srd.dir = CCW;
        step = -step;
			  stepPosition=stepPosition-step;
    }
    else//˳ʱ��
    {
			  stepPosition=stepPosition+step;
        srd.dir = CW;
    }
    // ����������
    DIR(srd.dir);
    // ���õ��Ϊ���״̬
   status.out_ena = TRUE;
    
    // ���ֻ�ƶ�һ��
    if(step == 1)
    {
        // ֻ�ƶ�һ��
        srd.accel_count = -1;
        // ����״̬
        srd.run_state = DECEL;
			
        // ����ʱ
        srd.step_delay = 1000;
        // ���õ��Ϊ����״̬
        status.running = TRUE;
        //���ö�ʱ����װֵ	
			  TIM_Handle.Init.RepetitionCounter = Pulse_width;
        //����ռ�ձ�Ϊ50%	
			  sConfig.Pulse = Pulse_width>>1;
        //ʹ�ܶ�ʱ��	      
			 HAL_TIM_OC_Start(&TIM_Handle,ADVANCE_TIM_CH);
     }
    // ������Ϊ����ƶ�
    else if(step != 0)
    {
    // ���ǵ��������û��ֲ�����ϸ�ļ��㼰�Ƶ�����

    // ��������ٶȼ���, ����õ�min_delay���ڶ�ʱ���ļ�������ֵ��
    // min_delay = (alpha / tt)/ w
    srd.min_delay = A_T_x100 / speed;

    // ͨ�������һ��(c0) �Ĳ�����ʱ���趨���ٶȣ�����accel��λΪ0.01rad/sec^2
    // step_delay = 1/tt * sqrt(2*alpha/accel)
    // step_delay = ( tfreq*0.676/100 )*100 * sqrt( (2*alpha*10000000000) / (accel*100) )/10000
    srd.step_delay = (T1_FREQ_148 * sqrt(A_SQ / accel))/100;

    // ������ٲ�֮��ﵽ����ٶȵ�����
    // max_s_lim = speed^2 / (2*alpha*accel)
    max_s_lim = (long)speed*speed/(long)(((long)A_x20000*accel)/100);
    // ����ﵽ����ٶ�С��0.5�������ǽ���������Ϊ0
    // ��ʵ�����Ǳ����ƶ�����һ�����ܴﵽ��Ҫ���ٶ�
    if(max_s_lim == 0)
    {
        max_s_lim = 1;
    }

    // ������ٲ�֮�����Ǳ��뿪ʼ����
    // n1 = (n1+n2)decel / (accel + decel)
    accel_lim = ((long)step*decel) / (accel+decel);
    // ���Ǳ����������1�����ܲ��ܿ�ʼ����.
    if(accel_lim == 0)
    {
        accel_lim = 1;
    }
    // ʹ�������������ǿ��Լ������һ�ο�ʼ���ٵ�λ��
    //srd.decel_valΪ����
    if(accel_lim <= max_s_lim)
    {
        srd.decel_val = accel_lim - step;
    }
    else
    {
        srd.decel_val = -(long)(max_s_lim*accel/decel);
    }
    // ��ֻʣ��һ�����Ǳ������
    if(srd.decel_val == 0)
    {
        srd.decel_val = -1;
    }

    // ���㿪ʼ����ʱ�Ĳ���
    srd.decel_start = step + srd.decel_val;

    // �������ٶȺ��������ǾͲ���Ҫ���м����˶�
    if(srd.step_delay <= srd.min_delay)
    {
        srd.step_delay = srd.min_delay;
        srd.run_state = RUN;
    }
    else
    {
        srd.run_state = ACCEL;
    }
    			  
    // ��λ���ٶȼ���ֵ
    srd.accel_count = 0;
    status.running = TRUE;
    //���ö�ʱ����װֵ	
		TIM_Handle.Init.RepetitionCounter = Pulse_width;
		//����ռ�ձ�Ϊ50%	
		sConfig.Pulse = Pulse_width>>1;
		 HAL_TIM_OC_Start(&TIM_Handle,ADVANCE_TIM_CH);
    }
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
 
  /**
  * @brief  �߼���ʱ�� TIMx,x=[1,8]��ʼ������
  * @param  ��
  * @retval ��
  */
static void TIMx_Configuration(void)
{
    //ʹ��TIMx��ʱ��
    ADVANCE_TIM_CLK_ENABLE();    
    //TIMx������Ĵ�������ַ
    TIM_Handle.Instance = SMD_PULSE_TIM;
    //����ģʽΪ���ϼ���������
    TIM_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    /* �ۼ� TIM_Period�������һ�����»����ж�*/		
    //����ʱ����0������9999����Ϊ10000�Σ�Ϊһ����ʱ����
    TIM_Handle.Init.Period = SMD_PULSE_TIM_PERIOD;
    //ʱ�Ӳ���Ƶ����һ��TIM_CLKʱ�Ӽ�1��
    TIM_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    //��ʱ��ʱ��ԴTIMxCLK = 2 * PCLK1  
    //				PCLK1 = HCLK / 4 
    //				=> TIMxCLK=HCLK/2=SystemCoreClock/2=200MHz
    // �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_Prescaler+1)=1000000Hz
    TIM_Handle.Init.Prescaler =  SMD_PULSE_TIM_PSC - 1;   
    // ��ʼ����ʱ��TIM
    HAL_TIM_PWM_Init(&TIM_Handle);
  
    /* ����TIMΪ���ģʽ */
    sConfig.OCMode = TIM_OCMODE_PWM1;
	  // ���ͨ����ƽ��������
    sConfig.OCPolarity = TIM_OCNPOLARITY_LOW;
		 
		sConfig.Pulse = SMD_PULSE_TIM_PERIOD/2;
		
    HAL_TIM_PWM_ConfigChannel(&TIM_Handle,&sConfig,ADVANCE_TIM_CH);//����TIM3ͨ��
    HAL_TIM_PWM_Start(&TIM_Handle,ADVANCE_TIM_CH);//����PWMͨ��3
   
	 
		HAL_NVIC_SetPriority(SMD_PULSE_TIM_IRQ,0,0);    //�����ж����ȼ�����ռ���ȼ�1�������ȼ�3
		HAL_NVIC_EnableIRQ(SMD_PULSE_TIM_IRQ);          //����ITM3�ж�   
    HAL_TIM_OC_Stop(&TIM_Handle,ADVANCE_TIM_CH); 		
}

/**
  * @brief  ��ʼ��������ʱ����ʱ��1ms����һ���ж�
  * @param  ��
  * @retval ��
  */
void TIM_Advance_Init(void)
{
    //TIMͨ�����ų�ʼ��
    TIMx_GPIO_Configuration();
    //TIM�����ʼ��
    TIMx_Configuration();
    HAL_TIM_Base_Start_IT(&TIM_Handle); //ʹ�ܶ�ʱ��3�Ͷ�ʱ��3�����жϣ�TIM_IT_UPDATE 
    HAL_TIM_OC_Stop(&TIM_Handle,ADVANCE_TIM_CH);    
	
}



void SMD_PULSE_TIM_IRQHandler(void)
{
	
  // ������һ����ʱ����
  unsigned int new_step_delay;
  // ���ٹ��������һ����ʱ.
  static int last_accel_delay;
  // �ƶ�����������
  static unsigned int step_count = 0;
  // ��¼new_step_delay�е������������һ������ľ���
  static signed int rest = 0;

if (__HAL_TIM_GET_IT_SOURCE(&TIM_Handle, TIM_IT_UPDATE) != RESET)
{
    /* Clear SMD_PULSE_TIM Capture Compare1 interrupt pending bit*/
    __HAL_TIM_CLEAR_IT(&TIM_Handle, TIM_IT_UPDATE);

    SMD_PULSE_TIM->CCR4=srd.step_delay >> 1;//���ڵ�һ��
    SMD_PULSE_TIM->ARR=srd.step_delay;
    //�����ֹ����������ֹͣ�˶�
    if(status.out_ena != TRUE)
    {
        srd.run_state = STOP;
    }
  switch(srd.run_state) {
    case STOP:
      step_count = 0;
      rest = 0;
      SMD_PULSE_TIM->CCER &= ~(1<<12); //��ֹ���
		  HAL_TIM_OC_Stop(&TIM_Handle,ADVANCE_TIM_CH);
      status.running = FALSE;
      break;

    case ACCEL:
      SMD_PULSE_TIM->CCER |= 1<<12; //ʹ�����
      step_count++;
      srd.accel_count++;
      new_step_delay = srd.step_delay - (((2 * (long)srd.step_delay) 
                       + rest)/(4 * srd.accel_count + 1));
      rest = ((2 * (long)srd.step_delay)+rest)%(4 * srd.accel_count + 1);
      //����ǹ�Ӧ�ÿ�ʼ����
      if(step_count >= srd.decel_start) {
        srd.accel_count = srd.decel_val;
        srd.run_state = DECEL;
      }
      //����Ƿ񵽴�����������ٶ�
      else if(new_step_delay <= srd.min_delay) {
        last_accel_delay = new_step_delay;
        new_step_delay = srd.min_delay;
        rest = 0;
        srd.run_state = RUN;
      }
      break;

    case RUN:
      SMD_PULSE_TIM->CCER |= 1<<12; //ʹ�����
      step_count++;
      new_step_delay = srd.min_delay;
      //����Ƿ���Ҫ��ʼ����
      if(step_count >= srd.decel_start) {
        srd.accel_count = srd.decel_val;
        //�����һ�μ��ٵ���ʱ��Ϊ��ʼ���ٵ���ʱ
        new_step_delay = last_accel_delay;
        srd.run_state = DECEL;
      }
      break;

    case DECEL:
      SMD_PULSE_TIM->CCER |= 1<<12; //ʹ�����
      step_count++;
      srd.accel_count++;
      new_step_delay = srd.step_delay - (((2 * (long)srd.step_delay) 
                       + rest)/(4 * srd.accel_count + 1));
      rest = ((2 * (long)srd.step_delay)+rest)%(4 * srd.accel_count + 1);
      //����Ƿ�Ϊ���һ��
      if(srd.accel_count >= 0){
        srd.run_state = STOP;
      }
      break;
  }
  srd.step_delay = new_step_delay;
 }


}
/*********************************************END OF FILE**********************/

