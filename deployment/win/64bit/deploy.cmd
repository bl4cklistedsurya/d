@echo off

set BUILD_DIR=%1
Set BUILD_DIR=%BUILD_DIR:"=%
set QT_DIR=%2
Set QT_DIR=%QT_DIR:"=%

del "%BUILD_DIR%\kleiner-brauhelfer-core.lib"
xcopy "%~dp0libcrypto-1_1-x64.dll" "%BUILD_DIR%" /Y
xcopy "%~dp0libssl-1_1-x64.dll" "%BUILD_DIR%" /Y

"%QT_DIR%\windeployqt.exe" "%BUILD_DIR%\kleiner-brauhelfer-2.exe"
