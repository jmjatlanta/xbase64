/* xb_test_expression.cpp

XBase Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

// This program tests the class xbExp
// usage:   xb_test_expression QUITE|NORMAL|VERBOSE

#include "xbase.h"
using namespace xb;

#include "tstfuncs.cpp"


/**************************************************************************/
///@cond DOXYOFF

class xbExpTest : public xbExp {

 public:
   xbExpTest( xbXBase * x );
   xbInt16 GetNextTokenTest( xbExpToken &t );
   xbInt16 OperatorWeightTest( const xbString &sOperator );
   xbExpNode *GetNextNodeTest( xbExpNode * );
};

/**************************************************************************/
xbExpTest::xbExpTest( xbXBase * x ) : xbExp( x ){
}
/**************************************************************************/
xbExpNode *xbExpTest::GetNextNodeTest( xbExpNode *n ){
  return GetNextNode( n );
}
/**************************************************************************/
xbInt16 xbExpTest::GetNextTokenTest( xbExpToken &t ){
  return GetNextToken( t );
}
/**************************************************************************/
xbInt16 xbExpTest::OperatorWeightTest( const xbString &sOperator ){
  return OperatorWeight( sOperator );
}
///@endcond DOXYOFF


/**************************************************************************/
xbInt16 DevTester( xbXBase * xb, xbDbf * dbf, const char * sExpIn ){

  xbExpTest exp( xb );
  xbExpNode *n;

  std::cout << "going to parse expression [" << sExpIn << "]\n";
  xbInt16 iRc = exp.ParseExpression( dbf, sExpIn );

  std::cout << "back from parse expression\n";
  std::cout << "dump nodes\n";
  n = exp.GetNextNodeTest( NULL );
  if( iRc == XB_NO_ERROR ){
    #ifdef XB_DEBUG_SUPPORT
    exp.DumpTree( xbTrue );
    #endif   // XB_DEBUG_SUPPORT
    std::cout << "dump nodes\n";
    n = exp.GetNextNodeTest( NULL );
    if( !n ){
      std::cout << "n is null\n";
    } else {
      xbInt16 i = 0;
      while( n && i < 30 ){
        std::cout << "Node Text = [" << n->GetStringResult().Str() << "]\n";
        n = exp.GetNextNodeTest( n );
        i++;
      }
    }
    std::cout << "end dump nodes\n";
    return 0;
  }else{
    std::cout << "Parse Error [" << iRc << "]\n";
    return -1;
  }
}

/**************************************************************************/
xbInt16 TestWeight( xbXBase * xb, xbInt16 iPrintOption, const char * title, const char *sOperator, xbInt16 iExpectedWeight );
xbInt16 TestWeight( xbXBase * xb, xbInt16 iPrintOption, const char * title, const char *sOperator, xbInt16 iExpectedWeight ){

  xbInt16   iRc = 0;
  xbExpTest expT1( xb );

  if(( iRc = expT1.OperatorWeightTest( sOperator )) != iExpectedWeight ){
    std::cout << std::endl << "[FAIL O1] " << title << std::endl;
    std::cout << " Operator [" << sOperator << "] Weight [" << iRc << "] Expected [" << iExpectedWeight << "]" << std::endl;
    return -1;
  }

  if( iPrintOption == 2 )
    std::cout << "[PASS] " << title << " Expected matches actual Data=[" << sOperator << "][" << iExpectedWeight << "]" << std::endl;
  else if( iPrintOption == 1 )
    std::cout << "[PASS] " << title << std::endl;

  return XB_NO_ERROR;
}
/**************************************************************************/
xbInt16 TestTokenMethod( xbXBase *xb, xbInt16 iPrintOption, const char * title, const char *sExpression, 
               const char *sExpectedToken, const char *sExpectedRemainder, char cExpectedNodeType, 
               char cExpectedReturnType, xbInt16 iErc, xbInt16 iTrc );

xbInt16 TestTokenMethod( xbXBase *xb, xbInt16 iPrintOption, const char * title, const char *sExpression, 
                         const char *sExpectedToken, const char *sExpectedRemainder, char cExpectedNodeType, 
                         char cExpectedReturnType, xbInt16 iErc, xbInt16 iTrc ){

  xbExpTest expT1( xb );
  xbExpToken t;
  xbInt16  iRc = XB_NO_ERROR;
  t.sExpression = sExpression;

  if(( iRc = expT1.GetNextTokenTest( t )) != iErc ){
    std::cout << std::endl << "[FAIL T1] " << title << std::endl;
    std::cout << " Expression [" << sExpression << "]" << std::endl;
    std::cout << " GetNextToken iRc = [" <<  iRc << "]" << std::endl;
    return -1;
  }

  if( iTrc != t.iSts ){
    std::cout << std::endl << "[FAIL T2] " << title << std::endl;
    std::cout << " Expression [" << sExpression << "]" << std::endl;
    std::cout << " GetNextToken Token Return Code = [" <<  iTrc << "]" << std::endl;
    return -1;
  }
  if( iErc == XB_NO_ERROR && iTrc == XB_NO_ERROR && t.sToken != sExpectedToken ){
    std::cout << std::endl << "[FAIL T3] " << title << std::endl;
    std::cout << " Expression " << sExpression << std::endl;
    std::cout << "  Expected Result = [" << sExpectedToken << "] Actual = [" << t.sToken.Str() << "]\n";
    return -1;
  }
  if( iErc == XB_NO_ERROR && iTrc == XB_NO_ERROR && t.sExpression != sExpectedRemainder ){
    std::cout << std::endl << "[FAIL T4] " << title << std::endl;
    std::cout << " Expression " << sExpression << std::endl;
    std::cout << "  Expected Remainder = [" << sExpectedRemainder << "] Actual = [" << t.sExpression.Str() << "]\n";
    return -1;
  }
  if( iErc == XB_NO_ERROR && iTrc == XB_NO_ERROR&& t.cNodeType != cExpectedNodeType ){
    std::cout << std::endl << "[FAIL T5] " << title << std::endl;
    std::cout << " Expression " << sExpression << std::endl;
    std::cout << "  Expected Node type = [" <<  cExpectedNodeType  << "] Actual = [" << t.cNodeType << "]\n";
    return -1;
  }
  if( iErc == XB_NO_ERROR && iTrc == XB_NO_ERROR&& t.cReturnType != cExpectedReturnType ){
    std::cout << std::endl << "[FAIL T6] " << title << std::endl;
    std::cout << " Expression " << sExpression << std::endl;
    std::cout << "  Expected Return type = [" << cExpectedReturnType << "] Actual = [" << t.cReturnType  << "]\n";
    return -1;
  }
  if( iPrintOption == 2 ){
    if( iErc == XB_NO_ERROR )
      std::cout << "[PASS] " << title << " Expected matches actual Data=[" << sExpectedToken << "]" << std::endl;
    else
      std::cout << "[PASS] " << title << " Expected return code matches actual =[" << iErc << "]" << std::endl;
  } else if( iPrintOption == 1 )
    std::cout << "[PASS] " << title << std::endl;

  return 0;
}

/**************************************************************************/
xbInt16 TestMethod( xbXBase *xb, xbDbf *d, xbInt16 PrintOption, const char * title,
                    const xbString &sExpression, const char * sExpectedResult, xbInt16 iExpectedResultLen );

xbInt16 TestMethod( xbXBase *xb, xbDbf *d, xbInt16 PrintOption, const char * title,
                    const xbString &sExpression, const char * sExpectedResult, xbInt16 iExpectedResultLen ){

  xbInt16 iRc = XB_NO_ERROR;
  xbString sResult;
  xbExp exp( xb );

  if(( iRc = exp.ParseExpression( d, sExpression )) != XB_NO_ERROR ){
    std::cout << std::endl << "[FAIL 1] " << title << std::endl;
    std::cout << "  Return Code  = [" << iRc << "]" << std::endl;
    return -1;
  }
  if(( iRc = exp.GetReturnType()) != XB_EXP_CHAR ){
    std::cout << std::endl << "[FAIL 2] " << title << std::endl;
    std::cout << "  Result type not XB_EXP_CHAR  = [" << exp.GetReturnType() << "]" << std::endl;
    return -1;
  }
  if(( iRc = exp.ProcessExpression()) != XB_NO_ERROR ){
    std::cout << std::endl << "[FAIL 3] " << title << std::endl;
    std::cout << "  Return Code  = [" << iRc << "]" << std::endl;
    return -1;
  }
  if(( iRc = exp.GetStringResult( sResult )) != XB_NO_ERROR ){
    std::cout << std::endl << "[FAIL 4] " << title << std::endl;
    std::cout << "  Return Code  = [" << iRc << "]" << std::endl;
    return -1;
  }
  if( sResult != sExpectedResult ){
    std::cout << std::endl << "[FAIL 5] " << title << std::endl;
    std::cout << "  Expected Result = [" << sExpectedResult << "] Actual Result =[" << sResult.Str() << "]" << std::endl;
    #ifdef XB_DEBUG_SUPPORT
    exp.DumpTree( 1 );
    #endif  // XB_DEBUG_SUPPORT
    return -1;
  }
  if( exp.GetResultLen() != iExpectedResultLen ){
    std::cout << std::endl << "[FAIL 6] " << title << std::endl;
    std::cout << "  Expected Result Len = [" << iExpectedResultLen << "] Actual Result =[" << exp.GetResultLen() << "]" << std::endl;
    #ifdef XB_DEBUG_SUPPORT
    exp.DumpTree( 1 );
    #endif  // XB_DEBUG_SUPPORT
    return -1;
  }
  if( PrintOption == 2 )
    std::cout << "[PASS] " << title << " Expected [" << sExpectedResult << "] matches actual [" << exp.GetResultLen() << "]" << std::endl;
  else if( PrintOption == 1 )
    std::cout << "[PASS] " << title << std::endl;

  return 0;
}

/**************************************************************************/
xbInt16 TestMethod( xbXBase * xb, xbDbf * d, xbInt16 PrintOption, const char * title,
                    const xbString &sExpression, xbDouble dExpectedResult );

xbInt16 TestMethod( xbXBase * xb, xbDbf * d, xbInt16 PrintOption, const char * title,
                    const xbString &sExpression, xbDouble dExpectedResult ){

  xbDouble dResult;
  xbInt16 iRc = XB_NO_ERROR;
  xbExp exp( xb );

  if(( iRc = exp.ParseExpression( d, sExpression )) != XB_NO_ERROR ){
    std::cout << std::endl << "[FAIL 1] " << title << std::endl;
    std::cout << "  Return Code  = [" << iRc << "]" << std::endl;
    return -1;
  }
  if(( iRc = exp.GetReturnType()) != XB_EXP_NUMERIC ){
    std::cout << std::endl << "[FAIL 2] " << title << std::endl;
    std::cout << "  Result type not XB_EXP_NUMERIC  = [" << exp.GetReturnType() << "]" << std::endl;
    return -1;
  }
  if(( iRc = exp.ProcessExpression()) != XB_NO_ERROR ){
    std::cout << std::endl << "[FAIL 3] " << title << std::endl;
    std::cout << "  Return Code  = [" << iRc << "]" << std::endl;
    return -1;
  }
  if(( iRc = exp.GetNumericResult( dResult )) != XB_NO_ERROR ){
    std::cout << std::endl << "[FAIL 4] " << title << std::endl;
    std::cout << "  Return Code  = [" << iRc << "]" << std::endl;
    return -1;
  }
  if( !dblEquals( dResult, dExpectedResult, .01 )){
    std::cout << std::endl << "[FAIL 5] " << title << std::endl;
    std::cout << "  Expected Result = [" << dExpectedResult << "] Actual Result =[" << dResult << "]" << std::endl;
    // dump out the tree
    #ifdef XB_DEBUG_SUPPORT
    exp.DumpTree( 1 );
    #endif  // XB_DEBUG_SUPPORT
    return -1;
  }
  if( PrintOption == 2 )
    std::cout << "[PASS] " << title << " Expected [" << dExpectedResult << "] matches actual [" << exp.GetResultLen() << "]" << std::endl;
  else if( PrintOption == 1 )
    std::cout << "[PASS] " << title << std::endl;
  return 0;
}
/**************************************************************************/
xbInt16 TestMethod( xbXBase * xb, xbDbf * d, xbInt16 PrintOption, const char * title,
                    const xbString &sExpression, xbDate dtExpectedResult );
xbInt16 TestMethod( xbXBase * xb, xbDbf * d, xbInt16 PrintOption, const char * title,
                    const xbString &sExpression, xbDate dtExpectedResult ){

  xbDate dtResult;
  xbInt16 iRc = XB_NO_ERROR;
  xbExp exp( xb );
  if(( iRc = exp.ParseExpression( d, sExpression )) != XB_NO_ERROR ){
    std::cout << std::endl << "[FAIL 1] " << title << std::endl;
    std::cout << "  Return Code  = [" << iRc << "]" << std::endl;
    return -1;
  }
  if(( iRc = exp.GetReturnType()) != XB_EXP_DATE ){
    std::cout << std::endl << "[FAIL 2] " << title << std::endl;
    std::cout << "  Result type not XB_EXP_DATE  = [" << exp.GetReturnType() << "]" << std::endl;
    return -1;
  }
  if(( iRc = exp.ProcessExpression()) != XB_NO_ERROR ){
    std::cout << std::endl << "[FAIL 3] " << title << std::endl;
    std::cout << "  Return Code  = [" << iRc << "]" << std::endl;
    return -1;
  }
  if(( iRc = exp.GetDateResult( dtResult )) != XB_NO_ERROR ){
    std::cout << std::endl << "[FAIL 4] " << title << std::endl;
    std::cout << "  Return Code  = [" << iRc << "]" << std::endl;
    return -1;
  }
  if( dtResult != dtExpectedResult ){
    std::cout << std::endl << "[FAIL 5] " << title << std::endl;
    std::cout << "  Expected Result = [" << dtExpectedResult.Str() << "] Actual Result =[" << dtResult.Str() << "]" << std::endl;
    // dump out the tree
    #ifdef XB_DEBUG_SUPPORT
    exp.DumpTree( 1 );
    #endif  // XB_DEBUG_SUPPORT
    return -1;
  }
  if( PrintOption == 2 )
    std::cout << "[PASS] " << title << " Expected [" << dtExpectedResult.Str() << "] matches actual [" << exp.GetResultLen() << "]" << std::endl;
  else if( PrintOption == 1 )
    std::cout << "[PASS] " << title << std::endl;

  return 0;
}
/**************************************************************************/
xbInt16 TestMethod( xbXBase * xb, xbDbf * d, xbInt16 PrintOption, const char * title,
                    const xbString &sExpression, xbBool bExpectedResult );
xbInt16 TestMethod( xbXBase * xb, xbDbf * d, xbInt16 PrintOption, const char * title,
                    const xbString &sExpression, xbBool bExpectedResult ){

  xbInt16 iRc = XB_NO_ERROR;

  xbBool bResult;
  xbExp exp( xb );
  if(( iRc = exp.ParseExpression( d, sExpression )) != XB_NO_ERROR ){
    std::cout << std::endl << "[FAIL 1] " << title << std::endl;
    std::cout << "  Return Code  = [" << iRc << "]" << std::endl;
    return -1;
  }
  if(( iRc = exp.GetReturnType()) != XB_EXP_LOGICAL ){
    std::cout << std::endl << "[FAIL 2] " << title << std::endl;
    std::cout << "  Result type not XB_EXP_LOGICAL = [" << exp.GetReturnType() << "]" << std::endl;
    return -1;
  }
  if(( iRc = exp.ProcessExpression()) != XB_NO_ERROR ){
    std::cout << std::endl << "[FAIL 3] " << title << std::endl;
    std::cout << "  Return Code  = [" << iRc << "]" << std::endl;
    return -1;
  }
  if(( iRc = exp.GetBoolResult( bResult )) != XB_NO_ERROR ){
    std::cout << std::endl << "[FAIL 4] " << title << std::endl;
    std::cout << "  Return Code  = [" << iRc << "]" << std::endl;
    return -1;
  }
  if( bResult != bExpectedResult ){
    std::cout << std::endl << "[FAIL 5] " << title << std::endl;
    std::cout << "  Expected Result = [" << bExpectedResult << "] Actual Result =[" << bResult << "]" << std::endl;
    // dump out the tree
    #ifdef XB_DEBUG_SUPPORT
    exp.DumpTree( 1 );
    #endif  //XB_DEBUG_SUPPORT
    return -1;
  }
  if( PrintOption == 2 )
    std::cout << "[PASS] " << title << " Expected [" << bExpectedResult << "] matches actual [" << bResult << "]"  << std::endl;
  else if( PrintOption == 1 )
    std::cout << "[PASS] " << title << std::endl;
  return 0;
}
/**************************************************************************/
int main( int argCnt, char **av )
{
  xbInt16 iRc  = 0;
  xbInt16 rc2 = 0;
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

  xbSchema MyRecord[] = 
  {
    { "CHAR1",      XB_CHAR_FLD,      5, 0 },
    { "CHAR2",      XB_CHAR_FLD,      7, 0 },
    { "DATE1",      XB_DATE_FLD,      8, 0 },
    { "DATE2",      XB_DATE_FLD,      8, 0 },
    { "NULLDATE1",  XB_DATE_FLD,      8, 0 },
    { "NULLDATE2",  XB_DATE_FLD,      8, 0 },
    { "NUM1",       XB_NUMERIC_FLD,   9, 2 },
    { "",0,0,0 }
  };

  xbXBase x;
  xbDbf * MyFile;
  xbDate d;
  xbDate dtTest1( "19890303" );
  xbDate dtTest2( "20120708" );



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
    std::cout << "XBase Expression testing program.." << std::endl;
    std::cout << "This program tests the XBase expression logic." << std::endl;
    std::cout << "Default Data Directory is [" << x.GetDataDirectory().Str() << "]" << std::endl;
  }

  #ifdef XB_DBF4_SUPPORT
  MyFile = new xbDbf4( &x );                /* version 4 dbf file */
  #else
  MyFile = new xbDbf3( &x );                /* version 3 dbf file */
  #endif


  rc2 = MyFile->CreateTable( "ExpTest.DBF", "ExpTest", MyRecord, XB_OVERLAY, XB_MULTI_USER );

  iRc += TestMethod( po, "CreateTable()", rc2, XB_NO_ERROR );
  iRc += TestMethod( po, "PutField()",     MyFile->PutField( "CHAR1", "TEST" ),      XB_NO_ERROR );
  iRc += TestMethod( po, "PutField()",     MyFile->PutField( "CHAR2", "TEST7B" ),    XB_NO_ERROR );
  iRc += TestMethod( po, "PutField()",     MyFile->PutFloatField( "NUM1", 5 ),       XB_NO_ERROR );
  iRc += TestMethod( po, "PutField()",     MyFile->PutField( "DATE1", "19890303" ),  XB_NO_ERROR );
  iRc += TestMethod( po, "PutField()",     MyFile->PutField ( "DATE2", "20120708" ), XB_NO_ERROR );
  iRc += TestMethod( po, "AppendRecord()", MyFile->AppendRecord(),                   XB_NO_ERROR );



  iRc += TestTokenMethod( &x, po, "EOX Test1", "", "", "", '?', XB_EXP_UNKNOWN, XB_NO_ERROR, XB_END_OF_EXPRESSION );
  iRc += TestTokenMethod( &x, po, "EOX Test2 ", "  ", "", "", '?', XB_EXP_UNKNOWN, XB_NO_ERROR, XB_END_OF_EXPRESSION );
  iRc += TestTokenMethod( &x, po, "Paren Test1 ", "(SOMETOKEN)+5-100", "SOMETOKEN", "+5-100", XB_EXP_NOTROOT, XB_EXP_UNKNOWN, XB_NO_ERROR , XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Paren Test2 ", "{ANOTHERTOKEN} + 55-100", "ANOTHERTOKEN", " + 55-100", XB_EXP_NOTROOT, XB_EXP_UNKNOWN, XB_NO_ERROR, XB_NO_ERROR  );
  iRc += TestTokenMethod( &x, po, "Paren Test3 ", "{{NESTED}TOKEN} + 55-100", "{NESTED}TOKEN", " + 55-100", XB_EXP_NOTROOT, XB_EXP_UNKNOWN, XB_NO_ERROR, XB_NO_ERROR  );

  // next line generates log message
  iRc += TestTokenMethod( &x, po, "Paren Test4 ", "{{NESTED}TOKEN + 55-100", "{NESTED}TOKEN", " + 55-100", XB_EXP_NOTROOT, XB_EXP_UNKNOWN, XB_PARSE_ERROR, XB_UNBALANCED_PARENS );
  iRc += TestTokenMethod( &x, po, "Paren Test5 ", " (SOMETOKEN )+5-100", "SOMETOKEN ", "+5-100", XB_EXP_NOTROOT, XB_EXP_UNKNOWN, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Quote Test1 ", "\"SOMETOKEN\"+5-100", "SOMETOKEN", "+5-100", XB_EXP_CONSTANT, XB_EXP_CHAR,  XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Quote Test2 ", "'SOMETOKEN2'", "SOMETOKEN2", "", XB_EXP_CONSTANT, XB_EXP_CHAR,  XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Quote Test3 ", " 'SOMETOKEN3 '", "SOMETOKEN3 ", "", XB_EXP_CONSTANT, XB_EXP_CHAR, XB_NO_ERROR, XB_NO_ERROR );



  // next line generates log message

  iRc += TestTokenMethod( &x, po, "Quote Test3 ", " 'SOMETOKEN4 ", "SOMETOKEN3 ", "", XB_EXP_CONSTANT, XB_EXP_CHAR, XB_PARSE_ERROR, XB_UNBALANCED_QUOTES );


  iRc += TestTokenMethod( &x, po, "Logical Constant1 ", ".T.", "T", "", XB_EXP_CONSTANT, XB_EXP_LOGICAL, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Logical Constant2 ", ".F.", "F", "", XB_EXP_CONSTANT, XB_EXP_LOGICAL, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Logical Constant3 ", ".TRUE.", "T", "", XB_EXP_CONSTANT, XB_EXP_LOGICAL, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Logical Constant4 ", ".FALSE.", "F", "", XB_EXP_CONSTANT, XB_EXP_LOGICAL, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Logical Constant5 ", ".T. and x", "T", " and x", XB_EXP_CONSTANT, XB_EXP_LOGICAL, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Numeric Constant1 ", "123", "123", "", XB_EXP_CONSTANT, XB_EXP_NUMERIC, XB_NO_ERROR, XB_NO_ERROR );



  iRc += TestTokenMethod( &x, po, "Numeric Constant2 ", "-123", "-123", "", XB_EXP_CONSTANT, XB_EXP_NUMERIC, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Numeric Constant3 ", " - 123", "-123", "", XB_EXP_CONSTANT, XB_EXP_NUMERIC, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Numeric Constant4 ", " - .456", "-.456", "", XB_EXP_CONSTANT, XB_EXP_NUMERIC, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Numeric Constant5 ", " -.002", "-.002", "", XB_EXP_CONSTANT, XB_EXP_NUMERIC, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Numeric Constant6 ", " - .002 + 1", "-.002", " + 1", XB_EXP_CONSTANT, XB_EXP_NUMERIC, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Operator 1", "-", "-", "", XB_EXP_OPERATOR, XB_EXP_UNKNOWN, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Operator 2", "+", "+", "", XB_EXP_OPERATOR, XB_EXP_UNKNOWN, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Operator 3", "*", "*", "", XB_EXP_OPERATOR, XB_EXP_NUMERIC, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Operator 4", "/", "/", "", XB_EXP_OPERATOR, XB_EXP_NUMERIC, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Operator 5", "^", "^", "", XB_EXP_OPERATOR, XB_EXP_NUMERIC, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Operator 6", "%", "%", "", XB_EXP_OPERATOR, XB_EXP_NUMERIC, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Operator 7", "=", "=", "", XB_EXP_OPERATOR, XB_EXP_LOGICAL, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Operator 8", "<", "<", "", XB_EXP_OPERATOR, XB_EXP_LOGICAL, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Operator 9", ">", ">", "", XB_EXP_OPERATOR, XB_EXP_LOGICAL, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Operator 10", "$", "$", "", XB_EXP_OPERATOR, XB_EXP_LOGICAL, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Operator 11", "**", "**", "", XB_EXP_OPERATOR, XB_EXP_NUMERIC, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Operator 12", "++", "++", "", XB_EXP_POST_OPERATOR, XB_EXP_NUMERIC, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Operator 13", "--", "--", "", XB_EXP_POST_OPERATOR, XB_EXP_NUMERIC, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Operator 14", "-- ", "--", " ", XB_EXP_POST_OPERATOR, XB_EXP_NUMERIC, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Operator 15", "--X", "--", "X", XB_EXP_PRE_OPERATOR, XB_EXP_NUMERIC, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Operator 16", "++X", "++", "X", XB_EXP_PRE_OPERATOR, XB_EXP_NUMERIC, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Operator 17", "+=", "+=", "", XB_EXP_OPERATOR, XB_EXP_NUMERIC, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Operator 18", "-=", "-=", "", XB_EXP_OPERATOR, XB_EXP_NUMERIC, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Operator 19", "*=", "*=", "", XB_EXP_OPERATOR, XB_EXP_NUMERIC, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Operator 20", "/=", "/=", "", XB_EXP_OPERATOR, XB_EXP_NUMERIC, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Operator 21", "<>", "<>", "", XB_EXP_OPERATOR, XB_EXP_LOGICAL, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Operator 22", "<=", "<=", "", XB_EXP_OPERATOR, XB_EXP_LOGICAL, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Operator 23", ">=", ">=", "", XB_EXP_OPERATOR, XB_EXP_LOGICAL, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Operator 24", ">= grapes", ">=", " grapes", XB_EXP_OPERATOR, XB_EXP_LOGICAL, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Operator 25", "< apples ", "<", " apples ", XB_EXP_OPERATOR, XB_EXP_LOGICAL, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Operator 26", ".NOT.", ".NOT.", "", XB_EXP_OPERATOR, XB_EXP_LOGICAL, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Operator 27", ".AND.", ".AND.", "", XB_EXP_OPERATOR, XB_EXP_LOGICAL, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Operator 28", ".OR.", ".OR.", "", XB_EXP_OPERATOR, XB_EXP_LOGICAL, XB_NO_ERROR, XB_NO_ERROR );

  iRc += TestTokenMethod( &x, po, "Function 1", "STOD ( \"08252017\" )", "STOD ( \"08252017\" )", "", XB_EXP_FUNCTION, XB_EXP_DATE, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Function 2", "STR( 8 )", "STR( 8 )", "", XB_EXP_FUNCTION, XB_EXP_CHAR, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Function 3", "STR( (8-3)+3 )", "STR( (8-3)+3 )", "", XB_EXP_FUNCTION, XB_EXP_CHAR, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Function 4", "STR( (8-3)+3 ) + \"A\"", "STR( (8-3)+3 )", " + \"A\"", XB_EXP_FUNCTION, XB_EXP_CHAR, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Function 5", "ISALPHA( \"A\" )", "ISALPHA( \"A\" )", "", XB_EXP_FUNCTION, XB_EXP_LOGICAL, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Function 6", "EXP( 6 )", "EXP( 6 )", "", XB_EXP_FUNCTION, XB_EXP_NUMERIC, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Field 1", "NUM1", "NUM1", "", XB_EXP_FIELD, XB_EXP_UNKNOWN, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Field 2", "NUM1 + X", "NUM1", " + X", XB_EXP_FIELD, XB_EXP_UNKNOWN, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Field 3", "ExpTest->NUM1", "ExpTest->NUM1", "", XB_EXP_FIELD, XB_EXP_UNKNOWN, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Field 4", "ExpTest ->NUM1", "ExpTest ->NUM1", "", XB_EXP_FIELD, XB_EXP_UNKNOWN, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Field 5", "ExpTest ->  NUM1", "ExpTest ->  NUM1", "", XB_EXP_FIELD, XB_EXP_UNKNOWN, XB_NO_ERROR, XB_NO_ERROR );
  iRc += TestTokenMethod( &x, po, "Field 6", "ExpTest ->  NUM1+1", "ExpTest ->  NUM1", "+1", XB_EXP_FIELD, XB_EXP_UNKNOWN, XB_NO_ERROR, XB_NO_ERROR );

  iRc += TestWeight( &x, po, "Weight Test", "", 0 );
  iRc += TestWeight( &x, po, "Weight Test", "toobig", 0 );
  iRc += TestWeight( &x, po, ".OR. Weight Test",  ".OR.",  1 );
  iRc += TestWeight( &x, po, ".AND. Weight Test", ".AND.", 2 );
  iRc += TestWeight( &x, po, ".NOT. Weight Test", ".NOT.", 3 );
  iRc += TestWeight( &x, po, ">  Weight Test", ">",  4 );
  iRc += TestWeight( &x, po, ">= Weight Test", ">=", 4 );
  iRc += TestWeight( &x, po, "<  Weight Test", "<",  4 );
  iRc += TestWeight( &x, po, "<= Weight Test", "<=", 4 );
  iRc += TestWeight( &x, po, "<> Weight Test", "<>", 4 );
  iRc += TestWeight( &x, po, "#  Weight Test", "#",  4 );
  iRc += TestWeight( &x, po, "$  Weight Test", "$",  4 );
  iRc += TestWeight( &x, po, "=  Weight Test", "=",  4 );
  iRc += TestWeight( &x, po, "-- Prefix decrement Weight Test", "--0",  9 );
  iRc += TestWeight( &x, po, "++ Prefix increment Weight Test", "++0",  9 );
  iRc += TestWeight( &x, po, "** Weight Test", "**", 8 );
  iRc += TestWeight( &x, po, "^  Weight Test", "^",  8 );
  iRc += TestWeight( &x, po, "*  Weight Test", "*",  7 );
  iRc += TestWeight( &x, po, "*  Weight Test", "/",  7 );
  iRc += TestWeight( &x, po, "*  Weight Test", "%",  7 );
  iRc += TestWeight( &x, po, "*  Weight Test", "*=", 7 );
  iRc += TestWeight( &x, po, "*  Weight Test", "/=", 7 );
  iRc += TestWeight( &x, po, "+  Weight Test", "+",  6 );
  iRc += TestWeight( &x, po, "-  Weight Test", "-",  6 );
  iRc += TestWeight( &x, po, "+= Weight Test", "+=",  6 );
  iRc += TestWeight( &x, po, "-= Weight Test", "-=",  6 );
  iRc += TestWeight( &x, po, "-- Postfix decrement Weight Test", "--1",  5 );
  iRc += TestWeight( &x, po, "++ Postfix increment Weight Test", "++1",  5 );

  iRc += TestMethod( &x, MyFile, po, "CharTest1", "CHAR1", "TEST ", 5 );
  iRc += TestMethod( &x, MyFile, po, "CharTest2", "ExpTest->CHAR1", "TEST ", 5 );
  iRc += TestMethod( &x, MyFile, po, "CharTest3", "ExpTest->CHAR1+CHAR1", "TEST TEST ", 10 );
  iRc += TestMethod( &x, MyFile, po, "CharTest4", "\"PRE_ \"+\" _POST\"", "PRE_  _POST", 11 );
  iRc += TestMethod( &x, MyFile, po, "CharTest5", "\"PRE_ \"-\" _POST\"", "PRE_ _POST ", 11 );
  iRc += TestMethod( &x, MyFile, po, "CharTest6", "\"PRE_\"+ExpTest->CHAR1+\"_POST\"", "PRE_TEST _POST", 14 );
  iRc += TestMethod( &x, MyFile, po, "CharTest7", "STR(123)", "       123", 10 );
  iRc += TestMethod( &x, MyFile, po, "CharTest8", "STR(123)+LTRIM(STR(456))", "       123456", 20 );
  iRc += TestMethod( &x, MyFile, po, "CharTest9", "STR(8)+STR(7)", "         8         7", 20 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest1", "ALLTRIM( \"    ABCD   \" )", "ABCD", 11 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest2", "CDOW( STOD( \"20171014\" ))", "Saturday", 9 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest3", "CHR( 66 )", "B", 1 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest4", "CMONTH( STOD( \"20171114\" ) )", "November", 9 );

  // next line causes MAC compiler to cough up a furball
  //iRc += TestMethod( &x, MyFile, po, "FuncTest5", "DESCEND( \"ABCDE\" )", "¾½¼»º", 5 );

  char sDescendResult[6];  // = "¾½¼»º";
  sDescendResult[0] = (char) 0xBE;
  sDescendResult[1] = (char) 0xBD;
  sDescendResult[2] = (char) 0xBC;
  sDescendResult[3] = (char) 0xBB;
  sDescendResult[4] = (char) 0xBA;
  sDescendResult[5] = (char) 0x00;
  iRc += TestMethod( &x, MyFile, po, "FuncTest5", "DESCEND( \"ABCDE\" )", sDescendResult, 5 );

  iRc += TestMethod( &x, MyFile, po, "FuncTest6", "DTOC( STOD( \"20171114\" ))", "11/14/17", 8 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest7", "DTOS( STOD( \"20171114\" ))", "20171114", 8 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest8", "LEFT( \"ABCDEFGH\", 5 )", "ABCDE", 5 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest9", "LOWER( \"ABCDEFGH\" )", "abcdefgh", 8 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest10", "LTRIM( \"   ABC\" )", "ABC", 6 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest11", "REPLICATE( \"ABC\", 3 )", "ABCABCABC", 9 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest12", "RIGHT( \"ABCDEFGH\", 5 )", "DEFGH", 5 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest13", "RTRIM( \"ABCD   \" )", "ABCD", 7 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest14", "SPACE( 5 )", "     ", 5 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest15", "STR( 8 )", "         8", 10 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest16", "STR( 3, 4 )", "   3", 4 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest17", "STR( 12.7, 5, 2 )", "12.70", 5 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest18", "STR( 43.17, 9, 4, \"0\" )", "0043.1700", 9 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest19", "STRZERO( 56.21, 9, 4  )", "0056.2100", 9 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest20", "SUBSTR( \"SOMESTRING\", 3, 5  )", "MESTR", 5 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest21", "TRIM( \"   abc123   \" )", "   abc123", 12 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest22", "UPPER( \"abc123RRR\" )", "ABC123RRR", 9 );

  // numeric logic tests
  iRc += TestMethod( &x, MyFile, po, "NumericTest1", "1 + 1", (xbDouble) 2 );
  iRc += TestMethod( &x, MyFile, po, "NumericTest2", "6 - 1", (xbDouble) 5 );
  iRc += TestMethod( &x, MyFile, po, "NumericTest3", "6 * 4", (xbDouble) 24 );
  iRc += TestMethod( &x, MyFile, po, "NumericTest4", "8 / 2", (xbDouble) 4 );
  iRc += TestMethod( &x, MyFile, po, "NumericTest5", "3 ^ 3", (xbDouble) 27 );
  iRc += TestMethod( &x, MyFile, po, "NumericTest6", "4 ** 4", (xbDouble) 256 );
  iRc += TestMethod( &x, MyFile, po, "NumericTest7", "NUM1", (xbDouble) 5 );
  iRc += TestMethod( &x, MyFile, po, "NumericTest8", "NUM1 + 2.3", (xbDouble) 7.3 );
  iRc += TestMethod( &x, MyFile, po, "NumericTest9", "++NUM1", (xbDouble) 6 );
  iRc += TestMethod( &x, MyFile, po, "NumericTest10", "NUM1++", (xbDouble)  5 );
  iRc += TestMethod( &x, MyFile, po, "NumericTest11", "--NUM1", (xbDouble) 4 );
  iRc += TestMethod( &x, MyFile, po, "NumericTest12", "NUM1--", (xbDouble) 5 );
  iRc += TestMethod( &x, MyFile, po, "NumericTest13", "3++", (xbDouble) 3 );
  iRc += TestMethod( &x, MyFile, po, "NumericTest14", "++3", (xbDouble)  4 );
  iRc += TestMethod( &x, MyFile, po, "NumericTest15", "4--", (xbDouble) 4 );
  iRc += TestMethod( &x, MyFile, po, "NumericTest16", "--4", (xbDouble) 3 );
  iRc += TestMethod( &x, MyFile, po, "NumericTest17", "3 += 12", (xbDouble) 15 );
  iRc += TestMethod( &x, MyFile, po, "NumericTest18", "3 -= 12", (xbDouble) -9 );
  iRc += TestMethod( &x, MyFile, po, "NumericTest19", "3 *= 12", (xbDouble) 36 );
  iRc += TestMethod( &x, MyFile, po, "NumericTest20", "12 /= 3", (xbDouble) 4 );
  iRc += TestMethod( &x, MyFile, po, "NumericTest21", "(12+3)*(15-13)", (xbDouble) 30 );

  // numeric functions
  iRc += TestMethod( &x, MyFile, po, "FuncTest23", "ABS( -22 )", (xbDouble) 22 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest24", "ABS( 23 )", (xbDouble) 23 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest25", "ASC( \"A\" )", (xbDouble) 65 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest26", "AT( \",\", \"Booth, Joseph\" )", (xbDouble) 6 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest27", "DAY( STOD( \"20171017\" ))", (xbDouble) 17 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest28", "DESCEND( 1991 )", (xbDouble) -1991 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest29", "DOW( STOD( \"20171017\" ) )", (xbDouble) 3 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest30", "DOW( STOD( \"20171021\" ) )", (xbDouble) 0 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest31", "EXP( 0 )", (xbDouble) 1 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest32", "INT( 123.45 )", (xbDouble) 123 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest33", "LEN( \"ABC123\" )", (xbDouble) 6 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest34", "LOG( 10 )", (xbDouble) 2.3 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest35", "MAX( 10, 20 )", (xbDouble)  20 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest36", "MIN( 10, 20 )", (xbDouble) 10 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest37", "MONTH( STOD( \"20171017\" ))", (xbDouble) 10 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest38", "RECCOUNT()", (xbDouble) 1 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest39", "RECNO()", (xbDouble) 1 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest40", "SQRT( 9 )", (xbDouble) 3 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest41", "VAL( \"89\" )", (xbDouble) 89 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest42", "VAL( \"22.13 and some text\" )", (xbDouble) 22.13 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest43", "YEAR( STOD( \"20171017\" ))", (xbDouble) 2017 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest44", "CTOD( \"07\\08\\12\" )", dtTest2 );
  xbDate dtToday;
  dtToday.Sysdate();
  iRc += TestMethod( &x, MyFile, po, "FuncTest45", "DATE()", dtToday );
  dtTest2 = "28870625";
  iRc += TestMethod( &x, MyFile, po, "FuncTest46", "DESCEND( DATE2 )", dtTest2 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest47", "STOD( \"19890303\" )", dtTest1 );


  // date logic tests

  iRc += TestMethod( &x, MyFile, po, "DateTest1", "ExpTest->DATE1", dtTest1 );
  iRc += TestMethod( &x, MyFile, po, "DateTest2", "DATE1", dtTest1 );
  dtTest2.Set( "20120708" );
  iRc += TestMethod( &x, MyFile, po, "DateTest3", "ExpTest->DATE2", dtTest2 );
  iRc += TestMethod( &x, MyFile, po, "DateTest4", "DATE2", dtTest2 );
  iRc += TestMethod( &x, MyFile, po, "DateTest5", "DATE2 - DATE1", (xbDouble) 8528 );


  dtTest1.Set( "20120705" );

  iRc += TestMethod( &x, MyFile, po, "DateTest6", "DATE2 - 3", dtTest1 );
  iRc += TestMethod( &x, MyFile, po, "DateTest7", "DATE2 -= 3", dtTest1 );
  dtTest1.Set( "20120718" );
  iRc += TestMethod( &x, MyFile, po, "DateTest8", "DATE2 + 10", dtTest1 );
  iRc += TestMethod( &x, MyFile, po, "DateTest9", "DATE2 += 10", dtTest1 );
  dtTest1.Set( "20120709" );
  iRc += TestMethod( &x, MyFile, po, "DateTest10", "++DATE2", dtTest1 );
  dtTest1.Set( "20120707" );
  iRc += TestMethod( &x, MyFile, po, "DateTest11", "--DATE2", dtTest1 );
  dtTest1.Set( "20120708" );
  iRc += TestMethod( &x, MyFile, po, "DateTest12", "DATE2++", dtTest1 );
  iRc += TestMethod( &x, MyFile, po, "DateTest13", "DATE2--", dtTest1 );
  iRc += TestMethod( &x, MyFile, po, "DateTest14", "{07/08/12}", dtTest1 );
  iRc += TestMethod( &x, MyFile, po, "DateTest15", "{07/08/2012}", dtTest1 );
  iRc += TestMethod( &x, MyFile, po, "DateTest16", "{07/11/12} -3", dtTest1 );
  iRc += TestMethod( &x, MyFile, po, "DateTest17", "{07/06/2012} + 2", dtTest1 );

  iRc += TestMethod( &x, MyFile, po, "DateTest18", "ExpTest->NULLDATE1  = {07/06/2012}", (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "DateTest19", "ExpTest->NULLDATE1 != {07/06/2012}", (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "DateTest20", "ExpTest->NULLDATE1  < {07/06/2012}", (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "DateTest21", "ExpTest->NULLDATE1 <= {07/06/2012}", (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "DateTest22", "ExpTest->NULLDATE1  > {07/06/2012}", (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "DateTest23", "ExpTest->NULLDATE1 >= {07/06/2012}", (xbBool) xbFalse );

  iRc += TestMethod( &x, MyFile, po, "DateTest24", "{07/06/2012}  = ExpTest->NULLDATE1", (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "DateTest25", "{07/06/2012} != ExpTest->NULLDATE1", (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "DateTest26", "{07/06/2012}  < ExpTest->NULLDATE1", (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "DateTest27", "{07/06/2012} <= ExpTest->NULLDATE1", (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "DateTest28", "{07/06/2012}  > ExpTest->NULLDATE1", (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "DateTest29", "{07/06/2012} >= ExpTest->NULLDATE1", (xbBool) xbTrue );

  iRc += TestMethod( &x, MyFile, po, "DateTest18", "ExpTest->NULLDATE1  = ExpTest->NULLDATE2", (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "DateTest19", "ExpTest->NULLDATE1 != ExpTest->NULLDATE2", (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "DateTest20", "ExpTest->NULLDATE1  < ExpTest->NULLDATE2", (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "DateTest21", "ExpTest->NULLDATE1 <= ExpTest->NULLDATE2", (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "DateTest22", "ExpTest->NULLDATE1  > ExpTest->NULLDATE2", (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "DateTest23", "ExpTest->NULLDATE1 >= ExpTest->NULLDATE2", (xbBool) xbTrue );

  // boolean logic tests
  iRc += TestMethod( &x, MyFile, po, "LogicTest1", "3=5", (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest2", "5=5", (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest3", "\"abc\"=\"def\"", (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest4", "\"abc\"=\"abc\"", (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest5", "CTOD( \"07\\08\\12\" ) = CTOD ( \"07\\08\\12\" )", (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest6", "CTOD( \"07\\09\\12\" ) = CTOD ( \"07\\08\\12\" )", (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest7",  "3<>5",  (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest8",  "5<>5",  (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest9",  "3!=5",  (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest10", "5!=5",  (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest11", "3 # 5", (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest12", "5#5",   (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest13", "\"3\"<>\"5\"",  (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest14", "\"5\"<>\"5\"",  (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest15", "\"3\"!=\"5\"",  (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest16", "\"5\"!=\"5\"",  (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest17", "\"3\" # \"5\"", (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest18", "\"5\"#\"5\"",   (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest19", "CTOD( \"07\\08\\12\" ) <> CTOD ( \"07\\08\\12\" )", (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest20", "CTOD( \"07\\09\\12\" ) <> CTOD ( \"07\\08\\12\" )", (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest21", "CTOD( \"07\\08\\12\" ) != CTOD ( \"07\\08\\12\" )", (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest22", "CTOD( \"07\\09\\12\" ) != CTOD ( \"07\\08\\12\" )", (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest23", "CTOD( \"07\\08\\12\" ) #  CTOD ( \"07\\08\\12\" )", (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest24", "CTOD( \"07\\09\\12\" ) #  CTOD ( \"07\\08\\12\" )", (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest25",  "3<5",  (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest26",  "5<5",  (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest27",  "5<4",  (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest28",  "\"a\"<\"b\"",  (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest29",  "\"a\"<\"a\"",  (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest30",  "\"c\"<\"b\"",  (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest31", "CTOD( \"07\\07\\12\" ) < CTOD ( \"07\\08\\12\" )", (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest32", "CTOD( \"07\\08\\12\" ) < CTOD ( \"07\\08\\12\" )", (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest33", "CTOD( \"07\\09\\12\" ) < CTOD ( \"07\\08\\12\" )", (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest34",  "3>5",  (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest35",  "5>5",  (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest36",  "5>4",  (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest37",  "\"a\">\"b\"",  (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest38",  "\"a\">\"a\"",  (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest39",  "\"c\">\"b\"",  (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest40", "CTOD( \"07\\07\\12\" ) > CTOD ( \"07\\08\\12\" )", (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest41", "CTOD( \"07\\08\\12\" ) > CTOD ( \"07\\08\\12\" )", (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest42", "CTOD( \"07\\09\\12\" ) > CTOD ( \"07\\08\\12\" )", (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest43",  "3<=5",  (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest44",  "5<=5",  (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest45",  "5<=4",  (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest46",  "\"a\"<=\"b\"",  (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest47",  "\"a\"<=\"a\"",  (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest48",  "\"c\"<=\"b\"",  (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest49", "CTOD( \"07\\07\\12\" ) <= CTOD ( \"07\\08\\12\" )", (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest50", "CTOD( \"07\\08\\12\" ) <= CTOD ( \"07\\08\\12\" )", (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest51", "CTOD( \"07\\09\\12\" ) <= CTOD ( \"07\\08\\12\" )", (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest52",  "3>=5",  (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest53",  "5>=5",  (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest54",  "5>=4",  (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest55",  "\"a\">=\"b\"",  (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest56",  "\"a\">=\"a\"",  (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest57",  "\"c\">=\"b\"",  (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest58", "CTOD( \"07\\07\\12\" ) >= CTOD ( \"07\\08\\12\" )", (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest59", "CTOD( \"07\\08\\12\" ) >= CTOD ( \"07\\08\\12\" )", (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest60", "CTOD( \"07\\09\\12\" ) >= CTOD ( \"07\\08\\12\" )", (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest61", "\"abc123\" $ \"abc\"", (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest62", "\"abc\" $ \"abc123\"", (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest63", "\"abc\" $ \"zzabc123\"", (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest64", ".T.", (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest65", ".F.", (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest66", ".TRUE.", (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest67", ".FALSE.", (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest68", ".NOT. .F.", (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest69", " NOT  .F.", (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest70", ".NOT. .T.", (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest71", " NOT  .T.", (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest72", ".T. .AND. .T.", (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest73", ".T.  AND  .T.", (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest74", ".T. .AND. .NOT. .T.", (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest75", ".T.  AND   NOT  .T.", (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest76", ".NOT. .T. .AND. .T.", (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest77", " NOT  .T.  AND  .T.", (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest78", ".NOT. .T. .AND. .NOT. .T.", (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest79", " NOT  .T.  AND   NOT .T.", (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest80", ".T. .OR. .T.", (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest81", ".T.  OR  .T.", (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest82", ".T. .OR. .NOT. .T.", (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest83", ".T.  OR   NOT  .T.", (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest84", ".NOT. .T. .OR. .T.", (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest85", " NOT  .T.  OR  .T.", (xbBool) xbTrue );
  iRc += TestMethod( &x, MyFile, po, "LogicTest86", ".NOT. .T. .OR. .NOT. .T.", (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "LogicTest87", " NOT  .T.  OR   NOT  .T.", (xbBool) xbFalse );

  iRc += TestMethod( &x, MyFile, po, "FuncTest48", "ISALPHA( \"12345\" )",  (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "FuncTest49", "ISALPHA( \"ABCDEF\" )", (xbBool) xbTrue  );
  iRc += TestMethod( &x, MyFile, po, "FuncTest50", "ISALPHA( \"A1234\" )",  (xbBool) xbTrue  );
  iRc += TestMethod( &x, MyFile, po, "FuncTest51", "ISLOWER( \"12345\" )",   (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "FuncTest52", "ISLOWER( \"ABCDEF\" )",  (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "FuncTest53", "ISLOWER( \"abc123\" )",  (xbBool) xbTrue  );
  iRc += TestMethod( &x, MyFile, po, "FuncTest54", "ISLOWER( \"xyz\" )",     (xbBool) xbTrue  );
  iRc += TestMethod( &x, MyFile, po, "FuncTest55", "ISUPPER( \"12345\" )",   (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "FuncTest56", "ISUPPER( \"ABCDEF\" )",  (xbBool) xbTrue  );
  iRc += TestMethod( &x, MyFile, po, "FuncTest57", "ISUPPER( \"abc123\" )",  (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "FuncTest58", "ISUPPER( \"xyz\" )",     (xbBool) xbFalse );
  iRc += TestMethod( &x, MyFile, po, "FuncTest59", "ISUPPER( \"Xyz\" )",     (xbBool) xbTrue  );
  iRc += TestMethod( &x, MyFile, po, "FuncTest60", "IIF( .T., \"TRUE \", \"FALSE\" )", "TRUE ", 5 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest61", "IIF( .F., \"TRUE \", \"FALSE\" )", "FALSE", 5 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest62", "DEL()", " ", 1 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest63", "DELETED()", (xbBool) xbFalse );
  iRc += TestMethod( po, "DeleteRecord()", MyFile->DeleteRecord(),           XB_NO_ERROR );
  iRc += TestMethod( &x, MyFile, po, "FuncTest64", "DEL()", "*", 1 );
  iRc += TestMethod( &x, MyFile, po, "FuncTest65", "DELETED()", (xbBool) xbTrue );
  iRc += TestMethod( po, "UndeleteRecord()", MyFile->UndeleteRecord(),       XB_NO_ERROR );


  iRc += TestMethod( po, "Close()", MyFile->Close(), XB_NO_ERROR );
  delete MyFile;

  if( po > 0 || iRc < 0 )
    fprintf( stdout, "Total Errors = %d\n", iRc * -1 );

  #ifdef XB_LOGGING_SUPPORT
  sMsg.Sprintf( "Program [%s] terminating with [%d] errors...", av[0], iRc * -1 );
  x.WriteLogMessage( sMsg );
  #endif

  return iRc;
}
