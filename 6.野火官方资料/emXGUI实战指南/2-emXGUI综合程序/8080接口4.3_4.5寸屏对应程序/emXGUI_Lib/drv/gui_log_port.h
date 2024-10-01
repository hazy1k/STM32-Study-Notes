#ifndef __GUI_LOG_PORT_H
#define	__GUI_LOG_PORT_H

#include	"emXGUI.h"
#if defined(STM32F429_439xx)
#include "stm32f4xx.h"
#elif defined(STM32H743xx)
#include "stm32h7xx.h"
#endif
#include	"gui_drv_cfg.h"

/* GUI_Log ��gui_os_port.h�ж��� */

/* INFO��ERROR���κ�����¶������ */
/* ��Ϣ��� */
#define GUI_INFO(fmt,arg...)           GUI_Log("<<-GUI-INFO->> "fmt"\n",##arg)
/* ������� */
#define GUI_ERROR(fmt,arg...)          GUI_Log("<<-GUI-ERROR->> "fmt"\n",##arg)
/* �����������GUI_DEBUG_EN���� */
#define GUI_DEBUG(fmt,arg...)          do{\
                                         if(GUI_DEBUG_EN)\
                                         GUI_Log("<<-GUI-DEBUG->> [%s] [%d]"fmt"\n",__FILE__,__LINE__, ##arg);\
																					}while(0)
/* �����������GUI_DEBUG_ARRAY_EN���� */
#define GUI_DEBUG_ARRAY(array, num)    do{\
                                         int32_t i;\
                                         uint8_t* a = array;\
                                         if(GUI_DEBUG_ARRAY_EN)\
                                         {\
                                            GUI_Log("<<-GUI-DEBUG-ARRAY->>\n");\
                                            for (i = 0; i < (num); i++)\
                                            {\
                                                GUI_Log("%02x   ", (a)[i]);\
                                                if ((i + 1 ) %10 == 0)\
                                                {\
                                                    GUI_Log("\n");\
                                                }\
                                            }\
                                            GUI_Log("\n");\
                                        }\
                                       }while(0)

/* ���뺯���������GUI_DEBUG_FUNC_EN����
 һ������Ҫ���Եĺ�����ͷ���е��ã�
 ����ʱ��ͨ�������Ϣ�˽�������ʲô����������˳��
*/
#define GUI_DEBUG_FUNC()               do{\
                                         if(GUI_DEBUG_FUNC_EN)\
                                         GUI_Log("<<-GUI-FUNC->> Func:%s@Line:%d\n",__func__,__LINE__);\
                                       }while(0)

                                       
BOOL	GUI_Log_Init(void);
                                       
void	GUI_Printf(const char *fmt,...);

    
#endif /* __GUI_LOG_PORT_H */
