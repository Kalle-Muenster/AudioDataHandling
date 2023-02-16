@echo off
set _output_=%~1
md %_output_%WaveLib.inl
pushd %_output_%
set _output_=%CD%
popd
pushd "%_output_%\WaveLib.inl"
md inc
md src
if "%~3"=="WITH_OUTPUT" (
    cd src
    md output
)
popd
pushd "%~2"
echo // This includes all header files it needs for>"%_output_%\wavelib.hpp"
echo // linking the native c++ WaveHandling library>>"%_output_%\wavelib.hpp"
echo #ifndef _wavelib_hpp_>>"%_output_%\wavelib.hpp"
echo #define _wavelib_hpp_>>"%_output_%\wavelib.hpp"
echo #include "./WaveLib.inl/inc/WaveFileIO.h">>"%_output_%\wavelib.hpp"
if "%~3"=="WITH_OUTPUT" (
    echo #include "./WaveLib.inl/inc/WaveHardIO.h">>"%_output_%\wavelib.hpp"
)
echo.>>"%_output_%\wavelib.hpp"
echo BEGIN_WAVESPACE>>"%_output_%\wavelib.hpp"
echo     WAVELIB_API unsigned     GetVersionNumber();>>"%_output_%\wavelib.hpp"
echo     WAVELIB_API const char*  GetVersionString();>>"%_output_%\wavelib.hpp"
echo ENDOF_WAVESPACE>>"%_output_%\wavelib.hpp"
echo.>>"%_output_%\wavelib.hpp"
echo #endif>>"%_output_%\wavelib.hpp"
set _output_=%_output_%\WaveLib.inl
echo #include "./src/settings.h">"%_output_%\settings.h"
cd..
cd..
set _rootdr_=%CD%
popd

set HALF16_TYPE=%~dp0..\..\..\..\Float16Type\Float16Type\src
set INT24_TYPES=%~dp0..\..\..\..\Int24Types\src

copy "%_rootdr_%\inc\settings.h" "%_output_%\inc"
copy "%_rootdr_%\inc\WaveFileIO.h" "%_output_%\inc"
copy "%_rootdr_%\inc\importsettings.h" "%_output_%\inc"
copy "%_rootdr_%\etc\namespacer.h" "%_output_%"
copy "%_rootdr_%\etc\enumoperators.h" "%_output_%"
copy "%_rootdr_%\etc\indiaccessfuncs.h" "%_output_%"
copy "%_rootdr_%\etc\numbersendian.h" "%_output_%"
copy "%INT24_TYPES%\int24bittypes.hpp" "%_output_%"
copy "%HALF16_TYPE%\half.hpp" "%_output_%"
copy "%HALF16_TYPE%\src\half.hpp" "%_output_%\src"
copy "%_rootdr_%\dll\cpplib\WaveLib\wavelib.hpp" "%_output_%\src"
copy "%_rootdr_%\dll\cpplib\WaveLib\precomphead.hpp" "%_output_%\src"
copy "%_rootdr_%\src\AudioChannel.hpp" "%_output_%\src"
copy "%_rootdr_%\src\AudioInline.hpp" "%_output_%\src"
copy "%_rootdr_%\src\CleanupMacros.h" "%_output_%\src"
copy "%_rootdr_%\src\Panorama.hpp" "%_output_%\src"
copy "%_rootdr_%\src\settings.h" "%_output_%\src"
copy "%_rootdr_%\src\WaveAudio.hpp" "%_output_%\src"
copy "%_rootdr_%\src\WaveBuffers.hpp" "%_output_%\src"
copy "%_rootdr_%\src\WaveFileReader.hpp" "%_output_%\src"
copy "%_rootdr_%\src\WaveFileWriter.hpp" "%_output_%\src"
copy "%_rootdr_%\src\WaveFormat.hpp" "%_output_%\src"
copy "%_rootdr_%\src\WaveHandling.hpp" "%_output_%\src"
copy "%_rootdr_%\src\WaveMacros.hpp" "%_output_%\src"
copy "%_rootdr_%\src\WaveSpacer.h" "%_output_%\src"
copy "%_rootdr_%\src\WaveStream.hpp" "%_output_%\src"
echo.
echo ###############################################################
echo # Copied all header files which are nessessary for importing  #
echo # the library into other projects into the libs output folder #
echo ###############################################################
echo.
