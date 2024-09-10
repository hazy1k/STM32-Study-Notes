/**
  ******************************************************************************
  * @file    bsp_ili9806g_lcd.c
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ILI9806GҺ��������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F407 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "./lcd/bsp_ili9806g_lcd.h"
#include "./font/fonts.h"	

//����Һ��ɨ�跽����仯��XY���ؿ��
//����ILI9806G_GramScan�������÷���ʱ���Զ�����
uint16_t LCD_X_LENGTH = ILI9806G_MORE_PIXEL;
uint16_t LCD_Y_LENGTH = ILI9806G_LESS_PIXEL;

//Һ����ɨ��ģʽ����������Ҫ���ڷ���ѡ�������ļ������
//������ѡֵΪ0-7
//����ILI9806G_GramScan�������÷���ʱ���Զ�����
//LCD�ճ�ʼ�����ʱ��ʹ�ñ�Ĭ��ֵ
uint8_t LCD_SCAN_MODE =6;


static sFONT *LCD_Currentfonts = &Font16x32;  //Ӣ������
static uint16_t CurrentTextColor   = WHITE;//ǰ��ɫ
static uint16_t CurrentBackColor   = BLACK;//����ɫ

//�����ȡ��������ģ����
static uint8_t ucBuffer [ WIDTH_CH_CHAR*HEIGHT_CH_CHAR/8 ];	


__inline void                 ILI9806G_Write_Cmd           ( uint16_t usCmd );
__inline void                 ILI9806G_Write_Data          ( uint16_t usData );
__inline uint16_t             ILI9806G_Read_Data           ( void );
static void                   ILI9806G_Delay               ( __IO uint32_t nCount );
static void                   ILI9806G_GPIO_Config         ( void );
static void                   ILI9806G_FSMC_Config         ( void );
static void                   ILI9806G_REG_Config          ( void );
static void                   ILI9806G_SetCursor           ( uint16_t usX, uint16_t usY );
static __inline void          ILI9806G_FillColor           ( uint32_t ulAmout_Point, uint16_t usColor );
static uint16_t               ILI9806G_Read_PixelData      ( void );


/**
  * @brief  ����ʱ����
  * @param  nCount ����ʱ����ֵ
  * @retval ��
  */	
static void Delay ( __IO uint32_t nCount )
{
  for ( ; nCount != 0; nCount -- );
	
}

/**
  * @brief  ��ILI9806Gд������
  * @param  usCmd :Ҫд��������Ĵ�����ַ��
  * @retval ��
  */	
__inline void ILI9806G_Write_Cmd ( uint16_t usCmd )
{
	* ( __IO uint16_t * ) ( FSMC_Addr_ILI9806G_CMD ) = usCmd;
	
}


/**
  * @brief  ��ILI9806Gд������
  * @param  usData :Ҫд�������
  * @retval ��
  */	
__inline void ILI9806G_Write_Data ( uint16_t usData )
{
	* ( __IO uint16_t * ) ( FSMC_Addr_ILI9806G_DATA ) = usData;
	
}


/**
  * @brief  ��ILI9806G��ȡ����
  * @param  ��
  * @retval ��ȡ��������
  */	
__inline uint16_t ILI9806G_Read_Data ( void )
{
	return ( * ( __IO uint16_t * ) ( FSMC_Addr_ILI9806G_DATA ) );
	
}


/**
  * @brief  ���� ILI9806G ����ʱ����
  * @param  nCount ����ʱ����ֵ
  * @retval ��
  */	
static void ILI9806G_Delay ( __IO uint32_t nCount )
{
  for ( ; nCount != 0; nCount -- );
	
}


/**
  * @brief  ��ʼ��ILI9806G��IO����
  * @param  ��
  * @retval ��
  */
 void ILI9806G_GPIO_Config ( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* ʹ��FSMC��Ӧ��Ӧ�ܽ�ʱ��*/
	RCC_AHB1PeriphClockCmd ( 	
													/*�����ź�*/
													ILI9806G_CS_CLK|ILI9806G_DC_CLK|ILI9806G_WR_CLK|
													ILI9806G_RD_CLK	|ILI9806G_BK_CLK|ILI9806G_RST_CLK|
													/*�����ź�*/
													ILI9806G_D0_CLK|ILI9806G_D1_CLK|	ILI9806G_D2_CLK | 
													ILI9806G_D3_CLK | ILI9806G_D4_CLK|ILI9806G_D5_CLK|
													ILI9806G_D6_CLK | ILI9806G_D7_CLK|ILI9806G_D8_CLK|
													ILI9806G_D9_CLK | ILI9806G_D10_CLK|ILI9806G_D11_CLK|
													ILI9806G_D12_CLK | ILI9806G_D13_CLK|ILI9806G_D14_CLK|
													ILI9806G_D15_CLK	, ENABLE );
		
	
	/* ����FSMC���Ӧ��������,FSMC-D0~D15 */	
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;

    GPIO_InitStructure.GPIO_Pin = ILI9806G_D0_PIN; 
    GPIO_Init(ILI9806G_D0_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_D0_PORT,ILI9806G_D0_PinSource,FSMC_AF);

    GPIO_InitStructure.GPIO_Pin = ILI9806G_D1_PIN; 
    GPIO_Init(ILI9806G_D1_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_D1_PORT,ILI9806G_D1_PinSource,FSMC_AF);

    GPIO_InitStructure.GPIO_Pin = ILI9806G_D2_PIN; 
    GPIO_Init(ILI9806G_D2_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_D2_PORT,ILI9806G_D2_PinSource,FSMC_AF);

    GPIO_InitStructure.GPIO_Pin = ILI9806G_D3_PIN; 
    GPIO_Init(ILI9806G_D3_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_D3_PORT,ILI9806G_D3_PinSource,FSMC_AF);

    GPIO_InitStructure.GPIO_Pin = ILI9806G_D4_PIN; 
    GPIO_Init(ILI9806G_D4_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_D4_PORT,ILI9806G_D4_PinSource,FSMC_AF);

    GPIO_InitStructure.GPIO_Pin = ILI9806G_D5_PIN; 
    GPIO_Init(ILI9806G_D5_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_D5_PORT,ILI9806G_D5_PinSource,FSMC_AF);

    GPIO_InitStructure.GPIO_Pin = ILI9806G_D6_PIN; 
    GPIO_Init(ILI9806G_D6_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_D6_PORT,ILI9806G_D6_PinSource,FSMC_AF);

    GPIO_InitStructure.GPIO_Pin = ILI9806G_D7_PIN; 
    GPIO_Init(ILI9806G_D7_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_D7_PORT,ILI9806G_D7_PinSource,FSMC_AF);

    GPIO_InitStructure.GPIO_Pin = ILI9806G_D8_PIN; 
    GPIO_Init(ILI9806G_D8_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_D8_PORT,ILI9806G_D8_PinSource,FSMC_AF);

    GPIO_InitStructure.GPIO_Pin = ILI9806G_D9_PIN; 
    GPIO_Init(ILI9806G_D9_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_D9_PORT,ILI9806G_D9_PinSource,FSMC_AF);

    GPIO_InitStructure.GPIO_Pin = ILI9806G_D10_PIN; 
    GPIO_Init(ILI9806G_D10_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_D10_PORT,ILI9806G_D10_PinSource,FSMC_AF);

    GPIO_InitStructure.GPIO_Pin = ILI9806G_D11_PIN; 
    GPIO_Init(ILI9806G_D11_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_D11_PORT,ILI9806G_D11_PinSource,FSMC_AF);

    GPIO_InitStructure.GPIO_Pin = ILI9806G_D12_PIN; 
    GPIO_Init(ILI9806G_D12_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_D12_PORT,ILI9806G_D12_PinSource,FSMC_AF);

    GPIO_InitStructure.GPIO_Pin = ILI9806G_D13_PIN; 
    GPIO_Init(ILI9806G_D13_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_D13_PORT,ILI9806G_D13_PinSource,FSMC_AF);

    GPIO_InitStructure.GPIO_Pin = ILI9806G_D14_PIN; 
    GPIO_Init(ILI9806G_D14_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_D14_PORT,ILI9806G_D14_PinSource,FSMC_AF);

    GPIO_InitStructure.GPIO_Pin = ILI9806G_D15_PIN; 
    GPIO_Init(ILI9806G_D15_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_D15_PORT,ILI9806G_D15_PinSource,FSMC_AF);

	/* ����FSMC���Ӧ�Ŀ�����
	 * FSMC_NOE   :LCD-RD
	 * FSMC_NWE   :LCD-WR
	 * FSMC_NE1   :LCD-CS
	 * FSMC_A0    :LCD-DC
	 */
    GPIO_InitStructure.GPIO_Pin = ILI9806G_RD_PIN; 
    GPIO_Init(ILI9806G_RD_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_RD_PORT,ILI9806G_RD_PinSource,FSMC_AF);

    GPIO_InitStructure.GPIO_Pin = ILI9806G_WR_PIN; 
    GPIO_Init(ILI9806G_WR_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_WR_PORT,ILI9806G_WR_PinSource,FSMC_AF);

    GPIO_InitStructure.GPIO_Pin = ILI9806G_CS_PIN; 
    GPIO_Init(ILI9806G_CS_PORT, &GPIO_InitStructure);   
    GPIO_PinAFConfig(ILI9806G_CS_PORT,ILI9806G_CS_PinSource,FSMC_AF);  

    GPIO_InitStructure.GPIO_Pin = ILI9806G_DC_PIN; 
    GPIO_Init(ILI9806G_DC_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(ILI9806G_DC_PORT,ILI9806G_DC_PinSource,FSMC_AF);
	

  /* ����LCD��λRST���ƹܽ�*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructure.GPIO_Pin = ILI9806G_RST_PIN; 
	GPIO_Init ( ILI9806G_RST_PORT, & GPIO_InitStructure );
		
	/* ����LCD������ƹܽ�BK*/
	GPIO_InitStructure.GPIO_Pin = ILI9806G_BK_PIN; 
	GPIO_Init ( ILI9806G_BK_PORT, & GPIO_InitStructure );

}


 /**
  * @brief  LCD  FSMC ģʽ����
  * @param  ��
  * @retval ��
  */
static void ILI9806G_FSMC_Config ( void )
{
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  readWriteTiming; 	
	
	/* ʹ��FSMCʱ��*/
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC,ENABLE);

	//��ַ����ʱ�䣨ADDSET��Ϊ1��HCLK 5/168M=30ns
	readWriteTiming.FSMC_AddressSetupTime      = 0x04;	 //��ַ����ʱ��
	//���ݱ���ʱ�䣨DATAST��+ 1��HCLK = 12/168M=72ns	
	readWriteTiming.FSMC_DataSetupTime         = 0x0b;	 //���ݽ���ʱ��
	//ѡ����Ƶ�ģʽ
	//ģʽB,�첽NOR FLASHģʽ����ILI9806G��8080ʱ��ƥ��
	readWriteTiming.FSMC_AccessMode            = FSMC_AccessMode_B;	
	
	/*����������ģʽB�޹�*/
	//��ַ����ʱ�䣨ADDHLD��ģʽAδ�õ�
	readWriteTiming.FSMC_AddressHoldTime       = 0x00;	 //��ַ����ʱ��
	//��������ת�����ڣ������ڸ���ģʽ��NOR����
	readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
	//����ʱ�ӷ�Ƶ��������ͬ�����͵Ĵ洢��
	readWriteTiming.FSMC_CLKDivision           = 0x00;
	//���ݱ���ʱ�䣬������ͬ���͵�NOR	
	readWriteTiming.FSMC_DataLatency           = 0x00;	

	
	FSMC_NORSRAMInitStructure.FSMC_Bank                  = FSMC_Bank1_NORSRAMx;
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux        = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStructure.FSMC_MemoryType            = FSMC_MemoryType_NOR;
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth       = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode       = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity    = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode              = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive      = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation        = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal            = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode          = FSMC_ExtendedMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst            = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct     = &readWriteTiming;  
	
	FSMC_NORSRAMInit ( & FSMC_NORSRAMInitStructure ); 
	
	
	/* ʹ�� FSMC_Bank1_NORSRAM3 */
	FSMC_NORSRAMCmd ( FSMC_Bank1_NORSRAMx, ENABLE );  
		
		
}


/**
 * @brief  ��ʼ��ILI9806G�Ĵ���
 * @param  ��
 * @retval ��
 */
/**
 * @brief  ��ʼ��ILI9806G�Ĵ���
 * @param  ��
 * @retval ��
 */
static void ILI9806G_REG_Config ( void )
{	
	//Һ�������ṩ�����ְ汾����Ļ������һ�������ǵ�����������������
	//������ͨ��#if #else #endif�����ã�����Ļ��ʾ���������#if 0�ĳ�#if 1����1�ĳ�0
	//��2017-12-14�պ���Ĳ�Ʒ��ʹ��#if 0
#if 0
	//�ɰ�
	/* EXTC Command Set enable register */
	DEBUG_DELAY  ();
	ILI9806G_Write_Cmd ( 0xFF  );
	ILI9806G_Write_Data ( 0xFF  );
	ILI9806G_Write_Data ( 0x98  );
	ILI9806G_Write_Data ( 0x06  );

	/* GIP 1(BCh)  */
	DEBUG_DELAY ();
	ILI9806G_Write_Cmd(0xBC);
	ILI9806G_Write_Data(0x01); 
	ILI9806G_Write_Data(0x0E); 
	ILI9806G_Write_Data(0x61); 
	ILI9806G_Write_Data(0xFB); 
	ILI9806G_Write_Data(0x10); 
	ILI9806G_Write_Data(0x10); 
	ILI9806G_Write_Data(0x0B); 
	ILI9806G_Write_Data(0x0F); 
	ILI9806G_Write_Data(0x2E); 
	ILI9806G_Write_Data(0x73); 
	ILI9806G_Write_Data(0xFF); 
	ILI9806G_Write_Data(0xFF); 
	ILI9806G_Write_Data(0x0E); 
	ILI9806G_Write_Data(0x0E); 
	ILI9806G_Write_Data(0x00); 
	ILI9806G_Write_Data(0x03); 
	ILI9806G_Write_Data(0x66); 
	ILI9806G_Write_Data(0x63); 
	ILI9806G_Write_Data(0x01); 
	ILI9806G_Write_Data(0x00); 
	ILI9806G_Write_Data(0x00);

	/* GIP 2 (BDh) */
	DEBUG_DELAY ();
	ILI9806G_Write_Cmd(0xBD);
	ILI9806G_Write_Data(0x01); 
	ILI9806G_Write_Data(0x23); 
	ILI9806G_Write_Data(0x45); 
	ILI9806G_Write_Data(0x67); 
	ILI9806G_Write_Data(0x01); 
	ILI9806G_Write_Data(0x23); 
	ILI9806G_Write_Data(0x45); 
	ILI9806G_Write_Data(0x67); 

	/* GIP 3 (BEh) */
	DEBUG_DELAY ();
	ILI9806G_Write_Cmd(0xBE);
	ILI9806G_Write_Data(0x00); 
	ILI9806G_Write_Data(0x21); 
	ILI9806G_Write_Data(0xAB); 
	ILI9806G_Write_Data(0x60); 
	ILI9806G_Write_Data(0x22); 
	ILI9806G_Write_Data(0x22); 
	ILI9806G_Write_Data(0x22); 
	ILI9806G_Write_Data(0x22); 
	ILI9806G_Write_Data(0x22); 

	/* Vcom  (C7h) */
	DEBUG_DELAY ();
	ILI9806G_Write_Cmd ( 0xC7 );
	ILI9806G_Write_Data ( 0x6F );

	/* EN_volt_reg (EDh)*/
	DEBUG_DELAY ();
	ILI9806G_Write_Cmd ( 0xED );
	ILI9806G_Write_Data ( 0x7F );
	ILI9806G_Write_Data ( 0x0F );
	ILI9806G_Write_Data ( 0x00 );

	/* Power Control 1 (C0h) */
	DEBUG_DELAY ();
	ILI9806G_Write_Cmd ( 0xC0 );
	ILI9806G_Write_Data ( 0x37 );
	ILI9806G_Write_Data ( 0x0B );
	ILI9806G_Write_Data ( 0x0A );

	/* LVGL (FCh) */
	DEBUG_DELAY ();
	ILI9806G_Write_Cmd ( 0xFC );
	ILI9806G_Write_Data ( 0x0A );

	/* Engineering Setting (DFh) */
	DEBUG_DELAY ();
	ILI9806G_Write_Cmd ( 0xDF );
	ILI9806G_Write_Data ( 0x00 );
	ILI9806G_Write_Data ( 0x00 );
	ILI9806G_Write_Data ( 0x00 );
	ILI9806G_Write_Data ( 0x00 );
	ILI9806G_Write_Data ( 0x00 );
	ILI9806G_Write_Data ( 0x20 );

	/* DVDD Voltage Setting(F3h) */
	DEBUG_DELAY ();
	ILI9806G_Write_Cmd ( 0xF3 );
	ILI9806G_Write_Data ( 0x74 );

	/* Display Inversion Control (B4h) */
	ILI9806G_Write_Cmd ( 0xB4 );
	ILI9806G_Write_Data ( 0x00 );
	ILI9806G_Write_Data ( 0x00 );
	ILI9806G_Write_Data ( 0x00 );

	/* 480x854 (F7h)  */
	ILI9806G_Write_Cmd ( 0xF7 );
	ILI9806G_Write_Data ( 0x89 );

	/* Frame Rate (B1h) */
	ILI9806G_Write_Cmd ( 0xB1 );
	ILI9806G_Write_Data ( 0x00 );
	ILI9806G_Write_Data ( 0x12 );
	ILI9806G_Write_Data ( 0x10 );

	/* Panel Timing Control (F2h) */
	ILI9806G_Write_Cmd ( 0xF2 );
	ILI9806G_Write_Data ( 0x80 );
	ILI9806G_Write_Data ( 0x5B );
	ILI9806G_Write_Data ( 0x40 );
	ILI9806G_Write_Data ( 0x28 );

	DEBUG_DELAY ();

	/* Power Control 2 (C1h) */
	ILI9806G_Write_Cmd ( 0xC1 ); 
	ILI9806G_Write_Data ( 0x17 );
	ILI9806G_Write_Data ( 0x7D );
	ILI9806G_Write_Data ( 0x7A );
	ILI9806G_Write_Data ( 0x20 );

	DEBUG_DELAY ();

	ILI9806G_Write_Cmd(0xE0); 
	ILI9806G_Write_Data(0x00); //P1 
	ILI9806G_Write_Data(0x11); //P2 
	ILI9806G_Write_Data(0x1C); //P3 
	ILI9806G_Write_Data(0x0E); //P4 
	ILI9806G_Write_Data(0x0F); //P5 
	ILI9806G_Write_Data(0x0C); //P6 
	ILI9806G_Write_Data(0xC7); //P7 
	ILI9806G_Write_Data(0x06); //P8 
	ILI9806G_Write_Data(0x06); //P9 
	ILI9806G_Write_Data(0x0A); //P10 
	ILI9806G_Write_Data(0x10); //P11 
	ILI9806G_Write_Data(0x12); //P12 
	ILI9806G_Write_Data(0x0A); //P13 
	ILI9806G_Write_Data(0x10); //P14 
	ILI9806G_Write_Data(0x02); //P15 
	ILI9806G_Write_Data(0x00); //P16 

	DEBUG_DELAY ();

	ILI9806G_Write_Cmd(0xE1); 
	ILI9806G_Write_Data(0x00); //P1 
	ILI9806G_Write_Data(0x12); //P2 
	ILI9806G_Write_Data(0x18); //P3 
	ILI9806G_Write_Data(0x0C); //P4 
	ILI9806G_Write_Data(0x0F); //P5 
	ILI9806G_Write_Data(0x0A); //P6 
	ILI9806G_Write_Data(0x77); //P7 
	ILI9806G_Write_Data(0x06); //P8 
	ILI9806G_Write_Data(0x07); //P9 
	ILI9806G_Write_Data(0x0A); //P10 
	ILI9806G_Write_Data(0x0E); //P11 
	ILI9806G_Write_Data(0x0B); //P12 
	ILI9806G_Write_Data(0x10); //P13 
	ILI9806G_Write_Data(0x1D); //P14 
	ILI9806G_Write_Data(0x17); //P15 
	ILI9806G_Write_Data(0x00); //P16  

	/* Tearing Effect ON (35h)  */
	ILI9806G_Write_Cmd ( 0x35 );
	ILI9806G_Write_Data ( 0x00 );

	ILI9806G_Write_Cmd ( 0x3A );
	ILI9806G_Write_Data ( 0x55 );

	ILI9806G_Write_Cmd ( 0x11 );
	DEBUG_DELAY ();
	ILI9806G_Write_Cmd ( 0x29 );
	
#else
	//�°�
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xFF);
	ILI9806G_Write_Data(0xFF);
	ILI9806G_Write_Data(0x98);
	ILI9806G_Write_Data(0x06);
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xBA);
	ILI9806G_Write_Data(0x60);  
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xBC);
	ILI9806G_Write_Data(0x03);
	ILI9806G_Write_Data(0x0E);
	ILI9806G_Write_Data(0x61);
	ILI9806G_Write_Data(0xFF);
	ILI9806G_Write_Data(0x05);
	ILI9806G_Write_Data(0x05);
	ILI9806G_Write_Data(0x1B);
	ILI9806G_Write_Data(0x10);
	ILI9806G_Write_Data(0x73);
	ILI9806G_Write_Data(0x63);
	ILI9806G_Write_Data(0xFF);
	ILI9806G_Write_Data(0xFF);
	ILI9806G_Write_Data(0x05);
	ILI9806G_Write_Data(0x05);
	ILI9806G_Write_Data(0x00);
	ILI9806G_Write_Data(0x00);
	ILI9806G_Write_Data(0xD5);
	ILI9806G_Write_Data(0xD0);
	ILI9806G_Write_Data(0x01);
	ILI9806G_Write_Data(0x00);
	ILI9806G_Write_Data(0x40); 
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xBD);
	ILI9806G_Write_Data(0x01);
	ILI9806G_Write_Data(0x23);
	ILI9806G_Write_Data(0x45);
	ILI9806G_Write_Data(0x67);
	ILI9806G_Write_Data(0x01);
	ILI9806G_Write_Data(0x23);
	ILI9806G_Write_Data(0x45);
	ILI9806G_Write_Data(0x67);  
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xBE);
	ILI9806G_Write_Data(0x01);
	ILI9806G_Write_Data(0x2D);
	ILI9806G_Write_Data(0xCB);
	ILI9806G_Write_Data(0xA2);
	ILI9806G_Write_Data(0x62);
	ILI9806G_Write_Data(0xF2);
	ILI9806G_Write_Data(0xE2);
	ILI9806G_Write_Data(0x22);
	ILI9806G_Write_Data(0x22);
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xC7);
	ILI9806G_Write_Data(0x63); 
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xED);
	ILI9806G_Write_Data(0x7F);
	ILI9806G_Write_Data(0x0F);
	ILI9806G_Write_Data(0x00);
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xC0);
	ILI9806G_Write_Data(0x03);
	ILI9806G_Write_Data(0x0B);
	ILI9806G_Write_Data(0x00);   
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xFC);
	ILI9806G_Write_Data(0x08); 
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xDF);
	ILI9806G_Write_Data(0x00);
	ILI9806G_Write_Data(0x00);
	ILI9806G_Write_Data(0x00);
	ILI9806G_Write_Data(0x00);
	ILI9806G_Write_Data(0x00);
	ILI9806G_Write_Data(0x20);
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xF3);
	ILI9806G_Write_Data(0x74);
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xF9);
	ILI9806G_Write_Data(0x00);
	ILI9806G_Write_Data(0xFD);
	ILI9806G_Write_Data(0x80);
	ILI9806G_Write_Data(0x80);
	ILI9806G_Write_Data(0xC0);
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xB4);
	ILI9806G_Write_Data(0x02);
	ILI9806G_Write_Data(0x02);
	ILI9806G_Write_Data(0x02); 
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xF7);
	ILI9806G_Write_Data(0x81);
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xB1);
	ILI9806G_Write_Data(0x00);
	ILI9806G_Write_Data(0x13);
	ILI9806G_Write_Data(0x13); 
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xF2);
	ILI9806G_Write_Data(0xC0);
	ILI9806G_Write_Data(0x02);
	ILI9806G_Write_Data(0x40);
	ILI9806G_Write_Data(0x28);  
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xC1);
	ILI9806G_Write_Data(0x17);
	ILI9806G_Write_Data(0x75);
	ILI9806G_Write_Data(0x75);
	ILI9806G_Write_Data(0x20); 
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xE0);
	ILI9806G_Write_Data(0x00);
	ILI9806G_Write_Data(0x05);
	ILI9806G_Write_Data(0x08);
	ILI9806G_Write_Data(0x0C);
	ILI9806G_Write_Data(0x0F);
	ILI9806G_Write_Data(0x15);
	ILI9806G_Write_Data(0x09);
	ILI9806G_Write_Data(0x07);
	ILI9806G_Write_Data(0x01);
	ILI9806G_Write_Data(0x06);
	ILI9806G_Write_Data(0x09);
	ILI9806G_Write_Data(0x16);
	ILI9806G_Write_Data(0x14);
	ILI9806G_Write_Data(0x3E);
	ILI9806G_Write_Data(0x3E);
	ILI9806G_Write_Data(0x00);
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0xE1);
	ILI9806G_Write_Data(0x00);
	ILI9806G_Write_Data(0x09);
	ILI9806G_Write_Data(0x12);
	ILI9806G_Write_Data(0x12);
	ILI9806G_Write_Data(0x13);
	ILI9806G_Write_Data(0x1c);
	ILI9806G_Write_Data(0x07);
	ILI9806G_Write_Data(0x08);
	ILI9806G_Write_Data(0x05);
	ILI9806G_Write_Data(0x08);
	ILI9806G_Write_Data(0x03);
	ILI9806G_Write_Data(0x02);
	ILI9806G_Write_Data(0x04);
	ILI9806G_Write_Data(0x1E);
	ILI9806G_Write_Data(0x1B);
	ILI9806G_Write_Data(0x00);
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0x3A);
	ILI9806G_Write_Data(0x55); 
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0x35);
	ILI9806G_Write_Data(0x00); 
	DEBUG_DELAY();
	ILI9806G_Write_Cmd(0x11);
	DEBUG_DELAY() ;
	ILI9806G_Write_Cmd(0x29);	   
	DEBUG_DELAY()  ; 
#endif	
}


/**
 * @brief  ILI9806G��ʼ�����������Ҫ�õ�lcd��һ��Ҫ�����������
 * @param  ��
 * @retval ��
 */
void ILI9806G_Init ( void )
{
	ILI9806G_GPIO_Config ();
	ILI9806G_FSMC_Config ();
	
	
	ILI9806G_Rst ();
	ILI9806G_REG_Config ();
	
	//����Ĭ��ɨ�跽������ 6 ģʽΪ�󲿷�Һ�����̵�Ĭ����ʾ����  
	ILI9806G_GramScan(LCD_SCAN_MODE);
    
	ILI9806G_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* ��������ʾȫ�� */
	ILI9806G_BackLed_Control ( ENABLE );      //����LCD�����
}


/**
 * @brief  ILI9806G����LED����
 * @param  enumState �������Ƿ�ʹ�ܱ���LED
  *   �ò���Ϊ����ֵ֮һ��
  *     @arg ENABLE :ʹ�ܱ���LED
  *     @arg DISABLE :���ñ���LED
 * @retval ��
 */
void ILI9806G_BackLed_Control ( FunctionalState enumState )
{
	if ( enumState )
		 GPIO_SetBits( ILI9806G_BK_PORT, ILI9806G_BK_PIN );	
	else
		 GPIO_ResetBits( ILI9806G_BK_PORT, ILI9806G_BK_PIN );
		
}



/**
 * @brief  ILI9806G �����λ
 * @param  ��
 * @retval ��
 */
void ILI9806G_Rst ( void )
{			
	GPIO_ResetBits ( ILI9806G_RST_PORT, ILI9806G_RST_PIN );	 //�͵�ƽ��λ

	ILI9806G_Delay ( 0xAFF ); 					   

	GPIO_SetBits ( ILI9806G_RST_PORT, ILI9806G_RST_PIN );		 	 

	ILI9806G_Delay ( 0xAFF ); 	
	
}




/**
 * @brief  ����ILI9806G��GRAM��ɨ�跽�� 
 * @param  ucOption ��ѡ��GRAM��ɨ�跽�� 
 *     @arg 0-7 :������ѡֵΪ0-7��˸�����
 *
 *	����������0��3��5��6 ģʽ�ʺϴ���������ʾ���֣�
 *				���Ƽ�ʹ������ģʽ��ʾ����	����ģʽ��ʾ���ֻ��о���Ч��			
 *		
 *	����0��2��4��6 ģʽ��X��������Ϊ480��Y��������Ϊ854
 *	����1��3��5��7 ģʽ��X��������Ϊ854��Y��������Ϊ480
 *
 *	���� 6 ģʽΪ�󲿷�Һ�����̵�Ĭ����ʾ����
 *	���� 3 ģʽΪ����ͷ����ʹ�õķ���
 *	���� 0 ģʽΪBMPͼƬ��ʾ����ʹ�õķ���
 *
 * @retval ��
 * @note  ����ͼ����A��ʾ���ϣ�V��ʾ���£�<��ʾ����>��ʾ����
					X��ʾX�ᣬY��ʾY��

------------------------------------------------------------
ģʽ0��				.		ģʽ1��		.	ģʽ2��			.	ģʽ3��					
					A		.					A		.		A					.		A									
					|		.					|		.		|					.		|							
					Y		.					X		.		Y					.		X					
					0		.					1		.		2					.		3					
	<--- X0 o		.	<----Y1	o		.		o 2X--->  .		o 3Y--->	
------------------------------------------------------------	
ģʽ4��				.	ģʽ5��			.	ģʽ6��			.	ģʽ7��					
	<--- X4 o		.	<--- Y5 o		.		o 6X--->  .		o 7Y--->	
					4		.					5		.		6					.		7	
					Y		.					X		.		Y					.		X						
					|		.					|		.		|					.		|							
					V		.					V		.		V					.		V		
---------------------------------------------------------				
											 LCD��ʾ��
								|-----------------|
								|			Ұ��Logo		|
								|									|
								|									|
								|									|
								|									|
								|									|
								|									|
								|									|
								|									|
								|-----------------|
								��Ļ���棨��480����854��

 *******************************************************/
void ILI9806G_GramScan ( uint8_t ucOption )
{	
	//������飬ֻ������0-7
	if(ucOption >7 )
		return;
	
	//����ģʽ����LCD_SCAN_MODE��ֵ����Ҫ���ڴ�����ѡ��������
	LCD_SCAN_MODE = ucOption;
	
	//����ģʽ����XY��������ؿ��
	if(ucOption%2 == 0)	
	{
		//0 2 4 6ģʽ��X�������ؿ��Ϊ480��Y����Ϊ854
		LCD_X_LENGTH = ILI9806G_LESS_PIXEL;
		LCD_Y_LENGTH =	ILI9806G_MORE_PIXEL;
	}
	else				
	{
		//1 3 5 7ģʽ��X�������ؿ��Ϊ854��Y����Ϊ480
		LCD_X_LENGTH = ILI9806G_MORE_PIXEL;
		LCD_Y_LENGTH =	ILI9806G_LESS_PIXEL; 
	}

	//0x36��������ĸ�3λ����������GRAMɨ�跽��	
	ILI9806G_Write_Cmd ( 0x36 ); 
	ILI9806G_Write_Data (0x00 | (ucOption<<5));//����ucOption��ֵ����LCD��������0-7��ģʽ
	ILI9806G_Write_Cmd ( CMD_SetCoordinateX ); 
	ILI9806G_Write_Data ( 0x00 );		/* x ��ʼ�����8λ */
	ILI9806G_Write_Data ( 0x00 );		/* x ��ʼ�����8λ */
	ILI9806G_Write_Data ( ((LCD_X_LENGTH-1)>>8)&0xFF ); /* x ���������8λ */	
	ILI9806G_Write_Data ( (LCD_X_LENGTH-1)&0xFF );				/* x ���������8λ */

	ILI9806G_Write_Cmd ( CMD_SetCoordinateY ); 
	ILI9806G_Write_Data ( 0x00 );		/* y ��ʼ�����8λ */
	ILI9806G_Write_Data ( 0x00 );		/* y ��ʼ�����8λ */
	ILI9806G_Write_Data ( ((LCD_Y_LENGTH-1)>>8)&0xFF );	/* y ���������8λ */	 
	ILI9806G_Write_Data ( (LCD_Y_LENGTH-1)&0xFF );				/* y ���������8λ */

	/* write gram start */
	ILI9806G_Write_Cmd ( CMD_SetPixel );	
}



/**
 * @brief  ��ILI9806G��ʾ���Ͽ���һ������
 * @param  usX �����ض�ɨ�跽���´��ڵ����X����
 * @param  usY �����ض�ɨ�跽���´��ڵ����Y����
 * @param  usWidth �����ڵĿ��
 * @param  usHeight �����ڵĸ߶�
 * @retval ��
 */
void ILI9806G_OpenWindow ( uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight )
{	
	ILI9806G_Write_Cmd ( CMD_SetCoordinateX ); 				 /* ����X���� */
	ILI9806G_Write_Data ( usX >> 8  );	 /* �ȸ�8λ��Ȼ���8λ */
	ILI9806G_Write_Data ( usX & 0xff  );	 /* ������ʼ��ͽ�����*/
	ILI9806G_Write_Data ( ( usX + usWidth - 1 ) >> 8  );
	ILI9806G_Write_Data ( ( usX + usWidth - 1 ) & 0xff  );

	ILI9806G_Write_Cmd ( CMD_SetCoordinateY ); 			     /* ����Y����*/
	ILI9806G_Write_Data ( usY >> 8  );
	ILI9806G_Write_Data ( usY & 0xff  );
	ILI9806G_Write_Data ( ( usY + usHeight - 1 ) >> 8 );
	ILI9806G_Write_Data ( ( usY + usHeight - 1) & 0xff );
	
}


/**
 * @brief  �趨ILI9806G�Ĺ������
 * @param  usX �����ض�ɨ�跽���¹���X����
 * @param  usY �����ض�ɨ�跽���¹���Y����
 * @retval ��
 */
static void ILI9806G_SetCursor ( uint16_t usX, uint16_t usY )	
{
	ILI9806G_OpenWindow ( usX, usY, 1, 1 );
}


/**
 * @brief  ��ILI9806G��ʾ������ĳһ��ɫ������ص�
 * @param  ulAmout_Point ��Ҫ�����ɫ�����ص������Ŀ
 * @param  usColor ����ɫ
 * @retval ��
 */
static __inline void ILI9806G_FillColor ( uint32_t ulAmout_Point, uint16_t usColor )
{
	uint32_t i = 0;
	
	
	/* memory write */
	ILI9806G_Write_Cmd ( CMD_SetPixel );	
		
	for ( i = 0; i < ulAmout_Point; i ++ )
		ILI9806G_Write_Data ( usColor );
		
	
}


/**
 * @brief  ��ILI9806G��ʾ����ĳһ������ĳ����ɫ��������
 * @param  usX �����ض�ɨ�跽���´��ڵ����X����
 * @param  usY �����ض�ɨ�跽���´��ڵ����Y����
 * @param  usWidth �����ڵĿ��
 * @param  usHeight �����ڵĸ߶�
 * @note ��ʹ��LCD_SetBackColor��LCD_SetTextColor��LCD_SetColors����������ɫ
 * @retval ��
 */
void ILI9806G_Clear ( uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight )
{
	ILI9806G_OpenWindow ( usX, usY, usWidth, usHeight );

	ILI9806G_FillColor ( usWidth * usHeight, CurrentBackColor );		
	
}


/**
 * @brief  ��ILI9806G��ʾ����ĳһ����ĳ����ɫ�������
 * @param  usX �����ض�ɨ�跽���¸õ��X����
 * @param  usY �����ض�ɨ�跽���¸õ��Y����
 * @note ��ʹ��LCD_SetBackColor��LCD_SetTextColor��LCD_SetColors����������ɫ
 * @retval ��
 */
void ILI9806G_SetPointPixel ( uint16_t usX, uint16_t usY )	
{	
	if ( ( usX < LCD_X_LENGTH ) && ( usY < LCD_Y_LENGTH ) )
  {
		ILI9806G_SetCursor ( usX, usY );
		
		ILI9806G_FillColor ( 1, CurrentTextColor );
	}
	
}


/**
 * @brief  ��ȡILI9806G GRAN ��һ����������
 * @param  ��
 * @retval ��������
 */
static uint16_t ILI9806G_Read_PixelData ( void )	
{	
	uint16_t usR=0, usG=0, usB=0 ;

	
	ILI9806G_Write_Cmd ( 0x2E );   /* ������ */
	
	usR = ILI9806G_Read_Data (); 	/*FIRST READ OUT DUMMY DATA*/
	
	usR = ILI9806G_Read_Data ();  	/*READ OUT RED DATA  */
	usB = ILI9806G_Read_Data ();  	/*READ OUT BLUE DATA*/
	usG = ILI9806G_Read_Data ();  	/*READ OUT GREEN DATA*/	
	
  return ( ( ( usR >> 11 ) << 11 ) | ( ( usG >> 10 ) << 5 ) | ( usB >> 11 ) );
	
}


/**
 * @brief  ��ȡ ILI9806G ��ʾ����ĳһ����������������
 * @param  usX �����ض�ɨ�跽���¸õ��X����
 * @param  usY �����ض�ɨ�跽���¸õ��Y����
 * @retval ��������
 */
uint16_t ILI9806G_GetPointPixel ( uint16_t usX, uint16_t usY )
{ 
	uint16_t usPixelData;

	
	ILI9806G_SetCursor ( usX, usY );
	
	usPixelData = ILI9806G_Read_PixelData ();
	
	return usPixelData;
	
}


/**
 * @brief  �� ILI9806G ��ʾ����ʹ�� Bresenham �㷨���߶� 
 * @param  usX1 �����ض�ɨ�跽�����߶ε�һ���˵�X����
 * @param  usY1 �����ض�ɨ�跽�����߶ε�һ���˵�Y����
 * @param  usX2 �����ض�ɨ�跽�����߶ε���һ���˵�X����
 * @param  usY2 �����ض�ɨ�跽�����߶ε���һ���˵�Y����
 * @note ��ʹ��LCD_SetBackColor��LCD_SetTextColor��LCD_SetColors����������ɫ
 * @retval ��
 */
void ILI9806G_DrawLine ( uint16_t usX1, uint16_t usY1, uint16_t usX2, uint16_t usY2 )
{
	uint16_t us; 
	uint16_t usX_Current, usY_Current;
	
	int32_t lError_X = 0, lError_Y = 0, lDelta_X, lDelta_Y, lDistance; 
	int32_t lIncrease_X, lIncrease_Y; 	
	
	
	lDelta_X = usX2 - usX1; //������������ 
	lDelta_Y = usY2 - usY1; 
	
	usX_Current = usX1; 
	usY_Current = usY1; 
	
	
	if ( lDelta_X > 0 ) 
		lIncrease_X = 1; //���õ������� 
	
	else if ( lDelta_X == 0 ) 
		lIncrease_X = 0;//��ֱ�� 
	
	else 
  { 
    lIncrease_X = -1;
    lDelta_X = - lDelta_X;
  } 

	
	if ( lDelta_Y > 0 )
		lIncrease_Y = 1; 
	
	else if ( lDelta_Y == 0 )
		lIncrease_Y = 0;//ˮƽ�� 
	
	else 
  {
    lIncrease_Y = -1;
    lDelta_Y = - lDelta_Y;
  } 

	
	if (  lDelta_X > lDelta_Y )
		lDistance = lDelta_X; //ѡȡ�������������� 
	
	else 
		lDistance = lDelta_Y; 

	
	for ( us = 0; us <= lDistance + 1; us ++ )//������� 
	{  
		ILI9806G_SetPointPixel ( usX_Current, usY_Current );//���� 
		
		lError_X += lDelta_X ; 
		lError_Y += lDelta_Y ; 
		
		if ( lError_X > lDistance ) 
		{ 
			lError_X -= lDistance; 
			usX_Current += lIncrease_X; 
		}  
		
		if ( lError_Y > lDistance ) 
		{ 
			lError_Y -= lDistance; 
			usY_Current += lIncrease_Y; 
		} 
		
	}  
	
	
}   


/**
 * @brief  �� ILI9806G ��ʾ���ϻ�һ������
 * @param  usX_Start �����ض�ɨ�跽���¾��ε���ʼ��X����
 * @param  usY_Start �����ض�ɨ�跽���¾��ε���ʼ��Y����
 * @param  usWidth�����εĿ�ȣ���λ�����أ�
 * @param  usHeight�����εĸ߶ȣ���λ�����أ�
 * @param  ucFilled ��ѡ���Ƿ����þ���
  *   �ò���Ϊ����ֵ֮һ��
  *     @arg 0 :���ľ���
  *     @arg 1 :ʵ�ľ��� 
 * @note ��ʹ��LCD_SetBackColor��LCD_SetTextColor��LCD_SetColors����������ɫ
 * @retval ��
 */
void ILI9806G_DrawRectangle ( uint16_t usX_Start, uint16_t usY_Start, uint16_t usWidth, uint16_t usHeight, uint8_t ucFilled )
{
	if ( ucFilled )
	{
		ILI9806G_OpenWindow ( usX_Start, usY_Start, usWidth, usHeight );
		ILI9806G_FillColor ( usWidth * usHeight ,CurrentTextColor);	
	}
	else
	{
		ILI9806G_DrawLine ( usX_Start, usY_Start, usX_Start + usWidth - 1, usY_Start );
		ILI9806G_DrawLine ( usX_Start, usY_Start + usHeight - 1, usX_Start + usWidth - 1, usY_Start + usHeight - 1 );
		ILI9806G_DrawLine ( usX_Start, usY_Start, usX_Start, usY_Start + usHeight - 1 );
		ILI9806G_DrawLine ( usX_Start + usWidth - 1, usY_Start, usX_Start + usWidth - 1, usY_Start + usHeight - 1 );		
	}

}


/**
 * @brief  �� ILI9806G ��ʾ����ʹ�� Bresenham �㷨��Բ
 * @param  usX_Center �����ض�ɨ�跽����Բ�ĵ�X����
 * @param  usY_Center �����ض�ɨ�跽����Բ�ĵ�Y����
 * @param  usRadius��Բ�İ뾶����λ�����أ�
 * @param  ucFilled ��ѡ���Ƿ�����Բ
  *   �ò���Ϊ����ֵ֮һ��
  *     @arg 0 :����Բ
  *     @arg 1 :ʵ��Բ
 * @note ��ʹ��LCD_SetBackColor��LCD_SetTextColor��LCD_SetColors����������ɫ
 * @retval ��
 */
void ILI9806G_DrawCircle ( uint16_t usX_Center, uint16_t usY_Center, uint16_t usRadius, uint8_t ucFilled )
{
	int16_t sCurrentX, sCurrentY;
	int16_t sError;
	
	
	sCurrentX = 0; sCurrentY = usRadius;	  
	
	sError = 3 - ( usRadius << 1 );     //�ж��¸���λ�õı�־
	
	
	while ( sCurrentX <= sCurrentY )
	{
		int16_t sCountY;
		
		
		if ( ucFilled ) 			
			for ( sCountY = sCurrentX; sCountY <= sCurrentY; sCountY ++ ) 
			{                      
				ILI9806G_SetPointPixel ( usX_Center + sCurrentX, usY_Center + sCountY );           //1���о����� 
				ILI9806G_SetPointPixel ( usX_Center - sCurrentX, usY_Center + sCountY );           //2       
				ILI9806G_SetPointPixel ( usX_Center - sCountY,   usY_Center + sCurrentX );           //3
				ILI9806G_SetPointPixel ( usX_Center - sCountY,   usY_Center - sCurrentX );           //4
				ILI9806G_SetPointPixel ( usX_Center - sCurrentX, usY_Center - sCountY );           //5    
        ILI9806G_SetPointPixel ( usX_Center + sCurrentX, usY_Center - sCountY );           //6
				ILI9806G_SetPointPixel ( usX_Center + sCountY,   usY_Center - sCurrentX );           //7 	
        ILI9806G_SetPointPixel ( usX_Center + sCountY,   usY_Center + sCurrentX );           //0				
			}
		
		else
		{          
			ILI9806G_SetPointPixel ( usX_Center + sCurrentX, usY_Center + sCurrentY );             //1���о�����
			ILI9806G_SetPointPixel ( usX_Center - sCurrentX, usY_Center + sCurrentY );             //2      
			ILI9806G_SetPointPixel ( usX_Center - sCurrentY, usY_Center + sCurrentX );             //3
			ILI9806G_SetPointPixel ( usX_Center - sCurrentY, usY_Center - sCurrentX );             //4
			ILI9806G_SetPointPixel ( usX_Center - sCurrentX, usY_Center - sCurrentY );             //5       
			ILI9806G_SetPointPixel ( usX_Center + sCurrentX, usY_Center - sCurrentY );             //6
			ILI9806G_SetPointPixel ( usX_Center + sCurrentY, usY_Center - sCurrentX );             //7 
			ILI9806G_SetPointPixel ( usX_Center + sCurrentY, usY_Center + sCurrentX );             //0
    }			
		
		
		sCurrentX ++;

		
		if ( sError < 0 ) 
			sError += 4 * sCurrentX + 6;	  
		
		else
		{
			sError += 10 + 4 * ( sCurrentX - sCurrentY );   
			sCurrentY --;
		} 	
		
		
	}
	
	
}

/**
 * @brief  �� ILI9806G ��ʾ������ʾһ��Ӣ���ַ�
 * @param  usX �����ض�ɨ�跽�����ַ�����ʼX����
 * @param  usY �����ض�ɨ�跽���¸õ����ʼY����
 * @param  cChar ��Ҫ��ʾ��Ӣ���ַ�
 * @note ��ʹ��LCD_SetBackColor��LCD_SetTextColor��LCD_SetColors����������ɫ
 * @retval ��
 */
void ILI9806G_DispChar_EN ( uint16_t usX, uint16_t usY, const char cChar )
{
	uint8_t  byteCount, bitCount,fontLength;	
	uint16_t ucRelativePositon;
	uint8_t *Pfont;
	
	//��ascii���ƫ�ƣ���ģ������ASCII���ǰ32����ͼ�η��ţ�
	ucRelativePositon = cChar - ' ';
	
	//ÿ����ģ���ֽ���
	fontLength = (LCD_Currentfonts->Width*LCD_Currentfonts->Height)/8;
		
	//��ģ�׵�ַ
	/*ascii���ƫ��ֵ����ÿ����ģ���ֽ����������ģ��ƫ��λ��*/
	Pfont = (uint8_t *)&LCD_Currentfonts->table[ucRelativePositon * fontLength];
	
	//������ʾ����
	ILI9806G_OpenWindow ( usX, usY, LCD_Currentfonts->Width, LCD_Currentfonts->Height);
	
	ILI9806G_Write_Cmd ( CMD_SetPixel );			

	//���ֽڶ�ȡ��ģ����
	//����ǰ��ֱ����������ʾ���ڣ���ʾ���ݻ��Զ�����
	for ( byteCount = 0; byteCount < fontLength; byteCount++ )
	{
			//һλһλ����Ҫ��ʾ����ɫ
			for ( bitCount = 0; bitCount < 8; bitCount++ )
			{
					if ( Pfont[byteCount] & (0x80>>bitCount) )
						ILI9806G_Write_Data ( CurrentTextColor );			
					else
						ILI9806G_Write_Data ( CurrentBackColor );
			}	
	}	
}


/**
 * @brief  �� ILI9806G ��ʾ������ʾӢ���ַ���
 * @param  line �����ض�ɨ�跽�����ַ�������ʼY����
  *   ��������ʹ�ú�LINE(0)��LINE(1)�ȷ�ʽָ���������꣬
  *   ��LINE(x)����ݵ�ǰѡ�������������Y����ֵ��
	*		��ʾ������ʹ��LINE��ʱ����Ҫ��Ӣ���������ó�Font8x16
 * @param  pStr ��Ҫ��ʾ��Ӣ���ַ������׵�ַ
 * @note ��ʹ��LCD_SetBackColor��LCD_SetTextColor��LCD_SetColors����������ɫ
 * @retval ��
 */
void ILI9806G_DispStringLine_EN (  uint16_t line,  char * pStr )
{
	uint16_t usX = 0;
	
	while ( * pStr != '\0' )
	{
		if ( ( usX - ILI9806G_DispWindow_X_Star + LCD_Currentfonts->Width ) > LCD_X_LENGTH )
		{
			usX = ILI9806G_DispWindow_X_Star;
			line += LCD_Currentfonts->Height;
		}
		
		if ( ( line - ILI9806G_DispWindow_Y_Star + LCD_Currentfonts->Height ) > LCD_Y_LENGTH )
		{
			usX = ILI9806G_DispWindow_X_Star;
			line = ILI9806G_DispWindow_Y_Star;
		}
		
		ILI9806G_DispChar_EN ( usX, line, * pStr);
		
		pStr ++;
		
		usX += LCD_Currentfonts->Width;
		
	}
	
}


/**
 * @brief  �� ILI9806G ��ʾ������ʾӢ���ַ���
 * @param  usX �����ض�ɨ�跽�����ַ�����ʼX����
 * @param  usY �����ض�ɨ�跽�����ַ�����ʼY����
 * @param  pStr ��Ҫ��ʾ��Ӣ���ַ������׵�ַ
 * @note ��ʹ��LCD_SetBackColor��LCD_SetTextColor��LCD_SetColors����������ɫ
 * @retval ��
 */
void ILI9806G_DispString_EN ( 	uint16_t usX ,uint16_t usY,  char * pStr )
{
	while ( * pStr != '\0' )
	{
		if ( ( usX - ILI9806G_DispWindow_X_Star + LCD_Currentfonts->Width ) > LCD_X_LENGTH )
		{
			usX = ILI9806G_DispWindow_X_Star;
			usY += LCD_Currentfonts->Height;
		}
		
		if ( ( usY - ILI9806G_DispWindow_Y_Star + LCD_Currentfonts->Height ) > LCD_Y_LENGTH )
		{
			usX = ILI9806G_DispWindow_X_Star;
			usY = ILI9806G_DispWindow_Y_Star;
		}
		
		ILI9806G_DispChar_EN ( usX, usY, * pStr);
		
		pStr ++;
		
		usX += LCD_Currentfonts->Width;
		
	}
	
}


/**
 * @brief  �� ILI9806G ��ʾ������ʾӢ���ַ���(��Y�᷽��)
 * @param  usX �����ض�ɨ�跽�����ַ�����ʼX����
 * @param  usY �����ض�ɨ�跽�����ַ�����ʼY����
 * @param  pStr ��Ҫ��ʾ��Ӣ���ַ������׵�ַ
 * @note ��ʹ��LCD_SetBackColor��LCD_SetTextColor��LCD_SetColors����������ɫ
 * @retval ��
 */
void ILI9806G_DispString_EN_YDir (	 uint16_t usX,uint16_t usY ,  char * pStr )
{	
	while ( * pStr != '\0' )
	{
		if ( ( usY - ILI9806G_DispWindow_Y_Star + LCD_Currentfonts->Height ) >LCD_Y_LENGTH  )
		{
			usY = ILI9806G_DispWindow_Y_Star;
			usX += LCD_Currentfonts->Width;
		}
		
		if ( ( usX - ILI9806G_DispWindow_X_Star + LCD_Currentfonts->Width ) >  LCD_X_LENGTH)
		{
			usX = ILI9806G_DispWindow_X_Star;
			usY = ILI9806G_DispWindow_Y_Star;
		}
		
		ILI9806G_DispChar_EN ( usX, usY, * pStr);
		
		pStr ++;
		
		usY += LCD_Currentfonts->Height;		
	}	
}



/**
 * @brief  �� ILI9806G ��ʾ������ʾһ�������ַ�
 * @param  usX �����ض�ɨ�跽�����ַ�����ʼX����
 * @param  usY �����ض�ɨ�跽�����ַ�����ʼY����
 * @param  usChar ��Ҫ��ʾ�������ַ��������룩
 * @note ��ʹ��LCD_SetBackColor��LCD_SetTextColor��LCD_SetColors����������ɫ
 * @retval ��
 */ 
void ILI9806G_DispChar_CH ( uint16_t usX, uint16_t usY, uint16_t usChar )
{
	uint8_t rowCount, bitCount;
  uint32_t usTemp; 
	
//	ռ�ÿռ�̫�󣬸ĳ�ȫ�ֱ��� 
//	uint8_t ucBuffer [ WIDTH_CH_CHAR*HEIGHT_CH_CHAR/8 ];	
	
	//������ʾ����
	ILI9806G_OpenWindow ( usX, usY, WIDTH_CH_CHAR, HEIGHT_CH_CHAR );
	
	ILI9806G_Write_Cmd ( CMD_SetPixel );
	
	//ȡ��ģ����  
  GetGBKCode ( ucBuffer, usChar );	
	
	for ( rowCount = 0; rowCount < HEIGHT_CH_CHAR; rowCount++ )
	{
    /* ȡ���ĸ��ֽڵ����ݣ���lcd�ϼ���һ�����ֵ�һ�� */
		usTemp = ucBuffer [ rowCount * 4 ];
		usTemp = ( usTemp << 8 );
		usTemp |= ucBuffer [ rowCount * 4 + 1 ];
		usTemp = ( usTemp << 8 );
		usTemp |= ucBuffer [ rowCount * 4 + 2 ];
		usTemp = ( usTemp << 8 );
		usTemp |= ucBuffer [ rowCount * 4 + 3 ];
		
		for ( bitCount = 0; bitCount < WIDTH_CH_CHAR; bitCount ++ )
		{			
			if ( usTemp & ( 0x80000000 >> bitCount ) )  //��λ��ǰ 
			  ILI9806G_Write_Data ( CurrentTextColor );				
			else
				ILI9806G_Write_Data ( CurrentBackColor );			
		}		
	}
	
}


/**
 * @brief  �� ILI9806G ��ʾ������ʾ�����ַ���
 * @param  line �����ض�ɨ�跽�����ַ�������ʼY����
  *   ��������ʹ�ú�LINE(0)��LINE(1)�ȷ�ʽָ���������꣬
  *   ��LINE(x)����ݵ�ǰѡ�������������Y����ֵ��
	*		��ʾ������ʹ��LINE��ʱ����Ҫ��Ӣ���������ó�Font8x16
 * @param  pStr ��Ҫ��ʾ��Ӣ���ַ������׵�ַ
 * @note ��ʹ��LCD_SetBackColor��LCD_SetTextColor��LCD_SetColors����������ɫ
 * @retval ��
 */
void ILI9806G_DispString_CH ( 	uint16_t usX , uint16_t usY, char * pStr )
{	
	uint16_t usCh;

	
	while( * pStr != '\0' )
	{		
		if ( ( usX - ILI9806G_DispWindow_X_Star + WIDTH_CH_CHAR ) > LCD_X_LENGTH )
		{
			usX = ILI9806G_DispWindow_X_Star;
			usY += HEIGHT_CH_CHAR;
		}
		
		if ( ( usY - ILI9806G_DispWindow_Y_Star + HEIGHT_CH_CHAR ) > LCD_Y_LENGTH )
		{
			usX = ILI9806G_DispWindow_X_Star;
			usY = ILI9806G_DispWindow_Y_Star;
		}	
		
		usCh = * ( uint16_t * ) pStr;	
	  usCh = ( usCh << 8 ) + ( usCh >> 8 );

		ILI9806G_DispChar_CH ( usX, usY, usCh );
		
		usX += WIDTH_CH_CHAR;
		
		pStr += 2;           //һ�����������ֽ� 

	}	   
	
}


/**
 * @brief  �� ILI9806G ��ʾ������ʾ��Ӣ���ַ���
 * @param  line �����ض�ɨ�跽�����ַ�������ʼY����
  *   ��������ʹ�ú�LINE(0)��LINE(1)�ȷ�ʽָ���������꣬
  *   ��LINE(x)����ݵ�ǰѡ�������������Y����ֵ��
	*		��ʾ������ʹ��LINE��ʱ����Ҫ��Ӣ���������ó�Font8x16
 * @param  pStr ��Ҫ��ʾ���ַ������׵�ַ
 * @note ��ʹ��LCD_SetBackColor��LCD_SetTextColor��LCD_SetColors����������ɫ
 * @retval ��
 */
void ILI9806G_DispStringLine_EN_CH (  uint16_t line, char * pStr )
{
	uint16_t usCh;
	uint16_t usX = 0;
	
	while( * pStr != '\0' )
	{
		if ( * pStr <= 126 )	           	//Ӣ���ַ�
		{
			if ( ( usX - ILI9806G_DispWindow_X_Star + LCD_Currentfonts->Width ) > LCD_X_LENGTH )
			{
				usX = ILI9806G_DispWindow_X_Star;
				line += LCD_Currentfonts->Height;
			}
			
			if ( ( line - ILI9806G_DispWindow_Y_Star + LCD_Currentfonts->Height ) > LCD_Y_LENGTH )
			{
				usX = ILI9806G_DispWindow_X_Star;
				line = ILI9806G_DispWindow_Y_Star;
			}			
		
		  ILI9806G_DispChar_EN ( usX, line, * pStr );
			
			usX +=  LCD_Currentfonts->Width;
		
		  pStr ++;

		}
		
		else	                            //�����ַ�
		{
			if ( ( usX - ILI9806G_DispWindow_X_Star + WIDTH_CH_CHAR ) > LCD_X_LENGTH )
			{
				usX = ILI9806G_DispWindow_X_Star;
				line += HEIGHT_CH_CHAR;
			}
			
			if ( ( line - ILI9806G_DispWindow_Y_Star + HEIGHT_CH_CHAR ) > LCD_Y_LENGTH )
			{
				usX = ILI9806G_DispWindow_X_Star;
				line = ILI9806G_DispWindow_Y_Star;
			}	
			
			usCh = * ( uint16_t * ) pStr;	
			
			usCh = ( usCh << 8 ) + ( usCh >> 8 );		

			ILI9806G_DispChar_CH ( usX, line, usCh );
			
			usX += WIDTH_CH_CHAR;
			
			pStr += 2;           //һ�����������ֽ� 
		
    }
		
  }	
} 

/**
 * @brief  �� ILI9806G ��ʾ������ʾ��Ӣ���ַ���
 * @param  usX �����ض�ɨ�跽�����ַ�����ʼX����
 * @param  usY �����ض�ɨ�跽�����ַ�����ʼY����
 * @param  pStr ��Ҫ��ʾ���ַ������׵�ַ
 * @note ��ʹ��LCD_SetBackColor��LCD_SetTextColor��LCD_SetColors����������ɫ
 * @retval ��
 */
void ILI9806G_DispString_EN_CH ( 	uint16_t usX , uint16_t usY, char * pStr )
{
	uint16_t usCh;
	
	while( * pStr != '\0' )
	{
		if ( * pStr <= 126 )	           	//Ӣ���ַ�
		{
			if ( ( usX - ILI9806G_DispWindow_X_Star + LCD_Currentfonts->Width ) > LCD_X_LENGTH )
			{
				usX = ILI9806G_DispWindow_X_Star;
				usY += LCD_Currentfonts->Height;
			}
			
			if ( ( usY - ILI9806G_DispWindow_Y_Star + LCD_Currentfonts->Height ) > LCD_Y_LENGTH )
			{
				usX = ILI9806G_DispWindow_X_Star;
				usY = ILI9806G_DispWindow_Y_Star;
			}			
		
		  ILI9806G_DispChar_EN ( usX, usY, * pStr );
			
			usX +=  LCD_Currentfonts->Width;
		
		  pStr ++;

		}
		
		else	                            //�����ַ�
		{
			if ( ( usX - ILI9806G_DispWindow_X_Star + WIDTH_CH_CHAR ) > LCD_X_LENGTH )
			{
				usX = ILI9806G_DispWindow_X_Star;
				usY += HEIGHT_CH_CHAR;
			}
			
			if ( ( usY - ILI9806G_DispWindow_Y_Star + HEIGHT_CH_CHAR ) > LCD_Y_LENGTH )
			{
				usX = ILI9806G_DispWindow_X_Star;
				usY = ILI9806G_DispWindow_Y_Star;
			}	
			
			usCh = * ( uint16_t * ) pStr;	
			
			usCh = ( usCh << 8 ) + ( usCh >> 8 );		

			ILI9806G_DispChar_CH ( usX, usY, usCh );
			
			usX += WIDTH_CH_CHAR;
			
			pStr += 2;           //һ�����������ֽ� 
		
    }
		
  }	
} 

/**
 * @brief  �� ILI9806G ��ʾ������ʾ��Ӣ���ַ���(��Y�᷽��)
 * @param  usX �����ض�ɨ�跽�����ַ�����ʼX����
 * @param  usY �����ض�ɨ�跽�����ַ�����ʼY����
 * @param  pStr ��Ҫ��ʾ����Ӣ���ַ������׵�ַ
 * @note ��ʹ��LCD_SetBackColor��LCD_SetTextColor��LCD_SetColors����������ɫ
 * @retval ��
 */
void ILI9806G_DispString_EN_CH_YDir (  uint16_t usX,uint16_t usY , char * pStr )
{
	uint16_t usCh;
	
	while( * pStr != '\0' )
	{			
			//ͳһʹ�ú��ֵĿ�������㻻��
			if ( ( usY - ILI9806G_DispWindow_Y_Star + HEIGHT_CH_CHAR ) >LCD_Y_LENGTH  )
			{
				usY = ILI9806G_DispWindow_Y_Star;
				usX += WIDTH_CH_CHAR;
			}			
			if ( ( usX - ILI9806G_DispWindow_X_Star + WIDTH_CH_CHAR ) >  LCD_X_LENGTH)
			{
				usX = ILI9806G_DispWindow_X_Star;
				usY = ILI9806G_DispWindow_Y_Star;
			}
			
		//��ʾ	
		if ( * pStr <= 126 )	           	//Ӣ���ַ�
		{			
			ILI9806G_DispChar_EN ( usX, usY, * pStr);
			
			pStr ++;
			
			usY += HEIGHT_CH_CHAR;		
		}
		else	                            //�����ַ�
		{			
			usCh = * ( uint16_t * ) pStr;	
			
			usCh = ( usCh << 8 ) + ( usCh >> 8 );		

			ILI9806G_DispChar_CH ( usX,usY , usCh );
			
			usY += HEIGHT_CH_CHAR;
			
			pStr += 2;           //һ�����������ֽ� 
		
    }
		
  }	
} 

/***********************��������****************************/
#define ZOOMMAXBUFF 16384
uint8_t zoomBuff[ZOOMMAXBUFF] = {0};	//�������ŵĻ��棬���֧�ֵ�128*128
uint8_t zoomTempBuff[1024] = {0};

/**
 * @brief  ������ģ�����ź����ģ��1�����ص���8������λ����ʾ
										0x01��ʾ�ʼ���0x00��ʾ�հ���
 * @param  in_width ��ԭʼ�ַ����
 * @param  in_heig ��ԭʼ�ַ��߶�
 * @param  out_width �����ź���ַ����
 * @param  out_heig�����ź���ַ��߶�
 * @param  in_ptr ���ֿ�����ָ��	ע�⣺1pixel 1bit
 * @param  out_ptr �����ź���ַ����ָ�� ע��: 1pixel 8bit
 *		out_ptrʵ����û������������ĳ���ֱ�������ȫ��ָ��zoomBuff��
 * @param  en_cn ��0ΪӢ�ģ�1Ϊ����
 * @retval ��
 */
void ILI9806G_zoomChar(uint16_t in_width,	//ԭʼ�ַ����
									uint16_t in_heig,		//ԭʼ�ַ��߶�
									uint16_t out_width,	//���ź���ַ����
									uint16_t out_heig,	//���ź���ַ��߶�
									uint8_t *in_ptr,	//�ֿ�����ָ��	ע�⣺1pixel 1bit
									uint8_t *out_ptr, //���ź���ַ����ָ�� ע��: 1pixel 8bit
									uint8_t en_cn)		//0ΪӢ�ģ�1Ϊ����	
{
	uint8_t *pts,*ots;
	//����Դ��ģ��Ŀ����ģ��С���趨����������ӣ�����16��Ϊ�˰Ѹ�������ת�ɶ�������
	unsigned int xrIntFloat_16=(in_width<<16)/out_width+1; 
  unsigned int yrIntFloat_16=(in_heig<<16)/out_heig+1;
	
	unsigned int srcy_16=0;
	unsigned int y,x;
	uint8_t *pSrcLine;
	
	uint16_t byteCount,bitCount;
	
	//�������Ƿ�Ϸ�
	if(in_width > 48) return;												//�ֿⲻ������48����
	if(in_width * in_heig == 0) return;	
	if(in_width * in_heig > 48*48 ) return; 					//����������� 48*48
	
	if(out_width * out_heig == 0) return;	
	if(out_width * out_heig >= ZOOMMAXBUFF ) return; //����������� 128*128
	pts = (uint8_t*)&zoomTempBuff;
	
	//Ϊ�������㣬�ֿ��������1 pixel/1bit ӳ�䵽1pixel/8bit
	//0x01��ʾ�ʼ���0x00��ʾ�հ���
	if(en_cn == 0x00)//Ӣ��
	{
		//Ӣ�ĺ������ֿ����±߽粻�ԣ����ڴ˴���������Ҫע��tempBuff��ֹ���
			for(byteCount=0;byteCount<in_heig*in_width/8;byteCount++)	
			{
				for(bitCount=0;bitCount<8;bitCount++)
					{						
						//��Դ��ģ������λӳ�䵽�ֽ�
						//in_ptr��bitXΪ1����pts�������ֽ�ֵΪ1
						//in_ptr��bitXΪ0����pts�������ֽ�ֵΪ0
						*pts++ = (in_ptr[byteCount] & (0x80>>bitCount))?1:0; 
					}
			}				
	}
	else //����
	{			
			for(byteCount=0;byteCount<in_heig*in_width/8;byteCount++)	
			{
				for(bitCount=0;bitCount<8;bitCount++)
					{						
						//��Դ��ģ������λӳ�䵽�ֽ�
						//in_ptr��bitXΪ1����pts�������ֽ�ֵΪ1
						//in_ptr��bitXΪ0����pts�������ֽ�ֵΪ0
						*pts++ = (in_ptr[byteCount] & (0x80>>bitCount))?1:0; 
					}
			}		
	}

	//zoom����
	pts = (uint8_t*)&zoomTempBuff;	//ӳ����Դ����ָ��
	ots = (uint8_t*)&zoomBuff;	//������ݵ�ָ��
	for (y=0;y<out_heig;y++)	/*�б���*/
    {
				unsigned int srcx_16=0;
        pSrcLine=pts+in_width*(srcy_16>>16);				
        for (x=0;x<out_width;x++) /*�������ر���*/
        {
            ots[x]=pSrcLine[srcx_16>>16]; //��Դ��ģ���ݸ��Ƶ�Ŀ��ָ����
            srcx_16+=xrIntFloat_16;			//������ƫ��Դ���ص�
        }
        srcy_16+=yrIntFloat_16;				  //������ƫ��Դ���ص�
        ots+=out_width;						
    }
	/*���������ź����ģ����ֱ�Ӵ洢��ȫ��ָ��zoomBuff����*/
	out_ptr = (uint8_t*)&zoomBuff;	//out_ptrû����ȷ�������������ֱ�Ӹĳ���ȫ�ֱ���ָ�룡
	
	/*ʵ�������ʹ��out_ptr����Ҫ������һ�䣡����
		ֻ����Ϊout_ptrû��ʹ�ã��ᵼ��warning��ǿ��֢*/
	out_ptr++; 
}			


/**
 * @brief  �������ź����ģ��ʾ�ַ�
 * @param  Xpos ���ַ���ʾλ��x
 * @param  Ypos ���ַ���ʾλ��y
 * @param  Font_width ���ַ����
 * @param  Font_Heig���ַ��߶�
 * @param  c ��Ҫ��ʾ����ģ����
 * @param  DrawModel ���Ƿ�ɫ��ʾ 
 * @retval ��
 */
void ILI9806G_DrawChar_Ex(uint16_t usX, //�ַ���ʾλ��x
												uint16_t usY, //�ַ���ʾλ��y
												uint16_t Font_width, //�ַ����
												uint16_t Font_Height,  //�ַ��߶� 
												uint8_t *c,						//��ģ����
												uint16_t DrawModel)		//�Ƿ�ɫ��ʾ
{
  uint32_t index = 0, counter = 0;

	//������ʾ����
	ILI9806G_OpenWindow ( usX, usY, Font_width, Font_Height);
	
	ILI9806G_Write_Cmd ( CMD_SetPixel );		
	
	//���ֽڶ�ȡ��ģ����
	//����ǰ��ֱ����������ʾ���ڣ���ʾ���ݻ��Զ�����
	for ( index = 0; index < Font_Height; index++ )
	{
			//һλһλ����Ҫ��ʾ����ɫ
			for ( counter = 0; counter < Font_width; counter++ )
			{
					//���ź����ģ���ݣ���һ���ֽڱ�ʾһ������λ
					//�����ֽ�ֵΪ1��ʾ������Ϊ�ʼ�
					//�����ֽ�ֵΪ0��ʾ������Ϊ����
					if ( *c++ == DrawModel )
						ILI9806G_Write_Data ( CurrentBackColor );			
					else
						ILI9806G_Write_Data ( CurrentTextColor );
			}	
	}	
}


/**
 * @brief  �������ź����ģ��ʾ�ַ���
 * @param  Xpos ���ַ���ʾλ��x
 * @param  Ypos ���ַ���ʾλ��y
 * @param  Font_width ���ַ���ȣ�Ӣ���ַ��ڴ˻�����/2��ע��Ϊż��
 * @param  Font_Heig���ַ��߶ȣ�ע��Ϊż��
 * @param  c ��Ҫ��ʾ���ַ���
 * @param  DrawModel ���Ƿ�ɫ��ʾ 
 * @retval ��
 */
void ILI9806G_DisplayStringEx(uint16_t x, 		//�ַ���ʾλ��x
														 uint16_t y, 				//�ַ���ʾλ��y
														 uint16_t Font_width,	//Ҫ��ʾ�������ȣ�Ӣ���ַ��ڴ˻�����/2��ע��Ϊż��
														 uint16_t Font_Height,	//Ҫ��ʾ������߶ȣ�ע��Ϊż��
														 uint8_t *ptr,					//��ʾ���ַ�����
														 uint16_t DrawModel)  //�Ƿ�ɫ��ʾ



{
	uint16_t Charwidth = Font_width; //Ĭ��ΪFont_width��Ӣ�Ŀ��Ϊ���Ŀ�ȵ�һ��
	uint8_t *psr;
	uint8_t Ascii;	//Ӣ��
	uint16_t usCh;  //����
	
	//ռ�ÿռ�̫�󣬸ĳ�ȫ�ֱ���	
	//	uint8_t ucBuffer [ WIDTH_CH_CHAR*HEIGHT_CH_CHAR/8 ];	
	
	while ( *ptr != '\0')
	{
			/****������*****/
			if ( ( x - ILI9806G_DispWindow_X_Star + Charwidth ) > LCD_X_LENGTH )
			{
				x = ILI9806G_DispWindow_X_Star;
				y += Font_Height;
			}
			
			if ( ( y - ILI9806G_DispWindow_Y_Star + Font_Height ) > LCD_Y_LENGTH )
			{
				x = ILI9806G_DispWindow_X_Star;
				y = ILI9806G_DispWindow_Y_Star;
			}	
			
		if(*ptr > 0x80) //���������
		{			
			Charwidth = Font_width;
			usCh = * ( uint16_t * ) ptr;				
			usCh = ( usCh << 8 ) + ( usCh >> 8 );
			GetGBKCode ( ucBuffer, usCh );	//ȡ��ģ����
			//������ģ���ݣ�Դ��ģΪ32*32
			ILI9806G_zoomChar(WIDTH_CH_CHAR,HEIGHT_CH_CHAR,Charwidth,Font_Height,(uint8_t *)&ucBuffer,psr,1); 
			//��ʾ�����ַ�
			ILI9806G_DrawChar_Ex(x,y,Charwidth,Font_Height,(uint8_t*)&zoomBuff,DrawModel);
			x+=Charwidth;
			ptr+=2;
		}
		else
		{
				Charwidth = Font_width / 2;
				Ascii = *ptr - 32;
				//ʹ��16*32����������ģ����
				ILI9806G_zoomChar(16,32,Charwidth,Font_Height,(uint8_t *)&Font16x32.table[Ascii * Font16x32.Height*Font16x32.Width/8],psr,0);
			  //��ʾ�����ַ�
				ILI9806G_DrawChar_Ex(x,y,Charwidth,Font_Height,(uint8_t*)&zoomBuff,DrawModel);
				x+=Charwidth;
				ptr++;
		}
	}
}


/**
 * @brief  �������ź����ģ��ʾ�ַ���(��Y�᷽��)
 * @param  Xpos ���ַ���ʾλ��x
 * @param  Ypos ���ַ���ʾλ��y
 * @param  Font_width ���ַ���ȣ�Ӣ���ַ��ڴ˻�����/2��ע��Ϊż��
 * @param  Font_Heig���ַ��߶ȣ�ע��Ϊż��
 * @param  c ��Ҫ��ʾ���ַ���
 * @param  DrawModel ���Ƿ�ɫ��ʾ 
 * @retval ��
 */
void ILI9806G_DisplayStringEx_YDir(uint16_t x, 		//�ַ���ʾλ��x
																		 uint16_t y, 				//�ַ���ʾλ��y
																		 uint16_t Font_width,	//Ҫ��ʾ�������ȣ�Ӣ���ַ��ڴ˻�����/2��ע��Ϊż��
																		 uint16_t Font_Height,	//Ҫ��ʾ������߶ȣ�ע��Ϊż��
																		 uint8_t *ptr,					//��ʾ���ַ�����
																		 uint16_t DrawModel)  //�Ƿ�ɫ��ʾ
{
	uint16_t Charwidth = Font_width; //Ĭ��ΪFont_width��Ӣ�Ŀ��Ϊ���Ŀ�ȵ�һ��
	uint8_t *psr;
	uint8_t Ascii;	//Ӣ��
	uint16_t usCh;  //����
	uint8_t ucBuffer [ WIDTH_CH_CHAR*HEIGHT_CH_CHAR/8 ];	
	
	while ( *ptr != '\0')
	{			
			//ͳһʹ�ú��ֵĿ�������㻻��
			if ( ( y - ILI9806G_DispWindow_X_Star + Font_width ) > LCD_X_LENGTH )
			{
				y = ILI9806G_DispWindow_X_Star;
				x += Font_width;
			}
			
			if ( ( x - ILI9806G_DispWindow_Y_Star + Font_Height ) > LCD_Y_LENGTH )
			{
				y = ILI9806G_DispWindow_X_Star;
				x = ILI9806G_DispWindow_Y_Star;
			}	
			
		if(*ptr > 0x80) //���������
		{			
			Charwidth = Font_width;
			usCh = * ( uint16_t * ) ptr;				
			usCh = ( usCh << 8 ) + ( usCh >> 8 );
			GetGBKCode ( ucBuffer, usCh );	//ȡ��ģ����
			//������ģ���ݣ�Դ��ģΪ16*16
			ILI9806G_zoomChar(WIDTH_CH_CHAR,HEIGHT_CH_CHAR,Charwidth,Font_Height,(uint8_t *)&ucBuffer,psr,1); 
			//��ʾ�����ַ�
			ILI9806G_DrawChar_Ex(x,y,Charwidth,Font_Height,(uint8_t*)&zoomBuff,DrawModel);
			y+=Font_Height;
			ptr+=2;
		}
		else
		{
				Charwidth = Font_width / 2;
				Ascii = *ptr - 32;
				//ʹ��16*24����������ģ����
				ILI9806G_zoomChar(16,24,Charwidth,Font_Height,(uint8_t *)&Font16x32.table[Ascii * Font16x32.Height*Font16x32.Width/8],psr,0);
			  //��ʾ�����ַ�
				ILI9806G_DrawChar_Ex(x,y,Charwidth,Font_Height,(uint8_t*)&zoomBuff,DrawModel);
				y+=Font_Height;
				ptr++;
		}
	}
}

/**
  * @brief  ����Ӣ����������
  * @param  fonts: ָ��Ҫѡ�������
	*		����Ϊ����ֵ֮һ
  * 	@arg��Font24x32;
  * 	@arg��Font16x24;
  * 	@arg��Font8x16;
  * @retval None
  */
void LCD_SetFont(sFONT *fonts)
{
  LCD_Currentfonts = fonts;
}

/**
  * @brief  ��ȡ��ǰ��������
  * @param  None.
  * @retval ���ص�ǰ��������
  */
sFONT *LCD_GetFont(void)
{
  return LCD_Currentfonts;
}


/**
  * @brief  ����LCD��ǰ��(����)��������ɫ,RGB565
  * @param  TextColor: ָ��ǰ��(����)��ɫ
  * @param  BackColor: ָ��������ɫ
  * @retval None
  */
void LCD_SetColors(uint16_t TextColor, uint16_t BackColor) 
{
  CurrentTextColor = TextColor; 
  CurrentBackColor = BackColor;
}

/**
  * @brief  ��ȡLCD��ǰ��(����)��������ɫ,RGB565
  * @param  TextColor: �����洢ǰ��(����)��ɫ��ָ�����
  * @param  BackColor: �����洢������ɫ��ָ�����
  * @retval None
  */
void LCD_GetColors(uint16_t *TextColor, uint16_t *BackColor)
{
  *TextColor = CurrentTextColor;
  *BackColor = CurrentBackColor;
}

/**
  * @brief  ����LCD��ǰ��(����)��ɫ,RGB565
  * @param  Color: ָ��ǰ��(����)��ɫ 
  * @retval None
  */
void LCD_SetTextColor(uint16_t Color)
{
  CurrentTextColor = Color;
}

/**
  * @brief  ����LCD�ı�����ɫ,RGB565
  * @param  Color: ָ��������ɫ 
  * @retval None
  */
void LCD_SetBackColor(uint16_t Color)
{
  CurrentBackColor = Color;
}

/**
  * @brief  ���ĳ������
  * @param  Line: ָ��Ҫɾ������
  *   ��������ʹ�ú�LINE(0)��LINE(1)�ȷ�ʽָ��Ҫɾ�����У�
  *   ��LINE(x)����ݵ�ǰѡ�������������Y����ֵ����ɾ����ǰ����߶ȵĵ�x�С�
  * @retval None
  */
void ILI9806G_ClearLine(uint16_t Line)
{
  ILI9806G_Clear(0,Line,LCD_X_LENGTH,((sFONT *)LCD_GetFont())->Height);	/* ��������ʾȫ�� */

}
/*********************end of file*************************/



