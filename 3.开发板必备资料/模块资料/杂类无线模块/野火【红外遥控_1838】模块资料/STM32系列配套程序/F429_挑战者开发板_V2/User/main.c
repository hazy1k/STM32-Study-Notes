/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ����ң��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F429 ������
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
 
#include "stm32f4xx.h"
#include "./led/bsp_led.h"
#include "./IrDa/bsp_irda.h" 
#include "./systick/bsp_SysTick.h"
#include "./usart/bsp_debug_usart.h"

#define CLI()      __set_PRIMASK(1)		/* �ر����ж� */  
#define SEI() __set_PRIMASK(0)				/* �������ж� */ 

extern uint8_t  frame_flag;
extern uint8_t  isr_cnt;
extern uint8_t  frame_cnt;

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */ 
int main(void)
{	
	uint8_t key_val;
  
  /* config the led */
	LED_GPIO_Config();
	LED1_ON;
  
	/* ����SysTick Ϊ10us�ж�һ�� */
	SysTick_Init();
	
  /* ��������SysTick���ж����ȼ�Ϊ��ߣ�Ҫ��ȻSysTick��ʱ�ж���ռ����IO EXTI�ж�
   * ��ΪSysTick��ʼ��ʱĬ�����õ����ȼ�����͵�
   * ���ߵ�����������ʱ������ʱ��ʱ��Ҫ���ö�ʱ�������ȼ�����IO EXTI�жϵ����ȼ�
   */
  NVIC_SetPriority (SysTick_IRQn, 0);
  
  
  /* USART config 115200 8-N-1 */
	Debug_USART_Config();
	printf("\r\n ����һ������ң�ط��������ʵ�� \r\n");
  
  /* ��ʼ���������ͷCP1838�õ���IO */
	IrDa_Init();  

	for(;;)
	{	  
    if( frame_flag == 1 ) /* һ֡�������ݽ������ */
    {
      key_val = IrDa_Process();
      printf("\r\n key_val=%d \r\n",key_val);
      printf("\r\n ��������frame_cnt=%d \r\n",frame_cnt);
      printf("\r\n �жϴ���isr_cnt=%d \r\n",isr_cnt);
      
      /* ��ͬ��ң��������Ӧ��ͬ�ļ�ֵ����Ҫʵ�ʲ��� */
      switch( key_val )
      {
        case 0:
        LED1_TOGGLE;  
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n Error \r\n");
        break;
        
        case 162:
        LED1_TOGGLE;  
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n POWER \r\n");
        break;
        
        case 226:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n MENU \r\n");
        break;
        
        case 34:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n TEST \r\n");
        break;
        
        case 2:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n + \r\n");
        break;
        
        case 194:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n RETURN \r\n");
        break;
        
        case 224:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n |<< \r\n");
        break;
        
        case 168:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n > \r\n");
        break;
        
        case 144:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n >>| \r\n");
        break;
        
        case 104:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n 0 \r\n");
        break;
        
        case 152:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n - \r\n");
        break;
        
        case 176:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n C \r\n");
        break;
        
        case 48:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n 1 \r\n");
        break;
        
        case 24:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n 2 \r\n");
        break;
        
        case 122:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n 3 \r\n");
        break;
        
        case 16:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n 4 \r\n");
        break;
        
        case 56:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n 5 \r\n");
        break;
        
        case 90:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n 6 \r\n");
        break;
        
          case 66:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n 7 \r\n");
        break;
        
        case 74:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n 8 \r\n");
        break;
        
        case 82:
          LED1_TOGGLE;
          printf("\r\n key_val=%d \r\n",key_val);
          printf("\r\n 9 \r\n");
        break;
        
        default:       
        break;
      }      
    }
	}
}


/*********************************************END OF FILE**********************/

