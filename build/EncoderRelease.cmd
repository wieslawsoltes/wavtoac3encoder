@echo off
setlocal

set BasePath=.

@call "%BasePath%\EncoderBuild.cmd"
@call "%BasePath%\EncoderPackages.cmd"
@call "%BasePath%\EncoderClean.cmd"

endlocal
