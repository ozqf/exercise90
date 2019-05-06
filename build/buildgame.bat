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

@rem -- Common module input --
set in1=../src/common/com_module.cpp

@rem -- Main App input --
set in2=../src/app/app_module.cpp ../src/app/app_textures.cpp
set in3=../src/sim/sim_module.cpp ../src/network/znet_module.cpp
set in4=../src/app/server/server.cpp ../src/app/client/client.cpp
set in5=../src/physics_wrapper/ZPhysics_module.cpp

@rem -- Stub App input --
@rem set in2=../src/app_stub/app_stub.cpp ../src/app/app_textures.cpp

set compIn=%in1% %in2% %in3% %in4% %in5%

set compOut=/Fe../bin/base/gamex86.dll

@rem /EHsc to avoid exception handling issues.
@rem Warnings as Errors
set compilerFlags=-nologo -Gm -MT -WX -W4 -wd4100 -wd4201 -wd4189 -wd4505 /Zi /EHsc

@rem No warning elevation
@rem set compilerFlags=-nologo -Gm -MT -W4 -wd4100 -wd4201 -wd4189 /Zi /EHsc
set compilerDefines=/DPARANOID=1
set linkInput=../lib/bullet/ZBulletPhysicsWrapper.lib
@echo on
@cl %compilerFlags% %compilerDefines% /LD %compIn% %compOut% %linkInput%

@set compIn=
@set compOut=
@set in1=
@set in2=
@set in3=
@set in4=
@set compilerFlags=
@set compilerDefines=
@set linkInput=

@cd..
@cd build

@echo on