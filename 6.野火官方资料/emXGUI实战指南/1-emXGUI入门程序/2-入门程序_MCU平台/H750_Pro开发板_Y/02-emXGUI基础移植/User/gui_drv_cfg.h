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

#include "board.h"

/*===========��ʾ������===gui_lcd_port.c===============================================*/
//Ұ��5.0 / 7.0TFT,800x480 ,4.3��480*272

/* �Դ����ַ */
#define  LCD_FRAME_BUFFER   LCD_BUFFER

/* ʹ�õ���ʾ��ʽ */
#define	LCD_FORMAT	  COLOR_FORMAT_RGB565
//#define	LCD_FORMAT	  COLOR_FORMAT_XRGB8888

/* ��ǰʹ��Һ���Ŀ�� */
#define	LCD_XSIZE	    LCD_PIXEL_WIDTH
#define	LCD_YSIZE	    LCD_PIXEL_HEIGHT

/* ʹ�����Һ�����Ŀ�ߣ����ڼ����Դ�ռ� */
#define	LCD_MAX_XSIZE	    LCD_MAX_PIXEL_WIDTH
#define	LCD_MAX_YSIZE	    LCD_MAX_PIXEL_HEIGHT

/* ʹ�õ�LCD������������֧�ֲ�ͬ�ķֱ��ʣ�ʹ�ò�ͬ������ */
#define  GUI_LCD_TYPE_NUM     LCD_TYPE_NUM

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

/* �Ƿ���ʾ��� */
#define  GUI_SHOW_CURSOR_EN        0

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

  #define LCD_FRAME_SIZE  (LCD_MAX_XSIZE*LCD_MAX_YSIZE*2)

#elif (LCD_FORMAT == COLOR_FORMAT_XRGB8888)

  #define LCD_FRAME_SIZE (LCD_MAX_XSIZE*LCD_MAX_YSIZE*4)

#endif
/*
 * SDRAM ����ַ��0xD0000000
 * ǰ LCD_FRAME_SIZE ��С�Ŀռ���ΪҺ�������Դ�
 * ʣ��Ŀռ���ΪVMEM��̬����ʹ��
*/
/* �ڴ�ѵĻ���ַ������Ϊ�ڲ�SRAM��������SDRAM�� */  
#define	VMEM_BASE	        (0xD0000000 + LCD_FRAME_SIZE)
/* �ڴ�ѵ��ܴ�С����λΪ�ֽ� */ 
#define	VMEM_SIZE	        ((64*1024*1024) - LCD_FRAME_SIZE)     
/* ��С�������ȣ���λΪ�ֽ�*/  
#define	VMEM_ALLOC_UNIT   (64)         //64�ֽ�   

/*===========��������===gui_font_port.c===============================================*/
/* Ĭ������ʹ��XFT������������겻������ */
#define	GUI_FONT_XFT_EN		      1
/* �Ƿ�֧��TTF���壬����������겻������ */
#define	GUI_FONT_TTF_EN		      0


/* Ĭ���ڲ�Ӣ��������������USE_EXTERN_FONTΪ0�� �ⲿ�������ʧ��ʱ����õ����� */
#define GUI_DEFAULT_FONT_EN          gui_font_param[cur_lcd].default_en

/* �Ƿ�ʹ���ڲ��������壬��������̫�󣬲���������ڲ� */
#define GUI_INER_CN_FONT_EN      0
/* Ĭ���ڲ��������������� */
#define GUI_DEFAULT_FONT_CN          GB2312_16_2BPP


/* �Ƿ�ʹ���ⲿFLASH�е�����
*  ���豸��������ط�ʽ��Ҫ�����������Ϊ1
*  ʹ�ñ�����ʱ��Ҫ��GUI_RES_DEV_EN����Ϊ1
*/
#define GUI_EXTERN_FONT_EN       0

/*
* �Ƿ�������ⲿ�������ݼ�����VMEM���򣬳�ʼ������ʱ��Ҫ�ϳ�ʱ�䣬
* ���غ�ɴ������ַ���ʾ���ٶ�
* ������Ϊ�棬��ʹ��������ط�ʽ������ʹ�����豸��ʽ
*/
#define GUI_FONT_LOAD_TO_RAM_EN    (1 && GUI_EXTERN_FONT_EN)

/* Ҫʹ�õ��ⲿĬ�������ļ���USE_EXTERN_FONTΪ1ʱ��Ч */
#define GUI_DEFAULT_EXTERN_FONT   gui_font_param[cur_lcd].default_extern_cn


/*===========��־����豸����===gui_log_port.c===============================================*/

/* �Ƿ������������������������뺯����������� */
#define GUI_DEBUG_EN         	      1
#define GUI_DEBUG_ARRAY_EN          0
#define GUI_DEBUG_FUNC_EN   	       0



/*===========��Դ�豸����===gui_resource_port.c===============================================*/
/* �Ƿ�ʹ����Դ�豸 */
#define GUI_RES_DEV_EN         0

/* �Ƿ�֧���ļ�ϵͳ�ӿ�,��Ҫ��ֲfatfs�ļ�ϵͳ */
#define GUI_FS_EN         0

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

/*===========�Ƿ����ø���APP===============================================*/

/* �������� */
#define GUI_APP_BOOT_INTERFACE_EN     0

/* ��Դ��¼����ʹ�ܺ�ȱ����Դ���Զ�������Դ��¼���� */
/* ��Ҫsd�ļ�ϵͳ/flash��Դ�豸֧�� */
#define GUI_APP_RES_WRITER_EN          0
/*============================================================================*/

#endif	/*__GUI_DRV_CFG_H__*/
