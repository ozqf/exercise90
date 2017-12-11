@echo off
@echo Build Game DLL

if not exist buildGame mkdir buildGame
cd buildGame
del *.* /q

set compilerFlags=-nologo -Gm -MT -WX -W4 -wd4100 -wd4201 -wd4189 /Zi
set compilerDefines=/DPARANOID=1

cl %compilerFlags% %compilerDefines% /LD /Fe../bin/base/gamex86.dll ../src/App/app_main.cpp /EXPORT:LinkToApp

set compilerFlags=
set compilerDefines=

cd..
@echo on