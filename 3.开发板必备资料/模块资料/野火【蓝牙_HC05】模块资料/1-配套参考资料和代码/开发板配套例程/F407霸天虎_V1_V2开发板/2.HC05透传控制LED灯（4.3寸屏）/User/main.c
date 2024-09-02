/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   HC05蓝牙模块测试程序
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F407 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_blt_usart.h"
#include "./usart/bsp_debug_usart.h"
#include "./systick/bsp_SysTick.h"
#include "./hc05/bsp_hc05.h"
#include "./key/bsp_key.h" 
#include "./lcd/bsp_nt35510_lcd.h"
#include <string.h>
#include <stdlib.h>
#include "./dwt_delay/core_delay.h"   
#include "./i2c/i2c.h"
#include "./mpu6050/mpu6050.h"



extern ReceiveData DEBUG_USART_ReceiveData;
extern ReceiveData BLT_USART_ReceiveData;
extern int hc05_inquery_connect;

//函数声明
void TransData_CtrlLED_Test(void);


/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
  char hc05_name[30]="HC05_SLAVE";
	char hc05_nameCMD[40];
  
  unsigned char mpu_reg = 0; //保存读取MPU6050寄存器的值
  
  
	/* 延时函数初始化 */
  CPU_TS_TmrInit();

  /* LCD初始化 */
  #ifdef ENABLE_LCD_DISPLAY
	NT35510_Init();	
	NT35510_GramScan( 6 );
	LCD_SetFont(&Font16x32);
	LCD_SetColors(RED,BLACK);
  NT35510_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* 清屏，显示全黑 */
  #endif
	
  /* 调试串口初始化 USART1 配置模式为 115200 8-N-1 接收中断 */
	Debug_USART_Config();

	LED_GPIO_Config();
	Key_GPIO_Config();
	
  /* HC05蓝牙模块初始化：GPIO 和 USART3 配置模式为 38400 8-N-1 接收中断 */
	if(HC05_Init() == 0)
	{
		HC05_INFO("HC05模块检测正常。");
		#ifdef ENABLE_LCD_DISPLAY
		NT35510_DispString_EN ( 40, 60, "HC05 module detected!" );
    #endif
	}
	else
	{
		HC05_ERROR("HC05模块检测不正常，请检查模块与开发板的连接，然后复位开发板重新测试。");
		#ifdef ENABLE_LCD_DISPLAY
		NT35510_DispString_EN ( 20, 60, "No HC05 module detected!"  );
		NT35510_DispString_EN ( 5, 100, "Please check the hardware connection and reset the system." );
    #endif
    
		while(1);
	}


	HC05_INFO("**********HC05模块实验************");
	#ifdef ENABLE_LCD_DISPLAY
	NT35510_DispString_EN ( 40, 20, "HC05 BlueTooth Demo" );
  #endif
  
	/*复位、恢复默认状态*/
	HC05_Send_CMD("AT+RESET\r\n",1);	//复位指令发送完成之后，需要一定时间HC05才会接受下一条指令
	HC05_Send_CMD("AT+ORGL\r\n",1);

	
	/*各种命令测试演示，默认不显示。
	 *在bsp_hc05.h文件把HC05_DEBUG_ON 宏设置为1，
	 *即可通过串口调试助手接收调试信息*/	
	
	HC05_Send_CMD("AT+VERSION?\r\n",1);
	
	HC05_Send_CMD("AT+ADDR?\r\n",1);
	
	HC05_Send_CMD("AT+UART?\r\n",1);
	
	HC05_Send_CMD("AT+CMODE?\r\n",1);
	
	HC05_Send_CMD("AT+STATE?\r\n",1);	

	HC05_Send_CMD("AT+ROLE=0\r\n",1);
	
	/*初始化SPP规范*/
	HC05_Send_CMD("AT+INIT\r\n",1);
	HC05_Send_CMD("AT+CLASS=0\r\n",1);
	HC05_Send_CMD("AT+INQM=1,9,48\r\n",1);
	
	/*设置模块名字*/
	sprintf(hc05_nameCMD,"AT+NAME=%s\r\n",hc05_name);
	HC05_Send_CMD(hc05_nameCMD,1);

	HC05_INFO("本模块名字为:%s ,模块已准备就绪。",hc05_name);
	#ifdef ENABLE_LCD_DISPLAY
	NT35510_DispStringLine_EN ( (LINE(4)), "ReceiveData USART1" );	
  NT35510_DispStringLine_EN ( (LINE(12)), "ReceiveData HC-05" );
  #endif
  
  
  
  /* 这部分是额外的代码
     目的是为了防止板载 MPU6050 的INT引脚干扰STM32 PF10引脚的电平变化
     使用霸天虎开发板这部分不可去掉，否则影响HC05的连接状态的检测
  */
	I2cMaster_Init(); //初始化 I2C
	if( ! MPU6050ReadID() ) {printf("无法读取MPU6050\r\n"); while(1);}
  MPU6050_WriteReg(MPU6050_RA_INT_PIN_CFG, (mpu_reg |((1<<MPU6050_INTCFG_INT_LEVEL_BIT) | 
                                                      (1<<MPU6050_INTCFG_INT_OPEN_BIT) | 
                                                      (1<<MPU6050_INTCFG_LATCH_INT_EN_BIT)) ) );
  
  
  /* SysTick 10ms中断初始化 */
  SysTick_Init();
  

	while(1)
	{
    //每 5s 检查一次蓝牙连接
    if( 1 == hc05_inquery_connect )
    {
      hc05_inquery_connect = 0; //清零标志位

      if( ! IS_HC05_CONNECTED() )
      {
        HC05_Send_CMD("AT+INQ\r\n",1);//模块在查询状态，才能容易被其它设备搜索到
        Usart_SendString( DEBUG_USART, "蓝牙尚未连接。请用手机打开蓝牙调试助手搜索连接蓝牙\r\n" );
      }
      else
      {
        Usart_SendString( DEBUG_USART, "蓝牙已连接。发送“RED_LED”可控制翻转LED灯\r\n" );
      }
    }
    
    TransData_CtrlLED_Test(); //处理数据
    
    
	}
  
  
}


/**
  * @brief  处理串口数据控制LED灯
  * @param  无
  * @retval 无
  */
void TransData_CtrlLED_Test(void)
{
  /* 处理调试串口接收到的串口助手数据 */
  if(DEBUG_USART_ReceiveData.receive_data_flag == 1)
  {
    DEBUG_USART_ReceiveData.uart_buff[DEBUG_USART_ReceiveData.datanum] = 0;
    //在这里可以自己定义想要接收的字符串然后处理
    //这里接收到串口调试助手发来的 “RED_LED”就会把板子上面的红灯取反一次
    if( strstr((char *)DEBUG_USART_ReceiveData.uart_buff,"RED_LED") )
    {
      LED1_TOGGLE;
    }

    //如果手机蓝牙连接了模块，则发送到手机端显示数据
    if( IS_HC05_CONNECTED() )
    {
      BLT_KEY_LOW;
      Usart_SendStr_length(BLT_USARTx, DEBUG_USART_ReceiveData.uart_buff, DEBUG_USART_ReceiveData.datanum);	
    }
    //如果蓝牙没有被连接，如果数据是以AT开头的，就把KEY置高，设置蓝牙模块
    else if( strstr((char *)DEBUG_USART_ReceiveData.uart_buff,"AT") == (char *)DEBUG_USART_ReceiveData.uart_buff )
    {
      BLT_KEY_HIGHT;
      Usart_SendStr_length(BLT_USARTx, DEBUG_USART_ReceiveData.uart_buff, DEBUG_USART_ReceiveData.datanum);
      Usart_SendStr_length(BLT_USARTx,"\r\n",2);
      BLT_KEY_LOW;
    }
    
    //串口助手显示接收到的数据
    Usart_SendString( DEBUG_USART, "\r\nrecv USART1 data:\r\n" );
    Usart_SendString( DEBUG_USART, DEBUG_USART_ReceiveData.uart_buff );
    Usart_SendString( DEBUG_USART, "\r\n" );
    //LCD显示接收到的数据
    #ifdef ENABLE_LCD_DISPLAY
    LCD_ClearLine(LINE(5));
    LCD_ClearLine(LINE(6));
    LCD_ClearLine(LINE(7));
    LCD_ClearLine(LINE(8));
    NT35510_DispStringLine_EN ( (LINE(5)), (char *)DEBUG_USART_ReceiveData.uart_buff );
    #endif
    
    //清零调试串口数据缓存
    DEBUG_USART_ReceiveData.receive_data_flag = 0;		//接收数据标志清零
    DEBUG_USART_ReceiveData.datanum = 0;               
  }
  
  
  /* 处理蓝牙串口接收到的蓝牙数据 */
  if(BLT_USART_ReceiveData.receive_data_flag == 1)
  {
    BLT_USART_ReceiveData.uart_buff[BLT_USART_ReceiveData.datanum] = 0;
    //在这里可以自己定义想要接收的字符串然后处理
    //这里接收到手机蓝牙发来的 “RED_LED”就会把板子上面的红灯取反一次
    if(strstr((char *)BLT_USART_ReceiveData.uart_buff,"RED_LED"))
    {
      LED1_TOGGLE;
    }
    
    //串口助手显示接收到的数据
    Usart_SendString( DEBUG_USART, "\r\nrecv HC-05 data:\r\n" );
    Usart_SendString( DEBUG_USART, BLT_USART_ReceiveData.uart_buff );
    Usart_SendString( DEBUG_USART, "\r\n" );
    
    //LCD显示接收到的数据
    #ifdef ENABLE_LCD_DISPLAY
    LCD_ClearLine(LINE(13));
    LCD_ClearLine(LINE(14));
    LCD_ClearLine(LINE(15));
    LCD_ClearLine(LINE(16));
    NT35510_DispStringLine_EN ( (LINE(13)), (char *)BLT_USART_ReceiveData.uart_buff );
    #endif
    
    //清零蓝牙串口数据缓存
    BLT_USART_ReceiveData.receive_data_flag = 0;		//接收数据标志清零
    BLT_USART_ReceiveData.datanum = 0;  
  }
}




/*********************************************END OF FILE**********************/

