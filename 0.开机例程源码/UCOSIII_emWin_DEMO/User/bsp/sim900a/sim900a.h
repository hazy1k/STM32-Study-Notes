#ifndef _SIM900A_H_
#define _SIM900A_H_

#include "includes.h"
#include "stm32f10x.h"
#include "ff.h"

typedef enum{
    SIM900A_TRUE,
    SIM900A_FALSE,
    
}sim900a_res_e;

typedef enum
{
    SIM900A_NULL                = 0,
    SIM900A_CMD_SEND            = '\r',         
    SIM900A_DATA_SEND           = 0x1A,         //��������(ctrl + z)
    SIM900A_DATA_CANCLE         = 0x1B,         //��������(Esc)    
}sim900a_cmd_end_e;

//                  ָ��             ��������
//��������          AT+CNUM\r         +CNUM: "","13265002063",129,7,4            //�ܶ�SIM��Ĭ�϶���û���ñ�������ģ������������ http://www.multisilicon.com/blog/a21234642.html
//SIMӪ����         AT+COPS?\r        +COPS: 0,0,"CHN-UNICOM"   OK
//SIM��״̬         AT+CPIN?\r        +CPIN: READY   OK
//SIM���ź�ǿ��     AT+CSQ\r          +CSQ: 8,0   OK

extern  uint8_t     sim900a_cmd         	(char *cmd, char *reply,uint32_t waittime );
extern  uint8_t     sim900a_cmd_check   	(char *reply);

#if 1

#define 	sim900a_config() 				USART2_Config()						
#define     sim900a_tx_printf(fmt, ...)     USART2_printf(USART2,fmt,##__VA_ARGS__)     //printf��ʽ������������ͣ��������κν������ݴ���

#else

#define 	sim900a_init 					USART1_Config()			
#define     sim900a_tx_printf(fmt, ...)     printf(fmt,##__VA_ARGS__)     //printf��ʽ������������ͣ��������κν������ݴ���

#endif

#define     SIM900A_CLEAN_RX()              clean_rebuff()
#define     sim900a_ate0()                  sim900a_cmd("ATE0\r","OK",100)              //�رջ���
#define     SIM900A_TX(cmd)                	sim900a_tx_printf("%s",cmd)
#define     SIM900A_IS_RX()                 (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET)
#define     SIM900A_RX(len)                 ((char *)get_rebuff(&(len)))
#define     SIM900A_DELAY(time)             bsp_DelayUS(1000*time)                 //��ʱ
#define     SIM900A_SWAP16(data)    				__REVSH(data)

/*************************** �绰 ���� ***************************/
#define  	SIM900A_HANGON()				SIM900A_TX("ATA\r");								
#define  	SIM900A_HANGOFF()				SIM900A_TX("ATH\r");	//�Ҷϵ绰	

extern 	uint8_t 	sim900a_init								(void);															//��ʼ�������ģ��
extern  uint8_t     sim900a_cnum            (char *num);                        //��ȡ��������
extern  void        sim900a_call           	(char *num);                        //���𲦴�绰�����ܽӲ���ͨ��
extern	uint8_t 	IsRing					(char *num);						//��ѯ�Ƿ����磬�������������

/***************************  ���Ź���  ****************************/
extern  void        sim900a_sms             (char *num,char *smstext);          //���Ͷ��ţ�֧����Ӣ��,����ΪGBK�룩
extern  char *      sim900a_waitask         (uint8_t waitask_hook(void));       //�ȴ�������Ӧ�𣬷��ؽ��ջ�������ַ
extern  void        sim900a_gbk2ucs2        (char * ucs2,char * gbk);           
extern	void 				sim900a_sms_utf8						(char *num,char *smstext,uint16_t numlen,uint16_t textlen);//���Ͷ��ţ�֧����Ӣ��,����ʱʹ��UTF8���룩

extern	uint8_t 	IsReceiveMS				(void);
extern	uint8_t 	readmessage				(uint8_t messadd,char *num,char *str);
extern	uint8_t 	hexuni2gbk				(char *hexuni,char *chgbk);

/*************************** GPRS ���� ***************************/
//extern  void        sim900a_gprs_init           (void);                                                 //GPRS��ʼ������
//extern  void        sim900a_gprs_tcp_link       (char *localport,char * serverip,char * serverport);    //TCP����
//extern  void        sim900a_gprs_udp_link       (char *localport,char * serverip,char * serverport);    //UDP����
//extern  void        sim900a_gprs_send           (char *str);                                            //��������
//extern  void        sim900a_gprs_link_close     (void);                                                 //IP���ӶϿ�

void 	sim900a_gprs_init		(void);																//GPRS��ʼ������
uint8_t sim900a_gprs_tcp_link	(char *localport,char * serverip,char * serverport);				//TCP����
uint8_t sim900a_gprs_udp_link	(char *localport,char * serverip,char * serverport);				//UDP����
uint8_t sim900a_gprs_send		(const char * str);														//��������
uint8_t sim900a_gprs_link_close	(void);              												//IP���ӶϿ�
uint8_t sim900a_gprs_shut_close	(void);               												//�رճ���
uint8_t	PostGPRS(void);
#endif

