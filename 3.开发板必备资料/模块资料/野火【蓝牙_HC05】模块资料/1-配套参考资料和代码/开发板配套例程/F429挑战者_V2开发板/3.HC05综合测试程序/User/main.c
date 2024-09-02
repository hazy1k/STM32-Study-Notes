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
  * 实验平台:野火  STM32 F429 开发板 
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
#include "./lcd/bsp_lcd.h"
#include <string.h>
#include <stdlib.h>
#include "./dwt_delay/core_delay.h"   


extern uint32_t Task_Delay_Group[]; //此处作为他用：用于产生一个随机数
extern int hc05_inquery_connect;
extern int hc05_check_recvbuff;


char sendData[1024];
char linebuff[1024];

char hc05_nameCMD[40];
char disp_buff[200];

uint8_t hc05_role=0;
char hc05_mode[10]="SLAVE";
char hc05_name[30]="HC05_SLAVE";
char * linkhint = "请先断开蓝牙连接!!!\r\n";
char * linkhint_en = "Please disconnect the Bluetooth connection first";


//测试函数声明
void CheckConnect_LinkHC05_Test(void);
void CheckRecvBltBuff_Test(void);
void Switch_HC05Mode_Test(void);
void Generate_Modify_HC05Name(void);

/*
 * 函数名：main
 * 描述  ：主函数
 * 输入  ：无
 * 输出  ：无
 */
int main(void)
{ 	
	/* 延时函数初始化 */
  CPU_TS_TmrInit();

  
    /* 调试串口初始化 USART1 配置模式为 115200 8-N-1 接收中断 */
	Debug_USART_Config();
	
	LED_GPIO_Config();
	Key_GPIO_Config(); 
  
  
  /* 初始化液晶屏 */
  #ifdef ENABLE_LCD_DISPLAY
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
	LCD_SetColors(LCD_COLOR_RED,LCD_COLOR_BLACK);

	/*经过LCD_SetLayer(LCD_FOREGROUND_LAYER)函数后，
	以下液晶操作都在前景层刷新，除非重新调用过LCD_SetLayer函数设置背景层*/	
  
  LCD_ClearLine(LCD_LINE_0);
	LCD_DisplayStringLine( LCD_LINE_0,(uint8_t *)"HC05 BlueTooth Demo" );
  #endif
	
	
  /* HC05蓝牙模块初始化：GPIO 和 USART6 配置模式为 38400 8-N-1 接收中断 */
	if(HC05_Init() == 0)
	{
		HC05_INFO("HC05模块检测正常。");
    #ifdef ENABLE_LCD_DISPLAY
		LCD_DisplayStringLine( LCD_LINE_1,(uint8_t *)"HC05 module detected!" );
    #endif
	}
	else
	{
		HC05_ERROR("HC05模块检测不正常，请检查模块与开发板的连接，然后复位开发板重新测试。");
    #ifdef ENABLE_LCD_DISPLAY
		LCD_DisplayStringLine( LCD_LINE_1,(uint8_t *)"No HC05 module detected!"  );
		LCD_DisplayStringLine( LCD_LINE_2, (uint8_t *)"Please check the hardware connection and reset the system." );
    #endif
    
		while(1);
	}


  HC05_INFO("**********HC05模块实验************");
  
  
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
	
	sprintf(disp_buff,"Device name:%s",hc05_name);
  #ifdef ENABLE_LCD_DISPLAY
	LCD_DisplayStringLine( LCD_LINE_2, (uint8_t *)disp_buff );
  #endif


  /* SysTick 10ms中断初始化 */
  SysTick_Init();


	while(1)
	{

    //每 5s 检查一次蓝牙连接，搜索蓝牙模块，并进行连接
    if( 1 == hc05_inquery_connect )
    {
      hc05_inquery_connect = 0; //清零标志位

      CheckConnect_LinkHC05_Test();
    }
    
    //连接后每隔一段时间检查接收缓冲区
		if(1 == hc05_check_recvbuff)
		{
      hc05_check_recvbuff = 0; //清零标志位
      
      CheckRecvBltBuff_Test();
		}
		
    
		//如果KEY1被单击，切换master-slave模式
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON  )
		{
      Switch_HC05Mode_Test();
		}
		
		//如果KEY2被单击，随机生成一个名字
		if( Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON  )
		{
      Generate_Modify_HC05Name();
		}
		
	}

}



/**
  * @brief  检查蓝牙连接
  *         作为主机时，搜索蓝牙并连接名字含有“HC05”的蓝牙模块
  *         作为从机时，连接后通过蓝牙模块发送字符串
  * @param  无
  * @retval 无
  */
void CheckConnect_LinkHC05_Test(void)
{
  #ifdef ENABLE_LCD_DISPLAY
  static uint8_t color=0;  //定义切换LCD显示文字颜色的变量
  #endif
  
  if( ! IS_HC05_CONNECTED() )
  {
    Usart_SendString( DEBUG_USART, (uint8_t *)"蓝牙尚未连接\r\n" );
    
    #ifdef ENABLE_LCD_DISPLAY
    if(color==0)
    {
      color=1;
    }
    else
    {
      LCD_SetColors(LCD_COLOR_BLUE,LCD_COLOR_BLACK); //切换文字颜色为蓝色
      color=0;
    }
    #endif
    
    if(hc05_role == 1)	//主模式
    {
      HC05_INFO("正在扫描蓝牙设备...");
      
      #ifdef ENABLE_LCD_DISPLAY
      LCD_DisplayStringLine( LCD_LINE_4, (uint8_t *)"Scaning bluetooth device..." );
      #endif
      
      /*搜索蓝牙模块，并进行连接*/
      if( linkHC05() == 0 )
      {
        while( ! IS_HC05_CONNECTED() ); //连接成功，等待HC05模块的INT引脚状态改变
      }
      
    }
    else	//从模式
    {
      HC05_INFO("请搜索连接蓝牙...");
      HC05_Send_CMD("AT+INQ\r\n",1);//模块在查询状态，才能容易被其它设备搜索到

      #ifdef ENABLE_LCD_DISPLAY
      LCD_ClearLine(LCD_LINE_4);
      LCD_DisplayStringLine( LCD_LINE_4, (uint8_t *)"please connect bluetooth..." );
      #endif
    }
  }
  else
  {
    static uint8_t testdata=0;
    HC05_INFO("蓝牙已连接");
    
    //连接后每隔一段时间通过蓝牙模块发送字符串
    sprintf(sendData,"<%s> send data test,testdata=%d\r\n",hc05_name,testdata++);
    HC05_SendString(sendData);	
  }
}


/**
  * @brief  检查蓝牙接收缓冲区、处理接收数据
  * @param  无
  * @retval 无
  */
void CheckRecvBltBuff_Test(void)
{
  char* redata;
	uint16_t len;
  
  if( IS_HC05_CONNECTED() )
  {
    uint16_t linelen;
  
    #ifdef ENABLE_LCD_DISPLAY
    LCD_SetColors(LCD_COLOR_YELLOW, LCD_COLOR_BLACK);
    LCD_ClearLine(LCD_LINE_4);
    LCD_DisplayStringLine( LCD_LINE_4, (uint8_t *)"Bluetooth connected!"  );
    #endif

    /*获取数据*/
    redata = get_rebuff(&len); 
    linelen = get_line(linebuff,redata,len);
  
    /*检查数据是否有更新*/
    if(linelen<200 && linelen != 0)
    {
      if(strcmp(redata,"AT+LED1=ON")==0)
      {
        LED1_ON;						
        HC05_SendString("+LED1:ON\r\nOK\r\n");	
        
        #ifdef ENABLE_LCD_DISPLAY
        LCD_ClearLine(LCD_LINE_5);						
        LCD_DisplayStringLine( LCD_LINE_5, (uint8_t *)"receive cmd: AT+LED1=ON" );
        #endif
      }
      else if(strcmp(redata,"AT+LED1=OFF")==0)
      {
        LED1_OFF;
        HC05_SendString("+LED1:OFF\r\nOK\r\n");
        
        #ifdef ENABLE_LCD_DISPLAY
        LCD_ClearLine(LCD_LINE_5);
        LCD_DisplayStringLine( LCD_LINE_5, (uint8_t *)"receive cmd: AT+LED1=OFF" );
        #endif
      }
      else
      {
        /*这里只演示显示单行的数据，如果想显示完整的数据，可直接使用redata数组*/
        HC05_INFO("receive:\r\n%s",linebuff);
        
        #ifdef ENABLE_LCD_DISPLAY
        LCD_ClearLine(LCD_LINE_6);						
        LCD_SetColors(LCD_COLOR_RED,LCD_COLOR_BLACK);
        LCD_DisplayStringLine( LCD_LINE_6, (uint8_t *)"receive data:" );
        
        LCD_ClearLine(LCD_LINE_7);
        LCD_SetColors(LCD_COLOR_YELLOW,LCD_COLOR_BLACK);
        LCD_DisplayStringLine( LCD_LINE_7,(uint8_t *)linebuff );
        #endif
      }
      
      /*处理数据后，清空接收蓝牙模块数据的缓冲区*/
      clean_rebuff();
      
    }
  }
  //如果蓝牙没有连接
  else
  {
//        Usart_SendString( DEBUG_USARTx, "\r\n蓝牙未连接。接收到蓝牙返回数据：\r\n" );
//        redata = get_rebuff(&len); 
//        Usart_SendString( DEBUG_USARTx, (uint8_t *)redata );
//        Usart_SendString( DEBUG_USARTx, "\r\n\r\n" );
  }
}


void Switch_HC05Mode_Test(void)
{
  if( ! IS_HC05_CONNECTED() )
  {
    hc05_role = !hc05_role;
    if(hc05_role == 0)
    {						
        HC05_Send_CMD("AT+RESET\r\n",1);
        Delay_ms(800);

        if(HC05_Send_CMD("AT+ROLE=0\r\n",1) == 0)	
        {				
          Delay_ms(100);
          
          sprintf(hc05_mode,"SLAVE");
          HC05_INFO("hc05_mode  = %s",hc05_mode);	

          sprintf(hc05_name,"HC05_%s_%d",hc05_mode,(uint8_t)rand());
          sprintf(hc05_nameCMD,"AT+NAME=%s\r\n",hc05_name);
          
          if(HC05_Send_CMD(hc05_nameCMD,1) == 0)
          {
            HC05_INFO("设备名字被更改为：%s",hc05_name);
            
            sprintf(disp_buff,"Device name: %s",hc05_name);
            
            #ifdef ENABLE_LCD_DISPLAY
            LCD_SetColors(LCD_COLOR_RED,LCD_COLOR_BLACK);
            LCD_ClearLine(LCD_LINE_2);											
            LCD_DisplayStringLine( LCD_LINE_2, (uint8_t *)disp_buff );
            #endif
          }
          else
          {							
            HC05_ERROR("更改名字失败");
            
            #ifdef ENABLE_LCD_DISPLAY
            LCD_SetColors(LCD_COLOR_BLUE,LCD_COLOR_BLACK);
            LCD_ClearLine(LCD_LINE_2);											
            LCD_DisplayStringLine( LCD_LINE_2, (uint8_t *)"Rename fail!" );
            #endif
          }
          
          HC05_Send_CMD("AT+INIT\r\n",1);
          HC05_Send_CMD("AT+CLASS=0\r\n",1);
          HC05_Send_CMD("AT+INQM=1,9,48\r\n",1);
          
          //清空蓝牙设备列表
          bltDevList.num = 0;
        }
    }
    else
    {
      HC05_Send_CMD("AT+RESET\r\n",1);
      Delay_ms(800);
      
      if(HC05_Send_CMD("AT+ROLE=1\r\n",1) == 0)	
      {
        Delay_ms(100);
        
        sprintf(hc05_mode,"MASTER");
        HC05_INFO("HC05 mode  = %s",hc05_mode);
          
        sprintf(hc05_name,"HC05_%s_%d",hc05_mode,(uint8_t)rand());
        sprintf(hc05_nameCMD,"AT+NAME=%s\r\n",hc05_name);	
        
        if(HC05_Send_CMD(hc05_nameCMD,1) == 0)
        {
          HC05_INFO("设备名字被更改为：%s",hc05_name);
          sprintf(disp_buff,"Device name: %s",hc05_name);
          
          #ifdef ENABLE_LCD_DISPLAY
          LCD_SetColors(LCD_COLOR_RED,LCD_COLOR_BLACK);
          LCD_ClearLine(LCD_LINE_2);											
          LCD_DisplayStringLine( LCD_LINE_2, (uint8_t *)disp_buff );
          #endif
        }
        else
        {
          HC05_ERROR("更改名字失败");
          
          #ifdef ENABLE_LCD_DISPLAY
          LCD_SetColors(LCD_COLOR_BLUE,LCD_COLOR_BLACK);
          LCD_ClearLine(LCD_LINE_2);											
          LCD_DisplayStringLine( LCD_LINE_2, (uint8_t *)"Rename fail!" );
          #endif
        }
          
        HC05_Send_CMD("AT+INIT\r\n",1);
        HC05_Send_CMD("AT+CLASS=0\r\n",1);
        HC05_Send_CMD("AT+INQM=1,9,48\r\n",1);	
        
        //清空蓝牙设备列表
        bltDevList.num = 0;

      }					

    }
  }
  else
  {
    HC05_INFO("%s", linkhint);
    HC05_SendString(linkhint_en);
  }
}


void Generate_Modify_HC05Name(void)
{
  unsigned long count;
	
  if( ! IS_HC05_CONNECTED() )
  {
    /*生成随机数，给名字随机编号*/
    count = Task_Delay_Group[0];
    srand(count);

    sprintf(hc05_name,"HC05_%s_%d",hc05_mode,(uint8_t)rand());
    sprintf(hc05_nameCMD,"AT+NAME=%s\r\n",hc05_name);

    if(HC05_Send_CMD(hc05_nameCMD,1) == 0)
    {
      HC05_INFO("设备名字被更改为：%s",hc05_name);
      sprintf(disp_buff,"Device name: %s",hc05_name);
      
      #ifdef ENABLE_LCD_DISPLAY
      LCD_SetColors(LCD_COLOR_RED,LCD_COLOR_BLACK);
      LCD_ClearLine(LCD_LINE_2);											
      LCD_DisplayStringLine( LCD_LINE_2, (uint8_t *)disp_buff );
      #endif
    }
    else
    {
      HC05_ERROR("更改名字失败");
      
      #ifdef ENABLE_LCD_DISPLAY
      LCD_SetColors(LCD_COLOR_BLUE,LCD_COLOR_BLACK);
      LCD_ClearLine(LCD_LINE_2);											
      LCD_DisplayStringLine( LCD_LINE_2, (uint8_t *)"Rename fail!" );
      #endif
    }
  }
  else
  {
    HC05_INFO("%s", linkhint); //提示更改失败，需要先断开蓝牙连接才能更改
    HC05_SendString(linkhint_en);
  }
}




/*********************************************END OF FILE**********************/

