#ifndef _PALETTE_H
#define _PALETTE_H

#include "stm32h7xx.h"
#include "./lcd/bsp_lcd.h"
 

#define COLOR_BLOCK_WIDTH   lcd_param[cur_lcd].m_palette_btn_width
#define COLOR_BLOCK_HEIGHT  lcd_param[cur_lcd].m_palette_btn_height

#define BUTTON_NUM 18
#define PALETTE_START_Y   0
#define PALETTE_END_Y     LCD_PIXEL_HEIGHT

#if 1     //��ť�������
  #define BUTTON_START_X      0
  #define PALETTE_START_X   COLOR_BLOCK_WIDTH*2+1
  #define PALETTE_END_X     LCD_PIXEL_WIDTH

#else     //��ť�����ұߣ�(���ڴ�������ʱҲ���bug�����ڲ��Դ�������߽�)
  #define BUTTON_START_X      LCD_PIXEL_WIDTH-2*COLOR_BLOCK_WIDTH
  #define PALETTE_START_X   0
  #define PALETTE_END_X     LCD_PIXEL_WIDTH-2*COLOR_BLOCK_WIDTH

#endif


typedef struct 
{
  uint16_t start_x;   //������x��ʼ����  
  uint16_t start_y;   //������y��ʼ����
  uint16_t end_x;     //������x�������� 
  uint16_t end_y;     //������y��������
  uint32_t para;      //��ɫ��ť�б�ʾѡ�����ɫ���ʼ���״��ť�б�ʾѡ��Ļ�ˢ
  uint8_t touch_flag; //�������µı�־
    
  void (*draw_btn)(void * btn);     //������溯��
  void (*btn_command)(void * btn);  //��������ִ�к����������л���ɫ����ˢ
 
}Touch_Button;


/*��ˢ��״�б�*/
typedef enum 
{
  LINE_SINGLE_PIXCEL = 0,   //��������
  
  LINE_2_PIXCEL,  //2������
  
  LINE_4_PIXCEL,  //4������
  
  LINE_6_PIXCEL,  //6������
  
  LINE_8_PIXCEL,  //8������
  
  LINE_16_PIXCEL, //16������
  
  LINE_20_PIXCEL, //20������
  
  LINE_WITH_CIRCLE,  //��������
    
  RUBBER,           //��Ƥ

}SHAPE;

/*��ˢ����*/
typedef struct
{
  uint32_t color;
  
  SHAPE  shape;
  
}Brush_Style;

/*brushȫ�ֱ����������ļ���ʹ�õ�*/
extern Brush_Style brush;

  
void Palette_Init(void);
void Touch_Button_Init(void);
void Touch_Button_Down(uint16_t x,uint16_t y);
void Touch_Button_Up(uint16_t x,uint16_t y);
void Draw_Trail(int16_t pre_x,int16_t pre_y,int16_t x,int16_t y,Brush_Style* brush);


#endif //_PALETTE_H

