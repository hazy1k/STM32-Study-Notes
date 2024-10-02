/**
******************************************************************
* @file   bsp_sd_fatfs_test.c
* @author  fire
* @version V1.0
* @date    2018-xx-xx
* @brief   SD������
******************************************************************
* @attention
*
* ʵ��ƽ̨:Ұ��  i.MXRT1052������ 
* ��̳    :http://www.firebbs.cn
* �Ա�    :http://firestm32.taobao.com
*
******************************************************************
*/
#include "fsl_debug_console.h"

#include "board.h"                      
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_sd.h"
#include "./bsp/sd_fatfs_test/bsp_sd_fatfs_test.h"
#include "fsl_sdmmc_host.h"

#include "emXGUI.h"

static int Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength);

/*
*�������ܣ������ļ�ϵͳ
*�������ļ���������·�����磨/dir_1/hello.txt��
*����ֵ������״̬��FR_OK ��ʾ�ɹ���
*/
FRESULT f_mount_test(FATFS* fileSystem)
{
  FRESULT error = FR_OK;
  char ch = '0';
  BYTE work[FF_MAX_SS];
  const TCHAR driverNumberBuffer[3U] = {SDDISK + '0', ':', '/'};
  FATFS* g_fileSystem = fileSystem;
  
  error = f_mount(g_fileSystem, driverNumberBuffer, 0U);
  if (error)
  {
    /*���������ж�*/
    if(error == FR_NO_FILESYSTEM)//��û���ļ�ϵͳ
    {
      PRINTF("SD����û���ļ�ϵͳ�������ļ�ϵͳ�����ʽ������SD����\r\n ȷ���Ƿ������\r\n");
      PRINTF("���롮y��ȷ����ʽ�������롮n��ȡ��\r\n");
      while(true)
      {
        ch = GETCHAR();
        PUTCHAR(ch);
        if(ch =='y'|| ch == 'Y')
        {
          /*ΪSD�������ļ�ϵͳ*/
#if FF_USE_MKFS
          PRINTF("\r\n�����ļ�ϵͳ...... SD������Խ�󣬸ù��̳���ʱ��Խ����\r\n");
          if (f_mkfs(driverNumberBuffer, FM_ANY, 0U, work, sizeof work))
          {
            PRINTF("z�����ļ�ϵͳʧ��.\r\n");
            while(1);
            
          }
          else
          {
            PRINTF("�����ļ�ϵͳ�ɹ�.\r\n");
            error = f_mount(g_fileSystem, driverNumberBuffer, 0U);
            if(error)
            {
              PRINTF("�����ļ�ϵͳʧ��\r\n");
              while(1);
            }
            else
            {
              break;
            }
            
          }
#endif /* FF_USE_MKFS */
        }
        else if(ch =='n'|| ch == 'N')
        {
          /*����ֹͣ*/
         while(1);
        }
        
        PRINTF("���롮y��ȷ����ʽ�������롮n��ȡ��\r\n");
      }  
    }
    else//����������ʱ������ֱ���˳�����
    {
      PRINTF("�����ļ�ϵͳʧ��\r\n");
      while(1);
    }
  }
  else 
  {
    PRINTF("�����ļ�ϵͳ�ɹ�\r\n");
  }
  /*�ж��Ƿ�����ʹ�����·��*/
#if (FF_FS_RPATH >= 2U)
  error = f_chdrive((char const *)&driverNumberBuffer[0U]);
  if (error)
  {
    PRINTF("Change drive failed.\r\n");
    while(1);
  }
  else
  {
    PRINTF("Change drive success.\r\n");
  }
#endif
  return error;
    
}



/*
*�������ܣ�����һ�����ļ�
*�������ļ���������·�����磨/dir_1/hello.txt��
*����ֵ������״̬��FR_OK ��ʾ�ɹ���
*/
FRESULT f_touch_test(char* dir)
{
  FRESULT error = FR_OK;
  FIL g_fileObject ;   /* File object */
  PRINTF("\r\n������%s���ļ�......\r\n",dir);
  error = f_open(&g_fileObject, _T(dir), FA_CREATE_NEW);
  if (error)
  {
    if (error == FR_EXIST)
    {
      PRINTF("�ļ��Ѿ�����.\r\n");
    }
    else
    {
      PRINTF("�����ļ�ʧ��\r\n");
      return error;
    }
    
  }
  else
  {
    PRINTF("�����ļ��ɹ� \r\n");
  }
  return error;
    
}

/*
*��������:����һ��Ŀ¼
*����������dir��Ŀ¼·����
*����ֵ������״̬��FR_OK ��ʾ�ɹ���
*/
FRESULT f_mkdir_test(char* dir)
{
  FRESULT error;
  PRINTF("\r\n����Ŀ¼ ��%s��......\r\n",dir);
  error = f_mkdir(_T(dir));
  if (error)
  {
    if (error == FR_EXIST)
    {
      PRINTF("Ŀ¼�Ѿ�����\r\n");
    }
    else
    {
      PRINTF("����Ŀ¼ʧ��.\r\n");
      return error;
    }
  }
  else
  {
    PRINTF("����Ŀ¼�ɹ�\r\n");
  }
  return error;
}


/*
*�������ܣ��Զ���д�ķ�ʽ��һ���ļ�������ļ��������򴴽��ļ�����
*����������dir���ļ�·����fileObject�������ļ���������
*����ֵ������״̬��FR_OK ��ʾ�ɹ���
*/
FRESULT f_open_test(char* dir,FIL* fileObject)
{
  FRESULT error = FR_OK;
  FIL* g_fileObject = fileObject;   /* File object */
  PRINTF("\r\n�򿪡�%s���ļ�......\r\n",dir);
  error = f_open(g_fileObject, _T(dir), (FA_WRITE | FA_READ ));
  if (error)
  {
    if (error == FR_EXIST)
    {
      PRINTF("�ļ���ʧ��.\r\n");
    } 
  }
  else
  {
    PRINTF("�ļ��򿪳ɹ� \r\n");
  }
 
  return error;
}


/*
*�������ܣ��ر�һ���򿪵��ļ�
*����������fileObject�������ļ���������
*����ֵ������״̬��FR_OK ��ʾ�ɹ���
*/
FRESULT f_close_test(FIL* fileObject)
{
  FRESULT error = FR_OK;
  FIL* g_fileObject = fileObject;   /* File object */
  PRINTF("\r\n�ر��ļ�\r\n");
  error = f_close(g_fileObject);
  if (error)
  {
    PRINTF("�ر��ļ�ʧ��\r\n");
    return error;
  }
  else
  {
    PRINTF("�ر��ļ��ɹ�\r\n");
  }
  return error;
}


/*
*�������ܣ���ȡһ��·���µ�����
*����������dir��·������directory�� Ŀ¼����ṹ�壬����򿪵�Ŀ¼��Ϣ��
fileInformation���ļ���Ϣ�ṹ�壬�����ļ���Ϣ��
*����ֵ������״̬��FR_OK ��ʾ�ɹ���
*/
FRESULT f_readdir_test(char* dir,DIR* directory,FILINFO* fileInformation)
{
  FRESULT error = FR_OK;
  DIR* g_directory = directory; /* Directory object */
  FILINFO* g_fileInformation = fileInformation;
  
  
  PRINTF("\r\n�г���/dir_1��Ŀ¼�µ�����......\r\n");
  
  error = f_opendir(g_directory, dir);
  if (error)
  {
    PRINTF("��·��ʧ��\r\n");
    return error;
  }
  
  for (;;)
  {
    error = f_readdir(g_directory, g_fileInformation);
    
    /* To the end. */
    if ((error != FR_OK) || (g_fileInformation->fname[0U] == 0U))
    {
      break;
    }
    if (g_fileInformation->fname[0] == '.')
    {
      continue;
    }
    if (g_fileInformation->fattrib & AM_DIR)
    {
      PRINTF("�ļ��� : %s\r\n", g_fileInformation->fname);
    }
    else
    {
      PRINTF("�ļ� : %s\r\n", g_fileInformation->fname);
    }
  }
  return error;
}


/*
*�������ܣ���д����
*����������dir��·������data_write,ָ����Ҫд���ļ���������ʼ��ַ��data_read�����ļ������ص����ݱ����ַ��
*����ֵ������״̬��FR_OK ��ʾ�ɹ���
*/
FRESULT f_write_read_test(char* dir, void* data_write, void* data_read)
{
  FIL g_fileObject ;
  void* g_data_write = data_write;
  void* g_data_read = data_read;
  UINT g_bytesWritten;
  UINT g_bytesRead;
  FRESULT error = FR_OK;
  
  /*���ļ�*/
  error = f_open_test(dir,&g_fileObject);
  if ((error))
  {
    PRINTF("���ļ�ʧ��. \r\n");
    return error;
  }

  /* ���ļ���д������ */
  PRINTF("\r\nд�����ݵ���%s���ļ�\r\n",dir);
  error = f_write(&g_fileObject, g_data_write, sizeof(g_data_write), &g_bytesWritten);
  if ((error) || (g_bytesWritten != sizeof(g_data_write)))
  {
    PRINTF("д���ļ�ʧ��. \r\n");
  }
  else
  {
    PRINTF("д���ļ��ɹ� \r\n");
  }
  
  /* �ƶ��ļ���дָ�뵽�ļ���ʼ�� */
  if (f_lseek(&g_fileObject, 0U))
  {
    PRINTF("�����ļ���дָ��ʧ�� \r\n");
  }
  else
  {
    PRINTF("�����ļ���дָ��ɹ� \r\n");
  }
  
  /*��ȡ"/dir_1/f_1.dat"�ļ������ݵ� g_data_read ������*/
  PRINTF("��ȡ��%s���ļ�\r\n",dir);
  memset(g_data_read, 0U, sizeof(g_data_read));
  error = f_read(&g_fileObject, g_data_read, sizeof(g_data_read), &g_bytesRead);
  if ((error) || (g_bytesRead != sizeof(g_data_read)))
  {
    PRINTF("��ȡ�ļ�ʧ�� \r\n");
  }
  else
  {
    PRINTF("��ȡ�ļ��ɹ�. \r\n");
  }

  
  /*�Ƚ϶�д�����Ƿ�һ��*/
  PRINTF("�Ƚ϶�д����\r\n");
  if (0 == Buffercmp((uint8_t*)g_data_write, (uint8_t*)g_data_read, sizeof(g_data_read)))
  {
    PRINTF("�ļ���д���ݲ�һ��\r\n");
  }
  else
  {
    PRINTF("�ļ���д����һ��\r\n");
  }
 
  /*�ر��ļ�*/
  f_close_test(&g_fileObject);
  return error;
}

static int Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer1 != *pBuffer2)
    {
      return 0;
    }

    pBuffer1++;
    pBuffer2++;
  }
  return 1;
} 

/****************************END OF FILE**********************/
