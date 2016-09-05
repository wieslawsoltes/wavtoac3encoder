@echo off
setlocal

set BasePath=.

cd "%BasePath%\installer"

@call "PackageBinaries.cmd"
@call "PackageBinariesAMD64.cmd"
@call "PackageSources.cmd"
@call "PackageInstallers.cmd"

endlocal
