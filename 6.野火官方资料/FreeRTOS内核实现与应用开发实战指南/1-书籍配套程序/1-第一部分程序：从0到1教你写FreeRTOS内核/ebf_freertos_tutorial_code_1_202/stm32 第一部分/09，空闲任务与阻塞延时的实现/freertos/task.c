#include "FreeRTOS.h"
#include "task.h"


/*
*************************************************************************
*                               ������ƿ�
*************************************************************************
*/
//typedef struct tskTaskControlBlock
//{
//	volatile StackType_t    *pxTopOfStack;    /* ջ�� */

//	ListItem_t			    xStateListItem;   /* ����ڵ� */
//    
//    StackType_t             *pxStack;         /* ����ջ��ʼ��ַ */
//	                                          /* �������ƣ��ַ�����ʽ */
//	char                    pcTaskName[ configMAX_TASK_NAME_LEN ];  
//} tskTCB;
//typedef tskTCB TCB_t;

/* ��ǰ�������е������������ƿ�ָ�룬Ĭ�ϳ�ʼ��ΪNULL */
TCB_t * volatile pxCurrentTCB = NULL;

/* ��������б� */
//static List_t pxReadyTasksLists[ configMAX_PRIORITIES ];
List_t pxReadyTasksLists[ configMAX_PRIORITIES ];


static volatile UBaseType_t uxCurrentNumberOfTasks 	= ( UBaseType_t ) 0U;
static TaskHandle_t xIdleTaskHandle					= NULL;
static volatile TickType_t xTickCount 				= ( TickType_t ) 0U;


/*
*************************************************************************
*                               ��������
*************************************************************************
*/

static void prvInitialiseNewTask( 	TaskFunction_t pxTaskCode,              /* ������� */
									const char * const pcName,              /* �������ƣ��ַ�����ʽ */
									const uint32_t ulStackDepth,            /* ����ջ��С����λΪ�� */
									void * const pvParameters,              /* �����β� */
									TaskHandle_t * const pxCreatedTask,     /* ������ */
									TCB_t *pxNewTCB );

//static void prvInitialiseTaskLists( void );
static portTASK_FUNCTION( prvIdleTask, pvParameters );
void vTaskSwitchContext( void );                                    
                                    
/*
*************************************************************************
*                               �궨��
*************************************************************************
*/
                                    
#if 0                                    
#if ( configUSE_PORT_OPTIMISED_TASK_SELECTION == 0 )

	/* If configUSE_PORT_OPTIMISED_TASK_SELECTION is 0 then task selection is
	performed in a generic way that is not optimised to any particular
	microcontroller architecture. */

	/* uxTopReadyPriority holds the priority of the highest priority ready
	state task. */
	#define taskRECORD_READY_PRIORITY( uxPriority )														\
	{																									\
		if( ( uxPriority ) > uxTopReadyPriority )														\
		{																								\
			uxTopReadyPriority = ( uxPriority );														\
		}																								\
	} /* taskRECORD_READY_PRIORITY */

	/*-----------------------------------------------------------*/

	#define taskSELECT_HIGHEST_PRIORITY_TASK()															\
	{																									\
	UBaseType_t uxTopPriority = uxTopReadyPriority;														\
																										\
		/* Find the highest priority queue that contains ready tasks. */								\
		while( listLIST_IS_EMPTY( &( pxReadyTasksLists[ uxTopPriority ] ) ) )							\
		{																								\
			configASSERT( uxTopPriority );																\
			--uxTopPriority;																			\
		}																								\
																										\
		/* listGET_OWNER_OF_NEXT_ENTRY indexes through the list, so the tasks of						\
		the	same priority get an equal share of the processor time. */									\
		listGET_OWNER_OF_NEXT_ENTRY( pxCurrentTCB, &( pxReadyTasksLists[ uxTopPriority ] ) );			\
		uxTopReadyPriority = uxTopPriority;																\
	} /* taskSELECT_HIGHEST_PRIORITY_TASK */

	/*-----------------------------------------------------------*/

	/* Define away taskRESET_READY_PRIORITY() and portRESET_READY_PRIORITY() as
	they are only required when a port optimised method of task selection is
	being used. */
	#define taskRESET_READY_PRIORITY( uxPriority )
	#define portRESET_READY_PRIORITY( uxPriority, uxTopReadyPriority )

#else /* configUSE_PORT_OPTIMISED_TASK_SELECTION */

	/* If configUSE_PORT_OPTIMISED_TASK_SELECTION is 1 then task selection is
	performed in a way that is tailored to the particular microcontroller
	architecture being used. */

	/* A port optimised version is provided.  Call the port defined macros. */
	#define taskRECORD_READY_PRIORITY( uxPriority )	portRECORD_READY_PRIORITY( uxPriority, uxTopReadyPriority )

	/*-----------------------------------------------------------*/

	#define taskSELECT_HIGHEST_PRIORITY_TASK()														\
	{																								\
	UBaseType_t uxTopPriority;																		\
																									\
		/* Find the highest priority list that contains ready tasks. */								\
		portGET_HIGHEST_PRIORITY( uxTopPriority, uxTopReadyPriority );								\
		configASSERT( listCURRENT_LIST_LENGTH( &( pxReadyTasksLists[ uxTopPriority ] ) ) > 0 );		\
		listGET_OWNER_OF_NEXT_ENTRY( pxCurrentTCB, &( pxReadyTasksLists[ uxTopPriority ] ) );		\
	} /* taskSELECT_HIGHEST_PRIORITY_TASK() */

	/*-----------------------------------------------------------*/

	/* A port optimised version is provided, call it only if the TCB being reset
	is being referenced from a ready list.  If it is referenced from a delayed
	or suspended list then it won't be in a ready list. */
	#define taskRESET_READY_PRIORITY( uxPriority )														\
	{																									\
		if( listCURRENT_LIST_LENGTH( &( pxReadyTasksLists[ ( uxPriority ) ] ) ) == ( UBaseType_t ) 0 )	\
		{																								\
			portRESET_READY_PRIORITY( ( uxPriority ), ( uxTopReadyPriority ) );							\
		}																								\
	}

#endif /* configUSE_PORT_OPTIMISED_TASK_SELECTION */                                   

#endif
                                   

/*
*************************************************************************
*                               ��̬���񴴽�����
*************************************************************************
*/
#if( configSUPPORT_STATIC_ALLOCATION == 1 )

TaskHandle_t xTaskCreateStatic(	TaskFunction_t pxTaskCode,           /* ������� */
					            const char * const pcName,           /* �������ƣ��ַ�����ʽ */
					            const uint32_t ulStackDepth,         /* ����ջ��С����λΪ�� */
					            void * const pvParameters,           /* �����β� */
					            StackType_t * const puxStackBuffer,  /* ����ջ��ʼ��ַ */
					            TCB_t * const pxTaskBuffer )  /* ������ƿ� */
{
	TCB_t *pxNewTCB;
	TaskHandle_t xReturn;

	if( ( pxTaskBuffer != NULL ) && ( puxStackBuffer != NULL ) )
	{		
		pxNewTCB = ( TCB_t * ) pxTaskBuffer; 
		pxNewTCB->pxStack = ( StackType_t * ) puxStackBuffer;

		/* �����µ����� */
		prvInitialiseNewTask( pxTaskCode, pcName, ulStackDepth, pvParameters, &xReturn, pxNewTCB);        

	}
	else
	{
		xReturn = NULL;
	}

	return xReturn;
}

#endif /* configSUPPORT_STATIC_ALLOCATION */


static void prvInitialiseNewTask( 	TaskFunction_t pxTaskCode,              /* ������� */
									const char * const pcName,              /* �������ƣ��ַ�����ʽ */
									const uint32_t ulStackDepth,            /* ����ջ��С����λΪ�� */
									void * const pvParameters,              /* �����β� */
									TaskHandle_t * const pxCreatedTask,     /* ������ */
									TCB_t *pxNewTCB )                       /* ������ƿ� */

{
	StackType_t *pxTopOfStack;
	UBaseType_t x;	
	
	/* ��ȡջ����ַ */
	pxTopOfStack = pxNewTCB->pxStack + ( ulStackDepth - ( uint32_t ) 1 );
	//pxTopOfStack = ( StackType_t * ) ( ( ( portPOINTER_SIZE_TYPE ) pxTopOfStack ) & ( ~( ( portPOINTER_SIZE_TYPE ) portBYTE_ALIGNMENT_MASK ) ) );
	/* ������8�ֽڶ��� */
	pxTopOfStack = ( StackType_t * ) ( ( ( uint32_t ) pxTopOfStack ) & ( ~( ( uint32_t ) 0x0007 ) ) );	

	/* ����������ִ洢��TCB�� */
	for( x = ( UBaseType_t ) 0; x < ( UBaseType_t ) configMAX_TASK_NAME_LEN; x++ )
	{
		pxNewTCB->pcTaskName[ x ] = pcName[ x ];

		if( pcName[ x ] == 0x00 )
		{
			break;
		}
	}
	/* �������ֵĳ��Ȳ��ܳ���configMAX_TASK_NAME_LEN */
	pxNewTCB->pcTaskName[ configMAX_TASK_NAME_LEN - 1 ] = '\0';
    
    /* ��ʼ��TCB�е�xStateListItem�ڵ� */
    vListInitialiseItem( &( pxNewTCB->xStateListItem ) );
    /* ����xStateListItem�ڵ��ӵ���� */
	listSET_LIST_ITEM_OWNER( &( pxNewTCB->xStateListItem ), pxNewTCB );

    /* ��ʼ������ջ */
	pxNewTCB->pxTopOfStack = pxPortInitialiseStack( pxTopOfStack, pxTaskCode, pvParameters );
    
    /* ��������ָ��������ƿ� */
    if( ( void * ) pxCreatedTask != NULL )
	{		
		*pxCreatedTask = ( TaskHandle_t ) pxNewTCB;
	}
}

/* ��ʼ��������ص��б� */
void prvInitialiseTaskLists( void )
{
    UBaseType_t uxPriority;
    
    
    for( uxPriority = ( UBaseType_t ) 0U; uxPriority < ( UBaseType_t ) configMAX_PRIORITIES; uxPriority++ )
	{
		vListInitialise( &( pxReadyTasksLists[ uxPriority ] ) );
	}
}

#if 0
extern TCB_t Task1TCB;
extern TCB_t Task2TCB;
void vTaskStartScheduler( void )
{    
    /* �ֶ�ָ����һ�����е����� */
    pxCurrentTCB = &Task1TCB;
    
    /* ���������� */
    if( xPortStartScheduler() != pdFALSE )
    {
        /* �����������ɹ����򲻻᷵�أ��������������� */
    }
}

#else

extern TCB_t Task1TCB;
extern TCB_t Task2TCB;

extern TCB_t IdleTaskTCB;
void vApplicationGetIdleTaskMemory( TCB_t **ppxIdleTaskTCBBuffer, 
                                    StackType_t **ppxIdleTaskStackBuffer, 
                                    uint32_t *pulIdleTaskStackSize );
void vTaskStartScheduler( void )
{
/*======================================������������start==============================================*/     
    TCB_t *pxIdleTaskTCBBuffer = NULL;               /* ����ָ�����������ƿ� */
    StackType_t *pxIdleTaskStackBuffer = NULL;       /* ���ڿ�������ջ��ʼ��ַ */
    uint32_t ulIdleTaskStackSize;
    
    /* ��ȡ����������ڴ棺����ջ������TCB */
    vApplicationGetIdleTaskMemory( &pxIdleTaskTCBBuffer, 
                                   &pxIdleTaskStackBuffer, 
                                   &ulIdleTaskStackSize );    
    
    xIdleTaskHandle = xTaskCreateStatic( (TaskFunction_t)prvIdleTask,              /* ������� */
					                     (char *)"IDLE",                           /* �������ƣ��ַ�����ʽ */
					                     (uint32_t)ulIdleTaskStackSize ,           /* ����ջ��С����λΪ�� */
					                     (void *) NULL,                            /* �����β� */
					                     (StackType_t *)pxIdleTaskStackBuffer,     /* ����ջ��ʼ��ַ */
					                     (TCB_t *)pxIdleTaskTCBBuffer );           /* ������ƿ� */
    /* ��������ӵ������б� */                                 
    vListInsertEnd( &( pxReadyTasksLists[0] ), &( ((TCB_t *)pxIdleTaskTCBBuffer)->xStateListItem ) );
/*======================================������������end================================================*/
                                         
    /* �ֶ�ָ����һ�����е����� */
    pxCurrentTCB = &Task1TCB;
                                         
    /* ��ʼ��ϵͳʱ�������� */
    xTickCount = ( TickType_t ) 0U;
    
    /* ���������� */
    if( xPortStartScheduler() != pdFALSE )
    {
        /* �����������ɹ����򲻻᷵�أ��������������� */
    }
}

#endif


static portTASK_FUNCTION( prvIdleTask, pvParameters )
{
	/* ��ֹ�������ľ��� */
	( void ) pvParameters;
    
    for(;;)
    {
        /* ����������ʱʲô������ */
    }
}


#if 0
void vTaskSwitchContext( void )
{    
    if( pxCurrentTCB == &Task1TCB )
    {
        pxCurrentTCB = &Task2TCB;
    }
    else
    {
        pxCurrentTCB = &Task1TCB;
    }
}
#else

void vTaskSwitchContext( void )
{
	/* �����ǰ�߳��ǿ����̣߳���ô��ȥ����ִ���߳�1�����߳�2��
       �������ǵ���ʱʱ���Ƿ����������̵߳���ʱʱ���û�е��ڣ�
       �Ǿͷ��ؼ���ִ�п����߳� */
	if( pxCurrentTCB == &IdleTaskTCB )
	{
		if(Task1TCB.xTicksToDelay == 0)
		{            
            pxCurrentTCB =&Task1TCB;
		}
		else if(Task2TCB.xTicksToDelay == 0)
		{
            pxCurrentTCB =&Task2TCB;
		}
		else
		{
			return;		/* �߳���ʱ��û�е����򷵻أ�����ִ�п����߳� */
		} 
	}
	else
	{
		/*�����ǰ�߳����߳�1�����߳�2�Ļ������������һ���߳�,���������̲߳�����ʱ�У����л������߳�
        �����ж��µ�ǰ�߳��Ƿ�Ӧ�ý�����ʱ״̬������ǵĻ������л��������̡߳�����Ͳ������κ��л� */
		if(pxCurrentTCB == &Task1TCB)
		{
			if(Task2TCB.xTicksToDelay == 0)
			{
                pxCurrentTCB =&Task2TCB;
			}
			else if(pxCurrentTCB->xTicksToDelay != 0)
			{
                pxCurrentTCB = &IdleTaskTCB;
			}
			else 
			{
				return;		/* ���أ��������л�����Ϊ�����̶߳�������ʱ�� */
			}
		}
		else if(pxCurrentTCB == &Task2TCB)
		{
			if(Task1TCB.xTicksToDelay == 0)
			{
                pxCurrentTCB =&Task1TCB;
			}
			else if(pxCurrentTCB->xTicksToDelay != 0)
			{
                pxCurrentTCB = &IdleTaskTCB;
			}
			else 
			{
				return;		/* ���أ��������л�����Ϊ�����̶߳�������ʱ�� */
			}
		}
	}
}

#endif

void vTaskDelay( const TickType_t xTicksToDelay )
{
    TCB_t *pxTCB = NULL;
    
    /* ��ȡ��ǰ�����TCB */
    pxTCB = pxCurrentTCB;
    
    /* ������ʱʱ�� */
    pxTCB->xTicksToDelay = xTicksToDelay;
    
    /* �����л� */
    taskYIELD();
}


void xTaskIncrementTick( void )
{
    TCB_t *pxTCB = NULL;
    BaseType_t i = 0;
    
    /* ����ϵͳʱ��������xTickCount��xTickCount��һ����port.c�ж����ȫ�ֱ��� */
    const TickType_t xConstTickCount = xTickCount + 1;
    xTickCount = xConstTickCount;

    
    /* ɨ������б��������̵߳�xTicksToDelay�������Ϊ0�����1 */
	for(i=0; i<configMAX_PRIORITIES; i++)
	{
        pxTCB = ( TCB_t * ) listGET_OWNER_OF_HEAD_ENTRY( ( &pxReadyTasksLists[i] ) );
		if(pxTCB->xTicksToDelay > 0)
		{
			pxTCB->xTicksToDelay --;
		}
	}
    
    /* �����л� */
    portYIELD();
}
