/*
 * emXGUI_JPEG.h
 *
 *  Created on: 2018Äê8ÔÂ21ÈÕ
 *      Author: liuwei
 */

#ifndef __EMXGUI_JPEG_H__
#define __EMXGUI_JPEG_H__


#ifdef	__cplusplus
extern "C"{
#endif

#include "emXGUI.h"

#define vmalloc  GUI_VMEM_Alloc
#define vfree	 GUI_VMEM_Free

typedef	struct	__jpg_dec JPG_DEC;

JPG_DEC*	JPG_Open(const void *dat,int cbSize);
BOOL 	JPG_GetImageSize(U16 *width,U16 *height,JPG_DEC* jdec);
BOOL	JPG_Draw(HDC hdc,int x,int y,JPG_DEC *jdec);
void	JPG_Close(JPG_DEC *jdec);


#ifdef	__cplusplus
}
#endif


#endif /* __EMXGUI_JPEG_H__ */
