/* xb_test_funcs.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

// This program tests the xb functions
// usage:   xb_test_funcs QUITE|NORMAL|VERBOSE

#include "xbase.h"
using namespace xb;

#include "tstfuncs.cpp"


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
    { "NUM1",       XB_NUMERIC_FLD,   9, 2 },
    { "DATE1",      XB_DATE_FLD,      8, 0 },
    { "DATE2",      XB_DATE_FLD,      8, 0 },
    { "",0,0,0 }
  };


  xbXBase x;
  xbDbf * MyFile;
  xbDate d;

  xbString sResult;
  xbDate   dtResult;
  xbDouble dResult;
  xbBool   bResult;
  xbDate   dtIn( "19890209" );


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
  x.EnableMsgLogging();
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

  rc2 = MyFile->CreateTable( "Functest.DBF", "ExpTest", MyRecord, XB_OVERLAY, XB_MULTI_USER );
  iRc += TestMethod( po, "CreateTable()", rc2, XB_NO_ERROR );
  iRc += TestMethod( po, "PutField()",     MyFile->PutFloatField( "NUM1", 5 ),       XB_NO_ERROR );
  iRc += TestMethod( po, "PutField()",     MyFile->PutField( "DATE1", "19890303" ),  XB_NO_ERROR );
  iRc += TestMethod( po, "PutField()",     MyFile->PutField ( "DATE2", "20120708" ), XB_NO_ERROR );

//  #ifdef XB_LOCKING_SUPPORT
//  MyFile->SetAutoLock( xbFalse );
//  #endif  // XB_LOCKING_SUPPORT


  iRc += TestMethod( po, "AppendRecord()", MyFile->AppendRecord(),                   XB_NO_ERROR );

  iRc += TestMethod( po, "ABS( -222, dResult )", x.ABS( -222, dResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "ABS( -222, dResult )", -222, -222 );
  iRc += TestMethod( po, "ABS( 333, dResult )", x.ABS( 333, dResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "ABS( 333, dResult )", 333, 333 );

  iRc += TestMethod( po, "x.ALLTRIM( \"   zzz   \", sResult )", x.ALLTRIM( "   zzz   ", sResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.ALLTRIM( \"   zzz   \", sResult )", sResult, "zzz", 3 );

  iRc += TestMethod( po, "ASC( \"A\", dResult )", x.ASC( "A", dResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "ASC( \"A\" )", dResult, (xbDouble) 65 );
  iRc += TestMethod( po, "ASC( \"B\", dResult )", x.ASC( "B", dResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "ASC( \"B\" )", dResult, (xbDouble) 66 );
  iRc += TestMethod( po, "AT( \"ABC\", \"XYZABC\", 4 )",  x.AT( "ABC", "XYZABC", dResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "AT( \"ABC\", \"XYZABC\", 4 )",  dResult, (xbDouble) 4 );
  iRc += TestMethod( po, "CDOW( dtIn, sResult )", x.CDOW( dtIn, sResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "CDOW( dtIn, sResult )", sResult, "Thursday", 8 );
  iRc += TestMethod( po, "CHR( 101, sResult )", x.CHR( 101, sResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "CHR( 101, sResult )", sResult, "e", 1 );

  dtIn = "19870103";
  iRc += TestMethod( po, "CMONTH( dtIn, sResult )", x.CMONTH( dtIn, sResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "CMONTH( dtIn, sResult )", sResult, "January", 7 );
  iRc += TestMethod( po, "CTOD( \"01\\03\\87\", dtResult )", x.CTOD( "01\\03\\87", dtResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "CTOD( \"01\\03\\87\", dtResult )", dtResult.Str(), "19870103", 8 );
  iRc += TestMethod( po, "DATE( dtResult )", x.DATE( dtResult ), XB_NO_ERROR );

  d.Sysdate();
  iRc += TestMethod( po, "DATE( dtResult )", dtResult.Str(), d.Str(), 8 );
  iRc += TestMethod( po, "DAY(\"19870103\", dResult )", x.DAY( "19870103", dResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "DAY(\"19870103\", dResult )", dResult, (xbDouble) 3 );

  iRc += TestMethod( po, "DEL( MyFile, sResult )", x.DEL( MyFile, sResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "DEL( MyFile, sResult )", sResult, " ", 1 );

  iRc += TestMethod( po, "DELETED( MyFile, bResult )", x.DELETED( MyFile, bResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "DELETED( MyFile, bResult )", (xbInt32) bResult, xbFalse );

  iRc += TestMethod( po, "DeleteRecord()", MyFile->DeleteRecord(), XB_NO_ERROR );

  iRc += TestMethod( po, "DEL( MyFile, sResult )", x.DEL( MyFile, sResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "DEL( MyFile, sResult )", sResult, "*", 1 );
  iRc += TestMethod( po, "DELETED( MyFile, bResult )", x.DELETED( MyFile, bResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "DELETED( MyFile, bResult )", (xbInt32) bResult, xbTrue );

  iRc += TestMethod( po, "UndeleteRecord()", MyFile->UndeleteRecord(),  XB_NO_ERROR );

  iRc += TestMethod( po, "DESCEND(\"ABCDE\", sResult )", x.DESCEND( "ABCDE", sResult ), XB_NO_ERROR );

  char sDescendResult[6];  // = "¾½¼»º";
  sDescendResult[0] = (char) 0xBE;
  sDescendResult[1] = (char) 0xBD;
  sDescendResult[2] = (char) 0xBC;
  sDescendResult[3] = (char) 0xBB;
  sDescendResult[4] = (char) 0xBA;
  sDescendResult[5] = (char) 0x00;
  iRc += TestMethod( po, "DESCEND(\"ABCDE\", sResult )", sResult, sDescendResult, 5 );

  iRc += TestMethod( po, "DESCEND( 12345, dResult )", x.DESCEND( 12345, dResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "DESCEND( 12345, dResult )", dResult, (xbDouble) -12345 );
  dtIn = "19890303";
  iRc += TestMethod( po, "DESCEND( dtIn, dtResult )", x.DESCEND( dtIn, dtResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "DESCEND( dtIn, dtResult )", dtResult.Str(), "29101031", 8 );
  dtIn = "20120708";
  iRc += TestMethod( po, "DESCEND( dtIn, dtResult )", x.DESCEND( dtIn, dtResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "DESCEND( dtIn, dtResult )", dtResult.Str(), "28870625" , 8 );

  dtIn = "20171014";
  iRc += TestMethod( po, "DOW( dtIn, dResult )", x.DOW( dtIn, dResult ),  XB_NO_ERROR );
  iRc += TestMethod( po, "DOW(\"20171015\", dResult )", dResult, (xbDouble) 0 );
  dtIn++;
  iRc += TestMethod( po, "DOW( dtIn, dResult )", x.DOW( dtIn, dResult ),  XB_NO_ERROR );
  iRc += TestMethod( po, "DOW(\"20171016\", dResult )", dResult, (xbDouble) 1 );
  dtIn++;
  iRc += TestMethod( po, "DOW( dtIn, dResult )", x.DOW( dtIn, dResult ),  XB_NO_ERROR );
  iRc += TestMethod( po, "DOW(\"20171021\", dResult )", dResult, (xbDouble) 2 );

  dtIn = "20000101";
  iRc += TestMethod( po, "DTOC( dtIn, sResult )", x.DTOC( dtIn, sResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "DTOC( dtIn, sResult )", sResult, "01/01/00", 8 );
  iRc += TestMethod( po, "DTOS( dtIn, sResult )", x.DTOS( dtIn, sResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "DTOS( dtIn, sResult )", sResult, "20000101", 8 );
  iRc += TestMethod( po, "EXP( 1, dResult )", x.EXP( 1, dResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "EXP( 1, dResult )", dResult, 2.71828, .001 );
  iRc += TestMethod( po, "x.INT( 621.5, dResult )", x.INT( 621.5, dResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.INT( 621.5, dResult )", dResult, (xbDouble) 621 );
  iRc += TestMethod( po, "x.ISALPHA( \"1\", bResult )", x.ISALPHA( "1", bResult ), 0 );
  iRc += TestMethod( po, "x.ISALPHA( \"1\", bResult )", (xbInt32) bResult, xbFalse );
  iRc += TestMethod( po, "x.ISALPHA( \"A\", bResult )", x.ISALPHA( "A", bResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.ISALPHA( \"A\", bResult )", (xbInt32) bResult, xbTrue );
  iRc += TestMethod( po, "x.ISLOWER( \"A\", bResult )", x.ISLOWER( "A", bResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.ISLOWER( \"A\", bResult )", bResult, xbFalse );
  iRc += TestMethod( po, "x.ISLOWER( \"a\", bResult )", x.ISLOWER( "a", bResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.ISLOWER( \"a\", bResult )", bResult, xbTrue );
  iRc += TestMethod( po, "x.ISUPPER( \"A\", bResult )", x.ISUPPER( "A", bResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.ISUPPER( \"A\", bResult )", bResult, xbTrue );
  iRc += TestMethod( po, "x.ISUPPER( \"a\", bResult )", x.ISUPPER( "a", bResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.ISUPPER( \"a\", bResult )", bResult, xbFalse );
  iRc += TestMethod( po, "x.LEFT( \"STRING\", 3, sResult )", x.LEFT( "STRING", 3, sResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.LEFT( \"STRING\", 3, sResult )", sResult, "STR", 3 );
  iRc += TestMethod( po, "x.LEN( \"AAAAA\", dResult )", x.LEN( "AAAAA", dResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.LEN( \"AAAAA\", dResult )", dResult, (xbDouble) 5 );
  iRc += TestMethod( po, "x.LOG( 2, dResult )", x.LOG( 2, dResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.LOG( 2, dResult )", dResult, (xbDouble) 0.69314700, .0001 );
  iRc += TestMethod( po, "x.LOWER( \"AAAA\", sResult )", x.LOWER( "AAAA", sResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.LOWER( \"AAAA\", sResult )", sResult, "aaaa", 4 );
  iRc += TestMethod( po, "x.LTRIM( \"   xxxxxx\" )", x.LTRIM( "   xxxxxx", sResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.LTRIM( \"   xxxxxx\" )", sResult, "xxxxxx", 6 );
  iRc += TestMethod( po, "x.MAX( 10, 27, dResult )", x.MAX( 10, 27, dResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.MAX( 10, 27, dResult )", dResult, (xbDouble) 27 );
  iRc += TestMethod( po, "x.MIN( 10, 5, dResult )", x.MIN( 10, 5, dResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.MIN( 10, 5, dResult )", dResult, (xbDouble) 5 );
  iRc += TestMethod( po, "x.MONTH( dtIn, dResult )", x.MONTH( dtIn, dResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.MONTH( dtIn, dResult )", dResult, (xbDouble) 1 );
  iRc += TestMethod( po, "x.RECCOUNT( MyFile, dResult)", x.RECNO( MyFile, dResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.RECCOUNT( MyFile, dResult)", dResult, (xbDouble) 1 );
  iRc += TestMethod( po, "x.RECNO( MyFile, dResult)", x.RECNO( MyFile, dResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.RECNO( MyFile, dResult)", dResult, (xbDouble) 1 );
  iRc += TestMethod( po, "x.REPLICATE( \"abc\", 3, sResult )", x.REPLICATE( "abc", 3, sResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.REPLICATE( \"abc\", 3, sResult )", sResult, "abcabcabc", 9 );
  iRc += TestMethod( po, "x.RIGHT( \"STRING\", 3, sResult )", x.RIGHT( "STRING", 3, sResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.RIGHT( \"STRING\", 3, sResult )", sResult, "ING", 3 );
  iRc += TestMethod( po, "x.RIGHT( \"STRING\", 3, sResult )", x.RIGHT( "STRING", 5, sResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.RIGHT( \"STRING\", 3, sResult )", sResult, "TRING", 5 );
  iRc += TestMethod( po, "x.RIGHT( \"STRING\", 3, sResult )", x.RIGHT( "STRING", 6, sResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.RIGHT( \"STRING\", 3, sResult )", sResult, "STRING", 6 );
  iRc += TestMethod( po, "x.RIGHT( \"STRING\", 3, sResult )", x.RIGHT( "STRING", 7, sResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.RIGHT( \"STRING\", 3, sResult )", sResult, "STRING", 6 );
  iRc += TestMethod( po, "x.RTRIM( \"zzz   \", sResult )", x.RTRIM( "zzz   ", sResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.RTRIM( \"zzz   \", sResult )", sResult, "zzz", 3 );
  iRc += TestMethod( po, "x.TRIM( \"aaa   \", sResult )", x.TRIM( "aaa   ", sResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.TRIM( \"aaa   \", sResult )", sResult, "aaa", 3 );

  iRc += TestMethod( po, "x.SPACE( 3, sResult )", x.SPACE( 3, sResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.SPACE( 3, sResult )", sResult, "   ", 3 );
  iRc += TestMethod( po, "x.SQRT( 9, dResult )", x.SQRT( 9, dResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.SQRT( 9, dResult )", dResult, (xbDouble) 3 );
  iRc += TestMethod( po, "x.STOD( \"20000101\", dtResult )", x.STOD( "20000101", dtResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.STOD( \"20000101\", dtResult )",dtResult.Str(), "20000101", 8 );

  xbString sPadChar = " ";
  iRc += TestMethod( po, "x.STR( 2001, 4, 0, sPadChar, sResult )", x.STR( 2001, 4, 0, sPadChar, sResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.STR( 2001, 4, 0, sPadChar, sResult )", sResult, "2001", 4 );
  iRc += TestMethod( po, "x.STR( 2002, 3, 0, sPadChar, sResult )", x.STR( 2002, 3, 0, sPadChar, sResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.STR( 2002, 3, 0, sPadChar, sResult )", sResult, "***", 3 );
  iRc += TestMethod( po, "x.STR( 203.2, 6, 2, sPadChar, sResult )", x.STR( 203.2, 6, 2, sPadChar, sResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.STR( 203.2, 6, 2, sPadChar, sResult )", sResult, "203.20", 6 );
  iRc += TestMethod( po, "x.STR( 204.11, 8, 2, sPadChar, sResult )", x.STR( 204.11, 8, 2, sPadChar, sResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.STR( 204.11, 8, 2, sPadChar, sResult )", sResult, "  204.11", 8 );
  iRc += TestMethod( po, "x.STR( -205.45, 8, 2, sPadChar, sResult )", x.STR( -205.45, 8, 2, sPadChar, sResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.STR( -205.45, 8, 2, sPadChar, sResult )", sResult, " -205.45", 8 );
  iRc += TestMethod( po, "x.STR( -306.45, 8, 2, sPadChar, sResult )", x.STR( -306.45, 8, 2, sPadChar, sResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.STR( -306.45, 8, 2, sPadChar, sResult )", sResult, " -306.45", 8 );
  iRc += TestMethod( po, "x.STR( 6.56, 5, 0, sResult )", x.STR( 6.56, 5, 0, sResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.STR( 6.56, 5, 0, sResult )", sResult, "    7", 5 );
  iRc += TestMethod( po, "x.STR( 7.77, 5, sResult )", x.STR( 7.77, 5, sResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.STR( 7.77, 5, sResult )", sResult, "    8", 5 );
  iRc += TestMethod( po, "x.STR( 8, sResult )", x.STR( 8, sResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.STR( 8, sResult )", sResult, "         8", 10 );

  sPadChar = "0";
  iRc += TestMethod( po, "x.STR( -5.2, 10, 2, sPadChar, sResult )", x.STR( -5.2, 10, 2, sPadChar, sResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.STR( -5.2, 10, 2, sPadChar, sResult )", sResult, "00000-5.20", 10 );
  iRc += TestMethod( po, "x.STRZERO( 8, 8, 0, sResult )", x.STRZERO( 8, 8, 0, sResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.STRZERO( 8, 8, 0, sResult )", sResult, "00000008", 8 );
  iRc += TestMethod( po, "x.STRZERO( -8, 8, 0, sResult )", x.STRZERO( -8, 8, 0, sResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.STRZERO( -8, 8, 0, sResult )", sResult, "-0000008", 8 );
  iRc += TestMethod( po, "x.STRZERO( -205.45, 10, 3 )", x.STRZERO( -205.45, 10, 3, sResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.STRZERO( -205.45, 10, 3 )", sResult, "-00205.450", 10 );
  iRc += TestMethod( po, "x.STRZERO( -205.45, 3, 1 )", x.STRZERO( -205.45, 3, 1, sResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.STRZERO( -205.45, 3, 1 )", sResult, "***", 3 );
  iRc += TestMethod( po, "x.SUBSTR( \"TESTSTRING\", 5, 2, sResult )", x.SUBSTR( "TESTSTRING", 5, 2, sResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.SUBSTR( \"TESTSTRING\", 5, 2, sResult )", sResult, "ST", 2 );
  iRc += TestMethod( po, "x.UPPER( \"abababa\", sResult )", x.UPPER( "abababa", sResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.UPPER( \"abababa\", sResult )", sResult, "ABABABA", 7 );
  iRc += TestMethod( po, "x.VAL( \"65\", dResult )", x.VAL( "65", dResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.VAL( \"65\", dResult )", dResult, (xbDouble) 65 );
  iRc += TestMethod( po, "x.YEAR( dtIn, dResult )", x.YEAR( dtIn, dResult ), XB_NO_ERROR );
  iRc += TestMethod( po, "x.YEAR( dtIn, dResult )", dResult, (xbDouble) 2000 );


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
