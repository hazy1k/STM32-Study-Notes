#include "led.h"   

void LED_GPIO_Config(void)
{		
		// 1.定义结构体-老套路了
		GPIO_InitTypeDef GPIO_InitStructure;

		// 2.开启控制GIPO的时钟也就是APB2咯
		RCC_APB2PeriphClockCmd( LED1_GPIO_CLK | LED2_GPIO_CLK | LED3_GPIO_CLK, ENABLE);
	
		// 3.选择我们要控制的引脚
		GPIO_InitStructure.GPIO_Pin = LED1_GPIO_PIN;	
	
		// 4.设置引脚模式为通用推挽输出
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	
		// 5.设置引脚速率为50MHz   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

		// 6.调用库函数，初始化GPIO
		GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure);	
		
	    // 如此，我们就完成了GPIO（控制LED1）的初始化工作了，那么LED2和LED3呢？-照葫芦画瓢

		/*选择要控制的GPIO引脚*/
		GPIO_InitStructure.GPIO_Pin = LED2_GPIO_PIN;
	
		/*调用库函数，初始化GPIO*/
		GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStructure);
		
		/*选择要控制的GPIO引脚*/
		GPIO_InitStructure.GPIO_Pin = LED3_GPIO_PIN;

		/*调用库函数，初始化GPIO*/
		GPIO_Init(LED3_GPIO_PORT, &GPIO_InitStructure);

		// 初始化时关闭所有led灯
		GPIO_SetBits(LED1_GPIO_PORT, LED1_GPIO_PIN);
		GPIO_SetBits(LED2_GPIO_PORT, LED2_GPIO_PIN);	 
		GPIO_SetBits(LED3_GPIO_PORT, LED3_GPIO_PIN);
}
