#ifndef __DS18B20_H
#define	__DS18B20_H



#include "stm32f10x.h"



/************************** DS18B20 连接引脚定义********************************/
#define      DS18B20_DQ_SCK_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define      DS18B20_DQ_GPIO_CLK                       RCC_APB2Periph_GPIOB

#define      DS18B20_DQ_GPIO_PORT                      GPIOB
#define      DS18B20_DQ_GPIO_PIN                       GPIO_Pin_2


/************************** DS18B20 函数宏定义********************************/
#define      DS18B20_DQ_0	                            GPIO_ResetBits ( DS18B20_DQ_GPIO_PORT, DS18B20_DQ_GPIO_PIN ) 
#define      DS18B20_DQ_1	                            GPIO_SetBits ( DS18B20_DQ_GPIO_PORT, DS18B20_DQ_GPIO_PIN ) 

#define      DS18B20_DQ_IN()	                          GPIO_ReadInputDataBit ( DS18B20_DQ_GPIO_PORT, DS18B20_DQ_GPIO_PIN ) 



/************************** DS18B20 函数声明 ********************************/
uint8_t                  DS18B20_Init                      ( void );
void                     DS18B20_ReadId                    ( uint8_t * ds18b20_id );
float                    DS18B20_GetTemp_SkipRom           ( void );
float                    DS18B20_GetTemp_MatchRom          ( uint8_t * ds18b20_id );



#endif /* __DS18B20_H */






