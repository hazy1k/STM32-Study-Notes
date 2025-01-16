#include "bsp_i2c_gpio.h"
#include <stm32f10x.h>

static void i2c_CfgGpio(void); // ��ʼ��ģ��I2C����

// ģ��I2C����λ�ӳ٣����400KHz
static void i2c_Delay(void)
{
	uint8_t i;
	/*��
	 	�����ʱ����ͨ���߼������ǲ��Եõ��ġ�
    	����������CPU��Ƶ72MHz ��MDK���뻷����1���Ż�
  
		ѭ������Ϊ10ʱ��SCLƵ�� = 205KHz 
		ѭ������Ϊ7ʱ��SCLƵ�� = 347KHz�� SCL�ߵ�ƽʱ��1.5us��SCL�͵�ƽʱ��2.87us 
	 	ѭ������Ϊ5ʱ��SCLƵ�� = 421KHz�� SCL�ߵ�ƽʱ��1.25us��SCL�͵�ƽʱ��2.375us 
	*/
	for (i = 0; i < 10; i++);
}

// CPU����I2C���������ź�
void i2c_Start(void)
{
	/* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C���������ź� */
	EEPROM_I2C_SDA_1(); // ����SDA
	EEPROM_I2C_SCL_1(); // ����SCL
	i2c_Delay();
	EEPROM_I2C_SDA_0(); // ����SDA�����������ź�
	i2c_Delay();
	EEPROM_I2C_SCL_0(); // ����SCL������һ��ʱ��
	i2c_Delay();
}

// CPU����I2C����ֹͣ�ź�
void i2c_Stop(void)
{
	// ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C����ֹͣ�ź�
	EEPROM_I2C_SDA_0(); // ����SDA
	EEPROM_I2C_SCL_1(); // ����SCL
	i2c_Delay();
	EEPROM_I2C_SDA_1(); // ����SDA������ֹͣ�ź�
}

// CPU��I2C�����豸����8bit����
void i2c_SendByte(uint8_t _ucByte) // ����������Ҫ���͵�8bit����
{
	uint8_t i;

	/* �ȷ����ֽڵĸ�λbit7 */
	for (i = 0; i < 8; i++)
	{		
		if (_ucByte & 0x80)
		{
			EEPROM_I2C_SDA_1(); // ���͸ߵ�ƽ
		}
		else
		{
			EEPROM_I2C_SDA_0(); // ���͵͵�ƽ
		}
		i2c_Delay();
		EEPROM_I2C_SCL_1(); // ����SCL
		i2c_Delay();	
		EEPROM_I2C_SCL_0(); // ����SCL������һ��ʱ��
		if (i == 7)
		{
			 EEPROM_I2C_SDA_1(); // �ͷ�����
		}
		_ucByte <<= 1;	// ����һ��bit
		i2c_Delay();
	}
}

// CPU��I2C�����豸��ȡ8bit����
uint8_t i2c_ReadByte(void)
{
	uint8_t i; // Ҫ��ȡ��bitλ��
	uint8_t value; // ��ȡ����8bit����

	// ������1��bitΪ���ݵ�bit7 
	value = 0;
	for(i = 0; i < 8; i++)
	{
		value <<= 1;
		EEPROM_I2C_SCL_1();
		i2c_Delay();
		if (EEPROM_I2C_SDA_READ())
		{
			value++;
		}
		EEPROM_I2C_SCL_0();
		i2c_Delay();
	}
	return value;
}

// CPU����һ��ʱ�ӣ�����ȡ������ACKӦ���ź�
uint8_t i2c_WaitAck(void)
{
	uint8_t re;

	EEPROM_I2C_SDA_1();	// CPU�ͷ�SDA����
	i2c_Delay();
	EEPROM_I2C_SCL_1();	/* CPU����SCL = 1, ��ʱ�����᷵��ACKӦ�� */
	i2c_Delay();
	if (EEPROM_I2C_SDA_READ()) // CPU��ȡSDA����״̬
	{
		re = 1; // ��������
	}
	else
	{
		re = 0; // �����쳣
	}
	EEPROM_I2C_SCL_0(); // ����SCL������һ��ʱ��
	i2c_Delay();
	return re; // ����ACKӦ����
}

// CPU����һ��ACK�ź�
void i2c_Ack(void)
{
	EEPROM_I2C_SDA_0();	/* CPU����SDA = 0 */
	i2c_Delay();
	EEPROM_I2C_SCL_1();	/* CPU����1��ʱ�� */
	i2c_Delay();
	EEPROM_I2C_SCL_0();
	i2c_Delay();
	EEPROM_I2C_SDA_1();	/* CPU�ͷ�SDA���� */
}

// CPU����1��NACK�ź�
void i2c_NAck(void)
{
	EEPROM_I2C_SDA_1();	/* CPU����SDA = 1 */
	i2c_Delay();
	EEPROM_I2C_SCL_1();	/* CPU����1��ʱ�� */
	i2c_Delay();
	EEPROM_I2C_SCL_0();
	i2c_Delay();	
}

// ����I2C���ߵ�GPIO������ģ��IO�ķ�ʽʵ��
static void i2c_CfgGpio(void)
{
	// 1.��ͷ��һ�����ȶ���һ���ṹ��
	GPIO_InitTypeDef GPIO_InitStructure;

	// 2.ʹ��GPIOʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	// 3.����GPIO����
	GPIO_InitStructure.GPIO_Pin = EEPROM_I2C_SCL_PIN | EEPROM_I2C_SDA_PIN;

	// 4.����GPIOΪ��©���ģʽ���ٶ�50MHz
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  	/* ��©��� */

	// 5.��ʼ��GPIO
	GPIO_Init(EEPROM_GPIO_PORT_I2C, &GPIO_InitStructure);

	// 6.��һ��ֹͣ�źţ�Ŀ���ǻָ�����ʼ״̬
	i2c_Stop();
}

// ���I2C�����豸��CPU�����豸��ַ��Ȼ���ȡ�豸Ӧ�����жϸ��豸�Ƿ����
uint8_t i2c_CheckDevice(uint8_t _Address) // ����������Ҫ�����豸��ַ
{
	uint8_t ucAck; // �豸Ӧ����

	i2c_CfgGpio(); // GPIO��ʼ��
	i2c_Start();   // ���������ź�

	/* �����豸��ַ+��д����bit��0 = w�� 1 = r) bit7 �ȴ� */
	i2c_SendByte(_Address | EEPROM_I2C_WR);

	ucAck = i2c_WaitAck(); // ����豸Ӧ��

	i2c_Stop(); // ������ɣ�����ֹͣ�ź�

	return ucAck; // �����豸Ӧ����
}