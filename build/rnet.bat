@echo off
cd..
cd bin
@echo *** Run Game ***
start exercise90.exe HOST TEST
timeout /t 2
start exercise90.exe JOIN TEST
timeout /t 2
start exercise90.exe JOIN TEST
cd..
cd build
@echo on