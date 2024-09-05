#ifndef __AS608_TEST_H
#define	__AS608_TEST_H


#include "./as608/bsp_as608.h"

extern uint8_t receive_data[20];
void Connect_Test(void);
void AS608_USART_Config(void);
uint16_t PS_Connect(uint32_t *PS_Addr);
uint16_t PS_GetImage(void);
void Add_FR(void);
void Compare_FR(void);
void Del_FR(void);
void Clean_FR(void);

void Show_Message(void);
void FR_Task(void);
  
#endif /* __AS608_TEST_H */
