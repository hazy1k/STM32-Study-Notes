/**
  ******************************************************************
  * @file    bsp_norflash.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   nor flash����
  ******************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  i.MXRT1052������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************
  */
#include "fsl_flexspi.h"
#include "fsl_debug_console.h"

#include "fsl_iomuxc.h"
#include "pin_mux.h"
#include "board.h"
#include "clock_config.h"
#include "fsl_common.h"

#include "pad_config.h"
#include "./norflash/bsp_norflash.h"  
#include "./delay/core_delay.h"  

/*******************************************************************************
* Definitions
******************************************************************************/
/* ����ָ���ڲ��ұ��еı�� */
#define NOR_CMD_LUT_SEQ_IDX_READ_NORMAL                 0
#define NOR_CMD_LUT_SEQ_IDX_READ_FAST                   1
#define NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD              2
#define NOR_CMD_LUT_SEQ_IDX_READSTATUS                  3
#define NOR_CMD_LUT_SEQ_IDX_WRITEENABLE                 4
#define NOR_CMD_LUT_SEQ_IDX_ERASESECTOR                 5
#define NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_SINGLE          6
#define NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_QUAD            7
#define NOR_CMD_LUT_SEQ_IDX_READID                      8
#define NOR_CMD_LUT_SEQ_IDX_READJEDECID                 9
#define NOR_CMD_LUT_SEQ_IDX_WRITESTATUSREG              10
#define NOR_CMD_LUT_SEQ_IDX_READSTATUSREG               11
#define NOR_CMD_LUT_SEQ_IDX_ERASECHIP                   12
#define NOR_CMD_LUT_SEQ_IDX_AHB_PAGEPROGRAM_QUAD_1      13
#define NOR_CMD_LUT_SEQ_IDX_AHB_PAGEPROGRAM_QUAD_2      14
#define NOR_CMD_LUT_SEQ_IDX_READ_UUID_ISSI              15
#define NOR_CMD_LUT_SEQ_IDX_READ_UUID_WB                16

/* ���ұ�ĳ��� */
#define CUSTOM_LUT_LENGTH           90


/* FLEXSPI������ʹ��ͬ����PAD���� */
#define FLEXSPI_PAD_CONFIG_DATA         (SRE_1_FAST_SLEW_RATE| \
                                        DSE_6_R0_6| \
                                        SPEED_3_MAX_200MHz| \
                                        ODE_0_OPEN_DRAIN_DISABLED| \
                                        PKE_1_PULL_KEEPER_ENABLED| \
                                        PUE_0_KEEPER_SELECTED| \
                                        PUS_0_100K_OHM_PULL_DOWN| \
                                        HYS_0_HYSTERESIS_DISABLED)   
    /* ����˵�� : */
    /* ת������: ת�����ʿ�
        ����ǿ��: R0/6 
        �������� : max(200MHz)
        ��©����: �ر� 
        ��/����������: ʹ��
        ��/������ѡ��: ������
        ����/����ѡ��: 100Kŷķ����(ѡ���˱�������������Ч)
        �ͻ�������: ��ֹ */ 

/*******************************************************************************
* Prototypes
******************************************************************************/
static void FlexSPI_NorFlash_IOMUXC_MUX_Config(void);
static void FlexSPI_NorFlash_IOMUXC_PAD_Config(void);
static void NorFlash_FlexSPI_ModeInit(void);
/*******************************************************************************
 * Variables
 ******************************************************************************/

extern flexspi_device_config_t deviceconfig;
extern const uint32_t customLUT[CUSTOM_LUT_LENGTH];

/*******************************************************************************
 * Code
 ******************************************************************************/
/* �豸������صĲ��� */
flexspi_device_config_t deviceconfig = {
    .flexspiRootClk = 120000000,
    .flashSize = (32*1024),
    .CSIntervalUnit = kFLEXSPI_CsIntervalUnit1SckCycle,
    .CSInterval = 2,
    .CSHoldTime = 1,
    .CSSetupTime = 1,
    .dataValidTime = 2,
    .columnspace = 0,
    .enableWordAddress = false,
    .AWRSeqIndex = NOR_CMD_LUT_SEQ_IDX_AHB_PAGEPROGRAM_QUAD_1,
    .AWRSeqNumber = 2,
    .ARDSeqIndex = NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD,
    .ARDSeqNumber = 1,
    /* W25Q256 typical time=0.7ms,max time=3ms 
     *  fAHB = 528MHz,T AHB = 1/528us
     *  unit = 32768/528 = 62.06us
     *  ȡ��ʱʱ��Ϊ1ms��
     *  AHBWriteWaitInterval = 1*1000/62.06 = 17
     */
    .AHBWriteWaitUnit = kFLEXSPI_AhbWriteWaitUnit32768AhbCycle,
    .AHBWriteWaitInterval = 17,
};

/* ������ұ�LUT
 * �±���[4 * NOR_CMD_LUT_SEQ_IDX_xxxx]��ʾ1�����У�
 * 1������������8��ָ�ʹ�ú�FLEXSPI_LUT_SEQ����һ�ζ���2��ָ�
 * һ��FLEXSPI_LUT_SEQռһ��LUT�Ĵ������˿�A�Ͷ˿�B����64��LUT�Ĵ�����
 * ����CUSTOM_LUT_LENGTH���ֵΪ64��
 *
 * FLEXSPI_LUT_SEQ��ʽ���£�LUTָ��0��ʹ�õ���������Ŀ��ָ��Ĳ�����
                            LUTָ��1��ʹ�õ���������Ŀ��ָ��Ĳ�����
 * 
 * ����8��ָ�������Ӧ��STOPָ���������kFLEXSPI_Command_STOP��
 * ������ΪSTOPָ���е����в�����Ϊ0��������ĳ�ʼֵҲ��Ϊ0��
 * ���Բ������е�ĩβ������STOPָ��Ҳ���������С�
 */
const uint32_t customLUT[CUSTOM_LUT_LENGTH] = {
        /* ��ͨ��ָ�Normal read mode -SDR */
        [4 * NOR_CMD_LUT_SEQ_IDX_READ_NORMAL] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_ReadData_4Addr, 
                            kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, FLASH_ADDR_LENGTH),
            
        [4 * NOR_CMD_LUT_SEQ_IDX_READ_NORMAL + 1] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04, 
                            kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

        /* ���ٶ�ָ�Fast read mode - SDR */
        [4 * NOR_CMD_LUT_SEQ_IDX_READ_FAST] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_FastReadData_4Addr, 
                            kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, FLASH_ADDR_LENGTH),
            
        [4 * NOR_CMD_LUT_SEQ_IDX_READ_FAST + 1] = 
          FLEXSPI_LUT_SEQ(kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_1PAD, 0x08,
                          kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04),

        /* QUADģʽ���ٶ�ָ�Fast read quad mode - SDR */
        [4 * NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_FastReadQuad_4Addr, 
                            kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, FLASH_ADDR_LENGTH),
        [4 * NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD + 1] = 
          FLEXSPI_LUT_SEQ(kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_4PAD, 0x08, 
                          kFLEXSPI_Command_READ_SDR, kFLEXSPI_4PAD, 0x04),

        /* ��ȡ��չ������Read extend parameters */
        [4 * NOR_CMD_LUT_SEQ_IDX_READSTATUS] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_ReadStatusReg, 
                            kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04),

        /* дʹ�ܣ�Write Enable */
        [4 * NOR_CMD_LUT_SEQ_IDX_WRITEENABLE] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_WriteEnable, 
                            kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

        /* ����������Erase Sector  */
        [4 * NOR_CMD_LUT_SEQ_IDX_ERASESECTOR] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_SectorErase_4Addr, 
                            kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, FLASH_ADDR_LENGTH),

        /* SINGLEģʽҳд�룬Page Program - single mode */
        [4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_SINGLE] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_PageProgram_4Addr, 
                            kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, FLASH_ADDR_LENGTH),
        [4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_SINGLE + 1] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 0x04, 
                            kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

        /* QUADģʽҳд�룬Page Program - quad mode */
        [4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_QUAD] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_PageProgramQuad_4Addr, 
                            kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, FLASH_ADDR_LENGTH),
        [4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_QUAD + 1] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_4PAD, 0x04, 
                            kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),  

        /* ��ID��Read ID ��ID7-ID0*/
        [4 * NOR_CMD_LUT_SEQ_IDX_READID] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_DeviceID, 
                            kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_1PAD, FLASH_ADDR_LENGTH),
        [4 * NOR_CMD_LUT_SEQ_IDX_READID + 1] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04, 
                            kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),
        
        /* ��JedecDeviceID,MF7-MF0+ID15-ID0 */
        [4 * NOR_CMD_LUT_SEQ_IDX_READJEDECID] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_JedecDeviceID, 
                            kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04),

        /* д״̬�Ĵ�����Enable Quad mode */
        [4 * NOR_CMD_LUT_SEQ_IDX_WRITESTATUSREG] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_WriteStatusReg, 
                            kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 0x04),

        /* ��״̬�Ĵ�����Read status register */
        [4 * NOR_CMD_LUT_SEQ_IDX_READSTATUSREG] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_ReadStatusReg, 
                            kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04),

        /* ������ƬFLASH��Erase Chip */
        [4 * NOR_CMD_LUT_SEQ_IDX_ERASECHIP] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_ChipErase, 
                            kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),
            
        
        /* ��AHB������ʵ� QUADģʽҳд�� ���У�����дʹ�ܺ�ҳд���������� */
        /* дʹ�ܣ�Write Enable */
        [4 * NOR_CMD_LUT_SEQ_IDX_AHB_PAGEPROGRAM_QUAD_1] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_WriteEnable, 
                            kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),
            
        /* QUADģʽҳд�룬Page Program - quad mode */
        [4 * NOR_CMD_LUT_SEQ_IDX_AHB_PAGEPROGRAM_QUAD_2] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_PageProgramQuad_4Addr, 
                            kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, FLASH_ADDR_LENGTH),
        [4 * NOR_CMD_LUT_SEQ_IDX_AHB_PAGEPROGRAM_QUAD_2 + 1] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_4PAD, 0x04, 
                            kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),  	

        // ��ΨһID ISSIоƬ������0X4B/
        [4 * NOR_CMD_LUT_SEQ_IDX_READ_UUID_ISSI] 	=
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR,kFLEXSPI_1PAD,0x4B, 
                              kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, FLASH_ADDR_LENGTH),
                              
         [4 * NOR_CMD_LUT_SEQ_IDX_READ_UUID_ISSI + 1] 	=
             FLEXSPI_LUT_SEQ(kFLEXSPI_Command_READ_SDR,kFLEXSPI_1PAD,0x04,
                              kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),
                              
        // ��ΨһID ����оƬ������0X4B/
        [4 * NOR_CMD_LUT_SEQ_IDX_READ_UUID_WB] 	=
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x4B, 
                              kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_1PAD, 0x18),
                              
        [4 * NOR_CMD_LUT_SEQ_IDX_READ_UUID_WB + 1] 	=
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04, 
                              kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),


 

};



/**
* @brief  ��ʼ��NORFLASH���IOMUXC��MUX��������
* @param  ��
* @retval ��
*/
static void FlexSPI_NorFlash_IOMUXC_MUX_Config(void)
{
  /* FlexSPIͨѶ���� */
  IOMUXC_SetPinMux(NORFLASH_SS_IOMUXC, 1U);                                   
  IOMUXC_SetPinMux(NORFLASH_SCLK_IOMUXC, 1U);  
  IOMUXC_SetPinMux(NORFLASH_DATA00_IOMUXC, 1U);  
  IOMUXC_SetPinMux(NORFLASH_DATA01_IOMUXC, 1U);  
  IOMUXC_SetPinMux(NORFLASH_DATA02_IOMUXC, 1U);  
  IOMUXC_SetPinMux(NORFLASH_DATA03_IOMUXC, 1U);  
}

/**
* @brief  ��ʼ��NORFLASH���IOMUXC��PAD��������
* @param  ��
* @retval ��
*/
static void FlexSPI_NorFlash_IOMUXC_PAD_Config(void)
{
  /* FlexSPIͨѶ����ʹ��ͬ������������ */
  IOMUXC_SetPinConfig(NORFLASH_SS_IOMUXC, FLEXSPI_PAD_CONFIG_DATA);   
  IOMUXC_SetPinConfig(NORFLASH_SCLK_IOMUXC, FLEXSPI_PAD_CONFIG_DATA);                               
  IOMUXC_SetPinConfig(NORFLASH_DATA00_IOMUXC, FLEXSPI_PAD_CONFIG_DATA);   
  IOMUXC_SetPinConfig(NORFLASH_DATA01_IOMUXC, FLEXSPI_PAD_CONFIG_DATA);                               
  IOMUXC_SetPinConfig(NORFLASH_DATA02_IOMUXC, FLEXSPI_PAD_CONFIG_DATA);   
  IOMUXC_SetPinConfig(NORFLASH_DATA03_IOMUXC, FLEXSPI_PAD_CONFIG_DATA);                               
}

/**
* @brief  ��ʼ��NorFlashʹ�õ�FlexSPI����ģʽ��ʱ��
* @param  ��
* @retval ��
*/  
static void NorFlash_FlexSPI_ModeInit(void)
{
  flexspi_config_t config;

  const clock_usb_pll_config_t g_ccmConfigUsbPll = {.loopDivider = 0U};

  /* ��ʼ��USB1PLL����PLL3��loopDivider=0��
      ����USB1PLL=PLL3 = 24*20 = 480MHz */
  CLOCK_InitUsb1Pll(&g_ccmConfigUsbPll);
  /* ����PLL3 PFD0Ƶ��Ϊ��PLL3*18/24 = 360MHZ. */
  CLOCK_InitUsb1Pfd(kCLOCK_Pfd0, 24);  
  /* ѡ��PLL3 PFD0��Ϊflexspiʱ��Դ
      00b derive clock from semc_clk_root_pre
      01b derive clock from pll3_sw_clk
      10b derive clock from PLL2 PFD2
      11b derive clock from PLL3 PFD0 */  
  CLOCK_SetMux(kCLOCK_FlexspiMux, 0x3); 
  /* ����flexspiDiv��Ƶ���ӣ��õ�FLEXSPI_CLK_ROOT = PLL3 PFD0/(flexspiDiv+1) = 120M. */
  CLOCK_SetDiv(kCLOCK_FlexspiDiv, 2);  
  
  /* �ر�DCache���� */
  SCB_DisableDCache();

  /*��ȡ FlexSPI ����Ĭ������ */
  FLEXSPI_GetDefaultConfig(&config);

  /* ����AHBԤ��ȡ�Ĺ��� */
  config.ahbConfig.enableAHBPrefetch = true;
  
  /* д������ */
  FLEXSPI_Init(FLEXSPI, &config);

  /* ���ݴ������湦�������������� */
  FLEXSPI_SetFlashConfig(FLEXSPI, &deviceconfig, kFLEXSPI_PortA1);

  /* ���²��ұ� */
  FLEXSPI_UpdateLUT(FLEXSPI, 0, customLUT, CUSTOM_LUT_LENGTH);
}

/**
* @brief  FlexSPI��ʼ��
* @param  ��
* @retval ��
*/
void FlexSPI_NorFlash_Init(void)
{
	uint32_t JedecDeviceID;
	
  FlexSPI_NorFlash_IOMUXC_MUX_Config();
  FlexSPI_NorFlash_IOMUXC_PAD_Config();
  NorFlash_FlexSPI_ModeInit();
	
	/* ��ȡJedecDevice ID. */
  FlexSPI_NorFlash_Get_JedecDevice_ID(FLEXSPI, &JedecDeviceID);
	
	/* ISSI���̵�FLASH��Ҫʹ��Quadģʽ,����IDΪ��8λ */
	if((JedecDeviceID &0xFF0000) == (FLASH_ISSI_JEDECDEVICE_ID &0xFF0000))
	{
		FlexSPI_NorFlash_Enable_Quad_Mode(FLEXSPI);
	}
}


/**
* @brief  ��ȡFLASHоƬ��JedecDevice ID
* @param  base:ʹ�õ�FlexSPI�˿�
* @param  vendorID[out]:�洢���յ���IDֵ�Ļ���������СΪ1�ֽڣ�����ʱ��ֵΪ0xEF4019
* @retval FlexSPI���䷵�ص�״ֵ̬������Ϊ0
*/
status_t FlexSPI_NorFlash_Get_JedecDevice_ID(FLEXSPI_Type *base, uint32_t *vendorID)
{
    uint32_t temp;
    flexspi_transfer_t flashXfer;
    
    /* Ҫ��д��FLASH�ڲ��洢��Ԫ��ַ�����������ַ */
    flashXfer.deviceAddress = 0;
    /* Ҫʹ�õĶ˿� */
    flashXfer.port = kFLEXSPI_PortA1;
    /* ����������� */
    flashXfer.cmdType = kFLEXSPI_Read;
    /* Ҫִ�е����и��� */
    flashXfer.SeqNumber = 1;
    /* Ҫִ�е����к� */
    flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_READJEDECID;
    /* ���յ��������Ȼ��浽temp */
    flashXfer.data = &temp;
    /* Ҫ���յ����ݸ��� */
    flashXfer.dataSize = 3;

    /* ��ʼ�������� */
    status_t status = FLEXSPI_TransferBlocking(base, &flashXfer);

    /* �����ߵ��ֽڣ������ֵ��vendorId */
    *vendorID = ((temp&0xFF)<<16) | (temp&0xFF00) | ((temp&0xFF0000)>>16);

    return status;
}

/**
* @brief  ��ȡFLASHоƬ��Device ID
* @param  base:ʹ�õ�FlexSPI�˿�
* @param  vendorID[out]:�洢���յ���IDֵ�Ļ���������СΪ1�ֽڣ�����ʱ��ֵΪ0x18
* @retval FlexSPI���䷵�ص�״ֵ̬������Ϊ0
*/
status_t FlexSPI_NorFlash_Get_Device_ID(FLEXSPI_Type *base, uint8_t *vendorID)
{
    uint32_t temp;
    flexspi_transfer_t flashXfer;
    
    /* Ҫ��д��FLASH�ڲ��洢��Ԫ��ַ�����������ַ */
    flashXfer.deviceAddress = 0;
    /* Ҫʹ�õĶ˿� */
    flashXfer.port = kFLEXSPI_PortA1;
    /* ����������� */
    flashXfer.cmdType = kFLEXSPI_Read;
    /* Ҫִ�е����и��� */
    flashXfer.SeqNumber = 1;
    /* Ҫִ�е����к� */
    flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_READID;
    /* ���յ��������Ȼ��浽temp */
    flashXfer.data = &temp;
    /* Ҫ���յ����ݸ��� */
    flashXfer.dataSize = 1;

    /* ��ʼ�������� */
    status_t status = FLEXSPI_TransferBlocking(base, &flashXfer);

    /* �ѽ��յ������ݸ�ֵ�������� */
    *vendorID = temp;

    return status;
}

/**
* @brief  ��ȡFLASHоƬ��UUID (ISSIоƬ)
* @param  buf:��ȡ����UUID ,ISоƬ16���ֽڣ�wbоƬ8���ֽ�
* @retval Ĭ�Ϸ�������
*/
uint8_t FlexSPI_FlashUUID_Get_ISSI(uint8_t *buf)
{
    volatile uint32_t pid,uid,data;
    flexspi_transfer_t FlashTransfer;

    FlashTransfer.deviceAddress=0;                      //��ַ
    FlashTransfer.port=kFLEXSPI_PortA1;                 //�˿�
    FlashTransfer.cmdType=kFLEXSPI_Read;                //�������ͣ�������
    FlashTransfer.SeqNumber=1;                          //���
    FlashTransfer.seqIndex=NOR_CMD_LUT_SEQ_IDX_READ_UUID_ISSI;  //LUT������������
    FlashTransfer.data=(uint32_t*)buf;                 //���ݻ�����
    FlashTransfer.dataSize=16;                           //���ݳ���
    FLEXSPI_TransferBlocking(FLEXSPI,&FlashTransfer);
    return 1;

}

/**
* @brief  ��ȡFLASHоƬ��UUID (WBоƬ)
* @param  buf:��ȡ����UUID ,ISоƬ16���ֽڣ�wbоƬ8���ֽ�
* @retval Ĭ�Ϸ�������
*/
uint8_t FlexSPI_FlashUUID_Get(uint8_t *buf)
{
    volatile uint32_t pid,uid,data;
    flexspi_transfer_t FlashTransfer;

    FlashTransfer.deviceAddress=0;                      //��ַ
    FlashTransfer.port=kFLEXSPI_PortA1;                 //�˿�
    FlashTransfer.cmdType=kFLEXSPI_Read;                //�������ͣ�������
    FlashTransfer.SeqNumber=1;                          //���
    FlashTransfer.seqIndex=NOR_CMD_LUT_SEQ_IDX_READ_UUID_WB;  //LUT������������
    FlashTransfer.data=(uint32_t*)buf;                 //���ݻ�����
    FlashTransfer.dataSize=8;                           //���ݳ���
    FLEXSPI_TransferBlocking(FLEXSPI,&FlashTransfer);
    return 1;

}
/**
* @brief  дʹ��
* @param  base:ʹ�õ�FlexSPI�˿�
* @retval FlexSPI���䷵�ص�״ֵ̬������Ϊ0
*/
status_t FlexSPI_NorFlash_Write_Enable(FLEXSPI_Type *base)
{
    flexspi_transfer_t flashXfer;
    status_t status;

    /* дʹ�� */
    flashXfer.deviceAddress = 0;
    flashXfer.port = kFLEXSPI_PortA1;
    flashXfer.cmdType = kFLEXSPI_Command;
    flashXfer.SeqNumber = 1;
    flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_WRITEENABLE;

    status = FLEXSPI_TransferBlocking(base, &flashXfer);

    return status;
}

/**
* @brief  �ȴ���FLASH����״̬
* @param  base:ʹ�õ�FlexSPI�˿�
* @retval FlexSPI���䷵�ص�״ֵ̬������Ϊ0
*/
status_t FlexSPI_NorFlash_Wait_Bus_Busy(FLEXSPI_Type *base)
{
    /* �ȴ�������״̬ */
    bool isBusy;
    uint32_t readValue;
    status_t status;
    flexspi_transfer_t flashXfer;

    /* ��״̬�Ĵ��� */
    flashXfer.deviceAddress = 0;
    flashXfer.port = kFLEXSPI_PortA1;
    flashXfer.cmdType = kFLEXSPI_Read;
    flashXfer.SeqNumber = 1;
    flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_READSTATUSREG;
    flashXfer.data = &readValue;
    flashXfer.dataSize = 1;

    do
    {
        status = FLEXSPI_TransferBlocking(base, &flashXfer);

        if (status != kStatus_Success)
        {
            return status;
        }
        /* ��busyλΪ1��ʾæµ */
        if (FLASH_BUSY_STATUS_POL)
        {
            /* �ж�״̬�Ĵ�����busyλ */
            if (readValue & (1U << FLASH_BUSY_STATUS_OFFSET))
            {
                isBusy = true;
            }
            else
            {
                isBusy = false;
            }
        }
        else /* ��busyλΪ0��ʾæµ */
        {
            if (readValue & (1U << FLASH_BUSY_STATUS_OFFSET))
            {
                isBusy = false;
            }
            else
            {
                isBusy = true;
            }
        }

    } while (isBusy);

    return status;
}


/**
* @brief  ��״̬�Ĵ���
* @param  base:ʹ�õ�FlexSPI�˿�
* @retval FlexSPI���䷵�ص�״ֵ̬������Ϊ0
*/
status_t FlexSPI_NorFlash_Read_Status_Register(FLEXSPI_Type *base, uint8_t *SR_value)
{

    uint32_t readValue;
    status_t status;
    flexspi_transfer_t flashXfer;

    /* ��״̬�Ĵ��� */
    flashXfer.deviceAddress = 0;
    flashXfer.port = kFLEXSPI_PortA1;
    flashXfer.cmdType = kFLEXSPI_Read;
    flashXfer.SeqNumber = 1;
    flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_READSTATUSREG;
    flashXfer.data = &readValue;
    flashXfer.dataSize = 1;

		status = FLEXSPI_TransferBlocking(base, &flashXfer);
		
		*SR_value = readValue;
   
		return status;
}

/**
* @brief  д״̬�Ĵ���
* @param  base:ʹ�õ�FlexSPI�˿�
* @param  config:Ҫд�����������
* @retval FlexSPI���䷵�ص�״ֵ̬������Ϊ0
*/
status_t FlexSPI_NorFlash_Write_Status_Register(FLEXSPI_Type *base, uint8_t *config)
{
		flexspi_transfer_t flashXfer;
    status_t status;
		
		 /* дʹ�� */
    status = FlexSPI_NorFlash_Write_Enable(base);

    if (status != kStatus_Success)
    {
        return status;
    }

    /* д������ */
    flashXfer.deviceAddress = 0;
    flashXfer.port = kFLEXSPI_PortA1;
    flashXfer.cmdType = kFLEXSPI_Write;
    flashXfer.SeqNumber = 1;
    flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_WRITESTATUSREG;
		flashXfer.data = (uint32_t *)config;
    flashXfer.dataSize = 1;

    status = FLEXSPI_TransferBlocking(base, &flashXfer);
		
		if (status != kStatus_Success)
    {
        return status;
    }
    
    /* �ȴ�FLASH������״̬ */
    status = FlexSPI_NorFlash_Wait_Bus_Busy(base);
    
    return status;
}

/**
* @brief  ʹ��Quadģʽ��ISSI��FLASH�ϵ�������ã�Winbond��Ӧ���ã�
* @note   ISSI���̵�FLASH��״̬�Ĵ�����bit6-QE�������øù��ܣ���λΪ1ʱ��ʾʹ��Quadģʽ
*				 Winbond���̵�FLASH��״̬�Ĵ�����bit6-TB������д�������ܵ�
*         ������ʹ��Winbond���̵�FLASHʱ��Ӧ���ñ�������
*         ��ʹ��ISSI���̵�FLASHʱ������ñ�����   		
* @param  base:ʹ�õ�FlexSPI�˿�
* @param  config:Ҫд�����������
* @retval FlexSPI���䷵�ص�״ֵ̬������Ϊ0
*/
status_t FlexSPI_NorFlash_Enable_Quad_Mode(FLEXSPI_Type *base)
{
	uint8_t SR_value;
	status_t status;
	
	/* ��ȡԭ�Ĵ��������� */
	status = FlexSPI_NorFlash_Read_Status_Register(base, &SR_value);
	
	if (status != kStatus_Success)
	{
		return status;
	}
	
	/* ��bit6-QEλ��1��ISSI���̵�FLASH�� */
	SR_value |= 0x40;

	/* д���ֵ */
	status = FlexSPI_NorFlash_Write_Status_Register(base, &SR_value);
	
	return status;
}


/**
* @brief  ��������
* @param  base:ʹ�õ�FlexSPI�˿�
* @param  dstAddr:Ҫ������������ʼ��ַ
* @retval FlexSPI���䷵�ص�״ֵ̬������Ϊ0
*/
status_t FlexSPI_NorFlash_Erase_Sector(FLEXSPI_Type *base, uint32_t dstAddr)
{
    status_t status;
    flexspi_transfer_t flashXfer;

    /* дʹ�� */
    status = FlexSPI_NorFlash_Write_Enable(base);

    if (status != kStatus_Success)
    {
        return status;
    }

    /* ��������ָ�� */
    flashXfer.deviceAddress = dstAddr;
    flashXfer.port = kFLEXSPI_PortA1;
    flashXfer.cmdType = kFLEXSPI_Command;
    flashXfer.SeqNumber = 1;
    flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_ERASESECTOR;
    
    status = FLEXSPI_TransferBlocking(base, &flashXfer);

    if (status != kStatus_Success)
    {
        return status;
    }
    
    /* �ȴ�FLASH������״̬ */
    status = FlexSPI_NorFlash_Wait_Bus_Busy(base);
    
    return status;
}

/**
* @brief  ҳд��
* @param  base:ʹ�õ�FlexSPI�˿�
* @param  dstAddr:Ҫд�����ʼ��ַ
* @param  src:Ҫд������ݵ�ָ��
* @param  dataSize:Ҫд��������������ܴ���256
* @retval FlexSPI���䷵�ص�״ֵ̬������Ϊ0
*/
status_t FlexSPI_NorFlash_Page_Program(FLEXSPI_Type *base, 
                                       uint32_t dstAddr, 
                                       uint8_t *src,
                                       uint16_t dataSize)
{
    status_t status;
    flexspi_transfer_t flashXfer;

    /* дʹ�� */
    status = FlexSPI_NorFlash_Write_Enable(base);

    if (status != kStatus_Success)
    {
        return status;
    }

    /* ���ô���ṹ�� */
    flashXfer.deviceAddress = dstAddr;
    flashXfer.port = kFLEXSPI_PortA1;
    flashXfer.cmdType = kFLEXSPI_Write;
    flashXfer.SeqNumber = 1;
    flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_QUAD;
    flashXfer.data = (uint32_t *)src;
    flashXfer.dataSize = dataSize;
    
    status = FLEXSPI_TransferBlocking(base, &flashXfer);

    if (status != kStatus_Success)
    {
        return status;
    }

    /* �ȴ�д����� */
    status = FlexSPI_NorFlash_Wait_Bus_Busy(base);

    return status;
}


/**
  * @brief ��FLASHд�벻����������
  * @note  д��ǰҪȷ���ÿռ��Ǳ�������״̬
  * @param  base:ʹ�õ�FlexSPI�˿�
  * @param  dstAddr:Ҫд�����ʼ��ַ
  * @param  src:Ҫд������ݵ�ָ��
  * @param  dataSize:Ҫд�����������û�����ƣ�ȷ�Ͽռ����ѱ��������ļ���
  * @retval д��ķ���ֵ��Ϊ0�Ǳ�ʾ����
  */
status_t FlexSPI_NorFlash_Buffer_Program(FLEXSPI_Type *base, 
                                         uint32_t dstAddr, 
                                         uint8_t *src,
                                         uint16_t dataSize)                              
{
  status_t status = kStatus_Success;
  uint16_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;
  /* ����Ҫ������ֽ�������ʼֵΪNumByteToWrite*/
  uint16_t NumByteToWriteRest = dataSize;
  /* ��������������д���
    1.д����׵�ַ�Ƿ���� 
    2.���һ��д���Ƿ�պ�д��һҳ */
  Addr = dstAddr % FLASH_PAGE_SIZE;
  count = FLASH_PAGE_SIZE - Addr;
  
  /* ��NumByteToWrite > count��
     ��һҳд��count���ֽڣ��������ֽ��ٽ��к�������
     ������ (NumByteToWriteRest = dataSize - count) ���������NumOfPage��NumOfSingle���д���
     ��NumByteToWrite < count��
     ������һҳ���ݣ�ֱ����NumByteToWriteRest = NumByteToWrite���NumOfPage��NumOfSingle���� */
  NumByteToWriteRest = (dataSize > count) ? (dataSize - count) : dataSize;
  
  /* Ҫ����д���ҳ����������ǰcount�ֽڣ�*/
  NumOfPage =  NumByteToWriteRest / FLASH_PAGE_SIZE;
  /* ���һҳҪд����ֽ�����������ǰcount�ֽڣ�*/
  NumOfSingle = NumByteToWriteRest % FLASH_PAGE_SIZE;
 
  /* dataSize > countʱ����Ҫ������һҳд��count���ֽ�
     dataSize < countʱ������д˲��� */
  if(count != 0 && dataSize > count)
  {  
    status = FlexSPI_NorFlash_Page_Program(base, dstAddr, src, count);    
    if(status != kStatus_Success) return status;
    
    dstAddr += count;
    src += count;
  }   
  
  /* ����������� */
  if(NumOfPage== 0 ) 
  {
    status = FlexSPI_NorFlash_Page_Program(base, dstAddr, src, NumOfSingle);
    if(status != kStatus_Success) return status;
  }
  else
  {   
    /* �������ݴ���һҳ */
    while(NumOfPage--)
    {
      status = FlexSPI_NorFlash_Page_Program(base, dstAddr, src, FLASH_PAGE_SIZE);
      if(status != kStatus_Success) return status;
      
      dstAddr +=  FLASH_PAGE_SIZE;
      src += FLASH_PAGE_SIZE;  
    }
    /* ���һҳ */
    if(NumOfSingle != 0)
    {
      status = FlexSPI_NorFlash_Page_Program(base, dstAddr, src, NumOfSingle); 
      if(status != kStatus_Success) return status;

    }
  }

  return status;
}

/**
* @brief  ��ȡ����
* @param  base:ʹ�õ�FlexSPI�˿�
* @param  address:Ҫ��ȡ����ʼ��ַ
* @param  dst[out]:�洢���յ������ݵ�ָ��
* @param  dataSize:Ҫ��ȡ�������������ܴ���65535
* @retval FlexSPI���䷵�ص�״ֵ̬������Ϊ0
*/
status_t FlexSPI_NorFlash_Buffer_Read(FLEXSPI_Type *base, 
                                      uint32_t address, 
                                      uint8_t *dst,
                                      uint16_t dataSize)
{
    status_t status;
    flexspi_transfer_t flashXfer;

    /* ���ô���ṹ�� */
    flashXfer.deviceAddress = address;
    flashXfer.port = kFLEXSPI_PortA1;
    flashXfer.cmdType = kFLEXSPI_Read;
    flashXfer.SeqNumber = 1;
    flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD;
    flashXfer.data = (uint32_t *)dst;
    flashXfer.dataSize = dataSize;
    
    status = FLEXSPI_TransferBlocking(base, &flashXfer);

    return status;
}

/****************************END OF FILE**********************/
