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

See program /source/examples/xb_ex_expression.cpp for examples of how to use this functionality.

*/

class XBDLLEXPORT xbExp{

  public:

   //! Constructor
   /*!
      @param x Pointer to xbXBase instance.
   */
    xbExp( xbXBase * x );

    //! @brief Constructor
    /*!
     @param x Pointer to xbXBase instance.
     @param dbf Pointer to xbDbf instance.
    */
    xbExp( xbXBase * x, xbDbf * dbf );

    //! @brief Destructor.
    virtual ~xbExp();

    //! @brief Clear tree handle.
    /*!
      This routine clears the expression tree of nodes and frees any associated memory.
    */
    void ClearTreeHandle();


    #ifdef XB_DEBUG_SUPPORT

    //! @brief Dump the tree of nodes and their contents.
    /*!
       @param iOption - Output option.
       <table border>
       <tr><td>1</td><td>Write to logfile</td></tr>
       <tr><td>2</td><td>Write to stdout</td></tr>
       <tr><td>3</td><td>Write to logfile and stdout</td></tr>
       </table>

       @note Available if XB_DEBUG_SUPPORT option compiled into the library.
    */
    void      DumpTree( xbInt16 iOption );
    //! @brief Dump token contents
    /*!
       @param pNode - Pointer to head node of the tree
       @param iOption - Output option.
       <table border>
       <tr><td>1</td><td>Write to logfile</td></tr>
       <tr><td>2</td><td>Write to stdout</td></tr>
       <tr><td>3</td><td>Write to logfile and stdout</td></tr>
       </table>

       @note Available if XB_DEBUG_SUPPORT option compiled into the library.
    */
    void      DumpToken( xbExpToken &pNode, xbInt16 iOption = 0 );
    #endif

    //! @brief Get result length.
    /*!
       This routine returns the result length.
       @returns Result length.
    */
    xbInt16   GetResultLen() const;

    //! @brief Get return type.
    /*!
       @returns One of:<br>
                XB_EXP_CHAR<br>
                XB_EXP_DATE<br>
                XB_EXP_LOGICAL<br>
                XB_EXP_NUMERIC
    */
    char      GetReturnType() const;

    //! @brief Get bool result.
    /*!
       If the expression generates a boolean return type, this method retrieves the boolean value.
       @param bResult - Output boolean value. 
       @returns <a href="xbretcod_8h.html">Return Codes</a>
    */
    xbInt16   GetBoolResult( xbBool &bResult );

    //! @brief Get date result.
    /*!
       If the expression generates a date return type, this method retrieves the date value.
       @param dtResult - Output date value.
       @returns <a href="xbretcod_8h.html">Return Codes</a>
    */
    xbInt16   GetDateResult( xbDate &dtResult );

    //! @brief Get numeric result.
    /*!
       If the expression generates a numeric return type, this method retrieves the numeric value.
       @param dResult - Output numeric value.
       @returns <a href="xbretcod_8h.html">Return Codes</a>
    */
    xbInt16   GetNumericResult( xbDouble &dResult );

    //! @brief Get string result.
    /*!
       If the expression generates a string return type, this method retrieves the string value.
       @param sResult - Output string value.
       @returns <a href="xbretcod_8h.html">Return Codes</a>
    */
    xbInt16   GetStringResult( xbString &sResult );

    //! @brief Get string result.
    /*!
      If the expression generates a string return type, this method retrieves the string value.
      @param vpResult - Pointer to user supplied buffer for result.
      @param ulLen - Max size of buffer.
      @returns <a href="xbretcod_8h.html">Return Codes</a>
    */
    xbInt16   GetStringResult( char * vpResult, xbUInt32 ulLen );

    //! @brief Get the expression tree handle.
    /*!
       @private
       @returns Pointer to the top most node in the expression tree.
    */
    xbExpNode *GetTreeHandle();

    //! @brief Parse expression.
    /*!
       @param sExpression - Expression to parse.
       @returns <a href="xbretcod_8h.html">Return Codes</a>
    */
    xbInt16   ParseExpression( const xbString &sExpression );

    //! @brief Parse expression.
    /*!
       @param dbf - Pointer to xbDbf instance.
       @param sExpression - Expression to parse.
       @returns <a href="xbretcod_8h.html">Return Codes</a>
    */
    xbInt16   ParseExpression( xbDbf *dbf, const xbString &sExpression );

    //! @brief ProcessExpression
    /*! This method processes an expression tree leaving the result in the
        root node of the tree
       @returns <a href="xbretcod_8h.html">Return Codes</a>
    */
    xbInt16   ProcessExpression();

    //! @brief ProcessExpression
    /*! This method processes a parsed expression tree leaving the result in the
        root node of the tree

        @param iRecBufSw Record buffer to use when evaluating expression.<br>
                   0 - Current record buffer.<br>
                   1 - Original record buffer.
    */
    xbInt16   ProcessExpression( xbInt16 iRecBufSw );

  protected:


    //! @brief GetNextToken
    /*!
        @private
        This method returns the next token in an expression of one or more tokens
        @param t Token
        @returns <a href="xbretcod_8h.html">Return Codes</a>
    */
    xbInt16   GetNextToken( xbExpToken &t );


    //! @brief Get operator weight.
    /*!
       @private
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

         @param sOper - Operator.
         @returns Operator weight
    */
    xbInt16   OperatorWeight( const xbString &sOperator );

    //! @brief Get the next node in the tree.
    /*!
       @private
       @param n Node to starting point.  To get the first node of the entire tree, set n = NULL
       @returns Pointer to next node.
    */

    xbExpNode *GetNextNode( xbExpNode * n ) const;  // traverses the tree from bottom left node, right, then up

  private:    // methods

    // xbInt16 CalcExpressionResultLen();

    //! @brief Calulate expression return length
    /*!
       @private
       This function returns the maximum possible length of an expression
       The create index functions use this for determining the fixed length keys 
       It sets the return length field in the node.

       @param n Start node
       @returns XB_NO_ERROR<br>XB_PARSE_ERROR
    */
    xbInt16 CalcFunctionResultLen( xbExpNode *n ) const;
    xbInt16 CalcCharNodeLen( xbExpNode *n );

    //! @brief Check parens and quotes
    /*!
       @private
       This routine looks for unbalanced parens and quotes

       @param sExpression Expression to examine.
       @returns <a href="xbretcod_8h.html">Return Codes</a>
    */
    xbInt16 CheckParensAndQuotes( const xbString &sExpression );
    xbInt16 GetExpressionResultLen() const;

    //! @brief GetTokenCharConstant
    /*! @private
        This method returns the character constant in a pair of quotes

        This routine returns the tokens inside a set of matching quotes in sOutToken
        If there is nothing between the quotes then sOutToken is returned empty
        sOutRemainder contains whatever remains to the right of the right quote

        @param t Token
        @returns <a href="xbretcod_8h.html">Return Codes</a>
    */

    //! GetTokenCharConstant
    /*!
        @private
        This method returns the character constant in a pair of quotes

        This routine returns the tokens inside a set of matching quotes in sOutToken
        If there is nothing between the quotes then sOutToken is returned empty
        sOutRemainder contains whatever remains to the right of the right quote

        @param t Token
        @returns <a href="xbretcod_8h.html">Return Codes</a>
    */
    xbInt16 GetTokenCharConstant   ( xbExpToken &t );

    //! @brief GetTokenField
    /*!
       @private
       This method gets a database field token 

       Looks for a xbase field in one of the following formats

       FIELDNAME
          or
       TABLENAME->FIELDNAME

       @param t Token.
       @returns <a href="xbretcod_8h.html">Return Codes</a>
    */
    xbInt16 GetTokenDatabaseField  ( xbExpToken &t );

    //! @brief GetTokenDateConstant
    /*! 
       @private
       This method returns the date constant in a pair of {}

       Date format is one of {mm/dd/yy}   or  {mm/dd/yyyy}
       @param t Token.
       @returns <a href="xbretcod_8h.html">Return Codes</a>
    */
    xbInt16 GetTokenDateConstant   ( xbExpToken &t );

    //! @brief GetTokenFunction
    /*!
      @private
      This method gets a function and everything between the following quotes
      @param t Token
      @returns <a href="xbretcod_8h.html">Return Codes</a>
    */
    xbInt16 GetTokenFunction       ( xbExpToken &t );

    xbInt16 GetTokenLogicalConstant( xbExpToken &t );

    //! @brief GetTokenNumericConstant
    /*!
       @private
       This method returns a numeric constant in 

       This routine returns a numeric constant token
       sOutRemainder contains whatever remains to the right of the right quote

       @param t Token
       @returns <a href="xbretcod_8h.html">Return Codes</a>
    */
    xbInt16 GetTokenNumericConstant( xbExpToken &t );

    //! @brief GetTokenOperator
    /*!
       @private
       This method returns the operator

       @param t Token
       @returns <a href="xbretcod_8h.html">Return Codes</a>
    */
    xbInt16 GetTokenOperator       ( xbExpToken &t );

    //! @brief GetTokenParen
    /*!
       @private
       This method returns the tokens in a pair of enclosed parens

       This routine returns the tokens inside a set of matching parens in sOutToken
       If there is nothing between the parens then sOutToken is returned empty
       sOutRemainder contains whatever remains to the right of the right paren

       @param t Token
       @returns <a href="xbretcod_8h.html">Return Codes</a>
    */
    xbInt16 GetTokenParen          ( xbExpToken &t );

   //! @brief Is Function
   /*!
      @private
      This method determines if the next token is a function.

      @param sExpression - String expression to be evaluated.
      @param cReturnType Output - Return type.
      @returns xbTrue - Is a function.<br>
               xbFalse - Is not a function.
    */
    xbBool  IsFunction             ( const xbString &sExp, char &cReturnType );

    //! @brief Is Logical constant
    /*!
       @private
       This method determines if the next token is a logical constant (T/F, etc).

       @param sExpression - String expression to be evaluated.
       @returns xbTrue - Is a logical constant.<br>
                xbFalse - Is not a logical constant.
    */
    xbBool  IsLogicalConstant      ( const xbString &sExp );

    //! @brief Is Numeric constant
    /*!
       @private
       This method determines if the next token is a numeric constant.

       @param sExpression - String expression to be evaluated.
       @param cPrevNodeType - Type of previous node.
       @returns xbTrue - Is a numeric constant.<br>
                xbFalse - Is not a numeric constant.
    */
    xbBool  IsNumericConstant      ( const xbString &sExp, char cPrevNodeType );

    //! @brief Is Operator.
    /*!
       @private
       This method determines if the next token is an operator.

       @param sExpression - String expression to be evaluated.
       @returns xbTrue - Is an operator.<br>
                xbFalse - Is not an operator.
    */
    xbBool  IsOperator             ( const xbString &sExp );

    //! @brief Is Token separator
    /*!
       @private
       This method determines if the next token is a separator.

       @param sExpression - String expression to be evaluated.
       @returns xbTrue - Is a token separator.<br>
                xbFalse - Is not a token separator.
    */
    char    IsTokenSeparator       ( char c );


    //! @brief Is White space
    /*!
       @private
       This method determines if a given character is white space.

       @param c - Character to be evaluated.
       @returns xbTrue - Is white space.<br>
                xbFalse - Is not white space.
    */
    xbBool  IsWhiteSpace           ( char c );

    //! @brief Parse expression.
    /*!
       @private
       @param sExpression - Expression to parse.
       @param iWeight.
       @returns <a href="xbretcod_8h.html">Return Codes</a>
    */
    xbInt16 ParseExpression        ( const xbString &sExpression, xbInt16 iWeight );


    //! @brief Parse expression constant.
    /*!
       @private
       @param t - Token.
       @param n - Node.
       @returns XB_NO_ERROR<br>XB_PARSE_ERROR
    */
    xbInt16 ParseExpressionConstant( xbExpToken &t, xbExpNode *n );

    //! @brief Parse expression function.
    /*!
       @private
       @param t - Token.
       @param n - Node.
       @param iWeight
       @returns XB_NO_ERROR<br>XB_INVALID_FUNCTION
    */
    xbInt16 ParseExpressionFunction( xbExpToken &t, xbExpNode *n, xbInt16 iWeight );

    //! @brief Parse expression function.
    /*!
       @private

       Creates a linked list of function parms as xbStrings
       This function pulls out the parms and addresses embedded parens and quotes within the parms

       @param sParms
       @param lParms
       @returns <a href="xbretcod_8h.html">Return Codes</a>
    */
    xbInt16 ParseExpressionFunctionParms( const xbString &sParms, xbLinkList<xbString> &llParms );


    //! @brief Parse expression field.
    /*!
       @private
       @param t - Token.
       @param n - Node.
       @returns <a href="xbretcod_8h.html">Return Codes</a>
    */
    xbInt16 ParseExpressionField   ( xbExpToken &t, xbExpNode *n );

    //! @brief Parse expression operator.
    /*!
       @private
       @param t - Token.
       @param n - Node.
       @param iWeight
       @returns XB_NO_ERROR
    */
    xbInt16 ParseExpressionOperator( xbExpToken &t, xbExpNode *n, xbInt16 iWeight );

    //! @brief ProcessExpression
    /*! This method processes an expression tree for a given node.
      @returns XB_NO_ERROR<br>
               XB_INCONSISTENT_PARM_LENS<br>
               XB_PARSE_ERROR
    */
    xbInt16 ProcessExpressionFunction( xbExpNode *n, xbInt16 iRecBufSw = 0 );


    //! @brief Process Expression Operator
    /*! This method processes an expression operator for a given node.
      @returns XB_NO_ERROR<br>
               XB_INCOMPATIBLE_OPERANDS<br>
               XB_PARSE_ERROR
    */
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


