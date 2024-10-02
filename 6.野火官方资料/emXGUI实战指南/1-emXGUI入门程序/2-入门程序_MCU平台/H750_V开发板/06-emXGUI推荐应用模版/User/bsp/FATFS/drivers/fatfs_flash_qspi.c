 /**
  ******************************************************************************
  * @file    bsp_xxx.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   QSPI flash �ײ�Ӧ�ú���bsp 
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��STM32H743 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./fatfs/drivers/fatfs_flash_qspi.h"
#include "ff_gen_drv.h"

static volatile DSTATUS TM_FATFS_FLASH_SPI_Stat = STA_NOINIT;	/* Physical drive status */


QSPI_HandleTypeDef hqspi;

#define QSPIHandle hqspi

const Diskio_drvTypeDef  QSPI_Driver =
{
  TM_FATFS_FLASH_SPI_disk_initialize,
  TM_FATFS_FLASH_SPI_disk_status,
  TM_FATFS_FLASH_SPI_disk_read, 
#if  _USE_WRITE == 1
  TM_FATFS_FLASH_SPI_disk_write,
#endif /* _USE_WRITE == 1 */
  
#if  _USE_IOCTL == 1
  TM_FATFS_FLASH_SPI_disk_ioctl,
#endif /* _USE_IOCTL == 1 */
};

/*******************************************************************************
* Function Name  : SPI_FLASH_Init
* Description    : Initializes the peripherals used by the SPI FLASH driver.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
DSTATUS TM_FATFS_FLASH_SPI_disk_initialize(BYTE lun)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
	/* ʹ�� QSPI �� GPIO ʱ�� */
	QSPI_FLASH_CLK_ENABLE();
	QSPI_FLASH_CLK_GPIO_ENABLE();
	QSPI_FLASH_BK1_IO0_CLK_ENABLE();
	QSPI_FLASH_BK1_IO1_CLK_ENABLE();
	QSPI_FLASH_BK1_IO2_CLK_ENABLE();
	QSPI_FLASH_BK1_IO3_CLK_ENABLE();
	QSPI_FLASH_CS_GPIO_CLK_ENABLE();

	//��������
	/*!< ���� QSPI_FLASH ����: CLK */
	GPIO_InitStruct.Pin = QSPI_FLASH_CLK_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = QSPI_FLASH_CLK_GPIO_AF;

	HAL_GPIO_Init(QSPI_FLASH_CLK_GPIO_PORT, &GPIO_InitStruct);

	/*!< ���� QSPI_FLASH ����: IO0 */
	GPIO_InitStruct.Pin = QSPI_FLASH_BK1_IO0_PIN;
	GPIO_InitStruct.Alternate = QSPI_FLASH_BK1_IO0_AF;
	HAL_GPIO_Init(QSPI_FLASH_BK1_IO0_PORT, &GPIO_InitStruct);

	/*!< ���� QSPI_FLASH ����: IO1 */
	GPIO_InitStruct.Pin = QSPI_FLASH_BK1_IO1_PIN;
	GPIO_InitStruct.Alternate = QSPI_FLASH_BK1_IO1_AF;
	HAL_GPIO_Init(QSPI_FLASH_BK1_IO1_PORT, &GPIO_InitStruct);

	/*!< ���� QSPI_FLASH ����: IO2 */
	GPIO_InitStruct.Pin = QSPI_FLASH_BK1_IO2_PIN;
	GPIO_InitStruct.Alternate = QSPI_FLASH_BK1_IO2_AF;
	HAL_GPIO_Init(QSPI_FLASH_BK1_IO2_PORT, &GPIO_InitStruct);

	/*!< ���� QSPI_FLASH ����: IO3 */
	GPIO_InitStruct.Pin = QSPI_FLASH_BK1_IO3_PIN;
	GPIO_InitStruct.Alternate = QSPI_FLASH_BK1_IO3_AF;
	HAL_GPIO_Init(QSPI_FLASH_BK1_IO3_PORT, &GPIO_InitStruct);

	/*!< ���� SPI_FLASH_SPI ����: NCS */
	GPIO_InitStruct.Pin = QSPI_FLASH_CS_PIN;
	GPIO_InitStruct.Alternate = QSPI_FLASH_CS_GPIO_AF;
	HAL_GPIO_Init(QSPI_FLASH_CS_GPIO_PORT, &GPIO_InitStruct);

	/* QSPI_FLASH ģʽ���� */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_QSPI;
  PeriphClkInitStruct.PLL2.PLL2M = 5;
  PeriphClkInitStruct.PLL2.PLL2N = 120;
  PeriphClkInitStruct.PLL2.PLL2P = 2;
  PeriphClkInitStruct.PLL2.PLL2Q = 2;
  PeriphClkInitStruct.PLL2.PLL2R = 3;
  PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_2;
  PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
  PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
  PeriphClkInitStruct.QspiClockSelection = RCC_QSPICLKSOURCE_PLL2;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
  
  QSPIHandle.Instance=QUADSPI;                          //QSPI
  QSPIHandle.Init.ClockPrescaler=1;                     //QPSI��Ƶ�ȣ�W25Q256���Ƶ��Ϊ104M��
                                                          //���Դ˴�Ӧ��Ϊ2��QSPIƵ�ʾ�Ϊ200/(1+1)=100MHZ
  QSPIHandle.Init.FifoThreshold=4;                      //FIFO��ֵΪ4���ֽ�
  QSPIHandle.Init.SampleShifting=QSPI_SAMPLE_SHIFTING_HALFCYCLE;//������λ�������(DDRģʽ��,��������Ϊ0)
  QSPIHandle.Init.FlashSize=POSITION_VAL(0X2000000)-1;  //SPI FLASH��С��W25Q256��СΪ32M�ֽ�
  QSPIHandle.Init.ChipSelectHighTime=QSPI_CS_HIGH_TIME_5_CYCLE;//Ƭѡ�ߵ�ƽʱ��Ϊ5��ʱ��(10*5=55ns),���ֲ������tSHSL����
  QSPIHandle.Init.ClockMode=QSPI_CLOCK_MODE_0;          //ģʽ0
  QSPIHandle.Init.FlashID=QSPI_FLASH_ID_1;              //��һƬflash
  QSPIHandle.Init.DualFlash=QSPI_DUALFLASH_DISABLE;     //��ֹ˫����ģʽ
  HAL_QSPI_Init(&QSPIHandle);

	return TM_FATFS_FLASH_SPI_disk_status(NULL);

}
/**
  * @brief  Initializes the QSPI interface.
  * @retval QSPI memory status
  */
uint8_t BSP_QSPI_Init(void)
{ 
	QSPI_CommandTypeDef s_command;
	uint8_t value = W25Q128FV_FSR_QE;
	
  /* QSPI memory reset */
  if (QSPI_ResetMemory() != QSPI_OK)
  {
    return QSPI_NOT_SUPPORTED;
  }
	
	/* Enable write operations */
	if (QSPI_WriteEnable() != QSPI_OK)
	{
		return QSPI_ERROR;
	}
	
	/* Set status register for Quad Enable,the Quad IO2 and IO3 pins are enable */
	s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
  s_command.Instruction       = WRITE_STATUS_REG2_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_4_LINES;
  s_command.DummyCycles       = 0;
  s_command.NbData            = 1;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
	/* Configure the command */
  if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }
	/* Transmit the data */
	if (HAL_QSPI_Transmit(&hqspi, &value, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }
  
  /* automatic polling mode to wait for memory ready */  
  if (QSPI_AutoPollingMemReady(W25Q128FV_SUBSECTOR_ERASE_MAX_TIME) != QSPI_OK)
  {
    return QSPI_ERROR;
  }
	
  return QSPI_OK;
}

/**
  * @brief  Reads an amount of data from the QSPI memory.
  * @param  pData: Pointer to data to be read
  * @param  ReadAddr: Read start address
  * @param  Size: Size of data to read    
  * @retval QSPI memory status
  */
uint8_t BSP_QSPI_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size)
{
  QSPI_CommandTypeDef s_command;
	/* Initialize the read command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
  s_command.Instruction       = READ_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_4_LINES;
  s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
  s_command.Address           = ReadAddr;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_4_LINES;
  s_command.DummyCycles       = 8;
  s_command.NbData            = Size;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
  
  /* Configure the command */
  if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }
  
  /* Reception of the data */
  if (HAL_QSPI_Receive(&QSPIHandle, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }
  return QSPI_OK;
}

/**
  * @brief  Writes an amount of data to the QSPI memory.
  * @param  pData: Pointer to data to be written
  * @param  WriteAddr: Write start address
  * @param  Size: Size of data to write    
  * @retval QSPI memory status
  */
uint8_t BSP_QSPI_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size)
{
  QSPI_CommandTypeDef s_command;
  uint32_t end_addr, current_size, current_addr;
  /* Calculation of the size between the write address and the end of the page */
  current_addr = 0;

  while (current_addr <= WriteAddr)
  {
    current_addr += W25Q128FV_PAGE_SIZE;
  }
  current_size = current_addr - WriteAddr;

  /* Check if the size of the data is less than the remaining place in the page */
  if (current_size > Size)
  {
    current_size = Size;
  }

  /* Initialize the adress variables */
  current_addr = WriteAddr;
  end_addr = WriteAddr + Size;

  /* Initialize the program command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
  s_command.Instruction       = QUAD_INPUT_PAGE_PROG_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_4_LINES;
  s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_4_LINES;
  s_command.DummyCycles       = 0;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
  
  /* Perform the write page by page */
  do
  {
    s_command.Address = current_addr;
    s_command.NbData  = current_size;

    /* Enable write operations */
    if (QSPI_WriteEnable() != QSPI_OK)
    {
      return QSPI_ERROR;
    }
    
    /* Configure the command */
    if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      return QSPI_ERROR;
    }
    
    /* Transmission of the data */
    if (HAL_QSPI_Transmit(&QSPIHandle, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      return QSPI_ERROR;
    }
    
    /* Configure automatic polling mode to wait for end of program */  
    if (QSPI_AutoPollingMemReady(HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
    {
      return QSPI_ERROR;
    }
    
    /* Update the address and size variables for next page programming */
    current_addr += current_size;
    pData += current_size;
    current_size = ((current_addr + W25Q128FV_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : W25Q128FV_PAGE_SIZE;
  } while (current_addr < end_addr);
  return QSPI_OK;
}

/**
  * @brief  Erases the specified block of the QSPI memory. 
  * @param  BlockAddress: Block address to erase  
  * @retval QSPI memory status
  */
uint8_t BSP_QSPI_Erase_Block(uint32_t BlockAddress)
{
  QSPI_CommandTypeDef s_command;
  /* Initialize the erase command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = SECTOR_ERASE_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
  s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
  s_command.Address           = BlockAddress;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DummyCycles       = 0;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Enable write operations */
  if (QSPI_WriteEnable() != QSPI_OK)
  {
    printf("WE Err\n");
		return QSPI_ERROR;
  }

  /* Send the command */
  if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
		return QSPI_ERROR;
  }
  
  /* Configure automatic polling mode to wait for end of erase */  
  if (QSPI_AutoPollingMemReady(W25Q128FV_SUBSECTOR_ERASE_MAX_TIME) != QSPI_OK)
  {
    return QSPI_ERROR;
  }
  return QSPI_OK;
}

/**
  * @brief  Erases the entire QSPI memory.
  * @retval QSPI memory status
  */
uint8_t BSP_QSPI_Erase_Chip(void)
{
  QSPI_CommandTypeDef s_command;
  /* Initialize the erase command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = CHIP_ERASE_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DummyCycles       = 0;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Enable write operations */
  if (QSPI_WriteEnable() != QSPI_OK)
  {
    return QSPI_ERROR;
  }

  /* Send the command */
  if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }
  
  /* Configure automatic polling mode to wait for end of erase */  
  if (QSPI_AutoPollingMemReady(W25Q128FV_BULK_ERASE_MAX_TIME) != QSPI_OK)
  {
    return QSPI_ERROR;
  }
  return QSPI_OK;
}

/**
  * @brief  Reads current status of the QSPI memory.
  * @retval QSPI memory status
  */
uint8_t BSP_QSPI_GetStatus(void)
{
  QSPI_CommandTypeDef s_command;
  uint8_t reg;
  /* Initialize the read flag status register command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = READ_STATUS_REG1_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_1_LINE;
  s_command.DummyCycles       = 0;
  s_command.NbData            = 1;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* Reception of the data */
  if (HAL_QSPI_Receive(&QSPIHandle, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }
  
  /* Check the value of the register */
  if((reg & W25Q128FV_FSR_BUSY) != 0)
  {
    return QSPI_BUSY;
  }
	else
	{
		return QSPI_OK;
	}
}

/**
  * @brief  Return the configuration of the QSPI memory.
  * @param  pInfo: pointer on the configuration structure  
  * @retval QSPI memory status
  */
uint8_t BSP_QSPI_GetInfo(QSPI_Info* pInfo)
{
  /* Configure the structure with the memory configuration */
  pInfo->FlashSize          = W25Q128FV_FLASH_SIZE;
  pInfo->EraseSectorSize    = W25Q128FV_SUBSECTOR_SIZE;
  pInfo->EraseSectorsNumber = (W25Q128FV_FLASH_SIZE/W25Q128FV_SUBSECTOR_SIZE);
  pInfo->ProgPageSize       = W25Q128FV_PAGE_SIZE;
  pInfo->ProgPagesNumber    = (W25Q128FV_FLASH_SIZE/W25Q128FV_PAGE_SIZE);
  return QSPI_OK;
}

///**
//  * @brief  Configure the QSPI in memory-mapped mode
//  * @retval QSPI memory status
//  */
//uint8_t BSP_QSPI_MemoryMappedMode(void)
//{
//  QSPI_CommandTypeDef      s_command;
//  QSPI_MemoryMappedTypeDef s_mem_mapped_cfg;

//  /* Configure the command for the read instruction */
//  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
//  s_command.Instruction       = QUAD_INOUT_FAST_READ_CMD;
//  s_command.AddressMode       = QSPI_ADDRESS_4_LINES;
//  s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
//  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
//  s_command.DataMode          = QSPI_DATA_4_LINES;
//  s_command.DummyCycles       = W25Q128FV_DUMMY_CYCLES_READ_QUAD;
//  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
//  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
//  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
//  
//  /* Configure the memory mapped mode */
//  s_mem_mapped_cfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_ENABLE;
//  s_mem_mapped_cfg.TimeOutPeriod     = 1;
//  
//  if (HAL_QSPI_MemoryMapped(&QSPIHandle, &s_command, &s_mem_mapped_cfg) != HAL_OK)
//  {
//    return QSPI_ERROR;
//  }

//  return QSPI_OK;
//}

/**
  * @brief  This function reset the QSPI memory.
  * @param  hqspi: QSPI handle
  * @retval None
  */
static uint8_t QSPI_ResetMemory()
{
  QSPI_CommandTypeDef s_command;
  /* Initialize the reset enable command */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = RESET_ENABLE_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DummyCycles       = 0;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* Send the reset memory command */
  s_command.Instruction = RESET_MEMORY_CMD;
  if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }

  /* Configure automatic polling mode to wait the memory is ready */  
  if (QSPI_AutoPollingMemReady(HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
  {
    return QSPI_ERROR;
  }
  return QSPI_OK;
}


/**
  * @brief  This function send a Write Enable and wait it is effective.
  * @param  hqspi: QSPI handle
  * @retval None
  */
static uint8_t QSPI_WriteEnable()
{
  QSPI_CommandTypeDef     s_command;
  QSPI_AutoPollingTypeDef s_config;
  /* Enable write operations */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = WRITE_ENABLE_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_NONE;
  s_command.DummyCycles       = 0;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
  if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }
  
  /* Configure automatic polling mode to wait for write enabling */  
  s_config.Match           = W25Q128FV_FSR_WREN;
  s_config.Mask            = W25Q128FV_FSR_WREN;
  s_config.MatchMode       = QSPI_MATCH_MODE_AND;
  s_config.StatusBytesSize = 1;
  s_config.Interval        = 0x10;
  s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

  s_command.Instruction    = READ_STATUS_REG1_CMD;
  s_command.DataMode       = QSPI_DATA_1_LINE;
	s_command.NbData         = 1;
	
  if (HAL_QSPI_AutoPolling(&QSPIHandle, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return QSPI_ERROR;
  }
  return QSPI_OK;
}

/**
  * @brief  This function read the SR of the memory and wait the EOP.
  * @param  hqspi: QSPI handle
  * @param  Timeout
  * @retval None
  */
static uint8_t QSPI_AutoPollingMemReady(uint32_t Timeout)
{
  QSPI_CommandTypeDef     s_command;
  QSPI_AutoPollingTypeDef s_config;
  /* Configure automatic polling mode to wait for memory ready */  
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = READ_STATUS_REG1_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_1_LINE;
  s_command.DummyCycles       = 0;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  s_config.Match           = 0x00;
  s_config.Mask            = W25Q128FV_FSR_BUSY;
  s_config.MatchMode       = QSPI_MATCH_MODE_AND;
  s_config.StatusBytesSize = 1;
  s_config.Interval        = 0x10;
  s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

  if (HAL_QSPI_AutoPolling(&QSPIHandle, &s_command, &s_config, Timeout) != HAL_OK)
  {
    return QSPI_ERROR;
  }
  return QSPI_OK;
}

 /**
  * @brief  ��ȡFLASH ID
  * @param 	��
  * @retval FLASH ID
  */
uint32_t QSPI_FLASH_ReadID(void)
{
  QSPI_CommandTypeDef s_command;
  uint32_t Temp = 0;
  uint8_t pData[3];
	/* Read JEDEC ID */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = READ_JEDEC_ID_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_NONE;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_1_LINE;
  s_command.DummyCycles       = 0;
  s_command.NbData            = 3;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
	
  if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
        printf("something wrong ....\r\n");
        /* User may add here some code to deal with this error */
        while(1)
        {
            
        }
  }
	if (HAL_QSPI_Receive(&hqspi, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
        printf("something wrong ....\r\n");
        /* User may add here some code to deal with this error */
        while(1)
        {
            
        }
  }
  
  Temp = pData[2] |( pData[1]<<8 )| ( pData[0]<<16 );

  return Temp;
}

 /**
  * @brief  ��ȡFLASH Device ID
  * @param 	��
  * @retval FLASH Device ID
  */
uint32_t QSPI_FLASH_ReadDeviceID(void)
{
  
  QSPI_CommandTypeDef s_command;
  uint32_t Temp = 0;
  uint8_t pData[3];
  /*##-2-Read Device ID Test    ###########################################*/
  /* Read Manufacture/Device ID */
  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  s_command.Instruction       = READ_ID_CMD;
  s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
  s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
  s_command.Address           = 0x000000;
  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode          = QSPI_DATA_1_LINE;
  s_command.DummyCycles       = 0;
  s_command.NbData            = 2;
  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
	
  if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
	printf("something wrong ....\r\n");
    /* User may add here some code to deal with this error */
    while(1)
    {
        
    }
  }
	if (HAL_QSPI_Receive(&hqspi, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
	printf("something wrong ....\r\n");
    /* User may add here some code to deal with this error */
    while(1)
    {
        
    }
  }
  
  Temp = pData[1] |( pData[0]<<8 ) ;

  return Temp;
}

DSTATUS TM_FATFS_FLASH_SPI_disk_status(BYTE lun)
{
	FLASH_DEBUG_FUNC();
	if(sFLASH_ID == QSPI_FLASH_ReadID())			/*���FLASH�Ƿ���������*/
	{
		return TM_FATFS_FLASH_SPI_Stat &= ~STA_NOINIT;	/* Clear STA_NOINIT flag */
	}
	else
	{
		return TM_FATFS_FLASH_SPI_Stat |= STA_NOINIT;
	}
}

DRESULT TM_FATFS_FLASH_SPI_disk_ioctl(BYTE lun,BYTE cmd, void *buff)
{

	  FLASH_DEBUG_FUNC();
	switch (cmd) {
		case GET_SECTOR_COUNT:
			*(DWORD * )buff = 2560;	/* ����������2560*4096/1024/1024=10(MB) */
		break;
		case GET_SECTOR_SIZE :     /*��ȡ������д�Ĵ�С���֣�*/

			*(WORD * )buff = 4096;	/*flash��Сд��ԪΪҳ��256�ֽڣ��˴�ȡ2ҳΪһ����д��λ*/
		break;
		case GET_BLOCK_SIZE :      /* ͬʱ��������������˫�֣� */
			*(DWORD * )buff = 1;   /*flash��1��sectorΪ��С������λ*/
		break;
		case CTRL_TRIM:	
		break;
		case CTRL_SYNC :
		break;
	}
	return RES_OK;
}

DRESULT TM_FATFS_FLASH_SPI_disk_read(
										BYTE lun,//��������������豸ʱ�õ�(0...)
										BYTE *buff,//���ݻ����� 
										DWORD sector, //�����׵�ַ
										UINT count)//��������(1..128)
{
	FLASH_DEBUG_FUNC();
	if ((TM_FATFS_FLASH_SPI_Stat & STA_NOINIT)) 
	{
		return RES_NOTRDY;
	}
	sector+=1536;//����ƫ�ƣ��ⲿFlash�ļ�ϵͳ�ռ�����ⲿFlash����6M�ռ�
	BSP_QSPI_Read(buff, sector <<12, count<<12);
	return RES_OK;
}

DRESULT TM_FATFS_FLASH_SPI_disk_write(
										BYTE lun,//��������������豸ʱ�õ�(0...)
										const BYTE *buff,//���ݻ����� 
										DWORD sector, //�����׵�ַ
										UINT count)//��������(1..128)
{
	uint32_t write_addr;  
	FLASH_DEBUG_FUNC();
	sector+=1536;//����ƫ�ƣ��ⲿFlash�ļ�ϵͳ�ռ�����ⲿFlash����4M�ռ�
	write_addr = sector<<12;    
	BSP_QSPI_Erase_Block(write_addr);
	BSP_QSPI_Write((uint8_t*)buff,write_addr,4096);
	return RES_OK;
}
   
/*********************************************END OF FILE**********************/
