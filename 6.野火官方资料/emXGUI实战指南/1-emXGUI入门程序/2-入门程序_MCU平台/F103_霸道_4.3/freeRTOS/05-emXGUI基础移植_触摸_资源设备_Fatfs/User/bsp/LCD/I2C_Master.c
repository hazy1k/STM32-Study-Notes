
//#ifdef	IIC_MASTER
///*=========================================================================================*/

///*=========================================================================================*/
//static volatile int speed=IIC_SPEED_100KHZ;

//static void iic_delay(void)
//{
//	volatile int a,b;

//	b =IIC_DELAY_COUNT;
//	switch(speed)
//	{
//		case IIC_SPEED_100KHZ:
//			b = b>>0;
//			break;
//		case IIC_SPEED_200KHZ:
//			b = b>>1;
//			break;
//		case IIC_SPEED_400KHZ:
//			b = b>>2;
//			break;
//		case IIC_SPEED_800KHZ:
//			b = b>>3;
//			break;
//	}

//	for(a=0;a<b;a++);

//}

//static void IIC_Stop(void);

//static	mutex_obj *mutex_lock=NULL;

////��ʼ��IIC
//static BOOL IIC_Init(void)
//{
//	
//	if(IIC_PortInit())
//	{
//		mutex_lock =SYS_mutex_create(NULL);


//		//��һ��ֹͣ�ź�, ��λI2C�����ϵ������豸������ģʽ
//		IIC_Stop();
//		return TRUE;
//	}
//	return FALSE;
//}

//static BOOL IIC_Open(int iic_speed)
//{
//	SYS_mutex_lock(mutex_lock,5000);
//	speed =iic_speed;
//	return TRUE;
//}

//static void IIC_Close(void)
//{
//	SYS_mutex_unlock(mutex_lock);
//}

///*=========================================================================================*/

////����IIC��ʼ�ź�
//static void IIC_Start(void)
//{//START:when CLK is high,DATA change form high to low

//	SDA_OUT();	//sda�����
//	IIC_SDA(1);
//	IIC_SCL(1);
//	iic_delay();
//	iic_delay();
// 	IIC_SDA(0);
//	iic_delay();
//	iic_delay();
//	IIC_SCL(0);//ǯסI2C���ߣ�׼�����ͻ�������� 
//	iic_delay();
//	iic_delay();
//	iic_delay();
//	iic_delay();
//}	  

////����IICֹͣ�ź�
//static void IIC_Stop(void)
//{//STOP:when CLK is high DATA change form low to high

//	SDA_OUT();//sda�����
//	IIC_SCL(0);
//	IIC_SDA(0);
// 	iic_delay();
// 	iic_delay();
//	IIC_SCL(1); 
//	iic_delay();
//	iic_delay();
//	IIC_SDA(1);//����I2C���߽����ź�
//	iic_delay();
//	iic_delay();
//}


////�ȴ�Ӧ���źŵ���
//static BOOL IIC_Wait_Ack(void)
//{
//	u16 timeout=0;

//	SDA_IN();      //SDA����Ϊ����  
//	IIC_SDA(1);
//	iic_delay();
//	IIC_SCL(1);    //CPU����SCL = 1, ��ʱ�����᷵��ACKӦ��
//	iic_delay();
//	while(READ_SDA)
//	{
//		if(timeout++ > 1000)
//		{
//			return FALSE;
//		}
//	}
//	IIC_SCL(0);//ʱ�����0
//	iic_delay();
//	return TRUE;
//} 

////����ACKӦ��
//static void IIC_Ack(void)
//{
//	SDA_OUT();
//	IIC_SDA(0);   //CPU����SDA = 0
//	iic_delay();
//	IIC_SCL(1);   //CPU����1��ʱ��
//	iic_delay();
//	IIC_SCL(0);
//	iic_delay();

//	IIC_SDA(1);   //CPU�ͷ�SDA����
//}

////������ACKӦ��		    
//static void IIC_NAck(void)
//{
//	SDA_OUT();
//	IIC_SDA(1);   //CPU����SDA = 1
//	iic_delay();
//	IIC_SCL(1);   //CPU����1��ʱ��
//	iic_delay();
//	IIC_SCL(0);
//	iic_delay();

//}

////IIC����һ���ֽ�
////���شӻ�����Ӧ��
//static BOOL IIC_Send_Byte(u8 txd,u8 ack)
//{                        
//	int i;

//	SDA_OUT(); 	    
//    for(i=0;i<8;i++)
//    {
//        IIC_SCL(0);//����ʱ�ӿ�ʼ���ݴ���
//        iic_delay();

//    	if(txd&0x80)
//    	{
//    		IIC_SDA(1);
//    	}
//    	else
//    	{
//    		IIC_SDA(0);
//    	}

//        txd <<= 1;
//		iic_delay();
//		IIC_SCL(1);
//		iic_delay();

//    }
//    IIC_SCL(0);
//    IIC_SDA(1); //free SDA
//    if(ack == IIC_NoACK)
//    {
//    	return TRUE;
//    }

//	if(IIC_Wait_Ack())
//	{
//		return TRUE;
//	}

//	return FALSE;
//} 	    

////��1���ֽ�.
//static u8 IIC_Read_Byte(u8 ack)
//{
//	u8 i,dat=0;
//	SDA_IN();//SDA����Ϊ����
//    for(i=0;i<8;i++ )
//	{
//		IIC_SCL(1);
//		iic_delay();
//		dat<<=1;
//        if(READ_SDA)
//        {
//        	dat |= 0x01;
//        }
//        IIC_SCL(0);
//        iic_delay();
//    }

//    if(ack==IIC_ACK)
//    {
//    	IIC_Ack(); //����ACK
//    }
//    else
//    {
//    	IIC_NAck();//����nACK
//    }
//    return dat;
//}

///*=========================================================================================*/

//#endif /* IIC_MASTER */


