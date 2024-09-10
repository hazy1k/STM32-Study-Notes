#ifndef __RC522_FUNCTION_H
#define	__RC522_FUNCTION_H


#include "stm32h7xx.h"


#define          macDummy_Data              0x00


void             PcdReset                   ( void );                       //复位
void             M500PcdConfigISOType       ( uint8_t type );                    //工作方式
char             PcdRequest                 ( uint8_t req_code, uint8_t * pTagType ); //寻卡
char             PcdAnticoll                ( uint8_t * pSnr);                   //读卡号


#endif /* __RC522_FUNCTION_H */

