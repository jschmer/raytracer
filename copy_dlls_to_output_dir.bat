@echo off
SET PROJECT_DIR=%1
SET CONFIGURATION=%2

rem bin folder
copy %PROJECT_DIR%lib\FreeImage\FreeImage.dll %PROJECT_DIR%bin\%CONFIGURATION%