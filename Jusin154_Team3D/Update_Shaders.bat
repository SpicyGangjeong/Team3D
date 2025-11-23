
robocopy ".\Editor\Bin\Resources\ShaderFiles" ".\Client\Bin\Resources\ShaderFiles" /MIR /FFT /NDL /NFL /R:0 /W:0 /ETA
IF %ERRORLEVEL% LEQ 4 EXIT /B 0
pause
