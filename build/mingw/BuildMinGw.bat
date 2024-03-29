rem Build using MinGW compiler


cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug .

rem cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release .

mingw32-make