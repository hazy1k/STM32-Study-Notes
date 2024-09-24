#ifndef __GUIFONT_CREATE_H
#define	__GUIFONT_CREATE_H

#include "GUI.h"
#include "stm32f10x.h"

//����SIF����洢��λ�ã�
//FLASH���ļ�ϵͳ�����Ƽ��� 	USE_FLASH_FONT             	0 
//SD���ļ�ϵͳ����							USE_SDCARD_FONT							1
#define SIF_FONT_SOURCE				1

//���ٶ���죩�ֿ���FLASH�ķ��ļ�ϵͳ����ʹ��ǰ��Ҫ��FLASH�ض���ַ���������ļ�
#define USE_FLASH_FONT				0	
//���ٶ��еȣ��ֿ�洢��SD���ļ�ϵͳ���򣬵��ԱȽϷ��㣬ֱ��ʹ�ö������ӵ��Կ��������ļ�����
#define USE_SDCARD_FONT				1


/*֧�ֵ��ֿ�����*/
extern GUI_FONT     FONT_XINSONGTI_16_4BPP;
extern GUI_FONT     FONT_SIYUANHEITI_20_4BPP;


void Create_SIF_Font(void) ;

#endif /* __GUIFONT_CREATE_H */
