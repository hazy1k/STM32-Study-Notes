#include "stm32f10x.h"
#include "./led/bsp_led.h"
#include "./key/bsp_key.h" 
#include "./usart/bsp_usart.h"
#include "./pvd/bsp_pvd.h"

int main(void)
{	
	LED_GPIO_Config();	
	// 亮绿灯，表示正常运行
	LED_GREEN; 
	// 配置PVD，当电压过低时，会进入中断服务函数，亮红灯
	PVD_Config();
	
  while(1)
  {					
		/*正常运行的程序*/
  }
}