#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

#include "stm32f4xx.h"
#include <stdio.h>

#include "./fatfs/diskio.h"
#include "./fatfs/integer.h"


#undef  USB_FLASH_SPI3
#define USB_FLASH_SPI3   //ʹ��SPI3���ƶ�д�ⲿSPI_FLASHоƬ
#undef  USB_FLASH_SPI1


#define USB_SPI_TIMEOUT

//#define  sFLASH_ID                        0xEF3015     //W25X16
//#define  sFLASH_ID                        0xEF4015	   //W25Q16
//#define  sFLASH_ID                        0XEF4017     //W25Q64
#define  sFLASH_ID                          0XEF4018     //W25Q128

/* Private typedef -----------------------------------------------------------*/
//#define SPI_FLASH_PageSize                4096
#define SPI_FLASH_PageSize                  256
#define SPI_FLASH_PerWritePageSize          256

/* Private define ------------------------------------------------------------*/
#define W25X_WriteEnable		                0x06 
#define W25X_WriteDisable		                0x04 
#define W25X_ReadStatusReg		              0x05 
#define W25X_WriteStatusReg		            0x01 
#define W25X_ReadData			                  0x03 
#define W25X_FastReadData		                0x0B 
#define W25X_FastReadDual		                0x3B 
#define W25X_PageProgram		                0x02 
#define W25X_BlockErase			                0xD8 
#define W25X_SectorErase		                0x20 
#define W25X_ChipErase			                0xC7 
#define W25X_PowerDown			                0xB9 
#define W25X_ReleasePowerDown	            0xAB 
#define W25X_DeviceID			                  0xAB 
#define W25X_ManufactDeviceID   	          0x90 
#define W25X_JedecDeviceID		              0x9F 

#define WIP_Flag                            0x01  /* Write In Progress (WIP) flag */

#define Dummy_Byte                          0xFF



#ifdef USB_FLASH_SPI3
/*SPI�ӿ�*/
#define FLASH_SPI                           SPI3
#define FLASH_SPI_CLK                       RCC_APB1Periph_SPI3
#define FLASH_SPI_CLK_INIT                  RCC_APB1PeriphClockCmd

#define FLASH_SPI_SCK_PIN                   GPIO_Pin_3                  
#define FLASH_SPI_SCK_GPIO_PORT             GPIOB                       
#define FLASH_SPI_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOB
#define FLASH_SPI_SCK_PINSOURCE             GPIO_PinSource3
#define FLASH_SPI_SCK_AF                    GPIO_AF_SPI3

#define FLASH_SPI_MISO_PIN                  GPIO_Pin_4                
#define FLASH_SPI_MISO_GPIO_PORT            GPIOB                   
#define FLASH_SPI_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define FLASH_SPI_MISO_PINSOURCE            GPIO_PinSource4
#define FLASH_SPI_MISO_AF                   GPIO_AF_SPI3

#define FLASH_SPI_MOSI_PIN                  GPIO_Pin_5                
#define FLASH_SPI_MOSI_GPIO_PORT            GPIOB                      
#define FLASH_SPI_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define FLASH_SPI_MOSI_PINSOURCE            GPIO_PinSource5
#define FLASH_SPI_MOSI_AF                   GPIO_AF_SPI3

#endif  //end #ifdef USB_FLASH_SPI3 

#ifdef  USB_FLASH_SPI1
/*SPI�ӿ�*/
#define FLASH_SPI                           SPI1
#define FLASH_SPI_CLK                       RCC_APB2Periph_SPI1
#define FLASH_SPI_CLK_INIT                  RCC_APB2PeriphClockCmd

#define FLASH_SPI_SCK_PIN                   GPIO_Pin_3                 
#define FLASH_SPI_SCK_GPIO_PORT             GPIOB                       
#define FLASH_SPI_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOB
#define FLASH_SPI_SCK_PINSOURCE             GPIO_PinSource3
#define FLASH_SPI_SCK_AF                    GPIO_AF_SPI1

#define FLASH_SPI_MISO_PIN                  GPIO_Pin_4                 
#define FLASH_SPI_MISO_GPIO_PORT            GPIOB                     
#define FLASH_SPI_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define FLASH_SPI_MISO_PINSOURCE            GPIO_PinSource4
#define FLASH_SPI_MISO_AF                   GPIO_AF_SPI1

#define FLASH_SPI_MOSI_PIN                  GPIO_Pin_5                 
#define FLASH_SPI_MOSI_GPIO_PORT            GPIOB                     
#define FLASH_SPI_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define FLASH_SPI_MOSI_PINSOURCE            GPIO_PinSource5
#define FLASH_SPI_MOSI_AF                   GPIO_AF_SPI1

#endif  //end #ifdef USB_FLASH_SPI1 

#define FLASH_CS_PIN                        GPIO_Pin_8                 
#define FLASH_CS_GPIO_PORT                  GPIOI                      
#define FLASH_CS_GPIO_CLK                   RCC_AHB1Periph_GPIOI

#define SPI_FLASH_CS_LOW()                  {FLASH_CS_GPIO_PORT->BSRRH=FLASH_CS_PIN;}				//����͵�ƽ GPIO_ResetBits(FLASH_CS_GPIO_PORT, FLASH_CS_PIN)
#define SPI_FLASH_CS_HIGH()                 {FLASH_CS_GPIO_PORT->BSRRL=FLASH_CS_PIN;}			  //����Ϊ�ߵ�ƽ GPIO_SetBits(FLASH_CS_GPIO_PORT, FLASH_CS_PIN)


/*�ȴ���ʱʱ��*/
#ifdef  USB_SPI_TIMEOUT
#define SPIT_FLAG_TIMEOUT                   ((uint32_t)0x1000)
#define SPIT_LONG_TIMEOUT                   ((uint32_t)(10 * SPIT_FLAG_TIMEOUT))
#endif

/*��Ϣ���*/
#define FLASH_DEBUG_ON                      0
#define FLASH_DEBUG_FUNC_ON                 0

#define FLASH_INFO(fmt,arg...)              printf("<<-FLASH-INFO->> "fmt"\n",##arg)
#define FLASH_ERROR(fmt,arg...)             printf("<<-FLASH-ERROR->> "fmt"\n",##arg)
#define FLASH_DEBUG(fmt,arg...)             do{\
                                               if(FLASH_DEBUG_ON)\
                                                  printf("<<-FLASH-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                            }while(0)

#define FLASH_DEBUG_FUNC()                  do{\
                                               if(FLASH_DEBUG_FUNC_ON)\
                                                  printf("<<-FLASH-FUNC->> Func:%s@Line:%d\n",__func__,__LINE__);\
                                            }while(0)

/*SPI init����*/
#define SPI_FLASH_Init() 			TM_FATFS_FLASH_SPI_disk_initialize()
																						
DSTATUS TM_FATFS_FLASH_SPI_disk_initialize(void);
DSTATUS TM_FATFS_FLASH_SPI_disk_status(void) ;
DRESULT TM_FATFS_FLASH_SPI_disk_ioctl(BYTE cmd, char *buff) ;
DRESULT TM_FATFS_FLASH_SPI_disk_read(BYTE *buff, DWORD sector, UINT count) ;
DRESULT TM_FATFS_FLASH_SPI_disk_write(BYTE *buff, DWORD sector, UINT count) ;

void SPI_FLASH_SectorErase(u32 SectorAddr);
void SPI_FLASH_BulkErase(void);
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, uint32_t NumByteToRead);
u32 SPI_FLASH_ReadID(void);
u32 SPI_FLASH_ReadDeviceID(void);
void SPI_FLASH_StartReadSequence(u32 ReadAddr);
void SPI_Flash_PowerDown(void);
void SPI_Flash_WAKEUP(void);


u8 SPI_FLASH_ReadByte(void);
u8 SPI_FLASH_SendByte(u8 byte);
u16 SPI_FLASH_SendHalfWord(u16 HalfWord);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_WaitForWriteEnd(void);




#endif /* __SPI_FLASH_H */

