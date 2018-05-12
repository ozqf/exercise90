
cl -nologo /P /C preprocess/preprocesstest.h

@xcopy /f preprocesstest.i preprocesstest.cpp

