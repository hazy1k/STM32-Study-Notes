/**
  ******************************************************************************
  * @file    bsp_lcd.c
  * @author  fire
  * @version V1.0
  * @date    2016-xx-xx
  * @brief   LCDӦ�ú����ӿڣ�֧��ARGB888/RGB888/RGB565/ARGB4444/ARGB1555 (����������ʾ)
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 H750 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "./lcd/bsp_lcd.h"


static LTDC_HandleTypeDef  Ltdc_Handler;
static DMA2D_HandleTypeDef Dma2d_Handler;

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


/**
  * @brief  ��ʼ������LCD��IO
  * @param  ��
  * @retval ��
  */
static void LCD_GPIO_Config(void)
{ 
	GPIO_InitTypeDef GPIO_InitStruct;
  
  /* ʹ��LCDʹ�õ�������ʱ�� */
                          //��ɫ������
  LTDC_R0_GPIO_CLK_ENABLE();LTDC_R1_GPIO_CLK_ENABLE();LTDC_R2_GPIO_CLK_ENABLE();\
  LTDC_R3_GPIO_CLK_ENABLE();LTDC_R4_GPIO_CLK_ENABLE();LTDC_R5_GPIO_CLK_ENABLE();\
  LTDC_R6_GPIO_CLK_ENABLE();LTDC_R7_GPIO_CLK_ENABLE();LTDC_G0_GPIO_CLK_ENABLE();\
  LTDC_G1_GPIO_CLK_ENABLE();LTDC_G2_GPIO_CLK_ENABLE();LTDC_G3_GPIO_CLK_ENABLE();\
  LTDC_G3_GPIO_CLK_ENABLE();LTDC_G5_GPIO_CLK_ENABLE();LTDC_G6_GPIO_CLK_ENABLE();\
  LTDC_G7_GPIO_CLK_ENABLE();LTDC_B0_GPIO_CLK_ENABLE();LTDC_B1_GPIO_CLK_ENABLE();\
  LTDC_B2_GPIO_CLK_ENABLE();LTDC_B3_GPIO_CLK_ENABLE();LTDC_B4_GPIO_CLK_ENABLE();\
  LTDC_B5_GPIO_CLK_ENABLE();LTDC_B6_GPIO_CLK_ENABLE();LTDC_B7_GPIO_CLK_ENABLE();\
  LTDC_CLK_GPIO_CLK_ENABLE();LTDC_HSYNC_GPIO_CLK_ENABLE();LTDC_VSYNC_GPIO_CLK_ENABLE();\
  LTDC_DE_GPIO_CLK_ENABLE();LTDC_BL_GPIO_CLK_ENABLE();
/* GPIO���� */

 /* ��ɫ������ */                        
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  
  GPIO_InitStruct.Pin =   LTDC_R0_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_R0_AF;
  HAL_GPIO_Init(LTDC_R0_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin =   LTDC_R1_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_R1_AF;
  HAL_GPIO_Init(LTDC_R1_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_R2_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_R2_AF;
  HAL_GPIO_Init(LTDC_R2_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_R3_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_R3_AF;
  HAL_GPIO_Init(LTDC_R3_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_R4_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_R4_AF;
  HAL_GPIO_Init(LTDC_R4_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_R5_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_R5_AF;
  HAL_GPIO_Init(LTDC_R5_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_R6_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_R6_AF;
  HAL_GPIO_Init(LTDC_R6_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_R7_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_R7_AF;
  HAL_GPIO_Init(LTDC_R7_GPIO_PORT, &GPIO_InitStruct);
  
  //��ɫ������
  GPIO_InitStruct.Pin =   LTDC_G0_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_G0_AF;
  HAL_GPIO_Init(LTDC_G0_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin =   LTDC_G1_GPIO_PIN; 
  GPIO_InitStruct.Alternate = LTDC_G1_AF;
  HAL_GPIO_Init(LTDC_G1_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_G2_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_G2_AF;
  HAL_GPIO_Init(LTDC_G2_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_G3_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_G3_AF;
  HAL_GPIO_Init(LTDC_G3_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_G4_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_G4_AF;
  HAL_GPIO_Init(LTDC_G4_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_G5_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_G5_AF;
  HAL_GPIO_Init(LTDC_G5_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_G6_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_G6_AF;
  HAL_GPIO_Init(LTDC_G6_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_G7_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_G7_AF;
  HAL_GPIO_Init(LTDC_G7_GPIO_PORT, &GPIO_InitStruct);
  
  //��ɫ������
  GPIO_InitStruct.Pin =   LTDC_B0_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B0_AF;
  HAL_GPIO_Init(LTDC_B0_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin =   LTDC_B1_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B1_AF;
  HAL_GPIO_Init(LTDC_B1_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_B2_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B2_AF;
  HAL_GPIO_Init(LTDC_B2_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_B3_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B3_AF;
  HAL_GPIO_Init(LTDC_B3_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_B4_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B4_AF;
  HAL_GPIO_Init(LTDC_B4_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_B5_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B5_AF;
  HAL_GPIO_Init(LTDC_B5_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_B6_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B6_AF;
  HAL_GPIO_Init(LTDC_B6_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin =   LTDC_B7_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_B7_AF;
  HAL_GPIO_Init(LTDC_B7_GPIO_PORT, &GPIO_InitStruct);
  
  //�����ź���
  GPIO_InitStruct.Pin = LTDC_CLK_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_CLK_AF;
  HAL_GPIO_Init(LTDC_CLK_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = LTDC_HSYNC_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_HSYNC_AF;
  HAL_GPIO_Init(LTDC_HSYNC_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = LTDC_VSYNC_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_VSYNC_AF;
  HAL_GPIO_Init(LTDC_VSYNC_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = LTDC_DE_GPIO_PIN;
  GPIO_InitStruct.Alternate = LTDC_DE_AF;
  HAL_GPIO_Init(LTDC_DE_GPIO_PORT, &GPIO_InitStruct);
  
  //����BL
  GPIO_InitStruct.Pin = LTDC_BL_GPIO_PIN;                             
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;

  HAL_GPIO_Init(LTDC_BL_GPIO_PORT, &GPIO_InitStruct);
  
}
/**
  * @brief  ʱ������
  * @retval lcd_clk_mhzҪ���õ�����ʱ��Ƶ��
  */
void LCD_ClockConfig(int lcd_clk_mhz)
{
  uint32_t div;
  static RCC_PeriphCLKInitTypeDef  periph_clk_init_struct;

  /* ����ʱ�Ӽ����Ƶ���� */
  div = 420/lcd_clk_mhz;

	/* Һ����ʱ������ */
	/* 5�����ĵ���PCLKΪlcd_clk_mhz MHz�����PLL3R����Ϊ�ṩ��ʱ�� */ 
	/* PLL3_VCO Input = HSE_VALUE/PLL3M = 1 Mhz */
	/* PLL3_VCO Output = PLL3_VCO Input * PLL3N = 420 Mhz */
	/* PLLLCDCLK = PLL3_VCO Output/PLL3R = 420/div = lcd_clk_mhz Mhz */
	/* LTDC clock frequency = PLLLCDCLK = lcd_clk_mhz Mhz */    
	periph_clk_init_struct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
	periph_clk_init_struct.PLL3.PLL3M = 25;    
	periph_clk_init_struct.PLL3.PLL3N = 420;
	periph_clk_init_struct.PLL3.PLL3P = 2;
	periph_clk_init_struct.PLL3.PLL3Q = 2;
	periph_clk_init_struct.PLL3.PLL3R = div;  
	HAL_RCCEx_PeriphCLKConfig(&periph_clk_init_struct);  
}

/**
 * @brief  ����LED����
 * @param  on 1Ϊ��������ֵΪ��
 * @retval ��
 */
void LCD_BackLed_Control ( int on )
{
	if ( on )
		  HAL_GPIO_WritePin(LTDC_BL_GPIO_PORT,LTDC_BL_GPIO_PIN,GPIO_PIN_SET);
	else
		  HAL_GPIO_WritePin(LTDC_BL_GPIO_PORT,LTDC_BL_GPIO_PIN,GPIO_PIN_RESET);
}
/**
  * @brief  ��ʼ��LCD�� 
  * @param  LayerIndex:  ǰ����(��1)���߱�����(��0)
  * @param  FB_Address:  ÿһ���Դ���׵�ַ
  * @param  PixelFormat: ������ظ�ʽ
  * @retval ��
  */
void LCD_LayerInit(uint16_t LayerIndex, uint32_t FB_Address,uint32_t PixelFormat)
{     
  LTDC_LayerCfgTypeDef  layer_cfg;

  /* ���ʼ�� */
  layer_cfg.WindowX0 = 0;				//������ʼλ��X����
  layer_cfg.WindowX1 = LCD_PIXEL_WIDTH;	//���ڽ���λ��X����
  layer_cfg.WindowY0 = 0;				//������ʼλ��Y����
  layer_cfg.WindowY1 = LCD_PIXEL_HEIGHT;  //���ڽ���λ��Y����
  layer_cfg.PixelFormat = PixelFormat;	//���ظ�ʽ
  layer_cfg.FBStartAdress = FB_Address; //���Դ��׵�ַ
  layer_cfg.Alpha = 0xFF;				//���ڻ�ϵ�͸���ȳ�������Χ��0��255��0Ϊ��ȫ͸��
  layer_cfg.Alpha0 = 0xFF;					//Ĭ��͸���ȳ�������Χ��0��255��0Ϊ��ȫ͸��
  layer_cfg.Backcolor.Blue = 0xFF;			//�㱳����ɫ��ɫ����
  layer_cfg.Backcolor.Green = 0xFF;		//�㱳����ɫ��ɫ����
  layer_cfg.Backcolor.Red = 0xFF;			//�㱳����ɫ��ɫ����
  layer_cfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;//����ϵ��1
  layer_cfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;//����ϵ��2
  layer_cfg.ImageWidth = LCD_PIXEL_WIDTH;//����ͼ����
  layer_cfg.ImageHeight = LCD_PIXEL_HEIGHT;//����ͼ��߶�
  
  HAL_LTDC_ConfigLayer(&Ltdc_Handler, &layer_cfg, LayerIndex); //����ѡ�еĲ����
  
  __HAL_LTDC_RELOAD_CONFIG(&Ltdc_Handler);//���ز�����ò���
}

/**
  * @brief LCD��ʼ��
  * @param fb_addr �Դ��׵�ַ
  * @param  lcd_clk_mhz ����ʱ��Ƶ�ʣ���ʱû��ʹ�ã���Ϊ 0 ʱֱ��ʹ���Ƽ�ʱ��Ƶ��
            RGB565��ʽ�Ƽ�Ϊ30~33��
            XRGB8888��ʽ�Ƽ�Ϊ20~22
            ���޷�ΧΪ15~52������ֵ�ᳬ��LTDCʱ�ӷ�Ƶ���÷�Χ
  * @param pixel_format ���ظ�ʽ����LTDC_Pixelformat_ARGB8888 ��LTDC_Pixelformat_RGB565��
  * @retval  None 
  */
void LCD_Init(uint32_t fb_addr, int lcd_clk_mhz, uint32_t pixel_format )
{ 
    /* ʹ��LTDCʱ�� */
    __HAL_RCC_LTDC_CLK_ENABLE();
    /* ʹ��DMA2Dʱ�� */
    __HAL_RCC_DMA2D_CLK_ENABLE();
  
    /* lcd_clk_mhzΪ0ʱʹ���Ƽ�ʱ��Ƶ�� */
    if(lcd_clk_mhz == 0)
    {
      if(pixel_format == LTDC_PIXEL_FORMAT_RGB565||
          pixel_format == LTDC_PIXEL_FORMAT_ARGB1555||
          pixel_format == LTDC_PIXEL_FORMAT_ARGB4444||
          pixel_format == LTDC_PIXEL_FORMAT_L8||
          pixel_format == LTDC_PIXEL_FORMAT_AL88)
      {
        lcd_clk_mhz = lcd_param[cur_lcd].comment_clock_2byte;
      }
      else if(pixel_format == LTDC_PIXEL_FORMAT_ARGB8888||
                pixel_format == LTDC_PIXEL_FORMAT_RGB888)
      {
        lcd_clk_mhz = lcd_param[cur_lcd].comment_clock_4byte;
      }
    }
    
	  /* Һ����ʱ������ */
		LCD_ClockConfig(lcd_clk_mhz);
    /* ��ʼ��LCD���� */
    LCD_GPIO_Config();
    /* ����LTDC���� */
    Ltdc_Handler.Instance = LTDC;  
    /* ������ͬ���źſ��(HSW-1) */
    Ltdc_Handler.Init.HorizontalSync =HSW-1;
    /* ���ô�ֱͬ���źſ��(VSW-1) */
    Ltdc_Handler.Init.VerticalSync = VSW-1;
    /* ����(HSW+HBP-1) */
    Ltdc_Handler.Init.AccumulatedHBP = HSW+HBP-1;
    /* ����(VSW+VBP-1) */
    Ltdc_Handler.Init.AccumulatedVBP = VSW+VBP-1;
    /* ����(HSW+HBP+��Ч���ؿ��-1) */
    Ltdc_Handler.Init.AccumulatedActiveW = HSW+HBP+LCD_PIXEL_WIDTH-1;
    /* ����(VSW+VBP+��Ч���ظ߶�-1) */
    Ltdc_Handler.Init.AccumulatedActiveH = VSW+VBP+LCD_PIXEL_HEIGHT-1;
    /* �����ܿ��(HSW+HBP+��Ч���ؿ��+HFP-1) */
    Ltdc_Handler.Init.TotalWidth =HSW+ HBP+LCD_PIXEL_WIDTH + HFP-1; 
    /* �����ܸ߶�(VSW+VBP+��Ч���ظ߶�+VFP-1) */
    Ltdc_Handler.Init.TotalHeigh =VSW+ VBP+LCD_PIXEL_HEIGHT + VFP-1;

    /* ��ʼ��LCD�����ؿ�Ⱥ͸߶� */
    Ltdc_Handler.LayerCfg->ImageWidth  = LCD_PIXEL_WIDTH;
    Ltdc_Handler.LayerCfg->ImageHeight = LCD_PIXEL_HEIGHT;
    /* ����LCD���������ɫ��Ĭ�Ϻ�ɫ */
    Ltdc_Handler.Init.Backcolor.Red = 0;
    Ltdc_Handler.Init.Backcolor.Green = 0;
    Ltdc_Handler.Init.Backcolor.Blue = 0;
    /* �������� */
    /* ��ʼ����ͬ�����ԣ��͵�ƽ��Ч */
    Ltdc_Handler.Init.HSPolarity = LTDC_HSPOLARITY_AL;
    /* ��ʼ����ͬ�����ԣ��͵�ƽ��Ч */
    Ltdc_Handler.Init.VSPolarity = LTDC_VSPOLARITY_AL;
    /* ��ʼ��������Ч���ԣ��͵�ƽ��Ч */
    Ltdc_Handler.Init.DEPolarity = LTDC_DEPOLARITY_AL;
    /* ��ʼ��������ʱ�Ӽ��ԣ�ͬ����ʱ�� */
    Ltdc_Handler.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
    HAL_LTDC_Init(&Ltdc_Handler);

    /* ��ʼ����ʾ�� */
    LCD_LayerInit(0,fb_addr,pixel_format);
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
