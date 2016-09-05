@echo off
setlocal

set varVersion=%1
set varOutput=EncWAVtoAC3-%varVersion%-src
set varBase=..
set var7Zip=C:\Program Files\7-Zip\7z.exe

mkdir "%varOutput%"

copy "%varBase%\*.sln" "%varOutput%"
copy "%varBase%\*.md" "%varOutput%"
copy "%varBase%\*.txt" "%varOutput%"
copy "%varBase%\*.ps1" "%varOutput%"
copy "%varBase%\*.sh" "%varOutput%"
copy "%varBase%\*.cake" "%varOutput%"

mkdir "%varOutput%\lang"
copy "%varBase%\lang\*.txt" "%varOutput%\lang"

mkdir "%varOutput%\engines\ansi\Win32"
copy "%varBase%\engines\ansi\Win32\EncWAVtoAC3.engines" "%varOutput%\engines\ansi\Win32"

mkdir "%varOutput%\engines\ansi\Win64"
copy "%varBase%\engines\ansi\Win64\EncWAVtoAC3.engines" "%varOutput%\engines\ansi\Win64"

mkdir "%varOutput%\engines\unicode\Win32"
copy "%varBase%\engines\unicode\Win32\EncWAVtoAC3.engines" "%varOutput%\engines\unicode\Win32"

mkdir "%varOutput%\engines\unicode\Win64"
copy "%varBase%\engines\unicode\Win64\EncWAVtoAC3.engines" "%varOutput%\engines\unicode\Win64"

mkdir "%varOutput%\scripts"
copy "*.cmd" "%varOutput%\scripts"
copy "*.iss" "%varOutput%\scripts"
copy "EncWAVtoAC3.portable" "%varOutput%\scripts"

set varPath=src
mkdir "%varOutput%\%varPath%"

copy "%varBase%\%varPath%\*.vcxproj.filters" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\*.vcxproj.user" "%varOutput%\%varPath%"
copy "%varBase%\%varPath%\*.vcxproj" "%varOutput%\%varPath%"

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

"%var7Zip%" a "%varOutput%.zip" "%varOutput%\*"
rmdir /S /Q "%varOutput%"

endlocal
