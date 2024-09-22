/*
************************************************************************************************************************
*                                                 ������ͷ�ļ�
************************************************************************************************************************
*/
#define    CPU_CORE_MODULE
#include  "cpu_core.h"
#include  "lib_def.h"



/*
*********************************************************************************************************
*                                             �Ĵ�������
*********************************************************************************************************
*/
/*
 ��Cortex-M�ں�������һ�������DWT(Data Watchpoint and Trace)����������һ��32λ�ļĴ�����CYCCNT��
 ����һ�����ϵ� ����������¼�����ں�ʱ�����еĸ�������CYCCNT���֮�󣬻���0���¿�ʼ���ϼ�����
 ʹ��CYCCNT�����Ĳ������裺
 1����ʹ��DWT���裬����������ں˵��ԼĴ���DEMCR��λ24���ƣ�д1ʹ��
 2��ʹ��CYCCNT�Ĵ���֮ǰ������0
 3��ʹ��CYCCNT�Ĵ����������DWT_CTRL(�����Ϻ궨��ΪDWT_CR)��λ0���ƣ�д1ʹ��
 */
#define  BSP_REG_DEM_CR                       (*(CPU_REG32 *)0xE000EDFC)
#define  BSP_REG_DWT_CR                       (*(CPU_REG32 *)0xE0001000)
#define  BSP_REG_DWT_CYCCNT                   (*(CPU_REG32 *)0xE0001004)
#define  BSP_REG_DBGMCU_CR                    (*(CPU_REG32 *)0xE0042004)

/*
*********************************************************************************************************
*                                            �Ĵ���λ����
*********************************************************************************************************
*/

#define  BSP_DBGMCU_CR_TRACE_IOEN_MASK                   0x10
#define  BSP_DBGMCU_CR_TRACE_MODE_ASYNC                  0x00
#define  BSP_DBGMCU_CR_TRACE_MODE_SYNC_01                0x40
#define  BSP_DBGMCU_CR_TRACE_MODE_SYNC_02                0x80
#define  BSP_DBGMCU_CR_TRACE_MODE_SYNC_04                0xC0
#define  BSP_DBGMCU_CR_TRACE_MODE_MASK                   0xC0

#define  BSP_BIT_DEM_CR_TRCENA                          (1<<24)

#define  BSP_BIT_DWT_CR_CYCCNTENA                       (1<<0)

/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/

#ifndef   CPU_CFG_LEAD_ZEROS_ASM_PRESENT
static  const  CPU_INT08U  CPU_CntLeadZerosTbl[256] = {                             /* Data vals :                      */
/*   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F   */
    8u,  7u,  6u,  6u,  5u,  5u,  5u,  5u,  4u,  4u,  4u,  4u,  4u,  4u,  4u,  4u,  /*   0x00 to 0x0F                   */
    3u,  3u,  3u,  3u,  3u,  3u,  3u,  3u,  3u,  3u,  3u,  3u,  3u,  3u,  3u,  3u,  /*   0x10 to 0x1F                   */
    2u,  2u,  2u,  2u,  2u,  2u,  2u,  2u,  2u,  2u,  2u,  2u,  2u,  2u,  2u,  2u,  /*   0x20 to 0x2F                   */
    2u,  2u,  2u,  2u,  2u,  2u,  2u,  2u,  2u,  2u,  2u,  2u,  2u,  2u,  2u,  2u,  /*   0x30 to 0x3F                   */
    1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  /*   0x40 to 0x4F                   */
    1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  /*   0x50 to 0x5F                   */
    1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  /*   0x60 to 0x6F                   */
    1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  1u,  /*   0x70 to 0x7F                   */
    0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  /*   0x80 to 0x8F                   */
    0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  /*   0x90 to 0x9F                   */
    0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  /*   0xA0 to 0xAF                   */
    0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  /*   0xB0 to 0xBF                   */
    0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  /*   0xC0 to 0xCF                   */
    0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  /*   0xD0 to 0xDF                   */
    0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  /*   0xE0 to 0xEF                   */
    0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u,  0u   /*   0xF0 to 0xFF                   */
};
#endif

/*
*********************************************************************************************************
*                                            ����ʵ��
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            CPU��ʼ������
*********************************************************************************************************
*/
/* CPU��ʼ������ */
void  CPU_Init (void)
{
/* CPU��ʼ���������ܹ�����������
   1����ʼ��ʱ���
   2����ʼ���ж�ʧ��ʱ�����
   3����ʼ��CPU����
   ����ֻ��ʱ������ܣ�ʣ�������ĳ�ʼ��������ɾ������ */
	
#if ((CPU_CFG_TS_EN     == DEF_ENABLED) || \
     (CPU_CFG_TS_TMR_EN == DEF_ENABLED))
    CPU_TS_Init();
#endif
	
}

/*
*********************************************************************************************************
*                                            ʱ�����غ���
*********************************************************************************************************
*/
/* ʱ�����ʼ������ */
#if ((CPU_CFG_TS_EN     == DEF_ENABLED) || \
     (CPU_CFG_TS_TMR_EN == DEF_ENABLED))
static  void  CPU_TS_Init (void)
{
                                                              
#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
    CPU_TS_TmrFreq_Hz   = 0u;                                  
    CPU_TS_TmrInit();                                          
#endif

}
#endif

/* ʱ�����ʱ����ʼ�� */
#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
void  CPU_TS_TmrInit (void)
{
    CPU_INT32U  fclk_freq;


    fclk_freq = BSP_CPU_ClkFreq();

    /* ʹ��DWT���� */
	BSP_REG_DEM_CR     |= (CPU_INT32U)BSP_BIT_DEM_CR_TRCENA;
	/* DWT CYCCNT�Ĵ���������0 */
    BSP_REG_DWT_CYCCNT  = (CPU_INT32U)0u;
/* ע�⣺��ʹ���������ȫ�����е�ʱ�򣬻���ͣ�����
         �ͺ���������������һ���ϵ�һ������Ҫ�ֶ����в��������� 
         ��ʹ��Ӳ�������ʱ��ȴ���� */
	/* ʹ��Cortex-M3 DWT CYCCNT�Ĵ��� */
    BSP_REG_DWT_CR     |= (CPU_INT32U)BSP_BIT_DWT_CR_CYCCNTENA;

    CPU_TS_TmrFreqSet((CPU_TS_TMR_FREQ)fclk_freq);
}
#endif


/* ��ȡCPU��HCLKʱ�� 
����Ǹ�Ӳ����صģ�Ŀǰ������������棬������ʱ�Ѹ�Ӳ����صĴ������ε���
ֱ���ֶ�����CPU��HCLKʱ��*/
CPU_INT32U  BSP_CPU_ClkFreq (void)
{
#if 0 
	RCC_ClocksTypeDef  rcc_clocks;


    RCC_GetClocksFreq(&rcc_clocks);
    return ((CPU_INT32U)rcc_clocks.HCLK_Frequency);
#else
	CPU_INT32U    CPU_HCLK;
	
	
	/* Ŀǰ�����������ʹ��25M��ϵͳʱ�� */
	CPU_HCLK = 25000000;
	
	return CPU_HCLK;
#endif
}

/* ��ʼ��CPU_TS_TmrFreq_Hz���������ϵͳ��ʱ�ӣ���λΪHZ */
#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
void  CPU_TS_TmrFreqSet (CPU_TS_TMR_FREQ  freq_hz)
{
    CPU_TS_TmrFreq_Hz = freq_hz;
}
#endif



#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
CPU_TS_TMR  CPU_TS_TmrRd (void)
{
    CPU_TS_TMR  ts_tmr_cnts;

                                                                
    ts_tmr_cnts = (CPU_TS_TMR)BSP_REG_DWT_CYCCNT;

    return (ts_tmr_cnts);
}
#endif

/*
*********************************************************************************************************
*                                            ǰ��0���㺯��
*********************************************************************************************************
*/
#ifndef   CPU_CFG_LEAD_ZEROS_ASM_PRESENT
CPU_DATA  CPU_CntLeadZeros (CPU_DATA  val)
{
    CPU_DATA    nbr_lead_zeros;
    CPU_INT08U  ix;


#if   (CPU_CFG_DATA_SIZE == CPU_WORD_SIZE_08)                                   /* ----------  8-BIT DATA VAL --------- */
                                                                                /* Chk bits [07:00] :                   */
                                                                                /* .. Nbr lead zeros =               .. */
    ix             = (CPU_INT08U)(val >>  0u);                                  /* .. lookup tbl ix  = 'val' >>  0 bits */
    nbr_lead_zeros = (CPU_DATA  )(CPU_CntLeadZerosTbl[ix] +  0u);               /* .. plus nbr msb lead zeros =  0 bits.*/


#elif (CPU_CFG_DATA_SIZE == CPU_WORD_SIZE_16)                                   /* ---------- 16-BIT DATA VAL --------- */
    if (val > 0x00FFu) {                                                        /* Chk bits [15:08] :                   */
                                                                                /* .. Nbr lead zeros =               .. */
        ix             = (CPU_INT08U)(val >>  8u);                              /* .. lookup tbl ix  = 'val' >>  8 bits */
        nbr_lead_zeros = (CPU_DATA  )(CPU_CntLeadZerosTbl[ix] +  0u);           /* .. plus nbr msb lead zeros =  0 bits.*/

    } else {                                                                    /* Chk bits [07:00] :                   */
                                                                                /* .. Nbr lead zeros =               .. */
        ix             = (CPU_INT08U)(val >>  0u);                              /* .. lookup tbl ix  = 'val' >>  0 bits */
        nbr_lead_zeros = (CPU_DATA  )(CPU_CntLeadZerosTbl[ix] +  8u);           /* .. plus nbr msb lead zeros =  8 bits.*/
    }


#elif (CPU_CFG_DATA_SIZE == CPU_WORD_SIZE_32)                                   /* ---------- 32-BIT DATA VAL --------- */
    if (val > 0x0000FFFFu) {
        if (val > 0x00FFFFFFu) {                                                /* Chk bits [31:24] :                   */
                                                                                /* .. Nbr lead zeros =               .. */
            ix             = (CPU_INT08U)(val >> 24u);                          /* .. lookup tbl ix  = 'val' >> 24 bits */
            nbr_lead_zeros = (CPU_DATA  )(CPU_CntLeadZerosTbl[ix] +  0u);       /* .. plus nbr msb lead zeros =  0 bits.*/

        } else {                                                                /* Chk bits [23:16] :                   */
                                                                                /* .. Nbr lead zeros =               .. */
            ix             = (CPU_INT08U)(val >> 16u);                          /* .. lookup tbl ix  = 'val' >> 16 bits */
            nbr_lead_zeros = (CPU_DATA  )(CPU_CntLeadZerosTbl[ix] +  8u);       /* .. plus nbr msb lead zeros =  8 bits.*/
        }

    } else {
        if (val > 0x000000FFu) {                                                /* Chk bits [15:08] :                   */
                                                                                /* .. Nbr lead zeros =               .. */
            ix             = (CPU_INT08U)(val >>  8u);                          /* .. lookup tbl ix  = 'val' >>  8 bits */
            nbr_lead_zeros = (CPU_DATA  )(CPU_CntLeadZerosTbl[ix] + 16u);       /* .. plus nbr msb lead zeros = 16 bits.*/

        } else {                                                                /* Chk bits [07:00] :                   */
                                                                                /* .. Nbr lead zeros =               .. */
            ix             = (CPU_INT08U)(val >>  0u);                          /* .. lookup tbl ix  = 'val' >>  0 bits */
            nbr_lead_zeros = (CPU_DATA  )(CPU_CntLeadZerosTbl[ix] + 24u);       /* .. plus nbr msb lead zeros = 24 bits.*/
        }
    }

/*$PAGE*/
#elif (CPU_CFG_DATA_SIZE == CPU_WORD_SIZE_64)                                   /* ---------- 64-BIT DATA VAL --------- */
    if (val > 0x00000000FFFFFFFFu) {
        if (val > 0x0000FFFFFFFFFFFFu) {
            if (val > 0x00FFFFFFFFFFFFFFu) {                                    /* Chk bits [63:56] :                   */
                                                                                /* .. Nbr lead zeros =               .. */
                ix             = (CPU_INT08U)(val >> 56u);                      /* .. lookup tbl ix  = 'val' >> 56 bits */
                nbr_lead_zeros = (CPU_DATA  )(CPU_CntLeadZerosTbl[ix] +  0u);   /* .. plus nbr msb lead zeros =  0 bits.*/

            } else {                                                            /* Chk bits [55:48] :                   */
                                                                                /* .. Nbr lead zeros =               .. */
                ix             = (CPU_INT08U)(val >> 48u);                      /* .. lookup tbl ix  = 'val' >> 48 bits */
                nbr_lead_zeros = (CPU_DATA  )(CPU_CntLeadZerosTbl[ix] +  8u);   /* .. plus nbr msb lead zeros =  8 bits.*/
            }

        } else {
            if (val > 0x000000FFFFFFFFFFu) {                                    /* Chk bits [47:40] :                   */
                                                                                /* .. Nbr lead zeros =               .. */
                ix             = (CPU_INT08U)(val >> 40u);                      /* .. lookup tbl ix  = 'val' >> 40 bits */
                nbr_lead_zeros = (CPU_DATA  )(CPU_CntLeadZerosTbl[ix] + 16u);   /* .. plus nbr msb lead zeros = 16 bits.*/

            } else {                                                            /* Chk bits [39:32] :                   */
                                                                                /* .. Nbr lead zeros =               .. */
                ix             = (CPU_INT08U)(val >> 32u);                      /* .. lookup tbl ix  = 'val' >> 32 bits */
                nbr_lead_zeros = (CPU_DATA  )(CPU_CntLeadZerosTbl[ix] + 24u);   /* .. plus nbr msb lead zeros = 24 bits.*/
            }
        }

    } else {
        if (val > 0x000000000000FFFFu) {
            if (val > 0x0000000000FFFFFFu) {                                    /* Chk bits [31:24] :                   */
                                                                                /* .. Nbr lead zeros =               .. */
                ix             = (CPU_INT08U)(val >> 24u);                      /* .. lookup tbl ix  = 'val' >> 24 bits */
                nbr_lead_zeros = (CPU_DATA  )(CPU_CntLeadZerosTbl[ix] + 32u);   /* .. plus nbr msb lead zeros = 32 bits.*/

            } else {                                                            /* Chk bits [23:16] :                   */
                                                                                /* .. Nbr lead zeros =               .. */
                ix             = (CPU_INT08U)(val >> 16u);                      /* .. lookup tbl ix  = 'val' >> 16 bits */
                nbr_lead_zeros = (CPU_DATA  )(CPU_CntLeadZerosTbl[ix] + 40u);   /* .. plus nbr msb lead zeros = 40 bits.*/
            }

        } else {
            if (val > 0x00000000000000FFu) {                                    /* Chk bits [15:08] :                   */
                                                                                /* .. Nbr lead zeros =               .. */
                ix             = (CPU_INT08U)(val >>  8u);                      /* .. lookup tbl ix  = 'val' >>  8 bits */
                nbr_lead_zeros = (CPU_DATA  )(CPU_CntLeadZerosTbl[ix] + 48u);   /* .. plus nbr msb lead zeros = 48 bits.*/

            } else {                                                            /* Chk bits [07:00] :                   */
                                                                                /* .. Nbr lead zeros =               .. */
                ix             = (CPU_INT08U)(val >>  0u);                      /* .. lookup tbl ix  = 'val' >>  0 bits */
                nbr_lead_zeros = (CPU_DATA  )(CPU_CntLeadZerosTbl[ix] + 56u);   /* .. plus nbr msb lead zeros = 56 bits.*/
            }
        }
    }


#else                                                                           /* See Note #1a.                        */
   (void)&ix;
    nbr_lead_zeros = 0u;
#endif

    return (nbr_lead_zeros);
}
#endif




