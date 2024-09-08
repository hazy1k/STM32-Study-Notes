/**
  ******************************************************************
  * @file    bsp_lcd.c
  * @author  fire
  * @version V2.0
  * @date    2018-xx-xx
  * @brief   lcdӦ�ú����ӿ�
  ******************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  i.MXRT1052������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************
  */
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"  
#include "fsl_elcdif.h" 
#include "fsl_clock.h"
#include "fsl_pxp.h"

	
#include "pad_config.h"  
#include "./lcd/bsp_lcd.h" 



/*******************************************************************************
 * ����
 ******************************************************************************/

/* ֡�жϱ�־ */
volatile bool s_frameDone = false;

/* ֡����������ʹ��֡�жϲ���Ч */
__IO uint32_t s_frame_count = 0;

/* �Դ� */
AT_NONCACHEABLE_SECTION_ALIGN( pixel_t s_psBufferLcd[2][LCD_PIXEL_HEIGHT][LCD_PIXEL_WIDTH], FRAME_BUFFER_ALIGN);

/*���ڴ洢��ǰѡ��������ʽ*/
static sFONT *LCD_Currentfonts = &Font16x32;
/* ���ڴ洢��ǰ������ɫ�����屳����ɫ�ı���*/
static pixel_t CurrentTextColor   = CL_BLUE;
static pixel_t CurrentBackColor   = CL_BLACK;

/* ָ��ǰ���Դ棬�����ǵ�ַ��������32λ���� */
static uint32_t CurrentFrameBuffer = (uint32_t)s_psBufferLcd[0];

/*******************************************************************************
 * ��
 ******************************************************************************/
/* �������ž�ʹ��ͬ����PAD���� */
#define LCD_PAD_CONFIG_DATA            (SRE_1_FAST_SLEW_RATE| \
                                        DSE_6_R0_6| \
                                        SPEED_3_MAX_200MHz| \
                                        ODE_0_OPEN_DRAIN_DISABLED| \
                                        PKE_1_PULL_KEEPER_ENABLED| \
                                        PUE_0_KEEPER_SELECTED| \
                                        PUS_0_100K_OHM_PULL_DOWN| \
                                        HYS_0_HYSTERESIS_DISABLED)   
    /* ����˵�� : */
    /* ת������: ת�����ʿ�
        ����ǿ��: R0/6 
        �������� : max(200MHz)
        ��©����: �ر� 
        ��/����������: ʹ��
        ��/������ѡ��: ������
        ����/����ѡ��: 100Kŷķ����(ѡ���˱�������������Ч)
        �ͻ�������: ��ֹ */
        
/*******************************************************************************
 * ����
 ******************************************************************************/
static void LCD_IOMUXC_MUX_Config(void);
static void LCD_IOMUXC_PAD_Config(void);
static void LCD_ELCDIF_Config(void);


/**
* @brief  ��ʼ��LCD���IOMUXC��MUX��������
* @param  ��
* @retval ��
*/
static void LCD_IOMUXC_MUX_Config(void)
{
    /* �������ž�������SION���� */
    /* ʱ������ź��� */
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_00_LCD_CLK, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_01_LCD_ENABLE, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_02_LCD_HSYNC, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_03_LCD_VSYNC, 0U);
  
    /* RGB565�����ź��ߣ�
     DATA0~DATA4:B3~B7
     DATA5~DATA10:G2~G7
     DATA11~DATA15:R3~R7 */
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_04_LCD_DATA00, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_05_LCD_DATA01, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_06_LCD_DATA02, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_07_LCD_DATA03, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_08_LCD_DATA04, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_09_LCD_DATA05, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_10_LCD_DATA06, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_11_LCD_DATA07, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_12_LCD_DATA08, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_13_LCD_DATA09, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_14_LCD_DATA10, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B0_15_LCD_DATA11, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_00_LCD_DATA12, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_01_LCD_DATA13, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_02_LCD_DATA14, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_03_LCD_DATA15, 0U); 

		/* ��ʹ��24λ�����ź�����Ҫ��ʼ�����������ź��� */
#if LCD_BUS_24_BIT
		IOMUXC_SetPinMux(IOMUXC_GPIO_B1_04_LCD_DATA16, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_05_LCD_DATA17, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_06_LCD_DATA18, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_07_LCD_DATA19, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_08_LCD_DATA20, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_09_LCD_DATA21, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_10_LCD_DATA22, 0U);                                    
    IOMUXC_SetPinMux(IOMUXC_GPIO_B1_11_LCD_DATA23, 0U);                                    
#endif 
		
    /* LCD_BL��������ź��� */
    IOMUXC_SetPinMux(LCD_BL_IOMUXC, 0U); 
}


/**
* @brief  ��ʼ��LCD���IOMUXC��PAD��������
* @param  ��
* @retval ��
*/
static void LCD_IOMUXC_PAD_Config(void)
{  
    /* �������ž�ʹ��ͬ����PAD���� */
    /* ʱ������ź��� */
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_00_LCD_CLK,LCD_PAD_CONFIG_DATA);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_01_LCD_ENABLE, LCD_PAD_CONFIG_DATA);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_02_LCD_HSYNC, LCD_PAD_CONFIG_DATA);  
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_03_LCD_VSYNC, LCD_PAD_CONFIG_DATA); 

    /* RGB565�����ź��ߣ�
     DATA0~DATA4:B3~B7
     DATA5~DATA10:G2~G7
     DATA11~DATA15:R3~R7 */
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_04_LCD_DATA00, LCD_PAD_CONFIG_DATA); 
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_05_LCD_DATA01, LCD_PAD_CONFIG_DATA); 
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_06_LCD_DATA02, LCD_PAD_CONFIG_DATA);  
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_07_LCD_DATA03, LCD_PAD_CONFIG_DATA); 
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_08_LCD_DATA04, LCD_PAD_CONFIG_DATA); 
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_09_LCD_DATA05, LCD_PAD_CONFIG_DATA);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_10_LCD_DATA06, LCD_PAD_CONFIG_DATA);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_11_LCD_DATA07, LCD_PAD_CONFIG_DATA);  
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_12_LCD_DATA08, LCD_PAD_CONFIG_DATA);  
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_13_LCD_DATA09, LCD_PAD_CONFIG_DATA);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_14_LCD_DATA10, LCD_PAD_CONFIG_DATA); 
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B0_15_LCD_DATA11, LCD_PAD_CONFIG_DATA);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_00_LCD_DATA12, LCD_PAD_CONFIG_DATA);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_01_LCD_DATA13, LCD_PAD_CONFIG_DATA);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_02_LCD_DATA14, LCD_PAD_CONFIG_DATA); 
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_03_LCD_DATA15, LCD_PAD_CONFIG_DATA); 
		
		/* ��ʹ��24λ�����ź�����Ҫ��ʼ�����������ź��� */
#if LCD_BUS_24_BIT
		IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_04_LCD_DATA16, LCD_PAD_CONFIG_DATA);                                    
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_05_LCD_DATA17, LCD_PAD_CONFIG_DATA);                                    
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_06_LCD_DATA18, LCD_PAD_CONFIG_DATA);                                    
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_07_LCD_DATA19, LCD_PAD_CONFIG_DATA);                                    
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_08_LCD_DATA20, LCD_PAD_CONFIG_DATA);                                    
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_09_LCD_DATA21, LCD_PAD_CONFIG_DATA);                                    
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_10_LCD_DATA22, LCD_PAD_CONFIG_DATA);                                    
    IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_11_LCD_DATA23, LCD_PAD_CONFIG_DATA);                                    
#endif     
    /* LCD_BL��������ź��� */
    IOMUXC_SetPinConfig(LCD_BL_IOMUXC, LCD_PAD_CONFIG_DATA);
}

/**
* @brief  ��ʼ��ELCDIF����
* @param  ��
* @retval ��
*/
static void LCD_ELCDIF_Config(void)
{	
    const elcdif_rgb_mode_config_t config = {
        .panelWidth = LCD_PIXEL_WIDTH,
        .panelHeight = LCD_PIXEL_HEIGHT,
        .hsw = LCD_HSW,
        .hfp = LCD_HFP,
        .hbp = LCD_HBP,
        .vsw = LCD_VSW,
        .vfp = LCD_VFP,
        .vbp = LCD_VBP,
        .polarityFlags =  LCD_POLARITY_FLAGS,													
        .bufferAddr = (uint32_t)s_psBufferLcd[0],
        .pixelFormat = ELCDIF_PIXEL_FORMAT,
        .dataBus = LCD_DATA_BUS_WIDTH,
    };
 
  ELCDIF_RgbModeInit(LCDIF, &config);
  ELCDIF_RgbModeStart(LCDIF);
}

/**
* @brief  ��ʼ��ELCDIFʹ�õ�ʱ��
* @param  ��
* @retval ��
*/
void LCD_InitClock(void)
{
    /*
     * Ҫ��֡�����ó�60Hz����������ʱ��Ƶ��Ϊ:
     * ˮƽ����ʱ�Ӹ�����(LCD_IMG_WIDTH + LCD_HSW + LCD_HFP + LCD_HBP ) 
     * ��ֱ������(LCD_IMG_HEIGHT + LCD_VSW + LCD_VFP + LCD_VBP)
     * 
     * ����ʱ��Ƶ�ʣ�(800 + 1 + 22 + 46) * (480 + 1 + 22 + 23) * 60 = 27.4M.
     * ���������� LCDIF ����ʱ��Ƶ��Ϊ 27M.
     *	 LCD��֡����ʵ���Ϊ׼��
     */

    /*
     * ��ʼ�� Vedio PLL����PLL5
     * Video PLL ���Ƶ��Ϊ 
     * OSC24M * (loopDivider + (denominator / numerator)) / postDivider = 108MHz.
     */
    clock_video_pll_config_t config = {
        .loopDivider = 36, .postDivider = 8, .numerator = 0, .denominator = 0,
    };

    CLOCK_InitVideoPll(&config);

    /*
     * 000 derive clock from PLL2
     * 001 derive clock from PLL3 PFD3
     * 010 derive clock from PLL5
     * 011 derive clock from PLL2 PFD0
     * 100 derive clock from PLL2 PFD1
     * 101 derive clock from PLL3 PFD1
     */
    /* ѡ��Ϊvedio PLL����PLL5 */
    CLOCK_SetMux(kCLOCK_LcdifPreMux, 2);

    /* ����Ԥ��Ƶ */  
    CLOCK_SetDiv(kCLOCK_LcdifPreDiv, 1);

		/* ���÷�Ƶ */  
    CLOCK_SetDiv(kCLOCK_LcdifDiv, 1);
}

/**
* @brief  ��ʼ���������Ų�����
* @param  ��
* @retval ��
*/
void LCD_BackLight_ON(void)
{    
    /* ���⣬�ߵ�ƽ���� */
    gpio_pin_config_t config = {
      kGPIO_DigitalOutput, 
      1,
      kGPIO_NoIntmode
    };

    GPIO_PinInit(LCD_BL_GPIO, LCD_BL_GPIO_PIN, &config);
}



/**
* @brief  ��ʼ��Һ����
* @param  enableInterrupt ���Ƿ�ʹ���ж�
*		@arg LCD_INTERRUPT_DISABLE ��ʹ��
*		@arg LCD_INTERRUPT_ENABLE  ʹ��
* @retval ��
*/
void LCD_Init(bool enableInterrupt)
{
#if LCD_RGB_888	
	/* 
  * ����������LCD read_qos �� write_qos �Ĵ�����֧������ֵ�ķ�ΧΪ0x0-0xF��
  * �˴�����qosΪ0xF���ֵ��
	*	Qos��
  * The Quality of Service (QoS) tidemark value represents the maximum
	*	permitted number of active transactions before the QoS mechanism is
	*	activated��
	*  ��ϸ˵����
	* ��IMXRT1050RM�����ο��ֲᣩ���½ڡ�Network Interconnect Bus System (NIC-301)��
	* ����CoreLink  Network Interconnect (NIC-301)Technical Reference Manua r2p3��
	* @note 
  *  ����˵�������LCDʹ��RT1052�ڲ����ߵĴ�������������
	*  ����800*480@XRGB8888@60Hz����ʾ����Ҫ�������ã�
  *  ����800*480@RGB565@60Hz����ʾ����Ҫ���ã�����Ĭ�ϼ��ɣ��Ƽ���
	*/
  *((uint32_t *)0x41044100) = 0x0000000f;
	*((uint32_t *)0x41044104) = 0x0000000f;
#endif
	
	/* ��ʼ��eLCDIF���š�ʱ�� ��ģʽ�������Լ��ж�*/
  LCD_IOMUXC_MUX_Config();
  LCD_IOMUXC_PAD_Config();
  LCD_InitClock();
	LCD_ELCDIF_Config();
  LCD_BackLight_ON();
  
  if(enableInterrupt)
  {
    LCD_InterruptConfig();
  }
}

/***************************�ж����******************************/
/**
* @brief  ����ELCDIF�ж�
* @param  ��
* @retval ��
*/
void LCD_InterruptConfig(void)
{
  /* ʹ���ж� */
  EnableIRQ(LCDIF_IRQn);
   
  /* ����ELCDIFΪCurFrameDoneInterrupt�ж� */
  ELCDIF_EnableInterrupts(LCDIF, kELCDIF_CurFrameDoneInterruptEnable);
}

/**
* @brief  ELCDIF�жϷ�����
* @param  ��
* @retval ��
*/
void LCDIF_IRQHandler(void)
{
    uint32_t intStatus;

    intStatus = ELCDIF_GetInterruptStatus(LCDIF);

    ELCDIF_ClearInterruptStatus(LCDIF, intStatus);

   if (intStatus & kELCDIF_CurFrameDone)
    {
				/* ��ǰ֡������ɱ�־ */
        s_frameDone = true;
				/* ֡������ */
				s_frame_count++;

    }

    /* ���²�����Ϊ ARM �Ŀ���838869��ӵ�, 
       �ô���Ӱ�� Cortex-M4, Cortex-M4F�ںˣ�       
       �����洢�����ص��쳣�����·��ز������ܻ�ָ�������ж�
        CM7����Ӱ�죬�˴������ô���
    */  
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

/***************************��ʾӦ�����******************************/

/***************************��ʾ�ַ����******************************/

/**
  * @brief  �����������ɫ������ı�����ɫ
  * @param  TextColor: ������ɫ
  * @param  BackColor: ����ı�����ɫ
  * @retval None
  */
void LCD_SetColors(pixel_t TextColor, pixel_t BackColor) 
{
  CurrentTextColor = TextColor; 
  CurrentBackColor = BackColor;
}

/**
  * @brief ��ȡ��ǰ���õ�������ɫ������ı�����ɫ
  * @param  TextColor: ָ��������ɫ��ָ��
  * @param  BackColor: ָ�����屳����ɫ��ָ��
  * @retval None
  */
void LCD_GetColors(pixel_t *TextColor, pixel_t *BackColor)
{
  *TextColor = CurrentTextColor;
  *BackColor = CurrentBackColor;
}

/**
  * @brief  ����������ɫ
  * @param  Color: ������ɫ
  * @retval None
  */
void LCD_SetTextColor(pixel_t Color)
{
  CurrentTextColor = Color;
}

/**
  * @brief  ��������ı�����ɫ
  * @param  Color: ����ı�����ɫ
  * @retval None
  */
void LCD_SetBackColor(pixel_t Color)
{
  CurrentBackColor = Color;
}

/**
  * @brief  ���������ʽ(Ӣ��)
  * @param  fonts: ѡ��Ҫ���õ������ʽ
  * @retval None
  */
void LCD_SetFont(sFONT *fonts)
{
  LCD_Currentfonts = fonts;
}

/**
  * @brief  ��ȡ��ǰ�����ʽ(Ӣ��)
  * @param  None.
  * @retval ��ǰӦ�õĸ�ʽ
  */
sFONT *LCD_GetFont(void)
{
  return LCD_Currentfonts;
}


/**
  * @brief  ����ʾ������ʾһ��Ӣ���ַ�
  * @param  Xpos ���ַ�����ʼX����
  * @param  Ypos ���ַ�����ʼY����
  * @param  Ascii: Ҫ��ʾ���ַ���ASCII��
  * @retval None
  */
void LCD_DisplayChar(uint16_t Xpos, uint16_t Ypos, char Ascii)
{
  uint16_t fontLength;	
  uint16_t page, column;

	uint16_t relativePositon;
	uint8_t *pfont;
  
  uint32_t yBufferPos = 0;
  uint32_t xPixelPos = 0;
  
  /*yBufferPos��ʾ��ǰ�е��Դ�ƫ��λ��*/
  yBufferPos = Ypos*LCD_PIXEL_WIDTH*LCD_BPP;
  
  /*xpixelPos��ʾ�������ص�λ��
    LCD_BPP*xPixelPos + yBufferPos ���ǵ�ǰ���ص���Դ�λ��
  */
  xPixelPos += Xpos;
	
	//��ascii���ƫ�ƣ���ģ������ASCII���ǰ32����ͼ�η��ţ�
	relativePositon = Ascii - ' ';
	
	//ÿ����ģ���ֽ���
	fontLength = (LCD_Currentfonts->Width*LCD_Currentfonts->Height)/8;
		
	//��ģ�׵�ַ
	/*ascii���ƫ��ֵ����ÿ����ģ���ֽ����������ģ��ƫ��λ��*/
	pfont = (uint8_t *)&LCD_Currentfonts->table[relativePositon * fontLength];
	
  //ÿ����ģ��LCD_Currentfonts->Height�У�����ÿһ��
  for ( page = 0; page < LCD_Currentfonts->Height; page++ )
	{    
    //ÿ����ģ��LCD_Currentfonts->Width/8 ���ֽڣ�����ÿ���ֽ�
    for ( column = 0; column < LCD_Currentfonts->Width/8; column++ ) 
		{	
      uint8_t bitCount = 0;

      //ÿ���ֽ���8������λ������ÿ������λ
      for(bitCount=0; bitCount<8; bitCount++)
      {
        if(*pfont & (0x80>>bitCount))
        {
           //����ɫ
           *(__IO pixel_t*)(CurrentFrameBuffer + (LCD_BPP*xPixelPos) + yBufferPos) = CurrentTextColor;        
        }
        else
        {
          //����ɫ
          *(__IO pixel_t*)(CurrentFrameBuffer + (LCD_BPP*xPixelPos) + yBufferPos) = CurrentBackColor; 
        }
        /*ָ��ǰ�е���һ����*/
        xPixelPos++;		
      }
      
      /* ָ����ģ���ݵ�һ�¸��ֽ� */
      pfont++;
    }      
    /*��ʾ��һ��*/
    /*ָ���ַ���ʾ������һ�еĵ�һ�����ص�*/
    xPixelPos += (LCD_PIXEL_WIDTH - LCD_Currentfonts->Width);		
  }
}

/**
 * @brief  ����ʾ������ʾ��Ӣ���ַ���,����Һ�����ʱ���Զ����С�
 * @param  Xpos ���ַ�����ʼX����
 * @param  Ypos ���ַ�����ʼY����
 * @param  pStr ��Ҫ��ʾ���ַ������׵�ַ
 * @retval ��
 */
void LCD_DispString(uint16_t Xpos, uint16_t Ypos, const uint8_t * pStr )
{
	while( * pStr != '\0' )
	{	
    /*�Զ�����*/
    if ( ( Xpos + LCD_Currentfonts->Width ) > LCD_PIXEL_WIDTH )
    {
      Xpos = 0;
      Ypos += LCD_Currentfonts->Height;
    }
    
    if ( ( Ypos + LCD_Currentfonts->Height ) > LCD_PIXEL_HEIGHT )
    {
      Xpos = 0;
      Ypos = 0;
    }			
        
    /* ��ʾ�����ַ� */
    LCD_DisplayChar(Xpos,Ypos,*pStr);
    
    Xpos += LCD_Currentfonts->Width;
  
    pStr ++;
  }		
} 

/**
  * @brief  ��ʾ�ַ���(Ӣ��)
  * @param  Line: ���ݵ�ǰ���������к�
  *     @arg Line(1),Line(2)��
  * @param  *ptr: Ҫ��ʾ���ַ���
  * @retval None
  */
void LCD_DisplayStringLine(uint16_t Line, uint8_t *ptr)
{  
  uint16_t refcolumn = 0;
  /* ѭ����ʾ�ַ���ֱ�������ַ���������
    ��ֱ��������ʾ�����ַ�
  */
  while ((refcolumn < LCD_PIXEL_WIDTH) && ((*ptr != 0) & 
    ((refcolumn + LCD_Currentfonts->Width) <= LCD_PIXEL_WIDTH)))
  {
    /* ��ʾ�����ַ� */
    LCD_DisplayChar(refcolumn,Line , *ptr);
    /* ƫ���ַ���ʾλ�� */
    refcolumn += LCD_Currentfonts->Width;
    /* ָ����һ���ַ� */
    ptr++;
  }
}

/**
  * @brief  ���ָ���е��ַ�
  * @param  Line: Ҫ�������,ע��LINE���Ǹ��ݵ�ǰ��������
  *     @arg LINE(1),LINE(2)
  * @retval None
  */
void LCD_ClearLine(uint16_t Line)
{
  uint16_t refcolumn = 0;
  /* ѭ����ʾ����Ļ���Ҳ� */
  while ((refcolumn < LCD_PIXEL_WIDTH) && 
    (((refcolumn + LCD_Currentfonts->Width)& 0xFFFF) >= LCD_Currentfonts->Width))
  {
    /* ��ʾ�ո��൱�������Ч���� */
    LCD_DisplayChar(refcolumn, Line, ' ');
    /* ƫ���ַ���ʾλ�� */
    refcolumn += LCD_Currentfonts->Width;
  }
}

/**
  * @brief  ������ʾ����
  * @param  Xpos: x����
  * @param  Ypos: y����
  * @retval �Դ�ĵ�ַ
  */
uint32_t LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{  
  return CurrentFrameBuffer + LCD_BPP*(Xpos + (LCD_PIXEL_WIDTH*Ypos));
}

/***************************��ʾͼ�����******************************/
/**
  * @brief ѡ��ǰҪ�������Դ�����
  * @param  index: 0��1
  * @retval None
  */
void LCD_SetFrameBuffer(uint8_t index)
{
  CurrentFrameBuffer = (uint32_t)s_psBufferLcd[index];
}

/**
  * @brief ���ú���Ҫ��ʾ���Դ�����
  * @param  index: 0��1
  * @retval None
  */
void LCD_SetDisplayBuffer(uint8_t index)
{
  /* ����ELCDIF����һ����������ַ */
  ELCDIF_SetNextBufferAddr(LCDIF, (uint32_t)s_psBufferLcd[index]);

}

/**
  * @brief ʹ�õ�ǰ��ɫ��ָ����λ�û���һ�����ص�
  * @param  Xpos: x����
  * @param  Ypos: y����
  * @note ��ʹ��LCD_SetBackColor��LCD_SetTextColor��LCD_SetColors����������ɫ
  * @retval None
  */
void PutPixel(uint16_t Xpos, uint16_t Ypos)
{   
	if ( ( Xpos < LCD_PIXEL_WIDTH ) && ( Ypos < LCD_PIXEL_HEIGHT ) )
  {
		*(pixel_t *)(CurrentFrameBuffer + LCD_BPP*(Xpos + (LCD_PIXEL_WIDTH*Ypos))) = CurrentTextColor;
	}
}

/**
  * @brief  �Ե�ǰ������ɫ���������Ļ
  * @param  ��
  * @note ��ʹ��LCD_SetBackColor��LCD_SetTextColor��LCD_SetColors����������ɫ
  * @retval ��
  */
void LCD_Clear(uint32_t Color)
{
  /* ������������� */
  uint16_t page, column;  
  
  /* ָ����ε�һ�����ص���Դ�λ�� */
  pixel_t *pRectImage = (pixel_t*)CurrentFrameBuffer ;
  
  /* ����ÿһ�� */
  for ( page = 0; page < LCD_PIXEL_HEIGHT; page++ )
  {    
    /* ����ÿһ�� */
    for ( column = 0; column < LCD_PIXEL_WIDTH; column++ ) 
    {	
      *pRectImage = Color;
      
      /* ָ����һ�����ص���Դ�λ�� */
      pRectImage++;
    }      
  }
}

/**
  * @brief ��ʾһ��ֱ��
  * @param Xpos: ֱ������x����
  * @param Ypos: ֱ������y����
  * @param Length: ֱ�ߵĳ���
  * @param Direction: ֱ�ߵķ��򣬿�����
      @arg LINE_DIR_HORIZONTAL(ˮƽ����) 
      @arg LINE_DIR_VERTICAL(��ֱ����).
  * @note ��ʹ��LCD_SetBackColor��LCD_SetTextColor��LCD_SetColors����������ɫ
  * @retval None
  */
void LCD_DrawLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length, bool Direction)
{
  uint16_t index; 
  
  uint16_t realLength;
  
  /* ָ��ֱ�ߵ�һ�����ص���Դ�λ�� */
  pixel_t *pLineImage = (pixel_t*)(CurrentFrameBuffer + LCD_BPP*(Xpos + (LCD_PIXEL_WIDTH*Ypos)));

  if(Direction == LINE_DIR_HORIZONTAL)
  {
    realLength = LCD_PIXEL_WIDTH-Xpos-Length > 0 ? Length : LCD_PIXEL_WIDTH - Xpos;
    
    /* ����ÿһ�� */
    for ( index = 0; index < realLength; index++ ) 
    {
        *pLineImage = CurrentTextColor;
        
        /* ָ����һ�����ص���Դ�λ�� */
        pLineImage++;
    }
  }
  else
  {
    realLength = LCD_PIXEL_HEIGHT-Ypos-Length > 0 ? Length : LCD_PIXEL_HEIGHT - Ypos;
    
    /* ����ÿһ�� */
    for ( index = 0; index < realLength; index++ ) 
    {
        *pLineImage = CurrentTextColor;
        
        /* ָ����һ�����ص���Դ�λ�� */
        pLineImage += LCD_PIXEL_WIDTH;
    }
  }   
}

/**
 * @brief  ��Һ������ʹ�� Bresenham �㷨���߶Σ��������㣩 
 * @param  Xpos1 ���߶ε�һ���˵�X����
 * @param  Ypos1 ���߶ε�һ���˵�Y����
 * @param  Xpos2 ���߶ε���һ���˵�X����
 * @param  Ypos2 ���߶ε���һ���˵�Y����
 * @note ��ʹ��LCD_SetBackColor��LCD_SetTextColor��LCD_SetColors����������ɫ
 * @retval ��
 */
void LCD_DrawUniLine ( uint16_t Xpos1, uint16_t Ypos1, uint16_t Xpos2, uint16_t Ypos2 )
{
	uint16_t us; 
	uint16_t usX_Current, usY_Current;
	
	int32_t lError_X = 0, lError_Y = 0, lDelta_X, lDelta_Y, lDistance; 
	int32_t lIncrease_X, lIncrease_Y; 	
	
	
	lDelta_X = Xpos2 - Xpos1; //������������ 
	lDelta_Y = Ypos2 - Ypos1; 
	
	usX_Current = Xpos1; 
	usY_Current = Ypos1; 
	
	
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
		PutPixel ( usX_Current, usY_Current );//���� 
		
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
  * @brief  ���ƿ��ľ���
  * @param  Xpos ���������ϽǶ˵�X����
  * @param  Ypos ���������ϽǶ˵�Y����
  * @param  Width �����ο�
  * @param  Height �����θ�
  * @note ��ʹ��LCD_SetBackColor��LCD_SetTextColor��LCD_SetColors����������ɫ
  * @retval ��
  */
void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{  
  uint16_t realHeight,realWidth;
  
  realHeight = LCD_PIXEL_HEIGHT-Ypos-Height > 0 ? Height : LCD_PIXEL_HEIGHT - Ypos;
  realWidth = LCD_PIXEL_WIDTH-Xpos-Width > 0 ? Width : LCD_PIXEL_WIDTH - Xpos;
  
  LCD_DrawLine(Xpos, Ypos, realWidth, LINE_DIR_HORIZONTAL);
  LCD_DrawLine(Xpos, Ypos, realHeight, LINE_DIR_VERTICAL);
  LCD_DrawLine(Xpos + realWidth - 1, Ypos, realHeight, LINE_DIR_VERTICAL);
  LCD_DrawLine(Xpos, Ypos + realHeight - 1, realWidth, LINE_DIR_HORIZONTAL);
}

/**
  * @brief  ����ʵ�ľ���
  * @param  Xpos ���������ϽǶ˵�X����
  * @param  Ypos ���������ϽǶ˵�Y����
  * @param  Width �����ο�
  * @param  Height �����θ�
  * @note ��ʹ��LCD_SetBackColor��LCD_SetTextColor��LCD_SetColors����������ɫ
  * @retval ��
  */
void LCD_DrawFullRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
  uint16_t page, column; 
  
  uint16_t realHeight,realWidth;
  
  realHeight = LCD_PIXEL_HEIGHT-Ypos-Height > 0 ? Height : LCD_PIXEL_HEIGHT - Ypos;
  realWidth = LCD_PIXEL_WIDTH-Xpos-Width > 0 ? Width : LCD_PIXEL_WIDTH - Xpos;
  
  /* ָ����ε�һ�����ص���Դ�λ�� */
  pixel_t *pRectImage = (pixel_t*)(CurrentFrameBuffer + LCD_BPP*(Xpos + (LCD_PIXEL_WIDTH*Ypos)));
  
  /* ����ÿһ�� */
  for ( page = 0; page < realHeight; page++ )
  {    
    /* ����ÿһ�� */
    for ( column = 0; column < realWidth; column++ ) 
    {	
      *pRectImage = CurrentTextColor;
      
      /* ָ����һ�����ص���Դ�λ�� */
      pRectImage++;
    }      
    /*��ʾ��һ��*/
    /*ָ����һ�еĵ�һ�����ص���Դ�λ��*/
    pRectImage += (LCD_PIXEL_WIDTH - realWidth);		
  }
}

/**
 * @brief  ����һ������Բ
 * @param  Xpos: Բ��X����
 * @param  Ypos: Բ��Y����
 * @param  Radius: �뾶
 * @retval None
 */
void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
   int x = -Radius, y = 0, err = 2-2*Radius, e2;
   do {
       *(__IO pixel_t*) (CurrentFrameBuffer + (LCD_BPP*((Xpos-x) + LCD_PIXEL_WIDTH*(Ypos+y)))) = CurrentTextColor;
       *(__IO pixel_t*) (CurrentFrameBuffer + (LCD_BPP*((Xpos+x) + LCD_PIXEL_WIDTH*(Ypos+y)))) = CurrentTextColor;
       *(__IO pixel_t*) (CurrentFrameBuffer + (LCD_BPP*((Xpos+x) + LCD_PIXEL_WIDTH*(Ypos-y)))) = CurrentTextColor;
       *(__IO pixel_t*) (CurrentFrameBuffer + (LCD_BPP*((Xpos-x) + LCD_PIXEL_WIDTH*(Ypos-y)))) = CurrentTextColor;

       e2 = err;
       if (e2 <= y) {
           err += ++y*2+1;
           if (-x == y && e2 <= x) e2 = 0;
       }
       if (e2 > x) err += ++x*2+1;
   }
   while (x <= 0);
}

/**
 * @brief  ����һ��ʵ��Բ
 * @param  Xpos: Բ��X����
 * @param  Ypos: Բ��Y����
 * @param  Radius: �뾶
 * @retval None
 */
void LCD_DrawFullCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{  
  /* ����ʵ��Բ��Ҫ���ӵĲ��� */
   int32_t  D;    /* Decision Variable */
   uint32_t  CurX;/* Current X Value */
   uint32_t  CurY;/* Current Y Value */

   D = 3 - (Radius << 1);

   CurX = 0;
   CurY = Radius;

   while (CurX <= CurY)
   {
     if(CurY > 0)
     {
       LCD_DrawLine(Xpos - CurX, Ypos - CurY, 2*CurY, LINE_DIR_VERTICAL);
       LCD_DrawLine(Xpos + CurX, Ypos - CurY, 2*CurY, LINE_DIR_VERTICAL);
     }

     if(CurX > 0)
     {
       LCD_DrawLine(Xpos - CurY, Ypos - CurX, 2*CurX, LINE_DIR_VERTICAL);
       LCD_DrawLine(Xpos + CurY, Ypos - CurX, 2*CurX, LINE_DIR_VERTICAL);
     }
     if (D < 0)
     {
       D += (CurX << 2) + 6;
     }
     else
     {
       D += ((CurX - CurY) << 2) + 10;
       CurY--;
     }
     CurX++; 
    }
 
   LCD_DrawCircle(Xpos, Ypos, Radius);

}
/*********************************************END OF FILE**********************/
