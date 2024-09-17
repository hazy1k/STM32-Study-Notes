/**
  ******************************************************************************
  * @file    sd_diskio.c
  * @author  MCD Application Team
  * @version V1.4.1
  * @date    14-February-2017
  * @brief   SD Disk I/O driver
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "ff_gen_drv.h"

#include "./flash/bsp_spi_flash.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
static __IO DSTATUS Stat = STA_NOINIT;
/* Private variables ---------------------------------------------------------*/
/* Disk status */
#define SPI_FLASH_REBUILD        0       // 1:ʹ��ÿ�����¸�ʽ������Flash��0������ÿ�����¸�ʽ������Flash
#define SPI_FLASH_SECTOR_SIZE    4096   // ����Flash������С
#define SPI_FLASH_START_SECTOR   514    // ����Flash�ļ�ϵͳFatFSƫ����
#define SPI_FLASH_SECTOR_COUNT   1534   // ����Flash�ļ�ϵͳFatFSռ����������    

/* Private function prototypes -----------------------------------------------*/
DSTATUS SPIFLASH_initialize(BYTE);
DSTATUS SPIFLASH_status(BYTE);
DRESULT SPIFLASH_read(BYTE, BYTE*, DWORD, UINT);
#if _USE_WRITE == 1   // �������д����
  DRESULT SPIFLASH_write (BYTE, const BYTE*, DWORD, UINT);
#endif /* _USE_WRITE == 1 */

#if _USE_IOCTL == 1   // ������������������
  DRESULT SPIFLASH_ioctl (BYTE, BYTE, void*);
#endif  /* _USE_IOCTL == 1 */

/* ���崮��FLASH�ӿں��� */
const Diskio_drvTypeDef  SPIFLASH_Driver =
{
  SPIFLASH_initialize,                    
  SPIFLASH_status,                       
  SPIFLASH_read,                         
#if  _USE_WRITE == 1
  SPIFLASH_write,                        
#endif /* _USE_WRITE == 1 */
  
#if  _USE_IOCTL == 1
  SPIFLASH_ioctl,                        
#endif /* _USE_IOCTL == 1 */
};

/* ������ --------------------------------------------------------------------*/
/**
  * @brief  flash��ʼ��
  * @param  ��
  * @retval ��
  */
DSTATUS SPIFLASH_initialize(BYTE lun)
{
#if SPI_FLASH_REBUILD == 1
  static uint8_t startflag=1;
#endif
  Stat = STA_NOINIT;
  /* ��ʼ��SPIFLASHIO���� */
  SPI_FLASH_Init();
  SPI_Flash_WAKEUP();
  /* ��ȡ����FLASH״̬ */
  if(SPI_FLASH_ReadID()==sFLASH_ID)
  {
#if SPI_FLASH_REBUILD == 1
      if(startflag)
      {
        SPI_FLASH_SectorErase(SPI_FLASH_START_SECTOR*SPI_FLASH_SECTOR_SIZE);
			  startflag=0;
      }
#endif
    Stat &=~STA_NOINIT;
  }
  return Stat;
}

/**
  * @brief  FLASH״̬��ȡ
  * @param  ��
  * @retval ��
  */
DSTATUS SPIFLASH_status(BYTE lun)
{
  Stat = STA_NOINIT;

  if(SPI_FLASH_ReadID()==sFLASH_ID)
  {
    Stat &= ~STA_NOINIT;
  }
  return Stat;
}

/**
  * @brief  �Ӵ���FLASH��ȡ���ݵ�������
  * @param   lun : ����
             buff����Ŷ�ȡ�����ݻ�����ָ��
             sector��������ַ(LBA)
             count��������Ŀ
  * @retval DSTATUS���������
  */
DRESULT SPIFLASH_read(BYTE lun, BYTE *buff, DWORD sector, UINT count)
{ 
  sector+=SPI_FLASH_START_SECTOR;      
  SPI_FLASH_BufferRead(buff, sector*SPI_FLASH_SECTOR_SIZE, count*SPI_FLASH_SECTOR_SIZE);
  return RES_OK;
}

/**
  * @brief  ������������д�뵽����FLASH��
  * @param   lun : ����
             buff����Ŷ�ȡ�����ݻ�����ָ��
             sector��������ַ(LBA)
             count��������Ŀ
  * @retval DSTATUS���������
  */
#if _USE_WRITE == 1
DRESULT SPIFLASH_write(BYTE lun, const BYTE *buff, DWORD sector, UINT count)
{
  uint32_t write_addr; 
  /* ����ƫ��7MB���ⲿFlash�ļ�ϵͳ�ռ����SPI Flash����9MB�ռ� */
  sector+=SPI_FLASH_START_SECTOR;
  write_addr = sector*SPI_FLASH_SECTOR_SIZE;    
  SPI_FLASH_SectorErase(write_addr);
  SPI_FLASH_BufferWrite((uint8_t *)buff,write_addr,count*SPI_FLASH_SECTOR_SIZE);
  return RES_OK;
}
#endif /* _USE_WRITE == 1 */


/**
  * @brief  ����������Ʋ���
  * @param   lun : ����
             cmd����������
             buff����Ŷ�ȡ�����ݻ�����ָ��
  * @retval DSTATUS���������
  */
#if _USE_IOCTL == 1
DRESULT SPIFLASH_ioctl(BYTE lun, BYTE cmd, void *buff)
{
  DRESULT res = RES_ERROR;
  
  if (Stat & STA_NOINIT) return RES_NOTRDY;
  
  switch (cmd)
  {
  /* Make sure that no pending write process */
  case CTRL_SYNC :
    res = RES_OK;
    break;
  
  /* ��ȡ����FLASH��������Ŀ(DWORD) */
  case GET_SECTOR_COUNT :
    *(DWORD * )buff = SPI_FLASH_SECTOR_COUNT;	
    res = RES_OK;
    break;
  
  /* ��ȡ��д������С(WORD) */
  case GET_SECTOR_SIZE :
    *(WORD * )buff = SPI_FLASH_SECTOR_SIZE;
    res = RES_OK;
    break;
  
  /* ��ȡ�������С(DWORD) */
  case GET_BLOCK_SIZE :
    *(DWORD * )buff = 1;
    res = RES_OK;
    break;
  
  default:
    res = RES_PARERR;
  }
  
  return res;
}
#endif /* _USE_IOCTL == 1 */
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

