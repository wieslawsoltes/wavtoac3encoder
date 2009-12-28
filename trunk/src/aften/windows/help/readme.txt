Aften Windows Win32 and Win64 builds by Wiesław Šoltés <wieslaw.soltes@gmail.com>

Win32 and Win64 builds website:

 http://code.google.com/p/wavtoac3encoder/

What is Aften:

 Aften is an audio encoder which generates compressed audio streams 
 based on ATSC A/52 specification. This type of audio is also known 
 as AC-3 or Dolby® Digital and is one of the audio codecs used in DVD-Video content.

 Aften, Copyright (c) 2006-2007 Justin Ruggles <justinruggles@bellsouth.net>
                      2006-2007 Prakash Punnoor <prakash@punnoor.de>

 http://aften.sourceforge.net/

Win32 Builds (x86):

 .\wavfilter_x86\wavfilter.exe           - x86 ICL Build
 .\wavinfo_x86\wavinfo.exe               - x86 ICL Build
 .\wavrms_x86\wavrms.exe                 - x86 ICL Build
 .\libaftendll_x86\libaften.dll          - x86 ICL PGO Build with Parallelization Enabled
 .\libaftendll_x86_SSE\libaften.dll      - x86 ICL PGO SSE Build with Parallelization Enabled
 .\libaftendll_x86_SSE2\libaften.dll     - x86 ICL PGO SSE2 Build with Parallelization Enabled
 .\libaftendll_x86_SSE3\libaften.dll     - x86 ICL PGO SSE3 Build with Parallelization Enabled
 .\aften_x86\aften.exe                   - x86 ICL PGO Build with Parallelization Enabled
 .\aften_x86_SSE\aften.exe               - x86 ICL PGO SSE Build with Parallelization Enabled
 .\aften_x86_SSE2\aften.exe              - x86 ICL PGO SSE2 Build with Parallelization Enabled
 .\aften_x86_SSE3\aften.exe              - x86 ICL PGO SSE3 Build with Parallelization Enabled
 
Win64 Builds (AMD64):

 .\wavfilter_AMD64\wavfilter.exe         - AMD64 ICL Build
 .\wavinfo_AMD64\wavinfo.exe             - AMD64 ICL Build
 .\wavrms_AMD64\wavrms.exe               - AMD64 ICL Build
 .\libaftendll_AMD64\libaften.dll        - AMD64 ICL PGO Build with Parallelization Enabled
 .\libaftendll_AMD64_SSE2\libaften.dll   - AMD64 ICL PGO SSE2 Build with Parallelization Enabled
 .\libaftendll_AMD64_SSE3\libaften.dll   - AMD64 ICL PGO SSE3 Build with Parallelization Enabled
 .\aften_AMD64\aften.exe                 - AMD64 ICL PGO Build with Parallelization Enabled
 .\aften_AMD64_SSE2\aften.exe            - AMD64 ICL PGO SSE2 Build with Parallelization Enabled
 .\aften_AMD64_SSE3\aften.exe            - AMD64 ICL PGO SSE3 Build with Parallelization Enabled

Glossary:

 ICL	- Intel C++ Compiler
 PGO	- Profile-Guided Optimizations by Intel C++ Compiler (http://www.intel.com/)
 IPO	- Interprocedural Optimization by Intel C++ Compiler (http://www.intel.com/)
 MMX	- Assembly Opimizations for MMX (http://en.wikipedia.org/wiki/MMX)
 SSE	- Assembly Opimizations for SSE (http://en.wikipedia.org/wiki/Streaming_SIMD_Extensions)
 SSE2	- Assembly Opimizations for SSE2 (http://en.wikipedia.org/wiki/SSE2)
 SSE3	- Assembly Opimizations for SSE2 (http://en.wikipedia.org/wiki/SSE3)
 x86	- Binaries for 32-bit microprocessor architecture (http://en.wikipedia.org/wiki/X86), and
	  used under Miscorsoft Windows 32 bit operating systems.
 AMD64	- Binaries for 64-bit microprocessor architecture (http://en.wikipedia.org/wiki/AMD64),
	  used under Miscorsoft Windows 64 bit operating systems.

 Find more about Intel C++ Compiler: http://www.intel.com/cd/software/products/asmo-na/eng/compilers/cwin/279578.htm

Notes:
 
 Win32 builds will run under Win64 operating systems but Win64 will not run under Win32 OS's.
 SSE, SSE2 and SSE3 builds require compatible CPU's. Use http://www.cpuid.com/cpuz.php 
 program to check if your hardware is compatible with specific build.

 SSE3 builds include SSE3, SSE2, SSE and MMX optimizations.
 SSE2 builds include SSE2, SSE and MMX optimizations.
 SSE builds include SSE, MMX optimizations.

 For each libaften.dll build there is libaften.exp and libaften.lib available,
 to use it in your software.

Thanks,
Wiesław Šoltés