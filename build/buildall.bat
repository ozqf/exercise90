@cls

@echo =========================================================
@echo =====================  Exercise 90  =====================
@echo =========================================================
@rem @call vsvars2015.bat
@rem Common library is no longer built separately
@rem @call buildcommon.bat

@call buildwin32.bat

@call buildrenderer.bat

@call buildsound.bat

@call buildgame.bat
@rem @PAUSE