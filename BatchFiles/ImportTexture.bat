@echo off
setlocal

cd ../Binaries/Win64/
TestbedGame-Win64-Debug.exe ImportTexture -src "%1" -dst "../../%2" -name "%3" %*