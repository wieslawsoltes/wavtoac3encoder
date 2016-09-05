@echo on
setlocal

set PackageVersion=git-20091226-icl11_1_054
set PackageSrc=aften-%PackageVersion%-src
set PackageWin32=aften-%PackageVersion%-win32-bin
set PackageWin64=aften-%PackageVersion%-win64-bin
set Archiver_7Zip=C:\Program Files\7-Zip\7z.exe

FOR /D %%i IN (%PackageSrc%;%PackageWin32%;%PackageWin64%) DO (
  "%Archiver_7Zip%" a "%%i.zip" "%%i\*"
)

endlocal
