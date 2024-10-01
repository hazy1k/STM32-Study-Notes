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
#include  "string.h"
#include "gui_mem_port.h"
#include "gui_font_port.h"
#include "gui_resource_port.h"

#include "GUI_AppDef.h"

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


/* Ĭ������ */
HFONT defaultFont =NULL;

/* Ĭ��Ӣ������ */
HFONT defaultFontEn = NULL;

/* ���ڱ���Ƿ�����Դ�ļ��޷��ҵ� */
BOOL res_not_found_flag = FALSE;

#if(GUI_ICON_LOGO_EN)  
/* logo���� */
HFONT logoFont =NULL;
HFONT logoFont_200 =NULL;
/* ͼ������ */
HFONT iconFont_50 =NULL;
HFONT iconFont_150 =NULL;
HFONT iconFont_252 =NULL;
/* ����ͼ������ */
HFONT controlFont_12 =NULL;
HFONT controlFont_24 =NULL;
HFONT controlFont_48 =NULL;
HFONT controlFont_32 =NULL;
HFONT controlFont_16 =NULL;
HFONT controlFont_72 =NULL;
HFONT controlFont_100 =NULL;
#endif

/** 
  * @brief  �������
  *  Ϊ��Ӧ��ͬ�ֱ��ʵ���Ļ��ʹ�ò�ͬ��Ĭ������ 
  *  ����Ҫʹ�ò�ͬ�������ʱ��ֱ������gui_drv_cfg.h�ļ�����
  *  ��Щ��������gui_drv_cfg.h�ļ��ĺ�GUI_DEFAULT_FONT_EN��GUI_DEFAULT_EXTERN_FONT
*/  
#if !((STM32F10X_HD) || (STM32F40_41xxx))
const FONT_PARAM_TypeDef gui_font_param[GUI_LCD_TYPE_NUM] = {
  /* 5���� */
  {   
    .default_en = ASCII_24_4BPP,                /* Ĭ��Ӣ�����壬�ڲ�flash */
    .default_extern_cn = "GB2312_24_4BPP.xft", /* Ĭ���������壬�ⲿ */
    .default_extern_logo100 = "APP_ICON_100_100_4BPP.xft",
  },
  /* 7���� */
  {   
    .default_en = ASCII_24_4BPP,                /* Ĭ��Ӣ�����壬�ڲ�flash */
    .default_extern_cn = "GB2312_24_4BPP.xft", /* Ĭ���������壬�ⲿ */
  },
  /* 4.3���� */
  {   
    .default_en = ASCII_16_4BPP,                /* Ĭ��Ӣ�����壬�ڲ�flash */
    .default_extern_cn = "GB2312_20_4BPP.xft", /* Ĭ���������壬�ⲿ */
  },
};
#endif
#if (GUI_FONT_LOAD_TO_RAM_EN)
  u8 *default_font_buf;
  u8 *logo_font_buf_100;
#endif

/*===================================================================================*/
#if (GUI_EXTERN_FONT_EN)

#if (!GUI_RES_DEV_EN)
  #error Use extern must enable macro 'GUI_RES_DEV_EN' first!
#endif

/***********************��1����*************************/
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
	offset += lParam;
  
  /* ��ȡ�������ģ�������� */
	RES_DevRead(buf,offset,size);
	return size;
}
/***********************��2����*************************/
/**
  * @brief  ��ʼ���ⲿFLASH����(���豸��ʽ)
  * @param  res_name ������Դ����
  * @retval ����Ĭ������ľ��
  */
HFONT GUI_Init_Extern_Font_Stream(const char* res_name)
{
  /* ʹ�����豸�������壬����Ҫ��ȡ */
  int font_base;
  HFONT hFont = NULL;
  CatalogTypeDef dir;

#if (GUI_APP_BOOT_INTERFACE_EN)
  {
    /* ��������Ľ����� */
    if(Boot_progbar != NULL)
    {  
      int count = 0;

      /* ������������Ľ����� */
      count = SendMessage(Boot_progbar,PBM_GET_VALUE,TRUE,NULL); 
      count++;
      SendMessage(Boot_progbar,PBM_SET_VALUE,TRUE,count); 
    }
  }    
#endif

  font_base =RES_GetInfo_AbsAddr(res_name, &dir);
  if(font_base > 0)
  {
    hFont =XFT_CreateFontEx(font_read_data_exFlash,font_base);
  }
  else
  {
    res_not_found_flag = TRUE;
    GUI_ERROR("Can not find RES:%s",res_name);
  }
  
  if(hFont==NULL)
  {
    res_not_found_flag = TRUE;    
    GUI_ERROR("%s font create failed",res_name);
  }
  return hFont;
}



/**
  * @brief  ��ʼ���ⲿFLASH���壨������ص�SDRAM��
  * @param  res_name[in] ������Դ����
  * @param  buf[out]��������Դ���Ƶ��Ļ�������
  *         ����ͨ����free��ռ�õĿռ䣬��ע���ͷź�ʹ������Ļ������
  * @retval ����Ĭ������ľ��
  */
HFONT GUI_Init_Extern_Font_2RAM(const char* res_name,u8** buf)
{
    /* ���������ļ�������RAM */
    int font_base; 
    HFONT hFont = NULL;  
    CatalogTypeDef dir;
#if (GUI_APP_BOOT_INTERFACE_EN)
  {
    /* ��������Ľ����� */
    if(Boot_progbar != NULL)
    {  
      int count = 0;

      /* ������������Ľ����� */
      count = SendMessage(Boot_progbar,PBM_GET_VALUE,TRUE,NULL); 
      count++;
      SendMessage(Boot_progbar,PBM_SET_VALUE,TRUE,count); 
    }  
  }
#endif
  
    /* RES_GetInfo��ȡ����dir.offset����Դ�ľ��Ե�ַ */
    font_base =RES_GetInfo_AbsAddr(res_name, &dir);

    if(font_base > 0)
    {
    	*buf =(u8*)GUI_VMEM_Alloc(dir.size);
      if(*buf!=NULL)
      {
        RES_DevRead((u8*)*buf, font_base, dir.size);

        hFont = XFT_CreateFont(*buf);
      }
    }
    else
    {
      res_not_found_flag = TRUE;
      GUI_ERROR("Can not find RES:%s",res_name);
    }
    
    if(hFont==NULL)
    {
      res_not_found_flag = TRUE;    
      GUI_ERROR("%s font create failed",res_name);
    }
    
   return hFont;
}


/**
  * @brief  �����ⲿ����
  * @param  ��
  * @retval �������´�����defaultFont
  */
HFONT GUI_Init_Extern_Font(void)
{
   /* ���������ļ�������RAM */
#if (GUI_FONT_LOAD_TO_RAM_EN  )
  {  
    defaultFont = GUI_Init_Extern_Font_2RAM(GUI_DEFAULT_EXTERN_FONT,&default_font_buf);
//    logoFont100 = GUI_Init_Extern_Font_2RAM(GUI_LOGO_FONT_100,&logo_font_buf_100);
    }
  
#else
   /* ʹ�����豸�������壬����Ҫ��ȡ */
  {
    defaultFont    = GUI_Init_Extern_Font_Stream(GUI_DEFAULT_EXTERN_FONT);
    logoFont       = GUI_Init_Extern_Font_Stream(GUI_LOGO_FONT);
    controlFont_48 = GUI_Init_Extern_Font_Stream(GUI_CONTROL_FONT_48);
    controlFont_24 = GUI_Init_Extern_Font_Stream(GUI_CONTROL_FONT_24);
    controlFont_32 = GUI_Init_Extern_Font_Stream(GUI_CONTROL_FONT_32);
    controlFont_16 = GUI_Init_Extern_Font_Stream(GUI_CONTROL_FONT_16);
    iconFont_50    = GUI_Init_Extern_Font_Stream(GUI_ICON_FONT_50);
//    iconFont_150   = GUI_Init_Extern_Font_Stream(GUI_ICON_FONT_150);
  }
#endif 
 
  return defaultFont;
}

#endif


/**
  * @brief  GUIĬ�������ʼ��
  * @param  ��
  * @retval ����Ĭ������ľ��
  */
HFONT GUI_Default_FontInit(void)
{
    /* ��ǰ����������ʧ�ܣ�ʹ���ڲ�FLASH�е����ݣ������е�C�������飩
    *  �����������ʱ���������ļ���ӵ����̣��ڱ��ļ�ͷ�����Ӧ���������������
    *  Ȼ�����XFT_CreateFont�����������弴��
    */
  
    /* Ĭ��Ӣ������ */ 
#if (STM32F10X_HD) || (STM32F40_41xxx)
    defaultFontEn =XFT_CreateFont(GUI_DEFAULT_FONT_EN);
#endif
    /* ���ʹ�����������棬�����������ټ����ⲿ���� */
#if (GUI_EXTERN_FONT_EN && (!GUI_APP_BOOT_INTERFACE_EN))
 
    /* ���ⲿ��Դ�豸�������� */
    defaultFont = GUI_Init_Extern_Font();  
  
#elif (GUI_INER_CN_FONT_EN && (!GUI_APP_BOOT_INTERFACE_EN))
    /* �����ֿ�洢ռ�ÿռ�ǳ��󣬲��Ƽ������ڲ�FLASH */
    /* ���ڲ�flash����Ĭ���������壬���Ƽ�*/
    defaultFont = XFT_CreateFont(GUI_DEFAULT_FONT_CN);    
#endif
 
/* ��ʹ���ⲿ����ʱ��Ĭ�ϰ�Load_state��1 */  
#if ((!GUI_EXTERN_FONT_EN) || (!GUI_APP_BOOT_INTERFACE_EN)) 
    Load_state = TRUE;
#endif  
  
    /* �������崴��ʧ��ʱʹ��Ӣ��������ΪĬ������ */
    if(defaultFont==NULL)
    { 
      defaultFont = defaultFontEn;  
    }
    
	return defaultFont;
}

/********************************END OF FILE****************************/

