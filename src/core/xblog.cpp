/*  xblog.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/


#include "xbase.h"
//#include <time.h>

#ifdef XB_LOGGING_SUPPORT

namespace xb{

/******************************************************************************/
//! @brief Constructor.
xbLog::xbLog() : xbFile( NULL ){


  // std::cout << "xbLog::xbLog(1) Directory = [" << GetLogDirectory() << "]\n";
  // std::cout << "xbLog::xbLog(1) Name = [" << GetLogFileName() << "]\n";

  SetDirectory( GetLogDirectory());
  SetFileName ( GetLogFileName());

  bLoggingStatus = xbFalse;
  lLogSize       = 100000;

  #ifdef XB_LOCKING_SUPPORT
  iShareMode = XB_MULTI_USER;
  #else
  iShareMode = XB_SINGLE_USER;
  #endif
}
/******************************************************************************/
//! @brief Constructor.
/*!
  \param sLogFileName - Log file name.
*/
xbLog::xbLog( const xbString & sLogFileName ) : xbFile( NULL ){
  if( sLogFileName.GetPathSeparator())
    SetFqFileName( sLogFileName );     // file name includes a path
  else
    SetFileName( sLogFileName );       // no file path

  bLoggingStatus = xbFalse;
  lLogSize       = 100000;

  #ifdef XB_LOCKING_SUPPORT
  iShareMode = XB_MULTI_USER;
  #else
  iShareMode = XB_SINGLE_USER;
  #endif

}
/******************************************************************************/
//! @brief Deconstructor.
xbLog::~xbLog(){
  xbFclose();
}
/******************************************************************************/
//! @brief Get the current log status
/*!
  \returns xbTrue - Logging turned on.<br>xbFalse - Logging turned off.
*/
xbBool xbLog::LogGetStatus(){
  return bLoggingStatus;
}
/******************************************************************************/
//! @brief Close the logfile.
/*!
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbLog::LogClose(){
  return xbFclose();
}
/******************************************************************************/
//! @brief Set maximum log file size.
/*!
  \param lSize - New maximum log file size.
  \returns void
*/
void xbLog::LogSetLogSize( size_t lSize ){
  lLogSize = lSize;
}
/******************************************************************************/
//! @brief Set log status.
/*!
  \param bStatus xbTrue - Turn logging on.<br>xbFalse - Turn logging off.
  \returns void
*/
void xbLog::LogSetStatus( xbBool bStatus ){
  if( bLoggingStatus && !bStatus )
    LogClose();
  bLoggingStatus = bStatus;
}
/******************************************************************************/
//! @brief Open the logfile.
/*!
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbLog::LogOpen(){
  xbInt16  rc;

//  std::cout << "*****\nxbLog::LogOpen(1) GetLogDirectory = " << GetLogDirectory() << "\n";
//  std::cout << "xbLog::LogOpen(1) GetLogFileName  = " << GetLogFileName() << "\n";
//  std::cout << "xbLog::GetFqFileName(1) = " << GetFqFileName() << "\n\n";

  // 4.1.3 added next two lines for dynamic log file name changing
  SetDirectory( GetLogDirectory());
  SetFileName ( GetLogFileName());

//  std::cout << "*****\nxbLog::LogOpen(2) GetLogDirectory = " << GetLogDirectory() << "\n";
//  std::cout << "xbLog::LogOpen(2) GetLogFileName  = " << GetLogFileName() << "\n";
//  std::cout << "xbLog::GetFqFileName(2) = " << GetFqFileName() << "\n\n";

  if(( rc = xbFopen( "a", iShareMode )) != XB_NO_ERROR )
    return rc;
  xbFTurnOffFileBuffering();
  return XB_NO_ERROR;
}
/******************************************************************************/
//! @brief Write a logfile message.
/*!
  \param sLogEntryData - Message to write to the logfile.
  \param iOutputOption 0 - Write to logfile.<br>
                       1 - Write to stdout.<br>
                       2 - Write to both logfile and stdout.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbLog::LogWrite( const xbString &sLogEntryData, xbInt16 iOutputOption ){

  if( bLoggingStatus == xbFalse ){    // logging turned off
    return XB_NO_ERROR;
  }
  xbInt16 rc = 0;
  if( iOutputOption != 1 && !FileIsOpen() ){
    if(( rc = LogOpen()) != XB_NO_ERROR ){
      fprintf( stderr, "Error - cant write to logfile\n" );
      return rc;
    }
  }
  if( iOutputOption != 1 && lLogSize < xbFtell()){
    xbFputs( "Swapping to next log file" );
    xbFclose();
    xbString sBackupName;
    sBackupName.Sprintf( "%s.bak", GetFqFileName().Str());
    if( FileExists( sBackupName ))
      xbRemove( sBackupName );

    xbRename( GetFqFileName(), sBackupName );
    xbFopen( "a", iShareMode );
  }
  xbString sTimeStamp;
  xbString sFled;     // formatted log entry data

  if( iOutputOption != 1 ){
    #ifdef HAVE__LOCALTIME64_S_F
      __time64_t timer;
      struct tm tb;
      _time64( &timer );
      _localtime64_s( &tb, &timer );
      tb.tm_year += 1900;
      tb.tm_mon++;
      sTimeStamp.Sprintf( "%4d-%02d-%02d %02d:%02d:%02d", tb.tm_year, tb.tm_mon, tb.tm_mday, tb.tm_hour, tb.tm_min, tb.tm_sec );
    #else
      time_t timer;
      struct tm *tb;
      timer = time( NULL );
      tb = localtime( &timer );
      tb->tm_year += 1900;
      tb->tm_mon++;
      sTimeStamp.Sprintf( "%4d-%02d-%02d %02d:%02d:%02d", tb->tm_year, tb->tm_mon, tb->tm_mday, tb->tm_hour, tb->tm_min, tb->tm_sec );
    #endif
    sFled.Sprintf( "%s - %s\n", sTimeStamp.Str(), sLogEntryData.Str() );
  }

  switch( iOutputOption ){
    case 0:
      xbFputs( sFled );
      break;
    case 1:
      std::cout << sLogEntryData << std::endl;
      break;
    case 2:
      xbFputs( sFled );
      std::cout << sLogEntryData << std::endl;
      break;
  }
  return XB_NO_ERROR;
}
/******************************************************************************/
//! @brief Write bytes to logfile.
/*!
  \param ulByteCnt - Number of bytes to write to logfile.
  \param p - Pointer to data to write to logfile.
  \returns XB_NO_ERROR
*/

xbInt16 xbLog::LogWriteBytes( xbUInt32 ulByteCnt, const char *p ){

  if( bLoggingStatus == xbFalse )    // logging turned off
    return XB_NO_ERROR;
  const char *p2 = p;
  xbFputc( '[' );
  for( xbUInt32 l = 0; l < ulByteCnt; l++ )
    xbFputc( *p2++ );
  xbFputc( ']' );
  return XB_NO_ERROR;
}
/******************************************************************************/
}         // namespace
#endif    // XB_LOGGING_ON




