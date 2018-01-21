@echo off
cls

@echo Build SDL Platform

if not exist bin mkdir bin
if not exist build mkdir build
cd build
del *.* /Q
@rem === COMPILER SETTINGS ===
set outputExe=/Fe../bin/exercise90.exe
@rem main compile flags, elevating warnings
set compilerFlags=-nologo -Gm -MT -WX -W4 -wd4100 -wd4201 -wd4189 /Zi
@rem No elevated warnings
@rem set compilerFlags=-nologo -Gm -MT -W4 -wd4100 -wd4201 -wd4189 /Zi
set compilerDefines=/DPARANOID=1
@rem /DVERBOSE=1

@rem === Compile Win32 Window application
set compilerInput=../src/Platform/win32_main.cpp

@rem === Compile Testing Win32 Console application
@rem set compilerInput=../src/Platform/win32_consoleApp.cpp
@rem set linkStr=/link /SUBSYSTEM:CONSOLE

@rem === LINK SETTINGS === (disable if running win32 console application test)
@rem set linkStr=/link
set linkInputB=user32.lib opengl32.lib
set linkInputC=Gdi32.lib
@echo on
cl %compilerFlags% %compilerDefines% %outputExe% %compilerInput% %linkStr% %linkInputA% %linkInputB% %linkInputC%
@echo off
set outputExe=
set compilerFlags=
set compilerDefines=
set compilerInput=

set linkStr=
set linkInputA=
set linkInputB=
set linkInputC=

@cd..
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
@rem -wd4100 -> disable warning
@rem -wd4201 -> disable warning
@rem -wd4189 -> disable warning
@rem /Zi -> generate debug information
@rem /Fe -> specify file name and path
@rem -subsystem:windows,5.1 creates xp compatible windows program
@rem -MT package standard library into exe (increase compatibility)
@rem -Gm switch off minimal rebuild stuff (not using any of it)
@rem -Fm Create map file (contains addresses of all variables/functions)
@rem -opt:ref -> make compiler aggressive in removal of unused code
@rem /LD -> compile to DLL
