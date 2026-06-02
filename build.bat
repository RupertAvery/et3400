@ECHO OFF

IF '%1'=='' goto err:
IF '%1'=='Debug' goto build:
IF '%1'=='Release' goto build:

goto err:

:build
SET VCPKG_ROOT=D:\git\vcpkg
SET VCPKG_TOOLCHAIN=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake

md %1
cd %1
cmake .. -DCMAKE_TOOLCHAIN_FILE=%VCPKG_TOOLCHAIN% -A x64 -DCMAKE_BUILD_TYPE=%1 & cmake --build . --config %1
cd ..

GOTO END:
:err
ECHO Error: Invalid or no configuration specified 
ECHO.
:end
