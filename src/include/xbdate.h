/*  xbdate.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/


#ifndef __XB_XBDATE_H__
#define __XB_XBDATE_H__

#ifdef  CMAKE_COMPILER_IS_GNUCC
#pragma interface
#endif


namespace xb{

#define XB_FMT_WEEK   1
#define XB_FMT_MONTH  2
#define XB_FMT_YEAR   3

// next value is added to the date calculation to match dbase calculation
// first valid dbase date is 01/01/0001
#define JUL_OFFSET  1721425L


//! @brief xbDate class.
/*!
This xbDate class handles two type of date values:<br>

1) Gregorian dates with a format of CCYYMMDD.  This is how dates are stored in dbf files.<br>
2) Julian dates calculated as the number of days since 1901-01-01 + 1721425.<br>

Leap Years: The routines in the class support both leap years (one every four 
years) and leap centuries (one every four hundred years.)

A leap year is a year having 366 days, which can be evenly
divisible by 4 and not by 100.<br>
Leap centuries are years which are evenly divisible by 400.<br>

From a programming perspective, Julian dates are useful for date 
arithmetic, determining the difference between two dates or calculating
a future or past date.<br>

To determine the difference between two dates,  convert both dates to a 
Julian date and subtract one from the other.<br>

To calculate a future or past date, convert the base date to a Julian date,
add (or subtract) the number of days necessary to (from) it and convert the
julian date back to a Gregorian date.
*/

class XBDLLEXPORT xbDate : public xbSsv {
  public:

  xbDate();
  xbDate( const char * Date8 );
  xbDate( const xbString &Date8 );
  xbDate( xbInt32 lJulDate );
  xbDate( xbUInt16 iInit );           // Constructor used to set the static variables, also defaults to sysdate
  xbDate( const xbDate &dtDate );
  ~xbDate();

  void operator=( const xbDate &d );
  void operator+=( xbInt32 i );
  void operator-=( xbInt32 i );
  void operator++( xbInt32 i );
  void operator--( xbInt32 i );

  xbInt32 operator-( const xbDate & ) const;
  const char * operator-( xbInt32 i );
  const char * operator+( xbInt32 i );

  xbBool operator==( const xbDate & ) const;
  xbBool operator!=( const xbDate & ) const;
  xbBool operator< ( const xbDate & ) const;
  xbBool operator> ( const xbDate & ) const;
  xbBool operator<=( const xbDate & ) const;
  xbBool operator>=( const xbDate & ) const;

  xbInt16     CalcRollingCenturyForYear( xbInt16 year ) const;
  xbInt16     CenturyOf() const;
  xbInt16     CharDayOf( xbString &sOutCharDay );
  xbInt16     CharMonthOf( xbString &sOutCharMonth );
  xbBool      DateIsValid ( const xbString &sDate8 ) const;
  xbInt16     DayOf( xbInt16 iFormat = XB_FMT_MONTH ) const;
  xbInt16     CTOD( const xbString &sCtodInDate );
  xbInt16     FormatDate( const xbString &sFmtIn, xbString &sFmtOut );
  const char  *Str() const;
  xbBool      IsLeapYear( xbInt16 iYear ) const;
  xbBool      IsLeapYear() const;
  xbBool      IsNull() const;
  xbInt32     JulianDays() const;
  xbInt16     JulToDate8( xbInt32 lJulDate );
  xbInt16     LastDayOfMonth();
  xbInt16     MonthOf() const;
  xbInt16     Set( const xbString &Date8 );
  xbInt16     Sysdate();
  xbInt16     YearOf() const;

#ifdef XB_DEBUG_SUPPORT
  void        Dump( const char * title );
  void        DumpDateTables();
#endif

 private:
  void SetDateTables();
  xbString sDate8;              // CCYYMMDD date format  ie; 20140718
                                // Null date is identified by sDate.Len() < 8

  static int iAggregatedDaysInMonths[2][13];
  static int iDaysInMonths[2][13];
};

}         /* namespace */
#endif    /*__XB_XBDATE_H__ */

