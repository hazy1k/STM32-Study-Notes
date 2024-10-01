/**
  *********************************************************************
  * @file    gui_font_port.c
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   �������ӿ�
  *********************************************************************
  * @attention
  * ����    :www.emXGUI.com
  *
  **********************************************************************
  */ 

#include	"emXGUI.h"
#include  "gui_drv_cfg.h"

#include "gui_mem_port.h"
#include "gui_font_port.h"
//#include "gui_resource_port.h"

/*===================================================================================*/
/*===================================================================================*/


#include <GUI_Font_XFT.h>

/**
  * @note  ��Ҫ��������뵽�ڲ�FLASH��
  * ����Ӧ�����������ļ���ӵ����̣�Ȼ���ڴ˴���������
  * ��ʹ��XFT_CreateFont��������
  */
extern const char GB2312_16_1BPP[];
extern const char GB2312_16_2BPP[];
extern const char GB2312_20_2BPP[];

extern const char GB2312_16_4BPP[];
extern const char GB2312_20_4BPP[];

extern const char ASCII_16_4BPP[];
extern const char ASCII_20_4BPP[];
extern const char ASCII_24_4BPP[];
extern const char ASCII_32_4BPP[];
extern const char LOGO_50_4BPP[];
extern const char ICON_100_4BPP[];
extern const char CONTROL_50_4BPP[];
extern const char CONTROL_60_8BPP[];
extern const char CONTROL_70_8BPP[];
extern const char CONTROL_80_8BPP[];



/* Ĭ������ */
HFONT defaultFont =NULL;

#if(GUI_ICON_LOGO_EN)  
/* logo���� */
HFONT logoFont =NULL;
/* ͼ������ */
HFONT iconFont =NULL;
/* ����ͼ������ */
HFONT controlFont =NULL;
#endif

/* ���� */
HFONT GB2312_32_Font =NULL;


/*===================================================================================*/
#if (GUI_USE_EXTERN_FONT && (!GUI_FONT_LOAD_TO_RAM))

/**
  * @brief  ����ý��������ݵĻص�����
  * @param  buf[out] �洢��ȡ�������ݻ�����
  * @param  offset Ҫ��ȡ��λ��
  * @param  size Ҫ��ȡ�����ݴ�С
  * @param  lParam ���ú���ʱ���Զ���������û�������
  * @retval ��ȡ�������ݴ�С
  */
static int font_read_data_exFlash(void *buf,int offset,int size,LONG lParam)
{
  /* ��������offset�Ǿ����ַ������������ļ��е�ƫ�� 
   * lParam �������ļ���FLASH�еĻ���ַ
  */
	//offset += lParam;
  
  /* ��ȡ�������ģ�������� */
	RES_DevRead(buf,offset,size);
	return size;
}
#endif

/**
  * @brief  GUIĬ�������ʼ��
  * @param  ��
  * @retval ����Ĭ������ľ��
  */
HFONT GUI_Default_FontInit(void)
{

#if (GUI_FONT_LOAD_TO_RAM  )
  {  
    /* ���������ļ�������RAM */
    
    int font_base;
    
    /* ָ�򻺳�����ָ�� */
    static u8 *pFontData_XFT=NULL;
    CatalogTypeDef dir;
    
    /* RES_GetInfo��ȡ����dir.offset����Դ�ľ��Ե�ַ */
    font_base =RES_GetInfo_AbsAddr(GUI_DEFAULT_EXTERN_FONT, &dir);

    if(font_base > 0)
    {
    	pFontData_XFT =(u8*)GUI_VMEM_Alloc(dir.size);
      if(pFontData_XFT!=NULL)
      {
        RES_DevRead(pFontData_XFT, 0, dir.size);

        defaultFont = XFT_CreateFont(pFontData_XFT);
      }
    }
  }
#elif (GUI_USE_EXTERN_FONT)   
  {
    /* ʹ�����豸�������壬����Ҫ��ȡ */
    if(defaultFont==NULL)
    { 
    	int font_base;
      CatalogTypeDef dir;

    	font_base =RES_GetInfo_AbsAddr(GUI_DEFAULT_EXTERN_FONT, &dir);
    	if(font_base > 0)
    	{
    		defaultFont =XFT_CreateFontEx(font_read_data_exFlash,font_base);
    	}
    }
  }
#endif

    /* ��ǰ����������ʧ�ܣ�ʹ���ڲ�FLASH�е����ݣ������е�C�������飩
    *  �����������ʱ���������ļ���ӵ����̣��ڱ��ļ�ͷ�����Ӧ���������������
    *  Ȼ�����XFT_CreateFont�����������弴��
    */
    if(defaultFont==NULL)
    { 
      /* �ӱ��ؼ���(������������) */    	
      defaultFont =XFT_CreateFont(GUI_DEFAULT_FONT);  /*ASCii�ֿ�,20x20,4BPP�����*/
      
      /* �����ֿ�洢ռ�ÿռ�ǳ��󣬲��Ƽ������ڲ�FLASH */
    	//defaultFont =XFT_CreateFont(GB2312_16_2BPP); /*GB2312�ֿ�,16x16,2BPP�����*/
    	//defaultFont =XFT_CreateFont(GB2312_20_4BPP); /*GB2312�ֿ�,20x20,4BPP�����*/
    }
#if(GUI_ICON_LOGO_EN)    
  /* ����logo���� */  
  logoFont =  XFT_CreateFont(GUI_LOGO_FONT);
  /* ����ͼ������ */  
  iconFont =  XFT_CreateFont(GUI_ICON_FONT);
    
  /* ��������ͼ������ */  
  controlFont =  XFT_CreateFont(GUI_CONTROL_FONT);
    
  if(logoFont==NULL)  
    GUI_ERROR("logoFont create failed");
      
  if(iconFont ==NULL) 
    GUI_ERROR("iconFont create failed");
  
  if(controlFont ==NULL) 
    GUI_ERROR("controlFont create failed");
    
#endif
    
  /* ���� */  
  //{
  //  /* ʹ�����豸�������壬����Ҫ��ȡ */
  //  	int font_base;
  //    CatalogTypeDef dir;

  //  	font_base =RES_GetInfo_AbsAddr("GB2312_32_4BPP.xft", &dir);
  //  	if(font_base > 0)
  //  	{
  //  		GB2312_32_Font =XFT_CreateFontEx(font_read_data_exFlash,font_base);
  //  	}  
  //    
  //    if(GB2312_32_Font ==NULL) 
  //      GUI_ERROR("GB2312_40_Font create failed");
  //}  

	return defaultFont;
}

/********************************END OF FILE****************************/

