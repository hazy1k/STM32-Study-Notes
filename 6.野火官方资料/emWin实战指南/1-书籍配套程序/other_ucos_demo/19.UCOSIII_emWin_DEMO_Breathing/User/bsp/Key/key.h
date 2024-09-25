#ifndef __KEY_H__
#define __KEY_H__

#include "stm32f10x.h"
/*****************************************�û�������������************************************************/
//�û��������

//Ұ��ISO-V2�����尴�������µ�ʱ��ܽ��Ǹߵ�ƽ,Ұ��ISO-V1�İ��Ӱ��µ�ʱ��ܽ��ǵ͵�ƽ��
//��������������ISO-V2������ģ����Զ������£����Ҫ��ISO-V1����������ֻ�轫�������������Ե����ɡ�
#define KEY_ON  1
#define KEY_OFF 0

//������������������
#define KEY1_PIN  GPIO_Pin_0
#define KEY1_PORT GPIOA

#define KEY2_PIN  GPIO_Pin_13
#define KEY2_PORT GPIOC

//�û������������

//�û���������
void Key1_GPIO_Config(void);
void Key2_GPIO_Config(void);
uint8_t GetPinStateOfKey1(void);
uint8_t GetPinStateOfKey2(void);
//�û�������������
/*****************************************�û�������������************************************************/

/*ʹ���û����ݣ�ÿһ��������һ���������Թ��û�����ʹ�ã��������
 *��������������ڸĳ�0���ɣ����Ҫ�þ͸ĳ�1����������˷�һ����
 *�ڵ��ڴ�
 */
#define USER_DATA_EN			0

#define KEY_WOBBLE_TIME		10      //��������ʱ�䡣Ҳ��������ʱ��,��λms
 
#define KEY_FIXED_PERIOD	10      //�̶�Ƶ�ʵ��ð���״̬���º���,������Ϊ�������ڣ����ڵ�λΪms

//����ص���������
typedef  uint8_t    (*KEY_CALLBACK_PTR)(void);

#define KEY_TIMES_MAX (0XFF)
typedef enum{
	KEY_ACCESS_READ = 0,   //Ĭ���Ƕ�ȡ
	KEY_ACCESS_WRITE_CLEAR = 0x01
}ACCESS_TYPE;

/************�������ݾ�����Ҫ���ģ�ֱ��ʹ�ü���*******************************/
typedef enum
{
	KEY_DOWN 				= 1,
	KEY_UP	 				= 2,
	KEY_UP_WOBBLE 	= 3,//ȷ�ϵ�������״̬
	KEY_DOWN_WOBBLE = 4 //ȷ�ϰ�������״̬
}KEY_STATE;

typedef enum
{
	CHANGED 				    = 1,
	NOT_CHANGED	 				= 2,
}KEY_STATE_CHAGE;

typedef struct
{
	KEY_CALLBACK_PTR    GetStatePtr;	//���ڻ�ȡ����״̬�ĺ��� 
	uint8_t     				Times;				//���²��������1��ʹ�ú���Ӧ�ó����1
  KEY_STATE		        State;
  KEY_STATE_CHAGE     StateChange;                      
#ifdef KEY_FIXED_PERIOD
	unsigned char				Time_ms;			//���ڹ̶����ڵ���״̬���º����ļ�ʱ
#endif
  
#if USER_DATA_EN>0             
	uint8_t				      User_Data;		//�û������������û�����ʹ��
#endif
  
}KEY;

/*			����������																							*/
void KeyCreate(KEY *p_Key,KEY_CALLBACK_PTR p_CallBack);
void Key_RefreshState(KEY* theKey);
uint8_t Key_AccessTimes(KEY* p_Key,ACCESS_TYPE opt);
uint8_t Key_AccessState(KEY* p_Key,KEY_STATE *p_State);
#endif
