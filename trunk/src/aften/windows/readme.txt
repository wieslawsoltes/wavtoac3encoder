Windows (Win32 and Win64) build environment for aften and libaften.

Building aften and libaften using Intel C++ Compiler v11.1.035:

  Configure paths in 'feedback.cmd' and 'build_intel.cmd' batch scripts to
  match your system configuration and available compilers.

  Run 'build_win32_release.cmd' batch script to create Win32 Release binaries.
  Run 'build_win64_release.cmd' batch script to create Win64 Release binaries (req. Windows x64 version).

  Run 'build_win32_debug.cmd' batch script to create Win32 Debug binaries.
  Run 'build_win64_debug.cmd' batch script to create Win64 Debug binaries (req. Windows x64 version).

Notes:

 - Release binaries for windows are placed in 'output' sub-directories.

 - Informations about each build can be found at 'help/readme.txt' file.

 - Check 'build_intel.cmd' batch script to find more informations about the build system.

Thanks,
Wiesław Šoltés <wieslaw.soltes@gmail.com>