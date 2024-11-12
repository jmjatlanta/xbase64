/*  xbfuncs.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2017,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

#include "xbase.h"

#ifdef XB_FUNCTION_SUPPORT

namespace xb{


// All funtions have a similar structure, return an xbInt16 return code
// Have a variable number of input operands and one output operand

/************************************************************************/
xbInt16 xbXBase::ABS( xbDouble dIn, xbDouble &dOut  ){
  if( dIn < 0 )
    dOut = dIn * -1;
  else
    dOut = dIn;
  return XB_NO_ERROR;
}
/*************************************************************************/
xbInt16 xbXBase::ALLTRIM( const xbString &sIn, xbString &sOut ){
  sOut = sIn;
  sOut.Trim();
  return XB_NO_ERROR;
}
/*************************************************************************/
xbInt16 xbXBase::ASC( const xbString &sIn, xbDouble &dAscOut ){
  if( sIn == "" )
    return XB_PARSE_ERROR;

  dAscOut = sIn[1];
  return XB_NO_ERROR;
}
/*************************************************************************/
xbInt16 xbXBase::AT( const xbString &s1, const xbString &s2, xbDouble &dPos ){
  /* looks for s1 in s2 */
  xbInt32 lCnt = 0;
  const char *p;
  const char *p2 = s2;
  if( strlen( s1 ) > strlen( s2 )) return 0;
  if(( p = strstr( s2, s1 )) == NULL )
    return XB_NO_ERROR;
  while( p2++ != p ) lCnt++;
  dPos = lCnt + 1;
  return XB_NO_ERROR;
}
/*************************************************************************/
xbInt16 xbXBase::CDOW( xbDate &dInDate, xbString &sOutDow ){
  return dInDate.CharDayOf( sOutDow );
}
/*************************************************************************/
xbInt16 xbXBase::CHR( xbDouble dAsciCd, xbString &sOut ){
  static char buf[2];
  buf[0] = (char) dAsciCd;
  buf[1] = 0x00;
  sOut = buf;
  return XB_NO_ERROR;
}
/*************************************************************************/
xbInt16 xbXBase::CMONTH( xbDate &dInDate, xbString &sOutMonth ){
  return dInDate.CharMonthOf( sOutMonth );
}
/*************************************************************************/
xbInt16 xbXBase::CTOD( const xbString &sInDate, xbDate &dOutDate ){
  return dOutDate.CTOD( sInDate );
}
/*************************************************************************/
xbInt16 xbXBase::DATE( xbDate &dOutDate ){
  dOutDate.Sysdate();
  return XB_NO_ERROR;
}
/*************************************************************************/
xbInt16 xbXBase::DAY( const xbDate &dInDate, xbDouble &dOutDay ){
  xbInt16 iOutDay;;
  iOutDay = dInDate.DayOf( XB_FMT_MONTH );
  if( iOutDay < 0 ){
    return iOutDay;
  }else{ 
    dOutDay = iOutDay;
    return XB_NO_ERROR;
  }
}
/*************************************************************************/
xbInt16 xbXBase::DEL( xbDbf *dbf , xbString &sOut, xbInt16 iRecBufSw ) {
  if( dbf ){
    if( dbf->RecordDeleted( iRecBufSw ))
      sOut = "*";
    else
      sOut = " ";
    return XB_NO_ERROR;
  } else {
    return XB_PARSE_ERROR;
  }
}
/*************************************************************************/
xbInt16 xbXBase::DELETED( xbDbf *dbf , xbBool &bOut, xbInt16 iRecBufSw ) {
  if( dbf ){
    bOut = dbf->RecordDeleted( iRecBufSw );
    return XB_NO_ERROR;
  } else {
    return XB_PARSE_ERROR;
  }
}
/*************************************************************************/
xbInt16 xbXBase::DESCEND( const xbDate &dtInDate, xbDate &dtOutDate ){
  xbDate d( "29991231" );
  dtOutDate.JulToDate8( 2415021 + d.JulianDays() - dtInDate.JulianDays());
  return XB_NO_ERROR;
}
/*************************************************************************/
xbInt16 xbXBase::DESCEND( xbDouble dIn, xbDouble &dOut ){
  dOut = dIn * -1;
  return XB_NO_ERROR;
}
/*************************************************************************/
xbInt16 xbXBase::DESCEND( const xbString &sIn, xbString &sOut ){
  sOut = sIn;
  for( xbUInt32 l = 0; l < sIn.Len(); l++ )
    sOut.PutAt( l+1, (char) (255 - sOut[l+1]));
  return XB_NO_ERROR;
}
/*************************************************************************/
xbInt16 xbXBase::DOW( const xbDate &dtInDate, xbDouble &dDowOut ){
  xbInt16 iDow;
  iDow = dtInDate.DayOf( XB_FMT_WEEK );
  if( iDow < 0 ){
    return XB_PARSE_ERROR;
  }else{ 
    dDowOut = iDow;
    return XB_NO_ERROR;
  }
}
/*************************************************************************/
xbInt16 xbXBase::DTOC( xbDate &dInDate, xbString &sOutFmtDate  ){
  return dInDate.FormatDate( "MM/DD/YY", sOutFmtDate );
}
/*************************************************************************/
xbInt16 xbXBase::DTOS( xbDate &dtInDate, xbString &sOutFmtDate ){
  return dtInDate.FormatDate( "YYYYMMDD", sOutFmtDate );
}
/*************************************************************************/
xbInt16 xbXBase::EXP( xbDouble dIn, xbDouble &dOut )
{
  dOut = exp( dIn );
  return XB_NO_ERROR;
}
/*************************************************************************/
xbInt16 xbXBase::IIF( xbBool bResult, const xbString &sTrueResult, const xbString &sFalseResult, xbString &sResult )
{
  if( sFalseResult.Len() != sTrueResult.Len())
    return XB_INCONSISTENT_PARM_LENS;
  if( bResult )
    sResult = sTrueResult;
  else
    sResult = sFalseResult;
  return XB_NO_ERROR;
}
/*************************************************************************/
xbInt16 xbXBase::INT( xbDouble dIn, xbDouble &dOut )
{
  xbInt64 ll = (xbInt64) dIn;
  dOut = (xbDouble) ll;
  return XB_NO_ERROR;
}
/*************************************************************************/
xbInt16 xbXBase::ISALPHA( const xbString &sIn, xbBool &bResult ){
  if( isalpha(sIn[1]))
    bResult = 1;
  else
    bResult = 0;
  return XB_NO_ERROR;
}
/*************************************************************************/
xbInt16 xbXBase::ISLOWER( const xbString &sIn, xbBool &bResult ){
  if( islower(sIn[1])) 
    bResult = 1;
  else
    bResult = 0;
  return XB_NO_ERROR;
}
/*************************************************************************/
xbInt16 xbXBase::ISUPPER( const xbString &sIn, xbBool &bResult ){
  if( isupper(sIn[1]))
    bResult = 1;
  else
    bResult = 0;
  return XB_NO_ERROR;
}
/*************************************************************************/
xbInt16 xbXBase::LEFT( const xbString &sIn, xbUInt32 ulCharCnt, xbString &sOut ){
  sOut.Assign( sIn, 1, ulCharCnt );
  return XB_NO_ERROR;
}
/*************************************************************************/
xbInt16 xbXBase::LEN( const xbString &sIn, xbDouble &dOut ){
  dOut = sIn.Len();
  return XB_NO_ERROR;
}
/*************************************************************************/
xbInt16 xbXBase::LOG( xbDouble dIn, xbDouble &dOut ){
  dOut = log( dIn );
  return XB_NO_ERROR;
}
/*************************************************************************/
xbInt16 xbXBase::LOWER( const xbString &sIn, xbString &sOut ){
  sOut = sIn;
  sOut.ToLowerCase();
  return XB_NO_ERROR;
}
/*************************************************************************/
xbInt16 xbXBase::LTRIM( const xbString &sIn, xbString &sOut ){
  sOut = sIn;
  sOut.Ltrim();
  return XB_NO_ERROR;
}
/*************************************************************************/
xbInt16 xbXBase::MAX( xbDouble d1, xbDouble d2, xbDouble &dOut )
{
  if( d1 > d2 ) 
    dOut = d1;
  else
    dOut = d2;
  return XB_NO_ERROR;
}
/*************************************************************************/
xbInt16 xbXBase::MIN( xbDouble d1, xbDouble d2, xbDouble &dOut )
{
  if( d1 < d2 ) 
    dOut = d1;
  else
    dOut = d2;
  return XB_NO_ERROR;
}
/*************************************************************************/
xbInt16 xbXBase::MONTH( xbDate &dInDate, xbDouble &dOutMonth ){
  xbInt16 iRc = dInDate.MonthOf();
  if( iRc < 0 )
    return XB_PARSE_ERROR;
  else{
    dOutMonth = iRc;
    return XB_NO_ERROR;
  }
}
/*************************************************************************/
xbInt16 xbXBase::RECCOUNT( xbDbf *dbf , xbDouble &dOut ) {
  xbUInt32 ulRecCnt;
  xbInt16 iRc = dbf->GetRecordCnt( ulRecCnt );
  dOut = (xbDouble) ulRecCnt;
  return iRc;
}
/*************************************************************************/
xbInt16 xbXBase::RECNO( xbDbf *dbf , xbDouble &dOut ) {
  if( dbf ){
    dOut = (xbDouble) dbf->GetCurRecNo();
    return XB_NO_ERROR;
  } else {
    dOut = -1;
    return XB_PARSE_ERROR;
  }
}
/*************************************************************************/
xbInt16 xbXBase::REPLICATE( const xbString &sIn, xbUInt32 ulRepCnt, xbString &sOut ){
  sOut = "";
  for( xbUInt32 i = 0; i < ulRepCnt; i++ )
    sOut += sIn;
  return XB_NO_ERROR;
}
/*************************************************************************/
xbInt16 xbXBase::RIGHT( const xbString &sIn, xbUInt32 ulCharCnt, xbString &sOut ){
  if( sIn.Len() < ulCharCnt )
    sOut = sIn;
  else
    sOut.Assign( sIn, sIn.Len() - ulCharCnt + 1, ulCharCnt );
  return XB_NO_ERROR;
}
/*************************************************************************/
xbInt16 xbXBase::RTRIM( const xbString &sIn, xbString &sOut ){
  sOut = sIn;
  sOut.Rtrim();
  return XB_NO_ERROR;
}
/*************************************************************************/
xbInt16 xbXBase::SPACE( xbInt32 lCnt, xbString &sOut ){
  sOut = "";
  for( xbInt32 i = 0; i < lCnt; i++ )
    sOut += ' ';
  return XB_NO_ERROR;
}
/*************************************************************************/
xbInt16 xbXBase::SQRT( xbDouble dBase, xbDouble &dSqrRoot )
{
  dSqrRoot = sqrt( dBase );
  return XB_NO_ERROR;
}
/*************************************************************************/
xbInt16 xbXBase::STOD( const xbString &sInDate, xbDate &dtOutDate ){
  if( dtOutDate.DateIsValid( sInDate )){
    dtOutDate = sInDate;
    return XB_NO_ERROR;
  } else {
    return XB_INVALID_DATE;
  }
}
/*************************************************************************/
xbInt16 xbXBase::STR( xbDouble dIn, xbString &sOut) {
  xbString sPadChar = " ";
  return STR( dIn, 10, 0, sPadChar, sOut );
}
/*************************************************************************/
xbInt16 xbXBase::STR( xbDouble dIn, xbUInt32 ulLen, xbString &sOut) {
  xbString sPadChar = " ";
  return STR( dIn, ulLen, 0, sPadChar, sOut );
}
/*************************************************************************/
xbInt16 xbXBase::STR( xbDouble dIn, xbUInt32 ulLen, xbUInt32 ulDec, xbString &sOut) {
  xbString sPadChar = " ";
  return STR( dIn, ulLen, ulDec, sPadChar, sOut );
}
/*************************************************************************/
xbInt16 xbXBase::STR( xbDouble dIn, xbUInt32 ulLen, xbUInt32 ulDec, xbString &sPadChar, xbString &sOut) {
  xbString sFmt;
  sFmt.Sprintf( "%c%d.%df", '%', ulLen, ulDec );
  sOut.Sprintf( sFmt.Str(), dIn, 0 );
  // convert to all "*" if result is too long
  if( sOut.Len() > ulLen ){
    sOut = "*";
    do{
      sOut += "*";
    } while( ulLen > sOut.Len());
  } else if( sPadChar.Len() > 0 && sPadChar != " " ){
    // this logic doesn't make sense when processing negative numbers,
    // but it does behave like the original dBASE
    //   you could end up with something like  0000-12.17 when you really want -000012.17
    // that is probably why the STRZERO function came into being
    xbUInt32 l = 1;
    while( sOut[l] == ' ' ){
      sOut.PutAt( l, sPadChar[1]);
      l++;
    }
  }
  return XB_NO_ERROR;
}
/*************************************************************************/
xbInt16 xbXBase::STRZERO( xbDouble dIn, xbUInt32 ulLen, xbUInt32 ulDec, xbString &sOut){

  xbString sFmt;
  if( dIn < 0 )
    sFmt.Sprintf( "%c+0%d.%df", '%', ulLen, ulDec );
  else
    sFmt.Sprintf( "%c0%d.%df", '%', ulLen, ulDec );
  sOut.Sprintf( sFmt.Str(), dIn );

  // convert to all "*" if result is too long
  if( sOut.Len() > ulLen ){
    sOut = "*";
    do{
      sOut += "*";
    } while( ulLen > sOut.Len());
  }
  return XB_NO_ERROR;
}
/*************************************************************************/
xbInt16 xbXBase::SUBSTR( const xbString &sIn, xbUInt32 ulStartPos, xbUInt32 ulLen, xbString &sOut ){
  sOut = sIn;
  sOut.Mid( ulStartPos, ulLen );
  return XB_NO_ERROR;
}
/*************************************************************************/
xbInt16 xbXBase::TRIM( const xbString &sIn, xbString &sOut ){
  return RTRIM( sIn, sOut );
}
/*************************************************************************/
xbInt16 xbXBase::UPPER( const xbString &sIn, xbString &sOut ){
  sOut = sIn;
  sOut.ToUpperCase();
  return XB_NO_ERROR;
}
/*************************************************************************/
xbInt16 xbXBase::VAL( const xbString &sIn, xbDouble &dOut )
{
  if( sIn )
    // strtod(nptr,NULL);
    dOut = atof( sIn );
  else
    dOut = 0;
  return XB_NO_ERROR;
}
/*************************************************************************/
xbInt16 xbXBase::YEAR( xbDate &dInDate, xbDouble &dOutYear ){

  xbInt16 iRc = dInDate.YearOf();
  if( iRc < 0 )
    return XB_PARSE_ERROR;
  else{
    dOutYear = iRc;
    return XB_NO_ERROR;
  }
}
/*************************************************************************/
};         // namespace
#endif     // XB_FUNCTION_SUPPORT
/*************************************************************************/
