#ifndef __DHT11_H_
#define __DHT11_H_

#include  "stm32f4xx.h"

#define DHT11_HIGH  1
#define DHT11_LOW   0


/*---------------------------------------*/
#define DHT11_CLK     RCC_AHB1Periph_GPIOE
#define DHT11_PIN     GPIO_Pin_2               
#define DHT11_PORT    GPIOE


#define DHT11_DATA_OUT(a)	if (a)	\
                                   GPIO_SetBits(DHT11_PORT,DHT11_PIN);\
                                   else		\
                                   GPIO_ResetBits(DHT11_PORT,DHT11_PIN)

#define  DHT11_DATA_IN()	  GPIO_ReadInputDataBit(DHT11_PORT,DHT11_PIN)

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
