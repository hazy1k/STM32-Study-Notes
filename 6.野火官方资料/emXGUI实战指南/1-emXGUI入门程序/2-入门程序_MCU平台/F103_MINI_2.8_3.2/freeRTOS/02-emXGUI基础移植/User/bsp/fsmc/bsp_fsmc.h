#ifndef		__BSP_FSMC_H___
#define		__BSP_FSMC_H___

//��Ƭѡ���ž�����NOR/SRAM��
#define      FSMC_Bank1_NORSRAMx           FSMC_Bank1_NORSRAM1

/******************************* ILI9341 ��ʾ��8080ͨѶ���Ŷ��� ***************************/
/******�����ź���******/
//Ƭѡ��ѡ��NOR/SRAM��
#define      ILI9341_CS_CLK                RCC_APB2Periph_GPIOC   
#define      ILI9341_CS_PORT               GPIOC
#define      ILI9341_CS_PIN                GPIO_Pin_4

//DC���ţ�ʹ��FSMC�ĵ�ַ�źſ��ƣ������ž����˷���LCDʱʹ�õĵ�ַ
//PD11ΪFSMC_A16
#define      ILI9341_DC_CLK                RCC_APB2Periph_GPIOC   
#define      ILI9341_DC_PORT               GPIOC
#define      ILI9341_DC_PIN                GPIO_Pin_7

//дʹ��
#define      ILI9341_WR_CLK                RCC_APB2Periph_GPIOC   
#define      ILI9341_WR_PORT               GPIOC
#define      ILI9341_WR_PIN                GPIO_Pin_6

//��ʹ��
#define      ILI9341_RD_CLK                RCC_APB2Periph_GPIOC   
#define      ILI9341_RD_PORT               GPIOC
#define      ILI9341_RD_PIN                GPIO_Pin_5

//��λ����
//#define      ILI9341_RST_CLK               RCC_APB2Periph_GPIOE
//#define      ILI9341_RST_PORT              GPIOE
//#define      ILI9341_RST_PIN               GPIO_Pin_1

//��������
#define      ILI9341_BK_CLK                RCC_APB2Periph_GPIOD    
#define      ILI9341_BK_PORT               GPIOD
#define      ILI9341_BK_PIN                GPIO_Pin_2

/********�����ź���***************/
#define      ILI9341_D0_CLK                RCC_APB2Periph_GPIOB  
#define      ILI9341_D0_PORT               GPIOB
#define      ILI9341_D0_PIN                GPIO_Pin_0

#define      ILI9341_D1_CLK                RCC_APB2Periph_GPIOB   
#define      ILI9341_D1_PORT               GPIOB
#define      ILI9341_D1_PIN                GPIO_Pin_1

#define      ILI9341_D2_CLK                RCC_APB2Periph_GPIOB   
#define      ILI9341_D2_PORT               GPIOB
#define      ILI9341_D2_PIN                GPIO_Pin_2

#define      ILI9341_D3_CLK                RCC_APB2Periph_GPIOB  
#define      ILI9341_D3_PORT               GPIOB
#define      ILI9341_D3_PIN                GPIO_Pin_3

#define      ILI9341_D4_CLK                RCC_APB2Periph_GPIOB   
#define      ILI9341_D4_PORT               GPIOB
#define      ILI9341_D4_PIN                GPIO_Pin_4

#define      ILI9341_D5_CLK                RCC_APB2Periph_GPIOB   
#define      ILI9341_D5_PORT               GPIOB
#define      ILI9341_D5_PIN                GPIO_Pin_5

#define      ILI9341_D6_CLK                RCC_APB2Periph_GPIOB   
#define      ILI9341_D6_PORT               GPIOB
#define      ILI9341_D6_PIN                GPIO_Pin_6

#define      ILI9341_D7_CLK                RCC_APB2Periph_GPIOB  
#define      ILI9341_D7_PORT               GPIOB
#define      ILI9341_D7_PIN                GPIO_Pin_7

#define      ILI9341_D8_CLK                RCC_APB2Periph_GPIOB   
#define      ILI9341_D8_PORT               GPIOB
#define      ILI9341_D8_PIN                GPIO_Pin_8

#define      ILI9341_D9_CLK                RCC_APB2Periph_GPIOB   
#define      ILI9341_D9_PORT               GPIOB
#define      ILI9341_D9_PIN                GPIO_Pin_9

#define      ILI9341_D10_CLK                RCC_APB2Periph_GPIOB   
#define      ILI9341_D10_PORT               GPIOB
#define      ILI9341_D10_PIN                GPIO_Pin_10

#define      ILI9341_D11_CLK                RCC_APB2Periph_GPIOB   
#define      ILI9341_D11_PORT               GPIOB
#define      ILI9341_D11_PIN                GPIO_Pin_11

#define      ILI9341_D12_CLK                RCC_APB2Periph_GPIOB   
#define      ILI9341_D12_PORT               GPIOB
#define      ILI9341_D12_PIN                GPIO_Pin_12

#define      ILI9341_D13_CLK                RCC_APB2Periph_GPIOB   
#define      ILI9341_D13_PORT               GPIOB
#define      ILI9341_D13_PIN                GPIO_Pin_13

#define      ILI9341_D14_CLK                RCC_APB2Periph_GPIOB   
#define      ILI9341_D14_PORT               GPIOB
#define      ILI9341_D14_PIN                GPIO_Pin_14

#define      ILI9341_D15_CLK                RCC_APB2Periph_GPIOB   
#define      ILI9341_D15_PORT               GPIOB
#define      ILI9341_D15_PIN                GPIO_Pin_15

#endif
