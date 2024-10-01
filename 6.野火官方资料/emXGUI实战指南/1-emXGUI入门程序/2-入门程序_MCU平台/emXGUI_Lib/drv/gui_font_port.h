#ifndef __GUI_FONT_PORT_H
#define	__GUI_FONT_PORT_H

#ifdef __cplusplus
extern "C" {
#endif 
  
#include	"emXGUI.h"
#include	"gui_drv_cfg.h"
  
  /** 
  * @brief  �������
  *  Ϊ��Ӧ��ͬ�ֱ��ʵ���Ļ��ʹ�ò�ͬ��Ĭ������ 
  */   
typedef struct
{
  const char *default_en;  //Ĭ��Ӣ������
  const char *default_extern_cn;  //Ĭ����������
  const char *default_extern_logo100;  //LOGO����

}FONT_PARAM_TypeDef;

/* ����������� */
extern const FONT_PARAM_TypeDef gui_font_param[];
extern HFONT logoFont100;
/* Ĭ������ */
extern HFONT defaultFont;
/* Ĭ��Ӣ������ */
extern HFONT defaultFontEn;
  
/* ���ڱ���Ƿ�����Դ�ļ��޷��ҵ� */
extern BOOL res_not_found_flag;

HFONT GUI_Default_FontInit(void);
HFONT GUI_Init_Extern_Font_Stream(const char* res_name);
HFONT GUI_Init_Extern_Font_2RAM(const char* res_name,u8** buf);
HFONT GUI_Init_Extern_Font(void);
  
#ifdef	__cplusplus
}
#endif

#endif /* __GUI_FONT_PORT_H */
