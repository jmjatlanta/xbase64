/*  xbtime.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023,2024 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

This module handles uda (user data area) methods 

*/

#include "xbase.h"

#ifdef XB_DBF5_SUPPORT

namespace xb{

/************************************************************************/
xbTime::xbTime() {
  lTime = -1;
}
/************************************************************************/
xbTime::xbTime( xbString &sTime ){
  Set( sTime );
}
/************************************************************************/
xbTime::xbTime( char  *sTime ){
  Set( sTime );
}
/************************************************************************/
xbTime::xbTime( xbInt32 lTime ){
  if( lTime == 0 )
    Systime();
  else
    this->lTime = lTime;
}
/************************************************************************/
xbTime::xbTime( xbInt16 iHH, xbInt16 iMM, xbInt16 iSS ){
  Set( iHH, iMM, iSS );
}
/************************************************************************/
xbInt32 xbTime::Add( char cType, xbInt32 lCnt ){

  if( cType == 'S' || cType == 's' ){

  } else if( cType == 'M' || cType == 'm' ){
    lCnt *= 60;
  } else if( cType == 'H' || cType == 'h' ){
    lCnt *= 3600;
  } else 
    return 0;

  xbInt32 lDays = lCnt / 86400L;   // no of seconds in a day
  time_t t = ToTimeT();
  t += lCnt;
  FromTimeT( t );
  return lDays;
}
/************************************************************************/
#ifdef XB_DEBUG_SUPPORT
void xbTime::Dump(){
  xbString s = "";
  Dump( s );
}
/************************************************************************/
void xbTime::Dump( const char * sHeader ){
  xbString s = sHeader;
  Dump( s );
}
/************************************************************************/
void xbTime::Dump( xbString &sHeader ){
  if( sHeader != "" )
    std::cout << sHeader.Str() << " ";
  if( lTime == -1 ){
    std::cout << "Null Time\n";
    return;
  }
  std::cout << "Time = [" << lTime << "] Hour = [" << GetHour() << "] Minute = [" << GetMinute() << "] Second = " << GetSecond() << "] Current Format = " << Str() << "]\n";

  std::cout << "Current Format = [" << GetTimeFormat() << "]  Time = [" << Str() << "]\n";
}
#endif
/************************************************************************/
xbInt16 xbTime::FromTimeT( time_t t ){
  #if defined HAVE__LOCALTIME64_S_F
    // _localtime64_s( &tb, &t );
    struct tm tb;
    _localtime64_s( &tb, &t );
    lTime = (tb.tm_hour * 3600000L) + (tb.tm_min * 60000L) + ( tb.tm_sec * 1000L);
  #else
    struct tm *tb;
    tb = localtime( &t );
    lTime = (tb->tm_hour * 3600000L) + (tb->tm_min * 60000L) + ( tb->tm_sec * 1000L);
  #endif
  return XB_NO_ERROR;
}
/************************************************************************/
xbString & xbTime::GetDefaultTimeFormat() const {
  return sDefaultTimeFormat;
}
/************************************************************************/
xbInt16 xbTime::GetHour() const {
  return lTime / 3600000L;
}
/************************************************************************/
xbInt16 xbTime::GetMinute() const {
  return  (lTime % 3600000L) / 60000L;
}
/************************************************************************/
xbInt16 xbTime::GetSecond() const {
  return  ((lTime % 3600000L) % 60000L) / 1000L;
}
/************************************************************************/
xbInt32 xbTime::GetTime() const {
  return lTime;
}
/************************************************************************/
xbInt16 xbTime::GetTime( xbString &sTimeOut ){
  xbString s;
  return GetTime( sTimeOut, s );
}
/************************************************************************/
xbInt16 xbTime::GetTime( xbString &sTimeOut, xbString &sFmtIn ){
  xbString sUseFmt;
  if( sFmtIn != "" )
    sUseFmt = sFmtIn;
  else if( sFmt != "" )
    sUseFmt = sFmt;
  else if( GetDefaultTimeFormat() != "" )
    sUseFmt = GetDefaultTimeFormat();
  else
    sUseFmt = "%H:%M:%S";
  char sBuf[30];
  memset( sBuf, 0x00, 30 );
  struct tm tBlock;
  memset( &tBlock, 0x00, sizeof( struct tm ));
  tBlock.tm_sec  = GetSecond();
  tBlock.tm_min  = GetMinute();
  tBlock.tm_hour = GetHour();
  xbInt16 iRc = strftime( sBuf, 29, sUseFmt.Str(), &tBlock );
  sTimeOut = sBuf;
  return iRc;
}
/************************************************************************/
xbInt16 xbTime::GetTime( xbString &sTimeOut, const char * sFmtIn ){
  xbString sFmt( sFmtIn );
  return GetTime( sTimeOut, sFmt );
}
/************************************************************************/
const xbString & xbTime::GetTimeFormat() const{
  if( sFmt == "" )
    return sDefaultTimeFormat;
  else
    return sFmt;
}
/************************************************************************/
xbBool xbTime::operator!=( const xbTime &tTm ) const {
  if( lTime != tTm.GetTime())
    return true;
  else
    return false;
}
/************************************************************************/
xbInt32 xbTime::operator+(xbInt32 l ) const {
  return lTime + l;
}
/*************************************************************************/
xbTime &xbTime::operator++(xbInt32){

  Add( 'S', 1 );
  return (*this);
}
/************************************************************************/
xbTime &xbTime::operator+=( xbInt32 l ) {

  Add( 'S', l );
  return (*this);
}
/************************************************************************/
xbInt32 xbTime::operator-(xbInt32 l ) const {
  return lTime - l;
}
/*************************************************************************/
xbTime &xbTime::operator--(xbInt32){

  Add( 'S', -1 );
  return (*this);
}
/************************************************************************/
xbTime &xbTime::operator-=( xbInt32 l ) {

  Add( 'S', (l * -1));
  return (*this);
}
/*************************************************************************/
xbBool xbTime::operator<( const xbTime &tm ) const {
  if( lTime < tm.GetTime() )
    return xbTrue;
  else
    return xbFalse;
}
/*************************************************************************/
xbBool xbTime::operator<=( const xbTime &tm ) const {
  if( lTime <= tm.GetTime() )
    return xbTrue;
  else
    return xbFalse;
}
/*************************************************************************/
void xbTime::operator=( xbInt32 t ){
  Set( t );
}
/*************************************************************************/
void xbTime::operator=( const xbTime &tTm ){
  Set( tTm.GetTime() );
  xbString s = tTm.GetTimeFormat();
  this->SetTimeFormat( s );
}
/************************************************************************/
xbBool xbTime::operator==( const xbTime &tTm ) const {

  if( lTime == tTm.GetTime())
    return true;
  else
    return false;
}
/*************************************************************************/
xbBool xbTime::operator>( const xbTime &tm ) const {
  if( lTime > tm.GetTime() )
    return xbTrue;
  else 
    return xbFalse;
}
/*************************************************************************/
xbBool xbTime::operator>=( const xbTime &tm ) const {
  if( lTime >= tm.GetTime() )
    return xbTrue;
  else
    return xbFalse;
}
/************************************************************************/
xbInt16 xbTime::Set( const char *sTime ){
  xbString s = sTime;
  return Set( s );
}
/************************************************************************/
xbInt16 xbTime::Set( xbString &sTime ){

  xbInt16 iCnt = sTime.CountChar( ':' );
  if( iCnt < 1 || iCnt > 2 ){
    lTime = -1;
    return XB_INVALID_PARAMETER;
  }
  xbUInt32 iPos1 = sTime.Pos( ':' );
  xbUInt32 iPos2 = 0;
  if( iCnt > 1 ){
    iPos2 = sTime.Pos( ':', iPos1 + 1 );
    if( iPos2 != (iPos1+2) && iPos2 != (iPos1+3)){
      lTime = -1;
      return XB_INVALID_PARAMETER;
    }
  }
  xbString sHH;
  sHH.Assign( sTime, 1, iPos1 - 1 );
  xbString sMM;
  xbString sSS;
  if( iCnt > 1 ){
    sMM.Assign( sTime, iPos1 + 1, iPos2 - iPos1 - 1 );
    sSS.Assign( sTime, iPos2 + 1 );
  } else {
    sMM.Assign( sTime, iPos1 + 1 );
  }
  Set( atoi( sHH.Str()), atoi( sMM.Str()), atoi( sSS.Str()) );
  return XB_NO_ERROR;
}
/************************************************************************/
xbInt16 xbTime::Set( xbInt32 lTime ){
  this->lTime = lTime;
  return XB_NO_ERROR;
}
/************************************************************************/
xbInt16 xbTime::Set( xbTime &tTm ){
  this->lTime = tTm.GetTime();
  xbString s = tTm.GetTimeFormat();
  this->SetTimeFormat( s );
  return XB_NO_ERROR;
}
/************************************************************************/
xbInt16 xbTime::Set( xbInt16 iHH, xbInt16 iMM, xbInt16 iSS ){

  if( iHH < 0 || iHH > 23 || iMM < 0 || iMM > 59 || iSS < 0 || iSS > 59 ){
    lTime = -1;
    return XB_INVALID_PARAMETER;
  }
  lTime = (iHH * 3600000L) + (iMM * 60000L) + (iSS * 1000L);
  return XB_NO_ERROR;
}
/************************************************************************/
void xbTime::SetDefaultTimeFormat( const char *sDefaultTimeFormat ) {
  this->sDefaultTimeFormat = sDefaultTimeFormat;
}
void xbTime::SetDefaultTimeFormat( const xbString &sDefaultTimeFormat ) {
  this->sDefaultTimeFormat = sDefaultTimeFormat;
}
/************************************************************************/
void xbTime::SetTimeFormat( const char *sFmtIn ){
  this->sFmt = sFmtIn;
}
void xbTime::SetTimeFormat( xbString &sFmtIn ){
  this->sFmt = sFmtIn;
}
/************************************************************************/
xbString & xbTime::Str(){
  xbString sCurrentFormat = GetTimeFormat();
  GetTime( sTime, sCurrentFormat );
  return sTime;
}
/************************************************************************/
xbInt16 xbTime::Systime(){
  #if defined(HAVE__LOCALTIME64_S_F)
    __time64_t timer;
    struct tm tb;
    _time64( &timer );
    _localtime64_s( &tb, &timer );
    return Set(  tb.tm_hour, tb.tm_min, tb.tm_sec );
  #else
    time_t timer;
    struct tm *tb;
    timer = time( NULL );
    tb = localtime( &timer );
    return Set(  tb->tm_hour, tb->tm_min, tb->tm_sec );
  #endif
}
/************************************************************************/
time_t xbTime::ToTimeT() const {
  time_t tt;
  struct tm *ts;
  time( &tt );
  ts = localtime( &tt );
  ts->tm_year  = 100;
  ts->tm_mon   = 0;
  ts->tm_mday  = 1;
  ts->tm_isdst = 0;
  ts->tm_sec   = GetSecond();
  ts->tm_min   = GetMinute();
  ts->tm_hour  = GetHour();
  return mktime( ts );
}
/************************************************************************/
}   /* namespace */ 
#endif         /*  XB_DBF5_SUPPORT */