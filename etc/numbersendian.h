#ifndef NUMBER_ENDIANES
#define NUMBER_ENDIANES (1)

#if defined( __x86_64__) || defined(_WIN64)
#define WORD_BYTESIZE 8
#define WORD_TYPENAME "QWORD"
#define WORD_VARIABLE unsigned long long int
#define WORD_PRCISION double
#elif defined(_M_IX86) || defined(__x86__)
#define WORD_BYTESIZE 4
#define WORD_TYPENAME "DWORD"
#define WORD_VARIABLE unsigned int
#define WORD_PRCISION float
#endif

#define ENDIAN_IS_BIG___ (1414743380u == ('T'|'E'<<8|'S'<<16|'T'<<24))
#define ENDIAN_IS_LITTLE (1413829460u == ('T'|'E'<<8|'S'<<16|'T'<<24))

#if ENDIAN_IS_BIG___
#define __BIG_ENDIAN (1)
#define __LITTLE_ENDIAN (0)
#else
#define __BIG_ENDIAN (0)
#define __LITTLE_ENDIAN (1)
#endif
#endif
