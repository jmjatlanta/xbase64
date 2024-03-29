rem Build using MS Visual Studio
pause To build the Windows Visual Studio 64 bit version, execute this script from a Visual Studio 64 bit command window

rem SET CC="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.38.33130\bin\Hostx64\x64\cl.exe"
rem SET CXX="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.38.33130\bin\Hostx64\x64\cl.exe"

cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Debug .
rem cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release .

pause

nmake