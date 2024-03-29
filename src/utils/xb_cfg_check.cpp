/*  xb_cfg_check.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2019,2023 Gary A Kunkel

The xb64 software library is covered under 
the terms of the GPL Version 3, 2007 license.

Email Contact:

    xb64-devel@lists.sourceforge.net
    xb64-users@lists.sourceforge.net

*/


#include <stdio.h>
#include <stdlib.h>
#include "xbase.h"

using namespace xb;



void PrintHelp();
void PrintHelp(){
  std::cout << "Usage: xb_cfg_check [-h] [-?] [--help] [-v] [--version]"  << std::endl << std::endl;
  std::cout << "This program displays the compile options that were in effect at the time the Xbase64 library was compiled.";
  std::cout << std::endl << std::endl;
}
void PrintVersion();
void PrintVersion(){
  std::cout << "Xbase64 Version: " << xbase_VERSION_MAJOR << "." << xbase_VERSION_MINOR << "." << xbase_VERSION_PATCH << std::endl;
}


int main( int argc, char *argv [] )
{

  xbXBase x;
  xbString sParm;

  if ( x.GetCmdLineOpt( argc, argv, "-h", sParm ) ||
       x.GetCmdLineOpt( argc, argv, "-?", sParm ) ||
       x.GetCmdLineOpt( argc, argv, "--help", sParm )){
    PrintHelp();
    return 1;
  }

  if ( x.GetCmdLineOpt( argc, argv, "-v", sParm ) ||
       x.GetCmdLineOpt( argc, argv, "--version", sParm )){
    PrintVersion();
    return 1;
  }

  fprintf( stdout, "\n%s version %d.%d.%d build options\n", CMAKE_PROJECT_NAME, xbase_VERSION_MAJOR,
                   xbase_VERSION_MINOR, xbase_VERSION_PATCH );
  // fprintf( stdout, "Usage: xb_cfg_check\n\n" );
  fprintf( stdout, "These options were in effect at library build time:\n" );

  fprintf( stdout, "\nGeneral system variables:\n" );
  fprintf( stdout, "SYSTEM_NAME             = [%s]\n", CMAKE_SYSTEM_NAME );
  fprintf( stdout, "SYSTEM_PROCESSOR        = [%s]\n", CMAKE_SYSTEM_PROCESSOR );
  fprintf( stdout, "SYSTEM_VERSION          = [%s]\n", CMAKE_SYSTEM_VERSION );
  fprintf( stdout, "PLATFORM                = [%s]\n", XB_PLATFORM );
  fprintf( stdout, "BUILD TYPE              = [%s]\n", CMAKE_BUILD_TYPE );
  fprintf( stdout, "CMAKE C FLAGS           = [%s]\n", CMAKE_C_FLAGS );
  fprintf( stdout, "CMAKE C FLAGS DEBUG     = [%s]\n", CMAKE_C_FLAGS_DEBUG );
  fprintf( stdout, "CMAKE C FLAGS RELEASE   = [%s]\n", CMAKE_C_FLAGS_RELEASE );


  #ifdef XB_PLATFORM_32
    fprintf( stdout, "XB_PLATFORM_32          = [TRUE]\n" );
  #endif
  #ifdef XB_PLATFORM_64
    fprintf( stdout, "XB_PLATFORM_64          = [TRUE]\n" );
  #endif
  fprintf( stdout, "COMPILER                = [%s]\n", CMAKE_COMPILER );
  #ifdef WIN32
    fprintf( stdout, "WIN32                   = [ON]\n" );
  #else
    fprintf( stdout, "WIN32                   = [OFF]\n" );
  #endif

  /*
  #ifdef XBDLLEXPORT
    fprintf( stdout, "XBDLLEXPORT             = [%s]\n", XBDLLEXPORT );
  #endif
  */
  printf( "\n" );


  #ifdef CMAKE_COMPILER_IS_GNUCC
    fprintf( stdout, "CMAKE_COMPILER_IS_GNUCC = [ON]\n" );
  #else
    fprintf( stdout, "CMAKE_COMPILER_IS_GNUCC = [OFF]\n" );
  #endif

  #ifdef _FILE_OFFSET_BITS
    fprintf( stdout, "_FILE_OFFSET_BITS       = [DEFINED]\n" );
  #endif

  fprintf( stdout, "User controlled build options:\n" );


  #ifdef XB_BLOCKREAD_SUPPORT
    fprintf( stdout, "XB_BLOCKREAD_SUPPORT    = [ON]\n" );
  #else
    fprintf( stdout, "XB_BLOCKREAD_SUPPORT    = [OFF]\n" );
  #endif

  #ifdef XB_DBF3_SUPPORT
    fprintf( stdout, "XB_DBF3_SUPPORT         = [ON]\n" );
  #else
    fprintf( stdout, "XB_DBF3_SUPPORT         = [OFF]\n" );
  #endif

  #ifdef XB_DBF4_SUPPORT
    fprintf( stdout, "XB_DBF4_SUPPORT         = [ON]\n" );
  #else
    fprintf( stdout, "XB_DBF4_SUPPORT         = [OFF]\n" );
  #endif

  #ifdef XB_DEBUG_SUPPORT
    fprintf( stdout, "XB_DEBUG_SUPPORT        = [ON]\n" );
  #else
    fprintf( stdout, "XB_DEBUG_SUPPORT        = [OFF]\n" );
  #endif

  #ifdef XB_EXAMPLES_SUPPORT
    fprintf( stdout, "XB_EXAMPLES_SUPPORT     = [ON]\n" );
  #else
    fprintf( stdout, "XB_EXAMPLES_SUPPORT     = [OFF]\n" );
  #endif

  #ifdef XB_EXPRESSION_SUPPORT
    fprintf( stdout, "XB_EXPRESSION_SUPPORT   = [ON]\n" );
  #else
    fprintf( stdout, "XB_EXPRESSION_SUPPORT   = [OFF]\n" );
  #endif

  #ifdef XB_FILTER_SUPPORT
    fprintf( stdout, "XB_FILTER_SUPPORT       = [ON]\n" );
  #else
    fprintf( stdout, "XB_FILTER_SUPPORT       = [OFF]\n" );
  #endif

  #ifdef XB_FUNCTION_SUPPORT
    fprintf( stdout, "XB_FUNCTION_SUPPORT     = [ON]\n" );
  #else
    fprintf( stdout, "XB_FUNCTION_SUPPORT     = [OFF]\n" );
  #endif

  #ifdef XB_INDEX_SUPPORT
    fprintf( stdout, "XB_INDEX_SUPPORT        = [ON]\n" );
  #else
    fprintf( stdout, "XB_INDEX_SUPPORT        = [OFF]\n" );
  #endif

  #ifdef XB_INF_SUPPORT
    fprintf( stdout, "XB_INF_SUPPORT          = [ON]\n" );
  #else
    fprintf( stdout, "XB_INF_SUPPORT          = [OFF]\n" );
  #endif

  #ifdef XB_LINKLIST_SUPPORT
    fprintf( stdout, "XB_LINKLIST_SUPPORT     = [ON]\n" );
  #else
    fprintf( stdout, "XB_LINKLIST_SUPPORT     = [OFF]\n" );
  #endif

  #ifdef XB_LOCKING_SUPPORT
    fprintf( stdout, "XB_LOCKING_SUPPORT      = [ON]\n" );
  #else
    fprintf( stdout, "XB_LOCKING_SUPPORT      = [OFF]\n" );
  #endif

  #ifdef XB_LOGGING_SUPPORT
    fprintf( stdout, "XB_LOGGING_SUPPORT      = [ON]\n" );
  #else
    fprintf( stdout, "XB_LOGGING_SUPPORT      = [OFF]\n" );
  #endif

  #ifdef XB_MDX_SUPPORT
    fprintf( stdout, "XB_MDX_SUPPORT          = [ON]\n" );
  #else
    fprintf( stdout, "XB_MDX_SUPPORT          = [OFF]\n" );
  #endif

  #ifdef XB_MEMO_SUPPORT
    fprintf( stdout, "XB_MEMO_SUPPORT         = [ON]\n" );
  #else
    fprintf( stdout, "XB_MEMO_SUPPORT         = [OFF]\n" );
  #endif

  #ifdef XB_NDX_SUPPORT
    fprintf( stdout, "XB_NDX_SUPPORT          = [ON]\n" );
  #else
    fprintf( stdout, "XB_NDX_SUPPORT          = [OFF]\n" );
  #endif

  #ifdef XB_RELATE_SUPPORT
    fprintf( stdout, "XB_RELATE_SUPPORT       = [ON]\n" );
  #else
    fprintf( stdout, "XB_RELATE_SUPPORT       = [OFF]\n" );
  #endif

  #ifdef XB_SQL_SUPPORT
    fprintf( stdout, "XB_SQL_SUPPORT          = [ON]\n" );
  #else
    fprintf( stdout, "XB_SQL_SUPPORT          = [OFF]\n" );
  #endif

  #ifdef XB_TDX_SUPPORT
    fprintf( stdout, "XB_TDX_SUPPORT          = [ON]\n" );
  #else
    fprintf( stdout, "XB_TDX_SUPPORT          = [OFF]\n" );
  #endif

  #ifdef XB_UTILS_SUPPORT
    fprintf( stdout, "XB_UTILS_SUPPORT        = [ON]\n" );
  #else
    fprintf( stdout, "XB_UTILS_SUPPORT        = [OFF]\n" );
  #endif

  fprintf( stdout, "\nDirectories:\n" );
  fprintf( stdout, "HOME_DIRECTORY                 = [%s]\n",   CMAKE_HOME_DIRECTORY );
  fprintf( stdout, "PROJECT_PARENT_DIR             = [%s]\n",   PROJECT_PARENT_DIR );
  fprintf( stdout, "PROJECT_BINARY_DIR             = [%s]\n",   PROJECT_BINARY_DIR );
  fprintf( stdout, "PROJECT_DATA_DIR               = [%s]\n",   PROJECT_DATA_DIR );
  fprintf( stdout, "PROJECT_LOG_DIR                = [%s]\n",   PROJECT_LOG_DIR );
  fprintf( stdout, "PROJECT_TEMP_DIR               = [%s]\n",   PROJECT_TEMP_DIR );
  fprintf( stdout, "PROJECT_DFLT_LOGFILE           = [%s]\n",   PROJECT_DFLT_LOGFILE );
  fprintf( stdout, "PROJECT_SOURCE_DIR             = [%s]\n",   PROJECT_SOURCE_DIR );
  fprintf( stdout, "CMAKE_RUNTIME_OUTPUT_DIRECTORY = [%s]\n\n", CMAKE_RUNTIME_OUTPUT_DIRECTORY );

  fprintf( stdout, "Libraries:\n" );
  fprintf( stdout, "BUILD_SHARED_LIBS     = [%s]\n",   BUILD_SHARED_LIBS );
  fprintf( stdout, "EXTRA_LIBS            = [%s]\n\n", EXTRA_LIBS );


  fprintf( stdout, "sizeof(char *)        = [%d]\n", (xbInt32) sizeof( char *));
  fprintf( stdout, "sizeof(int)           = [%d]\n", (xbInt32) sizeof( int ));
  fprintf( stdout, "sizeof(long)          = [%d]\n", (xbInt32) sizeof( long ));
  fprintf( stdout, "sizeof(char)          = [%d]\n", (xbInt32) sizeof( char ));
  #ifdef HAVE_WCHAR_H
    fprintf( stdout, "sizeof(wchar_t)       = [%d]\n", (xbInt32) sizeof( wchar_t ));
  #endif

  #ifdef HAVE_WINDOWS_H
    fprintf( stdout, "sizeof(DWORD)         = [%d]\n", (xbInt32) sizeof( DWORD ));
  #endif

  fprintf( stdout, "sizeof(double)        = [%d]\n", (xbInt32) sizeof( double ));
  fprintf( stdout, "sizeof(float)         = [%d]\n", (xbInt32)  sizeof( float ));
  fprintf( stdout, "sizeof(size_t)        = [%d]\n", (xbInt32) sizeof( size_t ));
  fprintf( stdout, "sizeof(off_t)         = [%d]\n\n", (xbInt32) sizeof( off_t ));

  fprintf( stdout, "sizeof(xbBool)        = [%d]\n", (xbInt32) sizeof( xbBool ));
  fprintf( stdout, "sizeof(xbInt16)       = [%d]\n", (xbInt32) sizeof( xbInt16 ));
  fprintf( stdout, "sizeof(xbUInt16)      = [%d]\n", (xbInt32) sizeof( xbUInt16 ));
  fprintf( stdout, "sizeof(xbInt32)       = [%d]\n", (xbInt32) sizeof( xbInt32 ));
  fprintf( stdout, "sizeof(xbUInt32)      = [%d]\n", (xbInt32) sizeof( xbUInt32 ));
  fprintf( stdout, "sizeof(xbInt64)       = [%d]\n", (xbInt32) sizeof( xbInt64 ));
  fprintf( stdout, "sizeof(xbUInt64)      = [%d]\n", (xbInt32) sizeof( xbUInt64 ));
  fprintf( stdout, "sizeof(xbFloat)       = [%d]\n", (xbInt32) sizeof( xbFloat ));
  fprintf( stdout, "sizeof(xbDouble)      = [%d]\n", (xbInt32) sizeof( xbDouble ));


  fprintf( stdout, "\nHeader files:\n" );
  #ifdef HAVE_CTYPE_H
    fprintf( stdout, "HAVE_CTYPE_H          = [YES]\n" );
  #else
    fprintf( stdout, "HAVE_CTYPE_H          = [NO]\n" );
  #endif

  #ifdef HAVE_DIRENT_H
    fprintf( stdout, "HAVE_DIRENT_H         = [YES]\n" );
  #else
    fprintf( stdout, "HAVE_DIRENT_H         = [NO]\n" );
  #endif

  #ifdef HAVE_FCNTL_H
    fprintf( stdout, "HAVE_FCNTL_H          = [YES]\n" );
  #else
    fprintf( stdout, "HAVE_FCNTL_H          = [NO]\n" );
  #endif

  #ifdef HAVE_INTTYPES_H
    fprintf( stdout, "HAVE_INTTYPES_H       = [YES]\n" );
  #else
    fprintf( stdout, "HAVE_INTTYPES_H       = [NO]\n" );
  #endif

  #ifdef HAVE_STDARG_H
    fprintf( stdout, "HAVE_STDARG_H         = [YES]\n" );
  #else
    fprintf( stdout, "HAVE_STDARG_H         = [NO]\n" );
  #endif

  #ifdef HAVE_STRING_H
    fprintf( stdout, "HAVE_STRING_H         = [YES]\n" );
  #else
    fprintf( stdout, "HAVE_STRING_H         = [NO]\n" );
  #endif

  #ifdef HAVE_STRINGS_H
    fprintf( stdout, "HAVE_STRINGS_H        = [YES]\n" );
  #else
    fprintf( stdout, "HAVE_STRINGS_H        = [NO]\n" );
  #endif

  #ifdef HAVE_STAT_H
    fprintf( stdout, "HAVE_STAT_H           = [YES]\n" );
  #else
    fprintf( stdout, "HAVE_STAT_H           = [NO]\n" );
  #endif

  #ifdef HAVE_UNISTD_H
    fprintf( stdout, "HAVE_UNISTD_H         = [YES]\n" );
  #else
    fprintf( stdout, "HAVE_UNISTD_H         = [NO]\n" );
  #endif

  #ifdef HAVE_WINDOWS_H
    fprintf( stdout, "HAVE_WINDOWS_H        = [YES]\n" );
  #else
    fprintf( stdout, "HAVE_WINDOWS_H        = [NO]\n" );
  #endif


  fprintf( stdout, "\nC Library functions:\n" );

  #ifdef HAVE__CLOSE_F
    fprintf( stdout, "HAVE__CLOSE_F         = [YES]\n" );
  #else
    fprintf( stdout, "HAVE__CLOSE_F         = [NO]\n" );
  #endif

  #ifdef HAVE_CREATEPROCESSW_F
    fprintf( stdout, "HAVE_CREATEPROCESSW_F = [YES]\n" );
  #else
    fprintf( stdout, "HAVE_CREATEPROCESSW_F = [NO]\n" );
  #endif

  #ifdef HAVE_FCNTL_F
    fprintf( stdout, "HAVE_FCNTL_F          = [YES]\n" );
  #else
    fprintf( stdout, "HAVE_FCNTL_F          = [NO]\n" );
  #endif

  #ifdef HAVE__FDOPEN_F
    fprintf( stdout, "HAVE__FDOPEN_F        = [YES]\n" );
  #else
    fprintf( stdout, "HAVE__FDOPEN_F        = [NO]\n" );
  #endif

  #ifdef HAVE_FOPEN_S_F
    fprintf( stdout, "HAVE_FOPEN_S_F        = [YES]\n" );
  #else
    fprintf( stdout, "HAVE_FOPEN_S_F        = [NO]\n" );
  #endif

  #ifdef HAVE__FSOPEN_F
    fprintf( stdout, "HAVE__FSOPEN_F        = [YES]\n" );
  #else
    fprintf( stdout, "HAVE__FSOPEN_F        = [NO]\n" );
  #endif

  #ifdef HAVE__FILENO_F
    fprintf( stdout, "HAVE__FILENO_F        = [YES]\n" );
  #else
    fprintf( stdout, "HAVE__FILENO_F        = [NO]\n" );
  #endif

  #ifdef HAVE_FORK_F
    fprintf( stdout, "HAVE_FORK_F           = [YES]\n" );
  #else
    fprintf( stdout, "HAVE_FORK_F           = [NO]\n" );
  #endif

  #ifdef HAVE__FSEEKI64_F
    fprintf( stdout, "HAVE__FSEEKI64_F      = [YES]\n" );
  #else
    fprintf( stdout, "HAVE__FSEEKI64_F      = [NO]\n" );
  #endif

  #ifdef HAVE_FSEEKO_F
    fprintf( stdout, "HAVE_FSEEKO_F         = [YES]\n" );
  #else
    fprintf( stdout, "HAVE_FSEEKO_F         = [NO]\n" );
  #endif

  #ifdef HAVE_FTRUNCATE_F
    fprintf( stdout, "HAVE_FTRUNCATE_F      = [YES]\n" );
  #else
    fprintf( stdout, "HAVE_FTRUNCATE_F      = [NO]\n" );
  #endif

  #ifdef HAVE__LOCALTIME_S_F
    fprintf( stdout, "HAVE__LOCALTIME_S_F   = [YES]\n" );
  #else
    fprintf( stdout, "HAVE__LOCALTIME_S_F   = [NO]\n" );
  #endif

  #ifdef HAVE_LOCKFILE_F
    fprintf( stdout, "HAVE_LOCKFILE_F       = [YES]\n" );
  #else
    fprintf( stdout, "HAVE_LOCKFILE_F       = [NO]\n" );
  #endif

  #ifdef HAVE_LOCKING_F
    fprintf( stdout, "HAVE_LOCKING_F        = [YES]\n" );
  #else
    fprintf( stdout, "HAVE_LOCKING_F        = [NO]\n" );
  #endif

  #ifdef HAVE__LOCKING_F
    fprintf( stdout, "HAVE__LOCKING_F       = [YES]\n" );
  #else
    fprintf( stdout, "HAVE__LOCKING_F       = [NO]\n" );
  #endif

  #ifdef HAVE__OPEN_F
    fprintf( stdout, "HAVE__OPEN_F          = [YES]\n" );
  #else
    fprintf( stdout, "HAVE__OPEN_F          = [NO]\n" );
  #endif

  #ifdef HAVE_SETENDOFFILE_F
    fprintf( stdout, "HAVE_SETENDOFFILE_F   = [YES]\n" );
  #else
    fprintf( stdout, "HAVE_SETENDOFFILE_F   = [NO]\n" );
  #endif

  #ifdef HAVE_VSNPRINTF_F
    fprintf( stdout, "HAVE_VSNPRINTF_F      = [YES]\n" );
  #else
    fprintf( stdout, "HAVE_VSNPRINTF_F      = [NO]\n" );
  #endif

  #ifdef HAVE_VSNPRINTF_S_F
    fprintf( stdout, "HAVE_VSNPRINTF_S_F    = [YES]\n" );
  #else
    fprintf( stdout, "HAVE_VSNPRINTF_S_F    = [NO]\n" );
  #endif

  return 0;
}

