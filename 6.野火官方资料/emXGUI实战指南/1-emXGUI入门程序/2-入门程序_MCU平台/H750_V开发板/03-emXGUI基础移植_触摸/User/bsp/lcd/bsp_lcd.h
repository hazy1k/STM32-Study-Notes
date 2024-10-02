#ifndef __BSP_LCD_H
#define	__BSP_LCD_H

#include "stm32h7xx.h"
/** @addtogroup Utilities
  * @{
  */

/** 
  * @brief  LCDҺ������
  */   
typedef struct
{
  /*����Һ�������ֲ�Ĳ�������*/
  uint8_t hbp;  //HSYNC�����Ч����
  uint8_t vbp;  //VSYNC�����Ч����

  uint8_t hsw;  	//HSYNC���
  uint8_t vsw;   //VSYNC���

  uint8_t hfp;  	//HSYNCǰ����Ч����
  uint8_t vfp;  	//VSYNCǰ����Ч����
  
  uint8_t comment_clock_2byte; //rgb565/argb4444��˫�ֽ�����ʱ�Ƽ�ʹ�õ�Һ��ʱ��Ƶ��
  uint8_t comment_clock_4byte; //Argb8888�����ֽ�����ʱ�Ƽ�ʹ�õ�Һ��ʱ��Ƶ��

  uint16_t lcd_pixel_width; //Һ���ֱ��ʣ���
  uint16_t lcd_pixel_height;//Һ���ֱ��ʣ���

}LCD_PARAM_TypeDef;


/** 
  * @brief  LCDҺ������
  */   
typedef enum
{ 
  INCH_5  = 0x00, /* Ұ��5���� */
  INCH_7,     /* Ұ��7���� */
  INCH_4_3,  /* Ұ��4.3���� */
  
  LCD_TYPE_NUM /* LCD��������*/
}LCD_TypeDef;

/* ��ǰʹ�õ�LCD��Ĭ��Ϊ5���� */
extern LCD_TypeDef cur_lcd;
/* ��ͬҺ�����Ĳ��� */
extern const LCD_PARAM_TypeDef lcd_param[];
/**
  * @brief  Initializes the LCD.
  * @param  None
  * @retval None
  */
/*����Һ�������ֲ�Ĳ�������*/
#define HBP  lcd_param[cur_lcd].hbp		//HSYNC�����Ч����
#define VBP  lcd_param[cur_lcd].vbp		//VSYNC�����Ч����

#define HSW  lcd_param[cur_lcd].hsw		//HSYNC���
#define VSW  lcd_param[cur_lcd].vsw		//VSYNC���

#define HFP  lcd_param[cur_lcd].hfp		//HSYNCǰ����Ч����
#define VFP  lcd_param[cur_lcd].vfp		//VSYNCǰ����Ч����


/* LCD Size (Width and Height) */
#define  LCD_PIXEL_WIDTH          lcd_param[cur_lcd].lcd_pixel_width
#define  LCD_PIXEL_HEIGHT         lcd_param[cur_lcd].lcd_pixel_height

/* ʹ�õĸ�����Ļ�����ķֱ��� */
#define  LCD_MAX_PIXEL_WIDTH    ((uint16_t)800)  
#define  LCD_MAX_PIXEL_HEIGHT   ((uint16_t)480)  

#define  LCD_BUFFER         ((uint32_t)0xD0000000)



//��ɫ������
#define LTDC_R0_GPIO_PORT        	GPIOI
#define LTDC_R0_GPIO_CLK_ENABLE()   __GPIOI_CLK_ENABLE()
#define LTDC_R0_GPIO_PIN         	GPIO_PIN_15
#define LTDC_R0_AF			        	GPIO_AF14_LTDC

#define LTDC_R1_GPIO_PORT        	GPIOJ
#define LTDC_R1_GPIO_CLK_ENABLE()   __GPIOJ_CLK_ENABLE()
#define LTDC_R1_GPIO_PIN         	GPIO_PIN_0
#define LTDC_R1_AF			        	GPIO_AF14_LTDC

#define LTDC_R2_GPIO_PORT        	GPIOJ
#define LTDC_R2_GPIO_CLK_ENABLE()	__GPIOJ_CLK_ENABLE()
#define LTDC_R2_GPIO_PIN         	GPIO_PIN_1
#define LTDC_R2_AF			        	GPIO_AF14_LTDC

#define LTDC_R3_GPIO_PORT        	GPIOJ
#define LTDC_R3_GPIO_CLK_ENABLE()	__GPIOJ_CLK_ENABLE()
#define LTDC_R3_GPIO_PIN         	GPIO_PIN_2
#define LTDC_R3_AF			        	GPIO_AF14_LTDC

#define LTDC_R4_GPIO_PORT        	GPIOJ
#define LTDC_R4_GPIO_CLK_ENABLE()	__GPIOJ_CLK_ENABLE()
#define LTDC_R4_GPIO_PIN         	GPIO_PIN_3
#define LTDC_R4_AF			        	GPIO_AF14_LTDC

#define LTDC_R5_GPIO_PORT        	GPIOJ
#define LTDC_R5_GPIO_CLK_ENABLE()	__GPIOJ_CLK_ENABLE()
#define LTDC_R5_GPIO_PIN         	GPIO_PIN_4
#define LTDC_R5_AF			        	GPIO_AF14_LTDC

#define LTDC_R6_GPIO_PORT        	GPIOJ
#define LTDC_R6_GPIO_CLK_ENABLE()	__GPIOJ_CLK_ENABLE()
#define LTDC_R6_GPIO_PIN         	GPIO_PIN_5
#define LTDC_R6_AF			        	GPIO_AF14_LTDC

#define LTDC_R7_GPIO_PORT        	GPIOJ
#define LTDC_R7_GPIO_CLK_ENABLE()	__GPIOJ_CLK_ENABLE()
#define LTDC_R7_GPIO_PIN         	GPIO_PIN_6
#define LTDC_R7_AF			        	GPIO_AF14_LTDC
//��ɫ������
#define LTDC_G0_GPIO_PORT        	GPIOJ
#define LTDC_G0_GPIO_CLK_ENABLE()	__GPIOJ_CLK_ENABLE()
#define LTDC_G0_GPIO_PIN         	GPIO_PIN_7
#define LTDC_G0_AF			        	GPIO_AF14_LTDC

#define LTDC_G1_GPIO_PORT        	GPIOJ
#define LTDC_G1_GPIO_CLK_ENABLE()	__GPIOJ_CLK_ENABLE()
#define LTDC_G1_GPIO_PIN         	GPIO_PIN_8
#define LTDC_G1_AF			        	GPIO_AF14_LTDC

#define LTDC_G2_GPIO_PORT        	GPIOJ
#define LTDC_G2_GPIO_CLK_ENABLE() 	__GPIOJ_CLK_ENABLE()
#define LTDC_G2_GPIO_PIN         	GPIO_PIN_9
#define LTDC_G2_AF			        	GPIO_AF14_LTDC

#define LTDC_G3_GPIO_PORT        	GPIOJ
#define LTDC_G3_GPIO_CLK_ENABLE()	__GPIOJ_CLK_ENABLE()
#define LTDC_G3_GPIO_PIN         	GPIO_PIN_10
#define LTDC_G3_AF			        	GPIO_AF14_LTDC

#define LTDC_G4_GPIO_PORT        	GPIOJ
#define LTDC_G4_GPIO_CLK_ENABLE()	__GPIOJ_CLK_ENABLE()
#define LTDC_G4_GPIO_PIN         	GPIO_PIN_11
#define LTDC_G4_AF			        	GPIO_AF14_LTDC

#define LTDC_G5_GPIO_PORT        	GPIOK
#define LTDC_G5_GPIO_CLK_ENABLE()	__GPIOK_CLK_ENABLE()
#define LTDC_G5_GPIO_PIN         	GPIO_PIN_0
#define LTDC_G5_AF			        	GPIO_AF14_LTDC

#define LTDC_G6_GPIO_PORT        	GPIOK
#define LTDC_G6_GPIO_CLK_ENABLE()	__GPIOK_CLK_ENABLE()
#define LTDC_G6_GPIO_PIN         	GPIO_PIN_1
#define LTDC_G6_AF			        	GPIO_AF14_LTDC

#define LTDC_G7_GPIO_PORT        	GPIOK
#define LTDC_G7_GPIO_CLK_ENABLE() 	__GPIOK_CLK_ENABLE()
#define LTDC_G7_GPIO_PIN         	GPIO_PIN_2
#define LTDC_G7_AF			        	GPIO_AF14_LTDC

//��ɫ������
#define LTDC_B0_GPIO_PORT        	GPIOJ
#define LTDC_B0_GPIO_CLK_ENABLE()  	__GPIOJ_CLK_ENABLE()
#define LTDC_B0_GPIO_PIN         	GPIO_PIN_12
#define LTDC_B0_AF			        	GPIO_AF14_LTDC

#define LTDC_B1_GPIO_PORT        	GPIOJ
#define LTDC_B1_GPIO_CLK_ENABLE()	__GPIOJ_CLK_ENABLE()
#define LTDC_B1_GPIO_PIN         	GPIO_PIN_13
#define LTDC_B1_AF			        	GPIO_AF14_LTDC

#define LTDC_B2_GPIO_PORT        	GPIOJ
#define LTDC_B2_GPIO_CLK_ENABLE()	__GPIOJ_CLK_ENABLE()
#define LTDC_B2_GPIO_PIN         	GPIO_PIN_14
#define LTDC_B2_AF			        	GPIO_AF14_LTDC

#define LTDC_B3_GPIO_PORT        	GPIOJ
#define LTDC_B3_GPIO_CLK_ENABLE() 	__GPIOJ_CLK_ENABLE()
#define LTDC_B3_GPIO_PIN         	GPIO_PIN_15
#define LTDC_B3_AF			        	GPIO_AF14_LTDC

#define LTDC_B4_GPIO_PORT        	GPIOK
#define LTDC_B4_GPIO_CLK_ENABLE() 	__GPIOK_CLK_ENABLE()
#define LTDC_B4_GPIO_PIN         	GPIO_PIN_3
#define LTDC_B4_AF			        	GPIO_AF14_LTDC

#define LTDC_B5_GPIO_PORT        	GPIOK
#define LTDC_B5_GPIO_CLK_ENABLE()	__GPIOK_CLK_ENABLE()
#define LTDC_B5_GPIO_PIN         	GPIO_PIN_4
#define LTDC_B5_AF			        	GPIO_AF14_LTDC

#define LTDC_B6_GPIO_PORT        	GPIOK
#define LTDC_B6_GPIO_CLK_ENABLE() 	__GPIOK_CLK_ENABLE()
#define LTDC_B6_GPIO_PIN         	GPIO_PIN_5
#define LTDC_B6_AF			        	GPIO_AF14_LTDC

#define LTDC_B7_GPIO_PORT        	GPIOK
#define LTDC_B7_GPIO_CLK_ENABLE()	__GPIOK_CLK_ENABLE()
#define LTDC_B7_GPIO_PIN         	GPIO_PIN_6
#define LTDC_B7_AF			        	GPIO_AF14_LTDC

//�����ź���
/*����ʱ��CLK*/
#define LTDC_CLK_GPIO_PORT              GPIOI
#define LTDC_CLK_GPIO_CLK_ENABLE()      __GPIOI_CLK_ENABLE()
#define LTDC_CLK_GPIO_PIN               GPIO_PIN_14
#define LTDC_CLK_AF			            		GPIO_AF14_LTDC
/*ˮƽͬ���ź�HSYNC*/
#define LTDC_HSYNC_GPIO_PORT            GPIOI
#define LTDC_HSYNC_GPIO_CLK_ENABLE()    __GPIOI_CLK_ENABLE()
#define LTDC_HSYNC_GPIO_PIN             GPIO_PIN_12
#define LTDC_HSYNC_AF			       				GPIO_AF14_LTDC
/*��ֱͬ���ź�VSYNC*/
#define LTDC_VSYNC_GPIO_PORT            GPIOI
#define LTDC_VSYNC_GPIO_CLK_ENABLE()    __GPIOI_CLK_ENABLE()
#define LTDC_VSYNC_GPIO_PIN             GPIO_PIN_13
#define LTDC_VSYNC_AF			        			GPIO_AF14_LTDC

/*����ʹ���ź�DE*/
#define LTDC_DE_GPIO_PORT               GPIOK
#define LTDC_DE_GPIO_CLK_ENABLE()       __GPIOK_CLK_ENABLE()
#define LTDC_DE_GPIO_PIN                GPIO_PIN_7
#define LTDC_DE_AF			            		GPIO_AF14_LTDC

/*Һ���������źţ��ߵ�ƽʹ��*/
#define LTDC_BL_GPIO_PORT               GPIOD
#define LTDC_BL_GPIO_CLK_ENABLE()       __GPIOD_CLK_ENABLE()
#define LTDC_BL_GPIO_PIN                GPIO_PIN_13

void LCD_Init(uint32_t fb_addr, int lcd_clk_mhz, uint32_t pixel_format );
void LCD_BackLed_Control ( int on );

#endif /* __BSP_LCD_H */
