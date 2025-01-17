#include "I2C.h"
#include "i2c_gpio.h"
#include "usart.h"

// �жϴ���EERPOM�Ƿ�����
uint8_t EEPROM_CheckOk(void)
{
	if(i2c_CheckDevice(EEPROM_ADDR) == 0)
	{
		return 1;
	}
	else
	{
		i2c_stop(); // ʧ�ܺ��мǷ���I2C����ֹͣ�ź�		
		return 0;
	}
}

// �Ӵ���EEPROMָ����ַ����ʼ��ȡ��������
uint8_t EEPROM_ReadBytes(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize) // ������������������ָ�룬��ʼ��ַ�����ݳ���
{
	uint16_t i;
	i2c_start(); // ��1��������I2C���������ź�
	// ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	i2c_SendByte(EEPROM_ADDR | I2C_WR);	// �˴���дָ��
	if (i2c_WaitAck() != 0) // ��3�����ȴ�ACK
	{
		goto cmd_fail;	// EEPROM������Ӧ��
	}
	// ��4���������ֽڵ�ַ��24C02ֻ��256�ֽڣ����1���ֽھ͹��ˣ������24C04���ϣ���ô�˴���Ҫ���������ַ 
	i2c_SendByte((uint8_t)_usAddress);
	if (i2c_WaitAck() != 0) // ��5�����ȴ�ACK
	{
		goto cmd_fail;
	}
	// ��6������������I2C���ߡ�ǰ��Ĵ����Ŀ����EEPROM���͵�ַ�����濪ʼ��ȡ����
	i2c_start();
	// ��7������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ��
	i2c_SendByte(EEPROM_ADDR | I2C_RD);
	if (i2c_WaitAck() != 0) // ��8�����ȴ�ACK
	{
		goto cmd_fail;
	}	
	// ��9����ѭ����ȡ����
	for (i = 0; i < _usSize; i++)
	{
		_pReadBuf[i] = i2c_ReadByte(); // ��1���ֽ�
		// ÿ����1���ֽں���Ҫ����Ack�� ���һ���ֽڲ���ҪAck����Nack
		if (i != _usSize - 1)
		{
			i2c_Ack(); // �м��ֽڶ����CPU����ACK�ź�(����SDA = 0) 
		}
		else
		{
			i2c_Nack();	// ���1���ֽڶ����CPU����NACK�ź�(����SDA = 1)
		}
	}
	i2c_stop();
	return 1;

cmd_fail: // ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 
	i2c_stop();
	return 0;
}

// ����EEPROMָ����ַд���������ݣ�����ҳд�������д��Ч��
uint8_t EEPROM_WriteBytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize) // ����������д������ָ�룬��ʼ��ַ�����ݳ���
{
	uint16_t i,m;
	uint16_t usAddr;
	/* 
		д����EEPROM�������������������ȡ�ܶ��ֽڣ�ÿ��д����ֻ����ͬһ��page��
		����24xx02��page size = 8
		�򵥵Ĵ�����Ϊ�����ֽ�д����ģʽ��ÿд1���ֽڣ������͵�ַ
		Ϊ���������д��Ч��: ����������page wirte������
	*/
	usAddr = _usAddress;	
	for (i = 0; i < _usSize; i++)
	{
		// �����͵�1���ֽڻ���ҳ���׵�ַʱ����Ҫ���·��������źź͵�ַ 
		if ((i == 0) || (usAddr & (EEPROM_PAGE_SIZE - 1)) == 0)
		{
			i2c_stop();	
			/* ͨ���������Ӧ��ķ�ʽ���ж��ڲ�д�����Ƿ����, һ��С�� 10ms 			
				CLKƵ��Ϊ200KHzʱ����ѯ����Ϊ30������
			*/
			for (m = 0; m < 1000; m++)
			{				
				i2c_start();
				i2c_SendByte(EEPROM_ADDR | I2C_WR);	
				// ����һ��ʱ�ӣ��ж������Ƿ���ȷӦ��
				if (i2c_WaitAck() == 0)
				{
					break;
				}
			}
			if (m  == 1000)
			{
				goto cmd_fail; // EEPROM����д��ʱ 
			}
			i2c_SendByte((uint8_t)usAddr);
			if (i2c_WaitAck() != 0)
			{
				goto cmd_fail;	// EEPROM������Ӧ��
			}
		}
		i2c_SendByte(_pWriteBuf[i]);
		if (i2c_WaitAck() != 0)
		{
			goto cmd_fail;
		}
		usAddr++;			
	}
	i2c_stop();
	return 1;

cmd_fail: // ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸
	i2c_stop();
	return 0;
}

void EEPROM_Erase(void) // EEPROM��������
{
	uint16_t i;
	uint8_t buf[EEPROM_SIZE];
	for (i = 0; i < EEPROM_SIZE; i++)
	{
		buf[i] = 0xFF;
	}
	// дEEPROM, ��ʼ��ַ = 0�����ݳ���Ϊ 256
	if(EEPROM_WriteBytes(buf, 0, EEPROM_SIZE) == 0)
	{
		printf("����eeprom����\r\n");
		return;
	}
	else
	{
		printf("����eeprom�ɹ���\r\n");
	}
}
