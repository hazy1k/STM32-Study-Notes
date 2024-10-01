#ifndef __GUI_FONT_PORT_H
#define	__GUI_FONT_PORT_H

#ifdef __cplusplus
extern "C" {
#endif 
  
#include	"emXGUI.h"
#include	"gui_drv_cfg.h"

/* Ĭ������ */
extern HFONT defaultFont;
/* logo���� */
extern HFONT logoFont;
/* ͼ������ */
extern HFONT iconFont;
/* ����ͼ������ */
extern HFONT controlFont;
/* ���� */
extern HFONT GB2312_32_Font;

HFONT GUI_Default_FontInit(void);

#ifdef	__cplusplus
}
#endif

#endif /* __GUI_FONT_PORT_H */
