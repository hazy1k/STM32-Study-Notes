/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   DS18B20 温湿度传感器实验
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

#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "./delay/core_delay.h"   
#include "./systick/bsp_systick.h"
#include "./ds18b20/bsp_ds18b20.h"

/*******************************************************************
 * Prototypes
 *******************************************************************/
__IO float temperature;
/*******************************************************************
 * Code
 *******************************************************************/
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
		uint8_t uc,DS18B20Id[8];
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
		PRINTF("\r\n*** DS18B20 温度传感器实验***\r\n");
    if(DS18B20_Init()==0)
    {
      PRINTF("DS18B20初始化成功\n");
    }
    else
    {
      PRINTF("DS18B20初始化失败\n");
      PRINTF("请将传感器正确插入到插槽内\n");
      /* 停机 */
      while(1)
      {}			
    }		
    DS18B20_ReadId ( DS18B20Id  );           // 读取 DS18B20 的序列号
    
    PRINTF("\r\nDS18B20的序列号是： 0x");
    
    for ( uc = 0; uc < 8; uc++ )             // 打印 DS18B20 的序列号
      PRINTF ( "%.2x", DS18B20Id[uc]);
    PRINTF("\n");
    
    while(1)
    {
      temperature=DS18B20_Get_Temp();
      PRINTF("DS18B20读取到的温度为：%0.3f\n",temperature);
      Delay_ms(1000);
    } 
   

}
/****************************END OF FILE**********************/
