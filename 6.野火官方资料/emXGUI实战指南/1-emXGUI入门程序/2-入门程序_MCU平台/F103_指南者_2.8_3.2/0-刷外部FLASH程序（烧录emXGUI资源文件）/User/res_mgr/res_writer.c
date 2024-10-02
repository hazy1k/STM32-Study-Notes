

//#include "Include.h"
//#include "x_file.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "./res_mgr/RES_MGR.h"
#include "ff.h"

#include "./led/bsp_led.h"
#include "./spi_flash/bsp_flash_spi.h"

/*============================================================================*/

/*flash��sd�����ļ�ϵͳ���*/
FATFS flash_fs;
FATFS sd_fs;													/* Work area (file system object) for logical drives */

//SD��Դ����·������
char src_dir[512]= RESOURCE_DIR;

static FIL file_temp;													/* file objects */
static char full_file_name[512];
static char line_temp[512];
static uint8_t state = 0;
uint32_t loop = 0;



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
  
  result = f_open(&file_temp, ignore_file, FA_OPEN_EXISTING | FA_READ);
  
  if(result == FR_NO_FILE)
  {
      BURN_DEBUG("�����ں����ļ���ֱ�ӷ���");
      return 0;
  }   
  
  ignore_file_dir = malloc(512);
  full_path = malloc(512);
  
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
    f_gets(line_temp, sizeof(line_temp), &file_temp);
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
      f_close(&file_temp);
      free(ignore_file_dir);
      free(full_path);
      
      return 1;
    }
    /* �ѵ��ļ�β���������,����ѭ�� */
    if(f_eof(&file_temp) != 0 )   
    { 
      f_close(&file_temp);
      break;
    }
  }  
  
  free(ignore_file_dir);
  free(full_path);
  
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
  uint32_t addr_temp = 0;
    
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
   
    /* ��һ��ִ��Make_Catalog����ʱɾ���ɵ���¼��Ϣ�ļ� */
//    f_unlink(BURN_INFO_NAME_FULL);
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
        
        f_open(&file_temp, full_file_name, FA_OPEN_EXISTING | FA_READ);
        file_size = f_size(&file_temp);
        f_close(&file_temp);
        
        BURN_INFO("-------------------------------------"); 
				BURN_INFO("��Դ�ļ�·��:%s", full_file_name);			//�����ļ���	
        BURN_INFO("��Դ�ļ���:%s", fn);						     //�ļ���         
        BURN_INFO("��Դ��С:%d", file_size);				 //�ļ���С	
        BURN_INFO("Ҫ��¼����ƫ�Ƶ�ַ:%d�����Ե�ַ:%d", 
                          resource_addr, 
                          resource_addr + RESOURCE_BASE_ADDR  );	 //�ļ���Ҫ�洢������ԴĿ¼	
        
        f_open(&file_temp, BURN_INFO_NAME_FULL, FA_OPEN_ALWAYS |FA_WRITE | FA_READ);
        
        f_lseek(&file_temp, f_size(&file_temp));
        /* ÿ���ļ���¼ռ5�� */
        f_printf(&file_temp, "%s\n", full_file_name);			//�����ļ���	
        f_printf(&file_temp, "%s\n", fn);						     //�ļ���         
        f_printf(&file_temp, "%d\n", file_size);				 //�ļ���С	
        f_printf(&file_temp, "%d\n\n", resource_addr);	 //�ļ���Ҫ�洢������ԴĿ¼(δ���ϻ���ַ)	

        f_close(&file_temp);

        /* ���е�ַ�������㣬ƫ���ļ�+4096����Ĵ�С */
//        addr_temp = (uint32_t)(file_size+4096)/4096;
//        addr_temp = (uint32_t)addr_temp*4096;
//        
//        resource_addr += addr_temp; /* ƫ���ļ��Ĵ�С */
        
        resource_addr += file_size; /* ƫ���ļ��Ĵ�С */

        /* ������������ȡ�ض���ʽ���ļ�·�� */        
      }//else
    } //for
  }
  else
  {
    BURN_ERROR("��Ŀ¼ʧ��");
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
  
  f_open(&file_temp, BURN_INFO_NAME_FULL, FA_OPEN_EXISTING | FA_READ);
  
  /* ����ǰN��,ÿ���ļ���¼5�� */
  for(i=0;i<file_index*5;i++)
  {
    f_gets(line_temp, sizeof(line_temp), &file_temp);
  }
  /* �����ļ�β*/
  if(f_eof(&file_temp) != 0 )
    return 1;
  
  /* �ļ�ȫ·�� */
  f_gets(line_temp, sizeof(line_temp), &file_temp);
  memcpy(full_name, line_temp, strlen(line_temp)+1);
  /* �滻���س� */
  full_name[strlen(full_name)-1] = '\0';
  
  /* �ļ��� */
  f_gets(line_temp, sizeof(line_temp), &file_temp);
  memcpy(dir->name, line_temp, strlen(line_temp)> (sizeof(CatalogTypeDef)-8) ? (sizeof(CatalogTypeDef)-8):strlen(line_temp)+1 );
  dir->name[strlen(dir->name)-1] = '\0';
  
  /* �ļ���С */
  f_gets(line_temp, sizeof(line_temp), &file_temp);
  dir->size = atoi(line_temp);
  
  /* �ļ�Ҫ��¼��λ�� */
  f_gets(line_temp, sizeof(line_temp), &file_temp);
  dir->offset = atoi(line_temp);
  
  f_close(&file_temp);  
  
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
  
  /* ����Ŀ¼�ļ� */
  for(i=0;1;i++)
  {
    is_end = Read_CatalogInfo(i, 
                                &dir,
                                full_file_name);  
    /* �ѱ������,����ѭ�� */
    if(is_end !=0)   
      break;
    loop = 0;
    
    if(sizeof(dir) == 0)break;//Ϊ0ʱ�����ж�д������  
    /* ��dir��Ϣ��¼��FLASH�� */  
   state =  SPI_FLASH_BufferWrite((uint8_t*)&dir,RESOURCE_BASE_ADDR + sizeof(dir)*i,sizeof(dir));
    if(state != 1)
    {
      BURN_ERROR("Burn_Catalog QSPI_Write ERROR");
      BURN_ERROR("loop=%d",loop);

    }
  }
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
  
  FRESULT result;   
  UINT  bw;            					    /* File R/W count */
  uint32_t write_addr=0;
  uint8_t tempbuf[256];
 
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
    LED_BLUE;
     
     result = f_open(&file_temp,full_file_name,FA_OPEN_EXISTING | FA_READ);
      if(result != FR_OK)
      {
          BURN_ERROR("���ļ�ʧ�ܣ�");
          LED_RED;
          return result;
      }
  
      BURN_INFO("������FLASHд������...");
      
      write_addr = dir.offset + RESOURCE_BASE_ADDR;
      loop = 0;
      while(result == FR_OK) 
      {
        loop++;
        result = f_read( &file_temp, tempbuf, 256, &bw);//��ȡ����	 
        if(result!=FR_OK)			 //ִ�д���
        {
          BURN_ERROR("��ȡ�ļ�ʧ�ܣ�result = %d",result);
          LED_RED;
          return result;
        }    
        if(bw == 0)break;//Ϊ0ʱ�����ж�д������  

        SPI_FLASH_BufferWrite(tempbuf,write_addr,bw);  //�������ݵ��ⲿflash��   
//        if(state != 1)
//        {
//          BURN_ERROR("Burn_Content QSPI_Write ERROR");
//          BURN_ERROR("loop=%d bw =%d",loop,bw);

//        } 
        write_addr+=bw;				
        if(bw !=256)break;
      }
      BURN_INFO("����д�����");          
           
    f_close(&file_temp);     
  }
  
  BURN_INFO("************************************");
  BURN_INFO("�����ļ�������¼��ϣ������ļ�ϵͳ���֣�");
  
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
 
  /* ����Ŀ¼�ļ� */
  for(i=0;1;i++)
  {
    is_end = Read_CatalogInfo(i, 
                                &dir,
                                full_file_name);  
    /* �ѱ������,����ѭ�� */
    if(is_end !=0)   
      break;    
    
    /* ��FLASH��Ŀ¼����Ҷ�Ӧ��offset */
    offset = GetResOffset(dir.name);
    if(offset == -1)
    {
      LED_RED;
      BURN_INFO("У��ʧ�ܣ��޷���FLASH���ҵ��ļ���%s ��Ŀ¼",dir.name);
      return FR_NO_FILE;
    }
    else
      dir.offset = offset + RESOURCE_BASE_ADDR;
       
    BURN_INFO("-------------------------------------"); 
    BURN_INFO("׼��У�����ݣ�%s",full_file_name);
    BURN_INFO("��������%s ",dir.name);
    BURN_INFO("���ݾ��Ե�ַ��%d ",dir.offset);
    BURN_INFO("���ݴ�С��%d ",dir.size);
    
      result = f_open(&file_temp,full_file_name,FA_OPEN_EXISTING | FA_READ);
      if(result != FR_OK)
      {
          BURN_ERROR("���ļ�ʧ�ܣ�");
              LED_RED;
          return result;
      }
          
       //У������
      read_addr = dir.offset;
     
      f_lseek(&file_temp,0);
      loop = 0;
      while(result == FR_OK) 
      {
        loop++;
        result = f_read( &file_temp, tempbuf, 256, &bw);//��ȡ����	 
        if(result!=FR_OK)			 //ִ�д���
        {
          BURN_ERROR("��ȡ�ļ�ʧ�ܣ�");
          LED_RED;
          return result;
        }    

        if(bw == 0)break;//Ϊ0ʱ�����ж�д������
  
        SPI_FLASH_BufferRead(flash_buf,read_addr,bw);  //��FLASH�ж�ȡ����
//        if(state != QSPI_OK)
//        {
//          BURN_ERROR("Check_Resource BSP_QSPI_Read ERROR");
//          BURN_ERROR("loop=%d bw =%d",loop,bw);


//        }
        read_addr+=bw;		
        
        for(j=0;j<bw;j++)
        {
          if(tempbuf[j] != flash_buf[j])
          {
            BURN_ERROR("����У��ʧ�ܣ�");
            BURN_ERROR("j=%d ",j);
            BURN_ERROR("tempbuf");
            BURN_DEBUG_ARRAY(tempbuf,bw);
            BURN_ERROR("flash_buf");
            BURN_DEBUG_ARRAY(flash_buf,bw);
            LED_RED;
            return FR_INT_ERR;
          }
         }  
     
        if(bw !=256)break;//�����ļ�β
      }      

      BURN_INFO("����У��������");
      LED_BLUE;
               
      f_close(&file_temp);     
  }
  
  LED_GREEN;
  BURN_INFO("************************************");
  BURN_INFO("�����ļ�У�������������ļ�ϵͳ���֣�");
  return FR_OK;
}

/*********************************************END OF FILE**********************/

