/* xb_test_xbdbf_v3_nomemos.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

// This program tests the class xbXdbf

// usage:   xb_test_dbf_v3_nomemos QUITE|NORMAL|VERBOSE


#include "xbase.h"

using namespace xb;

#include "tstfuncs.cpp"


int main( int argCnt, char **av )
{
  int iRc = 0;
  int iRc2 = 0;
  int po = 1;          /* print option */
                       /* 0 - QUIET    */
                       /* 1 - NORMAL   */
                       /* 2 - VERBOSE  */

  if( argCnt > 1 ) {
    if( av[1][0] == 'Q' )
      po = 0;
    else if( av[1][0] == 'V' )
      po = 2;
  }


  xbSchema MyV3Record[] = 
  {
    { "FIRSTNAME",  XB_CHAR_FLD,     15, 0 },
    { "LASTNAME",   XB_CHAR_FLD,     20, 0 },
    { "MIDDLEINIT", XB_CHAR_FLD,      1, 0 },
    { "BIRTHDATE",  XB_DATE_FLD,      8, 0 },
    { "STARTDATE",  XB_DATE_FLD,      8, 0 },
    { "AMOUNT",     XB_NUMERIC_FLD,   9, 2 },
    { "TESTNUM",    XB_NUMERIC_FLD,  12, 4 },
    { "SWITCH",     XB_LOGICAL_FLD,   1, 0 },
    { "ZIPCODE",    XB_NUMERIC_FLD,   5, 0 },
    { "",0,0,0 }
  };


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

  #ifdef XB_LOCKING_SUPPORT
  x.DisableDefaultAutoLock();
  #endif // XB_LOCKING_SUPPORT

  InitTime();

  if( po > 0 )
    std::cout << "Default Data Directory is [" << x.GetDataDirectory().Str() << "]" << std::endl;


  xbDbf3 V3Dbf( &x );   /* version 3 dbf file */

  iRc2 = V3Dbf.CreateTable( "TestV3.DBF", "TestV3", MyV3Record, XB_OVERLAY, XB_MULTI_USER );

  iRc += TestMethod( po, "CreateTable()", (xbInt32) iRc2, XB_NO_ERROR );
  if( iRc2 )
    x.DisplayError( iRc2 );

  if( po == 2 ){
    std::cout << "There should be one entry in the table list" << std::endl;
    x.DisplayTableList();
  }

  xbDbf3 V3Dbf2( &x );
  // next occurrence should error as a dup
  x.WriteLogMessage( "Second create attempt, should generate an error." );
  iRc2 = V3Dbf2.CreateTable( "TestV3.DBF", "TestV3", MyV3Record, XB_DONTOVERLAY, XB_MULTI_USER );
  iRc += TestMethod( po, "CreateTable()", iRc2, XB_FILE_EXISTS );
  if( iRc2 != XB_FILE_EXISTS ){
    x.DisplayError( iRc2 );
  }

  if( po == 2 ){
    std::cout << "There should be one entry in the table list" << std::endl;
    x.DisplayTableList();
  }

  iRc += TestMethod( po, "BlankRecord()", V3Dbf.BlankRecord(), XB_NO_ERROR );
  iRc += TestMethod( po, "GetFieldNo()", V3Dbf.GetFieldNo("LASTNAME"), 1 );
  iRc += TestMethod( po, "GetFieldNo()", V3Dbf.GetFieldNo("MIDDLEINIT"), 2 );

  xbInt16 fldLastName = V3Dbf.GetFieldNo( "LASTNAME" );

  iRc += TestMethod( po, "PutField()", V3Dbf.PutField( fldLastName, "NELSON" ), XB_NO_ERROR );
  iRc += TestMethod( po, "PutField()", V3Dbf.PutField( "FIRSTNAME", "WILLIE" ), XB_NO_ERROR );

  iRc += TestMethod( po, "PutField()", V3Dbf.PutField( "AMOUNT", "12.34" ), XB_NO_ERROR );
  xbDate d( "19991201" );
  iRc += TestMethod( po, "PutDateField()", V3Dbf.PutDateField( "STARTDATE", d ), XB_NO_ERROR );
  iRc += TestMethod( po, "PutField()", V3Dbf.PutField( "STARTDATE", "19991301" ), XB_INVALID_DATA );
  iRc += TestMethod( po, "PutField()", V3Dbf.PutField( "STARTDATE", "19991201" ), XB_NO_ERROR );


  iRc += TestMethod( po, "PutLongField()", V3Dbf.PutLongField( "ZIPCODE", 12345 ), XB_NO_ERROR );
  iRc += TestMethod( po, "PutLogicalField()", V3Dbf.PutLogicalField( "SWITCH", "Y" ), XB_NO_ERROR );
  iRc += TestMethod( po, "PutFloatField()", V3Dbf.PutFloatField( "TESTNUM", (xbFloat) 1234.5678 ), XB_NO_ERROR );
  iRc += TestMethod( po, "AppendRecord()", V3Dbf.AppendRecord(), XB_NO_ERROR );

  xbString sf;
  iRc += TestMethod( po, "GetField1()", V3Dbf.GetField( fldLastName, sf, 0 ), XB_NO_ERROR );
  iRc += TestMethod( po, "GetField2()", sf, "NELSON              ", 20 );
  iRc += TestMethod( po, "GetField3()", V3Dbf.GetField( fldLastName, sf ), XB_NO_ERROR );
  iRc += TestMethod( po, "GetField4()", sf, "NELSON              ", 20 );
  iRc += TestMethod( po, "GetField5()", V3Dbf.GetField( "LASTNAME", sf ), XB_NO_ERROR );
  iRc += TestMethod( po, "GetField6()", sf, "NELSON              ", 20 );
  xbInt16 iNoOfDecimals;
  iRc += TestMethod( po, "GetFieldDecimal()", V3Dbf.GetFieldDecimal( "AMOUNT", iNoOfDecimals ), XB_NO_ERROR );
  iRc += TestMethod( po, "GetFieldDecimal()", iNoOfDecimals, 2 );

  char FieldType;
  iRc += TestMethod( po, "GetFieldType()", V3Dbf.GetFieldType( "STARTDATE", FieldType ), XB_NO_ERROR );
  iRc += TestMethod( po, "GetFieldType()", FieldType , 'D' );

  xbInt16 iFieldLen;
  iRc += TestMethod( po, "GetFieldLen()", V3Dbf.GetFieldLen( "STARTDATE", iFieldLen ), XB_NO_ERROR );
  iRc += TestMethod( po, "GetFieldLen()", iFieldLen, 8 );

  xbInt16 fldAMT = V3Dbf.GetFieldNo( "AMOUNT" );
  iRc += TestMethod( po, "GetRawField1()", V3Dbf.GetField( fldAMT, sf ), XB_NO_ERROR );
  iRc += TestMethod( po, "GetRawField2()", sf, "    12.34", 9 );

  xbInt32 lZip;
  iRc += TestMethod( po, "GetLongField()", V3Dbf.GetLongField( "ZIPCODE", lZip ), XB_NO_ERROR );
  iRc += TestMethod( po, "GetLongField()", lZip, 12345 );

  xbString sSwitch;
  iRc += TestMethod( po, "GetLogicalField()", V3Dbf.GetLogicalField( "SWITCH", sSwitch ), XB_NO_ERROR );
  iRc += TestMethod( po, "GetLogicalField()", sSwitch, "Y", 1 );

  xbFloat fNum;
  iRc += TestMethod( po, "GetFloatField()", V3Dbf.GetFloatField( "TESTNUM", fNum ), 0 );
  iRc += TestMethod( po, "GetFloatField()", fNum, (xbFloat) 1234.5678 );

  iRc += TestMethod( po, "GetDateField()", V3Dbf.GetDateField( "STARTDATE", d ), 0 );
  iRc += TestMethod( po, "GetDateField()", d.Str(), "19991201", 8 );

  // init a second record for more testing
  iRc += TestMethod( po, "BlankRecord()", V3Dbf.BlankRecord(), XB_NO_ERROR );
  iRc += TestMethod( po, "PutLogicalField()", V3Dbf.PutLogicalField( "SWITCH", xbTrue ), XB_NO_ERROR );
  iRc += TestMethod( po, "AppendRecord()", V3Dbf.AppendRecord(), XB_NO_ERROR );
  xbBool bSwitch;
  iRc += TestMethod( po, "GetLogicalField()", V3Dbf.GetLogicalField( "SWITCH", bSwitch ), XB_NO_ERROR );
  iRc += TestMethod( po, "GetLogicalField()", bSwitch, xbTrue );

  // init a third record for more testing
  iRc += TestMethod( po, "BlankRecord()", V3Dbf.BlankRecord(), XB_NO_ERROR );
  iRc += TestMethod( po, "PutLogicalField()", V3Dbf.PutLogicalField( "SWITCH", xbFalse ), XB_NO_ERROR );
  iRc += TestMethod( po, "AppendRecord()", V3Dbf.AppendRecord(), XB_NO_ERROR );


  iRc += TestMethod( po, "GetLogicalField()", V3Dbf.GetLogicalField( "SWITCH", bSwitch ), XB_NO_ERROR );
  iRc += TestMethod( po, "GetLogicalField()", bSwitch, xbFalse );

  iRc += TestMethod( po, "PutField()", V3Dbf.PutField( "TESTNUM", "12345678" ), XB_INVALID_DATA );
  iRc += TestMethod( po, "PutField()", V3Dbf.PutField( "TESTNUM", "1234567" ), XB_NO_ERROR );
  iRc += TestMethod( po, "PutField()", V3Dbf.PutField( "TESTNUM", "1234567.12345" ), XB_INVALID_DATA );
  iRc += TestMethod( po, "PutField()", V3Dbf.PutField( "TESTNUM", ".12345" ), XB_INVALID_DATA );
  iRc += TestMethod( po, "PutField()", V3Dbf.PutField( "TESTNUM", ".1234" ), XB_NO_ERROR );
  iRc += TestMethod( po, "PutField()", V3Dbf.PutField( "TESTNUM", "12345678.1234" ), XB_INVALID_DATA );
  iRc += TestMethod( po, "PutField()", V3Dbf.PutField( "TESTNUM", "1234567.1234" ), XB_NO_ERROR );
  iRc += TestMethod( po, "AppendRecord()", V3Dbf.AppendRecord(), XB_NO_ERROR );

  iRc += TestMethod( po, "GetFieldCnt()", V3Dbf.GetFieldCnt(), 9 );
  iRc += TestMethod( po, "GetTblAlias()", V3Dbf.GetTblAlias(), "TestV3", 6 );
  iRc += TestMethod( po, "GetDbfStatus()", V3Dbf.GetDbfStatus(), XB_OPEN );
  iRc += TestMethod( po, "GetCurRecNo()", (xbInt32) V3Dbf.GetCurRecNo(), 4 );
  iRc += TestMethod( po, "GetFirstRecord()", V3Dbf.GetFirstRecord(), XB_NO_ERROR );
  iRc += TestMethod( po, "GetCurRecNo()", (xbInt32) V3Dbf.GetCurRecNo(), 1 );
  iRc += TestMethod( po, "GetNextRecord()", V3Dbf.GetNextRecord(), XB_NO_ERROR );
  iRc += TestMethod( po, "GetCurRecNo()", (xbInt32) V3Dbf.GetCurRecNo(), 2 );
  iRc += TestMethod( po, "GetLastRecord()", V3Dbf.GetLastRecord(), XB_NO_ERROR );
  iRc += TestMethod( po, "GetCurRecNo()", (xbInt32) V3Dbf.GetCurRecNo(), 4 );
  iRc += TestMethod( po, "GetPrevRecord()", V3Dbf.GetPrevRecord(), XB_NO_ERROR );
  iRc += TestMethod( po, "GetCurRecNo()", (xbInt32) V3Dbf.GetCurRecNo(), 3 );
  iRc += TestMethod( po, "GetRecordLen()", V3Dbf.GetRecordLen(), 80 );

  iRc += TestMethod( po, "GetFirstRecord( XB_ALL_RECS )", V3Dbf.GetFirstRecord( XB_ALL_RECS ), XB_NO_ERROR );
  iRc += TestMethod( po, "GetCurRecNo()", (xbInt32) V3Dbf.GetCurRecNo(), 1 );
  iRc += TestMethod( po, "GetNextRecord( XB_ALL_RECS )", V3Dbf.GetNextRecord( XB_ALL_RECS ), XB_NO_ERROR );
  iRc += TestMethod( po, "GetCurRecNo()", (xbInt32) V3Dbf.GetCurRecNo(), 2 );
  iRc += TestMethod( po, "GetLastRecord( XB_ALL_RECS )", V3Dbf.GetLastRecord( XB_ALL_RECS ), XB_NO_ERROR );
  iRc += TestMethod( po, "GetCurRecNo()", (xbInt32) V3Dbf.GetCurRecNo(), 4 );
  iRc += TestMethod( po, "GetPrevRecord( XB_ALL_RECS )", V3Dbf.GetPrevRecord( XB_ALL_RECS ), XB_NO_ERROR );
  iRc += TestMethod( po, "GetCurRecNo()", (xbInt32) V3Dbf.GetCurRecNo(), 3 );

  iRc += TestMethod( po, "GetFirstRecord( XB_DELETED_RECS )", V3Dbf.GetFirstRecord( XB_DELETED_RECS ), XB_EOF );
  iRc += TestMethod( po, "GetLastRecord( XB_DELETED_RECS )",  V3Dbf.GetLastRecord( XB_DELETED_RECS ),  XB_EOF );

  iRc += TestMethod( po, "GetFirstRecord( XB_ALL_RECS )", V3Dbf.GetFirstRecord( XB_ALL_RECS ), XB_NO_ERROR );
  iRc += TestMethod( po, "GetNextRecord( XB_DELETED_RECS )", V3Dbf.GetNextRecord( XB_DELETED_RECS ), XB_EOF );

  iRc += TestMethod( po, "GetLastRecord( XB_ALL_RECS )", V3Dbf.GetLastRecord( XB_ALL_RECS ), XB_NO_ERROR );
  iRc += TestMethod( po, "GetPrevRecord( XB_DELETED_RECS )", V3Dbf.GetPrevRecord( XB_DELETED_RECS ), XB_BOF );

  iRc += TestMethod( po, "DeleteAllRecords()", V3Dbf.DeleteAllRecords(), XB_NO_ERROR );


  iRc += TestMethod( po, "GetFirstRecord()", V3Dbf.GetFirstRecord(), XB_EOF );
  iRc += TestMethod( po, "GetLastRecord()", V3Dbf.GetLastRecord(), XB_EOF );

  iRc += TestMethod( po, "GetFirstRecord( XB_ALL_RECS )", V3Dbf.GetFirstRecord( XB_ALL_RECS ), XB_NO_ERROR );
  iRc += TestMethod( po, "GetCurRecNo()", (xbInt32) V3Dbf.GetCurRecNo(), 1 );

  iRc += TestMethod( po, "GetNextRecord( XB_ALL_RECS )", V3Dbf.GetNextRecord( XB_ALL_RECS ), XB_NO_ERROR );
  iRc += TestMethod( po, "GetCurRecNo()", (xbInt32) V3Dbf.GetCurRecNo(), 2 );

  iRc += TestMethod( po, "GetNextRecord()", V3Dbf.GetNextRecord(), XB_EOF );

  iRc += TestMethod( po, "GetLastRecord( XB_ALL_RECS )", V3Dbf.GetLastRecord( XB_ALL_RECS ), XB_NO_ERROR );
  iRc += TestMethod( po, "GetCurRecNo()", (xbInt32) V3Dbf.GetCurRecNo(), 4 );

  iRc += TestMethod( po, "GetPrevRecord( XB_ALL_RECS )", V3Dbf.GetPrevRecord( XB_ALL_RECS ), XB_NO_ERROR );
  iRc += TestMethod( po, "GetCurRecNo()", (xbInt32) V3Dbf.GetCurRecNo(), 3 );

  iRc += TestMethod( po, "UndeleteAllRecords()", V3Dbf.UndeleteAllRecords(), XB_NO_ERROR );

  char * p = V3Dbf.GetRecordBuf();
  iRc += TestMethod( po, "GetRecordBuf()", p[0], ' ' );

  if( po == 2 ){
    V3Dbf.DumpHeader( 3 );
    V3Dbf.DumpRecord( 4 );
  }



  iRc += TestMethod( po, "PutField()", V3Dbf.PutField( "LASTNAME", "CLINTON" ), XB_NO_ERROR );
  iRc += TestMethod( po, "PutRecord()", V3Dbf.PutRecord(), XB_NO_ERROR );
  iRc += TestMethod( po, "DeleteRecord()", V3Dbf.DeleteRecord(), XB_NO_ERROR );
  iRc += TestMethod( po, "UndeleteRecord()", V3Dbf.UndeleteRecord(), XB_NO_ERROR );
  xbUInt32 ulRecCnt = 0;
  iRc += TestMethod( po, "GetRecordCnt()", V3Dbf.GetRecordCnt( ulRecCnt ), XB_NO_ERROR );
  iRc += TestMethod( po, "GetRecordCnt()b",(xbInt32) ulRecCnt, 4 );
  iRc += TestMethod( po, "DeleteAllRecords()", V3Dbf.DeleteAllRecords(), XB_NO_ERROR );
  iRc += TestMethod( po, "UndeleteAllRecords()", V3Dbf.UndeleteAllRecords(), XB_NO_ERROR );
  iRc += TestMethod( po, "GetRecord()", V3Dbf.GetRecord( 4 ), XB_NO_ERROR );
  iRc += TestMethod( po, "DeleteRecord()", V3Dbf.DeleteRecord(), XB_NO_ERROR );
  iRc += TestMethod( po, "Commit()", V3Dbf.Commit(), XB_NO_ERROR );
  iRc += TestMethod( po, "GetDbfStatus()", V3Dbf.GetDbfStatus(), XB_OPEN );

  if( po == 2 ){
    V3Dbf.DumpRecord( 4 );
  }

  xbDbf3 CopyDbf( &x );
  iRc += TestMethod( po, "CopyDbfStructure()", V3Dbf.CopyDbfStructure( &CopyDbf, "CopyStructureV3NoMemos", "CopyAliasV3NoMemos", XB_OVERLAY, XB_MULTI_USER ), XB_NO_ERROR );
  iRc += TestMethod( po, "CopyDbf.Close()", CopyDbf.Close(), XB_NO_ERROR );

  if( po == 2 ){
    std::cout << "There should be one entry in the table list" << std::endl;
    x.DisplayTableList();
  }

  iRc += TestMethod( po, "GetDbfStatus()", CopyDbf.GetDbfStatus(), XB_CLOSED );
  iRc += TestMethod( po, "Open()", CopyDbf.xbDbf::Open( "CopyStructureV3NoMemos.DBF", "CopyAliasV3NoMemos" ), XB_NO_ERROR );

  if( po == 2 ){
    std::cout << "There should be two entries in the table list" << std::endl;
    x.DisplayTableList();
  }

  iRc += TestMethod( po, "CopyDbf.Close()", CopyDbf.Close(), XB_NO_ERROR );

  #ifdef XB_DEBUG_SUPPORT
   #ifdef XB_LOCKING_SUPPORT
    V3Dbf.DumpTableLockStatus();
   #endif
  #endif

  iRc += TestMethod( po, "PackTable( 0, 0 )", V3Dbf.Pack(), XB_NO_ERROR );
  iRc += TestMethod( po, "GetDbfStatus()", V3Dbf.GetDbfStatus(), XB_OPEN );

  iRc += TestMethod( po, "Zap()", V3Dbf.Zap(), XB_NO_ERROR );
  if( po == 2 )
    x.DisplayTableList();

  iRc += TestMethod( po, "Close()", V3Dbf.Close(), XB_NO_ERROR );
  if( po == 2 )
    x.DisplayTableList();

  if( po > 0 || iRc < 0 )
    fprintf( stdout, "Total Errors = %d\n", iRc * -1 );


  V3Dbf.Abort();   // don't crash the program if uncommited updates

#ifdef XB_LOGGING_SUPPORT
  sMsg.Sprintf( "Program [%s] terminating with [%d] errors...", av[0], iRc * -1 );
  x.WriteLogMessage( sMsg );
#endif

  return iRc;
}
