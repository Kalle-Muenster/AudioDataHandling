@echo off

set _name_=ControlledValues
set _call_=%CD%
cd %~dp0
set _here_=%CD%
set _root_=%CD%

:: Set Version
set ControlledVersionNumber=00000003
set ControlledVersionString=0.0.0.3


call MsBuild WaveFileHandling.sln /p:Platform=x64;Configuration=Release
call MsBuild WaveFileHandling.sln /p:Platform=x64;Configuration=Debug
call MsBuild WaveFileHandling.sln /p:Platform=x64;Configuration=StaticRelease
call MsBuild WaveFileHandling.sln /p:Platform=x64;Configuration=StaticDebug

call MsBuild WaveFileHandling.sln /p:Platform=x86;Configuration=Release
call MsBuild WaveFileHandling.sln /p:Platform=x86;Configuration=Debug
call MsBuild WaveFileHandling.sln /p:Platform=x86;Configuration=StaticRelease
call MsBuild WaveFileHandling.sln /p:Platform=x86;Configuration=StaticDebug

set _root_=
set _here_=
set _name_=
set _call_=