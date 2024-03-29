/*  xbexp.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/


#ifndef __XB_EXP_H__
#define __XB_EXP_H__


// #ifdef  CMAKE_COMPILER_IS_GNUCC
// #pragma interface
// #endif

#define XB_NULL_DATE 21474835648


#ifdef XB_FUNCTION_SUPPORT
#define XB_EXP_CHAR      'C'
#define XB_EXP_DATE      'D'
#define XB_EXP_LOGICAL   'L'
#define XB_EXP_NUMERIC   'N'
#define XB_EXP_UNKNOWN   'U'
#endif

#ifdef XB_EXPRESSION_SUPPORT

#define XB_EXP_CONSTANT      'C'
#define XB_EXP_FUNCTION      'F'
#define XB_EXP_FIELD         'D'
#define XB_EXP_OPERATOR      'O'
#define XB_EXP_NOTROOT       'N'    // not root node, needs further parsing
#define XB_EXP_PRE_OPERATOR  'B'    // (B)efore) pre increment, pre decrement
#define XB_EXP_POST_OPERATOR 'A'    // (A)fter)  post increment, pre decrement

#define XB_END_OF_EXPRESSION -100

// #define XB_UNBALANCED_PARENS -101
// #define XB_UNBALANCED_QUOTES -102


namespace xb{

///@cond DOXYOFF
struct XBDLLEXPORT xbExpToken {
  xbString sExpression;      // in - expression to pull next token from
                             // out - remainder of the expression after token removed
  xbString sToken;           // next token pulled from the expression
  char     cNodeType;        // one of XB_EXP_CONSTANT, XB_EXP_FUNCTION, XB_EXP_FIELD, XB_EXP_OPERATOR, XB_EXP_NOTROOT
  char     cReturnType;      // one of XB_EXP_CHAR, XB_EXP_DATE, XB_EXP_LOGICAL, XB_EXP_NUMERIC, XB_EXP_UNKNOWN
  xbInt16  iSts;             // return status after retrieving or attempting next token from expression
                             // 0 = no error
                             // XB_END_OF_EXPRESSION
                             // XB_UNBALANCED_PARENS
                             // XB_UNBALANCED_QUOTES
  char     cPrevNodeType;    // previous node type
  char     cPrevReturnType;  // previous return type

  // constructor
  xbExpToken() { cNodeType = 0; cReturnType = 0; iSts = 0; cPrevNodeType = 0; cPrevReturnType = 0; }
};
///@endcond DOXYOFF


/************************************************************************/

//! @brief Class for handling expressions.
/*!
The xbExp class is used for parsing and evaluating expression.

The Xbase64 library includes an expression parsing module which assists 
application programmers by providing a high level data manipulation tool and 
also allows for building complex index keys.

The functions included were derived from dBASE III Plus, Dbase IV and Clipper.<br><br>

<h3>Internal fuctioning</h3>
The expression module works in two phases.  Firstly, method 
<em>ParseExpression</em> is called and builds an expression tree from 
all the components of the expression. The tree is made up of individual 
nodes. The expression is checked for valid field names, literals, 
operands and functions.  Any field references are resolved.  If fields 
are used in an expression and the database name for the field is not 
included in the name with the -> operand, the routines assume the 
associated database has been successfully opened. 
<br>
Secondly, method <em>ProcessExpression</em> is called to process the 
expression tree created by ParseExpression().  The routine parses each 
node in the expression tree, executing functions, processing operands 
and manipulating data to produce the desired result.<br>

If an expression will be processed repeatedly, it is best to pre-parse the
tree using <em>ParseExpression</em>, then for each new call to the expression,
execute method <em>ProcessExpression</em> which processes the tree.<br><br>

<h3>Expression Return Types</h3>
Expressions will return a type of CHAR, NUMERIC, DATE or LOGICAL.<br>

An expression return type can be determined with method <em>
GetExpressionResultType</em> after parsing it.<br>

Expressions returning a return type of CHAR are limited to a 200 byte internal 
buffer.  There is also a 100 byte limit for NDX index key support.  If 
the 200 byte limit is not large enough for your application, adjust field
<em>enum { WorkBufMaxLen = 200 };</em> in file <em>exp.h</em>.<br><br>

<h3>Expression Functions</h3>
Each expression function also has a corresponding C++ function.  It is
slightly more efficient to call the C++ functions directly, rather than 
execute the expression parsing routines.<br><br>

<h3>Expression Components</h3>
Expressions are made up of one or more tokens.  A token is one of literal, 
database field, operand or function.  Literals are either numeric or character.
Character literals are enclosed in 'single' or "double" quotes.  numeric 
literals are a series of one or more contiguous numerals, ".", "+" or "-'".
<br><br>
A field is simply a field name in the default database, or is in the form
of database->fieldname.

*/

class XBDLLEXPORT xbExp{

  public:
    xbExp( xbXBase * );
    xbExp( xbXBase *, xbDbf * );
    virtual ~xbExp();
    void ClearTreeHandle();


    #ifdef XB_DEBUG_SUPPORT
    void      DumpTree( xbInt16 iOption );
    void      DumpToken( xbExpToken &t, xbInt16 iOption = 0 );
    #endif

    xbInt16   GetResultLen() const;
    char      GetReturnType() const;
    xbInt16   GetBoolResult( xbBool &bResult );
    xbInt16   GetDateResult( xbDate &dtResult );
    xbInt16   GetNumericResult( xbDouble &dResult );
    xbInt16   GetStringResult( xbString &sResult );
    xbInt16   GetStringResult( char * vpResult, xbUInt32 ulLen );
    xbExpNode *GetTreeHandle();
    xbInt16   ParseExpression( const xbString &sExpression );
    xbInt16   ParseExpression( xbDbf *dbf, const xbString &sExpression );
    xbInt16   ProcessExpression();
    xbInt16   ProcessExpression( xbInt16 iRecBufSw );


  protected:
    xbInt16   GetNextToken( xbExpToken &t );
    xbInt16   OperatorWeight( const xbString &sOperator );
    xbExpNode *GetNextNode( xbExpNode * n ) const;  // traverses the tree from bottom left node, right, then up

  private:    // methods

    // xbInt16 CalcExpressionResultLen();
    xbInt16 CalcFunctionResultLen( xbExpNode *n ) const;
    xbInt16 CalcCharNodeLen( xbExpNode *n );
    xbInt16 CheckParensAndQuotes( const xbString &sExpression );
    xbInt16 GetExpressionResultLen() const;

    xbInt16 GetTokenCharConstant   ( xbExpToken &t );
    xbInt16 GetTokenDatabaseField  ( xbExpToken &t );
    xbInt16 GetTokenDateConstant   ( xbExpToken &t );
    xbInt16 GetTokenFunction       ( xbExpToken &t );
    xbInt16 GetTokenLogicalConstant( xbExpToken &t );
    xbInt16 GetTokenNumericConstant( xbExpToken &t );
    xbInt16 GetTokenOperator       ( xbExpToken &t );
    xbInt16 GetTokenParen          ( xbExpToken &t );

    xbBool  IsFunction             ( const xbString &sExp, char &cReturnType );
    xbBool  IsLogicalConstant      ( const xbString &sExp );
    xbBool  IsNumericConstant      ( const xbString &sExp, char cPrevNodeType );
    xbBool  IsOperator             ( const xbString &sExp );
    char    IsTokenSeparator       ( char c );
    xbBool  IsWhiteSpace           ( char c );

    xbInt16 ParseExpression        ( const xbString &sExpression, xbInt16 iWeight );
    xbInt16 ParseExpressionConstant( xbExpToken &t, xbExpNode *n );
    xbInt16 ParseExpressionFunction( xbExpToken &t, xbExpNode *n, xbInt16 iWeight );
    xbInt16 ParseExpressionFunctionParms( const xbString &sParms, xbLinkList<xbString> &llParms );
    xbInt16 ParseExpressionField   ( xbExpToken &t, xbExpNode *n );
    xbInt16 ParseExpressionOperator( xbExpToken &t, xbExpNode *n, xbInt16 iWeight );

    xbInt16 ProcessExpressionFunction( xbExpNode *n, xbInt16 iRecBufSw = 0 );
    xbInt16 ProcessExpressionOperator( xbExpNode *n );

  private:   // fields
    xbXBase   *xbase;
    xbDbf     *dbf;
    xbExpNode *nTree;        // pointer to tree of expNodes
    // xbInt16   iExpLen;       // size of expression result


};

/* Expression handler */


};
#endif        // XB_EXPRESSION_SUPPORT
#endif        // __XB_EXP_H__


