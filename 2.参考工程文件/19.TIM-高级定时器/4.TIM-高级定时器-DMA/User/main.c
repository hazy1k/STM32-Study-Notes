#include "stm32f10x.h"
#include "./tim/bsp_advance_tim.h"
#include "./key/bsp_key.h" 

extern uint16_t aSRC_Buffer[3];

int main(void)
{	
	/* 初始化按键GPIO */
  Key_GPIO_Config();
	
  /* 初始化高级控制定时器，设置PWM模式 */
	TIMx_Configuration();
  
  while(1)
  {       
		/* 扫描KEY1 */
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
		{
			aSRC_Buffer[0]=1000;
			aSRC_Buffer[1]=1000;
			aSRC_Buffer[2]=100;
		}   
    /* 扫描KEY2 */
    if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
		{
			aSRC_Buffer[0]=300;
			aSRC_Buffer[1]=300;
			aSRC_Buffer[2]=100;
		}   
  }
}
