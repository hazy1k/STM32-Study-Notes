#ifndef __BSP_MPU_H__
#define __BSP_MPU_H__

#include "stm32h7xx_hal.h"
//����MPU->RASR�Ĵ���AP[26:24]λ������ֵ

#define	MPU_1KB		MPU_REGION_SIZE_1KB
#define	MPU_2KB		MPU_REGION_SIZE_2KB
#define	MPU_4KB		MPU_REGION_SIZE_4KB
#define	MPU_8KB		MPU_REGION_SIZE_8KB
#define	MPU_16KB	MPU_REGION_SIZE_16KB
#define	MPU_32KB	MPU_REGION_SIZE_32KB
#define	MPU_64KB	MPU_REGION_SIZE_64KB
#define	MPU_128KB	MPU_REGION_SIZE_128KB
#define	MPU_256KB	MPU_REGION_SIZE_256KB
#define	MPU_512KB	MPU_REGION_SIZE_512KB

#define	MPU_1MB		MPU_REGION_SIZE_1MB
#define	MPU_2MB		MPU_REGION_SIZE_2MB
#define	MPU_4MB		MPU_REGION_SIZE_4MB
#define	MPU_8MB		MPU_REGION_SIZE_8MB
#define	MPU_16MB	MPU_REGION_SIZE_16MB
#define	MPU_32MB	MPU_REGION_SIZE_32MB
#define	MPU_64MB	MPU_REGION_SIZE_64MB
#define	MPU_128MB	MPU_REGION_SIZE_128MB
#define	MPU_256MB	MPU_REGION_SIZE_256MB
#define	MPU_512MB	MPU_REGION_SIZE_512MB


#define  MPU_NO_ACCESS      ((uint8_t)0x00U)
#define  MPU_PRIV_RW        ((uint8_t)0x01U)
#define  MPU_PRIV_RW_URO    ((uint8_t)0x02U)
#define  MPU_FULL_ACCESS    ((uint8_t)0x03U)
#define  MPU_PRIV_RO        ((uint8_t)0x05U)
#define  MPU_PRIV_RO_URO    ((uint8_t)0x06U)

/*����ΪNormal��cache������Ϊ write-back����������cache�� 
 *�ں��ʵ�ʱ��(��cache���Ծ����������ǿ�Ƹ���)�����ݸ��µ���Ӧ��SRAM�ռ�
 *�ر�ע�⣺���Ҫ�����������£�д֮��ҪSCB_CleanDCache��������ʱҪSCB_InvalidateDCache
 */
#define	MPU_Normal_WB									0x00  


/*����ΪNormal��cache������Ϊ write-back����������cache�� 
 *�ں��ʵ�ʱ��(��cache���Ծ����������ǿ�Ƹ���)�����ݸ��µ���Ӧ��SRAM�ռ�
 *�ر�ע�⣺���Ҫ�����������£�д֮��ҪSCB_CleanDCache��������ʱҪSCB_InvalidateDCache
 */
#define	MPU_Normal_WBWARA							0x01  //�ⲿ���ڲ�д����д�����


/*����Ϊ normal��cache������Ϊ Write-through��������cache��ͬʱ��
 *������ͬʱд����Ӧ�������ַ�ռ�
 *�ر�ע�⣺���Ҫ�����������£�����ֱ�����ڴ�д���ݣ���������ʱҪSCB_InvalidateDCache
 */
#define	MPU_Normal_WT									0x02


/*����Ϊ normal�����ù���,���û���
 */
#define	MPU_Normal_NonCache						0x03


/*����Ϊ Device������������Ч�����ù���,���û���
 */
#define	MPU_Device_NonCache						0x04

void Board_MPU_Config(uint8_t	Region ,uint8_t Mode,uint32_t Address,uint32_t Size);
#endif

















