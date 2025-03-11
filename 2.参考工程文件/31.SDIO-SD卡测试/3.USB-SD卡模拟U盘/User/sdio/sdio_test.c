#include "sdio_test.h"
#include "led.h"
#include "sdio_sdcard.h"
#include "usart.h"

/* Private typedef */
typedef enum {FAILED = 0, PASSED = !FAILED}TestStatus; // ����״̬ö������

/* Private define */
#define BLOCK_SIZE 512 // ���С
#define NUMBER_OF_BLOCKS 10 // ������
#define MULTI_BUFFER_SIZE (BLOCK_SIZE * NUMBER_OF_BLOCKS) // ����д��������С

/* Private variables */
uint8_t Buffer_Block_Tx[BLOCK_SIZE], Buffer_Block_Rx[BLOCK_SIZE];
uint8_t Buffer_MultiBlock_Tx[MULTI_BUFFER_SIZE], Buffer_MultiBlock_Rx[MULTI_BUFFER_SIZE];
volatile TestStatus EraseStatus = FAILED, TransferStatus1 = FAILED, TransferStatus2 = FAILED;
SD_Error Status = SD_OK;

/* Private function prototypes */
static void SD_EraseTest(void);
static void SD_SingleBlockTest(void);
void SD_MultiBlockTest(void);
static void Fill_Buffer(uint8_t *pBuffer, uint32_t BufferLength, uint32_t Offset);
static TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength);
static TestStatus eBuffercmp(uint8_t* pBuffer, uint32_t BufferLength);

/* Private functions */
void SD_Test(void)
{
  LED_BLUE();
  if((Status = SD_Init()) != SD_OK)
  {
    LED_RED();
    printf("SD init error\r\n");
  }
  else
  {
    printf("SD init success\r\n");
  }
  if(Status == SD_OK)
  {
    LED_BLUE();
    SD_EraseTest(); // ������������
    LED_BLUE();
    SD_SingleBlockTest(); // �����д����
    LED_BLUE();
    SD_MultiBlockTest(); // ����д����
  }
}

// ������������
void SD_EraseTest(void)
{
  if(Status == SD_OK)
  {
    Status = SD_Erase(0x00, (BLOCK_SIZE * NUMBER_OF_BLOCKS)); // ��������
  }
  if(Status == SD_OK)
  {
    Status = SD_ReadMultiBlocks(Buffer_MultiBlock_Rx, 0x00, BLOCK_SIZE, NUMBER_OF_BLOCKS); // ��ȡ���������������
    Status = SD_WaitReadOperation(); // �ȴ���ȡ�������
    while(SD_GetStatus() != SD_TRANSFER_OK); // �ȴ��������
  }
  if(Status == SD_OK)
  {
    EraseStatus = eBuffercmp(Buffer_MultiBlock_Rx, MULTI_BUFFER_SIZE);
  }
  if(EraseStatus == PASSED)
  {
    LED_GREEN();
    printf("SD erase test success\r\n");
  }
  else
  {
    LED_RED();
    printf("SD erase test failed\r\n");
  }
}

// �����д����
void SD_SingleBlockTest(void)
{
  Fill_Buffer(Buffer_Block_Tx, BLOCK_SIZE, 0x320F);
  if(Status == SD_OK)
  {
    Status = SD_WriteBlock(Buffer_Block_Tx, 0x00, BLOCK_SIZE); // д�뵥������
    Status = SD_WaitWriteOperation(); // �ȴ�д��������
    while(SD_GetStatus() != SD_TRANSFER_OK); // �ȴ��������
  }
  if(Status == SD_OK)
  {
    Status = SD_ReadBlock(Buffer_Block_Rx, 0x00, BLOCK_SIZE); // ��ȡ��������
    Status = SD_WaitReadOperation(); // �ȴ���ȡ�������
    while(SD_GetStatus() != SD_TRANSFER_OK); // �ȴ��������
  }
  if(Status == SD_OK)
  {
    TransferStatus1 = Buffercmp(Buffer_Block_Tx, Buffer_Block_Rx, BLOCK_SIZE); // �Ƚ϶�д����
  }
  if(TransferStatus1 == PASSED)
  {
    LED_GREEN();
    printf("SD single block test success\r\n");
  }
  else
  {
    LED_RED();
    printf("SD single block test failed\r\n");
  }
}

// ����д����
void SD_MultiBlockTest(void)
{
  Fill_Buffer(Buffer_MultiBlock_Tx, MULTI_BUFFER_SIZE, 0x00);
  if(Status == SD_OK)
  {
    Status = SD_WriteMultiBlocks(Buffer_MultiBlock_Tx, 0x00, BLOCK_SIZE, NUMBER_OF_BLOCKS); // д��������
    Status = SD_WaitWriteOperation(); // �ȴ�д��������
    while(SD_GetStatus() != SD_TRANSFER_OK); // �ȴ��������
  }
  if(Status == SD_OK)
  {
    Status = SD_ReadMultiBlocks(Buffer_MultiBlock_Rx, 0x00, BLOCK_SIZE, NUMBER_OF_BLOCKS); // ��ȡ�������
    Status = SD_WaitReadOperation(); // �ȴ���ȡ�������
    while(SD_GetStatus() != SD_TRANSFER_OK); // �ȴ��������
  }
  if(Status == SD_OK)
  {
    TransferStatus2 = Buffercmp(Buffer_MultiBlock_Tx, Buffer_MultiBlock_Rx, MULTI_BUFFER_SIZE); // �Ƚ϶�д����
  }
  if(TransferStatus2 == PASSED)
  {
    LED_GREEN();
    printf("SD multi block test success\r\n");
  }
  else
  {
    LED_RED();
    printf("SD multi block test failed\r\n");
  }
}

// �Ƚ������������Ƿ���ͬ
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }
    pBuffer1++;
    pBuffer2++;
  }
  return PASSED;
}

void Fill_Buffer(uint8_t *pBuffer, uint32_t BufferLength, uint32_t Offset)
{
  uint16_t index = 0;
  for(index = 0; index < BufferLength; index++)
  {
    pBuffer[index] = (uint8_t)(index + Offset);
  }
}

TestStatus eBuffercmp(uint8_t* pBuffer, uint32_t BufferLength)
{
  while(BufferLength--)
  {
    if((*pBuffer != 0xFF)&&(pBuffer != 0x00))
    {
      return FAILED;
    }
    pBuffer++;
  }
  return PASSED;
}
