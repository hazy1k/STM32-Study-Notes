/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ���SPWM������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32 ָ���� ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include "./spwm/bsp_spwm.h"


//�ñ����ڶ�ʱ���жϷ�������ʹ�ã����ڿ��Ƹ�ͨ�������
//�޸ĸñ�����ֵ��ֱ�Ӹı�����Ƶ���ɫ
//������ʽ��RGB888
__IO uint32_t rgb_color = 0xFF00FF;

#define SOFT_DELAY() Delay(0x1FFFFFF);

void Delay(__IO u32 nCount); 


/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{			
	/* ��ʼ�������� */
	TIMx_Breathing_Init();

	while(1)
	{
		
		//������ɫ�ĺ�����
//	  rgb_color = 0xFF00FF;
//		SOFT_DELAY();		

//		rgb_color =0x8080ff;
//		SOFT_DELAY();
//		
//		rgb_color =0xff8000;
//		SOFT_DELAY();
//		
//		rgb_color =0xffc90e;
//		SOFT_DELAY();
	
	}		
}


void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}
/*********************************************END OF FILE**********************/

