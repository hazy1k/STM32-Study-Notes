#include "test.h"
#include "bsp_usart.h"

usart_data_typed test_data;


void do_process(usart_data_typed* udata)
{	
	//�Զ���Խ����ַ��������˴�ֻ�������շ���
	Usart_SendString(DEBUG_USARTx,udata->data);	
	
	
	
	//������һ�����ݼǵ����ñ�־
	udata->flag = 0;
	udata->len = 0;                  
}










