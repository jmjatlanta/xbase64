/* xbdate.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023,2024 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

#include "xbase.h"
#include <time.h>

namespace xb{

int xbDate::iDaysInMonths[2][13];
int xbDate::iAggregatedDaysInMonths[2][13];

/*************************************************************************/
xbDate::xbDate() {
  sDate8.Set( "" );
}
/*************************************************************************/
xbDate::xbDate( const char * sDate8In ) {
  if( DateIsValid( sDate8In ))
    sDate8.Set( sDate8In );
  else
    sDate8.Set( "" );
}
/*************************************************************************/
xbDate::xbDate( const xbDate &dtDateIn ) {
  sDate8.Set( dtDateIn.Str());
}
/*************************************************************************/
xbDate::xbDate( const xbString & sDate8In ) {
  if( DateIsValid( sDate8In ))
    sDate8.Set( sDate8In );
  else
    sDate8.Set( "" );
}
/*************************************************************************/
xbDate::xbDate( xbInt32 lDateOpt ) {
  if( lDateOpt == 0 )
    Sysdate();
  else if( lDateOpt == -1 )
    sDate8.Set( "" );
  else if( lDateOpt == -2 ){
    SetDateTables();
    sDate8.Set( "" );
  }
  else
    JulToDate8( lDateOpt );
}
/*************************************************************************/
xbDate::xbDate( xbInt16 iMonth, xbInt16 iDay, xbInt16 iYear ) {
  Set( iMonth, iDay, iYear );
}
/*************************************************************************/
/*
xbDate::xbDate( xbUInt16 iOption ) {
  if( iOption > 0 ){
    Sysdate();
    if( iOption > 1 ){    // set the date variables and tables, performed
      SetDateTables();   // during library initialization
    }
  }
}
*/
/*************************************************************************/
xbDate::~xbDate(){}
/*************************************************************************/
xbInt16 xbDate::CalcRollingCenturyForYear( xbInt16 iCalcYear ) const {
  xbDate d;
  d.Sysdate();
  xbInt16 iThisYear    = d.YearOf();
  xbInt16 iThisCentury = d.CenturyOf();
  iThisYear -= (iThisCentury * 100);
  if( iThisYear < 80 && iCalcYear < (iThisYear+20) )
    return iThisCentury;
  else if( iThisYear >= 80 &&
           iCalcYear <  iThisYear &&
           iCalcYear >= (iThisYear-80))
    return iThisCentury;
  else
    return iThisCentury - 1;
}
/*************************************************************************/
xbInt16 xbDate::CenturyOf() const {
  if( !IsNull() ){
    char Century[3];
    Century[0] = sDate8[1];
    Century[1] = sDate8[2];
    Century[2] = 0x00;
    return( atoi( Century ));
  } else {
    return 0;
  }
}
/*************************************************************************/
xbInt16 xbDate::CharDayOf( xbString &sOutCharDay ) {
  if( !IsNull()){
    struct tm tblock;
    char buf[25];
    tblock.tm_year  = YearOf() - 1900;
    tblock.tm_mon   = MonthOf() - 1;
    tblock.tm_mday  = DayOf( XB_FMT_MONTH );
    tblock.tm_hour  = 0;
    tblock.tm_min   = 0;
    tblock.tm_sec   = 1;
    tblock.tm_isdst = -1;
    if( mktime( &tblock ) == -1 ){
      sOutCharDay = "" ;
      return XB_INVALID_DATE;
    } else {
      strftime( buf, 25, "%A", &tblock );
      sOutCharDay = buf;
    }
  }
  return XB_NO_ERROR;;
}
/*************************************************************************/
xbInt16 xbDate::CharMonthOf( xbString &sOutCharMonth ) {

  if( !IsNull()){
    struct tm tblock;
    char buf[25];
    tblock.tm_year  = YearOf() - 1900;
    tblock.tm_mon   = MonthOf() - 1;
    tblock.tm_mday  = DayOf( XB_FMT_MONTH );
    tblock.tm_hour  = 0;
    tblock.tm_min   = 0;
    tblock.tm_sec   = 1;
    tblock.tm_isdst = -1;
    if( mktime( &tblock ) == -1 ){
      sOutCharMonth = "";
      return XB_INVALID_DATE;
    } else {
      strftime( buf, 25, "%B", &tblock );
      sOutCharMonth = buf;
    }
  }
  return XB_NO_ERROR;
}
/*************************************************************************/
xbInt16 xbDate::CTOD( const xbString &sCtodInDate ){

  if( sCtodInDate[1] != ' ' && ( sCtodInDate[3] == '\\' || sCtodInDate[3] == '/') ){
    char yy[3];
    yy[0] = sCtodInDate[7];
    yy[1] = sCtodInDate[8];
    yy[2] = 0x00;
    sDate8.Sprintf( "%02d%c%c%c%c%c%c", CalcRollingCenturyForYear( atoi( yy )),
         sCtodInDate[7], sCtodInDate[8], sCtodInDate[1], sCtodInDate[2], sCtodInDate[4], sCtodInDate[5] );
    return XB_NO_ERROR;
  }
  else{
    return XB_INVALID_DATE;
  }
}
/*************************************************************************/
xbBool  xbDate::DateIsValid( const xbString &sDateIn ) const {

   xbInt16 iYear, iMonth, iDay;
   char sYear[5];
   char sMonth[3];
   char sDay[3];
   if( sDateIn.Len() != 8 )
     return xbFalse;

   if(!isdigit( sDateIn[1] ) || !isdigit( sDateIn[2] ) || !isdigit( sDateIn[3] ) ||
      !isdigit( sDateIn[4] ) || !isdigit( sDateIn[5] ) || !isdigit( sDateIn[6] ) ||
      !isdigit( sDateIn[7] ) || !isdigit( sDateIn[8] ) )
        return xbFalse;

   sDay[0]   = sDateIn[7];
   sDay[1]   = sDateIn[8];
   sDay[2]   = 0x00;
   iDay      = atoi( sDay );
   sMonth[0] = sDateIn[5];
   sMonth[1] = sDateIn[6];
   sMonth[2] = 0x00;
   iMonth    = atoi( sMonth );
   sYear[0]  = sDateIn[1];
   sYear[1]  = sDateIn[2];
   sYear[2]  = sDateIn[3];
   sYear[3]  = sDateIn[4];
   sYear[4]  = 0x00;
   iYear     = atoi( sYear );

   // valid years are 0001 thru 9999
   if( iYear < 1 || iYear > 9999 || iMonth < 1 || iMonth > 12 || iDay < 1 || iDay > 31 )
      return xbFalse;

   // April, June, September and November have 30 days 
   if(( iMonth==4 || iMonth==6 || iMonth==9 || iMonth==11 )&& iDay > 30 )
      return xbFalse;

   // check for February with leap year 
   if( iMonth == 2 ){
     if(( iYear % 4 == 0 && iYear % 100 != 0 ) || iYear % 400 == 0 ){
       if( iDay > 29 ){
         return xbFalse;
       }
     } else if( iDay > 28 ){
         return xbFalse;
     }
   }
   return xbTrue;
}
/*************************************************************************/
xbInt16 xbDate::DayOf( xbInt16 iFormat ) const {

  if( !IsNull()){
    xbInt16 iOutDay = 0;
    char sDay[3];
    xbInt16  iDay, iMonth, iYear, iDay2;

    // check for valid format switch 
    if( iFormat!=XB_FMT_WEEK && iFormat!=XB_FMT_MONTH && iFormat!=XB_FMT_YEAR )
      return XB_INVALID_OPTION;

    if( iFormat == XB_FMT_WEEK ){
      //DayOf( XB_FMT_MONTH, iDay );
      iDay   = DayOf( XB_FMT_MONTH );
      iMonth = MonthOf();
      iYear  = YearOf();

      // The following formula uses Zeller's Congruence to determine the day of the week 
      if( iMonth > 2 )           // init to February 
        iMonth -= 2;
      else {
        iMonth += 10;
        iYear--;
      }
      iDay2 = ((13 * iMonth - 1) / 5) + iDay + ( iYear % 100 ) +
              (( iYear % 100 ) / 4) + ((iYear /100 ) / 4 ) - 2 *
               ( iYear / 100 ) + 77 ;

      iOutDay = iDay2 - 7 * ( iDay2 / 7 );
      iOutDay == 6 ? iOutDay = 0 : iOutDay++;
    }
    else if( iFormat == XB_FMT_MONTH ){
      sDay[0] = sDate8[7];
      sDay[1] = sDate8[8];
      sDay[2] = 0x00;
      iOutDay = atoi( sDay );
    } else {
      iOutDay = iAggregatedDaysInMonths[IsLeapYear()][MonthOf()-1] + DayOf( XB_FMT_MONTH );
    }
    return iOutDay;
  } else {
    return 0;
  }
}
/*************************************************************************/
#ifdef XB_DEBUG_SUPPORT
void xbDate::Dump( const char *sTitle ){
  fprintf( stdout, "%s\n  sDate = [%s]\n", sTitle, sDate8.Str() );
}
/*************************************************************************/
void xbDate::DumpDateTables(){
  fprintf( stdout, "Date Tables\n" );
  fprintf( stdout, "Month   *-Aggragated Days-*  *--Days In Month--*\n" );
  fprintf( stdout, "        *-NonLeap    Leap-*  *--NonLeap  Leap--*\n" );
  for( int i = 1; i < 13; i++ )
    fprintf( stdout, "  %2d        %3d       %3d         %3d     %3d\n", i,
    iAggregatedDaysInMonths[0][i],iAggregatedDaysInMonths[1][i],
    iDaysInMonths[0][i], iDaysInMonths[1][i]);
}
#endif
/*************************************************************************/
xbInt16 xbDate::FormatDate( const xbString &sFmtIn, xbString &sOutFmtDate ){
  xbUInt32 FmtCtr;
  char type;
  xbUInt32  iTypeCtr;
  xbString ws;
  xbString sWrkFmt;
  sOutFmtDate = "";

  if( IsNull())
    return XB_INVALID_DATE;

  /* use format for this specific string if available, else use default format */
  if( strlen( sFmtIn ) > 0 )
    sWrkFmt = sFmtIn;
  else
    sWrkFmt = sDefaultDateFormat;
    //sWrkFmt = GetDefaultDateFormat();

  if( strstr( sWrkFmt.Str(), "YYDDD" )){
    sOutFmtDate.Sprintf( "%c%c%03d", sDate8[3], sDate8[4], DayOf( XB_FMT_YEAR ));
  } else {
    FmtCtr = 1;
    while( FmtCtr <= sWrkFmt.Len() ){
      if( sWrkFmt[FmtCtr] != 'D' && sWrkFmt[FmtCtr] != 'M' && sWrkFmt[FmtCtr] != 'Y' ){
        sOutFmtDate += sWrkFmt[FmtCtr];
        FmtCtr++;
        iTypeCtr = 0;
      } else { 
        type = sWrkFmt[FmtCtr];
        iTypeCtr = 0;
        while( sWrkFmt[FmtCtr] == type ) {
          iTypeCtr++;
          FmtCtr++;
      }
      switch( type ){
       case 'D':

        if( iTypeCtr == 1 ){
          sOutFmtDate += ws.Sprintf( "%d", DayOf( XB_FMT_MONTH ));
        }
        else if( iTypeCtr == 2 ){
          sOutFmtDate += ws.Sprintf( "%c%c", sDate8[7], sDate8[8] );
        } else {
          xbString sCDO;
          CharDayOf( sCDO );
          ws.Assign( sCDO, 1, iTypeCtr );
          sOutFmtDate += ws.Str();
        }
        break;

       case 'M':
        if( iTypeCtr == 1 ){
          sOutFmtDate += ws.Sprintf( "%d", MonthOf());
        }
        else if( iTypeCtr == 2 ){
          sOutFmtDate += ws.Sprintf( "%c%c", sDate8[5], sDate8[6] );
        } else {
          xbString sCMO;
          CharMonthOf( sCMO );
          ws.Assign( sCMO, 1, iTypeCtr );
          sOutFmtDate += ws.Str();
        }
        break;

       case 'Y':
        if( iTypeCtr == 2 ){
          sOutFmtDate += ws.Sprintf( "%c%c", sDate8[3], sDate8[4] );
        }
        else if( iTypeCtr == 4 ){
          sOutFmtDate += ws.Sprintf( "%c%c%c%c", sDate8[1], sDate8[2], sDate8[3], sDate8[4] );
        }
        break;
       default:
        break;
       }
      }
    }
  }
  return XB_NO_ERROR;
}
/*************************************************************************/
xbString & xbDate::GetDefaultDateFormat() const {
  return sDefaultDateFormat;
}
/*************************************************************************/
xbBool xbDate::IsLeapYear() const {
   if( IsNull() )
     return xbFalse;
   xbInt16 iYear = YearOf();
   if(( iYear % 4 == 0 && iYear % 100 != 0 ) || iYear % 400 == 0 )
     return xbTrue;
   else
     return xbFalse;
}
/*************************************************************************/
xbBool xbDate::IsLeapYear( xbInt16 iYear ) const {
   if(( iYear % 4 == 0 && iYear % 100 != 0 ) || iYear % 400 == 0 )
     return xbTrue;
   else
     return xbFalse;
}
/*************************************************************************/
xbBool xbDate::IsNull() const {
   if( sDate8.Len() < 8 )
     return xbTrue;
   else
     return xbFalse;
}
/*************************************************************************/
xbInt32 xbDate::JulianDays() const{
   if( !IsNull()){
     xbInt32 ly = YearOf() - 1;
     xbInt32 lDays = ly * 365L + ly / 4L - ly / 100L + ly / 400L;
     lDays += DayOf( XB_FMT_YEAR );
     return lDays + JUL_OFFSET;
   } else {
     return 0;
   }
}
/*************************************************************************/
void xbDate::JulToDate8( xbInt32 lJulDays ){
  lJulDays -= JUL_OFFSET;
  // calculate the year
  xbInt16 iYear = (xbInt16)(lJulDays / 365.24 );
  lJulDays -= (iYear * 365L) + (iYear / 4L) - (iYear / 100L) + (iYear / 400L);
  iYear++;
  while( lJulDays <= 0 ){
    iYear--;
    lJulDays += (365L + IsLeapYear( iYear ));
  }
  // this for loop calculates the month by comparing the number of days remaining to one of the tables
  xbInt16 iIsLeap = IsLeapYear(iYear);
  xbInt16 iMonth = 1;
  while( ((xbInt16) lJulDays > iAggregatedDaysInMonths[iIsLeap][iMonth]) && (iMonth < 12) )
    iMonth++;
  lJulDays -= iAggregatedDaysInMonths[iIsLeap][iMonth-1];
  sDate8.Sprintf( "%04d%02d%02ld", iYear, iMonth, lJulDays );
//  return XB_NO_ERROR;
}
/*************************************************************************/
xbInt16 xbDate::LastDayOfMonth(){
  if( !IsNull()){
    sDate8.Sprintf( "%4.4d%2.2d%2.2d", YearOf(), MonthOf(), iDaysInMonths[IsLeapYear()][MonthOf()]);
    return XB_NO_ERROR;
  } else {
    return XB_INVALID_DATE;
  }
}
/*************************************************************************/
xbInt16 xbDate::MonthOf() const {
   if( !IsNull()){
     xbInt16 iOutMonth;
     char month[3];
     month[0] = sDate8[5];
     month[1] = sDate8[6];
     month[2] = 0x00;
     iOutMonth = atoi( month );
     return iOutMonth;
   } else {
     return 0;
   }
}
/*************************************************************************/
xbBool xbDate::operator!=( const xbDate &dt ) const{
  if( JulianDays() != dt.JulianDays() )
    return xbTrue;
  else
    return xbFalse;
}
/*************************************************************************/
const char *xbDate::operator+( xbInt32 lCount ){
  if( !IsNull() )
    JulToDate8( JulianDays() + lCount );
  return sDate8.Str();
}
/*************************************************************************/
void xbDate::operator++(xbInt32){
  if( !IsNull() )
    *this+=1;
}
/*************************************************************************/
void xbDate::operator+=( xbInt32 lDays ){
  if( !IsNull() )
    JulToDate8( JulianDays() + lDays );
}
/*************************************************************************/
xbInt32 xbDate::operator-( const xbDate &dt ) const{
  if( !IsNull() && !dt.IsNull() )
    return JulianDays() - dt.JulianDays();
  else
    return 0;
}
/*************************************************************************/
const char *xbDate::operator-( xbInt32 lCount ){
  if( !IsNull() )
    JulToDate8( JulianDays() - lCount );
  return sDate8;
}
/*************************************************************************/
void xbDate::operator--(xbInt32){
  if( !IsNull())
    *this-=1;
}
/*************************************************************************/
void xbDate::operator-=( xbInt32 lDays ){
  if( !IsNull() )
    JulToDate8( JulianDays() - lDays );
}
/*************************************************************************/
xbBool xbDate::operator<( const xbDate &dt ) const {
  if( JulianDays() < dt.JulianDays() )
    return xbTrue;
  else
    return xbFalse;
}
/*************************************************************************/
xbBool xbDate::operator<=( const xbDate &dt ) const {
  if( JulianDays() <= dt.JulianDays() )
    return xbTrue;
  else
    return xbFalse;
}
/*************************************************************************/
void xbDate::operator=( const xbDate &dt ){
  sDate8.Set( dt.Str());
}
/*************************************************************************/
xbBool xbDate::operator==( const xbDate &dt ) const{
  if( JulianDays() == dt.JulianDays() )
    return xbTrue;
  else
    return xbFalse;
}
/*************************************************************************/
xbBool xbDate::operator>( const xbDate &dt ) const {
  if( JulianDays() > dt.JulianDays() )
    return xbTrue;
  else 
    return xbFalse;
}
/*************************************************************************/
xbBool xbDate::operator>=( const xbDate &dt ) const {
  if( JulianDays() >= dt.JulianDays() )
    return xbTrue;
  else
    return xbFalse;
}
/*************************************************************************/
xbInt16 xbDate::Set( xbInt16 iMonth, xbInt16 iDay, xbInt16 iYear ){
  xbString s;
  s.Sprintf( "%04d%02d%02d", iYear, iMonth, iDay );
  if( DateIsValid( s )){
    sDate8 = s;
    return XB_NO_ERROR;
  } else {
    return XB_INVALID_DATE;
  }
}
/*************************************************************************/
xbInt16 xbDate::Set( const xbString & sDateIn ){

  if( DateIsValid( sDateIn )){
    sDate8 = sDateIn;
  } else {
    sDate8 = "";   // set to null date if invalid date
    return XB_INVALID_DATE;
  }
  return XB_NO_ERROR;
}
/*************************************************************************/
void xbDate::SetDefaultDateFormat( const xbString &sDefaultDateFormat ) {
  this->sDefaultDateFormat = sDefaultDateFormat;
}
/*************************************************************************/
//! @brief This routine sets up static data tables on startup.
/*!
  \returns void
*/
void xbDate::SetDateTables() {
 if( iAggregatedDaysInMonths[1][12] != 366 ){    /* first time called ? */

  iAggregatedDaysInMonths[0][0]  = 0;
  iAggregatedDaysInMonths[0][1]  = 31;
  iAggregatedDaysInMonths[0][2]  = 59;
  iAggregatedDaysInMonths[0][3]  = 90;
  iAggregatedDaysInMonths[0][4]  = 120;
  iAggregatedDaysInMonths[0][5]  = 151;
  iAggregatedDaysInMonths[0][6]  = 181;
  iAggregatedDaysInMonths[0][7]  = 212;
  iAggregatedDaysInMonths[0][8]  = 243;
  iAggregatedDaysInMonths[0][9]  = 273;
  iAggregatedDaysInMonths[0][10] = 304;
  iAggregatedDaysInMonths[0][11] = 334;
  iAggregatedDaysInMonths[0][12] = 365;
  iAggregatedDaysInMonths[1][0]  = 0; 
  iAggregatedDaysInMonths[1][1]  = 31;
  iAggregatedDaysInMonths[1][2]  = 60;
  iAggregatedDaysInMonths[1][3]  = 91;
  iAggregatedDaysInMonths[1][4]  = 121;
  iAggregatedDaysInMonths[1][5]  = 152;
  iAggregatedDaysInMonths[1][6]  = 182;
  iAggregatedDaysInMonths[1][7]  = 213;
  iAggregatedDaysInMonths[1][8]  = 244;
  iAggregatedDaysInMonths[1][9]  = 274;
  iAggregatedDaysInMonths[1][10] = 305;
  iAggregatedDaysInMonths[1][11] = 335;
  iAggregatedDaysInMonths[1][12] = 366;

  iDaysInMonths[0][0]  = 0; 
  iDaysInMonths[0][1]  = 31;
  iDaysInMonths[0][2]  = 28;
  iDaysInMonths[0][3]  = 31;
  iDaysInMonths[0][4]  = 30;
  iDaysInMonths[0][5]  = 31;
  iDaysInMonths[0][6]  = 30;
  iDaysInMonths[0][7]  = 31;
  iDaysInMonths[0][8]  = 31;
  iDaysInMonths[0][9]  = 30;
  iDaysInMonths[0][10] = 31;
  iDaysInMonths[0][11] = 30;
  iDaysInMonths[0][12] = 31;
  iDaysInMonths[1][0]  = 0; 
  iDaysInMonths[1][1]  = 31;
  iDaysInMonths[1][2]  = 29;
  iDaysInMonths[1][3]  = 31;
  iDaysInMonths[1][4]  = 30;
  iDaysInMonths[1][5]  = 31;
  iDaysInMonths[1][6]  = 30;
  iDaysInMonths[1][7]  = 31;
  iDaysInMonths[1][8]  = 31;
  iDaysInMonths[1][9]  = 30;
  iDaysInMonths[1][10] = 31;
  iDaysInMonths[1][11] = 30;
  iDaysInMonths[1][12] = 31;
 }
}
/*************************************************************************/
const char * xbDate::Str() const{
   return sDate8.Str();
};
/*************************************************************************/
void xbDate::Sysdate(){
  #if defined(HAVE__LOCALTIME64_S_F)
    __time64_t timer;
    _time64( &timer );
    struct tm tblock;
    _localtime64_s( &tblock, &timer );
    tblock.tm_year += 1900;
    tblock.tm_mon++;
    sDate8.Sprintf( "%4d%02d%02d", tblock.tm_year, tblock.tm_mon, tblock.tm_mday );
  #else
    time_t timer;
    timer = time( &timer );
    struct tm *tblock;
    tblock = localtime( &timer );
    tblock->tm_year += 1900;
    tblock->tm_mon++;
    sDate8.Sprintf( "%4d%02d%02d",tblock->tm_year,tblock->tm_mon,tblock->tm_mday );
  #endif
}
/*************************************************************************/
xbInt16  xbDate::YearOf() const {
  if( !IsNull()){
    char year[5];
    year[0] = sDate8[1];
    year[1] = sDate8[2];
    year[2] = sDate8[3];
    year[3] = sDate8[4];
    year[4] = 0x00;
    xbInt16 iOutYear = atoi( year );
    return iOutYear;
  } else {
    return 0;
  }
};
}   /* namespace */