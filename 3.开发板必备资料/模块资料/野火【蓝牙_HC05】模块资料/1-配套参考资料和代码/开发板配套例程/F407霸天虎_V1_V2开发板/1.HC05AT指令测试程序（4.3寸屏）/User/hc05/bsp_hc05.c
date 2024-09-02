/**
  ******************************************************************************
  * @file    bsp_hc05.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   HC05驱动
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F407 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "./hc05/bsp_hc05.h"
#include "./usart/bsp_blt_usart.h"
#include <string.h>
#include <stdio.h>
#include "./lcd/bsp_nt35510_lcd.h"



BLTDev bltDevList; //蓝牙设备列表，用于保存蓝牙设备数据


/**
  * @brief  初始化控制LED的IO
  * @param  无
  * @retval 无
  */
static void HC05_GPIO_Config(void)
{		
		
		GPIO_InitTypeDef GPIO_InitStructure;

		/*开启GPIO时钟*/
		RCC_AHB1PeriphClockCmd( BLT_INT_GPIO_CLK|BLT_KEY_GPIO_CLK, ENABLE); 

		GPIO_InitStructure.GPIO_Pin = BLT_INT_GPIO_PIN;	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(BLT_INT_GPIO_PORT, &GPIO_InitStructure);	
	
		
		GPIO_InitStructure.GPIO_Pin = BLT_KEY_GPIO_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(BLT_KEY_GPIO_PORT, &GPIO_InitStructure);	

}


/**
  * @brief  初始化GPIO及检测HC05模块
  * @param  无
  * @retval HC05状态，0 正常，非0异常
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
  * @brief  向HC05写入命令，不检查模块的响应
  * @param  command ，要发送的命令
  * @param  arg，命令参数，为0时不带参数，若command也为0时，发送"AT"命令
  * @retval 无
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
  * @brief  向HC05模块发送命令并检查OK。只适用于具有OK应答的命令，最长等待5s直到收到OK
  * @param  cmd:命令的完整字符串，需要加\r\n。
  * @param	clean 命令结束后是否清除接收缓冲区，1 清除，0 不清除
  * @template  复位命令：	HC05_Send_CMD("AT+RESET\r\n",1);	
  * @retval 0,设置成功;其他,设置失败.	
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
    i=500;              //初始化i，最长等待5秒
    hc05_delay_ms(10);  //
		
		do
    {
      redata = get_rebuff(&len); 
      if(len>0)
      {
        if(strstr(redata,"OK"))				
        {
          HC05_DEBUG("send CMD: %s",cmd); //打印发送的蓝牙指令和返回信息

          HC05_DEBUG("recv back: %s",redata);
          
          if(clean==1)
            clean_rebuff();
//          BLT_KEY_LOW;
          
          return 0; //AT指令成功
        }
      }
      
			hc05_delay_ms(10);
      
    }while( --i ); //继续等待
    
    HC05_DEBUG("send CMD: %s",cmd); //打印发送的蓝牙指令和返回信息
    HC05_DEBUG("recv back: %s",redata);
    HC05_DEBUG("HC05 send CMD fail %d times", retry); //提示失败重试
    
  }
  
//	BLT_KEY_LOW;
	HC05_DEBUG("HC05 send CMD fail ");
		
	if(clean==1)
		clean_rebuff();

	return 1; //AT指令失败
}


/**
  * @brief  向HC05模块发送命令并检查OK。只适用于具有OK应答的命令，等待固定时间
  * @param  cmd:命令的完整字符串，需要加\r\n。
  * @param	clean 命令结束后是否清除接收缓冲区，1 清除，0 不清除
  * @param  delayms 等待时间，单位ms
  * @template  查询命令：	HC05_Send_CMD_Wait("AT+INQ\r\n",0, 5000);	
  * @retval 0,设置成功;其他,设置失败.	
  */
uint8_t HC05_Send_CMD_Wait(char* cmd, uint8_t clean, uint32_t delayms) //Debug
{
	uint16_t len;
  char * redata;
  
  BLT_KEY_HIGHT;
  Usart_SendString(HC05_USART,(uint8_t *)cmd);
  
  hc05_delay_ms(delayms); //固定延时
  
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
      return 0; //AT指令成功
    }
  }
  
	BLT_KEY_LOW;
	HC05_DEBUG("HC05 send CMD fail ");

	if(clean==1)
		clean_rebuff();

	return 1; //AT指令失败
}


/**
  * @brief  使用HC05透传字符串数据
  * @param  str,要传输的字符串
  * @retval 无
  */
void HC05_SendString(char* str)
{
		BLT_KEY_LOW;
	
		Usart_SendString(HC05_USART,(uint8_t *)str);

}


/**
  * @brief  在str中，跳过它前面的prefix字符串,
            如str为"abcdefg",prefix为"abc"，则调用本函数后返回指向"defg"的指针
  * @param  str,原字符串
  * @param  str_length，字符串长度
  * @param 	prefix，要跳过的字符串
  * @retval 跳过prefix后的字符串指针
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
  * @brief  从stream中获取一行字符串到line中
  * @param  line,存储获得行的字符串数组
  * @param  stream，原字符串数据流
  * @param 	max_size，stream的大小 
  * @retval line的长度，若stream中没有‘\0’，'\r'，'\n'，则返回0
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
  * @brief  替换 rawstr 字符串中的所有 ':'字符
  * @param  rawstr,源字符串
  * @param  delimiter，要替换成的字符
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
  * @brief  扫描周边的蓝牙设备，并存储到设备列表中。
  * @retval 是否扫描到设备，0表示扫描到，非0表示没有扫描到
  */
uint8_t parseBltAddr(void)
{
  char *redata;
  uint16_t len;
  
  //清空蓝牙设备列表
  bltDevList.num = 0;
  
  
  HC05_INFO("正在查询设备列表...");
  HC05_Send_CMD("AT+INQ\r\n",0);
//  HC05_Send_CMD_Wait("AT+INQ\r\n",0, 5000);  //固定延时 5s
  redata = get_rebuff(&len);
  
  if(redata[0] != 0 && strstr(redata, "+INQ:") != 0 && strstr(redata, "OK") != 0)
  {
    char *pstr = strstr(redata, "+INQ:"); //初始化pstr
    char *ptmp;
    int num, i;
    
    for(num=0; num<BLTDEV_MAX_NUM; ) //这里限制允许扫描到的蓝牙设备数量
    {
      pstr = strstr(pstr, "+INQ:");
      if(pstr != NULL)
      {
        pstr += 5;
        ptmp = strchr(pstr, ','); //蓝牙地址的末尾
        *ptmp = '\0';             //字符串结束标志，替换','
        HC05_DEBUG("正在解析Str=%s", pstr);
        
        num ++;
        HC05_DEBUG("假设该地址有效,num = %d ", num);
        if(num == 1)
        {
          strcpy(bltDevList.unpraseAddr[num-1], pstr); //复制第一个蓝牙地址字符串
          HC05_DEBUG("是有效地址,num=%d, Addr: %s\r\n", num, bltDevList.unpraseAddr[num-1]);
        }
        for(i=0; i<num-1; i++) //比较前面的地址是否重复
        {
          if( strcmp(pstr, bltDevList.unpraseAddr[i]) == 0 )	
          {
            num --;            //这两个地址字符串相等时，表示该地址与解码语句的地址相同，丢弃该重复地址
            HC05_DEBUG("是重复地址,num = %d\r\n", num);
            break;
          }
          
          if(i == num-2)
          {
            strcpy(bltDevList.unpraseAddr[num-1], pstr); //复制蓝牙地址字符串
            
            HC05_DEBUG("是有效地址,num=%d, Addr: %s\r\n", num, bltDevList.unpraseAddr[num-1]);
          }
        }
        
        pstr = ptmp + 1; // pstr 跳过'\0'字符
      }
      else
      {
        break; //没有检测到"+INQ:"说明扫描完毕退出
      }
      
    }
    
    bltDevList.num = num; //保存实际蓝牙地址数目
    
    //打印所有蓝牙地址
    HC05_INFO("Addr List (%d):", bltDevList.num);
    for(i=0; i< num; i++)
    {
      HC05_INFO("Addr %d: %s", i, bltDevList.unpraseAddr[i]);
    }
    HC05_INFO("扫描完毕！\r\n");
    
    clean_rebuff();
		HC05_Send_CMD("AT+INQC\r\n",1);//退出前中断查询
    
    return 0;
  }
  
  HC05_INFO("没有找到蓝牙设备！\r\n");
  
  clean_rebuff();
  HC05_Send_CMD("AT+INQC\r\n",1);//退出前中断查询
  
  return 1;
}

 /**
  * @brief  获取远程蓝牙设备的名称
	* @param  bltDev ，蓝牙设备列表指针
  * @retval 0获取成功，非0不成功
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
    rawReplaceChar(bltDevList.unpraseAddr[i], ','); //地址格式替换':'为','
		sprintf(cmdbuff,"AT+RNAME?%s\r\n",bltDevList.unpraseAddr[i]);
		HC05_Send_CMD(cmdbuff,0);     //发送获取蓝牙名字命令
		
		redata =get_rebuff(&len);
		if(redata[0] != 0 && strstr(redata, "OK") != 0)
		{
			
			linelen = get_line(linebuff,redata,len);
			if(linelen>50 && linelen !=0 ) linebuff[linelen]='\0';	//超长截断
					
			p = skipPrefix(linebuff,linelen,"+RNAME:");
			if(p!=0)
			{
				strcpy(bltDevList.name[i],p);
			}

		}
		else
		{
			strcpy(bltDevList.name[i], "<Get Name Fail>");
      HC05_INFO("蓝牙名字获取失败！\r\n");
		}
		
		clean_rebuff(); //清空缓存
	
	}
	
	return 0;

}

 /**
  * @brief  输出蓝牙设备列表
	* @param  bltDev ，蓝牙设备列表指针
  * @retval 无
  */
void printBLTInfo(void)  
{
	uint8_t i;

  #ifdef ENABLE_LCD_DISPLAY
	char disp_buff[100];

  sprintf(disp_buff," %d device found.",bltDevList.num);
  LCD_SetColors(RED,BLACK);
  NT35510_Clear(0,160,480,640);
  NT35510_DispString_EN( 50, 160,disp_buff );
  #endif
  
	if(bltDevList.num==0)
	{
		HC05_INFO("/*******No remote BLT Device or in SLAVE mode********/");
	}
	else
	{
		HC05_INFO("扫描到 %d 个蓝牙设备",bltDevList.num);

		for(i=0;i<bltDevList.num;i++)
		{
			HC05_INFO("/*******Device[%d]********/",i);	
			HC05_INFO("Device Addr: %s",bltDevList.unpraseAddr[i]);
			HC05_INFO("Device name: %s",bltDevList.name[i]);
			
      #ifdef ENABLE_LCD_DISPLAY
			LCD_SetColors(YELLOW,BLACK);
			sprintf(disp_buff," /*******Device[%d]********/",i);
			NT35510_DispString_EN( 5, 200+i*100,disp_buff );
			
			sprintf(disp_buff,"Addr: %s",bltDevList.unpraseAddr[i]);
			NT35510_DispString_EN( 5, 230+i*100,disp_buff );
			
			sprintf(disp_buff,"Name: %s",bltDevList.name[i]);
			NT35510_DispString_EN( 5, 260+i*100,disp_buff );
      #endif
		}
	}

}



 /**
  * @brief  扫描蓝牙设备，并连接名称中含有"HC05"的设备
	* @param  无
  * @retval 0获取成功，非0不成功
  */
uint8_t linkHC05(void)
{
	uint8_t i=0;
	char cmdbuff[100];
	
  parseBltAddr();         //查询蓝牙设备并解析地址
	getRemoteDeviceName();  //获取蓝牙设备名字
	printBLTInfo();
	
	for(i=0;i<=bltDevList.num;i++)
	{
		if(strstr(bltDevList.name[i],"HC05") != NULL) //非NULL表示找到有名称部分为HC05的设备
		{
			HC05_INFO("搜索到远程HC05模块，即将进行配对连接...");
			
      #ifdef ENABLE_LCD_DISPLAY
			LCD_SetColors(YELLOW,BLACK);
			NT35510_Clear(0,80,240,20);
			NT35510_DispString_EN( 5, 80,"Found a HC05 ,conecting..." );
			#endif
      
			//配对
			sprintf(cmdbuff,"AT+PAIR=%s,20\r\n",bltDevList.unpraseAddr[i]);
			HC05_Send_CMD(cmdbuff,0);
				
			//连接	
			sprintf(cmdbuff,"AT+LINK=%s\r\n",bltDevList.unpraseAddr[i]);
						
			return HC05_Send_CMD(cmdbuff,0);		
		}
	}
  
  HC05_INFO("未搜索到名称中含有HC05的设备...");
	
	return 1;

}








