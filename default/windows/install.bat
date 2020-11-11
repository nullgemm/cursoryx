@echo off
mkdir %APPDATA%\microstorm
xcopy /s /i "cursors" "%APPDATA%\microstorm\cursors"
xcopy /s /i "cursors4k" "%APPDATA%\microstorm\cursors4k"
rem  set __COMPAT_LAYER=RunAsInvoker  
regedit  /s  "%~dp0\cursor_schemes.reg"
C:\Windows\System32\main.cpl