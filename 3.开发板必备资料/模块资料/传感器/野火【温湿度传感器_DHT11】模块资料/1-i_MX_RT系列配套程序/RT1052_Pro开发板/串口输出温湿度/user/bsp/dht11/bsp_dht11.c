/**
  ******************************************************************************
  * @file    bsp_dht11.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   温湿度传感器应用函数接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  i.MXRT1052开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
#include "./dht11/bsp_dht11.h"
#include "./delay/core_delay.h"  
#include "./systick/bsp_SysTick.h"
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"  
#include "pad_config.h"  
/*******************************************************************************
 * 宏
 ******************************************************************************/
/* 所有引脚均使用同样的PAD配置 */
#define DHT11_PAD_CONFIG_DATA            (SRE_0_SLOW_SLEW_RATE| \
                                        DSE_6_R0_6| \
                                        SPEED_2_MEDIUM_100MHz| \
                                        ODE_0_OPEN_DRAIN_DISABLED| \
                                        PKE_0_PULL_KEEPER_DISABLED| \
                                        PUE_0_KEEPER_SELECTED| \
                                        PUS_0_100K_OHM_PULL_DOWN| \
                                        HYS_0_HYSTERESIS_DISABLED)   
    /* 配置说明 : */
    /* 转换速率: 转换速率慢
      驱动强度: R0/6 
      带宽配置 : medium(100MHz)
      开漏配置: 关闭 
      拉/保持器配置: 关闭
      拉/保持器选择: 保持器（上面已关闭，配置无效）
      上拉/下拉选择: 100K欧姆下拉（上面已关闭，配置无效）
      滞回器配置: 关闭 */     

/*******************************************************************************
 * 声明
 ******************************************************************************/
static void DHT11_IOMUXC_MUX_Config(void);
static void DHT11_IOMUXC_PAD_Config(void);
static void DHT11_GPIO_Mode_Config(void);

/* 可以在下面的宏定义中把后面的延时函数替换换SysTick的延时函数，就是想用那个就换成那个的 */

#define DHT11_DELAY_US(us)  CPU_TS_Tmr_Delay_US(us)
#define DHT11_DELAY_MS(ms)  CPU_TS_Tmr_Delay_MS(ms)

 /**
* @brief  DHT11_Mode_IPU
* @param  无
* @retval 无
*/
static void DHT11_Mode_IPU(void)
{
	  /* 定义gpio初始化配置结构体 */
  gpio_pin_config_t dht11_config;      
   /** GPIO配置 **/       
  dht11_config.direction = kGPIO_DigitalInput; //输入模式	
  dht11_config.outputLogic =  0;                //默认低电平
  dht11_config.interruptMode = kGPIO_NoIntmode; //不使用中断
  /* 初始化 GPIO  */
  GPIO_PinInit(DHT11_GPIO, DHT11_GPIO_PIN, &dht11_config);
}

/**
* @brief  DHT11_Mode_Out_PP
* @param  无
* @retval 无
*/
static void DHT11_Mode_Out_PP(void)
{
	  /* 定义gpio初始化配置结构体 */
  gpio_pin_config_t dht11_config;      
   /** GPIO配置 **/       
  dht11_config.direction = kGPIO_DigitalOutput; //输出模式	
  dht11_config.outputLogic =  0;                //默认低电平
  dht11_config.interruptMode = kGPIO_NoIntmode; //不使用中断
  /* 初始化 GPIO */
  GPIO_PinInit(DHT11_GPIO, DHT11_GPIO_PIN, &dht11_config);
		 
}


/**
* @brief  初始化DHT11相关IOMUXC的MUX复用配置
* @param  无
* @retval 无
*/
static void DHT11_IOMUXC_MUX_Config(void)
{ 
  /* 设置引脚的复用模式为GPIO，不使用SION功能 */
  IOMUXC_SetPinMux(DHT11_IOMUXC, 0U);
}

/**
* @brief  初始化DHT11相关IOMUXC的MUX复用配置
* @param  无
* @retval 无
*/
static void DHT11_IOMUXC_PAD_Config(void)
{
  /* 开发板的DHT11接口 */    
  IOMUXC_SetPinConfig(DHT11_IOMUXC, DHT11_PAD_CONFIG_DATA);  
}


 /**
  * @brief  初始化DHT11相关的GPIO模式
  * @param  无
  * @retval 无
  */
static void DHT11_GPIO_Mode_Config(void)
{     
  /* 定义gpio初始化配置结构体 */
  gpio_pin_config_t dht11_config;      
    
   /** 开发板的DHT11接口，GPIO配置 **/       
  dht11_config.direction = kGPIO_DigitalOutput; //输出模式
  dht11_config.outputLogic =  1;                //默认高电平
  dht11_config.interruptMode = kGPIO_NoIntmode; //不使用中断
  
  /* 初始化 DHT11 GPIO. */
  GPIO_PinInit(DHT11_GPIO, DHT11_GPIO_PIN, &dht11_config);
}

/**
  * @brief  初始化控制DHT11的IO
  * @param  无
  * @retval 无
  */
void DHT11_GPIO_Config(void)
{
  /* 初始化GPIO复用、属性、模式 */
  DHT11_IOMUXC_MUX_Config();
  DHT11_IOMUXC_PAD_Config();
  DHT11_GPIO_Mode_Config();
}




/* 
 * 从DHT11读取一个字节，MSB先行
 */
static uint8_t Read_Byte(void)
{
	uint8_t i, temp=0;

	for(i=0;i<8;i++)    
	{	 
		/*每bit以50us低电平标置开始，轮询直到从机发出 的50us 低电平 结束*/  
		while(DHT11_DATA_IN()==Bit_RESET);

		/*DHT11 以26~28us的高电平表示“0”，以70us高电平表示“1”，
		 *通过检测 x us后的电平即可区别这两个状 ，x 即下面的延时 
		 */
		DHT11_DELAY_US(40); //延时x us 这个延时需要大于数据0持续的时间即可	   	  

		if(DHT11_DATA_IN()==Bit_SET)/* x us后仍为高电平表示数据“1” */
		{
			/* 等待数据1的高电平结束 */
			while(DHT11_DATA_IN()==Bit_SET);

			temp|=(uint8_t)(0x01<<(7-i));  //把第7-i位置1，MSB先行 
		}
		else	 // x us后为低电平表示数据“0”
		{			   
			temp&=(uint8_t)~(0x01<<(7-i)); //把第7-i位置0，MSB先行
		}
	}
	return temp;
}
/*
 * 一次完整的数据传输为40bit，高位先出
 * 8bit 湿度整数 + 8bit 湿度小数 + 8bit 温度整数 + 8bit 温度小数 + 8bit 校验和 
 */
uint8_t Read_DHT11(DHT11_Data_TypeDef *DHT11_Data)
{  
  uint16_t count;
	/*输出模式*/
	DHT11_Mode_Out_PP();
	/*主机拉低*/
	DHT11_DATA_OUT(DHT11_LOW);
	/*延时18ms*/
	DHT11_DELAY_US(20000);

	/*总线拉高 主机延时30us*/
	DHT11_DATA_OUT(DHT11_HIGH); 

	DHT11_DELAY_US(30);   //延时30us

	/*主机设为输入 判断从机响应信号*/ 
	DHT11_Mode_IPU();

	/*判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行*/   
	if(DHT11_DATA_IN()==Bit_RESET)     
	{
    count=0;
		/*轮询直到从机发出 的80us 低电平 响应信号结束*/  
		while(DHT11_DATA_IN()==Bit_RESET)
    {
      count++;
      if(count>1000)  return 0;
      DHT11_DELAY_US(10); 
    }    
    
    count=0;
		/*轮询直到从机发出的 80us 高电平 标置信号结束*/
		while(DHT11_DATA_IN()==Bit_SET)
    {
      count++;
      if(count>1000)  return 0;
      DHT11_DELAY_US(10); 
    }  
		/*开始接收数据*/   
		DHT11_Data->humi_int= Read_Byte();

		DHT11_Data->humi_deci= Read_Byte();

		DHT11_Data->temp_int= Read_Byte();

		DHT11_Data->temp_deci= Read_Byte();

		DHT11_Data->check_sum= Read_Byte();

		/*读取结束，引脚改为输出模式*/
		DHT11_Mode_Out_PP();
		/*主机拉高*/
		DHT11_DATA_OUT(DHT11_HIGH);

		/*检查读取的数据是否正确*/
		if(DHT11_Data->check_sum == DHT11_Data->humi_int + DHT11_Data->humi_deci + DHT11_Data->temp_int+ DHT11_Data->temp_deci)
			return 1;
		else 
			return 0;
	}
	else
	{		
		return 0;
	}   
}

