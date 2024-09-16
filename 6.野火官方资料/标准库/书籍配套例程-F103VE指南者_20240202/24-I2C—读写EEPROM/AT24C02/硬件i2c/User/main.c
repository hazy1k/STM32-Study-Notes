/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   I2C EEPROM(AT24C02)���ԣ�������Ϣͨ��USART1��ӡ�ڵ��Եĳ����ն�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_usart.h"
#include "./i2c/bsp_i2c_ee.h"
#include <string.h>

#define  EEP_Firstpage      0x00
uint8_t I2c_Buf_Write[256];
uint8_t I2c_Buf_Read[256];
uint8_t I2C_Test(void);

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{ 
  LED_GPIO_Config();
  
  LED_BLUE;
  /* ���ڳ�ʼ�� */
	USART_Config();
	
	printf("\r\n ����һ��I2C����(AT24C02)��д�������� \r\n");

	/* I2C �����(AT24C02)ʼ�� */
	I2C_EE_Init();

	printf("\r\n ����һ��I2C����(AT24C02)��д�������� \r\n");	
	 	 
  //EEPROM ��д����
	if(I2C_Test() ==1)
	{
			LED_GREEN;
	}
	else
	{
			LED_RED;
	}
  
  while (1)
  {      
  }
}

/**
  * @brief  I2C(AT24C02)��д����
  * @param  ��
  * @retval ��������1���쳣����0
  */
uint8_t I2C_Test(void)
{
	uint16_t i;

	printf("д�������\n\r");
    
	for ( i=0; i<=255; i++ ) //��仺��
  {   
    I2c_Buf_Write[i] = i;

    printf("0x%02X ", I2c_Buf_Write[i]);
    if(i%16 == 15)    
        printf("\n\r");    
   }

  //��I2c_Buf_Write��˳�����������д��EERPOM�� 
	I2C_EE_BufferWrite( I2c_Buf_Write, EEP_Firstpage, 256);
  
  EEPROM_INFO("\n\rд�ɹ�\n\r");
   
   EEPROM_INFO("\n\r����������\n\r");
  //��EEPROM��������˳�򱣳ֵ�I2c_Buf_Read��
	I2C_EE_BufferRead(I2c_Buf_Read, EEP_Firstpage, 256); 
   
  //��I2c_Buf_Read�е�����ͨ�����ڴ�ӡ
	for (i=0; i<256; i++)
	{	
		if(I2c_Buf_Read[i] != I2c_Buf_Write[i])
		{
			EEPROM_ERROR("0x%02X ", I2c_Buf_Read[i]);
			EEPROM_ERROR("����:I2C EEPROMд������������ݲ�һ��\n\r");
			return 0;
		}
    printf("0x%02X ", I2c_Buf_Read[i]);
    if(i%16 == 15)    
        printf("\n\r");
    
	}
  EEPROM_INFO("I2C(AT24C02)��д���Գɹ�\n\r");
  
  return 1;
}
/*********************************************END OF FILE**********************/
