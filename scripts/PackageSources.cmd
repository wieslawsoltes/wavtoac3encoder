@echo off
setlocal

set varWinRAR=C:\Program Files\WinRAR\WinRAR.exe

set varVersion=5.4
set varOutput=EncWAVtoAC3-%varVersion%-src
set varBase=..

mkdir "%varOutput%"

copy "%varBase%\*.sln" "%varOutput%"
copy "%varBase%\*.vcxproj.filters" "%varOutput%"
copy "%varBase%\*.vcxproj.user" "%varOutput%"
copy "%varBase%\*.vcxproj" "%varOutput%"

copy "%varBase%\*.cmd" "%varOutput%"
copy "%varBase%\*.inc" "%varOutput%"

copy "%varBase%\*.md" "%varOutput%"

set varPath=doc
mkdir "%varOutput%\%varPath%"

copy "%varBase%\%varPath%\*.txt" "%varOutput%\%varPath%"

set varPath=lang
mkdir "%varOutput%\%varPath%"

copy "%varBase%\%varPath%\*.txt" "%varOutput%\%varPath%"

set varPath=engines
mkdir "%varOutput%\%varPath%"

set varPath=engines\ansi
mkdir "%varOutput%\%varPath%"

set varPath=engines\ansi\Win32
mkdir "%varOutput%\%varPath%"

copy "%varBase%\%varPath%\EncWAVtoAC3.engines" "%varOutput%\%varPath%"

set varPath=engines\ansi\x64
mkdir "%varOutput%\%varPath%"

copy "%varBase%\%varPath%\EncWAVtoAC3.engines" "%varOutput%\%varPath%"

set varPath=engines\unicode
mkdir "%varOutput%\%varPath%"

set varPath=engines\unicode\Win32
mkdir "%varOutput%\%varPath%"

copy "%varBase%\%varPath%\EncWAVtoAC3.engines" "%varOutput%\%varPath%"

set varPath=engines\unicode\x64
mkdir "%varOutput%\%varPath%"

copy "%varBase%\%varPath%\EncWAVtoAC3.engines" "%varOutput%\%varPath%"

set varPath=installer
mkdir "%varOutput%\%varPath%"

copy "%varBase%\%varPath%\*.md" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\*.cmd" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\*.iss" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\EncWAVtoAC3.portable" "%varOutput%\%varPath%"

set varPath=src
mkdir "%varOutput%\%varPath%"

copy "%varBase%\%varPath%\*.cpp" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\*.h" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\*.rc" "%varOutput%\%varPath%"

set varPath=src\avisynth\src
mkdir "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\*.h" "%varOutput%\%varPath%"

set varPath=src\avisynth\src\core
mkdir "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\*.h" "%varOutput%\%varPath%"

set varPath=src\avisynth64\src
mkdir "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\*.h" "%varOutput%\%varPath%"

set varPath=src\avisynth64\src\core
mkdir "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\*.h" "%varOutput%\%varPath%"

set varPath=src\aften
mkdir "%varOutput%\%varPath%"

set varPath=src\aften\windows
mkdir "%varOutput%\%varPath%"

copy "%varBase%\%varPath%\*.cmd" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\*.txt" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\*.h" "%varOutput%\%varPath%"

set varPath=src\aften\windows\help
mkdir "%varOutput%\%varPath%"

copy "%varBase%\%varPath%\*.txt" "%varOutput%\%varPath%"

set varPath=src\aften\windows\output
mkdir "%varOutput%\%varPath%"

set varPath=src\res
mkdir "%varOutput%\%varPath%"

copy "%varBase%\%varPath%\EncWAVtoAC3.bmp" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\EncWAVtoAC3.ico" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\EncWAVtoAC3.rc2" "%varOutput%\%varPath%"

set varPath=Debug

mkdir "%varOutput%\%varPath%"

set varPath=Debug\Win32

mkdir "%varOutput%\%varPath%"

set varPath=Debug\x64

mkdir "%varOutput%\%varPath%"

set varPath=Release

mkdir "%varOutput%\%varPath%"

set varPath=Release\Win32

mkdir "%varOutput%\%varPath%"

set varPath=Release\x64

mkdir "%varOutput%\%varPath%"

rem "%varWinRAR%" a -ep1 -m5 -r -s -t "%varOutput%.rar" "%varOutput%\*"
"%varWinRAR%" a -ep1 -m5 -r -t "%varOutput%.zip" "%varOutput%\*"
rmdir /S /Q "%varOutput%"

endlocal
