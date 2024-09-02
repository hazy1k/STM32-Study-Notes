#ifndef __HC05_H
#define	__HC05_H


#include "stm32H7xx.h"
#include "./delay/core_delay.h"

#define delay_ms 			HAL_Delay
#define HC05_USART   	BLT_USARTx



/* ����LED���ӵ�GPIO�˿�, �û�ֻ��Ҫ�޸�����Ĵ��뼴�ɸı���Ƶ�LED���� */
#define BLT_INT_GPIO_PORT    	  GPIOI		                       /* GPIO�˿� */
#define BLT_INT_GPIO_CLK() 	   __HAL_RCC_GPIOI_CLK_ENABLE()		   /* GPIO�˿�ʱ�� */
#define BLT_INT_GPIO_PIN		  	GPIO_PIN_3		                 /* ���ӵ�HC05 INT���ŵ�GPIO */

#define BLT_KEY_GPIO_PORT    	  GPIOC			                        /* GPIO�˿� */
#define BLT_KEY_GPIO_CLK()	      __HAL_RCC_GPIOC_CLK_ENABLE()			/* GPIO�˿�ʱ�� */
#define BLT_KEY_GPIO_PIN		    GPIO_PIN_2		                   /* ���ӵ�HC05 KEY���ŵ�GPIO */




#define BLT_KEY_HIGHT  		  HAL_GPIO_WritePin(BLT_KEY_GPIO_PORT, BLT_KEY_GPIO_PIN,GPIO_PIN_SET);
#define BLT_KEY_LOW  				HAL_GPIO_WritePin(BLT_KEY_GPIO_PORT, BLT_KEY_GPIO_PIN,GPIO_PIN_RESET);

//IS_HC05_CONNECTED���ڼ��ģ���Ƿ������״̬
#define IS_HC05_CONNECTED() 	HAL_GPIO_ReadPin(BLT_INT_GPIO_PORT,BLT_INT_GPIO_PIN)


/*��Ϣ���*/
#define HC05_DEBUG_ON        0
#define HC05_DEBUG_FUNC_ON    0

#define HC05_INFO(fmt,arg...)           printf("<<-HC05-INFO->> "fmt"\n",##arg)
#define HC05_ERROR(fmt,arg...)          printf("<<-HC05-ERROR->> "fmt"\n",##arg)
#define HC05_DEBUG(fmt,arg...)          do{\
                                          if(HC05_DEBUG_ON)\
                                          printf("<<-HC05-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)

#define HC05_DEBUG_FUNC()               do{\
                                         if(HC05_DEBUG_FUNC_ON)\
                                         printf("<<-HC05-FUNC->> Func:%s@Line:%d\n",__func__,__LINE__);\
                                       }while(0)

	



#define BLTDEV_MAX_NUM 5

/*������ַ��������ʽ����NAP��UAP��LAP��*/																			 
typedef  struct 
{
	uint16_t NAP;
	uint8_t 	UAP;
	uint32_t LAP;
}BLTAddr;


typedef  struct 
{
	uint8_t num;		//ɨ�赽�������豸����
	
	BLTAddr addr[BLTDEV_MAX_NUM];	//�����豸��ַ��������ʽ
	
	char unpraseAddr[BLTDEV_MAX_NUM][50];	//�����豸��ַ���ַ�����ʽ������ɨ��ʱ������ʱʹ��
	
	char name[BLTDEV_MAX_NUM][50];	//�����豸������
	
}BLTDev;



enum
{
  HC05_DEFAULT_TIMEOUT = 200,
  HC05_INQUIRY_DEFAULT_TIMEOUT = 10000,
  HC05_PAIRING_DEFAULT_TIMEOUT = 10000,
  HC05_PASSWORD_MAXLEN = 16,
  HC05_PASSWORD_BUFSIZE = HC05_PASSWORD_MAXLEN + 1,
  HC05_NAME_MAXLEN = 32,
  HC05_NAME_BUFSIZE = HC05_NAME_MAXLEN + 1,
  HC05_ADDRESS_MAXLEN = 14,
  HC05_ADDRESS_BUFSIZE = HC05_ADDRESS_MAXLEN + 1,
};

																			 
																			 
																			 

																			 
uint8_t HC05_Init(void);
uint8_t HC05_Send_CMD(char* cmd,uint8_t clean);
void HC05_SendString(char* str);																			 
void strBLTAddr(BLTDev *bltDev,char delimiter);
uint8_t getRemoteDeviceName(BLTDev *bltDev);
void printBLTInfo(BLTDev *bltDev);
uint8_t linkHC05(void);
int get_line(char* line, char* stream ,int max_size);

#endif /* _HC05_H */







