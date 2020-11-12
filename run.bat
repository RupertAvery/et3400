
@ECHO OFF

IF '%1'=='' goto err:
IF '%1'=='Debug' goto run:
IF '%1'=='Release' goto run:

goto err:

:run
%1\%1\et3400.exe 

GOTO END:
:err
ECHO Error: Invalid or no configuration specified 
ECHO.
:end
