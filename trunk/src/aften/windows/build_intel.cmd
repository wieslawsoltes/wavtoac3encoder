@echo off
setlocal

rem
rem USAGE: @call build_intel.cmd <compiler_version> <build_name> <optimizations> [<debug_flag>]
rem

title Building Aften Win32 and Win64 Binaries (C) 2006-2008 Wieslaw Soltes

rem
rem SELECT COMPILER
rem

rem Select proper compiler environment variables
goto use_intel_compiler_%1

:use_intel_compiler_11_1_035

rem Set environment variables for Intel C++ Compiler v11.1.035 (x86 PGO, MMX, SSE, SSE2 and SSE3 Builds)
@call "C:\Program Files\Intel\Compiler\11.1\035\bin\ia32\iclvars_ia32.bat"
goto common_settings

:use_intel_compiler_11_1_035_x64

rem Set environment variables for Intel C++ Compiler v11.1.035 (x86-64, EM64T PGO, MMX, SSE, SSE2 and SSE3 Builds)
@call "C:\Program Files\Intel\Compiler\11.1\035\bin\ia32_intel64\iclvars_ia32_intel64.bat"
goto common_settings

rem
rem PREPARE BUILD NAMES
rem

:common_settings

rem Set working directory path and output directory prefix
set out_path=output

rem Set output directory names for release binaries

if /i "11_1_035_x64"=="%1" set build_type=AMD64
if /i "11_1_035"=="%1" set build_type=x86

if /i "DEFAULT"=="%3" (
  set build_opt=
) else (
  set build_opt=_%3
)

if /i "DEBUG"=="%4" (
  set build_opt=%build_opt%_DEBUG
) else (
  set build_opt=%build_opt%
)

set bin_wavfilter=wavfilter_%build_type%
set bin_wavinfo=wavinfo_%build_type%
set bin_wavrms=wavrms_%build_type%
set bin_libaftendll=libaftendll_%build_type%%build_opt%
set bin_aften=aften_%build_type%%build_opt%

rem Set base names for output binaries
set name_lib=libaften
set name_dll=libaften
set name_exe=aften

rem
rem PREPARE TOOLS
rem

rem Set command-line tools paths used for builds
set cmd_icl=icl.exe
set cmd_xilib=xilib.exe
set cmd_xilink=xilink.exe

rem
rem PREPARE OPTIONS
rem

rem Set Compiler common command-line options
set opt_icl_cmn=/c /TC /nologo /W3

if /i "DEBUG"=="%4" (
  set opt_icl_cmn=%opt_icl_cmn% /MTd
) else (
  set opt_icl_cmn=%opt_icl_cmn% /MT
)

set opt_icl_cmn=%opt_icl_cmn% /I "./" /I "../" /I "../libaften" /I "../libaften/x86/" /I "../pcm/" /Fo"%out_path%/" 

if /i "DEBUG"=="%4" (
  set opt_icl_cmn=%opt_icl_cmn% /D "_DEBUG"
) else (
  set opt_icl_cmn=%opt_icl_cmn% /D "NDEBUG"
)

set opt_icl_cmn=%opt_icl_cmn% /D "_CONSOLE" /D "_MBCS" /D "EMULATE_INTTYPES" /D "AFTEN_BUILD_LIBRARY" /D "MAX_NUM_THREADS=32" /D "HAVE_MM_MALLOC" /D "HAVE_CPU_CAPS_DETECTION" /D "HAVE_WINDOWS_THREADS" /D "_CRT_SECURE_NO_DEPRECATE"
set opt_icl_cmn=%opt_icl_cmn% /Qwd177 /Qwd186 /Qwd188 /Qwd1478

if /i "11_1_035"=="%1" set opt_icl_cmn=%opt_icl_cmn% /Gd /D "WIN32"
if /i "11_1_035_x64"=="%1" set opt_icl_cmn=%opt_icl_cmn% /D "WIN32" /D "WIN64"

rem Set Compiler optimization command-line options
if /i "DEBUG"=="%4" (
  set opt_icl_opt=%opt_icl_cmn% /Od /ZI /RTC1
) else (
  set opt_icl_opt=%opt_icl_cmn% /GA /Ox /Og /Ob1 /Ot     
)

if /i "11_1_035"=="%1" set opt_icl_opt=%opt_icl_opt% /Gd /Qipo4 /fp:fast /Qprec-div- /Qcomplex-limited-range /Qipo-jobs:8 /Qunroll-aggressive
if /i "11_1_035_x64"=="%1" set opt_icl_opt=%opt_icl_opt% /Qipo4 /fp:fast /Qprec-div- /Qcomplex-limited-range /Qipo-jobs:8 /Qunroll-aggressive

set opt_icl_opt=%opt_icl_opt% /D "HAVE_MMX" /D "HAVE_SSE" /D "HAVE_SSE2" /D "HAVE_SSE3"

if /i "DEFAULT"=="%3" set opt_icl_opt=%opt_icl_opt% /G7
if /i "SSE"=="%3" set opt_icl_opt=%opt_icl_opt% /G7 arch:SSE
if /i "SSE2"=="%3" set opt_icl_opt=%opt_icl_opt% /G7 /arch:SSE2
if /i "SSE3"=="%3" set opt_icl_opt=%opt_icl_opt% /G7 /arch:SSE3

set opt_icl_mmx=/D "USE_MMX"
set opt_icl_sse=/D "USE_MMX" /D "USE_SSE"
set opt_icl_sse2=/D "USE_MMX" /D "USE_SSE" /D "USE_SSE2"
set opt_icl_sse3=/D "USE_MMX" /D "USE_SSE" /D "USE_SSE2" /D "USE_SSE3"

rem Set Compiler PGO command-line options
set opt_icl_pgo_gen=/Qprof-gen /Qprof_dir "%out_path%"
set opt_icl_pgo_use=/Qprof_use /Qprof_dir "%out_path%"

set opt_icl_lib=/D "_LIB"
set opt_icl_dll=/D "_WINDLL"

rem Set Library Manager command-line options
set opt_xilib=/OUT:"%out_path%/%name_lib%.lib" /NOLOGO

rem Set Linker command-line options
set opt_xilink=/NOLOGO /INCREMENTAL:NO /OPT:REF /OPT:ICF /TLBID:1

rem DEBUG!!!
if /i "DEBUG"=="%4" set opt_xilink=%opt_xilink% /DEBUG

if /i "11_1_035"=="%1" set opt_xilink=%opt_xilink% /MACHINE:IX86
if /i "11_1_035_x64"=="%1" set opt_xilink=%opt_xilink% /MACHINE:X64

set opt_xilink_exe=%opt_xilink% /OUT:"%out_path%/%name_exe%.exe" /SUBSYSTEM:CONSOLE "%out_path%\%name_lib%.lib"

set opt_xilink_dll=%opt_xilink% /OUT:"%out_path%/%name_dll%.dll" /DLL /IMPLIB:"%out_path%\%name_lib%.lib"

rem
rem PREPARE SOURCES
rem

rem Set libaften sources

set src_lib="../libaften/a52.c"
set src_lib=%src_lib% "../libaften/a52enc.c"
set src_lib=%src_lib% "../libaften/a52tab.c"
set src_lib=%src_lib% "../libaften/bitalloc.c"
set src_lib=%src_lib% "../libaften/bitio.c"
set src_lib=%src_lib% "../libaften/convert.c"
set src_lib=%src_lib% "../libaften/crc.c"
set src_lib=%src_lib% "../libaften/dynrng.c"
set src_lib=%src_lib% "../libaften/exponent.c"
set src_lib=%src_lib% "../libaften/filter.c"
set src_lib=%src_lib% "../libaften/mdct.c"
set src_lib=%src_lib% "../libaften/util.c"
set src_lib=%src_lib% "../libaften/window.c"
set src_lib=%src_lib% "../libaften/x86/cpu_caps.c"

rem Set MMX optimized sources
set src_lib_mmx="../libaften/x86/exponent_mmx.c"

rem Set SSE optimized sources
set src_lib_sse="../libaften/x86/mdct_sse.c"
set src_lib_sse=%src_lib_sse% "../libaften/x86/mdct_common_sse.c"
set src_lib_sse=%src_lib_sse% "../libaften/x86/window_sse.c"

rem Set SSE2 optimized sources
set src_lib_sse2="../libaften/x86/exponent_sse2.c"

rem Set SSE3 optimized sources
set src_lib_sse3="../libaften/x86/mdct_sse3.c"

rem Set PCM sources
set src_pcm="../pcm/aiff.c"
set src_pcm=%src_pcm% "../pcm/byteio.c"
set src_pcm=%src_pcm% "../pcm/caff.c"
set src_pcm=%src_pcm% "../pcm/convert.c"
set src_pcm=%src_pcm% "../pcm/formats.c"
set src_pcm=%src_pcm% "../pcm/pcm.c"
set src_pcm=%src_pcm% "../pcm/pcm_io.c"
set src_pcm=%src_pcm% "../pcm/pcmfile.c"
set src_pcm=%src_pcm% "../pcm/raw.c"
set src_pcm=%src_pcm% "../pcm/wav.c"

rem Set Aften sources
set src_exe="../aften/aften.c"
set src_exe=%src_exe% "../aften/opts.c"
set src_exe=%src_exe% %src_pcm%

rem
rem PREPARE OBJECTS
rem

rem Set libaften object files

set obj_lib="%out_path%/a52.obj"
set obj_lib=%obj_lib% "%out_path%/a52enc.obj"
set obj_lib=%obj_lib% "%out_path%/a52tab.obj"
set obj_lib=%obj_lib% "%out_path%/bitalloc.obj"
set obj_lib=%obj_lib% "%out_path%/bitio.obj"
set obj_lib=%obj_lib% "%out_path%/convert.obj"
set obj_lib=%obj_lib% "%out_path%/crc.obj"
set obj_lib=%obj_lib% "%out_path%/dynrng.obj"
set obj_lib=%obj_lib% "%out_path%/exponent.obj"
set obj_lib=%obj_lib% "%out_path%/filter.obj"
set obj_lib=%obj_lib% "%out_path%/mdct.obj"
set obj_lib=%obj_lib% "%out_path%/util.obj"
set obj_lib=%obj_lib% "%out_path%/window.obj"
set obj_lib=%obj_lib% "%out_path%/cpu_caps.obj"

rem Set MMX object files
set obj_lib_mmx=%obj_lib% "%out_path%/exponent_mmx.obj"

rem Set SSE object files
set obj_lib_sse=%obj_lib_mmx% "%out_path%/mdct_sse.obj"
set obj_lib_sse=%obj_lib_sse% "%out_path%/mdct_common_sse.obj"
set obj_lib_sse=%obj_lib_sse% "%out_path%/window_sse.obj"

rem Set SSE2 object files
set obj_lib_sse2=%obj_lib_sse% "%out_path%/exponent_sse2.obj"

rem Set SSE3 object files
set obj_lib_sse3=%obj_lib_sse2% "%out_path%/mdct_sse3.obj"

rem Set PCM object files
set obj_pcm="%out_path%/aiff.obj"
set obj_pcm=%obj_pcm% "%out_path%/byteio.obj"
set obj_pcm=%obj_pcm% "%out_path%/caff.obj"
set obj_pcm=%obj_pcm% "%out_path%/convert.obj"
set obj_pcm=%obj_pcm% "%out_path%/formats.obj"
set obj_pcm=%obj_pcm% "%out_path%/pcm.obj"
set obj_pcm=%obj_pcm% "%out_path%/pcm_io.obj"
set obj_pcm=%obj_pcm% "%out_path%/pcmfile.obj"
set obj_pcm=%obj_pcm% "%out_path%/raw.obj"
set obj_pcm=%obj_pcm% "%out_path%/wav.obj"

rem Set Aften object files
set obj_exe="%out_path%/aften.obj"
set obj_exe=%obj_exe% "%out_path%/opts.obj"
set obj_exe=%obj_exe% %obj_pcm%

rem
rem CLEAN COMMANDS
rem

set cmd_clean_pgo_gen=del /Q
set cmd_clean_pgo_gen=%cmd_clean_pgo_gen% "%out_path%\*.obj"
set cmd_clean_pgo_gen=%cmd_clean_pgo_gen% "%out_path%\%name_lib%.lib"
set cmd_clean_pgo_gen=%cmd_clean_pgo_gen% "%out_path%\%name_exe%.exe"
set cmd_clean_pgo_gen=%cmd_clean_pgo_gen% "%out_path%\%name_exe%.lib"
set cmd_clean_pgo_gen=%cmd_clean_pgo_gen% "%out_path%\%name_exe%.exp"

set cmd_clean_pgo_use=del /Q
set cmd_clean_pgo_use=%cmd_clean_pgo_use% "%out_path%\*.obj"
set cmd_clean_pgo_use=%cmd_clean_pgo_use% "%out_path%\*.dpi"
set cmd_clean_pgo_use=%cmd_clean_pgo_use% "%out_path%\*.dyn"
set cmd_clean_pgo_use=%cmd_clean_pgo_use% "%out_path%\*.spi"
set cmd_clean_pgo_use=%cmd_clean_pgo_use% "%out_path%\*.spl"
set cmd_clean_pgo_use=%cmd_clean_pgo_use% "%out_path%\*.lock"

set cmd_clean_pgo_use_exe=%cmd_clean_pgo_use% "%out_path%\%name_lib%.lib"

rem
rem SELECT OUTPUT BUILD
rem

:make_output_path

rem Create output directory
if not exist "%out_path%" mkdir "%out_path%"

rem Go to selected build configuration
if /i "%2"=="" goto all_done
goto %2

rem
rem BUILD UTIL
rem

:wavfilter

if not exist "%out_path%\%bin_wavfilter%\" mkdir "%out_path%\%bin_wavfilter%\"

%cmd_icl% %opt_icl_opt% /I "../util" /I "../aften/" /I "../pcm/" "../util/wavfilter.c" "../libaften/filter.c" %src_pcm%
%cmd_xilink% %opt_xilink% /OUT:"%out_path%/wavfilter.exe" /SUBSYSTEM:CONSOLE "%out_path%/wavfilter.obj" "%out_path%/filter.obj" %obj_pcm%

del /Q "%out_path%\*.obj"
move /Y "%out_path%\wavfilter.exe" "%out_path%\%bin_wavfilter%\"
if /i "DEBUG"=="%4" move /Y "%out_path%\wavfilter.pdb" "%out_path%\%bin_wavfilter%\"

goto all_done

:wavinfo

if not exist "%out_path%\%bin_wavinfo%\" mkdir "%out_path%\%bin_wavinfo%\"

%cmd_icl%%opt_icl_opt% /I "../util" /I "../aften/" /I "../pcm/" "../util/wavinfo.c" %src_pcm%
%cmd_xilink% %opt_xilink% /OUT:"%out_path%/wavinfo.exe" /SUBSYSTEM:CONSOLE "%out_path%/wavinfo.obj" %obj_pcm%

del /Q "%out_path%\*.obj"
move /Y "%out_path%\wavinfo.exe" "%out_path%\%bin_wavinfo%\"
if /i "DEBUG"=="%4" move /Y "%out_path%\wavinfo.pdb" "%out_path%\%bin_wavinfo%\"

goto all_done

:wavrms

if not exist "%out_path%\%bin_wavrms%\" mkdir "%out_path%\%bin_wavrms%\"

%cmd_icl% %opt_icl_opt% /I "../util" /I "../aften/" /I "../pcm/" "../util/wavrms.c" %src_pcm%
%cmd_xilink% %opt_xilink% /OUT:"%out_path%/wavrms.exe" /SUBSYSTEM:CONSOLE "%out_path%/wavrms.obj" %obj_pcm%

del /Q "%out_path%\*.obj"
move /Y "%out_path%\wavrms.exe" "%out_path%\%bin_wavrms%\"
if /i "DEBUG"=="%4" move /Y "%out_path%\wavrms.pdb" "%out_path%\%bin_wavrms%\"

goto all_done

rem
rem BUILD LIBAFTEN.DLL
rem

:libaftendll

if not exist "%out_path%\%bin_libaftendll%\" mkdir "%out_path%\%bin_libaftendll%\"

%cmd_icl% %opt_icl_opt% %opt_icl_pgo_gen% %src_lib% %opt_icl_dll%
%cmd_icl% %opt_icl_opt% %opt_icl_mmx% %opt_icl_pgo_gen% %src_lib_mmx% %opt_icl_lib%
%cmd_icl% %opt_icl_opt% %opt_icl_sse% %opt_icl_pgo_gen% %src_lib_sse% %opt_icl_lib%
%cmd_icl% %opt_icl_opt% %opt_icl_sse2% %opt_icl_pgo_gen% %src_lib_sse2% %opt_icl_lib%
%cmd_icl% %opt_icl_opt% %opt_icl_sse3% %opt_icl_pgo_gen% %src_lib_sse3% %opt_icl_lib%
%cmd_xilink% %opt_xilink_dll% %obj_lib_sse3%
%cmd_icl% %opt_icl_opt% %opt_icl_pgo_gen% %src_exe%
%cmd_xilink% %opt_xilink_exe% %obj_exe%

@call feedback.cmd "%out_path%\%name_exe%.exe"

%cmd_clean_pgo_gen%

%cmd_icl% %opt_icl_opt% %opt_icl_pgo_use% %src_lib% %opt_icl_dll%
%cmd_icl% %opt_icl_opt% %opt_icl_mmx% %opt_icl_pgo_use% %src_lib_mmx% %opt_icl_lib%
%cmd_icl% %opt_icl_opt% %opt_icl_sse% %opt_icl_pgo_use% %src_lib_sse% %opt_icl_lib%
%cmd_icl% %opt_icl_opt% %opt_icl_sse2% %opt_icl_pgo_use% %src_lib_sse2% %opt_icl_lib%
%cmd_icl% %opt_icl_opt% %opt_icl_sse3% %opt_icl_pgo_use% %src_lib_sse3% %opt_icl_lib%
%cmd_xilink% %opt_xilink_dll% %obj_lib_sse3%
%cmd_icl% %opt_icl_opt% %opt_icl_pgo_use% %src_exe%
%cmd_xilink% %opt_xilink_exe% %obj_exe%

%cmd_clean_pgo_use%

move /Y "%out_path%\%name_dll%.dll" "%out_path%\%bin_libaftendll%\"
move /Y "%out_path%\%name_dll%.lib" "%out_path%\%bin_libaftendll%\"
move /Y "%out_path%\%name_dll%.exp" "%out_path%\%bin_libaftendll%\"
move /Y "%out_path%\%name_exe%.exe" "%out_path%\%bin_libaftendll%\"
if /i "DEBUG"=="%4" move /Y "%out_path%\%name_dll%.pdb" "%out_path%\%bin_libaftendll%\"
if /i "DEBUG"=="%4" move /Y "%out_path%\%name_exe%.pdb" "%out_path%\%bin_libaftendll%\"

goto all_done

rem
rem BUILD AFTEN.EXE
rem

:aften

if not exist "%out_path%\%bin_aften%\" mkdir "%out_path%\%bin_aften%\"

%cmd_icl% %opt_icl_opt% %opt_icl_pgo_gen% %src_lib% %opt_icl_lib%
%cmd_icl% %opt_icl_opt% %opt_icl_mmx% %opt_icl_pgo_gen% %src_lib_mmx% %opt_icl_lib%
%cmd_icl% %opt_icl_opt% %opt_icl_sse% %opt_icl_pgo_gen% %src_lib_sse% %opt_icl_lib%
%cmd_icl% %opt_icl_opt% %opt_icl_sse2% %opt_icl_pgo_gen% %src_lib_sse2% %opt_icl_lib%
%cmd_icl% %opt_icl_opt% %opt_icl_sse3% %opt_icl_pgo_gen% %src_lib_sse3% %opt_icl_lib%
%cmd_xilib% %opt_xilib% %obj_lib_sse3%
%cmd_icl% %opt_icl_opt% %opt_icl_pgo_gen% %src_exe%
%cmd_xilink% %opt_xilink_exe% %obj_exe%

@call feedback.cmd "%out_path%\%name_exe%.exe"

%cmd_clean_pgo_gen%

%cmd_icl% %opt_icl_opt% %opt_icl_pgo_use% %src_lib% %opt_icl_lib%
%cmd_icl% %opt_icl_opt% %opt_icl_mmx% %opt_icl_pgo_use% %src_lib_mmx% %opt_icl_lib%
%cmd_icl% %opt_icl_opt% %opt_icl_sse% %opt_icl_pgo_use% %src_lib_sse% %opt_icl_lib%
%cmd_icl% %opt_icl_opt% %opt_icl_sse2% %opt_icl_pgo_use% %src_lib_sse2% %opt_icl_lib%
%cmd_icl% %opt_icl_opt% %opt_icl_sse3% %opt_icl_pgo_use% %src_lib_sse3% %opt_icl_lib%
%cmd_xilib% %opt_xilib% %obj_lib_sse3%
%cmd_icl% %opt_icl_opt% %opt_icl_pgo_use% %src_exe%
%cmd_xilink% %opt_xilink_exe% %obj_exe%

%cmd_clean_pgo_use_exe%

move /Y "%out_path%\%name_exe%.lib" "%out_path%\%bin_aften%\"
move /Y "%out_path%\%name_exe%.exp" "%out_path%\%bin_aften%\"
move /Y "%out_path%\%name_exe%.exe" "%out_path%\%bin_aften%\"
if /i "DEBUG"=="%4" move /Y "%out_path%\%name_exe%.pdb" "%out_path%\%bin_aften%\"

goto all_done

:all_done

endlocal