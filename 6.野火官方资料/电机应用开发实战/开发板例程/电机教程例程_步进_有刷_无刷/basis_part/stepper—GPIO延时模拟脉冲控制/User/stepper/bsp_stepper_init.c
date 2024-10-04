#include "./stepper/bsp_stepper_init.h"
#include "./led/bsp_led.h"   
#include "./delay/core_delay.h"   
#include "stm32f1xx.h"

/**
  * @brief  ���������ת
  * @param  tim         �������� ��λMS ����Խ��Ƶ��Խ�ߣ�ת��Խ�� ϸ��Ϊ1ʱ����10ms
  * @param  angle       ��Ҫת���ĽǶ�ֵ
  * @param  dir       ѡ������ת(ȡֵ��Χ��0,1) 
  * @param  subdivide   ϸ��ֵ
  * @note   ��
  * @retval ��
  */
void stepper_turn(int tim,float angle,float subdivide,uint8_t dir)  
{
  int n,i;
  /*����ϸ������ò���Ǳ��ֳɶ��ٸ�����*/
  n=(int)(angle/(1.8/subdivide));
  if(dir==CW) //˳ʱ��
  {
    MOTOR_DIR(CW);
  }
  else if(dir==CCW)//��ʱ��
  {
    MOTOR_DIR(CCW);
  }
  /*��ʹ��*/
  MOTOR_EN(HIGH);
  /*ģ�ⷽ��*/
  for(i=0;i<n;i++)
  {   
    MOTOR_PLU(HIGH);
    delay_us(tim/2);
    MOTOR_PLU(LOW);
    delay_us(tim/2);
  }
  /*��ʹ��*/
  MOTOR_EN(LOW);
}


/**
  * @brief  ���ų�ʼ��
  * @retval ��
  */
void stepper_Init()
{
  /*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
  GPIO_InitTypeDef  GPIO_InitStruct;

  /*����Motor��ص�GPIO����ʱ��*/
  MOTOR_DIR_GPIO_CLK_ENABLE();
  MOTOR_PUL_GPIO_CLK_ENABLE();
  MOTOR_EN_GPIO_CLK_ENABLE();

  /*ѡ��Ҫ���Ƶ�GPIO����*/                                 
  GPIO_InitStruct.Pin = MOTOR_DIR_PIN;  

  /*�������ŵ��������Ϊ�������*/
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;  

  GPIO_InitStruct.Pull =GPIO_PULLUP;

  /*������������Ϊ���� */   
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

  /*Motor �������� ��ʼ��*/
  HAL_GPIO_Init(MOTOR_DIR_GPIO_PORT, &GPIO_InitStruct); 

  /*Motor �������� ��ʼ��*/
  GPIO_InitStruct.Pin = MOTOR_PUL_PIN;  
  HAL_GPIO_Init(MOTOR_PUL_GPIO_PORT, &GPIO_InitStruct); 

  /*Motor ʹ������ ��ʼ��*/
  GPIO_InitStruct.Pin = MOTOR_EN_PIN; 
  HAL_GPIO_Init(MOTOR_EN_GPIO_PORT, &GPIO_InitStruct);  

  /*�ص�ʹ��*/
  MOTOR_EN(LOW);

}

