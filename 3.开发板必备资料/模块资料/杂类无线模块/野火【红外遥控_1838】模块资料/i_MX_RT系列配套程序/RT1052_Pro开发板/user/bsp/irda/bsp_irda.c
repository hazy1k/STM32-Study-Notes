/**
  ******************************************************************************
  * @file    bsp_irda.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   ����ң�����ӿ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  i.MXRT1052������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include "./irda/bsp_irda.h"
#include "./systick/bsp_SysTick.h"
#include "./bsp/nvic/bsp_nvic.h"
#include "./delay/core_delay.h"  
#include "./led/bsp_led.h"
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"  
#include "pad_config.h"  

uint32_t frame_data=0;    /* һ֡���ݻ��� */
uint8_t  frame_cnt=0;
uint8_t  frame_flag=0;    /* һ֡���ݽ�����ɱ�־ */

/*******************************************************************************
 * ��
 ******************************************************************************/
/* �������ž�ʹ��ͬ����PAD���� */
#define IRDA_PAD_CONFIG_DATA            (SRE_0_SLOW_SLEW_RATE| \
                                        DSE_0_OUTPUT_DRIVER_DISABLED| \
                                        SPEED_2_MEDIUM_100MHz| \
                                        ODE_0_OPEN_DRAIN_DISABLED| \
                                        PKE_1_PULL_KEEPER_ENABLED| \
                                        PUE_1_PULL_SELECTED| \
                                        PUS_3_22K_OHM_PULL_UP| \
                                        HYS_1_HYSTERESIS_ENABLED)   

    /* ����˵�� : */
    /* ת������: ת��������
      ����ǿ��: �ر�
      �ٶ����� : medium(100MHz)
      ��©����: �ر� 
      ��/����������: ʹ��
      ��/������ѡ��: ������
      ����/����ѡ��: 22Kŷķ����
      �ͻ�������: ���� ��������ʱ��Ч��ʩ���ش�������ʹ�ܺ���Թ�������������*/
/*******************************************************************************
 * ����
 ******************************************************************************/
static void  Irda_IOMUXC_MUX_Config(void);
static void  Irda_IOMUXC_PAD_Config(void);
static void  Irda_GPIO_Mode_Config(void);

/* ����������ĺ궨���аѺ������ʱ�����滻��SysTick����ʱ���������������Ǹ��ͻ����Ǹ��� */

#define  Delay_us(us)  CPU_TS_Tmr_Delay_US(us)
#define  Delay_ms(ms)  CPU_TS_Tmr_Delay_MS(ms)

/**
* @brief  ��ʼ��IRDA���IOMUXC��MUX��������
* @param  ��
* @retval ��
*/
static void Irda_IOMUXC_MUX_Config(void)
{ 
  /* �������ŵĸ���ģʽΪGPIO����ʹ��SION���� */
  IOMUXC_SetPinMux(IRDA_IOMUXC, 0U);
}

/**
* @brief  ��ʼ��IRDA���IOMUXC��MUX��������
* @param  ��
* @retval ��
*/
static void Irda_IOMUXC_PAD_Config(void)
{
  /* �������IRDA�ӿ� */    
  IOMUXC_SetPinConfig(IRDA_IOMUXC, IRDA_PAD_CONFIG_DATA);  
}

 /**
  * @brief  ��ʼ��IRDA��ص�GPIOģʽ
  * @param  ��
  * @retval ��
  */
static void Irda_GPIO_Mode_Config(void)
{     
  /* ����gpio��ʼ�����ýṹ�� */
  gpio_pin_config_t irda_config;      
   
  irda_config.direction = kGPIO_DigitalInput; //����ģʽ
  irda_config.outputLogic =  1;                //Ĭ�ϸߵ�ƽ
	irda_config.interruptMode = kGPIO_IntFallingEdge; //�½����ж�
	
  /* ��ʼ�� IRDA GPIO. */
  GPIO_PinInit(IRDA_GPIO, IRDA_GPIO_PIN, &irda_config);
}

/**
 * @brief  ��ʼ���ж���ص�����
 * @param  ��
 * @retval ��
 */
static void Irda_Interrupt_Config(void)   
{
  /* ����GPIO���ŵ��ж� */
  GPIO_PortEnableInterrupts(IRDA_GPIO, 1U << IRDA_GPIO_PIN);                             
  /*�����ж����ȼ�,*/
	set_IRQn_Priority(IRDA_IRQ,Group4_PreemptPriority_6, Group4_SubPriority_1);  
  /* ����GPIO�˿��ж� */
  EnableIRQ(IRDA_IRQ);
}


/**
  * @brief  ��ʼ������Irda��IO
  * @param  ��
  * @retval ��
  */
void Irda_GPIO_Config(void)
{
  /* ��ʼ��GPIO���á����ԡ�ģʽ �Լ��ж�*/
  Irda_IOMUXC_MUX_Config();
  Irda_IOMUXC_PAD_Config();
  Irda_GPIO_Mode_Config();
	Irda_Interrupt_Config();
}
 
/* ��ʼ���������ͷ1838�õ���IO */
void IrDa_Init(void)
{
	Irda_GPIO_Config();
}

/* ��ȡ�ߵ�ƽ��ʱ�� */
uint8_t Get_Pulse_Time(void)
{
  uint8_t time = 0;
  while( IrDa_DATA_IN() )
  {
    time ++;
    Delay_us(20);     // ��ʱ 20us
    if(time == 250)
      return time;   // ��ʱ���   
  }
  return time;
}

/*
 * ֡������4���ֽڣ���һ���ֽ���ң�ص�ID���ڶ����ֽ��ǵ�һ���ֽڵķ���
 * ������������ң�ص������ļ�ֵ�����ĸ��ֽ��ǵ������ֽڵķ���
 */
uint8_t IrDa_Process(void)
{
  uint8_t first_byte, sec_byte, tir_byte, fou_byte;  
  
  first_byte = frame_data >> 24;
  sec_byte = (frame_data>>16) & 0xff;
  tir_byte = frame_data >> 8;
  fou_byte = frame_data;
  
  /* �ǵ����־λ */
  frame_flag = 0;
  
  if( (first_byte==(uint8_t)~sec_byte) && (first_byte==IRDA_ID) )
  {
    if( tir_byte == (uint8_t)~fou_byte )
      return tir_byte;
  }
  
  return 0;   /* ���󷵻� */
}
/********************�жϷ�����**************************/
/**
 * @brief  GPIO �����жϷ�����
 *         IRDA_IRQHandlerֻ��һ���꣬
 *         �жϷ��������ǹ̶��ģ������������ļ����ҵ���
 * @param  �жϷ������������������
 * @note   �жϺ���һ��ֻʹ�ñ�־λ����ָʾ����ɺ󾡿��˳���
 *         �����������ʱ�����������жϷ�������
 * @retval �жϷ����������з���ֵ
 */
uint8_t isr_cnt;  /* ���ڼ�����˶��ٴ��ж� */  
void IRDA_IRQHandler(void)
{ 
		uint8_t pulse_time = 0;
		uint8_t leader_code_flag = 0; /* �������־λ�������������ʱ����ʾһ֡���ݿ�ʼ */
		uint8_t irda_data = 0;        /* �����ݴ�λ */
    /* ����жϱ�־λ */
    RGB_RED_LED_TOGGLE
		while(1)
		{
			if( IrDa_DATA_IN()== SET )        /* ֻ�����ߵ�ƽ��ʱ�� */
			{       
				pulse_time = Get_Pulse_Time();
				
				/* >=5ms ���������ź� �����ָ��Ż���������ʱ��Ҳ��break����while(1)ѭ�� */
				if( pulse_time >= 250 )                
				{
					break; /* ����while(1)ѭ�� */
				}
				
				if(pulse_time>=200 && pulse_time<250)         /* ���ǰ��λ 4ms~4.5ms */
				{
					leader_code_flag = 1;
				}
				else if(pulse_time>=10 && pulse_time<50)      /* 0.56ms: 0.2ms~1ms */
				{
					irda_data = 0;
				}
				else if(pulse_time>=50 && pulse_time<100)     /* 1.68ms��1ms~2ms */
				{
					irda_data =1 ; 
				}        
				else if( pulse_time>=100 && pulse_time<=200 ) /* 2.1ms��2ms~4ms */
				{/* �����룬�ڵڶ����жϳ��� */
					frame_flag = 1;               /* һ֡���ݽ������ */
					frame_cnt++;                  /* ����������1 */
					isr_cnt ++;                   /* ���ж�һ�μ�1 */
					break;                        /* ����while(1)ѭ�� */
				}
				
				if( leader_code_flag == 1 )
				{/* �ڵ�һ���ж������ */
					frame_data <<= 1;
					frame_data += irda_data;
					frame_cnt = 0;
					isr_cnt = 1;
				}
			}      
			GPIO_PortClearInterruptFlags(IRDA_GPIO,1U << IRDA_GPIO_PIN);  
	}  
	
	    /* ���²�����Ϊ ARM �Ŀ���838869��ӵ�, 
       �ô���Ӱ�� Cortex-M4, Cortex-M4F�ںˣ�       
       �����洢�����ص��쳣�����·��ز������ܻ�ָ�������ж�
        CM7����Ӱ�죬�˴������ô���
    */
	 /* ԭע�ͣ�Add for ARM errata 838869, affects Cortex-M4,
       Cortex-M4F Store immediate overlapping
       exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
/*********************************************END OF FILE**********************/
