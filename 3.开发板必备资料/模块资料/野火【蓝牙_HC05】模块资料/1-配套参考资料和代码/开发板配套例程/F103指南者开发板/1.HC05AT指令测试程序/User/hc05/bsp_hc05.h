#ifndef __HC05_H
#define	__HC05_H


#include "stm32f10x.h"
#include "./dwt_delay/core_delay.h"   

#define hc05_delay_ms   Delay_ms
#define HC05_USART   	  BLT_USARTx



/* ����LED���ӵ�GPIO�˿�, �û�ֻ��Ҫ�޸�����Ĵ��뼴�ɸı���Ƶ�LED���� */
#define BLT_INT_GPIO_PORT    	GPIOB			              /* GPIO�˿� */
#define BLT_INT_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* GPIO�˿�ʱ�� */
#define BLT_INT_GPIO_PIN		  	GPIO_Pin_13			          /* ���ӵ�HC05 INT���ŵ�GPIO */

#define BLT_KEY_GPIO_PORT    	GPIOB			              /* GPIO�˿� */
#define BLT_KEY_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* GPIO�˿�ʱ�� */
#define BLT_KEY_GPIO_PIN		    GPIO_Pin_14		          /* ���ӵ�HC05 KEY���ŵ�GPIO */



#define BLT_KEY_HIGHT  		GPIO_SetBits(BLT_KEY_GPIO_PORT, BLT_KEY_GPIO_PIN);
#define BLT_KEY_LOW  				GPIO_ResetBits(BLT_KEY_GPIO_PORT, BLT_KEY_GPIO_PIN);

//IS_HC05_CONNECTED���ڼ��ģ���Ƿ������״̬
#define IS_HC05_CONNECTED() 	GPIO_ReadInputDataBit(BLT_INT_GPIO_PORT,BLT_INT_GPIO_PIN	)


/*��Ϣ���*/
#define HC05_DEBUG_ON         0
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




//#define ENABLE_LCD_DISPLAY    //�л�Һ����ʾ�� ʹ��Ұ�𡾵��败����ILI9341_XPT2046_3.2_2.8�硿





//��������豸����
#define BLTDEV_MAX_NUM 10

                                       
                                       
/*������ַ��������ʽ����NAP��UAP��LAP��*/																			 

typedef  struct 
{
	uint8_t num;		//ɨ�赽�������豸����
		
	char unpraseAddr[BLTDEV_MAX_NUM][50];	//�����豸��ַ���ַ�����ʽ������ɨ��ʱ������ʱʹ��
	
	char name[BLTDEV_MAX_NUM][50];	//�����豸������
	
}BLTDev;

//�����豸�б��� bsp_hc05.c �ļ��ж���
extern  BLTDev bltDevList;


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
uint8_t HC05_Send_CMD_Wait(char* cmd, uint8_t clean, uint32_t delayms); //Debug
void HC05_SendString(char* str);

uint8_t parseBltAddr(void);
uint8_t getRemoteDeviceName(void);
void printBLTInfo(void);
uint8_t linkHC05(void);

int get_line(char* line, char* stream ,int max_size);

#endif /* _HC05_H */







