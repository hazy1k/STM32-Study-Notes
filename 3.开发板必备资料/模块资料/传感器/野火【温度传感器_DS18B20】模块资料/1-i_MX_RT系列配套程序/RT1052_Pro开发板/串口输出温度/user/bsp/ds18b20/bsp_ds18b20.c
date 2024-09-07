/**
  ******************************************************************************
  * @file    bsp_ds18b20.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   DS18B20�¶ȴ�����Ӧ�ú����ӿ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  i.MXRT1052������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
#include "./ds18B20/bsp_ds18b20.h"
#include "./systick/bsp_SysTick.h"
#include "./delay/core_delay.h"
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"  
#include "pad_config.h"  
/*******************************************************************************
 * ��
 ******************************************************************************/
/* �������ž�ʹ��ͬ����PAD���� */
#define DS18B20_PAD_CONFIG_DATA            (SRE_0_SLOW_SLEW_RATE| \
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
static void  DS18B20_IOMUXC_MUX_Config(void);
static void  DS18B20_IOMUXC_PAD_Config(void);
static void  DS18B20_GPIO_Mode_Config(void);

/* ����������ĺ궨���аѺ������ʱ�����滻��SysTick����ʱ���������������Ǹ��ͻ����Ǹ��� */

#define  DS18B20_DELAY_US(us)  CPU_TS_Tmr_Delay_US(us)
#define  DS18B20_DELAY_MS(ms)  CPU_TS_Tmr_Delay_MS(ms)
 /**
* @brief  DS18B20_Mode_IPU
* @param  ��
* @retval ��
*/
static void DS18B20_Mode_IPU(void)
{
	  /* ����gpio��ʼ�����ýṹ�� */
  gpio_pin_config_t ds18b20_config;      
   /** GPIO���� **/       
  ds18b20_config.direction = kGPIO_DigitalInput; //����ģʽ	
  ds18b20_config.outputLogic =  1;                //Ĭ�ϵ͵�ƽ
  ds18b20_config.interruptMode = kGPIO_NoIntmode; //��ʹ���ж�
  /* ��ʼ�� GPIO  */
  GPIO_PinInit(DS18B20_GPIO, DS18B20_GPIO_PIN, &ds18b20_config);
}

/**
* @brief  DS18B20_Mode_Out_PP
* @param  ��
* @retval ��
*/
static void  DS18B20_Mode_Out_PP(void)
{
	  /* ����gpio��ʼ�����ýṹ�� */
  gpio_pin_config_t ds18b20_config;      
   /** GPIO���� **/       
  ds18b20_config.direction = kGPIO_DigitalOutput; //���ģʽ	
  ds18b20_config.outputLogic =  1;                //Ĭ�ϵ͵�ƽ
  ds18b20_config.interruptMode = kGPIO_NoIntmode; //��ʹ���ж�
  /* ��ʼ�� GPIO */
  GPIO_PinInit(DS18B20_GPIO, DS18B20_GPIO_PIN, &ds18b20_config);
		 
}


/**
* @brief  ��ʼ��DS18B20���IOMUXC��MUX��������
* @param  ��
* @retval ��
*/
static void DS18B20_IOMUXC_MUX_Config(void)
{ 
  /* �������ŵĸ���ģʽΪGPIO����ʹ��SION���� */
  IOMUXC_SetPinMux(DS18B20_IOMUXC, 0U);
}

/**
* @brief  ��ʼ��DS18B20���IOMUXC��MUX��������
* @param  ��
* @retval ��
*/
static void DS18B20_IOMUXC_PAD_Config(void)
{
  /* �������DS18B20�ӿ� */    
  IOMUXC_SetPinConfig(DS18B20_IOMUXC, DS18B20_PAD_CONFIG_DATA);  
}


 /**
  * @brief  ��ʼ��DS18B20��ص�GPIOģʽ
  * @param  ��
  * @retval ��
  */
static void DS18B20_GPIO_Mode_Config(void)
{     
  /* ����gpio��ʼ�����ýṹ�� */
  gpio_pin_config_t ds18b20_config;      
    
   /** �������DS18B20�ӿڣ�GPIO���� **/       
  ds18b20_config.direction = kGPIO_DigitalOutput; //���ģʽ
  ds18b20_config.outputLogic =  1;                //Ĭ�ϸߵ�ƽ
  ds18b20_config.interruptMode = kGPIO_NoIntmode; //��ʹ���ж�
  
  /* ��ʼ�� DS18B20 GPIO. */
  GPIO_PinInit(DS18B20_GPIO, DS18B20_GPIO_PIN, &ds18b20_config);
}



/**
  * @brief  ��ʼ������DS18B20��IO
  * @param  ��
  * @retval ��
  */
void DS18B20_GPIO_Config(void)
{
  /* ��ʼ��GPIO���á����ԡ�ģʽ */
  DS18B20_IOMUXC_MUX_Config();
  DS18B20_IOMUXC_PAD_Config();
  DS18B20_GPIO_Mode_Config();
}
/*
 *�������ӻ����͸�λ����
 */
static void DS18B20_Rst(void)
{
	/* ��������Ϊ������� */
	DS18B20_Mode_Out_PP();
	
	DS18B20_DATA_OUT(DS18B20_LOW);
	
	/* �������ٲ���480us�ĵ͵�ƽ��λ�ź� */
	DS18B20_DELAY_US(750);

	/* �����ڲ�����λ�źź��轫�������� */
	DS18B20_DATA_OUT(DS18B20_HIGH);
	
	/*�ӻ����յ������ĸ�λ�źź󣬻���15~60us���������һ����������*/
	DS18B20_DELAY_US(15);
}

/**
  * @brief  ���ӻ����������صĴ�������
  * @param  dat Ҫд����ֽ�
  * @retval 0 �ɹ�
	* @retval 1 ʧ��
  */
static uint8_t DS18B20_Presence(void)
{
	uint8_t pulse_time = 0;
	
	/* ��������Ϊ�������� */
	DS18B20_Mode_IPU();
	
	/* �ȴ���������ĵ�������������Ϊһ��60~240us�ĵ͵�ƽ�ź� 
	 * �����������û����������ʱ�����ӻ����յ������ĸ�λ�źź󣬻���15~60us���������һ����������
	 */
	while( DS18B20_DATA_IN() && pulse_time<100 )
	{
		pulse_time++;
		DS18B20_DELAY_US(1);
	}	
	/* ����100us�󣬴������嶼��û�е���*/
	if( pulse_time >=100 )
		return 1;
	else
		pulse_time = 0;
	
	/* �������嵽�����Ҵ��ڵ�ʱ�䲻�ܳ���240us */
	while( !DS18B20_DATA_IN() && pulse_time<240 )
	{
		pulse_time++;
		DS18B20_DELAY_US(1);
	}	
	if( pulse_time >=240 )
		return 1;
	else
		return 0;
}

/**
  * @brief  DS18B20��һ��λ
  * @retval ��
  */
static uint8_t DS18B20_Read_Bit(void)
{
	uint8_t dat;
	
	/* ��0�Ͷ�1��ʱ������Ҫ����60us */	
	DS18B20_Mode_Out_PP();
	/* ��ʱ�����ʼ���������������� >1us <15us �ĵ͵�ƽ�ź� */
	DS18B20_DATA_OUT(DS18B20_LOW);
	DS18B20_DELAY_US(10);
	DS18B20_DATA_OUT(DS18B20_HIGH);
	/* ���ó����룬�ͷ����ߣ����ⲿ�������轫�������� */
	DS18B20_Mode_IPU();
	DS18B20_DELAY_US(12);
	
	if( DS18B20_DATA_IN() == SET )
		dat = 1;
	else
		dat = 0;
	
	/* �����ʱ������ο�ʱ��ͼ */
	DS18B20_DELAY_US(50);//45
	
	return dat;
}

/**
  * @brief  DS18B20���ֽ�
  * @retval ��
  */
uint8_t DS18B20_Read_Byte(void)
{
	uint8_t i, j, dat = 0;	
	
	for(i=0; i<8; i++) 
	{
		j = DS18B20_Read_Bit();		
		dat = (dat) | (j<<i);
	}
	
	return dat;
}

/**
  * @brief  DS18B20д�ֽ�
  * @param  dat Ҫд����ֽ�
  * @retval ��
  */
void DS18B20_Write_Byte(uint8_t dat)
{
	uint8_t i, testb;
	/*����Ϊ���ģʽ*/
	DS18B20_Mode_Out_PP();
	
	for( i=0; i<8; i++ )
	{
		testb = dat&0x01;
		dat = dat>>1;		
		/* д0��д1��ʱ������Ҫ����60us */
		if (testb)
		{			
			DS18B20_DATA_OUT(DS18B20_LOW);
			/* 1us < �����ʱ < 15us */
			DS18B20_DELAY_US(8);
			
			DS18B20_DATA_OUT(DS18B20_HIGH);
			DS18B20_DELAY_US(58);
		}		
		else
		{			
			DS18B20_DATA_OUT(DS18B20_LOW);
			/* 60us < Tx 0 < 120us */
			DS18B20_DELAY_US(70);
			
			DS18B20_DATA_OUT(DS18B20_HIGH);			
			/* 1us < Trec(�ָ�ʱ��) < �����*/
			DS18B20_DELAY_US(2);
		}
	}
}
 /**
  * @brief  ��ʼ��DS18B20
  * @param  ��
  * @retval ��
  */
uint8_t DS18B20_Init(void)
{
	DS18B20_GPIO_Config();	
	DS18B20_Rst();
	
	return DS18B20_Presence();
}
/*
 * �洢���¶���16 λ�Ĵ�������չ�Ķ����Ʋ�����ʽ
 * ��������12λ�ֱ���ʱ������5������λ��7������λ��4��С��λ
 *
 *         |---------����----------|-----С�� �ֱ��� 1/(2^4)=0.0625----|
 * ���ֽ�  | 2^3 | 2^2 | 2^1 | 2^0 | 2^(-1) | 2^(-2) | 2^(-3) | 2^(-4) |
 *
 *
 *         |-----����λ��0->��  1->��-------|-----------����-----------|
 * ���ֽ�  |  s  |  s  |  s  |  s  |    s   |   2^6  |   2^5  |   2^4  |
 *
 * 
 * �¶� = ����λ + ���� + С��*0.0625
 */
float DS18B20_Get_Temp(void)
{
	uint8_t tpmsb, tplsb;
	short s_tem;
	float f_tem;
	
	DS18B20_Rst();	   
	DS18B20_Presence();	 
	DS18B20_Write_Byte(0XCC);				/* ���� ROM */
	DS18B20_Write_Byte(0X44);				/* ��ʼת�� */
	
	DS18B20_Rst();
	DS18B20_Presence();
	DS18B20_Write_Byte(0XCC);				/* ���� ROM */
	DS18B20_Write_Byte(0XBE);				/* ���¶�ֵ */
	
	tplsb = DS18B20_Read_Byte();		 
	tpmsb = DS18B20_Read_Byte(); 
	
	s_tem = tpmsb<<8;
	s_tem = s_tem | tplsb;
	
	if( s_tem < 0 )		/* ���¶� */
		f_tem = (~s_tem+1) * 0.0625;	
	else
		f_tem = s_tem * 0.0625;
	
	return f_tem; 	
}

 /**
  * @brief  ��ƥ�� ROM ����»�ȡ DS18B20 �¶�ֵ 
  * @param  ds18b20_id�����ڴ�� DS18B20 ���кŵ�������׵�ַ
  * @retval ��
  */
void DS18B20_ReadId ( uint8_t * ds18b20_id )
{
	uint8_t uc;
	
	
	DS18B20_Write_Byte(0x33);       //��ȡ���к�
	
	for ( uc = 0; uc < 8; uc ++ )
	  ds18b20_id [ uc ] = DS18B20_Read_Byte();
	
}
