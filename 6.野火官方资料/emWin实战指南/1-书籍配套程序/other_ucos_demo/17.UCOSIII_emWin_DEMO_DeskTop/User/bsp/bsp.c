#include "includes.h"

FATFS fs;	/* Work area (file system object) for logical drives */
extern 	uint8_t IsCal;

/*
*********************************************************************************************************
*                                             REGISTERS
*********************************************************************************************************
*/

#define  DWT_CR      *(CPU_REG32 *)0xE0001000
#define  DWT_CYCCNT  *(CPU_REG32 *)0xE0001004
#define  DEM_CR      *(CPU_REG32 *)0xE000EDFC
#define  DBGMCU_CR   *(CPU_REG32 *)0xE0042004


/*
*********************************************************************************************************
*                                            REGISTER BITS
*********************************************************************************************************
*/

#define  DBGMCU_CR_TRACE_IOEN_MASK       0x10
#define  DBGMCU_CR_TRACE_MODE_ASYNC      0x00
#define  DBGMCU_CR_TRACE_MODE_SYNC_01    0x40
#define  DBGMCU_CR_TRACE_MODE_SYNC_02    0x80
#define  DBGMCU_CR_TRACE_MODE_SYNC_04    0xC0
#define  DBGMCU_CR_TRACE_MODE_MASK       0xC0

#define  DEM_CR_TRCENA                   (1 << 24)

#define  DWT_CR_CYCCNTENA                (1 <<  0)

/*
*********************************************************************************************************
*                                            BSP_CPU_ClkFreq()
*
* Description : Read CPU registers to determine the CPU clock frequency of the chip.
*
* Argument(s) : none.
*
* Return(s)   : The CPU clock frequency, in Hz.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT32U  BSP_CPU_ClkFreq (void)
{
    RCC_ClocksTypeDef  rcc_clocks;


    RCC_GetClocksFreq(&rcc_clocks);
    return ((CPU_INT32U)rcc_clocks.HCLK_Frequency);
}

/*
*********************************************************************************************************
*                                            BSP_Tick_Init()
*
* Description : Initialize all the peripherals that required OS Tick services (OS initialized)
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/
void BSP_Tick_Init(void)
{
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    
    cpu_clk_freq = BSP_CPU_ClkFreq();                           /* Determine SysTick reference freq.                    */
    
#if (OS_VERSION >= 30000u)
    cnts  = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;       /* Determine nbr SysTick increments.                    */
#else
    cnts  = cpu_clk_freq / (CPU_INT32U)OS_TICKS_PER_SEC;        /* Determine nbr SysTick increments.                    */
#endif
    
    OS_CPU_SysTickInit(cnts);                                 /* ����Ĭ�ϵ���������ȼ�������ʵ������޸�             */
//	SysTick_Config(cnts);   //����Ĭ�ϵ���������ȼ�
}

/*
*********************************************************************************************************
*                                          CPU_TS_TmrInit()
*
* Description : Initialize & start CPU timestamp timer.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : CPU_TS_Init().
*
*               This function is an INTERNAL CPU module function & MUST be implemented by application/
*               BSP function(s) [see Note #1] but MUST NOT be called by application function(s).
*
* Note(s)     : (1) CPU_TS_TmrInit() is an application/BSP function that MUST be defined by the developer
*                   if either of the following CPU features is enabled :
*
*                   (a) CPU timestamps
*                   (b) CPU interrupts disabled time measurements
*
*                   See 'cpu_cfg.h  CPU TIMESTAMP CONFIGURATION  Note #1'
*                     & 'cpu_cfg.h  CPU INTERRUPTS DISABLED TIME MEASUREMENT CONFIGURATION  Note #1a'.
*
*               (2) (a) Timer count values MUST be returned via word-size-configurable 'CPU_TS_TMR'
*                       data type.
*
*                       (1) If timer has more bits, truncate timer values' higher-order bits greater
*                           than the configured 'CPU_TS_TMR' timestamp timer data type word size.
*
*                       (2) Since the timer MUST NOT have less bits than the configured 'CPU_TS_TMR'
*                           timestamp timer data type word size; 'CPU_CFG_TS_TMR_SIZE' MUST be
*                           configured so that ALL bits in 'CPU_TS_TMR' data type are significant.
*
*                           In other words, if timer size is not a binary-multiple of 8-bit octets
*                           (e.g. 20-bits or even 24-bits), then the next lower, binary-multiple
*                           octet word size SHOULD be configured (e.g. to 16-bits).  However, the
*                           minimum supported word size for CPU timestamp timers is 8-bits.
*
*                       See also 'cpu_cfg.h   CPU TIMESTAMP CONFIGURATION  Note #2'
*                              & 'cpu_core.h  CPU TIMESTAMP DATA TYPES     Note #1'.
*
*                   (b) Timer SHOULD be an 'up'  counter whose values increase with each time count.
*
*                   (c) When applicable, timer period SHOULD be less than the typical measured time
*                       but MUST be less than the maximum measured time; otherwise, timer resolution
*                       inadequate to measure desired times.
*
*                   See also 'CPU_TS_TmrRd()  Note #2'.
*********************************************************************************************************
*/

#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
void  CPU_TS_TmrInit (void)
{
    CPU_INT32U  cpu_clk_freq_hz;


    DEM_CR         |= (CPU_INT32U)DEM_CR_TRCENA;                /* Enable Cortex-M3's DWT CYCCNT reg.                   */
    DWT_CYCCNT      = (CPU_INT32U)0u;
    DWT_CR         |= (CPU_INT32U)DWT_CR_CYCCNTENA;

    cpu_clk_freq_hz = BSP_CPU_ClkFreq();
    CPU_TS_TmrFreqSet(cpu_clk_freq_hz);
}
#endif

/*
*********************************************************************************************************
*                                           CPU_TS_TmrRd()
*
* Description : Get current CPU timestamp timer count value.
*
* Argument(s) : none.
*
* Return(s)   : Timestamp timer count (see Notes #2a & #2b).
*
* Caller(s)   : CPU_TS_Init(),
*               CPU_TS_Get32(),
*               CPU_TS_Get64(),
*               CPU_IntDisMeasStart(),
*               CPU_IntDisMeasStop().
*
*               This function is an INTERNAL CPU module function & MUST be implemented by application/
*               BSP function(s) [see Note #1] but SHOULD NOT be called by application function(s).
*
* Note(s)     : (1) CPU_TS_TmrRd() is an application/BSP function that MUST be defined by the developer 
*                   if either of the following CPU features is enabled :
*
*                   (a) CPU timestamps
*                   (b) CPU interrupts disabled time measurements
*
*                   See 'cpu_cfg.h  CPU TIMESTAMP CONFIGURATION  Note #1'
*                     & 'cpu_cfg.h  CPU INTERRUPTS DISABLED TIME MEASUREMENT CONFIGURATION  Note #1a'.
*
*               (2) (a) Timer count values MUST be returned via word-size-configurable 'CPU_TS_TMR' 
*                       data type.
*
*                       (1) If timer has more bits, truncate timer values' higher-order bits greater 
*                           than the configured 'CPU_TS_TMR' timestamp timer data type word size.
*
*                       (2) Since the timer MUST NOT have less bits than the configured 'CPU_TS_TMR' 
*                           timestamp timer data type word size; 'CPU_CFG_TS_TMR_SIZE' MUST be 
*                           configured so that ALL bits in 'CPU_TS_TMR' data type are significant.
*
*                           In other words, if timer size is not a binary-multiple of 8-bit octets 
*                           (e.g. 20-bits or even 24-bits), then the next lower, binary-multiple 
*                           octet word size SHOULD be configured (e.g. to 16-bits).  However, the 
*                           minimum supported word size for CPU timestamp timers is 8-bits.
*
*                       See also 'cpu_cfg.h   CPU TIMESTAMP CONFIGURATION  Note #2'
*                              & 'cpu_core.h  CPU TIMESTAMP DATA TYPES     Note #1'.
*
*                   (b) Timer SHOULD be an 'up'  counter whose values increase with each time count.
*
*                       (1) If timer is a 'down' counter whose values decrease with each time count,
*                           then the returned timer value MUST be ones-complemented.
*
*                   (c) (1) When applicable, the amount of time measured by CPU timestamps is 
*                           calculated by either of the following equations :
*
*                           (A) Time measured  =  Number timer counts  *  Timer period
*
*                                   where
*
*                                       Number timer counts     Number of timer counts measured 
*                                       Timer period            Timer's period in some units of 
*                                                                   (fractional) seconds
*                                       Time measured           Amount of time measured, in same 
*                                                                   units of (fractional) seconds 
*                                                                   as the Timer period
*
*                                                  Number timer counts
*                           (B) Time measured  =  ---------------------
*                                                    Timer frequency
*
*                                   where
*
*                                       Number timer counts     Number of timer counts measured
*                                       Timer frequency         Timer's frequency in some units 
*                                                                   of counts per second
*                                       Time measured           Amount of time measured, in seconds
*
*                       (2) Timer period SHOULD be less than the typical measured time but MUST be less 
*                           than the maximum measured time; otherwise, timer resolution inadequate to 
*                           measure desired times.
*********************************************************************************************************
*/

#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
CPU_TS_TMR  CPU_TS_TmrRd (void)
{
    return ((CPU_TS_TMR)DWT_CYCCNT);
}
#endif

/*
*********************************************************************************************************
*                                         CPU_TSxx_to_uSec()
*
* Description : Convert a 32-/64-bit CPU timestamp from timer counts to microseconds.
*
* Argument(s) : ts_cnts   CPU timestamp (in timestamp timer counts [see Note #2aA]).
*
* Return(s)   : Converted CPU timestamp (in microseconds           [see Note #2aD]).
*
* Caller(s)   : Application.
*
*               This function is an (optional) CPU module application programming interface (API) 
*               function which MAY be implemented by application/BSP function(s) [see Note #1] & 
*               MAY be called by application function(s).
*
* Note(s)     : (1) CPU_TS32_to_uSec()/CPU_TS64_to_uSec() are application/BSP functions that MAY be 
*                   optionally defined by the developer when either of the following CPU features is 
*                   enabled :
*
*                   (a) CPU timestamps
*                   (b) CPU interrupts disabled time measurements
*
*                   See 'cpu_cfg.h  CPU TIMESTAMP CONFIGURATION  Note #1'
*                     & 'cpu_cfg.h  CPU INTERRUPTS DISABLED TIME MEASUREMENT CONFIGURATION  Note #1a'.
*
*               (2) (a) The amount of time measured by CPU timestamps is calculated by either of 
*                       the following equations :
*
*                                                                        10^6 microseconds
*                       (1) Time measured  =   Number timer counts   *  -------------------  *  Timer period
*                                                                            1 second
*
*                                              Number timer counts       10^6 microseconds
*                       (2) Time measured  =  ---------------------  *  -------------------
*                                                Timer frequency             1 second
*
*                               where
*
*                                   (A) Number timer counts     Number of timer counts measured
*                                   (B) Timer frequency         Timer's frequency in some units 
*                                                                   of counts per second
*                                   (C) Timer period            Timer's period in some units of 
*                                                                   (fractional)  seconds
*                                   (D) Time measured           Amount of time measured, 
*                                                                   in microseconds
*
*                   (b) Timer period SHOULD be less than the typical measured time but MUST be less 
*                       than the maximum measured time; otherwise, timer resolution inadequate to 
*                       measure desired times.
*
*                   (c) Specific implementations may convert any number of CPU_TS32 or CPU_TS64 bits 
*                       -- up to 32 or 64, respectively -- into microseconds.
*********************************************************************************************************
*/

#if (CPU_CFG_TS_32_EN == DEF_ENABLED)
CPU_INT64U  CPU_TS32_to_uSec (CPU_TS32  ts_cnts)
{
    CPU_INT64U  ts_us;
    CPU_INT64U  fclk_freq;

    
    fclk_freq = BSP_CPU_ClkFreq();    
    ts_us     = ts_cnts / (fclk_freq / DEF_TIME_NBR_uS_PER_SEC);

    return (ts_us);
}
#endif


#if (CPU_CFG_TS_64_EN == DEF_ENABLED)
CPU_INT64U  CPU_TS64_to_uSec (CPU_TS64  ts_cnts)
{
    CPU_INT64U  ts_us;
    CPU_INT64U  fclk_freq;
    

    fclk_freq = BSP_CPU_ClkFreq();    
    ts_us     = ts_cnts / (fclk_freq / DEF_TIME_NBR_uS_PER_SEC);

    return (ts_us);
}
#endif

/*
*********************************************************************************************************
*	�� �� ��: NVIC_Configuration
*	����˵��: �ж����ȼ����ã������Ȱ����ȼ��������ã�Ȼ����
*			  �����ģ������ʵ�����ȼ����� 
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void Bsp_NVIC_Configuration(void)
{			
	/* ����NVIC���ȼ�����ΪGroup2��0-3��ռʽ���ȼ���0-3����Ӧʽ���ȼ� */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}

/*
 * ��������BSP_Init
 * ����  ��ʱ�ӳ�ʼ����Ӳ����ʼ��
 * ����  ����
 * ���  ����
 */
void BSP_Init(void)
{  
	uint16_t readCal[8]={0};
	
	/*CRC��emWinû�й�ϵ��ֻ������Ϊ�˿�ı��������ģ�����STemWin�Ŀ�ֻ������ST��оƬ���棬���оƬ���޷�ʹ�õġ� */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
	
	Bsp_NVIC_Configuration();  /* �ж����ȼ��������� */
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();  
	/* ������ʼ�� */
	Key1_GPIO_Config();
	Key2_GPIO_Config();
	/* ��������ʼ�� */
	Beep_Init();
	/* ����1��ʼ�� */
	USART1_Config();
	/* ��������ʼ�� */
	XPT2046_Init();
	
	/* 
	 * �����еĴ������Զ���ʹ���ڴ��豸����GUI_Init����ò�����
	 * �������ڣ����洰�ڣ���֮ǰ���òŰ����������� 
	 */
	WM_SetCreateFlags(WM_CF_MEMDEV);
	
	/* ��ʼ��GUI */
  ILI9341_Init(); /* ��ǰ��ʼ��LCD��������GUI_Init����LCD_X_Config֮ǰȷ��LCD����оƬID: lcdid */
	GUI_Init();
	
	/* Register work area for each volume (Always succeeds regardless of disk status) */
	f_mount(&fs,"0:",1);
	
	/*************** ��ȡ������У׼����  **************************/
	SPI_FLASH_BufferRead(&IsCal,CALADD, 1);
	//printf("cal:%X\n",IsCal);
	if(IsCal==0x55)
	{
		SPI_FLASH_BufferRead((void *)readCal,CALADD+2, sizeof(readCal));
//		printf("emWin_Touch_Calibration:\n");
//		printf("XPos1:%2d-->%4d && XPos2:%3d-->%4d\n",readCal[0], readCal[6], readCal[4], readCal[2]);
//		printf("YPos1:%2d-->%4d && YPos2:%3d-->%4d\n",readCal[1], readCal[7], readCal[5], readCal[3]);
		/* Use the physical values to calibrate the touch screen */
		GUI_TOUCH_Calibrate(GUI_COORD_X,readCal[0], readCal[4], readCal[6], readCal[2]); /* Calibrate X-axis */
		GUI_TOUCH_Calibrate(GUI_COORD_Y,readCal[1], readCal[5], readCal[7], readCal[3]); /* Calibrate Y-axis */
	}
	/************************************************************/
	
}
/*
*********************************************************************************************************
*	�� �� ��: bsp_DelayUS
*	����˵��: �������ʱ����CPU���ڲ�����ʵ�֣�32λ������
*             	OSSchedLock(&err);
*				bsp_DelayUS(5);
*				OSSchedUnlock(&err); ����ʵ����������Ƿ���Ҫ�ӵ�������ѡ����ж�
*	��    �Σ�n : �ӳٳ��ȣ���λ1 us
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_DelayUS(uint32_t _ulDelayTime)
{
    uint32_t ticks;
	uint32_t told,tnow,tcnt=0;
		    	 
	ticks = _ulDelayTime * (SystemCoreClock / 1000000);	 /* ��Ҫ�Ľ����� */ 		 
	tcnt = 0;
	told = (uint32_t)CPU_TS_TmrRd();         /* �ս���ʱ�ļ�����ֵ */

	while(1)
	{
		tnow = (uint32_t)CPU_TS_TmrRd();	
		if(tnow != told)
		{	
		    /* 32λ�������ǵ��������� */    
			if(tnow > told)
			{
				tcnt += tnow - told;	
			}
			/* ����װ�� */
			else 
			{
				tcnt += UINT32_MAX - told + tnow;	
			}	
			
			told = tnow;

			/*ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳� */
			if(tcnt >= ticks)break;
		}  
	}
}

/*
 * ϵͳ�����λ
 */
void Soft_Reset(void)
{
  __set_FAULTMASK(1);   /* �ر������ж� */  
  NVIC_SystemReset();   /* ϵͳ��λ */
}


/* --------------------------------------end of file--------------------------------------- */
