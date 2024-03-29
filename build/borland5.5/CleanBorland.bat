
rem Use this batch file to completely clean the Borland directory tree for a clean build from scratch

rd /S /Q bin
rd /S /Q CMakeFiles
rd /S /Q include
rd /S /Q lib
rd /S /Q testing
del *.cmake
del CmakeCache.txt
del Makefile
del *.h
del *.exp
del *.lib
del *.filters
del *.vcxproj
del *.sln
del build.txt