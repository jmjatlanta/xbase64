/* xb_test_expnode.cpp

XBase Software Library

Copyright (c) 1997,2003,2014,2017,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

// This program tests the xb expression node functions
// usage:   xb_test_expnode QUITE|NORMAL|VERBOSE

#include "xbase.h"
using namespace xb;

#include "tstfuncs.cpp"


/**************************************************************************/

int main( int argCnt, char **av )
{
  xbInt16 iRc  = 0;
  xbInt16 po  = 1;     /* print option */
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
  InitTime();

  if( po > 0 ){
    std::cout << "XBase Expression Node testing program.." << std::endl;
    std::cout << "This program tests the XBase expression node logic." << std::endl;
  }

  xbString s1 = "TestNode1";
  xbExpNode *n1  = new xbExpNode();
  n1->SetNodeText( s1 );
  n1->SetNodeType( XB_EXP_OPERATOR );
  n1->SetReturnType( XB_EXP_LOGICAL );

  iRc += TestMethod( po, "GetNodeType()", n1->GetNodeType(), 'O' );
  iRc += TestMethod( po, "GetReturnType()", n1->GetReturnType(), 'L' );

  xbString s2 = "TestNode2";
  xbExpNode *n2  = new xbExpNode( s2, XB_EXP_FUNCTION, XB_EXP_CHAR );

  xbString s3 = "TestNode3";
  xbExpNode *n3  = new xbExpNode( s3, XB_EXP_CONSTANT, XB_EXP_NUMERIC );

  xbString s4 = "TestNode4";
  xbExpNode *n4  = new xbExpNode( s4, XB_EXP_OPERATOR, XB_EXP_DATE );

  xbString s5 = "TestNode5";
  xbExpNode *n5  = new xbExpNode( s5, XB_EXP_FIELD, XB_EXP_LOGICAL );

  n1->AddChild( n2 );
  n1->AddChild( n3 );
  n1->AddChild( n4 );
  n1->AddChild( n5 );

  iRc += TestMethod( po, "GetChildCnt()", (xbInt32) n1->GetChildCnt(), 4 );

  n1->SetResult( s1 );
  iRc += TestMethod( po, "SetResult() / GetStringResult()", s1, n1->GetStringResult(), 9 );

  xbBool bVal = xbTrue;
  n1->SetResult( bVal );
  iRc += TestMethod( po, "SetResult() / GetBoolResult()", xbTrue, n1->GetBoolResult() );

  xbDouble d = 123456.789;
  n1->SetResult( d );
  iRc += TestMethod( po, "SetResult() / GetNumericResult()", d, n1->GetNumericResult() );

  #ifdef XB_DEBUG_SUPPORT 
  if( po > 0 ){
    n1->DumpNode( xbTrue );
    n1->GetChild( 0 )->DumpNode( xbTrue );
    n1->GetChild( 1 )->DumpNode( xbTrue );
    n1->GetChild( 2 )->DumpNode( xbTrue );
    n1->GetChild( 3 )->DumpNode( xbTrue );
  }
  #endif

  delete n1;

  if( po > 0 || iRc < 0 )
    fprintf( stdout, "Total Errors = %d\n", iRc * -1 );

  #ifdef XB_LOGGING_SUPPORT
  sMsg.Sprintf( "Program [%s] terminating with [%d] errors...", av[0], iRc * -1 );
  x.WriteLogMessage( sMsg );
  #endif

  return iRc;
}
