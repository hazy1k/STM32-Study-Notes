#include "stm32f10x.h"
#include "./dac/bsp_dac.h"
//DAC通道与SPI-FLASH引脚共用，所以DAC和SPI-FLASH程序不能同时运行。

int main(void)
{
			/*初始化DAC，开始DAC转换*/
			DAC_Mode_Init();
	
  	  while(1);	 
}