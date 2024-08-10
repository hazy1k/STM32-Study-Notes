#include "./flash/bsp_spi_flash.h"

static __IO uint32_t  SPITimeout = SPIT_LONG_TIMEOUT;    
static uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode);

// SPI_FLASH��ʼ��
void SPI_FLASH_Init(void)
{
  // 1.���ȶ���SPI��GPIO��صĽṹ�����
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
	
	// 2.ʹ��SPIʱ��
	FLASH_SPI_APBxClock_FUN(FLASH_SPI_CLK, ENABLE);
	
	// 3.ʹ��SPI������ص�ʱ��
 	FLASH_SPI_CS_APBxClock_FUN(FLASH_SPI_CS_CLK|FLASH_SPI_SCK_CLK|FLASH_SPI_MISO_PIN|FLASH_SPI_MOSI_PIN, ENABLE);
	
  // 4.����SPI�� CS���ţ���ͨIO����
  GPIO_InitStructure.GPIO_Pin = FLASH_SPI_CS_PIN; // ����PC0
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // �ٶ�50MHz
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // �������
  GPIO_Init(FLASH_SPI_CS_PORT, &GPIO_InitStructure); // ��ʼ���ṹ��
	
  // 5.����SPI�� SCK����
  GPIO_InitStructure.GPIO_Pin = FLASH_SPI_SCK_PIN; // ����PA5
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // �����������
  GPIO_Init(FLASH_SPI_SCK_PORT, &GPIO_InitStructure); // ��ʼ���ṹ��

  // 6.����SPI�� MISO����
  GPIO_InitStructure.GPIO_Pin = FLASH_SPI_MISO_PIN; // ����PA6
  GPIO_Init(FLASH_SPI_MISO_PORT, &GPIO_InitStructure); // ��ʼ���ṹ��

  // 7.����SPI�� MOSI����
  GPIO_InitStructure.GPIO_Pin = FLASH_SPI_MOSI_PIN; // ����PA7
  GPIO_Init(FLASH_SPI_MOSI_PORT, &GPIO_InitStructure); // ��ʼ���ṹ��

  /* ֹͣ�ź� FLASH: CS���Ÿߵ�ƽ*/
  SPI_FLASH_CS_HIGH();

// SPI ģʽ����
// FLASHоƬ ֧��SPIģʽ0��ģʽ3���ݴ�����CPOL CPHA
SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // ˫��ȫ˫��
SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                      // ����ģʽ
SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                  // 8λ����ģʽ
SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;                        // ʱ�Ӽ��Ը�
SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;                       // ���ݲ����ڵڶ���ʱ�ӱ���
SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                          // ���NSS����
SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; // ʱ��Ԥ��Ƶ��
SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                 // MSBλ�ȴ�
SPI_InitStructure.SPI_CRCPolynomial = 7;                           // CRCУ��λ
SPI_Init(FLASH_SPIx , &SPI_InitStructure); // ��ʼ��SPI
SPI_Cmd(FLASH_SPIx , ENABLE); // ʹ��SPI
}

// ����FLASH����
void SPI_FLASH_SectorErase(u32 SectorAddr) // ��������:������ַ
{
  // 1.����FLASHдʹ������
  SPI_FLASH_WriteEnable();
  SPI_FLASH_WaitForWriteEnd(); // �ȴ�дʹ�ܽ���
  
  // 2.ѡ��FLASH: CS�͵�ƽ
  SPI_FLASH_CS_LOW();
  // 3.������������ָ��
  SPI_FLASH_SendByte(W25X_SectorErase);
  // 4.���Ͳ���������ַ�ĸ�λ
  SPI_FLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
  // 5.���Ͳ���������ַ����λ
  SPI_FLASH_SendByte((SectorAddr & 0xFF00) >> 8);
  // 6.���Ͳ���������ַ�ĵ�λ
  SPI_FLASH_SendByte(SectorAddr & 0xFF);
  // 7.ֹͣ�ź� FLASH: CS �ߵ�ƽ
  SPI_FLASH_CS_HIGH();
  // 8.�ȴ��������
  SPI_FLASH_WaitForWriteEnd();
}

// ����FLASH��������Ƭ����
void SPI_FLASH_BulkErase(void)
{
  // 1.����FLASHдʹ������
  SPI_FLASH_WriteEnable();
  // 2.ѡ��FLASH: CS�͵�ƽ
  SPI_FLASH_CS_LOW();
  // 3.�����������ָ��
  SPI_FLASH_SendByte(W25X_ChipErase);
  // 4.ֹͣ�ź� FLASH: CS �ߵ�ƽ
  SPI_FLASH_CS_HIGH();
  // 5.�ȴ��������
  SPI_FLASH_WaitForWriteEnd();
}

// ��FLASH��ҳд�����ݣ����ñ�����д������ǰ��Ҫ�Ȳ�������
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite) // ����������Ҫд�����ݵ�ָ�룬д���ַ��д�����ݳ���
{
  // 1.����FLASHдʹ������
  SPI_FLASH_WriteEnable();

  // 2.ѡ��FLASH: CS�͵�ƽ
  SPI_FLASH_CS_LOW();
  // 3.дҳдָ��
  SPI_FLASH_SendByte(W25X_PageProgram);
  // 4.����д��ַ�ĸ�λ
  SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
  // 5.����д��ַ����λ
  SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);
  // 6.����д��ַ�ĵ�λ
  SPI_FLASH_SendByte(WriteAddr & 0xFF);
  // 7.�ж�Ҫд������ݳ����Ƿ���ڵ�ҳ��󳤶�
  if(NumByteToWrite > SPI_FLASH_PerWritePageSize) // ��Ҫд������ݳ��ȴ��ڵ�ҳ��󳤶�
  {
     NumByteToWrite = SPI_FLASH_PerWritePageSize; // ��ֻд�뵥ҳ��󳤶�
     FLASH_ERROR("SPI_FLASH_PageWrite too large!"); // ��ӡ������Ϣ
  }
  // 8.д������
  while (NumByteToWrite--)
  {
    // ���͵�ǰҪд����ֽ�����
    SPI_FLASH_SendByte(*pBuffer);
    // ָ����һ�ֽ�����
    pBuffer++;
  }
  // 9.ֹͣ�ź� FLASH: CS �ߵ�ƽ
  SPI_FLASH_CS_HIGH();
  // 10.�ȴ�д����
  SPI_FLASH_WaitForWriteEnd();
}

//��FLASHд�����ݣ����ñ�����д������ǰ��Ҫ�Ȳ�������
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite) // ����������Ҫд�����ݵ�ָ�룬д���ַ��д�����ݳ���
{
  u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0; // �������
	
	// 1.mod�������࣬��writeAddr��SPI_FLASH_PageSize��������������AddrֵΪ0
  Addr = WriteAddr % SPI_FLASH_PageSize;
	// 2.��count������ֵ���պÿ��Զ��뵽ҳ��ַ
  count = SPI_FLASH_PageSize - Addr;
	// 3.�����Ҫд��������ҳ
  NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
	// 4.mod�������࣬�����ʣ�಻��һҳ���ֽ���
  NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;
	
	// ��Addr=0,��WriteAddr �պð�ҳ���� aligned
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
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }
			/*���ж���Ĳ���һҳ�����ݣ�����д��*/
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
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
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead) // �����������洢�������ݵ�ָ�룬��ȡ��ַ����ȡ���ݳ���
{
  // 1.ѡ��FLASH: CS�͵�ƽ
  SPI_FLASH_CS_LOW();

  // 2.���Ͷ�ָ��
  SPI_FLASH_SendByte(W25X_ReadData);

  // 3.���Ͷ���ַ��λ
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  // 4.���Ͷ���ַ��λ
  SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  // 5.���Ͷ���ַ��λ
  SPI_FLASH_SendByte(ReadAddr & 0xFF);
	
	// 6.��ȡ����
  while (NumByteToRead--)
  {
    // ��ȡһ���ֽ�
    *pBuffer = SPI_FLASH_SendByte(Dummy_Byte);
    // ָ����һ���ֽڻ�����
    pBuffer++;
  }
  // ֹͣ�ź� FLASH: CS �ߵ�ƽ
  SPI_FLASH_CS_HIGH();
}

// ��ȡFLASH ID
u32 SPI_FLASH_ReadID(void)
{
  u32 Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

  // 1.��ʼͨѶ��CS�͵�ƽ
  SPI_FLASH_CS_LOW();
  // 2.����JEDECָ���ȡID
  SPI_FLASH_SendByte(W25X_JedecDeviceID);
  // 3.��ȡһ���ֽ�����-��λ
  Temp0 = SPI_FLASH_SendByte(Dummy_Byte);
  // 4.��ȡһ���ֽ�����-��λ
  Temp1 = SPI_FLASH_SendByte(Dummy_Byte);
  // 5.��ȡһ���ֽ�����-��λ
  Temp2 = SPI_FLASH_SendByte(Dummy_Byte);
  // ֹͣͨѶ��CS�ߵ�ƽ
  SPI_FLASH_CS_HIGH();
  // 6.�����������������Ϊ�����ķ���ֵ
	Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;
  return Temp;
}
// ��ȡFLASH Device ID
u32 SPI_FLASH_ReadDeviceID(void)
{
  u32 Temp = 0;
  // 1.��ʼͨѶ��CS�͵�ƽ
  SPI_FLASH_CS_LOW();
  // 2.����RDIDָ���ȡDevice ID
  SPI_FLASH_SendByte(W25X_DeviceID);
  SPI_FLASH_SendByte(Dummy_Byte);
  SPI_FLASH_SendByte(Dummy_Byte);
  SPI_FLASH_SendByte(Dummy_Byte);
  // 3.��ȡһ���ֽ�����
  Temp = SPI_FLASH_SendByte(Dummy_Byte);
  // 4.ֹͣͨѶ��CS�ߵ�ƽ�����ؽ��
  SPI_FLASH_CS_HIGH();
  return Temp;
}

// ��ȡFLASH״̬�Ĵ���
void SPI_FLASH_StartReadSequence(u32 ReadAddr) // ����������FLASH���ڲ���ַ��ȡ
{
  // 1.��ʼͨѶ��CS�͵�ƽ
  SPI_FLASH_CS_LOW();
  // 2.ѡ���ָ��
  SPI_FLASH_SendByte(W25X_ReadData);
  // 3.���Ͷ���ַ�ĸߡ��С���λ
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  SPI_FLASH_SendByte(ReadAddr & 0xFF);
}

// ʹ��SPI��ȡһ���ֽڵ�����
u8 SPI_FLASH_ReadByte(void)
{
  return (SPI_FLASH_SendByte(Dummy_Byte));
}

// ʹ��SPI����һ���ֽڵ�����
u8 SPI_FLASH_SendByte(u8 byte)
{
	 SPITimeout = SPIT_FLAG_TIMEOUT; // �ȴ���ʱʱ���ʼ��

  // �ȴ����ͻ�����Ϊ�գ�TXE�¼�
  while(SPI_I2S_GetFlagStatus(FLASH_SPIx, SPI_I2S_FLAG_TXE) == RESET)
	{
    if((SPITimeout--) == 0) 
      return SPI_TIMEOUT_UserCallback(0);
   }
  // д�����ݼĴ�������Ҫд�������д�뷢�ͻ�����
  SPI_I2S_SendData(FLASH_SPIx, byte);
	SPITimeout = SPIT_FLAG_TIMEOUT;
  // �ȴ����ջ������ǿգ�RXNE�¼�
  while(SPI_I2S_GetFlagStatus(FLASH_SPIx, SPI_I2S_FLAG_RXNE) == RESET)
  {
    if((SPITimeout--) == 0) 
      return SPI_TIMEOUT_UserCallback(1);
  }
  // ��ȡ���ݼĴ�������ȡ���ջ���������
  return SPI_I2S_ReceiveData(FLASH_SPIx);
}

// ʹ��SPI���������ֽڵ�����
u16 SPI_FLASH_SendHalfWord(u16 HalfWord)
{
	SPITimeout = SPIT_FLAG_TIMEOUT;
  // �ȴ����ͻ�����Ϊ�գ�TXE�¼�
  while(SPI_I2S_GetFlagStatus(FLASH_SPIx , SPI_I2S_FLAG_TXE) == RESET)
	{
    if((SPITimeout--) == 0) 
      return SPI_TIMEOUT_UserCallback(2);
  }
  // д�����ݼĴ�������Ҫд�������д�뷢�ͻ�����
  SPI_I2S_SendData(FLASH_SPIx , HalfWord);
	SPITimeout = SPIT_FLAG_TIMEOUT;
  // �ȴ����ջ������ǿգ�RXNE�¼�
  while(SPI_I2S_GetFlagStatus(FLASH_SPIx , SPI_I2S_FLAG_RXNE) == RESET)
	 {
    if((SPITimeout--) == 0) 
      return SPI_TIMEOUT_UserCallback(3);
   }
  // ��ȡ���ݼĴ�������ȡ���ջ��������� 
  return SPI_I2S_ReceiveData(FLASH_SPIx);
}

// ��FLASHд������ǰ���ȷ���дʹ������
void SPI_FLASH_WriteEnable(void)
{
  // 1.ͨѶ��ʼ��CS��
  SPI_FLASH_CS_LOW();
  // 2.����дʹ������
  SPI_FLASH_SendByte(W25X_WriteEnable);

  // 3.ͨѶ������CS��
  SPI_FLASH_CS_HIGH();
}

/* WIP(busy)��־��FLASH�ڲ�����д�� */
#define WIP_Flag                  0x01

// �ȴ�WIP(BUSY)��־����0�����ȴ���FLASH�ڲ�����д�����
void SPI_FLASH_WaitForWriteEnd(void)
{
  u8 FLASH_Status = 0;

  // 1.ѡ�� FLASH: CS ��
  SPI_FLASH_CS_LOW();

  // 2.���� ��״̬�Ĵ��� ����
  SPI_FLASH_SendByte(W25X_ReadStatusReg);

  // 3.��FLASHæµ����ȴ�
  do
  {
		/* ��ȡFLASHоƬ��״̬�Ĵ��� */
    FLASH_Status = SPI_FLASH_SendByte(Dummy_Byte);	 
  }
  while((FLASH_Status & WIP_Flag) == SET);  /* ����д���־ */
  // 4.ֹͣ�ź�  FLASH: CS ��
  SPI_FLASH_CS_HIGH();
}

// �������ģʽ
void SPI_Flash_PowerDown(void)   
{ 
  /* ͨѶ��ʼ��CS�� */
  SPI_FLASH_CS_LOW();
  /* ���� ���� ���� */
  SPI_FLASH_SendByte(W25X_PowerDown);
  /*ͨѶ������CS�� */
  SPI_FLASH_CS_HIGH();
}   

// ����
void SPI_Flash_WAKEUP(void)   
{
  /*ѡ�� FLASH: CS �� */
  SPI_FLASH_CS_LOW();
  /* ���� �ϵ� ���� */
  SPI_FLASH_SendByte(W25X_ReleasePowerDown);
  /* ֹͣ�ź� FLASH: CS �� */
  SPI_FLASH_CS_HIGH();
}   
   
// ��ʱ�ص�����
static  uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode) // �����������������
{
  /* �ȴ���ʱ��Ĵ���,���������Ϣ */
  FLASH_ERROR("SPI �ȴ���ʱ!errorCode = %d", errorCode);
  return 0;
}