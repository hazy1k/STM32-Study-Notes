/**
  ******************************************************************************
  * @file    aux_data.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ���ڴ�SD����¼FLASH��������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 ָ���� ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */  


#include "aux_data.h"
#include "ff.h"
#include "./flash/bsp_spi_flash.h"
#include "./led/bsp_led.h" 


/***************************************************************************************************************/



//SD��Դ����·������
char src_dir[255]="0:/srcdata";

//Ҫ��������FLASH·��
char dst_dir[255]= FLASH_ROOT;

char flash_scan_dir[255]= FLASH_ROOT;
char sd_scan_dir[255]= SD_ROOT;


//Ҫд������ݵ���Ϣ
Aux_Data_Typedef  burn_data[] =  
{
  [AUX_XBF_XINSONGTI] =
  {
    .filename           =   "0:/srcdata/songti.xbf",
    .description        =   "XBF�ֿ��ļ���emWinʹ��,������.xbf��",
    .start_addr         =   60*4096 ,
    .length             =   317*4096, 
    .burn_option      =  UPDATE,
  },
  
  [AUX_HZLIB] =
  {
    .filename           =   "0:/srcdata/HZLIB.bin",
    .description        =   "��������ֿ⣨�ɰ棬Ϊ���ݶ�������",
    .start_addr         =   1*4096 ,
    .length             =   53*4096, 
    .burn_option      =  UPDATE,
  },
	
	 [AUX_GB2312] = 
  {
    .filename           =   "0:/srcdata/GB2312_H1616.FON",
    .description        =   "GB2312�ֿ⣨�޸�HZLIB��֧�ֱ������⣩",
    .start_addr         =   387*4096 ,
    .length             =   64*4096, 
    .burn_option      =  UPDATE,
  },
  
  [AUX_UNIGBK] =
  {
    .filename           =   "0:/srcdata/UNIGBK.BIN",
    .description        =   "�ļ�ϵͳ����֧���ֿ�(emWinʹ��,UNIGBK.BIN)",
    .start_addr         =   465*4096 ,
    .length             =   43*4096,
    .burn_option      =  UPDATE,      
  },      
  
  [AUX_FILE_SYSTEM] =
  {
    .filename           =   "",
    .description        =   "FATFS�ļ�ϵͳ",
    .start_addr         =   512*4096 ,
    .length             =   1536*4096, 
    .burn_option      =  DO_NOT_UPDATE,
  }

};



static int copy_dir(char *src_path,char *dst_path);
static int copy_file(char *src_path,char *dst_path);
static FRESULT scan_files (char* path) ; 
static FRESULT copy_all (char* src_path,char* dst_path); 


/*flash��sd�����ļ�ϵͳ���*/
FATFS flash_fs;
FATFS sd_fs;													/* Work area (file system object) for logical drives */



/*sdʹ�õ��ļ�ϵͳ����*/
static FIL fnew;													/* file objects */
 
/**
  * @brief  ��FLASHд���ļ�(д�����ļ�ϵͳ����)
  * @param  dat��Ҫд����ļ����ݵ���Ϣ
  * @param  file_num��Ҫд����ļ�����
  * @retval ��������FR_OK
  */
FRESULT burn_file_sd2flash(Aux_Data_Typedef *dat,uint8_t file_num) 
{
    uint8_t i;
  
    FRESULT result; 
    UINT  bw;            					    /* File R/W count */


    uint32_t write_addr=0,j=0;
    uint8_t tempbuf[256],flash_buf[256];
    
//    result = f_mount(&sd_fs,SD_ROOT,1);
//    
//    //����ļ�ϵͳ����ʧ�ܾ��˳�
//    if(result != FR_OK)
//    {
//      BURN_ERROR("f_mount ERROR!");
//      LED_RED;
//      return result;
//    }
    
    for(i=0;i<file_num;i++)
    {
       if (dat[i].burn_option == DO_NOT_UPDATE)
          continue;
       
       BURN_INFO("-------------------------------------"); 
       BURN_INFO("׼����¼���ݣ�%s",dat[i].description);
       LED_BLUE;
       
       result = f_open(&fnew,dat[i].filename,FA_OPEN_EXISTING | FA_READ);
        if(result != FR_OK)
        {
            BURN_ERROR("���ļ�ʧ�ܣ�");
            LED_RED;
            return result;
        }
        
      BURN_INFO("���ڲ���Ҫʹ�õ�FLASH�ռ�...");
  
      write_addr = dat[i].start_addr;
        
      for(j=0;j < dat[i].length/4096 ;j++)//������������ʼλ��710*4096��2116KB
      {
        SPI_FLASH_SectorErase(write_addr+j*4096);
      }
      
      BURN_INFO("������FLASHд������...");
      
      write_addr = dat[i].start_addr;
      while(result == FR_OK) 
      {
        result = f_read( &fnew, tempbuf, 256, &bw);//��ȡ����	 
        if(result!=FR_OK)			 //ִ�д���
        {
          BURN_ERROR("��ȡ�ļ�ʧ�ܣ�");
          LED_RED;
          return result;
        }      
        SPI_FLASH_PageWrite(tempbuf,write_addr,256);  //�������ݵ��ⲿflash��    
        write_addr+=256;				
        if(bw !=256)break;
      }

        BURN_INFO("����д�����,��ʼУ������...");

        //У������
      write_addr = dat[i].start_addr;
     
      f_lseek(&fnew,0);
      
      while(result == FR_OK) 
      {
        result = f_read( &fnew, tempbuf, 256, &bw);//��ȡ����	 
        if(result!=FR_OK)			 //ִ�д���
        {
          BURN_ERROR("��ȡ�ļ�ʧ�ܣ�");
          LED_RED;
          return result;
        }      
        SPI_FLASH_BufferRead(flash_buf,write_addr,bw);  //��FLASH�ж�ȡ����
        write_addr+=bw;		
        
        for(j=0;j<bw;j++)
        {
          if(tempbuf[i] != flash_buf[i])
          {
            BURN_ERROR("����У��ʧ�ܣ�");
            LED_RED;
            return FR_INT_ERR;
          }
         }  
     
        if(bw !=256)break;//�����ļ�β
      }
      

      BURN_INFO("����У��ɹ���");
      BURN_INFO("�ļ���%s д�����",dat[i].filename);
      BURN_INFO("-------------------------------------");
      LED_BLUE;

      f_close(&fnew); 
    }
    
    
    BURN_INFO("************************************");
    BURN_INFO("�����ļ�������¼�ɹ��������ļ�ϵͳ���֣�");
    return FR_OK;


}



/**
  * @brief  �����ļ�(��Ŀ¼�����ļ��У���ݹ鸴��)
  * @param  src_path:Ҫ���Ƶ��ļ�(���ļ���)
  * @param  dst_path:Ҫ���Ƶ��ĸ��ļ���
  * @retval result:�ļ�ϵͳ�ķ���ֵ
  */
FRESULT copy_file_sd2flash(char *src_path,char *dst_path) 
{
  FRESULT result; 

  BURN_INFO("-------------------------------------"); 
  //�����ļ���flash���ļ�ϵͳ����    
  BURN_INFO("���������ļ���FLASH���ļ�ϵͳ����...");


  //����flash
  result = f_mount(&flash_fs,FLASH_ROOT,1);
  
  BURN_INFO("���ڸ�ʽ��FLASH..."); 

  //��ʽ��FLASH
  result = f_mkfs(FLASH_ROOT,0,0);							

  //���¹���flash
  result = f_mount(NULL,FLASH_ROOT,1);
  result = f_mount(&flash_fs,FLASH_ROOT,1);
  
  //����ļ�ϵͳ����ʧ�ܾ��˳�
  if(result != FR_OK)
  {
    BURN_ERROR("FLASH�ļ�ϵͳ����ʧ�ܣ��븴λ���ԣ�");
    LED_RED;
    return result;
  } 
  
  BURN_INFO("*****************************************"); 
  BURN_INFO("\r\n ����Ҫ���Ƶ�SD���ļ��� \r\n"); 
  scan_files(sd_scan_dir);
  
  BURN_INFO("*****************************************"); 
  
  BURN_INFO("\r\n ��ʼ���ƣ� \r\n"); 
  result = copy_all(src_dir,dst_dir);   
  if(result != FR_OK)
  {
    BURN_ERROR("�����ļ���FLASHʧ�ܣ������룺%d",result);
    LED_RED;
    return result;
  }   

  BURN_INFO("*****************************************"); 
  BURN_INFO("\r\n ���ƺ��FLASH�ļ�(�ļ�ϵͳ����)�� \r\n"); 
  scan_files(flash_scan_dir);

  BURN_INFO("*****************************************"); 
  BURN_INFO("�����ļ����Ѹ��Ƴɹ������ļ�ϵͳ���֣�");
  LED_BLUE;
  
  return result;

}


/**
  * @brief  scan_files �ݹ�ɨ��FatFs�ڵ��ļ�
  * @param  path:��ʼɨ��·��
  * @retval result:�ļ�ϵͳ�ķ���ֵ
  */
static FRESULT scan_files (char* path)  
{ 
  FRESULT res; 		//�����ڵݹ���̱��޸ĵı���������ȫ�ֱ���	
  FILINFO fno; 
  DIR dir; 
  int i;            
  char *fn;        // �ļ���	
	
#if _USE_LFN 
  /* ���ļ���֧�� */
  /* ����������Ҫ2���ֽڱ���һ�����֡�*/
  static char lfn[_MAX_LFN*2 + 1]; 	
  fno.lfname = lfn; 
  fno.lfsize = sizeof(lfn); 
#endif 
  //��Ŀ¼
  res = f_opendir(&dir, path); 
  if (res == FR_OK) 
	{ 
    i = strlen(path); 
    for (;;) 
		{ 
      //��ȡĿ¼�µ����ݣ��ٶ����Զ�����һ���ļ�
      res = f_readdir(&dir, &fno); 								
      //Ϊ��ʱ��ʾ������Ŀ��ȡ��ϣ�����
      if (res != FR_OK || fno.fname[0] == 0) break; 	
#if _USE_LFN 
      fn = *fno.lfname ? fno.lfname : fno.fname; 
#else 
      fn = fno.fname; 
#endif 
      //���ʾ��ǰĿ¼������			
      if (*fn == '.') continue; 	
      //Ŀ¼���ݹ��ȡ      
      if (fno.fattrib & AM_DIR)         
			{ 			
        //�ϳ�����Ŀ¼��        
        sprintf(&path[i], "/%s", fn); 
        BURN_INFO("�ļ��У�%s",path);
        //�ݹ����         
        res = scan_files(path);	
        path[i] = 0;         
        //��ʧ�ܣ�����ѭ��        
        if (res != FR_OK) 
					break; 
      } 
			else 
			{ 
				BURN_INFO("%s/%s", path, fn);								//����ļ���	
        /* ������������ȡ�ض���ʽ���ļ�·�� */        
      }//else
    } //for
  } 
  return res; 
}


#define COPY_UNIT 4096

static FIL fsrc,fdst;													/* �ļ����� */
static BYTE read_buf[COPY_UNIT]={0};        /* �������� */
static UINT real_read_num;            					  /* �ļ��ɹ���д���� */
static UINT real_write_num;            					  /* �ļ��ɹ���д���� */

char newfn[255];

/**
  * @brief  �����ļ�
  * @param  src_path:Դ�ļ�·��
  * @param  dst_path:�ļ���Ҫ���Ƶ���Ŀ¼(�����ļ���)
  * @retval result:�ļ�ϵͳ�ķ���ֵ
  */
static int copy_file(char *src_path,char *dst_path)
{
    FRESULT res = FR_OK; 	    
    char *sub_dir = NULL;  
  
    res = f_open(&fsrc,src_path,FA_READ); 
    require_noerr(res,exit);
  
    //��ȡ���ļ�·��
    sub_dir =  strrchr(src_path,'/'); 
    require_noerr(!sub_dir,exit);  
  
    //ƴ�ӳ���·��
    sprintf(newfn,"%s%s",dst_path,sub_dir);
  
    BURN_INFO("���ڸ����ļ� %s ...",newfn);
    res = f_open(&fdst,newfn,FA_CREATE_ALWAYS|FA_WRITE|FA_READ);
    require_noerr(res,exit);    
  
    //�����ļ�
    while(res == FR_OK)
    {
      res = f_read(&fsrc,&read_buf,COPY_UNIT,&real_read_num);  
      require_noerr(res,exit);
      
      res = f_write(&fdst,&read_buf,real_read_num,&real_write_num);
      require_noerr(res,exit);
      
      if(real_read_num != COPY_UNIT)
        break;
    }
    
    f_close(&fsrc);
    f_close(&fdst);
    
    BURN_INFO("���Ƴɹ���");

exit:
    return res;
}


/**
  * @brief  �����ļ���
  * @param  src_path:Դ�ļ���·��
  * @param  dst_path:Ҫ�����ﴴ�����ļ���
  * @retval result:�ļ�ϵͳ�ķ���ֵ
  */
static int copy_dir(char *src_path,char *dst_path)
{
    FRESULT res; 	    
    char *sub_dir = NULL;
    
    //��ȡ���ļ���·��
    sub_dir =  strrchr(src_path,'/'); 
    require_noerr(!sub_dir,exit);    
    
    BURN_DEBUG("��Ҫ������·����%s",dst_path);
  
    //����·��
    res = f_mkdir(dst_path); 
  
    //�ļ��б����ʹ���,����
    if(res == FR_EXIST)
      res = FR_OK;
  
    //���
    require_noerr(res,exit);  

exit:
    return res;
}


//���ڻ���Դ�ļ�������
static char fntemp[_MAX_LFN*2 + 1];

/**
  * @brief  �����ļ�(��Ŀ¼�����ļ��У���ݹ鸴��)
  * @param  src_path:Ҫ���Ƶ��ļ�(���ļ���)
  * @param  dst_path:Ҫ���Ƶ��ĸ��ļ���
  * @retval result:�ļ�ϵͳ�ķ���ֵ
  */
static FRESULT copy_all (char* src_path,char* dst_path)  
{ 
  FRESULT res; 		//�����ڵݹ���̱��޸ĵı���������ȫ�ֱ���	
  FILINFO fno; 
  DIR dir; 
  int i,j;            
  char *fn;        // �ļ���	
	
#if _USE_LFN 
  /* ���ļ���֧�� */
  /* ����������Ҫ2���ֽڱ���һ�����֡�*/
  static char lfn[_MAX_LFN*2 + 1]; 	
  fno.lfname = lfn; 
  fno.lfsize = sizeof(lfn); 
#endif 
   
  //��Ŀ¼
  res = f_opendir(&dir, src_path); 
  if (res == FR_OK) 
	{ 
    i = strlen(src_path);//Դ�ļ�·��
    j = strlen(dst_path);//Ŀ��·��
    for (;;) 
		{ 
      //��ȡĿ¼�µ����ݣ��ٶ����Զ�����һ���ļ�
      res = f_readdir(&dir, &fno); 								
      //Ϊ��ʱ��ʾ������Ŀ��ȡ��ϣ�����
      if (res != FR_OK || fno.fname[0] == 0) break; 	
#if _USE_LFN 
      fn = *fno.lfname ? fno.lfname : fno.fname; 
#else 
      fn = fno.fname; 
#endif 
      //���ʾ��ǰĿ¼������			
      if (*fn == '.') continue; 	
      //Ŀ¼���ݹ��ȡ      
      if (fno.fattrib & AM_DIR)         
			{ 			
        //�ϳ�����Ŀ¼��        
        sprintf(&src_path[i], "/%s", fn); 
        BURN_DEBUG("src dir=%s",src_path);
        
        sprintf(&dst_path[j], "/%s", fn);
        BURN_DEBUG("dst dir=%s",dst_path);        
        copy_dir(src_path,dst_path);

        //�ݹ����         
        res = copy_all(src_path,dst_path);	
        src_path[i] = 0;   
        dst_path[j] = 0;           
        //��ʧ�ܣ�����ѭ��        
        if (res != FR_OK) 
					break; 
      } 
			else 
			{ 
				BURN_DEBUG("%s/%s", src_path, fn);								//����ļ���	
        sprintf(fntemp,"%s/%s",src_path,fn);
        BURN_DEBUG("%s",fntemp);
        BURN_DEBUG("dst_path = %s",dst_path);
        /* ������������ȡ�ض���ʽ���ļ�·�� */        
        copy_file(fntemp,dst_path);
      }//else
    } //for
  } 
  return res; 
}








/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
