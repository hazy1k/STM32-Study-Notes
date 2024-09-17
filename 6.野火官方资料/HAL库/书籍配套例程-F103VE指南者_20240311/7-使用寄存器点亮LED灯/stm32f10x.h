

/*���ļ�������ӼĴ�����ַ���ṹ�嶨��*/

/*Ƭ���������ַ  */
#define PERIPH_BASE           ((unsigned int)0x40000000)

/*APB2 ���߻���ַ */
#define APB2PERIPH_BASE       (PERIPH_BASE + 0x10000)
/* AHB���߻���ַ */
#define AHBPERIPH_BASE        (PERIPH_BASE + 0x20000)

/*GPIOB�������ַ*/
#define GPIOB_BASE            (APB2PERIPH_BASE + 0x0C00)

/* GPIOB�Ĵ�����ַ,ǿ��ת����ָ�� */
#define GPIOB_CRL			*(unsigned int*)(GPIOB_BASE+0x00)
#define GPIOB_CRH			*(unsigned int*)(GPIOB_BASE+0x04)
#define GPIOB_IDR			*(unsigned int*)(GPIOB_BASE+0x08)
#define GPIOB_ODR			*(unsigned int*)(GPIOB_BASE+0x0C)
#define GPIOB_BSRR	  *(unsigned int*)(GPIOB_BASE+0x10)
#define GPIOB_BRR			*(unsigned int*)(GPIOB_BASE+0x14)
#define GPIOB_LCKR		*(unsigned int*)(GPIOB_BASE+0x18)

/*RCC�������ַ*/
#define RCC_BASE      (AHBPERIPH_BASE + 0x1000)
/*RCC��AHB1ʱ��ʹ�ܼĴ�����ַ,ǿ��ת����ָ��*/
#define RCC_APB2ENR		 *(unsigned int*)(RCC_BASE+0x18)

