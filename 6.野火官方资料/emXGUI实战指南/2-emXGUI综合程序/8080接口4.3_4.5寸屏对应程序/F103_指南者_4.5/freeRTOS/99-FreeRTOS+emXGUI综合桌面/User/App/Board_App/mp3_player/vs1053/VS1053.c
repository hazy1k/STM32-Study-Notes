#include "ff.h"
#include "gui_drv_cfg.h"
#include  "VS1053.h"
#include "board.h"
#include "string.h"
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


/*�ȴ���ʱʱ��*/
#define VSSPIT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define VSSPIT_LONG_TIMEOUT         ((uint32_t)(10 * VSSPIT_FLAG_TIMEOUT))

static __IO uint32_t  VSSPITimeout = VSSPIT_LONG_TIMEOUT;

/*******************************************************************************
* Function Name  : SPI_FLASH_SendByte
* Description    : Sends a byte through the SPI interface and return the byte
*                  received from the SPI bus.
* Input          : byte : byte to send.
* Output         : None
* Return         : The value of the received byte.
*******************************************************************************/
unsigned char SPI2_ReadWriteByte(unsigned char writedat)
{
	/* Loop while DR register in not emplty */
	while(SPI_I2S_GetFlagStatus(VS_SPI,SPI_I2S_FLAG_TXE) == RESET);
	
	/* Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(VS_SPI, writedat);
  
	VSSPITimeout = VSSPIT_LONG_TIMEOUT;
  
	/* Wait to receive a byte */
	while(SPI_I2S_GetFlagStatus(VS_SPI, SPI_I2S_FLAG_RXNE) == RESET && VSSPITimeout--)
  {
    if (VSSPITimeout == 0)    // ��ʱ�˳�
    {
      printf("VS SPI Timeout");
      return 0;
    }
  }
	
	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(VS_SPI);
}

////////////////////////////////////////////////////////////////////////////////
//��ֲʱ��Ľӿ�
//data:Ҫд�������
//����ֵ:����������
u8 VS_SPI_ReadWriteByte(u8 data)
{			  	 
	return SPI2_ReadWriteByte(data);	  
}
static void SPI_SetSpeed(u8 SpeedSet)
{
	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI2->CR1&=0XFFC7; 
	SPI2->CR1|=SpeedSet;
	SPI_Cmd(SPI2,ENABLE); 
}

void VS_SPI_SpeedLow(void)
{								 
	SPI_SetSpeed(SPI_BaudRatePrescaler_32);//���õ�����ģʽ 
}

void VS_SPI_SpeedHigh(void)
{						  
	SPI_SetSpeed(SPI_BaudRatePrescaler_8);//���õ�����ģʽ		 
}
//��ʼ��VS1053��IO��	 
void VS_Init(void)
{
	
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
  SPI_Cmd(VS_SPI, DISABLE); 
	RCC_APB2PeriphClockCmd(VS_SPIGPIO_CLK|VS_GPIO_DREQ_CLK|VS_GPIO_RST_CLK|VS_GPIO_XDCS_CLK, ENABLE);
	
 	GPIO_InitStructure.GPIO_Pin = VS_DREQ;				 //DREQ
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //����
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(VS_GPIO_DREQ_PORT, &GPIO_InitStructure);

 	GPIO_InitStructure.GPIO_Pin = VS_RST;	 //PB9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_Init(VS_GPIO_RST_PORT, &GPIO_InitStructure);
  
//  GPIO_InitStructure.GPIO_Pin = VS_HORN;	 //PE5
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
//	GPIO_Init(VS_GPIO_HORN_PORT, &GPIO_InitStructure);
//  
//  GPIO_SetBits(VS_GPIO_HORN_PORT, VS_HORN);    // Ĭ�Ͻ�������
	
	/*��ʼ��STM32 SPI2�ӿ�*/
	RCC_APB1PeriphClockCmd(VS_SPI_CLK, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = VS_SCLK | VS_MISO | VS_MOSI;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		   // �������
	GPIO_Init(VS_SPIGPIO_PORT, &GPIO_InitStructure);
   
	GPIO_InitStructure.GPIO_Pin = VS_XCS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   // �������
	GPIO_Init(VS_SPIGPIO_PORT, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = VS_XDCS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   // �������
	GPIO_Init(VS_GPIO_XDCS_PORT, &GPIO_InitStructure);
	
	/* SPI2 ���� */ 
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(VS_SPI, &SPI_InitStructure);
	
	/* ʹ�� SPI2 */
	SPI_Cmd(VS_SPI, ENABLE); 
	SPI2_ReadWriteByte(0xff);//��������	
	
}	  
////////////////////////////////////////////////////////////////////////////////	 	  
//��λVS10XX
void VS_Soft_Reset(void)
{	 
	u8 retry=0;  				   
	while(VS_DREQ_IN==0); 					//�ȴ������λ����	   
	VS_SPI_ReadWriteByte(0Xff);			//��������
	retry=0;
	while(VS_RD_Reg(SPI_MODE)!=0x0800)	// �����λ,��ģʽ  
	{
		VS_WR_Cmd(SPI_MODE,0x0804);		// �����λ,��ģʽ	    
		GUI_msleep(2);//�ȴ�����1.35ms 
		if(retry++>100)break; 	    
	}	 		 
	while(VS_DREQ_IN==0);//�ȴ������λ����	 
	retry=0;
	while(VS_RD_Reg(SPI_CLOCKF)!=0X9800)//����VS1053��ʱ��,3��Ƶ ,1.5xADD 
	{
		VS_WR_Cmd(SPI_CLOCKF,0X9800);	//����VS1053��ʱ��,3��Ƶ ,1.5xADD
		if(retry++>100)break; 	    
	}	 
	GUI_msleep(20);
}
//Ӳ��λMP3
//����1:��λʧ��;0:��λ�ɹ�	   
u8 VS_HD_Reset(void)
{
	u8 retry=0;
	VS_RST_CLR;
	GUI_msleep(20);
	VS_XDCS_SET;//ȡ�����ݴ���
	VS_XCS_SET;//ȡ�����ݴ���
	VS_RST_SET;	   
	while(VS_DREQ_IN==0&&retry<20)//�ȴ�DREQΪ��
	{
		retry++;
		GUI_msleep(1);
	};
	GUI_msleep(20);	
	if(retry>=20)return 1;
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
	GUI_msleep(100);
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
	GUI_msleep(100);
	VS_XDCS_SET;		 

//  //�ٴν������Ҳ��Բ�����nֵΪ0x44���������Ҳ���Ƶ������Ϊ�����ֵ
//  VS_XDCS_CLR;//ѡ�����ݴ���      
//	VS_SPI_ReadWriteByte(0x53);
//	VS_SPI_ReadWriteByte(0xef);
//	VS_SPI_ReadWriteByte(0x6e);
//	VS_SPI_ReadWriteByte(0x44);
//	VS_SPI_ReadWriteByte(0x00);
//	VS_SPI_ReadWriteByte(0x00);
//	VS_SPI_ReadWriteByte(0x00);
//	VS_SPI_ReadWriteByte(0x00);
//	GUI_msleep(100);
// 	VS_XDCS_SET;
//    //�˳����Ҳ���
//    VS_XDCS_CLR;//ѡ�����ݴ���
//	VS_SPI_ReadWriteByte(0x45);
//	VS_SPI_ReadWriteByte(0x78);
//	VS_SPI_ReadWriteByte(0x69);
//	VS_SPI_ReadWriteByte(0x74);
//	VS_SPI_ReadWriteByte(0x00);
//	VS_SPI_ReadWriteByte(0x00);
//	VS_SPI_ReadWriteByte(0x00);
//	VS_SPI_ReadWriteByte(0x00);
//	GUI_msleep(100);
//	VS_XDCS_SET; 
}	 
//ram ���� 
//����ֵ:RAM���Խ��
// VS1053����õ���ֵΪ0x83FF����������;																			 
u16 VS_Ram_Test(void)
{ 
	VS_HD_Reset();     
 	VS_WR_Cmd(SPI_MODE,0x0820);// ����VS10XX�Ĳ���ģʽ
	while (VS_DREQ_IN==0){}; // �ȴ�DREQΪ��			   
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
	GUI_msleep(150);  
	VS_XDCS_SET;
	return VS_RD_Reg(SPI_HDAT0);// VS1053����õ���ֵΪ0x83FF����������;   
}     					   
//��VS1053д����
//address:�����ַ
//data:��������
void VS_WR_Cmd(u8 address,u16 data)
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
void VS_WR_Data(u8 data)
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
u16 VS_RD_Reg(u8 address)
{ 
	u16 temp=0;    	 
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
u16 VS_WRAM_Read(u16 addr) 
{ 
	u16 res;			   	  
 	VS_WR_Cmd(SPI_WRAMADDR, addr); 
	res=VS_RD_Reg(SPI_WRAM);  
 	return res;
} 
//дVS10xx��RAM
//addr��RAM��ַ
//val:Ҫд���ֵ 
void VS_WRAM_Write(u16 addr,u16 val) 
{  		   	  
 	VS_WR_Cmd(SPI_WRAMADDR,addr);	//дRAM��ַ 
	while(VS_DREQ_IN==0); 				//�ȴ�����	   
	VS_WR_Cmd(SPI_WRAM,val); 		//дRAMֵ 
} 
//���ò����ٶȣ���VS1053��Ч�� 
//t:0,1,�����ٶ�;2,2���ٶ�;3,3���ٶ�;4,4����;�Դ�����
void VS_Set_Speed(u8 t)
{
	VS_WRAM_Write(0X1E04,t);		//д�벥���ٶ� 
}
//FOR WAV HEAD0 :0X7761 HEAD1:0X7665    
//FOR MIDI HEAD0 :other info HEAD1:0X4D54
//FOR WMA HEAD0 :data speed HEAD1:0X574D
//FOR MP3 HEAD0 :data speed HEAD1:ID
//������Ԥ��ֵ,�ײ�III
const u16 bitrate[2][16]=
{ 
{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,0}, 
{0,32,40,48,56,64,80,96,112,128,160,192,224,256,320,0}
};
//����Kbps�Ĵ�С
//����ֵ���õ�������
u16 VS_Get_HeadInfo(void)
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
u32 VS_Get_ByteRate(void)
{
	return VS_WRAM_Read(0X1E05);//ƽ��λ��
}
//�õ���Ҫ��������
//����ֵ:��Ҫ��������
u16 VS_Get_EndFillByte(void)
{
	return VS_WRAM_Read(0X1E06);//����ֽ�
}  
//����һ����Ƶ����
//�̶�Ϊ32�ֽ�
//����ֵ:0,���ͳɹ�
//		 1,VS10xx��ȱ����,��������δ�ɹ�����    
u8 VS_Send_MusicData(u8* buf)
{
	u8 n;
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
	u16 temp;
	u16 i;
	u8 n;	  
	u8 vsbuf[32];
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
u16 VS_Get_DecodeTime(void)
{ 		
	u16 dt=0;	 
	dt=VS_RD_Reg(SPI_DECODE_TIME);      
 	return dt;
} 	    					  
//vs10xxװ��patch.
//patch��patch�׵�ַ
//len��patch����
void VS_Load_Patch(u16 *patch,u16 len) 
{
	u16 i; 
	u16 addr, n, val; 	  			   
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
void VS_Set_Vol(u8 volx)
{
    u16 volt=0; 			//�ݴ�����ֵ
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
void VS_Set_Bass(u8 bfreq,u8 bass,u8 tfreq,u8 treble)
{
    u16 bass_set=0; //�ݴ������Ĵ���ֵ
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
void VS_Set_Effect(u8 eft)
{
	u16 temp;	 
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

/***************��������****************/
//����ѡ�� DISABLE������ ENABLE������
void VS_Horn_Set(FunctionalState State)
{
  if (State == DISABLE)
  {
//    GPIO_SetBits(VS_GPIO_HORN_PORT, VS_HORN);    // ��������
  }
  else if (State == ENABLE)
  {
//    GPIO_ResetBits(VS_GPIO_HORN_PORT, VS_HORN);    // ���ý�������
  }
}

/*--------------  ������VS1053�������� -------------------------*/
/*--------------  ���濪ʼ�����ֲ��Ų��� -------------------------*/

/*
************************************************************************
*						  FatFs
************************************************************************
*/
#if  0
FRESULT result;
FIL file __EXRAM;
UINT bw;

static uint8_t  buffer[BUFSIZE] __EXRAM;

//���Ÿ���
void vs1053_player_song(uint8_t *filepath)
{
	uint16_t i=0;
	
	VS_Restart_Play();  					
	VS_Set_All();        							 
	VS_Reset_DecodeTime();
	
	if(strstr((const char*)filepath,".flac")||strstr((const char*)filepath,".FLAC"))
		VS_Load_Patch((u16*)vs1053b_patch,VS1053B_PATCHLEN);
	
	result=f_open(&file,(const TCHAR*)filepath,FA_READ);

	if(result==0)
	{ 
		VS_SPI_SpeedHigh();				   
		while(1)
		{
			i=0;	
			result=f_read(&file,buffer,BUFSIZE,(UINT*)&bw);		
			do
			{  	
				if(VS_Send_MusicData(buffer+i)==0)
				{
					i+=32;
				}
			}while(i<bw);
			
			if(bw!=BUFSIZE||result!=0)
			{
				break;	  
			}
		}
		f_close(&file);
	}	  					     	  
}
#endif


/*----------------- ¼�����ܺ��� --------------------------*/
//VS1053��WAV¼����bug,���plugin��������������� 							    
const uint16_t wav_plugin[40]=/* Compressed plugin */ 
{ 
0x0007, 0x0001, 0x8010, 0x0006, 0x001c, 0x3e12, 0xb817, 0x3e14, /* 0 */ 
0xf812, 0x3e01, 0xb811, 0x0007, 0x9717, 0x0020, 0xffd2, 0x0030, /* 8 */ 
0x11d1, 0x3111, 0x8024, 0x3704, 0xc024, 0x3b81, 0x8024, 0x3101, /* 10 */ 
0x8024, 0x3b81, 0x8024, 0x3f04, 0xc024, 0x2808, 0x4800, 0x36f1, /* 18 */ 
0x9811, 0x0007, 0x0001, 0x8028, 0x0006, 0x0002, 0x2a00, 0x040e,  
}; 

//����PCM ¼��ģʽ
//agc:0,�Զ�����.1024�൱��1��,512�൱��0.5��,���ֵ65535=64��		  
void recoder_enter_rec_mode(_recorder_obj *recset)
{
	//�����IMA ADPCM,�����ʼ��㹫ʽ����:
 	//������=CLKI/256*d;	
	//����d=0,��2��Ƶ,�ⲿ����Ϊ12.288M.��ôFc=(2*12288000)/256*6=16Khz
	//���������PCM,������ֱ�Ӿ�д����ֵ 
  VS_WR_Cmd(SPI_BASS,0x0000);    
 	VS_WR_Cmd(SPI_AICTRL0,recset->samplerate*8000);	//���ò�����
 	VS_WR_Cmd(SPI_AICTRL1,recset->agc*1024/2);		//��������,0,�Զ�����.1024�൱��1��,512�൱��0.5��,���ֵ65535=64��	
 	VS_WR_Cmd(SPI_AICTRL2,0);		//�����������ֵ,0,�������ֵ65536=64X
 	VS_WR_Cmd(SPI_AICTRL3,6+recset->channel);		//4������PCMģʽ + 2�� ��ͨ�� 3�� ��ͨ��
	VS_WR_Cmd(SPI_CLOCKF,0X2000);	//����VS10XX��ʱ��,MULT:2��Ƶ;ADD:������;CLK:12.288Mhz
	VS_WR_Cmd(SPI_MODE,0x1804|(recset->input<<14));		//MIC,¼������    
 	GUI_msleep(5);					//�ȴ�����1.35ms 
 	VS_Load_Patch((u16*)wav_plugin,40);//VS1053��WAV¼����Ҫpatch
}

////��ʼ��WAVͷ.
//void recoder_wav_init(__WaveHeader* wavhead) //��ʼ��WAVͷ			   
//{
//	wavhead->riff.ChunkID=0X46464952;	//"RIFF"
//	wavhead->riff.ChunkSize=0;			//��δȷ��,�����Ҫ����
//	wavhead->riff.Format=0X45564157; 	//"WAVE"
//	wavhead->fmt.ChunkID=0X20746D66; 	//"fmt "
//	wavhead->fmt.ChunkSize=16; 			//��СΪ16���ֽ�
//	wavhead->fmt.AudioFormat=0X01; 		//0X01,��ʾPCM;0X01,��ʾIMA ADPCM
// 	wavhead->fmt.NumOfChannels=1;		//������
// 	wavhead->fmt.SampleRate=8000;		//8Khz������ ��������
// 	wavhead->fmt.ByteRate=wavhead->fmt.SampleRate*2;//16λ,��2���ֽ�
// 	wavhead->fmt.BlockAlign=2;			//���С,2���ֽ�Ϊһ����
// 	wavhead->fmt.BitsPerSample=16;		//16λPCM
//  wavhead->data.ChunkID=0X61746164;	//"data"
// 	wavhead->data.ChunkSize=0;			//���ݴ�С,����Ҫ����  
//}
/*---------------------  END OF FILE  -------------------------*/
