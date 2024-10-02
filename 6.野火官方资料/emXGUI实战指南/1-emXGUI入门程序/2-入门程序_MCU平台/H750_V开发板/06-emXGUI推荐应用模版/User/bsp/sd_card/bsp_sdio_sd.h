#ifndef __STM32H743_SDIO_SD_H
#define __STM32H743_SDIO_SD_H

#include "stm32h7xx.h"
//�ȴ�ʱ��
#define SD_TIMEOUT             ((uint32_t)0x00100000U)
//��Ĵ�С�����ֽ�Ϊ��λ��
#define BLOCK_SIZE            512 
//�����Ŀ
#define NUMBER_OF_BLOCKS      10
//������ݴ�С
#define MULTI_BUFFER_SIZE    (BLOCK_SIZE * NUMBER_OF_BLOCKS)


static void BSP_SD_MspInit(void);
HAL_StatusTypeDef BSP_SD_Init(void);
static void SD_EraseTest(void);
static HAL_StatusTypeDef Wait_SDCARD_Ready(void);
static HAL_StatusTypeDef Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength);


void SD_Test(void);
void SD_SingleBlockTest(void);
void SD_MultiBlockTest(void);
void Fill_Buffer(uint8_t *pBuffer, uint32_t BufferLength, uint32_t Offset);
#endif 
/************************END OF FILE*******************/
