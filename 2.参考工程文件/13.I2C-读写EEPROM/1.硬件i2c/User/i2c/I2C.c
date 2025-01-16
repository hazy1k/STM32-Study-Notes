#include "I2C.h"
#include "usart.h"		

uint16_t EEPROM_ADDRESS;
static __IO uint32_t  I2CTimeout = I2CT_LONG_TIMEOUT; // ��ʱʱ���ʼ��   

// I2C GPIO����
void I2C_GPIO_Config(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 
  // ʹ��ʱ��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  // I2C_SCL��I2C_SDA����
  GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;	       
  GPIO_Init(I2C_SCL_PORT, &GPIO_InitStructure);   
  GPIO_InitStructure.GPIO_Pin = I2C_SDA_PIN;      
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;	       
  GPIO_Init(I2C_SDA_PORT, &GPIO_InitStructure);	  
}

// I2C������������
static void I2C_Mode_Configu(void)
{
  I2C_InitTypeDef  I2C_InitStructure; 
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C; // I2Cģʽ
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2; // 400KHzʱ��Ƶ��
  I2C_InitStructure.I2C_OwnAddress1 = I2Cx_OWN_ADDRESS7; // ����I2C�Լ��ĵ�ַ
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable; // ʹ��Ӧ��
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // 7λ��ַģʽ
  I2C_InitStructure.I2C_ClockSpeed = I2C_Speed; // ����ģʽ
	// I2C ��ʼ��
  I2C_Init(I2Cx, &I2C_InitStructure);
  I2C_Cmd(I2Cx, ENABLE);   
}

// I2C ����(EEPROM)��ʼ��
void I2C_EE_Init(void)
{
  I2C_GPIO_Config();  
  I2C_Mode_Configu();
  // ����ͷ�ļ�I2C.h�еĶ�����ѡ��EEPROMҪд��ĵ�ַ
#ifdef EEPROM_Block0_ADDRESS
  EEPROM_ADDRESS = EEPROM_Block0_ADDRESS; // ѡ�� EEPROM Block0 ��д��
#endif
#ifdef EEPROM_Block1_ADDRESS  
  EEPROM_ADDRESS = EEPROM_Block1_ADDRESS;
#endif
#ifdef EEPROM_Block2_ADDRESS  
  EEPROM_ADDRESS = EEPROM_Block2_ADDRESS;
#endif
#ifdef EEPROM_Block3_ADDRESS  
  EEPROM_ADDRESS = EEPROM_Block3_ADDRESS;
#endif
}

// �ȴ�I2C EEPROM�������ģʽ
void I2C_EE_WaitEepromStandbyState(void)      
{
  vu16 Temp_SR = 0;
  do
  {
    I2C_GenerateSTART(I2Cx, ENABLE); // ����I2C��ʼ�ź�
    Temp_SR = I2C_ReadRegister(I2Cx, I2C_Register_SR1); // ��ȡI2C״̬�Ĵ���1
    I2C_Send7bitAddress(I2Cx, EEPROM_ADDRESS, I2C_Direction_Transmitter); // ����I2C��ַ�ź�
  }while(!(I2C_ReadRegister(I2Cx, I2C_Register_SR1) & 0x0002)); // �ȴ�I2C״̬�Ĵ���1��BUSYλ����
  I2C_ClearFlag(I2Cx, I2C_FLAG_AF); // ���I2Cæ��־   
  I2C_GenerateSTOP(I2Cx, ENABLE); // I2Cֹͣ�ź� 
}

// �������ص�����
uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode)
{
  EEPROM_ERROR("I2C �ȴ���ʱ!errorCode = %d",errorCode); // ͨ����ͬ�Ĵ����룬�����ж��Ĳ��ֳ�ʱ
  return 0;
}
/*
  I2Cд���ݻ������̣�
  1.����I2C��ʼ�ź�
  2.����I2Cд����
  3.����Ҫд��ĵ�ַ
  4.����Ҫд�������
  5.����I2Cֹͣ�źţ��������ģʽ
*/
// ͨ��I2C��EEPROM��д��һ���ֽ�
// ����������pBuffer��Ҫд������ݣ�WriteAddr��Ҫд��ĵ�ַ
uint32_t I2C_EE_ByteWrite(u8* pBuffer, u8 WriteAddr) 
{
  I2C_GenerateSTART(I2Cx, ENABLE); // ����I2C��ʼ�ź�
  I2CTimeout = I2CT_FLAG_TIMEOUT;  // ��ʱʱ���ʼ�� 
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)) // �ȴ�I2C����״̬  
  {
    if((I2CTimeout--) == 0) 
      return I2C_TIMEOUT_UserCallback(0); // ���ش�����0��������I2C��ʼ�źų�ʱ
  } 
  I2CTimeout = I2CT_FLAG_TIMEOUT; // ��ʱʱ���ʼ��
  I2C_Send7bitAddress(I2Cx, EEPROM_ADDRESS, I2C_Direction_Transmitter); // ����I2Cд����
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((I2CTimeout--) == 0) 
      return I2C_TIMEOUT_UserCallback(1); // ���ش�����1��������I2C��ַ�ź�ʱ��ʱ
  }  
  I2C_SendData(I2Cx, WriteAddr); // ����Ҫд��ĵ�ַ
  I2CTimeout = I2CT_FLAG_TIMEOUT; 
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((I2CTimeout--) == 0) 
      return I2C_TIMEOUT_UserCallback(2); // ���ش�����2��������I2CҪд��ĵ�ַʱ��ʱ
  } 
  I2C_SendData(I2Cx, *pBuffer); // ����Ҫд����ֽ� 
  I2CTimeout = I2CT_FLAG_TIMEOUT;  
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((I2CTimeout--) == 0) 
      return I2C_TIMEOUT_UserCallback(3); // ���ش�����3��������I2CҪд����ֽ�ʱ��ʱ
  } 
  I2C_GenerateSTOP(I2Cx, ENABLE); // �����ֽ��ֽ���ϣ�ֹͣ�ź�
  return 1;
}

// ��EEPROM��һ��дѭ���п���д����ֽڣ���һ��д����ֽ��������ܳ���EEPROMҳ�Ĵ�С��AT24C02ÿҳ��8���ֽ�
// ����������pBuffer��Ҫд������ݣ�WriteAddr��Ҫд��ĵ�ַ��NumByteToWrite��Ҫд����ֽ���
uint32_t I2C_EE_PageWrite(u8* pBuffer, u8 WriteAddr, u8 NumByteToWrite)
{
  I2CTimeout = I2CT_LONG_TIMEOUT; // ��ʱʱ���ʼ��
  while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY)) // �ȴ�I2C����״̬
  {
    if((I2CTimeout--) == 0) 
      return I2C_TIMEOUT_UserCallback(4); // ���ش�����4������д��������ʱ�ȴ�I2C����״̬��ʱ
  } 
  I2C_GenerateSTART(I2Cx, ENABLE); // I2C��ʼ�ź�
  I2CTimeout = I2CT_FLAG_TIMEOUT; 
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))  
  {
    if((I2CTimeout--) == 0) 
      return I2C_TIMEOUT_UserCallback(5); // ���ش�����5������д�������ݷ���I2C��ʼ�źų�ʱ
  } 
  I2C_Send7bitAddress(I2Cx, EEPROM_ADDRESS, I2C_Direction_Transmitter); // ����I2Cд����
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))  
  {
    if((I2CTimeout--) == 0) 
      return I2C_TIMEOUT_UserCallback(6); // ���ش�����6������д�������ݷ���I2C��ַ�źų�ʱ
  } 
  I2C_SendData(I2Cx, WriteAddr); // ����Ҫд��ĵ�ַ  
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  while(! I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((I2CTimeout--) == 0) 
      return I2C_TIMEOUT_UserCallback(7); // ���ش�����7������д�������ݷ���I2CҪд��ĵ�ַ��ʱ
  } 
  while(NumByteToWrite--) // д�����ֽ�  
  {
    I2C_SendData(I2Cx, *pBuffer); // ����Ҫд����ֽ� 
    pBuffer++; // ����ָ�����
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
      if((I2CTimeout--) == 0) 
        return I2C_TIMEOUT_UserCallback(8); // ���ش�����8������д�������ݷ���I2CҪд����ֽڳ�ʱ
    } 
  }
  I2C_GenerateSTOP(I2Cx, ENABLE); // ���Ͷ��ֽ���ϣ�ֹͣ�ź�
  return 1;
}

// ���������е�����д��I2C EEPROM��
// ����������pBuffer��Ҫд������ݣ�WriteAddr��Ҫд��ĵ�ַ��NumByteToWrite��Ҫд����ֽ���
void I2C_EE_BufferWrite(u8* pBuffer, u8 WriteAddr, u16 NumByteToWrite) 
{
  u8 NumOfPage = 0; // ҳ��
  u8 NumOfSingle = 0; // �������ֽ�
  u8 Addr = 0; // ��ַ
  u8 count = 0; // ������
  Addr = WriteAddr % I2C_PageSize; // ����Ҫд��ĵ�ַ��ҳ�е�ƫ��
  count = I2C_PageSize - Addr; // ����ҳʣ��ռ��С
  NumOfPage =  NumByteToWrite / I2C_PageSize; // ����ҳ��
  NumOfSingle = NumByteToWrite % I2C_PageSize; // ���㵥�����ֽ�
  if(Addr == 0) // ���һҳ����д����
  {
    if(NumOfPage == 0) // ���NumByteToWrite < I2C_PageSize 
    {
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle); // д�뵥�����ֽ�
      I2C_EE_WaitEepromStandbyState(); // �ȴ�EEPROM�������ģʽ
    }
    else // ���NumByteToWrite > I2C_PageSize 
    {
      while(NumOfPage--)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_PageSize);  
    	  I2C_EE_WaitEepromStandbyState();  
        WriteAddr +=  I2C_PageSize;
        pBuffer += I2C_PageSize;
      }
      if(NumOfSingle!=0) // ������е������ֽ�
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
        I2C_EE_WaitEepromStandbyState();
      }
    }
  }
  else // ���һҳ����д���� 
  {
    if(NumOfPage== 0) // ���If NumByteToWrite < I2C_PageSize
    {
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      I2C_EE_WaitEepromStandbyState();
    }
    else // If NumByteToWrite > I2C_PageSize
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / I2C_PageSize;
      NumOfSingle = NumByteToWrite % I2C_PageSize;	
      if(count != 0)
      {  
        I2C_EE_PageWrite(pBuffer, WriteAddr, count);
        I2C_EE_WaitEepromStandbyState();
        WriteAddr += count;
        pBuffer += count;
      } 
      while(NumOfPage--)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_PageSize);
        I2C_EE_WaitEepromStandbyState();
        WriteAddr +=  I2C_PageSize;
        pBuffer += I2C_PageSize;  
      }
      if(NumOfSingle != 0)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle); 
        I2C_EE_WaitEepromStandbyState();
      }
    }
  }  
}

// ��I2C EEPROM�ж�ȡ���ݵ���������
// ����������pBuffer��Ҫ��ȡ�����ݣ�ReadAddr��Ҫ��ȡ�ĵ�ַ��NumByteToRead��Ҫ��ȡ���ֽ���
uint32_t I2C_EE_BufferRead(u8* pBuffer, u8 ReadAddr, u16 NumByteToRead)
{  
  I2CTimeout = I2CT_LONG_TIMEOUT; // ��ʱʱ���ʼ��
  while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY)) // �ȴ�I2C����״̬
  {
    if((I2CTimeout--) == 0) 
      return I2C_TIMEOUT_UserCallback(9); // ���ش�����9�������ȡ�������ʱ�ȴ�I2C����״̬��ʱ
  }
  I2C_GenerateSTART(I2Cx, ENABLE); // I2C��ʼ�ź�
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((I2CTimeout--) == 0) 
      return I2C_TIMEOUT_UserCallback(10); // ���ش�����10�������ȡ������ݷ���I2C��ʼ�źų�ʱ
  }
  I2C_Send7bitAddress(I2Cx, EEPROM_ADDRESS, I2C_Direction_Transmitter); // ����I2C������
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((I2CTimeout--) == 0) 
      return I2C_TIMEOUT_UserCallback(11); // ���ش�����11�������ȡ������ݷ���I2C��ַ�źų�ʱ
  }  
  I2C_Cmd(I2Cx, ENABLE); // ʹ��I2C
  I2C_SendData(I2Cx, ReadAddr); // ����Ҫ��ȡ�ĵ�ַ
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((I2CTimeout--) == 0) 
      return I2C_TIMEOUT_UserCallback(12); // ���ش�����12�������ȡ������ݷ���I2CҪ��ȡ�ĵ�ַ��ʱ
  }
  I2C_GenerateSTART(I2Cx, ENABLE); // ����I2C��ʼ�ź�
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((I2CTimeout--) == 0) 
      return I2C_TIMEOUT_UserCallback(13); // ���ش�����13�������ȡ������ݷ���I2C��ʼ�źų�ʱ
  }
  I2C_Send7bitAddress(I2Cx, EEPROM_ADDRESS, I2C_Direction_Receiver); // ����I2C������
  I2CTimeout = I2CT_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
  {
    if((I2CTimeout--) == 0) 
      return I2C_TIMEOUT_UserCallback(14); // ���ش�����14�������ȡ������ݷ���I2C��ַ�źų�ʱ
  }
  while(NumByteToRead) // ѭ���������� 
  {
    if(NumByteToRead == 1) // ���һ�ζ�������
    {
      I2C_AcknowledgeConfig(I2Cx, DISABLE); // ��ֹI2CӦ��
      I2C_GenerateSTOP(I2Cx, ENABLE); // ����I2Cֹͣ�ź�
    }
    I2CTimeout = I2CT_LONG_TIMEOUT;
		while(I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) == 0)   
		{
			if((I2CTimeout--) == 0) 
        return I2C_TIMEOUT_UserCallback(3); // ���ش�����3�������ȡ������ݽ���I2C���ݳ�ʱ
		} 
    {      
      *pBuffer = I2C_ReceiveData(I2Cx); // ����I2C����
      pBuffer++;  // ����ָ�����
      NumByteToRead--; // ��ȡ�ֽ����ݼ�        
    }   
  }
  I2C_AcknowledgeConfig(I2Cx, ENABLE); // ʹ��I2CӦ��
  return 1;
}
