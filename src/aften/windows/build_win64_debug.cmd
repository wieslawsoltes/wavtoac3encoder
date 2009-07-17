@echo off
setlocal

rem Win64 builds
rem Available Intel C++ Compiler versions: 11_1_035_x64

set icl_version=11_1_035_x64
set OutPath=output

@call build_intel.cmd %icl_version% wavfilter DEFAULT DEBUG
@call build_intel.cmd %icl_version% wavinfo DEFAULT DEBUG
@call build_intel.cmd %icl_version% wavrms DEFAULT DEBUG

@call build_intel.cmd %icl_version% libaftendll DEFAULT DEBUG
@call build_intel.cmd %icl_version% libaftendll SSE3 DEBUG

@call build_intel.cmd %icl_version% aften DEFAULT DEBUG
@call build_intel.cmd %icl_version% aften SSE3 DEBUG

rem Copy additional files for binary package
copy /Y ..\libaften\aften.h %OutPath%
copy /Y ..\COPYING %OutPath%
copy /Y "help\*.txt" %OutPath%

:build_done

endlocal
pause