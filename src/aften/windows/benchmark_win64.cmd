@echo off
setlocal

set wav_file=t.wav
set ac3_file=t.ac3
set timer_exe=timer.exe

set OutPath=output

echo [aften_AMD64]
%timer_exe% "%OutPath%/aften_AMD64/aften.exe" "%wav_file%" "%ac3_file%"
echo [aften_AMD64_SSE2]
%timer_exe% "%OutPath%/aften_AMD64_SSE2/aften.exe" "%wav_file%" "%ac3_file%"
echo [aften_AMD64_SSE3]
%timer_exe% "%OutPath%/aften_AMD64_SSE3/aften.exe" "%wav_file%" "%ac3_file%"

del /Q "%ac3_file%"

endlocal