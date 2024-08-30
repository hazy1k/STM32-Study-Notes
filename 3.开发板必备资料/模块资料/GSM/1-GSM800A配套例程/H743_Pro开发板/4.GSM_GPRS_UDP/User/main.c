/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   GSM模块UDP测试程序
  ******************************************************************
  * @attention
  *
  * 实验平台:野火 STM32H743 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************
  */  
#include "stm32h7xx.h"
#include "main.h"
#include "./led/bsp_led.h"
#include "./usart/bsp_usart.h"
#include "./gsm_gprs/bsp_gsm_usart.h"
#include "./systick/bsp_SysTick.h"
#include "./gsm_gprs/bsp_gsm_gprs.h"
#include "./key/bsp_key.h"
#include <string.h>

#define		LOCALPORT	"2000"

#define		SERVERIP	"163.142.57.125"
//#define		SERVERPORT	"5000"
#define		SERVERPORT	"4000"

const char *TESTBUFF1="\r\n1.野火GSM模块UDP数据上传功能测试";
const char *TESTBUFF2="\r\n2.野火GSM模块UDP数据上传功能测试";


/*
 * 系统软件复位
 */
void Soft_Reset(void)
{
  __set_FAULTMASK(1);   /* 关闭所有中断*/  
  NVIC_SystemReset();   /* 系统复位 */
}


/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{  
  static uint8_t timecount=1,timestop=0;
	uint8_t testCard = 0;
	
	/* 系统时钟初始化成400MHz */
	SystemClock_Config();
 
  SCB_EnableICache();    // 使能指令 Cache
  SCB_EnableDCache();    // 使能数据 Cache
    
	SysTick_Init();
	/* LED 端口初始化 */
	LED_GPIO_Config();
	
  Key_GPIO_Config();
    /* 配置串口1为：115200 8-N-1 */
	UARTx_Config();
  
	 printf("\r\n野火WF-GSM模块UDP收发例程\r\n");
    
    //检测模块响应是否正常
    while(gsm_init()!= GSM_TRUE)
    {
      printf("\r\n模块响应测试不正常！！\r\n");
      printf("\r\n若模块响应测试一直不正常，请检查模块的连接或是否已开启电源开关\r\n");
			GSM_DELAY(1000);          
   
		}
		
		printf("\r\n正在等待GSM模块初始化...\r\n");
		while(IsInsertCard() != GSM_TRUE)
		{
			
			if(++testCard >20)
			{
				printf("\r\n检测不到电话卡，请断电并重新接入电话卡\r\n");
			}
			GSM_DELAY(1000); 		
		}		
	
  GSM_DELAY(100); 
	
	//确认关闭之前的连接		
	gsm_gprs_link_close();	
	
	GSM_DELAY(100);
	//确认关闭之前的场境
	gsm_gprs_shut_close();
	
	printf("\n初始化GPRS工作环境...\n");
  //重新初始化gprs
	if(gsm_gprs_init()!= GSM_TRUE)//GPRS初始化环境
	{
		printf("\r初始化GPRS失败，请重新给模块上电并复位开发板\n");
		while(1);
	}
	
	printf("\n尝试建立UDP链接，请耐心等待...\n");

	if(gsm_gprs_udp_link(LOCALPORT,SERVERIP,SERVERPORT)!=GSM_TRUE)
	{
		printf("\r\nUDP链接失败，请检测正确设置各个模块\r\n");
		GSM_DELAY(1000); 
		printf("\nIP链接断开\n");
		GSM_DELAY(100);
		gsm_gprs_link_close();	
		
		printf("\n关闭场景\n");
		GSM_DELAY(100);
		gsm_gprs_shut_close();
		printf("\r\n5s后自动重启\r\n");
		GSM_DELAY(5000);  
		Soft_Reset();
	}
	
	printf("\r\n连接成功,尝试发送数据...\n");	
	printf("\r\n注意：经测试，使用UDP发送数据时从第二条数据开始才能被接收到\n");	
//	GSM_DELAY(1000);	
	if(gsm_gprs_send(TESTBUFF1)!=GSM_TRUE)
	{
		printf("\r\nUDP发送数据失败，请检测正确设置各个模块\r\n");
		GSM_DELAY(1000); 
		printf("\nIP链接断开\n");
		GSM_DELAY(100);
		gsm_gprs_link_close();	
		
		printf("\n关闭场景\n");
		GSM_DELAY(100);
		gsm_gprs_shut_close();
		while(1);
	}
	
	printf("\n尝试发送第二条数据...\n");	
	if(gsm_gprs_send(TESTBUFF2)!=GSM_TRUE)
	{
		printf("\r\nUDP发送数据失败，请检测正确设置各个模块\r\n");
		GSM_DELAY(1000); 
		printf("\nIP链接断开\n");
		GSM_DELAY(100);
		gsm_gprs_link_close();	
		
		printf("\n关闭场景\n");
		GSM_DELAY(100);
		gsm_gprs_shut_close();
		while(1);
	}
	printf("\n发送第二条数据成功\n");

	printf("\n已准备好接收远端数据，可使用网络调试助手往模块发送数据\n");
	GSM_CLEAN_RX();
	
//	printf("\nIP链接断开\n");
//	GSM_DELAY(100);
//	gsm_gprs_link_close();	
//	
//	printf("\n关闭场景\n");
//	GSM_DELAY(100);
//	gsm_gprs_shut_close();
    while(1)
	{
		if((timecount>=50)&& (timestop!=0xFF))
		{
			
			if(PostGPRS()!=GSM_TRUE)
				timestop++;
			else
				timestop=0;
			timecount=0;
		}
		if(timestop==120)//60s
		{
			printf("\n长时间无通讯，即将将关闭网络\n");

			printf("\nIP链接断开\n");
			GSM_DELAY(100);
			gsm_gprs_link_close();	
			
			printf("\n关闭场景\n");
			GSM_DELAY(100);
			gsm_gprs_shut_close();
			
			GSM_DELAY(1000);
			timestop=0xFF;
		}
		timecount++;
		GSM_DELAY(10);
	}		
	
}



/**
  * @brief  System Clock 配置
  *         system Clock 配置如下: 
	*            System Clock source  = PLL (HSE)
	*            SYSCLK(Hz)           = 400000000 (CPU Clock)
	*            HCLK(Hz)             = 200000000 (AXI and AHBs Clock)
	*            AHB Prescaler        = 2
	*            D1 APB3 Prescaler    = 2 (APB3 Clock  100MHz)
	*            D2 APB1 Prescaler    = 2 (APB1 Clock  100MHz)
	*            D2 APB2 Prescaler    = 2 (APB2 Clock  100MHz)
	*            D3 APB4 Prescaler    = 2 (APB4 Clock  100MHz)
	*            HSE Frequency(Hz)    = 25000000
	*            PLL_M                = 5
	*            PLL_N                = 160
	*            PLL_P                = 2
	*            PLL_Q                = 4
	*            PLL_R                = 2
	*            VDD(V)               = 3.3
	*            Flash Latency(WS)    = 4
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;
  
  /*使能供电配置更新 */
  MODIFY_REG(PWR->CR3, PWR_CR3_SCUEN, 0);

  /* 当器件的时钟频率低于最大系统频率时，电压调节可以优化功耗，
		 关于系统频率的电压调节值的更新可以参考产品数据手册。  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
 
  /* 启用HSE振荡器并使用HSE作为源激活PLL */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
  RCC_OscInitStruct.CSIState = RCC_CSI_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 160;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
 
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
//  if(ret != HAL_OK)
//  {

//    while(1) { ; }
//  }
  
	/* 选择PLL作为系统时钟源并配置总线时钟分频器 */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK  | \
																 RCC_CLOCKTYPE_HCLK    | \
																 RCC_CLOCKTYPE_D1PCLK1 | \
																 RCC_CLOCKTYPE_PCLK1   | \
                                 RCC_CLOCKTYPE_PCLK2   | \
																 RCC_CLOCKTYPE_D3PCLK1);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;  
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2; 
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2; 
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2; 
  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
}
/****************************END OF FILE***************************/
