#include "stm32f10x.h"
#include "led.h"
#include "spi_flash.h"
#include "usart.h"

// ��������ñ���
typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;

// ��ȡ�������ĳ���
#define countof(a) (sizeof(a)/sizeof(*(a)))
#define TxBufferSize1 (countof(TxBuffer1)-1)
#define RxBufferSize1 (countof(RxBuffer1)-1)
#define BufferSize (countof(Tx_Buffer))

// ����SPI FLASH ��ز���
#define FLASH_WriteAddress 0x00000 // д��ַ
#define FLASH_ReadAddress FLASH_WriteAddress   // ����ַ
#define FLASH_SectorToErase FLASH_WriteAddress // ����������ַ

// ���ͻ�������ʼ��
uint8_t Tx_Buffer[] = "Hello";
uint8_t Rx_Buffer[BufferSize];

__IO uint32_t DeviceID = 0; // �豸ID
__IO uint32_t FlashID = 0; // FLASH ID
__IO TestStatus TransferStatus1 = FAILED; // ���Խ��

// �Ƚ����������������Ƿ����
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
	while(BufferLength--)
	{
		if(*pBuffer1!=*pBuffer2)
		{
			return FAILED;
		}
		pBuffer1++;
		pBuffer2++;
	}
	return PASSED;
}

void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}

int main(void)
{
	LED_Init();
	LED_BLUE();
	USART_Config();
	SPI_FLASH_Init();
	DeviceID = SPI_FLASH_ReadDeviceID(); // ��ȡ�豸ID
	Delay(200);
	FlashID = SPI_FLASH_ReadID(); // ��ȡFLASH ID
	Delay(200);
	printf("DeviceID = 0x%08X\r\n", DeviceID);
	printf("FlashID = 0x%08X\r\n", FlashID);
	if(FlashID == sFLASH_ID)
	{
		printf("\r\n ��⵽flash W25Q64 \r\n");
		SPI_FLASH_SectorErase(FLASH_SectorToErase); // ��������
		SPI_FLASH_BufferWrite(Tx_Buffer, FLASH_WriteAddress, BufferSize); // д����
		printf("\r\n д�������Ϊ��%s \r\n", Tx_Buffer);
		Delay(200);
		SPI_FLASH_BufferRead(Rx_Buffer, FLASH_ReadAddress, BufferSize); // ������
		printf("\r\n ����������Ϊ��%s \r\n", Rx_Buffer);
		Delay(200);
		TransferStatus1 = Buffercmp(Tx_Buffer, Rx_Buffer, BufferSize); // �Ƚ϶�д�����Ƿ�һ��
		if(TransferStatus1 == PASSED)
		{
			LED_GREEN();
			printf("\r\n ��д���Գɹ� \r\n");
		}
		else
		{
			LED_RED();
			printf("\r\n ��д����ʧ�� \r\n");
		}
	}
	else
	{
		LED_YELLOW();
		printf("\r\n δ��⵽flash W25Q64 \r\n");
	}
	while(1)
	{
	}
}
