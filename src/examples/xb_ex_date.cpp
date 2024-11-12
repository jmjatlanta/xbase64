/*  xb_ex_date.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2021,2022,2023,2024 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

This program demonstrates using the xbDate class

*/

#include "xbase.h"
using namespace xb;

int main()
{

   xbXBase  x;                             /* initial date static variables */

   xbString StringDate( "19601007" );      /* oct 7 1960 */
   char     CharDate[9] = "19611109";      /* nov 9 1961 */

   xbDate d1;
   d1.Sysdate();                           /* set to SysDate     */
   xbDate d2( StringDate );                /* from string data   */ 
   xbDate d3( CharDate );                  /* from char data     */
   xbDate d4;                              /* another date class */
   xbDate d5( d2 );                        /* set new date d5 equal to date d2 */
   xbString s;

   std::cout << "This program demonstrates usage of the xbDate class" << std::endl;
   std::cout << "Date 1 (Sysdate) is    " << d1.Str() << std::endl;
   std::cout << "Date 2 (StringDate) is " << d2.Str() << std::endl;
   std::cout << "Date 3 (CharDate) is   " << d3.Str() << std::endl;

   std::cout << "This year is  " << d1.YearOf() << std::endl;
   std::cout << "Year of xbString Date is " <<  d2.YearOf()
             << std::endl;
   std::cout << "Year of char Date is "     <<  d3.YearOf()
             << std::endl;

   std::cout << "This Month is " << d1.MonthOf() << std::endl;
   std::cout << "Month of xbString Date is " <<  d2.MonthOf()
             << std::endl;
   std::cout << "Month of char Date is "     <<  d3.MonthOf()
             << std::endl;

   std::cout << "Today is day " << d1.DayOf( XB_FMT_WEEK )       << " of the week"  << std::endl;
   std::cout << "StringDate is day " << d2.DayOf( XB_FMT_MONTH ) << " of the month" << std::endl;
   std::cout << "CharDate is day " << d3.DayOf( XB_FMT_YEAR )    << " of the year"  << std::endl;

   if( d1.IsLeapYear())
      std::cout << "This is a leap year" << std::endl;
   else
      std::cout << "This is not a leap year." << std::endl;

   if( d2.IsLeapYear())
      std::cout << "StringDate is a leap year" << std::endl;
   else
      std::cout << "StringDate is not a leap year." << std::endl;

   if( d3.IsLeapYear())
      std::cout << "CharDate is a leap year" << std::endl;
   else
       std::cout << "CharDate is not a leap year." << std::endl;

   d1.Sysdate();
   std::cout << "Today is " << d1.Str() << std::endl;

   if( d1.DateIsValid( "19951301" ))
      std::cout << "19951301 is a valid date" << std::endl;
   else
      std::cout << "19951301 is not a valid date" << std::endl;

   if( d1.DateIsValid( "19920229" ))
      std::cout << "19920229 is a valid date" << std::endl;
   else
      std::cout << "19920229 is not a valid date" << std::endl;

   if( d2.DateIsValid( StringDate ))
      std::cout << StringDate.Str() << " is a valid date" << std::endl;
   else
      std::cout << StringDate.Str() << " is not a valid date" << std::endl;

   std::cout << "Today's Julian date " << d1.JulianDays() << std::endl;
   std::cout << "Julian date of Jan 01, 1970 " << d2.JulianDays() << std::endl;
   std::cout << "StringDate Julian date " << d2.JulianDays()      << std::endl;

   std::cout << "There are " << (d1.JulianDays() - d2.JulianDays()) << " days between " << d1.Str() << " and " << d2.Str() <<  std::endl;

   std::cout << "Todays Julian date (Number of days since Jan 1 0100):" << d1.JulianDays() << std::endl; 

   d4 = d1;    // set d4 class = to sysdate 
   std::cout << "Object d4 initialized to " << d4.Str() << std::endl;

   d1.CharDayOf( s );
   std::cout << "Today is " << s.Str() << std::endl;

   d2.CharDayOf( s );
   std::cout << "StringDate day is " << s.Str() << std::endl;
   d1.CharMonthOf( s );
   std::cout << "This month is " << s.Str() << std::endl;
   d2.CharMonthOf( s );
   std::cout << "StringDate month is " << s.Str() << std::endl;


   /* various format routines using different formats, strings and chars */
   xbString sOutDate;

   d1.FormatDate( "YYDDD", sOutDate );
   std::cout << "Format (YYDDD) = " << sOutDate.Str() << std::endl;

   d1.FormatDate( "MM/DD/YY", sOutDate );
   std::cout << "Format (MM/DD/YY) = " << sOutDate.Str() << std::endl;

   d1.FormatDate( "YY-MM-DD", sOutDate );
   std::cout << "Format (YY-MM-DD) = " << sOutDate.Str() << std::endl;

   d1.FormatDate( "DDDDDDDDD, MMMMMMMMMM DD YYYY", sOutDate );
   std::cout << "Format (DDDDDDDDD, MMMMMMMMMM DD YYYY) = " << sOutDate.Str() << std::endl;

   d1.LastDayOfMonth();
   std::cout << "Last day this month " << d1.Str() << std::endl;

   d2.LastDayOfMonth();
   std::cout << "Last day of month for StringDate is " << d2.Str() << std::endl;

   std::cout << "Operators test..." << std::endl;

   if( d1 == d2 )
     std::cout << d1.Str() << " is equal to " << d2.Str()
               << std::endl;
   else
     std::cout << d1.Str() << " is not equal to " << d2.Str()
               << std::endl;

   if( d1 != d3 )
     std::cout << d1.Str() << " is not equal to " << d3.Str()
               << std::endl;
   else
     std::cout << d1.Str() << " is equal to " << d3.Str()
               << std::endl;

   if( d1 < d2 )
     std::cout << d1.Str() << " is less than " << d2.Str()
               << std::endl;
   else
     std::cout << d1.Str() << " is not less than " << d2.Str()
               << std::endl;

   if( d1 > d2 )
     std::cout << d1.Str() << " is greater than " << d2.Str()
               << std::endl;
   else
     std::cout << d1.Str() << " is not greater than " << d2.Str()
               << std::endl;

   if( d1 <= d2 )
     std::cout << d1.Str() << " is less than or equal to " << d2.Str()
               << std::endl;
   else
     std::cout << d1.Str() << " is not less than or equal to "
               << d2.Str() << std::endl;

   if( d1 >= d2 )
     std::cout << d1.Str() << " is greater than or equal to "
               << d2.Str() << std::endl;
   else
     std::cout << d1.Str() << " is not greater than or equal to "
               << d2.Str() << std::endl;

   d1.Sysdate();
   d1++;
   std::cout << "Tomorrow is " << d1.Str() << std::endl;
   d1-=2;
   std::cout << "Yesterday was " << d1.Str() << std::endl;
   std::cout << "There are " << d1 - d2 << " days between " << d1.Str() 
             << " and " << d2.Str() << std::endl;

   d1="20140701";
   std::cout << "Operator = example " << d1.Str() << std::endl;

   d1+=5;
   std::cout << "Operator += 5 example " << d1.Str() << std::endl;

   d1--;
   std::cout << "Operator -- example " << d1.Str() << std::endl;

   d1-4;
   std::cout << "Operator -4 example " << d1.Str() << std::endl;

   d1+10;
   std::cout << "Operator +10 example " << d1.Str() << std::endl;
   std::cout << "CenturyOf() " << d1.CenturyOf() << std::endl;

   xbString sWorkDate;
   d1.CTOD( "10/07/60" );
   std::cout << "CTOD( '10/07/60' ) " << d1.Str() << std::endl;

   d1.Set( "19590118" );
   std::cout << "Set( '19590118' ) " << d1.Str() << std::endl;

   std::cout << "CalcRollingCenturyForYear( 95 ) = " << d1.CalcRollingCenturyForYear( 95 ) << std::endl;

   xbDate dtSystemDate( 0 );
   std::cout << "System date = " << dtSystemDate.Str() << std::endl;

   return 0;
}
