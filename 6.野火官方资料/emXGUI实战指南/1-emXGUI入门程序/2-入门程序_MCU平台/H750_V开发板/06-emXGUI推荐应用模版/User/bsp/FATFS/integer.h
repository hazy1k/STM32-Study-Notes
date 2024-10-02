/*-------------------------------------------*/
/* Integer type definitions for FatFs module */
/*-------------------------------------------*/

#ifndef _FF_INTEGER
#define _FF_INTEGER

#ifdef _WIN32	/* Development platform */

#include <windows.h>
#include <tchar.h>

#else			/* Embedded platform */

/* This type MUST be 8-bit */
//typedef unsigned char	BYTE;

/* These types MUST be 16-bit */


//typedef unsigned short	WCHAR;

/* These types MUST be 16-bit or 32-bit */
//typedef int				INT;
//typedef unsigned int	UINT;

/* These types MUST be 32-bit */
//typedef long			LONG;

#include "def.h"
#endif

#endif
