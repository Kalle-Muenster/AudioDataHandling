// Settings for compiling WaveFileHandling.Dll (.NET dll)
// * These settings apply to 'COMPILE_WAVELIB' build (dont't include this when compiling applications *
// * which should use WaveLib, just include this for compiling WaveLib itself as .a .lib, .dll or .so)  *
#define COMPILE_WAVELIB

// settings which apply to the relaying 'WaveFileHandling' native (c++) sources in 'src' folder 
#define SET_WAVESPACE stepflow
#define DECLARE_24BIT_NAMESPACE stepflow
#define USE_NAMESPACER (1)

// this makes the *24bit audio data types* (which are separate (inline) include header) also being exported
// and copied, together with all other WaveLib library headers when these will be collected an copied all
// togeter with into the build output 'inc' folder 
#define NAMESPACE_API WAVELIB_API

// below settins apply for compiling .NET dll: 
// actually *.NET-framework* and *.NET-Core* are supported..
// ...maybe *Mono-framework* support may follow later...
#define unsafe (1)

// for enabling AudioFX features using *Stepflow Controlled Values* (inline include library)
// HAVE_CONTROLLED_VALUES may be set 1 as global compiler switch:
#define HAVE_CONTROLLED_VALUES (0)

#define COMMANDLINER_DISABLE_DEBUG (1)
