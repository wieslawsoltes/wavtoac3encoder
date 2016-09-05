@echo off
setlocal

set varVersion=%1

@call PackageBinaries.cmd %varVersion%
@call PackageBinariesAMD64.cmd %varVersion%
@call PackageSources.cmd %varVersion%
@call PackageInstallers.cmd %varVersion%

endlocal
