/**
  ******************************************************************************
  * @file    i2c.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   i2c �������������������I2C������ͨѶ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����  STM32 H750 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 
#include "./i2c/i2c.h"

  
I2C_HandleTypeDef I2C_Handle;					
/*******************************  Function ************************************/

/**
  * @brief  ��ʼ��I2C���ߣ�ʹ��I2Cǰ��Ҫ����
  * @param  ��
  * @retval ��
  */
void I2cMaster_Init(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* ʹ��I2Cxʱ�� */
	SENSORS_I2C_RCC_CLK_ENABLE();

	/* ʹ��I2C GPIO ʱ�� */
	SENSORS_I2C_SCL_GPIO_CLK_ENABLE();
	SENSORS_I2C_SDA_GPIO_CLK_ENABLE();

	/* ����I2Cx����: SCL ----------------------------------------*/
	GPIO_InitStructure.Pin =  SENSORS_I2C_SCL_GPIO_PIN; 
	GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pull= GPIO_NOPULL;
	GPIO_InitStructure.Alternate=SENSORS_I2C_AF; 
	HAL_GPIO_Init(SENSORS_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

	/* ����I2Cx����: SDA ----------------------------------------*/
	GPIO_InitStructure.Pin = SENSORS_I2C_SDA_GPIO_PIN;  
	HAL_GPIO_Init(SENSORS_I2C_SDA_GPIO_PORT, &GPIO_InitStructure); 
	
	if(HAL_I2C_GetState(&I2C_Handle) == HAL_I2C_STATE_RESET)
	{	
		/* ǿ�Ƹ�λI2C����ʱ�� */  
		SENSORS_I2C_FORCE_RESET(); 

		/* �ͷ�I2C����ʱ�Ӹ�λ */  
		SENSORS_I2C_RELEASE_RESET(); 
		
		/* I2C ���� */
		I2C_Handle.Instance = SENSORS_I2C;
		I2C_Handle.Init.Timing           = 0x60201E2B;//100KHz
		I2C_Handle.Init.OwnAddress1      = 0;
		I2C_Handle.Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
		I2C_Handle.Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
		I2C_Handle.Init.OwnAddress2      = 0;
		I2C_Handle.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
		I2C_Handle.Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
		I2C_Handle.Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;

		/* ��ʼ��I2C */
		HAL_I2C_Init(&I2C_Handle);	
		/* ʹ��ģ���˲��� */
		HAL_I2CEx_AnalogFilter_Config(&I2C_Handle, I2C_ANALOGFILTER_ENABLE); 
	}
}
/**
  * @brief  Manages error callback by re-initializing I2C.
  * @param  Addr: I2C Address
  * @retval None
  */
static void I2Cx_Error(uint8_t Addr)
{
	/* �ָ�I2C�Ĵ���ΪĬ��ֵ */
	HAL_I2C_DeInit(&I2C_Handle); 
	/* ���³�ʼ��I2C���� */
	I2cMaster_Init();
}
/**
  * @brief  д�Ĵ����������ṩ���ϲ�Ľӿ�
	* @param  slave_addr: �ӻ���ַ
	* @param 	reg_addr:�Ĵ�����ַ
	* @param len��д��ĳ���
	*	@param data_ptr:ָ��Ҫд�������
  * @retval ����Ϊ0��������Ϊ��0
  */
int Sensors_I2C_WriteRegister(unsigned char slave_addr,
                                        unsigned char reg_addr,
                                        unsigned short len, 
                                        unsigned char *data_ptr)
{
	HAL_StatusTypeDef status = HAL_OK;
	status = HAL_I2C_Mem_Write(&I2C_Handle, slave_addr, reg_addr, I2C_MEMADD_SIZE_8BIT,data_ptr, len,I2Cx_FLAG_TIMEOUT); 
	/* ���ͨѶ״̬ */
	if(status != HAL_OK)
	{
		/* ���߳����� */
		I2Cx_Error(slave_addr);
	}
	while (HAL_I2C_GetState(&I2C_Handle) != HAL_I2C_STATE_READY)
	{
		
	}
	/* ���SENSOR�Ƿ����������һ�ζ�д���� */
	while (HAL_I2C_IsDeviceReady(&I2C_Handle, slave_addr, I2Cx_FLAG_TIMEOUT, I2Cx_FLAG_TIMEOUT) == HAL_TIMEOUT);
	/* �ȴ�������� */
	while (HAL_I2C_GetState(&I2C_Handle) != HAL_I2C_STATE_READY)
	{
		
	}
	return status;
}

/**
  * @brief  ���Ĵ����������ṩ���ϲ�Ľӿ�
	* @param  slave_addr: �ӻ���ַ
	* @param 	reg_addr:�Ĵ�����ַ
	* @param len��Ҫ��ȡ�ĳ���
	*	@param data_ptr:ָ��Ҫ�洢���ݵ�ָ��
  * @retval ����Ϊ0��������Ϊ��0
  */
int Sensors_I2C_ReadRegister(unsigned char slave_addr,
                                       unsigned char reg_addr,
                                       unsigned short len, 
                                       unsigned char *data_ptr)
{
	HAL_StatusTypeDef status = HAL_OK;
	status =HAL_I2C_Mem_Read(&I2C_Handle,slave_addr,reg_addr,I2C_MEMADD_SIZE_8BIT,data_ptr,len,I2Cx_FLAG_TIMEOUT);    
	/* ���ͨѶ״̬ */
	if(status != HAL_OK)
	{
		/* ���߳����� */
		I2Cx_Error(slave_addr);
	}
	while (HAL_I2C_GetState(&I2C_Handle) != HAL_I2C_STATE_READY)
	{
		
	}
	/* ���SENSOR�Ƿ����������һ�ζ�д���� */
	while (HAL_I2C_IsDeviceReady(&I2C_Handle, slave_addr, I2Cx_FLAG_TIMEOUT, I2Cx_FLAG_TIMEOUT) == HAL_TIMEOUT);
	/* �ȴ�������� */
	while (HAL_I2C_GetState(&I2C_Handle) != HAL_I2C_STATE_READY)
	{
		
	}
	return status;
}

/**
  * @brief  ��ȡ��ǰ����ֵ
  * @param  �洢���º���ֵ�ı���
  * @retval ��
  */
int get_tick_count(unsigned long *count)
{
   count[0] = HAL_GetTick();
	return 0;
}
