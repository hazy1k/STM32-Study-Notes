/**
  ************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   ��FreeRTOS�ں�ʵ����Ӧ�ÿ���ʵսָ�ϡ��鼮����
  *           �б����б���
  ************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� i.MX RT ϵ�� ������
  * 
  * ����    :www.embedfire.com  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ************************************************************************
  */
  
/*
*************************************************************************
*                             ������ͷ�ļ�
*************************************************************************
*/
#include "list.h"

/*
*************************************************************************
*                              ȫ�ֱ���
*************************************************************************
*/

/* ����������ڵ� */
struct xLIST       List_Test;

/* ����ڵ� */
struct xLIST_ITEM  List_Item1;
struct xLIST_ITEM  List_Item2;
struct xLIST_ITEM  List_Item3;



/*
************************************************************************
*                                main����
************************************************************************
*/
/*
* ע�����1���ù���ʹ��������棬debug��ѡ�� Ude Simulator
*           2����Targetѡ�����Ѿ���Xtal(Mhz)��ֵ��Ϊ25��Ĭ����12��
*              �ĳ�25��Ϊ�˸�system_ARMCM7.c�ж����__SYSTEM_CLOCK��ͬ��ȷ�������ʱ��ʱ��һ��
*/
int main(void)
{	
	
    /* ������ڵ��ʼ�� */
    vListInitialise( &List_Test );
    
    /* �ڵ�1��ʼ�� */
    vListInitialiseItem( &List_Item1 );
    List_Item1.xItemValue = 1;
    
    /* �ڵ�2��ʼ�� */    
    vListInitialiseItem( &List_Item2 );
    List_Item2.xItemValue = 2;
    
    /* �ڵ�3��ʼ�� */
    vListInitialiseItem( &List_Item3 );
    List_Item3.xItemValue = 3;
    
    /* ���ڵ�������������������� */
    vListInsert( &List_Test, &List_Item2 );    
    vListInsert( &List_Test, &List_Item1 );
    vListInsert( &List_Test, &List_Item3 );    
    
    for(;;)
	{
		/* ɶ�²��� */
	}
}
