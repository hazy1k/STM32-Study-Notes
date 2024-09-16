#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_clkconfig.h"
#include "bsp_mcooutput.h"

// �����ʱ������ʹ�ò�ͬ��ϵͳʱ�ӣ���ʱ��һ��
void Delay(__IO u32 nCount); 

int main(void)
{	
	// ��������main����֮ǰ�������ļ���statup_stm32f10x_hd.s�Ѿ�����
	// SystemInit()������ϵͳʱ�ӳ�ʼ����72MHZ
	// SystemInit()��system_stm32f10x.c�ж���
	// ����û����޸�ϵͳʱ�ӣ������б�д�����޸�
	
	// ��������ϵͳʱ�ӣ���ʱ�����ѡ��ʹ��HSE����HSI
	
	// ʹ��HSEʱ��SYSCLK = 8M * RCC_PLLMul_x, x:[2,3,...16],�����128M
	//HSE_SetSysClock(RCC_PLLMul_9);
	
	// ʹ��HSIʱ��SYSCLK = 4M * RCC_PLLMul_x, x:[2,3,...16],�����64MH
	HSI_SetSysClock(RCC_PLLMul_16);
	
	// MCO ���ų�ʼ��
	MCO_GPIO_Config();
	
	// ����MCO�������ʱ�ӣ���ʾ����������PA8�����������ʱ���źţ�
	// ���ǿ��԰�PLLCLK/2��ΪMCO���ŵ�ʱ�������ϵͳʱ���Ƿ�����׼ȷ
	// MCO�������������HSE,HSI,PLLCLK/2,SYSCLK	
	// RCC_MCOConfig(RCC_MCO_HSE);	             	        
	// RCC_MCOConfig(RCC_MCO_HSI);	                   
	// RCC_MCOConfig(RCC_MCO_PLLCLK_Div2);    	
	RCC_MCOConfig(RCC_MCO_SYSCLK);		      
	
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

