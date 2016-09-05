@echo off
setlocal

set varWinRAR=c:\Program Files\WinRAR\WinRAR.exe

set varVersion=5.4
set varOutputVer=EncWAVtoAC3-%varVersion%-AMD64
set varBase=..
set varPathAftenBin=src\aften\windows\output

set varOutput=%varOutputVer%
mkdir "%varOutput%"
mkdir "%varOutput%\Lang"
mkdir "%varOutput%\libaftendll_AMD64"
mkdir "%varOutput%\libaftendll_AMD64_SSE2"
mkdir "%varOutput%\libaftendll_AMD64_SSE3"

copy "%varBase%\ReadMe.md" "%varOutput%"

set varPath=doc
copy "%varBase%\%varPath%\Changes.txt" "%varOutput%"
copy "%varBase%\%varPath%\Copying.txt" "%varOutput%"
copy "%varBase%\%varPath%\Keyboard.txt" "%varOutput%"
copy "%varBase%\%varPath%\Command-Line.txt" "%varOutput%"

set varPath=Lang
copy "%varBase%\%varPath%\*.txt" "%varOutput%\%varPath%"

copy "%varBase%\Release\x64\EncWAVtoAC3.exe" "%varOutput%"
copy "%varBase%\%varPathAftenBin%\libaftendll_AMD64\libaften.dll" "%varOutput%\libaftendll_AMD64\libaften.dll"
copy "%varBase%\%varPathAftenBin%\libaftendll_AMD64_SSE2\libaften.dll" "%varOutput%\libaftendll_AMD64_SSE2\libaften.dll"
copy "%varBase%\%varPathAftenBin%\libaftendll_AMD64_SSE3\libaften.dll" "%varOutput%\libaftendll_AMD64_SSE3\libaften.dll"
copy "%varBase%\engines\unicode\x64\EncWAVtoAC3.engines" "%varOutput%"
copy "%varBase%\installer\EncWAVtoAC3.portable" "%varOutput%"

rem "%varWinRAR%" a -ep1 -m5 -r -s -t "%varOutput%.rar" "%varOutput%\*"
"%varWinRAR%" a -ep1 -m5 -r -t "%varOutput%.zip" "%varOutput%\*"
rmdir /S /Q "%varOutput%"

endlocal
