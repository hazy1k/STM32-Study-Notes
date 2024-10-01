#include <emXGUI.h>
#include <string.h>
#include "Widget.h"
#include "bsp_ov7725.h"
#include "x_libc.h"


extern void Write_Pixel_From_Buf(uint16_t sx,uint16_t sy,uint16_t width,
	uint16_t height);


TaskHandle_t h_autofocus;
BOOL update_flag = 0;//֡�ʸ��±�־
uint8_t fps=0;//֡��
HWND Cam_hwnd;//�����ھ��
static SURFACE *pSurf;
GUI_SEM *cam_sem = NULL;//����ͼ��ͬ���ź�������ֵ�ͣ�
//uint16_t *cam_buff;
static uint8_t OV7725_State = 0;
/*
 * @brief  ��һ��֡ͼ��
 * @param  ͼ��Ļ�����
 * @retval NONE
*/
void OV7725_Read_Frame(uint16_t *p)
{
  /* �����ٽ�Σ��ٽ�ο���Ƕ�� */
//  taskENTER_CRITICAL();
  
  for(int i = 0; i < 240*320; i++)
  {
    READ_FIFO_PIXEL(*p);		/* ��FIFO����һ�� rgb565 ���ص� p ���� */
    p++;
  }
  
//  taskEXIT_CRITICAL();
}

/*
 * @brief  ������Ļ
 * @param  NONE
 * @retval NONE
*/
static void Update_Dialog()
{
  /* ov7725 ���ź��߳�ʼ�� */
  Ov7725_vsync = 0;
  VSYNC_Init();
  
  
	while(1) //�߳��Ѵ�����
	{
    GUI_SemWait(cam_sem, 0xFFFFFFFF);

    FIFO_PREPARE;  			/*FIFO׼��*/

		;    // ��һ֡ͼ��
		Write_Pixel_From_Buf(0,0,320,240);

    fps ++;                         // ֡���Լ�
  Ov7725_vsync = 0;
//    InvalidateRect(Cam_hwnd,NULL,FALSE);
    OV7725_State = 2;
	}
}
extern int MessageDialogBox(HWND Phwnd, RECT rc,const WCHAR *pText,const WCHAR *pCaption,const MSGBOX_OPTIONS *ops);
/*
 * @brief  ����ͷ���ڻص�����
*/
static LRESULT WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  static int old_fps = 0;

  switch(msg)
  {
    case WM_CREATE:
    {
		  cam_sem = GUI_SemCreate(0,1);//ͬ������ͷͼ��
      /* ov7725 gpio ��ʼ�� */
      Ov7725_GPIO_Config();
      OV7725_State = 1;
      
      if(Ov7725_Init())
      {
        GUI_DEBUG("OV7725 ��ʼ���ɹ�");
        OV7725_State = 0;
        
      //�����Զ��Խ��߳�
      xTaskCreate((TaskFunction_t )(void(*)(void*))Update_Dialog,  /* ������ں��� */
                            (const char*    )"Update_Dialog",/* �������� */
                            (uint16_t       )1024/4,  /* ����ջ��СFreeRTOS������ջ����Ϊ��λ */
                            (void*          )NULL,/* ������ں������� */
                            (UBaseType_t    )5, /* ��������ȼ� */
                            (TaskHandle_t  )&h_autofocus);/* ������ƿ�ָ�� */
      }
      else
      {
//        RECT RC;
//        MSGBOX_OPTIONS ops;
//        //const WCHAR *btn[]={L"ȷ��"};
//        
//        const WCHAR *btn[] ={L"OK", L"������"};      //�Ի����ڰ�ť������

//        ops.Flag =MB_ICONERROR;
//        ops.pButtonText =btn;
//        ops.ButtonCount =2;
//        RC.w =200;
//        RC.h =120;
//        RC.x =(GUI_XSIZE - RC.w) >> 1;
//        RC.y =(GUI_YSIZE - RC.h) >> 1;
//        MessageDialogBox(hwnd, RC, L"û�м�⵽OV7725����ͷ��\n�����¼�����ӡ�", L"����", &ops); 
        OV7725_State = 1;     // û�м�⵽����ͷ
        SetTimer(hwnd,1,1,TMR_START|TMR_SINGLE,NULL);  
//        PostCloseMessage(hwnd);
        break;
      }
      break;  
    }
    case WM_LBUTTONDOWN://�����Ļ���رմ���
    {
      PostCloseMessage(hwnd);
      break;
    }
    case WM_TIMER://����ͷ״̬��
    {
      update_flag = 1;
      if(OV7725_State == 1)
      {
        RECT RC;
        MSGBOX_OPTIONS ops;        
        const WCHAR *btn[] ={L"ȷ��",L"ȡ��"};      //�Ի����ڰ�ť������

        ops.Flag =MB_ICONERROR;
        ops.pButtonText =btn;
        ops.ButtonCount =2;
        RC.w = 200;
        RC.h = 120;
        RC.x = (GUI_XSIZE - RC.w) >> 1;
        RC.y = (GUI_YSIZE - RC.h) >> 1;
        GUI_DEBUG(" �����˵�%d����ť", MessageDialogBox(hwnd, RC, L"û�м�⵽OV7725����ͷ��\n�����¼�����ӡ�", L"����", &ops));
        OV7725_State = 1;     // û�м�⵽����ͷ
        PostCloseMessage(hwnd);
      }
      break;
    }
    case WM_PAINT:
    {
      PAINTSTRUCT ps;
      HDC hdc;
      WCHAR wbuf[20];
      RECT rc;
      
      hdc = BeginPaint(hwnd,&ps);
      GetClientRect(hwnd,&rc);
      if(OV7725_State != 2)
      {
        SetTextColor(hdc,MapRGB(hdc,250,250,250));
        SetBrushColor(hdc,MapRGB(hdc,50,0,0));
        SetPenColor(hdc,MapRGB(hdc,250,0,0));
//        SetTimer(hwnd,1,20,TMR_SINGLE,NULL);  
        DrawText(hdc,L"���ڳ�ʼ������ͷ\r\n\n��ȴ�...",-1,&rc,DT_VCENTER|DT_CENTER|DT_BKGND);
      }              
      if(OV7725_State == 2)
      {   
        
      }
#if 0
      // ���´��ڷֱ���
      if(update_flag)
      {
        update_flag = 0;
        old_fps = fps;
        fps = 0;
      }
      
      rc.x = 0;
      rc.y = 0;
      rc.w = 70;
      rc.h = 15;
      
      x_wsprintf(wbuf,L"֡��:%dFPS",old_fps);
      DrawText(hdc,wbuf,-1,&rc,DT_VCENTER|DT_CENTER);
#endif
      EndPaint(hwnd,&ps);
      Ov7725_vsync = 0;    // ��ʼ��һ֡ͼ��Ĳɼ�
      break;
    }

    case WM_DESTROY:
    {
      old_fps = 0;
      fps = 0;
      
      if (OV7725_State != 1)
      {
        GUI_SemDelete(cam_sem);
        GUI_Thread_Delete(h_autofocus);
      }
			
      discameraexit();             // �ر��ж�
      DisableCamera();
      return PostQuitMessage(hwnd);	
    }
    default:
      return DefWindowProc(hwnd, msg, wParam, lParam);
  }
  return WM_NULL;
}


void	GUI_Camera_DIALOG(void)
{	
	WNDCLASS	wcex;
	MSG msg;

	wcex.Tag = WNDCLASS_TAG;  
  
	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WinProc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);

	//����������
	Cam_hwnd = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
                            &wcex,
                            L"GUI_Camera_Dialog",
                            WS_VISIBLE|WS_CLIPCHILDREN|WS_OVERLAPPED,
                            0, 0, GUI_XSIZE, GUI_YSIZE,
                            NULL, NULL, NULL, NULL);

	//��ʾ������
	ShowWindow(Cam_hwnd, SW_SHOW);

	//��ʼ������Ϣѭ��(���ڹرղ�����ʱ,GetMessage������FALSE,�˳�����Ϣѭ��)��
	while (GetMessage(&msg, Cam_hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
  }
}
