@echo off
SET PROJECT_DIR=%1
SET CONFIGURATION=%2

SET TARGET_DIR=..\$third-party-libs\RayTracer\

:::bin folder
copy %PROJECT_DIR%dependencies\FreeImage.dll %PROJECT_DIR%bin\%CONFIGURATION%\

:::third_party folder
xcopy /Y %PROJECT_DIR%bin\%CONFIGURATION% %TARGET_DIR%lib\
xcopy /S /Y %PROJECT_DIR%include %TARGET_DIR%include\