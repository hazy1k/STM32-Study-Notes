#include "i2c_gpio.h"

void i2c_gpio_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN | I2C_SDA_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	i2c_stop();
}

// ģ��I2C����λ�ӳ٣����400KHz
static void i2c_delay(void)
{
	uint8_t i;
	for(i = 0; i < 10; i++);
}

// MCU����I2C���������ź�
void i2c_start(void)
{
	// ��SCL�ߵ�ƽʱ��SDA����һ���½��ر�ʾI2C���������ź�
	I2C_SCL_1();
	I2C_SDA_1();
	i2c_delay();
	I2C_SDA_0(); // ����SDA������һ�������ź�
	i2c_delay();
	I2C_SCL_0(); // ����SCL������һ��ʱ��
	i2c_delay();
}

// MCU����I2C����ֹͣ�ź�
void i2c_stop(void)
{
	// ��SCL�ߵ�ƽ��SDA�͵�ƽʱ��SDA����һ�������ر�ʾI2C����ֹͣ�ź�
	I2C_SCL_1();
	I2C_SDA_0();
	i2c_delay();
	I2C_SDA_1(); // ����SDA������һ��ֹͣ�ź�
}

// MCU��I2C�����豸����8bit����
void i2c_SendByte(uint8_t Byte)
{
	uint8_t i;
	for(i = 0; i < 8; i++ ) // �ȷ��͸�7λ
	{
		if(Byte & 0x80) // �����λΪ1����SDAΪ�ߵ�ƽ
		{
			I2C_SDA_1();
		}
		else
		{
			I2C_SDA_0();
		}
		i2c_delay();
		I2C_SCL_1();
		i2c_delay();
		I2C_SCL_0();
		if(i == 7)
		{
			I2C_SDA_1(); // �ͷ�����
		}
		Byte <<= 1; // ����һλ
		i2c_delay();
	}
}

// MCU��I2C�����豸����8bit����
uint8_t i2c_ReadByte(void)
{
	uint8_t i, value = 0;
	for(i = 0; i < 8; i++)
	{
		value <<= 1; // ����һλ
		I2C_SCL_1();
		i2c_delay();
		if (I2C_SDA_READ())
		{
			value++;
		}
		I2C_SCL_0();
		i2c_delay();
	}
	return value;
}

// �ȴ�MCU����һ��ʱ�ӣ���ȡ������ACKӦ���ź�
uint8_t i2c_WaitAck(void)
{
	uint16_t STA;
	I2C_SDA_1(); 
	i2c_delay();
	I2C_SCL_1();
	i2c_delay();
	if(I2C_SDA_READ())
	{
		STA = 1;
	}
	else
	{
		STA = 0;
	}
	I2C_SCL_0();
	i2c_delay();
	return STA;
}

// MCU����һ��ACK�ź�
void i2c_Ack(void)
{
	I2C_SDA_0();
	i2c_delay();
	I2C_SCL_1();
	i2c_delay();
	I2C_SCL_0();
	i2c_delay();
	I2C_SDA_1();
}

// MCU����һ��NACK�ź�
void i2c_Nack(void)
{
	I2C_SDA_1();
	i2c_delay();
	I2C_SCL_1();
	i2c_delay();
	I2C_SCL_0();
	i2c_delay();
}

// ���I2C�����豸��MCU�����豸��ַ����ȡ�豸Ӧ��״���ж��豸�Ƿ����
uint8_t i2c_CheckDevice(uint8_t Device_Address)
{
	uint8_t ucAck;
	i2c_gpio_Init();
	i2c_start();
	i2c_SendByte(Device_Address | I2C_WR); // �����豸��ַ+дλ
	ucAck = i2c_WaitAck(); // �ȴ�ACKӦ��
	i2c_stop();
	return ucAck;
}
