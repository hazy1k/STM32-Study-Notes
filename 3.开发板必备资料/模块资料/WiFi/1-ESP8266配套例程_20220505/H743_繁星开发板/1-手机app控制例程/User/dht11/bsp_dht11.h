#ifndef __DHT11_H_
#define __DHT11_H_

#include "stm32h7xx.h"

#define DHT11_HIGH  GPIO_PIN_SET
#define DHT11_LOW   GPIO_PIN_RESET


#define Bit_RESET   0
#define Bit_SET     1
/*---------------------------------------*/
#define DHT11_PORT                  GPIOE
#define DHT11_PIN                   GPIO_PIN_3           
#define DHT11_GPIO_CLK_ENABLE()     __GPIOE_CLK_ENABLE()


#define  DHT11_DATA_IN()	          HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)
#define  DHT11_DATA_OUT(value)	    HAL_GPIO_WritePin(DHT11_PORT,DHT11_PIN,value)

typedef struct
{
	uint8_t  humi_int;		//ʪ�ȵ���������
	uint8_t  humi_deci;	 	//ʪ�ȵ�С������
	uint8_t  temp_int;	 	//�¶ȵ���������
	uint8_t  temp_deci;	 	//�¶ȵ�С������
	uint8_t  check_sum;	 	//У���
}DHT11_Data_TypeDef;

void DHT11_GPIO_Config(void);

uint8_t Read_DHT11(DHT11_Data_TypeDef *DHT11_Data);


#endif //__DHT11_H_

