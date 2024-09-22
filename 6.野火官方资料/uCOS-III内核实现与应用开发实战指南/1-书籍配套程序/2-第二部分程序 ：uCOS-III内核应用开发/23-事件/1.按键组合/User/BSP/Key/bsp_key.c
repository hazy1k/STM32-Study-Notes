#include "bsp_key.h" 
#include <includes.h>   //ʹ��uC/OS�ں˻����������ͷ�ļ�



static void Key1_Config ( void );
static void Key2_Config ( void );
//static void Key_Delay   ( volatile uint32_t ulCount );



void Key_Initial ( void )
{
	Key1_Config ();
	Key2_Config ();
}


static void Key1_Config ( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	

	RCC_APB2PeriphClockCmd ( macKEY1_GPIO_CLK, ENABLE );
	
	GPIO_InitStructure.GPIO_Pin = macKEY1_GPIO_PIN; 

	GPIO_InitStructure.GPIO_Mode = macKEY1_GPIO_Mode; 
	
	GPIO_Init ( macKEY1_GPIO_PORT, & GPIO_InitStructure );
}


static void Key2_Config ( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	

	RCC_APB2PeriphClockCmd ( macKEY2_GPIO_CLK, ENABLE );
	
	GPIO_InitStructure.GPIO_Pin = macKEY2_GPIO_PIN; 

	GPIO_InitStructure.GPIO_Mode = macKEY2_GPIO_Mode; 
	
	GPIO_Init ( macKEY2_GPIO_PORT, & GPIO_InitStructure );
}


//static void Key_Delay ( volatile uint32_t ulCount )
//{
//	for(; ulCount != 0; ulCount--);
//} 


/**
  * @brief  ��ⰴ���Ƿ񱻵���
  * @param  GPIOx ���������ڵĶ˿�
  * @param  GPIO_Pin ���������ڵĹܽ�
  * @param  ucPushState ������������ʱ�ĵ�ƽ����
  *   �ò���Ϊ����ֵ֮һ��
  *     @arg 1 :�ߵ�ƽ
  *     @arg 0 :�͵�ƽ
  * @param  pKeyPress �����ڴ�Ű����Ƿ񱻰��¹������Ǿ�̬����	
  * @retval �����Ƿ񱻵���
	*   �÷���ֵΪ����ֵ֮һ��
  *     @arg 1 :����������
  *     @arg 0 :����δ������
  */
uint8_t Key_Scan ( GPIO_TypeDef * GPIOx, uint16_t GPIO_Pin, uint8_t ucPushState, uint8_t * pKeyPress )
{			
	uint8_t ucKeyState, ucRet = 0;
	
	OS_ERR      err;
	
	ucKeyState = GPIO_ReadInputDataBit ( GPIOx, GPIO_Pin );
	
	if ( ucKeyState == ucPushState )
	{
//		Key_Delay( 10000 );	         //�ĳ�uC/OS����ʱ����
		OSTimeDlyHMSM ( 0, 0, 0, 20, OS_OPT_TIME_DLY, & err );
		
		ucKeyState = GPIO_ReadInputDataBit ( GPIOx, GPIO_Pin );
		
		if ( ucKeyState == ucPushState )
			* pKeyPress = 1;
	}

	if ( ( ucKeyState != ucPushState ) && ( * pKeyPress == 1 ) )
	{
		ucRet = 1;
		* pKeyPress = 0;
	}
	
	return ucRet;
	
}


/**
  * @brief  ��ⰴ����״̬
  * @param  GPIOx ���������ڵĶ˿�
  * @param  GPIO_Pin ���������ڵĹܽ�
  * @param  ucPushState ������������ʱ�ĵ�ƽ����
  *   �ò���Ϊ����ֵ֮һ��
  *     @arg 1 :�ߵ�ƽ
  *     @arg 0 :�͵�ƽ
  * @retval �����ĵ�ǰ״̬
	*   �÷���ֵΪ����ֵ֮һ��
  *     @arg 1 :����������
  *     @arg 0 :�������ͷ�
  */
uint8_t Key_ReadStatus ( GPIO_TypeDef * GPIOx, uint16_t GPIO_Pin, uint8_t ucPushState )
{			
	uint8_t ucKeyState;
	
	
	ucKeyState = GPIO_ReadInputDataBit ( GPIOx, GPIO_Pin );
	
	if ( ucKeyState == ucPushState )
		return 1;
	else 
		return 0;

}


/*********************************************END OF FILE**********************/
