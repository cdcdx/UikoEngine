@echo off 
cls
color 0A

echo **********************************************************
echo *                       VC清理工具
echo *   author: Garfield
echo *   taobao: http://shop64088102.taobao.com 
echo *   version:2012.6
echo **********************************************************
@echo off
title VC清理工具
ver
echo.

del /F /S /Q *.ilk
del /F /S /Q *.pdb
del /F /S /Q *.xml.bak
del /F /S /Q *.aps
del /F /S /Q *.ncb
del /F /S /Q *.vps
del /F /S /Q /A H *.suo
for /f "usebackq tokens=1* delims=/" %%a in (`dir /s /b /a:d ^| findstr /i "\\Debug$"`) do rmdir /s /q "%%a"
for /f "usebackq tokens=1* delims=/" %%a in (`dir /s /b /a:d ^| findstr /i "\\Release$"`) do rmdir /s /q "%%a"
for /f "usebackq tokens=1* delims=/" %%a in (`dir /s /b /a:d ^| findstr /i "\\Build\\Debug$"`) do rmdir /s /q "%%a"
for /f "usebackq tokens=1* delims=/" %%a in (`dir /s /b /a:d ^| findstr /i "\\Build\\Release$"`) do rmdir /s /q "%%a"
for /f "usebackq tokens=1* delims=/" %%a in (`dir /s /b /a:d ^| findstr /i "\\Build\\Debug_u$"`) do rmdir /s /q "%%a"
for /f "usebackq tokens=1* delims=/" %%a in (`dir /s /b /a:d ^| findstr /i "\\Build\\Release_u$"`) do rmdir /s /q "%%a"
for /f "usebackq tokens=1* delims=/" %%a in (`dir /s /b /a:d ^| findstr /i "\\_UpgradeReport_Files$"`) do rmdir /s /q "%%a"
for /f "usebackq tokens=1* delims=/" %%a in (`dir /s /b /a:d ^| findstr /i "\\ipch$"`) do rmdir /s /q "%%a"
del /F /S /Q *.user
del /F /S /Q *_d.exe
del /F /S /Q *_d.dll
del /F /S /Q *_d.lib
del /F /S /Q *_ud.exe
del /F /S /Q *_ud.dll
del /F /S /Q *_ud.lib
del /F /S /Q *.exp
del /F /S /Q *.sdf
del /F /S /Q *.vcxproj.user
pause
echo pause & exit