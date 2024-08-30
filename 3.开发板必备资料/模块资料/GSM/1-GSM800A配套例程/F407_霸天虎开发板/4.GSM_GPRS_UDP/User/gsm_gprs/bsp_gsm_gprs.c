/**
  ******************************************************************************
  * @file    bsp_gsm_gprs.c
  * @author  fire
  * @version V1.0
  * @date    2014-08-xx
  * @brief   GSMģ������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F407 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
	*/

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "./usart/bsp_debug_usart.h"
#include "./gsm_gprs/bsp_gsm_usart.h"
#include "./gsm_gprs/bsp_gsm_gprs.h"
#include "ff.h"

static uint8_t MaxMessAdd=50;


//0��ʾ�ɹ���1��ʾʧ��

uint8_t gsm_cmd(char *cmd, char *reply,uint32_t waittime )
{    
		GSM_DEBUG_FUNC();
	
    GSM_CLEAN_RX();                 //����˽��ջ���������

    GSM_TX(cmd);                    //��������

	  GSM_DEBUG("Send cmd:%s",cmd);	
	
    if(reply == 0)                      //����Ҫ��������
    {
        return GSM_TRUE;
    }
    
    GSM_DELAY(waittime);                 //��ʱ
    
    
    return gsm_cmd_check(reply);    //�Խ������ݽ��д���
}


//0��ʾ�ɹ���1��ʾʧ��
uint8_t gsm_cmd_check(char *reply)
{
    uint8_t len;
    uint8_t n;
    uint8_t off;
    char *redata;
    
		GSM_DEBUG_FUNC();

    redata = GSM_RX(len);   //��������
   	  
	  *(redata+len) = '\0';
	  GSM_DEBUG("Reply:%s",redata);	

//		GSM_DEBUG_ARRAY((uint8_t *)redata,len);
	
    n = 0;
    off = 0;
    while((n + off)<len)
    {
        if(reply[n] == 0)                 //����Ϊ�ջ��߱Ƚ����
        {
            return GSM_TRUE;
        }
        
        if(redata[ n + off]== reply[n])
        {
            n++;                //�ƶ�����һ����������
        }
        else
        {
            off++;              //������һ��ƥ��
            n=0;                //����
        }
        //n++;
    }

    if(reply[n]==0)   //�պ�ƥ�����
    {
        return GSM_TRUE;
    }
    
    return GSM_FALSE;       //����ѭ����ʾ�Ƚ���Ϻ�û����ͬ�����ݣ��������
}

char * gsm_waitask(uint8_t waitask_hook(void))      //�ȴ�������Ӧ��
{
    uint8_t len=0;
    char *redata;
	
		GSM_DEBUG_FUNC();
	
    do{
        redata = GSM_RX(len);   //��������
			
			
        if(waitask_hook!=0)
        {
            if(waitask_hook()==GSM_TRUE)           //���� GSM_TRUE ��ʾ��⵽�¼�����Ҫ�˳�
            {
                redata = 0;
                return redata;               
            }
        }
    }while(len==0);                 //��������Ϊ0ʱһֱ�ȴ�
    				
		GSM_DEBUG_ARRAY((uint8_t *)redata,len);

    
    GSM_DELAY(20);              //��ʱ��ȷ���ܽ��յ�ȫ�����ݣ�115200�������£�ÿms�ܽ���11.52���ֽڣ�
    return redata;
}


//��������
//0��ʾ�ɹ���1��ʾʧ��
uint8_t gsm_cnum(char *num)
{
    char *redata;
    uint8_t len;
  
		GSM_DEBUG_FUNC();  
	
    if(gsm_cmd("AT+CNUM\r","OK", 100) != GSM_TRUE)
    {
        return GSM_FALSE;
    }
    
    redata = GSM_RX(len);   //��������
    
		GSM_DEBUG_ARRAY((uint8_t *)redata,len);
		
    if(len == 0)
    {
        return GSM_FALSE;
    }
    
    //��һ�����ź��������Ϊ:"��������"
    while(*redata != ',')
    {
        len--;
        if(len==0)
        {
            return GSM_FALSE;
        }
        redata++;
    }
    redata+=2;
    
    while(*redata != '"')
    {
        *num++ = *redata++;
    }
    *num = 0;               //�ַ�����β��Ҫ��0
    return GSM_TRUE;
}


//��ʼ�������ģ��
//0��ʾ�ɹ���1��ʾʧ��
uint8_t gsm_init(void)
{
	char *redata;
	uint8_t len;
	
	GSM_DEBUG_FUNC();  

	GSM_CLEAN_RX();                 //����˽��ջ���������
	GSM_USART_Config();					//��ʼ������
	
   
	if(gsm_cmd("AT+CGMM\r","OK", 100) != GSM_TRUE)
	{
			return GSM_FALSE;
	}
	
	redata = GSM_RX(len);   //��������

	if(len == 0)
	{
			return GSM_FALSE;
	}
	//���������GSM900A��GSM800A��GSM800C
	if (strstr(redata,"SIMCOM_GSM900A") != 0)
	{
		return GSM_TRUE;
	}
	else if(strstr(redata,"SIMCOM_SIM800") != 0)
	{
		return GSM_TRUE;
	}
	else
		return GSM_FALSE;

}

/*---------------------------------------------------------------------*/

//���𲦴�绰�����ܽӲ���ͨ��
void gsm_call(char *num)
{
		GSM_DEBUG_FUNC();  

    GSM_CLEAN_RX();                 //����˽��ջ���������
    
		GSM_DEBUG("GSM printf:ATD%s;\r",num);

    GSM_USART_printf("ATD%s;\r",num);

    //������ǲ��������ݵ�
    //�����κ�Ӧ�𣬿�ͷ����"\r\n",��������Ч�������Ǵӵ�3���ֽڿ�ʼ
    //�Է��ҵ��绰��û��ͨ�������أ�BUSY
    //�Է������˵绰��+COLP: "555",129,"",0,"9648674F98DE"   OK
    //�Է������˵绰,Ȼ��ҵ���NO CARRIER
    //�Է���ʱû�ӵ绰��NO ANSWER
}

//����绰
//0��ʾ�ɹ���1��ʾʧ��
uint8_t IsRing(char *num)
{
    char *redata;
		char * redata_off;
    uint8_t len;
    		
	  GSM_DEBUG_FUNC();  

    if(gsm_cmd_check("RING"))
    {
        return GSM_FALSE;
    }

    
    redata = GSM_RX(len);   //��������
		

//��ʹ��CLIPʹ��������ʾ�Ļ�����Ӧֻ�У�
//		RING
//
		
//ʹ��CLIPʹ��������ʾ����ӦΪ��
		
//		RING
//
//+CLIP: "�绰����",161,"",0,"",0
		
		//ƫ������Ӧ���ַ�����ַ
		redata_off = strstr(redata,"RING");
		
		if(redata_off == NULL)
			return GSM_FALSE;
		
		//����ƫ�ƺ�ʣ��ĳ���
		len = len - (redata_off - redata); 
		
    if(len == 0)
    {
        return GSM_FALSE;
    }
    
    //��һ��ð�ź��������Ϊ:�����롰
    while(*redata != ':')
    {
        len--;
        if(len==0)
        {
            return GSM_FALSE;
        }
        redata++;
    }
    redata+=3;
    while(*redata != '"')
    {
        *num++ = *redata++;
    }
    *num = 0;               //�ַ�����β��Ҫ��0
	GSM_CLEAN_RX();
    return GSM_TRUE;
}



//����Ƿ��п�
//0��ʾ�ɹ���1��ʾʧ��
uint8_t IsInsertCard(void)
{
	GSM_DEBUG_FUNC();
	
	GSM_CLEAN_RX();
	return gsm_cmd("AT+CNUM\r","OK",200);
   
}


/*---------------------------------------------------------------------*/

/*---------------------------------------------------------------------*/
/**
 * @brief  IsASSIC �ж��Ƿ�ASSIC����
 * @param  str: �ַ���ָ��
 * @retval ��ASSIC���뷵��TRUE���Ǵ�ASSIC���뷵��FALSE
 */
uint8_t IsASSIC(char * str)
{
		GSM_DEBUG_FUNC();  

    while(*str)
    {
        if(*str>0x7F)
        {
            return GSM_FALSE;
        }
        str++;
    }
    
    return GSM_TRUE;
}


/**
 * @brief  gsm_gbk2ucs2 ��GBK����ת����UCS2����
 * @param  ucs2: ucs2�ַ���ָ�룬gbk��GBK�ַ���ָ��
 * @retval ��
 */
void gsm_gbk2ucs2(char * ucs2,char * gbk)
{
    WCHAR   tmp;
	
		GSM_DEBUG_FUNC();  

    
    while(*gbk)
    {
        if((*gbk&0xFF) < 0x7F)      //Ӣ��
        {
            
            *ucs2++ = 0;
            *ucs2++ = *gbk++;  
        }
        else                        //����
        {
            tmp = GSM_SWAP16(*(WCHAR *)gbk);
            
            *(WCHAR *)ucs2 = GSM_SWAP16(ff_convert(tmp,1));
            gbk+=2;
            ucs2+=2;
        }
    }
}

/**
 * @brief  gsm_char2hex ���ַ�ת����16�����ַ�
 * @param  hex: 16�����ַ��洢��λ��ָ�룬ch���ַ�
 * @retval ��
 */
void gsm_char2hex(char *hex,char ch)
{
    const char numhex[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
   
		GSM_DEBUG_FUNC();  

		*hex++  = numhex[(ch & 0xF0)>>4];
    *hex    = numhex[ ch & 0x0F];
}


/**
 * @brief  gsm_gbk2ucs2hex GBK����תucs2������ת��16�����ַ�
 * @param  ucs2hex: 16�������洢��λ��ָ�룬gbk���ַ�
 * @retval ��
 */
void gsm_gbk2ucs2hex(char * ucs2hex,char * gbk)
{
    WCHAR   tmp;
    
	  GSM_DEBUG_FUNC();  
	
    while(*gbk)
    {
        if((*gbk&0xFF) < 0x7F)      //Ӣ��
        {
            
            *ucs2hex++ = '0';
            *ucs2hex++ = '0';
            gsm_char2hex(ucs2hex,*gbk);
            ucs2hex+=2;
            gbk++;  
        }
        else                        //����
        {
            tmp = GSM_SWAP16(*(WCHAR *)gbk);
            tmp = ff_convert(tmp,1);
            gsm_char2hex(ucs2hex,(char)(tmp>>8));
            ucs2hex+=2;
            gsm_char2hex(ucs2hex,(char)tmp);
            ucs2hex+=2;
            gbk+=2;
        }
    }
    *ucs2hex=0;
}



//���Ͷ��ţ�֧����Ӣ��,����ΪGBK�룩
/**
 * @brief  gsm_sms ���Ͷ��ţ�֧����Ӣ��,����ΪGBK�룩
 * @param  num: �绰���룬smstext����������
 * @retval ��
 */
uint8_t gsm_sms(char *num,char *smstext)
{
    char converbuff[160];
	  char cmdbuff[80];
	  char end= 0x1A;
	  uint8_t testSend = 0;
		
	  GSM_DEBUG_FUNC(); 

    GSM_CLEAN_RX();                 //����˽��ջ���������
	
    if(IsASSIC(smstext)==GSM_TRUE)
    {
			//Ӣ��
			//"GSM"�ַ���
			if(gsm_cmd("AT+CSCS=\"GSM\"\r","OK", 100) != GSM_TRUE) goto sms_failure;

			//�ı�ģʽ
			if(gsm_cmd("AT+CMGF=1\r","OK", 100) != GSM_TRUE) goto sms_failure;

			//���ɵ绰�������� 
			sprintf(cmdbuff,"AT+CMGS=\"%s\"\r",num);
			//���ƶ�������
			strcpy(converbuff,smstext);

    }
    else
    {
			//����
			//"UCS2"�ַ���
			if(gsm_cmd("AT+CSCS=\"UCS2\"\r","OK", 100) != GSM_TRUE) goto sms_failure;
			//�ı�ģʽ
			if(gsm_cmd("AT+CMGF=1\r","OK", 100) != GSM_TRUE) goto sms_failure;

			if(gsm_cmd("AT+CSMP=17,167,0,8\r","OK", 100) != GSM_TRUE) goto sms_failure;

			//���ɵ绰�������UCS2�ĵ绰����(��Ҫת�� ucs2��)
			gsm_gbk2ucs2hex(converbuff,num);//ת��ΪUCS2
			sprintf(cmdbuff,"AT+CMGS=\"%s\"\r",converbuff);  
			
			//ת����������
			gsm_gbk2ucs2hex(converbuff,smstext);
    }
		
	//���͵绰����	
	if(gsm_cmd(cmdbuff,">",200) == 0)
	{
		GSM_USART_printf("%s",converbuff);//���Ͷ�������
		
		GSM_DEBUG("Send String:%s",converbuff);

		GSM_CLEAN_RX();
		gsm_cmd(&end,0,100);		//0x1A������
		
		//����Ƿ������
		while(gsm_cmd_check("OK") != GSM_TRUE )
		{		
			if(++testSend >100)//��ȴ�10��
			{
				goto sms_failure;
			}
			GSM_DELAY(100); 		
		}	
		return GSM_TRUE;
	}
	else
	{				
sms_failure:		
		end = 0x1B;
		gsm_cmd(&end,0,0);	//ESC,ȡ������ 
		return GSM_FALSE;		
	}		
}




//��ѯ�Ƿ���յ��¶���
//0:���¶��ţ���0���¶��ŵ�ַ
uint8_t IsReceiveMS(void)
{
//	char address[3]={0};
	uint8_t addressnum=0;
	char *redata;
	char *redata_off;
    uint8_t len;
	
			GSM_DEBUG_FUNC();  

/*------------- ��ѯ�Ƿ����¶��Ų���ȡ�洢λ�� ----------------------------*/
    if(gsm_cmd_check("+CMTI:"))
    {
        return 0;
    }
   
    redata = GSM_RX(len);   //��������
		
		//ƫ������Ӧ���ַ�����ַ
		redata_off = strstr(redata,"+CMTI: \"SM\",");
//		printf("redata_off=%s",redata_off);

		if(redata_off == NULL)
			return 0;
		
		//����ƫ�ƺ�ʣ��ĳ���
		len = len - (redata_off - redata); 
		
//		printf("CMTI:redata:%s,len=%d\n",redata,len);
    if(len == 0)
    {
        return 0;
    }
    
	  //�ָ��ַ���
		strtok(redata_off,",");		
	
		//strtok�ָ�ڶ�����Ϊatoi���룬��ת���ɹ���Ϊ���ŵ�ַ������atoi����0
		addressnum = atoi(strtok(NULL,","));
					
//		printf("\r\naddressnum=%d",addressnum);

		return addressnum;

}	

//��ȡ��������
//�βΣ�	messadd��	���ŵ�ַ
//			num��		���淢���˺���(unicode�����ʽ���ַ���)
//			str��		�����������(unicode�����ʽ���ַ���)
//����ֵ��	0��ʾʧ��
//			1��ʾ�ɹ���ȡ�����ţ��ö���δ�����˴��ǵ�һ�ζ����������Զ�����׼Ϊ�Ѷ���
//			2��ʾ�ɹ���ȡ�����ţ��ö����Ѷ�
uint8_t readmessage(uint8_t messadd,char *num,char *str)
{
	char *redata,cmd[20]={0};
    uint8_t len;
	char result=0;
	
	GSM_DEBUG_FUNC();  

	
	GSM_CLEAN_RX();                 //����˽��ջ���������
	if(messadd>MaxMessAdd)return 0;
	
/*------------- ��ȡ�������� ----------------------------*/
	sprintf(cmd,"AT+CMGR=%d\r",messadd);	
		  
	if(gsm_cmd(cmd,"+CMGR:",500) != GSM_TRUE)
	{
		printf("GSM_FALSE");
		return 0;
	}
	
	redata = GSM_RX(len);   //��������

	if(len == 0)
	{
		return 0;
	}
//	printf("CMGR:redata:%s\nlen=%d\n",redata,len);

	if(strstr(redata,"UNREAD")==0)result=2;
	else	result=1;
	//��һ�����ź��������Ϊ:�������˺��롰
	while(*redata != ',')
	{
		len--;
		if(len==0)
		{
			return 0;
		}
		redata++;
		
	}
	redata+=2;//ȥ��',"'
	while(*redata != '"')
	{
		*num++ = *redata++;
		len--;
	}
	*num = 0;               //�ַ�����β��Ҫ��0
	
	while(*redata != '+')
	{
		len--;
		if(len==0)
		{
			return 0;
		}
		redata++;
	}
	redata+=6;//ȥ��'+32"\r'
	while(*redata != '\r')
	{
		*str++ = *redata++;
	}
	*str = 0;               //�ַ�����β��Ҫ��0
	
//	printf("CMGR:result:%d\n",result);
	return result;
}

#include "ff.h"//firecc936.c�ļ��У�ff_convert()����ʵ��unicode��gbk��ת����

uint8_t hexuni2gbk(char *hexuni,char *chgbk)
{
	uint8_t len=0,i=0;
	WCHAR wgbk=0;
	WCHAR tmp=0;
	uint8_t unitmp[4]={0};
				
	GSM_DEBUG_FUNC();  
	
	GSM_CLEAN_RX();
	len=strlen(hexuni);
	if(!len)return GSM_FALSE;
	//printf("hexuni:%s::len:%d\n",hexuni,len);
	for(i=0;i<len/4;++i)
	{
		if(hexuni[4*i]>=0x41)	unitmp[0]=hexuni[4*i]-0x41+10;
		else	unitmp[0]=hexuni[4*i]-0x30;
		if(hexuni[4*i+1]>=0x41)unitmp[1]=hexuni[4*i+1]-0x41+10;
		else	unitmp[1]=hexuni[4*i+1]-0x30;
		if(hexuni[4*i+2]>=0x41)unitmp[2]=hexuni[4*i+2]-0x41+10;
		else	unitmp[2]=hexuni[4*i+2]-0x30;
		if(hexuni[4*i+3]>=0x41)unitmp[3]=hexuni[4*i+3]-0x41+10;
		else	unitmp[3]=hexuni[4*i+3]-0x30;
		
		tmp=unitmp[0]*0x1000+unitmp[1]*0x100+unitmp[2]*16+unitmp[3];
		wgbk=ff_convert(tmp,0);
		//printf("tmp:%X->wgbk:%X\n",tmp,wgbk);
		
		if(wgbk<0x80)
		{
			*chgbk=(char)wgbk;
			chgbk++;
		}
		else
		{
			*chgbk=(char)(wgbk>>8);
			chgbk++;
			*chgbk=(char)wgbk;
			chgbk++;
		}		
	}	
	*chgbk=0;
	return GSM_TRUE;		
}




/*---------------------------------------------------------------------*/

/*---------------------------------------------------------------------*/
//void gsm_gprs_init(void)
//{
//    GSM_USART_printf("AT+CGCLASS=\"B\"\r");                       //�����ƶ�̨���Ϊ"B"
//    GSM_DELAY(100);
//    GSM_USART_printf("AT+CGDCONT=1,\"IP\",\"CMNET\"\r");          //PDP�����ı�ʶ1��������Э�飬��������ƣ�CMNET
//    GSM_DELAY(100);
//    GSM_USART_printf("AT+CGATT=1\r");                             //���� GPRS ����
//    GSM_DELAY(100);
//    GSM_USART_printf("AT+CIPCSGP=1,\"CMNET\"\r");                 //����Ϊ GPRS ����ģʽ �������ΪCMNET
//}

//void gsm_gprs_tcp_link(char *localport,char * serverip,char * serverport)
//{
//    GSM_USART_printf("AT+CLPORT=\"TCP\",\"%s\"\r",localport);         //��ȡ���ض˿�
//    GSM_DELAY(100);

//    //���÷�����IP�Ͷ˿�
//    GSM_USART_printf("AT+CIPSTART=\"TCP\",\"%s\",\"%s\"\r",serverip,serverport);
//}

//void gsm_gprs_udp_link(char *localport,char * serverip,char * serverport)
//{
//    GSM_USART_printf("AT+CLPORT=\"UDP\",\"%s\"\r",localport);              //��ȡ���ض˿�
//    GSM_DELAY(100);

//    GSM_USART_printf("AT+CIPSTART=\"UDP\",\"%s\",\"%s\"\r",serverip,serverport);   //���÷�����IP�Ͷ˿�
//}

//void gsm_gprs_send(char * str)
//{
//    GSM_USART_printf("AT+CIPSEND\r");
//    GSM_DELAY(100);
//        
//    GSM_USART_printf("%s%c",str,0x1A);
//}

//void gsm_gprs_link_close(void)               //IP���ӶϿ�
//{
//    GSM_USART_printf("AT+CIPCLOSE=1\r");
//}

/**
 * @brief  ��ʼ��GPRS����
 * @param  ��
 * @retval ʧ��GSM_FALSE  �ɹ�GSM_TRUE
 */
uint8_t gsm_gprs_init(void)
{
	GSM_DEBUG_FUNC();  
		
	GSM_CLEAN_RX();
	if(gsm_cmd("AT+CGCLASS=\"B\"\r","OK", 200) != GSM_TRUE) return GSM_FALSE;

	GSM_CLEAN_RX();
	if(gsm_cmd("AT+CGDCONT=1,\"IP\",\"CMNET\"\r","OK", 200) != GSM_TRUE) return GSM_FALSE;		

	GSM_CLEAN_RX();	
	if(gsm_cmd("AT+CGATT=1\r","OK", 200)!= GSM_TRUE) return GSM_FALSE;

	GSM_CLEAN_RX();
	if(gsm_cmd("AT+CIPCSGP=1,\"CMNET\"\r","OK", 200)!= GSM_TRUE) return GSM_FALSE;
	
	return GSM_TRUE;
}

/**
 * @brief  ����TCP���ӣ���ȴ�ʱ��20�룬�����и��������޸�
	* @param  localport: ���ض˿�
	* @param  serverip: ������IP
	* @param  serverport: �������˿�
 * @retval ʧ��GSM_FALSE  �ɹ�GSM_TRUE
 */
uint8_t gsm_gprs_tcp_link(char *localport,char * serverip,char * serverport)
{
	char cmd_buf[100];
	uint8_t testConnect=0;
	
	GSM_DEBUG_FUNC();  
		
	sprintf(cmd_buf,"AT+CLPORT=\"TCP\",\"%s\"\r",localport);
	
	if(gsm_cmd(cmd_buf,"OK", 100)!= GSM_TRUE)
		return GSM_FALSE;
	
	GSM_CLEAN_RX();
		
	sprintf(cmd_buf,"AT+CIPSTART=\"TCP\",\"%s\",\"%s\"\r",serverip,serverport);	
	gsm_cmd(cmd_buf,0, 100);	
	
	//����Ƿ�������
	while(gsm_cmd_check("CONNECT OK") != GSM_TRUE)
	{		
		if(++testConnect >200)//��ȴ�20��
		{
			return GSM_FALSE;
		}
		GSM_DELAY(100); 		
	}				
	return GSM_TRUE;
}

/**
 * @brief  ����UDP���ӣ���ȴ�ʱ��20�룬�����и��������޸�
	* @param  localport: ���ض˿�
	* @param  serverip: ������IP
	* @param  serverport: �������˿�
 * @retval ʧ��GSM_FALSE  �ɹ�GSM_TRUE
 */
uint8_t gsm_gprs_udp_link(char *localport,char * serverip,char * serverport)
{
	char cmd_buf[100];
	uint8_t testConnect=0;
	
	GSM_DEBUG_FUNC();  

	sprintf(cmd_buf,"AT+CLPORT=\"UDP\",\"%s\"\r",localport);
	
	if(gsm_cmd(cmd_buf,"OK", 100)!= GSM_TRUE)
		return GSM_FALSE;
	
	GSM_CLEAN_RX();
		
	sprintf(cmd_buf,"AT+CIPSTART=\"UDP\",\"%s\",\"%s\"\r",serverip,serverport);
	
	gsm_cmd(cmd_buf,0, 100);	
	
		//����Ƿ�������
	while(gsm_cmd_check("CONNECT OK") != GSM_TRUE )
	{
		
		if(++testConnect >200)//��ȴ�20��
		{
			return GSM_FALSE;
		}
		GSM_DELAY(100); 		
	}		
		
	return GSM_TRUE;
}

/**
 * @brief  ʹ��GPRS�������ݣ�����ǰ��Ҫ�Ƚ���UDP��TCP����
	* @param  str: Ҫ���͵�����
 * @retval ʧ��GSM_FALSE  �ɹ�GSM_TRUE
 */
uint8_t gsm_gprs_send(const char * str)
{
	char end = 0x1A;
	uint8_t testSend=0;
	
	GSM_DEBUG_FUNC();

	GSM_CLEAN_RX();
	
	if(gsm_cmd("AT+CIPSEND\r",">",500) == 0)
	{
		GSM_USART_printf("%s",str);
		GSM_DEBUG("Send String:%s",str);

		GSM_CLEAN_RX();
		gsm_cmd(&end,0,100);		
		
		//����Ƿ������
		while(gsm_cmd_check("SEND OK") != GSM_TRUE )
		{		
			if(++testSend >200)//��ȴ�20��
			{
				goto gprs_send_failure;
			}
			GSM_DELAY(100); 		
		}	
		return GSM_TRUE;
	}
	else
	{
		
gprs_send_failure:
		
		end = 0x1B;
		gsm_cmd(&end,0,0);	//ESC,ȡ������ 

		return GSM_FALSE;
	}
}

/**
 * @brief  �Ͽ���������
 * @retval ʧ��GSM_FALSE  �ɹ�GSM_TRUE
 */
uint8_t gsm_gprs_link_close(void)              //IP���ӶϿ�
{
	GSM_DEBUG_FUNC();  

	GSM_CLEAN_RX();
	if(gsm_cmd("AT+CIPCLOSE=1\r","OK",200) != GSM_TRUE)
    {
        return GSM_FALSE;
    }
	return GSM_TRUE;
}

/**
 * @brief  �رճ���
 * @retval ʧ��GSM_FALSE  �ɹ�GSM_TRUE
 */
uint8_t gsm_gprs_shut_close(void)               //�رճ���
{
	uint8_t  check_time=0;
	GSM_DEBUG_FUNC();  

	GSM_CLEAN_RX();	
	gsm_cmd("AT+CIPSHUT\r",0,0) ;
	while(	gsm_cmd_check("OK") != GSM_TRUE)
	{
		if(++check_time >50)
			return GSM_FALSE;
		
		GSM_DELAY(200);
	}

	return GSM_TRUE;
}

/**
 * @brief  ���ش�GSMģ����յ����������ݣ���ӡ������
 * @retval ʧ��GSM_FALSE  �ɹ�GSM_TRUE
 */
uint8_t PostGPRS(void)
{

	char *redata;
	uint8_t len;

	GSM_DEBUG_FUNC();  

	redata = GSM_RX(len);   //�������� 

	if(len == 0)
	{
			return GSM_FALSE;
	}
	
	printf("PostTCP:%s\n",redata);
	GSM_CLEAN_RX();
	
	return GSM_TRUE;

}



/*---------------------------------------------------------------------*/
