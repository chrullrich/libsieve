@echo off

set DEST=C:\Daten\Projekte\3rdparty\libsieve\lib64\v145-SharedCRT
del /q "%DEST%\*"

call :buildstep
call :buildstep DEBUG=1
call :buildstep DLL=1
call :buildstep DLL=1 DEBUG=1

set DEST=C:\Daten\Projekte\3rdparty\libsieve\lib64\v145-StaticCRT
del /q "%DEST%\*"

call :buildstep STATIC=1
call :buildstep STATIC=1 DEBUG=1
call :buildstep STATIC=1 DLL=1
call :buildstep STATIC=1 DLL=1 DEBUG=1

exit /b

:buildstep
nmake clean
nmake %*
copy build\* %DEST%
