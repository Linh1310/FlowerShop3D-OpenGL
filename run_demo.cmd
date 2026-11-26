@echo off
setlocal
pushd "%~dp0" || exit /b 1

if exist "C:\msys64\ucrt64\bin\g++.exe" set "PATH=C:\msys64\ucrt64\bin;%PATH%"
where g++ >nul 2>&1
if errorlevel 1 (
    echo GCC is required. See the MSYS2 UCRT64 setup in README.md.
    popd
    exit /b 1
)

g++ -std=c++17 -Iinclude main.cpp ^
    src/bezier.cpp src/camera.cpp src/christmas.cpp src/flowers.cpp src/interaction.cpp ^
    src/lighting.cpp src/model3d.cpp src/order.cpp src/people.cpp ^
    src/scene.cpp src/shop.cpp src/ui.cpp ^
    -o flower_shop_christmas.exe -lfreeglut -lopengl32 -lglu32
if errorlevel 1 (
    popd
    exit /b 1
)

if /i "%~1"=="--build-only" (
    popd
    exit /b 0
)

flower_shop_christmas.exe
set "demo_exit=%errorlevel%"
popd
exit /b %demo_exit%
