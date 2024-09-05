/**
  ******************************************************************************
  * @file    bsp_as608.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ָ��ʶ��ģ��ʵ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��STM32 F103-�Ե� ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "./as608/as608_test.h"
#include "./as608/bsp_as608.h"

extern uint8_t TouchOut_int_flag;

 /**
  * @brief  ָ�����
  * @param  ��
  * @retval ��
  */
void Show_Message(void)
{
  AS608_INFO("   ������ָ�����ָ����������봰�ڰ��س������ٷ���ָ�����\r\n");
  AS608_INFO("   ָ��   ------      ���� \r\n");
  AS608_INFO("    1     ------    ���ָ�� \r\n");
  AS608_INFO("    2     ------    �Ա�ָ�� \r\n");
  AS608_INFO("    3     ------    ɾ��ָ���û�ָ��\r\n");
  AS608_INFO("    4     ------    ���ָ�ƿ�\r\n");  
}



/**
  * @brief  ָ��ģ�����
  * @param  ��
  * @retval ��
  */
void  FR_Task(void)
{	
  uint32_t   ch;
  scanf("%d",&ch);
	
  AS608_INFO("���յ��ַ���%d\r\n",ch);
  

	
  switch(ch)
  {
    case 1:
      Add_FR();                                    /*���ָ��*/
		  Show_Message();
    break;
			
		case 2:
      Compare_FR();                                /*�ȶ�ָ��*/
		  Show_Message();
    break;

    case 3:
      Del_FR();                                    /*ɾ��ָ���û�ָ��*/
		  Show_Message();
    break;
			
    case 4:
      Clean_FR();                                  /*���ָ�ƿ�*/
		  Show_Message();
    break;
   
    default:
      /* �������ָ��ָ���ַ�����ӡ��ʾ��Ϣ */
		  AS608_INFO("������Ϸ�ָ�\r\n\r\n");
      Show_Message();
    break;      
  }
  return;		
}


/**
  * @brief  �����ָ��ģ���ͨ��
  * @param  ��
  * @retval ��
  */
void  Connect_Test(void)
{   
  Usart_SendString( DEBUG_USARTx,"����һ��ָ��ģ��ʵ��\r\n");
  AS608_INFO("��ӭʹ��Ұ��STM32������\r\n\r\n");
	
  if(PS_Connect(&AS608_Addr))                      /*��AS608����ͨ��*/
  { 
    AS608_INFO("δ��⵽ָ��ģ�飬�������ӣ�����\r\n");			  
  }
  else
  {
	AS608_INFO("ͨѶ�ɹ�\r\n");
	Show_Message();
  }
}


/**
  * @brief  ¼ָ��
  * @param  ��
  * @retval ��
  */
void  Add_FR(void)
{
  uint16_t  i,j,sure,ID;
 
  i=j=0;
	
  while(1)
  {
    switch (j)
    {
      case 0:                            /*ִ�е�1��*/
        
        i++;
      
        AS608_INFO("******����밴��ָ******\r\n");
        
        sure=PS_GetImage();              /*¼��ͼ��*/      
        if(sure == 0x00)
        {
          sure=PS_GenChar(CHAR_BUFFER1); /*��������1*/
          if(sure==0x00)
          { 
				    AS608_INFO("����ָ��1����������\r\n");
            
            sure=PS_HighSpeedSearch(CHAR_BUFFER1,0,PS_MAXNUM,&ID);
            if(sure==0x00)
            {
             AS608_INFO ("ָ���Ѵ��ڣ�ָ��ID��%d������\r\n\r\n",ID);
             
             return ;
            }              
            else 
            {
              i=0;
              j=1;                      /*��ת����2��*/
             }
           }
				 else
				 {
					 ShowErrMessage(sure);	       
				 }
        }
			  else 
			  {
				  ShowErrMessage(sure);	
			  }
      break;


      case 1:
        
        i++;
      
        AS608_INFO("******������ٰ�һ����ָ******\r\n");
      
        sure=PS_GetImage();
        if(sure==0x00)
        { 
          sure=PS_GenChar(CHAR_BUFFER2); /*��������2*/
          if(sure==0x00)
          { 
				    AS608_INFO("����ָ��2����������\r\n");
            i=0;
            j=2;                         /*��ת����3��*/
          }
				  else 
				  {
					  ShowErrMessage(sure);
				  }

         }
			   else 
			   {
			   ShowErrMessage(sure);
         }				
       break;

			
       case 2:
         
         AS608_INFO("���ڶԱ����������ָ��\r\n");
       
         sure=PS_Match();                /*��ȷ�ȶ���öָ������*/
         if(sure==0x00) 
         {
				   AS608_INFO("����ָ�ƶԱȳɹ�������\r\n");
           
           j=3;                         /*��ת����4��*/
         }
         else
         {
           AS608_INFO("�Ա�ʧ�ܣ�������¼��ָ�ƣ�����\r\n");
           ShowErrMessage(sure);
           i=0;
           j=0;
         }
        
       break;
         
         
       case 3:
         
         AS608_INFO("��������ָ��ģ��\r\n");
       
         sure=PS_RegModel();            /*�ϲ�����������ģ�壩*/
         if(sure==0x00) 	
         {
           AS608_INFO("����ָ��ģ��ɹ�������\r\n");
           j=4;                        /*��ת����5��*/
         } 
         else 
        {
         j =0;
         ShowErrMessage(sure);
        }
        
       break;
        

       case 4:
        
         do
         { 
           AS608_INFO ("******���������洢ID����ΧΪ0��239******\r\n");
        
           ID=GET_NUM();
         }while(!(ID<PS_MAXNUM));
        
         sure=PS_StoreChar(CHAR_BUFFER2,ID);  /*����ģ��*/
         if(sure==0x00) 	
         {
           AS608_INFO("¼��ָ��ģ��ɹ�������\r\n\r\n");
      
           return ;
         } 
         else 
         {
           j =0;
           ShowErrMessage(sure);
         }
       break;
      }
    
      SysTick_Delay_Ms(1000);
      
      if(i==4)                       /*����4��û�а���ָ���˳�*/
      { 
        AS608_INFO("¼ָ��ʧ�ܣ�\r\n\r\n\r\n");

        break;
      }
  }
}


/**
  * @brief  �ȶ�ָ�ƣ�ʶ��ָ�ƣ�
  * @param  ��
  * @retval ��
  */
void Compare_FR(void)  
{  
  uint16_t  ID=0;                    /*��ʼ��IDֵ*/  
  uint16_t  sure;
	
  AS608_INFO("******����Ա�ָ�ƣ��밴��ָ******\r\n");

  sure=PS_GetImage();
  if(sure == 0x00)
  {
    sure=PS_GenChar(CHAR_BUFFER1);   /*��������1*/
    if(sure==0x00)
    {  
			/*��������ָ�ƿ�*/
      sure=PS_HighSpeedSearch(CHAR_BUFFER1,0,PS_MAXNUM,&ID);
      if(sure==0x00)
      {
        AS608_INFO ("�Ա�ָ�Ƴɹ���ָ��ID��%d������\r\n\r\n",ID);
      }
      else 
      {
			ShowErrMessage(sure);
			}
    }
    else 
    {   
      ShowErrMessage(sure);	
    }
  }
	else  
	{
	  ShowErrMessage(sure);
	}
 
}



/**
  * @brief  ���ָ�ƿ�
  * @param  ��
  * @retval ��
  */
void Clean_FR(void) 
{
  uint16_t  sure;
  
	AS608_INFO("���ָ�ƿ�\r\n");
  
	sure=PS_Empty();     
	if(sure == 0x00)
	{
		AS608_INFO("���ָ�ƿ�ɹ�������\r\n\r\n");
	}
	else 
	{
		ShowErrMessage(sure);
	}

}


/**
  * @brief  ɾ��ָ���û�ָ��
  * @param  ��
  * @retval ��
  */
void Del_FR(void)
{ 
	uint32_t  ID;
	uint16_t  sure;
  
	AS608_INFO("ɾ��ָ���û�ID\r\n");
  
	do
	{ 
		AS608_INFO ("******���������洢ID����ΧΪ0��239******\r\n");
		ID=GET_NUM();
	}while(!(ID<PS_MAXNUM));         /*����ID���ܳ���ָ��ģ���������*/
	
	sure=PS_DeletChar(ID,1);         /*ɾ��ָ���û���ָ��ģ��*/
	
	if(sure == 0x00)                
	{
		AS608_INFO("ɾ��ָ���û�ָ�Ƴɹ�������\r\n\r\n");
	}
	else 
	{
		ShowErrMessage(sure);
	}
}


/*********************************************END OF FILE**********************/
