#ifndef __BOARD_H__
#define __BOARD_H__

/*
*************************************************************************
*                             ������ͷ�ļ�
*************************************************************************
*/
/* STM32 �̼���ͷ�ļ� */
#include "stm32h7xx.h"

/* ������Ӳ��bspͷ�ļ� */
#include "./led/bsp_led.h" 
//#include "./key/bsp_key.h" 
#include "./lcd/bsp_lcd.h"
#include "./sdram/bsp_sdram.h" 
#include "./touch/bsp_i2c_touch.h"
#include "./touch/bsp_touch_gtxx.h"
#include "./usart/bsp_usart.h"
//#include "./flash/bsp_spi_flash.h"
//#include "./font/fonts.h"

#include "./flash/bsp_qspi_flash.h"

/*
*************************************************************************
*                               ��������
*************************************************************************
*/
	

#endif /* __BOARD_H__ */
