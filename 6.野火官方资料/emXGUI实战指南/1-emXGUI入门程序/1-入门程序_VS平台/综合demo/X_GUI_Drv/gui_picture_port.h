#ifndef __GUI_PICTURE_PORT_H
#define	__GUI_PICTURE_PORT_H

#include "emXGUI.h"

#ifdef	__cplusplus
extern "C"{
#endif

BOOL PIC_BMP_Draw_Res(HDC hdc, int x, int y, char *file_name, const RECT *lprc);
BOOL PIC_BMP_GetInfo_Res(BITMAPINFO *bm_info, char *file_name);
BOOL PIC_BMP_Draw_FS(HDC hdc, int x, int y, char *file_name, const RECT *lprc);
BOOL PIC_BMP_GetInfo_FS(BITMAPINFO *bm_info, char *file_name); 


#ifdef	__cplusplus
}
#endif

#endif /* __GUI_PICTURE_PORT_H */
