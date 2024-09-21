#ifndef INC_FREERTOS_H
#define INC_FREERTOS_H

#include "FreeRTOSConfig.h"
#include "projdefs.h"
#include "portable.h"
#include "list.h"


//#ifndef configUSE_TIME_SLICING
//	#define configUSE_TIME_SLICING 1
//#endif


typedef struct tskTaskControlBlock
{
	volatile StackType_t    *pxTopOfStack;    /* ջ�� */

	ListItem_t			    xStateListItem;   /* ����ڵ� */
    
    StackType_t             *pxStack;         /* ����ջ��ʼ��ַ */
	                                          /* �������ƣ��ַ�����ʽ */
	char                    pcTaskName[ configMAX_TASK_NAME_LEN ];

    TickType_t xTicksToDelay;
    UBaseType_t			uxPriority;    
} tskTCB;
typedef tskTCB TCB_t;

















#endif /* INC_FREERTOS_H */
