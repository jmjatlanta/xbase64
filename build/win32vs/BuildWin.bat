rem Build using MS Visual Studio
pause To build the Windows Visual Studio 32 bit version, execute this script from a Visual Studio 32 bit command window


rem cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Debug .
cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release .
nmake