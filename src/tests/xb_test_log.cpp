/*  xb_test_log.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

// This program tests the class xbLog

// usage:   xb_test_log QUITE|NORMAL|VERBOSE


#include "xbase.h"

using namespace xb;
#include "tstfuncs.cpp"

int main( int argCnt, char **av )
{
  int rc = 0;

  #ifdef XB_LOGGING_SUPPORT
  int po = 1;          /* print option */
                       /* 0 - QUIET    */
                       /* 1 - NORMAL   */
                       /* 2 - VERBOSE  */

  if( argCnt > 1 ) {
    if( av[1][0] == 'Q' )
      po = 0;
    else if( av[1][0] == 'V' )
      po = 2;
  }

  xbXBase x;

  x.EnableMsgLogging();
  InitTime();
  if( po ){
    std::cout << "Logfile is [" << x.GetLogFqFileName().Str() << "]" << std::endl;
  }
  xbString sMsg;
  sMsg.Sprintf( "Program [%s] initializing...", av[0] );
  x.WriteLogMessage( sMsg );

  // verify first logfile location is correct
  xbString sWork;
  x.GetHomeDir( sWork );
  std::cout << "Home Dir = " << sWork.Str() << std::endl;

  // verify home directory > 0 length
  xbInt32 iLen = (xbInt32) sWork.Len();
  if( sWork.Len() == 0 )
    rc += TestMethod( po, "GetHomeDir()", iLen, 1 );
  else
  rc += TestMethod( po, "GetHomeDir()", iLen, iLen );

  sWork.Sprintf( "%s%cxbase64%clogs", sWork.Str(), 
      x.GetPathSeparator(), x.GetPathSeparator());

  // verify path exists
  // std::cout << "Home Dir = " << sHomeDir.Str() << std::endl;
  xbFile f( &x );
  rc += TestMethod( po, "FileExists()", f.FileExists( sWork ), xbTrue );

 
  sWork.Sprintf( "%s%c%s", sWork.Str(), 
      x.GetPathSeparator(), x.GetLogFileName().Str() );

  // std::cout << "Home Dir = " << sWork.Str() << std::endl;
  rc += TestMethod( po, "FileExists()", f.FileExists( sWork ), xbTrue );
  rc += TestMethod( po, "Get Log Status()", x.GetLogStatus(), xbTrue );


  x.DisableMsgLogging();
  rc += TestMethod( po, "Get Log Status()", x.GetLogStatus(), xbFalse );

  xbString sNewLogFileName = "Logfile2.txt";
  x.SetLogDirectory( PROJECT_LOG_DIR );
  x.SetLogFileName( sNewLogFileName );
  sWork.Sprintf( "%s%c%s",
     PROJECT_LOG_DIR,  x.GetPathSeparator(), sNewLogFileName.Str());

  sMsg.Sprintf( "Switching to logfile [%s]", sWork.Str() );
  x.EnableMsgLogging();

  rc += TestMethod( po, "Get Log Status()", x.GetLogStatus(), xbTrue );

  x.WriteLogMessage( sMsg );
  rc += TestMethod( po, "Get Log Status()", x.GetLogStatus(), xbTrue );
  x.WriteLogMessage( "Test Log Message" );

  rc += TestMethod( po, "FileExists()", f.FileExists( sWork ), xbTrue );


  if( po > 0 || rc < 0 )
    fprintf( stdout, "Total Errors = %d\n", rc * -1 );

  #endif         /* XB_LOGGING_SUPPORT */

  return rc;
}




