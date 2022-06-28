/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      indiaccessfuncs.h                          ||
||     Author:    Kalle                                      ||
||     Generated: 25.09.2017                                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef _indiaccessfuncs_h_
#define _indiaccessfuncs_h_

#include <stdio.h>
#include <string.h>
#include <memory.h>

#ifndef NULL
#define NULL 0
#endif

#ifdef _MSC_VER
   #if _MSC_VER>0
      #define IS_MSVC (1)
   #endif
#endif

#if !defined(IS_MSVC)
#if !defined(__TINYC__)
#if !defined(__MINGW32__)
   template<const unsigned ce>
   inline int strcpy_s(char(&dst)[ce], const char* src) {
     int retval;
     try { if (!( (retval = (ce && src) ? (dst[ce - 1] = '\0') : 22) ) ) {
             retval = ( strcpy( &dst[0], src) == &dst[0] ) ? 0 : 34;
         } else { dst[0] = '\0'; }
     } catch(...) {
         retval = 22;
     } return retval;
   }
#endif
  template<const unsigned ce>
  inline int sprintf_s( char* dst, const char* fmt,
                        long unsigned exp ) {
    bool success;
    try { dst[ce - 1] = '\0';
        if ( (success = (dst[ce - 1] == '\0')) )
            sprintf(dst, fmt, (void*)exp);
    } catch (...) {
        success = false;
    } return success ? 0 : ce;
  }
#else
#define __SIZE_TYPE__ uintptr_t
#endif
  inline int _i64toa_s( signed long long val, char * dst,
                        uintptr_t cbd, int rdx ) {
    int error;
    try { if (!(error = (int)( cbd && dst && (rdx >= 2)
                             ? (dst[cbd - 1] = '\0') : 22) )
               ) lltoa(val, dst, rdx);
    } catch (...) {
        error = 34;
    } return error;
  }
  inline int fopen_s( FILE** file, const char* fname,
                      const char* format ) {
     FILE* validator = *file;
    #if defined(__MINGW64__)||defined(__x86_64__)&&(!defined(__TINYC__))
     *file = fopen64(fname, format);
    #else
     *file = fopen(fname, format);
    #endif
    #if !defined(__MINGW32__)&&(!defined(__TINYC__))
     funlockfile(*file);
    #endif
     return (2*(*file == NULL))
          + (5*(*file == validator));
  }
#endif

#if (__MINGW32__||__GNUC__||__GNUG__||__TINYC__||__MINGW64__)
 #include <stdlib.h> // if not MSVC ...
 #define  FREAD_FUNC( D, S, C, F )  fread(D,S,C,F)
 #define  MEMCPYFUNC( D, S, T, C )  memcpy(D,S,T*C)
 #define  FOPEN_FUNC( F, N, fm )    fopen_s(F,N,fm)
 #define  LONG_TOSTR( L, D, C, R ) _i64toa_s(L,D,C,R)
 #if defined(__TINYC__) //...if TCC
  #define  ARR_STRCPY( arDst, S )   strcpy(&arDst[0],S)
  #define  STR_STRCPY(C,D,S)        strcpy(D,S)
  #define  STR_PRINTF( C, D, F, S ) sprintf(D, F, S)
 #else // ..if not TCC
  #define  ARR_STRCPY( arDst, S )   strcpy_s(arDst,S)
  #define  STR_STRCPY(C,D,S)        strcpy_s<C>((char(&)[C])*D,S)
  #define  STR_PRINTF( C, D, F, S ) sprintf_s<C>(D, F, (unsigned long)(void*)S)
 #endif // ...if we ARE on Windows,
 #if defined(__MINGW32__)||defined(__MINGW64__)
  #define  WRITE_S16S( S, F )       fwrite(&S,1,2,F)
  #define  WRITE_BYTE( B, F )       fputc(B,F)
  #define  WRITE_DATA( D, S, C, F ) fwrite(D,S,C,F)
  #define  WRITE_F32S( S, F )       fwrite(&S,1,4,F)
  #define  FLUSH_FUNC( F )          fflush(F)
 #else // ...if we are NOT on Windows
  #define  WRITE_S16S( S, F )       fwrite_unlocked(&S,1,2,F)
  #define  WRITE_BYTE( B, F )       fputc_unlocked(B,F)
  #define  WRITE_DATA( D, S, C, F ) fwrite_unlocked(D,S,C,F)
  #define  WRITE_F32S( S, F )       fwrite_unlocked(&S,1,4,F)
  #define  FLUSH_FUNC( F )          fflush_unlocked(F)
 #endif // ...if we are on a x64 machine (any OS):
 #if defined(__MINGW64__)||defined(__x86_64__)
  #define  FSEEK_FUNC( F, P, O )    fseeko64(F,P,O)
  #define  FTELL_FUNC( F )          ftello64(f)
 #else // ...if on plain 32bit i386 based architecture:
  #define  FSEEK_FUNC( F, P, O )    fseek(F,P,O)
  #define  FTELL_FUNC( F )          ftell(f)
 #endif
#elif defined(_WIN32) //.. if MSVC, any machine architecture:
 #define STR_PRINTF( C, D, F, S )  sprintf_s(D,C,F,S)
 #define STR_STRCPY( C, D, S )     strcpy_s(D,C,S)
 #define ARR_STRCPY( arDst, S )    strcpy_s(arDst,S)
 #define FOPEN_FUNC( F, N, M )     fopen_s( &F, N, M )
 #define FREAD_FUNC( D, S, C, F )  fread_s( D, (S * C), S, C, F )
 #define WRITE_S16S( S, F )        fputwc(S,F)
 #define WRITE_BYTE( B, F )        fwrite(&B,1,1,F)
 #define WRITE_DATA( D, S, C, F )  fwrite(D,S,C,F)
 #define FTELL_FUNC( F )           ftell(F)
 #define FLUSH_FUNC( F )           fflush(F)
 #define FSEEK_FUNC( F, P, O )     fseek(F,P,O)
 #define WRITE_F32S( S, F )        fwrite(&S,1,4,F)
 #define MEMCPYFUNC( D, S, T, C )  _memccpy(D,S,T,C)
 #define LONG_TOSTR( L, D, C, R )  _i64toa_s(L,D,C,R)
#endif

#endif
