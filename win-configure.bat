set SDL2DIR=d:/lib/SDL2

md ..\js3d-build
cd ..\js3d-build
cmake -L ..\js3d
REM cmake --build . --config Debug

pause
