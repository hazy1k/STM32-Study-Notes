#ifndef _BSP_I2C_EE_GPIO_H
#define _BSP_I2C_EE_GPIO_H

#include <inttypes.h>

#define macI2C_EE_WR	0		/* д����bit */
#define macI2C_EE_RD	1		/* ������bit */

/* ����I2C�������ӵ�GPIO�˿�, �û�ֻ��Ҫ�޸�����4�д��뼴������ı�SCL��SDA������ */
#define macGPIO_PORT_I2C_EE	 GPIOB			/* GPIO�˿� */
#define macRCC_I2C_EE_PORT 	 RCC_APB2Periph_GPIOB		/* GPIO�˿�ʱ�� */
#define macI2C_EE_SCL_PIN		 GPIO_Pin_6			/* ���ӵ�SCLʱ���ߵ�GPIO */
#define macI2C_EE_SDA_PIN		 GPIO_Pin_7			/* ���ӵ�SDA�����ߵ�GPIO */

/* �����дSCL��SDA�ĺ꣬�����Ӵ���Ŀ���ֲ�ԺͿ��Ķ��� */
#if 0	/* �������룺 1 ѡ��GPIO�Ŀ⺯��ʵ��IO��д */
	#define macI2C_EE_SCL_1()  GPIO_SetBits(macGPIO_PORT_I2C_EE, macI2C_EE_SCL_PIN)		/* SCL = 1 */
	#define macI2C_EE_SCL_0()  GPIO_ResetBits(macGPIO_PORT_I2C_EE, macI2C_EE_SCL_PIN)		/* SCL = 0 */
	
	#define macI2C_EE_SDA_1()  GPIO_SetBits(macGPIO_PORT_I2C_EE, macI2C_EE_SDA_PIN)		/* SDA = 1 */
	#define macI2C_EE_SDA_0()  GPIO_ResetBits(macGPIO_PORT_I2C_EE, macI2C_EE_SDA_PIN)		/* SDA = 0 */
	
	#define macI2C_EE_SDA_READ()  GPIO_ReadInputDataBit(macGPIO_PORT_I2C_EE, macI2C_EE_SDA_PIN)	/* ��SDA����״̬ */
#else	/* �����֧ѡ��ֱ�ӼĴ�������ʵ��IO��д */
    /*��ע�⣺����д������IAR��߼����Ż�ʱ���ᱻ�����������Ż� */
	#define macI2C_EE_SCL_1()  macGPIO_PORT_I2C_EE->BSRR = macI2C_EE_SCL_PIN				/* SCL = 1 */
	#define macI2C_EE_SCL_0()  macGPIO_PORT_I2C_EE->BRR = macI2C_EE_SCL_PIN				/* SCL = 0 */
	
	#define macI2C_EE_SDA_1()  macGPIO_PORT_I2C_EE->BSRR = macI2C_EE_SDA_PIN				/* SDA = 1 */
	#define macI2C_EE_SDA_0()  macGPIO_PORT_I2C_EE->BRR = macI2C_EE_SDA_PIN				/* SDA = 0 */
	
	#define macI2C_EE_SDA_READ()  ((macGPIO_PORT_I2C_EE->IDR & macI2C_EE_SDA_PIN) != 0)	/* ��SDA����״̬ */
#endif

void i2c_ee_Start(void);
void i2c_ee_Stop(void);
void i2c_ee_SendByte(uint8_t _ucByte);
uint8_t i2c_ee_ReadByte(void);
uint8_t i2c_ee_WaitAck(void);
void i2c_ee_Ack(void);
void i2c_ee_NAck(void);
uint8_t i2c_ee_CheckDevice(uint8_t _Address);

#endif //_BSP_I2C_EE_GPIO_H
