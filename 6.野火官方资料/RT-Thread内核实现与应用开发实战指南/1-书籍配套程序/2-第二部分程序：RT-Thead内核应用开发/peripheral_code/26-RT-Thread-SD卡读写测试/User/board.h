#ifndef __BOARD_H__
#define __BOARD_H__

/*
*************************************************************************
*                             ������ͷ�ļ�
*************************************************************************
*/
/* STM32 �̼���ͷ�ļ� */
#include "stm32f10x.h"

/* ������Ӳ��bspͷ�ļ� */
#include "./sdio/bsp_sdio_sdcard.h"
#include "./sdio/sdio_test.h"
#include "bsp_usart.h"	
#include "./led/bsp_led.h"
#include "./key/bsp_key.h" 

/*
*************************************************************************
*                               ��������
*************************************************************************
*/
void rt_hw_board_init(void);
void SysTick_Handler(void);
	

#endif /* __BOARD_H__ */
