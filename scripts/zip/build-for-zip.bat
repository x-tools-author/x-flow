:: Remove build directory if it exists
if exist build (
    rmdir /s /q build
)

:: Create build directory and cd into it
mkdir build
cd build

if NOT "%~1"=="" (
    set QT_PATH=%~1%
) else (
    set QT_PATH=D:\512\Qt\Qt\6.8.3\msvc2022_64
)


call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
cmake -DCMAKE_PREFIX_PATH='%QT_PATH%' -DCMAKE_BUILD_TYPE:STRING=Release -DX_FLOW_STORE:BOOL=ON -G "Ninja" ../../../
cmake --build . --target xFlow
cmake --build . --target xFlow_zip