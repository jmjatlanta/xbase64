rem Build Make files for Borland free compiler 5.5

rem - This does not work if both BC45 and bcc55 are on the same drive

rem set CXX=C:\bcc55\bin\

cmake -G "Borland Makefiles" >> build.txt

make