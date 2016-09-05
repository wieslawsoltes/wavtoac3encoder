@echo off
setlocal

set BasePath=.
set BuildNames=EncoderBuilds.inc

set SolutionName=EncWAVtoAC3

setlocal

rem VS2008
@call "c:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86

FOR /F "tokens=*" %%i IN (%BuildNames%) DO (
    devenv /rebuild "%%i|Win32" "%BasePath%\%SolutionName%.sln"
)

endlocal

setlocal

rem VS2008
@call "c:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86_amd64

FOR /F "tokens=*" %%i IN (%BuildNames%) DO (
    devenv /rebuild "%%i|x64" "%BasePath%\%SolutionName%.sln"
)

endlocal
