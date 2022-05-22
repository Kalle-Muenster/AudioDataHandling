#define SET_WAVESPACE stepflow

// below settins apply for compiling .NET dll: 
// actually *.NET-framework* is supported 
// (but .NET-Core support may follow...)
#define unsafe (1)
//#define SET_POOLBOTTOM AudioFilenameWorkBuffer

// for enabling AudioFX features using stepflow controlled types
// HAVE_CONTROLLED_VALUES may be set 1 as global compiler switch:
#define HAVE_CONTROLLED_VALUES (0)
#define THREAD_WAITCYCLE_TIME EMPTY

