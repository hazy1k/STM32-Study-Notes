/*
*********************************************************************************************************
*
*	ģ������ : USB HOST ����ģ��,�������洢�豸
*	�ļ����� : usbh_bsp_msc.h
*	��    �� : V1.0
*	˵    �� : Ӧ�ó��������ͷ�ļ�������ʹ�ö�дU�̵Ĺ��ܺ���
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2013-02-01 armfly  ��ʽ����
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _USBH_BSP_USR_H
#define _USBH_BSP_USR_H

#include "./USBAPP/usb_conf.h"
#include "usb_bsp.h"
#include "usb_hcd_int.h"
#include "usbh_core.h"

#include "usbh_msc_scsi.h"
#include "usbh_msc_bot.h"
#include "usbh_msc_core.h"

#include "usbh_usr.h"

//void usbd_OpenMassStorage(void);
//void usbd_CloseMassStorage(void);

extern USB_OTG_CORE_HANDLE          USB_OTG_Core;
extern USBH_HOST                    USB_Host;

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
