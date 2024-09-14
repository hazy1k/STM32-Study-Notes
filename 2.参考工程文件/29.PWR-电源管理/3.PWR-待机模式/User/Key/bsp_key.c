#include "./key/bsp_key.h"  

// 按键引脚定义
void Key_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// 开启按键端口的时钟
	RCC_APB2PeriphClockCmd(KEY2_GPIO_CLK,ENABLE);

// 为强调PA0用作唤醒时无需初始化，此处注释掉
// 若PA0在STM32在正常运行时需要用作输入检测，则应加上初始化	
//	//选择按键的引脚
//	GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_PIN;  
//	// 设置按键的引脚为浮空输入
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
//	//使用结构体初始化按键
//	GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);
	
	// 选择按键的引脚
	GPIO_InitStructure.GPIO_Pin = KEY2_GPIO_PIN; 
	// 设置按键的引脚为浮空输入
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	// 使用结构体初始化按键
	GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure);	
}

// 按键扫描函数
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
{			
	// 检测是否有按键按下 
	if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON )  
	{	 
		// 等待按键释放 
		while(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON);   
		return 	KEY_ON;	 
	}
	else
		return KEY_OFF;
}