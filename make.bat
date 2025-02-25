@echo off
set TMP_PATH=%PATH%
set path=%path%;c:\Program Files\Dev-cpp\bin\
"C:\Program Files\dev-cpp\bin\mingw32-make.exe" -f makefile.win
set path=%TMP_PATH%
pause