#ifndef __RT_DEF_H__
#define __RT_DEF_H__

/*
*************************************************************************
*                               ��������
*************************************************************************
*/
/* RT-Thread basic data type definitions */
typedef signed   char                   rt_int8_t;      /**<  8bit integer type */
typedef signed   short                  rt_int16_t;     /**< 16bit integer type */
typedef signed   long                   rt_int32_t;     /**< 32bit integer type */
typedef unsigned char                   rt_uint8_t;     /**<  8bit unsigned integer type */
typedef unsigned short                  rt_uint16_t;    /**< 16bit unsigned integer type */
typedef unsigned long                   rt_uint32_t;    /**< 32bit unsigned integer type */
typedef int                             rt_bool_t;      /**< boolean type */

/* 32bit CPU */
typedef long                            rt_base_t;      /**< Nbit CPU related date type */
typedef unsigned long                   rt_ubase_t;     /**< Nbit unsigned CPU related data type */

typedef rt_base_t                       rt_err_t;       /**< Type for error number */
typedef rt_uint32_t                     rt_time_t;      /**< Type for time stamp */
typedef rt_uint32_t                     rt_tick_t;      /**< Type for tick count */
typedef rt_base_t                       rt_flag_t;      /**< Type for flags */
typedef rt_ubase_t                      rt_size_t;      /**< Type for size number */
typedef rt_ubase_t                      rt_dev_t;       /**< Type for device */
typedef rt_base_t                       rt_off_t;       /**< Type for offset */

/* boolean type definitions */
#define RT_TRUE                         1               /**< boolean true  */
#define RT_FALSE                        0               /**< boolean fails */

#ifdef __CC_ARM
		#define rt_inline                   static __inline
		#define ALIGN(n)                    __attribute__((aligned(n)))

#elif defined (__IAR_SYSTEMS_ICC__)
    #define rt_inline                   static inline
		#define ALIGN(n)                    PRAGMA(data_alignment=n)
		
#elif defined (__GNUC__)
    #define rt_inline                   static __inline
		#define ALIGN(n)                    __attribute__((aligned(n)))
#else
    #error not supported tool chain			
#endif

		
#define RT_ALIGN(size, align)           (((size) + (align) - 1) & ~((align) - 1))
#define RT_ALIGN_DOWN(size, align)      ((size) & ~((align) - 1))

		
#define RT_NULL                         (0)

/*
*************************************************************************
*                               �����붨��
*************************************************************************
*/
/* RT-Thread �������ض��� */
#define RT_EOK                          0               /**< There is no error */
#define RT_ERROR                        1               /**< A generic error happens */
#define RT_ETIMEOUT                     2               /**< Timed out */
#define RT_EFULL                        3               /**< The resource is full */
#define RT_EEMPTY                       4               /**< The resource is empty */
#define RT_ENOMEM                       5               /**< No memory */
#define RT_ENOSYS                       6               /**< No system */
#define RT_EBUSY                        7               /**< Busy */
#define RT_EIO                          8               /**< IO error */
#define RT_EINTR                        9               /**< Interrupted system call */
#define RT_EINVAL                       10              /**< Invalid argument */

/*
*************************************************************************
*                               ˫������ṹ��
*************************************************************************
*/
struct rt_list_node
{
    struct rt_list_node *next;              /* ָ���һ���ڵ� */
    struct rt_list_node *prev;              /* ָ��ǰһ���ڵ� */
};
typedef struct rt_list_node rt_list_t;                  


/*
*************************************************************************
*                               �߳̽ṹ��
*************************************************************************
*/

struct rt_thread
{
	void        *sp;	          /* �߳�ջָ�� */
	void        *entry;	          /* �߳���ڵ�ַ */
	void        *parameter;	      /* �߳��β� */	
	void        *stack_addr;      /* �߳���ʼ��ַ */
	rt_uint32_t stack_size;       /* �߳�ջ��С����λΪ�ֽ� */
	
	rt_list_t   tlist;            /* �߳�����ڵ� */
};
typedef struct rt_thread *rt_thread_t;


#endif  /* __RT_DEF_H__ */
