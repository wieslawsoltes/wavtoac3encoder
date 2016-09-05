@echo off
setlocal

set PackageVersion=git-20091226-icl11_1_054
set PackageSrc=aften-%PackageVersion%-src
set PackageWin32=aften-%PackageVersion%-win32-bin
set PackageWin64=aften-%PackageVersion%-win64-bin
set Archiver_7Zip=C:\Program Files\7-Zip\7z.exe
set Archiver_WinRAR=c:\Program Files\WinRAR\WinRAR.exe
set Options_WinRAR_zip=a -ep1 -m5 -r -t
set Options_WinRAR_rar=a -ep1 -m5 -r -s -t
set Options_7Zip_7z=a -r0 -t7z -mx=5 -ms=on -mmt=on

FOR /D %%i IN (%PackageSrc%;%PackageWin32%;%PackageWin64%) DO (
  "%Archiver_WinRAR%" %Options_WinRAR_zip% "%%i.zip" "%%i\*"
)

endlocal
