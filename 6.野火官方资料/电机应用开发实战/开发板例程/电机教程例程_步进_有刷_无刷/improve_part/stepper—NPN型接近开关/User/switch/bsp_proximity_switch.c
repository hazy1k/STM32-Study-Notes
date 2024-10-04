/**
  ******************************************************************************
  * @file    bsp_proximity_switch.c
  * @author  fire
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   �ӽ������ж�Ӧ��bsp
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F103 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./switch/bsp_proximity_switch.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_debug_usart.h"

/* �ӽ��������飬�����������ӽ����ص����� */
ProximitySwitch_TypeDef proximity_switch[4] = 
{
  {SWITCH1_INT_GPIO_PORT, SWITCH1_INT_GPIO_PIN, GPIO_MODE_IT_FALLING, SWITCH1_INT_EXTI_IRQ},
  {SWITCH2_INT_GPIO_PORT, SWITCH2_INT_GPIO_PIN, GPIO_MODE_IT_FALLING, SWITCH2_INT_EXTI_IRQ},
};

 /**
  * @brief  ���ýӽ�����IO�ڣ��������ж����ȼ�
  * @param  ��
  * @retval ��
  */
void ProximitySwitch_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure; 
  
  /*��������GPIO�ڵ�ʱ��*/
  SWITCH1_INT_GPIO_CLK_ENABLE();
  SWITCH2_INT_GPIO_CLK_ENABLE();

  uint8_t size = sizeof(proximity_switch)/sizeof(ProximitySwitch_TypeDef);

  for(uint8_t i = 0; i < size; i++)
  {
    /* ѡ��ӽ����ص����� */ 
    GPIO_InitStructure.Pin = proximity_switch[i].pin;
    /* ��������Ϊ�½��ش���ģʽ */ 
    GPIO_InitStructure.Mode = proximity_switch[i].gpio_mode;
    /* �������Ų�����Ҳ������ */
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    /* ʹ������Ľṹ���ʼ������ */
    HAL_GPIO_Init(proximity_switch[i].port, &GPIO_InitStructure);
    
    /* ���� EXTI �ж�Դ �����š������ж����ȼ�*/
    HAL_NVIC_SetPriority(proximity_switch[i].IRQn, 1, 1);
    /* ʹ���ж� */
    HAL_NVIC_EnableIRQ(proximity_switch[i].IRQn);
  }
}

/**
  * @brief  �ⲿ�жϻص�����
  * @param  GPIO_Pin�������ⲿ�жϵ�IO����
  * @retval ��
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  switch(GPIO_Pin)
  {
    case SWITCH1_INT_GPIO_PIN:
      LED1_TOGGLE;
      printf("�ӽ�����1�Դ���\r\n");
      break;
    case SWITCH2_INT_GPIO_PIN:
      LED2_TOGGLE;
      printf("�ӽ�����2�Դ���\r\n");
      break;
    default:
      break;
  }
}
/*********************************************END OF FILE**********************/
