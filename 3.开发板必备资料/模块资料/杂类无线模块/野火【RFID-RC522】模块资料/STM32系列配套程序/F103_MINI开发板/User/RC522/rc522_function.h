#ifndef __RC522_FUNCTION_H
#define	__RC522_FUNCTION_H


#include "stm32f10x_it.h"


#define          macDummy_Data              0x00


void             PcdReset                   ( void );                       //��λ
void             M500PcdConfigISOType       ( u8 type );                    //������ʽ
char             PcdRequest                 ( u8 req_code, u8 * pTagType ); //Ѱ��
char             PcdAnticoll                ( u8 * pSnr);                   //������
char             PcdHalt                    ( void );
char             PcdSelect                  ( uint8_t * pSnr );
char             PcdAuthState               ( uint8_t ucAuth_mode, uint8_t ucAddr, uint8_t * pKey, uint8_t * pSnr );
char             WriteAmount                ( uint8_t ucAddr, uint32_t pData );
char             ReadAmount                 ( uint8_t ucAddr, uint32_t *pData );


char IsDataBlock( uint8_t ucAddr );
char PcdWriteString( uint8_t ucAddr, uint8_t *pData);
char PcdReadString( uint8_t ucAddr, uint8_t *pData);

char ChangeKeyA( uint8_t ucAddr, uint8_t *pKeyA );
char WriteDataBlock( uint8_t ucAddr, uint8_t *pData, uint8_t Len);
char ReadDataBlock( uint8_t ucAddr, uint8_t *pData);


#endif /* __RC522_FUNCTION_H */

