/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   dht11 ��ʪ�ȴ�����ʵ��
  ******************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  i.MXRT1052������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************
  */
#include "fsl_debug_console.h"
#include "fsl_elcdif.h"
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "./delay/core_delay.h"   
#include "./systick/bsp_systick.h"
#include "./ds18b20/bsp_ds18b20.h"
#include "./lcd/bsp_lcd.h" 
#include "stdio.h" 
#include "string.h"
/*******************************************************************
 * Prototypes
 *******************************************************************/
/* ��ʾ������ */
uint8_t dis_buf[1024];
__IO float temperature;
/*******************************************************************
 * Code
 *******************************************************************/

char buffer[30];
void Float_TO_Char(float slope,char*buffer,int n)  //�����������洢���ַ����飬�ַ�����ĳ���
{
    int temp,i,j;
    if(slope>=0)//�ж��Ƿ����0
        //buffer[0] = '+';
				buffer[0] = ' ';
    else
    {
        buffer[0] = '-';
        slope = -slope;
    }
    temp = (int)slope;//ȡ��������
    for(i=0;temp!=0;i++)//�����������ֵ�λ��
        temp /=10;
    temp =(int)slope;
    for(j=i;j>0;j--)//����������ת�����ַ�����
    {
        buffer[j] = temp%10+'0';
        temp /=10;
    }
    buffer[i+1] = '.';
    slope -=(int)slope;
    for(i=i+2;i<n-1;i++)//��С������ת�����ַ�����
    {
        slope*=10;
        buffer[i]=(int)slope+'0';
        slope-=(int)slope;
    }
    buffer[n-1] = '\0';
}

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
		uint8_t uc,DS18B20Id[8];
		uint8_t DS18B20Id_str[20];
    /* ��ʼ���ڴ汣����Ԫ */
    BOARD_ConfigMPU();
    /* ��ʼ������������ */
    BOARD_InitPins();
    /* ��ʼ��������ʱ�� */
    BOARD_BootClockRUN();
    /* ��ʼ�����Դ��� */
    BOARD_InitDebugConsole();
	  /* ��ʼ����ȷ��ʱ*/
		CPU_TS_TmrInit();
		/* ��ʼ���δ�ʱ�� */
		SysTick_Init();
    /* ��ӡϵͳʱ�� */
    PRINTF("\r\n");
    PRINTF("*****��ӭʹ�� Ұ��i.MX RT1052 ������*****\r\n");
    PRINTF("CPU:             %d Hz\r\n", CLOCK_GetFreq(kCLOCK_CpuClk));
    PRINTF("AHB:             %d Hz\r\n", CLOCK_GetFreq(kCLOCK_AhbClk));
    PRINTF("SEMC:            %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SemcClk));
    PRINTF("SYSPLL:          %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllClk));
    PRINTF("SYSPLLPFD0:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd0Clk));
    PRINTF("SYSPLLPFD1:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd1Clk));
    PRINTF("SYSPLLPFD2:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd2Clk));
    PRINTF("SYSPLLPFD3:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd3Clk));  
		/* ��ʼ��LCD */
    LCD_Init(LCD_INTERRUPT_ENABLE);
		LCD_DisplayStringLine(LINE(1),(uint8_t* )"DS18B20 temperature detect demo");
		
		PRINTF("\r\n*** DS18B20 �¶ȴ�����ʵ��***\r\n");
		if(DS18B20_Init()==0)
		{
				PRINTF("DS18B20��ʼ���ɹ�\n");
		}
		else
		{    
				PRINTF("DS18B20��ʼ��ʧ��\n");
				PRINTF("�뽫��������ȷ���뵽�����\n");

				LCD_SetTextColor(CL_RED);
				LCD_DisplayStringLine(LINE(2),(uint8_t* )"DS18B20 initialization failed!");
				LCD_DisplayStringLine(LINE(3),(uint8_t* )"Please check the connection!");
				/* ͣ�� */
				while(1)
				{}			
		}		
    
		DS18B20_ReadId ( DS18B20Id  );           // ��ȡ DS18B20 �����к�	
    
		for ( uc = 0; uc < 8; uc++ )             // ��ӡ DS18B20 �����к�
		{    
				sprintf((char *)&DS18B20Id_str[2*uc], "%.2x",DS18B20Id[uc]);  
				
				if(uc == 7)
					DS18B20Id_str[17] = '\0';        
		}
		PRINTF("\r\nDS18B20�����к��ǣ� 0x%s\r\n",DS18B20Id_str);
		sprintf((char*)dis_buf,"DS18B20 serial num:0x%s",DS18B20Id_str);  
		LCD_DisplayStringLine(LINE(4),dis_buf);
		memset(dis_buf,0,1024);
		while(1)
		{
			temperature=DS18B20_Get_Temp();	
			PRINTF("DS18B20��ȡ�����¶�Ϊ��%0.3f\n",temperature);
#if defined(__CC_ARM)
			sprintf((char*)dis_buf,"Temperature:   %0.3f   degree Celsius",temperature);
#elif defined(__ICCARM__)
			/*IAR�е�sprintf��ʽ�����������*/
			Float_TO_Char(temperature,(char*)buffer,8);
			strcat((char*)dis_buf,"Temperature:   ");
			strcat((char*)dis_buf,(char*)buffer);
			strcat((char*)dis_buf,"   degree Celsius");
#endif
			PRINTF("str��%s\n",dis_buf);
			LCD_DisplayStringLine(LINE(5),dis_buf);
			memset(dis_buf,0,1024);
			Delay_ms(1000);
		}  

}
/****************************END OF FILE**********************/





