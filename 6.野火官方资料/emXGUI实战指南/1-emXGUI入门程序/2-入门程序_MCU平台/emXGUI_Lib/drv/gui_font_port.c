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
HFONT logoFont100 =NULL;
/* Ĭ��Ӣ������ */
HFONT defaultFontEn = NULL;

/* ���ڱ���Ƿ�����Դ�ļ��޷��ҵ� */
BOOL res_not_found_flag = FALSE;

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
    #if 0
    
    uint32_t loop = 0;
    
    FRESULT result; 
    UINT  bw;            					    /* File R/W count */
    uint32_t read_addr=0,j=0;
    uint8_t tempbuf[256],flash_buf[256];
    FIL file_temp;
   
//    res_name = strcat(",res_name);
    
      result = f_open(&file_temp,"0:/srcdata/GB2312_16_4BPP.xft",FA_OPEN_EXISTING | FA_READ);
      if(result != FR_OK)
      {
          printf("���ļ�ʧ�ܣ�\n");
          LED_RED;
      }
      else
      {
        printf("���ļ���%s  �ɹ�\n", res_name);
      }
      
       //У������
      read_addr = font_base;
     
      f_lseek(&file_temp,0);
      loop = 0;
      while(result == FR_OK) 
      {
        loop++;
        result = f_read( &file_temp, tempbuf, 256, &bw);//��ȡ����	 
        if(result!=FR_OK)			 //ִ�д���
        {
          printf("��ȡ�ļ�ʧ�ܣ�\n");
          LED_RED;
        }    

        if(bw == 0)break;//Ϊ0ʱ�����ж�д������
  
        SPI_FLASH_BufferRead(flash_buf,read_addr,bw);  //��FLASH�ж�ȡ����
        read_addr+=bw;		
        
        for(j=0;j<bw;j++)
        {
          if(tempbuf[j] != flash_buf[j])
          {
            printf("����У��ʧ�ܣ�\n");
            LED_RED;
          }   
        }  
     
        if(bw !=256)break;//�����ļ�β
      }   
      printf("����У��OK\n");
    
    #endif
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
    defaultFont =GUI_Init_Extern_Font_Stream(GUI_DEFAULT_EXTERN_FONT);  
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
#if (STM32F10X_HD) || (STM32F40_41xxx)|| (STM32F429_439xx) || (STM32H750xx) || (STM32H743xx)
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

