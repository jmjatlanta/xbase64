/*  xbexp.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2017,2021,2022,2023 Gary A Kunkel

The xb64 software library is covered under 
the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net


This module is part of the expression logic and has the code
for parsing various tokens out of an expression

*/

#include "xbase.h"

#ifdef XB_EXPRESSION_SUPPORT

namespace xb{

/*************************************************************************/
//! Constructor
/*!
  \param x Pointer to xbXBase instance.
*/

xbExp::xbExp( xbXBase *x ){
   xbase   = x;
   dbf     = NULL;
   nTree   = NULL;
}

/*************************************************************************/
//! Constructor
/*!
  \param x Pointer to xbXBase instance.
  \param d Pointer to xbDbf instance.
*/
xbExp::xbExp( xbXBase *x, xbDbf *d ){
   xbase   = x;
   dbf     = d;
   nTree   = NULL;
}

/*************************************************************************/
//! Deconstrucor.

xbExp::~xbExp() {

  if( nTree )
    delete nTree;
}

/*************************************************************************/
//! Calulate expression return length
/*!

   This function returns the maximum possible length of an expression
   The create index functions use this for determining the fixed length keys 
   It sets the return length field in the node.

  \param n Start node
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbExp::CalcFunctionResultLen( xbExpNode * n ) const{

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbInt16 iReturnLenCalc = 0;;
  xbInt32 lReturnLenVal = 0;
  xbString sNodeText;


  try{

    n->GetNodeText( sNodeText );
    char cReturnType = 0;
    if(( iRc = xbase->GetFunctionInfo( sNodeText, cReturnType, iReturnLenCalc, lReturnLenVal )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }

    if( iReturnLenCalc == 1 ){
      // use the value from iReturnLenVal
      n->SetResultLen( (xbUInt32) lReturnLenVal );
    }
    else if( iReturnLenCalc == 2 ){
      // use the length from the child node identified in lReturnLenVal
      xbExpNode *nChild = n->GetChild( (xbUInt32) lReturnLenVal - 1 );
      if( !nChild ){
        iErrorStop = 110;
        iRc = XB_PARSE_ERROR;
        throw iRc;
      }
      n->SetResultLen( nChild->GetResultLen());
    }

    else if( iReturnLenCalc == 3 ){
      // use the length from the child node identified in lReturnLenVal
      xbExpNode *nChild = n->GetChild( (xbUInt32) lReturnLenVal - 1 );
      if( !nChild ){
        iErrorStop = 120;
        iRc = XB_PARSE_ERROR;
        throw iRc;
      }
      n->SetResultLen( (xbUInt32) nChild->GetNumericResult());
    }
    else if( iReturnLenCalc == 4 ){
      // use the value from the length in parm 1 multiplied by the value in parm 2 (REPLICATE)
      xbExpNode *nChild1 = n->GetChild( 0 );
      xbExpNode *nChild2 = n->GetChild( 1 );
      if( !nChild1 || !nChild2 ){
        iErrorStop = 130;
        iRc = XB_PARSE_ERROR;
        throw iRc;
      }
      n->SetResultLen( nChild1->GetResultLen() * (xbUInt32) nChild2->GetNumericResult());
    }
    else if( iReturnLenCalc == 5 ){
      // use the larger of the length of the value in parm2 or parm 3  (IIF statement)
      xbExpNode *nChild2 = n->GetChild( 1 );
      xbExpNode *nChild3 = n->GetChild( 2 );
      if( !nChild2 || !nChild3 ){
        iErrorStop = 140;
        iRc = XB_PARSE_ERROR;
        throw iRc;
      }
      if( nChild2->GetResultLen() >= nChild3->GetResultLen())
        n->SetResultLen( nChild2->GetResultLen());
      else
        n->SetResultLen( nChild3->GetResultLen());
    }

    else if( iReturnLenCalc == 6 ){

      if( n->GetChildCnt() >= 2 ){
        xbExpNode *nChild2 = n->GetChild( 1 );
        n->SetResultLen( (xbUInt32) nChild2->GetNumericResult());
      } else {
        n->SetResultLen( (xbUInt32) lReturnLenVal );
      }

    } else {
      iErrorStop = 150;
      iRc = XB_PARSE_ERROR;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbexp::GetFunctionResultLen() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
  }
  return iRc;
}

/*************************************************************************/
//! Check parens and quotes
/*!
   This routine looks for unbalanced parens and quotes

  \param sExpression Expression to examine.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/


xbInt16 xbExp::CheckParensAndQuotes( const xbString &sExpression ){


  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;

  xbBool  bInQuotes = xbFalse;
  xbInt16 iLparenCtr = 0;
  xbInt16 iRparenCtr = 0;
  xbInt16 iQuoteType = 0;
  const char *s = sExpression;

  try{

    while( *s ){
      if( !bInQuotes ){
        if( *s == '(' ){
          iLparenCtr++;
        } else if( *s == ')' ){
          iRparenCtr++;
        } else if( *s == '\'' ){
          bInQuotes++;
          iQuoteType = 0;
        } else if( *s == '"' ){
          bInQuotes++;
          iQuoteType = 1;
        }
      } else {
        if(( *s == '\'' && iQuoteType == 0 ) || (*s == '"' && iQuoteType == 1 ))
          bInQuotes--;
      }
      s++;
    }
    if( iLparenCtr != iRparenCtr ){
      iErrorStop = 100;
      iRc = XB_UNBALANCED_PARENS;
      throw iRc;
    }
    if( bInQuotes ){
      iErrorStop = 110;
      iRc = XB_UNBALANCED_QUOTES;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbexp::CheckParensAndQuots() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( sExpression );
  }
  return iRc;
}
/*************************************************************************/
//! Clear tree handle.
/*!
   This routine clears the expression tree and frees any associated memory.
  \returns void.
*/

void xbExp::ClearTreeHandle(){
  if( nTree ){
    nTree = NULL;
  }
}

/*************************************************************************/
#ifdef XB_DEBUG_SUPPORT
//! Dump the tree.
/*!
  \param iOption - Output opton.
  \returns void.
*/

void xbExp::DumpTree( xbInt16 iOption ){
  nTree->DumpNode( iOption );
}

//! Dump token
/*!
  \param iOption - Output opton.
  \returns void.
*/


void xbExp::DumpToken( xbExpToken &t, xbInt16 iOption ){

  xbString sMsg;
  sMsg = "Processing Token";
  xbase->WriteLogMessage( sMsg.Str(), iOption );

  sMsg.Sprintf( "Expression     = [%s]", t.sExpression.Str());
  xbase->WriteLogMessage( sMsg.Str(), iOption );

  sMsg.Sprintf( "Token          = [%s]", t.sToken.Str());
  xbase->WriteLogMessage( sMsg.Str(), iOption );

  sMsg.Sprintf( "NodeType       = [%c]", t.cNodeType );
  xbase->WriteLogMessage( sMsg.Str(), iOption );

  sMsg.Sprintf( "ReturnType     = [%c]", t.cReturnType );
  xbase->WriteLogMessage( sMsg.Str(), iOption );

  sMsg.Sprintf( "Sts            = [%d]", t.iSts );
  xbase->WriteLogMessage( sMsg.Str(), iOption );

  sMsg.Sprintf( "PrevNodeType   = [%c]", t.cPrevNodeType );
  xbase->WriteLogMessage( sMsg.Str(), iOption );

  sMsg.Sprintf( "PrevReturnType = [%c]", t.cPrevReturnType );
  xbase->WriteLogMessage( sMsg.Str(), iOption );
}

#endif

/*************************************************************************/
//! Get date result.
/*!
  If the expression generates a date return type, this method retrieves the date value.
  \param dtResult - Output date value.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbExp::GetDateResult( xbDate &dtResult ){
  if( nTree ){
    dtResult.JulToDate8( (xbInt32) nTree->GetNumericResult() );
    return XB_NO_ERROR;
  }
  else{
    //dtResult = ?;
    return XB_PARSE_ERROR;
  }
}
/*************************************************************************/
//! Get bool result.
/*!
  If the expression generates a boolean return type, this method retrieves the boolean value.
  \param bResult - Output boolean value.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbExp::GetBoolResult( xbBool &bResult){
  if( nTree ){
    bResult = nTree->GetBoolResult();
    return XB_NO_ERROR;
  }
  else{
    return XB_PARSE_ERROR;
  }
}

/*************************************************************************/
//! Get the next node in the tree.
/*!
  \param n Node to starting point.  To get the first node of the entire tree, set n = NULL
  \returns Pointer to next node.
*/

xbExpNode *xbExp::GetNextNode( xbExpNode * n ) const {

  // to get the first node of the entire tree, set n = NULL
  // std::cout << "In GetNextNode\n";

  if( n == nTree )
    return NULL;

  else if( !n ){
    if( !nTree )
      return NULL;
    else
      return nTree->GetFirstNode();
  }
  return n->GetNextNode();
}

/*************************************************************************/
//! GetNextToken
/*! This method returns the next token in an expression of one or more tokens
  \param t Token
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbExp::GetNextToken( xbExpToken &t  ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;

  try{
    t.iSts = XB_NO_ERROR;
    t.sExpression.Ltrim();

    if( t.sExpression.Len() == 0 ){
      t.iSts = XB_END_OF_EXPRESSION;
      return XB_NO_ERROR;
    }

    // Check for date constant
    if((t.sExpression.Len() >= 10 && t.sExpression[1] == '{' && t.sExpression[4] == '/' && t.sExpression[7] == '/') &&
       (t.sExpression[10] == '}' || (t.sExpression.Len() >= 12 && t.sExpression[12] == '}'))){
      if(( iRc = GetTokenDateConstant( t )) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      }
    }
    // Check for parens
    else if( t.sExpression[1] == '(' || t.sExpression[1] == '{' ){
      if(( iRc = GetTokenParen( t )) != XB_NO_ERROR ){
        iErrorStop = 110;
        throw iRc;
      }
    }
    // Check for a char constant
    else if( t.sExpression[1] == '"' || t.sExpression[1] == '\'' ){
      if(( iRc = GetTokenCharConstant( t )) != XB_NO_ERROR ){
        iErrorStop = 120;
        iRc = XB_PARSE_ERROR;
        throw iRc;
      }
    }
    // Check for logical constant
    else if( IsLogicalConstant( t.sExpression )){
      if(( iRc = GetTokenLogicalConstant( t )) != XB_NO_ERROR ){
        iErrorStop = 130;
        iRc = XB_PARSE_ERROR;
        throw iRc;
      }
    }
    // check for numeric constant
    else if( IsNumericConstant( t.sExpression, t.cPrevNodeType )){
      if(( iRc = GetTokenNumericConstant( t )) != XB_NO_ERROR ){
        iErrorStop = 140;
        iRc = XB_PARSE_ERROR;
        throw iRc;
      }
    }
    // check for operator
    else if( IsOperator( t.sExpression )){
      if(( iRc = GetTokenOperator( t )) != XB_NO_ERROR ){
        iErrorStop = 150;
        iRc = XB_PARSE_ERROR;
        throw iRc;
      }
    }
    // check for function
    else if( IsFunction( t.sExpression, t.cReturnType )){
      if(( iRc = GetTokenFunction( t )) != XB_NO_ERROR ){
        iErrorStop = 160;
        iRc = XB_PARSE_ERROR;
        throw iRc;
      }
    }
    else if(( iRc = GetTokenDatabaseField( t )) != XB_NO_ERROR ){
      iErrorStop = 170;
      iRc = XB_PARSE_ERROR;
      throw iRc;
    }
  }

  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbexp::GetNextToken() Exception Caught. Error Stop = [%d] iRc = [%d] Expression = [%s]", iErrorStop, iRc, t.sExpression.Str() );sMsg.Sprintf( "xbexp::GetNextToken() Exception Caught. Error Stop = [%d] iRc = [%d] Expression = [%s]", iErrorStop, iRc, t.sExpression.Str() );sMsg.Sprintf( "xbexp::GetNextToken() Exception Caught. Error Stop = [%d] iRc = [%d] Expression = [%s]", iErrorStop, iRc, t.sExpression.Str() );sMsg.Sprintf( "xbexp::GetNextToken() Exception Caught. Error Stop = [%d] iRc = [%d] Expression = [%s]", iErrorStop, iRc, t.sExpression.Str() );
    xbase->WriteLogMessage( sMsg.Str() );
  }
  return iRc;
}

/*************************************************************************/
//! Get numeric result.
/*!
  If the expression generates a numeric return type, this method retrieves the numeric value.
  \param dResult - Output numeric value.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbExp::GetNumericResult( xbDouble &dResult){
  if( nTree ){
    dResult = nTree->GetNumericResult();
    return XB_NO_ERROR;
  }
  else{
    dResult = 0;
    return XB_PARSE_ERROR;
  }
}
/*************************************************************************/
//! Get result length.
/*!
  This routine returns the result length.
  \returns Result length.
*/

xbInt16 xbExp::GetResultLen() const{
  if( nTree )
    return nTree->GetResultLen();
  else
    return 0;
}

/*************************************************************************/
//! Get return type.
/*!
  \returns Expression return type.
*/

char xbExp::GetReturnType() const{
  if( nTree )
    return nTree->GetReturnType();
  else
    return ' ';
}

/*************************************************************************/
//! Get string result.
/*!
  If the expression generates a string return type, this method retrieves the string value.
  \param sResult - Output string value.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbExp::GetStringResult( xbString &sResult){
  if( nTree ){
    sResult = nTree->GetStringResult();
    return XB_NO_ERROR;
  }
  else{
    sResult = "";
    return XB_PARSE_ERROR;
  }
}

/*************************************************************************/
//! Get string result.
/*!
  If the expression generates a string return type, this method retrieves the string value.
  \param vpResult - Pointer to user supplied buffer for result.
  \param ulLen - Max size of buffer.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/


xbInt16 xbExp::GetStringResult( char * vpResult, xbUInt32 ulLen ){
  if( nTree ){
    nTree->GetStringResult().strncpy((char *) vpResult, ulLen );
    return XB_NO_ERROR;
  }
  else{
    return XB_PARSE_ERROR;
  }
}



/*************************************************************************/
//! GetTokenCharConstant
/*! This method returns the character constant in a pair of quotes

   This routine returns the tokens inside a set of matching quotes in sOutToken
   If there is nothing between the quotes then sOutToken is returned empty
   sOutRemainder contains whatever remains to the right of the right quote

  \param t Token
  \returns <a href="xbretcod_8h.html">Return Codes</a>

*/

xbInt16 xbExp::GetTokenCharConstant( xbExpToken &t ){

  const char *s = t.sExpression;
  const char *sToken;             // pointer to beginning of token
  xbInt16    iQuoteType;
  xbUInt32   ulTokenLen  = 0;
  xbInt16    iRc        = XB_NO_ERROR;
  xbInt16    iErrorStop = 0;
  xbBool     bDone = xbFalse;

  try{
    if( *s == '"' ) 
      iQuoteType = 0; 
    else 
      iQuoteType = 1;
    s++;
    sToken = s;
    while( *s && !bDone ){
      if(( *s == '"' && iQuoteType == 0 ) || (*s == '\'' && iQuoteType == 1 ))
        bDone = xbTrue;
      s++;
      ulTokenLen++;
    }
    if( bDone ){    // found matching paren
      t.cNodeType   = XB_EXP_CONSTANT;
      t.cReturnType = XB_EXP_CHAR;
      t.sToken.Set( sToken, ulTokenLen - 1 );
      t.sExpression.Ltrunc( ulTokenLen + 1 );
    } else {
      iRc = XB_PARSE_ERROR;
      t.iSts = XB_UNBALANCED_QUOTES;
      iErrorStop = 100;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbexp::GetTokenCharConstant() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
  }
  return iRc;
}


/*************************************************************************/
//! GetTokenDateConstant
/*! This method returns the date constant in a pair of {}

    Date format is one of {mm/dd/yy}   or  {mm/dd/yyyy}
    \param t Token.
    \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbExp::GetTokenDateConstant( xbExpToken &t ){

  xbInt16    iRc = XB_NO_ERROR;
  xbInt16    iErrorStop = 0;
  char       wBuf[13];
  xbDate     dt;

  try{
    memset( wBuf, 0x00, 13 );
    t.cNodeType   = XB_EXP_CONSTANT;
    t.cReturnType = XB_EXP_DATE;

    if( t.sExpression[10] == '}' ){
      for( xbInt16 i = 0; i < 8; i++ )
        wBuf[i] = t.sExpression[i+2];

      if(( iRc = dt.CTOD( wBuf )) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      }
      t.sToken.Set( dt.Str() );
      t.sExpression.Ltrunc( 10 );

    } else if( t.sExpression[12] == '}' ){

      wBuf[0] = t.sExpression[8];
      wBuf[1] = t.sExpression[9];
      wBuf[2] = t.sExpression[10];
      wBuf[3] = t.sExpression[11];
      wBuf[4] = t.sExpression[2];
      wBuf[5] = t.sExpression[3];
      wBuf[6] = t.sExpression[5];
      wBuf[7] = t.sExpression[6];

      t.sToken.Set( wBuf );
      t.sExpression.Ltrunc( 12 );
    } else {
      iRc = XB_PARSE_ERROR;
      t.iSts = XB_INVALID_DATE;
      iErrorStop = 110;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbexp::GetTokenDateConstant() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
  }
  return iRc;
}
/*************************************************************************/
//! GetTokenField
/*! This method gets a database field token 

   Looks for a xbase field in one of the following formats

   FIELDNAME
     or
   TABLENAME->FIELDNAME

   \param t Token.
   \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbExp::GetTokenDatabaseField( xbExpToken &t ){

  const  char *s = t.sExpression;
  xbUInt32 ulTokenLen  = 0;
  xbUInt32 ulTokenLen2 = 0;

  while( *s && !IsTokenSeparator( *s ) && !IsWhiteSpace( *s )) {
    ulTokenLen++;
    s++;
  }

  // go past any white space
  while( *s && !IsTokenSeparator( *s ) && IsWhiteSpace( *s )) {
    ulTokenLen2++;
    s++;
  }

  // look for ->
  // remove the table name from before the ->
  if( strncmp( s, "->", 2 ) == 0 ){
    ulTokenLen2+=2;
    s+=2;

/*
  if( strncmp( s, "->", 2 ) == 0 || strncmp( s, ".", 1 ) == 0){
    if( *s == '.' ){
      ulTokenLen2+=1;
      s+=1;
    } else {
      ulTokenLen2+=2;
      s+=2;
    }
*/

    // go past white space
    while( *s && !IsTokenSeparator( *s ) && IsWhiteSpace( *s )) {
      ulTokenLen2++;
      s++;
    }
    // go to the end
    while( *s && !IsTokenSeparator( *s ) && !IsWhiteSpace( *s )) {
      ulTokenLen2++;
      s++;
    }
    ulTokenLen += ulTokenLen2;
  }
  t.cNodeType   = XB_EXP_FIELD;
  t.cReturnType = XB_EXP_UNKNOWN;
  t.sToken.Set( t.sExpression, ulTokenLen  );
  t.sExpression.Ltrunc( ulTokenLen  );

  return XB_NO_ERROR;
}

/*************************************************************************/
//! GetTokenFunction
/*! 
   This method gets a function and everything between the following quotes
   \param t Token
   \returns <a href="xbretcod_8h.html">Return Codes</a>

*/

xbInt16 xbExp::GetTokenFunction( xbExpToken &t ){

  xbUInt32 lPos  = t.sExpression.Pos( '(' );
  if( lPos == 0 )
    return XB_PARSE_ERROR;

  xbBool   bDone = xbFalse;
  xbUInt32 lLen = t.sExpression.Len();
  xbInt16  iDepthCtr = 1;

  while( ++lPos <= lLen && !bDone ){
    if( t.sExpression[lPos] == ')' ){
      iDepthCtr--;
      if( iDepthCtr == 0 )
        bDone = xbTrue;
    } else if( t.sExpression[lPos] == '(' ){
      iDepthCtr++;
    }
  }

  t.cNodeType   = XB_EXP_FUNCTION;
  t.sToken.Set( t.sExpression, lPos-1 );
  t.sExpression.Ltrunc( lPos-1 );

//  std::cout << "lPos = [" << lPos << "] done= [" << bDone << "][" << t.sExpression << "] len=[" << lLen << "] return type = [" << t.cReturnType << "]\n";

  return XB_NO_ERROR;
}

/*************************************************************************/
//! GetTokenCharConstant
/*! This method returns the character constant in a pair of quotes

   This routine returns the tokens inside a set of matching quotes in sOutToken
   If there is nothing between the quotes then sOutToken is returned empty
   sOutRemainder contains whatever remains to the right of the right quote

   \param t Token
   \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbExp::GetTokenLogicalConstant( xbExpToken &t ){

  t.cNodeType   = XB_EXP_CONSTANT;
  t.cReturnType = XB_EXP_LOGICAL;
  t.sToken      = t.sExpression[2];

  if( t.sExpression[3] == '.' )
    t.sExpression.Ltrunc( 3 );
  else if( t.sExpression[6] == '.' )
    t.sExpression.Ltrunc( 6 );
  else if( t.sExpression[7] == '.' )
    t.sExpression.Ltrunc( 7 );

  return XB_NO_ERROR;
}


/*************************************************************************/
//! GetTokenNumericConstant
/*! This method returns a numeric constant in 

   This routine returns a numeric constant token
   sOutRemainder contains whatever remains to the right of the right quote

   \param t Token
   \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbExp::GetTokenNumericConstant( xbExpToken &t ){

  const char * s      = t.sExpression;
  xbUInt32 ulTokenLen = 0;
  t.sToken            = "";

  t.cNodeType   = XB_EXP_CONSTANT;
  t.cReturnType = XB_EXP_NUMERIC;

  //  capture the leading sign
  if( *s == '-' || *s == '+' || *s == '.' ){
    t.sToken = *s;
    ulTokenLen++;
    s++;

    // go past any white space between sign and number
    while( *s && IsWhiteSpace( *s )){
      s++;
      ulTokenLen++;
    }
  }

  // add the number to the token
  while( *s && (isdigit( *s ) || *s == '.' )){
    t.sToken += *s;
    s++;
    ulTokenLen++;
  }
  t.sExpression.Ltrunc( ulTokenLen );
  return XB_NO_ERROR;
}

/*************************************************************************/
//! GetTokenOperator
/*! This method returns the operator

   \param t Token
   \returns <a href="xbretcod_8h.html">Return Codes</a>

*/

xbInt16 xbExp::GetTokenOperator( xbExpToken &t ){

  const char *s = t.sExpression;

  // Logical operators
  if((strncmp( s, "<>", 2 ) == 0 ) || (strncmp( s, "!=", 2 ) == 0 ) ||
     (strncmp( s, "<=", 2 ) == 0 ) || (strncmp( s, ">=", 2 ) == 0 )){
    t.cReturnType = XB_EXP_LOGICAL;
    t.sToken.Assign( s, 1, 2 );
    t.sExpression.Ltrunc( 2 );
    t.cNodeType   = XB_EXP_OPERATOR;
    return XB_NO_ERROR;
  }

  if( *s == '=' || *s == '<' || *s == '>' || *s == '$' || *s == '#' ){
    t.cReturnType = XB_EXP_LOGICAL;
    t.sToken.Assign( s, 1, 1 );
    t.sExpression.Ltrunc( 1 );
    t.cNodeType   = XB_EXP_OPERATOR;
    return XB_NO_ERROR;
  }

  if( (strncmp( s, ".NOT.", 5 ) == 0 ) || (strncmp( s, ".AND.", 5 ) == 0 )){
    t.cReturnType = XB_EXP_LOGICAL;
    t.sToken.Assign( s, 1, 5 );
    t.sExpression.Ltrunc( 5 );
    t.cNodeType   = XB_EXP_OPERATOR;
    return XB_NO_ERROR;
  }

  if( (strncmp( s, "NOT ", 4 ) == 0 ) || (strncmp( s, "AND ", 4 ) == 0 )){
    t.cReturnType = XB_EXP_LOGICAL;
    t.sToken.Assign( s, 1, 3 );
    t.sExpression.Ltrunc( 3 );
    t.cNodeType   = XB_EXP_OPERATOR;
    return XB_NO_ERROR;
  }

  if( strncmp( s, ".OR.", 4 ) == 0 ) {
    t.cReturnType = XB_EXP_LOGICAL;
    t.sToken.Assign( s, 1, 4 );
    t.sExpression.Ltrunc( 4 );
    t.cNodeType   = XB_EXP_OPERATOR;
    return XB_NO_ERROR;
  }

  if( strncmp( s, "OR ", 3 ) == 0 ) {
    t.cReturnType = XB_EXP_LOGICAL;
    t.sToken.Assign( s, 1, 2 );
    t.sExpression.Ltrunc( 2 );
    t.cNodeType   = XB_EXP_OPERATOR;
    return XB_NO_ERROR;
  }

  // Numeric operators
  if(( strncmp( s, "**", 2 ) == 0 ) || ( strncmp( s, "+=", 2 ) == 0 ) ||
     ( strncmp( s, "-=", 2 ) == 0 ) || ( strncmp( s, "*=", 2 ) == 0 ) || ( strncmp( s, "/=", 2 ) == 0 )){
    t.cReturnType = XB_EXP_NUMERIC;
    t.sToken.Assign( s, 1, 2 );
    t.sExpression.Ltrunc( 2 );
    t.cNodeType   = XB_EXP_OPERATOR;
    return XB_NO_ERROR;
  }

  // Pre/post increment/decrement operators   ++ or --
  if(( strncmp( s, "--", 2 ) == 0 ) || ( strncmp( s, "++", 2 ) == 0 )){
    t.cReturnType = XB_EXP_NUMERIC;
    t.sToken.Assign( s, 1, 2 );
    t.sExpression.Ltrunc( 2 );
    if( t.sExpression.Len() > 0 && (isdigit( t.sExpression[1] ) || isalpha( t.sExpression[1] )))
      t.cNodeType    = XB_EXP_PRE_OPERATOR;
    else
      t.cNodeType    = XB_EXP_POST_OPERATOR;

    return XB_NO_ERROR;
  }

  if( *s == '*' || *s == '/' || *s == '%' || *s == '^' ){
    t.cReturnType = XB_EXP_NUMERIC;
    t.sToken.Assign( s, 1, 1 );
    t.sExpression.Ltrunc( 1 );
    t.cNodeType   = XB_EXP_OPERATOR;
    return XB_NO_ERROR;
  }

  // multi return type operators
  t.cReturnType = XB_EXP_UNKNOWN;
  if( *s == '+' || *s == '-' ){
    t.sToken.Assign( s, 1, 1 );
    t.sExpression.Ltrunc( 1 );
    t.cNodeType   = XB_EXP_OPERATOR;
    return XB_NO_ERROR;
  }
  return XB_PARSE_ERROR;
}

/*************************************************************************/
//! GetTokenParen
/*! This method returns the tokens in a pair of enclosed parens

   This routine returns the tokens inside a set of matching parens in sOutToken
   If there is nothing between the parens then sOutToken is returned empty
   sOutRemainder contains whatever remains to the right of the right paren

   \param t Token
   \returns <a href="xbretcod_8h.html">Return Codes</a>
*/


xbInt16 xbExp::GetTokenParen( xbExpToken &t ){

  const char * s     = t.sExpression;
  const char * sToken;             // pointer to beginning of token
  xbInt16  iParenType = 0;
  xbInt16  iRc        = XB_NO_ERROR;
  xbInt16  iErrorStop = 0;
  xbInt16  iDepthCtr  = 0;          // depth inside of nested parens
  xbUInt32 ulTokenLen = 0;
  xbBool   bDone      = xbFalse;

  try{
    if( *s == '{' ) 
      iParenType = 0; 
    else 
      iParenType = 1;
    iDepthCtr = 1;
    s++;
    sToken = s;

    while( *s && !bDone ){
      if(( *s == ')' && iParenType == 1 ) || (*s == '}' && iParenType == 0 )){
        iDepthCtr--;
        if( iDepthCtr == 0 )
          bDone = xbTrue;
      } else if(( *s == '(' && iParenType == 1 ) || (*s == '{' && iParenType == 0 )){
          iDepthCtr++;
      }
      s++;
      ulTokenLen++;
    }

    if( bDone ){  // found matching paren
      t.cNodeType   = XB_EXP_NOTROOT;
      t.cReturnType = XB_EXP_UNKNOWN;
      t.sToken.Set( sToken, ulTokenLen - 1 );
      t.sExpression.Ltrunc( ulTokenLen + 1 );
    } else {
      t.sToken      = "";
      t.cNodeType   = XB_EXP_NOTROOT;
      t.cReturnType = XB_EXP_UNKNOWN;
      t.iSts        = XB_UNBALANCED_PARENS;
      iRc           = XB_PARSE_ERROR;
      iErrorStop    = 100;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbexp::GetTokenParen() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
  }
  return iRc;
}

/*************************************************************************/
//! Get the expression tree handle.
/*!
  \returns Pointer to the top most node in the expression tree.
*/
xbExpNode *xbExp::GetTreeHandle(){
  return nTree;
}

/*************************************************************************/
//! Is Function
/*! This method determines if the next token is a function.

   \param sExpression - String expression to be evaluated.
   \param cReturnType Output - Return type.
   \returns xbTrue - Is a function.<br>
            xbFalse - Is not a function.
*/

xbBool xbExp::IsFunction( const xbString & sExpression, char &cReturnType ){

  xbInt16 i = 0;
  xbInt32 l = 0;
  if( sExpression.Pos( '(' ) > 0 ){
    if( xbase->GetFunctionInfo( sExpression, cReturnType, i, l ) == XB_NO_ERROR )
      return xbTrue;
  }
  return xbFalse;
}

/*************************************************************************/
//! Is Logical constant
/*! This method determines if the next token is a logical constant (T/F, etc).

   \param sExpression - String expression to be evaluated.
   \returns xbTrue - Is a logical constant.<br>
            xbFalse - Is not a logical constant.
*/

xbBool xbExp::IsLogicalConstant( const xbString & sExpression ){

  const char *s = sExpression;
  if(( strncmp( s, ".T.", 3 ) == 0 ) || ( strncmp( s, ".F.", 3 ) == 0 ))
    return xbTrue;
  else if( strncmp( s, ".TRUE.", 6 ) == 0 )
    return xbTrue;
  else if( strncmp( s, ".FALSE.", 7 ) == 0 )
    return xbTrue;

  return xbFalse;
}

/*************************************************************************/
//! Is Numeric constant
/*! This method determines if the next token is a numeric constant.

   \param sExpression - String expression to be evaluated.
   \param cPrevNodeType - Type of previous node.
   \returns xbTrue - Is a numeric constant.<br>
            xbFalse - Is not a numeric constant.
*/
xbBool xbExp::IsNumericConstant( const xbString & sExpression, char cPrevNodeType ){

  // check for positive, negative or decimal number constants 

  const char *s = sExpression;
  if(( *s == '-' && ( cPrevNodeType == 'O' || cPrevNodeType == 0 )) ||
     ( *s == '+' && ( cPrevNodeType == 'O' || cPrevNodeType == 0 ))){
    s++;
    while( *s && IsWhiteSpace( *s ))
      s++;
  }
  if( *s == '.' )
    s++;

  if( isdigit( *s ))
    return xbTrue;
  else
    return xbFalse;
}

/*************************************************************************/
//! Is Operator.
/*! This method determines if the next token is an operator.

   \param sExpression - String expression to be evaluated.
   \returns xbTrue - Is an operator.<br>
            xbFalse - Is not an operator.
*/
xbBool xbExp::IsOperator( const xbString & sExpression ){

  const char *s = sExpression;
  if( *s == '+' || *s == '-' || *s == '/' || *s == '^' || *s == '=' || *s == '$' || 
      *s == '#' || *s == '*' || *s == '<' || *s == '>' || *s == '%' )
    return xbTrue;

  if( strncmp( s, "!=", 2 ) == 0 )
    return xbTrue;

  if((strncmp( s, ".NOT.", 5 ) == 0 ) || (strncmp( s, ".OR.", 4 ) == 0 ) || (strncmp( s, ".AND.", 5 ) == 0 ))
    return xbTrue;

  if((strncmp( s, "NOT ", 4 ) == 0 ) || (strncmp( s, "OR ", 3 ) == 0 ) || (strncmp( s, "AND ", 4 ) == 0 ))
    return xbTrue;

  return xbFalse;
}

/*************************************************************************/
//! Is Token separator
/*! This method determines if the next token is a separator.

   \param sExpression - String expression to be evaluated.
   \returns xbTrue - Is a token separator.<br>
            xbFalse - Is not a token separator.
*/
char xbExp::IsTokenSeparator( char c ){
   if( c == '-' || c == '+' || c == '*' || c == '/' || c == '$' || c == '#' || 
       c == '<' || c == '>' || c == '^' || c == '=' || c == '.' || c == '!' )
      return c;
   else
      return 0;
}
/*************************************************************************/
//! Is White space
/*! This method determines if a given character is white space.

   \param c - Character to be evaluated.
   \returns xbTrue - Is white space.<br>
            xbFalse - Is not white space.
*/
xbBool xbExp::IsWhiteSpace( char c ){
   return(( c == 0x20 )? 1 : 0 );
}

/*************************************************************************/
//! Get operator weight.
/*!
   This method determines the priority of an operator

   Operator precendence
     10  .AND. .OR. .NOT.   (not really an operator)
     9  > or <  (includes <= or >=)
     6  unary plus or minus  (+,-)   -- not passed this routine
     5  prefix increment and/or decrement (++,--)
     4  exponentiation  ** or ^
     3  multiplication,division or modulus  (*,/,%)
     2  Addition, subtraction (+,-)
     1  Postfix increment and/or decrement  (++,--)

  \param sOper - Operator.
  \returns Operator weight

*/

xbInt16 xbExp::OperatorWeight( const xbString &sOper ){

  if( sOper == "" || sOper.Len() > 5 ) 
    return 0;

  else if( sOper == "--0" || sOper == "++0" )   // 0 is prefix
    return 9;
  else if( sOper == "**" || sOper == "^" )
    return 8;
  else if( sOper == "*" || sOper == "/" || sOper == "%" || sOper == "*=" || sOper == "/=" )
    return 7;
  else if( sOper == "+" || sOper == "-" || sOper == "+=" || sOper == "-=" )
    return 6;
  else if( sOper == "--1" || sOper == "++1" )   // 1 is post fix
    return 5;
  else if( sOper == ">"  || sOper == ">=" || sOper == "<" || sOper == "<=" ||
           sOper == "<>" || sOper == "!=" || sOper == "#"  || sOper == "$" || sOper == "=" )
    return 4;
  else if( sOper == ".NOT." || sOper == "NOT" )
    return 3;
  else if( sOper == ".AND." || sOper == "AND" )
    return 2;
  else if( sOper == ".OR." || sOper == "OR" )
    return 1;

  return 0;
}


/*************************************************************************/
//! Parse expression.
/*!
  \param sExpression - Expression to parse.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbExp::ParseExpression( const xbString &sExpression ){
  return ParseExpression( sExpression, (xbInt16) 0 );
}

/*************************************************************************/
//! Parse expression.
/*!
  \param dbf - Pointer to xbDbf instance.
  \param sExpression - Expression to parse.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbExp::ParseExpression( xbDbf *dbf, const xbString &sExpression  ){
  this->dbf = dbf;
  return ParseExpression( sExpression, (xbInt16) 0 );
}

/*************************************************************************/
//! Parse expression.
/*!
  \param sExpression - Expression to parse.
  \param iWeight.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbExp::ParseExpression( const xbString &sExpression, xbInt16 iWeight ){

  xbExpNode  *n = NULL;
  xbExpNode  *nLastNode = NULL;   // pointer to the last node processed
  xbExpToken t;
  xbInt16    iRc = XB_NO_ERROR;
  xbInt16    iErrorStop = 0;
  xbString   s;
  xbBool     bNewNode = xbFalse;

  try {

    if( nTree )
      delete nTree;

    if(( iRc = CheckParensAndQuotes( sExpression )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }

    t.sExpression = sExpression;

    xbString sOriginalExp;
    while( t.iSts == XB_NO_ERROR && iRc == XB_NO_ERROR ){

      sOriginalExp = t.sExpression;    // test code
      iRc = GetNextToken( t );
      if( !iRc && !t.iSts ){

        // comment / uncomment  debug / live
        // DumpToken( t, 0 );

        if( t.cNodeType == XB_EXP_NOTROOT ){
          xbExp enr( xbase, dbf );
          if(( iRc = enr.ParseExpression( t.sToken, iWeight + 10 )) != XB_NO_ERROR ){
            iErrorStop = 110;
            throw iRc;
          }
          n = enr.GetTreeHandle();
          enr.ClearTreeHandle();

        } else {

          switch( t.cNodeType ) {

            case XB_EXP_CONSTANT:
              n = new xbExpNode( t.sToken, t.cNodeType );
              bNewNode = xbTrue;
              if(( iRc = ParseExpressionConstant( t, n )) != XB_NO_ERROR ){
                iErrorStop = 120;
                throw iRc;
              }
              break;

            case XB_EXP_FUNCTION:
              n = new xbExpNode( t.cNodeType );
              bNewNode = xbTrue;
              if(( iRc = ParseExpressionFunction( t, n, iWeight )) != XB_NO_ERROR ){
                iErrorStop = 130;
                throw iRc;
              }
              break;

            case XB_EXP_FIELD:
              n = new xbExpNode( t.cNodeType );
              bNewNode = xbTrue;
              if(( iRc = ParseExpressionField( t, n )) != XB_NO_ERROR ){
                iErrorStop = 140;
                throw iRc;
              }
              break;

            case XB_EXP_OPERATOR:
            case XB_EXP_PRE_OPERATOR:
            case XB_EXP_POST_OPERATOR:
              n = new xbExpNode( t.sToken, t.cNodeType );
              bNewNode = xbTrue;
              if(( iRc = ParseExpressionOperator( t, n, iWeight )) != XB_NO_ERROR ){
                iErrorStop = 150;
                throw iRc;
              }
              break;

            default:
              iErrorStop = 160;
              iRc = XB_PARSE_ERROR;
              throw iRc;
              //  break;
          }
        }
        t.cPrevNodeType   = t.cNodeType;
        t.cPrevReturnType = t.cReturnType;

        // determine where in the expression tree to insert the latest node "n"
        // Is this the first node to be added to the tree?
        if( !nTree ){
          nTree = n;
        }

        // else if last node was XB_EXB_PRE_OPERATOR then append this as child to last node
        else if( nLastNode && nLastNode->GetNodeType() == XB_EXP_PRE_OPERATOR ){
          n->SetParent( nLastNode );
          nLastNode->AddChild( n );
        }

        // else if last node was XB_EXB_POST_OPERATOR then append this as child to last node
        else if( nLastNode && n->GetNodeType() == XB_EXP_POST_OPERATOR ){
          n->AddChild( nLastNode );
          nLastNode->SetParent( n );
          if( nLastNode == nTree ){
            nTree = n;
          } else {
            nLastNode->GetParent()->RemoveLastChild();
            nLastNode->GetParent()->AddChild( n );
            n->SetParent( nLastNode->GetParent() );
          }
        }

        else if( n->GetNodeType() == XB_EXP_OPERATOR ){
          xbExpNode * nWorkNode = nLastNode;
          while( nWorkNode && ( nWorkNode->GetNodeType() != XB_EXP_OPERATOR || n->GetWeight() <= nWorkNode->GetWeight())){
            nWorkNode = nWorkNode->GetParent();
          }

          if( !nWorkNode ){  // we are at the top
            nTree->SetParent( n );
            n->AddChild( nTree );
            nTree = n;

          } else if( nWorkNode->GetChildCnt() == 1 ){
            nWorkNode->AddChild( n );
            n->SetParent( nWorkNode );

          } else if( nWorkNode->GetChildCnt() == 2 ){
            xbExpNode * nChild2 = nWorkNode->GetChild(1);
            n->AddChild( nChild2 );
            nWorkNode->RemoveLastChild();
            nWorkNode->AddChild( n );
            n->SetParent( nWorkNode );

          } else{
            // should not be stopping on anything but an operator node with one or two children
            iErrorStop = 170;
            iRc = XB_PARSE_ERROR;
            throw iRc;
          }
        } else {
          n->SetParent( nLastNode );
          nLastNode->AddChild( n );
        }
        nLastNode = n;
        n = NULL;
      }
    }

    // for each node in the tree, calculate the length if it's not already set
    xbExpNode * nWork = GetNextNode( NULL );
    xbExpNode * nChild1;
    xbExpNode * nChild2;

    while( nWork ){
      if( nWork->GetReturnType() == XB_EXP_UNKNOWN ){
        nWork->GetNodeText( s );

        //  std::cout << "XB_EXP_UNKNOWN logic [" << s << "][" <<  nWork->GetChildCnt() << "]\n";
        //     if this is "-" and child 1 and child 2 are both dates, set this result type to numeric
        if( s == "-" && nWork->GetChildCnt() == 2 &&
             nWork->GetChild(0)->GetReturnType() == XB_EXP_DATE && nWork->GetChild(1)->GetReturnType() == XB_EXP_DATE )
          nWork->SetReturnType( XB_EXP_NUMERIC );
        else if( nWork->GetChildCnt() > 0 )
          nWork->SetReturnType( nWork->GetChild(0)->GetReturnType());
        else{
          iErrorStop = 180;
          iRc = XB_PARSE_ERROR;
          throw iRc;
        }
      }
      if( nWork->GetResultLen() == 0 ){

        switch( nWork->GetReturnType() ){

         case XB_EXP_NUMERIC:
           nWork->SetResultLen( 4 );
           break;

         case XB_EXP_CHAR:
           if( nWork->GetNodeType() != XB_EXP_OPERATOR ){
             iErrorStop = 190;
             iRc = XB_PARSE_ERROR;
             throw iRc;
           }
           if( nWork->GetChildCnt() < 2 ){
             iErrorStop = 200;
             iRc = XB_PARSE_ERROR;
             throw iRc;
           }
           nChild1 = nWork->GetChild( 0 );
           nChild2 = nWork->GetChild( 1 );
           nWork->SetResultLen( nChild1->GetResultLen() + nChild2->GetResultLen());
           break;

         case XB_EXP_DATE:
           nWork->SetResultLen( 8 );
           break;

         case XB_EXP_LOGICAL:
           nWork->SetResultLen( 1 );
           break;

         default:
           iErrorStop = 210;
           iRc = XB_PARSE_ERROR;
           throw iRc;
           // break;
        }
      }
      if( nWork->IsUnaryOperator() ){
        if( nWork->GetChildCnt() != 1 ){
          iErrorStop = 220;
          iRc = XB_PARSE_ERROR;
          throw iRc;
        }
      } else if( nWork->IsOperator() && nWork->GetChildCnt() != 2 ){
        iErrorStop = 230;
        iRc = XB_PARSE_ERROR;
        throw iRc;
      }
      nWork = GetNextNode( nWork );
    }
  }
  catch (xbInt16 iRc ){
    if( bNewNode && n )
      delete n;
    xbString sMsg;
    sMsg.Sprintf( "xbexp::ParseExpression() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
  }
  return iRc;
}
/*************************************************************************/
//! Parse expression constant.
/*!
  \param t - Token.
  \param n - Node.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbExp::ParseExpressionConstant( xbExpToken &t, xbExpNode *n ){

  xbDate     dtWork;
  n->SetReturnType( t.cReturnType ); 

  // std::cout << "parse expression constant[" << t.sToken << "]\n";

  switch( t.cReturnType ){
     case XB_EXP_CHAR:
       n->SetResultLen( t.sToken.Len() );
       n->SetResult( t.sToken );
       break;

     case XB_EXP_DATE:
       n->SetResultLen( 8 );
       dtWork.Set( t.sToken );
       n->SetResult( dtWork );
       break;

     case XB_EXP_LOGICAL:
       n->SetResultLen( 1 );
       if( strncmp( t.sToken, "T", 1 ) == 0 )
         n->SetResult( (xbBool) xbTrue );
       else
         n->SetResult( (xbBool) xbFalse );
       break;

     case XB_EXP_NUMERIC:
       n->SetResultLen( 4 );
       n->SetResult( strtod( t.sToken, 0 ));
       n->SetResult( t.sToken );
       break;

     default:
       return XB_PARSE_ERROR;
       // break;
  }
  return XB_NO_ERROR;
}

/*************************************************************************/
//! Parse expression field.
/*!
  \param t - Token.
  \param n - Node.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbExp::ParseExpressionField( xbExpToken &t, xbExpNode *n ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbDbf * pDbf;
  xbString sFieldName;

  // do the db lookup and set the field number for the field

  try{

    xbUInt32 lPos;

    if(( lPos = t.sToken.Pos( "->" )) > 0 ){
      //  table name is part of the token
      xbString sTableName = t.sToken;
      sTableName.Left( lPos-1 );
      sFieldName = t.sToken;
      sFieldName.Mid( lPos + 2, t.sToken.Len() - lPos - 1 );
      pDbf = (xbDbf *) xbase->GetDbfPtr( sTableName );


/*
    //  updated 1/2/23 to support either table.field or table->field
    if((( lPos = t.sToken.Pos( "->" )) > 0) || (( lPos = t.sToken.Pos( "." )) > 0) ){
      //  table name is part of the token
      xbString sTableName = t.sToken;
      sTableName.Left( lPos-1 );
      sFieldName = t.sToken;
      if( t.sToken[lPos] == '.' )
        sFieldName.Mid( lPos + 1, t.sToken.Len() - lPos );
      else  // ->
        sFieldName.Mid( lPos + 2, t.sToken.Len() - lPos - 1 );
      pDbf = (xbDbf *) xbase->GetDbfPtr( sTableName );
*/

    } else {
      // table name is not part of the token 
      pDbf = dbf;
      sFieldName = t.sToken;
    }
    if( !pDbf ){
      iErrorStop = 100;
      iRc = XB_INVALID_FIELD;
      throw iRc;
    }
    xbInt16 iFieldNo = 0;

    if(( iRc = pDbf->GetFieldNo( sFieldName, iFieldNo )) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }
    char cFieldType;
    if(( iRc = pDbf->GetFieldType( iFieldNo, cFieldType )) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw iRc;
    }
    n->SetDbfInfo( pDbf, iFieldNo );
    switch( cFieldType ){
      case XB_CHAR_FLD:
        n->SetReturnType( XB_EXP_CHAR );
        break;

      case XB_LOGICAL_FLD:
        n->SetReturnType( XB_EXP_LOGICAL );
        break;

      case XB_NUMERIC_FLD:
      case XB_FLOAT_FLD:
        n->SetReturnType( XB_EXP_NUMERIC );
        break;

      case XB_DATE_FLD:
        n->SetReturnType( XB_EXP_DATE );
        break;

      case XB_MEMO_FLD:
      default:
        iErrorStop = 130;
        iRc = XB_PARSE_ERROR;
        throw iRc;
        // break;
    }
    n->SetNodeText( sFieldName );
    xbInt16 iResultLen = 0;
    if(( iRc = pDbf->GetFieldLen( iFieldNo, iResultLen )) != XB_NO_ERROR ){
      iErrorStop = 140;
      throw iRc;
    }
    n->SetResultLen( (xbUInt32) iResultLen );
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbexp::ParseExpressionField() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
  }
  return iRc;
}

/*************************************************************************/
//! Parse expression function.
/*!
  \param t - Token.
  \param n - Node.
  \param iWeight
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbExp::ParseExpressionFunction( xbExpToken &t, xbExpNode *n, xbInt16 iWeight ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;

  try{
    // find the first "("
    xbUInt32 lPos = t.sToken.Pos( '(' );
    if( lPos == 0 ){
      iErrorStop = 100;
      iRc = XB_INVALID_FUNCTION;
      throw iRc;
    }
    // Get the function name and look it up in the table


    xbString sFunc = t.sToken;
    sFunc.Left( lPos - 1 ).Trim();
    char cReturnType;
    xbInt16 i = 0;
    xbInt32 l = 0;
    if(( iRc = xbase->GetFunctionInfo( sFunc, cReturnType, i, l )) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }
    n->SetNodeText( sFunc );


    // Get the function parms
    xbString sParms = t.sToken;
    sParms.Mid( lPos+1, t.sToken.Len() - lPos );
    lPos = sParms.GetLastPos( ')' );
    if( lPos == 0 ){
      iErrorStop = 120;
      iRc = XB_INVALID_FUNCTION;
      throw iRc;
    }

    // remove the trailing ")" paren
    sParms.Left( lPos - 1 ).Trim();

    // if this function has parms, put them in the tree
    if( sParms.Len() > 0 ){
      xbExp enr( xbase, dbf );

      // create a linked list of parms
      xbLinkList<xbString> llParms;
      if(( iRc = ParseExpressionFunctionParms( sParms, llParms )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }

      // for each function parm, recursively process it
      xbLinkListNode<xbString> * llN = llParms.GetHeadNode();
      xbString sParm;
      while( llN ){
        sParm = llN->GetKey();
        if(( iRc = enr.ParseExpression( sParm, iWeight + 10 )) != XB_NO_ERROR ){
          iErrorStop = 140;
          throw iRc;
        }
        n->AddChild( enr.GetTreeHandle());
        enr.ClearTreeHandle();
        llN = llN->GetNextNode();
      }
      llParms.Clear();
    }

    if( cReturnType == '1' ){
      if( n->GetChildCnt() > 0 ){
        xbExpNode *n1 = n->GetChild( 0 );
        n->SetReturnType( n1->GetReturnType());
      }

    } else {
      n->SetReturnType( cReturnType );
    }

    if(( iRc = CalcFunctionResultLen( n )) != XB_NO_ERROR ){
      iErrorStop = 150;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbexp::ParseExpressionFunction() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
  }
  return iRc;
}

/*************************************************************************/
//! Parse expression function.
/*!

  Creates a linked list of function parms as xbStrings
  This function pulls out the parms and addresses embedded parens and quotes within the parms

  \param sParms
  \param lParms
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbExp::ParseExpressionFunctionParms( const xbString &sParms, xbLinkList<xbString> & llParms ){

  xbInt16  iRc = XB_NO_ERROR;
  xbInt16  iErrorStop = 0;
  xbInt16  iParenCtr = 0;
  xbInt16  iInQuotes = 0;
  xbInt16  iDoubleQuotes = 0;
  xbInt16  iSingleQuotes = 0;
  xbInt32  lStartPos = 0;
  xbInt32  lParmLen = 0;

  xbString sParm;

  try{
    const char *sp = sParms;

    while( *sp ){
      if( *sp == '(')
        iParenCtr++;
      else if( *sp == ')' )
        iParenCtr--;
      else if( !iInQuotes && *sp == '"' ){
        iInQuotes++;
        iDoubleQuotes++;
      } else if( iInQuotes && *sp == '"' ){
        iInQuotes--;
        iDoubleQuotes--;
      }
      else if( !iInQuotes && *sp == '\'' ){
        iInQuotes++;
        iSingleQuotes++;
      } else if( iInQuotes && *sp == '\'' ){
        iInQuotes--;
        iSingleQuotes--;

      } else if( !iInQuotes && !iParenCtr && *sp == ',' ){
        // found a valid comma - at the end of a parm
        // add it to the end of the linked list
        sParm = sParms;
        sParm.Mid( (xbUInt32) lStartPos + 1, (xbUInt32) lParmLen ).Trim();   // mid is one based
        llParms.InsertAtEnd( sParm );

        // set the start pos for the next one on the list
        lStartPos += lParmLen + 1;
        lParmLen = -1;
        // lParmLen = 0;
      }
      lParmLen++;
      sp++;
    }
    if( lParmLen > 0 ){
      // get the last parm, it didn't end with a comma
        sParm = sParms;
        sParm.Mid( (xbUInt32) lStartPos + 1, (xbUInt32) lParmLen ).Trim();  
        llParms.InsertAtEnd( sParm );
     }

  }
  // try / catch not used in this method, structure added for potential future use
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbexp::ParseExpressionFunctionParms() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
  }
  return iRc;
}
/*************************************************************************/
//! Parse expression operator.
/*!
  \param t - Token.
  \param n - Node.
  \param iWeight
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbExp::ParseExpressionOperator( xbExpToken &t, xbExpNode *n, xbInt16 iWeight ){

  n->SetResult( t.sToken );
  n->SetWeight( iWeight + OperatorWeight( t.sToken) );

//   std::cout << "ParseExpressionOperator [" << t.cPrevNodeType << "][" << t.sToken << "] Weight = [" << iWeight;
//   std::cout << "] PrevReturnType [" << t.cPrevReturnType;
//   std::cout << "] Operator weight [" << OperatorWeight( t.sToken ) << "] getweight [" << n->GetWeight() << "]\n";

  if( t.sToken == "**" || t.sToken == "^"   ||
      t.sToken == "*"   || t.sToken == "/"   || t.sToken == "%"  || t.sToken == "*="  || t.sToken == "/=" )
    n->SetReturnType( XB_EXP_NUMERIC );

  else if( t.sToken == "--"  || t.sToken == "++"  ||  t.sToken == "+="  || t.sToken == "-=" )     // could be date or numeric
    n->SetReturnType( XB_EXP_UNKNOWN );

  else if( t.cPrevReturnType == XB_EXP_CHAR && ( t.sToken == "+" || t.sToken == "-" ))
    n->SetReturnType( XB_EXP_CHAR );

  else if( t.sToken == ".AND." || t.sToken == ".OR." || t.sToken == ".NOT." ||
           t.sToken == "AND"   || t.sToken == "OR"   || t.sToken == "NOT"   ||
           t.sToken == ">"     || t.sToken == ">="   || t.sToken == "<"     ||
           t.sToken == "<="    || t.sToken == "<>"   || t.sToken == "!="    ||
           t.sToken == "$"     || t.sToken == "#"    || t.sToken == "=" )
         n->SetReturnType( XB_EXP_LOGICAL );


  else if( t.cPrevReturnType == XB_EXP_UNKNOWN )
    n->SetReturnType( XB_EXP_UNKNOWN );

  // added for date constant logic 10/28/17
  else if(( t.sToken == "+" || t.sToken == "-" ) && t.cPrevReturnType == XB_EXP_DATE )
    n->SetReturnType( XB_EXP_DATE );

  else if( t.sToken == "+" || t.sToken == "-" )
    n->SetReturnType( XB_EXP_NUMERIC );

  return XB_NO_ERROR;
}


/************************************************************************/
//! ProcessExpression
/*! This method processes an expression tree leaving the result in the
    root node of the tree
*/
xbInt16 xbExp::ProcessExpression(){
  return ProcessExpression( 0 );
}
/************************************************************************/
//! ProcessExpression
/*! This method processes a parsed expression tree leaving the result in the
    root node of the tree
  \param iRecBufSw Record buffer to use when evaluating expression.<br>
                   0 - Current record buffer.<br>
                   1 - Original record buffer.
*/

xbInt16 xbExp::ProcessExpression( xbInt16 iRecBufSw ){

// iRecBufSw 0 - Record Buffer
//           1 - Original Record Buffer

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;

  try{

    xbExpNode * nWork = GetNextNode( NULL );
    xbExpNode * nChild1;
    xbDbf     * dbf;
    xbString    sWork1;
    xbString    sWork2;
    xbString    sOperator;
    xbDate      dtWork1;

    xbBool      bWork;
    xbDouble    dWork;

    while( nWork ){
      switch( nWork->GetNodeType() ){

        case XB_EXP_CONSTANT:
          break;

        case XB_EXP_PRE_OPERATOR:  // increment value before setting in head node
          if( nWork->GetChildCnt() != 1 ){
            iErrorStop = 100;
            iRc = XB_PARSE_ERROR;
            throw iRc;
          }
          nChild1 = nWork->GetChild( 0 );
          //if( nChild1->GetReturnType() == XB_EXP_DATE )
          //    nChild1->SetResult( (xbDouble) nChild1->GetDateResult().JulianDays());

          nWork->GetNodeText( sWork1 );
          if( sWork1 == "++" )
            nChild1->SetResult( nChild1->GetNumericResult() + 1 );
          else
            nChild1->SetResult( nChild1->GetNumericResult() - 1 );

          nWork->SetResult( nChild1->GetNumericResult());

          //if( nChild1->GetReturnType() == XB_EXP_DATE ){
          //    dtWork1.JulToDate8( (xbInt32) nChild1->GetNumericResult());
          //    nChild1->SetResult( dtWork1 );
          //     nWork->SetResult( dtWork1 );
          // }
          break;

        case XB_EXP_POST_OPERATOR:  // increment value after setting in head node
          if( nWork->GetChildCnt() != 1 ){
            iErrorStop = 110;
            iRc = XB_PARSE_ERROR;
            throw iRc;
          }
          nChild1 = nWork->GetChild( 0 );
          //if( nChild1->GetReturnType() == XB_EXP_DATE ){
          //    nWork->SetResult( nChild1->GetDateResult());
          //    nChild1->SetResult( (xbDouble) nChild1->GetDateResult().JulianDays());
          //}
          //else
          nWork->SetResult( nChild1->GetNumericResult());

          nWork->GetNodeText( sWork1 );
          if( sWork1 == "++" )
            nChild1->SetResult( nChild1->GetNumericResult() + 1 );
          else
            nChild1->SetResult( nChild1->GetNumericResult() - 1 );

          //if( nChild1->GetReturnType() == XB_EXP_DATE ){
          //    dtWork1.JulToDate8( (xbInt32) nChild1->GetNumericResult());
          //    nChild1->SetResult( dtWork1 );
          // }
          break;

        case XB_EXP_FIELD:

          if(( dbf = nWork->GetDbf()) == NULL ){
            iErrorStop = 120;
            iRc = XB_PARSE_ERROR;
            throw iRc;
          }
          switch( nWork->GetReturnType()){
            case XB_EXP_CHAR:
              if(( iRc = dbf->GetField( nWork->GetFieldNo(), sWork1, iRecBufSw )) < XB_NO_ERROR ){
                iErrorStop = 130;
                throw iRc;
              }
              nWork->SetResult( sWork1 );
              break;

            case XB_EXP_DATE:

              if(( iRc = dbf->GetField( nWork->GetFieldNo(), sWork1, iRecBufSw )) < XB_NO_ERROR ){
                iErrorStop = 140;
                throw iRc;
              }
              if( sWork1 == "        " ){
                // std::cout << "xbExp::ProcessExpression() line 1938 sWork is spaces\n";
                //nWork->SetResult( (xbDouble) 21474835648 );   // dbase sets a date value in both ndx and mdx index files to this if spaces on dbf record
                nWork->SetResult( (xbDouble) XB_NULL_DATE );
              } else {
                dtWork1.Set( sWork1 );
                nWork->SetResult( (xbDouble) dtWork1.JulianDays() );
              }
              break;

            case XB_EXP_LOGICAL:
              if(( iRc = dbf->GetLogicalField( nWork->GetFieldNo(), bWork, iRecBufSw )) != XB_NO_ERROR ){
                iErrorStop = 150;
                throw iRc;
              }
              nWork->SetResult( bWork );
              break;

            case XB_EXP_NUMERIC:
              if(( iRc = dbf->GetDoubleField( nWork->GetFieldNo(), dWork, iRecBufSw )) != XB_NO_ERROR ){
                iErrorStop = 160;
                throw iRc;
              }
              nWork->SetResult( dWork );
              break;

            default:
            iErrorStop = 170;
            iRc = XB_PARSE_ERROR;
            throw iRc;
            // break;
          }
          break;


        case XB_EXP_OPERATOR:
          if(( iRc = ProcessExpressionOperator( nWork )) != XB_NO_ERROR ){
            iErrorStop = 180;
            throw iRc;
          }

          break;

        case XB_EXP_FUNCTION:
          if(( iRc = ProcessExpressionFunction( nWork, iRecBufSw )) != XB_NO_ERROR ){
            iErrorStop = 190;
            throw iRc;
          }
          break;

        default:
          iErrorStop = 200;
          iRc = XB_PARSE_ERROR;
          throw iRc;
          // break;
        }
        nWork = GetNextNode( nWork );
     }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbexp::ProcessExpression() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
  }
  return iRc;
}
/*************************************************************************/
//! ProcessExpression
/*! This method processes an expression tree for a given node.
*/

xbInt16 xbExp::ProcessExpressionFunction( xbExpNode * n, xbInt16 iRecBufSw ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;

  try{

    xbString sFunction;
    xbString sResult;
    xbDouble dResult;
    xbDate   dtResult;
    xbBool   bResult;

    n->GetNodeText( sFunction );

    // process functions with no children first
    xbExpNode * nChild1;
    if( n->GetChildCnt() == 0 ){
      if( sFunction == "DATE" ){
        if(( iRc = xbase->DATE( dtResult )) != XB_NO_ERROR ){
          iErrorStop = 100;
          throw iRc;
        }
        n->SetResult( dtResult );
      } else if( sFunction == "DEL" ){
        if(( iRc = xbase->DEL( dbf, sResult, iRecBufSw )) != XB_NO_ERROR ){
          iErrorStop = 110;
          throw iRc;
        }
        n->SetResult( sResult );
      } else if( sFunction == "DELETED" ){
        if(( iRc = xbase->DELETED( dbf, bResult, iRecBufSw )) != XB_NO_ERROR ){
          iErrorStop = 120;
          throw iRc;
        }
        n->SetResult( bResult );
      } else if( sFunction == "RECCOUNT" ){
        if(( iRc = xbase->RECCOUNT( dbf, dResult )) != XB_NO_ERROR ){
          iErrorStop = 130;
          throw iRc;
        }
        n->SetResult( dResult );
      } else if( sFunction == "RECNO" ){
        if(( iRc = xbase->RECNO( dbf, dResult )) != XB_NO_ERROR ){
          iErrorStop = 140;
          throw iRc;
        }
        n->SetResult( dResult );
      }
    // process functions with one child
    } else if( n->GetChildCnt() == 1 ){

      nChild1 = n->GetChild( 0 );

      if( sFunction == "ABS" ){
        if(( iRc = xbase->ABS( nChild1->GetNumericResult(), dResult )) != XB_NO_ERROR ){
          iErrorStop = 200;
          throw iRc;
        }
        n->SetResult( dResult );
      } else if( sFunction == "ALLTRIM" ){
        if(( iRc = xbase->ALLTRIM( nChild1->GetStringResult(), sResult )) != XB_NO_ERROR ){
          iErrorStop = 210;
          throw iRc;
        }
        n->SetResult( sResult );
      } else if( sFunction == "ASC" ){
        if(( iRc = xbase->ASC( nChild1->GetStringResult(), dResult )) != XB_NO_ERROR ){
          iErrorStop = 220;
          throw iRc;
        }
        n->SetResult( dResult );
      } else if( sFunction == "CDOW" ){
        xbDate d( (xbInt32) nChild1->GetNumericResult());
        if(( iRc = xbase->CDOW( d, sResult )) != XB_NO_ERROR ){
          iErrorStop = 230;
          throw iRc;
        }
        n->SetResult( sResult );
      } else if( sFunction == "CHR" ){
        if(( iRc = xbase->CHR( nChild1->GetNumericResult(), sResult )) != XB_NO_ERROR ){
          iErrorStop = 240;
          throw iRc;
        }
        n->SetResult( sResult );
      } else if( sFunction == "CMONTH" ){
        xbDate d( (xbInt32) nChild1->GetNumericResult());
        if(( iRc = xbase->CMONTH( d, sResult )) != XB_NO_ERROR ){
          iErrorStop = 250;
          throw iRc;
        }
        n->SetResult( sResult );
      } else if( sFunction == "CTOD" ){
        if(( iRc = xbase->CTOD( nChild1->GetStringResult(), dtResult )) != XB_NO_ERROR ){
          iErrorStop = 260;
          throw iRc;
        }
        n->SetResult( dtResult );
      } else if( sFunction == "DAY" ){
        xbDate d( (xbInt32) nChild1->GetNumericResult());
        if(( iRc = xbase->DAY( d, dResult )) != XB_NO_ERROR ){
          iErrorStop = 270;
          throw iRc;
        }
        n->SetResult( dResult );
      } else if( sFunction == "DESCEND" ){

        if( n->GetReturnType() == XB_EXP_CHAR ){
           if(( iRc = xbase->DESCEND( nChild1->GetStringResult(), sResult )) != XB_NO_ERROR ){
             iErrorStop = 280;
             throw iRc;
           }
           n->SetResult( sResult );

        } else if( n->GetReturnType() == XB_EXP_DATE ){
           xbDate d( (xbInt32) nChild1->GetNumericResult());
           if(( iRc = xbase->DESCEND( d, dtResult )) != XB_NO_ERROR ){
             iErrorStop = 290;
             throw iRc;
           }
           n->SetResult( dtResult );

        } else if( n->GetReturnType() == XB_EXP_NUMERIC ){
           if(( iRc = xbase->DESCEND( nChild1->GetNumericResult(), dResult )) != XB_NO_ERROR ){
             iErrorStop = 300;
             throw iRc;
           }
           n->SetResult( dResult );

        } else {
           iErrorStop = 310;
           iRc = XB_PARSE_ERROR;
           throw iRc;
        }

      } else if( sFunction == "DOW" ){
        xbDate d( (xbInt32) nChild1->GetNumericResult());
        if(( iRc = xbase->DOW( d, dResult )) != XB_NO_ERROR ){
          iErrorStop = 320;
          throw iRc;
        }
        n->SetResult( dResult );
      } else if( sFunction == "DTOC" ){
        xbDate d( (xbInt32) nChild1->GetNumericResult());
        if(( iRc = xbase->DTOC( d, sResult )) != XB_NO_ERROR ){
          iErrorStop = 330;
          throw iRc;
        }
        n->SetResult( sResult );
      } else if( sFunction == "DTOS" ){
        xbDate d( (xbInt32) nChild1->GetNumericResult());
        if(( iRc = xbase->DTOS( d, sResult )) != XB_NO_ERROR ){
          iErrorStop = 340;
          throw iRc;
        }
        n->SetResult( sResult );
      } else if( sFunction == "EXP" ){
        if(( iRc = xbase->EXP( nChild1->GetNumericResult(), dResult )) != XB_NO_ERROR ){
          iErrorStop = 350;
          throw iRc;
        }
        n->SetResult( dResult );
      } else if( sFunction == "INT" ){
        if(( iRc = xbase->INT( nChild1->GetNumericResult(), dResult )) != XB_NO_ERROR ){
          iErrorStop = 360;
          throw iRc;
        }
        n->SetResult( dResult );
      } else if( sFunction == "ISALPHA" ){
        if(( iRc = xbase->ISALPHA( nChild1->GetStringResult(), bResult )) != XB_NO_ERROR ){
          iErrorStop = 370;
          throw iRc;
        }
        n->SetResult( bResult );
      } else if( sFunction == "ISLOWER" ){
        if(( iRc = xbase->ISLOWER( nChild1->GetStringResult(), bResult )) != XB_NO_ERROR ){
          iErrorStop = 380;
          throw iRc;
        }
        n->SetResult( bResult );
      } else if( sFunction == "ISUPPER" ){
        if(( iRc = xbase->ISUPPER( nChild1->GetStringResult(), bResult )) != XB_NO_ERROR ){
          iErrorStop = 390;
          throw iRc;
        }
        n->SetResult( bResult );
      } else if( sFunction == "LEN" ){
        if(( iRc = xbase->LEN( nChild1->GetStringResult(), dResult )) != XB_NO_ERROR ){
          iErrorStop = 400;
          throw iRc;
        }
        n->SetResult( dResult );
      } else if( sFunction == "LOG" ){
        if(( iRc = xbase->LOG( nChild1->GetNumericResult(), dResult )) != XB_NO_ERROR ){
          iErrorStop = 410;
          throw iRc;
        }
        n->SetResult( dResult );
      } else if( sFunction == "LTRIM" ){
        if(( iRc = xbase->LTRIM( nChild1->GetStringResult(), sResult )) != XB_NO_ERROR ){
          iErrorStop = 420;
          throw iRc;
        }
        n->SetResult( sResult );
      } else if( sFunction == "LOWER" ){
        if(( iRc = xbase->LOWER( nChild1->GetStringResult(), sResult )) != XB_NO_ERROR ){
          iErrorStop = 430;
          throw iRc;
        }
        n->SetResult( sResult );
      } else if( sFunction == "MONTH" ){
        xbDate d( (xbInt32) nChild1->GetNumericResult());
        if(( iRc = xbase->MONTH( d, dResult )) != XB_NO_ERROR ){
          iErrorStop = 440;
          throw iRc;
        }
        n->SetResult( dResult );
      } else if( sFunction == "RTRIM" ){
        if(( iRc = xbase->RTRIM( nChild1->GetStringResult(), sResult )) != XB_NO_ERROR ){
          iErrorStop = 450;
          throw iRc;
        }
        n->SetResult( sResult );
      } else if( sFunction == "STOD" ){
        if(( iRc = xbase->STOD( nChild1->GetStringResult(), dtResult )) != XB_NO_ERROR ){
          iErrorStop = 460;
          throw iRc;
        }
        n->SetResult( dtResult );
      } else if( sFunction == "SPACE" ){
        if(( iRc = xbase->SPACE( (xbInt32) nChild1->GetNumericResult(), sResult )) != XB_NO_ERROR ){
          iErrorStop = 470;
          throw iRc;
        }
        n->SetResult( sResult );
      } else if( sFunction == "SQRT" ){
        if(( iRc = xbase->SQRT( nChild1->GetNumericResult(), dResult )) != XB_NO_ERROR ){
          iErrorStop = 480;
          throw iRc;
        }
        n->SetResult( dResult );
      } else if( sFunction == "STR" ){
        if(( iRc = xbase->STR( nChild1->GetNumericResult(), sResult )) != XB_NO_ERROR ){
          iErrorStop = 490;
          throw iRc;
        }
        n->SetResult( sResult );
      } else if( sFunction == "TRIM" ){
        if(( iRc = xbase->TRIM( nChild1->GetStringResult(), sResult )) != XB_NO_ERROR ){
          iErrorStop = 500;
          throw iRc;
        }
        n->SetResult( sResult );
      } else if( sFunction == "UPPER" ){
        if(( iRc = xbase->UPPER( nChild1->GetStringResult(), sResult )) != XB_NO_ERROR ){
          iErrorStop = 510;
          throw iRc;
        }
        n->SetResult( sResult );
      } else if( sFunction == "VAL" ){
        if(( iRc = xbase->VAL( nChild1->GetStringResult(), dResult )) != XB_NO_ERROR ){
          iErrorStop = 520;
          throw iRc;
        }
        n->SetResult( dResult );
      } else if( sFunction == "YEAR" ){
        xbDate d( (xbInt32) nChild1->GetNumericResult());
        if(( iRc = xbase->YEAR( d, dResult )) != XB_NO_ERROR ){
          iErrorStop = 530;
          throw iRc;
        }
        n->SetResult( dResult );
      } else {
        iErrorStop = 540;
        iRc = XB_PARSE_ERROR;
        throw iRc;
      }
    } else if( n->GetChildCnt() == 2 ){
      xbExpNode * nChild2;
      nChild1 = n->GetChild( 0 );
      nChild2 = n->GetChild( 1 );

      if( sFunction == "AT" ){
        if(( iRc = xbase->AT( nChild1->GetStringResult(), nChild2->GetStringResult(), dResult )) != XB_NO_ERROR ){
          iErrorStop = 700;
          throw iRc;
        }
        n->SetResult( dResult );
      } else if( sFunction == "LEFT" ){
        if(( iRc = xbase->LEFT( nChild1->GetStringResult(), (xbUInt32) nChild2->GetNumericResult(), sResult )) != XB_NO_ERROR ){
          iErrorStop = 710;
          throw iRc;
        }
        n->SetResult( sResult );
      } else if( sFunction == "MAX" ){
        if(( iRc = xbase->MAX( nChild1->GetNumericResult(), nChild2->GetNumericResult(), dResult )) != XB_NO_ERROR ){
          iErrorStop = 720;
          throw iRc;
        }
        n->SetResult( dResult );
      } else if( sFunction == "MIN" ){
        if(( iRc = xbase->MIN( nChild1->GetNumericResult(), nChild2->GetNumericResult(), dResult )) != XB_NO_ERROR ){
          iErrorStop = 730;
          throw iRc;
        }
        n->SetResult( dResult );
      }
      else if( sFunction == "REPLICATE" ){
        if(( iRc = xbase->REPLICATE( nChild1->GetStringResult(), (xbUInt32) nChild2->GetNumericResult(), sResult )) != XB_NO_ERROR ){
          iErrorStop = 800;
          throw iRc;
        }
        n->SetResult( sResult );
      } else if( sFunction == "RIGHT" ){
        if(( iRc = xbase->RIGHT( nChild1->GetStringResult(), (xbUInt32) nChild2->GetNumericResult(), sResult )) != XB_NO_ERROR ){
          iErrorStop = 810;
          throw iRc;
        }
        n->SetResult( sResult );
      } else if( sFunction == "STR" ){
        if(( iRc = xbase->STR( nChild1->GetNumericResult(), (xbUInt32) nChild2->GetNumericResult(), sResult )) != XB_NO_ERROR ){
          iErrorStop = 820;
          throw iRc;
        }
        n->SetResult( sResult );
      } else {
        iErrorStop = 830;
        iRc = XB_PARSE_ERROR;
        throw iRc;
      }
    } else if( n->GetChildCnt() == 3 ){
      xbExpNode * nChild2;
      xbExpNode * nChild3;
      nChild1 = n->GetChild( 0 );
      nChild2 = n->GetChild( 1 );
      nChild3 = n->GetChild( 2 );

      if( sFunction == "IIF" ){
        if(( iRc = xbase->IIF( nChild1->GetBoolResult(), nChild2->GetStringResult(), nChild3->GetStringResult(), sResult )) != XB_NO_ERROR ){
          iErrorStop = 900;
          throw iRc;
        }
        n->SetResult( sResult );
      } else if( sFunction == "STR" ){
        if(( iRc = xbase->STR( nChild1->GetNumericResult(), (xbUInt32) nChild2->GetNumericResult(), (xbUInt32) nChild3->GetNumericResult(), sResult )) != XB_NO_ERROR ){
          iErrorStop = 910;
          throw iRc;
        }
        n->SetResult( sResult );
      } else if( sFunction == "STRZERO" ){
        if(( iRc = xbase->STRZERO( nChild1->GetNumericResult(), (xbUInt32) nChild2->GetNumericResult(), (xbUInt32) nChild3->GetNumericResult(), sResult )) != XB_NO_ERROR ){
          iErrorStop = 920;
          throw iRc;
        }
        n->SetResult( sResult );
      } else if( sFunction == "SUBSTR" ){
        if(( iRc = xbase->SUBSTR( nChild1->GetStringResult(), (xbUInt32) nChild2->GetNumericResult(), (xbUInt32) nChild3->GetNumericResult(), sResult )) != XB_NO_ERROR ){
          iErrorStop = 930;
          throw iRc;
        }
        n->SetResult( sResult );
      } else {
        iErrorStop = 950;
        iRc = XB_PARSE_ERROR;
        throw iRc;
      }
    } else if( n->GetChildCnt() == 4 ){
      xbExpNode * nChild2;
      xbExpNode * nChild3;
      xbExpNode * nChild4;
      nChild1 = n->GetChild( 0 );
      nChild2 = n->GetChild( 1 );
      nChild3 = n->GetChild( 2 );
      nChild4 = n->GetChild( 3 );

      if( sFunction == "STR" ){
        if(( iRc = xbase->STR( nChild1->GetNumericResult(), (xbUInt32) nChild2->GetNumericResult(), 
                               (xbUInt32) nChild3->GetNumericResult(), nChild4->GetStringResult(), sResult )) != XB_NO_ERROR ){
          iErrorStop = 1000;
          throw iRc;
        }
        n->SetResult( sResult );
      } else {
        iErrorStop = 1010;
        iRc = XB_PARSE_ERROR;
        throw iRc;
      }

    } else {
      iErrorStop = 2000;
      iRc = XB_PARSE_ERROR;
      throw iRc;
    }

  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbexp::ProcessExpressionFunction() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
  }
  return iRc;
}
/*************************************************************************/
//! Process Expression Operator
/*! This method processes an expression operator for a given node.
*/

xbInt16 xbExp::ProcessExpressionOperator( xbExpNode * n ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;

  xbExpNode * nChild1 = NULL;
  xbExpNode * nChild2 = NULL;
  xbString    sOperator;
  xbString    sWork1;
  xbString    sWork2;
  xbDate      dtWork1;

  xbString sMsg;

  try{
    n->GetNodeText( sOperator );
    nChild1 = n->GetChild( 0 );
    if( !n->IsUnaryOperator())
      nChild2 = n->GetChild( 1 );

    switch( n->GetReturnType()){
      case XB_EXP_CHAR:
        if( sOperator == "+" ){
          sWork1 = nChild1->GetStringResult();
          sWork1 += nChild2->GetStringResult();
          n->SetResult( sWork1 );
        } else if( sOperator == "-" ){
          sWork1 = nChild1->GetStringResult();
          sWork1.Rtrim();
          sWork1 += nChild2->GetStringResult();
          sWork1.PadRight( ' ', n->GetResultLen());
          n->SetResult( sWork1 );
        } else {
          iErrorStop = 100;
          iRc = XB_PARSE_ERROR;
          throw iRc;
        }
        break;

      case XB_EXP_NUMERIC:
        if( sOperator == "+" )
          n->SetResult( nChild1->GetNumericResult() + nChild2->GetNumericResult());
        else if( sOperator == "-" ){
          n->SetResult( nChild1->GetNumericResult() - nChild2->GetNumericResult());

        }
        else if( sOperator == "*" )
          n->SetResult( nChild1->GetNumericResult() * nChild2->GetNumericResult());
        else if( sOperator == "/" )
          n->SetResult( nChild1->GetNumericResult() / nChild2->GetNumericResult());
        else if( sOperator == "^" || sOperator == "**" )
          n->SetResult( pow( nChild1->GetNumericResult(), nChild2->GetNumericResult()));
        else if( sOperator == "+=" ){
          n->SetResult( nChild1->GetNumericResult() + nChild2->GetNumericResult());
          nChild1->SetResult( n->GetNumericResult() );
        }
        else if( sOperator == "-=" ){
          n->SetResult( nChild1->GetNumericResult() - nChild2->GetNumericResult());
          nChild1->SetResult( n->GetNumericResult() );
        }
        else if( sOperator == "*=" ){
          n->SetResult( nChild1->GetNumericResult() * nChild2->GetNumericResult());
          nChild1->SetResult( n->GetNumericResult() );
        }
        else if( sOperator == "/=" ){
          n->SetResult( nChild1->GetNumericResult() / nChild2->GetNumericResult());
          nChild1->SetResult( n->GetNumericResult() );
        } else {
          iErrorStop = 200;
          iRc = XB_PARSE_ERROR;
          throw iRc;
        }
        break;


      case XB_EXP_DATE:
        // if date values in the leaf nodes, convert to numeric for operator logic

        if( sOperator == "+" )
          n->SetResult( nChild1->GetNumericResult() + nChild2->GetNumericResult());
        else if( sOperator == "-" ){
          n->SetResult( nChild1->GetNumericResult() - nChild2->GetNumericResult());
          xbDate d( (xbInt32) n->GetNumericResult());
        }
        else if( sOperator == "+=" ){
          n->SetResult( nChild1->GetNumericResult() + nChild2->GetNumericResult());
          nChild1->SetResult( n->GetNumericResult() );
        }
        else if( sOperator == "-=" ){
          n->SetResult( nChild1->GetNumericResult() - nChild2->GetNumericResult());
          nChild1->SetResult( n->GetNumericResult() );
        } else {
          iErrorStop = 300;
          iRc = XB_PARSE_ERROR;
          throw iRc;
        }
        break;

      case XB_EXP_LOGICAL:

        if( !n->IsUnaryOperator() && (nChild1->GetReturnType() != nChild2->GetReturnType())){
            iErrorStop = 400;
            iRc = XB_INCOMPATIBLE_OPERANDS;
            throw iRc;
        }

        if( sOperator == ".AND." || sOperator == "AND" )
          n->SetResult((xbBool) (nChild1->GetBoolResult() && nChild2->GetBoolResult()) );

        else if( sOperator == ".OR." || sOperator == "OR" )
          n->SetResult((xbBool) (nChild1->GetBoolResult() || nChild2->GetBoolResult()) );

        else if( sOperator == ".NOT." || sOperator == "NOT" ){
          if( nChild1->GetBoolResult())
            n->SetResult((xbBool) xbFalse );
          else
            n->SetResult((xbBool) xbTrue );
        }

        else if( sOperator == ">" ){

          if( nChild1->GetReturnType() == XB_EXP_CHAR )
            n->SetResult((xbBool)(nChild1->GetStringResult() > nChild2->GetStringResult()));

          else if( nChild1->GetReturnType() == XB_EXP_NUMERIC )
            n->SetResult((xbBool)(nChild1->GetNumericResult() > nChild2->GetNumericResult()));

          else if( nChild1->GetReturnType() == XB_EXP_DATE ){
            xbDouble d1 = nChild1->GetNumericResult();
            xbDouble d2 = nChild2->GetNumericResult();
            if( d1 == XB_NULL_DATE ) d1 = 0;
            if( d2 == XB_NULL_DATE ) d2 = 0;
            n->SetResult((xbBool)( d1 > d2));
            //    n->SetResult((xbBool)(nChild1->GetNumericResult() > nChild2->GetNumericResult()));

          } else {
            iErrorStop = 410;
            iRc = XB_PARSE_ERROR;
            throw iRc;
          }
        }

        else if( sOperator == ">=" ){
          if( nChild1->GetReturnType() == XB_EXP_CHAR )
            n->SetResult((xbBool)(nChild1->GetStringResult() >= nChild2->GetStringResult()));

          else if( nChild1->GetReturnType() == XB_EXP_NUMERIC )
            n->SetResult((xbBool)(nChild1->GetNumericResult() >= nChild2->GetNumericResult()));

          else if( nChild1->GetReturnType() == XB_EXP_DATE  ){
            xbDouble d1 = nChild1->GetNumericResult();
            xbDouble d2 = nChild2->GetNumericResult();
            if( d1 == XB_NULL_DATE ) d1 = 0;
            if( d2 == XB_NULL_DATE ) d2 = 0;
            n->SetResult((xbBool)( d1 >= d2));
            //n->SetResult((xbBool)(nChild1->GetNumericResult() >= nChild2->GetNumericResult()));

          } else {
            iErrorStop = 420;
            iRc = XB_PARSE_ERROR;
            throw iRc;
          }
        }

        else if( sOperator == "<" ){

          if( nChild1->GetReturnType() == XB_EXP_CHAR ){
            n->SetResult((xbBool)( nChild1->GetStringResult() < nChild2->GetStringResult()));

          } else if( nChild1->GetReturnType() == XB_EXP_NUMERIC ){
            n->SetResult((xbBool)( nChild1->GetNumericResult() < nChild2->GetNumericResult()));

          } else if( nChild1->GetReturnType() == XB_EXP_DATE ){
            xbDouble d1 = nChild1->GetNumericResult();
            xbDouble d2 = nChild2->GetNumericResult();
            if( d1 == XB_NULL_DATE ) d1 = 0;
            if( d2 == XB_NULL_DATE ) d2 = 0;

            n->SetResult((xbBool)( d1 < d2));

            // std::cout << "xbexp() line 2567 [" << nChild1->GetNumericResult() << "][" << nChild2->GetNumericResult() << "]\n";

          } else {
            iErrorStop = 430;
            iRc = XB_PARSE_ERROR;
            throw iRc;
          }
        }

        else if( sOperator == "<=" ){

          if( nChild1->GetReturnType() == XB_EXP_CHAR )
            n->SetResult((xbBool)( nChild1->GetStringResult() <= nChild2->GetStringResult()));

          else if( nChild1->GetReturnType() == XB_EXP_NUMERIC )
            n->SetResult((xbBool)( nChild1->GetNumericResult() <= nChild2->GetNumericResult()));

          else if( nChild1->GetReturnType() == XB_EXP_DATE ){
            xbDouble d1 = nChild1->GetNumericResult();
            xbDouble d2 = nChild2->GetNumericResult();
            if( d1 == XB_NULL_DATE ) d1 = 0;
            if( d2 == XB_NULL_DATE ) d2 = 0;
            n->SetResult((xbBool)( d1 <= d2));
            // n->SetResult((xbBool)( nChild1->GetNumericResult() <= nChild2->GetNumericResult()));

          } else {
            iErrorStop = 440;
            iRc = XB_PARSE_ERROR;
            throw iRc;
          }
        }

        else if( sOperator == "<>" || sOperator == "#" || sOperator == "!=" ){

          if( nChild1->GetReturnType() == XB_EXP_CHAR )
            n->SetResult((xbBool)( nChild1->GetStringResult() != nChild2->GetStringResult()));

          else if( nChild1->GetReturnType() == XB_EXP_NUMERIC )
            n->SetResult((xbBool)( nChild1->GetNumericResult() != nChild2->GetNumericResult()));

          else if( nChild1->GetReturnType() == XB_EXP_DATE  ){
            xbDouble d1 = nChild1->GetNumericResult();
            xbDouble d2 = nChild2->GetNumericResult();
            if( d1 == XB_NULL_DATE ) d1 = 0;
            if( d2 == XB_NULL_DATE ) d2 = 0;
            n->SetResult((xbBool)( d1 != d2));
            // n->SetResult((xbBool)( nChild1->GetNumericResult() != nChild2->GetNumericResult()));

          } else {
            iErrorStop = 450;
            iRc = XB_PARSE_ERROR;
            throw iRc;
          }
        }

        else if( sOperator == "$" ){
          if( nChild1->GetReturnType() == XB_EXP_CHAR )
            if( nChild2->GetStringResult().Pos( nChild1->GetStringResult()) > 0 )
              n->SetResult((xbBool) xbTrue );
            else
              n->SetResult((xbBool) xbFalse );
          else {
            iErrorStop = 460;
            iRc = XB_INCOMPATIBLE_OPERANDS;
            throw iRc;
          }
        }

        else if( sOperator == "=" ){

          if( nChild1->GetReturnType() == XB_EXP_CHAR ){
            xbString sChld1 = nChild1->GetStringResult();
            xbString sChld2 = nChild2->GetStringResult();
            sChld1.Rtrim();
            sChld2.Rtrim();
            n->SetResult((xbBool)( sChld1 == sChld2 ));

          } else if( nChild1->GetReturnType() == XB_EXP_NUMERIC ){
            n->SetResult((xbBool)( nChild1->GetNumericResult() == nChild2->GetNumericResult()));

          } else if( nChild1->GetReturnType() == XB_EXP_DATE ){
            xbDouble d1 = nChild1->GetNumericResult();
            xbDouble d2 = nChild2->GetNumericResult();
            if( d1 == XB_NULL_DATE ) d1 = 0;
            if( d2 == XB_NULL_DATE ) d2 = 0;
            n->SetResult((xbBool)( d1 == d2));
            // n->SetResult((xbBool)( nChild1->GetNumericResult() == nChild2->GetNumericResult()));

          } else {
            iErrorStop = 470;
            iRc = XB_PARSE_ERROR;
            throw iRc;
          }


        } else {
          iErrorStop = 500;
          iRc = XB_PARSE_ERROR;
          throw iRc;
        }

        break;

      default:
        iErrorStop = 600;
        iRc = XB_PARSE_ERROR;
        throw iRc;
        // break;
      }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbexp::ProcessExpressionOperator() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
  }
  return iRc;
}

/*************************************************************************/
};     // namespace
#endif     // XB_EXPRESSION_SUPPORT
/*************************************************************************/
