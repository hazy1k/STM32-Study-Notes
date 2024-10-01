
#include "GUI_Drv.h"
//#include "BSP.h"

/*===================================================================================*/
/*
 *GPU绘图加速驱动函数,如果不支持操作可以直接返回FALSE.
 */

#if(DMA2D_EN)
BOOL DMA2D_FillRect(const SURFACE *pSurf,int x,int y,U16 w,U16 h,U32 color);
BOOL DMA2D_DrawBitmap_RGB565(const SURFACE *pSurf,int x,int y,U16 w,U16 h,int width_bytes,const U8 *bits);
BOOL DMA2D_DrawBitmap_ARGB(const SURFACE *pSurf,int x,int y,U16 w,U16 h,int width_bytes,const U8 *bits,U32 color_format);

#endif

/*===================================================================================*/

#if 0
BOOL	GUI_GPU_Init(void)
{

	return TRUE;
}
#endif

/*============================================================================*/
/*
 * 将指定区域原始像素复制到out缓冲区.
 */
BOOL 	GPU_CopyBits(const SURFACE *pSurf,int x,int y,int w,int h,void *out,int width_bytes)
{
	return FALSE;
}

/*============================================================================*/
/*
 * 矩形填充.
 */
BOOL	GPU_FillRect(const SURFACE *pSurf,int x,int y,int w,int h,COLORREF c)
{
#if(DMA2D_EN)
	{
		if(DMA2D_FillRect(pSurf,x,y,w,h,c))
		{
			return TRUE;
		}
		return FALSE;
	}
#else
	return FALSE;
#endif
}

/*============================================================================*/
/*
 * 矩形填充(ARGB方式).
 */
BOOL	GPU_FillRectARGB(const SURFACE *pSurf,int x,int y,int w,int h,U8 a,U8 r,U8 g,U8 b)
{
	return FALSE;
}

/*============================================================================*/
/*
 * 绘制位图.
 */
BOOL	GPU_DrawBitmap(const SURFACE *pSurf,int x,int y,int w,int h,const U8 *bits,int width_bytes,int format)
{
#if(DMA2D_EN)
	{
		switch(format)
		{

		case BM_RGB565:
			return	DMA2D_DrawBitmap_RGB565(pSurf,x,y,w,h,width_bytes,bits);

		case BM_ARGB8888:
			return	DMA2D_DrawBitmap_ARGB(pSurf,x,y,w,h,width_bytes,bits,CM_ARGB8888);

		default:
			return FALSE;

		}
	}
#else
	return FALSE;
#endif

}

/*============================================================================*/
/*
 * 缩放位图.
 */
BOOL 	GPU_ScaleBitmap(const SURFACE *pSurf,int x,int y,int w,int h,const BITMAP *bm)
{
#if(DMA2D_EN)
	{
	return FALSE;
	}
#else
	return FALSE;
#endif
}

/*============================================================================*/


