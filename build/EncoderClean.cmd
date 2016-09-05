@echo off
setlocal

set BasePath=.
set BuildNames=EncoderBuilds.inc

@del /Q "%BasePath%\*.aps"
@del /Q "%BasePath%\*.sdf"
@del /Q "%BasePath%\*.ncb"
@del /Q "%BasePath%\src\*.aps"
@del /Q /AH "%BasePath%\*.suo"
@del /Q "%BasePath%\*.log"

@rmdir /S /Q %BasePath%\ipch"


FOR /F "tokens=*" %%i IN (%BuildNames%) DO (
    @del "%BasePath%\%%i\EncWAVtoAC3.exe"
    @del "%BasePath%\%%i\*.htm"
    @del "%BasePath%\%%i\*.idb"
    @del "%BasePath%\%%i\*.ilk"
    @del "%BasePath%\%%i\*.obj"
    @del "%BasePath%\%%i\*.pch"
    @del "%BasePath%\%%i\*.pdb"
    @del "%BasePath%\%%i\*.res"
    @del "%BasePath%\%%i\*.manifest"
    @del "%BasePath%\%%i\*.dep"
    @del "%BasePath%\%%i\*.config
    @del "%BasePath%\%%i\*.dll
    @del "%BasePath%\%%i\*.files
    @del "%BasePath%\%%i\*.presets
    @del "%BasePath%\%%i\*.log
    @del "%BasePath%\%%i\*.tlog
    @del "%BasePath%\%%i\*.rc
    @del "%BasePath%\%%i\*.timestamp
)

endlocal
