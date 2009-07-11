@echo off
setlocal

rem Win32 builds
rem Available Intel C++ Compiler versions: 11_1_035

set icl_version=11_1_035
set OutPath=output

@call build_intel.cmd %icl_version% wavfilter DEFAULT
@call build_intel.cmd %icl_version% wavinfo DEFAULT
@call build_intel.cmd %icl_version% wavrms DEFAULT

@call build_intel.cmd %icl_version% libaftendll DEFAULT
@call build_intel.cmd %icl_version% libaftendll SSE
@call build_intel.cmd %icl_version% libaftendll SSE2
@call build_intel.cmd %icl_version% libaftendll SSE3

@call build_intel.cmd %icl_version% aften DEFAULT
@call build_intel.cmd %icl_version% aften SSE
@call build_intel.cmd %icl_version% aften SSE2
@call build_intel.cmd %icl_version% aften SSE3

rem Copy additional files for binary package 
copy /Y ..\libaften\aften.h %OutPath%
copy /Y ..\COPYING %OutPath%
copy /Y "help\*.txt" %OutPath%

:build_done

endlocal
pause