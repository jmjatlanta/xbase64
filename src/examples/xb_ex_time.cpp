/*  xb_ex_time.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2021,2022, 2023, 2024 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

This program demonstrates using the xbTime class

*/


#include "xbase.h"

using namespace xb;

int main()
{

   xbXBase  x;                             /* initial date static variables */

   xbString sTime;
   xbString sFmt;


  xbTime t1( 10, 11, 12 );
  sFmt = "%H:%M";
  t1.GetTime( sTime, sFmt );
  std::cout << "Time is [" << t1.GetHour() << "]\n";
  std::cout << "Hour is [" << t1.GetHour() << "]\n";
  std::cout << "Minute is [" << t1.GetMinute() << "]\n";
  std::cout << "Second is [" << t1.GetSecond() << "]\n";

  #ifdef XB_DEBUG_SUPPORT
  t1.Dump( "Dump of T1" );
  #endif  // XB_DEBUG_SUPPORT


  // various constructor examples
  xbString s = "08:15:45";
  xbTime t2( s );

  s = "9:2:5";
  xbTime t3( s );

  s = "3:2";
  xbTime t4( s );

  xbTime t5;
  t5.Systime();

  // add one second to t2
  t2.Add( 'S', 1 );

  // add 10 minutes to t2
  t2.Add( 'M', 10 );

  // add 1 hour to t2
  t2.Add( 'H', 1 );


  std::cout << "Default time format [" << t2.GetTimeFormat().Str() << "]\n";

  s = "%S|%M|%H";
  t2.SetTimeFormat( s );
  std::cout << "Over ride time format [" << t2.GetTimeFormat().Str() << "]\n";

  s = "%H%M%S";
  x.SetDefaultTimeFormat( s );
  std::cout << "Default time format is [" << x.GetDefaultTimeFormat().Str() << "]\n";

  // set t3 to what in t2
  t3 = t2;
  if( t2 == t3 )
    std::cout << "Times match\n";
  else
    std::cout << "Times don't match\n";

  if( t4 != t3 )
    std::cout << "Times don't match\n";
  else
    std::cout << "Times match\n";

  t1++;                      // add one second to t1
  t1+=6;                     // add six seconds to t1
  t1--;                      // subtract one second from t1
  t1-=15;                    // subtract 15 seconds from t1

  if( t3 > t4 )              // greater than compare
    std::cout << "true\n";
  else
    std::cout << "false\n";

  if( t3 < t4 )              // less than compare
    std::cout << "true\n";
  else
    std::cout << "false\n";

  if( t3 >= t4 )             // greater than or equal compare
    std::cout << "true\n";
  else
    std::cout << "false\n";

  if( t3 <= t4 )             // less than or equal compare
    std::cout << "true\n";
  else
    std::cout << "false\n";

  return 0;
}
