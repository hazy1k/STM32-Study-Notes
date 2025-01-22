#include "spi_flash.h"

static __IO uint32_t SPITimeout = SPIT_FLAG_TIMEOUT; // ��ʱʱ���ʼ��

// SPI_FLASH�ӿڳ�ʼ��
void SPI_FLASH_Init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  // ʹ��ʱ��
  FLASH_SPI_APBxClock_FUN(FLASH_SPI_CLK, ENABLE); // ʹ��SPIʱ��
  FLASH_SPI_CS_APBxClock_FUN(FLASH_SPI_CS_CLK|FLASH_SPI_SCK_CLK|FLASH_SPI_MOSI_CLK|FLASH_SPI_MISO_CLK, ENABLE); // ʹ��SPI�Ľӿ�����ʱ��
  // ����GPIO
  GPIO_InitStructure.GPIO_Pin = FLASH_SPI_CS_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(FLASH_SPI_CS_PORT, &GPIO_InitStructure);  // ��ʼ��CS����
  GPIO_InitStructure.GPIO_Pin = FLASH_SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(FLASH_SPI_SCK_PORT, &GPIO_InitStructure); // ��ʼ��SCK����
  GPIO_InitStructure.GPIO_Pin = FLASH_SPI_MOSI_PIN;
  GPIO_Init(FLASH_SPI_MOSI_PORT, &GPIO_InitStructure);// ��ʼ��MOSI����
  GPIO_InitStructure.GPIO_Pin = FLASH_SPI_MISO_PIN;
  GPIO_Init(FLASH_SPI_MISO_PORT, &GPIO_InitStructure);// ��ʼ��MISO����
  SPI_FLASH_CS_HIGH(); // CS�����øߣ�FLASHֹͣ�ź�
  // SPI����ģʽ����
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // ȫ˫��ģʽ
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master; // ����ģʽ
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; // 8λ����ģʽ
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High; // ʱ�Ӽ��Ը�
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; // �ڶ���ʱ���ز���
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; // ���NSS����
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; // ʱ��Ԥ��Ƶ��
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; // MSBλ����
  SPI_InitStructure.SPI_CRCPolynomial = 7; // CRCУ��λ
  SPI_Init(FLASH_SPIx, &SPI_InitStructure); // ��ʼ��SPI
  SPI_Cmd(FLASH_SPIx, ENABLE); // ʹ��SPI
}

// ��ʱ�ص�����
uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorcode)
{
  FLASH_ERROR("SPI �ȴ���ʱ��errorcode=%d\r\n", errorcode);
  return 0;
}

// ʹ��SPI����һ���ֽ�
uint8_t SPI_FLASH_SendByte(uint8_t byte)
{
  SPITimeout = SPIT_FLAG_TIMEOUT; // ��ʱʱ���ʼ��
  while(SPI_I2S_GetFlagStatus(FLASH_SPIx, SPI_I2S_FLAG_TXE) == RESET) // �ȴ����ͻ�������(TXE�¼�)
  {
    if((SPITimeout--) == 0)
      return SPI_TIMEOUT_UserCallback(0);
  }
  SPI_I2S_SendData(FLASH_SPIx, byte); // д�����ݼĴ���
  SPITimeout = SPIT_FLAG_TIMEOUT;
  while(SPI_I2S_GetFlagStatus(FLASH_SPIx, SPI_I2S_FLAG_RXNE) == RESET) // �ȴ����ջ������ǿ�(RXNE�¼�)
  {
    if((SPITimeout--) == 0)
      return SPI_TIMEOUT_UserCallback(1);
  }
  return SPI_I2S_ReceiveData(FLASH_SPIx); // ��ȡ���ݼĴ���
}

// ʹ��SPI���������ֽ�
uint16_t SPI_FLASH_SendHalfWord(uint16_t halfword)
{
  SPITimeout = SPIT_FLAG_TIMEOUT; // ��ʱʱ���ʼ��
  while(SPI_I2S_GetFlagStatus(FLASH_SPIx, SPI_I2S_FLAG_TXE) == RESET) // �ȴ����ͻ�������(TXE�¼�)
  {
    if((SPITimeout--) == 0)
      return SPI_TIMEOUT_UserCallback(2);
  }
  SPI_I2S_SendData(FLASH_SPIx, halfword); // д�����ݼĴ���
  SPITimeout = SPIT_FLAG_TIMEOUT;
  while(SPI_I2S_GetFlagStatus(FLASH_SPIx, SPI_I2S_FLAG_RXNE) == RESET) // �ȴ����ջ������ǿ�(RXNE�¼�)
  {
    if((SPITimeout--) == 0)
      return SPI_TIMEOUT_UserCallback(3);
  }
  return SPI_I2S_ReceiveData(FLASH_SPIx); // ��ȡ���ݼĴ���
}

// ʹ��SPI��ȡһ���ֽ�
uint8_t SPI_FLASH_ReadByte(void)
{
  return (SPI_FLASH_SendByte(Dummy_Byte));
}

// ��ȡFLASH��״̬�Ĵ���
void SPI_FLASH_ReadStatusReg(uint32_t ReadAddr)
{
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(W25X_ReadData);
  // ���͵�ַ�ĸߡ��С���λ
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  SPI_FLASH_SendByte((ReadAddr & 0xFF00) >> 8);
  SPI_FLASH_SendByte(ReadAddr & 0xFF);
}

// FALSHд������ǰ����Ҫ����дʹ������
void SPI_FLASH_WriteEnable(void)
{
  SPI_FLASH_CS_LOW(); // ͨѶ��ʼ������CS
  SPI_FLASH_SendByte(W25X_WriteEnable);
  SPI_FLASH_CS_HIGH(); // ͨѶ����������CS
}

// �ȴ�FLASHд�������������busy��־λΪ0
void SPI_FLASH_WaitForWriteEnd(void)
{
  uint8_t status = 0;
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(W25X_ReadStatusReg); // ���Ͷ�ȡ״̬�Ĵ�������
  do{
    status = SPI_FLASH_SendByte(Dummy_Byte); // ��ȡFLASH��״̬�Ĵ���
  }
  while((status & 0x01) == SET); // �ȴ�WTPλ����
  SPI_FLASH_CS_HIGH();
}

// �������ģʽ
void SPI_FLASH_PowerDowm(void)
{
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(W25X_PowerDown); // ���͵�������
  SPI_FLASH_CS_HIGH();
}

// ����FLASH
void SPI_FLASH_WakeUp(void)
{
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(W25X_ReleasePowerDown); // ���ͻ�������
  SPI_FLASH_CS_HIGH();
}

// ����FLASH����������������������ַ
void SPI_FLASH_SectorErase(uint32_t SectorAddress)
{
  SPI_FLASH_WriteEnable(); // ����дʹ������
  SPI_FLASH_WaitForWriteEnd(); // �ȴ�FLASHд�����
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(W25X_SectorErase); // ����������������
  // ����������ַ�ĸߡ��С���λ
  SPI_FLASH_SendByte((SectorAddress & 0xFF0000) >> 16);
  SPI_FLASH_SendByte((SectorAddress & 0xFF00) >> 8);
  SPI_FLASH_SendByte(SectorAddress & 0xFF);
  SPI_FLASH_CS_HIGH();
  SPI_FLASH_WaitForWriteEnd(); // �ȴ�FLASHд�����
}

// ��Ƭ����FLASH
void SPI_FLASH_BulkErase(void)
{
  SPI_FLASH_WriteEnable(); // ����дʹ������
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(W25X_ChipErase); // ������Ƭ��������
  SPI_FLASH_CS_HIGH();
  SPI_FLASH_WaitForWriteEnd(); // �ȴ�FLASHд�����
}

// ��FLASH��ҳд������
// ������������д������ָ�롢д���ַ��д�볤��
void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  SPI_FLASH_WriteEnable(); // ����дʹ������
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(W25X_PageProgram); // ����ҳд������
  // �ֱ�д��ߡ��С���λ��ַ
  SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
  SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);
  SPI_FLASH_SendByte(WriteAddr & 0xFF);
  // �ж�ѹ��д��������Ƿ���ڵ�ҳ��󳤶�(256)
  if(NumByteToWrite > SPI_FLASH_PerWritePageSize)
  {
    NumByteToWrite = SPI_FLASH_PerWritePageSize; // ��ȡд�볤��
    FLASH_ERROR("FLASHд�����ݳ��ȳ�����ҳ��󳤶ȣ�\r\n");
  }
  while(NumByteToWrite--)
  {
    SPI_FLASH_SendByte(*pBuffer); // д������
    pBuffer++;
  }
  SPI_FLASH_CS_HIGH();
  SPI_FLASH_WaitForWriteEnd(); // �ȴ�FLASHд�����
}

// ��FLASH����д������
// ������������д������ָ�롢д���ַ��д�볤��
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
  uint8_t NumOfPage = 0; // ����ҳ������
  uint8_t NumOfSingle = 0; // ���嵥ҳд�����ݳ���
  uint8_t Addr = 0; // �����ַƫ����
  uint8_t count = 0; // ������
  uint8_t temp = 0; // ��ʱ����
	// 1.mod�������࣬��writeAddr��SPI_FLASH_PageSize��������������AddrֵΪ0
  Addr = WriteAddr % SPI_FLASH_PageSize;
	// 2.��count������ֵ���պÿ��Զ��뵽ҳ��ַ
  count = SPI_FLASH_PageSize - Addr;
	// 3.�����Ҫд��������ҳ
  NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
	// 4.mod�������࣬�����ʣ�಻��һҳ���ֽ���
  NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;
	// ��Addr=0,��WriteAddr �պð�ҳ���룬ֱ��д
  if (Addr == 0)
  {
		/* NumByteToWrite < SPI_FLASH_PageSize */
    if (NumOfPage == 0)
    {
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    { 
			/*�Ȱ�����ҳ��д��*/
      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize); // д������
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }
			/*���ж���Ĳ���һҳ�����ݣ�����д��*/
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle); // д��ʣ������
    }
  }
	/* ����ַ�� SPI_FLASH_PageSize ������  */
  else 
  {
		/* NumByteToWrite < SPI_FLASH_PageSize */
    if (NumOfPage == 0)
    {
			/*��ǰҳʣ���count��λ�ñ�NumOfSingleС��һҳд����*/
      if (NumOfSingle > count) 
      {
        temp = NumOfSingle - count;
				/*��д����ǰҳ*/
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;
				/*��дʣ�������*/
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
      }
      else /*��ǰҳʣ���count��λ����д��NumOfSingle������*/
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
			/*��ַ����������count�ֿ������������������*/
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
      NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;
			/* ��д��count�����ݣ�Ϊ��������һ��Ҫд�ĵ�ַ���� */
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
			/* ���������ظ���ַ�������� */
      WriteAddr +=  count;
      pBuffer += count;
			/*������ҳ��д��*/
      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }
			/*���ж���Ĳ���һҳ�����ݣ�����д��*/
      if (NumOfSingle != 0)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}

// ��ȡFLASH����
// ������������ȡ����ָ�롢��ȡ��ַ����ȡ����
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(W25X_ReadData); // ���Ͷ�ȡ��������
  // ���͵�ַ�ĸߡ��С���λ
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  SPI_FLASH_SendByte((ReadAddr & 0xFF00) >> 8);
  SPI_FLASH_SendByte(ReadAddr & 0xFF);
  while(NumByteToRead--)
  {
    *pBuffer = SPI_FLASH_SendByte(Dummy_Byte); // ��ȡ����
    pBuffer++;
  }
  SPI_FLASH_CS_HIGH();
}

// ��ȡFLASH Device ID
uint32_t SPI_FLASH_ReadDeviceID(void)
{
  uint32_t Temp = 0;
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(W25X_DeviceID);
  SPI_FLASH_SendByte(Dummy_Byte);
  SPI_FLASH_SendByte(Dummy_Byte);
  SPI_FLASH_SendByte(Dummy_Byte);
  Temp = SPI_FLASH_SendByte(Dummy_Byte);
  SPI_FLASH_CS_HIGH();
  return Temp;
}

// ��ȡFLASH ID
uint32_t SPI_FLASH_ReadID(void)
{
  uint32_t Temp = 0, Temp_H = 0, Temp_M = 0, Temp_L = 0;
  SPI_FLASH_CS_LOW();
  SPI_FLASH_SendByte(W25X_JedecDeviceID);
  Temp_H = SPI_FLASH_SendByte(Dummy_Byte);
  Temp_M = SPI_FLASH_SendByte(Dummy_Byte);
  Temp_L = SPI_FLASH_SendByte(Dummy_Byte);
  SPI_FLASH_CS_HIGH();
  Temp = (Temp_H << 16) | (Temp_M << 8) | Temp_L;
  return Temp;
}
