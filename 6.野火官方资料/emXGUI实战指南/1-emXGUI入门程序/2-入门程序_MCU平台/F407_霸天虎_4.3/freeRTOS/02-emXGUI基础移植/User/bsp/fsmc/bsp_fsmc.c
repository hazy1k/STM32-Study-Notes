
#include "stm32f4xx.h"
#include "board.h"
/*=========================================================================================*/
void FSMC_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;


  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOG, ENABLE);//ʹ��PD,PE,PF,PGʱ��
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC,ENABLE);//ʹ��FSMCʱ��


  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;//�������
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;//����

  GPIO_InitStructure.GPIO_Pin = (0x3F<<0);//PF5:0
  GPIO_Init(GPIOF, &GPIO_InitStructure);//��ʼ��
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource0,GPIO_AF_FSMC); 	//PF0,A0
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource1,GPIO_AF_FSMC); 	//PF1,A1
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource2,GPIO_AF_FSMC); 	//PF2,A2
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource3,GPIO_AF_FSMC); 	//PF3,A3
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource4,GPIO_AF_FSMC); 	//PF4,A4
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource5,GPIO_AF_FSMC); 	//PF5,A5

  GPIO_InitStructure.GPIO_Pin = (0x0F<<12);//PF15:12
  GPIO_Init(GPIOF, &GPIO_InitStructure);//��ʼ��
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource12,GPIO_AF_FSMC); 	//PF12,A6
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource13,GPIO_AF_FSMC); 	//PF13,A7
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource14,GPIO_AF_FSMC); 	//PF14,A8
  GPIO_PinAFConfig(GPIOF,GPIO_PinSource15,GPIO_AF_FSMC); 	//PF15,A9

  GPIO_InitStructure.GPIO_Pin = (0x3F<<0);//PG5:0
  GPIO_Init(GPIOG, &GPIO_InitStructure);//��ʼ��
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource0,GPIO_AF_FSMC); 	//PG0,A10
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource1,GPIO_AF_FSMC); 	//PG1,A11
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource2,GPIO_AF_FSMC); 	//PG2,A12
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource3,GPIO_AF_FSMC); 	//PG3,A13
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource4,GPIO_AF_FSMC); 	//PG4,A14
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource5,GPIO_AF_FSMC); 	//PG5,A15

  GPIO_InitStructure.GPIO_Pin = (0x07<<11);//PD13:11
  GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource11,GPIO_AF_FSMC); 	//PD11,A16
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource12,GPIO_AF_FSMC); 	//PD12,A17
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource13,GPIO_AF_FSMC); 	//PD13,A18

  GPIO_InitStructure.GPIO_Pin = (0x0F<<3);//PE6:3
  GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource3,GPIO_AF_FSMC); 	//PE3,A19
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource4,GPIO_AF_FSMC); 	//PE4,A20
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource5,GPIO_AF_FSMC); 	//PE5,A21
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource6,GPIO_AF_FSMC); 	//PE6,A22

  GPIO_InitStructure.GPIO_Pin = (1<<12)|(1<<10);//PG12,PG10
  GPIO_Init(GPIOG, &GPIO_InitStructure);//��ʼ��
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource10,GPIO_AF_FSMC); 	//PG10,NE3
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource12,GPIO_AF_FSMC); 	//PG12,NE4

  GPIO_InitStructure.GPIO_Pin = (0x03<<14);//PD15:14
  GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource14,GPIO_AF_FSMC);	//PD14,D0
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_FSMC);	//PD15,D1

  GPIO_InitStructure.GPIO_Pin = (0x03<<0);//PD1:0
  GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_FSMC);		//PD0,D2
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_FSMC);		//PD1,D3

  GPIO_InitStructure.GPIO_Pin = (0x1FF<<7);//PE15:7
  GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource7,GPIO_AF_FSMC);		//PE7,D4
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource8,GPIO_AF_FSMC);		//PE8,D5
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource9,GPIO_AF_FSMC);		//PE9,D6
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource10,GPIO_AF_FSMC);	//PE10,D7
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_FSMC);	//PE11,D8
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource12,GPIO_AF_FSMC);	//PE12,D9
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF_FSMC);	//PE13,D10
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_FSMC);	//PE14,D11
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource15,GPIO_AF_FSMC);	//PE15,D12

  GPIO_InitStructure.GPIO_Pin = (0x07<<8);//PD10:8
  GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_FSMC);		//PD8,D13
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_FSMC);		//PD9,D14
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource10,GPIO_AF_FSMC);	//PD10,D15

  GPIO_InitStructure.GPIO_Pin = (0x03<<0);//PE1:0
  GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource0,GPIO_AF_FSMC); 	//PE0,NBL0
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource1,GPIO_AF_FSMC); 	//PE1,NBL1

  GPIO_InitStructure.GPIO_Pin = (0x03<<4);//PD5:4
  GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource4,GPIO_AF_FSMC);		//PD4,NOE
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_FSMC);		//PD5,NWE

}


void FSMC_InitSRAM(void)
{

	  //Bank1_NE3 is configured as follow:

	  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	  FSMC_NORSRAMTimingInitTypeDef rd_timing;
	  FSMC_NORSRAMTimingInitTypeDef wt_timing;


	  //FSMC��ʱ����ƼĴ���
	  rd_timing.FSMC_AddressSetupTime = 2; //��ַ����ʱ��
	  rd_timing.FSMC_AddressHoldTime = 0;  //��ַ����ʱ��ģʽAδ�õ�
	  rd_timing.FSMC_DataSetupTime = 6;    //���ݱ���ʱ��
	  rd_timing.FSMC_BusTurnAroundDuration = 0x00;
	  rd_timing.FSMC_CLKDivision = 0;
	  rd_timing.FSMC_DataLatency = 0;
	  rd_timing.FSMC_AccessMode = FSMC_AccessMode_A; //ģʽA

	  //FSMCдʱ����ƼĴ���
	  wt_timing.FSMC_AddressSetupTime = 2;  //��ַ����ʱ��
	  wt_timing.FSMC_AddressHoldTime = 0;   //��ַ����ʱ����ģʽAδ�õ�
	  wt_timing.FSMC_DataSetupTime = 3;	  //���ݱ���ʱ��
	  wt_timing.FSMC_BusTurnAroundDuration = 0x00;
	  wt_timing.FSMC_CLKDivision = 0;
	  wt_timing.FSMC_DataLatency = 0;
	  wt_timing.FSMC_AccessMode = FSMC_AccessMode_A; //ģʽA

	  FSMC_NORSRAMInitStructure.FSMC_Bank = EXT_SRAM_FSMC_BANK;
	  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &rd_timing;
	  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &wt_timing;

	  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //��ʼ��FSMC����

	  FSMC_NORSRAMCmd(EXT_SRAM_FSMC_BANK, ENABLE);  // ʹ��BANK1����4

}

void FSMC_InitLCD(void)
{

	GPIO_InitTypeDef gpio_init;

	FSMC_NORSRAMInitTypeDef fsmc_init;
	FSMC_NORSRAMTimingInitTypeDef rd_timing;
	FSMC_NORSRAMTimingInitTypeDef wt_timing;

	gpio_init.GPIO_Mode  = GPIO_Mode_AF;      // �������
	gpio_init.GPIO_OType = GPIO_OType_PP;     // �������
	gpio_init.GPIO_Speed = GPIO_Speed_100MHz; // 100MHz
	gpio_init.GPIO_PuPd  = GPIO_PuPd_UP;      // ����
	gpio_init.GPIO_Pin = GPIO_Pin_12;         // PG12
	GPIO_Init(GPIOG, &gpio_init);             // ��ʼ��
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource12,GPIO_AF_FSMC);

	//FSMC��ʱ����ƼĴ���
	rd_timing.FSMC_AddressSetupTime = 3; //��ַ����ʱ�� 16 �� HCLK =16X(1/168M)=96ns
	rd_timing.FSMC_AddressHoldTime = 0;  //��ַ����ʱ��ģʽAδ�õ�
	rd_timing.FSMC_DataSetupTime = 20;    //���ݱ���ʱ��Ϊ 25 �� HCLK 25x(1/168M)=150ns
	rd_timing.FSMC_BusTurnAroundDuration = 0x00;
	rd_timing.FSMC_CLKDivision = 0;
	rd_timing.FSMC_DataLatency = 0;
	rd_timing.FSMC_AccessMode = FSMC_AccessMode_A; //ģʽA

	//FSMCдʱ����ƼĴ���
	wt_timing.FSMC_AddressSetupTime = 2;  //��ַ����ʱ��Ϊ 8 �� HCLK 8x(1/168M)=48ns
	wt_timing.FSMC_AddressHoldTime = 0;   //��ַ����ʱ����ģʽAδ�õ�
	wt_timing.FSMC_DataSetupTime = 2;	    //ӦΪĳЩҺ������оƬ��ԭ��,�������ݱ���ʱ������Ϊ 9 ��HCLK Ϊ9x6=54ns
	wt_timing.FSMC_BusTurnAroundDuration = 0x00;
	wt_timing.FSMC_CLKDivision = 0;
	wt_timing.FSMC_DataLatency = 0;
	wt_timing.FSMC_AccessMode = FSMC_AccessMode_A;                           // ģʽA

	fsmc_init.FSMC_Bank = EXT_LCD_FSMC_BANK;                                 // NOR/SRAM��Bank4
	fsmc_init.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;             // ������������
	fsmc_init.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	fsmc_init.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;               // 16λ���ݿ��
	fsmc_init.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;           // �Ƿ�ʹ��ͻ������,����ͬ��ͻ���洢����Ч,�˴�δ�õ�
	fsmc_init.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;         // �Ƿ�ʹ��ͬ������ģʽ�µĵȴ��ź�,�˴�δ�õ�
	fsmc_init.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;         // �ȴ��źŵļ���,����ͻ��ģʽ����������
	fsmc_init.FSMC_WrapMode = FSMC_WrapMode_Disable;                         // �Ƿ�ʹ�ܻ�·ͻ��ģʽ,�˴�δ�õ�
	fsmc_init.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState; // �洢�����ڵȴ�����֮ǰ��һ��ʱ�����ڻ��ǵȴ������ڼ�ʹ��NWAIT
	fsmc_init.FSMC_WriteOperation = FSMC_WriteOperation_Enable;              // �洢��дʹ��
	fsmc_init.FSMC_WaitSignal = FSMC_WaitSignal_Disable;                     // �ȴ�ʹ��λ,�˴�δ�õ�
	fsmc_init.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;                  // ��дʹ�ò�ͬ��ʱ��
	fsmc_init.FSMC_WriteBurst = FSMC_WriteBurst_Disable;                     // �첽�����ڼ�ĵȴ��ź�
	fsmc_init.FSMC_ReadWriteTimingStruct = &rd_timing;
	fsmc_init.FSMC_WriteTimingStruct = &wt_timing;                           // дʱ��

	FSMC_NORSRAMInit(&fsmc_init);               // FSMC��ʼ��

	FSMC_NORSRAMCmd(EXT_LCD_FSMC_BANK,ENABLE);  // ʹ�� NOR/SRAM ����

}






