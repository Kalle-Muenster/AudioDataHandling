#ifndef _WaveLib_Hpp_
#define _WaveLib_Hpp_

#include "precomphead.hpp"
#ifndef USE_PRE_COMPILED_HEADER
#include <limits.h>
#include <memory.h>
#include <exception>
#include <iostream>
#include <thread>
#include <WaveSpacer.h>
#include <WaveLib.inl/half.hpp>
#include <WaveLib.inl/numbersendian.h>
#include <WaveLib.inl/enumoperators.h>
#include <WaveLib.inl/indiaccessfuncs.h>
#include <WaveLib.inl/int24bittypes.hpp>
#include <WaveFileIO.h>
#endif

BEGIN_WAVESPACE
    WAVELIB_API unsigned     GetVersionNumber();
    WAVELIB_API const char*  GetVersionString();
ENDOF_WAVESPACE

#endif
