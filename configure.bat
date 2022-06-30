@echo off
setlocal enabledelayedexpansion

set MY_PATH=%~dp0
set MY_COMMAND=%~fn0
echo CURRENT DIRECTORY: %CD%
echo ROOT DIRECTORY: %MY_PATH%
echo THIS COMMAND: %MY_COMMAND%
echo ARGUMENTS: %*
echo INTERNAL COMMAND: call %MY_PATH%\common\ArcaneInfra\tools\ArcaneInfra.Tools\Configurator\bin\Debug\Configurator.exe --base-command=%MY_COMMAND% %*

call %MY_PATH%\common\ArcaneInfra\tools\ArcaneInfra.Tools\Configurator\bin\Debug\Configurator.exe --base-command=%MY_COMMAND% %* || exit /b 1
