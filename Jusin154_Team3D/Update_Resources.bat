
robocopy ".\Editor\Bin\Resources" ".\Client\Bin\Resources"  /MIR /FFT /NDL /NFL /R:0 /W:0 /ETA
IF %ERRORLEVEL% LEQ 4 EXIT /B 0
pause