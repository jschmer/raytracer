@echo off
SET PROJECT_DIR=%1
SET CONFIGURATION=%2

copy %PROJECT_DIR%dependencies\FreeImage.dll %PROJECT_DIR%bin\%CONFIGURATION%