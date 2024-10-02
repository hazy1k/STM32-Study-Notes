#ifndef __FONT_H
#define __FONT_H       

#include "./font/fonts.h"

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>



#define LINE(x) ((x) * (((sFONT *)LCD_GetFont())->Height))
#define LINEY(x) ((x) * (((sFONT *)LCD_GetFont())->Width))

/** @defgroup FONTS_Exported_Types
  * @{
  */ 
typedef struct _tFont
{    
  const uint8_t *table; /*ָ����ģ���ݵ�ָ��*/
  uint16_t Width;       /*��ģ�����ؿ��*/
  uint16_t Height;      /*��ģ�����ظ߶�*/  
  
} sFONT;
  
/*��Щ��ѡ�������ʽ������fonts.c�ļ�*/
extern sFONT Font24x48;
extern sFONT Font16x32;
extern sFONT Font8x16;

//Ҫ֧��������Ҫʵ�ֱ��������ɲο���Һ����ʾ��Ӣ�ģ��ֿ����ⲿFLASH��������
#define      GetGBKCode( ucBuffer, usChar ) 


#endif /*end of __FONT_H    */


