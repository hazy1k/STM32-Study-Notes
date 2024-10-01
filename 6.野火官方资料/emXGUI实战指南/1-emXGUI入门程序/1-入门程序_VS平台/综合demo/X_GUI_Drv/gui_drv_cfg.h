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
//#define  LCD_FRAME_BUFFER   LCD_BUFFER
#define  LCD_FRAME_BUFFER   GetFrameBuffer()

/* ʹ�õ���ʾ��ʽ */
/* ע��VS�����£��л���ͬ��ʽʱ��
*  Ҫ�޸� X_GUI_SIM.CPP��WinMain��LCD_BPP��ֵ*/
//#define	LCD_FORMAT	  COLOR_FORMAT_RGB565
#define	LCD_FORMAT	  COLOR_FORMAT_XRGB8888

/* Һ����� */
#define	LCD_XSIZE	    800
#define	LCD_YSIZE	    480

//��Ļ��ת��Ĭ��
#define	LCD_ROTATE      ROTATE_0

/* �Ƿ�ʹ��Ӳ��ͼ�μ�������DMA2D֮�࣬������Ҫ���¶��ƣ� */
#define	DMA2D_EN                   0

/* �Ƿ�ʹ�û����Ϳؼ���ʹ�õĻ�������һ��Һ���������ݵ����ģ���ʾЧ��������˸*/
#define  FRAME_BUFFER_EN            1

/*==========�����豸����===gui_input_port.c==================================================*/
/* �Ƿ�ʹ�������豸 */
#define  GUI_INPUT_DEV_EN        1

/* �Ƿ�ʹ�ܼ��̡���ꡢ������ */
#define	GUI_TOUCHSCREEN_EN      0
#define	GUI_KEYBOARD_EN          0
#define	GUI_MOUSE_EN             1


/* �Ƿ���Ҫ����У׼-����������Ҫ */
#define	GUI_TOUCHSCREEN_CALIBRATE       0

/*===========�洢������===gui_mem_port.c===============================================*/
/* ͨ��GUI�ں�ʹ��MEM�ڴ�ѣ�VMEM�ڴ�Ѹ��û�ʹ�� */

/* GUI�ں�ʹ�õĴ洢���Ļ���ַ */  
//#define	GUI_CORE_MEM_BASE	  0xD0100000  //������ʹ��RTT����ʹ���ڲ�sram����ָ����ַ

/* GUI�ں�ʹ�õĴ洢����С���Ƽ���СֵΪ8KB */
//#define  GUI_CORE_MEM_SIZE  (64*1024) //������ʹ��RTT������board.cʵ��
///* ��С�������ȣ���λΪ�ֽ�*/  
//#define	GUI_CORE_MEM_ALLOC_UNIT   (64)         


/* ����vmem�Ļ���ַ����С�Լ��������� */
/* �Ƿ�ʹ��VMEM�ڴ�� */
#define  GUI_VMEM_EN      1
/* �ڴ�ѵĻ���ַ������Ϊ�ڲ�SRAM��������SDRAM�� */  
//#define	VMEM_BASE	        0xD0200000  // ��SDRAMǰ2MB��LCD��������Ϊ�Դ��� 
/* �ڴ�ѵ��ܴ�С����λΪ�ֽ� */ 
#define	VMEM_SIZE	        (64<<20)     // 64MB 
/* ��С�������ȣ���λΪ�ֽ�*/  
#define	VMEM_ALLOC_UNIT   (32)         //32�ֽ�   

/*===========�洢������===GUI_Arch.c===============================================*/
/** 
 �ھ�̬�ڴ�������RECT_LL ��������
 ʹ�þ�̬�ڴ�������Լ����ٵķ��䣬������ϵͳ����
 ��������̬ RECT_LL �������꣬�ͻ���� GUI_MEM_Alloc��
 �Զ�̬�ڴ淽ʽ���� RECT_LL 
 �ⲿ����GUI_RectLL_Alloc����ʵ��

 ����û����ڲ��࣬����ϵͳ��̬ RAM�ֱȽ���ʱ��
 �Ϳ��԰��������Сһ��
 
 һ��RECT_LLԪ��20�ֽڣ��ܵľ�̬�ռ�ռ��Ϊ 20*RECT_LL_SLOT_NUM ���ֽ�
*/
#define	RECT_LL_SLOT_NUM	        512

/*===========��������===gui_font_port.c===============================================*/
/* Ĭ������ʹ��XFT������������겻������ */
#define	GUI_FONT_XFT_EN		      1
/* �Ƿ�֧��TTF���壬����������겻������ */
#define	GUI_FONT_TTF_EN		      0

/* �Ƿ�֧��ICON LOGO���壬ʹ��ʱ��Ҫ�����Ӧ�����������ļ� */
#define  GUI_ICON_LOGO_EN         1
/* LOGOʹ�õ����� */
#define  GUI_LOGO_FONT          LOGO_50_4BPP
/* ICONʹ�õ����� */
#define   GUI_ICON_FONT         ICON_100_4BPP
/* ����ICONʹ�õ����� */
#define   GUI_CONTROL_FONT      CONTROL_60_8BPP



/* �Ƿ�ʹ���ⲿFLASH�е�����
*  ���豸��������ط�ʽ��Ҫ�����������Ϊ1
*/
#define GUI_USE_EXTERN_FONT       1

/*
* �Ƿ�������ⲿ�������ݼ�����VMEM���򣬳�ʼ������ʱ��Ҫ�ϳ�ʱ�䣬
* ���غ�ɴ������ַ���ʾ���ٶ�
* ������Ϊ�棬��ʹ��������ط�ʽ������ʹ�����豸��ʽ
*/
#define GUI_FONT_LOAD_TO_RAM    (1 && GUI_USE_EXTERN_FONT)

/* Ҫʹ�õ��ⲿĬ�������ļ���USE_EXTERN_FONTΪ1ʱ��Ч */
#define GUI_DEFAULT_EXTERN_FONT   "GB2312_24_4BPP.xft"

/* Ĭ���ڲ�������������USE_EXTERN_FONTΪ0�� �ⲿ�������ʧ��ʱ����õ����� */
#define GUI_DEFAULT_FONT          ASCII_20_4BPP



/*===========��־����豸����===gui_log_port.c===============================================*/

/* �Ƿ������������������������뺯����������� */
#define GUI_DEBUG_EN         	      1
#define GUI_DEBUG_ARRAY_EN          0
#define GUI_DEBUG_FUNC_EN   	       0



/*===========��Դ�豸����===gui_resource_port.c===============================================*/
/* �Ƿ�ʹ����Դ�豸 */
#define GUI_RES_DEV_EN         1

/* �Ƿ�֧���ļ�ϵͳ�ӿ�,��Ҫ��ֲfatfs�ļ�ϵͳ */
#define GUI_RES_FS_EN         1

/* ��Դ���ڵĻ���ַ */
#define GUI_RES_BASE             4096


/* �洢��FLASH�е���ԴĿ¼��С */
#define GUI_CATALOG_SIZE         4096

/* ��Դ��Ĭ��·�� */
#define RES_DIR					"../GUI_App/GUI_DEMO/res/"


/*===========ͼƬ�ӿ�����===gui_picture_port.c===============================================*/
/* �Ƿ�֧���ļ�ϵͳͼƬ�ӿ�,��Ҫ��ֲfatfs�ļ�ϵͳ */
#define GUI_PIC_FS_EN         1

/* �Ƿ�֧����ʾJPEGͼƬ,��Ҫ���jpeg����� */
#define GUI_PIC_JPEG_EN       1

/* �Ƿ�֧����ʾJPEGͼƬ,��Ҫ���png����� */
#define GUI_PIC_PNG_EN       1

/* ��ͼ */
#define GUI_PIC_CAPTURE_SCREEN_EN  ( 0 && GUI_PIC_FS_EN)


/*============================================================================*/

#endif	/*__GUI_DRV_CFG_H__*/
