@echo off

@echo --------------------------------------------------------
@echo Build Exercise 90 Renderer dll

cd..
if not exist bin mkdir bin
cd bin
if not exist base mkdir base
cd..

if not exist buildRenderer mkdir buildRenderer
cd buildRenderer
del *.* /q

set compilerFlags=-nologo -Gm -MT -WX -W4 -wd4100 -wd4201 -wd4189 /Zi /LD
set compilerDefines=/DPARANOID=1
set linkInputA=
set linkInputB=user32.lib opengl32.lib Gdi32.lib
set compilerInput=../src/Platform/win32_gl/win32_gl_module.cpp
set outputDLL=/Fe../bin/win32gl.dll
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
cd build
@echo on
