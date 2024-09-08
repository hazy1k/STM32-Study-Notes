/**
  ******************************************************************************
  * @file    bsp_dht11.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   ��ʪ�ȴ�����Ӧ�ú����ӿ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  i.MXRT1052������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
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
 * ��
 ******************************************************************************/
/* �������ž�ʹ��ͬ����PAD���� */
#define DHT11_PAD_CONFIG_DATA            (SRE_0_SLOW_SLEW_RATE| \
                                        DSE_6_R0_6| \
                                        SPEED_2_MEDIUM_100MHz| \
                                        ODE_0_OPEN_DRAIN_DISABLED| \
                                        PKE_0_PULL_KEEPER_DISABLED| \
                                        PUE_0_KEEPER_SELECTED| \
                                        PUS_0_100K_OHM_PULL_DOWN| \
                                        HYS_0_HYSTERESIS_DISABLED)   
    /* ����˵�� : */
    /* ת������: ת��������
      ����ǿ��: R0/6 
      �������� : medium(100MHz)
      ��©����: �ر� 
      ��/����������: �ر�
      ��/������ѡ��: �������������ѹرգ�������Ч��
      ����/����ѡ��: 100Kŷķ�����������ѹرգ�������Ч��
      �ͻ�������: �ر� */     

/*******************************************************************************
 * ����
 ******************************************************************************/
static void DHT11_IOMUXC_MUX_Config(void);
static void DHT11_IOMUXC_PAD_Config(void);
static void DHT11_GPIO_Mode_Config(void);

/* ����������ĺ궨���аѺ������ʱ�����滻��SysTick����ʱ���������������Ǹ��ͻ����Ǹ��� */

#define DHT11_DELAY_US(us)  CPU_TS_Tmr_Delay_US(us)
#define DHT11_DELAY_MS(ms)  CPU_TS_Tmr_Delay_MS(ms)

 /**
* @brief  DHT11_Mode_IPU
* @param  ��
* @retval ��
*/
static void DHT11_Mode_IPU(void)
{
	  /* ����gpio��ʼ�����ýṹ�� */
  gpio_pin_config_t dht11_config;      
   /** GPIO���� **/       
  dht11_config.direction = kGPIO_DigitalInput; //����ģʽ	
  dht11_config.outputLogic =  0;                //Ĭ�ϵ͵�ƽ
  dht11_config.interruptMode = kGPIO_NoIntmode; //��ʹ���ж�
  /* ��ʼ�� GPIO  */
  GPIO_PinInit(DHT11_GPIO, DHT11_GPIO_PIN, &dht11_config);
}

/**
* @brief  DHT11_Mode_Out_PP
* @param  ��
* @retval ��
*/
static void DHT11_Mode_Out_PP(void)
{
	  /* ����gpio��ʼ�����ýṹ�� */
  gpio_pin_config_t dht11_config;      
   /** GPIO���� **/       
  dht11_config.direction = kGPIO_DigitalOutput; //���ģʽ	
  dht11_config.outputLogic =  0;                //Ĭ�ϵ͵�ƽ
  dht11_config.interruptMode = kGPIO_NoIntmode; //��ʹ���ж�
  /* ��ʼ�� GPIO */
  GPIO_PinInit(DHT11_GPIO, DHT11_GPIO_PIN, &dht11_config);
		 
}


/**
* @brief  ��ʼ��DHT11���IOMUXC��MUX��������
* @param  ��
* @retval ��
*/
static void DHT11_IOMUXC_MUX_Config(void)
{ 
  /* �������ŵĸ���ģʽΪGPIO����ʹ��SION���� */
  IOMUXC_SetPinMux(DHT11_IOMUXC, 0U);
}

/**
* @brief  ��ʼ��DHT11���IOMUXC��MUX��������
* @param  ��
* @retval ��
*/
static void DHT11_IOMUXC_PAD_Config(void)
{
  /* �������DHT11�ӿ� */    
  IOMUXC_SetPinConfig(DHT11_IOMUXC, DHT11_PAD_CONFIG_DATA);  
}


 /**
  * @brief  ��ʼ��DHT11��ص�GPIOģʽ
  * @param  ��
  * @retval ��
  */
static void DHT11_GPIO_Mode_Config(void)
{     
  /* ����gpio��ʼ�����ýṹ�� */
  gpio_pin_config_t dht11_config;      
    
   /** �������DHT11�ӿڣ�GPIO���� **/       
  dht11_config.direction = kGPIO_DigitalOutput; //���ģʽ
  dht11_config.outputLogic =  1;                //Ĭ�ϸߵ�ƽ
  dht11_config.interruptMode = kGPIO_NoIntmode; //��ʹ���ж�
  
  /* ��ʼ�� DHT11 GPIO. */
  GPIO_PinInit(DHT11_GPIO, DHT11_GPIO_PIN, &dht11_config);
}

/**
  * @brief  ��ʼ������DHT11��IO
  * @param  ��
  * @retval ��
  */
void DHT11_GPIO_Config(void)
{
  /* ��ʼ��GPIO���á����ԡ�ģʽ */
  DHT11_IOMUXC_MUX_Config();
  DHT11_IOMUXC_PAD_Config();
  DHT11_GPIO_Mode_Config();
}




/* 
 * ��DHT11��ȡһ���ֽڣ�MSB����
 */
static uint8_t Read_Byte(void)
{
	uint8_t i, temp=0;

	for(i=0;i<8;i++)    
	{	 
		/*ÿbit��50us�͵�ƽ���ÿ�ʼ����ѯֱ���ӻ����� ��50us �͵�ƽ ����*/  
		while(DHT11_DATA_IN()==Bit_RESET);

		/*DHT11 ��26~28us�ĸߵ�ƽ��ʾ��0������70us�ߵ�ƽ��ʾ��1����
		 *ͨ����� x us��ĵ�ƽ��������������״ ��x ���������ʱ 
		 */
		DHT11_DELAY_US(40); //��ʱx us �����ʱ��Ҫ��������0������ʱ�伴��	   	  

		if(DHT11_DATA_IN()==Bit_SET)/* x us����Ϊ�ߵ�ƽ��ʾ���ݡ�1�� */
		{
			/* �ȴ�����1�ĸߵ�ƽ���� */
			while(DHT11_DATA_IN()==Bit_SET);

			temp|=(uint8_t)(0x01<<(7-i));  //�ѵ�7-iλ��1��MSB���� 
		}
		else	 // x us��Ϊ�͵�ƽ��ʾ���ݡ�0��
		{			   
			temp&=(uint8_t)~(0x01<<(7-i)); //�ѵ�7-iλ��0��MSB����
		}
	}
	return temp;
}
/*
 * һ�����������ݴ���Ϊ40bit����λ�ȳ�
 * 8bit ʪ������ + 8bit ʪ��С�� + 8bit �¶����� + 8bit �¶�С�� + 8bit У��� 
 */
uint8_t Read_DHT11(DHT11_Data_TypeDef *DHT11_Data)
{  
  uint16_t count;
	/*���ģʽ*/
	DHT11_Mode_Out_PP();
	/*��������*/
	DHT11_DATA_OUT(DHT11_LOW);
	/*��ʱ18ms*/
	DHT11_DELAY_US(20000);

	/*�������� ������ʱ30us*/
	DHT11_DATA_OUT(DHT11_HIGH); 

	DHT11_DELAY_US(30);   //��ʱ30us

	/*������Ϊ���� �жϴӻ���Ӧ�ź�*/ 
	DHT11_Mode_IPU();

	/*�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź� �粻��Ӧ����������Ӧ����������*/   
	if(DHT11_DATA_IN()==Bit_RESET)     
	{
    count=0;
		/*��ѯֱ���ӻ����� ��80us �͵�ƽ ��Ӧ�źŽ���*/  
		while(DHT11_DATA_IN()==Bit_RESET)
    {
      count++;
      if(count>1000)  return 0;
      DHT11_DELAY_US(10); 
    }    
    
    count=0;
		/*��ѯֱ���ӻ������� 80us �ߵ�ƽ �����źŽ���*/
		while(DHT11_DATA_IN()==Bit_SET)
    {
      count++;
      if(count>1000)  return 0;
      DHT11_DELAY_US(10); 
    }  
		/*��ʼ��������*/   
		DHT11_Data->humi_int= Read_Byte();

		DHT11_Data->humi_deci= Read_Byte();

		DHT11_Data->temp_int= Read_Byte();

		DHT11_Data->temp_deci= Read_Byte();

		DHT11_Data->check_sum= Read_Byte();

		/*��ȡ���������Ÿ�Ϊ���ģʽ*/
		DHT11_Mode_Out_PP();
		/*��������*/
		DHT11_DATA_OUT(DHT11_HIGH);

		/*����ȡ�������Ƿ���ȷ*/
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

