#include "stm32f10x.h"
#include "usart.h"
#include "ff.h"
#include "string.h"

FATFS fs; // FatFs�ļ�ϵͳ����
FIL fnew; // �´������ļ�����
FRESULT res_flash; // �ļ��������
UINT fnum; // �ļ��ɹ���д����
char fpath[100]; // �ļ�·��
char readbuffer[512]; // ��д������

// FatFs�������ܲ���
static FRESULT miscellaneous(void)
{
  DIR dir; // Ŀ¼�ļ�
  FATFS *pfs; // �ļ�ϵͳ����
  DWORD fre_clust, fre_sect, tot_sect; // ʣ�������ʣ������������������

  /* �豸��Ϣ��ȡ */
  printf("��һ����ԣ��豸��Ϣ��ȡ\r\n");
  res_flash = f_getfree("1:", &fre_clust, &pfs); // �����������������š�ʣ�����ָ�롢�ļ�ϵͳ����ָ��
  // ����õ������������Ϳմش�С
  tot_sect = (pfs->n_fatent-2)*pfs->csize;
  fre_sect = fre_clust * pfs->csize;
  printf("�豸�����ܿռ䣺%10lu KB \n �豸ʣ��ռ䣺%10lu KB \n", tot_sect*4, fre_sect*4);

  /* �����ļ�ϵͳ��ʽ����д�� */
  printf("�ڶ�����ԣ��ļ�ϵͳ��ʽ����д��\r\n");
  res_flash = f_open(&fnew, "1:FatFS��д�����ļ�.txt", FA_OPEN_ALWAYS|FA_WRITE|FA_READ);
  if(res_flash == FR_OK)
  {
    // �ļ���λ
    res_flash = f_lseek(&fnew, f_size(&fnew)); // �����������ļ������ļ�ָ��λ��
    if(res_flash == FR_OK)
    {
      // ��ʽ��д��
      f_printf(&fnew, "��ԭ���ļ������һ�����ݣ�\r\n");
      f_printf(&fnew, "�豸�ܿռ䣺%10lu KB \n �豸ʣ��ռ䣺%10lu KB \n", tot_sect*4, fre_sect*4);

      // �ļ���λ����ʼλ��
      res_flash = f_lseek(&fnew, 0);
      res_flash = f_read(&fnew, readbuffer, f_size(&fnew), &fnum);
      if(res_flash == FR_OK)
      {
        printf("�ļ����ݣ�\r\n%s", readbuffer);
      }
    }
    f_close(&fnew); // �ر��ļ�

    /* Ŀ¼���������������ܲ��� */
    printf("��������ԣ�Ŀ¼���������������ܲ���\r\n");
    res_flash = f_opendir(&dir, "1:TestDir");
    if(res_flash != FR_OK)
    {
      res_flash = f_mkdir("1:TestDir"); // ����Ŀ¼
    }
    else
    {
      // ���Ŀ¼�Ѿ����ڣ��ر���
      res_flash = f_closedir(&dir);
      f_unlink("1:TestDir/testdir.txt");
    }
    if(res_flash == FR_OK)
    {
      // ���������ƶ��ļ�
      res_flash = f_rename("1:FatFS��д�����ļ�.txt","1:TestDir/testdir.txt");
    }
  }
  else
  {
    printf("�ļ���ʧ��: %d\r\n", res_flash);
    printf("�ļ�ϵͳ��ʽ��ʧ�ܣ�\r\n");
  }
  return res_flash;
}

FILINFO fno; // �ļ���Ϣ����

// �ļ���ȡ
static FRESULT file_check(void)
{
  res_flash = f_stat("1:TestDir/testdir.txt", &fno);
  if(res_flash == FR_OK)
  {
    printf("testdir.txt�ļ���Ϣ:\n");
    printf("�ļ���С: %ld(�ֽ�)\n", fno.fsize);
        printf("ʱ�����Ϣ: %u/%02u/%02u, %02u:%02u\n",
           (fno.fdate >> 9) + 1980, fno.fdate >> 5 & 15, fno.fdate & 31,fno.ftime >> 11, fno.ftime >> 5 & 63);
    printf("�ļ�����: %c%c%c%c%c\n\n",
           (fno.fattrib & AM_DIR) ? 'D' : '-',  // Ŀ¼
           (fno.fattrib & AM_RDO) ? 'R' : '-',  // ֻ��
           (fno.fattrib & AM_HID) ? 'H' : '-',  // ����
           (fno.fattrib & AM_SYS) ? 'S' : '-',  // ϵͳ
           (fno.fattrib & AM_ARC) ? 'A' : '-'); // �浵
  }
  return res_flash;
}

// �ݹ�ɨ��FatFs�ڵ��ļ�
static FRESULT scan_files (char* path)
{ 
  FRESULT res; 	
  FILINFO fno; 
  DIR dir; 
  int i;            
  char *fn;   
	
#if _USE_LFN 
  static char lfn[_MAX_LFN*2 + 1]; 	
  fno.lfname = lfn; 
  fno.lfsize = sizeof(lfn); 
#endif 
  res = f_opendir(&dir, path); 
  if (res == FR_OK) 
	{ 
    i = strlen(path); 
    for (;;) 
		{ 
      res = f_readdir(&dir, &fno); 								
      if (res != FR_OK || fno.fname[0] == 0) break; 	
#if _USE_LFN 
      fn = *fno.lfname ? fno.lfname : fno.fname; 
#else 
      fn = fno.fname; 
#endif 	
      if (*fn == '.') continue; 	   
      if (fno.fattrib & AM_DIR)         
			{ 			    
        sprintf(&path[i], "/%s", fn); 		      
        res = scan_files(path);	
        path[i] = 0;              
        if (res != FR_OK) 
					break; 
      } 
			else 
			{ 
				printf("%s/%s\r\n", path, fn);     
      }
    }
  } 
  return res; 
}

int main(void)
{
  USART_Config();
  res_flash = f_mount(&fs, "1:", 1); // ����FatFs�ļ�ϵͳ
  if(res_flash != FR_OK)
  {
    printf("�ļ�ϵͳ����ʧ�ܣ�\r\n");
    while(1);
  }
  else{
    printf("FatFs�ļ�ϵͳ��ʼ���ɹ���\r\n");
  }
  printf("FatFs�ļ�ϵͳ���ú�������\r\n");
  res_flash = miscellaneous(); // FatFs�������ܲ���
  printf("�ļ�ɨ�����");
  res_flash = file_check();
  strcpy(fpath, "1:");
  scan_files(fpath);
  f_mount(NULL, "1:", 1); // ж��FatFs�ļ�ϵͳ
  while(1);
}


