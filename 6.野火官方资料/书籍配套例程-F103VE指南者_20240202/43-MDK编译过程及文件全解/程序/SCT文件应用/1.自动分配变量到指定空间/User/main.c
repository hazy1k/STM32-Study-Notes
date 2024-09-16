/**
  ******************************************************************************
  * @file    SCT�ļ�Ӧ��
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   SCT�ļ�Ӧ��
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
#include <stdlib.h>


void Delay(__IO u32 nCount); 




//�����������ָ���Ĵ洢�ռ䡱
uint32_t testValue  =7 ;
//�����������ָ���Ĵ洢�ռ䡱
uint32_t testValue2  =0;



//�������鵽��ָ���Ĵ洢�ռ䡱
uint8_t testGrup[100]  ={0};
//�������鵽��ָ���Ĵ洢�ռ䡱
uint8_t testGrup2[100] ={1,2,3};


/*��ʵ���е�sct���ã���ʹ���ⲿ�洢��ʱ�������������ܲ�������
  ʹ��malloc�޷��õ������ĵ�ַ�����Ƽ���ʵ�ʹ���Ӧ��*/
/*��һ�������Ƽ���������ο��̳��е�˵��*/

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
	uint32_t inerTestValue =10;
	
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();	 
    
  /* ��ʼ������ */
  USART_Config();
  
  printf("\r\nSCT�ļ�Ӧ�á����Զ������������ָ���Ĵ洢�ռ䡱ʵ��\r\n");
  
	printf("\r\nʹ�á�	uint32_t inerTestValue =10; ����䶨��ľֲ�������\r\n");
	printf("��������ĵ�ַΪ��0x%x,����ֵΪ��%d\r\n",(uint32_t)&inerTestValue,inerTestValue);
	
  printf("\r\nʹ�á�uint32_t testValue  =7 ;����䶨���ȫ�ֱ�����\r\n");
	printf("��������ĵ�ַΪ��0x%x,����ֵΪ��%d\r\n",(uint32_t)&testValue,testValue);
	
  printf("\r\nʹ�á�uint32_t testValue2  =0 ; ����䶨���ȫ�ֱ�����\r\n");
	printf("��������ĵ�ַΪ��0x%x,����ֵΪ��%d\r\n",(uint32_t)&testValue2,testValue2);
	
	
	printf("\r\nʹ�á�uint8_t testGrup[100]  ={0};����䶨���ȫ�����飺\r\n");
	printf("��������ĵ�ַΪ��0x%x,����ֵΪ��%d,%d,%d\r\n",(uint32_t)&testGrup,testGrup[0],testGrup[1],testGrup[2]);
	
  printf("\r\nʹ�á�uint8_t testGrup2[100] ={1,2,3};����䶨���ȫ�����飺\r\n");
	printf("��������ĵ�ַΪ��0x%x,����ֵΪ��%d��%d,%d\r\n",(uint32_t)&testGrup2,testGrup2[0],testGrup2[1],testGrup2[2]);
	
	
/*��ʵ���е�sct���ã���ʹ���ⲿ�洢��ʱ�������������ܲ�������
  ʹ��malloc�޷��õ������ĵ�ַ�����Ƽ���ʵ�ʹ���Ӧ��*/
  /*��һ�������Ƽ���������ο��̳��е�˵��*/
	
	uint32_t * pointer = (uint32_t*)malloc(sizeof(uint32_t)*3); 
	if(pointer != NULL)
	{
		*(pointer)=1;
		*(++pointer)=2;
		*(++pointer)=3;	
		
		printf("\r\nʹ�á�	uint32_t *pointer = (uint32_t*)malloc(sizeof(uint32_t)*3); ����̬����ı���\r\n");
		printf("\r\n�����Ĳ���Ϊ��\r\n*(pointer++)=1;\r\n*(pointer++)=2;\r\n*pointer=3;\r\n\r\n");
		printf("��������������ĵ�ַΪ��0x%x,�鿴����ֵ������\r\n",(uint32_t)pointer); 
		printf("*(pointer--)=%d, \r\n",*(pointer--));
		printf("*(pointer--)=%d, \r\n",*(pointer--));
		printf("*(pointer)=%d, \r\n",*(pointer));
		
		free(pointer);
	}
	else
	{
		printf("\r\nʹ��malloc��̬���������������\r\n");	
	}
  
  LED_BLUE; 
	

  while(1); 
 

}


void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}

/*********************************************END OF FILE**********************/
