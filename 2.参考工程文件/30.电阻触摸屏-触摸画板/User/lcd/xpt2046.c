#include "xpt2046.h"
#include "ili9341.h"
#include "led.h"
#include "fonts.h"
#include "flash.h"
#include "palette.h"
#include <stdio.h> 
#include <string.h>

static void XPT2046_DelayUS(__IO uint32_t ulCount);
static void XPT2046_WriteCMD(uint8_t ucCmd);
static uint16_t XPT2046_ReadCMD(void);
static uint16_t XPT2046_ReadAdc(uint8_t ucChannel);
static void XPT2046_ReadAdc_XY(int16_t *sX_Ad, int16_t *sY_Ad);
static uint8_t XPT2046_ReadAdc_Smooth_XY(strType_XPT2046_Coordinate *pScreenCoordinate);
static uint8_t XPT2046_Calculate_CalibrationFactor(strType_XPT2046_Coordinate *pDisplayCoordinate, strType_XPT2046_Coordinate *pScreenSample, strType_XPT2046_Calibration *pCalibrationFactor);
static void ILI9341_DrawCross(uint16_t usX, uint16_t usY);
// Ĭ�ϴ�����������ͬ����Ļ���в��죬�����µ��ô���У׼������ȡ
strType_XPT2046_TouchPara strXPT2046_TouchPara[] = { 	
 -0.006464,   -0.073259,  280.358032,    0.074878,    0.002052,   -6.545977,  // ɨ�跽ʽ0
	0.086314,    0.001891,  -12.836658,   -0.003722,   -0.065799,  254.715714,// ɨ�跽ʽ1
	0.002782,    0.061522,  -11.595689,    0.083393,    0.005159,  -15.650089,// ɨ�跽ʽ2
	0.089743,   -0.000289,  -20.612209,   -0.001374,    0.064451,  -16.054003,// ɨ�跽ʽ3
	0.000767,   -0.068258,  250.891769,   -0.085559,   -0.000195,  334.747650,// ɨ�跽ʽ4
 -0.084744,    0.000047,  323.163147,   -0.002109,   -0.066371,  260.985809,  // ɨ�跽ʽ5
 -0.001848,    0.066984,  -12.807136,   -0.084858,   -0.000805,  333.395386,  // ɨ�跽ʽ6
 -0.085470,   -0.000876,  334.023163,   -0.003390,    0.064725,   -6.211169,  // ɨ�跽ʽ7
};

volatile uint8_t ucXPT2046_TouchFlag = 0; // ������־λ

void XPT2046_Init(void) // ��ʼ��XPT2046
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(XPT2046_SPI_GPIO_CLK|XPT2046_PENIRQ_GPIO_CLK, ENABLE);
  /* ģ��SPI GPIO��ʼ�� */       
  GPIO_InitStructure.GPIO_Pin = XPT2046_SPI_CLK_PIN; // ʱ������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;	  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(XPT2046_SPI_CLK_PORT, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = XPT2046_SPI_MOSI_PIN; // ����������������
  GPIO_Init(XPT2046_SPI_MOSI_PORT, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = XPT2046_SPI_MISO_PIN; // ���������������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;      
  GPIO_Init(XPT2046_SPI_MISO_PORT, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = XPT2046_SPI_CS_PIN; // Ƭѡ����
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      
  GPIO_Init(XPT2046_SPI_CS_PORT, &GPIO_InitStructure); 
  /* ����Ƭѡ��ѡ��XPT2046 */
  XPT2046_CS_DISABLE();			
  // �����������ź�ָʾ���ţ���ʹ���ж�
  GPIO_InitStructure.GPIO_Pin = XPT2046_PENIRQ_GPIO_PIN;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 // ��������
  GPIO_Init(XPT2046_PENIRQ_GPIO_PORT, &GPIO_InitStructure);		
}
	
static void XPT2046_DelayUS(__IO uint32_t ulCount) // ����ʱ����
{
	uint32_t i;
	for(i = 0; i < ulCount; i ++)
	{
		uint8_t uc = 12;     
		while ( uc -- );   
	}
}
// ��XPT2046�����������������0x90 :ͨ��Y+��ѡ������֣�0xd0 :ͨ��X+��ѡ�������
static void XPT2046_WriteCMD(uint8_t ucCmd) 
{
	uint8_t i;
	XPT2046_MOSI_0(); // ����MOSI
	XPT2046_CLK_LOW(); // ����ʱ��
	for(i = 0; i < 8; i ++) // ��������
	{
		((ucCmd >> (7 - i)) & 0x01 ) ? XPT2046_MOSI_1() : XPT2046_MOSI_0();
		XPT2046_DelayUS(5);
		XPT2046_CLK_HIGH();
		XPT2046_DelayUS(5);
		XPT2046_CLK_LOW();
	}
}
// ��ȡXPT2046������
static uint16_t XPT2046_ReadCMD ( void ) 
{
	uint8_t i;
	uint16_t usBuf=0, usTemp;
	XPT2046_MOSI_0();
	XPT2046_CLK_HIGH();
	for(i = 0; i < 12; i++) 
	{
		XPT2046_CLK_LOW();    
		usTemp = XPT2046_MISO();
		usBuf |= usTemp << ( 11 - i );
		XPT2046_CLK_HIGH();
	}
	return usBuf;
}
// �� XPT2046 ѡ��һ��ģ��ͨ��������ADC��������ADC�������
static uint16_t XPT2046_ReadAdc(uint8_t ucChannel)
{
	XPT2046_WriteCMD(ucChannel);
	return XPT2046_ReadCMD();
}
// ��ȡ XPT2046 ��Xͨ����Yͨ����ADֵ��12 bit�������4096��
static void XPT2046_ReadAdc_XY(int16_t * sX_Ad, int16_t * sY_Ad)  
{ 
	int16_t sX_Ad_Temp, sY_Ad_Temp; 
	sX_Ad_Temp = XPT2046_ReadAdc(XPT2046_CHANNEL_X);
	XPT2046_DelayUS(1); 
	sY_Ad_Temp = XPT2046_ReadAdc ( XPT2046_CHANNEL_Y ); 
	* sX_Ad = sX_Ad_Temp; 
	* sY_Ad = sY_Ad_Temp; 
}
// �ڴ��� XPT2046 ��Ļʱ��ȡһ�������ADֵ�����Ը���������˲�
// ע�⣺����Ӧ��ʵ��ר��,���Ǻܾ�׼�����Ǽ򵥣��ٶȱȽϿ�   
static uint8_t XPT2046_ReadAdc_Smooth_XY ( strType_XPT2046_Coordinate * pScreenCoordinate )
{
	uint8_t ucCount = 0, i;
	int16_t sAD_X, sAD_Y;
	int16_t sBufferArray[2][10] = {{0},{0}}; // ����X��Y���ж�β���
	// �洢�����е���Сֵ�����ֵ
	int32_t lX_Min, lX_Max, lY_Min, lY_Max;
	/* ѭ������10�� */ 
	do					       				
	{		  
		XPT2046_ReadAdc_XY(&sAD_X, &sAD_Y);  
		sBufferArray[0][ucCount] = sAD_X;  
		sBufferArray[1][ucCount] = sAD_Y;
		ucCount ++;  
	}while((XPT2046_PENIRQ_Read() == XPT2046_PENIRQ_ActiveLevel)&&(ucCount < 10)); // �û����������ʱ��TP_INT_IN�ź�Ϊ�� ���� ucCount<10
	/* ������ʵ��� */
	if(XPT2046_PENIRQ_Read() != XPT2046_PENIRQ_ActiveLevel)
		ucXPT2046_TouchFlag = 0; // �жϱ�־��λ
	/* ����ɹ�����10������ */
	if(ucCount == 10)		 					
	{
		lX_Max = lX_Min = sBufferArray[0][0];
		lY_Max = lY_Min = sBufferArray[1][0];       
		for(i = 1; i < 10; i++)
		{
			if(sBufferArray[0][i] < lX_Min)
				lX_Min = sBufferArray[0][i];
			else if ( sBufferArray[0][i] > lX_Max)
				lX_Max = sBufferArray[0][i];
		}
		for(i = 1; i < 10; i ++)
		{
			if(sBufferArray[1][i] < lY_Min)
				lY_Min = sBufferArray[1][i];
			else if(sBufferArray[1][i] > lY_Max)
				lY_Max = sBufferArray[1][i];
		}
		/*ȥ����Сֵ�����ֵ֮����ƽ��ֵ*/
		pScreenCoordinate ->x =  ( sBufferArray [ 0 ] [ 0 ] + sBufferArray [ 0 ] [ 1 ] + sBufferArray [ 0 ] [ 2 ] + sBufferArray [ 0 ] [ 3 ] + sBufferArray [ 0 ] [ 4 ] + 
		                           sBufferArray [ 0 ] [ 5 ] + sBufferArray [ 0 ] [ 6 ] + sBufferArray [ 0 ] [ 7 ] + sBufferArray [ 0 ] [ 8 ] + sBufferArray [ 0 ] [ 9 ] - lX_Min-lX_Max ) >> 3;
		pScreenCoordinate ->y =  ( sBufferArray [ 1 ] [ 0 ] + sBufferArray [ 1 ] [ 1 ] + sBufferArray [ 1 ] [ 2 ] + sBufferArray [ 1 ] [ 3 ] + sBufferArray [ 1 ] [ 4 ] + 
		                           sBufferArray [ 1 ] [ 5 ] + sBufferArray [ 1 ] [ 6 ] + sBufferArray [ 1 ] [ 7 ] + sBufferArray [ 1 ] [ 8 ] + sBufferArray [ 1 ] [ 9 ] - lY_Min-lY_Max ) >> 3; 
		return 1;		
	}   	
	return 0;    	
}
static uint8_t XPT2046_Calculate_CalibrationFactor(strType_XPT2046_Coordinate *pDisplayCoordinate, strType_XPT2046_Coordinate *pScreenSample, strType_XPT2046_Calibration *pCalibrationFactor) {
    uint8_t ucRet = 1;
    pCalibrationFactor->Divider = ((pScreenSample[0].x - pScreenSample[2].x) * (pScreenSample[1].y - pScreenSample[2].y)) - ((pScreenSample[1].x - pScreenSample[2].x) * (pScreenSample[0].y - pScreenSample[2].y));
    if (pCalibrationFactor->Divider == 0)
        ucRet = 0;
    else {
        pCalibrationFactor->An = ((pDisplayCoordinate[0].x - pDisplayCoordinate[2].x) * (pScreenSample[1].y - pScreenSample[2].y)) - ((pDisplayCoordinate[1].x - pDisplayCoordinate[2].x) * (pScreenSample[0].y - pScreenSample[2].y));
        pCalibrationFactor->Bn = ((pScreenSample[0].x - pScreenSample[2].x) * (pDisplayCoordinate[1].x - pDisplayCoordinate[2].x)) - ((pDisplayCoordinate[0].x - pDisplayCoordinate[2].x) * (pScreenSample[1].x - pScreenSample[2].x));
        pCalibrationFactor->Cn = (pScreenSample[2].x * pDisplayCoordinate[1].x - pScreenSample[1].x * pDisplayCoordinate[2].x) * pScreenSample[0].y +
                                 (pScreenSample[0].x * pDisplayCoordinate[2].x - pScreenSample[2].x * pDisplayCoordinate[0].x) * pScreenSample[1].y +
                                 (pScreenSample[1].x * pDisplayCoordinate[0].x - pScreenSample[0].x * pDisplayCoordinate[1].x) * pScreenSample[2].y;
        pCalibrationFactor->Dn = ((pDisplayCoordinate[0].y - pDisplayCoordinate[2].y) * (pScreenSample[1].y - pScreenSample[2].y)) - ((pDisplayCoordinate[1].y - pDisplayCoordinate[2].y) * (pScreenSample[0].y - pScreenSample[2].y));
        pCalibrationFactor->En = ((pScreenSample[0].x - pScreenSample[2].x) * (pDisplayCoordinate[1].y - pDisplayCoordinate[2].y)) - ((pDisplayCoordinate[0].y - pDisplayCoordinate[2].y) * (pScreenSample[1].x - pScreenSample[2].x));
        pCalibrationFactor->Fn = (pScreenSample[2].x * pDisplayCoordinate[1].y - pScreenSample[1].x * pDisplayCoordinate[2].y) * pScreenSample[0].y +
                                 (pScreenSample[0].x * pDisplayCoordinate[2].y - pScreenSample[2].x * pDisplayCoordinate[0].y) * pScreenSample[1].y +
                                 (pScreenSample[1].x * pDisplayCoordinate[0].y - pScreenSample[0].x * pDisplayCoordinate[1].y) * pScreenSample[2].y;
    }
    return ucRet;
}
// �� ILI9341 ����ʾУ������ʱ��Ҫ��ʮ��
static void ILI9341_DrawCross(uint16_t usX, uint16_t usY)
{
	ILI9341_DrawLine(usX-10,usY,usX+10,usY);
	ILI9341_DrawLine(usX, usY - 10, usX, usY+10);	
}
// XPT2046 ����У׼������LCD_Mode Ϊ��ǰʹ�õ�LCDģʽ
uint8_t XPT2046_Touch_Calibrate(uint8_t LCD_Mode) {
    uint8_t i;
    char cStr[100];
    uint16_t usTest_x = 0, usTest_y = 0, usGap_x = 0, usGap_y = 0;
    char *pStr = 0;
    strType_XPT2046_Coordinate strCrossCoordinate[4], strScreenSample[4];
    strType_XPT2046_Calibration CalibrationFactor;
    LCD_SetFont(&Font8x16);
    LCD_SetColors(BLUE, BLACK);
    ILI9341_GramScan(LCD_Mode);
    // ���á�ʮ���ֽ���������
    strCrossCoordinate[0].x = LCD_X_LENGTH >> 2;
    strCrossCoordinate[0].y = LCD_Y_LENGTH >> 2;
    strCrossCoordinate[1].x = strCrossCoordinate[0].x;
    strCrossCoordinate[1].y = (LCD_Y_LENGTH * 3) >> 2;
    strCrossCoordinate[2].x = (LCD_X_LENGTH * 3) >> 2;
    strCrossCoordinate[2].y = strCrossCoordinate[1].y;
    strCrossCoordinate[3].x = strCrossCoordinate[2].x;
    strCrossCoordinate[3].y = strCrossCoordinate[0].y;
    for (i = 0; i < 4; i++) {
        ILI9341_Clear(0, 0, LCD_X_LENGTH, LCD_Y_LENGTH);
        pStr = "Touch Calibrate ......";
        sprintf(cStr, "%*c%s", ((LCD_X_LENGTH / (((sFONT *)LCD_GetFont())->Width)) - strlen(pStr)) / 2, ' ', pStr);
        ILI9341_DispStringLine_EN(LCD_Y_LENGTH >> 1, cStr);
        sprintf(cStr, "%*c%d", ((LCD_X_LENGTH / (((sFONT *)LCD_GetFont())->Width)) - 1) / 2, ' ', i + 1);
        ILI9341_DispStringLine_EN((LCD_Y_LENGTH >> 1) - (((sFONT *)LCD_GetFont())->Height), cStr);
        XPT2046_DelayUS(300000); // �ʵ�����ʱ���б�Ҫ
        ILI9341_DrawCross(strCrossCoordinate[i].x, strCrossCoordinate[i].y);
        while (!XPT2046_ReadAdc_Smooth_XY(&strScreenSample[i])); // ��ȡXPT2046���ݵ�����pCoordinate
    }
    XPT2046_Calculate_CalibrationFactor(strCrossCoordinate, strScreenSample, &CalibrationFactor);
    if (CalibrationFactor.Divider == 0) goto Failure;
    usTest_x = ((CalibrationFactor.An * strScreenSample[3].x) + (CalibrationFactor.Bn * strScreenSample[3].y) + CalibrationFactor.Cn) / CalibrationFactor.Divider;
    usTest_y = ((CalibrationFactor.Dn * strScreenSample[3].x) + (CalibrationFactor.En * strScreenSample[3].y) + CalibrationFactor.Fn) / CalibrationFactor.Divider;
    usGap_x = (usTest_x > strCrossCoordinate[3].x) ? (usTest_x - strCrossCoordinate[3].x) : (strCrossCoordinate[3].x - usTest_x);
    usGap_y = (usTest_y > strCrossCoordinate[3].y) ? (usTest_y - strCrossCoordinate[3].y) : (strCrossCoordinate[3].y - usTest_y);
    if ((usGap_x > 15) || (usGap_y > 15)) goto Failure; // ����ͨ���޸�������ֵ�Ĵ�С����������
    // У׼ϵ��Ϊȫ�ֱ���
    strXPT2046_TouchPara[LCD_Mode].dX_X = (CalibrationFactor.An * 1.0) / CalibrationFactor.Divider;
    strXPT2046_TouchPara[LCD_Mode].dX_Y = (CalibrationFactor.Bn * 1.0) / CalibrationFactor.Divider;
    strXPT2046_TouchPara[LCD_Mode].dX = (CalibrationFactor.Cn * 1.0) / CalibrationFactor.Divider;
    strXPT2046_TouchPara[LCD_Mode].dY_X = (CalibrationFactor.Dn * 1.0) / CalibrationFactor.Divider;
    strXPT2046_TouchPara[LCD_Mode].dY_Y = (CalibrationFactor.En * 1.0) / CalibrationFactor.Divider;
    strXPT2046_TouchPara[LCD_Mode].dY = (CalibrationFactor.Fn * 1.0) / CalibrationFactor.Divider;
    ILI9341_Clear(0, 0, LCD_X_LENGTH, LCD_Y_LENGTH);
    LCD_SetTextColor(GREEN);
    pStr = "Calibrate Succed";
    sprintf(cStr, "%*c%s", ((LCD_X_LENGTH / (((sFONT *)LCD_GetFont())->Width)) - strlen(pStr)) / 2, ' ', pStr);
    ILI9341_DispStringLine_EN(LCD_Y_LENGTH >> 1, cStr);
    XPT2046_DelayUS(1000000);
    return 1;

Failure:
    ILI9341_Clear(0, 0, LCD_X_LENGTH, LCD_Y_LENGTH);
    LCD_SetTextColor(RED);
    pStr = "Calibrate fail";
    sprintf(cStr, "%*c%s", ((LCD_X_LENGTH / (((sFONT *)LCD_GetFont())->Width)) - strlen(pStr)) / 2, ' ', pStr);
    ILI9341_DispStringLine_EN(LCD_Y_LENGTH >> 1, cStr);
    pStr = "try again";
    sprintf(cStr, "%*c%s", ((LCD_X_LENGTH / (((sFONT *)LCD_GetFont())->Width)) - strlen(pStr)) / 2, ' ', pStr);
    ILI9341_DispStringLine_EN((LCD_Y_LENGTH >> 1) + (((sFONT *)LCD_GetFont())->Height), cStr);
    XPT2046_DelayUS(1000000);
    return 0;
}
// ��FLASH�л�ȡ �� ����У������������У�����д�뵽SPI FLASH�У�
void Calibrate_or_Get_TouchParaWithFlash(uint8_t LCD_Mode,uint8_t forceCal)
{
	uint8_t para_flag=0;
	
	//��ʼ��FLASH
	SPI_FLASH_Init();
	
	//��ȡ����������־
	SPI_FLASH_BufferRead(&para_flag,FLASH_TOUCH_PARA_FLAG_ADDR,1);

	//�������ڱ�־��florceCal=1ʱ������У������
	if(para_flag != FLASH_TOUCH_PARA_FLAG_VALUE | forceCal ==1)
	{ 		
		//����־���ڣ�˵��ԭ��FLASH���д���������
		//�ȶ�������LCDģʽ�Ĳ���ֵ���Ա��Ժ�ǿ�Ƹ���ʱֻ����ָ��LCDģʽ�Ĳ���,����ģʽ�Ĳ���
		if(  para_flag == FLASH_TOUCH_PARA_FLAG_VALUE && forceCal == 1)
		{
			SPI_FLASH_BufferRead((uint8_t *)&strXPT2046_TouchPara,FLASH_TOUCH_PARA_ADDR,4*6*8);	
		}
		
		//�ȴ�������У�����,����ָ��LCDģʽ�Ĵ�������ֵ
		while( ! XPT2046_Touch_Calibrate (LCD_Mode) );     

		//��������
		SPI_FLASH_SectorErase(0);
		
		//���ô���������־
		para_flag = FLASH_TOUCH_PARA_FLAG_VALUE;
		//д�봥��������־
		SPI_FLASH_BufferWrite(&para_flag,FLASH_TOUCH_PARA_FLAG_ADDR,1);
		//д�����µĴ�������
		SPI_FLASH_BufferWrite((uint8_t *)&strXPT2046_TouchPara,FLASH_TOUCH_PARA_ADDR,4*6*8);
 
	}
	else	//����־�����Ҳ�ǿ��У������ֱ�Ӵ�FLASH�ж�ȡ
	{
		SPI_FLASH_BufferRead((uint8_t *)&strXPT2046_TouchPara,FLASH_TOUCH_PARA_ADDR,4*6*8);	
	}
}
// ��ȡ XPT2046 �����㣨У׼�󣩵�����
uint8_t XPT2046_Get_TouchedPoint(strType_XPT2046_Coordinate *pDisplayCoordinate, strType_XPT2046_TouchPara *pTouchPara) {
    uint8_t ucRet = 1; // ���������򷵻�1
    strType_XPT2046_Coordinate strScreenCoordinate;
    if (XPT2046_ReadAdc_Smooth_XY(&strScreenCoordinate)) {
        pDisplayCoordinate->x = (pTouchPara[LCD_SCAN_MODE].dX_X * strScreenCoordinate.x) + (pTouchPara[LCD_SCAN_MODE].dX_Y * strScreenCoordinate.y) + pTouchPara[LCD_SCAN_MODE].dX;
        pDisplayCoordinate->y = (pTouchPara[LCD_SCAN_MODE].dY_X * strScreenCoordinate.x) + (pTouchPara[LCD_SCAN_MODE].dY_Y * strScreenCoordinate.y) + pTouchPara[LCD_SCAN_MODE].dY;
    } else {
        ucRet = 0; // �����ȡ�Ĵ�����Ϣ�����򷵻�0
    }
    return ucRet;
}
// ���������״̬��
uint8_t XPT2046_TouchDetect(void)
{ 
	static enumTouchState touch_state = XPT2046_STATE_RELEASE;
	static uint32_t i;
	uint8_t detectResult = TOUCH_NOT_PRESSED;
	switch(touch_state)
	{
		case XPT2046_STATE_RELEASE:	
		if(XPT2046_PENIRQ_Read() == XPT2046_PENIRQ_ActiveLevel) //��һ�γ��ִ����ź�
		{
			touch_state = XPT2046_STATE_WAITING;
			detectResult =TOUCH_NOT_PRESSED;
		}
		else	//�޴���
		{
			touch_state = XPT2046_STATE_RELEASE;
			detectResult =TOUCH_NOT_PRESSED;
		}
		break;		
		case XPT2046_STATE_WAITING:
			if(XPT2046_PENIRQ_Read() == XPT2046_PENIRQ_ActiveLevel)
			{
				i++;
				// �ȴ�ʱ�������ֵ����Ϊ����������
				// ����ʱ�� = DURIATION_TIME * �����������õ�ʱ����
				// ���ڶ�ʱ���е��ã�ÿ10ms����һ�Σ�������ʱ��Ϊ��DURIATION_TIME*10ms
				if(i > DURIATION_TIME)		
				{
					i = 0;
					touch_state = XPT2046_STATE_PRESSED;
					detectResult = TOUCH_PRESSED;
				}
				else // �ȴ�ʱ���ۼ�
				{
					touch_state = XPT2046_STATE_WAITING;
					detectResult =	 TOUCH_NOT_PRESSED;					
				}
			}
			else // �ȴ�ʱ��ֵδ�ﵽ��ֵ��Ϊ��Ч��ƽ�����ɶ�������					
			{
				i = 0;
            	touch_state = XPT2046_STATE_RELEASE; 
				detectResult = TOUCH_NOT_PRESSED;
			}
			break;
		case XPT2046_STATE_PRESSED:	
			if(XPT2046_PENIRQ_Read() == XPT2046_PENIRQ_ActiveLevel)		//������������
			{
				touch_state = XPT2046_STATE_PRESSED;
				detectResult = TOUCH_PRESSED;
			}
			else // �����ͷ�
			{
				touch_state = XPT2046_STATE_RELEASE;
				detectResult = TOUCH_NOT_PRESSED;
			}
			break;			
		default:
			touch_state = XPT2046_STATE_RELEASE;
			detectResult = TOUCH_NOT_PRESSED;
			break;
	}		
	return detectResult;
}
// �����������µ�ʱ�����ñ�����
void XPT2046_TouchDown(strType_XPT2046_Coordinate * touch)
{
	// ��Ϊ��ֵ��ʾ֮ǰ�Ѵ����
	if(touch->pre_x == -1 && touch->pre_x == -1)
		return;  
	/* �����������ѡ��ť */
  	Touch_Button_Down(touch->x,touch->y);
  	/* �������켣 */
  	Draw_Trail(touch->pre_x,touch->pre_y,touch->x,touch->y,&brush);	
}
// �������ͷŵ�ʱ�����ñ�����
void XPT2046_TouchUp(strType_XPT2046_Coordinate * touch) 
{
	if(touch->pre_x == -1 && touch->pre_x == -1)
		return;
	Touch_Button_Up(touch->pre_x,touch->pre_y);	
}
// ��⵽�����ж�ʱ���õĴ�����,ͨ��������tp_down ��tp_up�㱨������
void XPT2046_TouchEvenHandler(void )
{
	static strType_XPT2046_Coordinate cinfo={-1,-1,-1,-1};
	if(XPT2046_TouchDetect() == TOUCH_PRESSED)
	{
		LED_GREEN;	
		// ��ȡ��������
		XPT2046_Get_TouchedPoint(&cinfo,strXPT2046_TouchPara);
		// ���������Ϣ������
		XPT2046_DEBUG("x=%d,y=%d",cinfo.x,cinfo.y);
		// ���ô���������ʱ�Ĵ����������ڸú�����д�Լ��Ĵ������´������
		XPT2046_TouchDown(&cinfo);
		/* ���´�����Ϣ��pre xy */
		cinfo.pre_x = cinfo.x; cinfo.pre_y = cinfo.y;  
	}
	else
	{
		LED_BLUE;
		// ���ô������ͷ�ʱ�Ĵ����������ڸú�����д�Լ��Ĵ����ͷŴ������
		XPT2046_TouchUp(&cinfo);
		/* �����ͷţ��� xy ����Ϊ�� */
		cinfo.x = -1;
		cinfo.y = -1; 
		cinfo.pre_x = -1;
		cinfo.pre_y = -1;
	}
}
