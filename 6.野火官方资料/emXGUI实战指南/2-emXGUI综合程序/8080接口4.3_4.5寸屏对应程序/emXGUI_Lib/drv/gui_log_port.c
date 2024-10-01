/**
  *********************************************************************
  * @file    gui_input_port.c
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   �����豸�����ӿ�
  *********************************************************************
  * @attention
  * ����    :www.emXGUI.com
  *
  **********************************************************************
  */ 
  
#include "board.h"
#include "gui_drv.h"
#include "gui_drv_cfg.h"
#include "gui_log_port.h"



/*============================================================================*/
/**
  * @brief  ��ʼ��GUI��־�ӿ�
  * @param  ��
  * @retval �Ƿ��ʼ������
  */
BOOL	GUI_Log_Init(void)
{
  /* ʹ�ô�����Ϊ��־����ӿ� */
  /* ��ʼ������ */
  Debug_USART_Config();
  return TRUE;
}



/**
  * @brief  ��ʽ���ַ������
  * @param  �ɱ����
  * @note   ����������GUI������Ϣ,�����������Ϊ��
  * @retval ��
  */
void	GUI_Printf(const char *fmt,...)
{
#if 0
    x_va_list ap;
    char *buf;
    static	char str_buf[256];
    static  GUI_MUTEX *mutex_gui_printf=NULL;
    ////
    buf	=str_buf/*(char*)GUI_MEM_Alloc(256)*/;
    if(buf)
    {

      if(GUI_MutexLock(mutex_gui_printf,0xFFFFFFFF))
      {
          x_va_start(ap,fmt);
          x_vsprintf(buf,fmt,ap);
          /* �ײ���Ҫ�ṩDebugPuts�ӿڽ������ */
          DebugPuts(buf);
          //DebugOutput(buf);

          GUI_MutexUnlock(mutex_gui_printf);
          x_va_end(ap);
      }

    //GUI_MEM_Free(buf);
  }
#else

//  rt_kprintf(fmt);  
#endif   
  
}


///**
//  * @brief  ��ʽ���ַ������
//  * @param  �ɱ����
//  * @note   ����������GUI������Ϣ,�����������Ϊ��
//  * @retval ��
//  */
//void	GUI_Log(const char *fmt,...)
//{
//  rt_kprintf(fmt);
//}
/********************************END OF FILE****************************/

