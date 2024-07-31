#include "stm32f10x.h"
#include "./led/bsp_led.h" 
#include "./usart/bsp_usart.h" 
#include <stdio.h> // ��ӱ�׼���������ͷ�ļ�

static void Show_Message(void); // ��ӡָ��������ʾ��Ϣ
  
int main(void)
{	
  char i;
  
  // 1.��ʼ��led
  LED_GPIO_Config();
  
  // 2.��ʼ������
  USART_Config();
	
  // 3.��ӡ��ʾ��Ϣ
  Show_Message();
  while(1)
	{	
    /* ��ȡ�ַ�ָ�� */
    i = getchar();
    printf("���յ��ַ���%c\n", i); // ��ӡ�ַ�������ASCIIֵ

      /* �����ַ�ָ�����RGB�ʵ���ɫ */
      switch(i)
      {
        case '1': // ʹ���ַ����бȽ�
          LED_RED; // 1.��ɫ
          break;
        case '2':
          LED_GREEN; // 2.��ɫ
          break; 
        case '3':
          LED_BLUE; // 3.��ɫ
          break;
        case '4':
          LED_YELLOW; // 4.��ɫ
          break;
        case '5':
          LED_PURPLE; // 5.��ɫ
          break;
        case '6':
          LED_CYAN; // 6.��ɫ
          break;
        case '7':
          LED_WHITE; // 7.��ɫ
          break;
        case '8':
          LED_RGBOFF; // 8.��
          break;
        default:
          Show_Message();
          break;      
      }   
	}	
}

// ��ӡָ��������ʾ��Ϣ
static void Show_Message(void)
{
  printf("\r\n   ����һ��ͨ������ͨ��ָ�����RGB�ʵ�ʵ�� \n");
  printf("ʹ��  USART  ����Ϊ��%d 8-N-1 \n", DEBUG_USART_BAUDRATE);
  printf("������ӵ�ָ������RGB�ʵ���ɫ��ָ���Ӧ���£�\n");
  printf("   ָ��   ------ �ʵ���ɫ \n");
  printf("     1    ------    �� \n");
  printf("     2    ------    �� \n");
  printf("     3    ------    �� \n");
  printf("     4    ------    �� \n");
  printf("     5    ------    �� \n");
  printf("     6    ------    �� \n");
  printf("     7    ------    �� \n");
  printf("     8    ------    �� \n");  
}
