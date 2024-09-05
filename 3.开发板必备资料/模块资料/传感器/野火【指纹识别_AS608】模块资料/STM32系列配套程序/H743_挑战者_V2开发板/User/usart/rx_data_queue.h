#ifndef __ESP_DATA_QUEUE_H_
#define __ESP_DATA_QUEUE_H_

#include "stm32h7xx.h"

#include <string.h>
#include <stdio.h>


#define  RINGBUFF_LEN     500//定义最大接收字节数 200
#define  FALSE    0
#define  TRUE     1 



typedef struct
 {
   uint16_t Head;
	 uint16_t Tail;
   uint16_t Lenght;
	 uint8_t  Ring_Buff[RINGBUFF_LEN];
 }RingBuff_t;
extern RingBuff_t ringBuff;//创建一个ringBuff的缓冲区



void RingBuff_Init(void);
uint8_t Write_RingBuff(uint8_t data);
uint8_t Read_RingBuff(uint8_t *rData);



void rx_queue_init(void);
void pull_data_from_queue(void);
void push_data_to_queue(char *src_dat,uint16_t src_len);


#endif



