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

/*============================================================================*/

/* ע�ᴥ���豸 */
#if	GUI_TOUCHSCREEN_EN
#ifdef STM32F10X_HD ||  
#if GUI_TOUCHSCREEN_CALIBRATE
#define	SET_TOUCH_DEV(obj)	  extern const GUI_TOUCH_DEV obj; \
							  const GUI_TOUCH_DEV *pTouchDev=(GUI_TOUCH_DEV*)&obj;

SET_TOUCH_DEV(TouchDev_ADS7843);
#endif
#endif
#endif	/* GUI_TOUCHSCREEN_EN */

/*========================�����ӿڲ���===================================*/
/**
  * @brief  ������ʼ���ӿ�,�ᱻgui_input_port.c�ļ���GUI_InputInit��������
  * @note  ��Ҫ�ڱ�������ʼ�����������Ӳ��
  * @retval �Ƿ��ʼ������
  */

BOOL TouchDev_Init(void)
{	
  /* ��ʼ�����׵�5/7/3.2/4.5���� */
  if(GTP_Init_Panel() == 0)
    return TRUE;
  else
    return FALSE;  
}
 

/**
  * @brief  ��ȡ����״̬�����꣬����Ҫ�û��޸� 
  * @note  ����������GTP_Execu�ӿڣ��ýӿ���Ҫ���ش���������Ƿ񱻰��µ�״̬��
  *        ��������bsp_touch_gt9xx.c�ļ�ʵ��
  * @param  pt[out] �洢��ȡ����x y����
  * @retval ����״̬
  *    @arg  TS_ACT_DOWN  ��������
  *    @arg  TS_ACT_UP    �����ͷ�
  *    @arg  TS_ACT_NONE  �޴�������
  */

BOOL TouchDev_GetPoint(POINT *pt)
{
#if GUI_TOUCHSCREEN_CALIBRATE
  return pTouchDev->GetPoint(pt);
#else
  static int ts_state=TS_ACT_NONE;

  /* ͨ��GTP_Execu��ȡ���������״̬ */
	if(GTP_Execu(&pt->x,&pt->y) > 0)
	{
			ts_state =TS_ACT_DOWN;
	}
	else
	{
		if(ts_state==TS_ACT_DOWN)
		{
			ts_state =TS_ACT_UP;
		}
		else
		{
			ts_state =TS_ACT_NONE;
		}
	}
	return ts_state;
#endif
}
/**
  * @brief  ��Ҫ����ʱ���õĴ���������
  * @note   ��������ͨ��gui_input_port.c�ļ���GUI_InputHandler����ʱ����
  * @param  ��
  * @retval ��
  */ 

void GUI_TouchHandler(void)
{
	int act;
	POINT pt;
  
#if GUI_TOUCHSCREEN_CALIBRATE
  /* ���������� */
  act =TouchDev_GetAction();
	if(act==TS_ACT_DOWN)
	{
		if(TouchDev_GetPoint(&pt))
		{
			MouseInput(pt.x,pt.y,MK_LBUTTON);
		}
	}

	if(act==TS_ACT_UP)
	{
		GetCursorPos(&pt);
		MouseInput(pt.x,pt.y,0);
	}
#else
  /* �жϴ���״̬������ */
	act =TouchDev_GetPoint(&pt);
	if(act==TS_ACT_DOWN)
	{
    /* �������£�ʹ�ô���������Ϊ���� */
		MouseInput(pt.x,pt.y,MK_LBUTTON);
	}

	if(act==TS_ACT_UP)
	{
    /* �����ͷţ�ʹ�õ�ǰ�����Ϊ����*/
		GetCursorPos(&pt);
		MouseInput(pt.x,pt.y,0);
	}
#endif
}


/********************************END OF FILE****************************/

