@echo off
setlocal
set IssCompiler32=c:\Program Files (x86)\Inno Setup 5\Compil32.exe
"%IssCompiler32%" /cc "SetupScript.iss"
"%IssCompiler32%" /cc "SetupScriptAMD64.iss"
endlocal
