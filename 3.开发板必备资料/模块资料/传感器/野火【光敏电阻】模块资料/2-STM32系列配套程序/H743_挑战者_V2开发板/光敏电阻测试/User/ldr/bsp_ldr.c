/**
  ******************************************************************************
  * @file    bsp_photoresistors.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   光敏模块测试
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 STM32H750 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "./ldr/bsp_ldr.h" 
#include "./led/bsp_led.h"

/**
  * @brief  配置光敏用到的I/O口
  * @param  无
  * @retval 无
  */
void LDR_GPIO_Config(void)
{
	
/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	/*开启端口的时钟*/
	LDR_GPIO_CLK_ENABLE();
	
	//选择输入的引脚
	GPIO_InitStruct.Pin = LDR_PIN; 
	
  /*设置引脚的输出类型为推挽输出*/
	GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;  
  
 	/*设置引脚为上拉模式*/
	GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
	
	/*设置引脚速率为高速 */   
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; 

	/*调用库函数，使用上面配置的GPIO_InitStructure初始化GPIO*/
	HAL_GPIO_Init(LDR_GPIO_PORT, &GPIO_InitStruct);	
}

 /*
 * 函数名：光敏电阻测试
 * 描述  ：检测光敏电阻的状态
 * 输入  ：GPIOx：x 可以是 A，B，C，D或者 E
 *		     GPIO_Pin：待读取的端口位 	
 * 输出  ：LDR_ON：有光   LDR_OFF：无光
 */
uint8_t LDR_Test(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
{			
	/*检测光敏输入状态 */
	if(HAL_GPIO_ReadPin(GPIOx,GPIO_Pin) == 1 )  
	{	  
		return LDR_OFF;    // 无光
	}
	else
		return LDR_ON;    // 有光
}
/*********************************************END OF FILE**********************/
