#include "rc522_function.h"
#include "rc522_config.h"
#include "stm32f10x.h"
#include "bsp_SysTick.h"
#include "./usart/bsp_usart.h"	


#define   RC522_DELAY()  Delay_us ( 2 )

 /**
  * @brief  ��RC522����1 Byte ����
  * @param  byte��Ҫ���͵�����
  * @retval RC522���ص�����
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
  * @brief  ��RC522����1 Byte ����
  * @param  ��
  * @retval RC522���ص�����
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
  * @brief  ��RC522�Ĵ���
  * @param  ucAddress���Ĵ�����ַ
  * @retval �Ĵ����ĵ�ǰֵ
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
  * @brief  дRC522�Ĵ���
  * @param  ucAddress���Ĵ�����ַ
  * @param  ucValue��д��Ĵ�����ֵ
  * @retval ��
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
  * @brief  ��RC522�Ĵ�����λ
  * @param  ucReg���Ĵ�����ַ
  * @param   ucMask����λֵ
  * @retval ��
  */
void SetBitMask ( uint8_t ucReg, uint8_t ucMask )  
{
   uint8_t ucTemp;
	
   ucTemp = ReadRawRC ( ucReg );
   WriteRawRC ( ucReg, ucTemp | ucMask );         // set bit mask
}


/**
  * @brief  ��RC522�Ĵ�����λ
  * @param  ucReg���Ĵ�����ַ
  * @param  ucMask����λֵ
  * @retval ��
  */
void ClearBitMask ( uint8_t ucReg, uint8_t ucMask )  
{
   uint8_t ucTemp;
	
   ucTemp = ReadRawRC ( ucReg );
   WriteRawRC ( ucReg, ucTemp & ( ~ ucMask) );  // clear bit mask
}


/**
  * @brief  �������� 
  * @param  ��
  * @retval ��
  */
void PcdAntennaOn ( void )
{
   uint8_t uc;
  
   uc = ReadRawRC ( TxControlReg );
   if ( ! ( uc & 0x03 ) )
		SetBitMask(TxControlReg, 0x03);		
}


/**
  * @brief  �ر�����
  * @param  ��
  * @retval ��
  */
void PcdAntennaOff ( void )
{
  ClearBitMask ( TxControlReg, 0x03 );	
}


/**
  * @brief  ��λRC522 
  * @param  ��
  * @retval ��
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
	
  WriteRawRC ( ModeReg, 0x3D );            //���巢�ͺͽ��ճ���ģʽ ��Mifare��ͨѶ��CRC��ʼֵ0x6363
	
  WriteRawRC ( TReloadRegL, 30 );          //16λ��ʱ����λ    
	WriteRawRC ( TReloadRegH, 0 );			     //16λ��ʱ����λ
	
  WriteRawRC ( TModeReg, 0x8D );				   //�����ڲ���ʱ��������
	
  WriteRawRC ( TPrescalerReg, 0x3E );			 //���ö�ʱ����Ƶϵ��
	
	WriteRawRC ( TxAutoReg, 0x40 );				   //���Ʒ����ź�Ϊ100%ASK	
	
}



/**
  * @brief  ����RC522�Ĺ�����ʽ
  * @param  ucType��������ʽ
  * @retval ��
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
		
		PcdAntennaOn ();//������
		
   }	 
}



/**
  * @brief  ͨ��RC522��ISO14443��ͨѶ
  * @param  ucCommand��RC522������
  * @param  pInData��ͨ��RC522���͵���Ƭ������
  * @param  ucInLenByte���������ݵ��ֽڳ���
  * @param  pOutData�����յ��Ŀ�Ƭ��������
  * @param  pOutLenBit���������ݵ�λ����
  * @retval ״ֵ̬= MI_OK���ɹ�
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
     case PCD_AUTHENT:		//Mifare��֤
        ucIrqEn   = 0x12;		//��������ж�����ErrIEn  ��������ж�IdleIEn
        ucWaitFor = 0x10;		//��֤Ѱ���ȴ�ʱ�� ��ѯ�����жϱ�־λ
        break;
     
     case PCD_TRANSCEIVE:		//���շ��� ���ͽ���
        ucIrqEn   = 0x77;		//����TxIEn RxIEn IdleIEn LoAlertIEn ErrIEn TimerIEn
        ucWaitFor = 0x30;		//Ѱ���ȴ�ʱ�� ��ѯ�����жϱ�־λ�� �����жϱ�־λ
        break;
     
     default:
       break;     
  }

  WriteRawRC ( ComIEnReg, ucIrqEn | 0x80 );		//IRqInv��λ�ܽ�IRQ��Status1Reg��IRqλ��ֵ�෴ 
  ClearBitMask ( ComIrqReg, 0x80 );			//Set1��λ����ʱ��CommIRqReg������λ����
  WriteRawRC ( CommandReg, PCD_IDLE );		//д��������
  SetBitMask ( FIFOLevelReg, 0x80 );			//��λFlushBuffer����ڲ�FIFO�Ķ���дָ���Լ�ErrReg��BufferOvfl��־λ�����

  for ( ul = 0; ul < ucInLenByte; ul ++ )
    WriteRawRC ( FIFODataReg, pInData [ ul ] );    		//д���ݽ�FIFOdata
    
  WriteRawRC ( CommandReg, ucCommand );					//д����


  if ( ucCommand == PCD_TRANSCEIVE )
    SetBitMask(BitFramingReg,0x80);  				//StartSend��λ�������ݷ��� ��λ���շ�����ʹ��ʱ����Ч

  ul = 1000;//����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms

  do 														//��֤ ��Ѱ���ȴ�ʱ��	
  {
       ucN = ReadRawRC ( ComIrqReg );							//��ѯ�¼��ж�
       ul --;
  } while ( ( ul != 0 ) && ( ! ( ucN & 0x01 ) ) && ( ! ( ucN & ucWaitFor ) ) );		//�˳�����i=0,��ʱ���жϣ���д��������

  ClearBitMask ( BitFramingReg, 0x80 );					//��������StartSendλ

  if ( ul != 0 )
  {
    if ( ! ( ReadRawRC ( ErrorReg ) & 0x1B ) )			//�������־�Ĵ���BufferOfI CollErr ParityErr ProtocolErr
    {
      cStatus = MI_OK;
      
      if ( ucN & ucIrqEn & 0x01 )					//�Ƿ�����ʱ���ж�
        cStatus = MI_NOTAGERR;   
        
      if ( ucCommand == PCD_TRANSCEIVE )
      {
        ucN = ReadRawRC ( FIFOLevelReg );			//��FIFO�б�����ֽ���
        
        ucLastBits = ReadRawRC ( ControlReg ) & 0x07;	//�����յ����ֽڵ���Чλ��
        
        if ( ucLastBits )
          * pOutLenBit = ( ucN - 1 ) * 8 + ucLastBits;   	//N���ֽ�����ȥ1�����һ���ֽڣ�+���һλ��λ�� ��ȡ����������λ��
        else
          * pOutLenBit = ucN * 8;   					//�����յ����ֽ������ֽ���Ч
        
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
  * @brief Ѱ��
  * @param  ucReq_code��Ѱ����ʽ = 0x52��Ѱ��Ӧ�������з���14443A��׼�Ŀ���Ѱ����ʽ= 0x26��Ѱδ��������״̬�Ŀ�
  * @param  pTagType����Ƭ���ʹ���
             = 0x4400��Mifare_UltraLight
             = 0x0400��Mifare_One(S50)
             = 0x0200��Mifare_One(S70)
             = 0x0800��Mifare_Pro(X))
             = 0x4403��Mifare_DESFire
  * @retval ״ֵ̬= MI_OK���ɹ�
  */
char PcdRequest ( uint8_t ucReq_code, uint8_t * pTagType )
{
  char cStatus;  
  uint8_t ucComMF522Buf [ MAXRLEN ]; 
  uint32_t ulLen;


  ClearBitMask ( Status2Reg, 0x08 );	//����ָʾMIFARECyptol��Ԫ��ͨ�Լ����п�������ͨ�ű����ܵ����
  WriteRawRC ( BitFramingReg, 0x07 );	//	���͵����һ���ֽڵ� ��λ
  SetBitMask ( TxControlReg, 0x03 );	//TX1,TX2�ܽŵ�����źŴ��ݾ����͵��Ƶ�13.56�������ز��ź�

  ucComMF522Buf [ 0 ] = ucReq_code;		//���� ��Ƭ������

  cStatus = PcdComMF522 ( PCD_TRANSCEIVE,	ucComMF522Buf, 1, ucComMF522Buf, & ulLen );	//Ѱ��  

  if ( ( cStatus == MI_OK ) && ( ulLen == 0x10 ) )	//Ѱ���ɹ����ؿ����� 
  {    
     * pTagType = ucComMF522Buf [ 0 ];
     * ( pTagType + 1 ) = ucComMF522Buf [ 1 ];
  }

  else
   cStatus = MI_ERR;

  return cStatus;	 
}

/**
  * @brief  ����ײ
  * @param  pSnr����Ƭ���кţ�4�ֽ�
  * @retval ״ֵ̬= MI_OK���ɹ�
  */
char PcdAnticoll ( uint8_t * pSnr )
{
  char cStatus;
  uint8_t uc, ucSnr_check = 0;
  uint8_t ucComMF522Buf [ MAXRLEN ]; 
  uint32_t ulLen;
  

  ClearBitMask ( Status2Reg, 0x08 );		//��MFCryptol Onλ ֻ�гɹ�ִ��MFAuthent����󣬸�λ������λ
  WriteRawRC ( BitFramingReg, 0x00);		//����Ĵ��� ֹͣ�շ�
  ClearBitMask ( CollReg, 0x80 );			//��ValuesAfterColl���н��յ�λ�ڳ�ͻ�����
 
  ucComMF522Buf [ 0 ] = 0x93;	//��Ƭ����ͻ����
  ucComMF522Buf [ 1 ] = 0x20;
 
  cStatus = PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 2, ucComMF522Buf, & ulLen);//�뿨Ƭͨ��

  if ( cStatus == MI_OK)		//ͨ�ųɹ�
  {
    for ( uc = 0; uc < 4; uc ++ )
    {
       * ( pSnr + uc )  = ucComMF522Buf [ uc ];			//����UID
       ucSnr_check ^= ucComMF522Buf [ uc ];
    }
    
    if ( ucSnr_check != ucComMF522Buf [ uc ] )
      cStatus = MI_ERR;    				 
  }
  
  SetBitMask ( CollReg, 0x80 );
      
  return cStatus;		
}


/**
  * @brief  ��RC522����CRC16
  * @param  pIndata������CRC16������
  * @param  ucLen������CRC16�������ֽڳ���
  * @param   pOutData����ż�������ŵ��׵�ַ
  * @retval ��
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
  * @brief  ѡ����Ƭ
  * @param  pSnr����Ƭ���кţ�4�ֽ�
  * @retval ״ֵ̬= MI_OK���ɹ�
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
  * @brief  ��֤��Ƭ����
  * @param  ucAuth_mode��������֤ģʽ= 0x60����֤A��Կ��������֤ģʽ= 0x61����֤B��Կ
  * @param  uint8_t ucAddr�����ַ
  * @param  pKey������ 
  * @param  pSnr����Ƭ���кţ�4�ֽ�
  * @retval ״ֵ̬= MI_OK���ɹ�
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
  * @brief  д���ݵ�M1��һ��
  * @param  ucAddr�����ַ��0-63����M1���ܹ���16������(ÿ�������У�3�����ݿ�+1�����ƿ�),��64����
  * @param  pData��д������ݣ�16�ֽ�
  * @retval ״ֵ̬= MI_OK���ɹ�
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
  * @brief  ��ȡM1��һ������
  * @param  ucAddr�����ַ��0-63����M1���ܹ���16������(ÿ�������У�3�����ݿ�+1�����ƿ�),��64����
  * @param  pData�����������ݣ�16�ֽ�
  * @retval ״ֵ̬= MI_OK���ɹ�
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
  * @brief  �ж� ucAddr �Ƿ����ݿ�
  * @param  ucAddr������Ե�ַ��0-63��
  * @retval ����ֵ 1:�����ݿ飻0:�������ݿ�
  */
char IsDataBlock( uint8_t ucAddr )
{
  if(ucAddr == 0)
  {
    printf("��0�����Ŀ�0���ɸ���,��Ӧ������в���\r\n");
    return 0;
  }
  /* ��������ݿ�(���������ݿ�0) */
  if( (ucAddr<64) && (((ucAddr+1)%4) != 0) )
  {
    return 1;
  }
  
  printf("���ַ����ָ�����ݿ�\r\n");
  return 0;
}

/**
  * @brief  д pData �ַ�����M1���е����ݿ�
  * @param  ucAddr�����ݿ��ַ������д����ƿ飩
  * @param  pData��д������ݣ�16�ֽ�
  * @retval ״ֵ̬= MI_OK���ɹ�
  */
char PcdWriteString ( uint8_t ucAddr, uint8_t * pData )
{
  /* ��������ݿ�(���������ݿ�0)����д�� */
  if( IsDataBlock(ucAddr) )
  {
    return PcdWrite(ucAddr, pData);
  }

  return MI_ERR;
}

/**
  * @brief  ��ȡM1���е�һ�����ݵ� pData
  * @param  ucAddr�����ݿ��ַ������ȡ���ƿ飩
  * @param  pData�����������ݣ�16�ֽ�
  * @retval ״ֵ̬= MI_OK���ɹ�
  */
char PcdReadString ( uint8_t ucAddr, uint8_t * pData )
{
  /* ��������ݿ�(���������ݿ�0)�����ȡ */
  if( IsDataBlock(ucAddr) )
  {
    return PcdRead(ucAddr, pData);
  }

  return MI_ERR;
}



/**
  * @brief  ���Ƭ��������״̬
  * @param  ��
  * @retval ״ֵ̬= MI_OK���ɹ�
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
//��    �ܣ�д��Ǯ�����
//����˵��: ucAddr[IN]�����ַ
//          pData��д��Ľ��
//��    ��: �ɹ�����MI_OK
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
//��    �ܣ���ȡǮ�����
//����˵��: ucAddr[IN]�����ַ
//          *pData�������Ľ��
//��    ��: �ɹ�����MI_OK
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
		if((ucComMF522Buf[j] != ucComMF522Buf[j+8]) && (ucComMF522Buf[j] != ~ucComMF522Buf[j+4]))//��֤һ���ǲ���Ǯ��������
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
 * @brief �޸Ŀ��ƿ� ucAddr ������A��ע�� ucAddr ָ���ǿ��ƿ�ĵ�ַ��
 *        ����ҪУ������B������BĬ��Ϊ6��0xFF���������BҲ�����ˣ��Ǿ͸Ĳ�������A��
 * @note  ע�⣺�ú�����������Ĭ�ϵĴ洢����ģʽ�����������Ļ����ܳ�������
 * @param ucAddr��[���ƿ�]���ڵĵ�ַ��M1���ܹ���16������(ÿ�������У�3�����ݿ�+1�����ƿ�),��64����
 * @param pKeyA��ָ���µ�����A�ַ����������ַ������� "123456"
 * @retval �ɹ����� MI_OK
 */
char ChangeKeyA( uint8_t ucAddr, uint8_t *pKeyA )
{
	uint8_t KeyBValue[]={0xFF ,0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // B��Կ
  uint8_t ucArray_ID [ 4 ];    /*�Ⱥ���IC�������ͺ�UID(IC�����к�)*/                                                                                         
  uint8_t ucComMF522Buf[16];
  uint8_t j;
  
  /*Ѱ��*/
  while ( PcdRequest ( PICC_REQALL, ucArray_ID ) != MI_OK )
  {    
    printf( "Ѱ��ʧ��\r\n" );
    Delay_us(1000000);
  }
  
  printf ( "Ѱ���ɹ�\n" );
  
  /* ����ͻ�����ж��ſ������д��������Χʱ������ͻ���ƻ������ѡ��һ�Ž��в�����*/
  if ( PcdAnticoll ( ucArray_ID ) == MI_OK )                                                                   
  {
    /* ѡ�п� */
    PcdSelect(ucArray_ID);			

    /* У�� B ���� */
    if( PcdAuthState( PICC_AUTHENT1B, ucAddr, KeyBValue, ucArray_ID ) != MI_OK )
    {
      printf( "��������Bʧ��\r\n" );
    }
    
    // ��ȡ���ƿ���ԭ�������ݣ�ֻҪ�޸�����A���������ݲ��ģ�
    if( PcdRead(ucAddr,ucComMF522Buf) != MI_OK)
    {
      printf( "��ȡ���ƿ�����ʧ��\r\n" );
      return MI_ERR;
    }
    
    /* �޸�����A */
    for(j=0; j<6; j++)
      ucComMF522Buf[j] = pKeyA[j];
    
    if( PcdWrite(ucAddr,ucComMF522Buf) != MI_OK)
    {
      printf( "д�����ݵ����ƿ�ʧ��\r\n" );
      return MI_ERR;
    }
    
    printf( "����A�޸ĳɹ���\r\n" );
    PcdHalt();
    
    return MI_OK;
  }
  
  return MI_ERR;
}


/**
 * @brief ����RC522��������д��16�ֽ����ݵ��� ucAddr
 *        ������У���������B������BĬ��Ϊ6��0xFF��Ҳ����У������A
 *        �÷���WriteDataBlock( 1, "123456789\n", 10); //�ַ�������16���ֽڵĺ��油��д��
 * @note  ע�⣺�ú�����������Ĭ�ϵĴ洢����ģʽ�����������Ļ����ܳ�������
 *        ע�⣺ʹ�øú���Ҫע�� ucAddr �ǿ�0�����ݿ黹�ǿ��ƿ飬�ú����ڲ����Դ����ж�
 * @param ucAddr��������ַ��M1���ܹ���16������(ÿ�������У�3�����ݿ�+1�����ƿ�),��64����
 * @param pData��ָ��Ҫд������ݣ����16���ַ�
 * @param Len��Ҫд�����ݵ��ֽ���
 * @retval �ɹ����� MI_OK
 */
char WriteDataBlock( uint8_t ucAddr, uint8_t *pData, uint8_t Len)
{
	uint8_t KeyBValue[]={0xFF ,0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // B��Կ
  uint8_t ucArray_ID [ 4 ];    /*�Ⱥ���IC�������ͺ�UID(IC�����к�)*/                                                                                         
  uint8_t ucComMF522Buf[16];
  uint8_t j;
  
  /*Ѱ��*/
  while ( PcdRequest ( PICC_REQALL, ucArray_ID ) != MI_OK )
  {    
    printf( "Ѱ��ʧ��\r\n" );
    Delay_us(1000000);
  }
  
  printf ( "Ѱ���ɹ�\n" );
  
  /* ����ͻ�����ж��ſ������д��������Χʱ������ͻ���ƻ������ѡ��һ�Ž��в�����*/
  if ( PcdAnticoll ( ucArray_ID ) == MI_OK )                                                                   
  {
    /* ѡ�п� */
    PcdSelect(ucArray_ID);			

    /* У�� B ���� */
    if( PcdAuthState( PICC_AUTHENT1B, ucAddr, KeyBValue, ucArray_ID ) != MI_OK )
    {
      printf( "��������Bʧ��\r\n" );
    }
    
    /* ���� pData ��� Len ���ַ��� ucComMF522Buf */
    for(j=0; j<16; j++)
    {
      if( j < Len ) ucComMF522Buf[j] = pData[j];
      else ucComMF522Buf[j] = 0; //16���ֽ�����δ�������ֽ���0
    }
    
    /* д���ַ��� */
    if( PcdWrite(ucAddr,ucComMF522Buf) != MI_OK)
    {
      printf( "д�����ݵ����ݿ�ʧ��\r\n" );
      return MI_ERR;
    }
    
    printf( "д�����ݳɹ���\r\n" );
    PcdHalt();
    
    return MI_OK;
  }
  
  return MI_ERR;
}


/**
 * @brief ����RC522�������̶�ȡ�� ucAddr
 *        ������У���������B������BĬ��Ϊ6��0xFF��Ҳ����У������A
 *        �÷���ReadDataBlock( 1, databuf);  // databuf ����Ϊ16�ֽڣ�uint8_t databuf[16];
 * @note  ע�⣺�ú�����������Ĭ�ϵĴ洢����ģʽ�����������Ļ����ܳ�������
 *        ע�⣺ʹ�øú���Ҫע�� ucAddr �ǿ�0�����ݿ黹�ǿ��ƿ飬�ú����ڲ����Դ����ж�
 * @param ucAddr��������ַ��M1���ܹ���16������(ÿ�������У�3�����ݿ�+1�����ƿ�),��64����
 * @param pData��ָ���ȡ�������ݣ�����16���ַ�
 * @retval �ɹ����� MI_OK
 */
char ReadDataBlock( uint8_t ucAddr, uint8_t *pData)
{
	uint8_t KeyBValue[]={0xFF ,0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // B��Կ
  uint8_t ucArray_ID [ 4 ];    /*�Ⱥ���IC�������ͺ�UID(IC�����к�)*/                                                                                         
  
  /*Ѱ��*/
  while ( PcdRequest ( PICC_REQALL, ucArray_ID ) != MI_OK )
  {    
    printf( "Ѱ��ʧ��\r\n" );
    Delay_us(1000000);
  }
  
  printf ( "Ѱ���ɹ�\n" );
  
  /* ����ͻ�����ж��ſ������д��������Χʱ������ͻ���ƻ������ѡ��һ�Ž��в�����*/
  if ( PcdAnticoll ( ucArray_ID ) == MI_OK )                                                                   
  {
    /* ѡ�п� */
    PcdSelect(ucArray_ID);			

    /* У�� B ���� */
    if( PcdAuthState( PICC_AUTHENT1B, ucAddr, KeyBValue, ucArray_ID ) != MI_OK )
    {
      printf( "��������Bʧ��\r\n" );
    }
    
    // ��ȡ���ݿ�������ݵ� pData
    if( PcdRead(ucAddr, pData) != MI_OK)
    {
      printf( "��ȡ���ݿ�ʧ��\r\n" );
      return MI_ERR;
    }
    
    printf( "��ȡ���ݳɹ���\r\n" );
    PcdHalt();
    
    return MI_OK;
  }
  
  return MI_ERR;
}

