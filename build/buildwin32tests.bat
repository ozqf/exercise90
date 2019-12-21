@echo off
cls

@echo --------------------------------------------------------
@echo Build Exercise 90 win32 tests

set BUILD_WIN32_TESTS_DIR=buildwin32tests

cd..
if not exist bin mkdir bin
if not exist %BUILD_WIN32_TESTS_DIR% mkdir %BUILD_WIN32_TESTS_DIR%
cd %BUILD_WIN32_TESTS_DIR%
del *.* /Q
@rem === COMPILER SETTINGS ===
set outputExe=/Fe../bin/test90.exe
@rem main compile flags, elevating warnings
set compilerFlags=-nologo -Gm -MT -WX -W4 -wd4100 -wd4201 -wd4189 -wd4505 /Zi
@rem No elevated warnings
@rem set compilerFlags=-nologo -Gm -MT -W4 -wd4100 -wd4201 -wd4189 /Zi
set compilerDefines=/DPARANOID=1
@rem /DVERBOSE=1

@rem === Compile Win32 Window application
@rem set compilerInput=../src/Platform/win32_main.cpp

@rem === Compile Testing Win32 Console application
@rem Ws2_32.lib == winsock2
set compInput_0=../src/tools/win32_consoleApp.cpp ../src/tools/tests/tests_module.cpp
@rem set compInput_1=../src/platform/win32_net/win32_net_module.cpp
set compInput_1=../src/common/com_module.cpp
set compInput_2=../src/network/znet_module.cpp ../src/proc_gen/ze_proc_gen.cpp

@rem === LINK SETTINGS === (disable if running win32 console application test)
@rem set linkStr=/link
@rem set linkInputB=user32.lib opengl32.lib
@rem set linkInputC=Gdi32.lib
set linkStr=/link /SUBSYSTEM:CONSOLE ../lib/zlib/zlibstatic.lib Ws2_32.lib

cl %compilerFlags% %compilerDefines% %outputExe% %compInput_0% %compInput_1% %compInput_2% %linkStr%
@if not %ERRORLEVEL% == 0 goto :FINISHED

@rem Auto run if you like
@call ../build/rtests.bat

@echo off

:FINISHED
@cd..
@cd build

set outputExe=
set compilerFlags=
set compilerDefines=
set compInput_0=
set compInput_1=

set linkStr=
set linkInputA=
set linkInputB=
set linkInputC=

@rem @cd..
@rem cd build

@echo on

@rem Project defines
@rem /DPARANOID=1 -> enable all asserts

@rem === COMPILER FLAGS ===
@rem -WX -> warnings as errors
@rem -W4 -> Max warning level that's sane
@rem -Oi -> enable compiler 'intrinsics'
@rem -GR -> turn off runtime type information (C++ feature)
@rem -EHa -> turn off exception handling
@rem -nologo -> disable compile command line header
@rem -wd4100 -> disable warning about unreferenced function parameters
@rem -wd4189 -> disable warning - "'identifier': local variable is initialized but not referenced"
@rem -wd4201 -> disable warning - "nonstandard extension used : nameless struct/union"
@rem -wd4505 -> disable warning about removal of unused internal functions
@rem /Zi -> generate debug information
@rem /Fe -> specify file name and path
@rem -subsystem:windows,5.1 creates xp compatible windows program
@rem -MT package standard library into exe (increase compatibility)
@rem -Gm switch off minimal rebuild stuff (not using any of it)
@rem -Fm Create map file (contains addresses of all variables/functions)
@rem -opt:ref -> make compiler aggressive in removal of unused code
@rem /LD -> compile to DLL
