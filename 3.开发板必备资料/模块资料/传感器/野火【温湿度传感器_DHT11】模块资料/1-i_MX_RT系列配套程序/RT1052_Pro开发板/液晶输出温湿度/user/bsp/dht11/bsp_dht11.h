#ifndef __DHT11_H_
#define __DHT11_H_

#include "fsl_debug_console.h"
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_gpio.h" 

#define DHT11_HIGH  1
#define DHT11_LOW   0

/*********************************************************
 * DHT11端口、引脚号及IOMUXC复用宏定义
 *********************************************************/
#define DHT11_GPIO             GPIO1
#define DHT11_GPIO_PIN         (27U)
#define DHT11_IOMUXC           IOMUXC_GPIO_AD_B1_11_GPIO1_IO27

#define DHT11_DATA_OUT(a)		GPIO_PinWrite(DHT11_GPIO,DHT11_GPIO_PIN,a)
#define DHT11_DATA_IN()	  GPIO_PinReadPadStatus(DHT11_GPIO,DHT11_GPIO_PIN)
#define SUCCESS 1
#define ERROR 0
#define	Bit_SET		1
#define Bit_RESET 0
typedef struct
{
	uint8_t  humi_int;		//湿度的整数部分
	uint8_t  humi_deci;	 	//湿度的小数部分
	uint8_t  temp_int;	 	//温度的整数部分
	uint8_t  temp_deci;	 	//温度的小数部分
	uint8_t  check_sum;	 	//校验和
}DHT11_Data_TypeDef;

void DHT11_GPIO_Config(void);

uint8_t Read_DHT11(DHT11_Data_TypeDef *DHT11_Data);

#endif //__DHT11_H_
