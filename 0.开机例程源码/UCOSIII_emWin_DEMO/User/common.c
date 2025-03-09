#include "common.h"
#include "ff.h"
#include "GUI.h"

/*********************************************************************
*
*       data
*
**********************************************************************
*/
uint8_t comdata[COMDATA_SIZE];

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
void com_data2null(uint8_t *data,uint32_t length)
{
	uint32_t i=0;
	while(i<length)
	{
		data[i]='\0';
		++i;
	}
}

void com_gbk2utf8(const char *src, char *str)
{
	uint32_t j=0,k=0;
	uint16_t gbkdata=0;
	uint16_t UCbuffer[1024];
	for(j=0,k=0;src[j]!='\0';k++)
	{
		if((uint8_t)src[j]>0x80)
		{
			gbkdata=src[j+1]+src[j]*256;
			UCbuffer[k]=ff_convert(gbkdata,1);
			j+=2;
		}
		else
		{
			UCbuffer[k]=0x00ff&src[j];
			j+=1;
		}
	}
	UCbuffer[k]='\0';
	GUI_UC_ConvertUC2UTF8(UCbuffer,2*k+2,str,k*3);
}



void com_utf82gbk(const char *src, char *str)
{
	uint16_t k=0,j=0,i=0;
	uint16_t buffer[1024];
	uint16_t gbkdata=0;
	k=strlen(src);
	com_data2null((uint8_t *)buffer,1024);
	GUI_UC_ConvertUTF82UC(src,k,buffer,1024);
	j=0;
	i=0;
	while(buffer[i]!='\0'&&j<k)
	{
		if(buffer[i]>0x80)
		{
			gbkdata=ff_convert(buffer[i],0);
			str[j]=(char)(gbkdata>>8);
			str[j+1]=(char)gbkdata;
			j+=2;
		}
		else
		{
			str[j]=(char)buffer[i];
			j+=1;
		}
		i++;
	}
	str[j]='\0';
	str[j+1]='\0';
}

void com_createdir(const char *dir)
{
	FRESULT res;
	DIR recdir;
	res=f_opendir(&recdir,dir);
	while(res!=FR_OK)
	{
		f_mkdir(dir);
		res=f_opendir(&recdir,dir);
	}
}
/*---------------------- end of file --------------------------*/
