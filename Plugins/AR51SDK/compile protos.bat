@echo off
set EXEC_PATH=.\GrpcPrograms\Win64
set PROTO_SRC=E:\Workspace\oms\src\OMS\protos
set OUTPUT_DIR=.\Source\AR51SDK\Private\protos


rem Check if EXEC_PATH exists
if not exist "%EXEC_PATH%" (
    echo The path %EXEC_PATH% does not exist.
    exit /b
)

rem Check if PROTO_SRC exists and contains .proto files
if not exist "%PROTO_SRC%" (
    echo The path %PROTO_SRC% does not exist.
    exit /b
)

for /F %%i in ('dir /A-D /B "%PROTO_SRC%\*.proto" 2^>nul') do (
    goto continue
)
echo The path %PROTO_SRC% contains no .proto files.
exit /b

:continue

rem Print a description of what the script does
@echo off
echo.
echo ===============================
echo       STARTING THE SCRIPT      
echo ===============================
echo.
echo This script will do the following:
echo --------------------------------
echo 1. Check if the path to the protoc and gRPC executables exists.
echo 2. Check if the path to the .proto source files exists and contains .proto files.
echo 3. If the output directory does not exist, it will be created.
echo 4. Delete all .h files in the output directory, excluding unreal.grpc.h.
echo 5. Delete all .hpp files in the output directory, excluding unreal.grpc.hpp.
echo 6. Compile the .proto files using the protoc and gRPC executables.
echo.
echo -------------------------------
echo Script is now starting...
echo -------------------------------
echo.


rem Make sure that the output folder exists
if not exist %OUTPUT_DIR% mkdir %OUTPUT_DIR%

rem Delete all header files
for /R %OUTPUT_DIR% %%G in (*.h) do (
    if not "%%~nxG"=="unreal.grpc.h" del /F /Q "%%G"
)

rem Delete all code/.hpp files
for /R %OUTPUT_DIR% %%G in (*.hpp) do (
    if not "%%~nxG"=="unreal.grpc.hpp" del /F /Q "%%G"
)

rem compile the protos
rem .\Plugins\InfraworldRuntime\GrpcPrograms\Win64\protoc.exe -I E:\Workspace\oms\src\OMS\protos --cpp_out=.\Source\UnrealSDK\Private\protos --grpc_out=.\Source\UnrealSDK\Private\protos --plugin=protoc-gen-grpc=.\Plugins\InfraworldRuntime\GrpcPrograms\Win64\grpc_cpp_plugin.exe E:\Workspace\oms\src\OMS\protos\cvs.proto E:\Workspace\oms\src\OMS\protos\dgs.proto E:\Workspace\oms\src\OMS\protos\AR51.Core.proto E:\Workspace\oms\src\OMS\protos\dgsEnums.proto E:\Workspace\oms\src\OMS\protos\registrationService.proto


rem compile the protos
%EXEC_PATH%\protoc.exe -I %PROTO_SRC% --cpp_out=%OUTPUT_DIR% --grpc_out=%OUTPUT_DIR% --plugin=protoc-gen-grpc=%EXEC_PATH%\grpc_cpp_plugin.exe ^
	%PROTO_SRC%\cvs.proto ^
	%PROTO_SRC%\dgs.proto ^
	%PROTO_SRC%\AR51.Core.proto ^
	%PROTO_SRC%\dgsEnums.proto ^
	%PROTO_SRC%\registrationService.proto

	
echo.
echo -------------------------------
echo Compile succesfully completed...
echo Next, Applying code bug fix in hpp files...
echo -------------------------------
echo.

rem rename all .cc files to .hpp files
ren %OUTPUT_DIR%\*.cc *.hpp

rem This section fixes compile errors in the auto generate code
@echo off
setlocal enabledelayedexpansion
set /A c=0

for /r %%f in (%OUTPUT_DIR%\*.hpp) do (
	set /A c=c+1
	powershell -Command "(gc '%%f') -replace 'schemas', 'schemas!c!' | Out-File -encoding ASCII '%%f'"
	powershell -Command "(gc '%%f') -replace 'file_default_instances', 'file_default_instances!c!' | Out-File -encoding ASCII '%%f'"
	echo replacing schemas and file_default_instances with unique ids for file: %%f
)
endlocal


echo.
echo -------------------------------
echo Script succesfully completed...
echo -------------------------------
echo.



pause