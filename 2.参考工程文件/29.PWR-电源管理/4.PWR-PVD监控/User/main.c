#include "stm32f10x.h"
#include "./led/bsp_led.h"
#include "./key/bsp_key.h" 
#include "./usart/bsp_usart.h"
#include "./pvd/bsp_pvd.h"

int main(void)
{	
	LED_GPIO_Config();	
	// ���̵ƣ���ʾ��������
	LED_GREEN; 
	// ����PVD������ѹ����ʱ��������жϷ������������
	PVD_Config();
	
  while(1)
  {					
		/*�������еĳ���*/
  }
}