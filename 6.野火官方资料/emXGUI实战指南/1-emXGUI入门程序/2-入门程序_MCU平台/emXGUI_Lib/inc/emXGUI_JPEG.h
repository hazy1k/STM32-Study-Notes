/*
 * emXGUI_JPEG.h
 *
 *  Created on: 2018��8��21��
 *      Author: liuwei
 */

#ifndef __EMXGUI_JPEG_H__
#define __EMXGUI_JPEG_H__


#ifdef	__cplusplus
extern "C"{
#endif

#include "emXGUI.h"

typedef	struct	__jpg_dec JPG_DEC;

/**
  * @brief  ��ȡJPG_DEC���
  * @param  dat[in]ͼƬ���ڵĻ�����
  * @param  cbSizeͼƬ�ļ���С
  * @retval ���ɵ�JPG_DEC���
  */  
JPG_DEC*	JPG_Open(const void *dat,int cbSize);

/**
* @brief  ��ȡͼƬ�Ŀ��
* @param  width[out] height[out] ͼƬ�Ŀ��
* @param  JPG_DEC ͼƬ���
* @retval �Ƿ�����
*/  
BOOL 	JPG_GetImageSize(U16 *width,U16 *height,JPG_DEC* jdec);

/**
* @brief  ����ͼƬ
* @param  hdc ��ͼ������
* @param  x y ��ʾ��λ��
* @param  JPG_DEC ͼƬ���
* @retval �Ƿ�����
*/  
BOOL	JPG_Draw(HDC hdc,int x,int y,JPG_DEC *jdec);

/**
* @brief  ����ͼƬ(����)
* @param  hdc ��ͼ������
* @param  x y ��ʾ��λ��
* @param  mjpegbuffer ͼƬ����
* @param  size ͼƬ��С
* @retval �Ƿ�����
*/  
void JPEG_Out(HDC hdc,int x,int y,u8 *mjpegbuffer,s32 size);

/**
* @brief  �ͷŻ�ͼ���
* @param  JPG_DEC ͼƬ���
* @retval ��
*/  
void	JPG_Close(JPG_DEC *jdec);



#ifdef	__cplusplus
}
#endif


#endif /* __EMXGUI_JPEG_H__ */
