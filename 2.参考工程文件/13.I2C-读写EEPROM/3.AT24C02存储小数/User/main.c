#include "stm32f10x.h"
#include "usart.h"
#include "I2C.h"
#include "led.h"
#include <string.h>

#define double_addr 10 // С���洢��ַ
#define long_addr 70 // �������洢��ַ

uint8_t flag = 0;
uint8_t i;
long double double_buf[7] = {0};
int long_buf[7] = {0};

int main(void)
{
	LED_Init();
	USART_Config();
	I2C_EE_Init();
	printf("����һ��EEPROM��дС���ͳ������ĳ���\r\n");
	I2C_EE_BufferRead(&flag, 0, 1); // ��ȡ��־λ
	if(flag != 0xEE) // ����б�־λ0xCD����ʾ֮ǰ�Ѿ�д�������
	{
		printf("û���ҵ�֮ǰд������ݣ���ʼд������\r\n");
		flag = 0xEE; // д���־λ
		I2C_EE_BufferWrite(&flag, 0, 1); // д���־λ
		for(i = 0; i < 7; i++)
		{
			double_buf[i] = i+0.1; // д��С������
			long_buf[i] = i*100+1; // д�볤��������
		}
		// д������
		I2C_EE_BufferWrite((void*)double_buf, double_addr, sizeof(double_buf));
		I2C_EE_BufferWrite((void*)long_buf, long_addr, sizeof(long_buf));
		printf("������EEPROMд�������Ϊ��\r\n");
		for(i = 0; i < 7; i++)
		{
			printf("С�� = %LF\r\n", double_buf[i]);
			printf("������ = %d\r\n", long_buf[i]);
		}
		printf("д�����ݳɹ�����λ�����壬��������Ч��\r\n");
	}
	else // ����б�־λ0xCD����ʾ֮ǰ�Ѿ�д�������
	{
		printf("�ҵ�֮ǰд������ݣ���ʼ��ȡ����\r\n");
		// ��ȡ����
		I2C_EE_BufferRead((void*)double_buf, double_addr, sizeof(double_buf));
		I2C_EE_BufferRead((void*)long_buf, long_addr, sizeof(long_buf));
		printf("���Ƕ�ȡ��EEPROM����Ϊ��\r\n");
		for(i = 0; i < 7; i++)
		{
			printf("С�� = %LF\r\n", double_buf[i]);
			printf("������ = %d\r\n", long_buf[i]);
		}
	}
	while(1)
	{
	}
}
