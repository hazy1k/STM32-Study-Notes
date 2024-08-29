/**
  ******************************************************************
  * @file    bsp_mpu.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   ����MPU�������Ժʹ�С
  ******************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32H743������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************
  */ 
#include "./mpu/bsp_mpu.h" 

/**
  * @brief  ����MPU�������Ժʹ�С�Ĵ���ֵ
	* @param  Region  					MPU��������ȡֵ��Χ��0��7��
	* @param  DisableExec  			ָ����ʽ���λ��0=����ָ����ʣ�1=��ָֹ����ʡ�
	* @param  AccessPermission	���ݷ���Ȩ�ޣ�ȡֵ��Χ��MPU_NO_ACCESS��MPU_PRIV_RO_URO��
	* @param  TypeExtField 			������չ�ֶΣ������������ڴ�������ͣ�����ǿ�������衣
	*														����ȡֵ��0��1��2��
  * @param  Address 					MPU�����������ַ���ر�ע�����õ�Address��Ҫ��Size����
  * @param  Size 							MPU���������С,����ȡֵ��MPU_1KB��MPU_4KB ...MPU_512MB��
	* @param  IsShareable 			�����Ĵ洢�ռ��Ƿ���Թ���1=������0=��ֹ����
  * @param  IsCacheable 			�����Ĵ洢�ռ��Ƿ���Ի��棬1=�����棬0=��ֹ���档
  * @param  IsBufferable 			ʹ��Cache֮�󣬲�����write-through����write-back(bufferable)
	*												    1=�����壬����д��write-back����0=��ֹ���壬��ֱд��write-through����
  * @retval None
  */
void BSP_MPU_ConfigRegion(	uint8_t		Region,
														uint8_t  	DisableExec,
														uint8_t 	AccessPermission,
														uint8_t 	TypeExtField, 
														uint32_t 	Address, 
														uint32_t 	Size,
														uint8_t 	IsShareable, 
														uint8_t  	IsCacheable, 
														uint8_t  	IsBufferable	)
{
  MPU_Region_InitTypeDef MPU_InitStruct;
  
  /* ����MPU */
  HAL_MPU_Disable();

  /* ����MPU����*/
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;											
  MPU_InitStruct.BaseAddress = Address;									//�������ַ��
  MPU_InitStruct.Size = Size;														//Ҫ���õ�����������С��
  MPU_InitStruct.AccessPermission = AccessPermission;		//���ݷ���Ȩ�������������û�����Ȩģʽ�Ķ�/д����Ȩ�ޡ�
  MPU_InitStruct.IsBufferable = IsBufferable;						//�����ǿɻ���ģ���ʹ�û�д���档 �ɻ��浫���ɻ��������ʹ��ֱд���ԡ�
  MPU_InitStruct.IsCacheable = IsCacheable;							//�����Ƿ�ɻ���ģ�����ֵ�Ƿ���Ա����ڻ����С�
  MPU_InitStruct.IsShareable = IsShareable;							//�����Ƿ�����ڶ������������֮�乲��
  MPU_InitStruct.Number = Region;												//���򱣻���
  MPU_InitStruct.TypeExtField = TypeExtField;						//������չ�ֶΣ������������ڴ�������͡�
  MPU_InitStruct.SubRegionDisable = 0x00;								//����������ֶΡ�
  MPU_InitStruct.DisableExec = DisableExec;							//ָ����ʽ���λ��

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* ����MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

void Board_MPU_Config(uint8_t	Region ,uint8_t Mode,uint32_t Address,uint32_t Size)
{
	switch(Mode)
	{
		case MPU_Normal_WB:
				 /* �����ڴ�ΪNormal����,���ù���, ��дģʽ����д���ȡ����*/
				 BSP_MPU_ConfigRegion(Region,0,MPU_FULL_ACCESS,0,Address,Size,0,1,1);	
				 break;

		case MPU_Normal_WBWARA:
				 /* �����ڴ�ΪNormal����,���ù���, ��дģʽ��д���ȡ����*/
				 BSP_MPU_ConfigRegion(Region,0,MPU_FULL_ACCESS,1,Address,Size,0,1,1);	
				 break;
		
		case MPU_Normal_WT:
				 /* �����ڴ�ΪNormal����,���ù���, ֱдģʽ*/
				 BSP_MPU_ConfigRegion(Region,0,MPU_FULL_ACCESS,0,Address,Size,0,1,0);	
				 break;		
		
		case MPU_Normal_NonCache:
				 /* �����ڴ�ΪNormal����,���ù������û���ģʽ*/
				 BSP_MPU_ConfigRegion(Region,0,MPU_FULL_ACCESS,1,Address,Size,0,0,0);	
				 break;
		
		case MPU_Device_NonCache:
				 /*�����ڴ�ΪDevice����,����������Ч����ֹ�������û���ģʽ*/
				 BSP_MPU_ConfigRegion(Region,0,MPU_FULL_ACCESS,2,Address,Size,0,0,0);	
				 break;
	}
	
}
