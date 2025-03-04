#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "..\User\bsp\usart\bsp_usart1.h"
#include "..\User\bsp\sim900a\bsp_usart2.h"
#include "..\User\bsp\sim900a\sim900a.h"
#include "GUI.h"


static uint8_t MaxMessAdd=50;


//0��ʾ�ɹ���1��ʾʧ��

uint8_t sim900a_cmd(char *cmd, char *reply,uint32_t waittime )
{    
    SIM900A_CLEAN_RX();                 //����˽��ջ���������

    SIM900A_TX(cmd);                    //��������

    if(reply == 0)                      //����Ҫ��������
    {
        return SIM900A_TRUE;
    }
    
    SIM900A_DELAY(waittime);                 //��ʱ
    
    
    return sim900a_cmd_check(reply);    //�Խ������ݽ��д���
}


//0��ʾ�ɹ���1��ʾʧ��
uint8_t sim900a_cmd_check(char *reply)
{
    uint8_t len;
    uint8_t n;
    uint8_t off;
    char *redata;
    
    redata = SIM900A_RX(len);   //��������
		//printf("check:redata:%s,len=%d\n",redata,len);
    n = 0;
    off = 0;
    while((n + off)<len)
    {
        if(reply[n] == 0)                 //����Ϊ�ջ��߱Ƚ����
        {
            return SIM900A_TRUE;
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
        return SIM900A_TRUE;
    }
    
    return SIM900A_FALSE;       //����ѭ����ʾ�Ƚ���Ϻ�û����ͬ�����ݣ��������
}

char * sim900a_waitask(uint8_t waitask_hook(void))      //�ȴ�������Ӧ��
{
    uint8_t len=0;
    char *redata;
    do{
        redata = SIM900A_RX(len);   //��������
        if(waitask_hook!=0)
        {
            if(waitask_hook()==SIM900A_TRUE)           //���� SIM900A_TRUE ��ʾ��⵽�¼�����Ҫ�˳�
            {
                redata = 0;
                return redata;               
            }
        }
    }while(len==0);                 //��������Ϊ0ʱһֱ�ȴ�
    
    
    SIM900A_DELAY(20);              //��ʱ��ȷ���ܽ��յ�ȫ�����ݣ�115200�������£�ÿms�ܽ���11.52���ֽڣ�
    return redata;
}


//��������
//0��ʾ�ɹ���1��ʾʧ��
uint8_t sim900a_cnum(char *num)
{
    char *redata;
    uint8_t len;
    
    if(sim900a_cmd("AT+CNUM\r","OK", 100) != SIM900A_TRUE)
    {
        return SIM900A_FALSE;
    }
    
    redata = SIM900A_RX(len);   //��������
    
    if(len == 0)
    {
        return SIM900A_FALSE;
    }
    
    //��һ�����ź��������Ϊ:"��������"
    while(*redata != ',')
    {
        len--;
        if(len==0)
        {
            return SIM900A_FALSE;
        }
        redata++;
    }
    redata+=2;
    
    while(*redata != '"')
    {
        *num++ = *redata++;
    }
    *num = 0;               //�ַ�����β��Ҫ��0
    return SIM900A_TRUE;
}


//��ʼ�������ģ��
//0��ʾ�ɹ���1��ʾʧ��
uint8_t sim900a_init(void)
{
	char *redata;
	uint8_t len;
	
	SIM900A_CLEAN_RX();                 //����˽��ջ���������
	sim900a_config();					//��ʼ������
	
    
	if(sim900a_cmd("AT+CGMM\r","OK", 100) != SIM900A_TRUE)
	{
			return SIM900A_FALSE;
	}
	
	redata = SIM900A_RX(len);   //��������
	
	if(len == 0)
	{
			return SIM900A_FALSE;
	}
	if (strstr(redata,"SIMCOM_SIM900A") != 0)
	{
		return SIM900A_TRUE;
	}
	else if(strstr(redata,"SIMCOM_SIM800") != 0)
	{
		return SIM900A_TRUE;
	}
	else
		return SIM900A_FALSE;

}

/*---------------------------------------------------------------------*/

//���𲦴�绰�����ܽӲ���ͨ��
void sim900a_call(char *num)
{
    SIM900A_CLEAN_RX();                 //����˽��ջ���������
    
    sim900a_tx_printf("ATD%s;\r",num);

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
    uint8_t len;
    
    if(sim900a_cmd_check("RING"))
    {
        return SIM900A_FALSE;
    }
    redata = SIM900A_RX(len);   //��������
		
    if(len == 0)
    {
        return SIM900A_FALSE;
    }
    
    //��һ�����ź��������Ϊ:�����롰
    while(*redata != ':')
    {
        len--;
        if(len==0)
        {
            return SIM900A_FALSE;
        }
        redata++;
    }
    redata+=3;
    while(*redata != '"')
    {
        *num++ = *redata++;
    }
    *num = 0;               //�ַ�����β��Ҫ��0
	  SIM900A_CLEAN_RX();
    return SIM900A_TRUE;
}

/*---------------------------------------------------------------------*/

/*---------------------------------------------------------------------*/

uint8_t IsASSIC(char * str)
{
    while(*str)
    {
        if(*str>0x7F)
        {
            return SIM900A_FALSE;
        }
        str++;
    }
    
    return SIM900A_TRUE;
}



void sim900a_gbk2ucs2(char * ucs2,char * gbk)
{
    WCHAR   tmp;
    
    while(*gbk)
    {
        if((*gbk&0xFF) < 0x7F)      //Ӣ��
        {
            
            *ucs2++ = 0;
            *ucs2++ = *gbk++;  
        }
        else                        //����
        {
            tmp = SIM900A_SWAP16(*(WCHAR *)gbk);
            
            *(WCHAR *)ucs2 = SIM900A_SWAP16(ff_convert(tmp,1));
            gbk+=2;
            ucs2+=2;
        }
    }
}

void sim900a_char2hex(char *hex,char ch)
{
    const char numhex[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    *hex++  = numhex[(ch & 0xF0)>>4];
    *hex    = numhex[ ch & 0x0F];
}

void sim900a_gbk2ucs2hex(char * ucs2hex,char * gbk)
{
    WCHAR   tmp;
    
    while(*gbk)
    {
        if((*gbk&0xFF) < 0x7F)      //Ӣ��
        {
            
            *ucs2hex++ = '0';
            *ucs2hex++ = '0';
            sim900a_char2hex(ucs2hex,*gbk);
            ucs2hex+=2;
            gbk++;  
        }
        else                        //����
        {
            tmp = SIM900A_SWAP16(*(WCHAR *)gbk);
            tmp = ff_convert(tmp,1);
            sim900a_char2hex(ucs2hex,(char)(tmp>>8));
            ucs2hex+=2;
            sim900a_char2hex(ucs2hex,(char)tmp);
            ucs2hex+=2;
            gbk+=2;
        }
    }
    *ucs2hex=0;
}

void sim900a_utf82ucs2hex(char * ucs2hex,char * utf8,uint16_t utf8len)
{
    char *ucs,*tmp;//ucs���ڴ洢unicode������tmp����unicode������ָ�����㣬��������free tmp�����
		
		ucs	= (char *)malloc(sizeof(char)*utf8len*2);		//UC����ȫΪ2�ֽ�һ��			
		
		if(ucs == NULL)
		{
				return;
		}
	
		tmp = ucs;	
	
		/* ת����UCS���� */
		GUI_UC_ConvertUTF82UC((const char GUI_UNI_PTR*)utf8,utf8len,(unsigned short *)tmp,utf8len*2);
		
		while(!(*tmp == 0 && *(tmp+1) == 0))//�ǽ�����
		{
			*(WCHAR *)tmp = SIM900A_SWAP16(*(WCHAR *)tmp);
			
			if(((*tmp)&0xFF) < 0x7F)      //Ӣ��
        {	
				
					sim900a_char2hex(ucs2hex,*tmp);
					ucs2hex+=2;
					tmp++;
					
					sim900a_char2hex(ucs2hex,*tmp);
					ucs2hex+=2;
					tmp++;
					
        }
			else
			{			
				sim900a_char2hex(ucs2hex,(char)(*tmp));
				ucs2hex+=2;
				tmp++;
				sim900a_char2hex(ucs2hex,(char)(*tmp));
				ucs2hex+=2;
				tmp++;
			}
		}
		
    *ucs2hex=0;
		
		free(ucs);
		ucs = NULL;
}


//���Ͷ��ţ�֧����Ӣ��,����ΪGBK�룩
void sim900a_sms(char *num,char *smstext)
{
    char ucsbuff[160];

    SIM900A_CLEAN_RX();                 //����˽��ջ���������
	
    if(IsASSIC(smstext)==SIM900A_TRUE)
    {
        //Ӣ��
        sim900a_tx_printf("AT+CSCS=\"GSM\"\r");     //"GSM"�ַ���
        SIM900A_DELAY(100);
        
        sim900a_tx_printf("AT+CMGF=1\r");           //�ı�ģʽ
        SIM900A_DELAY(100);
        
        sim900a_tx_printf("AT+CMGS=\"%s\"\r",num);  //�绰����
        SIM900A_DELAY(100);

        sim900a_tx_printf("%s",smstext);            //��������
        //SIM900A_DELAY(100);          
    }
    else
    {
        //����
        sim900a_tx_printf("AT+CSCS=\"UCS2\"\r");    //"UCS2"�ַ���
        SIM900A_DELAY(100);
        
        sim900a_tx_printf("AT+CMGF=1\r");           //�ı�ģʽ
        SIM900A_DELAY(100);
        
        sim900a_tx_printf("AT+CSMP=17,167,0,8\r");  //
        SIM900A_DELAY(100);
        
        sim900a_gbk2ucs2hex(ucsbuff,num);
        sim900a_tx_printf("AT+CMGS=\"%s\"\r",ucsbuff);  //UCS2�ĵ绰����(��Ҫת�� ucs2��)
        SIM900A_DELAY(100);
        
        sim900a_gbk2ucs2hex(ucsbuff,smstext);
        sim900a_tx_printf("%s",ucsbuff);          //UCS2���ı�����(��Ҫת�� ucs2��)
        //SIM900A_DELAY(100);
    }
    SIM900A_DELAY(1); 
   // sim900a_tx_printf("%c",0x1A);
   // sim900a_tx_printf("%s",end);
	//while((USART2->SR&0X40)==0);//�ȴ���һ�����ݷ������  
	USART2->DR=(u32)0x1A;		//����ʮ����������0X1A,��Ϣ��������
}

//���Ͷ��ţ�֧����Ӣ��,����ʱʹ��UTF8���룩
void sim900a_sms_utf8(char *num,char *smstext,uint16_t numlen,uint16_t textlen)
{
    char ucsbuff[160];
	  char end[2] = {0x1A,0x00};

    SIM900A_CLEAN_RX();                 //����˽��ջ���������
	
    if(IsASSIC(smstext)==SIM900A_TRUE)
    {
        //Ӣ��
        sim900a_tx_printf("AT+CSCS=\"GSM\"\r");     //"GSM"�ַ���
        SIM900A_DELAY(100);
        
        sim900a_tx_printf("AT+CMGF=1\r");           //�ı�ģʽ
        SIM900A_DELAY(100);
        
        sim900a_tx_printf("AT+CMGS=\"%s\"\r",num);  //�绰����
        SIM900A_DELAY(100);

        sim900a_tx_printf("%s",smstext);            //��������
        //SIM900A_DELAY(100);          
    }
    else
    {
        //����
        sim900a_tx_printf("AT+CSCS=\"UCS2\"\r");    //"UCS2"�ַ���
        SIM900A_DELAY(100);
        
        sim900a_tx_printf("AT+CMGF=1\r");           //�ı�ģʽ
        SIM900A_DELAY(100);
        
        sim900a_tx_printf("AT+CSMP=17,167,0,8\r");  //
        SIM900A_DELAY(100);
        
        sim900a_utf82ucs2hex(ucsbuff,num,numlen);
        sim900a_tx_printf("AT+CMGS=\"%s\"\r",ucsbuff);  //UCS2�ĵ绰����(��Ҫת�� ucs2��)
        SIM900A_DELAY(100);

        sim900a_utf82ucs2hex(ucsbuff,smstext,textlen);
        sim900a_tx_printf("%s\r",ucsbuff);          //UCS2���ı�����(��Ҫת�� ucs2��)
        //SIM900A_DELAY(100);			

    }
    
    sim900a_tx_printf("%s",end);
}



//��ѯ�Ƿ���յ��¶���
//0:���¶��ţ���0���¶��ŵ�ַ
uint8_t IsReceiveMS(void)
{
	char address[3]={0};
	uint8_t addressnum=0;
	char *redata;
    uint8_t len;
/*------------- ��ѯ�Ƿ����¶��Ų���ȡ�洢λ�� ----------------------------*/
    if(sim900a_cmd_check("+CMTI:"))
    {
        return 0;
    }
   
    redata = SIM900A_RX(len);   //��������
    if(len == 0)
    {
        return 0;
    }
    
    //��һ�����ź��������Ϊ:�����Ŵ洢��ַ��
    while(*redata != ',')
    {
        len--;
        if(len==0)
        {
            return 0;
        }
        redata++;
    }
	redata+=1;//ȥ����','
	address[0]= *redata++;
	address[1]= *redata++;
	address[2]= *redata++;
	if((address[2]>='0')&&(address[2]<='9'))
		addressnum=(address[0]-'0')*100+(address[1]-'0')*10+(address[2]-'0');
	else if((address[2]>='0')&&(address[2]<='9'))
		addressnum=(address[0]-'0')*10+(address[1]-'0');
	else
		addressnum=address[0]-'0';
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
	SIM900A_CLEAN_RX();                 //����˽��ջ���������
	if(messadd>MaxMessAdd)return 0;
	
/*------------- ��ȡ�������� ----------------------------*/
	sprintf(cmd,"AT+CMGR=%d\r",messadd);	
	if(sim900a_cmd(cmd,"+CMGR:",500) != SIM900A_TRUE)
	{
		return 0;
	}
	redata = SIM900A_RX(len);   //��������
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
	
	//printf("CMGR:result:%d\n",result);
	return result;
}

#include "ff.h"//firecc936.c�ļ��У�ff_convert()����ʵ��unicode��gbk��ת����

uint8_t hexuni2gbk(char *hexuni,char *chgbk)
{
	uint8_t len=0,i=0;
	WCHAR wgbk=0;
	WCHAR tmp=0;
	uint8_t unitmp[4]={0};
	SIM900A_CLEAN_RX();
	len=strlen(hexuni);
	if(!len)return SIM900A_FALSE;
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
	return SIM900A_TRUE;		
}




/*---------------------------------------------------------------------*/

void sim900a_gprs_init(void)
{
    sim900a_tx_printf("AT+CGCLASS=\"B\"\r");                       //�����ƶ�̨���Ϊ"B"
    SIM900A_DELAY(100);
    sim900a_tx_printf("AT+CGDCONT=1,\"IP\",\"CMNET\"\r");          //PDP�����ı�ʶ1��������Э�飬��������ƣ�CMNET
    SIM900A_DELAY(100);
    sim900a_tx_printf("AT+CGATT=1\r");                             //���� GPRS ����
    SIM900A_DELAY(100);
    sim900a_tx_printf("AT+CIPCSGP=1,\"CMNET\"\r");                 //����Ϊ GPRS ����ģʽ �������ΪCMNET
	SIM900A_DELAY(100);
}

uint8_t sim900a_gprs_tcp_link(char *localport,char * serverip,char * serverport)
{
    char *redata;
	sim900a_tx_printf("AT+CLPORT=\"TCP\",\"%s\"\r",localport);         //��ȡ���ض˿�
    SIM900A_DELAY(100);

	SIM900A_CLEAN_RX();
    //���÷�����IP�Ͷ˿�
    sim900a_tx_printf("AT+CIPSTART=\"TCP\",\"%s\",\"%s\"\r",serverip,serverport);
	
	redata=sim900a_waitask(0);
	SIM900A_CLEAN_RX();
	redata=sim900a_waitask(0);
	if((strstr(redata,"OK") == NULL)&&(strstr(redata,"ALREADY")==NULL))
    {
        return SIM900A_FALSE;
    }
	return SIM900A_TRUE;
}

uint8_t sim900a_gprs_udp_link(char *localport,char * serverip,char * serverport)
{
	char *redata;
    sim900a_tx_printf("AT+CLPORT=\"UDP\",\"%s\"\r",localport);              //��ȡ���ض˿�
    SIM900A_DELAY(100);
	
	SIM900A_CLEAN_RX();
    sim900a_tx_printf("AT+CIPSTART=\"UDP\",\"%s\",\"%s\"\r",serverip,serverport);   //���÷�����IP�Ͷ˿�
	
	redata=sim900a_waitask(0);
	SIM900A_CLEAN_RX();
	redata=sim900a_waitask(0);
	
	if(strstr(redata,"OK") == NULL)
    {
        return SIM900A_FALSE;
    }
	return SIM900A_TRUE;
}


uint8_t sim900a_gprs_send(const char * str)
{
    char *redata;
	SIM900A_CLEAN_RX();
	sim900a_tx_printf("AT+CIPSEND\r");
    //SIM900A_DELAY(100);	
    redata=sim900a_waitask(0);
	
	SIM900A_CLEAN_RX();
    sim900a_tx_printf("%s",str);
	while((USART2->SR&0X40)==0);//�ȴ���һ�����ݷ������  
	USART2->DR=(u32)0x1A;		//����ʮ����������0X1A,��Ϣ��������
	
	redata=sim900a_waitask(0);
	if(strstr(redata,"OK")==NULL)
    {
        return SIM900A_FALSE;
    }
	return SIM900A_TRUE;
}

uint8_t sim900a_gprs_link_close(void)              //IP���ӶϿ�
{
	SIM900A_CLEAN_RX();
	if(sim900a_cmd("AT+CIPCLOSE=1\r","OK",200) != SIM900A_TRUE)
    {
        return SIM900A_FALSE;
    }
	return SIM900A_TRUE;
}

uint8_t sim900a_gprs_shut_close(void)               //�رճ���
{
	SIM900A_CLEAN_RX();
	if(sim900a_cmd("AT+CIPSHUT\r","OK",200) != SIM900A_TRUE)
    {
        return SIM900A_FALSE;
    }
	return SIM900A_TRUE;
}

uint8_t PostGPRS(void)
{
	char *redata;
    uint8_t len;
   
    redata = SIM900A_RX(len);   //�������� 
	
    if(len == 0)
    {
        return SIM900A_FALSE;
    }
	printf("PostUDP:%s\n",redata);
	SIM900A_CLEAN_RX();
    return SIM900A_TRUE;

}
/*---------------------------------------------------------------------*/
