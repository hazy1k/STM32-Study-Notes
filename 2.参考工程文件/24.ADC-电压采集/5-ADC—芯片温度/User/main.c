// 通过ADC1通道16获取芯片内部温度，并通过串口打印到电脑串口调试助手显示

#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./temp/bsp_tempad.h"

// ADC1转换的电压值通过MDA方式传到sram
extern __IO u16 ADC_ConvertedValue;
//计算后的温度值
u16 Current_Temp;	 

void Delay(__IO u32 nCount)
{
  for(; nCount != 0; nCount--);
} 

int main(void)
{		 
	USART_Config();

	Temp_ADC1_Init();

	printf("\r\n 这是一个内部温度传感器实验 \r\n");
	printf( "\r\n Print current Temperature  \r\n");	

  while (1)
  { 
	Delay(0xffffee);      // 延时 
	// 计算方法1            	                 									  
 	// Current_Temp= (1.43- ADC_ConvertedValue*3.3/4096)*1000 / 4.3+ 25 ;
	
	//计算方法2
	Current_Temp=(V25-ADC_ConvertedValue)/AVG_SLOPE+25;	

	//10进制显示
	printf("\r\n The IC current tem= %3d ℃\r\n", Current_Temp);	      
 	 
	//16进制显示 						 
 	// printf("\r\n The current temperature= %04x \r\n", Current_Temp);		 
  }
}

