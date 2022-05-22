#ifdef   WAVELIB_SETTINGS_HEADER
#include WAVELIB_SETTINGS_HEADER
#ifdef  _BUILD_STATIC_LIB
#define  USE_PRE_COMPILED_HEADER
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
#define USE_NAMESPACER (1)
#define NAMESPACE_API WAVELIB_API

// implement 'Literal Operator' for Int24 types.. makes compile
// time constants available which are bound to Int24 type limits 
#define INT24_LITERAL_OPERATORS (0)
#define INT24_TYPETRAIT_SUPPORT (1)

//#define COMPILE_HARDWARE_DEVICEIO (1)

#define PAM_WAVEFILE_HEADERS (1)

#define WAVELIB_VERSION_STRING SET_WAVELIB_VERSION
#ifdef _DEBUG
#define WAVE_LIB_CONFIGURATION "Debug"
#else
#define WAVE_LIB_CONFIGURATION "Release"
#endif

// for enabling AudioFX features using stepflow controlled types
// HAVE_CONTROLLED_VALUES may be set 1 as global compiler switch:
#ifdef HAVE_CONTROLLED_VALUES
#if    HAVE_CONTROLLED_VALUES > 0

// native stepflow sources are included within *namespace*:
#define SET_STEPFLOW_NAMESPACE SET_WAVESPACE
#define SET_STEPLIST_NAMESPACE EMPTY
#define SET_CONTROLLERSPACE    EMPTY

   // just stepflow *minimal* sources will be included:
   #define STEPFLOW_MINI_CONFIG
   // but explicitely include *additionally* also thse:
   #define WITH_PEAKFINDER_MODE
   #define WITH_CAPACITOR_MODE
   #define WITH_ELEMENT_MODE
   #define WITH_RAMP_FORM_MODE
   #define WITH_PULS_FORM_MODE
   #define WITH_SINE_FORM_MODE
   #define WITH_BANDFILTER_MODE
   #define WITH_MULTISTOP_MODE

   // may be set to (1) to make the native stepflow *Sinus* mode
   // using the 'glm' math library instead of std math.h library
   #define USE_GLM_SINUS (0)

   // control modes which use internaly an additional,
   // different data type will be implemented as using:
   #define CONTROLER_INTERNAL_PRECISION WORD_PRCISION

   // enable threadsafe features for controllers supporting it:
   // will wait THREAD_WAITCYCLE_TIME *ms* when doing a waitcycle:
   // (e.g. undef to disable threadding support or define EMPTY )
   #ifndef  THREAD_WAITCYCLE_TIME
   #define  THREAD_WAITCYCLE_TIME 2
   #endif
#endif
#endif

#if defined(THREAD_WAITCYCLE_TIME)
#if THREAD_WAITCYCLE_TIME != EMPTY

   // define the function call which shall be used to let the current thread doing a WaitCycle:...
 #ifdef _MANAGED
   // ...when compiled within a .Net project
   #define THREAD_WAITCYCLE_FUNC(ms) System::Threading::Thread::CurrentThread->Sleep( ms )
 #elif defined(QT_VERSION)
   // ...when compiled within a Qt project
   #define THREAD_WAITCYCLE_FUNC(ms) QThread::currentThread()->msleep( ms )
#else
   // ...when compiled within a cpp stdlib project
   #include <chrono>
   #include <thread>
   #define THREAD_WAITCYCLE_FUNC(ms) std::this_thread::sleep_for(std::chrono::milliseconds( ms ))
 #endif

#else
#undef THREAD_WAITCYCLE_TIME
#endif
#endif


