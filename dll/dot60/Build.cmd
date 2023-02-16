@if "%ECHO_STATE%"=="" (@echo off ) else (@echo %ECHO_STATE% )

:: Prepare locations
set _name_=WaveFileHandling
set _call_=%CD%
cd %~dp0
set _here_=%CD%
cd..
cd..
set _root_=%CD%

:: Set VersionNumber
set WaveLibVersionNumber=00000002
set WaveLibVersionString=0.0.0.2
set DotNetVersionString=dot60

:: Set Dependencies
if "%ConsolaBinRoot%"=="" (
set ConsolaBinRoot=%_root_%\..\Consola\bin\%DotNetVersionString%
)
if "%Int24TypesBinRoot%"=="" (
set Int24TypesBinRoot=%_root_%\..\Int24Types\bin\%DotNetVersionString%
)
if "%Float16TypeBinRoot%"=="" (
set Float16TypeBinRoot=%_root_%\..\Float16Type\bin\%DotNetVersionString%
)
if "%ControlledValuesBinRoot%"=="" (
set ControlledValuesBinRoot=%_root_%\..\ControlledValues\bin\%DotNetVersionString%
)

:: Set parameters and solution files
call %_root_%\Args "%~1" "%~2" "%~3" "%~4" WaveFileHandling.sln WaveLibTestDot60.sln

:: Do the Build
cd %_here_%
call MsBuild %_target_% %_args_%
cd %_call_%

:: Cleanup Environment
call %_root_%\Args ParameterCleanUp

