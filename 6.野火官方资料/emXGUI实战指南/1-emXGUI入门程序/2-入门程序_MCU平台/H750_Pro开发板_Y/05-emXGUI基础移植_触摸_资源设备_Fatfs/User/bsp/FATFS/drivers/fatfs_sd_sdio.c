/**
  ******************************************************************************
  * @file    bsp_sdio_sd.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   SDIO sd�����������������ļ�ϵͳ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����  STM32 H743 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
#include "./drivers/fatfs_sd_sdio.h"
#include <stdio.h>
#include <string.h>
#include "./sd_card/bsp_sdio_sd.h"
#include "ff_gen_drv.h"
#include "./led/bsp_led.h" 
#include "emXGUI.h"
/* Disk status */
static volatile DSTATUS Stat = STA_NOINIT;
static GUI_SEM *sem_sd = NULL;
static GUI_MUTEX *mutex_lock=NULL;
extern SD_HandleTypeDef uSdHandle;
//���ͱ�־λ
extern volatile uint8_t TX_Flag;
//���ܱ�־λ
extern volatile uint8_t RX_Flag; 
const Diskio_drvTypeDef  SD_Driver =
{
  SD_initialize,
  SD_status,
  SD_read, 
#if  _USE_WRITE == 1
  SD_write,
#endif /* _USE_WRITE == 1 */
  
#if  _USE_IOCTL == 1
  SD_ioctl,
#endif /* _USE_IOCTL == 1 */
};
DSTATUS SD_initialize(BYTE lun)
{
    Stat = STA_NOINIT;
  
    if(BSP_SD_Init() == HAL_OK)
    {    
        sem_sd = GUI_SemCreate(0,1);//ͬ������ͷͼ��
        mutex_lock = GUI_MutexCreate();
        Stat &= ~STA_NOINIT;
    }
    return Stat;
}

DSTATUS SD_status(BYTE lun){

    Stat = STA_NOINIT;
    if(HAL_SD_GetCardState(&uSdHandle) == HAL_SD_CARD_TRANSFER)
    {
        Stat &= ~STA_NOINIT;
    }
    return Stat;
}


DRESULT SD_ReadBlocks(BYTE *buff,//���ݻ����� 
                      DWORD sector, //�����׵�ַ
                      UINT count)//��������(1..128)
{
  DRESULT res = RES_ERROR;  
  uint32_t timeout;
  uint32_t alignedAddr;
  RX_Flag = 0;
  taskENTER_CRITICAL();
  if(HAL_SD_ReadBlocks_DMA(&uSdHandle, (uint8_t*)buff,
                           (uint32_t) (sector),
                           count) == HAL_OK)
  {
    taskEXIT_CRITICAL();
    /* Wait that the reading process is completed or a timeout occurs */
//    timeout = HAL_GetTick();
//    while((RX_Flag == 0) && ((HAL_GetTick() - timeout) < SD_TIMEOUT))
//    {
//    }
//    /* incase of a timeout return error */
//    if (RX_Flag == 0)
//    {
//      res = RES_ERROR;
//    }
//    else
    GUI_SemWait(sem_sd,2000);
    {
//      RX_Flag = 0;
      //timeout = HAL_GetTick();

      //while((HAL_GetTick() - timeout) < SD_TIMEOUT)
      {
        //if (HAL_SD_GetCardState(&uSdHandle) == HAL_SD_CARD_TRANSFER)
        {
          res = RES_OK;

          /*
             the SCB_InvalidateDCache_by_Addr() requires a 32-Byte aligned address,
             adjust the address and the D-Cache size to invalidate accordingly.
           */
          alignedAddr = (uint32_t)buff & ~0x1F;
          //ʹ��Ӧ��DCache��Ч
          SCB_InvalidateDCache_by_Addr((uint32_t*)alignedAddr, count*BLOCKSIZE + ((uint32_t)buff - alignedAddr));

//           break;
        }
      }
    }
  }

  return res;  
}



DRESULT SD_read(BYTE lun,//��������������豸ʱ�õ�(0...)
                BYTE *buff,//���ݻ����� 
                DWORD sector, //�����׵�ַ
                UINT count)//��������(1..128)
{
  DRESULT res = RES_ERROR;
  uint32_t i;
  DWORD pbuff[512/4];	
  
	if((DWORD)buff&3)
    GUI_DEBUG("������");
	{
    GUI_MutexLock(mutex_lock,0xffffff);
	 	for(i=0;i<count;i++)
		{
      //GUI_DEBUG("1");
		 	res = SD_ReadBlocks((BYTE *)pbuff,sector+i,1);//����sector�Ķ�����
      taskENTER_CRITICAL();
			memcpy(buff,pbuff,512);
      taskEXIT_CRITICAL();
			buff+=512;
		} 
	}
//  else 
//  {
////    GUI_msleep(2);
//    res = SD_ReadBlock(buff,sector,count);	//����/���sector     

//  }
  GUI_MutexUnlock(mutex_lock);
  return RES_OK;
}
  
DRESULT SD_write(BYTE lun,//��������������豸ʱ�õ�(0...)
                 const BYTE *buff,//���ݻ����� 
                 DWORD sector, //�����׵�ַ
                 UINT count)//��������(1..128)
{
    DRESULT res = RES_ERROR;
    uint32_t timeout;

  
    TX_Flag = 0;
//    alignedAddr = (uint32_t)buff & ~0x1F;
//    //������Ӧ��DCache
//    SCB_CleanDCache_by_Addr((uint32_t*)alignedAddr, count*BLOCKSIZE + ((uint32_t)buff - alignedAddr));
    if(HAL_SD_WriteBlocks_DMA(&uSdHandle, (uint8_t*)buff,
                             (uint32_t) (sector),
                             count) == HAL_OK)
    {
      /* Wait that the reading process is completed or a timeout occurs */
      timeout = HAL_GetTick();
      while((TX_Flag == 0) && ((HAL_GetTick() - timeout) < SD_TIMEOUT))
      {
      }
      /* incase of a timeout return error */
      if (TX_Flag == 0)
      {
        res = RES_ERROR;
      }
      else
      {
        TX_Flag = 0;
        timeout = HAL_GetTick();

        while((HAL_GetTick() - timeout) < SD_TIMEOUT)
        {
          if (HAL_SD_GetCardState(&uSdHandle) == HAL_SD_CARD_TRANSFER)
          {
            res = RES_OK;
            //ʹ��Ӧ��DCache��Ч
//            SCB_InvalidateDCache_by_Addr((uint32_t*)alignedAddr, count*BLOCKSIZE + ((uint32_t)buff - alignedAddr));

             break;
          }
        }
      }
    }
    return res;
}



DRESULT SD_ioctl(BYTE lun,BYTE cmd, void *buff){
    DRESULT res = RES_ERROR;
    HAL_SD_CardInfoTypeDef CardInfo;

    if (Stat & STA_NOINIT) return RES_NOTRDY;

    switch (cmd)
    {
    /* Make sure that no pending write process */
    case CTRL_SYNC :
      res = RES_OK;
      break;

    /* Get number of sectors on the disk (DWORD) */
    case GET_SECTOR_COUNT :
      HAL_SD_GetCardInfo(&uSdHandle, &CardInfo);
      *(DWORD*)buff = CardInfo.LogBlockNbr;
      res = RES_OK;
      break;

    /* Get R/W sector size (WORD) */
    case GET_SECTOR_SIZE :
      HAL_SD_GetCardInfo(&uSdHandle, &CardInfo);
      *(WORD*)buff = CardInfo.LogBlockSize;
      res = RES_OK;
      break;

    /* Get erase block size in unit of sector (DWORD) */
    case GET_BLOCK_SIZE :
      HAL_SD_GetCardInfo(&uSdHandle, &CardInfo);
      *(DWORD*)buff = CardInfo.LogBlockSize / BLOCK_SIZE;
      res = RES_OK;
      break;

    default:
      res = RES_PARERR;
    }
    return RES_OK;
}

//SDMMC1������ɻص�����
void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd)
{
  //GUI_SemPostISR(sem_sd);
  TX_Flag=1; //���д���
}

//SDMMC1������ɻص�����
void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd)
{
  GUI_SemPostISR(sem_sd);
  RX_Flag=1;
}
/*****************************END OF FILE****************************/

