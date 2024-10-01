

//#include "Include.h"
//#include "x_file.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <emXGUI.h>
#include <x_libc.h>
#include "backend_res_mgr.h"
#include "gui_resource_port.h"
#include "board.h"

/*============================================================================*/

/* ����buff�Ĵ�С��Ϊ��ֱ����ͬһ��С�� */
#define BUFFER_SIZE     1024
//SD��Դ����·������
static char *src_dir ;
const char src_dir_const[] = RESOURCE_DIR;

static FIL *fp;													/* file objects */
static char *full_file_name;
static char *line_temp ;
static u32 file_num = 0;

extern HWND wnd_res_writer_info_textbox ;
extern HWND wnd_res_writer_progbar;


#if defined(STM32H743xx) || defined(STM32F767xx)
#define SPI_FLASH_BufferWrite BSP_QSPI_Write
#define SPI_FLASH_BufferRead  BSP_QSPI_Read 
enum
{
  eRES_OK = QSPI_OK,
  eRES_ERROR = QSPI_ERROR
};
#elif defined(STM32F429_439xx) || defined(STM32F10X_HD) 
extern void SPI_FLASH_BulkErase_GUI(void);
enum 
{
  eRES_OK = 1,
  eRES_ERROR = 0
};
#endif


int Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength)
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
/**
  * @brief  ����Ƿ�Ҫ�����ļ��л��ļ�
  * @param  check_name[in] Ҫ�����ļ���,ȫ·��
  * @param  ignore_file[in] ignore�ļ���·����ȫ·��
  * @retval 1��ʾ���ԣ�0��ʾ������
  */
uint8_t Is_Ignore(char *check_name, char *ignore_file)
{
  FRESULT result;
  uint32_t i;
  
  char *ignore_file_dir;
  char *dir_ptr ;
  char *full_path;
  
  result = f_open(fp, ignore_file, FA_OPEN_EXISTING | FA_READ);
  
  if(result == FR_NO_FILE)
  {
      BURN_DEBUG("�����ں����ļ���ֱ�ӷ���");
      return 0;
  }   
  
  ignore_file_dir = GUI_VMEM_Alloc(512);
  full_path = GUI_VMEM_Alloc(512);
  
  if(ignore_file_dir == NULL || full_path == NULL)
  {
    BURN_ERROR("����mallocʱ�ڴ治�㣡");
    return 0;
  }
  
  /* �õ�ignore�ļ����ڵ�Ŀ¼ */
  strcpy(ignore_file_dir, ignore_file);
  dir_ptr = strrchr(ignore_file_dir, '/');
  *(dir_ptr) = '\0';  

  /* ���������ļ� */
  for(i=0;1;i++)
  {
     /* ��һ�к��Ե��ļ��� */
    f_gets(line_temp, BUFFER_SIZE, fp);
    /* ע���У�����*/
    if(line_temp[0] == '#')
      continue;
    
    /* �滻���س� */
    line_temp[strlen(line_temp)-1] = '\0';
    
    /* ƴ�ӳ������ĺ����ļ�·�� */
    sprintf(full_path, "%s/%s",ignore_file_dir,line_temp);
  
//    BURN_DEBUG("full_path=%s,\r\ncheck_name=%s\r\n",
//                full_path,
//                check_name);
    /* �Ƚ��Ƿ�һ�� */
    if(strcasecmp(check_name,full_path) == 0)
    {      
      f_close(fp);
      GUI_VMEM_Free(ignore_file_dir);
      GUI_VMEM_Free(full_path);
      
      return 1;
    }
    /* �ѵ��ļ�β���������,����ѭ�� */
    if(f_eof(fp) != 0 )   
    { 
      f_close(fp);
      break;
    }
  }  
  
  GUI_VMEM_Free(ignore_file_dir);
  GUI_VMEM_Free(full_path);
  
  return 0;
}

/**
  * @brief  Make_Catalog ������ԴĿ¼�ļ������Ǹ��ݹ麯��
  * @param  path[in]:��Դ·������������һ���ռ��㹻�����飬
                     ����ִ��ʱ���������·����ֵ����
  * @param  clear: 0 ��ʾ��������Ŀ¼�ļ���1��ʾ��ԭ�ļ���׷������
  * @note   ���ⲿ���ñ�����ʱ��clear��������Ϊ0��
  *         �����ڲ��ݹ���ò�����Ϊ1.
  * @retval result:�ļ�ϵͳ�ķ���ֵ
  */
FRESULT Make_Catalog (char* path,uint8_t clear)  
{ 
  FRESULT res; 		//�����ڵݹ���̱��޸ĵı���������ȫ�ֱ���	
  FILINFO fno; 
  DIR dir; 
  int i;            
  char *fn;        // �ļ���	
   
  /* ��¼��ַƫ����Ϣ */
  static uint32_t resource_addr = CATALOG_SIZE ;
    
#if _USE_LFN 
  /* ���ļ���֧�� */
  /* ����������Ҫ2���ֽڱ���һ�����֡�*/
  static char lfn[_MAX_LFN*2 + 1]; 	
  fno.lfname = lfn; 
  fno.lfsize = sizeof(lfn); 
#endif 
  
  if(clear == 0)
  {  
    BURN_INFO("����������¼��Ϣ�ļ�catalog.txt...\r\n"); 

    /* ���ý����� */
    SendMessage(wnd_res_writer_progbar,PBM_SET_VALUE,TRUE,0);
    SetWindowText(wnd_res_writer_progbar,L"Creating catalog...");
    /* ���ý��������ֵΪ100 */
    SendMessage(wnd_res_writer_progbar,PBM_SET_RANGLE,TRUE,100);

    SetWindowText(wnd_res_writer_info_textbox,L"Creating catalog file...");
    GUI_msleep(20);
    
    /* ��һ��ִ��Make_Catalog����ʱɾ���ɵ���¼��Ϣ�ļ� */
    f_unlink(BURN_INFO_NAME_FULL);
  }
  
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
        
        BURN_INFO("-------------------------------------");
        BURN_INFO("�ļ��У�%s",path);
        
        /* �����ָ�ignore�ļ��м�¼����ͬ������*/
        if(Is_Ignore(path, IGNORE_NAME_FULL) == 1)
        {
          BURN_INFO("�����ļ��У�%s",path);
          continue;
        }
        //�ݹ����         
        res = Make_Catalog(path,1);	
        path[i] = 0;         
        //��ʧ�ܣ�����ѭ��        
        if (res != FR_OK) 
					break; 
      } 
			else 
			{ 

        uint32_t file_size; 
        
        /* ���Ա����Ŀ¼�ļ� �� �����ļ� */
        if(strcasecmp(fn,BURN_INFO_NAME) == 0 ||
            strcasecmp(fn,IGNORE_NAME) == 0)
          continue;     

        /* �õ�ȫ�ļ��� */
        sprintf(full_file_name, "%s/%s",path, fn); 
        
        /* �����ָ�ignore�ļ��м�¼����ͬ������*/
        if(Is_Ignore(full_file_name, IGNORE_NAME_FULL) == 1)
        {
          BURN_INFO("-------------------------------------");

          BURN_INFO("�����ļ���%s",full_file_name);
          continue;
        }
        
        f_open(fp, full_file_name, FA_OPEN_EXISTING | FA_READ);
        file_size = f_size(fp);
        f_close(fp);
        
        BURN_INFO("-------------------------------------"); 
				BURN_INFO("��Դ�ļ�·��:%s", full_file_name);			//�����ļ���	
        BURN_INFO("��Դ�ļ���:%s", fn);						     //�ļ���         
        BURN_INFO("��Դ��С:%d", file_size);				 //�ļ���С	
        BURN_INFO("Ҫ��¼����ƫ�Ƶ�ַ:%d�����Ե�ַ:%d", 
                          resource_addr, 
                          resource_addr + RESOURCE_BASE_ADDR  );	 //�ļ���Ҫ�洢������ԴĿ¼	
        
        f_open(fp, BURN_INFO_NAME_FULL, FA_OPEN_ALWAYS |FA_WRITE | FA_READ);
        
        f_lseek(fp, f_size(fp));
        /* ÿ���ļ���¼ռ5�� */
        f_printf(fp, "%s\n", full_file_name);			//�����ļ���	
        f_printf(fp, "%s\n", fn);						     //�ļ���         
        f_printf(fp, "%d\n", file_size);				 //�ļ���С	
        f_printf(fp, "%d\n\n", resource_addr);	 //�ļ���Ҫ�洢������ԴĿ¼(δ���ϻ���ַ)	

        /* ��¼�ļ��� */
        file_num++;
        if(file_num < 100)
        {
          SendMessage(wnd_res_writer_progbar,PBM_SET_VALUE,TRUE,file_num);
        }
        else
        {
          /* �������������ֵʱ��ʾ99 */
          SendMessage(wnd_res_writer_progbar,PBM_SET_VALUE,TRUE,99);
        }
        GUI_msleep(20);
        
        f_close(fp);

        resource_addr += file_size; /* ƫ���ļ��Ĵ�С */

        /* ������������ȡ�ض���ʽ���ļ�·�� */        
      }//else
    } //for
    f_closedir(&dir);
  }
  else
  {
    printf("�ļ���ʧ��\n");
  }
  
  /* �����¼�ļ���Ϣ */
  if(clear == 0)
  {
    BURN_INFO("-------------------------------------\r\n"); 
    /* ִ�������ʱresource_addr�����һ����Դ��β��ַ */
    BURN_INFO("������¼�������ݴ�С:%d ����Ŀ¼��", resource_addr );	 

    BURN_INFO("��¼��ռ�õĵ�ַ�ռ�:%d - %d",
                                      RESOURCE_BASE_ADDR, 
                                      resource_addr + RESOURCE_BASE_ADDR );	 
    
    /* ��ʾ��� */
    SendMessage(wnd_res_writer_progbar,PBM_SET_VALUE,TRUE,100);
    GUI_msleep(20);

    }
      
  return res; 
}



/**
  * @brief  ��ȡĿ¼�ļ�����Ϣ��
  * @param  catalog_name[in] �洢��Ŀ¼��Ϣ���ļ���
  * @param  file_index ��file_index����¼
  * @param  dir[out] Ҫ��¼��FLASH��Ŀ¼��Ϣ
  * @param  full_file_name[out] ��file_index����¼�е��ļ���ȫ·��
  * @retval 1��ʾ�������ļ�β��0Ϊ����
  */
uint8_t Read_CatalogInfo(uint32_t file_index,
                            CatalogTypeDef *dir,
                            char *full_name)
{
  uint32_t i;

  f_open(fp, BURN_INFO_NAME_FULL, FA_OPEN_EXISTING | FA_READ);

  /* ����ǰN��,ÿ���ļ���¼5�� */
  for(i=0;i<file_index*5;i++)
  {
    f_gets(line_temp, BUFFER_SIZE, fp);
  }
  /* �����ļ�β*/
  if(f_eof(fp) != 0 )
    return 1;
  
  /* �ļ�ȫ·�� */
  f_gets(line_temp, BUFFER_SIZE, fp);
  memcpy(full_name, line_temp, strlen(line_temp)+1);
  /* �滻���س� */
  full_name[strlen(full_name)-1] = '\0';
  
  /* �ļ��� */
  f_gets(line_temp, BUFFER_SIZE, fp);
  memcpy(dir->name, line_temp, strlen(line_temp)> (sizeof(CatalogTypeDef)-8) ? (sizeof(CatalogTypeDef)-8):strlen(line_temp)+1 );
  dir->name[strlen(dir->name)-1] = '\0';
  
  /* �ļ���С */
  f_gets(line_temp, BUFFER_SIZE, fp);
  dir->size = atoi(line_temp);
  
  /* �ļ�Ҫ��¼��λ�� */
  f_gets(line_temp, BUFFER_SIZE, fp);
  dir->offset = atoi(line_temp);
  
  f_close(fp);  
  
  BURN_DEBUG("ful name=%s,\r\nname=%s,\r\nsize=%d,\r\noffset=%d\r\n",
              full_name,
              dir->name,
              dir->size,
              dir->offset
              );
  
  return 0;
}

/**
  * @brief  ��¼Ŀ¼��FLASH��
  * @param  catalog_name ��¼����¼��Ϣ��Ŀ¼�ļ���
  * @note  ʹ�ñ�����ǰ��Ҫȷ��flash��Ϊ����״̬
  * @retval 1��ʾ�������ļ�β��0Ϊ����
  */
void Burn_Catalog(void)
{
  CatalogTypeDef dir;
  uint8_t i;
  uint8_t is_end;
//  uint8_t* rx_buff;
  /* ���ý����� */
  SendMessage(wnd_res_writer_progbar,PBM_SET_VALUE,TRUE,0);
  SetWindowText(wnd_res_writer_progbar,L"Writing catalog...");
  /* �������ֵ */
  SendMessage(wnd_res_writer_progbar,PBM_SET_RANGLE,TRUE,file_num);
  SetWindowText(wnd_res_writer_info_textbox,L"Writing catalog...");

  GUI_msleep(20);

  /* ����Ŀ¼�ļ� */
  for(i=0;1;i++)
  {
    uint8_t state = eRES_ERROR;
    is_end = Read_CatalogInfo(i, 
                                &dir,
                              full_file_name);  
    /* �ѱ������,����ѭ�� */
    if(is_end !=0)   
      break;
    
    /* ���½����� */
    SendMessage(wnd_res_writer_progbar,PBM_SET_VALUE,TRUE,i);
    GUI_msleep(20);

    /* ��dir��Ϣ��¼��FLASH�� */  
    state = SPI_FLASH_BufferWrite((u8*)&dir,RESOURCE_BASE_ADDR + sizeof(dir)*i,sizeof(dir));
//    rx_buff = (uint8_t *)GUI_VMEM_Alloc(sizeof(dir));
//    SPI_FLASH_BufferRead(rx_buff,RESOURCE_BASE_ADDR + sizeof(dir)*i,sizeof(dir));
//    
//    printf("%d:%d\n",i,Buffercmp((u8*)&dir, rx_buff, sizeof(dir)));
//    
//    
//    GUI_VMEM_Free(rx_buff);
    if(state != eRES_OK)
      printf("Error Write\n");
  }
  
  SendMessage(wnd_res_writer_progbar,PBM_SET_VALUE,TRUE,file_num);
  SetWindowText(wnd_res_writer_info_textbox,L"Writing catalog complete.");
  GUI_msleep(200);

}

/**
  * @brief  ����Ŀ¼�ļ���Ϣ��¼����
  * @param  catalog_name ��¼����¼��Ϣ��Ŀ¼�ļ���
  * @note  ʹ�ñ�����ǰ��Ҫȷ��flash��Ϊ����״̬
  * @retval �ļ�ϵͳ��������ֵ
  */
FRESULT Burn_Content(void)
{  
  CatalogTypeDef dir;
  uint8_t i;
  uint8_t is_end;   
//  uint8_t* rx_buff;
  FRESULT result;   
  UINT  bw;            					    /* File R/W count */
  uint32_t write_addr=0;
  uint8_t tempbuf[512];
  
  /* ���ý����� */
  SendMessage(wnd_res_writer_progbar,PBM_SET_VALUE,TRUE,0);
  SetWindowText(wnd_res_writer_progbar,L"Writing file");
  /* �������ֵ*/
  SendMessage(wnd_res_writer_progbar,PBM_SET_RANGLE,TRUE,file_num);
  GUI_msleep(20);

 
  /* ����Ŀ¼�ļ� */
  for(i=0;1;i++)
  {
    is_end = Read_CatalogInfo(i, 
                                &dir,
                                full_file_name);  
    /* �ѱ������,����ѭ�� */
    if(is_end !=0)   
      break;  
  
    BURN_INFO("-------------------------------------"); 
    BURN_INFO("׼����¼���ݣ�%s",full_file_name);
    
    x_wsprintf((WCHAR*)tempbuf,L"Writing file %d/%d.\r\nWriting big files will take a long \r\ntime.Please wait...",i,file_num);
    SetWindowText(wnd_res_writer_info_textbox,(WCHAR*)tempbuf);
    SendMessage(wnd_res_writer_progbar,PBM_SET_VALUE,TRUE,i);

    GUI_msleep(20);

    LED_BLUE;
     
     result = f_open(fp,full_file_name,FA_OPEN_EXISTING | FA_READ);
      if(result != FR_OK)
      {
          BURN_ERROR("���ļ�ʧ�ܣ�");
          LED_RED;
          return result;
      }
  
      BURN_INFO("������FLASHд������...");
      
      write_addr = dir.offset + RESOURCE_BASE_ADDR;
      while(result == FR_OK) 
      {
        result = f_read( fp, tempbuf, 512, &bw);//��ȡ����	 
        if(result!=FR_OK)			 //ִ�д���
        {
          BURN_ERROR("��ȡ�ļ�ʧ�ܣ�->(%d)", result);
          LED_RED;
          return result;
        }      
        SPI_FLASH_BufferWrite(tempbuf,write_addr,bw);  //�������ݵ��ⲿflash��    
        
//        rx_buff = (uint8_t *)GUI_VMEM_Alloc(bw);
//        SPI_FLASH_BufferRead(rx_buff,write_addr,bw);
//    
//        printf("%d:%d --%d\n",i,Buffercmp(tempbuf, rx_buff, bw),bw);
//    
//    
//        GUI_VMEM_Free(rx_buff);

        
        write_addr+=bw;				
        if(bw !=512)break;
      }
      BURN_INFO("����д�����");          
           
    f_close(fp);     
  }
  
  BURN_INFO("************************************");
  BURN_INFO("�����ļ�������¼��ϣ������ļ�ϵͳ���֣�");
  
  SendMessage(wnd_res_writer_progbar,PBM_SET_VALUE,TRUE,file_num);
  SetWindowText(wnd_res_writer_info_textbox,L"All file have been complete.");
  GUI_msleep(20);

  return FR_OK;
}

/**
  * @brief  У��д�������
  * @param  catalog_name ��¼����¼��Ϣ��Ŀ¼�ļ���
  * @retval �ļ�ϵͳ��������ֵ
*/
FRESULT Check_Resource(void)
{

  CatalogTypeDef dir;
  uint8_t i;
  uint8_t is_end;
  int offset;
  
  FRESULT result; 
  UINT  bw;            					    /* File R/W count */
  uint32_t read_addr=0,j=0;
  uint8_t tempbuf[256],flash_buf[256];
  
  /* ���ý����� */
  SendMessage(wnd_res_writer_progbar,PBM_SET_VALUE,TRUE,0);
  SetWindowText(wnd_res_writer_progbar,L"Checking file");

  /* �������ֵ */
  SendMessage(wnd_res_writer_progbar,PBM_SET_RANGLE,TRUE,file_num);
  GUI_msleep(20);
 
  /* ����Ŀ¼�ļ� */
  for(i=0;1;i++)
  {
    is_end = Read_CatalogInfo(i, 
                                &dir,
                                full_file_name);  
    /* �ѱ������,����ѭ�� */
    if(is_end !=0)   
      break;    
    
    x_wsprintf((WCHAR *)tempbuf,L"Checking file %d/%d.\r\nChecking big files will take a long time.\r\nPlease wait...",i,file_num);
    SetWindowText(wnd_res_writer_info_textbox,(WCHAR *)tempbuf);
    SendMessage(wnd_res_writer_progbar,PBM_SET_VALUE,TRUE,i);

    GUI_msleep(20);
    
    /* ��FLASH��Ŀ¼����Ҷ�Ӧ��offset */
    offset = RES_GetOffset(dir.name);
    if(offset == -1)
    {
      LED_RED;
      BURN_INFO("У��ʧ�ܣ��޷���FLASH���ҵ��ļ���%s ��Ŀ¼",dir.name);
      return FR_NO_FILE;
    }
    else
      dir.offset = offset;
       
    BURN_INFO("-------------------------------------"); 
    BURN_INFO("׼��У�����ݣ�%s",full_file_name);
    BURN_INFO("��������%s ",dir.name);
    BURN_INFO("���ݾ��Ե�ַ��%d ",dir.offset);
    BURN_INFO("���ݴ�С��%d ",dir.size);
    
      result = f_open(fp,full_file_name,FA_OPEN_EXISTING | FA_READ);
      if(result != FR_OK)
      {
          BURN_ERROR("���ļ�ʧ�ܣ�");
              LED_RED;
          return result;
      }
          
       //У������
      read_addr = dir.offset;
     
      f_lseek(fp,0);
      
      while(result == FR_OK) 
      {
        result = f_read( fp, tempbuf, 256, &bw);//��ȡ����	 
        if(result!=FR_OK)			 //ִ�д���
        {
          BURN_ERROR("��ȡ�ļ�ʧ�ܣ�");
          LED_RED;
          return result;
        }      
        SPI_FLASH_BufferRead(flash_buf,read_addr,bw);  //��FLASH�ж�ȡ����
        read_addr+=bw;		
        
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

      BURN_INFO("����У��������");
      LED_BLUE;
               
      f_close(fp);     
  }
  
  LED_GREEN;
  BURN_INFO("************************************");
  BURN_INFO("�����ļ�У�������������ļ�ϵͳ���֣�");
  
  SendMessage(wnd_res_writer_progbar,PBM_SET_VALUE,TRUE,file_num);
  SetWindowText(wnd_res_writer_info_textbox,L"All files check normal!");
  GUI_msleep(20);

  return FR_OK;
}


/**
  * @brief  ������¼����
  * @param  ��
  * @retval ��
  */
FRESULT BurnFile(void)
{
  FRESULT result;   
  DIR dir; 
  
  /* ��ʼ������ȫ�ֱ��� */
  {
   src_dir =  (char *)GUI_VMEM_Alloc(BUFFER_SIZE);
   full_file_name =  (char *)GUI_VMEM_Alloc(BUFFER_SIZE);
   line_temp =  (char *)GUI_VMEM_Alloc(BUFFER_SIZE);
   fp = (FIL *)GUI_VMEM_Alloc(sizeof(FIL));
  }

//  BURN_INFO("ע��ò������FLASH��ԭ���ݻᱻɾ������");   
  file_num = 0;
  
  /* ���Ƴ�ʼ·�� */
  strcpy(src_dir,src_dir_const);//�ֿ���Դ�ļ����λ��
  
  /* ��·������ */
  result = f_opendir(&dir, src_dir); 

  if(result != FR_OK)
  {
    GUI_ERROR("��������srcdata����¼���ݵ�SD��,�����¸�λ�����壡 result = %d",result);
    SetWindowText(wnd_res_writer_info_textbox,L"1.Please insert an SD card with \r\n[srcdata] resources.\r\n2.Powerup again the board.");
    GUI_msleep(20);
    
    goto exit;
  }
  f_closedir(&dir);
  
  SetWindowText(wnd_res_writer_info_textbox,L"  Erasing FLASH,it will take a long \r\ntime,please wait...");
  GUI_msleep(20);
  
  BURN_INFO("���ڽ�����Ƭ������ʱ��ܳ��������ĵȺ�...");  

  /* ��ƬFLASH���� */
#if defined(STM32F429_439xx)  
  SPI_FLASH_BulkErase_GUI();
#elif defined(STM32H743xx)  
  if(BSP_QSPI_Erase_Chip() != eRES_OK)
  {
    printf("Erase Error\n");
    while(1);
  }
#elif defined(STM32F767xx)  
  if(BSP_QSPI_Erase_Chip() != eRES_OK)
  {
    printf("Erase Error\n");
    while(1);
  }
#elif defined(STM32F10X_HD)  
  SPI_FLASH_BulkErase();
#endif  
  /* ������¼Ŀ¼��Ϣ�ļ� */
  Make_Catalog(src_dir,0);
  
  /* ��¼ Ŀ¼��Ϣ��FLASH*/
  Burn_Catalog();  
  /* ���� Ŀ¼ ��¼������FLASH*/
  Burn_Content();
  /* У����¼������ */
  //result =  Check_Resource();

exit:
  /* �ͷ�����Ŀռ� */
  {
    GUI_VMEM_Free(src_dir);
    GUI_VMEM_Free(full_file_name);
    GUI_VMEM_Free(line_temp);
    GUI_VMEM_Free(fp);
  }
    
  return result;
}

/*********************************************END OF FILE**********************/

