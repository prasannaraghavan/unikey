@echo off
echo ==================================================
echo Barrier Application Deployment Script
echo ==================================================
echo.

set BUILD_DIR=%~dp0build\bin\Release
set DEPLOY_DIR=%~dp0Barrier-Deploy
set CURRENT_DIR=%~dp0

echo Source directory: %BUILD_DIR%
echo Deploy directory: %DEPLOY_DIR%
echo.

REM Check if build directory exists
if not exist "%BUILD_DIR%" (
    echo ERROR: Build directory not found!
    echo Please build Barrier first using: cmake --build build --config Release
    pause
    exit /b 1
)

REM Create deployment directory
if exist "%DEPLOY_DIR%" (
    echo Removing existing deployment directory...
    rmdir /s /q "%DEPLOY_DIR%"
)

echo Creating deployment directory...
mkdir "%DEPLOY_DIR%"

REM Copy all files from build directory
echo Copying Barrier application files...
xcopy "%BUILD_DIR%\*" "%DEPLOY_DIR%\" /E /I /H /Y

REM Create README for deployment
echo Creating deployment README...
(
echo Barrier - Mouse and Keyboard Sharing Application
echo =============================================
echo.
echo This is a portable deployment of Barrier that can run on any Windows PC
echo without installation. Simply extract this folder and run barrier.exe.
echo.
echo Files included:
echo - barrier.exe          ^(Main GUI application^)
echo - barrierc.exe         ^(Client application^)
echo - barriers.exe         ^(Server application^)
echo - barrierd.exe         ^(Service daemon^)
echo - All Qt6 and OpenSSL libraries
echo - Supporting files and resources
echo.
echo To use:
echo 1. Extract this folder to any location on the target PC
echo 2. Run barrier.exe to start the GUI
echo 3. Configure as server or client as needed
echo.
echo System Requirements:
echo - Windows 10 or later
echo - .NET Framework ^(included: vc_redist.x64.exe^)
echo.
echo For more information visit: https://github.com/debauchee/barrier
) > "%DEPLOY_DIR%\README.txt"

REM Create a simple launcher script
echo Creating launcher script...
(
echo @echo off
echo cd /d "%%~dp0"
echo start "" barrier.exe
) > "%DEPLOY_DIR%\Start-Barrier.bat"

echo.
echo ==================================================
echo Deployment completed successfully!
echo ==================================================
echo.
echo Deployment location: %DEPLOY_DIR%
echo.
echo To distribute:
echo 1. Zip the '%DEPLOY_DIR%' folder
echo 2. Send to target PC
echo 3. Extract and run Start-Barrier.bat or barrier.exe
echo.

REM Create ZIP package
echo Creating ZIP package for easy distribution...
powershell "Compress-Archive -Path '%DEPLOY_DIR%\*' -DestinationPath '%CURRENT_DIR%Barrier-Portable-Complete.zip' -Force"

if exist "%CURRENT_DIR%Barrier-Portable-Complete.zip" (
    echo.
    echo ZIP package created: Barrier-Portable-Complete.zip
    echo This file is ready to send to other PCs!
) else (
    echo.
    echo Note: Could not create ZIP package, but deployment folder is ready.
)

echo.
pause
