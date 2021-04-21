@ECHO off
REM %1 is the project name
REM %2 is the platform name
REM %3 is the configuration name

IF EXIST ..\..\Binaries\lifeBuildTool.exe (
         ..\..\Binaries\lifeBuildTool.exe %* -DEPLOY
) ELSE (
	ECHO lifeBuildTool.exe not found in ..\..\Binaries\lifeBuildTool.exe
	EXIT /B 999
)
