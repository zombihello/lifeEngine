@echo off
setlocal

cd ../Binaries/Win64/
rem HeliumGame-Win64-Debug.exe CompileTMX -src "../../Develompent/ContentOriginals/Maps/Frontend.tmx" -dst "../../Content/Maps/Frontend.lmap" -dstNewPackages "../../Content/Test"
HeliumGame-Win64-Debug.exe CompileTMX -src "%1" -dst "%2" -dstNewPackages "%3" %*