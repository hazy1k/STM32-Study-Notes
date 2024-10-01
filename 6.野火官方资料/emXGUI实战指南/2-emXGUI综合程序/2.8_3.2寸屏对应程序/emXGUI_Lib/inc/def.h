
#ifndef __DEF_H_20170322_1635__
#define __DEF_H_20170322_1635__

#ifdef	__cplusplus
extern "C"{
#endif

/*=========================================================================================*/

#ifndef	FALSE
#define FALSE 	0
#endif

#ifndef	TRUE
#define	TRUE	(!FALSE)
#endif

#ifndef	NULL
#define	NULL	0
#endif

#ifndef	CONST
#define	CONST	const	
#endif

/*=========================================================================================*/

#ifdef	__cplusplus 

#define	INLINE	inline
typedef	wchar_t	WCHAR;

#else

#define	INLINE	__inline
typedef	unsigned short int  WCHAR;

#endif

/*=========================================================================================*/

typedef	char		    CHAR;
typedef	unsigned char	UCHAR;
typedef unsigned short	WORD;
typedef signed int		INT;
typedef unsigned int	UINT;
typedef signed long		LONG;
typedef	unsigned long	ULONG;
typedef unsigned long	DWORD;
typedef float       	FLOAT;
typedef	double			DOUBLE;
typedef	void*		    PVOID;
typedef	void*		    LPVOID;

typedef	signed 		char	    s8,S8;
typedef	unsigned 	char 	    u8,U8;
typedef	signed		short int	s16,S16;
typedef short			SHORT;
typedef	unsigned 	short int	u16,U16;
typedef	signed 		int		    s32,S32;
typedef	unsigned 	int 	    u32,U32;

typedef unsigned char	BYTE;
/*=========================================================================================*/

typedef int         BOOL;
typedef	void*		HANDLE;
typedef int         err_t;

typedef WCHAR*          LPWSTR;
typedef const WCHAR*    LPCWSTR;

/*=========================================================================================*/

//�����ֵ
#ifndef	MAX
#define	MAX(a,b)	(((a)>(b))?(a):(b))	
#endif

//����Сֵ	
#ifndef	MIN
#define	MIN(a,b)	(((a)<(b))?(a):(b))		
#endif

//�õ�һ��field�ڽṹ��(struct)�е�ƫ����
#ifndef	FPOS
#define FPOS(type, field)	( (unsigned int) &(( type *) 0)-> field )	
#endif

//�õ�һ���ṹ����field��ռ�õ��ֽ���
#ifndef	FSIZE
#define FSIZE(type, field)	sizeof( ((type *) 0)->field )	
#endif

//��һ����ĸת��Ϊ��д
#ifndef	UPCASE
#define UPCASE( c ) ( ((c) >= 'a' && (c) <= 'z') ? ((c) - 0x20) : (c) ) 
#endif

//�ж��ַ��ǲ���10��ֵ������ 
#ifndef	DECCHK
#define DECCHK( c ) ((c) >= '0' && (c) <= '9') 
#endif

//�ж��ַ��ǲ���16��ֵ������ 
#ifndef	HEXCHK
#define HEXCHK( c ) ( ((c) >= '0' && (c) <= '9') ||((c) >= 'A' && (c) <= 'F') || ((c) >= 'a' && (c) <= 'f'))
#endif

//��ֹ�����һ������ 
#ifndef	INC_SAT
#define INC_SAT( val )  (val = ((val)+1 > (val)) ? (val)+1 : (val))
#endif

//��������Ԫ�صĸ���
#ifndef	ARR_SIZE
#define ARR_SIZE( a )  ( sizeof( (a) ) / sizeof( (a[0]) ) )
#endif

/*=========================================================================================*/

#ifdef	__cplusplus
}
#endif

#endif /*__DEF_H__*/

