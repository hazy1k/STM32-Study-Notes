// ͨ�ö�ʱ��TIMx,x[2,3,4,5]��ʱӦ��
#include "stm32f10x.h"
#include "led.h"
#include "GENERALTim.h" 
#include "usart.h"

uint32_t time = 0;

int main(void)
{
  LED_Init();
  USART_Config();
  TIMx_Init();
  while(1)
  {
    if(time == 1000) // 1000*1 = 1s
    {
      time = 0;
      printf("����һ����TIM2�����жϷ����ľ���Ŷ\r\n");
      LED1_TOGGLE();
    }

  }
}
