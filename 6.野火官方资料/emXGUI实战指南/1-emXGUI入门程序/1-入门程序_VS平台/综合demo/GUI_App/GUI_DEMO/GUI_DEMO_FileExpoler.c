
/*
 *CDlg_FileExpoler.c
 *2018��3��5������2:14:42
 *
 */

#include "emXGUI.h"
#include "x_libc.h"
#include <stdio.h>
#include "GUI_DEMO_FileExpoler_Port.h"


/*============================================================================*/
//����ؼ�ID
enum eID
{
	eID_OK =0x1000,
	eID_EXIT,
	eID_PREV,
	eID_NEXT,

	eID_TEXT,
	eID_LIST,

};

enum eMSG //�û��Զ�����Ϣ
{
	eMSG_SCAN =WM_USER+1,
};

/*============================================================================*/

typedef struct	{
	char *pszOutDir;
	const char *pInitDir;
	int exit_code;
}win_ext_data;




////����ָ��Ŀ¼�µ��ļ��������ӵ�Listbox��.
static void scan_file(const char *path,HWND hListWnd)
{	
	int i;
	void *hFind;

	WCHAR wbuf[512];
	char path_temp[512];

	EXPOLER_DATA pNextInfo;
	
	/* ��ӡ�/���� ��*�� ��ͨ���*/
	x_strcpy(path_temp, path);
	x_strcat(path_temp, "/*");

	hFind = FindFirstFile_Port(path_temp, &pNextInfo);

	do
	{
		if (pNextInfo.cFileName[0] == '.')//����.��..  
			continue;

		/* ������� */
		//if (pNextInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY_PORT)
		//{
		//	printf("\r\n <DIR> %s\r\n",pNextInfo.cFileName);
		//}
		//else
		//{
		//	printf("\r\n <FILE> %s\r\n", pNextInfo.cFileName);

		//}

		x_mbstowcs_cp936(wbuf, pNextInfo.cFileName, sizeof(wbuf));	//��Ansi�ַ�ת����GUI��unicode�ַ�.

		////��Listbox������һ��Item���¼�ļ������ļ�����.
		i = SendMessage(hListWnd, LB_GETCOUNT, 0, 0);
		SendMessage(hListWnd, LB_ADDSTRING, i, (LPARAM)wbuf);
		/* ͨ������Item���DATAֵ����¼�ļ�������(�����������ļ��к��ļ�). */
		SendMessage(hListWnd, LB_SETITEMDATA, i, (LPARAM)pNextInfo.dwFileAttributes);

	} while (FindNextFile_Port(hFind, &pNextInfo));
	
	FindClose_Port(hFind);
}
/*============================================================================*/

/*============================================================================*/

static void listbox_owner_draw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	int i,count,cursel,attr;
	WCHAR wbuf[128];

	hwnd =ds->hwnd;
	hdc =ds->hDC;

	//SetBrushColor(hdc,MapRGB(hdc,250,250,250));
	//FillRect(hdc,&ds->rc);

	i=SendMessage(hwnd,LB_GETTOPINDEX,0,0);    //��ȡ��һ����ʾ����.
	count=SendMessage(hwnd,LB_GETCOUNT,0,0);   //��ȡ�б�����Ŀ��.
	cursel=SendMessage(hwnd,LB_GETCURSEL,0,0); //��õ�ǰѡ�е���Ŀ��.

	SendMessage(hwnd,LB_GETITEMRECT,i,(LPARAM)&rc); //���ָ��Item��ľ��β���.
	if(rc.y > ds->rc.y)
	{
		rc.h =rc.y;
		rc.y =ds->rc.y;
		SetBrushColor(hdc,MapRGB(hdc,240,240,250));
		FillRect(hdc,&rc);
	}

	SetPenColor(hdc,MapRGB(hdc,230,235,240));
	while(i < count)
	{
		SendMessage(hwnd,LB_GETITEMRECT,i,(LPARAM)&rc); //���ָ��Item��ľ��β���.

		if(rc.y > ds->rc.h) //�����ͻ�����Item��Ͳ��������.
		{
			break;
		}

		attr =SendMessage(hwnd,LB_GETITEMDATA,i,0); //���Item���DATAֵ(��scan_fileʱ,�ļ����Լ�¼�ڶ�Ӧ��Item���DATAֵ����...).

		if(i==cursel)
		{//�����ǰѡ�е�Item��.
			SetBrushColor(hdc,MapRGB(hdc,200,200,220));
		}
		else
		{
			SetBrushColor(hdc,MapRGB(hdc,240,240,250));
		}
		FillRect(hdc,&rc);

		//���ָ��Item�����������.
		SendMessage(hwnd,LB_GETTEXT,i,(LPARAM)wbuf);

		if(attr&FILE_ATTRIBUTE_DIRECTORY_PORT) //���ļ��У�
		//if (attr) //���ļ��У�
		{//�����ļ���Item��������ɫ
			SetTextColor(hdc,MapRGB(hdc,100,0,200));
		}
		else
		{//�����ļ�Item��������ɫ
			SetTextColor(hdc,MapRGB(hdc,20,20,50));
		}

		DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_LEFT|DT_VCENTER);
		HLine(hdc,rc.x+2,rc.y+rc.h-1,rc.x+rc.w-2*2);

		OffsetRect(&rc,0,rc.h);

		i++;
	}

	////
	if(rc.y < ds->rc.h) //���ʣ��ı�����.
	{
		rc.h =ds->rc.h-rc.y;
		SetBrushColor(hdc,MapRGB(hdc,240,240,250));
		FillRect(hdc,&rc);
	}
}

static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc,rc0;
	WCHAR wbuf[256];
	char buf[256];
	HWND wnd;
	win_ext_data *ex_data;
	////

	ex_data =(win_ext_data*)GetWindowLong(hwnd,GWL_USERDATA);

	switch(msg)
	{

		case WM_CREATE:
		{
			ex_data =(win_ext_data*)lParam;
			SetWindowLong(hwnd,GWL_USERDATA,lParam);

			ex_data->pszOutDir[0] ='\0';
			ex_data->exit_code =FALSE;

			GetClientRect(hwnd,&rc0); //��ô��ڵĿͻ�������

			rc.x =0;
			rc.y =0;
			rc.w =rc0.w;
			rc.h =40;
			CreateWindow(TEXTBOX,L" ",TBS_SUNK|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_TEXT,NULL,NULL);

			OffsetRect(&rc,0,rc.h+2);
			rc.h=rc0.h-rc.y-36;
			CreateWindow(LISTBOX,L"LIST",WS_OWNERDRAW|WS_VISIBLE|WS_BORDER,rc.x,rc.y,rc.w,rc.h,hwnd,eID_LIST,NULL,NULL);

			OffsetRect(&rc,1,rc.h+4);
			rc.w =(rc0.w/4)-4;
			rc.w =MAX(rc.w,50);
			rc.w =MIN(rc.w,100);

			rc.h =32-4;
			CreateWindow(BUTTON,L"Exit",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_EXIT,NULL,NULL);

			OffsetRect(&rc,rc.w+2,0);
			CreateWindow(BUTTON,L"<-",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_PREV,NULL,NULL);

			OffsetRect(&rc,rc.w+2,0);
			CreateWindow(BUTTON,L"->",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_NEXT,NULL,NULL);

			OffsetRect(&rc,rc.w+2,0);
			CreateWindow(BUTTON,L"OK",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_OK,NULL,NULL);

			//wnd =GetDlgItem(hwnd,eID_LIST);
			//SendMessage(wnd,LB_ADDSTRING,0,(LPARAM)L"B:");
			//SendMessage(wnd,LB_SETITEMDATA,0,(LPARAM)FA_DIR);
			x_strcpy(ex_data->pszOutDir,ex_data->pInitDir);
			PostAsyncMessage(hwnd,eMSG_SCAN,0,0); //ִ��һ���ļ�������
			////

		}
		return TRUE;
		////

		case eMSG_SCAN:
		{
			int i;

			wnd=GetDlgItem(hwnd,eID_LIST);

			SendMessage(wnd,LB_RESETCONTENT,0,0);

			scan_file(ex_data->pszOutDir,wnd);
			InvalidateRect(wnd,NULL,TRUE);

			i=SendMessage(wnd,LB_GETCOUNT,0,0);
			if(i > 0)
			{
				x_sprintf(buf,"<%d>:%s",i,ex_data->pszOutDir);
				x_mbstowcs_cp936(wbuf,buf,256);

				SetWindowText(GetDlgItem(hwnd,eID_TEXT),wbuf);
			}

		}
		return TRUE;
		////


		case WM_NOTIFY:
		{
//			u16 code,id;
      u16 id;
			int i;

//			code =HIWORD(wParam);
			id   =LOWORD(wParam);

			if(id==eID_PREV) //������һ��.
			{
				x_strcpy(buf,ex_data->pszOutDir);
				i =x_strlen(buf);
				while(i > 0)
				{
					if(buf[i]=='/' || buf[i]=='\\')
					{
						buf[i] ='\0';
						x_strcpy(ex_data->pszOutDir,buf);
						PostMessage(hwnd,eMSG_SCAN,0,0);
						break;
					}
					buf[i--] ='\0';
				}

			}

			if(id==eID_NEXT) //������һ��.
			{
				wnd=GetDlgItem(hwnd,eID_LIST);

				i =SendMessage(wnd,LB_GETCOUNT,0,0);
				if(i > 0)
				{
					i =SendMessage(wnd,LB_GETCURSEL,0,0);
					if(i >= 0)
					{
						if(SendMessage(wnd,LB_GETITEMDATA,i,0)&FILE_ATTRIBUTE_DIRECTORY_PORT) //���ѡ�е�Item�����ļ���,�����Ŀ¼����������.
						{
							SendMessage(wnd,LB_GETTEXT,i,(LPARAM)wbuf);
							x_wcstombs_cp936(buf,wbuf,256);

							if(x_strlen(ex_data->pszOutDir)>0)
							{
								x_strcat(ex_data->pszOutDir,"/");
							}

							x_strcat(ex_data->pszOutDir,buf);

							PostMessage(hwnd,eMSG_SCAN,0,0);
						}
					}

				}


			}

			if(id==eID_OK)
			{
				wnd=GetDlgItem(hwnd,eID_LIST);

				i =SendMessage(wnd,LB_GETCURSEL,0,0);
				if(i >= 0)
				{
					SendMessage(wnd,LB_GETTEXT,i,(LPARAM)wbuf);
					x_wcstombs_cp936(buf,wbuf,256);

					if(x_strlen(ex_data->pszOutDir)>0)
					{
						x_strcat(ex_data->pszOutDir,"/");
					}
					x_strcat(ex_data->pszOutDir,buf);

					ex_data->exit_code =TRUE;
				}
				PostCloseMessage(hwnd);

			}

			if(id==eID_EXIT)
			{
				PostCloseMessage(hwnd);
			}
		}
		break;
		////

		case WM_DRAWITEM:
		{
			DRAWITEM_HDR *ds;

			ds =(DRAWITEM_HDR*)lParam;
			if(ds->ID==eID_LIST)
			{
				listbox_owner_draw(ds);
				return TRUE;
			}

			return FALSE;
		}
//		break;
		////

		case WM_CLOSE:
		{

			return DefWindowProc(hwnd,msg,wParam,lParam);
		}
//		break;
		////

		default:
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

	}

	return WM_NULL;
}

/*============================================================================*/

BOOL	GUI_DEMO_FileExpoler(HWND hwndParent,int x,int y,int w,int h,const WCHAR *Name,const char *cur_dir,char *out)
{
	HWND hwnd;
	WNDCLASS	wcex;
	MSG msg;
	win_ext_data ex_data;

	/////

	if(hwndParent==NULL)
	{
		hwndParent =GetDesktopWindow();
	}

	ex_data.exit_code =FALSE;
	ex_data.pInitDir =cur_dir;
	ex_data.pszOutDir =(char*)GUI_VMEM_Alloc(1024);

	wcex.Tag 		    = WNDCLASS_TAG;

	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WinProc; //������������Ϣ����Ļص�����.
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;//hInst;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;//LoadCursor(NULL, IDC_ARROW);

#if 1 //��ģ̬�Ի��򴰿ڷ�ʽ����.
	DialogWindowEx(	NULL,
			&wcex,
			Name,
			WS_CAPTION|WS_BORDER|WS_CLIPCHILDREN,
			x,y,w,h,
			hwndParent,NULL,NULL,(LPVOID)&ex_data);
#endif

#if 0//�������ڷ�ʽ����.
	//����������
	hwnd	=CreateWindowEx(	NULL,
								&wcex,
								Name,
								WS_CAPTION|WS_BORDER|WS_CLIPCHILDREN,
								x,y,w,h,
								NULL,NULL,NULL,(LPVOID)&ex_data);

	//��ʾ������
	ShowWindow(hwnd,SW_SHOW);

	//��ʼ������Ϣѭ��(���ڹرղ�����ʱ,GetMessage������FALSE,�˳�����Ϣѭ��)��
	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
#endif

	if(ex_data.exit_code!=FALSE)
	{
		if(out!=NULL)
		{
			x_strcpy(out,ex_data.pszOutDir);
		}
	}
	GUI_VMEM_Free(ex_data.pszOutDir);
	return ex_data.exit_code;
}

/*============================================================================*/
