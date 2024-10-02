
#include "BSP.h"

#define IIC_SPEED_100KHZ     1
#define IIC_SPEED_200KHZ     2
#define IIC_SPEED_400KHZ     3
#define IIC_SPEED_800KHZ     4

/*=========================================================================================*/
//#define	SCL_GPIO_RCC_Init()		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE)
#define	SCL_GPIO				GPIOF
#define	SCL_PIN					GPIO_Pin_10
#define	SCL_BIT					GPIO_PinSource10

//#define	SDA_GPIO_RCC_Init()		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE)
#define	SDA_GPIO				GPIOF
#define	SDA_PIN					GPIO_Pin_6
#define SDA_BIT					GPIO_PinSource6


//IO��������
#define IIC_SCL(x)		if(x){GPIO_SetBits(GTP_I2C_SCL_GPIO_PORT, GTP_I2C_SCL_PIN);}else{GPIO_ResetBits(GTP_I2C_SCL_GPIO_PORT, GTP_I2C_SCL_PIN);}
#define IIC_SDA(x)		if(x){GPIO_SetBits(GTP_I2C_SDA_GPIO_PORT, GTP_I2C_SDA_PIN);}else{GPIO_ResetBits(GTP_I2C_SDA_GPIO_PORT, GTP_I2C_SDA_PIN);}
#define READ_SDA		  (GPIO_ReadInputDataBit(GTP_I2C_SDA_GPIO_PORT, GTP_I2C_SDA_PIN))  		//��ȡSDA

/*I2C����*/
#define GTP_I2C_SCL_PIN                  GPIO_Pin_10                 
#define GTP_I2C_SCL_GPIO_PORT            GPIOF                     
#define GTP_I2C_SCL_GPIO_CLK             RCC_APB2Periph_GPIOF
#define GTP_I2C_SCL_SOURCE               GPIO_PinSource10

#define GTP_I2C_SDA_PIN                  GPIO_Pin_6                 
#define GTP_I2C_SDA_GPIO_PORT            GPIOF                    
#define GTP_I2C_SDA_GPIO_CLK             RCC_APB2Periph_GPIOF
#define GTP_I2C_SDA_SOURCE               GPIO_PinSource6

/*��λ����*/
#define GTP_RST_GPIO_PORT                GPIOF
#define GTP_RST_GPIO_CLK                 RCC_APB2Periph_GPIOF
#define GTP_RST_GPIO_PIN                 GPIO_Pin_11
/*�ж�����*/
#define GTP_INT_GPIO_PORT                GPIOF
#define GTP_INT_GPIO_CLK                 RCC_APB2Periph_GPIOF
#define GTP_INT_GPIO_PIN                 GPIO_Pin_9

/*=========================================================================================*/

static void SDA_IN(void)
{	//SDA ����ģʽ
//	volatile int i;

//	SDA_GPIO->MODER &= ~(3<<(SDA_BIT*2));
//	for(i=0;i<5;i++);
}

static	void SDA_OUT(void)
{	//SDA ���ģʽ
//	volatile int i;

//	SDA_GPIO->MODER &= ~(3<<(SDA_BIT*2));
//	SDA_GPIO->MODER |=  (1<<(SDA_BIT*2));
//	for(i=0;i<5;i++);
}


//��ʼ��IIC
static BOOL IIC_PortInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;  
  
  /*ʹ�ܴ�����ʹ�õ����ŵ�ʱ��*/
  RCC_APB2PeriphClockCmd(GTP_I2C_SCL_GPIO_CLK|
                       	 GTP_I2C_SDA_GPIO_CLK|
	                       GTP_RST_GPIO_CLK|GTP_INT_GPIO_CLK, 
	                       ENABLE);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    
  /*����SCL���� */   
  GPIO_InitStructure.GPIO_Pin = GTP_I2C_SCL_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GTP_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

  /*����SDA���� */
  GPIO_InitStructure.GPIO_Pin = GTP_I2C_SDA_PIN;
  GPIO_Init(GTP_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);
 
  /*����RST���ţ�������� */   
  GPIO_InitStructure.GPIO_Pin = GTP_RST_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GTP_RST_GPIO_PORT, &GPIO_InitStructure);
  
  /*���� INT���ţ���������������ʼ�� */   
  GPIO_InitStructure.GPIO_Pin = GTP_INT_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//����Ϊ�����������ʼ��
  GPIO_Init(GTP_INT_GPIO_PORT, &GPIO_InitStructure);
	return TRUE;
}


/*=========================================================================================*/
#define		IIC_DELAY_COUNT	220 //for 100KZ
#define		IIC_MASTER

/*=========================================================================================*/

static volatile int speed=IIC_SPEED_100KHZ;

static void iic_delay(void)
{
	volatile int a,b;

	b =IIC_DELAY_COUNT;
	switch(speed)
	{
		case IIC_SPEED_100KHZ:
			b = b>>0;
			break;
		case IIC_SPEED_200KHZ:
			b = b>>1;
			break;
		case IIC_SPEED_400KHZ:
			b = b>>2;
			break;
		case IIC_SPEED_800KHZ:
			b = b>>3;
			break;
	}

	for(a=0;a<b;a++);

}

static void IIC_Stop(void);

static	mutex_obj *mutex_lock=NULL;

//��ʼ��IIC
static BOOL IIC_Init(void)
{
	
	if(IIC_PortInit())
	{
		mutex_lock =SYS_mutex_create(NULL);


		//��һ��ֹͣ�ź�, ��λI2C�����ϵ������豸������ģʽ
		IIC_Stop();
		return TRUE;
	}
	return FALSE;
}

static BOOL IIC_Open(int iic_speed)
{
	SYS_mutex_lock(mutex_lock,5000);
	speed =iic_speed;
	return TRUE;
}

static void IIC_Close(void)
{
	SYS_mutex_unlock(mutex_lock);
}

/*=========================================================================================*/

//����IIC��ʼ�ź�
static void IIC_Start(void)
{//START:when CLK is high,DATA change form high to low

	SDA_OUT();	//sda�����
	IIC_SDA(1);
	IIC_SCL(1);
	iic_delay();
	iic_delay();
 	IIC_SDA(0);
	iic_delay();
	iic_delay();
	IIC_SCL(0);//ǯסI2C���ߣ�׼�����ͻ�������� 
	iic_delay();
	iic_delay();
	iic_delay();
	iic_delay();
}	  

//����IICֹͣ�ź�
static void IIC_Stop(void)
{//STOP:when CLK is high DATA change form low to high

	SDA_OUT();//sda�����
	IIC_SCL(0);
	IIC_SDA(0);
 	iic_delay();
 	iic_delay();
	IIC_SCL(1); 
	iic_delay();
	iic_delay();
	IIC_SDA(1);//����I2C���߽����ź�
	iic_delay();
	iic_delay();
}


//�ȴ�Ӧ���źŵ���
static BOOL IIC_Wait_Ack(void)
{
	u16 timeout=0;

	SDA_IN();      //SDA����Ϊ����  
	IIC_SDA(1);
	iic_delay();
	IIC_SCL(1);    //CPU����SCL = 1, ��ʱ�����᷵��ACKӦ��
	iic_delay();
	while(READ_SDA)
	{
		if(timeout++ > 1000)
		{
			return FALSE;
		}
	}
	IIC_SCL(0);//ʱ�����0
	iic_delay();
	return TRUE;
} 

//����ACKӦ��
static void IIC_Ack(void)
{
	SDA_OUT();
	IIC_SDA(0);   //CPU����SDA = 0
	iic_delay();
	IIC_SCL(1);   //CPU����1��ʱ��
	iic_delay();
	IIC_SCL(0);
	iic_delay();

	IIC_SDA(1);   //CPU�ͷ�SDA����
}

//������ACKӦ��		    
static void IIC_NAck(void)
{
	SDA_OUT();
	IIC_SDA(1);   //CPU����SDA = 1
	iic_delay();
	IIC_SCL(1);   //CPU����1��ʱ��
	iic_delay();
	IIC_SCL(0);
	iic_delay();

}

//IIC����һ���ֽ�
//���شӻ�����Ӧ��
static BOOL IIC_Send_Byte(u8 txd,u8 ack)
{                        
	int i;

	SDA_OUT(); 	    
    for(i=0;i<8;i++)
    {
        IIC_SCL(0);//����ʱ�ӿ�ʼ���ݴ���
        iic_delay();

    	if(txd&0x80)
    	{
    		IIC_SDA(1);
    	}
    	else
    	{
    		IIC_SDA(0);
    	}

        txd <<= 1;
		iic_delay();
		IIC_SCL(1);
		iic_delay();

    }
    IIC_SCL(0);
    IIC_SDA(1); //free SDA
    if(ack == IIC_NoACK)
    {
    	return TRUE;
    }

	if(IIC_Wait_Ack())
	{
		return TRUE;
	}

	return FALSE;
} 	    

//��1���ֽ�.
static u8 IIC_Read_Byte(u8 ack)
{
	u8 i,dat=0;
	SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
		IIC_SCL(1);
		iic_delay();
		dat<<=1;
        if(READ_SDA)
        {
        	dat |= 0x01;
        }
        IIC_SCL(0);
        iic_delay();
    }

    if(ack==IIC_ACK)
    {
    	IIC_Ack(); //����ACK
    }
    else
    {
    	IIC_NAck();//����nACK
    }
    return dat;
}

/*=========================================================================================*/

const struct	i2c_ops I2C_GPD7_GPD3={

	IIC_Init,
	IIC_Open,
	IIC_Close,
	IIC_Start,
	IIC_Stop,
	IIC_Send_Byte,
	IIC_Read_Byte,
};

/*=========================================================================================*/

