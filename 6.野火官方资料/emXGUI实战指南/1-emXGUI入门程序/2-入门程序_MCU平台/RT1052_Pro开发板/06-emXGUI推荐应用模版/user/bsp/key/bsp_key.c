/**
  ******************************************************************
  * @file    bsp_key.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   ����Ӧ�ú����ӿ�
  ******************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  i.MXRT1052������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************
  */
#include "fsl_iomuxc.h"
#include "fsl_gpio.h" 

#include "pad_config.h" 
#include "./key/bsp_key.h"   

/*******************************************************************
 * ��������
 *******************************************************************/
/* GPIO �������ַ���� */
static GPIO_Type *const s_gpioBases[] = GPIO_BASE_PTRS;



/******************************************************************
 * ��
  ******************************************************************/
/* �������ž�ʹ��ͬ����PAD���� */
#define KEY_PAD_CONFIG_DATA            (SRE_0_SLOW_SLEW_RATE| \
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

/******************************************************************
 * ����
  ******************************************************************/
static void Key_IOMUXC_MUX_Config(void);
static void Key_IOMUXC_PAD_Config(void);
static void Key_GPIO_Mode_Config(void);

/******************************************************************
 * ��������
  ******************************************************************/
/**
* @brief  ��ʼ���������IOMUXC��MUX��������
* @param  ��
* @retval ��
*/
static void Key_IOMUXC_MUX_Config(void)
{
  /* ���ð������ŵĸ���ģʽΪGPIO����ʹ��SION���� */
  IOMUXC_SetPinMux(CORE_BOARD_WAUP_KEY_IOMUXC, 0U);
  IOMUXC_SetPinMux(CORE_BOARD_MODE_KEY_IOMUXC, 0U); 
}

/**
* @brief  ��ʼ���������IOMUXC��MUX��������
* @param  ��
* @retval ��
*/
static void Key_IOMUXC_PAD_Config(void)
{
  /* ���ð����������Թ��� */    
  IOMUXC_SetPinConfig(CORE_BOARD_WAUP_KEY_IOMUXC, KEY_PAD_CONFIG_DATA); 
  IOMUXC_SetPinConfig(CORE_BOARD_MODE_KEY_IOMUXC, KEY_PAD_CONFIG_DATA); 
}

 /**
  * @brief  ��ʼ��������ص�GPIOģʽ
  * @param  ��
  * @retval ��
  */
static void Key_GPIO_Mode_Config(void)
{     
  /* ����gpio��ʼ�����ýṹ�� */
  gpio_pin_config_t key_config; 
    
  /** ���İ�İ�����GPIO���� **/       
  key_config.direction = kGPIO_DigitalInput; //����ģʽ
  key_config.outputLogic =  1;                //Ĭ�ϸߵ�ƽ������ģʽʱ��Ч��
  key_config.interruptMode = kGPIO_NoIntmode; //��ʹ���ж�
  
  /* ��ʼ������ GPIO. */
  GPIO_PinInit(CORE_BOARD_WAUP_KEY_GPIO, CORE_BOARD_WAUP_KEY_GPIO_PIN, &key_config);
  GPIO_PinInit(CORE_BOARD_MODE_KEY_GPIO, CORE_BOARD_MODE_KEY_GPIO_PIN, &key_config);
}


 /**
  * @brief  ��ʼ������KEY��IO
  * @param  ��
  * @retval ��
  */
void Key_GPIO_Config(void)
{
  /* ��ʼ��GPIO���á����ԡ�ģʽ */
  Key_IOMUXC_MUX_Config();
  Key_IOMUXC_PAD_Config();
  Key_GPIO_Mode_Config();
}


/**
  * @brief   ����Ƿ��а������� 
  * @note   �������ڰ�������ʱ��������ֱ���ͷ�
  * @param  base:����Ķ˿�
  * @param  pin:��������ź�
  * @retval  ������״̬
  *   @arg KEY_ON:��������
  *   @arg KEY_OFF:����û����
  */
uint8_t Key_Scan(GPIO_Type* base, uint32_t pin)
{     
  /*����Ƿ��а������� */
  if(KEY_ON == GPIO_PinRead(base, pin))
  {  
    /*�ȴ������ͷ� */
    while(KEY_ON == GPIO_PinRead(base, pin));   
    return  KEY_ON;  
  }
  else
    return KEY_OFF;
}

/******** ʹ��״̬���ļ�ⷽ��,��Ҫ������*************/

/**
* @brief GPIO base ��ȡGPIO�˿ڱ��
*        ��������fsl_gpio.c�ļ����ƹ�����
*        ��ú�����static���ͣ��޷����ⲿ�ļ����ã�
*        ��Ϊ�˱��ֿ��ļ���ԭ���ԣ����޸Ŀ��ļ���ֱ�Ӹ�������
*
* @param base GPIO�������ַָ��
* @retval GPIO�˿ڱ��
*/
static uint32_t GPIO_GetInstance(GPIO_Type *base)
{
    uint32_t instance;
  
    /* ����baseָ����ҳ�GPIO�˿ڱ�� */
    for (instance = 0; instance < ARRAY_SIZE(s_gpioBases); instance++)
    {
        if (s_gpioBases[instance] == base)
        {
            break;
        }
    }
    assert(instance < ARRAY_SIZE(s_gpioBases));
    return instance;
}

 /**
  * @brief  �������״̬��
  * @note   ��������Ҫ��ѭ����ʱ�����ã�ͨ������ֵ���Ի񰴼�״̬
  * @param  base:����Ķ˿�
  * @param  pin:��������ź�
  * @retval Key_State
  */
Key_State Key_Scan_Machine(GPIO_Type* base, uint32_t pin)
{
  /* Ϊ֧�ֶ��������⣬ÿ�������ж����ľ�̬����  */
  static Key_State state[5][16] ={ KEY_NOT_PRESS };
  static uint32_t press_count[5][16] = { 0 };
  static uint32_t filtering_count[5][16] = { 0 };

  switch(state[GPIO_GetInstance(base)-1][pin])
  {
    case KEY_PRESS_RELEASE:
    case KEY_LONG_PRESS_RELEASE:
      //�����ͷ�״̬����Ϊ�ް���
      state[GPIO_GetInstance(base)-1][pin] = KEY_NOT_PRESS;
      break;
    
    case KEY_NOT_PRESS:
        //��ⰴ��
        if(KEY_ON == GPIO_PinRead(base, pin))
        {
          __asm("NOP"); /* delay */

          //����
          if(filtering_count[GPIO_GetInstance(base)-1][pin]++ > KEY_FILTERING_COUNT)
          {
            state[GPIO_GetInstance(base)-1][pin] = KEY_PRESSING; //���COUNT�κ�ת��״̬
            filtering_count[GPIO_GetInstance(base)-1][pin] = 0;
          }
        }   
      break;
        
    case KEY_PRESSING:
        //��ⰴ���ͷ�
        if(KEY_OFF == GPIO_PinRead(base, pin))
        {
          __asm("NOP"); /* delay */

          if(filtering_count[GPIO_GetInstance(base)-1][pin]++ > KEY_FILTERING_COUNT)
          {
            state[GPIO_GetInstance(base)-1][pin] = KEY_PRESS_RELEASE; //���COUNT�κ�ת��״̬
            filtering_count[GPIO_GetInstance(base)-1][pin] = 0;
          }       
        }
        //�������
        else if(KEY_ON == GPIO_PinRead(base, pin))
        {
          __asm("NOP"); /* delay */

          if(press_count[GPIO_GetInstance(base)-1][pin]++ > KEY_LONG_PRESS_COUNT)
          {
            state[GPIO_GetInstance(base)-1][pin] = KEY_LONG_PRESSING;//���COUNT�κ�ת��״̬
            press_count[GPIO_GetInstance(base)-1][pin] = 0;
          }
        }   
      break;
        
    case KEY_LONG_PRESSING:
        //��ⰴ���ͷ�
        if(KEY_OFF == GPIO_PinRead(base, pin))
        {
          __asm("NOP"); /* delay */

          if(filtering_count[GPIO_GetInstance(base)-1][pin]++ > KEY_FILTERING_COUNT)
          {
            state[GPIO_GetInstance(base)-1][pin] = KEY_LONG_PRESS_RELEASE; //���COUNT�κ�ת��״̬
            filtering_count[GPIO_GetInstance(base)-1][pin] = 0;
          }       
        }
      break;
  }
  
  return state[GPIO_GetInstance(base)-1][pin];
}

/*********************************************END OF FILE**********************/
