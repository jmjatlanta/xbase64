/*  xbdate.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2024 Gary A Kunkel

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

// next value is added to the date calculation to match dBASE calculation
// first valid dBASE date is 01/01/0001
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

class XBDLLEXPORT xbDate : public xbCore {
 public:

  ///@{
   /**
    @brief Constructor, set to null date
   */
   xbDate();

   /**
    @brief Constructor, set to input date
    @param sDate8 Null terminated char string in CCYYMMDD format.
   */
   xbDate( const char *sDate8 );

   /**
    @brief Constructor, set to input date
    @param dtDate Date object
   */
   xbDate( const xbDate &dtDate );

   /**
    @brief Constructor, set to input date
    @param sDate8 xbString in CCYYMMDD format.
   */
   xbDate( const xbString &sDate8 );

   /**
    @brief Constructor, set to input date
    @param lDateOpt -1 Null Date<br>
                     0 Sysdate<br>
                     Julian date
   */
   xbDate( xbInt32 lDateOpt );

   /**
    @brief Constructor, Set to input month, day year
    @param iMonth Input month<br>
    @param iDay Input Day<br>
    @param iYear Input year
   */
   xbDate( xbInt16 iMonth, xbInt16 iDay, xbInt16 iYear );

  ///@}



  //! @brief Destructor.
  ~xbDate();

 /*! @brief Calculate century for a given year. 
    Calculate a century for a given year using an 80/20 rolling date window. The routine
    compares the input year and looks 80 years into the future or 20 years into the past
    to calculate the century.

    @param iYear Two digit year to calculate a century on.
    @returns Century calculated for the two digit year.
  */
  xbInt16 CalcRollingCenturyForYear( xbInt16 iYear ) const;

  /*! @brief Get century for date object.
    @returns The century of the date object or 0 if null date.
  */
  xbInt16 CenturyOf() const;

  /*!
     @brief Get the day of the week for date object.
     @param sOutCharDay - Output character day of week (Sun,Mon,Tue,Wed,Thu,Fri,Sat).
     @returns XB_INVALID_DATE<br>XB_NO_ERROR
  */
  xbInt16 CharDayOf( xbString &sOutCharDay );

  /*! @brief Get the month from date object.
    @param sOutCharMonth - Output character month.
    @returns XB_INVALID_DATE<br>XB_NO_ERROR
  */
  xbInt16 CharMonthOf( xbString &sOutCharMonth );

  /*! @brief Format MM/DD/YY date 
    This routine takes an MM/DD/YY format date as input and populates a
    date class with the appropriate YYYYMMDD data.

    @param sCtodInDate - MM/DD/YY formatted date as input.
    @returns XB_INVALID_OPTION<br>XB_NO_ERROR
  */
  xbInt16     CTOD( const xbString &sCtodInDate );


  /*! @brief Check an input date in CCYYMMDD format for valid data. 
    @param sDate8In - Date to check for valid formaat of CCYYMMDD.
    @returns xbTrue - Valid date.<br>xbFalse - Not a valid date.
  */
  xbBool      DateIsValid ( const xbString &sDate8In ) const;

  /*! @brief DayOf
    This routine returns the numeric day for the given date object.
    @param iFormat
      XB_FMT_WEEK   Number of day in WEEK  0-6 ( Sat - Fri )<br>
      XB_FMT_MONTH  Number of day in MONTH 1-31<br>
      XB_FMT_YEAR   Number of day in YEAR  1-366
    @returns XB_INVALID_OPTION<br>XB_NO_ERROR
  */
  xbInt16     DayOf( xbInt16 iFormat = XB_FMT_MONTH ) const;

  #ifdef XB_DEBUG_SUPPORT
  /*! @brief Dump date information to stdout.
    @param sTitle - Title for output.
    @note This method is only available if the XB_DEBUG_SUPPORT option is compiled into libarary.
  */
  void        Dump( const char * sTitle );

  /*! @brief Dump the date tables.
    This dumps the internal date structures to stdout.
    @note This method is only available if the XB_DEBUG_SUPPORT option iscompiled into libarary.
  */
  void        DumpDateTables();
  #endif

  /*! @brief
    Create a formatted date based on the format specifiers entered in sFormatIn.
    If no input format is specified, the routine will use the system default date format.

    @param  sFormatIn - A format specifier with the following paramaters:<br>

        1) YYDDD    -  A julian date format
        2) YY or YYYY will print a 2 or 4 digit year
        3) M,MM,MMM or MMMM
           M    - one digit month if no leading zero
           MM   - two digit month, contains leading zero
           MMM  - Jan through Dec
           MMMM - January through December
        4) D,DD,DDD or DDDD
           D    - one digit dayif no leading zero
           DD   - two digit day, contains leading zero
           DDD  - Sun through Sat (or julian if YYDDD)
           DDDD - Sunday through Saturday


     Examples for sFmtIn:
     MM/DD/YY<br>
     YYYY-MM-DD<br>
     DDDDDDDDDDD MMMMMMMMMMM DD,YYYY


    @param sOutFormattedDate - Reformatted output date.
    @returns XB_NO_ERROR<br>XB_INVALID_DATE
  */
  xbInt16     FormatDate( const xbString &sFormatIn, xbString &sOutFormattedDate );

  /*! @brief Get the default date format for all dates.
    @returns xbString containing the default date format. 
  */
  xbString&  GetDefaultDateFormat      () const;

  ///@{
   /**
    @brief Determine if date object is a leap year.
    @returns xbTrue - Date object is a leapyear.<br>xbFalse - Date object is not a leap year.
   */
   xbBool IsLeapYear() const;
   /**
    @brief Determine if input year is a leap year.
    @param iYear - Input year in the format of CCYY
    @returns xbTrue - Date object is a leapyear.<br>xbFalse - Date object is not a leap year.
   */
   xbBool IsLeapYear( xbInt16 iYear ) const;
  ///@}


  /*! @brief Determine if date object is null.
    @returns xbTrue - Date object is null.<br> xbFalse - Date object is not null.
  */
  xbBool      IsNull() const;


  /*! @brief Calculate julian days for a given date. 
    @returns The number of days since 01/01/0001 + JUL_OFFSET (1721425L).
  */
  xbInt32     JulianDays() const;


  /*! @brief Convert julian days to gregorian date.
    @param lJulDate - Julian days.
  */
  void     JulToDate8( xbInt32 lJulDate );


  /*! @brief Set the date object to the last day of month.
      @returns XB_NO_ERROR<br>XB_INVALID_DATE
  */
  xbInt16     LastDayOfMonth();


  /*! @brief Return the month for the date object.
    @returns The month of the date.
  */
  xbInt16     MonthOf() const;

  /*! @brief operator != 
    Compare two dates for inequality.
    @param dt - Date to compare.
    @returns  xbTrue - Dates don't match.<br>xbFalse - Dates match.
  */
  xbBool operator!=( const xbDate &dt ) const;

  /*! @brief operator +
    Add <em>lCount</em> days to a date object.
    @param  lCount - Number of days to add.
    @returns New date in CCYYMMDD format.
  */
  const char * operator+( xbInt32 lCount );

  /*! @brief operator ++
    Add one day to the date object if the date is not null.
  */
  void operator++( xbInt32 );

  /*! @brief operator +=
    Add <em>lDays</em> to the date object if the date is not null.
    @param lDays - Number of days to add to the date.
  */
  void operator+=( xbInt32 lDays );

  ///@{
   /*! @brief operator -
     Subtract one date from another date returning the difference.
     @param dt - Date to subtract
     @returns Number of days difference or zero if one of the dates is null.
   */
   xbInt32 operator-( const xbDate &dt ) const;

   /*! @brief operator -
     Subtract <em>lCount</em> days from a date object.
     @param  lCount - Number of days to subtract.
     @returns New date in CCYYMMDD format.
   */
   const char * operator-( xbInt32 lCount );
  ///@}

  /*! @brief operator --
    Subtracts one day from the date object if the date is not null.
  */
  void operator--( xbInt32 lDays );


  /*! @brief operator -=
    Subtract <em>lDays</em> from the date object if the date is not null.
    @param lDays - Number of days to subtract from the date.
  */
  void operator-=( xbInt32 lDays );

  /*! @brief operator <
    Compares two dates for less than.
    @param dt - Date to compare.
    @returns  xbTrue - Left date is less than right date.<br>
            xbFalse - Left date is not less than right date.
  */
  xbBool operator< ( const xbDate &dt ) const;

  /*! @brief operator <=
    Compare two dates for less than or equal.
    @param dt - Date to compare.
    @returns  xbTrue - Left date is less than or equal to right date.<br>
              xbFalse - Left date is not less than or equal to right date.
  */
  xbBool operator<=( const xbDate &dt ) const;


  /*! @brief Set operator=
      @param dt - Date value for set operation.
  */
  void operator=( const xbDate &dt );

  /*! @brief operator ==
    Compare two dates for equality.
    @param dt - Date to compare.
    @returns xbTrue - Dates match.<br>xbFalse - Dates don't match.
  */
  xbBool operator==( const xbDate &dt ) const;

  /*! @brief operator >
    Compares two dates for greater than.
    @param dt - Date to compare.
    @returns  xbTrue - Left date is greater than right date.<br>
            xbFalse - Left date is not greater than right date.
  */
  xbBool operator> ( const xbDate &dt ) const;


  /*! @brief operator >=
    Compare two dates for greater than or equal.
    @param dt - Date to compare.
    @returns  xbTrue - Left date is greater than or equal to right date.<br>
            xbFalse - Left date is not greater than or equal to right date.
  */
  xbBool operator>=( const xbDate &dt ) const;


  /*! @brief Set the date object from sDateIn.
    @param sDateIn - Input date in format CCYYMMDD.
    @returns XB_NO_ERROR<br>XB_INVALID_DATE
  */
  xbInt16 Set( const xbString &sDateIn );

  /*! @brief Set the date object from input month day and year
    @param iMonth Input month<br>
    @param iDay Input Day<br>
    @param iYear Input year
    @returns XB_NO_ERROR<br>XB_INVALID_DATE
  */
  xbInt16 Set( xbInt16 iMonth, xbInt16 iDay, xbInt16 iYear );


  /*! @brief Set the default date format for all dates.
    @param sDefaultDateFormat - A format specifier with the following paramaters:<br>

        1) YYDDD    -  A julian date format
        2) YY or YYYY will print a 2 or 4 digit year
        3) M,MM,MMM or MMMM
           M    - one digit month if no leading zero
           MM   - two digit month, contains leading zero
           MMM  - Jan through Dec
           MMMM - January through December
        4) D,DD,DDD or DDDD
           D    - one digit dayif no leading zero
           DD   - two digit day, contains leading zero
           DDD  - Sun through Sat (or julian if YYDDD)
           DDDD - Sunday through Saturday

     Examples for sFmtIn:
     MM/DD/YY<br>
     YYYY-MM-DD<br>
     DDDDDDDDDDD MMMMMMMMMMM DD,YYYY
  */
  void SetDefaultDateFormat( const xbString &sDefaultDateFormat );


  /*! @brief Return the date value.
    @returns char ptr to date value in CCYYMMDD format.
  */
  const char *Str() const;

  /*! @brief Set the date equal to the system date.
  */
  void Sysdate();

  /*! @brief Returns the year of the date object.
    @returns The year of the date.
  */
  xbInt16 YearOf() const;

 private:
  void SetDateTables();
  xbString sDate8;              // CCYYMMDD date format  ie; 20140718
                                // Null date is identified by sDate.Len() < 8

  static int iAggregatedDaysInMonths[2][13];
  static int iDaysInMonths[2][13];
};

}         /* namespace */
#endif    /*__XB_XBDATE_H__ */

