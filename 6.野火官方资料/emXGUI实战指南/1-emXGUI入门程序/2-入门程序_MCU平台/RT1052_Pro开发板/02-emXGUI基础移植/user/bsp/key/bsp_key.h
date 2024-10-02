#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#include "fsl_common.h"

/******************************************************************
 * �궨��
  ******************************************************************/
//WAUP����
#define CORE_BOARD_WAUP_KEY_GPIO        GPIO5
#define CORE_BOARD_WAUP_KEY_GPIO_PIN    (0U)
#define CORE_BOARD_WAUP_KEY_IOMUXC       IOMUXC_SNVS_WAKEUP_GPIO5_IO00
#define CORE_BOARD_WAUP_KEY_NAME         "CORE_BORE_WAUP_KEY"

//MODE����
#define CORE_BOARD_MODE_KEY_GPIO        GPIO1
#define CORE_BOARD_MODE_KEY_GPIO_PIN    (5U)
#define CORE_BOARD_MODE_KEY_IOMUXC      IOMUXC_GPIO_AD_B0_05_GPIO1_IO05
#define CORE_BOARD_MODE_KEY_NAME        "CORE_BORE_MODE_KEY"

#define KEY_ON      0   //�͵�ƽ��ʾ���°���
#define KEY_OFF     1   

/* ���ݵĺ�ӿ� */
#define KEY1_GPIO_PORT      CORE_BOARD_WAUP_KEY_GPIO
#define KEY1_PIN             CORE_BOARD_WAUP_KEY_GPIO_PIN

#define KEY2_GPIO_PORT      CORE_BOARD_MODE_KEY_GPIO
#define KEY2_PIN             CORE_BOARD_MODE_KEY_GPIO_PIN

/*************״̬�����ʹ�õĶ���****************/
#define KEY_LONG_PRESS_COUNT  3000000 //�������ʱ��
#define KEY_FILTERING_COUNT   500     //����ʱ��


typedef enum {
  KEY_NOT_PRESS,          //�ް��£���ʼ״̬
  KEY_PRESSING,           //���ڱ��̰���
  KEY_LONG_PRESSING,      //���ڱ�������
  KEY_PRESS_RELEASE,      //�̰��²����ͷ�
  KEY_LONG_PRESS_RELEASE, //�����²����ͷ�
}Key_State;

/******************************************************************
 * ����
 ******************************************************************/
void Key_GPIO_Config(void);
Key_State Key_Scan_Machine(GPIO_Type* base, uint32_t pin);
uint8_t Key_Scan(GPIO_Type* base, uint32_t pin);

#endif /* __BSP_KEY_H */
