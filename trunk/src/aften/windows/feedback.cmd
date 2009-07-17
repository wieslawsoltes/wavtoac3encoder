@echo off
setlocal

rem Currently using this wave file:
rem http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/Samples/Microsoft/6_Channel_ID.wav
rem Mono WAV files are extracted from 6_Channel_ID.wav WAV file

set WavFiles=c:\DOWNLOADS\DEV\WAV
set WavFiles_Mono=c:\DOWNLOADS\DEV\WAV\MONO

set EncoderExe=%1
set EncoderOptions=feedback.txt
set OutputFile=temp.ac3

FOR /F "tokens=*" %%i IN (%EncoderOptions%) DO (
  echo [ Multi channel input WAV, used encoder options: %%i ]
  %EncoderExe% %%i "%WavFiles%\6_Channel_ID.wav" "%OutputFile%"
  del "%OutputFile%"
  echo [ Multi mono input WAV, used encoder options: %%i ]
  %EncoderExe% %%i -ch_fl "%WavFiles_Mono%\FL.wav" -ch_fr "%WavFiles_Mono%\FR.wav" -ch_fc "%WavFiles_Mono%\C.wav" -ch_lfe "%WavFiles_Mono%\LFE.wav" -ch_sl "%WavFiles_Mono%\SL.wav" -ch_sr "%WavFiles_Mono%\SR.wav" "%OutputFile%"
  del "%OutputFile%"
)

endlocal