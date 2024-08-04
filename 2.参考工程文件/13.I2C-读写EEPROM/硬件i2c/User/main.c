#include "stm32f10x.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_usart.h"
#include "./i2c/bsp_i2c_ee.h"
#include <string.h>

#define  EEP_Firstpage      0x00 // ����EEPROM����ʼҳ��ַ
uint8_t I2c_Buf_Write[256]; // ����д�뻺����
uint8_t I2c_Buf_Read[256];  // �����ȡ������
uint8_t I2C_Test(void);

int main(void)
{ 
  LED_GPIO_Config(); // LED��ʼ��
  LED_BLUE; // ������
	USART_Config(); // USART��ʼ��
	printf("\r\n ����һ��I2C����(AT24C02)��д�������� \r\n");

	// I2C �����(AT24C02)ʼ��
	I2C_EE_Init();
	printf("\r\n ����һ��I2C����(AT24C02)��д�������� \r\n");	
	 	 
  //EEPROM ��д����
	if(I2C_Test() == 1)
	{
			LED_GREEN; // �̵���
	}
	else
	{
			LED_RED; // ���󣬺����
	}
  
  while (1)
  {      
  }
}

// I2C EEPROM��д���Ժ���
uint8_t I2C_Test(void)
{
	uint16_t i;
	printf("д�������\n\r");
	for ( i=0; i <= 255; i++) //��仺��
  {   
    I2c_Buf_Write[i] = i; // д������
    printf("0x%02X ", I2c_Buf_Write[i]); // ��ӡд������
    if(i % 16 == 15) // ÿ�д�ӡ16������   
        printf("\n\r");    
   }

  //��I2c_Buf_Write��˳�����������д��EERPOM�� 
	I2C_EE_BufferWrite(I2c_Buf_Write, EEP_Firstpage, 256); // д��EEPROM����������д�뻺��������ʼҳ��ַ��д�볤��
  EEPROM_INFO("\n\rд�ɹ�\n\r"); // �궨�壬��ӡ��ʾ��Ϣ 
  EEPROM_INFO("\n\r����������\n\r"); // �궨�壬��ӡ��ʾ��Ϣ 
  //��EEPROM��������˳�򱣳ֵ�I2c_Buf_Read��
	I2C_EE_BufferRead(I2c_Buf_Read, EEP_Firstpage, 256); // ��ȡEEPROM������������ȡ����������ʼҳ��ַ����ȡ����
   
  // ��I2c_Buf_Read�е�����ͨ�����ڴ�ӡ
	for(i = 0; i < 256; i++)
	{	
		if(I2c_Buf_Read[i] != I2c_Buf_Write[i]) // �ж�д��������������Ƿ�һ��
		{
			EEPROM_ERROR("0x%02X ", I2c_Buf_Read[i]);
			EEPROM_ERROR("����:I2C EEPROMд������������ݲ�һ��\n\r");
			return 0;
		}
    printf("0x%02X ", I2c_Buf_Read[i]); // ��ӡ��������
    if(i % 16 == 15)    // ÿ�д�ӡ16������   
        printf("\n\r");
	}
  EEPROM_INFO("I2C(AT24C02)��д���Գɹ�\n\r");

  return 1;
}