#include "..\User\bsp\Key\key.h"

/***************************************�û��Զ��庯��***********************************/
/*
************************************************************************************************************************
*                              �����ܽ�״̬��ȡ����
*
* ������������������������ֻ�ʺ�STM32��Ƭ���⿪�����������͵�CPU���������޸ġ�
*
*     ��������
*
*   ����ֵ������������ڱ�����״̬�ĵ�ƽ������1��
*           ����������ڵ���״̬�ĵ�ƽ������0��
*
************************************************************************************************************************
*/

uint8_t GetPinStateOfKey1(void)
{
	if(GPIO_ReadInputDataBit(KEY1_PORT,KEY1_PIN)==1)
	{
			return 1;
	}
	else
	{
			return 0;
	}
}
/*
************************************************************************************************************************
*                              ����1�ܽų�ʼ������
*
* �����������ڰ���1ʹ��֮ǰ�����ȵ���
*
*     ��������
*
*   ����ֵ����
*
************************************************************************************************************************
*/
void Key1_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Pin=KEY1_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;
	GPIO_Init(KEY1_PORT,&GPIO_InitStructure);
}

/*
************************************************************************************************************************
*                              �����ܽ�״̬��ȡ����
*
* ������������������������ֻ�ʺ�STM32��Ƭ���⿪�����������͵�CPU���������޸ġ�
*
*     ��������
*
*   ����ֵ������������ڱ�����״̬�ĵ�ƽ������1��
*           ����������ڵ���״̬�ĵ�ƽ������0��
*
************************************************************************************************************************
*/

uint8_t GetPinStateOfKey2(void)
{
	if(GPIO_ReadInputDataBit(KEY2_PORT,KEY2_PIN)==1)
	{
			return 1;
	}
	else
	{
			return 0;
	}
}

/*
************************************************************************************************************************
*                              ����2�ܽų�ʼ������
*
* �����������ڰ���2ʹ��֮ǰ�����ȵ���
*
*     ��������
*
*   ����ֵ����
*
************************************************************************************************************************
*/

void Key2_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	GPIO_InitStructure.GPIO_Pin=KEY2_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;
	GPIO_Init(KEY2_PORT,&GPIO_InitStructure);
}
/***************************************�û��Զ��庯������***********************************/



/*
************************************************************************************************************************
*                              ������ʱ����
*
* �����������û����û�ж�ʱ���ã���û�ж���KEY_FIXED_PERIOD������Ҫ���¸���CPU��д�˺�����
*
*     ������xms        ���ݲ�ͬ����������ʱ�䣬һ����20ms
*
*   ����ֵ����
*
*     ע�⣺������ʱ
*
************************************************************************************************************************
*/
#ifndef  KEY_FIXED_PERIOD
void key_delay1ms(uint8_t xms)
{
    
}
#endif


/*					�������ݲ���Ҫ���ģ�ֱ��ʹ�ü���								*/
/*
************************************************************************************************************************
*                              ������������
*
* ����������ʹ�ð���ǰ��Ҫ����������
*
*     ������p_Key        ָ�򰴼����Ʊ�����ָ��
*           p_CallBack   ������ȡ������ƽ�Ļص�����ָ�룬��Ϊ��ͬCPU��ƽ��ȡ�ķ�ʽ��һ�����ⲿ����Ҫ�û��Լ���д
*                        α�������£�
*
*                          uint8_t GetPinStateOfKeyXX(void)
*                          {
*                            ����ܽŵĵ�ƽ�ǰ�������ʱ�ĵ�ƽ
*                            ����1
*
*                            ����ܽŵĵ�ƽ�ǰ�������ʱ�ĵ�ƽ
*                            ����0
*                          }
*
*   ����ֵ������������ڱ�����״̬�ĵ�ƽ������1��
*           ����������ڵ���״̬�ĵ�ƽ������0��
*
************************************************************************************************************************
*/
void KeyCreate(KEY *p_Key,KEY_CALLBACK_PTR p_CallBack)
{
  p_Key->GetStatePtr=p_CallBack;
  p_Key->Times=0;
  p_Key->State=KEY_UP;
  p_Key->StateChange=NOT_CHANGED;
#ifdef KEY_FIXED_PERIOD
	p_Key->Time_ms=0;          //����ʱ���ʼ��Ϊ0
#endif
  
#if USER_DATA_EN>0             
	p_Key->User_Data=0;		           //�û������������û�����ʹ��
#endif
  
}

/*
************************************************************************************************************************
*                              ����״̬���º���
*
* ������������������������ֻ�ʺ�STM32��Ƭ���⿪�����������͵ĵ�Ƭ�����������޸ġ�
*
*     ������p_Key        ָ�򰴼����Ʊ�����ָ��
*
*   ����ֵ����
*
*     ע�⣺����Ƶ����Ҫ����20Hz
*
************************************************************************************************************************
*/
void Key_RefreshState(KEY* p_Key)
{
  switch(p_Key->State)
  {
    case KEY_UP:
    {
      if((*(p_Key->GetStatePtr))())//ִ�лص������жϰ����ܽ�״̬
      {
#ifdef  KEY_FIXED_PERIOD   
        p_Key->Time_ms = 0;
        p_Key->State = KEY_DOWN_WOBBLE;//����������ʱ
#else
        p_Key->State = KEY_DOWN_WOBBLE;
        key_delay1ms(KEY_WOBBLE_TIME);				
        if((*(p_Key->GetStatePtr))())
        {
          p_Key->StateChange=CHANGED;
          p_Key->State = KEY_DOWN;
        }
#endif
      }
    }break;
    
    #ifdef  KEY_FIXED_PERIOD
    case KEY_DOWN_WOBBLE:
    {
      p_Key->Time_ms += KEY_FIXED_PERIOD;
      if( p_Key->Time_ms >=KEY_WOBBLE_TIME )
      {
        if((*(p_Key->GetStatePtr))())
        {
          p_Key->StateChange=CHANGED;
          p_Key->State = KEY_DOWN;
        }
        else
        {
          p_Key->State = KEY_UP;
        }
      }
    }break;
    #endif

    case KEY_DOWN:
    {
      if( (*(p_Key->GetStatePtr))() == 0 )
      {
#ifdef  KEY_FIXED_PERIOD
        p_Key->Time_ms = 0;
        p_Key->State = KEY_UP_WOBBLE;//����������ʱ
#else
        key_delay1ms(KEY_WOBBLE_TIME);
        if( (*(p_Key->GetStatePtr))() == 0 )
        {
          p_Key->StateChange=CHANGED;
          p_Key->State = KEY_UP;
          p_Key->Times++;
          if( p_Key->Times > 250)
            p_Key->Times = 250;//���������250��û����
        }
#endif
      }
    }break;

#ifdef  KEY_FIXED_PERIOD
    case KEY_UP_WOBBLE:
    {
      p_Key->Time_ms += KEY_FIXED_PERIOD;
      if( p_Key->Time_ms >= KEY_WOBBLE_TIME )
      {
        if( (*(p_Key->GetStatePtr))()==0)
        {
          p_Key->StateChange=CHANGED;
          p_Key->State = KEY_UP;
          p_Key->Times++;
          if( p_Key->Times > 250)
            p_Key->Times = 250;//���������250��û����
        }
        else
        {
          p_Key->State = KEY_DOWN;
        }
      }
    }break;
#endif
  }
}

/*
************************************************************************************************************************
*                              ���������´������ʺ���
*
* �����������԰��������´����ķ���
*
*     ������p_Key        ָ�򰴼����Ʊ�����ָ��
*           option	     ѡ����ڰ�������һ��ֻ�õ�һ�����ֲ��������������ľ�û�а�������
*                        KEY_ACCESS_READ         ��ȡ���������µĴ���
*                        KEY_ACCESS_WRITE_CLEAR  ���㰴�������µĴ���
*   ����ֵ��->times��ֵ��
*
*     ע�⣺���ʹ���û�����ź��������������ͬһ����������µ���Key_AccessTimes��Key_RefreshState
*           ��û��ʲô��ϵ�ģ�����ϵͳ������������������������о�Ҫ�����ٽ�α����ˣ�������������
*           һ�㶼Ҫ��һ��������ʹ�ã���Ϊÿ�ε���Key_RefreshState��Times���п��ܱ����£����ں���
*           ��ѯ���
*
************************************************************************************************************************
*/

uint8_t Key_AccessTimes(KEY* p_Key,ACCESS_TYPE opt)
{
	uint8_t times_temp;
	
	if((opt&KEY_ACCESS_WRITE_CLEAR) == KEY_ACCESS_WRITE_CLEAR)
	{
			p_Key->Times = 0;					
	}
		
	times_temp = p_Key->Times;
	
	return times_temp;
}

/*
************************************************************************************************************************
*                              ����״̬���ʺ���
*
* �����������԰���������״̬�ķ���
*
*     ������p_Key        ָ�򰴼����Ʊ�����ָ��

*   ����ֵ��->State��ֵ��
*
*     ע�⣺1.���ʹ���û�����ź��������������ͬһ����������µ���Key_AccessTimes��Key_RefreshState
*           ��û��ʲô��ϵ�ģ�����ϵͳ������������������������о�Ҫ�����ٽ�α����ˣ�������������
*           һ�㶼Ҫ��һ��������ʹ�ã���Ϊÿ�ε���Key_RefreshState��Times���п��ܱ����£����ں���
*           ��ѯ���
************************************************************************************************************************
*/
uint8_t Key_AccessState(KEY* p_Key,KEY_STATE *p_State)
{
  uint8_t StateChange=p_Key->StateChange;
  //��ȡ״̬
  *p_State = p_Key->State;
  //��ȡ�����״̬�Ƿ񱻸ı�
  p_Key->StateChange=NOT_CHANGED;
  //����״̬�Ƿ�ı�
  return StateChange;
}
