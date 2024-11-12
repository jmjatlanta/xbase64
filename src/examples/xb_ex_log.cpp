/*  xb_ex_log.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023,2024 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

// This program demostrates how to use logging


#include "xbase.h"

using namespace xb;

int main( int, char **av )
{

  #ifdef XB_LOGGING_SUPPORT

  xbXBase  x;
  xbString sMsg;


  std::cout << "Default Logfile Name is: [" << x.GetLogFqFileName().Str() 
            << "]  Rollover size = [" << x.GetLogSize() 
            << "]" << std::endl;

  if( x.GetLogStatus() )
    std::cout << "Logging is active" << std::endl;
  else
    std::cout << "Logging is inactive" << std::endl;

  x.SetLogDirectory( PROJECT_LOG_DIR );               // use the library log directory
  x.SetLogFileName ( "MySpecialLogFile.txt" );        // set to use a special name
  x.SetLogSize     ( x.GetLogSize() * 2 );            // double the log file size

  // enable the logfile and write a message for the new settings to take effect
  x.EnableMsgLogging();
  sMsg.Sprintf( "Program [%s] initializing...", av[0] );
  x.WriteLogMessage( sMsg );

  std::cout << "Logfile Name is: [" << x.GetLogFileName().Str()
            << "]\nFully Qualified Logfile Name is: [" <<  x.GetLogFqFileName().Str()
            << "]\nRollover size = [" << x.GetLogSize() 
            << "]" << std::endl;

  if( x.GetLogStatus() )
    std::cout << "Logging is active" << std::endl;
  else
    std::cout << "Logging is inactive" << std::endl;

  // write some messages to the logfile
  for( int i = 0; i < 5; i++ ){
    sMsg.Sprintf( "Test message [%d]", i );
    x.WriteLogMessage( sMsg );
  }

  sMsg.Sprintf( "Program [%s] terminating..", av[0] );
  x.WriteLogMessage( sMsg );

  x.FlushLog();   // not really needed, but here for demonstration purposes

  #endif          // B_LOGGING_SUPPORT

  return 0;
}




