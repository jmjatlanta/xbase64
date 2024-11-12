/*  xbtime.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2024 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

#ifndef __XB_XBTIME_H__
#define __XB_XBTIME_H__



#ifdef XB_DBF5_SUPPORT

namespace xb{


//! @brief xbTime class.
/*!

The xbTime class handles time values.

Times are stored internally in a long integer field with the following formula:
      Hours * 3600000L + Minutes * 60000L + Seconds * 1000L;

Methods for setting the default time format at the system level:

    xbXBase::SetDefaultTimeFormat()
    xbXBase::GetDefaultTimeFormat()
    xbTime::SetDefaultTimeFormat()
    xbTime::GetDefaultTimeFormat()

Methods for setting the time format at the instance level (over ride system level default).

    xbTime::SetTimeFmt()
    xbTime::GetTimeFmt()

The system default time format is HH:MM:SS (\%H:\%M:\%S).


Time format specifiers (for applicable class methods).<br><br>
   \%H - is replaced by the hour (24 hour clock) as a decimal number (00-23)<br>
   \%I - is replaced by the hour (12 hour clock) as a decimal number (01-12)<br>
   \%M - is replaced by the minute as a decimal number (00-59)<br>
   \%S - is replaced by the second as a decimla number (00-59)<br>
   \%p - is replaced by the locale's equivalent of AM/PM with 12 hour clock<br>

@note The xbTime class is available when the XB_DBF5_SUPPORT option is compiled into the library.

*/


/*
Valid Time Format Specifiers:

  %H - is replaced by the hour (24 hour clock) as a decimal number (00-23)
  %I - is replaced by the hour (12 hour clock) as a decimal number (01-12)
  %M - is replaced by the minute as a decimal number (00-59)
  %S - is replaced by the second as a decimla number (00-59)
  %p - is replaced by the locale's equivalent of AM/PM with 12 hour clock
*/



class XBDLLEXPORT xbTime : xbCore {
 public:

  ///@{
   /** @brief Constructor
     This constructor sets the time to null (-1).
   */
   xbTime();

   /** @brief Constructor.
     @param sTime formated as HH:MM:SS or HH:MM
   */
   xbTime( xbString &sTime );

   /** @brief Constructor.
     @param sTime formated as HH:MM:SS or HH:MM
   */
   xbTime( char *sTime );

   /** @brief Constructor.
     @param lTime 0 - Set to system time<br>
                  or a valid time value as (hours * 3600000L) + (minutes * 60000L) + (seconds * 1000L)
   */
   xbTime( xbInt32 lTime );

   /** @brief Constructor.
     @param iHH Hours<br>
     @param iMM Minutes<br>
     @param iSS Seconds
   */
   xbTime( xbInt16 iHH, xbInt16 iMM, xbInt16 iSS = 0 );
  ///@}


  /** @brief Add method or subtract time
   @param cType is one of H,h,M,m,S,s for hour minute or second<br>
   @param lCnt Depending on cType, lCnt is number of hours, minutes or seconds to add to the time.<br>
               Use negative numbers to subtract from time.
  */
  xbInt32 Add      ( char cType, xbInt32 lCnt );

  ///@{
   #ifdef XB_DEBUG_SUPPORT
   /** @brief Dump time internals to stdout */
   void     Dump     ();

   /** @brief Dump time internals to stdout
     @param sHeader - Header message included with time internals.
   */

   void     Dump     ( xbString & sHeader );
   /** @brief Dump time internals to stdout
     @param sHeader - Header message included with time internals.
   */
   void     Dump     ( const char *sHeader );
   #endif
  ///@}

  /** @brief FromTimeT - Set time value from a time_t value
    @param tIn - time_t value to set the time
  */
  xbInt16  FromTimeT( time_t tIn );

  /** @brief Get the current default time format.
    @returns xbString containing the default time format.
    @note  Available if XB_DBF5_SUPPORT is on.
  */
  xbString&  GetDefaultTimeFormat() const;

  /** @brief GetHour
    @returns Hour value.
  */
  xbInt16 GetHour() const;

  /** @brief GetMinute
    @returns Minute value.
  */
  xbInt16 GetMinute() const;

  /** @brief GetSecond
    @returns Second value.
  */
  xbInt16 GetSecond() const;

  /** @brief GetTime
    @returns Long int time value.<br>
    Time is calculated as (hours * 3600000L) + (minutes * 60000L) + (seconds * 1000L)
  */
  xbInt32 GetTime() const;

  /** @brief GetTime
    @param sTimeOut - Output string in HH:MM:SS format.
  */
  xbInt16  GetTime  ( xbString &sTimeOut );

  /** @brief GetTime
     @param sTimeOut - Output string formatted based on specifers in sFmtIn
     @param sFmtIn - Time format specifier<br>
           \%H - is replaced by the hour (24 hour clock) as a decimal number (00-23)<br>
           \%I - is replaced by the hour (12 hour clock) as a decimal number (01-12)<br>
           \%M - is replaced by the minute as a decimal number (00-59)<br>
           \%S - is replaced by the second as a decimla number (00-59)<br>
           \%p - is replaced by the locale's equivalent of AM/PM with 12 hour clock<br>
     Example  \%H:\%M:\%S
  */
  xbInt16 GetTime( xbString &sTimeOut, xbString &sFmtIn );


  /** @brief GetTime
     @param sTimeOut - Output string formatted based on specifers in sFmtIn
     @param sFmtIn - Time format specifier<br>
           \%H - is replaced by the hour (24 hour clock) as a decimal number (00-23)<br>
           \%I - is replaced by the hour (12 hour clock) as a decimal number (01-12)<br>
           \%M - is replaced by the minute as a decimal number (00-59)<br>
           \%S - is replaced by the second as a decimla number (00-59)<br>
           \%p - is replaced by the locale's equivalent of AM/PM with 12 hour clock<br>
     Example  \%H:\%M:\%S
  */
  xbInt16 GetTime( xbString &sTimeOut, const char * sFmtIn );


  /** @brief GetTimeFmt returns the time format for the given time

     Returns the time format if set, else returns the system default time format.

     @returns Time format used for string representation of time.

  */
  const xbString &GetTimeFormat() const;

  /** @brief operator != Not Equal
    @param tTm Time to compare 
    @returns xbTrue - Times match.<br>
     xbFalse - Times don't match.<br>
  */
  xbBool operator!=( const xbTime &tTm ) const;


  /** @brief operator + 
    Add lSecs seconds to time.
    @param lSecs No of seconds to add.
  */
  xbInt32 operator+( xbInt32 lSecs ) const;

  /** @brief operator ++ Increment one second.
  */
  xbTime &operator++( xbInt32 );

  /** @brief Addition operator += Add lSecs seconds.
    @param lSecs - Number of seconds to add to time.
  */
  xbTime &operator+=( xbInt32 lSecs );

  /** @brief operator -
    Subtract lSecs seconds from time.
    @param lSecs No of seconds to subtract..
  */
  xbInt32 operator-( xbInt32 lSecs ) const;

  /** @brief operator -- Decrement one second.
  */
  xbTime &operator--( xbInt32 lSecs );

  /** @brief Subtraction operator -= Subtract lSecs seconds.
    \param lSecs - Number of seconds to subtract from time.
  */
  xbTime &operator-=( xbInt32 lSecs );

  /** @brief operator <
    This routine compares two times 
    @param tm - Time to compare.
    @returns  xbTrue - Left time is less than right date.<br>
              xbFalse - Left time is not less than right date.
  */
  xbBool operator< ( const xbTime &tm ) const;

  /** @brief operator <=
    This routine compares two times 
     @param tm - Time to compare.
     @returns  xbTrue - Left time is less than or equal to right date.<br>
            xbFalse - Left time is not less than or equal to right date.
  */
  xbBool operator<=( const xbTime &tm ) const;

  /** @brief Set operator=  Assignment operator.  Assign time value.
    Does not assign the optional format specifier.
    @param lTime - Time value for assignment operation.
  */
  void operator=( xbInt32 lTime );

  /** @brief Set operator=  Assignment operator.  
    Assigns the time value and format specifier from time object.
    @param tm - Time value for assignment operation.
  */
  void operator=( const xbTime &tm );

  /** @brief operator ==
    @param tm Time to compare 
    @returns xbTrue - Times match.<br>
     zbFalse - Times don't match.
  */
  xbBool operator==( const xbTime &tm ) const;

  /** @brief operator >
   This routine compares two times 
    @param tm - Time to compare.
    @returns  xbTrue - Left time is greater than right date.<br>
            xbFalse - Left time is not greater than right date.
  */
  xbBool operator> ( const xbTime &tm ) const;

  /** @brief operator >=
    This routine compares two times 
      @param tm - Time to compare.
      @returns  xbTrue - Left Time is greater than or equal to right date.<br>
            xbFalse - Left Time is not greater than or equal to right date.
  */
  xbBool operator>=( const xbTime &tm ) const;

  ///@{
   /** @brief Set
    Set the time
    @param iHH Hour
    @param iMM Minute
    @param iSS Second
    @returns XB_NO_ERROR or XB_INVALID_PARAMETER
   */
   xbInt16 Set( xbInt16 iHH = 0, xbInt16 iMM = 0, xbInt16 iSS = 0 );

   /** @brief Set
    Set the time
    @param lTime Set time to lTime.
    lTime is (hours * 3600000L) + (minutes * 60000L) + (seconds * 1000L)
    @returns XB_NO_ERROR or XB_INVALID_PARAMETER
   */
   xbInt16 Set( xbInt32 lTime );

   /** @brief Set
    @param sTime formated as HH:MM:SS or HH:MM
    @returns XB_NO_ERROR or XB_INVALID_PARAMETER
   */
   xbInt16 Set( xbString &sTime );

   /** @brief Set
    @param sTime formated as HH:MM:SS or HH:MM
    @returns XB_NO_ERROR or XB_INVALID_PARAMETER
   */
   xbInt16 Set( const char *sTime );

   /** @brief Set
    @param tm Set time and format from xbTime object tTm.
    @returns XB_NO_ERROR or XB_INVALID_PARAMETER
   */
    xbInt16 Set( xbTime &tm );
  ///@}

  ///@{
  /** @brief Set the default time format.
    @param sDefaultTimeFormat Format specifier
          \%H - is replaced by the hour (24 hour clock) as a decimal number (00-23)<br>
          \%I - is replaced by the hour (12 hour clock) as a decimal number (01-12)<br>
          \%M - is replaced by the minute as a decimal number (00-59)<br>
          \%S - is replaced by the second as a decimla number (00-59)<br>
          \%p - is replaced by the locale's equivalent of AM/PM with 12 hour clock<br>
     Example  \%H:\%M:\%S
  */
  void       SetDefaultTimeFormat( const xbString &sDefaultTimeFormat );
  void       SetDefaultTimeFormat( const char * sDefaultTimeFormat );
  ///@}

  ///@{
  /** @brief Set the time format for this specific instance.

    @param sFmtIn - Time format specifier<br>
           \%H - is replaced by the hour (24 hour clock) as a decimal number (00-23)<br>
           \%I - is replaced by the hour (12 hour clock) as a decimal number (01-12)<br>
           \%M - is replaced by the minute as a decimal number (00-59)<br>
           \%S - is replaced by the second as a decimla number (00-59)<br>
           \%p - is replaced by the locale's equivalent of AM/PM with 12 hour clock<br>
  */
  void SetTimeFormat( xbString &sFmtIn );
  void SetTimeFormat( const char * sFmtIn );
  ///@}
  /** @brief Set the time to the system time.
  */
  xbInt16 Systime ();

  /** @brief Return string representation of time

    Uses the currnt format to generate a string value and return it.

    @returns xbString
  */
  xbString & Str();

//  const xbString &GetTimeFormat() const;


  /** @brief ToTimeT
    Calulate and return the time in a time_t structure.
    Returns time in time_t structure
  */
  time_t ToTimeT() const;


 private:
  xbInt32  lTime;
  xbString sFmt;
  xbString sTime;
};



}        /* namespace xb   */
#endif   /* XB_DBF5_SUPPORT */
#endif   /* __XB_TIME_H__  */

