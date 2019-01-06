@echo off

@rem echo Setup vs
@rem @call vsvars2015

@echo --------------------------------------------------------
@echo Build Exercise 90 Game dll

cd..
if not exist bin mkdir bin
cd bin
if not exist base mkdir base
cd..

if not exist buildGame mkdir buildGame
cd buildGame
del *.* /q

@rem set compIn=../src/App/game/game_module.cpp ../src/App/app_module.cpp ../src/App/Physics/ZPhysics_module.cpp ../src/network/znet_module.cpp
set in1=../src/network/znet_module.cpp ../src/common/com_module.cpp
set in2=../src/app/app_module.cpp ../src/app/app_textures.cpp
set in3=../src/sim/sim_module.cpp
set compOut=/Fe../bin/base/gamex86.dll

@rem /EHsc to avoid exception handling issues.
@rem Warnings as Errors
set compilerFlags=-nologo -Gm -MT -WX -W4 -wd4100 -wd4201 -wd4189 -wd4505 /Zi /EHsc

@rem No warning elevation
@rem set compilerFlags=-nologo -Gm -MT -W4 -wd4100 -wd4201 -wd4189 /Zi /EHsc
set compilerDefines=/DPARANOID=1
set linkInput=../lib/bullet/ZBulletPhysicsWrapper.lib
@echo on
@cl %compilerFlags% %compilerDefines% /LD %in1% %in2% %in3% %compOut% %linkInput%

@set compilerFlags=
@set compilerDefines=

@cd..
@cd build

@echo on