
cl -nologo /P /C ../preprocess/preprocesstest.h

@xcopy /F /Y preprocesstest.i preprocesstest.cpp

