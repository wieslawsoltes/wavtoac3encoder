@echo off
setlocal

set wav_file=t.wav
set ac3_file=t.ac3
set timer_exe=timer.exe

set OutPath=output

echo [aften_x86]
%timer_exe% "%OutPath%/aften_x86/aften.exe" "%wav_file%" "%ac3_file%"
echo [aften_x86_SSE]
%timer_exe% "%OutPath%/aften_x86_SSE/aften.exe" "%wav_file%" "%ac3_file%"
echo [aften_x86_SSE2]
%timer_exe% "%OutPath%/aften_x86_SSE2/aften.exe" "%wav_file%" "%ac3_file%"
echo [aften_x86_SSE3]
%timer_exe% "%OutPath%/aften_x86_SSE3/aften.exe" "%wav_file%" "%ac3_file%"

del /Q "%ac3_file%"

endlocal