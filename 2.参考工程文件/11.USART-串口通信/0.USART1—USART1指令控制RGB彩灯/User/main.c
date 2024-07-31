#include "stm32f10x.h"
#include "./led/bsp_led.h" 
#include "./usart/bsp_usart.h" 
#include <stdio.h> // 添加标准输入输出库头文件

static void Show_Message(void); // 打印指令输入提示信息
  
int main(void)
{	
  char i;
  
  // 1.初始化led
  LED_GPIO_Config();
  
  // 2.初始化串口
  USART_Config();
	
  // 3.打印提示信息
  Show_Message();
  while(1)
	{	
    /* 获取字符指令 */
    i = getchar();
    printf("接收到字符：%c\n", i); // 打印字符而不是ASCII值

      /* 根据字符指令控制RGB彩灯颜色 */
      switch(i)
      {
        case '1': // 使用字符进行比较
          LED_RED; // 1.红色
          break;
        case '2':
          LED_GREEN; // 2.绿色
          break; 
        case '3':
          LED_BLUE; // 3.蓝色
          break;
        case '4':
          LED_YELLOW; // 4.黄色
          break;
        case '5':
          LED_PURPLE; // 5.紫色
          break;
        case '6':
          LED_CYAN; // 6.青色
          break;
        case '7':
          LED_WHITE; // 7.白色
          break;
        case '8':
          LED_RGBOFF; // 8.灭
          break;
        default:
          Show_Message();
          break;      
      }   
	}	
}

// 打印指令输入提示信息
static void Show_Message(void)
{
  printf("\r\n   这是一个通过串口通信指令控制RGB彩灯实验 \n");
  printf("使用  USART  参数为：%d 8-N-1 \n", DEBUG_USART_BAUDRATE);
  printf("开发板接到指令后控制RGB彩灯颜色，指令对应如下：\n");
  printf("   指令   ------ 彩灯颜色 \n");
  printf("     1    ------    红 \n");
  printf("     2    ------    绿 \n");
  printf("     3    ------    蓝 \n");
  printf("     4    ------    黄 \n");
  printf("     5    ------    紫 \n");
  printf("     6    ------    青 \n");
  printf("     7    ------    白 \n");
  printf("     8    ------    灭 \n");  
}
