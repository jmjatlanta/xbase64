
rem Use this batch file to completely clean the Windows directory tree for a clean build from scratch

rem rd /S /Q bin
rd /S /Q CMakeFiles
rd /S /Q include
rd /S /Q lib
rd /S /Q testing
rd /S /Q Win32VS
rd /S /Q bin
del *.cmake
del CmakeCache.txt
del Makefile
del *.h
del *.exp
del *.lib
del *.filters
del *.vcxproj
del *.sln

