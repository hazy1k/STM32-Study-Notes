#ifndef __I2C_H
#define	__I2C_H

#include "stm32f10x.h"

// I2C��ز����궨��
#define  I2Cx         I2C1   
#define  I2C_SCL_PORT GPIOB   
#define  I2C_SCL_PIN  GPIO_Pin_6
#define  I2C_SDA_PORT GPIOB 
#define  I2C_SDA_PIN  GPIO_Pin_7
#define  I2C_Speed    400000 

// �����ַֻҪ��STM32��ҵ�I2C������ַ��һ������
#define I2Cx_OWN_ADDRESS7 0X0A  

// �ȴ���ʱʱ��
#define I2CT_FLAG_TIMEOUT  ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT  ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))

// ��Ϣ���
#define EEPROM_DEBUG_ON         0
#define EEPROM_INFO(fmt,arg...)           printf("<<-EEPROM-INFO->> "fmt"\n",##arg)
#define EEPROM_ERROR(fmt,arg...)          printf("<<-EEPROM-ERROR->> "fmt"\n",##arg)
#define EEPROM_DEBUG(fmt,arg...)          do{\
                                          if(EEPROM_DEBUG_ON)\
                                          printf("<<-EEPROM-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)

// AT24C01/02ÿҳ��8���ֽ�
#define I2C_PageSize   8
/* 
AT24C02 2kb = 2048bit = 2048/8 B = 256 B
32 pages of 8 bytes each
Device Address
 1 0 1 0 A2 A1 A0 R/W
 1 0 1 0 0  0  0  0 = 0XA0
 1 0 1 0 0  0  0  1 = 0XA1 
*/
// EEPROM��ַ�궨��
#define EEPROM_Block0_ADDRESS 0xA0   
//#define EEPROM_Block1_ADDRESS 0xA2 
//#define EEPROM_Block2_ADDRESS 0xA4  
//#define EEPROM_Block3_ADDRESS 0xA6  

void I2C_EE_Init(void);
uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode);
void I2C_EE_WaitEepromStandbyState(void);
uint32_t I2C_EE_ByteWrite(u8* pBuffer, u8 WriteAddr);
uint32_t I2C_EE_PageWrite(u8* pBuffer, u8 WriteAddr, u8 NumByteToWrite);
void I2C_EE_BufferWrite(u8* pBuffer, u8 WriteAddr, u16 NumByteToWrite);
uint32_t I2C_EE_BufferRead(u8* pBuffer, u8 ReadAddr, u16 NumByteToRead);

#endif /* __I2C_H */
