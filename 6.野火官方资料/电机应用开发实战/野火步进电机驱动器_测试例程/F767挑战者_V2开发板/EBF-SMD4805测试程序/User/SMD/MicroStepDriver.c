/**
  ******************************************************************************
  * @file    bsp_basic_tim.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   步进电机测试
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32F767 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
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

//系统加减速参数
speedRampData srd;
//记录步进电机的位置
int stepPosition = 0;
//系统电机、串口状态
__IO GLOBAL_FLAGS status = {FALSE, FALSE,TRUE};
 /**
  * @brief  高级定时器 TIMx,x=[1,8]通道引脚初始化
  * @param  无
  * @retval 无
  */
static void TIMx_GPIO_Configuration(void)
{
	  GPIO_InitTypeDef GPIO_Initure;
     SMD_PULSE_GPIO_CLK();			//开启GPIOB时钟
	   SMD_DIR_GPIO_CLK();
	   SMD_ENA_GPIO_CLK();
	
    GPIO_Initure.Pin=SMD_PULSE_GPIO_PIN;    //PC9
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;  	  //复用推完输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;     //高速
	  GPIO_Initure.Alternate=GPIO_AF2_TIM3;	//PC9复用为TIM3_CH4
    HAL_GPIO_Init(SMD_PULSE_GPIO_PORT,&GPIO_Initure);
	
	 //电机方向输出 GPIO 初始化
    GPIO_Initure.Pin =  SMD_DIR_PIN;
	  GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; 
	  GPIO_Initure.Pull  = GPIO_PULLUP;
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SMD_DIR_PORT, &GPIO_Initure);
    HAL_GPIO_WritePin(SMD_DIR_PORT,SMD_DIR_PIN,GPIO_PIN_RESET);
    
    //电机使能输出 GPIO 初始化
    GPIO_Initure.Pin =  SMD_ENA_PIN;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; 
	  GPIO_Initure.Pull  = GPIO_PULLUP;
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SMD_ENA_PORT, &GPIO_Initure);
    HAL_GPIO_WritePin(SMD_ENA_PORT,SMD_ENA_PIN,GPIO_PIN_SET);
}  
/**

  * @brief  驱动器紧急停止

  * @param  NewState：使能或者禁止

  * @retval 无

  */
void SMD_ENA(FunctionalState NewState)
{
    if(NewState)
    {
      //ENA失能，禁止驱动器输出
			HAL_GPIO_WritePin(SMD_ENA_PORT,SMD_ENA_PIN,GPIO_PIN_RESET);
      //紧急停止标志位为真
      status.out_ena = FALSE; 
      printf("\n\r驱动器禁止输出（脱机状态）此时电机为无保持力矩状态，可以手动旋转电机");        
    }
    else
    {
      //ENA使能
     		HAL_GPIO_WritePin(SMD_ENA_PORT,SMD_ENA_PIN,GPIO_PIN_SET);
      //紧急停止标志位为假
      status.out_ena = TRUE; 
      printf("\n\r驱动器恢复运行，此时电机为保持力矩状态，此时串口指令可以正常控制电机");         
    }
    
}
/*! \brief 以给定的步数移动步进电机
 *  通过计算加速到最大速度，以给定的步数开始减速
 *  如果加速度和减速度很小，步进电机会移动很慢，还没达到最大速度就要开始减速
 *  \param step   移动的步数 (正数为顺时针，负数为逆时针).
 *  \param accel  加速度,如果取值为100，实际值为100*0.01*rad/sec^2=1rad/sec^2
 *  \param decel  减速度,如果取值为100，实际值为100*0.01*rad/sec^2=1rad/sec^2
 *  \param speed  最大速度,如果取值为100，实际值为100*0.01*rad/sec=1rad/sec
 */
void SMD_Move(signed int step, unsigned int accel, unsigned int decel, unsigned int speed)
{
    //达到最大速度时的步数.
    unsigned int max_s_lim;
    //必须开始减速的步数(如果还没加速到达最大速度时)。
    unsigned int accel_lim;

    // 根据步数的正负来判断方向
    if(step < 0)//逆时针
    {
        srd.dir = CCW;
        step = -step;
			  stepPosition=stepPosition-step;
    }
    else//顺时针
    {
			  stepPosition=stepPosition+step;
        srd.dir = CW;
    }
    // 输出电机方向
    DIR(srd.dir);
    // 配置电机为输出状态
   status.out_ena = TRUE;
    
    // 如果只移动一步
    if(step == 1)
    {
        // 只移动一步
        srd.accel_count = -1;
        // 减速状态
        srd.run_state = DECEL;
			
        // 短延时
        srd.step_delay = 1000;
        // 配置电机为运行状态
        status.running = TRUE;
        //设置定时器重装值	
			  TIM_Handle.Init.RepetitionCounter = Pulse_width;
        //设置占空比为50%	
			  sConfig.Pulse = Pulse_width>>1;
        //使能定时器	      
			 HAL_TIM_OC_Start(&TIM_Handle,ADVANCE_TIM_CH);
     }
    // 步数不为零才移动
    else if(step != 0)
    {
    // 我们的驱动器用户手册有详细的计算及推导过程

    // 设置最大速度极限, 计算得到min_delay用于定时器的计数器的值。
    // min_delay = (alpha / tt)/ w
    srd.min_delay = A_T_x100 / speed;

    // 通过计算第一个(c0) 的步进延时来设定加速度，其中accel单位为0.01rad/sec^2
    // step_delay = 1/tt * sqrt(2*alpha/accel)
    // step_delay = ( tfreq*0.676/100 )*100 * sqrt( (2*alpha*10000000000) / (accel*100) )/10000
    srd.step_delay = (T1_FREQ_148 * sqrt(A_SQ / accel))/100;

    // 计算多少步之后达到最大速度的限制
    // max_s_lim = speed^2 / (2*alpha*accel)
    max_s_lim = (long)speed*speed/(long)(((long)A_x20000*accel)/100);
    // 如果达到最大速度小于0.5步，我们将四舍五入为0
    // 但实际我们必须移动至少一步才能达到想要的速度
    if(max_s_lim == 0)
    {
        max_s_lim = 1;
    }

    // 计算多少步之后我们必须开始减速
    // n1 = (n1+n2)decel / (accel + decel)
    accel_lim = ((long)step*decel) / (accel+decel);
    // 我们必须加速至少1步才能才能开始减速.
    if(accel_lim == 0)
    {
        accel_lim = 1;
    }
    // 使用限制条件我们可以计算出第一次开始减速的位置
    //srd.decel_val为负数
    if(accel_lim <= max_s_lim)
    {
        srd.decel_val = accel_lim - step;
    }
    else
    {
        srd.decel_val = -(long)(max_s_lim*accel/decel);
    }
    // 当只剩下一步我们必须减速
    if(srd.decel_val == 0)
    {
        srd.decel_val = -1;
    }

    // 计算开始减速时的步数
    srd.decel_start = step + srd.decel_val;

    // 如果最大速度很慢，我们就不需要进行加速运动
    if(srd.step_delay <= srd.min_delay)
    {
        srd.step_delay = srd.min_delay;
        srd.run_state = RUN;
    }
    else
    {
        srd.run_state = ACCEL;
    }
    			  
    // 复位加速度计数值
    srd.accel_count = 0;
    status.running = TRUE;
    //设置定时器重装值	
		TIM_Handle.Init.RepetitionCounter = Pulse_width;
		//设置占空比为50%	
		sConfig.Pulse = Pulse_width>>1;
		 HAL_TIM_OC_Start(&TIM_Handle,ADVANCE_TIM_CH);
    }
}

/*
 * 注意：TIM_TimeBaseInitTypeDef结构体里面有5个成员，TIM6和TIM7的寄存器里面只有
 * TIM_Prescaler和TIM_Period，所以使用TIM6和TIM7的时候只需初始化这两个成员即可，
 * 另外三个成员是通用定时器和高级定时器才有.
 *-----------------------------------------------------------------------------
 * TIM_Prescaler         都有
 * TIM_CounterMode			 TIMx,x[6,7]没有，其他都有（基本定时器）
 * TIM_Period            都有
 * TIM_ClockDivision     TIMx,x[6,7]没有，其他都有(基本定时器)
 * TIM_RepetitionCounter TIMx,x[1,8]才有(高级定时器)
 *-----------------------------------------------------------------------------
 */
 
  /**
  * @brief  高级定时器 TIMx,x=[1,8]初始化函数
  * @param  无
  * @retval 无
  */
static void TIMx_Configuration(void)
{
    //使能TIMx的时钟
    ADVANCE_TIM_CLK_ENABLE();    
    //TIMx的外设寄存器基地址
    TIM_Handle.Instance = SMD_PULSE_TIM;
    //计数模式为向上计数，递增
    TIM_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    /* 累计 TIM_Period个后产生一个更新或者中断*/		
    //当定时器从0计数到9999，即为10000次，为一个定时周期
    TIM_Handle.Init.Period = SMD_PULSE_TIM_PERIOD;
    //时钟不分频，即一个TIM_CLK时钟计1次
    TIM_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    //定时器时钟源TIMxCLK = 2 * PCLK1  
    //				PCLK1 = HCLK / 4 
    //				=> TIMxCLK=HCLK/2=SystemCoreClock/2=200MHz
    // 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=1000000Hz
    TIM_Handle.Init.Prescaler =  SMD_PULSE_TIM_PSC - 1;   
    // 初始化定时器TIM
    HAL_TIM_PWM_Init(&TIM_Handle);
  
    /* 配置TIM为输出模式 */
    sConfig.OCMode = TIM_OCMODE_PWM1;
	  // 输出通道电平极性配置
    sConfig.OCPolarity = TIM_OCNPOLARITY_LOW;
		 
		sConfig.Pulse = SMD_PULSE_TIM_PERIOD/2;
		
    HAL_TIM_PWM_ConfigChannel(&TIM_Handle,&sConfig,ADVANCE_TIM_CH);//配置TIM3通道
    HAL_TIM_PWM_Start(&TIM_Handle,ADVANCE_TIM_CH);//开启PWM通道3
   
	 
		HAL_NVIC_SetPriority(SMD_PULSE_TIM_IRQ,0,0);    //设置中断优先级，抢占优先级1，子优先级3
		HAL_NVIC_EnableIRQ(SMD_PULSE_TIM_IRQ);          //开启ITM3中断   
    HAL_TIM_OC_Stop(&TIM_Handle,ADVANCE_TIM_CH); 		
}

/**
  * @brief  初始化基本定时器定时，1ms产生一次中断
  * @param  无
  * @retval 无
  */
void TIM_Advance_Init(void)
{
    //TIM通道引脚初始化
    TIMx_GPIO_Configuration();
    //TIM外设初始化
    TIMx_Configuration();
    HAL_TIM_Base_Start_IT(&TIM_Handle); //使能定时器3和定时器3更新中断：TIM_IT_UPDATE 
    HAL_TIM_OC_Stop(&TIM_Handle,ADVANCE_TIM_CH);    
	
}



void SMD_PULSE_TIM_IRQHandler(void)
{
	
  // 保存下一个延时周期
  unsigned int new_step_delay;
  // 加速过程中最后一次延时.
  static int last_accel_delay;
  // 移动步数计数器
  static unsigned int step_count = 0;
  // 记录new_step_delay中的余数，提高下一步计算的精度
  static signed int rest = 0;

if (__HAL_TIM_GET_IT_SOURCE(&TIM_Handle, TIM_IT_UPDATE) != RESET)
{
    /* Clear SMD_PULSE_TIM Capture Compare1 interrupt pending bit*/
    __HAL_TIM_CLEAR_IT(&TIM_Handle, TIM_IT_UPDATE);

    SMD_PULSE_TIM->CCR4=srd.step_delay >> 1;//周期的一半
    SMD_PULSE_TIM->ARR=srd.step_delay;
    //如果禁止输出，电机则停止运动
    if(status.out_ena != TRUE)
    {
        srd.run_state = STOP;
    }
  switch(srd.run_state) {
    case STOP:
      step_count = 0;
      rest = 0;
      SMD_PULSE_TIM->CCER &= ~(1<<12); //禁止输出
		  HAL_TIM_OC_Stop(&TIM_Handle,ADVANCE_TIM_CH);
      status.running = FALSE;
      break;

    case ACCEL:
      SMD_PULSE_TIM->CCER |= 1<<12; //使能输出
      step_count++;
      srd.accel_count++;
      new_step_delay = srd.step_delay - (((2 * (long)srd.step_delay) 
                       + rest)/(4 * srd.accel_count + 1));
      rest = ((2 * (long)srd.step_delay)+rest)%(4 * srd.accel_count + 1);
      //检查是够应该开始减速
      if(step_count >= srd.decel_start) {
        srd.accel_count = srd.decel_val;
        srd.run_state = DECEL;
      }
      //检查是否到达期望的最大速度
      else if(new_step_delay <= srd.min_delay) {
        last_accel_delay = new_step_delay;
        new_step_delay = srd.min_delay;
        rest = 0;
        srd.run_state = RUN;
      }
      break;

    case RUN:
      SMD_PULSE_TIM->CCER |= 1<<12; //使能输出
      step_count++;
      new_step_delay = srd.min_delay;
      //检查是否需要开始减速
      if(step_count >= srd.decel_start) {
        srd.accel_count = srd.decel_val;
        //以最后一次加速的延时作为开始减速的延时
        new_step_delay = last_accel_delay;
        srd.run_state = DECEL;
      }
      break;

    case DECEL:
      SMD_PULSE_TIM->CCER |= 1<<12; //使能输出
      step_count++;
      srd.accel_count++;
      new_step_delay = srd.step_delay - (((2 * (long)srd.step_delay) 
                       + rest)/(4 * srd.accel_count + 1));
      rest = ((2 * (long)srd.step_delay)+rest)%(4 * srd.accel_count + 1);
      //检查是否为最后一步
      if(srd.accel_count >= 0){
        srd.run_state = STOP;
      }
      break;
  }
  srd.step_delay = new_step_delay;
 }


}
/*********************************************END OF FILE**********************/

