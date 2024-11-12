/*  xblog.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023,2024 Gary A Kunkel

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
xbLog::xbLog() : xbFile( NULL ){
  SetDirectory( xbase->GetLogDirectory());
  SetFileName ( xbase->GetLogFileName());
  bLoggingStatus = xbFalse;
  lLogSize       = 100000;
  #ifdef XB_LOCKING_SUPPORT
  iShareMode = XB_MULTI_USER;
  #else
  iShareMode = XB_SINGLE_USER;
  #endif
}
/******************************************************************************/
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
xbLog::~xbLog(){
  xbFclose();
}
/******************************************************************************/
xbBool xbLog::LogGetStatus(){
  return bLoggingStatus;
}
/******************************************************************************/
xbInt16 xbLog::LogClose(){
  return xbFclose();
}
/******************************************************************************/
void xbLog::LogSetLogSize( size_t lSize ){
  lLogSize = lSize;
}
/******************************************************************************/
void xbLog::LogSetStatus( xbBool bStatus ){
  if( bLoggingStatus && !bStatus )
    LogClose();
  bLoggingStatus = bStatus;
}
/******************************************************************************/
xbInt16 xbLog::LogOpen(){
  xbInt16  rc;
  // 4.1.3 added next two lines for dynamic log file name changing
  SetDirectory( xbase->GetLogDirectory());
  SetFileName ( xbase->GetLogFileName());
  if(( rc = xbFopen( "a", iShareMode )) != XB_NO_ERROR )
    return rc;
  // xbFTurnOffFileBuffering();, handled in the open
  return XB_NO_ERROR;
}
/******************************************************************************/
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
    #if defined(HAVE__LOCALTIME64_S_F)
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
size_t xbLog::LogGetLogSize() const { 
  return lLogSize; 
}
/******************************************************************************/

}         // namespace
#endif    // XB_LOGGING_ON




