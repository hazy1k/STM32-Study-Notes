/**
  *********************************************************************
  * @file    gui_os_port.h
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   ��ͬϵͳ�Ľӿ�
  *********************************************************************
  * @attention
  * ����    :www.emXGUI.com
  *
  **********************************************************************
  */ 
  
#ifndef	__GUI_OS_PORT_H__
#define	__GUI_OS_PORT_H__

/* �ڹ���ѡ���ж������µĺ꣬������ʹ�õĲ���ϵͳ */

//X_GUI_USE_RTTHREAD
//X_GUI_USE_FREERTOS

#ifdef	X_GUI_USE_RTTHREAD
  /* RT-Thread ϵͳͷ�ļ� */ 
  #include	"rtthread.h"
  
  /* ϵͳ������ڴ�ѣ�gui_mem_port.c */
  #define OS_MALLOC   rt_malloc
  #define OS_FREE     rt_free
    
  /* ���Ի������Ϣ����ӿ� gui_log_port.h */
  #define   GUI_Log     rt_kprintf

#elif X_GUI_USE_FREERTOS
  /* FreeRTOS ϵͳͷ�ļ� */ 
  #include <stdio.h>
  #include "FreeRTOS.h"
  #include "task.h"  
  
  /* ϵͳ������ڴ�ѣ�gui_mem_port.c */
  #define OS_MALLOC   pvPortMalloc
  #define OS_FREE     vPortFree
    
  /* ���Ի������Ϣ����ӿ� gui_log_port.h */
  #define   GUI_Log     printf
  
#else
  /* û����Ĵ�����ʾ */
  #error No OS type select,please define macro 'X_GUI_USE_RTTHREAD'/'X_GUI_USE_FREERTOS' or else.
#endif



#endif
