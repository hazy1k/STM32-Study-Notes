#ifndef __BSP_GSM_GPRS_H
#define __BSP_GSM_GPRS_H

#include "stm32f4xx.h"
#include "./systick/bsp_SysTick.h"
#include "./gsm_gprs/bsp_gsm_usart.h"

#include "ff.h"

typedef enum{
    GSM_TRUE,
    GSM_FALSE,
    
}gsm_res_e;

typedef enum
{
    GSM_NULL                = 0,
    GSM_CMD_SEND            = '\r',         
    GSM_DATA_SEND           = 0x1A,         //��������(ctrl + z)
    GSM_DATA_CANCLE         = 0x1B,         //��������(Esc)    
}gsm_cmd_end_e;

//                  ָ��             ��������
//��������          AT+CNUM\r         +CNUM: "","13265002063",129,7,4            //�ܶ�SIM��Ĭ�϶���û���ñ�������ģ������������ http://www.multisilicon.com/blog/a21234642.html
//SIMӪ����         AT+COPS?\r        +COPS: 0,0,"CHN-UNICOM"   OK
//SIM��״̬         AT+CPIN?\r        +CPIN: READY   OK
//SIM���ź�ǿ��     AT+CSQ\r          +CSQ: 8,0   OK

extern  uint8_t     gsm_cmd         	(char *cmd, char *reply,uint32_t waittime );
extern  uint8_t     gsm_cmd_check   	(char *reply);


#define     GSM_CLEAN_RX()              clean_rebuff()
#define     gsm_ate0()                  gsm_cmd("ATE0\r","OK",100)              //�رջ���
#define     GSM_TX(cmd)                	GSM_USART_printf("%s",cmd)
#define     GSM_IS_RX()                 (USART_GetFlagStatus(GSM_USARTx, USART_FLAG_RXNE) != RESET)
#define     GSM_RX(len)                 ((char *)get_rebuff(&(len)))
#define     GSM_DELAY(time)             Delay_ms(time)                 //��ʱ
#define     GSM_SWAP16(data)    				 __REVSH(data)

/*************************** �绰 ���� ***************************/
#define  	GSM_HANGON()				GSM_TX("ATA\r");								
#define  	GSM_HANGOFF()				GSM_TX("ATH\r");	//�Ҷϵ绰	

uint8_t 	gsm_init								(void);															//��ʼ�������ģ��
uint8_t     gsm_cnum            (char *num);                        //��ȡ��������
void        gsm_call           	(char *num);                        //���𲦴�绰�����ܽӲ���ͨ��
uint8_t 	IsRing					(char *num);						//��ѯ�Ƿ����磬�������������
uint8_t  IsInsertCard(void);

/***************************  ���Ź���  ****************************/
uint8_t        gsm_sms             (char *num,char *smstext);          //���Ͷ��ţ�֧����Ӣ��,����ΪGBK�룩
char *      gsm_waitask         (uint8_t waitask_hook(void));       //�ȴ�������Ӧ�𣬷��ؽ��ջ�������ַ
void        gsm_gbk2ucs2        (char * ucs2,char * gbk);           

uint8_t 	IsReceiveMS				(void);
uint8_t 	readmessage				(uint8_t messadd,char *num,char *str);
uint8_t 	hexuni2gbk				(char *hexuni,char *chgbk);

/*************************** GPRS ���� ***************************/
uint8_t 	gsm_gprs_init		(void);																//GPRS��ʼ������
uint8_t gsm_gprs_tcp_link	(char *localport,char * serverip,char * serverport);				//TCP����
uint8_t gsm_gprs_udp_link	(char *localport,char * serverip,char * serverport);				//UDP����
uint8_t gsm_gprs_send		(const char * str);														//��������
uint8_t gsm_gprs_link_close	(void);              												//IP���ӶϿ�
uint8_t gsm_gprs_shut_close	(void);               												//�رճ���
uint8_t	PostGPRS(void);





/*�����ô���*/

#define GSM_DEBUG_ON         	0
#define GSM_DEBUG_ARRAY_ON    0
#define GSM_DEBUG_FUNC_ON   	0
// Log define
#define GSM_INFO(fmt,arg...)           printf("<<-GSM-INFO->> "fmt"\n",##arg)
#define GSM_ERROR(fmt,arg...)          printf("<<-GSM-ERROR->> "fmt"\n",##arg)
#define GSM_DEBUG(fmt,arg...)          do{\
                                         if(GSM_DEBUG_ON)\
                                         printf("<<-GSM-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
																					}while(0)

#define GSM_DEBUG_ARRAY(array, num)    do{\
                                         int32_t i;\
                                         uint8_t* a = array;\
                                         if(GSM_DEBUG_ARRAY_ON)\
                                         {\
                                            printf("<<-GSM-DEBUG-ARRAY->> [%d]\n",__LINE__);\
                                            for (i = 0; i < (num); i++)\
                                            {\
                                                printf("%02x   ", (a)[i]);\
                                                if ((i + 1 ) %10 == 0)\
                                                {\
                                                    printf("\n");\
                                                }\
                                            }\
                                            printf("\n");\
                                        }\
                                       }while(0)

#define GSM_DEBUG_FUNC()               do{\
                                         if(GSM_DEBUG_FUNC_ON)\
                                         printf("<<-GSM-FUNC->> Func:%s@Line:%d\n",__func__,__LINE__);\
                                       }while(0)

#endif

