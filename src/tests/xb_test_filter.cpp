/* xb_test_filter.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2020,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

// This program tests the class xbFilter

// usage:   xb_test_filter QUITE|NORMAL|VERBOSE


#include "xbase.h"

using namespace xb;

#include "tstfuncs.cpp"


int main( int argCnt, char **av )
{
  int iRc = 0;
  int iRc2 = 0;
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

  xbSchema MyV4Record[] = 
  {
    { "CFLD",       XB_CHAR_FLD,      6, 0 },
    { "NFLD",       XB_NUMERIC_FLD,   6, 0 },
    { "ZFLD",       XB_CHAR_FLD,      1, 0 },
    { "",0,0,0 }
  };

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
  x.SetLogSize( 1000000 );
  #endif

  x.SetDataDirectory( PROJECT_DATA_DIR );

  char c;
  xbString s;
  xbInt32 lRecCnt = 0;
  iRc = 0;

  if( iPo > 0 )
    std::cout << "Default Data Directory is [" << x.GetDataDirectory().Str() << "]" << std::endl;

  #ifdef XB_DBF4_SUPPORT
  xbDbf *MyFile = new xbDbf4( &x );
  #else
  xbDbf *MyFile = new xbDbf3( &x );
  #endif

  iRc2 = MyFile->CreateTable( "TestFilt.DBF", "TestFilter", MyV4Record, XB_OVERLAY, XB_MULTI_USER );
  iRc += TestMethod( iPo, "CreateTable()", (xbInt32) iRc2, XB_NO_ERROR );

  #ifdef XB_NDX_SUPPORT
  xbIx *ixPtr;
  void *ndx;
  iRc2 = MyFile->CreateTag( "NDX", "TestFilt.NDX", "CFLD", "", 0, 0, XB_OVERLAY, &ixPtr, &ndx );
  iRc += TestMethod( iPo, "CreateTag()", (xbInt32) iRc2, XB_NO_ERROR );
  if( iRc2 )
    x.DisplayError( iRc2 );
  #endif   // XB_NDX_SUPPORT

  if( iRc2 )
    x.DisplayError( iRc2 );

  // load up a test file with a bunch of records
  for( xbUInt16 i = 0; i < 5 && iRc == XB_NO_ERROR; i++ ){
    // for( xbUInt16 j = 0; j < 26 && iRc == XB_NO_ERROR; j++ ){ 
    for( xbInt16 j = 25; j >= 0 && iRc == XB_NO_ERROR; j-- ){ 
      c = j + 65;
      s = c;
      s.PadRight( c, (xbUInt32) i + 1 );
      MyFile->BlankRecord();
      MyFile->PutField( "CFLD", s );
      MyFile->PutLongField( "NFLD", ++lRecCnt );
      iRc = MyFile->AppendRecord();
    }
  }

  #ifdef XB_INDEX_SUPPORT
  iRc += TestMethod( iPo, "SetCurTag()", MyFile->SetCurTag( "" ), XB_NO_ERROR );
  #endif // XB_INDEX_SUPPORT

  xbFilter f1( MyFile );
  xbString sMyFilterExpression = "LEFT( CFLD, 2 ) = 'YY'";
  iRc += TestMethod( iPo, "Set()", f1.Set( sMyFilterExpression ), XB_NO_ERROR );

  #ifdef XB_INDEX_SUPPORT
  iRc += TestMethod( iPo, "SetCurTag()", MyFile->SetCurTag( "" ), XB_NO_ERROR );
  #endif // XB_INDEX_SUPPORT

  iRc += TestMethod( iPo, "Set(1)", f1.Set( sMyFilterExpression ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "GetFirstRecord(2)", f1.GetFirstRecord(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "GetCurRecNo(3)", (xbInt32) MyFile->GetCurRecNo(), 28 );
  iRc += TestMethod( iPo, "GetNextRecord(4)", f1.GetNextRecord(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "GetCurRecNo(5)", (xbInt32) MyFile->GetCurRecNo(), 54 );
  iRc += TestMethod( iPo, "GetNextRecord(6)", f1.GetNextRecord(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "GetCurRecNo(7)", (xbInt32) MyFile->GetCurRecNo(), 80 );
  iRc += TestMethod( iPo, "GetLast(8)", f1.GetLastRecord(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "GetCurRecNo(9)", (xbInt32) MyFile->GetCurRecNo(), 106 );
  iRc += TestMethod( iPo, "GetPrev(10)", f1.GetPrevRecord(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "GetCurRecNo(11)", (xbInt32) MyFile->GetCurRecNo(), 80 );

  sMyFilterExpression = "LEFT( CFLD, 2 ) = 'CC'";
  iRc += TestMethod( iPo, "Set(20)", f1.Set( sMyFilterExpression ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "GetFirstRecord(21)", f1.GetFirstRecord(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "GetCurRecNo(22)", (xbInt32) MyFile->GetCurRecNo(), 50 );
  iRc += TestMethod( iPo, "GetNextRecord(23)", f1.GetNextRecord(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "GetCurRecNo(24)", (xbInt32) MyFile->GetCurRecNo(), 76 );
  iRc += TestMethod( iPo, "GetNextRecord(25)", f1.GetNextRecord(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "GetCurRecNo(26)", (xbInt32) MyFile->GetCurRecNo(), 102 );
  iRc += TestMethod( iPo, "GetLast(27)", f1.GetLastRecord(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "GetCurRecNo(28)", (xbInt32) MyFile->GetCurRecNo(), 128 );
  iRc += TestMethod( iPo, "GetPrev(29)", f1.GetPrevRecord(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "GetCurRecNo(30)", (xbInt32) MyFile->GetCurRecNo(), 102 );


  #ifdef XB_NDX_SUPPORT
  f1.SetLimit( 0 );

  // change things up a bit
  iRc += TestMethod( iPo, "GetRecord(40)", MyFile->GetRecord( 32 ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(41)",  MyFile->PutField( "ZFLD", "Z" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "GetRecord(42)", MyFile->GetRecord( 52 ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(43)",  MyFile->PutField( "ZFLD", "Z" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "GetRecord(44)", MyFile->GetRecord( 76 ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(45)",  MyFile->PutField( "ZFLD", "Z" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "GetRecord(46)", MyFile->GetRecord( 103 ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(47)",  MyFile->PutField( "ZFLD", "Z" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "Commit(48)",  MyFile->Commit(), XB_NO_ERROR );


  iRc += TestMethod( iPo, "SetIxTag(50)", f1.SetIxTag( "TestFilt" ), XB_NO_ERROR );
  sMyFilterExpression = "ZFLD = 'Z'";
  iRc += TestMethod( iPo, "Set(51)", f1.Set( sMyFilterExpression ), XB_NO_ERROR );

  iRc += TestMethod( iPo, "GetFirstRecordIx(52)", f1.GetFirstRecordIx(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "GetCurRecNo(53)", (xbInt32) MyFile->GetCurRecNo(), 52 );
  iRc += TestMethod( iPo, "GetNextRecordIx(54)", f1.GetNextRecordIx(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "GetCurRecNo(55)", (xbInt32) MyFile->GetCurRecNo(), 103 );
  iRc += TestMethod( iPo, "GetNextRecordIx(56)", f1.GetNextRecordIx(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "GetCurRecNo(57)", (xbInt32) MyFile->GetCurRecNo(), 76 );
  iRc += TestMethod( iPo, "GetLastRecordIx(58)", f1.GetLastRecordIx(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "GetCurRecNo(59)", (xbInt32) MyFile->GetCurRecNo(), 32 );
  iRc += TestMethod( iPo, "GetPrevRecordIx(60)", f1.GetPrevRecordIx(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "GetCurRecNo(61)", (xbInt32) MyFile->GetCurRecNo(), 76 );


  iRc += TestMethod( iPo, "Set(70)", f1.Set( "ZFLD = 'A'", "TestFilt" ), XB_NO_ERROR );



  #endif  // XB_NDX_SUPPORT


  iRc += TestMethod( iPo, "Close(100)", MyFile->Close(), XB_NO_ERROR );
  delete MyFile;

  if( iPo > 0 || iRc < 0 )
    fprintf( stdout, "Total Errors = %d\n", iRc * -1 );

#ifdef XB_LOGGING_SUPPORT
  sMsg.Sprintf( "Program [%s] terminating with [%d] errors...", av[0], iRc * -1 );
  x.WriteLogMessage( sMsg, 2 );
#endif

  return iRc;
}
