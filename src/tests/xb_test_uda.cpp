/* xb_test_uda.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

// This program tests the uda (user data area) functions

// usage:   xb_test_uda QUITE|NORMAL|VERBOSE



#include "xbase.h"

using namespace xb;

#include "tstfuncs.cpp"


int main( int argCnt, char **av )
{
  int iRc = 0;
  //int iRc2 = 0;
  int po = 1;          /* print option */
                       /* 0 - QUIET    */
                       /* 1 - NORMAL   */
                       /* 2 - VERBOSE  */

  if( argCnt > 1 ) {
    if( av[1][0] == 'Q' )
      po = 0;
    else if( av[1][0] == 'V' )
      po = 2;
  }



  xbXBase x;
  #ifdef XB_LOGGING_SUPPORT
  x.SetLogDirectory( PROJECT_LOG_DIR );
  x.EnableMsgLogging();
  if( po ){
    std::cout << "Logfile is [" << x.GetLogFqFileName().Str() << "]" << std::endl;
  }
  xbString sMsg;
  sMsg.Sprintf( "Program [%s] initializing...", av[0] );
  x.WriteLogMessage( sMsg );
  #endif

  x.SetDataDirectory( PROJECT_DATA_DIR );

  xbUda uda;
  iRc += TestMethod( po, "GetTokencCnt()",  uda.GetTokenCnt(), 0 );
  xbString s1;
  xbString s2;
  iRc += TestMethod( po, "AddTokenForKey()", uda.AddTokenForKey( "B", "BBBB" ), XB_NO_ERROR );
  s1 = "C";
  iRc += TestMethod( po, "AddTokenForKey()", uda.AddTokenForKey( s1, "CCCC" ), XB_NO_ERROR );
  s2 = "DDDD";
  iRc += TestMethod( po, "AddTokenForKey()", uda.AddTokenForKey( "D", s2 ), XB_NO_ERROR );

  s1 = "A";
  s2 = "AAAA";
  iRc += TestMethod( po, "AddTokenForKey()", uda.AddTokenForKey( s1, s2 ), XB_NO_ERROR );
  iRc += TestMethod( po, "AddTokenForKey()", uda.AddTokenForKey( "AA", "AAAAA" ), XB_NO_ERROR );
  iRc += TestMethod( po, "AddTokenForKey()", uda.AddTokenForKey( "BB", "BBBBB" ), XB_NO_ERROR );
  iRc += TestMethod( po, "AddTokenForKey()", uda.AddTokenForKey( "CC", "CCCCC" ), XB_NO_ERROR );
  iRc += TestMethod( po, "AddTokenForKey()", uda.AddTokenForKey( "DD", "DDDDD" ), XB_NO_ERROR );
  iRc += TestMethod( po, "GetTokencCnt()",  uda.GetTokenCnt(), 8);

  iRc += TestMethod( po, "UpdTokenForKey()", uda.UpdTokenForKey( "BB", "9999" ), XB_NO_ERROR );
  iRc += TestMethod( po, "UpdTokenForKey()", uda.DelTokenForKey( "CC" ), XB_NO_ERROR );
  iRc += TestMethod( po, "GetTokencCnt()",  uda.GetTokenCnt(), 7 );

#ifdef XB_DEBUG_SUPPORT
   uda.DumpUda();
#endif  // XB_DEBUG_SUPPORT

   uda.Clear();
   iRc += TestMethod( po, "GetTokencCnt()",  uda.GetTokenCnt(), 0 );

#ifdef XB_LOGGING_SUPPORT
  sMsg.Sprintf( "Program [%s] terminating with [%d] errors...", av[0], iRc * -1 );
  x.WriteLogMessage( sMsg );
#endif

  return iRc;
}
