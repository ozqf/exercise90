@echo off
cls

@echo Build SDL Platform

if not exist bin mkdir bin
@rem cd bin
@rem del /s /q /f *.exe
@rem del /s /q /f *.exp
@rem cd..
if not exist build mkdir build
cd build
del *.* /Q

@rem @set EnableDelayedExpansion

@rem === COMPILER SETTINGS ===
set outputExe=/Fe../bin/cppTest.exe
set compilerFlags=-nologo -Gm -MT -WX -W4 -wd4100 -wd4201 -wd4189 /Zi
set compilerDefines=/DPARANOID=1
set compilerInput=../src/Platform/PlatformSDL/sdl_main.cpp 
@rem ../src/App/app_main.cpp

@rem === LINK SETTINGS ===
set linkStr=/link /SUBSYSTEM:CONSOLE
set linkInputA=../src/external/SDL2-2.0.7/lib/x86/SDL2main.lib
set linkInputB=../src/external/SDL2-2.0.7/lib/x86/SDL2.lib
@echo on
cl %compilerFlags% %compilerDefines% %outputExe% %compilerInput% %linkStr% %linkInputA% %linkInputB%
@echo off
set outputExe=
set compilerFlags=
set compilerDefines=
set compilerInput=

set linkStr=
set linkInputA=
set linkInputB=

@rem cl /LD /Fe../bin/gamex86.dll ../src/App/app_main.cpp
@rem @echo off
@rem @endlocal
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


@rem Disable local variable stuff so that directory change commands work again