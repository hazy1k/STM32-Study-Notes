
#include "stm32f10x.h"
#include "board.h"
#include "bsp_fsmc.h"
/*=========================================================================================*/


void FSMC_Init(void)
{
#if 0
	  GPIO_InitTypeDef GPIO_InitStructure;

	  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOG | RCC_APB2Periph_GPIOE |
	                         RCC_APB2Periph_GPIOF, ENABLE);

	/*-- GPIO Configuration ------------------------------------------------------*/
	  /* SRAM Data lines configuration */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 |
	                                GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15 | GPIO_Pin_7;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_Init(GPIOD, &GPIO_InitStructure);

	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
	                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |
	                                GPIO_Pin_15 ;
	  GPIO_Init(GPIOE, &GPIO_InitStructure);

	  /* SRAM Address lines configuration */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
	                                GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_12 | GPIO_Pin_13 |
	                                GPIO_Pin_14 | GPIO_Pin_15;
	  GPIO_Init(GPIOF, &GPIO_InitStructure);

	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
	                                GPIO_Pin_4 | GPIO_Pin_5;
	  GPIO_Init(GPIOG, &GPIO_InitStructure);

	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
	  GPIO_Init(GPIOD, &GPIO_InitStructure);

	  /* NOE and NWE configuration */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 |GPIO_Pin_5;
	  GPIO_Init(GPIOD, &GPIO_InitStructure);

	  /* NE3 configuration */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	  GPIO_Init(GPIOG, &GPIO_InitStructure);

	  /* NE4 configuration */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	  GPIO_Init(GPIOG, &GPIO_InitStructure);

	  /* NBL0, NBL1 configuration */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	  GPIO_Init(GPIOE, &GPIO_InitStructure);
#endif
	GPIO_InitTypeDef GPIO_InitStructure;

	/* ʹ��FSMC��Ӧ��Ӧ�ܽ�ʱ��*/
	RCC_APB2PeriphClockCmd ( 	
													/*�����ź�*/
													ILI9341_CS_CLK|ILI9341_DC_CLK|ILI9341_WR_CLK|
													ILI9341_RD_CLK	|ILI9341_BK_CLK|ILI9341_RST_CLK|
													/*�����ź�*/
													ILI9341_D0_CLK|ILI9341_D1_CLK|	ILI9341_D2_CLK | 
													ILI9341_D3_CLK | ILI9341_D4_CLK|ILI9341_D5_CLK|
													ILI9341_D6_CLK | ILI9341_D7_CLK|ILI9341_D8_CLK|
													ILI9341_D9_CLK | ILI9341_D10_CLK|ILI9341_D11_CLK|
													ILI9341_D12_CLK | ILI9341_D13_CLK|ILI9341_D14_CLK|
													ILI9341_D15_CLK	, ENABLE );
		
	
	/* ����FSMC���Ӧ��������,FSMC-D0~D15 */	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D0_PIN;
	GPIO_Init ( ILI9341_D0_PORT, & GPIO_InitStructure );

	GPIO_InitStructure.GPIO_Pin = ILI9341_D1_PIN;
	GPIO_Init ( ILI9341_D1_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D2_PIN;
	GPIO_Init ( ILI9341_D2_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D3_PIN;
	GPIO_Init ( ILI9341_D3_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D4_PIN;
	GPIO_Init ( ILI9341_D4_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D5_PIN;
	GPIO_Init ( ILI9341_D5_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D6_PIN;
	GPIO_Init ( ILI9341_D6_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D7_PIN;
	GPIO_Init ( ILI9341_D7_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D8_PIN;
	GPIO_Init ( ILI9341_D8_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D9_PIN;
	GPIO_Init ( ILI9341_D9_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D10_PIN;
	GPIO_Init ( ILI9341_D10_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D11_PIN;
	GPIO_Init ( ILI9341_D11_PORT, & GPIO_InitStructure );

	GPIO_InitStructure.GPIO_Pin = ILI9341_D12_PIN;
	GPIO_Init ( ILI9341_D12_PORT, & GPIO_InitStructure );	
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D13_PIN;
	GPIO_Init ( ILI9341_D13_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D14_PIN;
	GPIO_Init ( ILI9341_D14_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D15_PIN;
	GPIO_Init ( ILI9341_D15_PORT, & GPIO_InitStructure );
	

	
	/* ����FSMC���Ӧ�Ŀ�����
	 * FSMC_NOE   :LCD-RD
	 * FSMC_NWE   :LCD-WR
	 * FSMC_NE1   :LCD-CS
	 * FSMC_A16  	:LCD-DC
	 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_RD_PIN; 
	GPIO_Init (ILI9341_RD_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_WR_PIN; 
	GPIO_Init (ILI9341_WR_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_CS_PIN; 
	GPIO_Init ( ILI9341_CS_PORT, & GPIO_InitStructure );  
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_DC_PIN; 
	GPIO_Init ( ILI9341_DC_PORT, & GPIO_InitStructure );
	

  /* ����LCD��λRST���ƹܽ�*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_RST_PIN; 
	GPIO_Init ( ILI9341_RST_PORT, & GPIO_InitStructure );
	
	
	/* ����LCD������ƹܽ�BK*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_BK_PIN; 
	GPIO_Init ( ILI9341_BK_PORT, & GPIO_InitStructure );
}

void FSMC_InitSRAM(void)
{
//	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
//	FSMC_NORSRAMTimingInitTypeDef  p;



//	 /*-- FSMC Configuration ------------------------------------------------------*/
//	  p.FSMC_AddressSetupTime = 1;
//	  p.FSMC_AddressHoldTime = 0;
//	  p.FSMC_DataSetupTime = 1;
//	  p.FSMC_BusTurnAroundDuration = 0;
//	  p.FSMC_CLKDivision = 0;
//	  p.FSMC_DataLatency = 0;
//	  p.FSMC_AccessMode = FSMC_AccessMode_A;


//	  FSMC_NORSRAMInitStructure.FSMC_Bank = EXT_SRAM_FSMC_BANK;
//	  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
//	  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
//	  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
//	  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
//	  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;

//	  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
//	  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
//	  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
//	  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
//	  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
//	  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
//	  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
//	  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
//	  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct     = &p;

//	  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

//	  /* Enable FSMC Bank1_SRAM Bank */
//	  FSMC_NORSRAMCmd(EXT_SRAM_FSMC_BANK, ENABLE);
}

void FSMC_InitLCD(void)
{
#if 0
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  wr_t,rd_t;

	GPIO_InitTypeDef GPIO_InitStructure;

	//PD12 -> FSMC_A23 - LCD_RS
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);


	wr_t.FSMC_AddressSetupTime      = 0x01;	 //��ַ����ʱ��
	wr_t.FSMC_AddressHoldTime       = 0x00;	 //��ַ����ʱ��
	wr_t.FSMC_DataSetupTime         = 0x01;	 //���ݽ���ʱ��
	wr_t.FSMC_BusTurnAroundDuration = 0x00;
	wr_t.FSMC_CLKDivision           = 0x00;
	wr_t.FSMC_DataLatency           = 0x00;
	wr_t.FSMC_AccessMode            = FSMC_AccessMode_B;

	rd_t.FSMC_AddressSetupTime      = 0x01;	 //��ַ����ʱ��
	rd_t.FSMC_AddressHoldTime       = 0x00;	 //��ַ����ʱ��
	rd_t.FSMC_DataSetupTime         = 0x02;	 //���ݽ���ʱ��
	rd_t.FSMC_BusTurnAroundDuration = 0x00;
	rd_t.FSMC_CLKDivision           = 0x00;
	rd_t.FSMC_DataLatency           = 0x00;
	rd_t.FSMC_AccessMode            = FSMC_AccessMode_B;

	FSMC_NORSRAMInitStructure.FSMC_Bank                  = EXT_LCD_FSMC_BANK;
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux        = FSMC_DataAddressMux_Disable;
	//FSMC_NORSRAMInitStructure.FSMC_MemoryType          = FSMC_MemoryType_SRAM;
	FSMC_NORSRAMInitStructure.FSMC_MemoryType            = FSMC_MemoryType_NOR;
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth       = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode       = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait 	 = FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity    = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode              = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive      = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation        = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal            = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode          = FSMC_ExtendedMode_Enable; // ��дʹ�ò�ͬ��ʱ��
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst            = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &rd_t;
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct     = &wr_t;

	FSMC_NORSRAMInit ( & FSMC_NORSRAMInitStructure );


	/* ʹ�� FSMC_Bank1_NORSRAM4 */
	FSMC_NORSRAMCmd ( EXT_LCD_FSMC_BANK, ENABLE );
#endif
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  wr_t,rd_t; 	
	
	/* ʹ��FSMCʱ��*/
	RCC_AHBPeriphClockCmd ( RCC_AHBPeriph_FSMC, ENABLE );

//	//��ַ����ʱ�䣨ADDSET��Ϊ1��HCLK 2/72M=28ns
//	wr_t.FSMC_AddressSetupTime      = 0x01;	 //��ַ����ʱ��
//	//���ݱ���ʱ�䣨DATAST��+ 1��HCLK = 5/72M=70ns	
//	wr_t.FSMC_DataSetupTime         = 0x01;	 //���ݽ���ʱ��
//	//ѡ����Ƶ�ģʽ
//	//ģʽB,�첽NOR FLASHģʽ����ILI9341��8080ʱ��ƥ��
//	wr_t.FSMC_AccessMode            = FSMC_AccessMode_B;	
//	
//	/*����������ģʽB�޹�*/
//	//��ַ����ʱ�䣨ADDHLD��ģʽAδ�õ�
//	wr_t.FSMC_AddressHoldTime       = 0x00;	 //��ַ����ʱ��
//	//��������ת�����ڣ������ڸ���ģʽ��NOR����
//	wr_t.FSMC_BusTurnAroundDuration = 0x00;
//	//����ʱ�ӷ�Ƶ��������ͬ�����͵Ĵ洢��
//	wr_t.FSMC_CLKDivision           = 0x00;
//	//���ݱ���ʱ�䣬������ͬ���͵�NOR	
//	wr_t.FSMC_DataLatency           = 0x00;	
	
	wr_t.FSMC_AddressSetupTime      = 0x01;	 //��ַ����ʱ��
	wr_t.FSMC_AddressHoldTime       = 0x00;	 //��ַ����ʱ��
	wr_t.FSMC_DataSetupTime         = 0x01;	 //���ݽ���ʱ��
	wr_t.FSMC_BusTurnAroundDuration = 0x00;
	wr_t.FSMC_CLKDivision           = 0x00;
	wr_t.FSMC_DataLatency           = 0x00;
	wr_t.FSMC_AccessMode            = FSMC_AccessMode_B;

	rd_t.FSMC_AddressSetupTime      = 0x01;	 //��ַ����ʱ��
	rd_t.FSMC_AddressHoldTime       = 0x00;	 //��ַ����ʱ��
	rd_t.FSMC_DataSetupTime         = 0x02;	 //���ݽ���ʱ��
	rd_t.FSMC_BusTurnAroundDuration = 0x00;
	rd_t.FSMC_CLKDivision           = 0x00;
	rd_t.FSMC_DataLatency           = 0x00;
	rd_t.FSMC_AccessMode            = FSMC_AccessMode_B;
	
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
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &rd_t;
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct     = &wr_t;  
	
	FSMC_NORSRAMInit ( & FSMC_NORSRAMInitStructure ); 
	
	/* ʹ�� FSMC_Bank1_NORSRAM4 */
	FSMC_NORSRAMCmd ( FSMC_Bank1_NORSRAMx, ENABLE );  
		
}

/*=========================================================================================*/
/*=========================================================================================*/









