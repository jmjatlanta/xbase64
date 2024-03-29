/* xb_test_bcd.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2017,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

// This program tests the xb bcd functions
// usage:   xb_test_expnode QUITE|NORMAL|VERBOSE

#include "xbase.h"
using namespace xb;

#include "tstfuncs.cpp"

/**************************************************************************/

int main( int argCnt, char **av )
{

  #ifdef XB_INDEX_SUPPORT

  xbInt16 iRc  = 0;
  xbInt16 iPo  = 1;     /* print option */
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

  if( iPo > 0 ){
    std::cout << "XBase bcd testing program.." << std::endl;
    std::cout << "This program tests the bcd logic." << std::endl;
  }

  iRc += TestMethod( iPo, "sizeof( xbBcdStruct )", (xbInt32) sizeof( xbBcdStruct ), (xbInt32) 12 );

  xbString s1( -12345.60 );
  xbBcd bcd1( s1 );
  xbDouble d1;
  bcd1.ToDouble( d1 );
  xbString s2( d1 );
  bcd1.ToString( s2 );

  iRc += TestMethod( iPo, "String Constructor1", d1, -12345.6 );
  iRc += TestMethod( iPo, "String Constructor2", s2.Str(), "-12345.6", 8 );

  d1 = 0034.04;
  xbBcd bcd2( d1 );
  bcd2.ToString( s2 );
  iRc += TestMethod( iPo, "xbDouble Constructor1", s2.Str(), "34.04", 5 );

  // test the sign comparison logic
  xbInt16 iComp = bcd1.Compare( bcd2 );
  iRc += TestMethod( iPo, "Sign Compare 1", iComp, -1 );
  iComp = bcd2.Compare( bcd1 );
  iRc += TestMethod( iPo, "Sign Compare 2", iComp, 1 );

  // bcd length compare scenarios
  bcd1.Set( 123 );
  iRc += TestMethod( iPo, "Length Compare 1", bcd1.Compare( 12 ), 1 );
  iRc += TestMethod( iPo, "Length Compare 2", bcd1.Compare( 1234 ), -1 );
  bcd1.Set( -456 );
  iRc += TestMethod( iPo, "Length Compare 3", bcd1.Compare( -12 ), -1 );
  iRc += TestMethod( iPo, "Length Compare 4", bcd1.Compare( -1234 ), 1 );

  // same length, value compares
  bcd1.Set( 11 );
  iRc += TestMethod( iPo, "Value Compare 1", bcd1.Compare( 10 ), 1 );

  bcd1.Set( 111 );
  iRc += TestMethod( iPo, "Value Compare 2", bcd1.Compare( 110 ), 1 );


  bcd1.Set( 111.111 );
  iRc += TestMethod( iPo, "Value Compare 3", bcd1.Compare( 111.112 ), -1 );

  bcd1.Set( -100 );
  iRc += TestMethod( iPo, "Value Compare 4", bcd1.Compare( -111 ), 1 );
  iRc += TestMethod( iPo, "Value Compare 5", bcd1.Compare( -99 ), -1 );

  bcd1.Set( (xbDouble) 0 );
  iRc += TestMethod( iPo, "Value Compare 6", bcd1.Compare( (xbDouble) 0 ), 0 );

  if( iPo > 0 || iRc < 0 )
    fprintf( stdout, "Total Errors = %d\n", iRc * -1 );

  #ifdef XB_LOGGING_SUPPORT
  sMsg.Sprintf( "Program [%s] terminating with [%d] errors...", av[0], iRc * -1 );
  x.WriteLogMessage( sMsg );
  #endif

  return iRc;
  #else
  return XB_NO_ERROR;
  #endif     // XB_INDEX_SUPPORT
  
}
