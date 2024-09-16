
// �߼���ʱ��TIMx,x[1,8]��ʱӦ��
#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_AdvanceTim.h" 

uint32_t time = 0; // ms ��ʱ���� 

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{
	/* led �˿����� */ 
	LED_GPIO_Config();
	
	ADVANCE_TIM_Init();
	
  while(1)
  {
    if ( time == 1000 ) /* 1000 * 1 ms = 1s ʱ�䵽 */
    {
      time = 0;
			/* LED1 ȡ�� */      
			LED1_TOGGLE; 
    }        
  }
}
/*********************************************END OF FILE**********************/

