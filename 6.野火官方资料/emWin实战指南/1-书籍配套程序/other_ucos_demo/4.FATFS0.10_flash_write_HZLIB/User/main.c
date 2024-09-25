#include "stm32f10x.h"
#include "..\User\spi_flash\fatfs_flash_spi.h"
#include "..\User\usart\bsp_usart1.h"
#include "..\User\led\bsp_led.h" 
#include "ff.h"

//ʹ��֮ǰ��
//1.��֤�Ѿ����ֿ��ļ�(songti.xbf, HZLIB.bin, UNIGBK.BIN)�������ⲿFlash�ļ�ϵͳ�У�
//	���û����������(2.STM32-ISO-V3_USB Mass Storage_flash)����
//2.ʹ�øù���д�ֿ��ļ���Ҫһ��ʱ�������ĵȺ򱣴��ֿ�д��ɹ�
//3.ʹ�øù��̿����ڴ��ڵ������ֶ˲鿴�ֿ�д����ȣ�����򿪴��ڵ������ֲ鿴
//4.������������LED�ƻ�ָʾ�ֿ�д�������ÿ�ɹ�д��һ���ֿ��ļ���Ӧһ��LED������

FATFS fs;													/* Work area (file system object) for logical drives */
FRESULT myres;
FIL myfsrc;
UINT mybr;

void fatfs_to_flash(void);

int main(void)
{
	USART1_Config();
	LED_GPIO_Config();
	LED1_OFF;
	LED2_OFF;
	LED3_OFF;
	printf("this is a HZLIB write test demo \r\n");
	fatfs_to_flash();
	while(1);

}

void fatfs_to_flash(void)
{
	uint32_t write_addr=0,j=0;
	uint8_t tempbuf[256]={0};	
	myres=f_mount(&fs,"0:",1);
/*****************************  emWin�����õ��������ֿ� ***************************************/
#if 1
	write_addr=60*4096;	
	
	printf("1.f_mount_xbf res=%d\n",myres);
	myres = f_open(&myfsrc,"0:songti.xbf",FA_OPEN_EXISTING | FA_READ);
	printf("2.f_open_xbf res=%d\n",myres);
	
	for(j=0;j<348;j++)//������������ʼλ��60*4096��1392KB
	{
		SPI_FLASH_SectorErase(write_addr+j*4096);
	}
	j=0;
	while(myres == FR_OK) 
	{
		myres = f_read( &myfsrc, tempbuf, 256, &mybr);//��ȡ����	 
		//printf("%d.f_read_xbf res=%d\n",j,myres);
		if(myres!=FR_OK)break;			 //ִ�д���  
		SPI_FLASH_PageWrite(tempbuf,write_addr,256);  //�������ݵ��ⲿflash��    
		write_addr+=256;				
		j++;
		if(mybr !=256)break;
	} 
	
	f_close(&myfsrc);
	
	SPI_FLASH_BufferRead(tempbuf,60*4096,100);			//��ȡ���ݣ���ӡ��֤
	printf("readbuf_xbf\n");
	for(j=0;j<100;j++)
		printf("%X",tempbuf[j]);
	printf("\n���tempbuf��ȫΪ FF ����ô˵���ֿ⿽���ɹ�������\n");
	if((tempbuf[0]!=0xff)&&(tempbuf[1]!=0xff))LED1_ON;
#endif
/*****************************  ��������õ��������ֿ� ***************************************/
#if 1
	write_addr=4096;	
	
	printf("1.f_mount_bin res=%d\n",myres);
	myres = f_open(&myfsrc,"0:HZLIB.bin",FA_OPEN_EXISTING | FA_READ);

	printf("2.f_open_bin res=%d\n",myres);
	
	for(j=0;j<53;j++)//������������ʼλ��4096��212KB
	{
		SPI_FLASH_SectorErase(write_addr+j*4096);
	}
	j=0;
	while(myres == FR_OK) 
	{
		myres = f_read( &myfsrc, tempbuf, 256, &mybr);//��ȡ����	 
		//printf("%d.f_read_bin res=%d\n",j,myres);
		if(myres!=FR_OK)break;			 //ִ�д���  
		SPI_FLASH_PageWrite(tempbuf,write_addr,256);  //�������ݵ��ⲿflash��    
		write_addr+=256;				
		j++;
		if(mybr !=256)break;
	} 
	
	f_close(&myfsrc);
	
	SPI_FLASH_BufferRead(tempbuf,4096,100);			//��ȡ���ݣ���ӡ��֤
	printf("readbuf_bin\n");
	for(j=0;j<100;j++)
		printf("%X",tempbuf[j]);
	printf("\n���tempbuf��ȫΪ FF ����ô˵���ֿ⿽���ɹ�������\n");
	if((tempbuf[0]!=0xff)&&(tempbuf[1]!=0xff))LED2_ON;
#endif	

/************************* emWin�����й���FatFs�ļ�ϵͳ�õ��������ֿ� ***********************/
#if 1
	write_addr=465*4096;
	printf("1.f_mount_gbk res=%d\n",myres);
	myres = f_open(&myfsrc,"0:UNIGBK.BIN",FA_OPEN_EXISTING | FA_READ);
	printf("2.f_open_gbk res=%d,filesize=%d\n",myres,(int)myfsrc.fsize);
	
	for(j=0;j<43;j++)//������������ʼλ��465*4096��172KB
	{
		SPI_FLASH_SectorErase(write_addr+j*4096);
	}
	j=0;
	while(myres == FR_OK) 
	{
		myres = f_read( &myfsrc, tempbuf, 256, &mybr);//��ȡ����	 
		//printf("%d.f_read_gbk res=%d\n",j,myres);
		if(myres!=FR_OK)break;			 //ִ�д���  
		SPI_FLASH_PageWrite(tempbuf,write_addr,256);  //�������ݵ��ⲿflash��    
		write_addr+=256;				
		j++;
		if(mybr !=256)break;
	} 
	
	f_close(&myfsrc);
	SPI_FLASH_BufferRead(tempbuf,465*4096+174344/2,100);			//��ȡ���ݣ���ӡ��֤
	printf("readbuf_UNIGBK.BIN\n");
	for(j=0;j<100;j++)
		printf("%X",tempbuf[j]);
	printf("\n���tempbuf��ȫΪ FF ����ô˵���ֿ⿽���ɹ�������\n");
	if((tempbuf[0]!=0xff)&&(tempbuf[1]!=0xff))LED3_ON;
#endif		
	myres=f_mount(&fs,"0:",0);
}
/*********************************************END OF FILE**********************/
