@echo off

@echo --------------------------------------------------------
@echo Build Exercise 90 Sound dll

if not exist bin mkdir bin
cd bin
if not exist base mkdir base
cd..

if not exist buildSound mkdir buildSound
cd buildSound
del *.* /q

set compilerFlags=-nologo -Gm -MT -WX -W4 -wd4100 -wd4201 -wd4189 /Zi /LD
set compilerDefines=/DPARANOID=1
set linkInputA=
set linkInputB=user32.lib Gdi32.lib ../lib/fmod/fmod_vc.lib ../lib/fmod/fmodstudio_vc.lib
set compilerInput=../src/Platform/win32_snd/win32_snd_module.cpp
set outputDLL=/Fe../bin/win32sound.dll
@echo on
@cl %compilerFlags% %compilerDefines% %compilerInput% %outputDLL% %linkInputB%
@echo off
set compilerFlags=
set compilerDefines=
set linkInputA=
set linkInputB=
set compilerInput=
set outputDLL=

cd..
@echo on
