/*  xb_ex_core.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2021,2022,2023,2024 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

This program demonstrates using functionality of the xbCore class  (Shared system values)

*/

#include "xbase.h"

using namespace xb;

//int main( int ac, char ** av ){

int main( int, char ** av ){

  xbXBase  x;              // set up xbase for business
  xbString sMsg;           // a message string

  sMsg.Sprintf( "Program [%s] initializing...", av[0] );
  std::cout << sMsg.Str() << std::endl;


  // example code to set up log file usage
  #ifdef XB_LOGGING_SUPPORT
    char     cSeperator;                      // is this a unix (/) or windows (\) file system
    xbString sLog;                            // general string for log file activities
    sLog = x.GetLogFqFileName().Str();        // get the system default log file name
    std::cout << "System default logfile is [" << sLog.Str() << "]" << std::endl;

    cSeperator = sLog.GetPathSeparator();     // get the seperator from 
    std::cout << "Path seperator = [" << cSeperator << "]" << std::endl;

    sLog.Sprintf( "..%c", cSeperator );
    x.SetLogDirectory( sLog );
    std::cout << "sLog = [" << sLog.Str() << "]\n";

    sLog = x.GetLogFqFileName().Str();        // get the system default log file name
    std::cout << "New logfile is [" << sLog.Str() << "]" << std::endl;

    // turn on logging after file name set
    x.EnableMsgLogging();
  #endif  // XB_LOGGING_SUPPORT

  // const char *GetErrorMessage( xbInt16 ErrorCode ) const;
  // void DisplayError( xbInt16 ErrorCode ) const;
  std::cout << "DisplayError( -100 ) - ";
  x.DisplayError( -100 );
  //  << "]" << std::endl;

  // void      SetDefaultDateFormat( const xbString &sDefaultDateFormat );
  // xbString& GetDefaultDateFormat() const;
  std::cout << "GetDefaultDateFormat() - " << x.GetDefaultDateFormat().Str() << std::endl;


  //  void       SetDataDirectory          ( const xbString &sDataDirectory );
  // xbString& GetDataDirectory() const;
  std::cout << "GetDataDirectory() - " << x.GetDataDirectory().Str() << std::endl;

  // xbInt16 GetEndianType() const;
  if( x.GetEndianType() == 'L' )
    std::cout << "Little Endian Architecture." << std::endl;
  else
    std::cout << "Bid Endian Architecture." << std::endl;

  //xbBool GetDefaultAutoCommit() const;
  //void SetDefaultAutoCommit( xbBool bDefaultAutoCommit );
  if( x.GetDefaultAutoCommit())
    std::cout << "AutoCommit is on." << std::endl;
  else
    std::cout << "AutoCommit is off." << std::endl;

  //xbBool     GetMultiUser              () const;
  //void       SetMultiUser              ( xbBool bMultiUser );
  if( x.GetMultiUser())
    std::cout << "Multi user (locking) is enabled." << std::endl;
  else
    std::cout << "Multi user (locking) not enabled." << std::endl;

  #if defined (XB_NDX_SUPPORT) || defined (XB_MDX_SUPPORT)
  // Get / SetDefaultIxTagMode
  if( x.GetDefaultIxTagMode() == XB_IX_DBASE_MODE )
    std::cout << "DefaultTagMode option - XB_IX_DBASE_MODE" << std::endl;
  else if( x.GetDefaultIxTagMode() == XB_IX_XBASE_MODE )
    std::cout << "DefaultTagMode - XB_IX_XBASE_MODE" << std::endl;
  #endif  // (XB_NDX_SUPPORT) || defined (XB_MDX_SUPPORT)

  #ifdef XB_LOCKING_SUPPORT
  //xbInt16    GetDefaultLockRetries     () const;
  //void       SetDefaultLockRetries     ( xbInt16 iRetryCount );
  //xbInt32    GetDefaultLockWait        () const;
  //void       SetDefaultLockWait        ( xbInt32 lRetryWait );
  //xbInt16    GetDefaultLockFlavor      () const;
  //void       SetDefaultLockFlavor      ( xbInt16 iLockFlavor );
  //xbBool     GetDefaultAutoLock        () const;
  //void       SetDefaultAutoLock        ( xbBool bAutoLock );
  //void       EnableDefaultAutoLock     ();
  //void       DisableDefaultAutoLock    ();

  std::cout << "GetDefaultLockRetries() - " << x.GetDefaultLockRetries() << std::endl;
  std::cout << "GetDefaultLockWait() - " << x.GetDefaultLockWait() << std::endl;
  //std::cout << "GetDefaultAutoLock() - " << x.GetDefaultAutoLock() << std::endl;
  #endif  // XB_LOCKING_SUPPORT

  #ifdef XB_MDX_SUPPORT
  // xbInt16    GetCreateMdxBlockSize() const;
  // xbInt16    SetCreateMdxBlockSize( xbInt16 ulBlockSize );
  std::cout << "GetCreateMdxBlockSize() - " << x.GetCreateMdxBlockSize() << std::endl;
  #endif  // XB_MDX_SUPPORT

  #ifdef XB_BLOCKREAD_SUPPORT
  // xbUInt32   GetDefaultBlockReadSize() const;
  // void       SetDefaultBlockReadSize( xbUInt32 ulDfltBlockReadSize );
  std::cout << "GetDefaultBlockReadSize() - " << x.GetDefaultBlockReadSize() << std::endl;  
  #endif  // XB_BLOCKREAD_SUPPORT

  std::cout << std::endl;
  return 0;
}
