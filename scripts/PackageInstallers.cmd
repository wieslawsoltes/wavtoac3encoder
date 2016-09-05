@echo on
setlocal
set IssCompiler=c:\Program Files (x86)\Inno Setup 5\ISCC.exe
"%IssCompiler%" "SetupScript.iss"
"%IssCompiler%" "SetupScriptAMD64.iss"
endlocal
