#ifndef _BSP_SIM900A_H_
#define _BSP_SIM900A_H_

#include "stm32f10x.h"
#include "./sim900a/bsp_gsm_usart.h"

typedef enum{
  SIM900A_TRUE, 
  SIM900A_FALSE,         
}sim900a_res_e;


//                  ָ��             ��������
//��������          AT+CNUM\r         +CNUM: "","13265002063",129,7,4            //�ܶ�SIM��Ĭ�϶���û���ñ�������ģ������������ http://www.multisilicon.com/blog/a21234642.html
//SIMӪ����         AT+COPS?\r        +COPS: 0,0,"CHN-UNICOM"   OK
//SIM��״̬         AT+CPIN?\r        +CPIN: READY   OK
//SIM���ź�ǿ��     AT+CSQ\r          +CSQ: 8,0   OK

uint8_t   sim900a_cmd(char *cmd, char *reply,uint32_t waittime );
uint8_t   sim900a_cmd_check(char *reply);

#define 	sim900a_config() 				        GSM_USART_Config()						
#define   sim900a_tx_printf(fmt, ...)     GSM_USART_printf(fmt,##__VA_ARGS__)     //printf��ʽ������������ͣ��������κν������ݴ���

#define   SIM900A_CLEAN_RX()              gsm_clean_rebuff()
#define   sim900a_ate0()                  sim900a_cmd("ATE0\r","OK",100)              //�رջ���
#define   SIM900A_TX(cmd)                	sim900a_tx_printf("%s",cmd)
#define   SIM900A_IS_RX()                 (USART_GetFlagStatus(GSM_USART, USART_FLAG_RXNE) != RESET)
#define   SIM900A_RX(len)                 ((char *)gsm_get_rebuff(&(len)))
#define   SIM900A_DELAY(time)             CPU_TS_Tmr_Delay_MS(time)                 //��ʱ
#define   SIM900A_SWAP16(data)    		    __REVSH(data)

/*************************** �绰 ���� ***************************/
#define  	SIM900A_HANGON()				        SIM900A_TX("ATA\r");								
#define  	SIM900A_HANGOFF()				        SIM900A_TX("ATH\r");	          //�Ҷϵ绰	

uint8_t 	sim900a_init(void);															                //��ʼ�������ģ��
uint8_t   sim900a_cnum(char *num);															          //��ȡ��������
void      sim900a_call(char *num);															          //���𲦴�绰�����ܽӲ���ͨ��
uint8_t 	IsRing(char *num);															                //��ѯ�Ƿ����磬�������������
uint8_t   IsNOCARRIER(void);

/***************************  ���Ź���  ****************************/
void      sim900a_sms(char *num,char *smstext);                           //���Ͷ��ţ�֧����Ӣ��,����ΪGBK�룩
char *    sim900a_waitask(uint8_t waitask_hook(void));                    //�ȴ�������Ӧ�𣬷��ؽ��ջ�������ַ
void      sim900a_gbk2ucs2(char * ucs2,char * gbk);           
void 			sim900a_sms_utf8(char *num,char *smstext,uint16_t numlen,uint16_t textlen);//���Ͷ��ţ�֧����Ӣ��,����ʱʹ��UTF8���룩
uint16_t sim900a_save_sms(char *num,char *smstext);
uint8_t 	IsReceiveMS(void);
uint8_t 	readmessage(uint8_t messadd,char *num,char *str);
uint8_t   ReadMessageInfo(uint8_t messadd, char *Num, char *Time);
uint8_t 	hexuni2gbk(char *hexuni,char *chgbk);

/*************************** GPRS ���� ***************************/
void 	    sim900a_gprs_init		(void);																       //GPRS��ʼ������
uint8_t   sim900a_gprs_tcp_link	(char *localport,char * serverip,char * serverport);				//TCP����
uint8_t   sim900a_gprs_udp_link	(char *localport,char * serverip,char * serverport);				//UDP����
uint8_t   sim900a_gprs_send		(const char * str);													 //��������
uint8_t   sim900a_gprs_link_close	(void);              										 //IP���ӶϿ�
uint8_t   sim900a_gprs_shut_close	(void);               									 //�رճ���
uint8_t   PostGPRS(void);                                                  //IP���ӶϿ�
uint8_t sim900a_cmd_isr(char *cmd, char *reply,uint32_t waittime );
#endif  //_BSP_SIM900A_H_

