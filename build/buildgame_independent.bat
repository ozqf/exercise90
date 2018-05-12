@echo off

@echo Setup vs
@call vsvars2015

@echo -----------------------------
@echo Build Exercise 90 Game dll

cd..
if not exist bin mkdir bin
cd bin
if not exist base mkdir base
cd..

if not exist buildGame mkdir buildGame
cd buildGame
del *.* /q

set compIn=../src/App/app_main.cpp ../src/App/Physics/ZPhysics.cpp
set compOut=/Fe../bin/base/gamex86.dll

@rem /EHsc to avoid exception handling issues.
@rem Warnings as Errors
set compilerFlags=-nologo -Gm -MT -WX -W4 -wd4100 -wd4201 -wd4189 /Zi /EHsc

@rem No warning elevation
@rem set compilerFlags=-nologo -Gm -MT -W4 -wd4100 -wd4201 -wd4189 /Zi /EHsc
set compilerDefines=/DPARANOID=1
set linkInput=../lib/bullet/ZBulletPhysicsWrapper.lib
@echo on
cl %compilerFlags% %compilerDefines% /LD %compIn% %compOut% %linkInput%

@if %ERRORLEVEL% == 0 goto :FINISHED

@echo Error: %ERRORLEVEL%

@PAUSE

:FINISHED

@set compilerFlags=
@set compilerDefines=

@cd..
@cd build

@echo on