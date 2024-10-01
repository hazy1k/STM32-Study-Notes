/**
  *********************************************************************
  * @file    X_GUI_FreeRTOS.c
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   emXGUI��FreeRTOS����ϵͳ�ӿ�
  *********************************************************************
  * @attention
  * ����    :www.emXGUI.com
  *
  **********************************************************************
  */ 
/* �ڹ���ѡ���ж�������꣬������ʹ�õĲ���ϵͳ */  
#ifdef	X_GUI_USE_FREERTOS

#include	<stddef.h>
#include	"emXGUI_Arch.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

/*===================================================================================*/
/*
��������: ����һ������(�û���������֧��Ƕ��ʹ��)
����: ���������(Ψһ��ʶ)
˵��: �����������ʵ��OS����,������ָ��,ID�ŵ�...
*/
GUI_MUTEX*	GUI_MutexCreate(void)
{
	return (GUI_MUTEX*)xSemaphoreCreateRecursiveMutex();	
}

/*===================================================================================*/
/*
��������: ��������
����: hMutex(��GUI_MutexCreate���صľ��); 
      time ��ȴ�������,0���ȷ���,0xFFFFFFFF,һֱ�ȴ�
����: TRUE:�ɹ�;FALSE:ʧ�ܻ�ʱ
˵��: .
*/
BOOL	GUI_MutexLock(GUI_MUTEX *hMutex,U32 time)
{
	if(xSemaphoreTakeRecursive((SemaphoreHandle_t)hMutex,pdMS_TO_TICKS(time))==pdTRUE)
	{
		return TRUE;
	}
	return	FALSE;
}

/*===================================================================================*/
/*
��������: �������
����: hMutex(��GUI_MutexCreate���صľ��);    
����: ��
˵��: .
*/
void	GUI_MutexUnlock(GUI_MUTEX *hMutex)
{
	 xSemaphoreGiveRecursive((SemaphoreHandle_t)hMutex);
}

/*===================================================================================*/
/*
��������: ����ɾ��
����: hMutex(��GUI_MutexCreate���صľ��);    
����: ��
˵��: .
*/
void	GUI_MutexDelete(GUI_MUTEX *hMutex)
{
	vSemaphoreDelete((SemaphoreHandle_t)hMutex);
}

/*===================================================================================*/
/*
��������: ����һ���ź���
����: init: �ź�����ʼֵ; max: �ź������ֵ
����: �ź���������(Ψһ��ʶ)
˵��: �ź�����������ʵ��OS����,������ָ��,ID�ŵ�...
*/
GUI_SEM*	GUI_SemCreate(int init,int max)
{
	return (GUI_SEM*)xSemaphoreCreateCounting(max,init);
}

/*===================================================================================*/
/*
��������: �ź����ȴ�
����: hsem(��GUI_SemCreate���صľ��); 
      time ��ȴ�������,0���ȷ���,0xFFFFFFFF,һֱ�ȴ�
����: TRUE:�ɹ�;FALSE:ʧ�ܻ�ʱ
˵��: .
*/
BOOL	GUI_SemWait(GUI_SEM *hsem,U32 time)
{

	if(xSemaphoreTake((SemaphoreHandle_t)hsem,pdMS_TO_TICKS(time))== pdTRUE)
	{
		return TRUE;
	}
	return FALSE;
}

/*===================================================================================*/
/*
��������: �ź�������
����: hsem(��GUI_SemCreate���صľ��);  
����: ��
˵��: .
*/
void	GUI_SemPost(GUI_SEM *hsem)
{
	xSemaphoreGive((SemaphoreHandle_t)hsem);
}
/*
��������: �ź�������(��freertos������ж�)
����: hsem(��GUI_SemCreate���صľ��);  
����: ��
˵��: ������freertos������ж��е���GUI_SemPost,�ᵼ��port.c��425
*/
void GUI_SemPostISR(GUI_SEM *hsem)
{
   
  BaseType_t pxHigherPriorityTaskWoken;
  
  
	xSemaphoreGiveFromISR((SemaphoreHandle_t)hsem, &pxHigherPriorityTaskWoken);
  
  portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
}
/*===================================================================================*/
/*
��������: �ź���ɾ��
����: hsem(��GUI_SemCreate���صľ��);    
����: ��
˵��: .
*/
void	GUI_SemDelete(GUI_SEM *hsem)
{
	vSemaphoreDelete((SemaphoreHandle_t)hsem);
}

/*===================================================================================*/
/*
��������: ��õ�ǰ�߳̾��(Ψһ��ʶ)
����: ��  
����: ��ǰ�߳�Ψһ��ʶ,��ʵ��OS����,������ָ��,ID�ŵ�...
˵��: .
*/
HANDLE	GUI_GetCurThreadHandle(void)
{
	return	(HANDLE)xTaskGetCurrentTaskHandle();
}


/*===================================================================================*/
/*
��������: ��õ�ǰϵͳʱ��(��λ:����)
����: ��  
����: ��ǰϵͳʱ��
˵��: .
*/
U32	GUI_GetTickCount(void)
{
	U32 i;
	
	i=xTaskGetTickCount();
	
	return (i*1000)/configTICK_RATE_HZ;

}

/*===================================================================================*/
/*
��������: ���ʱ�����ó�CPU
����: ��  
����: ��
˵��: ������OS�������,��򵥵ķ�����:OS Delay һ�� tick ����.
*/
void	GUI_Yield(void)
{
	vTaskDelay(2);
}

/*===================================================================================*/
/*
��������: ��ʱ����
����: ms: ��ʱʱ��(��λ:����) 
����: ��
˵��: 
*/
void	GUI_msleep(u32 ms)
{
	vTaskDelay(pdMS_TO_TICKS(ms));
}

/*
 * ��������: �����߳�
 * @param name �߳���
 * @param entry �߳���ں���
 * @param parameter �̲߳���
 * @param stack_size �߳�ջ��С����λ�ֽڣ�ע�ⲿ��ϵͳ��Ҫ���е�λת����
 * @param priority �߳����ȼ�
 * @param tick FreeRTOSû������ܣ�ʱ��Ƭ��ͬ���ȼ������ʱ��Ƭ��ת��
 * @return �Ƿ񴴽��ɹ�
*/
BOOL GUI_Thread_Create(void (*entry)(void *parameter),
                         const char *name,
                         u32  stack_size,
                         void *parameter,
                         u32  priority,
                         u32  tick)
{
   BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */

   xReturn = xTaskCreate((TaskFunction_t )entry,  /* ������ں��� */
                            (const char*    )name,/* �������� */
                            (uint16_t       )stack_size/4,  /* ����ջ��СFreeRTOS������ջ����Ϊ��λ */
                            (void*          )NULL,/* ������ں������� */
                            (UBaseType_t    )priority, /* ��������ȼ� */
                            (TaskHandle_t*  )NULL);/* ������ƿ�ָ�� */
                            
  if(xReturn == pdPASS )
    return TRUE;
  else
  {
    GUI_ERROR("GUI Thread Create failed:%s",name);
    return FALSE; 
  }    
}

/**
 * @breif: ɾ���̣߳���ͨ��GUI_GetCurThreadHandle��ȡ��ǰ��������Ϊ�������
 * @return ��
*/
void GUI_Thread_Delete(HANDLE thread)
{
  vTaskDelete((TaskHandle_t) thread);
}
#endif

/********************************END OF FILE****************************/

