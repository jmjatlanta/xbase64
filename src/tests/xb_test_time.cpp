/* xb_test_time.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023,20224 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

// This program tests the time class and functions

// usage:   xb_test_uda QUITE|NORMAL|VERBOSE



#include "xbase.h"


using namespace xb;

#include "tstfuncs.cpp"


int main( int argCnt, char **av )
{
  int iRc = 0;
  //int iRc2 = 0;
  int iPo = 1;          /* print option */
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

  x.SetDataDirectory( PROJECT_DATA_DIR );

  xbTime t1( 10, 11, 12 );
  iRc += TestMethod( iPo, "GetHour(100)",   t1.GetHour(), 10 );
  iRc += TestMethod( iPo, "GetMinute(101)", t1.GetMinute(), 11 );
  iRc += TestMethod( iPo, "GetSecond(102)", t1.GetSecond(), 12 );
  #ifdef XB_DEBUG_SUPPORT
  t1.Dump();
  #endif  // XB_DEBUG_SUPPORT


  xbString s = "08:15:45";
  xbTime t2( s );
  iRc += TestMethod( iPo, "GetHour(110)",   t2.GetHour(), 8 );
  iRc += TestMethod( iPo, "GetMinute(111)", t2.GetMinute(), 15 );
  iRc += TestMethod( iPo, "GetSecond(112)", t2.GetSecond(), 45 );

  s = "9:2:5";
  xbTime t3( s );
  iRc += TestMethod( iPo, "GetHour(120)",   t3.GetHour(), 9 );
  iRc += TestMethod( iPo, "GetMinute(121)", t3.GetMinute(), 2 );
  iRc += TestMethod( iPo, "GetSecond(122)", t3.GetSecond(), 5 );

  s = "3:2";
  xbTime t4( s );
  iRc += TestMethod( iPo, "GetHour(130)",   t4.GetHour(), 3 );
  iRc += TestMethod( iPo, "GetMinute(131)", t4.GetMinute(), 2 );
  iRc += TestMethod( iPo, "GetSecond(132)", t4.GetSecond(), 0 );


  xbTime t5;
  t5.Systime();
  #ifdef XB_DEBUG_SUPPORT
  t5.Dump( "Now" );
  #endif  // XB_DEBUG_SUPPORT

  t2.Add( 'S', 1 );

  iRc += TestMethod( iPo, "GetHour(140)",   t2.GetHour(), 8 );
  iRc += TestMethod( iPo, "GetMinute(141)", t2.GetMinute(), 15 );
  iRc += TestMethod( iPo, "GetSecond(142)", t2.GetSecond(), 46 );

  t2.Add( 'S', 10 );
  iRc += TestMethod( iPo, "GetHour(150)",   t2.GetHour(), 8 );
  iRc += TestMethod( iPo, "GetMinute(151)", t2.GetMinute(), 15 );
  iRc += TestMethod( iPo, "GetSecond(152)", t2.GetSecond(), 56 );

  t2.Add( 'M', 10 );
  iRc += TestMethod( iPo, "GetHour(160)",   t2.GetHour(), 8 );
  iRc += TestMethod( iPo, "GetMinute(161)", t2.GetMinute(), 25 );
  iRc += TestMethod( iPo, "GetSecond(162)", t2.GetSecond(), 56 );

  t2.Add( 'M', 40 );
  iRc += TestMethod( iPo, "GetHour(170)",   t2.GetHour(), 9 );
  iRc += TestMethod( iPo, "GetMinute(171)", t2.GetMinute(), 5 );
  iRc += TestMethod( iPo, "GetSecond(172)", t2.GetSecond(), 56 );

  t2.Add( 'H', 1 );
  iRc += TestMethod( iPo, "GetHour(180)",   t2.GetHour(), 10 );

  t2.Add( 'H', 5 );
  iRc += TestMethod( iPo, "GetHour(190)",   t2.GetHour(), 15 );

  t2.Add( 'h', -4 );
  iRc += TestMethod( iPo, "GetHour(200)",   t2.GetHour(), 11 );

  xbInt32 lDays = t2.Add( 'h', 60 );
  iRc += TestMethod( iPo, "GetDays(201)",    lDays, 2 );

  s = "%H-%M";
   xbString sOut;
  t2.GetTime( sOut, s );
  iRc += TestMethod( iPo, "GetTime( 300 )", sOut.Str(), "23-05", 5 );
  t2.GetTime( sOut );
  iRc += TestMethod( iPo, "GetTime( 301 )", sOut.Str(), "23:05:56", 8 );


  s = "%H:%M:%S";
  iRc += TestMethod( iPo, "GetTimeFormat( 310 )", t2.GetTimeFormat(), s.Str(), 8 );

  s = "%S|%M|%H";
  t2.SetTimeFormat( s );
  iRc += TestMethod( iPo, "Set/GetTimeFormat( 315 )", t2.GetTimeFormat(), s.Str(), 8 );

  t3 = t2;
  if( t2 == t3 )
    iRc += TestMethod( iPo, "operator= / operator== (320)", xbTrue, xbTrue );
  else
    iRc += TestMethod( iPo, "operator= / operator== (320)", xbTrue, xbFalse );

  if( t4 != t3 )
    iRc += TestMethod( iPo, "operator!= (321)", xbTrue, xbTrue );
  else
    iRc += TestMethod( iPo, "operator!= (321)", xbTrue, xbFalse );

  t1++;
  iRc += TestMethod( iPo, "operator++ (322)", t1.GetSecond(), 13 );
  t1+=6;
  iRc += TestMethod( iPo, "operator++ (323)", t1.GetSecond(), 19 );
  t1--;
  iRc += TestMethod( iPo, "operator++ (324)", t1.GetSecond(), 18 );
  t1-=15;
  iRc += TestMethod( iPo, "operator++ (325)", t1.GetSecond(), 03 );

  iRc += TestMethod( iPo, "operator > (330)", t3 > t4, xbTrue );
  iRc += TestMethod( iPo, "operator > (331)", t4 > t3, xbFalse );
  iRc += TestMethod( iPo, "operator < (332)", t3 < t4, xbFalse );
  iRc += TestMethod( iPo, "operator < (333)", t4 < t3, xbTrue );
  iRc += TestMethod( iPo, "operator >= (334)", t3 >= t4, xbTrue );
  iRc += TestMethod( iPo, "operator >= (335)", t4 >= t3, xbFalse );
  iRc += TestMethod( iPo, "operator <= (336)", t3 <= t4, xbFalse );
  iRc += TestMethod( iPo, "operator <= (337)", t4 <= t3, xbTrue );
  t3 = t4;
  iRc += TestMethod( iPo, "operator >= (334)", t3 >= t4, xbTrue );
  iRc += TestMethod( iPo, "operator >= (335)", t4 >= t3, xbTrue );
  iRc += TestMethod( iPo, "operator <= (336)", t3 <= t4, xbTrue );
  iRc += TestMethod( iPo, "operator <= (337)", t4 <= t3, xbTrue );

  t2.GetTime( s, "%H%M%S" );
  iRc += TestMethod( iPo, "GetTime()", s.Str(), "230556", 6 );

  if( iPo > 0 || iRc < 0 )
    fprintf( stdout, "Total Errors = %d\n", iRc * -1 );


#ifdef XB_LOGGING_SUPPORT
  sMsg.Sprintf( "Program [%s] terminating with [%d] errors...", av[0], iRc * -1 );
  x.WriteLogMessage( sMsg );
#endif

  return iRc;
}

