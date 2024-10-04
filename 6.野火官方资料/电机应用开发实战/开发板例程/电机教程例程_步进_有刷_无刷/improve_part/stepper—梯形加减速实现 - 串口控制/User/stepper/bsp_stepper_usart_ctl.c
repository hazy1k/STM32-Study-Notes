/**
  ******************************************************************************
  * @file    bsp_stepper_usart_ctl.c
  * @author  fire
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   ���ڿ���
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F103 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */

#include "./stepper/bsp_stepper_usart_ctl.h"

void ShowHelp(void);
void ShowData(int position, int accel_val, int decel_val, int speed, int steps);
void MSD_demo_run(void);

/*! \brief ��ӡ��������
 */
void ShowHelp(void)
{
    printf("\n\r����������������������������Ұ�𲽽�������μӼ�����ʾ���򡪡�������������������������");
    printf("\n\r�������");
    printf("\n\r< ? >       -�����˵�");
    printf("\n\ra[data]     -���ò�������ļ��ٶȣ���Χ��71��32000����λΪ��0.01rad/s^2");
    printf("\n\rd[data]     -���ò�������ļ��ٶȣ���Χ��71��32000����λΪ��0.01rad/s^2");
    printf("\n\rs[data]     -���ò������������ٶȣ���Χ��12��3000����λΪ��0.01rad/s");
    printf("\n\rm[data]     -���趨�Ĳ����ƶ������������Χ��-2147483647��2147483647����������Ϊ˳ʱ�룬����Ϊ��ʱ�룩");
  
    printf("\n\rmove [steps] [accel] [decel] [speed]");
    printf("\n\r            -���趨�Ĳ��������ٶȣ����ٶȣ�����ٶȿ�ʼ�˶�");
    printf("\n\r<Enter��>   -�ظ����һ�ζ���");
    printf("\n\r�����尴����KEY1����������ֹ���(�ѻ�״̬)  KEY2���������ָ���������\n\r ");
    
}

/*! \brief ��ӡ�������
 *  \param accel_val ���ٶ�
 *  \param decel_val ���ٶ�
 *  \param speed        ����ٶ�
 *  \param steps        �ƶ�����
 */
void ShowData(int position, int accel_val, int decel_val, int speed, int steps)
{
  printf("\n\r���ٶ�:%.2frad/s^2",1.0*accel_val/100);
  printf("  ���ٶ�:%.2frad/s^2",1.0*decel_val/100);
  printf("  ����ٶ�:%.2frad/s(%.2frpm)",1.0*speed/100,9.55*speed/100);
  printf("  �ƶ�����:%d",steps);
  printf("\n\r�����ǰλ��: %d\r\n",position);
}


/**
  * @brief  �����ڽ��յ�������
  * @param  ��
  * @retval ��
  */
void DealSerialData(void)
{
    static char showflag =1;
    //Ĭ���ƶ�����
    static int steps = SPR*5;
    //Ĭ�ϼ��ٶ�
    static int accel_val = 500;
    //Ĭ�ϼ��ٶ�
    static int decel_val = 100;
    //Ĭ������ٶ�
    static int speed = 5000;
    
    int acc_temp=0;
    int dec_temp=0;
    int speed_temp=0;
    
    //���յ���ȷ��ָ���ΪTRUE
    char okCmd = FALSE;
    if(showflag)
    {
      showflag = 0;
      ShowData(stepPosition, accel_val, decel_val, speed, steps);
    }
    //����Ƿ���յ�ָ��
    if(status.cmd == TRUE)
    {        
      if(UART_RxBuffer[0] == 'm')
      {
        //�Թ̶������ƶ�
        if(UART_RxBuffer[1] == ' ')
        {
          //�Ӵ��ڻ�ȡ����
          steps = atoi((char const *)UART_RxBuffer+2);
          stepper_move_T(steps, accel_val, decel_val, speed);
          okCmd = TRUE;
          printf("\n\r  ");
        }
        else if(UART_RxBuffer[1] == 'o')
        {
          if(UART_RxBuffer[2] == 'v')
          {
            if(UART_RxBuffer[3] == 'e')
            {
              //���ò��������ٶȡ����ٶȡ�����ٶ�
              if(UART_RxBuffer[4] == ' ')
              {
                int i = 6;
                steps = atoi((char const *)UART_RxBuffer+5);
                while((UART_RxBuffer[i] != ' ') && (UART_RxBuffer[i] != 13)) i++;
                i++;
                accel_val = atoi((char const *)UART_RxBuffer+i);
                while((UART_RxBuffer[i] != ' ') && (UART_RxBuffer[i] != 13)) i++;
                i++;
                decel_val = atoi((char const *)UART_RxBuffer+i);
                while((UART_RxBuffer[i] != ' ') && (UART_RxBuffer[i] != 13)) i++;
                i++;
                speed = atoi((char const *)UART_RxBuffer+i);
                stepper_move_T(steps, accel_val, decel_val, speed);
                okCmd = TRUE;
                printf("\n\r  ");
              }
            }
          }
        }
      }
      else if(UART_RxBuffer[0] == 'a')
      {
        //���ü��ٶ�
        if(UART_RxBuffer[1] == ' ')
        {
          acc_temp = atoi((char const *)UART_RxBuffer+2);
          if(acc_temp>=71 && acc_temp<=32000)
          {
            accel_val = acc_temp;
            printf("\n\r���ٶ�:%.2frad/s^2",1.0*accel_val/100);
            okCmd = TRUE;
          }
        }
      }
      else if(UART_RxBuffer[0] == 'd')
      {
        //���ü��ٶ�
        if(UART_RxBuffer[1] == ' ')
        {
          dec_temp = atoi((char const *)UART_RxBuffer+2);
          if(dec_temp>=71 && dec_temp<=32000)
          {
            decel_val = dec_temp;
            printf("\n\r���ٶ�:%.2frad/s^2",1.0*decel_val/100);
            okCmd = TRUE;
          }
        }
      }
      else if(UART_RxBuffer[0] == 's')
      {
        //��������ٶ�
        if(UART_RxBuffer[1] == ' ')
        {
          speed_temp = atoi((char const *)UART_RxBuffer+2);
          if(speed_temp>=12 && speed_temp<=20000)
          {
            speed = speed_temp;
            printf("\n\r����ٶ�:%.2frad/s",1.0*speed/100);
            okCmd = TRUE;
          }
        }
      }
      else if(UART_RxBuffer[0] == 13)
      {
        //����ǻس���ֱ���ظ���һ���˶�
        stepper_move_T(steps, accel_val, decel_val, speed);
        okCmd = TRUE;
      }
      else if(UART_RxBuffer[0] == '?')
      {
        //��ӡ��������
        ShowHelp();
        okCmd = TRUE;
      }
      else if(UART_RxBuffer[0] == 't')
      {
        //�����������ָ��
        MSD_demo_run();         
        okCmd = TRUE;
      }
      //���ָ���������ӡ��������
      if(okCmd != TRUE)
      {
        printf("\n\r ������������������...");
        ShowHelp();
      }

      //��մ��ڽ��ջ�������
      status.cmd = FALSE;
      uart_FlushRxBuffer();

      if(status.running == TRUE)
      {
        if(status.out_ena == TRUE)
            printf("\n\r�����������...");
        while(status.running == TRUE)
        {
          if(status.out_ena != TRUE)
          break;
        };
        if(status.out_ena == TRUE)
        {
          printf("OK\n\r");
          ShowData(stepPosition, accel_val, decel_val, speed, steps);  
        }

      }

      
    }
}


/**
  * @brief  ������������ʾ
  * @param  ��
  * @retval ��
  */
void MSD_demo_run(void)
{
    uint8_t step_cnt=0;
    int8_t step_num[10] = {2,2,-2,-2,2,2,-4,-4,4,20};
    for(int i=0;i<5;i++)
    {
				if(step_cnt==11)
						step_cnt=0;
				while(status.running == TRUE)
				{
						if(status.out_ena != TRUE)
							 break;
				};
				if(status.out_ena != TRUE)
						break;
				else
				{
						HAL_Delay(1000);
						stepper_move_T(SPR*step_num[step_cnt], 32000, 32000, 3000);

						ShowData(stepPosition, 32000, 32000, 3000, SPR*step_num[step_cnt]);
						step_cnt++;  
				}
    }
    step_cnt=0;
}

