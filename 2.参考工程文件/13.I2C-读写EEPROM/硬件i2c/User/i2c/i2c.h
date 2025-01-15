#ifndef __I2C_H
#define __I2C_H

#include "stm32f10x.h"
#include "stm32f10x_i2c.h"

// I2C相关参数宏定义
#define I2Cx I2C1
#define I2C_SCL_Port GPIOB
#define I2C_SCL_Pin GPIO_Pin_6
#define I2C_SDA_Port GPIOB
#define I2C_SDA_Pin GPIO_Pin_7
#define I2C_Speend 400000 // I2C时钟频率
#define I2Cx_OWM_ADDRESS 0x0A // 这个地址只需与STM32外挂的I2C器件地址不一样即可

// AT34C01/02每页有8个字节
#define I2C_PageSize 8

// 等待超时时间
#define I2CT_FLAG_TIMEOUT ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT ((uint32_t)(10*I2CT_FLAG_TIMEOUT))

// EEPROM Block 地址定义
#define EEPROM_Block0_ADDRESS 0xA0   
//#define EEPROM_Block1_ADDRESS 0xA2 
//#define EEPROM_Block2_ADDRESS 0xA4 
//#define EEPROM_Block3_ADDRESS 0xA6 

// 信息输出
#define EEPROM_DEBUG_ON 0
#define EEPROM_INFO(fmt,arg...)           printf("<<-EEPROM-INFO->> "fmt"\n",##arg)
#define EEPROM_ERROR(fmt,arg...)          printf("<<-EEPROM-ERROR->> "fmt"\n",##arg)
#define EEPROM_DEBUG(fmt,arg...)          do{\
                                          if(EEPROM_DEBUG_ON)\
                                          printf("<<-EEPROM-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)

void I2C_EEPROM_Init(void);
uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode);
void I2C_EEPROM_WaitEepromStandbyState(void);
uint32_t I2C_EEPROM_ByetWrite(uint8_t* pBuffer, uint8_t WriteAddr);
uint32_t I2C_EEPROM_PageWrite(uint8_t* pBuffer, uint8_t WriteAddr, uint8_t NumByteToWrite);
void I2C_EEPROM_BufferWrite(uint8_t* pBuffer, uint8_t WriteAddr, uint8_t NumByteToWrite);
uint32_t I2C_EEPROM_BufferRead(uint8_t* pBuffer, uint8_t ReadAddr, uint8_t NumByteToRead);


#endif /* __I2C_H */

/*
AT24C02 有2KB空间 = 2048bit = 256Byte = 32页 x 8字节
设备地址（高四位固定）：
1 0 1 0 A2 A1 A0 R/W
----------------------
1 0 1 0 0 0 0 0 0 = 0xA0
1 0 1 0 0 0 0 0 1 = 0xA1
*/
