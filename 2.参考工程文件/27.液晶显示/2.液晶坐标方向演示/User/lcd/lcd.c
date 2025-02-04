#include "lcd.h"
#include "fonts.h"	

// ����Һ��ɨ�跽����仯��XY���ؿ��
uint16_t LCD_X_LENGTH = ST7789V_LESS_PIXEL; // 240
uint16_t LCD_Y_LENGTH = ST7789V_MORE_PIXEL; // 320
uint8_t LCD_SCAN_MODE = 6; // Һ����ɨ��ģʽ����������Ҫ���ڷ���ѡ�������ļ������
uint16_t lcdid = LCDID_ST7789V; // ����Һ��������ic�� ID
static sFONT *LCD_Currentfonts = &Font8x16; // ����
static uint16_t CurrentTextColor = BLACK; // ������ɫ-��
static uint16_t CurrentBackColor = WHITE; // ������ɫ-��
/* �������� */
static void     ST7789V_Write_Cmd(uint16_t usCmd);
static void     ST7789V_Write_Data(uint16_t usData);
static uint16_t ST7789V_Read_Data(void);
static void     ST7789V_Delay( __IO uint32_t nCount);
static void     ST7789V_GPIO_Config(void);
static void     ST7789V_FSMC_Config(void);
static void     ST7789V_REG_Config(void);
static void ST7789V_FillColor(uint32_t ulAmout_Point, uint16_t usColor);
static uint16_t ST7789V_Read_PixelData(void);
// ��ST7789Vд������
static void ST7789V_Write_Cmd(uint16_t usCmd)
{
	*(__IO uint16_t*)(FSMC_Addr_ST7789V_CMD) = usCmd;	
}
// ��ST7789Vд������
static void ST7789V_Write_Data(uint16_t usData)
{
	*(__IO uint16_t*)(FSMC_Addr_ST7789V_DATA) = usData;
}
// ��ST7789V��ȡ����	
static uint16_t ST7789V_Read_Data(void)
{
	return(*( __IO uint16_t *)(FSMC_Addr_ST7789V_DATA));
	
}
// ����ʱ����	
static void ST7789V_Delay(__IO uint32_t nCount)
{
  for(;nCount != 0; nCount--);
	
}
// ��ʼ��ST7789V��GPIO
static void ST7789V_GPIO_Config ( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( // �����ź�
	ST7789V_CS_CLK|ST7789V_DC_CLK|ST7789V_WR_CLK|ST7789V_RD_CLK	|ST7789V_BK_CLK|ST7789V_RST_CLK|
	// �����ź�
	ST7789V_D0_CLK|ST7789V_D1_CLK|ST7789V_D2_CLK|ST7789V_D3_CLK|ST7789V_D4_CLK|ST7789V_D5_CLK|
	ST7789V_D6_CLK|ST7789V_D7_CLK|ST7789V_D8_CLK|ST7789V_D9_CLK|ST7789V_D10_CLK|ST7789V_D11_CLK|
	ST7789V_D12_CLK|ST7789V_D13_CLK|ST7789V_D14_CLK|ST7789V_D15_CLK, ENABLE);
	// ����FSMC���Ӧ��������	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP; // �����������
	GPIO_InitStructure.GPIO_Pin = ST7789V_D0_PIN;
	GPIO_Init(ST7789V_D0_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ST7789V_D1_PIN;
	GPIO_Init(ST7789V_D1_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ST7789V_D2_PIN;
	GPIO_Init(ST7789V_D2_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ST7789V_D3_PIN;
	GPIO_Init(ST7789V_D3_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ST7789V_D4_PIN;
	GPIO_Init(ST7789V_D4_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ST7789V_D5_PIN;
	GPIO_Init(ST7789V_D5_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ST7789V_D6_PIN;
	GPIO_Init(ST7789V_D6_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ST7789V_D7_PIN;
	GPIO_Init(ST7789V_D7_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ST7789V_D8_PIN;
	GPIO_Init(ST7789V_D8_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ST7789V_D9_PIN;
	GPIO_Init(ST7789V_D9_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ST7789V_D10_PIN;
	GPIO_Init(ST7789V_D10_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ST7789V_D11_PIN;
	GPIO_Init(ST7789V_D11_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ST7789V_D12_PIN;
	GPIO_Init(ST7789V_D12_PORT, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin = ST7789V_D13_PIN;
	GPIO_Init(ST7789V_D13_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ST7789V_D14_PIN;
	GPIO_Init(ST7789V_D14_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ST7789V_D15_PIN;
	GPIO_Init(ST7789V_D15_PORT, &GPIO_InitStructure);
	/* ����FSMC���Ӧ�Ŀ�����
	FSMC_NOE   :LCD-RD
	FSMC_NWE   :LCD-WR
	FSMC_NE1   :LCD-CS
	FSMC_A16   :LCD-DC */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = ST7789V_RD_PIN; 
	GPIO_Init(ST7789V_RD_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ST7789V_WR_PIN; 
	GPIO_Init(ST7789V_WR_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ST7789V_CS_PIN; 
	GPIO_Init(ST7789V_CS_PORT, &GPIO_InitStructure);  
	GPIO_InitStructure.GPIO_Pin = ST7789V_DC_PIN; 
	GPIO_Init(ST7789V_DC_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = ST7789V_RST_PIN; 
	GPIO_Init(ST7789V_RST_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_InitStructure.GPIO_Pin = ST7789V_BK_PIN; 
	GPIO_Init(ST7789V_BK_PORT, &GPIO_InitStructure);
}
// LCD FSMC ģʽ����
static void ST7789V_FSMC_Config ( void )
{
	FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStructure = {0};
	FSMC_NORSRAMTimingInitTypeDef readWriteTiming = {0};
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
	readWriteTiming.FSMC_AddressSetupTime = 0x01;// ��ַ����ʱ��
	readWriteTiming.FSMC_DataSetupTime = 0x04;	 // ���ݱ���ʱ��
	readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_B; // ģʽB,�첽NOR FLASHģʽ����ILI9341��8080ʱ��ƥ��	
	/* ����������ģʽB�޹� */
	readWriteTiming.FSMC_AddressHoldTime = 0x00; 
	readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
	readWriteTiming.FSMC_CLKDivision = 0x00;
	readWriteTiming.FSMC_DataLatency = 0x00;	
	/* FSMC���� */
	FSMC_NORSRAMInitStructure.FSMC_Bank                  = FSMC_Bank1_NORSRAMx; // ʹ��NOR FLASH��BANK1
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux        = FSMC_DataAddressMux_Disable; // ��ֹ��ַ����
	FSMC_NORSRAMInitStructure.FSMC_MemoryType            = FSMC_MemoryType_NOR; // ʹ��NOR FLASH
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth       = FSMC_MemoryDataWidth_16b; // ���ݿ��Ϊ16λ
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode       = FSMC_BurstAccessMode_Disable; // ��ֹͻ������ģʽ
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity    = FSMC_WaitSignalPolarity_Low; // ʱ�ӵ͵�ƽ��Ч
	FSMC_NORSRAMInitStructure.FSMC_WrapMode              = FSMC_WrapMode_Disable; // ��ֹWRAPģʽ
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive      = FSMC_WaitSignalActive_BeforeWaitState; // �ȴ�״̬ǰ��Ч
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation        = FSMC_WriteOperation_Enable; // ʹ��д����
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal            = FSMC_WaitSignal_Disable; // ��ֹ�ȴ��ź�
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode          = FSMC_ExtendedMode_Disable; // ��ֹ��չģʽ
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst            = FSMC_WriteBurst_Disable; // ��ֹдͻ��ģʽ
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming; // ����ʱ�����
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct     = &readWriteTiming; // ����ʱ�����
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAMx, ENABLE);  
}
// ��ʼ��ST7789V�ļĴ���
static void ST7789V_REG_Config ( void )
{
    lcdid = ST7789V_ReadID(); // ��ȡLCD ID
	if(lcdid == LCDID_ST7789V)
	{
		ST7789V_Write_Cmd(0xCF);
		ST7789V_Write_Data(0x00);
		ST7789V_Write_Data(0x81);
		ST7789V_Write_Data(0x30);
		/* Power on sequence control (EDh) */
		ST7789V_Write_Cmd(0xED);
		ST7789V_Write_Data(0x64);
		ST7789V_Write_Data(0x03);
		ST7789V_Write_Data(0x12);
		ST7789V_Write_Data(0x81);
		/* Driver timing control A (E8h) */
		ST7789V_Write_Cmd(0xE8);
		ST7789V_Write_Data(0x85);
		ST7789V_Write_Data(0x10);
		ST7789V_Write_Data(0x78);
		/* Power control A (CBh) */
		ST7789V_Write_Cmd(0xCB);
		ST7789V_Write_Data(0x39);
		ST7789V_Write_Data(0x2C);
		ST7789V_Write_Data(0x00);
		ST7789V_Write_Data(0x34);
		ST7789V_Write_Data(0x06); // ��Ϊ0x06�ɷ�ֹҺ����ʾ����ʱ�����Ƶ����
		/* Pump ratio control (F7h) */
		ST7789V_Write_Cmd(0xF7);
		ST7789V_Write_Data(0x20);
		/* Driver timing control B */
		ST7789V_Write_Cmd(0xEA);
		ST7789V_Write_Data(0x00);
		ST7789V_Write_Data(0x00);
		/* Frame Rate Control (In Normal Mode/Full Colors) (B1h) */
		ST7789V_Write_Cmd(0xB1);
		ST7789V_Write_Data(0x00);
		ST7789V_Write_Data(0x1B);
		/* Display Function Control (B6h) */
		ST7789V_Write_Cmd(0xB6);
		ST7789V_Write_Data(0x0A);
		ST7789V_Write_Data(0xA2);
		/* Power Control 1 (C0h) */
		ST7789V_Write_Cmd(0xC0);
		ST7789V_Write_Data(0x35);
		/* Power Control 2 (C1h) */
		ST7789V_Write_Cmd(0xC1);
		ST7789V_Write_Data(0x11);
		/* VCOM Control 1 (C5h) */
		ST7789V_Write_Cmd(0xC5);
		ST7789V_Write_Data(0x45);
		ST7789V_Write_Data(0x45);
		/* VCOM Control 2 (C7h) */
		ST7789V_Write_Cmd(0xC7);
		ST7789V_Write_Data(0xA2);
		/* Enable 3G (F2h) */
		ST7789V_Write_Cmd(0xF2);
		ST7789V_Write_Data(0x00);
		/* Gamma Set (26h) */
		ST7789V_Write_Cmd(0x26);
		ST7789V_Write_Data(0x01);
		/* Positive Gamma Correction */
		ST7789V_Write_Cmd(0xE0); // Set Gamma
		ST7789V_Write_Data(0x0F);
		ST7789V_Write_Data(0x26);
		ST7789V_Write_Data(0x24);
		ST7789V_Write_Data(0x0B);
		ST7789V_Write_Data(0x0E);
		ST7789V_Write_Data(0x09);
		ST7789V_Write_Data(0x54);
		ST7789V_Write_Data(0xA8);
		ST7789V_Write_Data(0x46);
		ST7789V_Write_Data(0x0C);
		ST7789V_Write_Data(0x17);
		ST7789V_Write_Data(0x09);
		ST7789V_Write_Data(0x0F);
		ST7789V_Write_Data(0x07);
		ST7789V_Write_Data(0x00);
		/* Negative Gamma Correction (E1h) */
		ST7789V_Write_Cmd(0xE1); // Set Gamma
		ST7789V_Write_Data(0x00);
		ST7789V_Write_Data(0x19);
		ST7789V_Write_Data(0x1B);
		ST7789V_Write_Data(0x04);
		ST7789V_Write_Data(0x10);
		ST7789V_Write_Data(0x07);
		ST7789V_Write_Data(0x2A);
		ST7789V_Write_Data(0x47);
		ST7789V_Write_Data(0x39);
		ST7789V_Write_Data(0x03);
		ST7789V_Write_Data(0x06);
		ST7789V_Write_Data(0x06);
		ST7789V_Write_Data(0x30);
		ST7789V_Write_Data(0x38);
		ST7789V_Write_Data(0x0F);
		/* Memory access control set */
		ST7789V_Write_Cmd(0x36); 
		ST7789V_Write_Data(0xC8);    /* ���������Ͻǵ����½�ɨ�跽ʽ */
		/* Column address control set */
		ST7789V_Write_Cmd(CMD_SetCoordinateX); 
		ST7789V_Write_Data(0x00);
		ST7789V_Write_Data(0x00);
		ST7789V_Write_Data(0x00);
		ST7789V_Write_Data(0xEF);
		/* Page address control set */
		ST7789V_Write_Cmd(CMD_SetCoordinateY); 
		ST7789V_Write_Data(0x00);
		ST7789V_Write_Data(0x00);
		ST7789V_Write_Data(0x01);
		ST7789V_Write_Data(0x3F);
		/* Pixel Format Set (3Ah) */
		ST7789V_Write_Cmd(0x3a); 
		ST7789V_Write_Data(0x55);
		/* Sleep Out (11h) */
		ST7789V_Write_Cmd(0x11);
		ST7789V_Delay(0xAFFf<<2);
		/* Display ON (29h) */
		ST7789V_Write_Cmd(0x29);
	}
}
// ST7789V��ʼ������
void ST7789V_Init ( void )
{
	ST7789V_GPIO_Config (); 
	ST7789V_FSMC_Config (); 
	ST7789V_BackLed_Control(ENABLE);
	ST7789V_Rst(); 
	ST7789V_REG_Config();
	// ����Ĭ��ɨ�跽������ 6 ģʽΪ�󲿷�Һ�����̵�Ĭ����ʾ����  
	ST7789V_GramScan(LCD_SCAN_MODE);
}
// ST7789V������ƺ���
void ST7789V_BackLed_Control(FunctionalState enumState )
{
	if(enumState) // ���������
		GPIO_ResetBits(ST7789V_BK_PORT, ST7789V_BK_PIN); // �͵�ƽ����
	else
		GPIO_SetBits(ST7789V_BK_PORT, ST7789V_BK_PIN);
}
// ��ȡLCD����оƬID����
uint16_t ST7789V_ReadID(void)
{
	uint16_t id = 0;	
	ST7789V_Write_Cmd(0xD3); // д������
	ST7789V_Read_Data();
	ST7789V_Read_Data();
	id = ST7789V_Read_Data();
	id <<= 8;
	id |= ST7789V_Read_Data();
    if(id == LCDID_ST7789V)
    {
    	return id; // �ɹ���ȡ��ST7789V ID
  	}
	return 0; // ��ȡʧ��
}
// ST7789V�����λ
void ST7789V_Rst(void)
{
	GPIO_ResetBits(ST7789V_RST_PORT, ST7789V_RST_PIN); // �͵�ƽ��λ
	ST7789V_Delay(0xAFF); 					   
	GPIO_SetBits (ST7789V_RST_PORT, ST7789V_RST_PIN);		 	 
	ST7789V_Delay(0xAFF); 
}
// ����ST7789V��GRAM��ɨ�跽�� 
void ST7789V_GramScan(uint8_t ucOption)
{	
	// ������飬ֻ������0-7
	if(ucOption > 7)
	{
		return;
	}
	LCD_SCAN_MODE = ucOption; // ����ģʽ����LCD_SCAN_MODE��ֵ����Ҫ���ڴ�����ѡ��������
	if(ucOption%2 == 0)	
	{
		// 0 2 4 6ģʽ��X�������ؿ��Ϊ240��Y����Ϊ320
		LCD_X_LENGTH = ST7789V_LESS_PIXEL;
		LCD_Y_LENGTH = ST7789V_MORE_PIXEL;
	}
	else				
	{
		// 1 3 5 7ģʽ��X�������ؿ��Ϊ320��Y����Ϊ240
		LCD_X_LENGTH = ST7789V_MORE_PIXEL;
		LCD_Y_LENGTH = ST7789V_LESS_PIXEL; 
	}
	ST7789V_Write_Cmd(0x36);	// 0x36��������ĸ�3λ����������GRAMɨ�跽��	
	if(lcdid == LCDID_ST7789V)
  	{
    	ST7789V_Write_Data(0x00 |(ucOption<<5)); // ����ucOption��ֵ����LCD��������0-7��ģʽ
  	}
	ST7789V_Write_Cmd(CMD_SetCoordinateX); 
	ST7789V_Write_Data(0x00);		
	ST7789V_Write_Data(0x00);	
	ST7789V_Write_Data(((LCD_X_LENGTH-1)>>8)&0xFF); 
	ST7789V_Write_Data((LCD_X_LENGTH-1)&0xFF);				
	ST7789V_Write_Cmd(CMD_SetCoordinateY); 
	ST7789V_Write_Data(0x00);		
	ST7789V_Write_Data(0x00);		
	ST7789V_Write_Data(((LCD_Y_LENGTH-1)>>8)&0xFF); 
	ST7789V_Write_Data((LCD_Y_LENGTH-1)&0xFF);			
	ST7789V_Write_Cmd(CMD_SetPixel);	
}
// ST7789V��ʾ���Ͽ���һ������
void ST7789V_OpenWindow(uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight)
{	
	ST7789V_Write_Cmd(CMD_SetCoordinateX); 				
	ST7789V_Write_Data(usX>>8);	 
	ST7789V_Write_Data(usX&0xff);	 
	ST7789V_Write_Data((usX+usWidth-1)>>8);
	ST7789V_Write_Data((usX+usWidth-1)&0xff);
	ST7789V_Write_Cmd(CMD_SetCoordinateY); 			  
	ST7789V_Write_Data(usY>>8);
	ST7789V_Write_Data(usY&0xff);
	ST7789V_Write_Data((usY+usHeight-1)>>8);
	ST7789V_Write_Data((usY+usHeight-1)&0xff);
}
// ����ST7789V�������
static void ST7789V_SetCursor(uint16_t usX, uint16_t usY)	
{
	ST7789V_OpenWindow(usX, usY, 1, 1 );
}
// �����ɫ
static void ST7789V_FillColor(uint32_t ulAmout_Point, uint16_t usColor)
{
	uint32_t i = 0;
	ST7789V_Write_Cmd(CMD_SetPixel);	
	for(i = 0; i < ulAmout_Point; i++)
	{
		ST7789V_Write_Data(usColor);
	}
}
// ����
void ST7789V_Clear(uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight)
{
	ST7789V_OpenWindow(usX, usY, usWidth, usHeight);
	ST7789V_FillColor(usWidth * usHeight, CurrentBackColor) ;		
}
// ��LCD��ĳһ����ĳ����ɫ�������
void ST7789V_SetPointPixel(uint16_t usX, uint16_t usY)	
{	
	if((usX < LCD_X_LENGTH)&&(usY < LCD_Y_LENGTH))
  	{
		ST7789V_SetCursor(usX, usY);	
		ST7789V_FillColor(1, CurrentTextColor);
	}
}
// ��ȡ GRAM ��һ����������
static uint16_t ST7789V_Read_PixelData(void)	
{	
	uint16_t usRG=0, usB=0;
	ST7789V_Write_Cmd(0x2E);
	ST7789V_Read_Data (); // ȥ��ǰһ�ζ�ȡ���
	// ��ȡ��ɫͨ������ɫͨ����ֵ
	usRG = ST7789V_Read_Data();  
	usB = ST7789V_Read_Data();  	
  	return((usRG&0xF800)|((usRG<<3)&0x7E0)|(usB>>11));
}
// ��ȡLCD��ĳһ�����ɫֵ
uint16_t ST7789V_GetPointPixel(uint16_t usX, uint16_t usY)
{ 
	uint16_t usPixelData;
	ST7789V_SetCursor(usX, usY);
	usPixelData = ST7789V_Read_PixelData();
	return usPixelData;
}
// ��LCDʹ�� Bresenham �㷨���߶� 
void ST7789V_DrawLine(uint16_t usX1, uint16_t usY1, uint16_t usX2, uint16_t usY2)
{
	uint16_t us; 
	uint16_t usX_Current, usY_Current;
	int32_t lError_X = 0, lError_Y = 0, lDelta_X, lDelta_Y, lDistance; 
	int32_t lIncrease_X, lIncrease_Y; 	
	lDelta_X = usX2 - usX1; // ������������ 
	lDelta_Y = usY2 - usY1; 
	usX_Current = usX1; 
	usY_Current = usY1; 
	if ( lDelta_X > 0 ) 
	{
		lIncrease_X = 1; // ���õ�������
	} 
	else if(lDelta_X == 0) 
	{
		lIncrease_X = 0; // ��ֱ��
	} 
	else 
  	{ 
    	lIncrease_X = -1;
    	lDelta_X = - lDelta_X;
  	} 
	if(lDelta_Y > 0)
	{
		lIncrease_Y = 1; 
	}
	else if(lDelta_Y == 0)
	{
		lIncrease_Y = 0; // ˮƽ�� 
	}
	else 
  	{
    	lIncrease_Y = -1;
    	lDelta_Y = - lDelta_Y;
  	} 
	if(lDelta_X > lDelta_Y)
	{
		lDistance = lDelta_X; // ѡȡ�������������� 
	}
	else 
	{
		lDistance = lDelta_Y; 
	}
	for(us = 0; us <= lDistance + 1; us++ ) // ������� 
	{  
		ST7789V_SetPointPixel(usX_Current, usY_Current ); // ���� 
		lError_X += lDelta_X ; 
		lError_Y += lDelta_Y ; 		
		if(lError_X > lDistance) 
		{ 
			lError_X -= lDistance; 
			usX_Current += lIncrease_X; 
		}  
		if(lError_Y > lDistance) 
		{ 
			lError_Y -= lDistance; 
			usY_Current += lIncrease_Y; 
		} 
	}  
}   
// ��LCD�ϻ�һ������
void ST7789V_DrawRectangle(uint16_t usX_Start, uint16_t usY_Start, uint16_t usWidth, uint16_t usHeight, uint8_t ucFilled)
{
	if(ucFilled)
	{
		ST7789V_OpenWindow(usX_Start, usY_Start, usWidth, usHeight);
		ST7789V_FillColor(usWidth * usHeight ,CurrentTextColor);	
	}
	else
	{
		ST7789V_DrawLine(usX_Start, usY_Start, usX_Start + usWidth - 1, usY_Start);
		ST7789V_DrawLine(usX_Start, usY_Start + usHeight - 1, usX_Start + usWidth - 1, usY_Start + usHeight - 1);
		ST7789V_DrawLine(usX_Start, usY_Start, usX_Start, usY_Start + usHeight - 1);
		ST7789V_DrawLine(usX_Start + usWidth - 1, usY_Start, usX_Start + usWidth - 1, usY_Start + usHeight - 1);		
	}
}
// ��LCD��ʹ�� Bresenham �㷨��Բ
void ST7789V_DrawCircle ( uint16_t usX_Center, uint16_t usY_Center, uint16_t usRadius, uint8_t ucFilled)
{
	int16_t sCurrentX, sCurrentY;
	int16_t sError;
	sCurrentX = 0; sCurrentY = usRadius;	  
	sError = 3 - ( usRadius << 1 ); // �ж��¸���λ�õı�־
	while(sCurrentX <= sCurrentY)
	{
		int16_t sCountY;
		if(ucFilled)
		{			
			for(sCountY = sCurrentX; sCountY <= sCurrentY; sCountY ++) 
			{                      
				ST7789V_SetPointPixel(usX_Center + sCurrentX, usY_Center + sCountY);          
				ST7789V_SetPointPixel(usX_Center - sCurrentX, usY_Center + sCountY);                  
				ST7789V_SetPointPixel(usX_Center - sCountY,   usY_Center + sCurrentX);         
				ST7789V_SetPointPixel(usX_Center - sCountY,   usY_Center - sCurrentX);        
				ST7789V_SetPointPixel(usX_Center - sCurrentX, usY_Center - sCountY);               
        		ST7789V_SetPointPixel(usX_Center + sCurrentX, usY_Center - sCountY);           
				ST7789V_SetPointPixel(usX_Center + sCountY,   usY_Center - sCurrentX);          	
        		ST7789V_SetPointPixel(usX_Center + sCountY,   usY_Center + sCurrentX);        				
			}
		}
		else
		{          
			ST7789V_SetPointPixel(usX_Center + sCurrentX, usY_Center + sCurrentY);            
			ST7789V_SetPointPixel(usX_Center - sCurrentX, usY_Center + sCurrentY);              
			ST7789V_SetPointPixel(usX_Center - sCurrentY, usY_Center + sCurrentX);            
			ST7789V_SetPointPixel(usX_Center - sCurrentY, usY_Center - sCurrentX);            
			ST7789V_SetPointPixel(usX_Center - sCurrentX, usY_Center - sCurrentY);             
			ST7789V_SetPointPixel(usX_Center + sCurrentX, usY_Center - sCurrentY);            
			ST7789V_SetPointPixel(usX_Center + sCurrentY, usY_Center - sCurrentX);             
			ST7789V_SetPointPixel(usX_Center + sCurrentY, usY_Center + sCurrentX);           
    	}			
		sCurrentX ++;
		if(sError < 0 )
		{ 
			sError += 4 * sCurrentX + 6;
		}	  
		else
		{
			sError += 10 + 4 * (sCurrentX - sCurrentY);   
			sCurrentY--;
		} 	
	}
}
// ��LCD����ʾһ��Ӣ���ַ�
void ST7789V_DispChar_EN(uint16_t usX, uint16_t usY, const char cChar)
{
	uint8_t  byteCount, bitCount,fontLength;	
	uint16_t ucRelativePositon;
	uint8_t *Pfont;
	// ��ascii���ƫ�ƣ���ģ������ASCII���ǰ32����ͼ�η��ţ�
	ucRelativePositon = cChar - ' ';
	// ÿ����ģ���ֽ���
	fontLength = (LCD_Currentfonts->Width*LCD_Currentfonts->Height)/8;
	//��ģ�׵�ַ
	/*ascii���ƫ��ֵ����ÿ����ģ���ֽ����������ģ��ƫ��λ��*/
	Pfont = (uint8_t *)&LCD_Currentfonts->table[ucRelativePositon * fontLength];
	// ������ʾ����
	ST7789V_OpenWindow(usX, usY, LCD_Currentfonts->Width, LCD_Currentfonts->Height);	
	ST7789V_Write_Cmd(CMD_SetPixel);			
	// ���ֽڶ�ȡ��ģ����
	for(byteCount = 0; byteCount < fontLength; byteCount++)
	{
		// һλһλ����Ҫ��ʾ����ɫ
		for(bitCount = 0; bitCount < 8; bitCount++ )
		{
			if(Pfont[byteCount] & (0x80>>bitCount))
				ST7789V_Write_Data(CurrentTextColor);			
			else
				ST7789V_Write_Data(CurrentBackColor);
		}	
	}	
}
// ��LCD����ʾӢ���ַ���
void ST7789V_DispStringLine_EN(uint16_t line,  char* pStr)
{
	uint16_t usX = 0;
	while(*pStr != '\0')
	{
		if((usX - ST7789V_X_Star + LCD_Currentfonts->Width) > LCD_X_LENGTH)
		{
			usX = ST7789V_X_Star;
			line += LCD_Currentfonts->Height;
		}
		if ( ( line - ST7789V_Y_Star + LCD_Currentfonts->Height) > LCD_Y_LENGTH)
		{
			usX = ST7789V_X_Star;
			line = ST7789V_Y_Star;
		}
		ST7789V_DispChar_EN(usX, line, * pStr);
		pStr ++;
		usX += LCD_Currentfonts->Width;	
	}
}
// ��LCD����ʾӢ���ַ���
void ST7789V_DispString_EN(uint16_t usX ,uint16_t usY,  char* pStr)
{
	while(*pStr != '\0')
	{
		if((usX - ST7789V_X_Star + LCD_Currentfonts->Width) > LCD_X_LENGTH)
		{
			usX = ST7789V_X_Star;
			usY += LCD_Currentfonts->Height;
		}		
		if((usY - ST7789V_Y_Star + LCD_Currentfonts->Height ) > LCD_Y_LENGTH)
		{
			usX = ST7789V_X_Star;
			usY = ST7789V_Y_Star;
		}	
		ST7789V_DispChar_EN(usX, usY, * pStr);
		pStr ++;
		usX += LCD_Currentfonts->Width;	
	}
}
// ��LCD����ʾӢ���ַ���(��Y�᷽��)
void ST7789V_DispString_EN_YDir(uint16_t usX,uint16_t usY, char* pStr)
{	
	while(* pStr != '\0')
	{
		if((usY - ST7789V_Y_Star + LCD_Currentfonts->Height)>LCD_Y_LENGTH)
		{
			usY = ST7789V_Y_Star;
			usX += LCD_Currentfonts->Width;
		}
		if((usX - ST7789V_X_Star + LCD_Currentfonts->Width)> LCD_X_LENGTH)
		{
			usX = ST7789V_X_Star;
			usY = ST7789V_Y_Star;
		}
		ST7789V_DispChar_EN(usX, usY, *pStr);
		pStr ++;
		usY += LCD_Currentfonts->Height;		
	}	
}
// ����Ӣ����������
void LCD_SetFont(sFONT *fonts)
{
  LCD_Currentfonts = fonts;
}
// ��ȡ��ǰ��������
sFONT *LCD_GetFont(void)
{
  return LCD_Currentfonts;
}
// ����LCD��ǰ��(����)��������ɫ,RGB565
void LCD_SetColors(uint16_t TextColor, uint16_t BackColor) 
{
  CurrentTextColor = TextColor; 
  CurrentBackColor = BackColor;
}
// ��ȡLCD��ǰ��(����)��������ɫ,RGB565
void LCD_GetColors(uint16_t *TextColor, uint16_t *BackColor)
{
  *TextColor = CurrentTextColor;
  *BackColor = CurrentBackColor;
}
// ����LCD��ǰ��(����)��ɫ,RGB565
void LCD_SetTextColor(uint16_t Color)
{
  CurrentTextColor = Color;
}
// ����LCD�ı�����ɫ,RGB565
void LCD_SetBackColor(uint16_t Color)
{
  CurrentBackColor = Color;
}
// ���ĳ������
void LCD_ClearLine(uint16_t Line)
{
  ST7789V_Clear(0,Line,LCD_X_LENGTH,((sFONT *)LCD_GetFont())->Height);
}
