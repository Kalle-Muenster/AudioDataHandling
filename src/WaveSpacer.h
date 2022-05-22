/*///////////////////////////////////////////////////////////*\
||                                                           ||
||     File:      WaveSpacer.h                               ||
||     Author:    Kalle                                      ||
||     Generated: by Command Generater v.0.1                 ||
||                                                           ||
\*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
#ifndef WAVESPACE_DEFINED
#define WAVESPACE_DEFINED

#ifndef SET_WAVESPACE
#include "settings.h"
#endif

#ifdef _MSC_VER
#if    _MSC_VER > 0
#ifdef  COMPILE_WAVELIB
#ifdef  QT_VERSION
#define WAVELIB_API Q_DECL_EXPORT
#else
#define WAVELIB_API __declspec(dllexport)
#endif
#else
#ifdef  IMPORT_WAVELIB
#ifdef  QT_VERSION
#define WAVELIB_API Q_DECL_IMPORT
#else
#define WAVELIB_API __declspec(dllimport)
#endif
#else
#define WAVELIB_API
#endif
#endif
#else
#define WAVELIB_API
#endif
#else
#define WAVELIB_API
#endif

#if !defined(SET_WAVESPACE)
 #define  WAVENAMESPACE stepflow
#else
 #if (SET_WAVESPACE != -1) && (SET_WAVESPACE != 0xffffffff)
  #define WAVENAMESPACE SET_WAVESPACE
 #else
  #ifdef  WAVENAMESPACE
   #undef WAVENAMESPACE
  #endif
 #endif
#endif

#ifndef USE_PRECOMPILED_PROJECT_HEADER
#include <iostream>
#endif

#if !defined(WAVENAMESPACE)
 #ifndef SpacedName(decl) decl
  #define WaveSpace(decl) SpacedName(decl)
 #endif
 #define BEGIN_WAVESPACE
 #define USING_WAVESPACE
 #define ENDOF_WAVESPACE
#else
 #define SpacedName(decl) WAVENAMESPACE :: decl
 #define WaveSpace(decl) SpacedName(decl)
 #define BEGIN_WAVESPACE namespace WAVENAMESPACE {
 #define USING_WAVESPACE using namespace WAVENAMESPACE;
 #define ENDOF_WAVESPACE }
#endif

#endif