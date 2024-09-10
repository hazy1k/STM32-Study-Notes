/**
  ******************************************************************
  * @file    rc522_config.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   RC522测试
  ******************************************************************
  * @attention
  *
  * 实验平台:野火 STM32 F407 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************
  */  

#include "./RC522/rc522_config.h"
#include "stm32f4xx.h"

static void   RC522_SPI_Config  ( void );

/**
  * @brief  RC522初始化
  * @param  无
  * @retval 无
  */
void RC522_Init ( void )
{
	RC522_SPI_Config ();
	
	RC522_Reset_Disable();
	
	RC522_CS_Disable();
}

/**
  * @brief  SPI配置
  * @param  无
  * @retval 无
  */
static void RC522_SPI_Config ( void )
{
  /* SPI_InitTypeDef  SPI_InitStructure */
  GPIO_InitTypeDef GPIO_InitStruct;
  
	/* 使能相应端口的时钟 */
	RC522_GPIO_CS_CLK() ;
	RC522_GPIO_SCK_CLK();
	RC522_GPIO_MOSI_CLK();
	RC522_GPIO_MISO_CLK();
	RC522_GPIO_RST_CLK();
	
	/* 配置片选引脚 */
	GPIO_InitStruct.Pin = RC522_GPIO_CS_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(RC522_GPIO_CS_PORT,&GPIO_InitStruct);
	
	/* 配置时钟引脚 */
	GPIO_InitStruct.Pin = RC522_GPIO_SCK_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(RC522_GPIO_SCK_PORT,&GPIO_InitStruct);
	
	/* 配置MOSI引脚 */
	GPIO_InitStruct.Pin = RC522_GPIO_MOSI_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(RC522_GPIO_MOSI_PORT,&GPIO_InitStruct);
	
	/* 配置MISO引脚 */
	GPIO_InitStruct.Pin = RC522_GPIO_MISO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(RC522_GPIO_MISO_PORT,&GPIO_InitStruct);
	
	/* 配置复位引脚 */
	GPIO_InitStruct.Pin = RC522_GPIO_RST_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(RC522_GPIO_RST_PORT,&GPIO_InitStruct);

}


