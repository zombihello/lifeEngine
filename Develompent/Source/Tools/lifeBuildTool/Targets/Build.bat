@ECHO off
REM %1 is the project name
REM %2 is the platform name
REM %3 is the configuration name

IF EXIST ..\Intermediate\lifeBuildTool\%3\lifeBuildTool.exe (
         ..\Intermediate\lifeBuildTool\%3\lifeBuildTool.exe %* -DEPLOY
) ELSE (
	ECHO lifeBuildTool.exe not found in ..\Intermediate\lifeBuildTool\%3%\lifeBuildTool.exe
	EXIT /B 999
)
