/* xb_test_date.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023,2024 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

// This program tests the date class xbDate

// usage:   xb_test_date QUITE|NORMAL|VERBOSE


#include "xbase.h"

using namespace xb;

#include "tstfuncs.cpp"


int main( int argCnt, char **av )
{
  int iRc  = 0;
  xbInt16 iPo = 1;      /* print option */
                       /* 0 - QUIET    */
                       /* 1 - NORMAL   */
                       /* 2 - VERBOSE  */

  if( argCnt > 1 ) {
    if( av[1][0] == 'Q' )
      iPo = 0;
    else if( av[1][0] == 'V' )
      iPo = 2;
  }

  xbXBase x;
  #ifdef XB_LOGGING_SUPPORT
  x.SetLogDirectory( PROJECT_LOG_DIR );
  x.EnableMsgLogging();
  if( iPo ){
    std::cout << "Logfile is [" << x.GetLogFqFileName().Str() << "]" << std::endl;
  }
  xbString sMsg;
  sMsg.Sprintf( "Program [%s] initializing...", av[0] );
  x.WriteLogMessage( sMsg );
  #endif
  InitTime();

  xbDate d1;
#ifdef XB_DEBUG_SUPPORT
  if( iPo > 0 )
    d1.DumpDateTables();
#endif

  xbDate d2( "19890209" );
  xbString s3 = "20120709";
  xbDate d3( s3 );
  xbString s4;
  xbString sFmt = "";
  xbString sOutDate; 
  xbString s;

  iRc += TestMethod( iPo, "FormatDate( '', sOutDate ) (sys default format)", d3.FormatDate( sFmt, sOutDate ), 0 );
  iRc += TestMethod( iPo, "FormatDate( '', sOutDate ) (sys default format)", sOutDate, "07/09/12", 8 );


  iRc += TestMethod( iPo, "CenturyOf()" , d3.CenturyOf(), 20 );
  iRc += TestMethod( iPo, "YearOf()", d3.YearOf(), 2012 );
  iRc += TestMethod( iPo, "MonthOf()", d3.MonthOf(), 7 );

  iRc += TestMethod( iPo, "DayOf( XB_FMT_DAY )", d3.DayOf( XB_FMT_WEEK ), 2 );
  iRc += TestMethod( iPo, "DayOf( XB_FMT_MONTH )", d3.DayOf( XB_FMT_MONTH ), 9 );
  iRc += TestMethod( iPo, "DayOf( XB_FMT_YEAR )", d3.DayOf( XB_FMT_YEAR ), 191 );

  iRc += TestMethod( iPo, "DateIsValid( '12345678' )", d3.DateIsValid( "12345678" ), 0 );
  iRc += TestMethod( iPo, "d3.DateIsValid( '20120708' )", d3.DateIsValid( "20120708" ), 1 );
  iRc += TestMethod( iPo, "d3.IsLeapYear()", d3.IsLeapYear(), 1 );
  iRc += TestMethod( iPo, "d2.IsLeapYear()", d2.IsLeapYear(), 0 );

  d1.Set( "20010102" );
  iRc += TestMethod( iPo, "d1.Set('20010102')", d1.Str(), "20010102", 8 );

  iRc += TestMethod( iPo, "d1.CalcRollingCenturyForYear(10)", d1.CalcRollingCenturyForYear( 10 ), 20 );
  iRc += TestMethod( iPo, "d1.JulianDays()", d1.JulianDays(), 2451912 );
  iRc += TestMethod( iPo, "d1.Set( '20140709' )", d1.Set( "20140709" ) , 0 );

  d1.CharDayOf( s );
  iRc += TestMethod( iPo, "d1.CharDayOf(s)", s, "Wednesday", 9 );

  d1.CharMonthOf( s );
  iRc += TestMethod( iPo, "d1.CharMonthOf()", s, "July", 4 );

  //iRc += TestMethod( iPo, "d1.JulToDate8( 2451913 )", d1.JulToDate8( 2451912 ), 0 ); is void
  d1.JulToDate8( 2451912 );
  iRc += TestMethod( iPo, "d1.JulToDate8( 2451913 )", d1.Str(), "20010102", 8 );

  d1.LastDayOfMonth();
  iRc += TestMethod( iPo, "d1.LastDayOfMonth()", d1.Str(), "20010131", 8 );

  iRc += TestMethod( iPo, "d1.CTOD( '12/01/02' )", d1.CTOD( "12/01/02" ), 0 );
  iRc += TestMethod( iPo, "d1.CTOD( '12/01/02' )", d1.Str(), "20021201", 8 );

  xbString s1, s2;
  s1 = "MM-DD-YY";
  d1 = "20130402";
  iRc+= TestMethod( iPo, "d1.FormatDate( s1, sOutDate )", d1.FormatDate( s1, sOutDate ), 0 );
  iRc+= TestMethod( iPo, "d1.FormatDate( s1, sOutDate )", sOutDate, "04-02-13", 8 );

  d1 = "20120203";
  iRc += TestMethod( iPo, "d1=20120203", d1.Str(), "20120203", 8 );

  s1 = "20130405";
  d1 = s1;
  iRc += TestMethod( iPo, "d1=s1", d1.Str(), "20130405", 8 );

  d2 = d1;
  iRc += TestMethod( iPo, "d2=d1", d2.Str(), "20130405", 8 );

  d2+=5;
  iRc += TestMethod( iPo, "d2+=5", d2.Str(), "20130410", 8 );

  d2-=7;
  iRc += TestMethod( iPo, "d2-=5", d2.Str(), "20130403", 8 );

  d2++;
  iRc += TestMethod( iPo, "d2++", d2.Str(), "20130404", 8 );

  d2--;
  iRc+= TestMethod( iPo, "d2--", d2.Str(), "20130403", 8 );
  iRc+= TestMethod( iPo, "d2-d1", d1-d2, 2 );
  iRc+= TestMethod( iPo, "d1+2", d1+2, "20130407", 8 );
  iRc+= TestMethod( iPo, "d1-5", d1-5, "20130402", 8 );

  //d1.Dump( "d1" );
  //d2.Dump( "d2" );

  iRc+= TestMethod( iPo, "d1==d2", d1==d2, 0 );
  d2 = d1;
  iRc+= TestMethod( iPo, "d1==d2", d1==d2, 1 );
  iRc+= TestMethod( iPo, "d1>=d2", d1>=d2, 1 );
  iRc+= TestMethod( iPo, "d1<=d2", d1<=d2, 1 );
  iRc+= TestMethod( iPo, "d1!=d2", d1!=d2, 0 );
  d2++;
  iRc+= TestMethod( iPo, "d1!=d2", d1!=d2, 1 );

  iRc+= TestMethod( iPo, "d1>d2", d1>d2, 0 );
  iRc+= TestMethod( iPo, "d1>=d2", d1>=d2, 0 );
  iRc+= TestMethod( iPo, "d1<d2", d1<d2, 1 );
  iRc+= TestMethod( iPo, "d1<=d2", d1<=d2, 1 );

  d2-=2;
  iRc+= TestMethod( iPo, "d1>d2", d1>d2, 1 );
  iRc+= TestMethod( iPo, "d1>=d2", d1>=d2, 1 );
  iRc+= TestMethod( iPo, "d1<d2", d1<d2, 0 );
  iRc+= TestMethod( iPo, "d1<=d2", d1<=d2, 0 );
  iRc+= TestMethod( iPo, "d1-d2", d1-d2, 1 );


  // null date testing follows
  iRc+= TestMethod( iPo, "IsNull", d1.IsNull(), xbFalse );
  xbDate dN;
  iRc+= TestMethod( iPo, "IsNull", dN.IsNull(), xbTrue );

  xbDate dtNull1;
  xbDate dtNull2;

  iRc+= TestMethod( iPo, "dtNull1==dtNull2", dtNull1 == dtNull2, 1 );
  iRc+= TestMethod( iPo, "dtNull1!=dtNull2", dtNull1 != dtNull2, 0 );

  iRc+= TestMethod( iPo, "dtNull1 < d2", dtNull1 < d2, 1 );
  iRc+= TestMethod( iPo, "dtNull1 > d2", dtNull1 > d2, 0 );
  iRc+= TestMethod( iPo, "dtNull1 <= d2", dtNull1 <= d2, 1 );
  iRc+= TestMethod( iPo, "dtNull1 >= d2", dtNull1 >= d2, 0 );

  iRc+= TestMethod( iPo, "d2 < dtNull1",  d2 <  dtNull1, 0 );
  iRc+= TestMethod( iPo, "d2 > dtNull1",  d2 >  dtNull1, 1 );
  iRc+= TestMethod( iPo, "d2 <= dtNull1", d2 <= dtNull1, 0 );
  iRc+= TestMethod( iPo, "d2 >= dtNull1", d2 >= dtNull1, 1 );


/*
  xbDate d4( "20171015" );
  std::cout << "day of [" << d4.Str() << "][" << d4.DayOf( XB_FMT_WEEK ) << "]\n";
  d4++;
  std::cout << "day of [" << d4.Str() << "][" << d4.DayOf( XB_FMT_WEEK ) << "]\n";
  d4++;
  std::cout << "day of [" << d4.Str() << "][" << d4.DayOf( XB_FMT_WEEK ) << "]\n";
  d4++;
  std::cout << "day of [" << d4.Str() << "][" << d4.DayOf( XB_FMT_WEEK ) << "]\n";
  d4++;
  std::cout << "day of [" << d4.Str() << "][" << d4.DayOf( XB_FMT_WEEK ) << "]\n";
  d4++;
  std::cout << "day of [" << d4.Str() << "][" << d4.DayOf( XB_FMT_WEEK ) << "]\n";
  d4++;
  std::cout << "day of [" << d4.Str() << "][" << d4.DayOf( XB_FMT_WEEK ) << "]\n";
  d4++;
  std::cout << "day of [" << d4.Str() << "][" << d4.DayOf( XB_FMT_WEEK ) << "]\n";
*/

  d2 = "19890209";
  xbDate d5( d2 );
 // std::cout << "d5 = " << d5.Str() << "\n";
  iRc += TestMethod( iPo, "d5(xbDate)", d5.Str() , "19890209", 8 );

  d2.SetDefaultDateFormat( "MM/DD/YY" );
  iRc+= TestMethod( iPo, "GetDefaulDateFormat()", d2.GetDefaultDateFormat().Str(), "MM/DD/YY", 8 );


  xbDate dALK( 2, 9, 1989 );
  iRc += TestMethod( iPo, "Constructor", dALK.Str(), "19890209", 8 );

  dALK.Set( 1, 3, 1988 );
  iRc += TestMethod( iPo, "Set", dALK.Str(), "19880103", 8 );

  if( iPo > 0 || iRc < 0 )
    fprintf( stdout, "Total Errors = %d\n", iRc * -1 );

#ifdef XB_LOGGING_SUPPORT
  sMsg.Sprintf( "Program [%s] terminating with [%d] errors...", av[0], iRc * -1 );
  x.WriteLogMessage( sMsg );
#endif

  return iRc;
}
