/**
  ******************************************************************************
  * @file    gt5xx.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   i2c��������������gt9157оƬ
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����  STM32 F407 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "BSP.h"
#include "gt5xx.h"


#define	IIC_SPEED	IIC_SPEED_400KHZ

static	const struct i2c_ops *pIIC =&I2C_GPD7_GPD3;

#define	GT_CMD_WR 	(0x28)
#define GT_CMD_RD 	(0x29)

#define	GTP_ADDRESS	GT_CMD_WR

#define CTP_RST_GPIO 		GPIOD
#define CTP_RST_GPIO_PIN 	GPIO_Pin_6

#define CTP_INT_GPIO 		GPIOG
#define CTP_INT_GPIO_PIN 	GPIO_Pin_8

#define	LCD_X_LENGTH	LCD_XSIZE
#define	LCD_Y_LENGTH	LCD_YSIZE

static void GTP_PortInit(void)
{
	GPIO_InitTypeDef gpio_init;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); //��GPIOʱ��

	gpio_init.GPIO_Mode  = GPIO_Mode_OUT;
	gpio_init.GPIO_OType = GPIO_OType_PP; 	//PPģʽ��ODģʽ�и��ߵ��ٶ�,��ǿ�����������Ϳ���������...
	gpio_init.GPIO_PuPd  = GPIO_PuPd_UP;	//��������ʹ��
	gpio_init.GPIO_Speed = GPIO_Speed_25MHz;
	gpio_init.GPIO_Pin = CTP_RST_GPIO_PIN ;
	GPIO_Init(CTP_RST_GPIO, &gpio_init);

	//GTP_IRQEnable();
	if(1)
	{
		gpio_init.GPIO_Mode  = GPIO_Mode_OUT;
		gpio_init.GPIO_Pin = CTP_INT_GPIO_PIN ;
		GPIO_Init(CTP_INT_GPIO, &gpio_init);

		////����GT9xx��ַΪ 0x28/0x29
		GPIO_Pin_Set(CTP_RST_GPIO,CTP_RST_GPIO_PIN,0); //RST=0;
		SYS_msleep(50);
		GPIO_Pin_Set(CTP_INT_GPIO,CTP_INT_GPIO_PIN,0); //INT=0;
		SYS_msleep(50);
		GPIO_Pin_Set(CTP_INT_GPIO,CTP_INT_GPIO_PIN,1); //INT=1;
		SYS_msleep(50);
		GPIO_Pin_Set(CTP_RST_GPIO,CTP_RST_GPIO_PIN,1); //RST=1;

		////
		SYS_msleep(50);
	}

	gpio_init.GPIO_Mode  = GPIO_Mode_IN;
	gpio_init.GPIO_Pin = CTP_INT_GPIO_PIN ;
	GPIO_Init(CTP_INT_GPIO, &gpio_init);

}

// 4.5����GT5688��������
static const u8 CTP_CFG_GT5688[] =  {
			0x96,0xE0,0x01,0x56,0x03,0x05,0x35,0x00,0x01,0x00,
			0x00,0x05,0x50,0x3C,0x53,0x11,0x00,0x00,0x22,0x22,
			0x14,0x18,0x1A,0x1D,0x0A,0x04,0x00,0x00,0x00,0x00,
			0x00,0x00,0x53,0x00,0x14,0x00,0x00,0x84,0x00,0x00,
			0x3C,0x19,0x19,0x64,0x1E,0x28,0x88,0x29,0x0A,0x2D,
			0x2F,0x29,0x0C,0x20,0x33,0x60,0x13,0x02,0x24,0x00,
			0x00,0x20,0x3C,0xC0,0x14,0x02,0x00,0x00,0x54,0xAC,
			0x24,0x9C,0x29,0x8C,0x2D,0x80,0x32,0x77,0x37,0x6E,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x50,0x3C,
			0xFF,0xFF,0x07,0x00,0x00,0x00,0x02,0x14,0x14,0x03,
			0x04,0x00,0x21,0x64,0x0A,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x32,0x20,0x50,0x3C,0x3C,0x00,0x00,0x00,0x00,0x00,
			0x0D,0x06,0x0C,0x05,0x0B,0x04,0x0A,0x03,0x09,0x02,
			0xFF,0xFF,0xFF,0xFF,0x00,0x01,0x02,0x03,0x04,0x05,
			0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
			0x10,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x3C,0x00,0x05,0x1E,0x00,0x02,
			0x2A,0x1E,0x19,0x14,0x02,0x00,0x03,0x0A,0x05,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xFF,0xFF,0x86,
			0x22,0x03,0x00,0x00,0x33,0x00,0x0F,0x00,0x00,0x00,
			0x50,0x3C,0x50,0x00,0x00,0x00,0x1A,0x64,0x01

};


//u8 config[GTP_CONFIG_MAX_LENGTH + GTP_ADDR_LENGTH]
//                = {GTP_REG_CONFIG_DATA >> 8, GTP_REG_CONFIG_DATA & 0xff};

static TOUCH_IC touchIC;

static s8 GTP_I2C_Test(void);

static void Delay(__IO int nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}


/**
  * @brief   ��IIC�豸�ж�ȡ����
  * @param
  *		@arg client_addr:�豸��ַ
  *		@arg  buf[0~1]: ��ȡ���ݼĴ�������ʼ��ַ
  *		@arg buf[2~len-1]: �洢���������ݵĻ���buffer
  *		@arg len:    GTP_ADDR_LENGTH + read bytes count���Ĵ�����ַ����+��ȡ�������ֽ�����
  * @retval  i2c_msgs����ṹ��ĸ�����2Ϊ�ɹ�������Ϊʧ��
  */
static s32 GTP_I2C_Read(u8 client_addr, u8 *buf, s32 len)
{
	int i;
	pIIC->Open(IIC_SPEED);

	pIIC->Start();
	if(!pIIC->WriteByte(GT_CMD_WR,IIC_ACK))
	{
	    pIIC->Stop();
	    pIIC->Close();
	    return FALSE;
	}
	pIIC->WriteByte(buf[0],IIC_ACK);
	pIIC->WriteByte(buf[1],IIC_ACK);

	pIIC->Start();
	pIIC->WriteByte(GT_CMD_RD,IIC_ACK);
	for (i = 0; i < len - 1; i++)
	{
		   buf[2+i] = pIIC->ReadByte(IIC_ACK);
	}

	// ���һ������
	buf[2+i] =pIIC->ReadByte(IIC_NoACK);

	pIIC->Stop();
	pIIC->Close();
	return TRUE;

}



/**
  * @brief   ��IIC�豸д������
  * @param
  *		@arg client_addr:�豸��ַ
  *		@arg  buf[0~1]: Ҫд������ݼĴ�������ʼ��ַ
  *		@arg buf[2~len-1]: Ҫд�������
  *		@arg len:    GTP_ADDR_LENGTH + write bytes count���Ĵ�����ַ����+д��������ֽ�����
  * @retval  i2c_msgs����ṹ��ĸ�����1Ϊ�ɹ�������Ϊʧ��
  */
static BOOL GTP_I2C_Write(u8 client_addr,u8 *buf,int len)
{
	int i;

	pIIC->Open(IIC_SPEED);

	pIIC->Start();
    if(!pIIC->WriteByte(GT_CMD_WR,IIC_ACK))
    {
    	pIIC->Stop();

    	pIIC->Close();
    	return FALSE;
    }
    pIIC->WriteByte(buf[0],IIC_ACK);
	pIIC->WriteByte(buf[1],IIC_ACK);

	for (i = 0; i < len; i++)
	{
		pIIC->WriteByte(buf[2+i],IIC_ACK);
	}

	pIIC->Stop();

	pIIC->Close();
	return TRUE;
}


/**
  * @brief   ʹ��IIC��ȡ�ٴ����ݣ������Ƿ�����
  * @param
  *		@arg client:�豸��ַ
  *		@arg  addr: �Ĵ�����ַ
  *		@arg rxbuf: �洢����������
  *		@arg len:    ��ȡ���ֽ���
  * @retval
  * 	@arg FAIL
  * 	@arg SUCCESS
  */
s32 GTP_I2C_Read_dbl_check(u8 client_addr, u16 addr, u8 *rxbuf, int len)
{
    u8 buf[16] = {0};
    u8 confirm_buf[16] = {0};
    u8 retry = 0;
    
    GTP_DEBUG_FUNC();

    while (retry++ < 3)
    {
        memset(buf, 0xAA, 16);
        buf[0] = (u8)(addr >> 8);
        buf[1] = (u8)(addr & 0xFF);
        GTP_I2C_Read(client_addr, buf, len + 2);
        
        memset(confirm_buf, 0xAB, 16);
        confirm_buf[0] = (u8)(addr >> 8);
        confirm_buf[1] = (u8)(addr & 0xFF);
        GTP_I2C_Read(client_addr, confirm_buf, len + 2);

      
        if (!memcmp(buf, confirm_buf, len+2))
        {
            memcpy(rxbuf, confirm_buf+2, len);
            return SUCCESS;
        }
    }    
    GTP_ERROR("I2C read 0x%04X, %d bytes, double check failed!", addr, len);
    return FAIL;
}


/**
  * @brief   �ر�GT91xx�ж�
  * @param ��
  * @retval ��
  */
void GTP_IRQ_Disable(void)
{

    GTP_DEBUG_FUNC();

    I2C_GTP_IRQDisable();
}

/**
  * @brief   ʹ��GT91xx�ж�
  * @param ��
  * @retval ��
  */
void GTP_IRQ_Enable(void)
{
    GTP_DEBUG_FUNC();
     
	 I2C_GTP_IRQEnable();
}

#if 0

/**
  * @brief   ���ڴ���򱨸津����⵽����
  * @param
  *    @arg     id: ����˳��trackID
  *    @arg     x:  ������ x ����
  *    @arg     y:  ������ y ����
  *    @arg     w:  ������ ��С
  * @retval ��
  */
/*���ڼ�¼��������ʱ(����)����һ�δ���λ�ã�����ֵ��ʾ��һ���޴�������*/
static s16 pre_x[GTP_MAX_TOUCH] ={-1,-1,-1,-1,-1};
static s16 pre_y[GTP_MAX_TOUCH] ={-1,-1,-1,-1,-1};

static void GTP_Touch_Down(s32 id,s32 x,s32 y,s32 w)
{
  
	GTP_DEBUG_FUNC();

	/*ȡx��y��ʼֵ������Ļ����ֵ*/
    GTP_DEBUG("ID:%d, X:%d, Y:%d, W:%d", id, x, y, w);

		/************************************/
		/*�ڴ˴�����Լ��Ĵ����㰴��ʱ������̼���*/
		/* (x,y) ��Ϊ���µĴ����� *************/
		/************************************/
	
		/*prex,prey����洢��һ�δ�����λ�ã�idΪ�켣���(��㴥��ʱ�ж�켣)*/
    pre_x[id] = x; pre_y[id] =y;
	
}


/**
  * @brief   ���ڴ���򱨸津���ͷ�
  * @param �ͷŵ��id��
  * @retval ��
  */
static void GTP_Touch_Up( s32 id)
{
		/*****************************************/
		/*�ڴ˴�����Լ��Ĵ������ͷ�ʱ�Ĵ�����̼���*/
		/* pre_x[id],pre_y[id] ��Ϊ���µ��ͷŵ� ****/
		/*******************************************/	
		/***idΪ�켣���(��㴥��ʱ�ж�켣)********/
	
	
    /*�����ͷţ���pre xy ����Ϊ��*/
	  pre_x[id] = -1;
	  pre_y[id] = -1;		
  
    GTP_DEBUG("Touch id[%2d] release!", id);

}


/**
  * @brief   ��������������ѯ�����ڴ����жϵ���
  * @param ��
  * @retval ��
  */
static void Goodix_TS_Work_Func(void)
{
    u8  end_cmd[3] = {GTP_READ_COOR_ADDR >> 8, GTP_READ_COOR_ADDR & 0xFF, 0};
    u8  point_data[2 + 1 + 8 * GTP_MAX_TOUCH + 1]={GTP_READ_COOR_ADDR >> 8, GTP_READ_COOR_ADDR & 0xFF};
    u8  touch_num = 0;
    u8  finger = 0;
    static u16 pre_touch = 0;
    static u8 pre_id[GTP_MAX_TOUCH] = {0};

    u8 client_addr;//=GTP_ADDRESS;
    u8* coor_data = NULL;
    s32 input_x = 0;
    s32 input_y = 0;
    s32 input_w = 0;
    u8 id = 0;
 
    s32 i  = 0;
    s32 ret = -1;

    GTP_DEBUG_FUNC();

    ret = GTP_I2C_Read(client_addr, point_data, 12);//10�ֽڼĴ�����2�ֽڵ�ַ
    if (ret < 0)
    {
        GTP_ERROR("I2C transfer error. errno:%d\n ", ret);

        return;
    }
    
    finger = point_data[GTP_ADDR_LENGTH];//״̬�Ĵ�������

    if (finger == 0x00)		//û�����ݣ��˳�
    {
        return;
    }

    if((finger & 0x80) == 0)//�ж�buffer statusλ
    {
        goto exit_work_func;//����δ������������Ч
    }

    touch_num = finger & 0x0f;//�������
    if (touch_num > GTP_MAX_TOUCH)
    {
        goto exit_work_func;//�������֧�ֵ����������˳�
    }

    if (touch_num > 1)//��ֹһ����
    {
    	//ʹ�ò���ϵͳ�����жϵ��ñ��������˱�������static������
		static u8 buf[8 * GTP_MAX_TOUCH] = {(GTP_READ_COOR_ADDR + 10) >> 8, (GTP_READ_COOR_ADDR + 10) & 0xff};

        ret = GTP_I2C_Read(client_addr, buf, 2 + 8 * (touch_num - 1));
        memcpy(&point_data[12], &buf[2], 8 * (touch_num - 1));			//����������������ݵ�point_data
    }

    
    
    if (pre_touch>touch_num)				//pre_touch>touch_num,��ʾ�еĵ��ͷ���
    {
        for (i = 0; i < pre_touch; i++)						//һ����һ���㴦��
         {
            u8 j;
           for(j=0; j<touch_num; j++)
           {
               coor_data = &point_data[j * 8 + 3];
               id = coor_data[0] & 0x0F;									//track id
              if(pre_id[i] == id)
                break;

              if(j >= touch_num-1)											//������ǰ����id���Ҳ���pre_id[i]����ʾ���ͷ�
              {
                 GTP_Touch_Up( pre_id[i]);
              }
           }
       }
    }


    if (touch_num)
    {
        for (i = 0; i < touch_num; i++)						//һ����һ���㴦��
        {
            coor_data = &point_data[i * 8 + 3];

            id = coor_data[0] & 0x0F;									//track id
            pre_id[i] = id;

            input_x  = coor_data[1] | (coor_data[2] << 8);	//x����
            input_y  = coor_data[3] | (coor_data[4] << 8);	//y����
            input_w  = coor_data[5] | (coor_data[6] << 8);	//size
#if 0
            {
							
									/*����ɨ��ģʽ����X/Y��ʼ����*/
								switch(LCD_SCAN_MODE)
								{
									case 0:case 7:
										input_y  = LCD_Y_LENGTH - input_y;
										break;
									
									case 2:case 3: 
										input_x  = LCD_X_LENGTH - input_x;
										input_y  = LCD_Y_LENGTH - input_y;
										break;
									
									case 1:case 6:
										input_x  = LCD_X_LENGTH - input_x;
										break;	
									
									default:
									break;
								}
								
                GTP_Touch_Down( id, input_x, input_y, input_w);//���ݴ���
            }
#endif
        }
    }
    else if (pre_touch)		//touch_ num=0 ��pre_touch��=0
    {
      for(i=0;i<pre_touch;i++)
      {
          GTP_Touch_Up(pre_id[i]);
      }
    }


    pre_touch = touch_num;


exit_work_func:
    {
        ret = GTP_I2C_Write(client_addr, end_cmd, 3);
        if (ret < 0)
        {
            GTP_INFO("I2C write end_cmd error!");
        }
    }

}
#endif


/**
  * @brief   ������оƬ���¸�λ
  * @param ��
  * @retval ��
  */
 s8 GTP_Reset_Guitar(void)
{
    GTP_DEBUG_FUNC();
#if 1
    I2C_ResetChip();
    return 0;
#else 		//�����λ
    s8 ret = -1;
    s8 retry = 0;
    u8 reset_command[3]={(u8)GTP_REG_COMMAND>>8,(u8)GTP_REG_COMMAND&0xFF,2};

    //д�븴λ����
    while(retry++ < 5)
    {
        ret = GTP_I2C_Write(GTP_ADDRESS, reset_command, 3);
        if (ret > 0)
        {
            GTP_INFO("GTP enter sleep!");

            return ret;
        }

    }
    GTP_ERROR("GTP send sleep cmd failed.");
    return ret;
#endif

}



 /**
   * @brief   ����˯��ģʽ
   * @param ��
   * @retval 1Ϊ�ɹ�������Ϊʧ��
   */
//s8 GTP_Enter_Sleep(void)
//{
//    s8 ret = -1;
//    s8 retry = 0;
//    u8 reset_comment[3] = {(u8)(GTP_REG_COMMENT >> 8), (u8)GTP_REG_COMMENT&0xFF, 5};//5
//
//    GTP_DEBUG_FUNC();
//
//    while(retry++ < 5)
//    {
//        ret = GTP_I2C_Write(GTP_ADDRESS, reset_comment, 3);
//        if (ret > 0)
//        {
//            GTP_INFO("GTP enter sleep!");
//
//            return ret;
//        }
//
//    }
//    GTP_ERROR("GTP send sleep cmd failed.");
//    return ret;
//}


s8 GTP_Send_Command(u8 command)
{
    s8 ret = -1;
    s8 retry = 0;
    u8 command_buf[3] = {(u8)(GTP_REG_COMMAND >> 8), (u8)GTP_REG_COMMAND&0xFF, GTP_COMMAND_READSTATUS};

    GTP_DEBUG_FUNC();

    while(retry++ < 5)
    {
        ret = GTP_I2C_Write(GTP_ADDRESS, command_buf, 3);
        if (ret > 0)
        {
            GTP_INFO("send command success!");

            return ret;
        }

    }
    GTP_ERROR("send command fail!");
    return ret;
}

/**
  * @brief   ���Ѵ�����
  * @param ��
  * @retval 0Ϊ�ɹ�������Ϊʧ��
  */
s8 GTP_WakeUp_Sleep(void)
{
    u8 retry = 0;
    s8 ret = -1;

    GTP_DEBUG_FUNC();

    while(retry++ < 10)
    {
        ret = GTP_I2C_Test();
        if (ret > 0)
        {
            GTP_INFO("GTP wakeup sleep.");
            return ret;
        }
        GTP_Reset_Guitar();
    }

    GTP_ERROR("GTP wakeup sleep failed.");
    return ret;
}

static s32 GTP_Get_Info(void)
{
    u8 opr_buf[10] = {0};
    s32 ret = 0;

    u16 abs_x_max = GTP_MAX_WIDTH;
    u16 abs_y_max = GTP_MAX_HEIGHT;
    u8 int_trigger_type = GTP_INT_TRIGGER;
        
    opr_buf[0] = (u8)((GTP_REG_CONFIG_DATA+1) >> 8);
    opr_buf[1] = (u8)((GTP_REG_CONFIG_DATA+1) & 0xFF);
    
    ret = GTP_I2C_Read(GTP_ADDRESS, opr_buf, 10);
    if (ret < 0)
    {
        return FAIL;
    }
    
    abs_x_max = (opr_buf[3] << 8) + opr_buf[2];
    abs_y_max = (opr_buf[5] << 8) + opr_buf[4];
		GTP_DEBUG("RES");   
		GTP_DEBUG_ARRAY(&opr_buf[0],10);

    opr_buf[0] = (u8)((GTP_REG_CONFIG_DATA+6) >> 8);
    opr_buf[1] = (u8)((GTP_REG_CONFIG_DATA+6) & 0xFF);
    ret = GTP_I2C_Read(GTP_ADDRESS, opr_buf, 3);
    if (ret < 0)
    {
        return FAIL;
    }
    int_trigger_type = opr_buf[2] & 0x03;
    
    GTP_INFO("X_MAX = %d, Y_MAX = %d, TRIGGER = 0x%02x",
            abs_x_max,abs_y_max,int_trigger_type);
    
    return SUCCESS;    
}

/*******************************************************
Function:
    Initialize gtp.
Input:
    ts: goodix private data
Output:
    Executive outcomes.
        0: succeed, otherwise: failed
*******************************************************/
static s32 GTP_Init_Panel(void)
{
    s32 ret = -1;

    s32 i = 0;
    u16 check_sum = 0;
    s32 retry = 0;

    const u8* cfg_info;
    u8 cfg_info_len  ;
	u8* config;

    u8 cfg_num =0 ;		//��Ҫ���õļĴ�������

    GTP_DEBUG_FUNC();
	
//u8 config[GTP_CONFIG_MAX_LENGTH + GTP_ADDR_LENGTH]
//                = {GTP_REG_CONFIG_DATA >> 8, GTP_REG_CONFIG_DATA & 0xff};

	config = (u8 *)vmalloc (GTP_ADDR_LENGTH + GTP_CONFIG_MAX_LENGTH);
	config[0] = GTP_REG_CONFIG_DATA >> 8;
	config[1] = GTP_REG_CONFIG_DATA & 0xff;
	
    //I2C_Touch_Init();

    ret = GTP_I2C_Test();
    if (ret < 0)
    {
        GTP_ERROR("I2C communication ERROR!");
		return ret;
    } 
		
		//��ȡ����IC���ͺ�
    GTP_Read_Version(); 
		
	//����IC���ͺ�ָ��ͬ������
    if(touchIC == GT5688)
	{
		cfg_info =  CTP_CFG_GT5688; //ָ��Ĵ�������
		cfg_info_len = CFG_GROUP_LEN(CTP_CFG_GT5688);//�������ñ�Ĵ�С
	}
		
    memset(&config[GTP_ADDR_LENGTH], 0, GTP_CONFIG_MAX_LENGTH);
    memcpy(&config[GTP_ADDR_LENGTH], cfg_info, cfg_info_len);
		

	cfg_num = cfg_info_len;
		
	GTP_DEBUG("cfg_info_len = %d ",cfg_info_len);
	GTP_DEBUG("cfg_num = %d ",cfg_num);
	GTP_DEBUG_ARRAY(config,6);
		
	/*����LCD��ɨ�跽�����÷ֱ���*/
	config[GTP_ADDR_LENGTH+1] = LCD_X_LENGTH & 0xFF;
	config[GTP_ADDR_LENGTH+2] = LCD_X_LENGTH >> 8;
	config[GTP_ADDR_LENGTH+3] = LCD_Y_LENGTH & 0xFF;
	config[GTP_ADDR_LENGTH+4] = LCD_Y_LENGTH >> 8;

	config[GTP_ADDR_LENGTH+6] &= ~(X2Y_LOC);
#if 0
		/*����ɨ��ģʽ����X2Y����*/
		switch(LCD_SCAN_MODE)
		{
			case 0:case 2:case 4: case 6:
				config[GTP_ADDR_LENGTH+6] &= ~(X2Y_LOC);
				break;
			
			case 1:case 3:case 5: case 7:
				config[GTP_ADDR_LENGTH+6] |= (X2Y_LOC);
				break;		
		}
#endif

    //����Ҫд��checksum�Ĵ�����ֵ
    check_sum = 0;
	for (i = GTP_ADDR_LENGTH; i < (cfg_num+GTP_ADDR_LENGTH -3); i += 2)
	{
		check_sum += (config[i] << 8) + config[i + 1];
	}
		
	check_sum = 0 - check_sum;
	GTP_DEBUG("Config checksum: 0x%04X", check_sum);
	//����checksum
	config[(cfg_num+GTP_ADDR_LENGTH -3)] = (check_sum >> 8) & 0xFF;
	config[(cfg_num+GTP_ADDR_LENGTH -2)] = check_sum & 0xFF;
	config[(cfg_num+GTP_ADDR_LENGTH -1)] = 0x01;
		

    //д��������Ϣ
    for (retry = 0; retry < 5; retry++)
    {
        ret = GTP_I2C_Write(GTP_ADDRESS, config , cfg_num + GTP_ADDR_LENGTH+2);
        if (ret > 0)
        {
            break;
        }
    }
    SYS_msleep(50);				//�ӳٵȴ�оƬ����
		
		
#if 0	//����д������ݣ�����Ƿ�����д��
    //���������������д����Ƿ���ͬ
	{
    	    u16 i;
    	    u8 buf[300];
    	     buf[0] = config[0];
    	     buf[1] =config[1];    //�Ĵ�����ַ

    	    GTP_DEBUG_FUNC();

    	    ret = GTP_I2C_Read(GTP_ADDRESS, buf, sizeof(buf));
			   
					GTP_DEBUG("read ");

					GTP_DEBUG_ARRAY(buf,cfg_num);
		
			    GTP_DEBUG("write ");

					GTP_DEBUG_ARRAY(config,cfg_num);

					//���ԱȰ汾��
    	    for(i=1;i<cfg_num+GTP_ADDR_LENGTH-3;i++)
    	    {

    	    	if(config[i] != buf[i])
    	    	{
    	    		GTP_ERROR("Config fail ! i = %d ",i);
							free(config);
    	    		return -1;
    	    	}
    	    }
    	    if(i==cfg_num+GTP_ADDR_LENGTH-3)
	    		GTP_DEBUG("Config success ! i = %d ",i);
	}
#endif
	

	 /*ʹ���жϣ��������ܼ�ⴥ������*/
	//I2C_GTP_IRQEnable();
    GTP_Get_Info();
		
	vfree(config);
    return 0;
}


/*******************************************************
Function:
    Read chip version.
Input:
    client:  i2c device
    version: buffer to keep ic firmware version
Output:
    read operation return.
        2: succeed, otherwise: failed
*******************************************************/
s32 GTP_Read_Version(void)
{
    s32 ret = -1;
    u8 buf[8] = {GTP_REG_VERSION >> 8, GTP_REG_VERSION & 0xff};    //�Ĵ�����ַ

    GTP_DEBUG_FUNC();

    ret = GTP_I2C_Read(GTP_ADDRESS, buf, sizeof(buf));
    if (ret < 0)
    {
        GTP_ERROR("GTP read version failed");
        return ret;
    }
    if (buf[2] == '5')
    {
        GTP_INFO("IC1 Version: %c%c%c%c_%02x%02x", buf[2], buf[3], buf[4], buf[5], buf[7], buf[6]);
				
				//GT5688оƬ
				if(buf[2] == '5' && buf[3] == '6' && buf[4] == '8'&& buf[5] == '8')
					touchIC = GT5688;
    }        
    else if (buf[5] == 0x00)
    {
        GTP_INFO("IC2 Version: %c%c%c_%02x%02x", buf[2], buf[3], buf[4], buf[7], buf[6]);
				
				//GT911оƬ
				if(buf[2] == '9' && buf[3] == '1' && buf[4] == '1')
					touchIC = GT911;
    }
    else
    {
        GTP_INFO("IC3 Version: %c%c%c%c_%02x%02x", buf[2], buf[3], buf[4], buf[5], buf[7], buf[6]);
				
				//GT9157оƬ
				if(buf[2] == '9' && buf[3] == '1' && buf[4] == '5' && buf[5] == '7')
					touchIC = GT9157;
		}
    return ret;
}

/*******************************************************
Function:
    I2c test Function.
Input:
    client:i2c client.
Output:
    Executive outcomes.
        2: succeed, otherwise failed.
*******************************************************/
static s8 GTP_I2C_Test( void)
{
    u8 test[3] = {GTP_REG_CONFIG_DATA >> 8, GTP_REG_CONFIG_DATA & 0xff};
    u8 retry = 0;
    s8 ret = -1;

    GTP_DEBUG_FUNC();
  
    while(retry++ < 5)
    {
        ret = GTP_I2C_Read(GTP_ADDRESS, test, 3);
        if (ret > 0)
        {
            return ret;
        }
        GTP_ERROR("GTP i2c test failed time %d.",retry);
    }
    return ret;
}

/*============================================================================*/

void GT5688_Init(void)
{
	GTP_PortInit();
	GTP_Init_Panel();
}

int GT5688_Execu(int *x,int *y)
{
	  u8  end_cmd[3] = {GTP_READ_COOR_ADDR >> 8, GTP_READ_COOR_ADDR & 0xFF, 0};
	  u8  point_data[2 + 1 + 8 * GTP_MAX_TOUCH + 1]={GTP_READ_COOR_ADDR >> 8, GTP_READ_COOR_ADDR & 0xFF};
	  u8  touch_num = 0;
	  u8  finger = 0;
	  static u16 pre_touch = 0;
	  static u8 pre_id[GTP_MAX_TOUCH] = {0};

	  u8 client_addr;//=GTP_ADDRESS;
	  u8* coor_data = NULL;
	  s32 input_x = 0;
	  s32 input_y = 0;
	  s32 input_w = 0;
	  u8 id = 0;

	  s32 i  = 0;
	  s32 ret = 0;

	    GTP_DEBUG_FUNC();

	    ret = GTP_I2C_Read(client_addr, point_data, 12);//10�ֽڼĴ�����2�ֽڵ�ַ
	    if (ret < 0)
	    {
	        GTP_ERROR("I2C transfer error. errno:%d\n ", ret);
	        return touch_num;
	    }

	    finger = point_data[GTP_ADDR_LENGTH];//״̬�Ĵ�������

	    if (finger == 0x00)		//û�����ݣ��˳�
	    {
	    	return touch_num;
	    }

	    if((finger & 0x80) == 0)//�ж�buffer statusλ
	    {
	        goto exit;//����δ������������Ч
	    }

	    touch_num = finger & 0x0f;//�������
	    if (touch_num > GTP_MAX_TOUCH)
	    {
	        goto exit;//�������֧�ֵ����������˳�
	    }

	    if (touch_num > 1)//��ֹһ����
	    {
	    	//ʹ�ò���ϵͳ�����жϵ��ñ��������˱�������static������
			static u8 buf[8 * GTP_MAX_TOUCH] = {(GTP_READ_COOR_ADDR + 10) >> 8, (GTP_READ_COOR_ADDR + 10) & 0xff};

	        ret = GTP_I2C_Read(client_addr, buf, 2 + 8 * (touch_num - 1));
	        memcpy(&point_data[12], &buf[2], 8 * (touch_num - 1));			//����������������ݵ�point_data
	    }

	    if (touch_num>0)
	    {
	        for (i = 0; i < touch_num; i++)						//һ����һ���㴦��
	        {
	            coor_data = &point_data[i * 8 + 3];

	            id = coor_data[0] & 0x0F;									//track id
	            pre_id[i] = id;

	            input_x  = coor_data[1] | (coor_data[2] << 8);	//x����
	            input_y  = coor_data[3] | (coor_data[4] << 8);	//y����
	            input_w  = coor_data[5] | (coor_data[6] << 8);	//size

	           *x = input_x;
	           *y = input_y;

	        }
	    }
	    pre_touch = touch_num;

exit:
	    ret = GTP_I2C_Write(client_addr, end_cmd, 3);
	    if (ret==FALSE)
	    {
	        GTP_INFO("I2C write end_cmd error!");
	    }
	    return touch_num;
}


void GT5688_Test(void)
{
	GTP_PortInit();
	GTP_Init_Panel();
	SYS_msleep(100);

	while(1)
	{
		volatile int x,y,n;
		n=GT5688_Execu(&x,&y);
		SYS_msleep(50);
	}

}


/*============================================================================*/

#include "GUI_Drv.h"

static BOOL ts_down=FALSE;
static int ts_x,ts_y;


static BOOL GTxx_Init(void)
{
	ts_down=FALSE;
	ts_x=0;
	ts_y=0;

	GT5688_Init();

	return TRUE;
}


static BOOL GetState(void)
{
	int x,y;

	if(GT5688_Execu(&x,&y) > 0)
	{
		x =480-x;

		//if((x<800) && (y<480))
		{
			ts_x =x;
			ts_y =y;
			ts_down =TRUE;

		}
		return TRUE;
	}
	else
	{
		ts_down =FALSE;
		return FALSE;
	}
}


static BOOL GetSample(POINT *pt)
{
	if(ts_down==TRUE)
	{
		pt->x =ts_x;
		pt->y =ts_y;

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

static BOOL GetPoint(POINT *pt)
{
	if(GetSample(pt))
	{
#if	GUI_TOUCHSCREEN_CALIBRATE
		TouchPanel_TranslatePoint(LCD_XSIZE,LCD_YSIZE,pt);
#endif
		return TRUE;
	}
	return	TRUE;
}



static BOOL LoadCfg(TS_CFG_DATA *cfg)
{

	return TRUE;
}

static BOOL SaveCfg(TS_CFG_DATA *cfg)
{
	return TRUE;
}

/*============================================================================*/

const GUI_TOUCH_DEV TouchDev_GT5688=
{
	.Init 		=GTxx_Init,
	.GetState 	=GetState,
	.GetSample	=GetSample,
	.GetPoint	=GetPoint,
	.LoadCfg	=LoadCfg,
	.SaveCfg	=SaveCfg,
};

/*============================================================================*/


