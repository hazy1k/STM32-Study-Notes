#include "stm32f10x.h"
#include "usart.h"	
#include "lcd.h"
#include "flash.h"
#include <stdio.h>
#include <string.h>

static void LCD_Test(void);	
static void Delay(__IO uint32_t nCount);

int main(void)
{	
	ST7789V_Init();         
	USART_Config();  
  	ST7789V_GramScan(6);
	while(1)
	{
		LCD_Test();
	}
}

void LCD_Test(void)
{
	/* ������ʾ���� */
	static uint8_t testIntCNT = 0;	
	static float testFloatCNT = 0;	
	char dispBuff[100];
	char *pStr = 0;
	testIntCNT++;	
	testFloatCNT += 0.1;
	LCD_SetFont(&Font8x16);	
	LCD_SetColors(RED,BLACK);
  	ST7789V_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
	sprintf(dispBuff,"��ʾ���������� %d ",testIntCNT);
    LCD_ClearLine(LINE(4));
	ST7789V_DispStringLine_EN_CH(LINE(4),dispBuff);
	sprintf(dispBuff,"��ʾ�����ͱ����� %f ",testFloatCNT);
  	LCD_ClearLine(LINE(5));
	ST7789V_DispStringLine_EN_CH(LINE(5),dispBuff);
	sprintf(dispBuff,"������(����2λС��)�� %.2f ",testFloatCNT);
  	LCD_ClearLine(LINE(6));	
	ST7789V_DispStringLine_EN_CH(LINE(6),dispBuff);
	/* ���Ծ�����ʾ */
	LCD_SetTextColor(GREEN);
	pStr = "����2��Ӣ�Ŀո�ʾ��";
	// ʹ�� %*c ���ַ���ǰ����ָ��������Ӣ�Ŀո�
	sprintf(dispBuff,"%*c%s",2,' ',pStr);
  	LCD_ClearLine(LINE(9));					 
	ST7789V_DispStringLine_EN_CH(LINE(9),dispBuff);
	ST7789V_DispStringLine_EN_CH(LINE(11),"��Ӣ��24x32����ʾ����");
	LCD_SetFont(&Font24x32);	
	pStr = "ABCDEF";
	// ����ʱ��Ҫ����Ŀո���� = (Һ�����/���������� - �ַ�������)/2
	sprintf(dispBuff,"%*c%s",((LCD_X_LENGTH/(((sFONT *)LCD_GetFont())->Width))-strlen(pStr))/2,' ',pStr);
	LCD_ClearLine(LINE(6));	
	ST7789V_DispStringLine_EN_CH(LINE(6),dispBuff);
	LCD_SetFont(&Font8x16);	
	pStr = "���ľ���ʾ��";
	// ����ʱ��Ҫ����Ŀո���� = (Һ�����/������ - �ַ�������)/2
	// strlen���㳤��ʱ��һ�����ĵ���2���ֽڣ���2��Ӣ���ַ������Ҳ������Ӣ�Ŀո�
	// ������(WIDTH_CH_CHAR/2)������������
	sprintf(dispBuff,"%*c%s",(LCD_X_LENGTH/(WIDTH_CH_CHAR/2)-strlen(pStr))/2,' ',pStr);
  	LCD_ClearLine(LINE(15));					 
	ST7789V_DispStringLine_EN_CH(LINE(15),dispBuff);
	pStr = "��Ӣ�ľ���ʾ��ABCDEFG";
	sprintf(dispBuff,"%*c%s",(LCD_X_LENGTH/(WIDTH_CH_CHAR/2)-strlen(pStr))/2,' ',pStr);
	LCD_ClearLine(LINE(16));												 
	ST7789V_DispStringLine_EN_CH(LINE(16),dispBuff);
	Delay(0xffffff);
}

static void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount --);	
}
