@echo off
setlocal

cd ../Binaries/Win64/
HeliumGame-Win64-Debug.exe ImportTMX -src "%1" -dst "%2" %*