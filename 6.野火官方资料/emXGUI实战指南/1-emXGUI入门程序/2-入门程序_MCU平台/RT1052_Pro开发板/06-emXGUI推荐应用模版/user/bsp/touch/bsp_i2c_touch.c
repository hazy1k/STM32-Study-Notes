/**
  ******************************************************************************
  * @file    bsp_i2c_touch.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ���ݴ�������ר��iic����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  i.MXRT1052������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
#include "fsl_debug_console.h"	

#include "fsl_iomuxc.h"
#include "pin_mux.h"
#include "fsl_gpio.h"
#include "fsl_lpi2c.h"

#include "./delay/core_delay.h"
#include "./touch/bsp_i2c_touch.h"
#include "./touch/bsp_touch_gtxx.h"
#include "./pad_config.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* I2C��SCL��SDA����ʹ��ͬ����PAD���� */
#define I2C_PAD_CONFIG_DATA            (SRE_0_SLOW_SLEW_RATE| \
                                        DSE_6_R0_6| \
                                        SPEED_1_MEDIUM_100MHz| \
                                        ODE_0_OPEN_DRAIN_DISABLED| \
                                        PKE_1_PULL_KEEPER_ENABLED| \
                                        PUE_0_KEEPER_SELECTED| \
                                        PUS_3_22K_OHM_PULL_UP| \
                                        HYS_0_HYSTERESIS_DISABLED)   
    /* ����˵�� : */
    /* ת������: ת��������
        ����ǿ��: R0/6 
        �������� : medium(100MHz)
        ��©����: �ر� 
        ��/����������: ʹ��
        ��/������ѡ��: ������
        ����/����ѡ��: 22Kŷķ����(ѡ���˱�������������Ч)
        �ͻ�������: ��ֹ */ 
        

/* ����оƬ��RST��INT����ʹ��ͬ����PAD���� */
#define GTP_RST_INT_PAD_CONFIG_DATA            (SRE_0_SLOW_SLEW_RATE| \
                                                DSE_6_R0_6| \
                                                SPEED_1_MEDIUM_100MHz| \
                                                ODE_0_OPEN_DRAIN_DISABLED| \
                                                PKE_1_PULL_KEEPER_ENABLED| \
                                                PUE_0_KEEPER_SELECTED| \
                                                PUS_2_100K_OHM_PULL_UP| \
                                                HYS_0_HYSTERESIS_DISABLED)   
    /* ����˵�� : */
    /* ת������: ת��������
        ����ǿ��: R0/6 
        �������� : medium(100MHz)
        ��©����: �ر� 
        ��/����������: ʹ��
        ��/������ѡ��: ������
        ����/����ѡ��: 100Kŷķ����(ѡ���˱�������������Ч)
        �ͻ�������: ��ֹ */   

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void I2C_GTP_IOMUXC_MUX_Config(void);
static void I2C_GTP_IOMUXC_PAD_Config(void);
static void GTP_I2C_ModeInit(void);

static void I2C_Master_Callback(LPI2C_Type *base, lpi2c_master_handle_t *handle, status_t status, void *userData);
static  uint32_t I2C_Timeout_Callback(uint8_t errorCode);

/*******************************************************************************
 * Variables
 ******************************************************************************/

lpi2c_master_handle_t g_m_handle;
volatile bool g_MasterCompletionFlag = false;
volatile bool g_TouchPadInputSignal = false;
/*******************************************************************************
 * Code
 ******************************************************************************/

/**
  * @brief  ��GT91xxоƬ���и�λ
  * @param  ��
  * @retval ��
  */
void GTP_ResetChip(void)
{
	/* �Ȱ�RST INT����Ϊ���ģʽ */
	gpio_pin_config_t rst_int_config = {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode};

	/* ��ʼ�� RST INT ���� */
	GPIO_PinInit(TOUCH_PAD_RST_GPIO, TOUCH_PAD_RST_GPIO_PIN, &rst_int_config);
	GPIO_PinInit(TOUCH_PAD_INT_GPIO, TOUCH_PAD_INT_GPIO_PIN, &rst_int_config);

  /*��gt9157���豸��ַ������Ϊ0xBA��
    1.RST��INT�͵�ƽ
    2.������ʱ100us
    3.RST�л�Ϊ�ߵ�ƽ
    4.������ʱ5ms
    5.INT�л�Ϊ�������� */
  
	/*��λΪ�͵�ƽ��Ϊ��ʼ����׼��*/
	GPIO_PinWrite(TOUCH_PAD_INT_GPIO, TOUCH_PAD_INT_GPIO_PIN, 0U);
	GPIO_PinWrite(TOUCH_PAD_RST_GPIO, TOUCH_PAD_RST_GPIO_PIN, 0U);

	CPU_TS_Tmr_Delay_US(200);

	/*����һ��ʱ�䣬���г�ʼ��*/
	GPIO_PinWrite(TOUCH_PAD_RST_GPIO, TOUCH_PAD_RST_GPIO_PIN, 1U);

	CPU_TS_Tmr_Delay_MS(20);

	//INT���ó��ж�����
	rst_int_config.direction = kGPIO_DigitalInput;
	rst_int_config.outputLogic = 0;
	rst_int_config.interruptMode = kGPIO_IntRisingEdge;

	GPIO_PinInit(TOUCH_PAD_INT_GPIO, TOUCH_PAD_INT_GPIO_PIN, &rst_int_config);

	/* ʹ�������ж� */  
	GPIO_PortEnableInterrupts(TOUCH_PAD_INT_GPIO, 1U << TOUCH_PAD_INT_GPIO_PIN);
}

/**
* @brief  ��ֹ����оƬ���ж�
* @param  ��
* @retval ��
*/
void GTP_IRQDisable(void)
{
	DisableIRQ(TOUCH_PAD_INT_IRQ);
}

/**
* @brief  ʹ�ܴ���оƬ���ж�
* @param  ��
* @retval ��
*/
void GTP_IRQEnable(void)
{
  gpio_pin_config_t rst_int_config;
   
  //INT���ó��ж�����
	rst_int_config.direction = kGPIO_DigitalInput;
	rst_int_config.outputLogic = 0;
	rst_int_config.interruptMode = kGPIO_IntRisingEdge;

	GPIO_PinInit(TOUCH_PAD_INT_GPIO, TOUCH_PAD_INT_GPIO_PIN, &rst_int_config);

	/* ʹ�������ж� */  
	GPIO_PortEnableInterrupts(TOUCH_PAD_INT_GPIO, 1U << TOUCH_PAD_INT_GPIO_PIN);

  /* ʹ���ж�IRQ */  
	EnableIRQ(TOUCH_PAD_INT_IRQ);
}


/**
* @brief  ��ʼ���������IOMUXC��MUX��������
* @param  ��
* @retval ��
*/
static void I2C_GTP_IOMUXC_MUX_Config(void)
{
  /* SCL��SDA���ţ���Ҫʹ��SION�Խ������� */
  IOMUXC_SetPinMux(TOUCH_PAD_SCL_IOMUXC, 1U);                                   
  IOMUXC_SetPinMux(TOUCH_PAD_SDA_IOMUXC, 1U);  
	
  /* RST��INT���� */
	IOMUXC_SetPinMux(TOUCH_PAD_RST_IOMUXC, 0U);                                   
  IOMUXC_SetPinMux(TOUCH_PAD_INT_IOMUXC, 0U);
}

/**
* @brief  ��ʼ���������IOMUXC��PAD��������
* @param  ��
* @retval ��
*/
static void I2C_GTP_IOMUXC_PAD_Config(void)
{
  /* SCL��SDA���� */
  IOMUXC_SetPinConfig(TOUCH_PAD_SCL_IOMUXC, I2C_PAD_CONFIG_DATA);                               
  IOMUXC_SetPinConfig(TOUCH_PAD_SDA_IOMUXC, I2C_PAD_CONFIG_DATA); 
  
  /* RST��INT���� */
  IOMUXC_SetPinConfig(TOUCH_PAD_RST_IOMUXC, GTP_RST_INT_PAD_CONFIG_DATA); 
  IOMUXC_SetPinConfig(TOUCH_PAD_INT_IOMUXC, GTP_RST_INT_PAD_CONFIG_DATA);	    
}


/**
* @brief  ��ʼ������оƬʹ�õ�I2C����
* @param  ��
* @retval ��
*/	
void GTP_I2C_ModeInit(void)
{
	lpi2c_master_config_t masterConfig;	

	/* ����ʱ�� LPI2C */
	CLOCK_SetMux(kCLOCK_Lpi2cMux, LPI2C_CLOCK_SOURCE_SELECT);
	CLOCK_SetDiv(kCLOCK_Lpi2cDiv, LPI2C_CLOCK_SOURCE_DIVIDER);
	
  /*��masterConfig��ֵΪ����Ĭ������*/
	/*
	 * masterConfig.debugEnable = false;
	 * masterConfig.ignoreAck = false;
	 * masterConfig.pinConfig = kLPI2C_2PinOpenDrain;
	 * masterConfig.baudRate_Hz = 100000U;
	 * masterConfig.busIdleTimeout_ns = 0;
	 * masterConfig.pinLowTimeout_ns = 0;
	 * masterConfig.sdaGlitchFilterWidth_ns = 0;
	 * masterConfig.sclGlitchFilterWidth_ns = 0;
	 */
	LPI2C_MasterGetDefaultConfig(&masterConfig);

	/* ��Ĭ�ϲ����ʸ�ΪI2C_BAUDRATE */
	masterConfig.baudRate_Hz = GTP_I2C_BAUDRATE;

	/*  ʹ���������ó�ʼ�� LPI2C ���� */
	LPI2C_MasterInit(GTP_I2C_MASTER, &masterConfig, LPI2C_MASTER_CLOCK_FREQUENCY);

	/* ���� LPI2C ����������ľ�� */
	LPI2C_MasterTransferCreateHandle(GTP_I2C_MASTER, &g_m_handle, I2C_Master_Callback, NULL);

}


/**
  * @brief   ʹ��IIC��ȡ����
  * @param   
  * 	@arg ClientAddr:���豸��ַ
  *		@arg pBuffer:����ɴӻ���ȡ�����ݵĻ�����ָ��
  *		@arg NumByteToRead:��ȡ�����ݳ���
  * @note NumByteToRead <= 256
  * @retval  ��
  */
uint32_t I2C_ReadBytes(uint8_t ClientAddr,uint8_t* pBuffer, uint16_t NumByteToRead)
{
	lpi2c_master_transfer_t masterXfer = {0};
	status_t reVal = kStatus_Fail;
  /* 400Kbps,����һ���ֽ�(9bits)����23us�����ﰴÿ�ֽ�1024us����*/
  uint32_t i2c_timeout = NumByteToRead<<10;

	/* subAddress = 0x00, data = pBuffer �Դӻ�������
		��ʼ�ź�start + �豸��ַslaveaddress(w д����) + �ӵ�ַsubAddress + 
    �ظ���ʼ�ź�repeated start + �豸��ַslaveaddress(r ������) + 
    ���ջ�������rx data buffer + ֹͣ�ź�stop */
	masterXfer.slaveAddress = (ClientAddr>>1);
	masterXfer.direction = kLPI2C_Read;
	masterXfer.subaddress = (uint32_t)0;
	masterXfer.subaddressSize = 0;
	masterXfer.data = pBuffer;
	masterXfer.dataSize = NumByteToRead;
	masterXfer.flags = kLPI2C_TransferDefaultFlag;

	reVal = LPI2C_MasterTransferNonBlocking(GTP_I2C_MASTER, &g_m_handle, &masterXfer);
	if (reVal != kStatus_Success)
	{
			return 1;
	}
	/* ��λ������ɱ�־ */
	g_MasterCompletionFlag = false;

	/* �ȴ�������� */
	while (!g_MasterCompletionFlag)
	{
    CPU_TS_Tmr_Delay_US(1);
    if((i2c_timeout--) == 0) return I2C_Timeout_Callback(0);
	}
	g_MasterCompletionFlag = false;
	
	return 0;
}

/**
  * @brief   ʹ��IICд������
  * @param   
  * 	@arg ClientAddr:���豸��ַ
  *		@arg pBuffer:������ָ��
  *     @arg NumByteToWrite:д���ֽ���
  * @retval  ��
  */
uint32_t I2C_WriteBytes(uint8_t ClientAddr,uint8_t* pBuffer,  uint8_t NumByteToWrite)
{
	lpi2c_master_transfer_t masterXfer = {0};
	status_t reVal = kStatus_Fail;
  /* 400Kbps,����һ���ֽ�(9bits)����23us�����ﰴÿ�ֽ�1024us����*/
  uint32_t i2c_timeout = NumByteToWrite<<10;


	/* subAddress = 0x00, data = pBuffer �������ӻ�
		��ʼ�ź�start + �豸��ַslaveaddress(w д����) + 
    ���ͻ�������tx data buffer + ֹͣ�ź�stop */
  
	masterXfer.slaveAddress = (ClientAddr>>1);
	masterXfer.direction = kLPI2C_Write;
	masterXfer.subaddress = (uint32_t)0;
	masterXfer.subaddressSize = 0;
	masterXfer.data = pBuffer;
	masterXfer.dataSize = NumByteToWrite;
	masterXfer.flags = kLPI2C_TransferDefaultFlag;

	reVal = LPI2C_MasterTransferNonBlocking(GTP_I2C_MASTER, &g_m_handle, &masterXfer);
	if (reVal != kStatus_Success)
	{
			return 1;
	}
	/* ��λ������ɱ�־ */
	g_MasterCompletionFlag = false;

	/* �ȴ�������� */
	while (!g_MasterCompletionFlag)
	{
    CPU_TS_Tmr_Delay_US(1);
    if((i2c_timeout--) == 0) return I2C_Timeout_Callback(1);
	}
	g_MasterCompletionFlag = false;
	
	return 0;

}

/**
  * @brief  IIC�ȴ���ʱ���ñ��������������Ϣ
  * @param  None.
  * @retval ����0xff����ʾIIC��ȡ����ʧ��
  */
static  uint32_t I2C_Timeout_Callback(uint8_t errorCode)
{
  /* Block communication and all processes */
  GTP_ERROR("I2C �ȴ���ʱ!errorCode = %d",errorCode);
  
  return 0xFF;
}

/**
* @brief  I2C���贫����ɵĻص�����
* @param  ��
* @retval ��
*/
static void I2C_Master_Callback(LPI2C_Type *base, lpi2c_master_handle_t *handle, status_t status, void *userData)
{		
    /* ���յ�kStatus_Success��־��
       ����g_MasterCompletionFlag��־��������ɹ� */
    if (status == kStatus_Success)
    {
        g_MasterCompletionFlag = true;
    }
}



/**
* @brief  �������ż�оƬ��ʼ��
* @param  ��
* @retval ��
*/
void I2C_Touch_Init(void)
{
  /* ��ʼ��������IOMUXC��� */
  I2C_GTP_IOMUXC_MUX_Config();
  I2C_GTP_IOMUXC_PAD_Config();

  /* ��ʼ��I2C���蹤��ģʽ */
  GTP_I2C_ModeInit(); 
  
  /* ��λ����оƬ�����õ�ַ */
  GTP_ResetChip();
 
  GTP_IRQDisable();

//  I2C_GTP_IRQEnable();
}


/***************************�ж����******************************/
/**
* @brief  ����оƬTOUCH_PAD_INT_GPIO_PIN���ŵ��жϷ�����
* @param  ��
* @retval ��
*/
void TOUCH_PAD_IRQHANDLER(void)
{ 
    /* ȷ���Ǵ���оƬ���ж� */
    if(GPIO_GetPinsInterruptFlags(TOUCH_PAD_INT_GPIO) & 1U << TOUCH_PAD_INT_GPIO_PIN)
    {
       /* ����жϱ�־ */
      GPIO_PortClearInterruptFlags(TOUCH_PAD_INT_GPIO, 1U << TOUCH_PAD_INT_GPIO_PIN);
      /* �л���������״̬��־ */
      g_TouchPadInputSignal = true;

     }

    /* ���²�����Ϊ ARM �Ŀ���838869��ӵ�, 
       �ô���Ӱ�� Cortex-M4, Cortex-M4F�ںˣ�       
       �����洢�����ص��쳣�����·��ز������ܻ�ָ�������ж�
        CM7����Ӱ�죬�˴������ô���
    */  
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

/*********************************************END OF FILE**********************/
