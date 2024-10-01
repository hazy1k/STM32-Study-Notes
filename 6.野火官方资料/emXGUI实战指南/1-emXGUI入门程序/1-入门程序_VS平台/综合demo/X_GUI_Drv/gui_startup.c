
#include <string.h>
#include "gui_drv.h"


/*============================================================================*/
extern BOOL	GUI_Arch_Init(void);
extern void GUI_DesktopStartup(void);

static BOOL GUI_LowLevelInit(void);



/**
  * @brief  GUI�ͼ���ĳ�ʼ��,����GUI�ĵ�һ����ʼ������
  * @param  ��
  * @retval TRUE:�ɹ�; FALSE��ʧ��.
  */
static BOOL GUI_LowLevelInit(void)
{

  //GUI Core�ڴ�����ʼ��
  GUI_MEM_Init();

#if(GUI_VMEM_EN)    
   //vmem�ڴ�����ʼ��
	GUI_VMEM_Init();
#endif  
  
  //��ʼ����־�ӿ�
  if(GUI_Log_Init() != TRUE)
  {
  
  }
    
  /* ֱ�ӷ���TRUE��Ϊ�˲����豸��ʼ����������Ӱ��������� */
	return TRUE;
}

/**
  * @brief  ����GUI�����������ڴ桢Һ���������豸�ĳ�ʼ��
  * @param  ��
  * @retval ��
  */
void	GUI_Startup(void)
{
	SURFACE *pSurf=NULL;
	HFONT hFont=NULL;

	if(!GUI_LowLevelInit()) //GUI�ͼ���ʼ��.
	{
    GUI_ERROR("GUI_LowLevelInit Failed.");
		return;
	}

	if(!GUI_Arch_Init())   //GUI�ܹ�������ʼ��.
	{
    GUI_ERROR("GUI_Arch_Init Failed.");
		return;
	}

	if(!X_GUI_Init())	   //GUI�ں˳�ʼ��.
	{
    GUI_ERROR("X_GUI_Init Failed.");
		return;
	}
  
	pSurf =GUI_DisplayInit(); //��ʾ�豸��ʼ��
	if(pSurf==NULL)
	{
    GUI_ERROR("GUI_DisplayInit Failed.");
		return;
	}
	GUI_SetScreenSurface(pSurf); //������ĻSurface����
  
#if(GUI_RES_DEV_EN)  
   //��Դ�豸��ʼ����FLASH��
  if(RES_DevInit() != TRUE)
  {
    GUI_ERROR("RES_DevInit Failed.");
  }
#endif   

#if(GUI_INPUT_DEV_EN)    
  //��ʼ�������豸
  if(GUI_InputInit() != TRUE)
  {
    GUI_ERROR("GUI_InputInit Failed.");
  } 

	GL_CursorInit(pSurf,pSurf->Width>>1,pSurf->Height>>1); //��ʼ�����
#endif
  
	hFont =GUI_Default_FontInit(); //��ʼ��Ĭ�ϵ�����
	if(hFont==NULL)
	{
    GUI_ERROR("GUI_Default_FontInit Failed.");
		return;
	}
	GUI_SetDefFont(hFont);  //����Ĭ�ϵ�����

	GUI_DesktopStartup();	//�������洰��(�ú������᷵��).

}

/********************************END OF FILE****************************/

