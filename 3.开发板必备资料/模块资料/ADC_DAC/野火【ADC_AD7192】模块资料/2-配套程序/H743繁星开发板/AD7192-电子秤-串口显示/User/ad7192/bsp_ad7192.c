#include "./ad7192/bsp_ad7192.h"
#include "./usart/bsp_debug_usart.h"
#include <string.h>

uint32_t AD7192Registers[8]={0,0,0,0,0,0,0,0};//要读或写AD7192Registers[8]
//AD7192Registers数组为什么定义成无符号32位整型呢？又为什么定义8个元素呢？
//因为数据寄存器的位数最多(数据寄存器)可以达到32位，以32位为准。除了通讯寄存器，可操作的寄存器为8个，所以定义8个元素，一个寄存器对应AD7192Registers数组的一个元素，互不干扰。
//8个寄存器见ReadFromAD7192ViaSPI()
uint32_t AD7192Data = 0;
volatile unsigned char  Read_Flag=0;

SPI_HandleTypeDef hspi_AD7192;

/**
  * @brief   AD7192初始化定义
  * @param   	 
  * @retval  
  */
void AD7192_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  /* 使能SPI外设以及SPI引脚时钟 */
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
  
  /* SPI外设配置 */
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
  
  /* 片选使能 */
  AD7192_CS_ENABLE();
}


/**
  * @brief   AD7192模式和配置寄存器配置
  * @param   Channels：要启动的通道
  * @retval 
  */
void ad7192_mode_cfg_reg(uint32_t mode, uint32_t cfg)
{
  /* 更新配置 */
	AD7192Registers[REG_CONF] = cfg;
	AD7192Registers[REG_MODE] = mode;
  
  /* 写入配置 */
	WriteToAD7192ViaSPI(REG_MODE, 2, AD7192Registers, REG_MODE);    // 连续写模式寄存器和配置寄存器
}

/**
  * @brief   AD7192软件复位函数
  * @param   	 
  * @retval  
  */
void AD7192SoftwareReset(void)        //AD7192软件复位函数；软件复位的原理，通过执行一个占用至少40个串行时钟周期的写操作，并式DIN处去高电平状态。
{
  /*分别定义一个读缓存和写缓存，再把写缓存赋值，然后通过
	调用void ADuC7026SpiOperation(unsigned char* WriteBuffer, unsigned char *ReadBuffer, unsigned char NumberOfByte)来执行，将写缓存数据传送至AD7192*/
	unsigned char WriteBuf[1]; //写缓存
	unsigned char ReadBuf[1];  //读缓存
	unsigned char i;
	
	
	WriteBuf[0]	= 0xFF;	 
	for(i=0; i<10; i++)
	{
		AD7192readdata(WriteBuf, ReadBuf, 1);//调用STM32F103SpiOperation()来实现数据传输，向AD7192的通信寄存器写40个1；0xFF是8个1，循环5次一共是40个1。
	}
	 AD7192_SCLK_H;
}

/**
  * @brief   AD7192读写函数
  * @param   WriteBuffer ：需要写入数据的缓冲区
	* @param   ReadBuffer ： 读出数据的缓冲区
	* @param   NumberOfByte ： 数据大小(字节)
  * @retval  
  */
void AD7192readdata(unsigned char* WriteBuffer, unsigned char *ReadBuffer, unsigned char NumberOfByte)
{
  HAL_SPI_TransmitReceive(&hspi_AD7192, WriteBuffer, ReadBuffer, NumberOfByte, 1000);
}

/**
  * @brief   通过SPI对AD7192执行一次读操作函数
  * @param   RegisterStartAddress ：要读寄存器的起始地址(取值范围是从0x00——0x07)
	* @param   NumberOfRegistersToRead ： 要读寄存器的个数
	* @param   DataBuffer ： 要读入的值
	* @param   OffsetInBuffer ： 缓存内偏移
  * @retval  
  */
unsigned char ReadFromAD7192ViaSPI(const unsigned char RegisterStartAddress, const unsigned char NumberOfRegistersToRead, uint32_t *DataBuffer, const unsigned char OffsetInBuffer)
{
//形参包括要读寄存器的起始地址RegisterStartAddress(取值范围是从0x00——0x07)，要读取寄存器的个数，指向将读取AD7192寄存器数据存入的数组的指针(DataBuffer才是要读入的值其他是中间变量)，
//一般指向AD7192Registers[8]，
//const unsigned char OffsetInBuffer，字面意思是缓存内偏移，是指AD7192Registers[8]数组内部偏移，注意是数组哦，之前我们说过AD7192Registers[8]之所以定义8个元素，
//一个寄存器对应AD7192Registers[8]数组的一个元素，互不干扰。
	
	unsigned char WriteBuf[4]={0,0,0,0}; //定义有4个元素的写缓存数组，每个数组元素占1个字节。
	unsigned char ReadBuf[4]={0,0,0,0};  //定义有4个元素的读缓存数组，每个数组元素占1个字节。
	unsigned char i;

	
	//Delay(0xFFFF);
	for(i=0; i < NumberOfRegistersToRead; i++)
	{
		WriteBuf[0] = WEN|RW_R|((RegisterStartAddress + i)<<3)|CREAD_DIS;	 //写入通信寄存器;8位数据;下一个操作是对指定寄存器执行读操作。CREAD_DIS表示不使能连续读。
		//确定下一步进行寄存器读操作，那么写操作自然无效喽。
		AD7192readdata(WriteBuf, ReadBuf, 1);//首先通过写入通信寄存器来选定下一步要读取的寄存器
                                                //然后再将WriteBuf清空
		WriteBuf[0] = NOP;
		WriteBuf[1]	= NOP;
		WriteBuf[2]	= NOP;
		WriteBuf[3]	= NOP;                       

		switch(RegisterStartAddress + i)
			{
			case REG_ID		   :     //ID寄存器(0x04，8位寄存器)
			case REG_COM_STA :     //状态寄存器(0x00，8位寄存器)
			case REG_GPOCON  :     //通用数字输出控制寄存器(0x05，8位寄存器)
				AD7192readdata(WriteBuf, ReadBuf, 1);    //此3种情况是读取一个字节
				DataBuffer[OffsetInBuffer + i ] =  ReadBuf[0];
				break;
				
			case REG_MODE   :     //模式寄存器(0x01，24位)
			case REG_CONF	  :     //配置寄存器(0x02，24位)
			case REG_OFFSET :     //失调寄存器(0x06，24位)
			case REG_FS		  :     //满量程寄存器(0x07，24位)
				AD7192readdata(WriteBuf, ReadBuf, 3);	      //此4种情况是读取3个字节
        DataBuffer[OffsetInBuffer + i ] = ReadBuf[0];			
				DataBuffer[OffsetInBuffer + i ] = (DataBuffer[OffsetInBuffer + i ]<<8) + ReadBuf[1];
				DataBuffer[OffsetInBuffer + i ] = (DataBuffer[OffsetInBuffer + i ]<<8) + ReadBuf[2];
				break;
				
			case REG_DATA	 :     //数据寄存器(0x03，24位或32位) 
				if (AD7192Registers[REG_MODE] & DAT_STA_EN)	      //多通道使能，将状态寄存器的内容附加到数据寄存器24位的数据上，所以是32位数据
					{
					AD7192readdata(WriteBuf, ReadBuf, 4);	    //所以此情况是读4个字节 
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
  * @brief   通过SPI向AD7192执行写入控制寄存器的操作函数
  * @param   RegisterStartAddress ：写入寄存器的起始地址
	* @param   NumberOfRegistersToWrite ： 要写寄存器的个数
	* @param   DataBuffer ： 写出的值
	* @param   OffsetInBuffer ： 没有用到，主要是与通过SPI向AD7192读操作函数对称
  * @retval  操作成功返回0
  */
unsigned char WriteToAD7192ViaSPI(const unsigned char RegisterStartAddress, const unsigned char NumberOfRegistersToWrite, uint32_t *DataBuffer, const unsigned char OffsetInBuffer)
{
//形参包括要写入寄存器的起始地址RegisterStartAddress(取值范围是从0x00——0x07)，写入寄存器的个数，指向要写入AD7192寄存器的数组的指针(DataBuffer才是要写出的值其他是中间变量)，
//const unsigned char OffsetInBuffer虽然定义但没有用到，主要是与通过SPI向AD7192读操作函数对称
/*分别定义一个读缓存和写缓存，再把写缓存赋值，然后通过
	调用void ADuC7026SpiOperation(unsigned char* WriteBuffer, unsigned char *ReadBuffer, unsigned char NumberOfByte)来执行，将写缓存数据传送至AD7192*/
	unsigned char WriteBuf[4];//定义unsigned char WriteBuf[4]数组，数组中的元素为无符号字符型，占一个字节
	unsigned char ReadBuf[4]; //定义unsigned char ReadBuf[4]数组，数组中的元素为无符号字符型，占一个字节
	unsigned char i;

	for(i=0; i<NumberOfRegistersToWrite; i++)        //DataBuffer通常指向AD7192Registers
	{
		WriteBuf[0]	= WEN|RW_W|((RegisterStartAddress + i)<<3)|CREAD_DIS;//无一例外，首先写入通信寄存器;8位数据;下一个操作是对指定寄存器执行写操作。
		WriteBuf[1] = DataBuffer[RegisterStartAddress + i]>>16;          //右移16位表示什么意思？DataBuffer是指向无符号长整型的数组指针，每个数组元素占4个字节(32位)，最高的8位无效，该语句是将16-23这8位二进制数赋给WriteBuf[1]。
		WriteBuf[2] = DataBuffer[RegisterStartAddress + i]>>8;           //右移8位表示什么意思？同理该语句是将8-15这8位二进制数赋给WriteBuf[2]。
		WriteBuf[3]	= DataBuffer[RegisterStartAddress + i];              //同理该语句是将0-7这8位二进制数赋给WriteBuf[3]。
		AD7192readdata(WriteBuf, ReadBuf, 4);                      //看到了吧，最后还是通过调用STM32F103SpiOperation(WriteBuf, ReadBuf, 4)来完成写操作
	}

	return 0;
}

/**
  * @brief   AD7192内部零电平校准
  * @param   
  * @retval  
  */
void AD7192InternalZeroScaleCalibration()
{	
	AD7192Registers[REG_MODE] = 0;
  
  ReadFromAD7192ViaSPI(REG_MODE, 1, AD7192Registers, REG_MODE);    // 读模式寄存器

	AD7192Registers[REG_MODE] &= ~0xE00000;                          // 清除模式位
  AD7192Registers[REG_MODE] |= MODE_INZCL;                         // 设置内部零电平校准

	WriteToAD7192ViaSPI(REG_MODE, 1, AD7192Registers, REG_MODE);     // 写入模式寄存器

  do{
    ReadFromAD7192ViaSPI(REG_COM_STA, 1, AD7192Registers, REG_COM_STA);    // 读状态寄存器
  }while((AD7192Registers[REG_COM_STA] & RDY_H) != 0);    // 等待校准完成
}

/**
  * @brief   AD7192内部满量程校准
  * @param   
  * @retval  
  */
void AD7192InternalFullScaleCalibration()
{
  AD7192Registers[REG_MODE] = 0;
  
  ReadFromAD7192ViaSPI(REG_MODE, 1, AD7192Registers, REG_MODE);    // 读模式寄存器

	AD7192Registers[REG_MODE] &= ~0xE00000;                          // 清除模式位
  AD7192Registers[REG_MODE] |= MODE_INFCL;                         // 设置内部满量程校准

	WriteToAD7192ViaSPI(REG_MODE, 1, AD7192Registers, REG_MODE);     // 写入模式寄存器

  do{
    ReadFromAD7192ViaSPI(REG_COM_STA, 1, AD7192Registers, REG_COM_STA);    // 读状态寄存器
  }while((AD7192Registers[REG_COM_STA] & RDY_H) != 0);                     // 等待校准完成
  
}

/**
  * @brief   AD7192系统零电平校准
  * @param   
  * @retval  
  */
void AD7192ExternalZeroScaleCalibration()
{
	AD7192Registers[REG_MODE] = 0;
	AD7192Registers[REG_CONF] = 0;
	AD7192Registers[REG_CONF] = CHOP_DIS|REF_IN1|AIN1_AIN2|BURN_DIS|REFDET_DIS|BUF_DIS|UB_BI|GAIN_1;	// Gain = 1
	WriteToAD7192ViaSPI(REG_CONF, 1, AD7192Registers, REG_CONF);//写入配置寄存器

//	The user should connect the system zero-scale input to the channel input pins as selected by the CH7 to CH0 bits in the configuration register	
	AD7192Registers[REG_MODE] = 0;
	AD7192Registers[REG_MODE] = MODE_SYSZCL|DAT_STA_EN|EXT_XTAL|SINC_4|ENPAR_EN|CLK_DIV_DIS|SINGLECYCLE_DIS|REJ60_DIS|0x080;		
	WriteToAD7192ViaSPI(REG_MODE, 1, AD7192Registers, REG_MODE);//写入模式寄存器
}

/**
  * @brief   AD7192系统满量程校准
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
  * @brief   AD7192启动连续读函数(注意是启动哦！)
	* 				 连续读和执行一次读不一样，需要启动
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
  * @brief   AD7192读取转换数据函数
  * @param   
  * @retval  DataBuffer：转换结果的值
  */
uint32_t AD7192ReadConvertingData(void)
{
	unsigned char WriteBuf[4];
	unsigned char ReadBuf[4];
	unsigned long int DataBuffer;

//	AD7192_CS_ENABLE();//片选信号使能，选通AD7192。
	WriteBuf[0] = WEN|RW_R|((REG_DATA)<<3)|CREAD_DIS;	 

	AD7192readdata(WriteBuf, ReadBuf, 1);//首先写入通信寄存器，确认下一步操作是读取数据寄存器

	WriteBuf[0] = NOP;
	WriteBuf[1] = NOP;	
	WriteBuf[2] = NOP;
	WriteBuf[3]	= NOP;

	while(HAL_GPIO_ReadPin(AD7192_MISO_GPIO_Port,AD7192_MISO_Pin)== 0){;}			
	while(HAL_GPIO_ReadPin(AD7192_MISO_GPIO_Port,AD7192_MISO_Pin)== 1){;}			//	waiting the 1st RDY failling edge;即DOUT变为低电平。

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
  * @brief   AD7192连续读函数
  * @param   
  * @retval  DataBuffer：读取到的值
  */
uint32_t AD7192ContinuousRead()
{
	unsigned char WriteBuf[4];
	unsigned char ReadBuf[4];
	unsigned long int DataBuffer;//无符号32位长整型数据缓存变量。

	WriteBuf[0] = NOP;
	WriteBuf[1] = NOP;	
	WriteBuf[2] = NOP;
	WriteBuf[3]	= NOP;

	while(HAL_GPIO_ReadPin(AD7192_MISO_GPIO_Port,AD7192_MISO_Pin)== 0){;}			
	while(HAL_GPIO_ReadPin(AD7192_MISO_GPIO_Port,AD7192_MISO_Pin)== 1){;}			//	waiting the 1st RDY failling edge;即DOUT变为低电平。

	if ((AD7192Registers[REG_MODE] & DAT_STA_EN) == DAT_STA_EN)	//多通道使能，将状态寄存器的内容附加到数据寄存器24位的数据上，所以此情况是读4个字节。
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
  * @brief   AD7192退出连续读函数(和连续读启动函数、连续读操作函数配合使用)
  * @param   
  * @retval  
  */
void AD7192ExitContinuousRead()
{
	unsigned char WriteBuf[1];
	unsigned char ReadBuf[1];

	while(HAL_GPIO_ReadPin(AD7192_MISO_GPIO_Port,AD7192_MISO_Pin)== 0){;}			
	while(HAL_GPIO_ReadPin(AD7192_MISO_GPIO_Port,AD7192_MISO_Pin)== 1){;}			//	waiting the 1st RDY failling edge;即DOUT变为低电平。

	WriteBuf[0]	= WEN|RW_R|(REG_DATA<<3)|CREAD_DIS;

	AD7192readdata(WriteBuf, ReadBuf, 1);		
}

/**
  * @brief   AD7192读取温度
  * @param   
  * @retval  AD7192Data：未转化的温度值
  */
uint32_t AD7192ReadTemperature()
{
  uint32_t reg_data = 0;
	
  ReadFromAD7192ViaSPI(REG_MODE, 2, AD7192Registers, REG_MODE);    // 读模式和状态寄存器寄存器

	AD7192Registers[REG_MODE] &= ~0xE00000;                          // 清除模式位
  AD7192Registers[REG_MODE] |= MODE_SING;                          // 设置为单次转换模式
  
  AD7192Registers[REG_CONF] |= TEMP;                               // 设置温度转换通道

	WriteToAD7192ViaSPI(REG_MODE, 2, AD7192Registers, REG_MODE);     // 写入模式寄存器

	reg_data = AD7192ReadConvertingData();	

	return reg_data;
}

/**
  * @brief   读取的温度值计算函数
  * @param   
  * @retval  temp：计算结果
  */
float RealTemperature(uint32_t TemperatureCode)
{
	float temp = 0.0;
  
	temp = (TemperatureCode - 0x800000) / 2815.0 - 273;
  
	return temp;
}

/**
  * @brief   AD7192启动单次转换
  * @param   Channels：要启动的通道
  * @retval 
  */
void AD7192StartSingleConvertion(uint32_t Channels)
{
  ReadFromAD7192ViaSPI(REG_MODE, 2, AD7192Registers, REG_MODE);    // 读模式和状态寄存器寄存器

	AD7192Registers[REG_MODE] &= ~0xE00000;                          // 清除模式位
  AD7192Registers[REG_MODE] |= MODE_SING;                          // 设置为单次转换模式
  
  AD7192Registers[REG_CONF] &= ~0xFF00;                            // 清除通道选择位
  AD7192Registers[REG_CONF] |= Channels;                           // 设置要转换的通道

	WriteToAD7192ViaSPI(REG_MODE, 2, AD7192Registers, REG_MODE);     // 写入模式寄存器
}

/**
  * @brief   AD7192启动连续转换
  * @param   Channels：要启动的通道（没有选择的通道会被关闭）
  * @retval 
  */
void AD7192StartContinuousConvertion(uint32_t Channels)
{
	AD7192Registers[REG_MODE] = 0;
  
  ReadFromAD7192ViaSPI(REG_MODE, 2, AD7192Registers, REG_MODE);    // 读模式和状态寄存器寄存器

	AD7192Registers[REG_MODE] &= ~0xE00000;                          // 清除模式位
  AD7192Registers[REG_MODE] |= MODE_CONT;                          // 设置为连续转换模式
  
  AD7192Registers[REG_CONF] &= ~0xFF00;                            // 清除通道选择位
  AD7192Registers[REG_CONF] |= Channels;                           // 设置要转换的通道

	WriteToAD7192ViaSPI(REG_MODE, 2, AD7192Registers, REG_MODE);     // 写入模式寄存器
}


