/**
  ******************************************************************************
  * @file    bsp_hc05.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   HC05����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� �Ե� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "./hc05/bsp_hc05.h"
#include "./usart/bsp_usart_blt.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include <string.h>
#include <stdio.h>



BLTDev bltDevList; //�����豸�б����ڱ��������豸����


/**
  * @brief  ��ʼ������LED��IO
  * @param  ��
  * @retval ��
  */
static void HC05_GPIO_Config(void)
{		
		
		GPIO_InitTypeDef GPIO_InitStructure;

		/*����GPIOʱ��*/
		RCC_APB2PeriphClockCmd( BLT_INT_GPIO_CLK|BLT_KEY_GPIO_CLK, ENABLE); 

		GPIO_InitStructure.GPIO_Pin = BLT_INT_GPIO_PIN;	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(BLT_INT_GPIO_PORT, &GPIO_InitStructure);	
	
		
		GPIO_InitStructure.GPIO_Pin = BLT_KEY_GPIO_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(BLT_KEY_GPIO_PORT, &GPIO_InitStructure);	

}


/**
  * @brief  ��ʼ��GPIO�����HC05ģ��
  * @param  ��
  * @retval HC05״̬��0 ��������0�쳣
  */
uint8_t HC05_Init(void)
{
	uint8_t i;
	
	HC05_GPIO_Config();

	BLT_USART_Config();
	
	for(i=0;i<BLTDEV_MAX_NUM;i++)
	{
		sprintf(bltDevList.unpraseAddr[i]," ");
		sprintf(bltDevList.name[i]," ");

	}	
		bltDevList.num = 0;

	return HC05_Send_CMD("AT\r\n",1);
}

/**
  * @brief  ��HC05д����������ģ�����Ӧ
  * @param  command ��Ҫ���͵�����
  * @param  arg�����������Ϊ0ʱ������������commandҲΪ0ʱ������"AT"����
  * @retval ��
  */
void writeCommand(const char *command, const char *arg)
{
		char str_buf[50];

		BLT_KEY_HIGHT;
		hc05_delay_ms(10);

		if (arg && arg[0] != 0)
			sprintf(str_buf,"AT+%s%s\r\n",command,arg);
		else if (command && command[0] != 0)
		{
			sprintf(str_buf,"AT+%s\r\n",command);
		}
		else
			sprintf(str_buf,"AT\r\n");

		HC05_DEBUG("CMD send:%s",str_buf);
		
		Usart_SendString(HC05_USART,(uint8_t *)str_buf);

}
	
/**
  * @brief  ��HC05ģ�鷢��������OK��ֻ�����ھ���OKӦ��������ȴ�5sֱ���յ�OK
  * @param  cmd:����������ַ�������Ҫ��\r\n��
  * @param	clean ����������Ƿ�������ջ�������1 �����0 �����
  * @template  ��λ���	HC05_Send_CMD("AT+RESET\r\n",1);	
  * @retval 0,���óɹ�;����,����ʧ��.	
  */
uint8_t HC05_Send_CMD(char* cmd,uint8_t clean)
{	 		 
  uint8_t retry=3;
	uint32_t i;
	uint16_t len;
  char * redata;
  
	while(retry--)
	{
		BLT_KEY_HIGHT;
		Usart_SendString(HC05_USART,(uint8_t *)cmd);
    i=500;              //��ʼ��i����ȴ�5��
    hc05_delay_ms(10);  //
		
		do
    {
      redata = get_rebuff(&len); 
      if(len>0)
      {
        if(strstr(redata,"OK"))				
        {
          HC05_DEBUG("send CMD: %s",cmd); //��ӡ���͵�����ָ��ͷ�����Ϣ

          HC05_DEBUG("recv back: %s",redata);
          
          if(clean==1)
            clean_rebuff();
//          BLT_KEY_LOW;
          
          return 0; //ATָ��ɹ�
        }
      }
      
			hc05_delay_ms(10);
      
    }while( --i ); //�����ȴ�
    
    HC05_DEBUG("send CMD: %s",cmd); //��ӡ���͵�����ָ��ͷ�����Ϣ
    HC05_DEBUG("recv back: %s",redata);
    HC05_DEBUG("HC05 send CMD fail %d times", retry); //��ʾʧ������
    
  }
  
//	BLT_KEY_LOW;
	HC05_DEBUG("HC05 send CMD fail ");
		
	if(clean==1)
		clean_rebuff();

	return 1; //ATָ��ʧ��
}


/**
  * @brief  ��HC05ģ�鷢��������OK��ֻ�����ھ���OKӦ�������ȴ��̶�ʱ��
  * @param  cmd:����������ַ�������Ҫ��\r\n��
  * @param	clean ����������Ƿ�������ջ�������1 �����0 �����
  * @param  delayms �ȴ�ʱ�䣬��λms
  * @template  ��ѯ���	HC05_Send_CMD_Wait("AT+INQ\r\n",0, 5000);	
  * @retval 0,���óɹ�;����,����ʧ��.	
  */
uint8_t HC05_Send_CMD_Wait(char* cmd, uint8_t clean, uint32_t delayms) //Debug
{
	uint16_t len;
  char * redata;
  
  BLT_KEY_HIGHT;
  Usart_SendString(HC05_USART,(uint8_t *)cmd);
  
  hc05_delay_ms(delayms); //�̶���ʱ
  
  redata = get_rebuff(&len); 
  if(len>0)
  {
    redata[len] = '\0';
    if(redata[0]!=0)
    {
      HC05_DEBUG("send CMD: %s",cmd);

      HC05_DEBUG("receive %s",redata);
    }
    if(strstr(redata,"OK"))				
    {
      
      if(clean==1)
        clean_rebuff();
      BLT_KEY_LOW;
      return 0; //ATָ��ɹ�
    }
  }
  
	BLT_KEY_LOW;
	HC05_DEBUG("HC05 send CMD fail ");

	if(clean==1)
		clean_rebuff();

	return 1; //ATָ��ʧ��
}


/**
  * @brief  ʹ��HC05͸���ַ�������
  * @param  str,Ҫ������ַ���
  * @retval ��
  */
void HC05_SendString(char* str)
{
		BLT_KEY_LOW;
	
		Usart_SendString(HC05_USART,(uint8_t *)str);

}


/**
  * @brief  ��str�У�������ǰ���prefix�ַ���,
            ��strΪ"abcdefg",prefixΪ"abc"������ñ������󷵻�ָ��"defg"��ָ��
  * @param  str,ԭ�ַ���
  * @param  str_length���ַ�������
  * @param 	prefix��Ҫ�������ַ���
  * @retval ����prefix����ַ���ָ��
  */
char *skipPrefix(char *str, size_t str_length, const char *prefix)
{

  uint16_t prefix_length = strlen(prefix);

  if (!str || str_length == 0 || !prefix)
    return 0;

  if (str_length >= prefix_length && strncmp(str, prefix, prefix_length) == 0)
    return str + prefix_length;

  return 0;
}

/**
  * @brief  ��stream�л�ȡһ���ַ�����line��
  * @param  line,�洢����е��ַ�������
  * @param  stream��ԭ�ַ���������
  * @param 	max_size��stream�Ĵ�С 
  * @retval line�ĳ��ȣ���stream��û�С�\0����'\r'��'\n'���򷵻�0
  */
int get_line(char* line, char* stream ,int max_size)  
{  
		char *p;	
    int len = 0;  
		p=stream;
    while( *p != '\0' && len < max_size ){  
        line[len++] = *p;  
				p++;
        if('\n' == *p || '\r'==*p)  
            break;  
    }
	
		if(*p != '\0' && *p != '\n' && *p != '\r')
			return 0;
  
      
    line[len] = '\0';  
    return len;  
} 

/**
  * @brief  �滻 rawstr �ַ����е����� ':'�ַ�
  * @param  rawstr,Դ�ַ���
  * @param  delimiter��Ҫ�滻�ɵ��ַ�
  */
void rawReplaceChar(char *rawstr, char delimiter)
{
  do
  {
    rawstr = strchr(rawstr, ':');
    if(rawstr) *rawstr = delimiter;
  }while(rawstr != NULL);
  
}


/**
  * @brief  ɨ���ܱߵ������豸�����洢���豸�б��С�
  * @retval �Ƿ�ɨ�赽�豸��0��ʾɨ�赽����0��ʾû��ɨ�赽
  */
uint8_t parseBltAddr(void)
{
  char *redata;
  uint16_t len;
  
  //��������豸�б�
  bltDevList.num = 0;
  
  HC05_INFO("���ڲ�ѯ�豸�б�...");
  HC05_Send_CMD("AT+INQ\r\n",0);
//  HC05_Send_CMD_Wait("AT+INQ\r\n",0, 5000);  //�̶���ʱ 5s
  redata = get_rebuff(&len);
  
  if(redata[0] != 0 && strstr(redata, "+INQ:") != 0 && strstr(redata, "OK") != 0)
  {
    char *pstr = strstr(redata, "+INQ:"); //��ʼ��pstr
    char *ptmp;
    int num, i;
    
    for(num=0; num<BLTDEV_MAX_NUM; ) //������������ɨ�赽�������豸����
    {
      pstr = strstr(pstr, "+INQ:");
      if(pstr != NULL)
      {
        pstr += 5;
        ptmp = strchr(pstr, ','); //������ַ��ĩβ
        *ptmp = '\0';             //�ַ���������־���滻','
        HC05_DEBUG("���ڽ���Str=%s", pstr);
        
        num ++;
        HC05_DEBUG("����õ�ַ��Ч,num = %d ", num);
        if(num == 1)
        {
          strcpy(bltDevList.unpraseAddr[num-1], pstr); //���Ƶ�һ��������ַ�ַ���
          HC05_DEBUG("����Ч��ַ,num=%d, Addr: %s\r\n", num, bltDevList.unpraseAddr[num-1]);
        }
        for(i=0; i<num-1; i++) //�Ƚ�ǰ��ĵ�ַ�Ƿ��ظ�
        {
          if( strcmp(pstr, bltDevList.unpraseAddr[i]) == 0 )	
          {
            num --;            //��������ַ�ַ������ʱ����ʾ�õ�ַ��������ĵ�ַ��ͬ���������ظ���ַ
            HC05_DEBUG("���ظ���ַ,num = %d\r\n", num);
            break;
          }
          
          if(i == num-2)
          {
            strcpy(bltDevList.unpraseAddr[num-1], pstr); //����������ַ�ַ���
            
            HC05_DEBUG("����Ч��ַ,num=%d, Addr: %s\r\n", num, bltDevList.unpraseAddr[num-1]);
          }
        }
        
        pstr = ptmp + 1; // pstr ����'\0'�ַ�
      }
      else
      {
        break; //û�м�⵽"+INQ:"˵��ɨ������˳�
      }
      
    }
    
    bltDevList.num = num; //����ʵ��������ַ��Ŀ
    
    //��ӡ����������ַ
    HC05_INFO("Addr List (%d):", bltDevList.num);
    for(i=0; i< num; i++)
    {
      HC05_INFO("Addr %d: %s", i, bltDevList.unpraseAddr[i]);
    }
    HC05_INFO("ɨ����ϣ�\r\n");
    
    clean_rebuff();
		HC05_Send_CMD("AT+INQC\r\n",1);//�˳�ǰ�жϲ�ѯ
    
    return 0;
  }
  
  HC05_INFO("û���ҵ������豸��\r\n");
  
  clean_rebuff();
  HC05_Send_CMD("AT+INQC\r\n",1);//�˳�ǰ�жϲ�ѯ
  
  return 1;
}

 /**
  * @brief  ��ȡԶ�������豸������
	* @param  bltDev �������豸�б�ָ��
  * @retval 0��ȡ�ɹ�����0���ɹ�
  */
uint8_t getRemoteDeviceName(void)
{
	uint8_t i;
	char *redata;
	uint16_t len;
	
	char linebuff[50];
	uint16_t linelen;
	char *p;
	
	char cmdbuff[100];
	

	HC05_DEBUG("device num =%d", bltDevList.num);
	
	for(i=0;i<bltDevList.num;i++)
	{
    rawReplaceChar(bltDevList.unpraseAddr[i], ','); //��ַ��ʽ�滻':'Ϊ','
		sprintf(cmdbuff,"AT+RNAME?%s\r\n",bltDevList.unpraseAddr[i]);
		HC05_Send_CMD(cmdbuff,0);     //���ͻ�ȡ������������
		
		redata =get_rebuff(&len);
		if(redata[0] != 0 && strstr(redata, "OK") != 0)
		{
			
			linelen = get_line(linebuff,redata,len);
			if(linelen>50 && linelen !=0 ) linebuff[linelen]='\0';	//�����ض�
					
			p = skipPrefix(linebuff,linelen,"+RNAME:");
			if(p!=0)
			{
				strcpy(bltDevList.name[i],p);
			}

		}
		else
		{
			strcpy(bltDevList.name[i], "<���ֻ�ȡʧ��>");
      HC05_INFO("�������ֻ�ȡʧ�ܣ�\r\n");
		}
		
		clean_rebuff(); //��ջ���
	
	}
	
	return 0;

}

 /**
  * @brief  ��������豸�б�
	* @param  bltDev �������豸�б�ָ��
  * @retval ��
  */
void printBLTInfo(void)  
{
	uint8_t i;

  #ifdef ENABLE_LCD_DISPLAY
	char disp_buff[100];
  
  sprintf(disp_buff," %d device found.",bltDevList.num);
  LCD_SetColors(RED,BLACK);
  ILI9341_Clear(0,120,240,200);
  ILI9341_DispString_EN( 50, 120,disp_buff );
  #endif
	
	if(bltDevList.num==0)
	{
		HC05_INFO("/*******No remote BLT Device or in SLAVE mode********/");
	}
	else
	{
		HC05_INFO("ɨ�赽 %d �������豸",bltDevList.num);
		

		for(i=0;i<bltDevList.num;i++)
		{
			HC05_INFO("/*******Device[%d]********/",i);	
			HC05_INFO("Device Addr: %s",bltDevList.unpraseAddr[i]);
			HC05_INFO("Device name: %s",bltDevList.name[i]);
			
      #ifdef ENABLE_LCD_DISPLAY
			LCD_SetColors(YELLOW,BLACK);
			sprintf(disp_buff," /*******Device[%d]********/",i);
			ILI9341_DispString_EN( 5, 140+i*60,disp_buff );
			
			sprintf(disp_buff,"Device Addr: %s",bltDevList.unpraseAddr[i]);
			ILI9341_DispString_EN( 5, 160+i*60,disp_buff );
			
			sprintf(disp_buff,"Device name: %s",bltDevList.name[i]);
			ILI9341_DispString_EN( 5, 180+i*60,disp_buff );
      #endif
		}
	}

}



 /**
  * @brief  ɨ�������豸�������������к���"HC05"���豸
	* @param  ��
  * @retval 0��ȡ�ɹ�����0���ɹ�
  */
uint8_t linkHC05(void)
{
	uint8_t i=0;
	char cmdbuff[100];
	
  parseBltAddr();         //��ѯ�����豸��������ַ
	getRemoteDeviceName();  //��ȡ�����豸����
	printBLTInfo();
	
	for(i=0;i<=bltDevList.num;i++)
	{
		if(strstr(bltDevList.name[i],"HC05") != NULL) //��NULL��ʾ�ҵ������Ʋ���ΪHC05���豸
		{
			HC05_INFO("������Զ��HC05ģ�飬���������������...");
			
      #ifdef ENABLE_LCD_DISPLAY
			LCD_SetColors(YELLOW,BLACK);
			ILI9341_Clear(0,80,240,20);
			ILI9341_DispString_EN( 5, 80,"Found a HC05 ,conecting..." );
      #endif
			
			//���
			sprintf(cmdbuff,"AT+PAIR=%s,20\r\n",bltDevList.unpraseAddr[i]);
			HC05_Send_CMD(cmdbuff,0);
				
			//����	
			sprintf(cmdbuff,"AT+LINK=%s\r\n",bltDevList.unpraseAddr[i]);
						
			return HC05_Send_CMD(cmdbuff,0);		
		}
	}
  
  HC05_INFO("δ�����������к���HC05���豸...");
	
	return 1;

}








