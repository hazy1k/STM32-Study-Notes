/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   dht11 温湿度传感器实验
  ******************************************************************
  * @attention
  *
  * 实验平台:野火  i.MXRT1052开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************
  */
#include "fsl_debug_console.h"
#include "fsl_elcdif.h"
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "./delay/core_delay.h"   
#include "./systick/bsp_systick.h"
#include "./ds18b20/bsp_ds18b20.h"
#include "./lcd/bsp_lcd.h" 
#include "stdio.h" 
#include "string.h"
/*******************************************************************
 * Prototypes
 *******************************************************************/
/* 显示缓冲区 */
uint8_t dis_buf[1024];
__IO float temperature;
/*******************************************************************
 * Code
 *******************************************************************/

char buffer[30];
void Float_TO_Char(float slope,char*buffer,int n)  //浮点型数，存储的字符数组，字符数组的长度
{
    int temp,i,j;
    if(slope>=0)//判断是否大于0
        //buffer[0] = '+';
				buffer[0] = ' ';
    else
    {
        buffer[0] = '-';
        slope = -slope;
    }
    temp = (int)slope;//取整数部分
    for(i=0;temp!=0;i++)//计算整数部分的位数
        temp /=10;
    temp =(int)slope;
    for(j=i;j>0;j--)//将整数部分转换成字符串型
    {
        buffer[j] = temp%10+'0';
        temp /=10;
    }
    buffer[i+1] = '.';
    slope -=(int)slope;
    for(i=i+2;i<n-1;i++)//将小数部分转换成字符串型
    {
        slope*=10;
        buffer[i]=(int)slope+'0';
        slope-=(int)slope;
    }
    buffer[n-1] = '\0';
}

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
		uint8_t uc,DS18B20Id[8];
		uint8_t DS18B20Id_str[20];
    /* 初始化内存保护单元 */
    BOARD_ConfigMPU();
    /* 初始化开发板引脚 */
    BOARD_InitPins();
    /* 初始化开发板时钟 */
    BOARD_BootClockRUN();
    /* 初始化调试串口 */
    BOARD_InitDebugConsole();
	  /* 初始化精确延时*/
		CPU_TS_TmrInit();
		/* 初始化滴答定时器 */
		SysTick_Init();
    /* 打印系统时钟 */
    PRINTF("\r\n");
    PRINTF("*****欢迎使用 野火i.MX RT1052 开发板*****\r\n");
    PRINTF("CPU:             %d Hz\r\n", CLOCK_GetFreq(kCLOCK_CpuClk));
    PRINTF("AHB:             %d Hz\r\n", CLOCK_GetFreq(kCLOCK_AhbClk));
    PRINTF("SEMC:            %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SemcClk));
    PRINTF("SYSPLL:          %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllClk));
    PRINTF("SYSPLLPFD0:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd0Clk));
    PRINTF("SYSPLLPFD1:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd1Clk));
    PRINTF("SYSPLLPFD2:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd2Clk));
    PRINTF("SYSPLLPFD3:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd3Clk));  
		/* 初始化LCD */
    LCD_Init(LCD_INTERRUPT_ENABLE);
		LCD_DisplayStringLine(LINE(1),(uint8_t* )"DS18B20 temperature detect demo");
		
		PRINTF("\r\n*** DS18B20 温度传感器实验***\r\n");
		if(DS18B20_Init()==0)
		{
				PRINTF("DS18B20初始化成功\n");
		}
		else
		{    
				PRINTF("DS18B20初始化失败\n");
				PRINTF("请将传感器正确插入到插槽内\n");

				LCD_SetTextColor(CL_RED);
				LCD_DisplayStringLine(LINE(2),(uint8_t* )"DS18B20 initialization failed!");
				LCD_DisplayStringLine(LINE(3),(uint8_t* )"Please check the connection!");
				/* 停机 */
				while(1)
				{}			
		}		
    
		DS18B20_ReadId ( DS18B20Id  );           // 读取 DS18B20 的序列号	
    
		for ( uc = 0; uc < 8; uc++ )             // 打印 DS18B20 的序列号
		{    
				sprintf((char *)&DS18B20Id_str[2*uc], "%.2x",DS18B20Id[uc]);  
				
				if(uc == 7)
					DS18B20Id_str[17] = '\0';        
		}
		PRINTF("\r\nDS18B20的序列号是： 0x%s\r\n",DS18B20Id_str);
		sprintf((char*)dis_buf,"DS18B20 serial num:0x%s",DS18B20Id_str);  
		LCD_DisplayStringLine(LINE(4),dis_buf);
		memset(dis_buf,0,1024);
		while(1)
		{
			temperature=DS18B20_Get_Temp();	
			PRINTF("DS18B20读取到的温度为：%0.3f\n",temperature);
#if defined(__CC_ARM)
			sprintf((char*)dis_buf,"Temperature:   %0.3f   degree Celsius",temperature);
#elif defined(__ICCARM__)
			/*IAR中的sprintf格式化输出浮点型*/
			Float_TO_Char(temperature,(char*)buffer,8);
			strcat((char*)dis_buf,"Temperature:   ");
			strcat((char*)dis_buf,(char*)buffer);
			strcat((char*)dis_buf,"   degree Celsius");
#endif
			PRINTF("str：%s\n",dis_buf);
			LCD_DisplayStringLine(LINE(5),dis_buf);
			memset(dis_buf,0,1024);
			Delay_ms(1000);
		}  

}
/****************************END OF FILE**********************/





