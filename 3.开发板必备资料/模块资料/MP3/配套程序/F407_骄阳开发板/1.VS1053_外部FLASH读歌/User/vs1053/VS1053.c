
#include  "VS1053.h"
#include "./usart/bsp_debug_usart.h"
#include "string.h"

SPI_HandleTypeDef VS_SpiHandle;

static __IO uint32_t  SPITimeout = SPIT_LONG_TIMEOUT;   

static uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode);

/**
  * @brief  �ȴ���ʱ�ص�����
  * @param  None.
  * @retval None.
  */
static  uint16_t SPI_TIMEOUT_UserCallback(uint8_t errorCode)
{
  /* �ȴ���ʱ��Ĵ���,���������Ϣ */
  printf("SPI �ȴ���ʱ!errorCode = %d",errorCode);
  return 0;
}


//VS1053Ĭ�����ò���
_vs1053_obj vsset=
{
	220,	//����:220
	6,		//�������� 60Hz
	15,		//�������� 15dB	
	10,		//�������� 10Khz	
	15,		//�������� 10.5dB
	0,		//�ռ�Ч��	
};

/*******************************************************************************
* Function Name  : SPI_FLASH_SendByte
* Description    : Sends a byte through the SPI interface and return the byte
*                  received from the SPI bus.
* Input          : byte : byte to send.
* Output         : None
* Return         : The value of the received byte.
*******************************************************************************/
unsigned char SPI2_ReadWriteByte(uint8_t  byte)
{
  
  SPITimeout = SPIT_FLAG_TIMEOUT;

  /* �ȴ����ͻ�����Ϊ�գ�TXE�¼� */
  while (__HAL_SPI_GET_FLAG( &VS_SpiHandle, SPI_FLAG_TXE ) == RESET)
   {
    if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(0);
   }

  /* д�����ݼĴ�������Ҫд�������д�뷢�ͻ����� */
  WRITE_REG(VS_SpiHandle.Instance->DR, byte);

  SPITimeout = SPIT_FLAG_TIMEOUT;

  /* �ȴ����ջ������ǿգ�RXNE�¼� */
  while (__HAL_SPI_GET_FLAG( &VS_SpiHandle, SPI_FLAG_RXNE ) == RESET)
   {
    if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(1);
   }

  /* ��ȡ���ݼĴ�������ȡ���ջ��������� */
  return READ_REG(VS_SpiHandle.Instance->DR);

}

////////////////////////////////////////////////////////////////////////////////
//��ֲʱ��Ľӿ�
//data:Ҫд�������
//����ֵ:����������
uint8_t VS_SPI_ReadWriteByte(uint8_t data)
{			  	 
	return SPI2_ReadWriteByte(data);	  
}
static void SPI_SetSpeed(uint8_t SpeedSet)
{
	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI2->CR1&=0XFFC7; 
	SPI2->CR1|=SpeedSet;
	__HAL_SPI_ENABLE(&VS_SpiHandle); 
}

void VS_SPI_SpeedLow(void)
{								 
	SPI_SetSpeed(SPI_BAUDRATEPRESCALER_32);//���õ�����ģʽ 
}

void VS_SPI_SpeedHigh(void)
{						  
	SPI_SetSpeed(SPI_BAUDRATEPRESCALER_8);//���õ�����ģʽ		 
}
//��ʼ��VS1053��IO��	 
void VS_Init(void)
{
  /*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
  GPIO_InitTypeDef GPIO_InitStructure;
  
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();	
  SPIx_CLK_ENABLE();
  
	/*ѡ��Ҫ���Ƶ�GPIO����*/															   
	GPIO_InitStructure.Pin = VS_DREQ;	
	GPIO_InitStructure.Mode  =GPIO_MODE_INPUT;  
	GPIO_InitStructure.Pull  = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(VS_GPIO_DREQ_PORT, &GPIO_InitStructure);	
															 
	GPIO_InitStructure.Pin = VS_RST;	
	GPIO_InitStructure.Mode  =GPIO_MODE_OUTPUT_PP; 
	HAL_GPIO_Init(VS_GPIO_RST_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = VS_XCS;
	HAL_GPIO_Init(VS_SPIGPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = VS_XDCS;
	HAL_GPIO_Init(VS_GPIO_XDCS_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin =VS_SCLK | VS_MISO | VS_MOSI;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStructure.Alternate = GPIO_AF5_SPI2;
	HAL_GPIO_Init(VS_SPIGPIO_PORT, &GPIO_InitStructure);  
  //-----SPI����---------------------------------------------------------------
  
  VS_SpiHandle.Instance               = VS_SPI;
  VS_SpiHandle.Init.Mode              = SPI_MODE_MASTER;
  VS_SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  VS_SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
  VS_SpiHandle.Init.CLKPhase          = SPI_PHASE_2EDGE;
  VS_SpiHandle.Init.CLKPolarity       = SPI_POLARITY_HIGH;
  VS_SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
  VS_SpiHandle.Init.CRCPolynomial     = 7;
  VS_SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
  VS_SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
  VS_SpiHandle.Init.NSS               = SPI_NSS_SOFT;
  VS_SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;

  HAL_SPI_Init(&VS_SpiHandle); 
  
  __HAL_SPI_ENABLE(&VS_SpiHandle);   
	
}	  
////////////////////////////////////////////////////////////////////////////////	 	  
//��λVS10XX
void VS_Soft_Reset(void)
{	 
	uint8_t retry=0;  				   
	while(VS_DREQ_IN==0); 					//�ȴ������λ����	   
	VS_SPI_ReadWriteByte(0Xff);			//��������
	retry=0;
	while(VS_RD_Reg(SPI_MODE)!=0x0800)	// �����λ,��ģʽ  
	{
		VS_WR_Cmd(SPI_MODE,0x0804);		// �����λ,��ģʽ	    
		HAL_Delay(2);//�ȴ�����1.35ms 
		if(retry++>100)break; 	    
	}	 		 
	while(VS_DREQ_IN==0);//�ȴ������λ����	 
	retry=0;
	while(VS_RD_Reg(SPI_CLOCKF)!=0X9800)//����VS1053��ʱ��,3��Ƶ ,1.5xADD 
	{
		VS_WR_Cmd(SPI_CLOCKF,0X9800);	//����VS1053��ʱ��,3��Ƶ ,1.5xADD
		if(retry++>100)break; 	    
	}	 
	HAL_Delay(20);
}
//Ӳ��λMP3
//����1:��λʧ��;0:��λ�ɹ�	   
uint8_t VS_HD_Reset(void)
{
	uint8_t retry=0;
	VS_RST_CLR;
	HAL_Delay(20);
	VS_XDCS_SET;//ȡ�����ݴ���
	VS_XCS_SET;//ȡ�����ݴ���
	VS_RST_SET;	   
	while(VS_DREQ_IN==0&&retry<200)//�ȴ�DREQΪ��
	{
		retry++;
		HAL_Delay(1);
	};
	HAL_Delay(20);	
	if(retry>=200)return 1;
	else return 0;	    		 
}
//���Ҳ��� 
void VS_Sine_Test(void)
{											    
	VS_HD_Reset();	 
	VS_WR_Cmd(0x0b,0X2020);	  //��������	 
 	VS_WR_Cmd(SPI_MODE,0x0820);//����VS10XX�Ĳ���ģʽ     
	while(VS_DREQ_IN==0);     //�ȴ�DREQΪ��
	//printf("mode sin:%x\n",VS_RD_Reg(SPI_MODE));
 	//��VS1053�������Ҳ������0x53 0xef 0x6e n 0x00 0x00 0x00 0x00
 	//����n = 0x24, �趨VS1053�����������Ҳ���Ƶ��ֵ��������㷽����VS1053��datasheet
	VS_SPI_SpeedLow();//���� 
	VS_XDCS_CLR;//ѡ�����ݴ���
	VS_SPI_ReadWriteByte(0x53);
	VS_SPI_ReadWriteByte(0xef);
	VS_SPI_ReadWriteByte(0x6e);
	VS_SPI_ReadWriteByte(0x24);
	VS_SPI_ReadWriteByte(0x00);
	VS_SPI_ReadWriteByte(0x00);
	VS_SPI_ReadWriteByte(0x00);
	VS_SPI_ReadWriteByte(0x00);
	HAL_Delay(100);
	VS_XDCS_SET;
    //�˳����Ҳ���
    VS_XDCS_CLR;//ѡ�����ݴ���
	VS_SPI_ReadWriteByte(0x45);
	VS_SPI_ReadWriteByte(0x78);
	VS_SPI_ReadWriteByte(0x69);
	VS_SPI_ReadWriteByte(0x74);
	VS_SPI_ReadWriteByte(0x00);
	VS_SPI_ReadWriteByte(0x00);
	VS_SPI_ReadWriteByte(0x00);
	VS_SPI_ReadWriteByte(0x00);
	HAL_Delay(100);
	VS_XDCS_SET;		 

}	 
//ram ���� 
//����ֵ:RAM���Խ��
// VS1053����õ���ֵΪ0x83FF����������;																			 
uint16_t VS_Ram_Test(void)
{ 
	VS_HD_Reset();     
 	VS_WR_Cmd(SPI_MODE,0x0820);// ����VS10XX�Ĳ���ģʽ
	while (VS_DREQ_IN==0); // �ȴ�DREQΪ��			   
 	VS_SPI_SpeedLow();//���� 
	VS_XDCS_CLR;  		    // xDCS = 1��ѡ��VS10XX�����ݽӿ�
	VS_SPI_ReadWriteByte(0x4d);
	VS_SPI_ReadWriteByte(0xea);
	VS_SPI_ReadWriteByte(0x6d);
	VS_SPI_ReadWriteByte(0x54);
	VS_SPI_ReadWriteByte(0x00);
	VS_SPI_ReadWriteByte(0x00);
	VS_SPI_ReadWriteByte(0x00);
	VS_SPI_ReadWriteByte(0x00);
	HAL_Delay(150);  
	VS_XDCS_SET;
	return VS_RD_Reg(SPI_HDAT0);// VS1053����õ���ֵΪ0x83FF����������;   
}     					   
//��VS1053д����
//address:�����ַ
//data:��������
void VS_WR_Cmd(uint8_t address,uint16_t data)
{  
	while(VS_DREQ_IN==0);//�ȴ�����		  
	VS_SPI_SpeedLow();//���� 	   
	VS_XDCS_SET; 
	VS_XCS_CLR;	 
	VS_SPI_ReadWriteByte(VS_WRITE_COMMAND);//����VS10XX��д����
	VS_SPI_ReadWriteByte(address); 	//��ַ
	VS_SPI_ReadWriteByte(data>>8); 	//���͸߰�λ
	VS_SPI_ReadWriteByte(data);	 	//�ڰ�λ
	VS_XCS_SET;           
	VS_SPI_SpeedHigh();				//����	   
} 
//��VS10XXд����
//data:Ҫд�������
void VS_WR_Data(uint8_t data)
{
	VS_SPI_SpeedHigh();//����,��VS1003B,���ֵ���ܳ���36.864/4Mhz����������Ϊ9M 
	VS_XDCS_CLR;   
	VS_SPI_ReadWriteByte(data);
	VS_XDCS_SET;      
}         
//��VS10XX�ļĴ���           
//address���Ĵ�����ַ
//����ֵ��������ֵ
//ע�ⲻҪ�ñ��ٶ�ȡ,�����
uint16_t VS_RD_Reg(uint8_t address)
{ 
	uint16_t temp=0;    	 
  while(VS_DREQ_IN==0);//�ǵȴ�����״̬ 		  
	VS_SPI_SpeedLow();//���� 
	VS_XDCS_SET;       
	VS_XCS_CLR;      
	VS_SPI_ReadWriteByte(VS_READ_COMMAND);	//����VS10XX�Ķ�����
	VS_SPI_ReadWriteByte(address);       	//��ַ
	temp=VS_SPI_ReadWriteByte(0xff); 		//��ȡ���ֽ�
	temp=temp<<8;
	temp+=VS_SPI_ReadWriteByte(0xff); 		//��ȡ���ֽ�
	VS_XCS_SET;     
	VS_SPI_SpeedHigh();//����	  
   return temp; 
}  
//��ȡVS10xx��RAM
//addr��RAM��ַ
//����ֵ��������ֵ
uint16_t VS_WRAM_Read(uint16_t addr) 
{ 
	uint16_t res;			   	  
 	VS_WR_Cmd(SPI_WRAMADDR, addr); 
	res=VS_RD_Reg(SPI_WRAM);  
 	return res;
} 
//дVS10xx��RAM
//addr��RAM��ַ
//val:Ҫд���ֵ 
void VS_WRAM_Write(uint16_t addr,uint16_t val) 
{  		   	  
 	VS_WR_Cmd(SPI_WRAMADDR,addr);	//дRAM��ַ 
	while(VS_DREQ_IN==0); 				//�ȴ�����	   
	VS_WR_Cmd(SPI_WRAM,val); 		//дRAMֵ 
} 
//���ò����ٶȣ���VS1053��Ч�� 
//t:0,1,�����ٶ�;2,2���ٶ�;3,3���ٶ�;4,4����;�Դ�����
void VS_Set_Speed(uint8_t t)
{
	VS_WRAM_Write(0X1E04,t);		//д�벥���ٶ� 
}
//FOR WAV HEAD0 :0X7761 HEAD1:0X7665    
//FOR MIDI HEAD0 :other info HEAD1:0X4D54
//FOR WMA HEAD0 :data speed HEAD1:0X574D
//FOR MP3 HEAD0 :data speed HEAD1:ID
//������Ԥ��ֵ,�ײ�III
const uint16_t bitrate[2][16]=
{ 
{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,0}, 
{0,32,40,48,56,64,80,96,112,128,160,192,224,256,320,0}
};
//����Kbps�Ĵ�С
//����ֵ���õ�������
uint16_t VS_Get_HeadInfo(void)
{
	unsigned int HEAD0;
	unsigned int HEAD1;  
 	HEAD0=VS_RD_Reg(SPI_HDAT0); 
  HEAD1=VS_RD_Reg(SPI_HDAT1);
  //printf("(H0,H1):%x,%x\n",HEAD0,HEAD1);
	switch(HEAD1)
	{        
			case 0x7665://WAV��ʽ
			case 0X4D54://MIDI��ʽ 
			case 0X4154://AAC_ADTS
			case 0X4144://AAC_ADIF
			case 0X4D34://AAC_MP4/M4A
			case 0X4F67://OGG
			case 0X574D://WMA��ʽ
			case 0X664C://FLAC��ʽ
			{
					//printf("HEAD0:%d\n",HEAD0);
					HEAD1=HEAD0*2/25;//�൱��*8/100
					if((HEAD1%10)>5)return HEAD1/10+1;//��С�����һλ��������
					else return HEAD1/10;
			}
			default://MP3��ʽ,�����˽ײ�III�ı�
			{
					HEAD1>>=3;
					HEAD1=HEAD1&0x03; 
					if(HEAD1==3)HEAD1=1;
					else HEAD1=0;
					return bitrate[HEAD1][HEAD0>>12];
			}
	}  
}
//�õ�ƽ���ֽ���
//����ֵ��ƽ���ֽ����ٶ�
uint32_t VS_Get_ByteRate(void)
{
	return VS_WRAM_Read(0X1E05);//ƽ��λ��
}
//�õ���Ҫ��������
//����ֵ:��Ҫ��������
uint16_t VS_Get_EndFillByte(void)
{
	return VS_WRAM_Read(0X1E06);//����ֽ�
}  
//����һ����Ƶ����
//�̶�Ϊ32�ֽ�
//����ֵ:0,���ͳɹ�
//		 1,VS10xx��ȱ����,��������δ�ɹ�����    
uint8_t VS_Send_MusicData(uint8_t* buf)
{
	uint8_t n;
	if(VS_DREQ_IN!=0)  //�����ݸ�VS10XX
	{			   	 
		VS_XDCS_CLR;  
        for(n=0;n<32;n++)
		{
			VS_SPI_ReadWriteByte(buf[n]);	 			
		}
		VS_XDCS_SET;     				   
	}else return 1;
	return 0;//�ɹ�������
}
//�и�
//ͨ���˺����и裬��������л���������				
void VS_Restart_Play(void)
{
	uint16_t temp;
	uint16_t i;
	uint8_t n;	  
	uint8_t vsbuf[32];
	for(n=0;n<32;n++)vsbuf[n]=0;//����
	temp=VS_RD_Reg(SPI_MODE);	//��ȡSPI_MODE������
	temp|=1<<3;					//����SM_CANCELλ
	temp|=1<<2;					//����SM_LAYER12λ,������MP1,MP2
	VS_WR_Cmd(SPI_MODE,temp);	//����ȡ����ǰ����ָ��
	for(i=0;i<2048;)			//����2048��0,�ڼ��ȡSM_CANCELλ.���Ϊ0,���ʾ�Ѿ�ȡ���˵�ǰ����
	{
		if(VS_Send_MusicData(vsbuf)==0)//ÿ����32���ֽں���һ��
		{
			i+=32;						//������32���ֽ�
   			temp=VS_RD_Reg(SPI_MODE);	//��ȡSPI_MODE������
 			if((temp&(1<<3))==0)break;	//�ɹ�ȡ����
		}	
	}
	if(i<2048)//SM_CANCEL����
	{
		temp=VS_Get_EndFillByte()&0xff;//��ȡ����ֽ�
		for(n=0;n<32;n++)vsbuf[n]=temp;//����ֽڷ�������
		for(i=0;i<2052;)
		{
			if(VS_Send_MusicData(vsbuf)==0)i+=32;//���	  
		}   	
	}else VS_Soft_Reset();  	//SM_CANCEL���ɹ�,�����,��Ҫ��λ 	  
	temp=VS_RD_Reg(SPI_HDAT0); 
    temp+=VS_RD_Reg(SPI_HDAT1);
	if(temp)					//��λ,����û�гɹ�ȡ��,��ɱ���,Ӳ��λ
	{
		VS_HD_Reset();		   	//Ӳ��λ
		VS_Soft_Reset();  		//��λ 
	} 
}
//�������ʱ��                          
void VS_Reset_DecodeTime(void)
{
	VS_WR_Cmd(SPI_DECODE_TIME,0x0000);
	VS_WR_Cmd(SPI_DECODE_TIME,0x0000);//��������
}
//�õ�mp3�Ĳ���ʱ��n sec
//����ֵ������ʱ��
uint16_t VS_Get_DecodeTime(void)
{ 		
	uint16_t dt=0;	 
	dt=VS_RD_Reg(SPI_DECODE_TIME);      
 	return dt;
} 	    					  
//vs10xxװ��patch.
//patch��patch�׵�ַ
//len��patch����
void VS_Load_Patch(uint16_t *patch,uint16_t len) 
{
	uint16_t i; 
	uint16_t addr, n, val; 	  			   
	for(i=0;i<len;) 
	{ 
		addr = patch[i++]; 
		n    = patch[i++]; 
		if(n & 0x8000U) //RLE run, replicate n samples 
		{ 
			n  &= 0x7FFF; 
			val = patch[i++]; 
			while(n--)VS_WR_Cmd(addr, val);  
		}else //copy run, copy n sample 
		{ 
			while(n--) 
			{ 
				val = patch[i++]; 
				VS_WR_Cmd(addr, val); 
			} 
		} 
	} 	
} 		  	  
//�趨VS10XX���ŵ������͸ߵ���
//volx:������С(0~254)
void VS_Set_Vol(uint8_t volx)
{
    uint16_t volt=0; 			//�ݴ�����ֵ
    volt=254-volx;			//ȡ��һ��,�õ����ֵ,��ʾ���ı�ʾ 
	volt<<=8;
    volt+=254-volx;			//�õ��������ú��С
    VS_WR_Cmd(SPI_VOL,volt);//������ 
}
//�趨�ߵ�������
//bfreq:��Ƶ����Ƶ��	2~15(��λ:10Hz)
//bass:��Ƶ����			0~15(��λ:1dB)
//tfreq:��Ƶ����Ƶ�� 	1~15(��λ:Khz)
//treble:��Ƶ����  	 	0~15(��λ:1.5dB,С��9��ʱ��Ϊ����)
void VS_Set_Bass(uint8_t bfreq,uint8_t bass,uint8_t tfreq,uint8_t treble)
{
    uint16_t bass_set=0; //�ݴ������Ĵ���ֵ
    signed char temp=0;   	 
	if(treble==0)temp=0;	   		//�任
	else if(treble>8)temp=treble-8;
 	else temp=treble-9;  
	bass_set=temp&0X0F;				//�����趨
	bass_set<<=4;
	bass_set+=tfreq&0xf;			//��������Ƶ��
	bass_set<<=4;
	bass_set+=bass&0xf;				//�����趨
	bass_set<<=4;
	bass_set+=bfreq&0xf;			//��������    
	VS_WR_Cmd(SPI_BASS,bass_set);	//BASS 
}
//�趨��Ч
//eft:0,�ر�;1,��С;2,�е�;3,���.
void VS_Set_Effect(uint8_t eft)
{
	uint16_t temp;	 
	temp=VS_RD_Reg(SPI_MODE);	//��ȡSPI_MODE������
	if(eft&0X01)temp|=1<<4;		//�趨LO
	else temp&=~(1<<5);			//ȡ��LO
	if(eft&0X02)temp|=1<<7;		//�趨HO
	else temp&=~(1<<7);			//ȡ��HO						   
	VS_WR_Cmd(SPI_MODE,temp);	//�趨ģʽ    
}	 

///////////////////////////////////////////////////////////////////////////////
//��������,��Ч��.
void VS_Set_All(void) 				
{			 
	VS_Set_Vol(vsset.mvol);			//��������
	VS_Set_Bass(vsset.bflimit,vsset.bass,vsset.tflimit,vsset.treble);  
	VS_Set_Effect(vsset.effect);	//���ÿռ�Ч��
}


/*--------------  END OF FILE -----------------------*/
