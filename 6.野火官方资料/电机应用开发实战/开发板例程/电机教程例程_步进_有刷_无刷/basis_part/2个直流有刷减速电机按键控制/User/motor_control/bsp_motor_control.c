/**
  ******************************************************************************
  * @file    bsp_motor_control.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   ������ƽӿ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F103 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 

#include ".\motor_control\bsp_motor_control.h"
#include "./usart/bsp_debug_usart.h"
#include <math.h>
#include <stdlib.h>

/* ˽�б��� */
static motor_dir_t direction  = MOTOR_FWD;      // ��¼���1����
static uint16_t    dutyfactor = 0;              // ��¼���1ռ�ձ�
static motor_dir_t direction2  = MOTOR_FWD;     // ��¼���2����
static uint16_t    dutyfactor2 = 0;             // ��¼���2ռ�ձ�

void sd_gpio_config(void)
{
   GPIO_InitTypeDef GPIO_InitStruct;
  
  /* ��ʱ��ͨ���������Ŷ˿�ʱ��ʹ�� */
	
	ENA_GPIO_CLK_ENABLE();
	ENB_GPIO_CLK_ENABLE();
  
  /* ��ʱ��ͨ��1��������IO��ʼ�� */
	/*�����������*/
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	/*������������ */ 
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	
	/*ѡ��Ҫ���Ƶ�GPIO����*/	
	GPIO_InitStruct.Pin = ENA_PIN;
	/*���ÿ⺯����ʹ���������õ�GPIO_InitStructure��ʼ��GPIO*/
  HAL_GPIO_Init(ENA_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = ENB_PIN;	
  HAL_GPIO_Init(ENB_GPIO_PORT, &GPIO_InitStruct);
}

/**
  * @brief  �����ʼ��
  * @param  ��
  * @retval ��
  */
void motor_init(void)
{
  TIMx_Configuration();     // ��ʼ����� 1
  TIMx_Configuration2();    // ��ʼ����� 2
  sd_gpio_config();
}

/**
  * @brief  ���õ���ٶ�
  * @param  v: �ٶȣ�ռ�ձȣ�
  * @retval ��
  */
void set_motor_speed(uint16_t v)
{
  dutyfactor = v;
  
  if (direction == MOTOR_FWD)
  {
    SET_FWD_COMPAER(dutyfactor);     // �����ٶ�
  }
  else
  {
    SET_REV_COMPAER(dutyfactor);     // �����ٶ�
  }
}

/**
  * @brief  ���õ������
  * @param  ��
  * @retval ��
  */
void set_motor_direction(motor_dir_t dir)
{
  direction = dir;
  
  if (direction == MOTOR_FWD)
  {
    SET_FWD_COMPAER(dutyfactor);     // �����ٶ�
    SET_REV_COMPAER(0);              // �����ٶ�
  }
  else
  {
    SET_FWD_COMPAER(0);              // �����ٶ�
    SET_REV_COMPAER(dutyfactor);     // �����ٶ�
  }
}

/**
  * @brief  ʹ�ܵ��
  * @param  ��
  * @retval ��
  */
void set_motor_enable(void)
{
  MOTOR_ENABLE_A();
  MOTOR_FWD_ENABLE();
  MOTOR_REV_ENABLE();
}

/**
  * @brief  ���õ��
  * @param  ��
  * @retval ��
  */
void set_motor_disable(void)
{
  MOTOR_DISABLE_A();
  MOTOR_FWD_DISABLE();
  MOTOR_REV_DISABLE();
}

/**
  * @brief  ���õ���ٶ�
  * @param  v: �ٶȣ�ռ�ձȣ�
  * @retval ��
  */
void set_motor2_speed(uint16_t v)
{
  dutyfactor2 = v;
  
  if (direction2 == MOTOR_FWD)
  {
    SET2_FWD_COMPAER(dutyfactor2);     // �����ٶ�
  }
  else
  {
    SET2_REV_COMPAER(dutyfactor2);     // �����ٶ�
  }
}

/**
  * @brief  ���õ������
  * @param  ��
  * @retval ��
  */
void set_motor2_direction(motor_dir_t dir)
{
  direction2 = dir;
  
  if (direction2 == MOTOR_FWD)
  {
    SET2_FWD_COMPAER(dutyfactor2);     // �����ٶ�
    SET2_REV_COMPAER(0);              // �����ٶ�
  }
  else
  {
    SET2_FWD_COMPAER(0);              // �����ٶ�
    SET2_REV_COMPAER(dutyfactor2);     // �����ٶ�
  }
}

/**
  * @brief  ʹ�ܵ��
  * @param  ��
  * @retval ��
  */
void set_motor2_enable(void)
{
  MOTOR_ENABLE_B();
  MOTOR2_FWD_ENABLE();
  MOTOR2_REV_ENABLE();
}

/**
  * @brief  ���õ��
  * @param  ��
  * @retval ��
  */
void set_motor2_disable(void)
{
  MOTOR_DISABLE_B();
  MOTOR2_FWD_DISABLE();
  MOTOR2_REV_DISABLE();
}
#if 0
/**
  * @brief  ��ӡ��������
  * @param  ��
  * @retval ��
  */
void show_help(void)
{
    printf("����������������������������Ұ��ֱ�����ٵ��������ʾ���򡪡�������������������������\n\r");
    printf("��������(�Իس�����)��\n\r");
    printf("< ? >       -�����˵�\n\r");
    printf("v1[data]     -���õ�����ٶȣ���Χ��0��%d��\n\r", PWM_MAX_PERIOD_COUNT);
    printf("d1[data]     -���õ���ķ���%d:����ת��%d:����ת\n\r", MOTOR_FWD, MOTOR_REV);
    printf("v2[data]     -���õ�����ٶȣ���Χ��0��%d��\n\r", PWM2_MAX_PERIOD_COUNT);
    printf("d2[data]     -���õ���ķ���%d:����ת��%d:����ת\n\r", MOTOR_FWD, MOTOR_REV);
}

/**
  * @brief  �����ڽ��յ�������
  * @param  ��
  * @retval ��
  */
void deal_serial_data(void)
{
    static char showflag =1;
    int dec_temp=0;
    int speed_temp=0;
    
    //���յ���ȷ��ָ���Ϊ1
    char okCmd = 0;
  
    if (showflag)
    {
      show_help();
      showflag = !showflag;
    }

    //����Ƿ���յ�ָ��
    if(receive_cmd == 1)
    {
      if(UART_RxBuffer[0] == 'v' || UART_RxBuffer[0] == 'V')
      {
        //�����ٶ�
        if(UART_RxBuffer[1] == '1')
        {
          if (UART_RxBuffer[2] == ' ')
          {
            speed_temp = atoi((char const *)UART_RxBuffer+3);
            if(speed_temp>=0 && speed_temp <= PWM_MAX_PERIOD_COUNT)
            {
              set_motor_speed(speed_temp);
              printf("\n\r���1�ٶ�: %d\n\r", speed_temp);
              okCmd = 1;
            }
          }
        }
        //�����ٶ�
        if(UART_RxBuffer[1] == '2')
        {
          if (UART_RxBuffer[2] == ' ')
          {
            speed_temp = atoi((char const *)UART_RxBuffer+3);
            if(speed_temp>=0 && speed_temp <= PWM2_MAX_PERIOD_COUNT)
            {
              set_motor2_speed(speed_temp);
              printf("\n\r���2�ٶ�: %d\n\r", speed_temp);
              okCmd = 1;
            }
          }
        }
      }
      else if(UART_RxBuffer[0] == 'd')
      {
        if (UART_RxBuffer[1] == '1')
        {
          //���÷���
          if(UART_RxBuffer[2] == ' ')
          {
            dec_temp = atoi((char const *)UART_RxBuffer+3);

            if(dec_temp>=0)
            {
              set_motor_direction(dec_temp);
              printf("\n\r���1����:%s\n\r", dec_temp ? "����ת" : "����ת");
              okCmd = 1;
            }
          }
        }
        if (UART_RxBuffer[1] == '2')
        {
          //���÷���
          if(UART_RxBuffer[2] == ' ')
          {
            dec_temp = atoi((char const *)UART_RxBuffer+3);

            if(dec_temp>=0)
            {
              set_motor2_direction(dec_temp);
              printf("\n\r���2����:%s\n\r", dec_temp ? "����ת" : "����ת");
              okCmd = 1;
            }
          }
        }
      }
      else if(UART_RxBuffer[0] == '?')
      {
        //��ӡ��������
        show_help();
        okCmd = 1;
      }
      //���ָ���������ӡ��������
      if(okCmd != 1)
      {
        printf("\n\r ������������������...\n\r");
        show_help();
      }

      //��մ��ڽ��ջ�������
      receive_cmd = 0;
      uart_FlushRxBuffer();

    }
}
#endif
/*********************************************END OF FILE**********************/

