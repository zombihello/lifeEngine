@ECHO off
REM %1 is the project name
REM %2 is the platform name
REM %3 is the configuration name

IF EXIST ..\Binaries\DotNET\lifeBuildTool.exe (
         ..\Binaries\DotNET\lifeBuildTool.exe %* -DEPLOY
) ELSE (
	ECHO lifeBuildTool.exe not found in ..\Binaries\DotNET\lifeBuildTool.exe
	EXIT /B 999
)
