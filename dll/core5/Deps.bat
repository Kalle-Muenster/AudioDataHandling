@echo off

set ConsolaProject=%~dp0..\..\..\Consola\ConsolaCore5
set Int24TypesProject=%~dp0..\..\..\Int24Types\core5
set Float16TypeProject=%~dp0..\..\..\Float16Type
set ControllerProject=%~dp0..\..\..\ControlledValues\Core5Dll

set ARCH=%~1
set CONF=%~2
set CLEAN=%~3

pushd %ConsolaProject%
call Build.cmd "%ARCH%" "%CONF%" %CLEAN%
call Build.cmd "%ARCH%" "%CONF%" Test %CLEAN%
popd

pushd "%Int24TypesProject%"
call Build.cmd "%ARCH%" "%CONF%" %CLEAN%
popd

pushd "%Float16TypeProject%"
call Build.cmd "%ARCH%" "%CONF%" %CLEAN%
popd

pushd "%ControllerProject%"
call Build.cmd "%ARCH%" "%CONF%" %CLEAN%
popd

pushd "%~dp0"
call Build.cmd "%ARCH%" "%CONF%" %CLEAN%
call Build.cmd "%ARCH%" "%CONF%" Test %CLEAN%
popd

set ARCH=
set CONF=
set CLEAN=
