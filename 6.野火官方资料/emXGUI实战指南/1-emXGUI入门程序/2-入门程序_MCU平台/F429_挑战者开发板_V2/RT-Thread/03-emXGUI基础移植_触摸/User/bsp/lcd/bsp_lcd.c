/**
  ******************************************************************************
  * @file    bsp_lcd.c
  * @author  fire
  * @version V1.1
  * @date    2015-xx-xx
  * @brief   LCD���������ӿ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F429 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  

/* Includes ------------------------------------------------------------------*/
#include "bsp_lcd.h"

/* ��ͬҺ�����Ĳ��� */
const LCD_PARAM_TypeDef lcd_param[LCD_TYPE_NUM]={

  /* 5�������� */
  {
    /*����Һ�������ֲ�Ĳ�������*/
    .hbp = 0,   //HSYNC�����Ч����
    .vbp = 45,  //VSYNC�����Ч����

    .hsw = 128,   //HSYNC���
    .vsw = 128,   //VSYNC���

    .hfp = 88,  	//HSYNCǰ����Ч����
    .vfp = 13,  	//VSYNCǰ����Ч����
    
    .comment_clock_2byte = 33, //rgb565/argb4444��˫�ֽ�����ʱ�Ƽ�ʹ�õ�Һ��ʱ��Ƶ��
    .comment_clock_4byte = 21, //Argb8888�����ֽ�����ʱ�Ƽ�ʹ�õ�Һ��ʱ��Ƶ��

    
    .lcd_pixel_width = LCD_MAX_PIXEL_WIDTH,//Һ���ֱ��ʣ���
    .lcd_pixel_height = LCD_MAX_PIXEL_HEIGHT,//Һ���ֱ��ʣ���
  },
  
   /* 7������������5��һ���� */
  {
    /*����Һ�������ֲ�Ĳ�������*/
    .hbp = 46,  //HSYNC�����Ч����
    .vbp = 23,  //VSYNC�����Ч����

    .hsw = 1,  	//HSYNC���
    .vsw = 1,   //VSYNC���

    .hfp = 22,  	//HSYNCǰ����Ч����
    .vfp = 22,  	//VSYNCǰ����Ч����
    
    .comment_clock_2byte = 33, //rgb565/argb4444��˫�ֽ�����ʱ�Ƽ�ʹ�õ�Һ��ʱ��Ƶ��
    .comment_clock_4byte = 21, //Argb8888�����ֽ�����ʱ�Ƽ�ʹ�õ�Һ��ʱ��Ƶ��

    
    .lcd_pixel_width = LCD_MAX_PIXEL_WIDTH,//Һ���ֱ��ʣ���
    .lcd_pixel_height = LCD_MAX_PIXEL_HEIGHT,//Һ���ֱ��ʣ���
  },

  /* 4.3�������� */
  {
      /*����Һ�������ֲ�Ĳ�������*/
    .hbp = 8,  //HSYNC�����Ч����
    .vbp = 2,  //VSYNC�����Ч����

    .hsw = 41,  	//HSYNC���
    .vsw = 10,   //VSYNC���

    .hfp = 4,  	//HSYNCǰ����Ч����
    .vfp = 4,  	//VSYNCǰ����Ч����
    
    .comment_clock_2byte = 15, //rgb565/argb4444��˫�ֽ�����ʱ�Ƽ�ʹ�õ�Һ��ʱ��Ƶ��
    .comment_clock_4byte = 15, //Argb8888�����ֽ�����ʱ�Ƽ�ʹ�õ�Һ��ʱ��Ƶ��
    
    .lcd_pixel_width = 480,//Һ���ֱ��ʣ���
    .lcd_pixel_height = 272,//Һ���ֱ��ʣ���
  }
};

/* ��ǰʹ�õ�LCD��Ĭ��Ϊ5/7����
  * �ڴ���������ʼ��ʱ�ɸ��ݴ���оƬ���ͺ����ֲ�ͬ��LCD
*/
LCD_TypeDef cur_lcd = INCH_5;

/* ÿ�����ص�ռ���ٸ��ֽ�
ARGB8888/RGB888/RGB565/ARGB1555/ARGB4444/L8/AL44/AL88 
ʹ�ú�LTDC_Pixelformat_ARGB8888��Ϊ��������
*/
const uint8_t PIXEL_BPP[]={4,3,2,2,2,1,1,2};  

/**
  * @brief  ��ʼ��LCD��GPIO��RGB888�����⡢DISP
  * @param  ��
  * @retval ��
  */
static void LCD_GPIO_Config(void)
{
 GPIO_InitTypeDef GPIO_InitStruct;

 /* ʹ��LCDʹ�õ�������ʱ�� */
                         //��ɫ������
 RCC_AHB1PeriphClockCmd(LTDC_R0_GPIO_CLK | LTDC_R1_GPIO_CLK | LTDC_R2_GPIO_CLK|
                        LTDC_R3_GPIO_CLK | LTDC_R4_GPIO_CLK | LTDC_R5_GPIO_CLK|
                        LTDC_R6_GPIO_CLK | LTDC_R7_GPIO_CLK |
                         //��ɫ������
                         LTDC_G0_GPIO_CLK|LTDC_G1_GPIO_CLK|LTDC_G2_GPIO_CLK|
                         LTDC_G3_GPIO_CLK|LTDC_G4_GPIO_CLK|LTDC_G5_GPIO_CLK|
                         LTDC_G6_GPIO_CLK|LTDC_G7_GPIO_CLK|
                         //��ɫ������
                         LTDC_B0_GPIO_CLK|LTDC_B1_GPIO_CLK|LTDC_B2_GPIO_CLK|
                         LTDC_B3_GPIO_CLK|LTDC_B4_GPIO_CLK|LTDC_B5_GPIO_CLK|
                         LTDC_B6_GPIO_CLK|LTDC_B7_GPIO_CLK|
                         //�����ź���
                         LTDC_CLK_GPIO_CLK | LTDC_HSYNC_GPIO_CLK |LTDC_VSYNC_GPIO_CLK|
                         LTDC_DE_GPIO_CLK  | LTDC_BL_GPIO_CLK    |LTDC_DISP_GPIO_CLK ,ENABLE);


/* GPIO���� */

/* ��ɫ������ */
 GPIO_InitStruct.GPIO_Pin = LTDC_R0_GPIO_PIN;
 GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
 GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

 GPIO_Init(LTDC_R0_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_R0_GPIO_PORT, LTDC_R0_PINSOURCE, LTDC_R0_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_R1_GPIO_PIN;
 GPIO_Init(LTDC_R1_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_R1_GPIO_PORT, LTDC_R1_PINSOURCE, LTDC_R1_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_R2_GPIO_PIN;
 GPIO_Init(LTDC_R2_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_R2_GPIO_PORT, LTDC_R2_PINSOURCE, LTDC_R2_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_R3_GPIO_PIN;
 GPIO_Init(LTDC_R3_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_R3_GPIO_PORT, LTDC_R3_PINSOURCE, LTDC_R3_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_R4_GPIO_PIN;
 GPIO_Init(LTDC_R4_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_R4_GPIO_PORT, LTDC_R4_PINSOURCE, LTDC_R4_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_R5_GPIO_PIN;
 GPIO_Init(LTDC_R5_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_R5_GPIO_PORT, LTDC_R5_PINSOURCE, LTDC_R5_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_R6_GPIO_PIN;
 GPIO_Init(LTDC_R6_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_R6_GPIO_PORT, LTDC_R6_PINSOURCE, LTDC_R6_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_R7_GPIO_PIN;
 GPIO_Init(LTDC_R7_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_R7_GPIO_PORT, LTDC_R7_PINSOURCE, LTDC_R7_AF);

 //��ɫ������
 GPIO_InitStruct.GPIO_Pin = LTDC_G0_GPIO_PIN;
 GPIO_Init(LTDC_G0_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_G0_GPIO_PORT, LTDC_G0_PINSOURCE, LTDC_G0_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_G1_GPIO_PIN;
 GPIO_Init(LTDC_G1_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_G1_GPIO_PORT, LTDC_G1_PINSOURCE, LTDC_G1_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_G2_GPIO_PIN;
 GPIO_Init(LTDC_G2_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_G2_GPIO_PORT, LTDC_G2_PINSOURCE, LTDC_G2_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_G3_GPIO_PIN;
 GPIO_Init(LTDC_G3_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_G3_GPIO_PORT, LTDC_G3_PINSOURCE, LTDC_G3_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_G4_GPIO_PIN;
 GPIO_Init(LTDC_G4_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_G4_GPIO_PORT, LTDC_G4_PINSOURCE, LTDC_G4_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_G5_GPIO_PIN;
 GPIO_Init(LTDC_G5_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_G5_GPIO_PORT, LTDC_G5_PINSOURCE, LTDC_G5_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_G6_GPIO_PIN;
 GPIO_Init(LTDC_G6_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_G6_GPIO_PORT, LTDC_G6_PINSOURCE, LTDC_G6_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_G7_GPIO_PIN;
 GPIO_Init(LTDC_G7_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_G7_GPIO_PORT, LTDC_G7_PINSOURCE, LTDC_G7_AF);

 //��ɫ������
 GPIO_InitStruct.GPIO_Pin = LTDC_B0_GPIO_PIN;
 GPIO_Init(LTDC_B0_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_B0_GPIO_PORT, LTDC_B0_PINSOURCE, LTDC_B0_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_B1_GPIO_PIN;
 GPIO_Init(LTDC_B1_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_B1_GPIO_PORT, LTDC_B1_PINSOURCE, LTDC_B1_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_B2_GPIO_PIN;
 GPIO_Init(LTDC_B2_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_B2_GPIO_PORT, LTDC_B2_PINSOURCE, LTDC_B2_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_B3_GPIO_PIN;
 GPIO_Init(LTDC_B3_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_B3_GPIO_PORT, LTDC_B3_PINSOURCE, LTDC_B3_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_B4_GPIO_PIN;
 GPIO_Init(LTDC_B4_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_B4_GPIO_PORT, LTDC_B4_PINSOURCE, LTDC_B4_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_B5_GPIO_PIN;
 GPIO_Init(LTDC_B5_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_B5_GPIO_PORT, LTDC_B5_PINSOURCE, LTDC_B5_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_B6_GPIO_PIN;
 GPIO_Init(LTDC_B6_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_B6_GPIO_PORT, LTDC_B6_PINSOURCE, LTDC_B6_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_B7_GPIO_PIN;
 GPIO_Init(LTDC_B7_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_B7_GPIO_PORT, LTDC_B7_PINSOURCE, LTDC_B7_AF);

 //�����ź���
 GPIO_InitStruct.GPIO_Pin = LTDC_CLK_GPIO_PIN;
 GPIO_Init(LTDC_CLK_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_CLK_GPIO_PORT, LTDC_CLK_PINSOURCE, LTDC_CLK_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_HSYNC_GPIO_PIN;
 GPIO_Init(LTDC_HSYNC_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_HSYNC_GPIO_PORT, LTDC_HSYNC_PINSOURCE, LTDC_HSYNC_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_VSYNC_GPIO_PIN;
 GPIO_Init(LTDC_VSYNC_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_VSYNC_GPIO_PORT, LTDC_VSYNC_PINSOURCE, LTDC_VSYNC_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_DE_GPIO_PIN;
 GPIO_Init(LTDC_DE_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_DE_GPIO_PORT, LTDC_DE_PINSOURCE, LTDC_DE_AF);

 //BL DISP
 GPIO_InitStruct.GPIO_Pin = LTDC_DISP_GPIO_PIN;
 GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
 GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;

 GPIO_Init(LTDC_DISP_GPIO_PORT, &GPIO_InitStruct);


 GPIO_InitStruct.GPIO_Pin = LTDC_BL_GPIO_PIN;
 GPIO_Init(LTDC_BL_GPIO_PORT, &GPIO_InitStruct);

 //����ʹ��lcd
 GPIO_SetBits(LTDC_DISP_GPIO_PORT,LTDC_DISP_GPIO_PIN);
 GPIO_SetBits(LTDC_BL_GPIO_PORT,LTDC_BL_GPIO_PIN);
}

/**
 * @brief  ����LED����
 * @param  on 1Ϊ��������ֵΪ��
 * @retval ��
 */
void LCD_BackLed_Control ( int on )
{
	if ( on )
		  GPIO_SetBits(LTDC_BL_GPIO_PORT,LTDC_BL_GPIO_PIN);
	else
		  GPIO_ResetBits(LTDC_BL_GPIO_PORT,LTDC_BL_GPIO_PIN);		
}

/**
  * @brief ��ʼ��LTDC�� �� ����
  * @param fb_addr �Դ��׵�ַ
  * @param pixel_format ���ظ�ʽ��
           ��LTDC_Pixelformat_ARGB8888 ��LTDC_Pixelformat_RGB565��
  * @retval None
  */
void LCD_LayerInit(uint32_t fb_addr, uint32_t pixel_format )
{
  LTDC_Layer_InitTypeDef LTDC_Layer_InitStruct; 
  
  /* �㴰������ */
  /* ���ñ���Ĵ��ڱ߽磬ע����Щ�����ǰ���HBP HSW VBP VSW�� */    
	//һ�еĵ�һ����ʼ���أ��ó�ԱֵӦ��Ϊ (LTDC_InitStruct.LTDC_AccumulatedHBP+1)��ֵ
	LTDC_Layer_InitStruct.LTDC_HorizontalStart = HBP + HSW;
	//һ�е����һ�����أ��ó�ԱֵӦ��Ϊ (LTDC_InitStruct.LTDC_AccumulatedActiveW)��ֵ
	LTDC_Layer_InitStruct.LTDC_HorizontalStop = HSW+HBP+LCD_PIXEL_WIDTH-1;
	//һ�еĵ�һ����ʼ���أ��ó�ԱֵӦ��Ϊ (LTDC_InitStruct.LTDC_AccumulatedVBP+1)��ֵ
	LTDC_Layer_InitStruct.LTDC_VerticalStart =  VBP + VSW;
	//һ�е����һ�����أ��ó�ԱֵӦ��Ϊ (LTDC_InitStruct.LTDC_AccumulatedActiveH)��ֵ
	LTDC_Layer_InitStruct.LTDC_VerticalStop = VSW+VBP+LCD_PIXEL_HEIGHT-1;
	
  /* ���ظ�ʽ����*/
  LTDC_Layer_InitStruct.LTDC_PixelFormat = pixel_format;
  /* �㶨Alphaֵ���ã�0-255 */
  LTDC_Layer_InitStruct.LTDC_ConstantAlpha = 255; 
  /* Ĭ�ϱ�����ɫ������ɫ�ڶ���Ĳ㴰������ڲ��ֹʱʹ�á� */          
  LTDC_Layer_InitStruct.LTDC_DefaultColorBlue = 0xFF;        
  LTDC_Layer_InitStruct.LTDC_DefaultColorGreen = 0xFF;       
  LTDC_Layer_InitStruct.LTDC_DefaultColorRed = 0xFF;         
  LTDC_Layer_InitStruct.LTDC_DefaultColorAlpha = 0xFF;
  /* ���û������ CA��ʾʹ�ú㶨Alphaֵ��PAxCA��ʾʹ������Alpha x �㶨Alphaֵ */       
  LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_CA;    
  LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_PAxCA;
  
  /* �ó�ԱӦд��(һ����������ռ�õ��ֽ���+3)
  Line Lenth = ����Ч���ظ��� x ÿ�����ص��ֽ��� + 3 
  ����Ч���ظ��� = LCD_PIXEL_WIDTH 
  ÿ�����ص��ֽ��� = 2��RGB565/RGB1555��/ 3 (RGB888)/ 4��ARGB8888�� 
  */ 
  LTDC_Layer_InitStruct.LTDC_CFBLineLength = ((LCD_PIXEL_WIDTH * PIXEL_BPP[pixel_format]) + 3);
  /* ��ĳ�е���ʼλ�õ���һ����ʼλ�ô�����������
  Pitch = ����Ч���ظ��� x ÿ�����ص��ֽ��� */ 
  LTDC_Layer_InitStruct.LTDC_CFBPitch = (LCD_PIXEL_WIDTH * PIXEL_BPP[pixel_format]);
  
  /* ������Ч������ */  
  LTDC_Layer_InitStruct.LTDC_CFBLineNumber = LCD_PIXEL_HEIGHT;
  
  /* ���ñ�����Դ��׵�ַ */    
  LTDC_Layer_InitStruct.LTDC_CFBStartAdress = fb_addr;
  
  /* ����������ó�ʼ���� 1 ��*/
  LTDC_LayerInit(LTDC_Layer1, &LTDC_Layer_InitStruct);

  /* ������������ */  
  LTDC_ReloadConfig(LTDC_IMReload);

 /*ֻʹ��һ�� */
  LTDC_LayerCmd(LTDC_Layer1, ENABLE); 
  LTDC_LayerCmd(LTDC_Layer2, DISABLE);
 
  /* ������������ */  
  LTDC_ReloadConfig(LTDC_IMReload);
}

/**
  * @brief LCD��ʼ��
  * @param fb_addr �Դ��׵�ַ
  * @param  lcd_clk_mhz ����ʱ��Ƶ�ʣ�Ϊ 0 ʱֱ��ʹ���Ƽ�ʱ��Ƶ��
            RGB565��ʽ�Ƽ�Ϊ30~33��
            XRGB8888��ʽ�Ƽ�Ϊ20~22
            ���޷�ΧΪ15~52������ֵ�ᳬ��LTDCʱ�ӷ�Ƶ���÷�Χ
  * @param pixel_format ���ظ�ʽ����LTDC_Pixelformat_ARGB8888 ��LTDC_Pixelformat_RGB565��
  * @retval  None 
  */
void LCD_Init(uint32_t fb_addr, int lcd_clk_mhz, uint32_t pixel_format )
{ 
  uint32_t div;
  LTDC_InitTypeDef       LTDC_InitStruct;
  
  /* lcd_clk_mhzΪ0ʱʹ���Ƽ�ʱ��Ƶ�� */
  if(lcd_clk_mhz == 0)
  {
    if(pixel_format == LTDC_Pixelformat_RGB565||
        pixel_format == LTDC_Pixelformat_ARGB1555||
        pixel_format == LTDC_Pixelformat_ARGB4444||
        pixel_format == LTDC_Pixelformat_L8||
        pixel_format == LTDC_Pixelformat_AL88)
    {
      lcd_clk_mhz = lcd_param[cur_lcd].comment_clock_2byte;
    }
    else if(pixel_format == LTDC_Pixelformat_ARGB8888||
              pixel_format == LTDC_Pixelformat_RGB888)
    {
      lcd_clk_mhz = lcd_param[cur_lcd].comment_clock_4byte;
    }
  }
  
  /* ʹ��LTDC����ʱ�� */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_LTDC, ENABLE);
  
  /* ʹ��DMA2Dʱ�� */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2D, ENABLE);
	
	/* ��ʼ��LCD�Ŀ������� */
  LCD_GPIO_Config();   
	
	/* ���� PLLSAI ��Ƶ�������������Ϊ����ͬ��ʱ��CLK*/
  /* PLLSAI_VCO ����ʱ�� = HSE_VALUE/PLL_M = 1 Mhz */
  /* PLLSAI_VCO ���ʱ�� = PLLSAI_VCO���� * PLLSAI_N = 420 Mhz */
  /* PLLLCDCLK = PLLSAI_VCO ���/PLLSAI_R = 420/div  Mhz */
  /* LTDC ʱ��Ƶ�� = PLLLCDCLK / DIV = 420/div/4 = PIXEL_CLK_MHZ Mhz */
	/* LTDCʱ��̫�߻ᵼ����������ˢ���ٶ�Ҫ�󲻸ߣ�����ʱ��Ƶ�ʿɼ��ٻ�������*/
	/* ���º������������ֱ�Ϊ��PLLSAIN,PLLSAIQ,PLLSAIR������PLLSAIQ��LTDC�޹�*/
  div = 420/4/lcd_clk_mhz;
  
  RCC_PLLSAIConfig(420,7,div);
	/*���º����Ĳ���ΪDIVֵ*/
  RCC_LTDCCLKDivConfig(RCC_PLLSAIDivR_Div4);
  
  /* ʹ�� PLLSAI ʱ�� */
  RCC_PLLSAICmd(ENABLE);
  /* �ȴ� PLLSAI ��ʼ����� */
  while(RCC_GetFlagStatus(RCC_FLAG_PLLSAIRDY) == RESET)
  {
  }
  
  /* LTDC����*********************************************************/  
  /*�źż�������*/
  /* ��ͬ���źż��� */
  LTDC_InitStruct.LTDC_HSPolarity = LTDC_HSPolarity_AL;     
  /* ��ֱͬ���źż��� */  
  LTDC_InitStruct.LTDC_VSPolarity = LTDC_VSPolarity_AL;     
  /* ����ʹ���źż��� */
  LTDC_InitStruct.LTDC_DEPolarity = LTDC_DEPolarity_AL;     
  /* ����ͬ��ʱ�Ӽ��� */ 
  LTDC_InitStruct.LTDC_PCPolarity = LTDC_PCPolarity_IPC;
  
  /* ����LCD������ɫ */                   
  LTDC_InitStruct.LTDC_BackgroundRedValue = 0;            
  LTDC_InitStruct.LTDC_BackgroundGreenValue = 0;          
  LTDC_InitStruct.LTDC_BackgroundBlueValue = 0;    
 
  /* ʱ��������� */  
 /* ������ͬ���źſ��(HSW-1) */
 LTDC_InitStruct.LTDC_HorizontalSync =HSW-1;
 /* ���ô�ֱͬ���źſ��(VSW-1) */
 LTDC_InitStruct.LTDC_VerticalSync = VSW-1;
 /* ����(HSW+HBP-1) */
 LTDC_InitStruct.LTDC_AccumulatedHBP =HSW+HBP-1;
 /* ����(VSW+VBP-1) */
 LTDC_InitStruct.LTDC_AccumulatedVBP = VSW+VBP-1;
 /* ����(HSW+HBP+��Ч���ؿ��-1) */
 LTDC_InitStruct.LTDC_AccumulatedActiveW = HSW+HBP+LCD_PIXEL_WIDTH-1;
 /* ����(VSW+VBP+��Ч���ظ߶�-1) */
 LTDC_InitStruct.LTDC_AccumulatedActiveH = VSW+VBP+LCD_PIXEL_HEIGHT-1;
 /* �����ܿ��(HSW+HBP+��Ч���ؿ��+HFP-1) */
 LTDC_InitStruct.LTDC_TotalWidth =HSW+ HBP+LCD_PIXEL_WIDTH  + HFP-1; 
 /* �����ܸ߶�(VSW+VBP+��Ч���ظ߶�+VFP-1) */
 LTDC_InitStruct.LTDC_TotalHeigh =VSW+ VBP+LCD_PIXEL_HEIGHT  + VFP-1;

  LTDC_Init(&LTDC_InitStruct);
  
  LTDC_Cmd(ENABLE);  

  LCD_LayerInit(fb_addr, pixel_format);
}  

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
