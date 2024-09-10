/**
  ******************************************************************
  * @file    rc522_config.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   RC522����
  ******************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32 F407 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************
  */  

#include "./RC522/rc522_config.h"
#include "stm32f4xx.h"

static void   RC522_SPI_Config  ( void );

/**
  * @brief  RC522��ʼ��
  * @param  ��
  * @retval ��
  */
void RC522_Init ( void )
{
	RC522_SPI_Config ();
	
	RC522_Reset_Disable();
	
	RC522_CS_Disable();
}

/**
  * @brief  SPI����
  * @param  ��
  * @retval ��
  */
static void RC522_SPI_Config ( void )
{
  /* SPI_InitTypeDef  SPI_InitStructure */
  GPIO_InitTypeDef GPIO_InitStruct;
  
	/* ʹ����Ӧ�˿ڵ�ʱ�� */
	RC522_GPIO_CS_CLK() ;
	RC522_GPIO_SCK_CLK();
	RC522_GPIO_MOSI_CLK();
	RC522_GPIO_MISO_CLK();
	RC522_GPIO_RST_CLK();
	
	/* ����Ƭѡ���� */
	GPIO_InitStruct.Pin = RC522_GPIO_CS_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(RC522_GPIO_CS_PORT,&GPIO_InitStruct);
	
	/* ����ʱ������ */
	GPIO_InitStruct.Pin = RC522_GPIO_SCK_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(RC522_GPIO_SCK_PORT,&GPIO_InitStruct);
	
	/* ����MOSI���� */
	GPIO_InitStruct.Pin = RC522_GPIO_MOSI_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(RC522_GPIO_MOSI_PORT,&GPIO_InitStruct);
	
	/* ����MISO���� */
	GPIO_InitStruct.Pin = RC522_GPIO_MISO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(RC522_GPIO_MISO_PORT,&GPIO_InitStruct);
	
	/* ���ø�λ���� */
	GPIO_InitStruct.Pin = RC522_GPIO_RST_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(RC522_GPIO_RST_PORT,&GPIO_InitStruct);

}


