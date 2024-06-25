#include "stm32f10x.h"
#include "./beep/bsp_beep.h"

void Delay(__IO u32 nCount); 

int main(void)
{	
	// 蜂鸣器初始化
	BEEP_GPIO_Config();	
	
	while(1)
	{		
		BEEP( ON ); 	
		Delay(0x0FFFFF);	
		BEEP( OFF );		
		Delay(0x0FFFFF);
	}
}

void Delay(__IO uint32_t nCount) // 简单的延时函数
{
	for(; nCount != 0; nCount--);
}
