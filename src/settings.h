#ifdef   WAVELIB_SETTINGS_HEADER
#include WAVELIB_SETTINGS_HEADER
#ifdef  _BUILD_STATIC_LIB
#define  USE_PRE_COMPILED_HEADER
#define  USE_NAMESPACER (1)
#endif
#define  COMPILE_WAVELIB
#else
#include "../inc/importsettings.h"
#define  IMPORT_WAVELIB
#define  WAVELIB_VERSION_NUMBER SET_WAVESPACE::GetVersionNumber()
#endif

// declare any other includes which support
// 'namespacer.h' into a distinct namespace
#define SET_NAMESPACE SET_WAVESPACE

// make 24bit audio data types (separate incl.)
// also being exported within WaveFileHandling.Dll
//#define USE_NAMESPACER (0)
#define NAMESPACE_API WAVELIB_API

// implement 'Literal Operator' for Int24 types.. makes compile
// time constants available which are bound to Int24 type limits 
#define INT24_LITERAL_OPERATORS (1)

// std::typetrait and std::numeric_limits extensions for Int24 types 
#define INT24_TYPETRAIT_SUPPORT (1)

#define PAM_WAVEFILE_HEADERS (1)
