@ECHO OFF

IF '%1'=='' goto err:
IF '%1'=='Debug' goto clean:
IF '%1'=='Release' goto clean:

goto err:

:clean
del /f/q/s %1\*.* 
rmdir /q/s %1\*.* 

GOTO END:
:err
ECHO Error: Invalid or no configuration specified 
ECHO.
:end
