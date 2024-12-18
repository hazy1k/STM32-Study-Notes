#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./i2c/bsp_i2c_ee.h"
#include "./led/bsp_led.h"
#include <string.h>

uint8_t cal_flag = 0;
uint8_t k;

// 用来存储小数和整数的缓冲区
long double double_buffer[7] = {0};
int int_bufffer[7] = {0};

#define DOUBLE_ADDR       10 // 小数数据存储地址
#define LONGINT_ADDR      70 // 长整数数据存储地址

int main(void)
{ 
  LED_GPIO_Config(); // led初始化
	USART_Config(); // usart初始化
	printf("\r\n 这是一个EEPROM 读写小数和长整数实验 \r\n");
	I2C_EE_Init(); // I2C外设初始化-EEPROM
   
  // 读取数据的标志位
  I2C_EE_BufferRead(&cal_flag, 0, 1); // 函数参数：读取数据地址、读取数据长度、存放数据的地址
  
  if(cal_flag != 0xCD )	/*若标志等于0xcd，表示之前已有写入数据*/
  {      
    printf("\r\n没有检测到数据标志，FLASH没有存储数据，即将进行小数写入实验\r\n");
    cal_flag = 0xCD; // 给一个标志位
    I2C_EE_BufferWrite(&cal_flag, 0, 1); // 写入标志位到FLASH 
    for(k = 0; k < 7; k++ ) // 生成要存放至EEPROM的数据
    {
      double_buffer[k] = k +0.1;
      int_bufffer[k]=k*500+1 ;
    }

    /*写入小数数据到地址10*/
    I2C_EE_BufferWrite((void*)double_buffer, DOUBLE_ADDR, sizeof(double_buffer));
    /*写入整数数据到地址60*/
    I2C_EE_BufferWrite((void*)int_bufffer, LONGINT_ADDR, sizeof(int_bufffer));
    printf("向芯片写入数据：");
    // 打印我们写入的数据
    for(k = 0; k < 7; k++ )
    {
      printf("小数tx = %LF\r\n",double_buffer[k]);
      printf("整数tx = %d\r\n",int_bufffer[k]);
    }
    printf("\r\n请复位开发板，以读取数据进行检验\r\n");      
	}    
  else // 此时EEPROM已经有数据
  {      
    printf("\r\n检测到数据标志\r\n");

		/*读取小数数据*/
    I2C_EE_BufferRead((void*)double_buffer, DOUBLE_ADDR, sizeof(double_buffer));
		/*读取整数数据*/
    I2C_EE_BufferRead((void*)int_bufffer, LONGINT_ADDR, sizeof(int_bufffer));
		printf("\r\n从芯片读到数据：\r\n");			
    for( k=0; k<7; k++ )
		{
			printf("小数 rx = %LF \r\n",double_buffer[k]);
			printf("整数 rx = %d \r\n",int_bufffer[k]);				
		}
  }   
  while (1)
  {      
  }
}