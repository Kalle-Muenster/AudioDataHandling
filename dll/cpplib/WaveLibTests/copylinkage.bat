@echo off
set _arch_=%~1
set _plat_=%~2
set _conf_=%~3
set _root_=%~4..\..\..\bin
set _outp_=%~5

copy /Y "%_root_%\native\%_plat_%\%_arch_%\%_conf_%\WaveLib.*" "%_outp_%"
del /s /f /q "%_outp_%\WaveLib.hpp"
