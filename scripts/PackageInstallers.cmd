@echo off
setlocal

set varVersion=%1
set IssCompiler=c:\Program Files (x86)\Inno Setup 5\ISCC.exe

"%IssCompiler%" "SetupScript.iss" /DVERSION=%varVersion%
"%IssCompiler%" "SetupScriptAMD64.iss" /DVERSION=%varVersion%

endlocal
