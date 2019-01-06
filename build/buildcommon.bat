@echo off

@echo --------------------------------------------------------
@echo Build Exercise 90 Common Library

cd..
if not exist bin mkdir bin
if not exist buildcommon mkdir buildcommon
cd buildcommon
del *.* /Q
@rem === COMPILER SETTINGS ===
@set output=/Fo../lib/common.obj
@rem main compile flags, elevating warnings
set compilerFlags=-nologo -Gm -MT -WX -W4 -wd4100 -wd4201 -wd4189 -wd4505 /Zi /c
@rem No elevated warnings
@rem set compilerFlags=-nologo -Gm -MT -W4 -wd4100 -wd4201 -wd4189 /Zi
set compilerDefines=/DPARANOID=1
@rem /DVERBOSE=1

@rem === Compile Common module
set compilerInput=../src/common/com_module.cpp

@echo on
@cl %compilerFlags% %compilerDefines% %compilerInput% %output%

@rem --
@rem lib.exe /OUT:common.lib com_module.obj
@echo off
set outputExe=
set compilerFlags=
set compilerDefines=
set compilerInput=

@cd..
@cd build
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
@rem /c -> No link
@rem /FoPathNameHere -> output object file
