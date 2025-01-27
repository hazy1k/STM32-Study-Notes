#include "stm32f10x.h"
#include "led.h"
#include "AdvanceTim.h" 
#include "usart.h"

extern volatile uint32_t time;

int main(void)
{ 
	LED_Init();
	ATIM_Config();
	USART_Config();
  while(1)
  {
    if(time == 1000) // 1000 * 1 ms = 1s ʱ�䵽 
    {
      time = 0;
			LED1_TOGGLE(); 
			printf("������TIM1������1s��ʱ���ж�\r\n");
    }        
  }
}
