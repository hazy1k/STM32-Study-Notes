#include "stm32f10x.h"
#include "./beep/bsp_beep.h"

void Delay(__IO u32 nCount); 

int main(void)
{	
	// 初始化蜂鸣器GPIO
	BEEP_GPIO_Config();	
	
	while(1)
	{		
		BEEP( ON ); // 开启蜂鸣器	
		Delay(0x0FFFFF);	

		BEEP( OFF );		
		Delay(0x0FFFFF);
	}
}

// 延时函数
void Delay(__IO uint32_t nCount) 
{
	for(; nCount != 0; nCount--);
}
