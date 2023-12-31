#ifndef PLATFORM_TYPES_H
#define PLATFORM_TYPES_H

#ifndef _Bool
#define _Bool unsigned char
#endif

#define CPU_TYPE            CPU_TYPE_32 
#define CPU_BIT_ORDER       MSB_FIRST 
#define CPU_BYTE_ORDER      HIGH_BYTE_FIRST

#ifndef FALSE
#define FALSE		(boolean)false
#endif
#ifndef TRUE
#define TRUE		(boolean)true
#endif

#ifndef False
#define False		(boolean)false
#endif
#ifndef True
#define True		(boolean)true
#endif

typedef _Bool      			boolean;
typedef char         		sint8;
typedef unsigned char  		uint8;
typedef char				char_t;
typedef short        		sint16;
typedef unsigned short 		uint16;
typedef long             	sint32;
typedef unsigned long  		uint32;
typedef float               float32; 
typedef double              float64;  

#endif /* PLATFORM_TYPES_H */