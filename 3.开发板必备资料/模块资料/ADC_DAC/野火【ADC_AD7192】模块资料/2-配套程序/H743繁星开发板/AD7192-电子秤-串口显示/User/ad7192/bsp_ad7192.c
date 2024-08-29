#include "./ad7192/bsp_ad7192.h"
#include "./usart/bsp_debug_usart.h"
#include <string.h>

uint32_t AD7192Registers[8]={0,0,0,0,0,0,0,0};//Ҫ����дAD7192Registers[8]
//AD7192Registers����Ϊʲô������޷���32λ�����أ���Ϊʲô����8��Ԫ���أ�
//��Ϊ���ݼĴ�����λ�����(���ݼĴ���)���Դﵽ32λ����32λΪ׼������ͨѶ�Ĵ������ɲ����ļĴ���Ϊ8�������Զ���8��Ԫ�أ�һ���Ĵ�����ӦAD7192Registers�����һ��Ԫ�أ��������š�
//8���Ĵ�����ReadFromAD7192ViaSPI()
uint32_t AD7192Data = 0;
volatile unsigned char  Read_Flag=0;

SPI_HandleTypeDef hspi_AD7192;

/**
  * @brief   AD7192��ʼ������
  * @param   	 
  * @retval  
  */
void AD7192_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  /* ʹ��SPI�����Լ�SPI����ʱ�� */
  AD7192_SPIx_CLK_ENABLE();
  AD7192_GPIO_CLK_ENABLE();
  
  GPIO_InitStruct.Pin = AD7192_MISO_Pin|AD7192_MOSI_Pin|AD7192_SCK_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(AD7192_MISO_GPIO_Port, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = AD7192_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(AD7192_CS_GPIO_Port, &GPIO_InitStruct);  
  
  /* SPI�������� */
  hspi_AD7192.Instance = AD7192_SPIx;
  hspi_AD7192.Init.Mode = SPI_MODE_MASTER;
  hspi_AD7192.Init.Direction = SPI_DIRECTION_2LINES;
  hspi_AD7192.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi_AD7192.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi_AD7192.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi_AD7192.Init.NSS = SPI_NSS_SOFT;
  hspi_AD7192.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi_AD7192.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi_AD7192.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi_AD7192.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi_AD7192.Init.CRCPolynomial = 7;
	
	hspi_AD7192.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  hspi_AD7192.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi_AD7192.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi_AD7192.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi_AD7192.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi_AD7192.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_ENABLE;
  hspi_AD7192.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_ENABLE;
  HAL_SPI_Init(&hspi_AD7192);
  
  /* Ƭѡʹ�� */
  AD7192_CS_ENABLE();
}


/**
  * @brief   AD7192ģʽ�����üĴ�������
  * @param   Channels��Ҫ������ͨ��
  * @retval 
  */
void ad7192_mode_cfg_reg(uint32_t mode, uint32_t cfg)
{
  /* �������� */
	AD7192Registers[REG_CONF] = cfg;
	AD7192Registers[REG_MODE] = mode;
  
  /* д������ */
	WriteToAD7192ViaSPI(REG_MODE, 2, AD7192Registers, REG_MODE);    // ����дģʽ�Ĵ��������üĴ���
}

/**
  * @brief   AD7192�����λ����
  * @param   	 
  * @retval  
  */
void AD7192SoftwareReset(void)        //AD7192�����λ�����������λ��ԭ��ͨ��ִ��һ��ռ������40������ʱ�����ڵ�д��������ʽDIN��ȥ�ߵ�ƽ״̬��
{
  /*�ֱ���һ���������д���棬�ٰ�д���渳ֵ��Ȼ��ͨ��
	����void ADuC7026SpiOperation(unsigned char* WriteBuffer, unsigned char *ReadBuffer, unsigned char NumberOfByte)��ִ�У���д�������ݴ�����AD7192*/
	unsigned char WriteBuf[1]; //д����
	unsigned char ReadBuf[1];  //������
	unsigned char i;
	
	
	WriteBuf[0]	= 0xFF;	 
	for(i=0; i<10; i++)
	{
		AD7192readdata(WriteBuf, ReadBuf, 1);//����STM32F103SpiOperation()��ʵ�����ݴ��䣬��AD7192��ͨ�żĴ���д40��1��0xFF��8��1��ѭ��5��һ����40��1��
	}
	 AD7192_SCLK_H;
}

/**
  * @brief   AD7192��д����
  * @param   WriteBuffer ����Ҫд�����ݵĻ�����
	* @param   ReadBuffer �� �������ݵĻ�����
	* @param   NumberOfByte �� ���ݴ�С(�ֽ�)
  * @retval  
  */
void AD7192readdata(unsigned char* WriteBuffer, unsigned char *ReadBuffer, unsigned char NumberOfByte)
{
  HAL_SPI_TransmitReceive(&hspi_AD7192, WriteBuffer, ReadBuffer, NumberOfByte, 1000);
}

/**
  * @brief   ͨ��SPI��AD7192ִ��һ�ζ���������
  * @param   RegisterStartAddress ��Ҫ���Ĵ�������ʼ��ַ(ȡֵ��Χ�Ǵ�0x00����0x07)
	* @param   NumberOfRegistersToRead �� Ҫ���Ĵ����ĸ���
	* @param   DataBuffer �� Ҫ�����ֵ
	* @param   OffsetInBuffer �� ������ƫ��
  * @retval  
  */
unsigned char ReadFromAD7192ViaSPI(const unsigned char RegisterStartAddress, const unsigned char NumberOfRegistersToRead, uint32_t *DataBuffer, const unsigned char OffsetInBuffer)
{
//�βΰ���Ҫ���Ĵ�������ʼ��ַRegisterStartAddress(ȡֵ��Χ�Ǵ�0x00����0x07)��Ҫ��ȡ�Ĵ����ĸ�����ָ�򽫶�ȡAD7192�Ĵ������ݴ���������ָ��(DataBuffer����Ҫ�����ֵ�������м����)��
//һ��ָ��AD7192Registers[8]��
//const unsigned char OffsetInBuffer��������˼�ǻ�����ƫ�ƣ���ָAD7192Registers[8]�����ڲ�ƫ�ƣ�ע��������Ŷ��֮ǰ����˵��AD7192Registers[8]֮���Զ���8��Ԫ�أ�
//һ���Ĵ�����ӦAD7192Registers[8]�����һ��Ԫ�أ��������š�
	
	unsigned char WriteBuf[4]={0,0,0,0}; //������4��Ԫ�ص�д�������飬ÿ������Ԫ��ռ1���ֽڡ�
	unsigned char ReadBuf[4]={0,0,0,0};  //������4��Ԫ�صĶ��������飬ÿ������Ԫ��ռ1���ֽڡ�
	unsigned char i;

	
	//Delay(0xFFFF);
	for(i=0; i < NumberOfRegistersToRead; i++)
	{
		WriteBuf[0] = WEN|RW_R|((RegisterStartAddress + i)<<3)|CREAD_DIS;	 //д��ͨ�żĴ���;8λ����;��һ�������Ƕ�ָ���Ĵ���ִ�ж�������CREAD_DIS��ʾ��ʹ����������
		//ȷ����һ�����мĴ�������������ôд������Ȼ��Чඡ�
		AD7192readdata(WriteBuf, ReadBuf, 1);//����ͨ��д��ͨ�żĴ�����ѡ����һ��Ҫ��ȡ�ļĴ���
                                                //Ȼ���ٽ�WriteBuf���
		WriteBuf[0] = NOP;
		WriteBuf[1]	= NOP;
		WriteBuf[2]	= NOP;
		WriteBuf[3]	= NOP;                       

		switch(RegisterStartAddress + i)
			{
			case REG_ID		   :     //ID�Ĵ���(0x04��8λ�Ĵ���)
			case REG_COM_STA :     //״̬�Ĵ���(0x00��8λ�Ĵ���)
			case REG_GPOCON  :     //ͨ������������ƼĴ���(0x05��8λ�Ĵ���)
				AD7192readdata(WriteBuf, ReadBuf, 1);    //��3������Ƕ�ȡһ���ֽ�
				DataBuffer[OffsetInBuffer + i ] =  ReadBuf[0];
				break;
				
			case REG_MODE   :     //ģʽ�Ĵ���(0x01��24λ)
			case REG_CONF	  :     //���üĴ���(0x02��24λ)
			case REG_OFFSET :     //ʧ���Ĵ���(0x06��24λ)
			case REG_FS		  :     //�����̼Ĵ���(0x07��24λ)
				AD7192readdata(WriteBuf, ReadBuf, 3);	      //��4������Ƕ�ȡ3���ֽ�
        DataBuffer[OffsetInBuffer + i ] = ReadBuf[0];			
				DataBuffer[OffsetInBuffer + i ] = (DataBuffer[OffsetInBuffer + i ]<<8) + ReadBuf[1];
				DataBuffer[OffsetInBuffer + i ] = (DataBuffer[OffsetInBuffer + i ]<<8) + ReadBuf[2];
				break;
				
			case REG_DATA	 :     //���ݼĴ���(0x03��24λ��32λ) 
				if (AD7192Registers[REG_MODE] & DAT_STA_EN)	      //��ͨ��ʹ�ܣ���״̬�Ĵ��������ݸ��ӵ����ݼĴ���24λ�������ϣ�������32λ����
					{
					AD7192readdata(WriteBuf, ReadBuf, 4);	    //���Դ�����Ƕ�4���ֽ� 
					DataBuffer[OffsetInBuffer + i ] = ReadBuf[0];
					DataBuffer[OffsetInBuffer + i ] = (DataBuffer[OffsetInBuffer + i ]<<8) + ReadBuf[1];
					DataBuffer[OffsetInBuffer + i ] = (DataBuffer[OffsetInBuffer + i ]<<8) + ReadBuf[2];					
					DataBuffer[OffsetInBuffer + i ] = (DataBuffer[OffsetInBuffer + i ]<<8) + ReadBuf[3];
					break;
					}
				else 
					{
					AD7192readdata(WriteBuf, ReadBuf, 3);	   //do not transfer the status contents after read data register 
					DataBuffer[OffsetInBuffer + i ] = ReadBuf[0];
					DataBuffer[OffsetInBuffer + i ] = (DataBuffer[OffsetInBuffer + i ]<<8) + ReadBuf[1];
					DataBuffer[OffsetInBuffer + i ] = (DataBuffer[OffsetInBuffer + i ]<<8) + ReadBuf[2];
					break;
					}
	
			default			 : 
					break;
															
		}
	}
	                          
	return 0;
}

/**
  * @brief   ͨ��SPI��AD7192ִ��д����ƼĴ����Ĳ�������
  * @param   RegisterStartAddress ��д��Ĵ�������ʼ��ַ
	* @param   NumberOfRegistersToWrite �� Ҫд�Ĵ����ĸ���
	* @param   DataBuffer �� д����ֵ
	* @param   OffsetInBuffer �� û���õ�����Ҫ����ͨ��SPI��AD7192�����������Գ�
  * @retval  �����ɹ�����0
  */
unsigned char WriteToAD7192ViaSPI(const unsigned char RegisterStartAddress, const unsigned char NumberOfRegistersToWrite, uint32_t *DataBuffer, const unsigned char OffsetInBuffer)
{
//�βΰ���Ҫд��Ĵ�������ʼ��ַRegisterStartAddress(ȡֵ��Χ�Ǵ�0x00����0x07)��д��Ĵ����ĸ�����ָ��Ҫд��AD7192�Ĵ����������ָ��(DataBuffer����Ҫд����ֵ�������м����)��
//const unsigned char OffsetInBuffer��Ȼ���嵫û���õ�����Ҫ����ͨ��SPI��AD7192�����������Գ�
/*�ֱ���һ���������д���棬�ٰ�д���渳ֵ��Ȼ��ͨ��
	����void ADuC7026SpiOperation(unsigned char* WriteBuffer, unsigned char *ReadBuffer, unsigned char NumberOfByte)��ִ�У���д�������ݴ�����AD7192*/
	unsigned char WriteBuf[4];//����unsigned char WriteBuf[4]���飬�����е�Ԫ��Ϊ�޷����ַ��ͣ�ռһ���ֽ�
	unsigned char ReadBuf[4]; //����unsigned char ReadBuf[4]���飬�����е�Ԫ��Ϊ�޷����ַ��ͣ�ռһ���ֽ�
	unsigned char i;

	for(i=0; i<NumberOfRegistersToWrite; i++)        //DataBufferͨ��ָ��AD7192Registers
	{
		WriteBuf[0]	= WEN|RW_W|((RegisterStartAddress + i)<<3)|CREAD_DIS;//��һ���⣬����д��ͨ�żĴ���;8λ����;��һ�������Ƕ�ָ���Ĵ���ִ��д������
		WriteBuf[1] = DataBuffer[RegisterStartAddress + i]>>16;          //����16λ��ʾʲô��˼��DataBuffer��ָ���޷��ų����͵�����ָ�룬ÿ������Ԫ��ռ4���ֽ�(32λ)����ߵ�8λ��Ч��������ǽ�16-23��8λ������������WriteBuf[1]��
		WriteBuf[2] = DataBuffer[RegisterStartAddress + i]>>8;           //����8λ��ʾʲô��˼��ͬ�������ǽ�8-15��8λ������������WriteBuf[2]��
		WriteBuf[3]	= DataBuffer[RegisterStartAddress + i];              //ͬ�������ǽ�0-7��8λ������������WriteBuf[3]��
		AD7192readdata(WriteBuf, ReadBuf, 4);                      //�����˰ɣ������ͨ������STM32F103SpiOperation(WriteBuf, ReadBuf, 4)�����д����
	}

	return 0;
}

/**
  * @brief   AD7192�ڲ����ƽУ׼
  * @param   
  * @retval  
  */
void AD7192InternalZeroScaleCalibration()
{	
	AD7192Registers[REG_MODE] = 0;
  
  ReadFromAD7192ViaSPI(REG_MODE, 1, AD7192Registers, REG_MODE);    // ��ģʽ�Ĵ���

	AD7192Registers[REG_MODE] &= ~0xE00000;                          // ���ģʽλ
  AD7192Registers[REG_MODE] |= MODE_INZCL;                         // �����ڲ����ƽУ׼

	WriteToAD7192ViaSPI(REG_MODE, 1, AD7192Registers, REG_MODE);     // д��ģʽ�Ĵ���

  do{
    ReadFromAD7192ViaSPI(REG_COM_STA, 1, AD7192Registers, REG_COM_STA);    // ��״̬�Ĵ���
  }while((AD7192Registers[REG_COM_STA] & RDY_H) != 0);    // �ȴ�У׼���
}

/**
  * @brief   AD7192�ڲ�������У׼
  * @param   
  * @retval  
  */
void AD7192InternalFullScaleCalibration()
{
  AD7192Registers[REG_MODE] = 0;
  
  ReadFromAD7192ViaSPI(REG_MODE, 1, AD7192Registers, REG_MODE);    // ��ģʽ�Ĵ���

	AD7192Registers[REG_MODE] &= ~0xE00000;                          // ���ģʽλ
  AD7192Registers[REG_MODE] |= MODE_INFCL;                         // �����ڲ�������У׼

	WriteToAD7192ViaSPI(REG_MODE, 1, AD7192Registers, REG_MODE);     // д��ģʽ�Ĵ���

  do{
    ReadFromAD7192ViaSPI(REG_COM_STA, 1, AD7192Registers, REG_COM_STA);    // ��״̬�Ĵ���
  }while((AD7192Registers[REG_COM_STA] & RDY_H) != 0);                     // �ȴ�У׼���
  
}

/**
  * @brief   AD7192ϵͳ���ƽУ׼
  * @param   
  * @retval  
  */
void AD7192ExternalZeroScaleCalibration()
{
	AD7192Registers[REG_MODE] = 0;
	AD7192Registers[REG_CONF] = 0;
	AD7192Registers[REG_CONF] = CHOP_DIS|REF_IN1|AIN1_AIN2|BURN_DIS|REFDET_DIS|BUF_DIS|UB_BI|GAIN_1;	// Gain = 1
	WriteToAD7192ViaSPI(REG_CONF, 1, AD7192Registers, REG_CONF);//д�����üĴ���

//	The user should connect the system zero-scale input to the channel input pins as selected by the CH7 to CH0 bits in the configuration register	
	AD7192Registers[REG_MODE] = 0;
	AD7192Registers[REG_MODE] = MODE_SYSZCL|DAT_STA_EN|EXT_XTAL|SINC_4|ENPAR_EN|CLK_DIV_DIS|SINGLECYCLE_DIS|REJ60_DIS|0x080;		
	WriteToAD7192ViaSPI(REG_MODE, 1, AD7192Registers, REG_MODE);//д��ģʽ�Ĵ���
}

/**
  * @brief   AD7192ϵͳ������У׼
  * @param   
  * @retval  
  */
void AD7192ExternalFullScaleCalibration()
{
	AD7192Registers[REG_MODE] = 0;
	AD7192Registers[REG_CONF] = 0;
	AD7192Registers[REG_CONF] = CHOP_DIS|REF_IN1|AIN1_AIN2|BURN_DIS|REFDET_DIS|BUF_DIS|UB_BI|GAIN_1;	// Gain = 1
	WriteToAD7192ViaSPI(REG_CONF, 1, AD7192Registers, REG_CONF);

//	The user should connect the system full-scale input to the channel input pins as selected by the CH7 to CH0 bits in the configuration register
	AD7192Registers[REG_MODE] = 0;
	AD7192Registers[REG_MODE] = MODE_SYSFCL|DAT_STA_EN|EXT_XTAL|SINC_4|ENPAR_EN|CLK_DIV_2|SINGLECYCLE_DIS|REJ60_DIS|0x080;		
	WriteToAD7192ViaSPI(REG_MODE, 1, AD7192Registers, REG_MODE);


}

/**
  * @brief   AD7192��������������(ע��������Ŷ��)
	* 				 ��������ִ��һ�ζ���һ������Ҫ����
  * @param   
  * @retval  
  */
void AD7192StartContinuousRead()
	
{

	unsigned char WriteBuf[1];
	unsigned char ReadBuf[1];

	WriteBuf[0] = WEN|RW_R|(REG_DATA<<3)|CREAD_EN;

	AD7192readdata(WriteBuf, ReadBuf, 1);	

}

/**
  * @brief   AD7192��ȡת�����ݺ���
  * @param   
  * @retval  DataBuffer��ת�������ֵ
  */
uint32_t AD7192ReadConvertingData(void)
{
	unsigned char WriteBuf[4];
	unsigned char ReadBuf[4];
	unsigned long int DataBuffer;

//	AD7192_CS_ENABLE();//Ƭѡ�ź�ʹ�ܣ�ѡͨAD7192��
	WriteBuf[0] = WEN|RW_R|((REG_DATA)<<3)|CREAD_DIS;	 

	AD7192readdata(WriteBuf, ReadBuf, 1);//����д��ͨ�żĴ�����ȷ����һ�������Ƕ�ȡ���ݼĴ���

	WriteBuf[0] = NOP;
	WriteBuf[1] = NOP;	
	WriteBuf[2] = NOP;
	WriteBuf[3]	= NOP;

	while(HAL_GPIO_ReadPin(AD7192_MISO_GPIO_Port,AD7192_MISO_Pin)== 0){;}			
	while(HAL_GPIO_ReadPin(AD7192_MISO_GPIO_Port,AD7192_MISO_Pin)== 1){;}			//	waiting the 1st RDY failling edge;��DOUT��Ϊ�͵�ƽ��

	if ((AD7192Registers[REG_MODE] & DAT_STA_EN) == DAT_STA_EN)
  {
		AD7192readdata(WriteBuf, ReadBuf, 4);	  
		DataBuffer = ReadBuf[0];
		DataBuffer = (DataBuffer<<8) + ReadBuf[1];
		DataBuffer = (DataBuffer<<8) + ReadBuf[2];
		DataBuffer = (DataBuffer<<8) + ReadBuf[3];
  }
	else 
  {
		AD7192readdata(WriteBuf, ReadBuf, 3);	   //do not transfer the status contents after read data register 
		DataBuffer = ReadBuf[0];
		DataBuffer = (DataBuffer<<8) + ReadBuf[1];
		DataBuffer = (DataBuffer<<8) + ReadBuf[2];
  }

	return DataBuffer;
}

/**
  * @brief   AD7192����������
  * @param   
  * @retval  DataBuffer����ȡ����ֵ
  */
uint32_t AD7192ContinuousRead()
{
	unsigned char WriteBuf[4];
	unsigned char ReadBuf[4];
	unsigned long int DataBuffer;//�޷���32λ���������ݻ��������

	WriteBuf[0] = NOP;
	WriteBuf[1] = NOP;	
	WriteBuf[2] = NOP;
	WriteBuf[3]	= NOP;

	while(HAL_GPIO_ReadPin(AD7192_MISO_GPIO_Port,AD7192_MISO_Pin)== 0){;}			
	while(HAL_GPIO_ReadPin(AD7192_MISO_GPIO_Port,AD7192_MISO_Pin)== 1){;}			//	waiting the 1st RDY failling edge;��DOUT��Ϊ�͵�ƽ��

	if ((AD7192Registers[REG_MODE] & DAT_STA_EN) == DAT_STA_EN)	//��ͨ��ʹ�ܣ���״̬�Ĵ��������ݸ��ӵ����ݼĴ���24λ�������ϣ����Դ�����Ƕ�4���ֽڡ�
  {
		AD7192readdata(WriteBuf, ReadBuf, 4);	  
		DataBuffer = ReadBuf[0];
		DataBuffer = (DataBuffer<<8) + ReadBuf[1];
		DataBuffer = (DataBuffer<<8) + ReadBuf[2];
		DataBuffer = (DataBuffer<<8) + ReadBuf[3];
  }
	else 
  {
		AD7192readdata(WriteBuf, ReadBuf, 3);	   //do not transfer the status contents after read data register 
		DataBuffer = 0x00;
		DataBuffer = (DataBuffer<<8) + ReadBuf[0];	
		DataBuffer = (DataBuffer<<8) + ReadBuf[1];
		DataBuffer = (DataBuffer<<8) + ReadBuf[2];
  }

	return DataBuffer;
}

/**
  * @brief   AD7192�˳�����������(�����������������������������������ʹ��)
  * @param   
  * @retval  
  */
void AD7192ExitContinuousRead()
{
	unsigned char WriteBuf[1];
	unsigned char ReadBuf[1];

	while(HAL_GPIO_ReadPin(AD7192_MISO_GPIO_Port,AD7192_MISO_Pin)== 0){;}			
	while(HAL_GPIO_ReadPin(AD7192_MISO_GPIO_Port,AD7192_MISO_Pin)== 1){;}			//	waiting the 1st RDY failling edge;��DOUT��Ϊ�͵�ƽ��

	WriteBuf[0]	= WEN|RW_R|(REG_DATA<<3)|CREAD_DIS;

	AD7192readdata(WriteBuf, ReadBuf, 1);		
}

/**
  * @brief   AD7192��ȡ�¶�
  * @param   
  * @retval  AD7192Data��δת�����¶�ֵ
  */
uint32_t AD7192ReadTemperature()
{
  uint32_t reg_data = 0;
	
  ReadFromAD7192ViaSPI(REG_MODE, 2, AD7192Registers, REG_MODE);    // ��ģʽ��״̬�Ĵ����Ĵ���

	AD7192Registers[REG_MODE] &= ~0xE00000;                          // ���ģʽλ
  AD7192Registers[REG_MODE] |= MODE_SING;                          // ����Ϊ����ת��ģʽ
  
  AD7192Registers[REG_CONF] |= TEMP;                               // �����¶�ת��ͨ��

	WriteToAD7192ViaSPI(REG_MODE, 2, AD7192Registers, REG_MODE);     // д��ģʽ�Ĵ���

	reg_data = AD7192ReadConvertingData();	

	return reg_data;
}

/**
  * @brief   ��ȡ���¶�ֵ���㺯��
  * @param   
  * @retval  temp��������
  */
float RealTemperature(uint32_t TemperatureCode)
{
	float temp = 0.0;
  
	temp = (TemperatureCode - 0x800000) / 2815.0 - 273;
  
	return temp;
}

/**
  * @brief   AD7192��������ת��
  * @param   Channels��Ҫ������ͨ��
  * @retval 
  */
void AD7192StartSingleConvertion(uint32_t Channels)
{
  ReadFromAD7192ViaSPI(REG_MODE, 2, AD7192Registers, REG_MODE);    // ��ģʽ��״̬�Ĵ����Ĵ���

	AD7192Registers[REG_MODE] &= ~0xE00000;                          // ���ģʽλ
  AD7192Registers[REG_MODE] |= MODE_SING;                          // ����Ϊ����ת��ģʽ
  
  AD7192Registers[REG_CONF] &= ~0xFF00;                            // ���ͨ��ѡ��λ
  AD7192Registers[REG_CONF] |= Channels;                           // ����Ҫת����ͨ��

	WriteToAD7192ViaSPI(REG_MODE, 2, AD7192Registers, REG_MODE);     // д��ģʽ�Ĵ���
}

/**
  * @brief   AD7192��������ת��
  * @param   Channels��Ҫ������ͨ����û��ѡ���ͨ���ᱻ�رգ�
  * @retval 
  */
void AD7192StartContinuousConvertion(uint32_t Channels)
{
	AD7192Registers[REG_MODE] = 0;
  
  ReadFromAD7192ViaSPI(REG_MODE, 2, AD7192Registers, REG_MODE);    // ��ģʽ��״̬�Ĵ����Ĵ���

	AD7192Registers[REG_MODE] &= ~0xE00000;                          // ���ģʽλ
  AD7192Registers[REG_MODE] |= MODE_CONT;                          // ����Ϊ����ת��ģʽ
  
  AD7192Registers[REG_CONF] &= ~0xFF00;                            // ���ͨ��ѡ��λ
  AD7192Registers[REG_CONF] |= Channels;                           // ����Ҫת����ͨ��

	WriteToAD7192ViaSPI(REG_MODE, 2, AD7192Registers, REG_MODE);     // д��ģʽ�Ĵ���
}


