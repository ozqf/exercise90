@echo off

@rem echo Setup vs
@rem @call vsvars2015

@echo --------------------------------------------------------
@echo Build Exercise 90 Game dll

cd..
if not exist bin mkdir bin
cd bin
if not exist stub mkdir stub
cd..

if not exist buildStub mkdir buildStub
cd buildStub
del *.* /q

@rem -- Common module input --
set in1=../src/common/com_module.cpp

@rem -- Main App input --
@rem set in2=../src/app/app_module.cpp ../src/app/app_textures.cpp
@rem set in3=../src/sim/sim_module.cpp ../src/network/znet_module.cpp
@rem set in4=../src/app/server/server.cpp ../src/app/client/client.cpp

@rem -- Stub App input --
set in2=../src/app_stub/app_stub.cpp ../src/app/app_textures.cpp

set compIn=%in1% %in2% %in3% %in4%

set compOut=/Fe../bin/stub/gamex86.dll

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
@set int1=
@set int2=
@set int3=
@set int4=
@set compilerFlags=
@set compilerDefines=
@set linkInput=

@cd..
@cd build

@echo on