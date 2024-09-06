/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   软件驱动MPU6050
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F429 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "./usart/bsp_debug_usart.h"
#include "./led/bsp_led.h"
#include "./i2c/i2c.h"
#include "./mpu6050/mpu6050.h"


//设置是否使用LCD进行显示，不需要的话把这个宏注释掉即可
#define USE_LCD_DISPLAY

#ifdef USE_LCD_DISPLAY
 #include "./lcd/bsp_lcd.h"
#endif

/* MPU6050数据 */
short Acel[3];
short Gyro[3];
float Temp;

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	/* LED 端口初始化 */
  LED_GPIO_Config();

  /*初始化USART1*/
  Debug_USART_Config(); 
	
#ifdef USE_LCD_DISPLAY	
		 /*初始化液晶屏*/
		LCD_Init();
		LCD_LayerInit();
		LTDC_Cmd(ENABLE);
		
		/*把背景层刷黑色*/
		LCD_SetLayer(LCD_BACKGROUND_LAYER);  
		LCD_Clear(LCD_COLOR_BLACK);
		
		/*初始化后默认使用前景层*/
		LCD_SetLayer(LCD_FOREGROUND_LAYER); 
		/*默认设置不透明	，该函数参数为不透明度，范围 0-0xff ，0为全透明，0xff为不透明*/
		LCD_SetTransparency(0xFF);
		LCD_Clear(LCD_COLOR_BLACK);
		
			/*设置字体颜色及字体的背景颜色*/
		LCD_SetColors(LCD_COLOR_RED,LCD_COLOR_BLACK);	

		/*英文字体*/
		LCD_SetFont(&Font16x24); 	
		
		LCD_DisplayStringLine(LINE(1),(uint8_t* )"           This is a MPU6050 demo       ");			
		LCD_DisplayStringLine(LINE(2),(uint8_t* )"                 (Base Version)       ");		
#endif
	
  /* 配置SysTick定时器和中断 */
  SysTick_Init(); //配置 SysTick 为 1ms 中断一次，在中断里读取传感器数据
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; //启动定时器
	
	//初始化 I2C
	I2cMaster_Init(); 

	printf("\r\n 欢迎使用野火  STM32 F429 开发板。\r\n");		 
	printf("\r\n 这是一个MPU6050测试例程 \r\n");

  //MPU6050初始化
	MPU6050_Init();
	//检测MPU6050
	if( MPU6050ReadID() == 0 )
  {
		printf("\r\n没有检测到MPU6050传感器！\r\n");
		LED_RED;
    
    #ifdef USE_LCD_DISPLAY			
      /*设置字体颜色及字体的背景颜色*/
      LCD_SetColors(LCD_COLOR_BLUE,LCD_COLOR_BLACK);	

      LCD_DisplayStringLine(LINE(4),(uint8_t* )"No MPU6050 detected! ");			//野火自带的16*24显示
      LCD_DisplayStringLine(LINE(5),(uint8_t* )"Please check the hardware connection! ");			//野火自带的16*24显示
    #endif
    
		while(1);	//检测不到MPU6050 会红灯亮然后卡死
	}

  while(1)
  {
    if( task_readdata_finish ) //task_readdata_finish = 1 表示读取MPU6050数据完成
    {
      
      printf("加速度：%8d%8d%8d",Acel[0],Acel[1],Acel[2]);
      
      printf("    陀螺仪%8d%8d%8d",Gyro[0],Gyro[1],Gyro[2]);
      
      printf("    温度%8.2f\r\n",Temp);
      
      #ifdef USE_LCD_DISPLAY	
      {
        char cStr [ 70 ];
        sprintf ( cStr, "Acceleration:%8d%8d%8d",Acel[0],Acel[1],Acel[2] );	//加速度原始数据
        LCD_DisplayStringLine(LINE(7),(uint8_t* )cStr);			

        sprintf ( cStr, "Gyro        :%8d%8d%8d",Gyro[0],Gyro[1],Gyro[2] );	//角原始数据
        LCD_DisplayStringLine(LINE(8),(uint8_t* )cStr);			

        sprintf ( cStr, "Temperture  :%8.2f",Temp );	//温度值
        LCD_DisplayStringLine(LINE(9),(uint8_t* )cStr);			
      }
      #endif
      
      task_readdata_finish = 0; // 清零标志位
    }
  }

}




/*********************************************END OF FILE**********************/

