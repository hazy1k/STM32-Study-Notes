#include "stm32f10x.h"
#include "led.h"
#include "usart.h"
#include "I2C.h"
#include <string.h>

#define  EEP_Firstpage     0x00 // EEPROM����ʼ��ַ
uint8_t I2c_Buf_Write[256]; // д�뻺����
uint8_t I2c_Buf_Read[256];  // ��ȡ������
uint8_t I2C_Test(void);

int main(void)
{ 
  	LED_Init(); 
  	LED_BLUE(); 
  	USART_Config(); 
  	I2C_EE_Init();
	if(I2C_Test() == 1)
	{
		LED_GREEN();
	} 
	else
	{	
		LED_RED();
	} 
  while (1)
  {      
  }
}

// I2C EEPROM��д����
uint8_t I2C_Test(void)
{
	uint16_t i;
	printf("Ҫд�������n\r");
	for ( i=0; i <= 255; i++)
    {   
		I2c_Buf_Write[i] = i; 
    	printf("0x%02X ", I2c_Buf_Write[i]); 
    	if(i % 16 == 15)   
        	printf("\n\r");    
    }
	I2C_EE_BufferWrite(I2c_Buf_Write, EEP_Firstpage, 256); // д�뻺���������ݵ�EEPROM
  	EEPROM_INFO("\n\rд��ɹ�\r");  
  	EEPROM_INFO("\n\rEEPROM����������n\r");  
  	I2C_EE_BufferRead(I2c_Buf_Read, EEP_Firstpage, 256);
	for(i = 0; i < 256; i++) // ��ӡ���������Ƿ����ݶ�д��ͬ
	{	
		if(I2c_Buf_Read[i] != I2c_Buf_Write[i]) // ������ݲ���ͬ
		{
			EEPROM_ERROR("0x%02X ", I2c_Buf_Read[i]);
			EEPROM_ERROR("�������ζ��������ݲ���ͬ\n\r");
			return 0;
		}
    printf("0x%02X ", I2c_Buf_Read[i]); // ��ӡ��������������
    if(i % 16 == 15)  
        printf("\n\r");
	}
  return 1;
}
