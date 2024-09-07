#ifndef __DS18B20_H_
#define __DS18B20_H_

#include "stm32f7xx.h"

#define DS18B20_HIGH  GPIO_PIN_SET
#define DS18B20_LOW   GPIO_PIN_RESET


#define Bit_RESET   0
#define Bit_SET     1
/*---------------------------------------*/
#define DS18B20_PORT                  GPIOE
#define DS18B20_PIN                   GPIO_PIN_2            
#define DS18B20_GPIO_CLK_ENABLE()     __GPIOE_CLK_ENABLE()


#define  DS18B20_DATA_IN()	          HAL_GPIO_ReadPin(DS18B20_PORT, DS18B20_PIN)
#define  DS18B20_DATA_OUT(value)	    HAL_GPIO_WritePin(DS18B20_PORT,DS18B20_PIN,value)

uint8_t DS18B20_Init(void);
float DS18B20_Get_Temp(void);
void DS18B20_ReadId ( uint8_t * ds18b20_id );			

#endif //__DS18B20_H_

