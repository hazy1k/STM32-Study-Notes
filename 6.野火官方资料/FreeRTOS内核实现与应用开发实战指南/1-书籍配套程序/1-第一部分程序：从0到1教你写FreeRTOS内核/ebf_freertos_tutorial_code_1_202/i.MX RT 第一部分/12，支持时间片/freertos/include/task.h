#ifndef INC_TASK_H
#define INC_TASK_H

#include "list.h"

#define tskIDLE_PRIORITY			       ( ( UBaseType_t ) 0U )
#define taskYIELD()					       portYIELD()

#define taskENTER_CRITICAL()		       portENTER_CRITICAL()
#define taskENTER_CRITICAL_FROM_ISR()      portSET_INTERRUPT_MASK_FROM_ISR()

#define taskEXIT_CRITICAL()			       portEXIT_CRITICAL()
#define taskEXIT_CRITICAL_FROM_ISR( x )    portCLEAR_INTERRUPT_MASK_FROM_ISR( x )

typedef void * TaskHandle_t;


#if( configSUPPORT_STATIC_ALLOCATION == 1 )
TaskHandle_t xTaskCreateStatic(	TaskFunction_t pxTaskCode,           /* ������� */
					            const char * const pcName,           /* �������ƣ��ַ�����ʽ */
					            const uint32_t ulStackDepth,         /* ����ջ��С����λΪ�� */
					            void * const pvParameters,           /* �����β� */
                                UBaseType_t uxPriority,              /* �������ȼ�����ֵԽ�����ȼ�Խ�� */
					            StackType_t * const puxStackBuffer,  /* ����ջ��ʼ��ַ */
					            TCB_t * const pxTaskBuffer );        /* ������ƿ� */
#endif /* configSUPPORT_STATIC_ALLOCATION */
                                
void prvInitialiseTaskLists( void );                                
void vTaskStartScheduler( void );
void vTaskSwitchContext( void );
void vTaskDelay( const TickType_t xTicksToDelay );
//void xTaskIncrementTick( void );
BaseType_t xTaskIncrementTick( void );

                                
#endif /* INC_TASK_H */ 

