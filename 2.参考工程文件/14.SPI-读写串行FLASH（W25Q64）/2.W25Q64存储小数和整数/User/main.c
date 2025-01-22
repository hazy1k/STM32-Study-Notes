#include "led.h"
#include "spi_flash.h"
#include "usart.h"

__IO uint32_t DeviceID = 0;
__IO uint32_t FlashID = 0;

void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}

int main(void)
{
	uint8_t flag = 0;
	uint8_t k;
    long double doule_buffer[7] = {0}; // С�����黺����
	int long_buffer[7] = {0}; // ���������黺����
	LED_Init();
	LED_BLUE();
	SPI_FLASH_Init();
	USART_Config();
	printf("SPI FLASH Test Program\r\n");
	DeviceID = SPI_FLASH_ReadDeviceID(); // ��ȡ�豸ID
	Delay(200);
	FlashID = SPI_FLASH_ReadID(); // ��ȡFlash ID
	printf("Device ID: 0x%X\r\n", DeviceID);
	printf("Flash ID: 0x%X\r\n", FlashID);
	if(FlashID == sFLASH_ID)
	{
		LED_GREEN();
		printf("��⵽W25Q64\r\n");
		SPI_FLASH_BufferRead(&flag, SPI_FLASH_PageSize*0, 1);
		if(flag == 0xCD) // ������ݱ�־
		{
			printf("֮ǰ�Ѿ�д������\r\n");
			// ��ȡС������
			SPI_FLASH_BufferRead((void*)doule_buffer, SPI_FLASH_PageSize*1, sizeof(doule_buffer));
			// ��ȡ����������
			SPI_FLASH_BufferRead((void*)long_buffer, SPI_FLASH_PageSize*2, sizeof(long_buffer));
			printf("\r\n ��оƬ����������Ϊ: \r\n");
			for(k = 0; k < 7; k++)
			{
				printf("С�� = %LF \r\n", doule_buffer[k]);
				printf("������ = %d \r\n", long_buffer[k]);
			}
		}
		else // δд������
		{
			printf("δд������, ׼��д������\r\n");
			flag = 0xCD; // ���ݱ�־
			SPI_FLASH_SectorErase(0); // ������0ҳ
			SPI_FLASH_BufferWrite(&flag, SPI_FLASH_PageSize*0, 1); // д�����ݱ�־
			for(k = 0; k < 7; k++)
			{
				doule_buffer[k] = k +0.01; // д��С������
				long_buffer[k] = k + 1000000000; // д�볤��������
			}
			SPI_FLASH_BufferWrite((void*)doule_buffer, SPI_FLASH_PageSize*1, sizeof(doule_buffer));
			SPI_FLASH_BufferWrite((void*)long_buffer, SPI_FLASH_PageSize*2, sizeof(long_buffer));
			printf("��оƬд�������Ϊ: \r\n");
			for(k = 0; k < 7; k++)
			{
				printf("С�� = %LF \r\n", doule_buffer[k]);
				printf("������ = %d \r\n", long_buffer[k]);
			}
			printf("\r\n ��λ�����壬��������Ч�� \r\n");
		}
	}
	else
	{
		LED_RED();
		printf("δ��⵽W25Q64\r\n");
	}
	while(1)
	{}
}
