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
#include "gui_touch_port.h"
#include "gui_input_port.h"
/*============================================================================*/

/**
  * @brief  GUI�����豸�ĳ�ʼ��
  * @param  ��
  * @retval �Ƿ��ʼ������
  */
BOOL GUI_InputInit(void)
{
  BOOL state = FALSE;
  #if(GUI_KEYBOARD_EN)
  {
   state = KeyBoardDev_Init();
  }
  #endif

  #if(GUI_TOUCHSCREEN_EN)
  {
    state = TouchDev_Init();
    if(state) /*�������豸��ʼ��*/
    {
    #if(GUI_TOUCHSCREEN_CALIBRATE)
        TS_CFG_DATA ts_cfg;
        if(TouchDev_LoadCfg(&ts_cfg)) /*����У������(��������Ҫ)*/
        {
          TouchPanel_TranslateInit(&ts_cfg); /*��ʼ������ת������(��������Ҫ)*/
        }
    #endif
    }

  }
  #endif

  #if(GUI_MOUSE_EN)
  {
    state = MouseDev_Init();
  }
  #endif
  return state;
}

/*===================================================================================*/

/**
  * @brief  GUI�����豸�Ķ�ʱ������
  * @note   �ú�����Ҫ����ʱִ�У�
  *         ��ʹ�ö������̵߳��� �� ������Ķ�ʱ����Ϣ�е���
  *         
  * @param  ��
  * @retval ��
  */
void GUI_InputHandler(void)
{

  #if(GUI_KEYBOARD_EN)
  {
    if(KeyBoardDev_IsActive())
    {
      KeyBoardDev_Handler();
    }
  }
  #endif

  #if(GUI_TOUCHSCREEN_EN)
  {
    GUI_TouchHandler(); //���ô�����������
  }
  #endif

  #if(GUI_MOUSE_EN)
  {
    if(MouseDev_IsActive())
    {
      MouseDev_Handler();
    }
  }
  #endif

}

/********************************END OF FILE****************************/

