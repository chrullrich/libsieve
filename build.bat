@echo off

if not "%1"=="" goto :run

copy build.bat "%TEMP%"
%TEMP%\build.bat "%~dp0"
goto :eof

:run
cd /d %1

git checkout libsieve-2.3
if errorlevel 1 goto :eof

git branch -D build
git checkout -b build
if errorlevel 1 goto :eof
git merge --no-edit msvc-build storefile-action
if errorlevel 1 goto :eof

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
