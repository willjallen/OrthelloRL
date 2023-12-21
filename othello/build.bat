@echo off
REM Batch script to build a CMake project with libtorch

REM Create a new directory 'build' and enter it
if not exist build mkdir build
cd build

REM Configure the project with CMake
REM Assuming libtorch is in the same directory as this script
REM and CMakeLists.txt is in the parent directory of 'build'
set LIBTORCH_DIR=%~dp0libtorch
cmake -DCMAKE_PREFIX_PATH=%LIBTORCH_DIR%  ..
@REM -DCUDA_TOOLKIT_ROOT_DIR="C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.1" -DCudaToolkitDir="C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.1/extras/visual_studio_integration/MSBuildExtensions"
REM Build the project in Release configuration
cmake -G "Visual Studio 17 2022" --build --config Release . 

REM Return to the original directory
cd ..
