/**
  ******************************************************************************
  * @file    bsp_touchpad.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ���ݰ���Ӧ�ú����ӿ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F103 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
#include "./TouchPad/bsp_touchpad.h"
#include "./usart/bsp_debug_usart.h"
#include "stm32f1xx_hal.h"

//��ʱ��������ֵ
#define TPAD_ARR_MAX_VAL 	0XFFFF	

//����û����ʱ��ʱ������ֵ
__IO uint16_t tpad_default_val=0;

/***********************************
 *
 * ��ʱ�����벶������
 *
 ***********************************/	
 TIM_HandleTypeDef TIM_Handle;
 
 void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  /* ������ʱ������ʱ��ʹ�� */
  TOUCHPAD_TIM_RCC_CLK_ENABLE();
  /* ��ʱ��ͨ������ʱ��ʹ�� */
  TOUCHPAD_GPIO_RCC_CLK_ENABLE();
  /* ��ʱ��ͨ���������ã�����������Ϊ����ģʽ */
  GPIO_InitStruct.Pin = TOUCHPAD_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(TOUCHPAD_GPIO, &GPIO_InitStruct);
}
  
 
static void TPAD_TIM_Mode_Config(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_IC_InitTypeDef sConfigIC;
  
  TIM_Handle.Instance = TOUCHPAD_TIMx;
  TIM_Handle.Init.Prescaler = TOUCHPAD_TIM_PRESCALER;
  TIM_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
  TIM_Handle.Init.Period = TOUCHPAD_TIM_ARR;
  TIM_Handle.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_Base_Init(&TIM_Handle);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&TIM_Handle, &sClockSourceConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&TIM_Handle, &sMasterConfig);
  
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 3;
  HAL_TIM_IC_ConfigChannel(&TIM_Handle, &sConfigIC, TOUCHPAD_TIM_CHANNEL);
}

/****************************************
 *
 * Ϊ���ݰ����ŵ�
 * �����ʱ����־������
 *
 *****************************************/
static void TPAD_Reset(void)
{
     /* ����IOӲ����ʼ���ṹ����� */
  GPIO_InitTypeDef GPIO_InitStruct;
	
	/* ʹ�ܵ��ݰ������Ŷ�ӦIO�˿�ʱ�� */  
  TOUCHPAD_GPIO_RCC_CLK_ENABLE();
  
  /* �����������Ϊ�͵�ƽ */
  HAL_GPIO_WritePin(TOUCHPAD_GPIO, TOUCHPAD_GPIO_PIN, GPIO_PIN_RESET);  
  
  /* �趨���ݰ�����Ӧ����IO��� */
  GPIO_InitStruct.Pin = TOUCHPAD_GPIO_PIN;
  /* �趨���ݰ�����Ӧ����IOΪ���ģʽ */
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  /* �趨���ݰ�����Ӧ����IO�����ٶ� */
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  /* ��ʼ�����ݰ�����Ӧ����IO */
  HAL_GPIO_Init(TOUCHPAD_GPIO, &GPIO_InitStruct);  

	HAL_Delay(5);

	__HAL_TIM_SET_COUNTER(&TIM_Handle,0); // ���㶨ʱ������  
  __HAL_TIM_CLEAR_FLAG(&TIM_Handle, TIM_FLAG_UPDATE|TIM_FLAG_CC2);//����жϱ�־
  
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(TOUCHPAD_GPIO, &GPIO_InitStruct);
  
  HAL_TIM_IC_Start(&TIM_Handle,TOUCHPAD_TIM_CHANNEL);  
}

/****************************************************
 *
 * �õ���ʱ������ֵ
 * �����ʱ,��ֱ�ӷ��ض�ʱ���ļ���ֵ.
 *
 *****************************************************/
static uint16_t TPAD_Get_Val(void)
{		
  TPAD_Reset();
	while(__HAL_TIM_GET_FLAG(&TIM_Handle,TOUCHPAD_TIM_FLAG_CCR)==RESET)
  {    
    uint16_t count;
    count=__HAL_TIM_GET_COUNTER(&TIM_Handle);
		if(count>(TOUCHPAD_TIM_ARR-500))
      return count;//��ʱ��,ֱ�ӷ���CNT��ֵ
	};	
	return HAL_TIM_ReadCapturedValue(&TIM_Handle,TOUCHPAD_TIM_CHANNEL);
} 	

/****************************************************
 *
 * ��ȡn��,ȡ���ֵ
 * n��������ȡ�Ĵ���
 * ����ֵ��n�ζ������������������ֵ
 * 
 *****************************************************/
static uint16_t TPAD_Get_MaxVal(uint8_t n)
{
	uint16_t temp=0;
	uint16_t res=0;
	while(n--)
	{
		temp=TPAD_Get_Val();//�õ�һ��ֵ
		if(temp>res)res=temp;
	};
	return res;
}  

/********************************************************
*
* ��ʼ����������
* ��ÿ��ص�ʱ����������ȡֵ.
* ����ֵ:0,��ʼ���ɹ�;1,��ʼ��ʧ��
*
*********************************************************/
uint8_t TPAD_Init(void)
{
	uint16_t buf[10];
	uint32_t temp=0;
	uint8_t j,i;
	
	/* ��1.5Mhz��Ƶ�ʼ���  */
	TPAD_TIM_Mode_Config();
  HAL_TIM_IC_Start(&TIM_Handle,TOUCHPAD_TIM_CHANNEL);
  
  /* ������ȡ10�� */
	for(i=0;i<10;i++)
	{				 
		buf[i]=TPAD_Get_Val();
		HAL_Delay(10);	    
	}
  /* ���� */
	for(i=0;i<9;i++)
	{
		for(j=i+1;j<10;j++)
		{
      /* �������� */
			if(buf[i]>buf[j])
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}
	temp=0;
  /* ȡ�м��6�����ݽ���ƽ�� */
	for(i=2;i<8;i++)temp+=buf[i];
	tpad_default_val=temp/6;
	printf("tpad_default_val:%d\r\n",tpad_default_val);	
  /* ��ʼ����������TPAD_ARR_MAX_VAL/2����ֵ,������! */
	if(tpad_default_val>TOUCHPAD_TIM_ARR/2)return 1;
	return 0;		     			   
}
 

/*******************************************************************************
*
* ɨ�败������
* mode:0,��֧����������(����һ�α����ɿ����ܰ���һ��);1,֧����������(����һֱ����)
* ����ֵ:0,û�а���;1,�а���;			
*
*******************************************************************************/
//��ֵ������ʱ��������(tpad_default_val + TPAD_GATE_VAL),����Ϊ����Ч����.
#define TPAD_GATE_VAL 	100	

uint8_t TPAD_Scan(uint8_t mode)
{
	//0,���Կ�ʼ���;>0,�����ܿ�ʼ���	
	static uint8_t keyen=0;
	//ɨ����
	uint8_t res=0;
	//Ĭ�ϲ�������Ϊ3��
	uint8_t sample=3;	
  //����ֵ	
	uint16_t rval;
	
	if(mode)
	{
		//֧��������ʱ�����ò�������Ϊ6��
		sample=6;	
		//֧������	
		keyen=0;	  
	}	
	/* ��ȡ��ǰ����ֵ(���� sample ��ɨ������ֵ) */
	rval=TPAD_Get_MaxVal(sample); 	
	/* printf��ӡ��������ʹ�ã�����ȷ����ֵTPAD_GATE_VAL����Ӧ�ù�����Ӧע�͵� */
	printf("scan_rval=%d\n",rval);
	
	//����tpad_default_val+TPAD_GATE_VAL,��С��10��tpad_default_val,����Ч
	if(rval>(tpad_default_val+TPAD_GATE_VAL)&&rval<(10*tpad_default_val))
	{			
    //keyen==0,��Ч 		
		if(keyen==0)
		{
			res=1;		 
		}			
		keyen=3;				//����Ҫ�ٹ�3��֮����ܰ�����Ч   
	}
	
	if(keyen)
	{
		keyen--;		
	}		
	return res;
}	 
/*********************************************END OF FILE**********************/
