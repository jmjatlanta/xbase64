/*  xbxbase.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/


#ifndef __XB_XBDBLIST_H__
#define __XB_XBDBLIST_H__

#ifdef  CMAKE_COMPILER_IS_GNUCC
#pragma interface
#endif

namespace xb{

class XBDLLEXPORT xbLog;

/************************************************************************/
/* 
    Xbase functions

   Min Parm Count is the minimum number of input parms needed by the function

   Return Type
       C        Char or string
       D        Date
       L        Logical
       N        Numeric
       1        Varies - pull return type from first sibling
       2        Varies - pull return type from second sibling
*/

#ifdef XB_FUNCTION_SUPPORT

/*
struct XBDLLEXPORT xbFunctionInfo {
  const   char * FuncName;          // function name
  char    ReturnType;               // return type of function
  xbInt16 iReturnLenCalc;           // used to calculate the function return value is
                                    // 1 = use value specified in iReturnLenVal
                                    // 2 = use length of operand specified in col 4
                                    // 3 = use valued of numeric operand specified in col 4
                                    // 4 = length of parm 1 * numeric value parm
                                    // 5 = larger length of parm 2 or length of parm 3
                                    // 6 = if two or more parms, use numeric value from second parm,
                                    //     otherwise use col4 value
  xbInt16 iReturnLenVal;            // Used in combination with iReturnLenCalc

};
*/
/*
static xbFunctionInfo FunctionList[] =
//    Func       Return  -Rtrn Len-
//    Name        Type   -Calc Val-
{
   { "ABS",       'N',      1,   4 },
   { "ALLTRIM",   'C',      2,   1 },
   { "ASC",       'N',      1,   4 },
   { "AT",        'N',      1,   4 },
   { "CDOW",      'C',      1,   9 },
   { "CHR",       'C',      1,   1 },
   { "CMONTH",    'C',      1,   9 },
   { "CTOD",      'D',      1,   8 },
   { "DATE",      'D',      1,   8 },
   { "DAY",       'N',      1,   4 },
   { "DEL",       'C',      1,   1 },
   { "DELETED",   'L',      1,   1 },
   { "DESCEND",   '1',      2,   1 },
   { "DOW",       'N',      1,   4 },
   { "DTOC",      'C',      1,   8 },
   { "DTOS",      'C',      1,   8 },
   { "EXP",       'N',      1,   4 },
   { "IIF",       'C',      5,   0 },
   { "INT",       'N',      1,   4 },
   { "ISALPHA",   'L',      1,   1 },
   { "ISLOWER",   'L',      1,   1 },
   { "ISUPPER",   'L',      1,   1 },
   { "LEFT",      'C',      3,   2 },
   { "LEN",       'N',      1,   4 },
   { "LOG",       'N',      1,   4 },
   { "LOWER",     'C',      2,   1 },
   { "LTRIM",     'C',      2,   1 },
   { "MAX",       'N',      1,   4 },
   { "MIN",       'N',      1,   4 },
   { "MONTH",     'N',      1,   4 },
   { "RECNO",     'N',      1,   4 },
   { "RECCOUNT",  'N',      1,   4 },
   { "REPLICATE", 'C',      4,   0 },
   { "RIGHT",     'C',      3,   2 },
   { "RTRIM",     'C',      2,   1 },
   { "SPACE",     'C',      3,   1 },
   { "SQRT",      'N',      1,   4 },
   { "STOD",      'D',      1,   8 },
   { "STR",       'C',      6,  10 },
   { "STRZERO",   'C',      3,   2 },
   { "SUBSTR",    'C',      3,   3 },
   { "TRIM",      'C',      2,   1 },
   { "UPPER",     'C',      2,   1 },
   { "VAL",       'N',      1,   4 },
   { "YEAR",      'N',      1,   4 },
   { 0, 0, 0, 0 },
};
*/
#endif



//! @brief xbXbase class.
/*!
  The xbXBase class is the core class that needs to be in every application program.
*/

class XBDLLEXPORT xbXBase : public xbTblMgr{
 public:
  xbXBase();
  ~xbXBase();

  xbInt16  CloseAllTables();
  xbInt16  CreateFqn( const xbString &sDirIn, const xbString &sNameIn, const xbString &sExtIn, xbString &sFqfnOut );
  void     DisableMsgLogging();
  void     EnableMsgLogging ();
  xbInt16  FlushLog();

  //const    xbString &GetLogDirectory () const;
  //const    xbString &GetLogFileName  () const;
  const    xbString &GetLogFqFileName() const;
  xbBool   GetLogStatus   () const;

  xbInt16  OpenHighestVersion( const xbString &sTableName, const xbString &sAlias, xbDbf &dbf, int dummy );
  xbInt16  OpenHighestVersion( const xbString &sTableName, const xbString &sAlias, xbDbf **dbf );

  xbDbf *  Open( const xbString &sTableName, xbInt16 &iRc );
  xbDbf *  Open( const xbString &sTableName, const xbString &sAlias, xbInt16 iOpenMode, xbInt16 iShareMode, xbInt16 iVersion, xbInt16 &iRc );

  // next three methods moved to xbssv for consistency
  // void     SetLogDirectory( const xbString &sLogFileDirectory );
  // void     SetLogFileName ( const xbString &sLogFileName );
  void     SetLogSize     ( size_t lSize );
  size_t   GetLogSize     () const;

  xbInt16  WriteLogMessage( const xbString &sLogMessage, xbInt16 iOutputOpt = 0 );
  xbInt16  WriteLogBytes  ( xbUInt32 lCnt, const char *p );

  void     xbSleep        ( xbInt32 lMillisecs );

  xbInt16  GetCmdLineOpt  ( xbInt32 lArgc, char **argv, const char *sOptRqst, xbString &sParmOut );
  xbInt16  GetCmdLineOpt  ( xbInt32 lArgc, char **argv, xbString &sOptRqst, xbString &sParmOut );


  /* xbase functions */
  #ifdef XB_FUNCTION_SUPPORT
  xbInt16 ABS( xbDouble dIn, xbDouble &dOut );
  xbInt16 ALLTRIM( const xbString &sIn, xbString &sOut );
  xbInt16 ASC( const xbString &s, xbDouble &dAscOut );
  xbInt16 AT( const xbString &sSrchFor, const xbString &sBase, xbDouble &dPos );
  xbInt16 CDOW( xbDate &dInDate, xbString &sOutDow );
  xbInt16 CHR( xbDouble dAsciCd, xbString &sOut );
  xbInt16 CMONTH( xbDate &dInDate, xbString &sOutMonth );
  xbInt16 CTOD( const xbString &sInDate, xbDate &dOutDate );
  xbInt16 DATE( xbDate &dOutDate );
  xbInt16 DAY( const xbDate &dInDate, xbDouble &dOutDay );
  xbInt16 DEL( xbDbf * d, xbString &sOut, xbInt16 iRecBufSw = 0 );
  xbInt16 DELETED( xbDbf * d, xbBool &bOut, xbInt16 iRecBufSw = 0 );
  xbInt16 DESCEND( const xbString &sIn,  xbString &sOut );
  xbInt16 DESCEND( const xbDate   &dInDate, xbDate &dOutDate );
  xbInt16 DESCEND( const xbDouble dIn,   xbDouble &dsOut );
  xbInt16 DOW( const xbDate &sInDate, xbDouble &dDowOut );
  xbInt16 DTOC( xbDate &dInDate, xbString &sOutFmtDate );
  xbInt16 DTOS( xbDate &dInDate, xbString &sOutFmtDate );
  xbInt16 EXP( xbDouble dIn, xbDouble &dOut );
  xbInt16 IIF( xbBool bResult, const xbString &sTrueResult, const xbString &sFalseResult, xbString &sResult );
  xbInt16 INT( xbDouble dIn, xbDouble &dOut );
  xbInt16 ISALPHA( const xbString &s, xbBool &bResult );
  xbInt16 ISLOWER( const xbString &s, xbBool &bResult );
  xbInt16 ISUPPER( const xbString &s, xbBool &bResult );
  xbInt16 LEFT( const xbString &sIn, xbUInt32 lCharCnt, xbString &sOut );
  xbInt16 LEN( const xbString &sIn, xbDouble &dLen );
  xbInt16 LOG( xbDouble dIn, xbDouble &dOut );
  xbInt16 LOWER( const xbString &sIn, xbString &sOut );
  xbInt16 LTRIM( const xbString &sIn, xbString & sOut );
  xbInt16 MAX( xbDouble dIn1, xbDouble dIn2, xbDouble &dOut );
  xbInt16 MIN( xbDouble dIn1, xbDouble dIn2, xbDouble &dOut );
  xbInt16 MONTH( xbDate &dInDate, xbDouble &dMonthOut );
  xbInt16 RECCOUNT( xbDbf * d, xbDouble &dRecOut );
  xbInt16 RECNO( xbDbf * d, xbDouble &dRecOut );
  xbInt16 REPLICATE( const xbString &sIn, xbUInt32 ulRepCnt, xbString &sOut );
  xbInt16 RIGHT( const xbString &sIn, xbUInt32 iCharCnt, xbString &sOut );
  xbInt16 RTRIM( const xbString &sIn, xbString &sOut );
  xbInt16 SPACE( xbInt32 lCnt, xbString &sOut );
  xbInt16 SQRT( xbDouble dBase, xbDouble &dSqrRt );
  xbInt16 STOD( const xbString &sIn, xbDate &sDateOut );
  xbInt16 STR( xbDouble dIn, xbString &sOut );
  xbInt16 STR( xbDouble dIn, xbUInt32 ulLen, xbString &sOut );
  xbInt16 STR( xbDouble dIn, xbUInt32 ulLen, xbUInt32 ulDec, xbString &sOut );
  xbInt16 STR( xbDouble dIn, xbUInt32 ulLen, xbUInt32 ulDec, xbString &sPadChar, xbString &sOut );
  xbInt16 STRZERO( xbDouble dIn, xbUInt32 ulLen, xbUInt32 ulDec, xbString &sOut );
  xbInt16 SUBSTR( const xbString &sIn, xbUInt32 ulStartPos, xbUInt32 ulLen, xbString &sOut );
  xbInt16 TRIM( const xbString &sIn, xbString &sOut );
  xbInt16 UPPER( const xbString &sIn, xbString &sOut );
  xbInt16 VAL( const xbString &sIn, xbDouble &dOut );
  xbInt16 YEAR( xbDate &dInDate, xbDouble &dOutYear );
  #endif

 protected:
  friend class xbBcd;
  friend class xbExp;

  #ifdef XB_BLOCKREAD_SUPPORT
  friend class xbBlockRead;
  #endif // XB_BLOCKREAD_SUPPORT


  xbInt16 GetFunctionInfo( const xbString &sExpLine, char &cReturnType, xbInt16 &iReturnLenCalc, xbInt32 &lReturnLenVal ) const;
  static xbInt16 xbMemcmp( const unsigned char *s1, const unsigned char *s2, size_t n );

 private:

  #ifdef XB_LOGGING_SUPPORT
  xbLog *xLog;
  #endif

};

}        /* namespace xb    */
#endif   /* __XB_DBLIST_H__ */