#include "emXGUI.h" 


void GUI_DEMO_Button(void);
void	GUI_DEMO_RectSpeed(void);
void	GUI_DEMO_Hello(void);

void	GUI_DEMO_Button(void);
void	GUI_DEMO_Checkbox(void);
void	GUI_DEMO_Radiobox(void);
void	GUI_DEMO_Textbox(void);
void	GUI_DEMO_Progressbar(void);
void	GUI_DEMO_Scrollbar(void);
void	GUI_DEMO_Listbox(void);
void	GUI_DEMO_Drawbitmap(void);
void	GUI_DEMO_TextOut(void);
void	GUI_DEMO_Messagebox(void);
void	GUI_DEMO_Drive_Panel(void);
void	GUI_DEMO_DrawJPEG(void);
void	GUI_DEMO_DrawPNG(void);
void	GUI_DEMO_DrawJPEG_Extern(void);
void	GUI_DEMO_Drawbitmap_Extern(void);
void	GUI_DEMO_BitBlt_Speed(void);
void	GUI_DEMO_BMP_Speed(void);
void	GUI_DEMO_DrawGIF(void);
void	GUI_DEMO_DrawGIF_Extern(void);
void	GUI_DEMO_CustomWidget(void);
int		number_input_box(int x, int y, int w, int h,
							const WCHAR *pCaption,
							WCHAR *pOut,
							int MaxCount,
							HWND hwndParent);

void	GUI_DEMO_Graphics_Accelerator(void);
void	GUI_DEMO_LogoIconFont(void);
void	GUI_DEMO_ShowWave(void);
void	GUI_DEMO_MoveWindow(void);
void	GUI_DEMO_MultiWindow(void);
void	GUI_DEMO_WatchTime(void);
void	GUI_DEMO_MoveRect(void);
void	GUI_DEMO_MultiWindow2(void);
void	GUI_DEMO_RadiaMenu(void);
void	GUI_DEMO_Rotatebitmap(void);


void GUI_AppMain(void)
{
	static WCHAR keyboard_out[256];
	char f_path[256];

	while (1)
	{
    //GUI_DEMO_Rotatebitmap();
  ////  GUI_DEMO_RadiaMenu();
		////GUI_DEMO_MultiWindow2();
  ////  GUI_DEMO_RotateIcon();
		//GUI_DEMO_WatchTime();
		//GUI_DEMO_MultiWindow();
		//GUI_DEMO_MoveWindow();

		//GUI_DEMO_ShowWave();
		//GUI_DEMO_LogoIconFont();
		//GUI_DEMO_Graphics_Accelerator();

		////scan_file22();
		//GUI_DEMO_FileExpoler(NULL,
		//	20, 20, 400, 300,
		//	L"File Explorer", "G:", f_path); //通过文件对话框选择ttf文件

		//number_input_box(20, 20, 400, 300,
		//	L"Number Keyboard", keyboard_out, 50, NULL); //通过文件对话框选择ttf文件

		//GUI_DEMO_CustomWidget();
		//GUI_DEMO_DrawGIF_Extern();
		//GUI_DEMO_DrawGIF();
		//GUI_DEMO_BMP_Speed();
		//GUI_DEMO_BitBlt_Speed();
		//GUI_DEMO_Drawbitmap_Extern();
		//GUI_DEMO_DrawJPEG_Extern();
		//GUI_DEMO_DrawPNG();
		//GUI_DEMO_DrawJPEG();
		//GUI_DEMO_Drive_Panel();
		
		GUI_DEMO_Hello();

		GUI_DEMO_Button();
		GUI_DEMO_Checkbox();
		GUI_DEMO_Radiobox();
		GUI_DEMO_Textbox();
		GUI_DEMO_Progressbar();
		GUI_DEMO_Scrollbar();
		GUI_DEMO_Listbox();
		GUI_DEMO_Drawbitmap();
		GUI_DEMO_TextOut();
		GUI_DEMO_Messagebox();

	}
	
}
