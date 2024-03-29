/*  xb_test_xbase.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

// This program tests the class xbXBase

// usage:   xb_test_xbase QUITE|NORMAL|VERBOSE


#include "xbase.h"

using namespace xb;

#include "tstfuncs.cpp"

int main( int argCnt, char **av )
{
  int iRc = 0;
  int iPo = 1;          /* print option */
                       /* 0 - QUIET    */
                       /* 1 - NORMAL   */
                       /* 2 - VERBOSE  */

  if( argCnt > 1 ) {
    if( av[1][0] == 'Q' )
      iPo = 0;
    else if( av[1][0] == 'V' )
      iPo = 2;
  }

  xbXBase x;
  xbString sMsg;

  #ifdef XB_LOGGING_SUPPORT


  xbString sLogDir = PROJECT_LOG_DIR;
//  x.SetLogDirectory( sLogDir );
  x.SetLogDirectory( PROJECT_LOG_DIR );


  x.EnableMsgLogging();
  if( iPo ){
    std::cout << "Logfile is [" << x.GetLogFqFileName().Str() << "]" << std::endl;
  }

  sMsg.Sprintf( "Program [%s] initializing...", av[0] );
  x.WriteLogMessage( sMsg );
  #endif
  InitTime();


  x.SetDefaultDateFormat( "YY-MM-DD" );
  iRc += TestMethod( iPo, "SetDefaultDateFormat", x.GetDefaultDateFormat(), "YY-MM-DD", 8 );
  if( iPo == 2 ){
    if( x.GetEndianType() == 'L' )
      std::cout << "Little Endian Architecture" << std::endl;
    else if( x.GetEndianType() == 'B' )
      std::cout << "Big Endian Architecture" << std::endl;
    else
      std::cout << "Undefine Endian Architecture" << std::endl;
  }

  iRc += TestMethod( iPo, "GetErrorMessage", x.GetErrorMessage( XB_DBF_FILE_NOT_OPEN ), "DBF File Not Open", 17 );


  #ifdef XB_LOGGING_SUPPORT

  char cSep = x.GetPathSeparator();
  if( cSep == '/' )
    sLogDir.SwapChars( '\\', '/' );
  else
    sLogDir.SwapChars( '/', '\\' );

  iRc += TestMethod( iPo, "GetLogDirectory()", x.GetLogDirectory(), sLogDir, sLogDir.Len());

  xbString sLogName = CMAKE_SYSTEM_NAME;
  sLogName += "_";
  sLogName += XB_PLATFORM;
  sLogName += ".xbLog.txt";

  iRc += TestMethod( iPo, "GetLogFileName()", x.GetLogFileName(), sLogName, sLogName.Len());
  x.WriteLogMessage( "Program xb_test_xbase - test logfile message" );
  #endif

  x.xbSleep( 250 );
  if( iPo == 2 ){
    std::cout << "DisplayError Test ==> ";
    x.DisplayError( 0 );
  }

  #ifdef WIN32
  // cSep = '\\';
  iRc += TestMethod( iPo, "GetPathSeparator()", x.GetPathSeparator(), cSep );
  #else
  // char cSep = '/';
  iRc += TestMethod( iPo, "GetPathSeparator()", x.GetPathSeparator(), cSep );
  #endif


  xbString sDir  = "\\dir\\path";
  xbString sFile = "myfile";
  xbString sExt  = "DBF";
  xbString sFqn;
  iRc += TestMethod( iPo, "CreateFQN()", x.CreateFqn( sDir, sFile, sExt, sFqn ), XB_NO_ERROR );
  //std::cout << "FQN = [" << sFqn.Str() << "]\n";
  #ifdef WIN32 
    iRc += TestMethod( iPo, "CreateFQN()", sFqn.Str(), "\\dir\\path\\myfile.DBF", 20 );
  #else
    iRc += TestMethod( iPo, "CreateFQN()", sFqn.Str(), "/dir/path/myfile.DBF", 20 );
  #endif

  sDir  = "/dir/path";
  iRc += TestMethod( iPo, "CreateFQN()", x.CreateFqn( sDir, sFile, sExt, sFqn ), XB_NO_ERROR );
  #ifdef WIN32 
    iRc += TestMethod( iPo, "CreateFQN()", sFqn.Str(), "\\dir\\path\\myfile.DBF", 20 );
  #else
    iRc += TestMethod( iPo, "CreateFQN()", sFqn.Str(), "/dir/path/myfile.DBF", 20 );
  #endif


  x.SetDataDirectory( "/ABCDEFG/" );
  #ifdef WIN32
    iRc += TestMethod( iPo, "Set/GetDataDirectory()", x.GetDataDirectory(),  "\\ABCDEFG\\", 9 );
  #else
    iRc += TestMethod( iPo, "Set/GetDataDirectory()", x.GetDataDirectory(),  "/ABCDEFG/", 9 );
  #endif

  x.SetDataDirectory( "/ABCDEFG" );
  #ifdef WIN32
    iRc += TestMethod( iPo, "Set/GetDataDirectory()", x.GetDataDirectory(),  "\\ABCDEFG", 8 );
  #else
    iRc += TestMethod( iPo, "Set/GetDataDirectory()", x.GetDataDirectory(),  "/ABCDEFG", 8 );
  #endif

  x.SetDataDirectory( "\\ABCDEFG\\");
  #ifdef WIN32
    iRc += TestMethod( iPo, "Set/GetDataDirectory()", x.GetDataDirectory(),  "\\ABCDEFG\\", 9 );
  #else
    iRc += TestMethod( iPo, "Set/GetDataDirectory()", x.GetDataDirectory(),  "/ABCDEFG/", 9 );
  #endif

  x.SetDataDirectory( "\\ABCDEFG" );
  #ifdef WIN32
    iRc += TestMethod( iPo, "Set/GetDataDirectory()", x.GetDataDirectory(),  "\\ABCDEFG", 8 );
  #else
    iRc += TestMethod( iPo, "Set/GetDataDirectory()", x.GetDataDirectory(),  "/ABCDEFG", 8 );
  #endif

#ifdef XB_LOGGING_SUPPORT

  x.SetLogDirectory( "ABCDEFG" );
  iRc += TestMethod( iPo, "Set/GetLogDirectory()", x.GetLogDirectory(),  "ABCDEFG", 7 );
  x.SetLogDirectory( "/ABCDEFG/" );
  #ifdef WIN32
    iRc += TestMethod( iPo, "Set/GetLogDirectory()", x.GetLogDirectory(),  "\\ABCDEFG\\", 9 );
  #else
    iRc += TestMethod( iPo, "Set/GetLogDirectory()", x.GetLogDirectory(),  "/ABCDEFG/", 9 );
  #endif

  x.SetLogDirectory( "/ABCDEFG" );
  #ifdef WIN32
    iRc += TestMethod( iPo, "Set/GetLogDirectory()", x.GetLogDirectory(),  "\\ABCDEFG", 8 );
  #else
    iRc += TestMethod( iPo, "Set/GetLogDirectory()", x.GetLogDirectory(),  "/ABCDEFG", 8 );
  #endif

  x.SetLogDirectory( "\\ABCDEFG\\");
  #ifdef WIN32
    iRc += TestMethod( iPo, "Set/GetLogDirectory()", x.GetLogDirectory(),  "\\ABCDEFG\\", 9 );
  #else
    iRc += TestMethod( iPo, "Set/GetLogDirectory()", x.GetLogDirectory(),  "/ABCDEFG/", 9 );
  #endif

  x.SetLogFileName( "LogFileNameTest" );
  iRc += TestMethod( iPo, "Set/GetLogFileName()", x.GetLogFileName(),  "LogFileNameTest", 15 );
#endif

  x.SetTempDirectory( "/ABCDEFG/" );
  #ifdef WIN32
    iRc += TestMethod( iPo, "Set/GetTempDirectory()", x.GetTempDirectory(),  "\\ABCDEFG\\", 9 );
  #else
    iRc += TestMethod( iPo, "Set/GetTempDirectory()", x.GetTempDirectory(),  "/ABCDEFG/", 9 );
  #endif

  x.SetTempDirectory( "/ABCDEFG" );
  #ifdef WIN32
    iRc += TestMethod( iPo, "Set/GetTempDirectory()", x.GetTempDirectory(),  "\\ABCDEFG", 8 );
  #else
    iRc += TestMethod( iPo, "Set/GetTempDirectory()", x.GetTempDirectory(),  "/ABCDEFG", 8 );
  #endif

  x.SetTempDirectory( "\\ABCDEFG\\");
  #ifdef WIN32
    iRc += TestMethod( iPo, "Set/GetTempDirectory()", x.GetTempDirectory(),  "\\ABCDEFG\\", 9 );
  #else
    iRc += TestMethod( iPo, "Set/GetTempDirectory()", x.GetTempDirectory(),  "/ABCDEFG/", 9 );
  #endif


//  std::cout << "path separator = [" << x.GetPathSeparator() << "]\n";

  if( iPo > 0 || iRc < 0 )
    fprintf( stdout, "Total Errors = %d\n", iRc * -1 );


  #ifdef XB_LOGGING_SUPPORT
  sMsg.Sprintf( "Program [%s] terminating with [%d] errors...", av[0], iRc * -1 );
  x.WriteLogMessage( sMsg );
  #endif

  return iRc;
}

