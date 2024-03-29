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
//! @brief Calculate absolute value of a numeric expression.
/*!
  Expression function ABS().
  \param dIn Input - Numeric expression.
  \param dOut Output - Absolute value.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbXBase::ABS( xbDouble dIn, xbDouble &dOut  ){
  if( dIn < 0 )
    dOut = dIn * -1;
  else
    dOut = dIn;
  return XB_NO_ERROR;
}

/*************************************************************************/
//! @brief Trim leading and trailing white space from a string.
/*!
  Expression function ALLTRIM().
  \param sIn Input - Input string to trim.
  \param sOut Output - Trimmed string.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbXBase::ALLTRIM( const xbString &sIn, xbString &sOut ){
  sOut = sIn;
  sOut.Trim();
  return XB_NO_ERROR;
}

/*************************************************************************/
//! @brief Return ASCII code for the first character in a string.
/*!
  Expression function ASC().
  \param sIn Input - Input character string.
  \param dAscOut Output - Ascii code of first character in string.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbXBase::ASC( const xbString &sIn, xbDouble &dAscOut ){
  if( sIn == "" )
    return XB_PARSE_ERROR;

  dAscOut = sIn[1];
  return XB_NO_ERROR;
}
/*************************************************************************/
//! @brief Return number indicating starting position of string within a string.
/*!
  Expression function AT().
  \param s1 Input - Input string to search for.
  \param s2 Input - Input string to search.
  \param dPos Output - Position of string s1 within s2.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
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
//! @brief Return character weekday name for date.
/*!
  Expression function CDOW().
  \param dInDate Input - Input date.
  \param sOutDow Output - Character day of week.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbXBase::CDOW( xbDate &dInDate, xbString &sOutDow ){
  return dInDate.CharDayOf( sOutDow );
}

/*************************************************************************/
//! @brief Convert numeric expression to a character.
/*!
  Expression function CHR().
  \param dAsciCd Input - Numeric expression.
  \param sOut Output - Character result.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbXBase::CHR( xbDouble dAsciCd, xbString &sOut ){
  static char buf[2];
  buf[0] = (char) dAsciCd;
  buf[1] = 0x00;
  sOut = buf;
  return XB_NO_ERROR;
}

/*************************************************************************/
//! @brief Return character month  name for date.
/*!
  Expression function CMONTH().
  \param dInDate Input - Input date.
  \param sOutMonth Output - Character month.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbXBase::CMONTH( xbDate &dInDate, xbString &sOutMonth ){
  return dInDate.CharMonthOf( sOutMonth );
}
/*************************************************************************/
//! @brief Return date from character input date.
/*!
  Expression function CTOD().
  \param sInDate Input - Input date in MM/DD/YY format.
  \param dOutDate Output - Output date.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbXBase::CTOD( const xbString &sInDate, xbDate &dOutDate ){
  return dOutDate.CTOD( sInDate );
}
/*************************************************************************/
//! @brief Return system date.
/*!
  Expression function DATE().
  \param dOutDate Output - Output date.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbXBase::DATE( xbDate &dOutDate ){
  return dOutDate.Sysdate();
}
/*************************************************************************/
//! @brief Return the day of the month from a date.
/*!
  Expression function DAY().
  \param dInDate Input - Input date.
  \param dOutDay Output - Output day of month.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
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
//! @brief Return record deletion status for current record.
/*!
  Expression function DEL().
  \param dbf Input - Table to check record deletion status.
  \param iRecBufSw Input - Which buffer.  0 = Current record buffer, 1 = Original record buffer
  \param sOut Output - "*" if record is deleted, otherise space.
  \returns XB_NO_ERROR<br>XB_PARSE_ERROR.
  
*/
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
//! @brief Return record deletion status for current record.
/*!
  Expression function DELETED().
  \param dbf Input - Table to check record deletion status for.
  \param iRecBufSw Input - Which buffer.  0 = Current record buffer, 1 = Original record buffer
  \param bOut Output - xbTrue if record is deleted.<br>xbFalse if record is not deleted.
  \returns XB_NO_ERROR<br>XB_PARSE_ERROR.
*/
xbInt16 xbXBase::DELETED( xbDbf *dbf , xbBool &bOut, xbInt16 iRecBufSw ) {

  if( dbf ){
    bOut = dbf->RecordDeleted( iRecBufSw );
    return XB_NO_ERROR;
  } else {
    return XB_PARSE_ERROR;
  }
}

/*************************************************************************/
//! @brief Clipper DESCEND function.
/*!
  Expression function DESCEND().
  \param dtInDate Input - Input date.
  \param dtOutDate Output - Output date.
  \returns XB_NO_ERROR.
*/
xbInt16 xbXBase::DESCEND( const xbDate &dtInDate, xbDate &dtOutDate ){
  xbDate d( "29991231" );
  dtOutDate.JulToDate8( 2415021 + d.JulianDays() - dtInDate.JulianDays());
  return XB_NO_ERROR;
}

/*************************************************************************/
//! @brief Clipper DESCEND function.
/*!
  Expression function DESEND().
  \param dIn Input - Input number.
  \param dOut Output - Output number.
  \returns XB_NO_ERROR.
*/
xbInt16 xbXBase::DESCEND( xbDouble dIn, xbDouble &dOut ){
  dOut = dIn * -1;
  return XB_NO_ERROR;
}

/*************************************************************************/
//! @brief Clipper DESCEND function.
/*!
  Expression function DESEND().
  \param sIn Input - Input string.
  \param sOut Output - Output string.
  \returns XB_NO_ERROR.
*/
xbInt16 xbXBase::DESCEND( const xbString &sIn, xbString &sOut ){

  sOut = sIn;
  for( xbUInt32 l = 0; l < sIn.Len(); l++ )
    sOut.PutAt( l+1, (char) (255 - sOut[l+1]));
  return XB_NO_ERROR;
}

/*************************************************************************/
//! @brief Return number of day of week.
/*!
  Expression function DOW().
  \param dtInDate Input - Input date.
  \param dDowOut Output - Output day of week.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
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
//! @brief Return character date from input date.
/*!
  Expression function DTOC().
  \param dInDate Input - Input date.
  \param sOutFmtDate Output - Output date.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbXBase::DTOC( xbDate &dInDate, xbString &sOutFmtDate  ){
  return dInDate.FormatDate( "MM/DD/YY", sOutFmtDate );
}
/*************************************************************************/
//! @brief Return char CCYYMMDD date from input date.
/*!
  Expression function DTOS().
  \param dtInDate Input - Input date.
  \param sOutFmtDate Output - Output date.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbXBase::DTOS( xbDate &dtInDate, xbString &sOutFmtDate ){
  return dtInDate.FormatDate( "YYYYMMDD", sOutFmtDate );
}
/*************************************************************************/
//! @brief Return exponent value.
/*!
  Expression function EXP().
  This function returns e**x where e is approximately 2.71828 and x is dIn.

  \param dIn Input - exp value.
  \param dOut Output - value.
  \returns XB_NO_ERROR
*/
xbInt16 xbXBase::EXP( xbDouble dIn, xbDouble &dOut )
{
  dOut = exp( dIn );
  return XB_NO_ERROR;
}
/*************************************************************************/
//! @brief Immediate if.
/*!
  Expression function IIF().
  \param bResult Input - boolean expression.
  \param sTrueResult Input - value if boolean expression is true.
  \param sFalseResult Input - value if boolean expression is false.
  \param sResult Output - sTrueResult or sFalseResult depending on bResultvalue.
  \returns XB_NO_ERROR
*/
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
//! @brief Convert number to integer, truncate any decimals.
/*!
  Expression function INT().
  \param dIn Input - Input number.
  \param dOut Output - integer.
  \returns XB_NO_ERROR
*/

xbInt16 xbXBase::INT( xbDouble dIn, xbDouble &dOut )
{
  xbInt64 ll = (xbInt64) dIn;
  dOut = (xbDouble) ll;
  return XB_NO_ERROR;
}

/*************************************************************************/
//! @brief Check if string begins with alpha character.
/*!
  Expression function ISALPHA().
  \param sIn Input - Input string.
  \param bResult Output - xbTrue if string begins with alpha character.<br>xbFalse if string does not begin with alpha character.
  \returns XB_NO_ERROR
*/
xbInt16 xbXBase::ISALPHA( const xbString &sIn, xbBool &bResult ){
  if( isalpha(sIn[1]))
    bResult = 1;
  else
    bResult = 0;
  return XB_NO_ERROR;
}

/*************************************************************************/
//! @brief Check if string begins with lower case alpha character.
/*!
  Expression function ISLOWER().
  \param sIn Input - Input string.
  \param bResult Output - xbTrue if string begins with lower case alpha character.<br>
  xbFalse if string does not begin with lower case alpha character.
  \returns XB_NO_ERROR
*/

xbInt16 xbXBase::ISLOWER( const xbString &sIn, xbBool &bResult ){
  if( islower(sIn[1])) 
    bResult = 1;
  else
    bResult = 0;
  return XB_NO_ERROR;
}
/*************************************************************************/
//! @brief Check if string begins with upper case alpha character.
/*!
  Expression function ISUPPER().
  \param sIn Input - Input string.
  \param bResult Output - xbTrue if string begins with upper case alpha character.<br>
  xbFalse if string does not begin with upper case alpha character.
  \returns XB_NO_ERROR
*/

xbInt16 xbXBase::ISUPPER( const xbString &sIn, xbBool &bResult ){
  if( isupper(sIn[1]))
    bResult = 1;
  else
    bResult = 0;
  return XB_NO_ERROR;
}

/*************************************************************************/
//! @brief Return left characters from string.
/*!
  Expression function LEFT().
  \param sIn Input - string.
  \param ulCharCnt Input - number of characters to extract from string.
  \param sOut Output - resultant string.
  \returns XB_NO_ERROR
*/
xbInt16 xbXBase::LEFT( const xbString &sIn, xbUInt32 ulCharCnt, xbString &sOut ){
  sOut.Assign( sIn, 1, ulCharCnt );
  return XB_NO_ERROR;
}

/*************************************************************************/
//! @brief Return length of string.
/*!
  Expression function LEN().
  \param sIn Input - Input string.
  \param dOut Output - string length.
  \returns XB_NO_ERROR
*/

xbInt16 xbXBase::LEN( const xbString &sIn, xbDouble &dOut ){
  dOut = sIn.Len();
  return XB_NO_ERROR;
}

/*************************************************************************/
//! @brief Calculate logarithm.
/*!
  Expression function LOG().
  \param dIn Input - numeric expression.
  \param dOut Output - numeric log value.
  \returns XB_NO_ERROR
*/
xbInt16 xbXBase::LOG( xbDouble dIn, xbDouble &dOut ){
  dOut = log( dIn );
  return XB_NO_ERROR;
}
/*************************************************************************/
//! @brief Trim left side of string.
/*!
  Expression function LTRIM().
  \param sIn Input - string.
  \param sOut Output - string result.
  \returns XB_NO_ERROR
*/
xbInt16 xbXBase::LTRIM( const xbString &sIn, xbString &sOut ){
  sOut = sIn;
  sOut.Ltrim();
  return XB_NO_ERROR;
}
/*************************************************************************/
//! @brief Convert upper case to lower case.
/*!
  Expression function LOWER().
  \param sIn Input - string.
  \param sOut Output - string result.
  \returns XB_NO_ERROR
*/

xbInt16 xbXBase::LOWER( const xbString &sIn, xbString &sOut ){
  sOut = sIn;
  sOut.ToLowerCase();
  return XB_NO_ERROR;
}

/*************************************************************************/
//! @brief Return higher of two values.
/*!
  Expression function MAX().
  \param d1 Input - Numeric value 1.
  \param d2 Input - Numeric value 2.
  \param dOut Output - Higher of d1 or d2.
  \returns XB_NO_ERROR
*/
xbInt16 xbXBase::MAX( xbDouble d1, xbDouble d2, xbDouble &dOut )
{
  if( d1 > d2 ) 
    dOut = d1;
  else
    dOut = d2;
  return XB_NO_ERROR;
}

/*************************************************************************/
//! @brief Return lessor of two values.
/*!
  Expression function MIN().
  \param d1 Input - Numeric value 1.
  \param d2 Input - Numeric value 2.
  \param dOut Output - Lessor of d1 or d2.
  \returns XB_NO_ERROR
*/
xbInt16 xbXBase::MIN( xbDouble d1, xbDouble d2, xbDouble &dOut )
{
  if( d1 < d2 ) 
    dOut = d1;
  else
    dOut = d2;
  return XB_NO_ERROR;
}

/*************************************************************************/
//! @brief Return number of month for a given date.
/*!
  Expression function MONTH().
  \param dInDate Input date.
  \param dOutMonth - Month number.
  \returns XB_NO_ERROR.<br>XB_PARSE_ERROR.
*/
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
//! @brief Return number of records in a given table.
/*!
  Expression function RECCOUNT().
  \param dbf - Table.
  \param dOut - Number of records.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbXBase::RECCOUNT( xbDbf *dbf , xbDouble &dOut ) {

  xbUInt32 ulRecCnt;
  xbInt16 iRc = dbf->GetRecordCnt( ulRecCnt );
  dOut = (xbDouble) ulRecCnt;
  return iRc;

}

/*************************************************************************/
//! @brief Return current record number for a given table.
/*!
  Expression function RECNO().
  \param dbf  - Table.
  \param dOut - Record number.
  \returns XB_NO_ERROR.<br>XB_PARSE_ERROR.
*/

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
//! @brief Repeat character expression N times.
/*!
  Expression function REPLICATE().
  \param sIn Inout - String to replicate.
  \param ulRepCnt Input - Number of times to repeat.
  \param sOut Output - String result.
  \returns XB_NO_ERROR.<br>XB_PARSE_ERROR.
*/
xbInt16 xbXBase::REPLICATE( const xbString &sIn, xbUInt32 ulRepCnt, xbString &sOut ){
  sOut = "";
  for( xbUInt32 i = 0; i < ulRepCnt; i++ )
    sOut += sIn;
  return XB_NO_ERROR;
}

/*************************************************************************/
//! @brief Return right characters from string.
/*!
  Expression function RIGHT().
  \param sIn Input - string.
  \param ulCharCnt Input - number of characters to extract from string.
  \param sOut Output - resultant string.
  \returns XB_NO_ERROR
*/

xbInt16 xbXBase::RIGHT( const xbString &sIn, xbUInt32 ulCharCnt, xbString &sOut ){
  if( sIn.Len() < ulCharCnt )
    sOut = sIn;
  else
    sOut.Assign( sIn, sIn.Len() - ulCharCnt + 1, ulCharCnt );
  return XB_NO_ERROR;
}

/*************************************************************************/
//! @brief Trim right side of string.
/*!
  Expression function RTRIM().
  \param sIn Input - string.
  \param sOut Output - string result.
  \returns XB_NO_ERROR
*/

xbInt16 xbXBase::RTRIM( const xbString &sIn, xbString &sOut ){
  sOut = sIn;
  sOut.Rtrim();
  return XB_NO_ERROR;
}

/*************************************************************************/
//! @brief Generate a string of N spaces.
/*!
  Expression function SPACE().
  \param lCnt Input - Number of spaces.
  \param sOut Output - Output string consisting of specified number of spaces.
  \returns XB_NO_ERROR.
*/
xbInt16 xbXBase::SPACE( xbInt32 lCnt, xbString &sOut ){
  sOut = "";
  for( xbInt32 i = 0; i < lCnt; i++ )
    sOut += ' ';
  return XB_NO_ERROR;
}

/*************************************************************************/
//! @brief Calculate a square root.
/*!
  Expression function SQRT().
  \param dBase Input - Base number.
  \param dSqrRoot Output - Square root.
  \returns XB_NO_ERROR.
*/

xbInt16 xbXBase::SQRT( xbDouble dBase, xbDouble &dSqrRoot )
{
  dSqrRoot = sqrt( dBase );
  return XB_NO_ERROR;
}

/*************************************************************************/
//! @brief Converts a valid 8 byte (CCYYMMDD) input date into a date class.
/*!
  Expression function STOD().
  \param sInDate Input - Input date.
  \param dtOutDate Output - Output date.
  \returns XB_NO_ERROR.<br>XB_INVALID_DATE.
*/

xbInt16 xbXBase::STOD( const xbString &sInDate, xbDate &dtOutDate ){

  if( dtOutDate.DateIsValid( sInDate )){
    dtOutDate = sInDate;
    return XB_NO_ERROR;
  } else {
    return XB_INVALID_DATE;
  }
}

/*************************************************************************/
//! @brief Convert number to a character string.
/*!
  Expression function STR().
  \param dIn Input - Number.
  \param sOut Output - String.
  \returns XB_NO_ERROR.
*/

xbInt16 xbXBase::STR( xbDouble dIn, xbString &sOut) {
  xbString sPadChar = " ";
  return STR( dIn, 10, 0, sPadChar, sOut );
}

/*************************************************************************/
//! @brief Convert number to a character string.
/*!
  Expression function STR().
  \param dIn Input - Number.
  \param ulLen Input - Length.
  \param sOut Output - String.
  \returns XB_NO_ERROR.
*/
xbInt16 xbXBase::STR( xbDouble dIn, xbUInt32 ulLen, xbString &sOut) {
  xbString sPadChar = " ";
  return STR( dIn, ulLen, 0, sPadChar, sOut );
}

/*************************************************************************/
//! @brief Convert number to a character string.
/*!
  Expression function STR().
  \param dIn Input - Number.
  \param ulLen Input - Length.
  \param ulDec Input - Number of decimals.
  \param sOut Output - String.
  \returns XB_NO_ERROR.
*/

xbInt16 xbXBase::STR( xbDouble dIn, xbUInt32 ulLen, xbUInt32 ulDec, xbString &sOut) {
  xbString sPadChar = " ";
  return STR( dIn, ulLen, ulDec, sPadChar, sOut );
}

/*************************************************************************/
//! @brief Convert number to a character string.
/*!
  Expression function STR().
  \param dIn Input - Number.
  \param ulLen Input - Length.
  \param ulDec Input - Number of decimals.
  \param sPadChar Input - Left pad character, typically zero or space.
  \param sOut Output - String.
  \returns XB_NO_ERROR.
*/

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
    // but it does behave like the original dbase
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
//! @brief Convert number to a character string with leading zeroes.
/*!
  Expression function STRZERO().
  \param dIn Input - Number.
  \param ulLen Input - Length.
  \param ulDec Input - Number of decimals.
  \param sOut Output - String.
  \returns XB_NO_ERROR.
*/
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
//! @brief Extract a portion of a string from another string.
/*!
  Expression function SUBSTR().
  \param sIn Input - Source string.
  \param ulStartPos Input - Starting position for string extraction.
  \param ulLen Input - Number of characters to extract.
  \param sOut Output - String.
  \returns XB_NO_ERROR.
*/
xbInt16 xbXBase::SUBSTR( const xbString &sIn, xbUInt32 ulStartPos, xbUInt32 ulLen, xbString &sOut ){
  sOut = sIn;
  sOut.Mid( ulStartPos, ulLen );
  return XB_NO_ERROR;
}

/*************************************************************************/
//! @brief Trim left and right sides of string.
/*!
  Expression function TRIM().
  \param sIn Input - string.
  \param sOut Output string result.
  \returns XB_NO_ERROR
*/
xbInt16 xbXBase::TRIM( const xbString &sIn, xbString &sOut ){
  return RTRIM( sIn, sOut );
}

/*************************************************************************/
//! @brief Convert lower case to upper case.
/*!
  Expression function UPPER().
  \param sIn Input - string.
  \param sOut Output - string result.
  \returns XB_NO_ERROR
*/
xbInt16 xbXBase::UPPER( const xbString &sIn, xbString &sOut ){
  sOut = sIn;
  sOut.ToUpperCase();
  return XB_NO_ERROR;
}

/*************************************************************************/
//! @brief Convert numeric characters to number.
/*!
  Expression function VAL().
  \param sIn Input - string.
  \param dOut Output - numeric result.
  \returns XB_NO_ERROR
*/

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
//! @brief Return year for a given date.
/*!
  Expression function YEAR().
  \param dInDate Input date.
  \param dOutYear - Year.
  \returns XB_NO_ERROR.<br>XB_PARSE_ERROR.
*/

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
