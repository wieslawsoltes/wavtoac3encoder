@echo off
setlocal

set EnginesFileName=EncWAVtoAC3.engines
set BasePathAftenBin=..\src\aften\windows\output
set BasePathUnicodeWin32=..\engines\unicode\Win32
set BasePathUnicodeWin64=..\engines\unicode\x64
set LibAftenDll=libaften.dll

set BasePath=..\src\bin\Debug\Win32
mkdir "%BasePath%\libaftendll_x86"
mkdir "%BasePath%\libaftendll_x86_SSE"
mkdir "%BasePath%\libaftendll_x86_SSE2"
mkdir "%BasePath%\libaftendll_x86_SSE3"
copy /Y "%BasePathAftenBin%\libaftendll_x86\%LibAftenDll%" "%BasePath%\libaftendll_x86\%LibAftenDll%"
copy /Y "%BasePathAftenBin%\libaftendll_x86_SSE\%LibAftenDll%" "%BasePath%\libaftendll_x86_SSE\%LibAftenDll%"
copy /Y "%BasePathAftenBin%\libaftendll_x86_SSE2\%LibAftenDll%" "%BasePath%\libaftendll_x86_SSE2\%LibAftenDll%"
copy /Y "%BasePathAftenBin%\libaftendll_x86_SSE3\%LibAftenDll%" "%BasePath%\libaftendll_x86_SSE3\%LibAftenDll%"
copy /Y "%BasePathUnicodeWin32%\%EnginesFileName%" "%BasePath%\%EnginesFileName%"

set BasePath=..\src\bin\Release\Win32
mkdir "%BasePath%\libaftendll_x86"
mkdir "%BasePath%\libaftendll_x86_SSE"
mkdir "%BasePath%\libaftendll_x86_SSE2"
mkdir "%BasePath%\libaftendll_x86_SSE3"
copy /Y "%BasePathAftenBin%\libaftendll_x86\%LibAftenDll%" "%BasePath%\libaftendll_x86\%LibAftenDll%"
copy /Y "%BasePathAftenBin%\libaftendll_x86_SSE\%LibAftenDll%" "%BasePath%\libaftendll_x86_SSE\%LibAftenDll%"
copy /Y "%BasePathAftenBin%\libaftendll_x86_SSE2\%LibAftenDll%" "%BasePath%\libaftendll_x86_SSE2\%LibAftenDll%"
copy /Y "%BasePathAftenBin%\libaftendll_x86_SSE3\%LibAftenDll%" "%BasePath%\libaftendll_x86_SSE3\%LibAftenDll%"
copy /Y "%BasePathUnicodeWin32%\%EnginesFileName%" "%BasePath%\%EnginesFileName%"

set BasePath=..\src\bin\Debug\x64
mkdir "%BasePath%\libaftendll_AMD64"
mkdir "%BasePath%\libaftendll_AMD64_SSE2"
mkdir "%BasePath%\libaftendll_AMD64_SSE3"
copy /Y "%BasePathAftenBin%\libaftendll_AMD64\%LibAftenDll%" "%BasePath%\libaftendll_AMD64\%LibAftenDll%"
copy /Y "%BasePathAftenBin%\libaftendll_AMD64_SSE2\%LibAftenDll%" "%BasePath%\libaftendll_AMD64_SSE2\%LibAftenDll%"
copy /Y "%BasePathAftenBin%\libaftendll_AMD64_SSE3\%LibAftenDll%" "%BasePath%\libaftendll_AMD64_SSE3\%LibAftenDll%"
copy /Y "%BasePathUnicodeWin64%\%EnginesFileName%" "%BasePath%\%EnginesFileName%"

set BasePath=..\src\bin\Release\x64
mkdir "%BasePath%\libaftendll_AMD64"
mkdir "%BasePath%\libaftendll_AMD64_SSE2"
mkdir "%BasePath%\libaftendll_AMD64_SSE3"
copy /Y "%BasePathAftenBin%\libaftendll_AMD64\%LibAftenDll%" "%BasePath%\libaftendll_AMD64\%LibAftenDll%"
copy /Y "%BasePathAftenBin%\libaftendll_AMD64_SSE2\%LibAftenDll%" "%BasePath%\libaftendll_AMD64_SSE2\%LibAftenDll%"
copy /Y "%BasePathAftenBin%\libaftendll_AMD64_SSE3\%LibAftenDll%" "%BasePath%\libaftendll_AMD64_SSE3\%LibAftenDll%"
copy /Y "%BasePathUnicodeWin64%\%EnginesFileName%" "%BasePath%\%EnginesFileName%"

endlocal
