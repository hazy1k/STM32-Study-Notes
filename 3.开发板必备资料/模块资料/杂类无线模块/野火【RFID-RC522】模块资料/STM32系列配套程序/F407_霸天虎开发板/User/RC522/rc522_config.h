#ifndef __RC522_CONFIG_H
#define	__RC522_CONFIG_H



/////////////////////////////////////////////////////////////////////
//MF522������
/////////////////////////////////////////////////////////////////////
#define PCD_IDLE              0x00               //ȡ����ǰ����
#define PCD_AUTHENT           0x0E               //��֤��Կ
#define PCD_RECEIVE           0x08               //��������
#define PCD_TRANSMIT          0x04               //��������
#define PCD_TRANSCEIVE        0x0C               //���Ͳ���������
#define PCD_RESETPHASE        0x0F               //��λ
#define PCD_CALCCRC           0x03               //CRC����

/////////////////////////////////////////////////////////////////////
//Mifare_One��Ƭ������
/////////////////////////////////////////////////////////////////////
#define PICC_REQIDL           0x26               //Ѱ��������δ��������״̬
#define PICC_REQALL           0x52               //Ѱ��������ȫ����
#define PICC_ANTICOLL1        0x93               //����ײ
#define PICC_ANTICOLL2        0x95               //����ײ
#define PICC_AUTHENT1A        0x60               //��֤A��Կ
#define PICC_AUTHENT1B        0x61               //��֤B��Կ
#define PICC_READ             0x30               //����
#define PICC_WRITE            0xA0               //д��
#define PICC_DECREMENT        0xC0               //�ۿ�
#define PICC_INCREMENT        0xC1               //��ֵ
#define PICC_RESTORE          0xC2               //�������ݵ�������
#define PICC_TRANSFER         0xB0               //���滺����������
#define PICC_HALT             0x50               //����

/////////////////////////////////////////////////////////////////////
//MF522 FIFO���ȶ���
/////////////////////////////////////////////////////////////////////
#define DEF_FIFO_LENGTH       64                 //FIFO size=64byte
#define MAXRLEN  18

/////////////////////////////////////////////////////////////////////
//MF522�Ĵ�������
/////////////////////////////////////////////////////////////////////
// PAGE 0
#define     RFU00                 0x00    
#define     CommandReg            0x01    
#define     ComIEnReg             0x02    
#define     DivlEnReg             0x03    
#define     ComIrqReg             0x04    
#define     DivIrqReg             0x05
#define     ErrorReg              0x06    
#define     Status1Reg            0x07    
#define     Status2Reg            0x08    
#define     FIFODataReg           0x09
#define     FIFOLevelReg          0x0A
#define     WaterLevelReg         0x0B
#define     ControlReg            0x0C
#define     BitFramingReg         0x0D
#define     CollReg               0x0E
#define     RFU0F                 0x0F
// PAGE 1     
#define     RFU10                 0x10
#define     ModeReg               0x11
#define     TxModeReg             0x12
#define     RxModeReg             0x13
#define     TxControlReg          0x14
#define     TxAutoReg             0x15
#define     TxSelReg              0x16
#define     RxSelReg              0x17
#define     RxThresholdReg        0x18
#define     DemodReg              0x19
#define     RFU1A                 0x1A
#define     RFU1B                 0x1B
#define     MifareReg             0x1C
#define     RFU1D                 0x1D
#define     RFU1E                 0x1E
#define     SerialSpeedReg        0x1F
// PAGE 2    
#define     RFU20                 0x20  
#define     CRCResultRegM         0x21
#define     CRCResultRegL         0x22
#define     RFU23                 0x23
#define     ModWidthReg           0x24
#define     RFU25                 0x25
#define     RFCfgReg              0x26
#define     GsNReg                0x27
#define     CWGsCfgReg            0x28
#define     ModGsCfgReg           0x29
#define     TModeReg              0x2A
#define     TPrescalerReg         0x2B
#define     TReloadRegH           0x2C
#define     TReloadRegL           0x2D
#define     TCounterValueRegH     0x2E
#define     TCounterValueRegL     0x2F
// PAGE 3      
#define     RFU30                 0x30
#define     TestSel1Reg           0x31
#define     TestSel2Reg           0x32
#define     TestPinEnReg          0x33
#define     TestPinValueReg       0x34
#define     TestBusReg            0x35
#define     AutoTestReg           0x36
#define     VersionReg            0x37
#define     AnalogTestReg         0x38
#define     TestDAC1Reg           0x39  
#define     TestDAC2Reg           0x3A   
#define     TestADCReg            0x3B   
#define     RFU3C                 0x3C   
#define     RFU3D                 0x3D   
#define     RFU3E                 0x3E   
#define     RFU3F		  		        0x3F

/////////////////////////////////////////////////////////////////////
//��MF522ͨѶʱ���صĴ������
/////////////////////////////////////////////////////////////////////
#define 	MI_OK                 0x26
#define 	MI_NOTAGERR           0xcc
#define 	MI_ERR                0xbb

/*********************************** RC522 ���Ŷ��� *********************************************/
#define               RC522_GPIO_CS_CLK_FUN                  RCC_AHB1PeriphClockCmd
#define               RC522_GPIO_CS_CLK                      RCC_AHB1Periph_GPIOB
#define               RC522_GPIO_CS_PORT    	               GPIOB			   
#define               RC522_GPIO_CS_PIN		                   GPIO_Pin_12
#define               RC522_GPIO_CS_Mode		                 GPIO_Mode_OUT

#define               RC522_GPIO_SCK_CLK_FUN                 RCC_AHB1PeriphClockCmd
#define               RC522_GPIO_SCK_CLK                     RCC_AHB1Periph_GPIOB
#define               RC522_GPIO_SCK_PORT    	               GPIOB			   
#define               RC522_GPIO_SCK_PIN		                 GPIO_Pin_3
#define               RC522_GPIO_SCK_Mode		                 GPIO_Mode_OUT

#define               RC522_GPIO_MOSI_CLK_FUN                RCC_AHB1PeriphClockCmd
#define               RC522_GPIO_MOSI_CLK                    RCC_AHB1Periph_GPIOB
#define               RC522_GPIO_MOSI_PORT    	             GPIOB			   
#define               RC522_GPIO_MOSI_PIN		                 GPIO_Pin_5
#define               RC522_GPIO_MOSI_Mode		               GPIO_Mode_OUT

#define               RC522_GPIO_MISO_CLK_FUN                RCC_AHB1PeriphClockCmd
#define               RC522_GPIO_MISO_CLK                    RCC_AHB1Periph_GPIOB
#define               RC522_GPIO_MISO_PORT    	             GPIOB			   
#define               RC522_GPIO_MISO_PIN		                 GPIO_Pin_4
#define               RC522_GPIO_MISO_Mode		               GPIO_Mode_IN

#define               RC522_GPIO_RST_CLK_FUN                 RCC_AHB1PeriphClockCmd
#define               RC522_GPIO_RST_CLK                     RCC_AHB1Periph_GPIOB
#define               RC522_GPIO_RST_PORT    	               GPIOB		   
#define               RC522_GPIO_RST_PIN		                 GPIO_Pin_8
#define               RC522_GPIO_RST_Mode		                 GPIO_Mode_OUT


/*********************************** RC522 �����궨��*********************************************/
#define          RC522_CS_Enable()         GPIO_ResetBits ( RC522_GPIO_CS_PORT, RC522_GPIO_CS_PIN )
#define          RC522_CS_Disable()        GPIO_SetBits ( RC522_GPIO_CS_PORT, RC522_GPIO_CS_PIN )

#define          RC522_Reset_Enable()      GPIO_ResetBits( RC522_GPIO_RST_PORT, RC522_GPIO_RST_PIN )
#define          RC522_Reset_Disable()     GPIO_SetBits ( RC522_GPIO_RST_PORT, RC522_GPIO_RST_PIN )

#define          RC522_SCK_0()             GPIO_ResetBits( RC522_GPIO_SCK_PORT, RC522_GPIO_SCK_PIN )
#define          RC522_SCK_1()             GPIO_SetBits ( RC522_GPIO_SCK_PORT, RC522_GPIO_SCK_PIN )

#define          RC522_MOSI_0()            GPIO_ResetBits( RC522_GPIO_MOSI_PORT, RC522_GPIO_MOSI_PIN )
#define          RC522_MOSI_1()            GPIO_SetBits ( RC522_GPIO_MOSI_PORT, RC522_GPIO_MOSI_PIN )

#define          RC522_MISO_GET()          GPIO_ReadInputDataBit ( RC522_GPIO_MISO_PORT, RC522_GPIO_MISO_PIN )



/*********************************** ���� *********************************************/
void             RC522_Init                   ( void );



#endif /* __RC522_CONFIG_H */

