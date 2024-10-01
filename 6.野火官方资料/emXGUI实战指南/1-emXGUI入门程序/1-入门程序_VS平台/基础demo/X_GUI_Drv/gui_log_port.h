#ifndef __GUI_LOG_PORT_H
#define	__GUI_LOG_PORT_H

#include	"emXGUI.h"
#include	"gui_drv_cfg.h"
#include	<stdio.h>


/* 调试或错误、信息输出接口 */
#define   GUI_Log     printf

/* INFO和ERROR在任何情况下都会输出 */
/* 信息输出 */
#define GUI_INFO(fmt,...)           GUI_Log("<<-GUI-INFO->> "fmt"\n",##__VA_ARGS__)
/* 错误输出 */
#define GUI_ERROR(fmt,...)          GUI_Log("<<-GUI-ERROR->> "fmt"\n",##__VA_ARGS__)
/* 调试输出，受GUI_DEBUG_EN控制 */
#define GUI_DEBUG(fmt,...)          do{\
                                         if(GUI_DEBUG_EN)\
                                         GUI_Log("<<-GUI-DEBUG->> [%s] [%d]"fmt"\n",__FILE__,__LINE__, ##__VA_ARGS__);\
																					}while(0)
/* 数组输出，受GUI_DEBUG_ARRAY_EN控制 */
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

/* 进入函数输出，受GUI_DEBUG_FUNC_EN控制
 一般在需要调试的函数开头进行调用，
 调试时可通过输出信息了解运行了什么函数或运行顺序
*/
#define GUI_DEBUG_FUNC()               do{\
                                         if(GUI_DEBUG_FUNC_EN)\
                                         GUI_Log("<<-GUI-FUNC->> Func:%s@Line:%d\n",__func__,__LINE__);\
                                       }while(0)

                                       
BOOL	GUI_Log_Init(void);
                                       
void	GUI_Printf(const char *fmt,...);

    
#endif /* __GUI_LOG_PORT_H */
