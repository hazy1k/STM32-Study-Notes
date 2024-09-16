/**
  ******************************************************************************
  * @file    main
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   RCC MCO���
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-ָ���� STM32  ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */


/* 
 * ����MCO���ţ�PA8 �����ṩʱ�ӣ����Ƶ�ʲ��ܳ���IO�ڵķ�תƵ��50MHZ
 * MCO ʱ����Դ�����ǣ�PLLCLK/2 ,HSI,HSE,SYSCLK
 */
#include "stm32f10x.h"
#include "./led/bsp_led.h"
#include "bsp_clkconfig.h"
#include "bsp_mcooutput.h"

//  �����ʱ������ʹ�ò�ͬ��ϵͳʱ�ӣ���ʱ��һ��
void Delay(__IO u32 nCount); 

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	// ��������main����֮ǰ�������ļ���statup_stm32f10x_hd.s�Ѿ�����SystemInit()������ϵͳʱ�ӳ�ʼ����72MHZ
	// SystemInit()��system_stm32f10x.c�ж���
	// ����û����޸�ϵͳʱ�ӣ������б�д�����޸�
	
	// ��������ϵͳʱ�ӣ�������Ҫ�޸ģ�һ���������Ϊ128MHz
	// SYSCLK = 8M * RCC_PLLMul_x, x:[2,3,...16]
	//HSE_SetSysClock(RCC_PLLMul_9);
	
	HSI_SetSysClock(RCC_PLLMul_16);
	// MCO ���ų�ʼ��
	MCO_GPIO_Config();
	
	// ����MCO�������ʱ�ӣ���ʾ����������PA8�����������ʱ���ź�
	// MCO�������������HSE,HSI,PLLCLK/2,SYSCLK
	//RCC_MCOConfig(RCC_MCO_HSE);	             // 8M	        
	//RCC_MCOConfig(RCC_MCO_HSI);	           // 8M 	        
	//RCC_MCOConfig(RCC_MCO_PLLCLK_Div2);    // 36M		
	RCC_MCOConfig(RCC_MCO_SYSCLK);		     // 72M  
	
	// LED �˿ڳ�ʼ��
	LED_GPIO_Config();
	while (1)
	{
		LED1( ON );			  // ��
		Delay(0x0FFFFF);
		LED1( OFF );		  // �� 
		Delay(0x0FFFFF);		
	}

}


//  �����ʱ������ʹ�ò�ͬ��ϵͳʱ�ӣ���ʱ��һ��
void Delay(__IO uint32_t nCount)	
{
	for(; nCount != 0; nCount--);
}
/*********************************************END OF FILE**********************/
