/**
  ******************************************************************************
  * @file    FSMC���ⲿSRAM(�ڴ����)
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   sramӦ������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-ָ���� STM32  ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./led/bsp_led.h"  
#include "./sram/sram.h"
#include "malloc.h"



int main(void)
 {	   
	u8 *p=0;
	u8 sramx=0;				//Ĭ��Ϊ�ڲ�sram
  

  Debug_USART_Config();		 	                         //���ڳ�ʼ��Ϊ115200 
 	FSMC_SRAM_Init();			  //��ʼ���ⲿSRAM  
	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
	my_mem_init(SRAMEX);		//��ʼ���ⲿ�ڴ��

	 
	printf ( "\r\nҰ���ڴ����\r\n" );

	 
	printf ( "\r\n�����ڴ�\r\n" );
	 
  p = mymalloc ( sramx, 1024 *2 );//����2K�ֽ�
	
	sprintf((char*)p,"Memory Malloc");//ʹ������ռ�洢����
	
	printf ( "SRAMIN USED:%d%%\r\n", my_mem_perused(SRAMIN) );//��ʾ�ڲ��ڴ�ʹ����
	printf ( "SRAMEX USED:%d%%\r\n", my_mem_perused(SRAMEX) );//��ʾ�ⲿ�ڴ�ʹ����

	myfree(sramx,p);//�ͷ��ڴ�
	p=0;		      	//ָ��յ�ַ

  printf ( "\r\n�ͷ��ڴ�\r\n" );
	printf ( "SRAMIN USED:%d%%\r\n", my_mem_perused(SRAMIN) );//��ʾ�ڲ��ڴ�ʹ����
	printf ( "SRAMEX USED:%d%%\r\n", my_mem_perused(SRAMEX) );//��ʾ�ⲿ�ڴ�ʹ����
	
	
  while(1);
	
		
}



/*********************************************END OF FILE**********************/
