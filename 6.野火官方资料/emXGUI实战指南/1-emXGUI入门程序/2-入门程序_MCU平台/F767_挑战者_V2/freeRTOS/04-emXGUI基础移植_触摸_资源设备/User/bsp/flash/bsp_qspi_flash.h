#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

#include "stm32f7xx.h"
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
//#define  sFLASH_ID                       0xEF3015     //W25X16
//#define  sFLASH_ID                       0xEF4015	    //W25Q16
//#define  sFLASH_ID                       0XEF4017     //W25Q64
//#define  sFLASH_ID                       0XEF4018     //W25Q128
#define  sFLASH_ID                         0XEF4019     //W25Q256

/* QSPI Error codes */
#define QSPI_OK            ((uint8_t)0x00)
#define QSPI_ERROR         ((uint8_t)0x01)
#define QSPI_BUSY          ((uint8_t)0x02)
#define QSPI_NOT_SUPPORTED ((uint8_t)0x04)
#define QSPI_SUSPENDED     ((uint8_t)0x08)


/* W25Q256JV Micron memory */
/* Size of the flash */
#define QSPI_FLASH_SIZE            32     /* Address bus width to access whole memory space */
#define QSPI_PAGE_SIZE             256

/* QSPI Info */
typedef struct {
  uint32_t FlashSize;          /*!< Size of the flash */
  uint32_t EraseSectorSize;    /*!< Size of sectors for the erase operation */
  uint32_t EraseSectorsNumber; /*!< Number of sectors for the erase operation */
  uint32_t ProgPageSize;       /*!< Size of pages for the program operation */
  uint32_t ProgPagesNumber;    /*!< Number of pages for the program operation */
} QSPI_Info;

/* Private define ------------------------------------------------------------*/
/*�����-��ͷ*******************************/
/** 
  * @brief  W25Q256JV Configuration  
  */  
#define W25Q256JV_FLASH_SIZE                  0x2000000 /* 256 MBits => 32MBytes */
#define W25Q256JV_SECTOR_SIZE                 0x10000   /* 256 sectors of 64KBytes */
#define W25Q256JV_SUBSECTOR_SIZE              0x1000    /* 4096 subsectors of 4kBytes */
#define W25Q256JV_PAGE_SIZE                   0x100     /* 65536 pages of 256 bytes */

#define W25Q256JV_DUMMY_CYCLES_READ           4
#define W25Q256JV_DUMMY_CYCLES_READ_QUAD      10

#define W25Q256JV_BULK_ERASE_MAX_TIME         250000
#define W25Q256JV_SECTOR_ERASE_MAX_TIME       3000
#define W25Q256JV_SUBSECTOR_ERASE_MAX_TIME    800

/** 
  * @brief  W25Q256JV Commands  
  */  
/* Reset Operations */
#define RESET_ENABLE_CMD                     0x66
#define RESET_MEMORY_CMD                     0x99

#define ENTER_QPI_MODE_CMD                   0x38
#define EXIT_QPI_MODE_CMD                    0xFF

/* Identification Operations */
#define READ_ID_CMD                          0x90
#define DUAL_READ_ID_CMD                     0x92
#define QUAD_READ_ID_CMD                     0x94
#define READ_JEDEC_ID_CMD                    0x9F

/* Read Operations */
#define READ_CMD                             0x03
#define FAST_READ_CMD                        0x0B
#define DUAL_OUT_FAST_READ_CMD               0x3B
#define DUAL_INOUT_FAST_READ_CMD             0xBB
#define QUAD_OUT_FAST_READ_CMD               0x6B
#define QUAD_INOUT_FAST_READ_CMD             0xEB

/* Write Operations */
#define WRITE_ENABLE_CMD                     0x06
#define WRITE_DISABLE_CMD                    0x04

/* Register Operations */
#define READ_STATUS_REG1_CMD                  0x05
#define READ_STATUS_REG2_CMD                  0x35
#define READ_STATUS_REG3_CMD                  0x15

#define WRITE_STATUS_REG1_CMD                 0x01
#define WRITE_STATUS_REG2_CMD                 0x31
#define WRITE_STATUS_REG3_CMD                 0x11


/* Program Operations */
#define PAGE_PROG_CMD                        0x02
#define QUAD_INPUT_PAGE_PROG_CMD             0x32
#define EXT_QUAD_IN_FAST_PROG_CMD            0x12

/* Erase Operations */
#define SECTOR_ERASE_CMD                     0x20
#define CHIP_ERASE_CMD                       0xC7

#define PROG_ERASE_RESUME_CMD                0x7A
#define PROG_ERASE_SUSPEND_CMD               0x75

/* 4�ֽڵ�ַģʽ���� */
#define ENTER_4_BYTE_ADDR_MODE_CMD           0xB7
#define EXIT_4_BYTE_ADDR_MODE_CMD            0xE9

/* Flag Status Register */
#define W25Q256JV_FSR_BUSY                    ((uint8_t)0x01)    /*!< busy */
#define W25Q256JV_FSR_WREN                    ((uint8_t)0x02)    /*!< write enable */
#define W25Q256JV_FSR_QE                      ((uint8_t)0x02)    /*!< quad enable */
/*�����-��β*******************************/


/*QSPI�ӿڶ���-��ͷ****************************/
#define QSPI_FLASH                         QUADSPI
#define QSPI_FLASH_CLK_ENABLE()            __HAL_RCC_QSPI_CLK_ENABLE()

#define QSPI_FLASH_CLK_PIN                 GPIO_PIN_2                  
#define QSPI_FLASH_CLK_GPIO_PORT           GPIOB                       
#define QSPI_FLASH_CLK_GPIO_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define QSPI_FLASH_CLK_GPIO_AF             GPIO_AF9_QUADSPI

#define QSPI_FLASH_BK1_IO0_PIN             GPIO_PIN_8                
#define QSPI_FLASH_BK1_IO0_PORT            GPIOF                   
#define QSPI_FLASH_BK1_IO0_CLK_ENABLE()    __HAL_RCC_GPIOF_CLK_ENABLE()
#define QSPI_FLASH_BK1_IO0_AF              GPIO_AF10_QUADSPI

#define QSPI_FLASH_BK1_IO1_PIN             GPIO_PIN_9                
#define QSPI_FLASH_BK1_IO1_PORT            GPIOF                      
#define QSPI_FLASH_BK1_IO1_CLK_ENABLE()    __HAL_RCC_GPIOF_CLK_ENABLE()
#define QSPI_FLASH_BK1_IO1_AF              GPIO_AF10_QUADSPI

#define QSPI_FLASH_BK1_IO2_PIN             GPIO_PIN_7                
#define QSPI_FLASH_BK1_IO2_PORT            GPIOF                      
#define QSPI_FLASH_BK1_IO2_CLK_ENABLE()    __HAL_RCC_GPIOF_CLK_ENABLE()
#define QSPI_FLASH_BK1_IO2_AF              GPIO_AF9_QUADSPI

#define QSPI_FLASH_BK1_IO3_PIN             GPIO_PIN_6                
#define QSPI_FLASH_BK1_IO3_PORT            GPIOF                      
#define QSPI_FLASH_BK1_IO3_CLK_ENABLE()    __HAL_RCC_GPIOF_CLK_ENABLE()
#define QSPI_FLASH_BK1_IO3_AF              GPIO_AF9_QUADSPI

#define QSPI_FLASH_CS_PIN                 GPIO_PIN_10               
#define QSPI_FLASH_CS_GPIO_PORT           GPIOB                   
#define QSPI_FLASH_CS_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()
#define QSPI_FLASH_CS_GPIO_AF             GPIO_AF9_QUADSPI



uint8_t QSPI_FLASH_Init(void);
uint8_t BSP_QSPI_Init(void);
uint8_t BSP_QSPI_Erase_Block(uint32_t BlockAddress);
uint8_t BSP_QSPI_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size);
uint8_t BSP_QSPI_FastRead(uint8_t* pData, uint32_t ReadAddr, uint32_t Size);
uint8_t BSP_QSPI_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size);
static uint8_t BSP_QSPI_4BYTE_ADDR_MOD(void);
uint32_t QSPI_FLASH_ReadStatusReg(uint8_t reg);

static uint8_t QSPI_ResetMemory          (void);
static uint8_t QSPI_WriteEnable          (void);
static uint8_t QSPI_AutoPollingMemReady  (uint32_t Timeout);

uint32_t QSPI_FLASH_ReadDeviceID(void);
uint32_t QSPI_FLASH_ReadID(void);

uint8_t BSP_QSPI_Erase_Chip(void);

#endif /* __SPI_FLASH_H */

