@echo off
SET PROJECT_DIR=%1
SET CONFIGURATION=%2

rem bin folder
copy %PROJECT_DIR%dependencies\FreeImage.dll %PROJECT_DIR%bin\%CONFIGURATION%

rem scenes folder
copy %PROJECT_DIR%dependencies\FreeImage.dll %PROJECT_DIR%scenes\
copy %PROJECT_DIR%bin\%CONFIGURATION%\raytracer.exe %PROJECT_DIR%scenes\