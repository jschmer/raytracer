@echo off
SET PROJECT_DIR=%1
SET CONFIGURATION=%2
SET DEBUG=%3

rem bin folder
copy %PROJECT_DIR%lib\FreeImage\FreeImage.dll %PROJECT_DIR%bin\%CONFIGURATION%
copy %PROJECT_DIR%lib\Assimp\bin\Assimp32%DEBUG%.dll %PROJECT_DIR%bin\%CONFIGURATION%