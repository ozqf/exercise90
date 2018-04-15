@echo off
cls
echo ----------------------------------
echo Build Ex90 Release Candidate

@echo Make directories.
if not exist release mkdir release

cd release
if not exist base mkdir base
cd..

@echo Copying files
xcopy "bin\exercise90.exe" "release\ex90.exe" /s /f /y
xcopy "bin\win32gl.dll" "release\win32gl.dll" /s /f /y
xcopy "bin\base\gamex86.dll" "release\base\gamex86.dll" /s /f /y

echo Done
echo on
@echo on
@PAUSE
