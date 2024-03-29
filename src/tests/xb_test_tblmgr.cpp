/*  xb_test_tblmgr.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

// This program tests the table manager functions.
// usage:   xb_test_tblmgr QUITE|NORMAL|VERBOSE


#include "xbase.h"

using namespace xb;

#include "tstfuncs.cpp"

int main( int argCnt, char **av )
{
  int iRc = 0;
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

  InitTime();

  if( iPo == 2 ){
    std::cout << "DisplayError Test ==> ";
    x.DisplayError( 0 );
    #ifdef WIN32
    std::cout << "WIN32 environment" << std::endl;
    #else
    std::cout << "Not WIN32 environment" << std::endl;
    #endif
  }

  xbDbf * d1;
  xbDbf * d2;

  #if defined (XB_DBF3_SUPPORT )
    d1 = new xbDbf3( &x );
  #elif defined( XB_DBF4_SUPPORT )
    d1 = new xbDbf4( &x );
  #else
    std::cout << "No dbf file support built into library" << std::endl;
    return -1;
  #endif

  iRc += TestMethod( iPo, "AddTblToTblLst()", x.AddTblToTblList( d1, "TestTableC" ), 0 );
  iRc += TestMethod( iPo, "AddTblToTblLst()", x.AddTblToTblList( d1, "TestTableA" ), 0 );
  iRc += TestMethod( iPo, "AddTblToTblLst()", x.AddTblToTblList( d1, "TestTableB" ), 0 );
  iRc += TestMethod( iPo, "AddTblToTblLst()", x.AddTblToTblList( d1, "TestTableD" ), 0 );


  // Next line should generate an exception
  iRc += TestMethod( iPo, "AddTblToTblLst()", x.AddTblToTblList( d1, "TestTableC" ), XB_DUP_TABLE_OR_ALIAS );

  std::cout << "**** Next list should have one each of TestTableA, B, C, D sorted in alpha order ****" << std::endl;
  x.DisplayTableList();

  d2 = (xbDbf *) x.GetDbfPtr( "TestTableA" );
  if( d2 )
    std::cout << "[PASS] GetDbfPtr()" << std::endl;
  else{
    std::cout << "[FAIL] GetDbfPtr()" << std::endl;
    iRc--;
  }



  iRc += TestMethod( iPo, "RemoveTblFromDbList()", x.RemoveTblFromTblList( "TestTableB" ), 0 );
  iRc += TestMethod( iPo, "RemoveTblFromDbList()", x.RemoveTblFromTblList( "TestTableB" ), XB_NOT_FOUND );
  std::cout << "**** Next list should not have TestTableB in it ****" << std::endl;
  x.DisplayTableList();


  iRc += TestMethod( iPo, "RemoveTblFromDbList()", x.RemoveTblFromTblList( "TestTableA" ), 0 );
  iRc += TestMethod( iPo, "RemoveTblFromDbList()", x.RemoveTblFromTblList( "TestTableC" ), 0 );
  iRc += TestMethod( iPo, "RemoveTblFromDbList()", x.RemoveTblFromTblList( "TestTableD" ), 0 );


  delete d1;

  if( iPo > 0 || iRc < 0 )
    fprintf( stdout, "Total Errors = %d\n", iRc * -1 );


#ifdef XB_LOGGING_SUPPORT
  sMsg.Sprintf( "Program [%s] terminating with [%d] errors...", av[0], iRc * -1 );
  x.WriteLogMessage( sMsg );
#endif


  return iRc;
}

