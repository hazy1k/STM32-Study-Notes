#include "stm32f10x.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_usart.h"
#include "./i2c/bsp_i2c_ee.h"
#include <string.h>

#define  EEP_Firstpage      0x00 // 定义EEPROM的起始页地址
uint8_t I2c_Buf_Write[256]; // 定义写入缓冲区
uint8_t I2c_Buf_Read[256];  // 定义读取缓冲区
uint8_t I2C_Test(void);

int main(void)
{ 
  LED_GPIO_Config(); // LED初始化
  LED_BLUE; // 蓝灯亮
	USART_Config(); // USART初始化
	printf("\r\n 这是一个I2C外设(AT24C02)读写测试例程 \r\n");

	// I2C 外设初(AT24C02)始化
	I2C_EE_Init();
	printf("\r\n 这是一个I2C外设(AT24C02)读写测试例程 \r\n");	
	 	 
  //EEPROM 读写测试
	if(I2C_Test() == 1)
	{
			LED_GREEN; // 绿灯亮
	}
	else
	{
			LED_RED; // 错误，红灯亮
	}
  
  while (1)
  {      
  }
}

// I2C EEPROM读写测试函数
uint8_t I2C_Test(void)
{
	uint16_t i;
	printf("写入的数据\n\r");
	for ( i=0; i <= 255; i++) //填充缓冲
  {   
    I2c_Buf_Write[i] = i; // 写入数据
    printf("0x%02X ", I2c_Buf_Write[i]); // 打印写入数据
    if(i % 16 == 15) // 每行打印16个数据   
        printf("\n\r");    
   }

  //将I2c_Buf_Write中顺序递增的数据写入EERPOM中 
	I2C_EE_BufferWrite(I2c_Buf_Write, EEP_Firstpage, 256); // 写入EEPROM函数参数：写入缓冲区，起始页地址，写入长度
  EEPROM_INFO("\n\r写成功\n\r"); // 宏定义，打印提示信息 
  EEPROM_INFO("\n\r读出的数据\n\r"); // 宏定义，打印提示信息 
  //将EEPROM读出数据顺序保持到I2c_Buf_Read中
	I2C_EE_BufferRead(I2c_Buf_Read, EEP_Firstpage, 256); // 读取EEPROM函数参数：读取缓冲区，起始页地址，读取长度
   
  // 将I2c_Buf_Read中的数据通过串口打印
	for(i = 0; i < 256; i++)
	{	
		if(I2c_Buf_Read[i] != I2c_Buf_Write[i]) // 判断写入与读出的数据是否一致
		{
			EEPROM_ERROR("0x%02X ", I2c_Buf_Read[i]);
			EEPROM_ERROR("错误:I2C EEPROM写入与读出的数据不一致\n\r");
			return 0;
		}
    printf("0x%02X ", I2c_Buf_Read[i]); // 打印读出数据
    if(i % 16 == 15)    // 每行打印16个数据   
        printf("\n\r");
	}
  EEPROM_INFO("I2C(AT24C02)读写测试成功\n\r");

  return 1;
}