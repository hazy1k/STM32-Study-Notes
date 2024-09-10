#include "rc522_function.h"
#include "rc522_config.h"
#include "stm32f10x.h"
#include "bsp_SysTick.h"
#include "./usart/bsp_usart.h"	


#define   RC522_DELAY()  Delay_us ( 2 )

 /**
  * @brief  向RC522发送1 Byte 数据
  * @param  byte，要发送的数据
  * @retval RC522返回的数据
  */
void SPI_RC522_SendByte ( uint8_t byte )
{
  uint8_t counter;

  for(counter=0;counter<8;counter++)
  {     
    if ( byte & 0x80 )
      RC522_MOSI_1 ();
    else 
      RC522_MOSI_0 ();
    //Delay_us ( 3 );
    RC522_DELAY();
    RC522_SCK_0 ();
    //Delay_us ( 1 );
    //Delay_us ( 3 );
    RC522_DELAY();
    RC522_SCK_1();
    //Delay_us ( 3 );
    RC522_DELAY();
    byte <<= 1; 
  } 	
}


/**
  * @brief  从RC522发送1 Byte 数据
  * @param  无
  * @retval RC522返回的数据
  */
uint8_t SPI_RC522_ReadByte ( void )
{
  uint8_t counter;
  uint8_t SPI_Data;

  for(counter=0;counter<8;counter++)
  {
    SPI_Data <<= 1;
    RC522_SCK_0 ();
    //Delay_us ( 3 );
    RC522_DELAY();
    if ( RC522_MISO_GET() == 1)
     SPI_Data |= 0x01;
    //Delay_us ( 2 );
    //Delay_us ( 3 );
    RC522_DELAY();
    RC522_SCK_1 ();
    //Delay_us ( 3 );
    RC522_DELAY();
  }
  return SPI_Data;
	
}



/**
  * @brief  读RC522寄存器
  * @param  ucAddress，寄存器地址
  * @retval 寄存器的当前值
  */
uint8_t ReadRawRC ( uint8_t ucAddress )
{
	uint8_t ucAddr, ucReturn;
	
	ucAddr = ( ( ucAddress << 1 ) & 0x7E ) | 0x80;	
	RC522_CS_Enable();
	SPI_RC522_SendByte ( ucAddr );
	ucReturn = SPI_RC522_ReadByte ();
	RC522_CS_Disable();
	
	return ucReturn;	
}

/**
  * @brief  写RC522寄存器
  * @param  ucAddress，寄存器地址
  * @param  ucValue，写入寄存器的值
  * @retval 无
  */
void WriteRawRC ( uint8_t ucAddress, uint8_t ucValue )
{  
	uint8_t ucAddr;
	
	ucAddr = ( ucAddress << 1 ) & 0x7E;	
	RC522_CS_Enable();	
	SPI_RC522_SendByte ( ucAddr );	
	SPI_RC522_SendByte ( ucValue );
	RC522_CS_Disable();		
}


/**
  * @brief  对RC522寄存器置位
  * @param  ucReg，寄存器地址
  * @param   ucMask，置位值
  * @retval 无
  */
void SetBitMask ( uint8_t ucReg, uint8_t ucMask )  
{
   uint8_t ucTemp;
	
   ucTemp = ReadRawRC ( ucReg );
   WriteRawRC ( ucReg, ucTemp | ucMask );         // set bit mask
}


/**
  * @brief  对RC522寄存器清位
  * @param  ucReg，寄存器地址
  * @param  ucMask，清位值
  * @retval 无
  */
void ClearBitMask ( uint8_t ucReg, uint8_t ucMask )  
{
   uint8_t ucTemp;
	
   ucTemp = ReadRawRC ( ucReg );
   WriteRawRC ( ucReg, ucTemp & ( ~ ucMask) );  // clear bit mask
}


/**
  * @brief  开启天线 
  * @param  无
  * @retval 无
  */
void PcdAntennaOn ( void )
{
   uint8_t uc;
  
   uc = ReadRawRC ( TxControlReg );
   if ( ! ( uc & 0x03 ) )
		SetBitMask(TxControlReg, 0x03);		
}


/**
  * @brief  关闭天线
  * @param  无
  * @retval 无
  */
void PcdAntennaOff ( void )
{
  ClearBitMask ( TxControlReg, 0x03 );	
}


/**
  * @brief  复位RC522 
  * @param  无
  * @retval 无
  */
void PcdReset ( void )
{
	RC522_Reset_Disable();
	
	Delay_us ( 1 );
	
	RC522_Reset_Enable();
	
	Delay_us ( 1 );
	
	RC522_Reset_Disable();
	
	Delay_us ( 1 );
	
	WriteRawRC ( CommandReg, 0x0f );
	
	while ( ReadRawRC ( CommandReg ) & 0x10 );
	
	Delay_us ( 1 );
	
  WriteRawRC ( ModeReg, 0x3D );            //定义发送和接收常用模式 和Mifare卡通讯，CRC初始值0x6363
	
  WriteRawRC ( TReloadRegL, 30 );          //16位定时器低位    
	WriteRawRC ( TReloadRegH, 0 );			     //16位定时器高位
	
  WriteRawRC ( TModeReg, 0x8D );				   //定义内部定时器的设置
	
  WriteRawRC ( TPrescalerReg, 0x3E );			 //设置定时器分频系数
	
	WriteRawRC ( TxAutoReg, 0x40 );				   //调制发送信号为100%ASK	
	
}



/**
  * @brief  设置RC522的工作方式
  * @param  ucType，工作方式
  * @retval 无
  */
void M500PcdConfigISOType ( uint8_t ucType )
{
	if ( ucType == 'A')                     //ISO14443_A
  {
		ClearBitMask ( Status2Reg, 0x08 );
		
    WriteRawRC ( ModeReg, 0x3D );//3F
		
		WriteRawRC ( RxSelReg, 0x86 );//84
		
		WriteRawRC( RFCfgReg, 0x7F );   //4F
		
		WriteRawRC( TReloadRegL, 30 );//tmoLength);// TReloadVal = 'h6a =tmoLength(dec) 
		
		WriteRawRC ( TReloadRegH, 0 );
		
		WriteRawRC ( TModeReg, 0x8D );
		
		WriteRawRC ( TPrescalerReg, 0x3E );
		
		Delay_us ( 2 );
		
		PcdAntennaOn ();//开天线
		
   }	 
}



/**
  * @brief  通过RC522和ISO14443卡通讯
  * @param  ucCommand，RC522命令字
  * @param  pInData，通过RC522发送到卡片的数据
  * @param  ucInLenByte，发送数据的字节长度
  * @param  pOutData，接收到的卡片返回数据
  * @param  pOutLenBit，返回数据的位长度
  * @retval 状态值= MI_OK，成功
  */
char PcdComMF522 ( uint8_t ucCommand, 
                    uint8_t * pInData,
                    uint8_t ucInLenByte, 
                    uint8_t * pOutData, 
                    uint32_t * pOutLenBit )		
{
  char cStatus = MI_ERR;
  uint8_t ucIrqEn   = 0x00;
  uint8_t ucWaitFor = 0x00;
  uint8_t ucLastBits;
  uint8_t ucN;
  uint32_t ul;

  switch ( ucCommand )
  {
     case PCD_AUTHENT:		//Mifare认证
        ucIrqEn   = 0x12;		//允许错误中断请求ErrIEn  允许空闲中断IdleIEn
        ucWaitFor = 0x10;		//认证寻卡等待时候 查询空闲中断标志位
        break;
     
     case PCD_TRANSCEIVE:		//接收发送 发送接收
        ucIrqEn   = 0x77;		//允许TxIEn RxIEn IdleIEn LoAlertIEn ErrIEn TimerIEn
        ucWaitFor = 0x30;		//寻卡等待时候 查询接收中断标志位与 空闲中断标志位
        break;
     
     default:
       break;     
  }

  WriteRawRC ( ComIEnReg, ucIrqEn | 0x80 );		//IRqInv置位管脚IRQ与Status1Reg的IRq位的值相反 
  ClearBitMask ( ComIrqReg, 0x80 );			//Set1该位清零时，CommIRqReg的屏蔽位清零
  WriteRawRC ( CommandReg, PCD_IDLE );		//写空闲命令
  SetBitMask ( FIFOLevelReg, 0x80 );			//置位FlushBuffer清除内部FIFO的读和写指针以及ErrReg的BufferOvfl标志位被清除

  for ( ul = 0; ul < ucInLenByte; ul ++ )
    WriteRawRC ( FIFODataReg, pInData [ ul ] );    		//写数据进FIFOdata
    
  WriteRawRC ( CommandReg, ucCommand );					//写命令


  if ( ucCommand == PCD_TRANSCEIVE )
    SetBitMask(BitFramingReg,0x80);  				//StartSend置位启动数据发送 该位与收发命令使用时才有效

  ul = 1000;//根据时钟频率调整，操作M1卡最大等待时间25ms

  do 														//认证 与寻卡等待时间	
  {
       ucN = ReadRawRC ( ComIrqReg );							//查询事件中断
       ul --;
  } while ( ( ul != 0 ) && ( ! ( ucN & 0x01 ) ) && ( ! ( ucN & ucWaitFor ) ) );		//退出条件i=0,定时器中断，与写空闲命令

  ClearBitMask ( BitFramingReg, 0x80 );					//清理允许StartSend位

  if ( ul != 0 )
  {
    if ( ! ( ReadRawRC ( ErrorReg ) & 0x1B ) )			//读错误标志寄存器BufferOfI CollErr ParityErr ProtocolErr
    {
      cStatus = MI_OK;
      
      if ( ucN & ucIrqEn & 0x01 )					//是否发生定时器中断
        cStatus = MI_NOTAGERR;   
        
      if ( ucCommand == PCD_TRANSCEIVE )
      {
        ucN = ReadRawRC ( FIFOLevelReg );			//读FIFO中保存的字节数
        
        ucLastBits = ReadRawRC ( ControlReg ) & 0x07;	//最后接收到得字节的有效位数
        
        if ( ucLastBits )
          * pOutLenBit = ( ucN - 1 ) * 8 + ucLastBits;   	//N个字节数减去1（最后一个字节）+最后一位的位数 读取到的数据总位数
        else
          * pOutLenBit = ucN * 8;   					//最后接收到的字节整个字节有效
        
        if ( ucN == 0 )		
          ucN = 1;    
        
        if ( ucN > MAXRLEN )
          ucN = MAXRLEN;   
        
        for ( ul = 0; ul < ucN; ul ++ )
          pOutData [ ul ] = ReadRawRC ( FIFODataReg );   
        
        }        

    }   
    else
      cStatus = MI_ERR;       
  }

  SetBitMask ( ControlReg, 0x80 );           // stop timer now
  WriteRawRC ( CommandReg, PCD_IDLE ); 
   
  return cStatus;
}

/**
  * @brief 寻卡
  * @param  ucReq_code，寻卡方式 = 0x52，寻感应区内所有符合14443A标准的卡；寻卡方式= 0x26，寻未进入休眠状态的卡
  * @param  pTagType，卡片类型代码
             = 0x4400，Mifare_UltraLight
             = 0x0400，Mifare_One(S50)
             = 0x0200，Mifare_One(S70)
             = 0x0800，Mifare_Pro(X))
             = 0x4403，Mifare_DESFire
  * @retval 状态值= MI_OK，成功
  */
char PcdRequest ( uint8_t ucReq_code, uint8_t * pTagType )
{
  char cStatus;  
  uint8_t ucComMF522Buf [ MAXRLEN ]; 
  uint32_t ulLen;


  ClearBitMask ( Status2Reg, 0x08 );	//清理指示MIFARECyptol单元接通以及所有卡的数据通信被加密的情况
  WriteRawRC ( BitFramingReg, 0x07 );	//	发送的最后一个字节的 七位
  SetBitMask ( TxControlReg, 0x03 );	//TX1,TX2管脚的输出信号传递经发送调制的13.56的能量载波信号

  ucComMF522Buf [ 0 ] = ucReq_code;		//存入 卡片命令字

  cStatus = PcdComMF522 ( PCD_TRANSCEIVE,	ucComMF522Buf, 1, ucComMF522Buf, & ulLen );	//寻卡  

  if ( ( cStatus == MI_OK ) && ( ulLen == 0x10 ) )	//寻卡成功返回卡类型 
  {    
     * pTagType = ucComMF522Buf [ 0 ];
     * ( pTagType + 1 ) = ucComMF522Buf [ 1 ];
  }

  else
   cStatus = MI_ERR;

  return cStatus;	 
}

/**
  * @brief  防冲撞
  * @param  pSnr，卡片序列号，4字节
  * @retval 状态值= MI_OK，成功
  */
char PcdAnticoll ( uint8_t * pSnr )
{
  char cStatus;
  uint8_t uc, ucSnr_check = 0;
  uint8_t ucComMF522Buf [ MAXRLEN ]; 
  uint32_t ulLen;
  

  ClearBitMask ( Status2Reg, 0x08 );		//清MFCryptol On位 只有成功执行MFAuthent命令后，该位才能置位
  WriteRawRC ( BitFramingReg, 0x00);		//清理寄存器 停止收发
  ClearBitMask ( CollReg, 0x80 );			//清ValuesAfterColl所有接收的位在冲突后被清除
 
  ucComMF522Buf [ 0 ] = 0x93;	//卡片防冲突命令
  ucComMF522Buf [ 1 ] = 0x20;
 
  cStatus = PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 2, ucComMF522Buf, & ulLen);//与卡片通信

  if ( cStatus == MI_OK)		//通信成功
  {
    for ( uc = 0; uc < 4; uc ++ )
    {
       * ( pSnr + uc )  = ucComMF522Buf [ uc ];			//读出UID
       ucSnr_check ^= ucComMF522Buf [ uc ];
    }
    
    if ( ucSnr_check != ucComMF522Buf [ uc ] )
      cStatus = MI_ERR;    				 
  }
  
  SetBitMask ( CollReg, 0x80 );
      
  return cStatus;		
}


/**
  * @brief  用RC522计算CRC16
  * @param  pIndata，计算CRC16的数组
  * @param  ucLen，计算CRC16的数组字节长度
  * @param   pOutData，存放计算结果存放的首地址
  * @retval 无
  */
void CalulateCRC ( uint8_t * pIndata, uint8_t ucLen, uint8_t * pOutData )
{
  uint8_t uc, ucN;


  ClearBitMask(DivIrqReg,0x04);

  WriteRawRC(CommandReg,PCD_IDLE);

  SetBitMask(FIFOLevelReg,0x80);

  for ( uc = 0; uc < ucLen; uc ++)
    WriteRawRC ( FIFODataReg, * ( pIndata + uc ) );   

  WriteRawRC ( CommandReg, PCD_CALCCRC );

  uc = 0xFF;

  do 
  {
      ucN = ReadRawRC ( DivIrqReg );
      uc --;
  } while ( ( uc != 0 ) && ! ( ucN & 0x04 ) );
  
  pOutData [ 0 ] = ReadRawRC ( CRCResultRegL );
  pOutData [ 1 ] = ReadRawRC ( CRCResultRegM );		
}


/**
  * @brief  选定卡片
  * @param  pSnr，卡片序列号，4字节
  * @retval 状态值= MI_OK，成功
  */
char PcdSelect ( uint8_t * pSnr )
{
  char ucN;
  uint8_t uc;
  uint8_t ucComMF522Buf [ MAXRLEN ]; 
  uint32_t  ulLen;
  
  
  ucComMF522Buf [ 0 ] = PICC_ANTICOLL1;
  ucComMF522Buf [ 1 ] = 0x70;
  ucComMF522Buf [ 6 ] = 0;

  for ( uc = 0; uc < 4; uc ++ )
  {
    ucComMF522Buf [ uc + 2 ] = * ( pSnr + uc );
    ucComMF522Buf [ 6 ] ^= * ( pSnr + uc );
  }
  
  CalulateCRC ( ucComMF522Buf, 7, & ucComMF522Buf [ 7 ] );

  ClearBitMask ( Status2Reg, 0x08 );

  ucN = PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 9, ucComMF522Buf, & ulLen );
  
  if ( ( ucN == MI_OK ) && ( ulLen == 0x18 ) )
    ucN = MI_OK;  
  else
    ucN = MI_ERR;    
  
  return ucN;		
}



/**
  * @brief  验证卡片密码
  * @param  ucAuth_mode，密码验证模式= 0x60，验证A密钥，密码验证模式= 0x61，验证B密钥
  * @param  uint8_t ucAddr，块地址
  * @param  pKey，密码 
  * @param  pSnr，卡片序列号，4字节
  * @retval 状态值= MI_OK，成功
  */
char PcdAuthState ( uint8_t ucAuth_mode, uint8_t ucAddr, uint8_t * pKey, uint8_t * pSnr )
{
  char cStatus;
  uint8_t uc, ucComMF522Buf [ MAXRLEN ];
  uint32_t ulLen;
  

  ucComMF522Buf [ 0 ] = ucAuth_mode;
  ucComMF522Buf [ 1 ] = ucAddr;

  for ( uc = 0; uc < 6; uc ++ )
    ucComMF522Buf [ uc + 2 ] = * ( pKey + uc );   

  for ( uc = 0; uc < 6; uc ++ )
    ucComMF522Buf [ uc + 8 ] = * ( pSnr + uc );   

  cStatus = PcdComMF522 ( PCD_AUTHENT, ucComMF522Buf, 12, ucComMF522Buf, & ulLen );

  if ( ( cStatus != MI_OK ) || ( ! ( ReadRawRC ( Status2Reg ) & 0x08 ) ) )
    cStatus = MI_ERR;   
    
  return cStatus;
}


/**
  * @brief  写数据到M1卡一块
  * @param  ucAddr，块地址（0-63）。M1卡总共有16个扇区(每个扇区有：3个数据块+1个控制块),共64个块
  * @param  pData，写入的数据，16字节
  * @retval 状态值= MI_OK，成功
  */
char PcdWrite ( uint8_t ucAddr, uint8_t * pData )
{
  char cStatus;
  uint8_t uc, ucComMF522Buf [ MAXRLEN ];
  uint32_t ulLen;
   
  
  ucComMF522Buf [ 0 ] = PICC_WRITE;
  ucComMF522Buf [ 1 ] = ucAddr;

  CalulateCRC ( ucComMF522Buf, 2, & ucComMF522Buf [ 2 ] );

  cStatus = PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, & ulLen );

  if ( ( cStatus != MI_OK ) || ( ulLen != 4 ) || ( ( ucComMF522Buf [ 0 ] & 0x0F ) != 0x0A ) )
    cStatus = MI_ERR;   
      
  if ( cStatus == MI_OK )
  {
    //memcpy(ucComMF522Buf, pData, 16);
    for ( uc = 0; uc < 16; uc ++ )
      ucComMF522Buf [ uc ] = * ( pData + uc );  
    
    CalulateCRC ( ucComMF522Buf, 16, & ucComMF522Buf [ 16 ] );

    cStatus = PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 18, ucComMF522Buf, & ulLen );
    
    if ( ( cStatus != MI_OK ) || ( ulLen != 4 ) || ( ( ucComMF522Buf [ 0 ] & 0x0F ) != 0x0A ) )
      cStatus = MI_ERR;   
    
  } 	
  return cStatus;		
}


/**
  * @brief  读取M1卡一块数据
  * @param  ucAddr，块地址（0-63）。M1卡总共有16个扇区(每个扇区有：3个数据块+1个控制块),共64个块
  * @param  pData，读出的数据，16字节
  * @retval 状态值= MI_OK，成功
  */
char PcdRead ( uint8_t ucAddr, uint8_t * pData )
{
  char cStatus;
  uint8_t uc, ucComMF522Buf [ MAXRLEN ]; 
  uint32_t ulLen;
  
  ucComMF522Buf [ 0 ] = PICC_READ;
  ucComMF522Buf [ 1 ] = ucAddr;

  CalulateCRC ( ucComMF522Buf, 2, & ucComMF522Buf [ 2 ] );
 
  cStatus = PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, & ulLen );

  if ( ( cStatus == MI_OK ) && ( ulLen == 0x90 ) )
  {
    for ( uc = 0; uc < 16; uc ++ )
      * ( pData + uc ) = ucComMF522Buf [ uc ];   
  }
  
  else
    cStatus = MI_ERR;   
   
  return cStatus;		
}




/**
  * @brief  判断 ucAddr 是否数据块
  * @param  ucAddr，块绝对地址（0-63）
  * @retval 返回值 1:是数据块；0:不是数据块
  */
char IsDataBlock( uint8_t ucAddr )
{
  if(ucAddr == 0)
  {
    printf("第0扇区的块0不可更改,不应对其进行操作\r\n");
    return 0;
  }
  /* 如果是数据块(不包含数据块0) */
  if( (ucAddr<64) && (((ucAddr+1)%4) != 0) )
  {
    return 1;
  }
  
  printf("块地址不是指向数据块\r\n");
  return 0;
}

/**
  * @brief  写 pData 字符串到M1卡中的数据块
  * @param  ucAddr，数据块地址（不能写入控制块）
  * @param  pData，写入的数据，16字节
  * @retval 状态值= MI_OK，成功
  */
char PcdWriteString ( uint8_t ucAddr, uint8_t * pData )
{
  /* 如果是数据块(不包含数据块0)，则写入 */
  if( IsDataBlock(ucAddr) )
  {
    return PcdWrite(ucAddr, pData);
  }

  return MI_ERR;
}

/**
  * @brief  读取M1卡中的一块数据到 pData
  * @param  ucAddr，数据块地址（不读取控制块）
  * @param  pData，读出的数据，16字节
  * @retval 状态值= MI_OK，成功
  */
char PcdReadString ( uint8_t ucAddr, uint8_t * pData )
{
  /* 如果是数据块(不包含数据块0)，则读取 */
  if( IsDataBlock(ucAddr) )
  {
    return PcdRead(ucAddr, pData);
  }

  return MI_ERR;
}



/**
  * @brief  命令卡片进入休眠状态
  * @param  无
  * @retval 状态值= MI_OK，成功
  */
char PcdHalt( void )
{
	uint8_t ucComMF522Buf [ MAXRLEN ]; 
	uint32_t  ulLen;
  

  ucComMF522Buf [ 0 ] = PICC_HALT;
  ucComMF522Buf [ 1 ] = 0;
	
  CalulateCRC ( ucComMF522Buf, 2, & ucComMF522Buf [ 2 ] );
 	PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, & ulLen );

  return MI_OK;	
}

/////////////////////////////////////////////////////////////////////
//功    能：写入钱包金额
//参数说明: ucAddr[IN]：块地址
//          pData：写入的金额
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char WriteAmount( uint8_t ucAddr, uint32_t pData )
{
	char status;
	uint8_t ucComMF522Buf[16];
	ucComMF522Buf[0] = (pData&((uint32_t)0x000000ff));
	ucComMF522Buf[1] = (pData&((uint32_t)0x0000ff00))>>8;
	ucComMF522Buf[2] = (pData&((uint32_t)0x00ff0000))>>16;
	ucComMF522Buf[3] = (pData&((uint32_t)0xff000000))>>24;	
	
	ucComMF522Buf[4] = ~(pData&((uint32_t)0x000000ff));
	ucComMF522Buf[5] = ~(pData&((uint32_t)0x0000ff00))>>8;
	ucComMF522Buf[6] = ~(pData&((uint32_t)0x00ff0000))>>16;
	ucComMF522Buf[7] = ~(pData&((uint32_t)0xff000000))>>24;	
	
	ucComMF522Buf[8] = (pData&((uint32_t)0x000000ff));
	ucComMF522Buf[9] = (pData&((uint32_t)0x0000ff00))>>8;
	ucComMF522Buf[10] = (pData&((uint32_t)0x00ff0000))>>16;
	ucComMF522Buf[11] = (pData&((uint32_t)0xff000000))>>24;	
	
	ucComMF522Buf[12] = ucAddr;
	ucComMF522Buf[13] = ~ucAddr;
	ucComMF522Buf[14] = ucAddr;
	ucComMF522Buf[15] = ~ucAddr;
  status = PcdWrite(ucAddr,ucComMF522Buf);
	return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：读取钱包金额
//参数说明: ucAddr[IN]：块地址
//          *pData：读出的金额
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char ReadAmount( uint8_t ucAddr, uint32_t *pData )
{
	
	char status = MI_ERR;
	uint8_t j;
	uint8_t ucComMF522Buf[16];
  status = PcdRead(ucAddr,ucComMF522Buf);
	if(status != MI_OK)
		return status;
	for(j=0;j<4;j++)
	{
		if((ucComMF522Buf[j] != ucComMF522Buf[j+8]) && (ucComMF522Buf[j] != ~ucComMF522Buf[j+4]))//验证一下是不是钱包的数据
		break;
	}
	if(j == 4)
	{
		  status = MI_OK;
			*pData = ucComMF522Buf[0] + (ucComMF522Buf[1]<<8) + (ucComMF522Buf[2]<<16) + (ucComMF522Buf[3]<<24);
	}
	else
	{
		status = MI_ERR;
		*pData = 0;
	}
  return status;	
}





/**
 * @brief 修改控制块 ucAddr 的密码A。注意 ucAddr 指的是控制块的地址。
 *        必须要校验密码B，密码B默认为6个0xFF，如果密码B也忘记了，那就改不了密码A了
 * @note  注意：该函数仅适用于默认的存储控制模式，若是其他的话可能出现问题
 * @param ucAddr：[控制块]所在的地址。M1卡总共有16个扇区(每个扇区有：3个数据块+1个控制块),共64个块
 * @param pKeyA：指向新的密码A字符串，六个字符，比如 "123456"
 * @retval 成功返回 MI_OK
 */
char ChangeKeyA( uint8_t ucAddr, uint8_t *pKeyA )
{
	uint8_t KeyBValue[]={0xFF ,0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // B密钥
  uint8_t ucArray_ID [ 4 ];    /*先后存放IC卡的类型和UID(IC卡序列号)*/                                                                                         
  uint8_t ucComMF522Buf[16];
  uint8_t j;
  
  /*寻卡*/
  while ( PcdRequest ( PICC_REQALL, ucArray_ID ) != MI_OK )
  {    
    printf( "寻卡失败\r\n" );
    Delay_us(1000000);
  }
  
  printf ( "寻卡成功\n" );
  
  /* 防冲突（当有多张卡进入读写器操作范围时，防冲突机制会从其中选择一张进行操作）*/
  if ( PcdAnticoll ( ucArray_ID ) == MI_OK )                                                                   
  {
    /* 选中卡 */
    PcdSelect(ucArray_ID);			

    /* 校验 B 密码 */
    if( PcdAuthState( PICC_AUTHENT1B, ucAddr, KeyBValue, ucArray_ID ) != MI_OK )
    {
      printf( "检验密码B失败\r\n" );
    }
    
    // 读取控制块里原本的数据（只要修改密码A，其他数据不改）
    if( PcdRead(ucAddr,ucComMF522Buf) != MI_OK)
    {
      printf( "读取控制块数据失败\r\n" );
      return MI_ERR;
    }
    
    /* 修改密码A */
    for(j=0; j<6; j++)
      ucComMF522Buf[j] = pKeyA[j];
    
    if( PcdWrite(ucAddr,ucComMF522Buf) != MI_OK)
    {
      printf( "写入数据到控制块失败\r\n" );
      return MI_ERR;
    }
    
    printf( "密码A修改成功！\r\n" );
    PcdHalt();
    
    return MI_OK;
  }
  
  return MI_ERR;
}


/**
 * @brief 按照RC522操作流程写入16字节数据到块 ucAddr
 *        函数里校验的是密码B，密码B默认为6个0xFF，也可以校验密码A
 *        用法：WriteDataBlock( 1, "123456789\n", 10); //字符串不够16个字节的后面补零写入
 * @note  注意：该函数仅适用于默认的存储控制模式，若是其他的话可能出现问题
 *        注意：使用该函数要注意 ucAddr 是块0、数据块还是控制块，该函数内部不对此做判断
 * @param ucAddr：任意块地址。M1卡总共有16个扇区(每个扇区有：3个数据块+1个控制块),共64个块
 * @param pData：指向要写入的数据，最大16个字符
 * @param Len：要写入数据的字节数
 * @retval 成功返回 MI_OK
 */
char WriteDataBlock( uint8_t ucAddr, uint8_t *pData, uint8_t Len)
{
	uint8_t KeyBValue[]={0xFF ,0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // B密钥
  uint8_t ucArray_ID [ 4 ];    /*先后存放IC卡的类型和UID(IC卡序列号)*/                                                                                         
  uint8_t ucComMF522Buf[16];
  uint8_t j;
  
  /*寻卡*/
  while ( PcdRequest ( PICC_REQALL, ucArray_ID ) != MI_OK )
  {    
    printf( "寻卡失败\r\n" );
    Delay_us(1000000);
  }
  
  printf ( "寻卡成功\n" );
  
  /* 防冲突（当有多张卡进入读写器操作范围时，防冲突机制会从其中选择一张进行操作）*/
  if ( PcdAnticoll ( ucArray_ID ) == MI_OK )                                                                   
  {
    /* 选中卡 */
    PcdSelect(ucArray_ID);			

    /* 校验 B 密码 */
    if( PcdAuthState( PICC_AUTHENT1B, ucAddr, KeyBValue, ucArray_ID ) != MI_OK )
    {
      printf( "检验密码B失败\r\n" );
    }
    
    /* 拷贝 pData 里的 Len 个字符到 ucComMF522Buf */
    for(j=0; j<16; j++)
    {
      if( j < Len ) ucComMF522Buf[j] = pData[j];
      else ucComMF522Buf[j] = 0; //16个字节若是未填满的字节置0
    }
    
    /* 写入字符串 */
    if( PcdWrite(ucAddr,ucComMF522Buf) != MI_OK)
    {
      printf( "写入数据到数据块失败\r\n" );
      return MI_ERR;
    }
    
    printf( "写入数据成功！\r\n" );
    PcdHalt();
    
    return MI_OK;
  }
  
  return MI_ERR;
}


/**
 * @brief 按照RC522操作流程读取块 ucAddr
 *        函数里校验的是密码B，密码B默认为6个0xFF，也可以校验密码A
 *        用法：ReadDataBlock( 1, databuf);  // databuf 至少为16字节：uint8_t databuf[16];
 * @note  注意：该函数仅适用于默认的存储控制模式，若是其他的话可能出现问题
 *        注意：使用该函数要注意 ucAddr 是块0、数据块还是控制块，该函数内部不对此做判断
 * @param ucAddr：任意块地址。M1卡总共有16个扇区(每个扇区有：3个数据块+1个控制块),共64个块
 * @param pData：指向读取到的数据，包含16个字符
 * @retval 成功返回 MI_OK
 */
char ReadDataBlock( uint8_t ucAddr, uint8_t *pData)
{
	uint8_t KeyBValue[]={0xFF ,0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // B密钥
  uint8_t ucArray_ID [ 4 ];    /*先后存放IC卡的类型和UID(IC卡序列号)*/                                                                                         
  
  /*寻卡*/
  while ( PcdRequest ( PICC_REQALL, ucArray_ID ) != MI_OK )
  {    
    printf( "寻卡失败\r\n" );
    Delay_us(1000000);
  }
  
  printf ( "寻卡成功\n" );
  
  /* 防冲突（当有多张卡进入读写器操作范围时，防冲突机制会从其中选择一张进行操作）*/
  if ( PcdAnticoll ( ucArray_ID ) == MI_OK )                                                                   
  {
    /* 选中卡 */
    PcdSelect(ucArray_ID);			

    /* 校验 B 密码 */
    if( PcdAuthState( PICC_AUTHENT1B, ucAddr, KeyBValue, ucArray_ID ) != MI_OK )
    {
      printf( "检验密码B失败\r\n" );
    }
    
    // 读取数据块里的数据到 pData
    if( PcdRead(ucAddr, pData) != MI_OK)
    {
      printf( "读取数据块失败\r\n" );
      return MI_ERR;
    }
    
    printf( "读取数据成功！\r\n" );
    PcdHalt();
    
    return MI_OK;
  }
  
  return MI_ERR;
}

