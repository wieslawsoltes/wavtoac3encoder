# CMake project file by Prakash Punnoor
CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

IF(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

IF(NOT DEFINED CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
ENDIF(NOT DEFINED CMAKE_BUILD_TYPE)

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules")
Project(Aften C)

SET(SO_MAJOR_VERSION "0")
SET(SO_MINOR_VERSION "0")
SET(SO_BUILD_VERSION "8")
SET(SO_VERSION "${SO_MAJOR_VERSION}.${SO_MINOR_VERSION}.${SO_BUILD_VERSION}")

IF(${Aften_SOURCE_DIR} MATCHES ${Aften_BINARY_DIR})
  MESSAGE("Please do an out-of-tree build:")
  MESSAGE("rm -f CMakeCache.txt; mkdir -p default; cd default; cmake ..; make")
  MESSAGE(FATAL_ERROR "in-tree-build detected")
ENDIF(${Aften_SOURCE_DIR} MATCHES ${Aften_BINARY_DIR})

INCLUDE(${CMAKE_ROOT}/Modules/TestBigEndian.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/CheckCSourceCompiles.cmake)
INCLUDE(${CMAKE_MODULE_PATH}/ConfigHelper.cmake)
INCLUDE(${CMAKE_MODULE_PATH}/HeaderTests.cmake)
INCLUDE(${CMAKE_MODULE_PATH}/FlagsTests.cmake)
INCLUDE(${CMAKE_MODULE_PATH}/CompilerVisibility.cmake)
INCLUDE(${CMAKE_MODULE_PATH}/CompilerSIMD.cmake)
INCLUDE(${CMAKE_MODULE_PATH}/DetectCompiler.cmake)

OPTION(SHARED "build shared Aften library" OFF)
OPTION(DOUBLE "build Aften with doubles instead of floats" OFF)
IF(DOUBLE)
  ADD_DEFINE(CONFIG_DOUBLE)
ENDIF(DOUBLE)
OPTION(BINDINGS_CS "build C# bindings" OFF)
OPTION(BINDINGS_CXX "build C++ bindings" OFF)
IF(BINDINGS_CXX)
  SET(SHARED ON CACHE BOOL "build shared Aften library" FORCE)
ENDIF(BINDINGS_CXX)

INCLUDE_DIRECTORIES(${Aften_BINARY_DIR}/)
INCLUDE_DIRECTORIES(${Aften_SOURCE_DIR}/)
INCLUDE_DIRECTORIES(${Aften_SOURCE_DIR}/libaften)
INCLUDE_DIRECTORIES(${Aften_SOURCE_DIR}/aften)
INCLUDE_DIRECTORIES(${Aften_SOURCE_DIR}/pcm)

SET(LIBAFTEN_SRCS libaften/a52enc.h
                  libaften/a52enc.c
                  libaften/a52.h
                  libaften/a52.c
                  libaften/a52tab.h
                  libaften/a52tab.c
                  libaften/bitalloc.h
                  libaften/bitalloc.c
                  libaften/bitio.h
                  libaften/bitio.c
                  libaften/crc.h
                  libaften/crc.c
                  libaften/dynrng.h
                  libaften/dynrng.c
                  libaften/window.h
                  libaften/window.c
                  libaften/mdct.h
                  libaften/mdct.c
                  libaften/exponent.h
                  libaften/exponent.c
                  libaften/filter.h
                  libaften/filter.c
                  libaften/util.c
                  libaften/convert.h
                  libaften/convert.c
                  libaften/threading.h
                  libaften/a52dec.h
                  libaften/aften.h
                  libaften/aften-types.h
                  libaften/cpu_caps.h
                  libaften/mem.h
                  common.h
                  bswap.h)

SET(LIBAFTEN_X86_SRCS libaften/x86/cpu_caps.c
                      libaften/x86/cpu_caps.h
                      libaften/x86/asm_common.h
                      libaften/x86/asm_support.h
                      libaften/x86/gas_support.h
                      libaften/x86/intelas_support.h)

SET(LIBAFTEN_X86_MMX_SRCS libaften/x86/exponent_mmx.c
                          libaften/x86/exponent.h
                          libaften/x86/simd_support.h)

SET(LIBAFTEN_X86_SSE_SRCS libaften/x86/mdct_sse.c
                          libaften/x86/mdct_common_sse.c
                          libaften/x86/mdct_common_sse.h
                          libaften/x86/mdct.h
                          libaften/x86/window_sse.c
                          libaften/x86/window.h
                          libaften/x86/simd_support.h)

SET(LIBAFTEN_X86_SSE2_SRCS libaften/x86/exponent_sse2.c
                           libaften/x86/exponent.h
                           libaften/x86/simd_support.h)

SET(LIBAFTEN_X86_SSE3_SRCS libaften/x86/mdct_sse3.c
                           libaften/x86/mdct_common_sse.h
                           libaften/x86/mdct.h
                           libaften/x86/simd_support.h)

SET(LIBAFTEN_PPC_SRCS libaften/ppc/cpu_caps.c
                      libaften/ppc/cpu_caps.h)

SET(LIBAFTEN_ALTIVEC_SRCS libaften/ppc/mdct_altivec.c
                          libaften/ppc/mdct.h
                          libaften/ppc/altivec_common.h)

SET(AFTEN_SRCS aften/aften.c
               aften/opts.c
               aften/opts.h
               aften/helptext.h)

SET(PCM_SRCS pcm/aiff.c
             pcm/byteio.c
             pcm/byteio.h
             pcm/caff.c
             pcm/convert.c
             pcm/formats.c
             pcm/formats.h
             pcm/pcm.c
             pcm/pcm.h
             pcm/pcmfile.c
             pcm/pcmfile.h
             pcm/pcm_io.c
             pcm/raw.c
             pcm/wav.c)


IF(CMAKE_UNAME)
  EXEC_PROGRAM(uname ARGS -m OUTPUT_VARIABLE CMAKE_SYSTEM_MACHINE)
ELSE(CMAKE_UNAME)
  IF(CMAKE_SYSTEM MATCHES "Windows" AND CMAKE_SYSTEM_PROCESSOR MATCHES "x86")
	IF(CMAKE_SIZEOF_VOID_P MATCHES "8")
      SET(CMAKE_SYSTEM_MACHINE "x86_64")
	ELSE(CMAKE_SIZEOF_VOID_P MATCHES "8")
      SET(CMAKE_SYSTEM_MACHINE "i386")
	ENDIF(CMAKE_SIZEOF_VOID_P MATCHES "8")
  ELSE(CMAKE_SYSTEM MATCHES "Windows" AND CMAKE_SYSTEM_PROCESSOR MATCHES "x86")
	MESSAGE(FATAL_ERROR "Could not detect machine type")
  ENDIF(CMAKE_SYSTEM MATCHES "Windows" AND CMAKE_SYSTEM_PROCESSOR MATCHES "x86")
ENDIF(CMAKE_UNAME)

DETECT_COMPILER()

CHECK_64BITS()

IF(CMAKE_COMPILER_IS_GNUCC)
  SET(CMAKE_C_FLAGS_RELEASE "-funroll-loops -fomit-frame-pointer ${CMAKE_C_FLAGS_RELEASE}")
  SET(ADD_CFLAGS "-pedantic -Wall -Wpointer-arith -Wredundant-decls -Wformat -Wunused ${ADD_CFLAGS}")

  # gcc 2.9.5 doesn't know following
  CHECK_CFLAGS_APPEND(ADD_EXCLUSIVE_CFLAGS "-std=gnu99")
  CHECK_CFLAGS_APPEND(ADD_CFLAGS "-Wdisabled-optimization")
  CHECK_CFLAGS_APPEND(ADD_CFLAGS "-Wfloat-equal")
  CHECK_CFLAGS_APPEND(ADD_EXCLUSIVE_CFLAGS "-Wbad-function-cast")

  # gcc 3.0 doesn't know following
  CHECK_CFLAGS_APPEND(ADD_EXCLUSIVE_CFLAGS "-Wdeclaration-after-statement")
  CHECK_CFLAGS_APPEND(ADD_CFLAGS "-Wextra")
  CHECK_CFLAGS_APPEND(ADD_CFLAGS "-Wno-switch")

  # gcc 4.1 doesn't know following
  CHECK_CFLAGS_APPEND(ADD_CFLAGS "-Wstrict-overflow")
ENDIF(CMAKE_COMPILER_IS_GNUCC)

IF(CMAKE_COMPILER_IS_ICC)
  SET(CMAKE_C_FLAGS_RELEASE "-O3 -funroll-loops -fomit-frame-pointer ${CMAKE_C_FLAGS_RELEASE}")
  SET(ADD_CFLAGS "-std=c99 -Wall -wd810 ${ADD_CFLAGS}")
ENDIF(CMAKE_COMPILER_IS_ICC)

IF(MSVC)
  # msvc ignores bogus options, so detection makes no sense
  SET(CMAKE_C_FLAGS_RELEASE "/Ot /Oy /Oi /Ox ${CMAKE_C_FLAGS_RELEASE}")
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
ENDIF(MSVC)

TEST_BIG_ENDIAN(CMAKE_WORDS_BIGENDIAN)
IF(CMAKE_WORDS_BIGENDIAN)
  ADD_DEFINE(WORDS_BIGENDIAN)
ENDIF(CMAKE_WORDS_BIGENDIAN)

IF(UNIX)
  SET(LIBM "m")

  TEST_COMPILER_VISIBILITY()

  IF(APPLE)
    ADD_DEFINE(SYS_DARWIN)
  ELSE(APPLE)
    CHECK_FUNCTION_DEFINE("#include <sys/sysinfo.h>" "get_nprocs" "()" HAVE_GET_NPROCS)

    IF(NOT HAVE_GET_NPROCS)
      MESSAGE(STATUS "Hardcoding 2 threads usage")
      ADD_DEFINE("NUM_THREADS 2")
    ENDIF(NOT HAVE_GET_NPROCS)
  ENDIF(APPLE)
ENDIF(UNIX)

# threads handling
FIND_PACKAGE(Threads)
IF(CMAKE_USE_WIN32_THREADS_INIT)
  ADD_DEFINE(HAVE_WINDOWS_THREADS)
  IF(MINGW)
    ADD_DEFINE(MINGW_ALIGN_STACK_HACK)
  ENDIF(MINGW)
ELSE(CMAKE_USE_WIN32_THREADS_INIT)

IF(CMAKE_USE_PTHREADS_INIT)
  ADD_DEFINE(HAVE_POSIX_THREADS)
  SET(ADD_LIBS ${ADD_LIBS} ${CMAKE_THREAD_LIBS_INIT})
ENDIF(CMAKE_USE_PTHREADS_INIT)
ENDIF(CMAKE_USE_WIN32_THREADS_INIT)

ADD_DEFINE("MAX_NUM_THREADS 32")

ADD_DEFINITIONS(-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE)

CHECK_INCLUDE_FILE_DEFINE(inttypes.h HAVE_INTTYPES_H)
CHECK_INCLUDE_FILE_DEFINE(byteswap.h HAVE_BYTESWAP_H)

# output GIT version to config.h
EXECUTE_PROCESS(COMMAND git log -1 --pretty=format:%h
OUTPUT_VARIABLE GIT_VERSION ERROR_QUIET)
IF(GIT_VERSION)
  ADD_DEFINE("GIT_VERSION \"${GIT_VERSION}\"")
ENDIF(GIT_VERSION)

CHECK_POSIX_MEMALIGN()

# do SIMD stuff
IF(CMAKE_SYSTEM_MACHINE MATCHES "i.86" OR CMAKE_SYSTEM_MACHINE MATCHES "x86_64")
  CHECK_MMX()
  # MSVC doesn't have MMX support in x64 mode, so check for SSE independently
  CHECK_SSE()
  
  IF(HAVE_MMX OR HAVE_SSE)
    INCLUDE_DIRECTORIES(${Aften_SOURCE_DIR}/libaften/x86)
    SET(LIBAFTEN_SRCS ${LIBAFTEN_SRCS} ${LIBAFTEN_X86_SRCS})
    ADD_DEFINE(HAVE_CPU_CAPS_DETECTION)
  ENDIF(HAVE_MMX OR HAVE_SSE)
  
  IF(HAVE_MMX)
    SET(LIBAFTEN_SRCS ${LIBAFTEN_SRCS} ${LIBAFTEN_X86_MMX_SRCS})
      SET(SIMD_FLAGS "${SIMD_FLAGS} ${MMX_FLAGS} -DUSE_MMX")
      FOREACH(SRC ${LIBAFTEN_X86_MMX_SRCS})
        SET_SOURCE_FILES_PROPERTIES(${SRC} PROPERTIES COMPILE_FLAGS ${SIMD_FLAGS})
      ENDFOREACH(SRC)
    ADD_DEFINE(HAVE_MMX)
  ENDIF(HAVE_MMX)

  IF(HAVE_SSE)
    SET(SIMD_FLAGS "${SIMD_FLAGS} ${SSE_FLAGS} -DUSE_SSE")
    IF(NOT DOUBLE)
      SET(LIBAFTEN_SRCS ${LIBAFTEN_SRCS} ${LIBAFTEN_X86_SSE_SRCS})
      FOREACH(SRC ${LIBAFTEN_X86_SSE_SRCS})
        SET_SOURCE_FILES_PROPERTIES(${SRC} PROPERTIES COMPILE_FLAGS ${SIMD_FLAGS})
      ENDFOREACH(SRC)
    ENDIF(NOT DOUBLE)
    ADD_DEFINE(HAVE_SSE)

    CHECK_SSE2()
    IF(HAVE_SSE2)
      SET(SIMD_FLAGS "${SIMD_FLAGS} ${SSE2_FLAGS} -DUSE_SSE2")
      SET(LIBAFTEN_SRCS ${LIBAFTEN_SRCS} ${LIBAFTEN_X86_SSE2_SRCS})
      FOREACH(SRC ${LIBAFTEN_X86_SSE2_SRCS})
        SET_SOURCE_FILES_PROPERTIES(${SRC} PROPERTIES COMPILE_FLAGS ${SIMD_FLAGS})
      ENDFOREACH(SRC)
      ADD_DEFINE(HAVE_SSE2)

      CHECK_SSE3()
      IF(HAVE_SSE3)
        SET(SIMD_FLAGS "${SIMD_FLAGS} ${SSE3_FLAGS} -DUSE_SSE3")
        IF(NOT DOUBLE)
          SET(LIBAFTEN_SRCS ${LIBAFTEN_SRCS} ${LIBAFTEN_X86_SSE3_SRCS})
          FOREACH(SRC ${LIBAFTEN_X86_SSE3_SRCS})
            SET_SOURCE_FILES_PROPERTIES(${SRC} PROPERTIES COMPILE_FLAGS ${SIMD_FLAGS})
          ENDFOREACH(SRC)
        ENDIF(NOT DOUBLE)
        ADD_DEFINE(HAVE_SSE3)

        CHECK_CASTSI128()
      ENDIF(HAVE_SSE3)
    ENDIF(HAVE_SSE2)
  ENDIF(HAVE_SSE)
ENDIF(CMAKE_SYSTEM_MACHINE MATCHES "i.86" OR CMAKE_SYSTEM_MACHINE MATCHES "x86_64")

IF(CMAKE_SYSTEM_MACHINE MATCHES "Power Macintosh" OR CMAKE_SYSTEM_MACHINE MATCHES "ppc")
  CHECK_ALTIVEC()

  IF(HAVE_ALTIVEC AND NOT DOUBLE)
    INCLUDE_DIRECTORIES(${Aften_SOURCE_DIR}/libaften/ppc)
    SET(LIBAFTEN_SRCS ${LIBAFTEN_SRCS} ${LIBAFTEN_PPC_SRCS})
    SET(LIBAFTEN_SRCS ${LIBAFTEN_SRCS} ${LIBAFTEN_ALTIVEC_SRCS})
    FOREACH(SRC ${LIBAFTEN_ALTIVEC_SRCS})
      SET_SOURCE_FILES_PROPERTIES(${SRC} PROPERTIES COMPILE_FLAGS "-maltivec")
    ENDFOREACH(SRC)
    ADD_DEFINE(HAVE_ALTIVEC)

  ENDIF(HAVE_ALTIVEC AND NOT DOUBLE)
ENDIF(CMAKE_SYSTEM_MACHINE MATCHES "Power Macintosh" OR CMAKE_SYSTEM_MACHINE MATCHES "ppc")


GENERATE_CONFIG_H()

SET(CMAKE_C_FLAGS "${ADD_CFLAGS} ${ADD_EXCLUSIVE_CFLAGS} ${CMAKE_C_FLAGS}")

IF(SHARED)
  ADD_LIBRARY(aften SHARED ${LIBAFTEN_SRCS})
  SET_TARGET_PROPERTIES(aften PROPERTIES VERSION ${SO_VERSION} SOVERSION ${SO_MAJOR_VERSION})
  SET_TARGET_PROPERTIES(aften PROPERTIES LINKER_LANGUAGE C)
  SET_TARGET_PROPERTIES(aften PROPERTIES DEFINE_SYMBOL AFTEN_BUILD_LIBRARY)
  TARGET_LINK_LIBRARIES(aften ${LIBM} ${ADD_LIBS})
  SET(INSTALL_TARGETS ${INSTALL_TARGETS} aften)
ENDIF(SHARED)

ADD_LIBRARY(aften_static STATIC ${LIBAFTEN_SRCS})
#SET_TARGET_PROPERTIES(aften_static PROPERTIES OUTPUT_NAME aften)
SET_TARGET_PROPERTIES(aften_static PROPERTIES LINKER_LANGUAGE C)
SET_TARGET_PROPERTIES(aften_static PROPERTIES COMPILE_FLAGS -DAFTEN_BUILD_LIBRARY)
TARGET_LINK_LIBRARIES(aften_static ${LIBM} ${ADD_LIBS})

# building a separate static lib for the pcm audio decoder
ADD_LIBRARY(aften_pcm STATIC ${PCM_SRCS})

ADD_EXECUTABLE(aften_exe ${AFTEN_SRCS})
SET_TARGET_PROPERTIES(aften_exe PROPERTIES OUTPUT_NAME aften)
SET_TARGET_PROPERTIES(aften_exe PROPERTIES LINKER_LANGUAGE C)
IF(WIN32)
  # When linking to static aften, dllimport mustn't be used
  SET_TARGET_PROPERTIES(aften_exe PROPERTIES COMPILE_FLAGS -DAFTEN_BUILD_LIBRARY)
ENDIF(WIN32)
TARGET_LINK_LIBRARIES(aften_exe aften_pcm aften_static)

ADD_EXECUTABLE(wavinfo util/wavinfo.c)
TARGET_LINK_LIBRARIES(wavinfo aften_pcm)

ADD_EXECUTABLE(wavrms util/wavrms.c)
TARGET_LINK_LIBRARIES(wavrms aften_pcm ${LIBM})

ADD_EXECUTABLE(wavfilter util/wavfilter.c libaften/filter.c)
TARGET_LINK_LIBRARIES(wavfilter aften_pcm ${LIBM})

IF(BINDINGS_CXX)
  MESSAGE("## WARNING: The C++ bindings are only lightly tested. Feed-back appreciated. ##")
  Project(Aften CXX)
  SET(CMAKE_CXX_FLAGS_RELEASE "${ADD_CFLAGS} ${CMAKE_C_FLAGS_RELEASE}")
  INCLUDE_DIRECTORIES(${Aften_SOURCE_DIR}/bindings)
  ADD_LIBRARY(aftenxx SHARED bindings/aftenxx.cxx)
  SET_TARGET_PROPERTIES(aftenxx PROPERTIES VERSION ${SO_VERSION} SOVERSION ${SO_MAJOR_VERSION})
  SET_TARGET_PROPERTIES(aftenxx PROPERTIES LINKER_LANGUAGE CXX)
  SET_TARGET_PROPERTIES(aftenxx PROPERTIES DEFINE_SYMBOL AFTENXX_BUILD_LIBRARY)
  TARGET_LINK_LIBRARIES(aftenxx aften)
  SET(INSTALL_TARGETS ${INSTALL_TARGETS} aftenxx)
  SET(INSTALL_HEADERS ${INSTALL_HEADERS} bindings/aftenxx.h)
ENDIF(BINDINGS_CXX)
IF(BINDINGS_CS)
  MESSAGE("## WARNING: The C# bindings are not completed, but tested (see \"API C#.txt\"). ##")
  SET(AFTENSHARP_SRCS bindings/cs/AftenTypes.cs
                      bindings/cs/FrameEncoder.cs
                      bindings/cs/FrameEventArgs.cs
                      bindings/cs/Utility.cs)
  IF(WIN32)
    SET(WIN_SRCS "")
    FOREACH(SRC ${AFTENSHARP_SRCS})
      STRING(REPLACE "/" "\\" WIN_SRC ${SRC})
      SET(WIN_SRCS ${WIN_SRCS} ${WIN_SRC})
    ENDFOREACH(SRC ${AFTENSHARP_SRCS})
    SET(AFTENSHARP_SRCS ${WIN_SRCS})
  ENDIF(WIN32)
  SET(AFTENSHARP_DLL ${Aften_BINARY_DIR}/AftenSharp.dll)
  FIND_PROGRAM(
    CSCOMPILER
    NAMES csc gmcs)
  IF(CSCOMPILER MATCHES "NOTFOUND")
    MESSAGE(SEND_ERROR "Could not locate a supported C# compiler")
  ENDIF(CSCOMPILER MATCHES "NOTFOUND")
  ADD_CUSTOM_COMMAND(
    OUTPUT ${AFTENSHARP_DLL}
    COMMAND ${CSCOMPILER} -t:library -out:${AFTENSHARP_DLL} -o+ ${AFTENSHARP_SRCS}
    DEPENDS ${AFTENSHARP_SRCS}
    WORKING_DIRECTORY ${Aften_SOURCE_DIR})
  ADD_CUSTOM_TARGET(
    AftenSharp
    ALL
    DEPENDS ${AFTENSHARP_DLL})
ENDIF(BINDINGS_CS)


INSTALL(TARGETS ${INSTALL_TARGETS} aften_exe wavinfo wavrms wavfilter
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib${LIB_SUFFIX}
        ARCHIVE DESTINATION lib${LIB_SUFFIX})
INSTALL(FILES ${INSTALL_HEADERS} libaften/aften.h libaften/aften-types.h DESTINATION include/aften)
