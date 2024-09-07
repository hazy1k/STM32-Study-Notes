#ifndef __DS18B20_H_
#define __DS18B20_H_

#include "fsl_debug_console.h"
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
/*********************************************************
 * DS18B20端口、引脚号及IOMUXC复用宏定义
 *********************************************************/
#define DS18B20_HIGH  1
#define DS18B20_LOW   0
#define DS18B20_GPIO             	GPIO1
#define DS18B20_GPIO_PIN         	(27U)
#define DS18B20_IOMUXC           	IOMUXC_GPIO_AD_B1_11_GPIO1_IO27
#define DS18B20_DATA_OUT(a)				GPIO_PinWrite(DS18B20_GPIO,DS18B20_GPIO_PIN,a)
#define DS18B20_DATA_IN()	 				GPIO_PinRead(DS18B20_GPIO,DS18B20_GPIO_PIN)
#define	SET		1
#define RESET 0																
uint8_t DS18B20_Init(void);
float DS18B20_Get_Temp(void);
void DS18B20_ReadId ( uint8_t * ds18b20_id );					
																	 
#endif //__DS18B20_H_
