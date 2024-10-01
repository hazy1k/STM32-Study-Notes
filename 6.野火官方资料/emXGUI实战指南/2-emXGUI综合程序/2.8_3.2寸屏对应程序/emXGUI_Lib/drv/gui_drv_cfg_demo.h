/**
  *********************************************************************
  * @file    gui_drv_cfg.h
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   emXGUI���������ļ�
  *          ������ʾ���������豸���洢�������塢��Դ�豸������
  *********************************************************************
  * @attention
  * ����    :www.emXGUI.com
  *
  **********************************************************************
  */ 
#ifndef	__GUI_DRV_CFG_H__
#define	__GUI_DRV_CFG_H__


/*===========��ʾ������===gui_lcd_port.c===============================================*/
//Ұ��5.0 / 7.0TFT,800x480

/* �Դ����ַ */
#define  LCD_FRAME_BUFFER   LCD_BUFFER

/* ʹ�õ���ʾ��ʽ */
#define	LCD_FORMAT	  COLOR_FORMAT_RGB565
//#define	LCD_FORMAT	  COLOR_FORMAT_XRGB8888

/* Һ����� */
#define	LCD_XSIZE	    LCD_PIXEL_WIDTH
#define	LCD_YSIZE	    LCD_PIXEL_HEIGHT

//��Ļ��ת��Ĭ��
#define	LCD_ROTATE      ROTATE_0

/* �Ƿ�ʹ��Ӳ��ͼ�μ�������DMA2D֮�࣬������Ҫ���¶��ƣ� */
#define	DMA2D_EN                   0

/* �Ƿ�ʹ�û����Ϳؼ���ʹ�õĻ�������һ��Һ���������ݵ����ģ���ʾЧ��������˸*/
#define  FRAME_BUFFER_EN            0

/*==========�����豸����===gui_input_port.c==================================================*/
/* �Ƿ�ʹ�������豸 */
#define  GUI_INPUT_DEV_EN        0

/* �Ƿ�ʹ�ܼ��̡���ꡢ������ */
#define	GUI_TOUCHSCREEN_EN      1
#define	GUI_KEYBOARD_EN          0
#define	GUI_MOUSE_EN             0


/* �Ƿ���Ҫ����У׼-����������Ҫ */
#define	GUI_TOUCHSCREEN_CALIBRATE       0

/*===========�洢������===gui_mem_port.c===============================================*/
/* ͨ��GUI�ں�ʹ��MEM�ڴ�ѣ�VMEM�ڴ�Ѹ��û�ʹ�� */

/* GUI�ں�ʹ�õĴ洢���Ļ���ַ */  
//#define	GUI_CORE_MEM_BASE	  0xD0100000  //������ʹ��RTT����ʹ���ڲ�sram����ָ����ַ

/* GUI�ں�ʹ�õĴ洢����С���Ƽ���СֵΪ8KB */
#define  GUI_CORE_MEM_SIZE  (32*1024) //������ʹ��ϵͳ������rttϵͳ��board.cʵ�֣�freertos��heap_4.cʵ��
/* ��С�������ȣ���λΪ�ֽ�*/  
#define	GUI_CORE_MEM_ALLOC_UNIT   (64)         


/* ����vmem�Ļ���ַ����С�Լ��������� */
/* �Ƿ�ʹ��VMEM�ڴ�� */
#define  GUI_VMEM_EN      1

/* Һ�������Դ�ʹ�õĿռ� */
#if (LCD_FORMAT == COLOR_FORMAT_RGB565)

  #define LCD_FRAME_SIZE  (LCD_XSIZE*LCD_YSIZE*2)

#elif (LCD_FORMAT == COLOR_FORMAT_XRGB8888)

  #define LCD_FRAME_SIZE (LCD_XSIZE*LCD_YSIZE*4)

#endif
/*
 * SDRAM ����ַ��0xD0000000
 * ǰ LCD_FRAME_SIZE ��С�Ŀռ���ΪҺ�������Դ�
 * ʣ��Ŀռ���ΪVMEM��̬����ʹ��
*/
/* �ڴ�ѵĻ���ַ������Ϊ�ڲ�SRAM��������SDRAM�� */  
#define	VMEM_BASE	        (0xD0000000 + LCD_FRAME_SIZE)
/* �ڴ�ѵ��ܴ�С����λΪ�ֽ� */ 
#define	VMEM_SIZE	        ((8*1024*1024) - LCD_FRAME_SIZE)     
/* ��С�������ȣ���λΪ�ֽ�*/  
#define	VMEM_ALLOC_UNIT   (64)         //64�ֽ�   

/*===========��������===gui_font_port.c===============================================*/
/* Ĭ������ʹ��XFT������������겻������ */
#define	GUI_FONT_XFT_EN		      1
/* �Ƿ�֧��TTF���壬����������겻������ */
#define	GUI_FONT_TTF_EN		      0

/* �Ƿ�ʹ���ⲿFLASH�е�����
*  ���豸��������ط�ʽ��Ҫ�����������Ϊ1
*/
#define GUI_USE_EXTERN_FONT       0

/*
* �Ƿ�������ⲿ�������ݼ�����VMEM���򣬳�ʼ������ʱ��Ҫ�ϳ�ʱ�䣬
* ���غ�ɴ������ַ���ʾ���ٶ�
* ������Ϊ�棬��ʹ��������ط�ʽ������ʹ�����豸��ʽ
*/
#define GUI_FONT_LOAD_TO_RAM    (0 && GUI_USE_EXTERN_FONT)

/* Ҫʹ�õ��ⲿĬ�������ļ���USE_EXTERN_FONTΪ1ʱ��Ч */
#define GUI_DEFAULT_EXTERN_FONT   "GB2312_24_4BPP.xft"

/* Ĭ���ڲ�������������USE_EXTERN_FONTΪ0�� �ⲿ�������ʧ��ʱ����õ����� */
#define GUI_DEFAULT_FONT          ASCII_24_4BPP



/*===========��־����豸����===gui_log_port.c===============================================*/

/* �Ƿ������������������������뺯����������� */
#define GUI_DEBUG_EN         	      1
#define GUI_DEBUG_ARRAY_EN          0
#define GUI_DEBUG_FUNC_EN   	       0



/*===========��Դ�豸����===gui_resource_port.c===============================================*/
/* �Ƿ�ʹ����Դ�豸 */
#define GUI_RES_DEV_EN         0

/* �Ƿ�֧���ļ�ϵͳ�ӿ�,��Ҫ��ֲfatfs�ļ�ϵͳ */
#define GUI_RES_FS_EN         0

/* ��Դ���ڵĻ���ַ */
#define GUI_RES_BASE             4096

/* �洢��FLASH�е���ԴĿ¼��С */
#define GUI_CATALOG_SIZE         (8*1024)

/*===========ͼƬ�ӿ�����===gui_picture_port.c===============================================*/
/* �Ƿ�֧���ļ�ϵͳͼƬ�ӿ�,��Ҫ��ֲfatfs�ļ�ϵͳ */
#define GUI_PIC_FS_EN         0

/* �Ƿ�֧����ʾJPEGͼƬ,��Ҫ���jpeg����� */
#define GUI_PIC_JPEG_EN       0

/* �Ƿ�֧����ʾJPEGͼƬ,��Ҫ���png����� */
#define GUI_PIC_PNG_EN       0

/* ��ͼ */
#define GUI_PIC_CAPTURE_SCREEN_EN  ( 1 && GUI_PIC_FS_EN)

/*============================================================================*/

#endif	/*__GUI_DRV_CFG_H__*/
