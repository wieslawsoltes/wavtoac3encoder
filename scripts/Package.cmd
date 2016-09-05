@echo on
setlocal
@call PackageBinaries.cmd
@call PackageBinariesAMD64.cmd
@call PackageSources.cmd
@call PackageInstallers.cmd
endlocal
