/* xb_test_xbdbf_v4_nomemos.cpp

XBase Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

// This program tests the class xbXdbf

// usage:   xb_test_dbf_v4_nomemos QUITE|NORMAL|VERBOSE


#include "xbase.h"

using namespace xb;

#include "tstfuncs.cpp"


int main( int argCnt, char **av )
{
  int rc = 0;
  int rc2 = 0;
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

  xbSchema MyV4Record[] = 
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
    { "DBLFLDTST",  XB_FLOAT_FLD,    14, 4 },
    { "",0,0,0 }
  };

  xbSchema MyV4ZipRecord[] = 
  {
    { "ZIPCODE",    XB_NUMERIC_FLD,   5, 0 },
    { "CITY",       XB_CHAR_FLD,     30, 0 },
    { "STATE",      XB_CHAR_FLD,      2, 0 },
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

  xbDbf4 V4Dbf( &x );                // version 4 dbf file
  rc2 = V4Dbf.CreateTable( "TestV4.DBF", "TestV4", MyV4Record, XB_OVERLAY, XB_MULTI_USER );
  rc += TestMethod( po, "CreateTable()", (xbInt32) rc2, XB_NO_ERROR );
  if( rc2 )
    x.DisplayError( rc2 );

  xbUInt64 ullFileSize;
  rc2 = V4Dbf.GetFileSize( ullFileSize );

  rc = TestMethod( po, "CheckTableSize", ullFileSize, (xbUInt64) 353 );

  if( po == 2 ){
    std::cout << "There should be one entry in the table list" << std::endl;
    x.DisplayTableList();
  }

  xbDbf4 V4Dbf2( &x );

  // next occurrence should error as a dup
  x.WriteLogMessage( "Second create attempt" );
  rc2 = V4Dbf2.CreateTable( "TestV4.DBF", "TestV4", MyV4Record, XB_DONTOVERLAY, XB_MULTI_USER );
  rc += TestMethod( po, "CreateTable()", rc2, XB_FILE_EXISTS );
  if( rc2 != XB_FILE_EXISTS ){
    x.DisplayError( rc2 );
  }

  if( po == 2 ){
    std::cout << "There should be one entry in the table list" << std::endl;
    x.DisplayTableList();
  }


  rc += TestMethod( po, "BlankRecord()", V4Dbf.BlankRecord(), XB_NO_ERROR );
  rc += TestMethod( po, "GetFieldNo()", V4Dbf.GetFieldNo("LASTNAME"), 1 );
  rc += TestMethod( po, "GetFieldNo()", V4Dbf.GetFieldNo("MIDDLEINIT"), 2 );

  xbBool bIsNull;
  rc += TestMethod( po, "GetNullSts()", V4Dbf.GetNullSts( V4Dbf.GetFieldNo("LASTNAME"), bIsNull, 0 ), XB_NO_ERROR );
  rc += TestMethod( po, "GetNullSts()", bIsNull, xbTrue );



  xbInt16 fldLastName = V4Dbf.GetFieldNo( "LASTNAME" );
  rc += TestMethod( po, "PutField()", V4Dbf.PutField( fldLastName, "NELSON" ), XB_NO_ERROR );

  rc += TestMethod( po, "GetNullSts()", V4Dbf.GetNullSts( V4Dbf.GetFieldNo("LASTNAME"), bIsNull, 0 ), XB_NO_ERROR );
  rc += TestMethod( po, "GetNullSts()", bIsNull, xbFalse );

  rc += TestMethod( po, "GetNullSts()", V4Dbf.GetNullSts( V4Dbf.GetFieldNo("FIRSTNAME"), bIsNull, 0 ), XB_NO_ERROR );
  rc += TestMethod( po, "GetNullSts()", bIsNull, xbTrue );


  rc += TestMethod( po, "PutField()", V4Dbf.PutField( "FIRSTNAME", "WILLIE" ), XB_NO_ERROR );
  rc += TestMethod( po, "PutField()", V4Dbf.PutField( "AMOUNT", "12.34" ), XB_NO_ERROR );
  xbDate dt( "19500209" );
  rc += TestMethod( po, "PutDateField()", V4Dbf.PutDateField( "BIRTHDATE", dt ), XB_NO_ERROR );
  rc += TestMethod( po, "PutLongField()", V4Dbf.PutLongField( "ZIPCODE", 12345 ), XB_NO_ERROR );
  rc += TestMethod( po, "PutLogicalField()", V4Dbf.PutLogicalField( "SWITCH", "Y" ), XB_NO_ERROR );
  rc += TestMethod( po, "PutFloatField()", V4Dbf.PutFloatField( "TESTNUM", (xbFloat) 1234.5678 ), XB_NO_ERROR );
  rc += TestMethod( po, "PutDoubleField()", V4Dbf.PutDoubleField( "DBLFLDTST", (xbDouble) 9876.5432 ), XB_NO_ERROR );
  rc += TestMethod( po, "AppendRecord()", V4Dbf.AppendRecord(), XB_NO_ERROR );


  xbString sf;
  rc += TestMethod( po, "GetField()", V4Dbf.GetField( fldLastName, sf, 0 ), XB_NO_ERROR );
  rc += TestMethod( po, "GetField()", sf, "NELSON              ", 20 );
  rc += TestMethod( po, "GetField()", V4Dbf.GetField( fldLastName, sf ), XB_NO_ERROR );
  rc += TestMethod( po, "GetField()", sf, "NELSON              ", 20 );
  rc += TestMethod( po, "GetField()", V4Dbf.GetField( "LASTNAME", sf ), XB_NO_ERROR );
  rc += TestMethod( po, "GetField()", sf, "NELSON              ", 20 );
  xbInt16 iNoOfDecimals;
  rc += TestMethod( po, "GetFieldDecimal()", V4Dbf.GetFieldDecimal( "AMOUNT", iNoOfDecimals ), XB_NO_ERROR );
  rc += TestMethod( po, "GetFieldDecimal()", iNoOfDecimals, 2 );

  xbDouble d;
  rc += TestMethod( po, "GetDoubleField()", V4Dbf.GetDoubleField( "DBLFLDTST", d ), XB_NO_ERROR );
  rc += TestMethod( po, "GetDoubleField()-b", d, (xbDouble) 9876.5432 );

  char FieldType;
  rc += TestMethod( po, "GetFieldType()", V4Dbf.GetFieldType( "STARTDATE", FieldType ), XB_NO_ERROR );
  rc += TestMethod( po, "GetFieldType()", FieldType , 'D' );

  xbInt16 iFieldLen;
  rc += TestMethod( po, "GetFieldLen()", V4Dbf.GetFieldLen( "STARTDATE", iFieldLen ), XB_NO_ERROR );
  rc += TestMethod( po, "GetFieldLen()", iFieldLen, 8 );

  xbInt16 fldAMT = V4Dbf.GetFieldNo( "AMOUNT" );
  rc += TestMethod( po, "GetField()", V4Dbf.GetField( fldAMT, sf ), XB_NO_ERROR );
  rc += TestMethod( po, "GetField()", sf, "    12.34", 9 );

  xbInt32 lZip;
  rc += TestMethod( po, "GetLongField()", V4Dbf.GetLongField( "ZIPCODE", lZip ), XB_NO_ERROR );
  rc += TestMethod( po, "GetLongField()", lZip, 12345 );

  xbString sSwitch;
  rc += TestMethod( po, "GetLogicalField()", V4Dbf.GetLogicalField( "SWITCH", sSwitch ), XB_NO_ERROR );
  rc += TestMethod( po, "GetLogicalField()", sSwitch, "Y", 1 );

  xbFloat fNum;
  rc += TestMethod( po, "GetFloatField()", V4Dbf.GetFloatField( "TESTNUM", fNum ), 0 );
  rc += TestMethod( po, "GetFloatField()", fNum, (xbFloat) 1234.5678 );

  // init a second record for more testing
  rc += TestMethod( po, "BlankRecord()", V4Dbf.BlankRecord(), XB_NO_ERROR );
  rc += TestMethod( po, "PutLogicalField()", V4Dbf.PutLogicalField( "SWITCH", xbTrue ), XB_NO_ERROR );
  rc += TestMethod( po, "AppendRecord()", V4Dbf.AppendRecord(), XB_NO_ERROR );
  xbBool bSwitch;
  rc += TestMethod( po, "GetLogicalField()", V4Dbf.GetLogicalField( "SWITCH", bSwitch ), 0 );
  rc += TestMethod( po, "GetLogicalField()", bSwitch, xbTrue );

  // init a third record for more testing
  rc += TestMethod( po, "BlankRecord()", V4Dbf.BlankRecord(), XB_NO_ERROR );
  rc += TestMethod( po, "PutLogicalField()", V4Dbf.PutLogicalField( "SWITCH", xbFalse ), XB_NO_ERROR );
  rc += TestMethod( po, "AppendRecord()", V4Dbf.AppendRecord(), XB_NO_ERROR );
  rc += TestMethod( po, "GetLogicalField()", V4Dbf.GetLogicalField( "SWITCH", bSwitch ), 0 );
  rc += TestMethod( po, "GetLogicalField()", bSwitch, xbFalse );

  rc += TestMethod( po, "PutField()", V4Dbf.PutField( "TESTNUM", "12345678" ), XB_INVALID_DATA );
  rc += TestMethod( po, "PutField()", V4Dbf.PutField( "TESTNUM", "1234567" ), XB_NO_ERROR );
  rc += TestMethod( po, "PutField()", V4Dbf.PutField( "TESTNUM", "1234567.12345" ), XB_INVALID_DATA );
  rc += TestMethod( po, "PutField()", V4Dbf.PutField( "TESTNUM", ".12345" ), XB_INVALID_DATA );
  rc += TestMethod( po, "PutField()", V4Dbf.PutField( "TESTNUM", ".1234" ), XB_NO_ERROR );
  rc += TestMethod( po, "PutField()", V4Dbf.PutField( "TESTNUM", "12345678.1234" ), XB_INVALID_DATA );
  rc += TestMethod( po, "PutField()", V4Dbf.PutField( "TESTNUM", "1234567.1234" ), XB_NO_ERROR );

  rc += TestMethod( po, "AppendRecord()", V4Dbf.AppendRecord(), XB_NO_ERROR );

  rc += TestMethod( po, "GetFieldCnt()", V4Dbf.GetFieldCnt(), 10 );
  rc += TestMethod( po, "GetTblAlias()", V4Dbf.GetTblAlias(), "TestV4", 6 );
  rc += TestMethod( po, "GetDbfStatus()", V4Dbf.GetDbfStatus(), XB_OPEN );
  rc += TestMethod( po, "GetCurRecNo()", (xbInt32) V4Dbf.GetCurRecNo(), 4 );
  rc += TestMethod( po, "GetFirstRecord()", V4Dbf.GetFirstRecord(), XB_NO_ERROR );
  rc += TestMethod( po, "GetCurRecNo()", (xbInt32) V4Dbf.GetCurRecNo(), 1 );
  rc += TestMethod( po, "GetNextRecord()", V4Dbf.GetNextRecord(), XB_NO_ERROR );
  rc += TestMethod( po, "GetCurRecNo()", (xbInt32) V4Dbf.GetCurRecNo(), 2 );
  rc += TestMethod( po, "GetLastRecord()", V4Dbf.GetLastRecord(), XB_NO_ERROR );
  rc += TestMethod( po, "GetCurRecNo()", (xbInt32) V4Dbf.GetCurRecNo(), 4 );
  rc += TestMethod( po, "GetPrevRecord()", V4Dbf.GetPrevRecord(), XB_NO_ERROR );
  rc += TestMethod( po, "GetCurRecNo()", (xbInt32) V4Dbf.GetCurRecNo(), 3 );
  rc += TestMethod( po, "GetRecordLen()", (xbInt32) V4Dbf.GetRecordLen(), 94 );

  char * p = V4Dbf.GetRecordBuf();
  rc += TestMethod( po, "GetRecordBuf()", p[0], ' ' );

  if( po == 2 ){
    V4Dbf.DumpHeader( 3 );
    V4Dbf.DumpRecord( 4 );
  }

  rc += TestMethod( po, "PutField()", V4Dbf.PutField( "LASTNAME", "CLINTON" ), XB_NO_ERROR );
  rc += TestMethod( po, "PutRecord()", V4Dbf.PutRecord(), XB_NO_ERROR );
  rc += TestMethod( po, "DeleteRecord()", V4Dbf.DeleteRecord(), XB_NO_ERROR );
  rc += TestMethod( po, "UndeleteRecord()", V4Dbf.UndeleteRecord(), XB_NO_ERROR );
  xbUInt32 ulRecCnt = 0;
  rc += TestMethod( po, "GetRecordCnt()", V4Dbf.GetRecordCnt( ulRecCnt ), XB_NO_ERROR );
  rc += TestMethod( po, "GetRecordCnt()", (xbInt32) ulRecCnt, 4 );
  rc += TestMethod( po, "DeleteAllRecords()", V4Dbf.DeleteAllRecords(), XB_NO_ERROR );
  rc += TestMethod( po, "UndeleteAllRecords()", V4Dbf.UndeleteAllRecords(), XB_NO_ERROR );
  rc += TestMethod( po, "GetRecord()", V4Dbf.GetRecord( 2L ), XB_NO_ERROR );
  rc += TestMethod( po, "DeleteRecord()", V4Dbf.DeleteRecord(), XB_NO_ERROR );

  if( po == 2 ){
    V4Dbf.DumpRecord( 4 );
  }

  xbDbf4 CopyDbf( &x );
  rc += TestMethod( po, "CopyDbfStructure()", V4Dbf.CopyDbfStructure( &CopyDbf, "CopyStructureV4NoMemos", "CopyAliasV4NoMemos", XB_OVERLAY, XB_MULTI_USER ), XB_NO_ERROR );
  rc += TestMethod( po, "CopyDbf.Close()", CopyDbf.Close(), XB_NO_ERROR );

  if( po == 2 ){
    std::cout << "There should be one entry in the table list" << std::endl;
    x.DisplayTableList();
  }

  rc += TestMethod( po, "GetDbfStatus()", CopyDbf.GetDbfStatus(), XB_CLOSED );
  rc += TestMethod( po, "Open()", CopyDbf.xbDbf::Open( "CopyStructureV4NoMemos.DBF", "CopyAliasV4NoMemos" ), XB_NO_ERROR );

  if( po == 2 ){
    std::cout << "There should be two entries in the table list" << std::endl;
    x.DisplayTableList();
  }

  rc += TestMethod( po, "Pack()", V4Dbf.Pack(), XB_NO_ERROR );
  rc += TestMethod( po, "GetDbfStatus()", V4Dbf.GetDbfStatus(), XB_OPEN );

  rc += TestMethod( po, "Zap()", V4Dbf.Zap(), XB_NO_ERROR );
  if( po == 2 )
    x.DisplayTableList();

  xbUInt64 ullFileSize2 = 0;
  rc2 = V4Dbf.GetFileSize( ullFileSize2 );
  rc = TestMethod( po, "CheckTableSize", ullFileSize2, ullFileSize );

// Test of auto commit code here
  rc += TestMethod( po, "BlankRecord()", V4Dbf.BlankRecord(), XB_NO_ERROR );
  rc += TestMethod( po, "PutField()", V4Dbf.PutField( fldLastName, "NELSON" ), XB_NO_ERROR );
  rc += TestMethod( po, "PutField()", V4Dbf.PutField( "FIRSTNAME", "WILLIE" ), XB_NO_ERROR );
  rc += TestMethod( po, "PutField()", V4Dbf.PutField( "AMOUNT", "12.34" ), XB_NO_ERROR );
  rc += TestMethod( po, "PutLongField()", V4Dbf.PutLongField( "ZIPCODE", 12345 ), XB_NO_ERROR );
  rc += TestMethod( po, "PutLogicalField()", V4Dbf.PutLogicalField( "SWITCH", "Y" ), XB_NO_ERROR );
  rc += TestMethod( po, "PutFloatField()", V4Dbf.PutFloatField( "TESTNUM", (xbFloat) 1234.5678 ), XB_NO_ERROR );
  rc += TestMethod( po, "PutDoubleField()", V4Dbf.PutDoubleField( "DBLFLDTST", (xbDouble) 9876.5432 ), XB_NO_ERROR );
  rc += TestMethod( po, "AppendRecord()", V4Dbf.AppendRecord(), XB_NO_ERROR );

  rc += TestMethod( po, "BlankRecord()", V4Dbf.BlankRecord(), XB_NO_ERROR );
  rc += TestMethod( po, "PutField()", V4Dbf.PutField( fldLastName, "JOHNSON" ), XB_NO_ERROR );
  rc += TestMethod( po, "PutField()", V4Dbf.PutField( "FIRSTNAME", "JIMMY" ), XB_NO_ERROR );
  rc += TestMethod( po, "PutField()", V4Dbf.PutField( "AMOUNT", "555.33" ), XB_NO_ERROR );
  rc += TestMethod( po, "PutLongField()", V4Dbf.PutLongField( "ZIPCODE", 76523 ), XB_NO_ERROR );
  rc += TestMethod( po, "PutLogicalField()", V4Dbf.PutLogicalField( "SWITCH", "N" ), XB_NO_ERROR );
  rc += TestMethod( po, "PutFloatField()", V4Dbf.PutFloatField( "TESTNUM", (xbFloat) 8765.4321 ), XB_NO_ERROR );
  rc += TestMethod( po, "PutDoubleField()", V4Dbf.PutDoubleField( "DBLFLDTST", (xbDouble) 9876.5432 ), XB_NO_ERROR );
  rc += TestMethod( po, "AppendRecord()", V4Dbf.AppendRecord(), XB_NO_ERROR );

  rc += TestMethod( po, "PutField()", V4Dbf.PutField( "AMOUNT", "666.22" ), XB_NO_ERROR );
  rc += TestMethod( po, "Abort()", V4Dbf.Abort(), XB_NO_ERROR );
  rc += TestMethod( po, "GetField()", V4Dbf.GetField( "AMOUNT", sf ), XB_NO_ERROR );
  rc += TestMethod( po, "GetField()", sf, "   555.33", 9 );

  rc += TestMethod( po, "PutField()", V4Dbf.PutField( "AMOUNT", "666.22" ), XB_NO_ERROR );
  rc += TestMethod( po, "Commit()", V4Dbf.Commit(), XB_NO_ERROR );
  rc += TestMethod( po, "GetField()", V4Dbf.GetField( fldAMT, sf ), XB_NO_ERROR );
  rc += TestMethod( po, "GetField()", sf, "   666.22", 9 );

  rc += TestMethod( po, "GetPrevRecord()", V4Dbf.GetPrevRecord(), XB_NO_ERROR );
  rc += TestMethod( po, "GetNextRecord()", V4Dbf.GetNextRecord(), XB_NO_ERROR );
  rc += TestMethod( po, "GetField()", V4Dbf.GetField( fldAMT, sf ), XB_NO_ERROR );
  rc += TestMethod( po, "GetField()", sf, "   666.22", 9 );

  rc += TestMethod( po, "AutoCommit()", V4Dbf.SetAutoCommit( 0 ), XB_NO_ERROR );
  rc += TestMethod( po, "PutField()", V4Dbf.PutField( "AMOUNT", "999.33" ), XB_NO_ERROR );
  rc += TestMethod( po, "GetPrevRecord()", V4Dbf.GetPrevRecord(), XB_NO_ERROR );
  rc += TestMethod( po, "GetNextRecord()", V4Dbf.GetNextRecord(), XB_NO_ERROR );
  rc += TestMethod( po, "GetField()", V4Dbf.GetField( fldAMT, sf ), XB_NO_ERROR );
  rc += TestMethod( po, "GetField()", sf, "   666.22", 9 );

  rc += TestMethod( po, "AutoCommit()", V4Dbf.SetAutoCommit( -1 ), XB_NO_ERROR );
  rc += TestMethod( po, "PutField()", V4Dbf.PutField( "AMOUNT", "999.33" ), XB_NO_ERROR );
  rc += TestMethod( po, "GetCurRecNo()", (xbInt32) V4Dbf.GetCurRecNo(), 2 );
  rc += TestMethod( po, "GetPrevRecord()", V4Dbf.GetPrevRecord(), XB_NO_ERROR );
  rc += TestMethod( po, "GetNextRecord()", V4Dbf.GetNextRecord(), XB_NO_ERROR );
  rc += TestMethod( po, "GetField()", V4Dbf.GetField( fldAMT, sf ), XB_NO_ERROR );
  rc += TestMethod( po, "GetField()", sf, "   999.33", 9 );

  rc += TestMethod( po, "AutoCommit()", V4Dbf.SetAutoCommit( 1 ), XB_NO_ERROR );
  rc += TestMethod( po, "PutField()", V4Dbf.PutField( "AMOUNT", "432.55" ), XB_NO_ERROR );
  rc += TestMethod( po, "GetCurRecNo()", (xbInt32) V4Dbf.GetCurRecNo(), 2 );
  rc += TestMethod( po, "GetPrevRecord()", V4Dbf.GetPrevRecord(), XB_NO_ERROR );
  rc += TestMethod( po, "GetNextRecord()", V4Dbf.GetNextRecord(), XB_NO_ERROR );
  rc += TestMethod( po, "GetField()", V4Dbf.GetField( fldAMT, sf ), XB_NO_ERROR );
  rc += TestMethod( po, "GetField()", sf, "   432.55", 9 );


  xbDbf4 V4DbfZ( &x );                // version 4 dbf file
  rc2 = V4DbfZ.CreateTable( "TestV4Zip.DBF", "TestV4Zip", MyV4ZipRecord, XB_OVERLAY, XB_MULTI_USER );
  rc += TestMethod( po, "CreateTable()", (xbInt32) rc2, XB_NO_ERROR );
  if( rc2 )
    x.DisplayError( rc2 );



  rc += TestMethod( po, "Close()", V4Dbf.Close(), XB_NO_ERROR );
  if( po == 2 )
    x.DisplayTableList();


  rc += TestMethod( po, "Close()", V4DbfZ.Close(), XB_NO_ERROR );
  if( po == 2 )
    x.DisplayTableList();



  if( po > 0 || rc < 0 )
    fprintf( stdout, "Total Errors = %d\n", rc * -1 );

#ifdef XB_LOGGING_SUPPORT
  sMsg.Sprintf( "Program [%s] terminating with [%d] errors...", av[0], rc * -1 );
  x.WriteLogMessage( sMsg );
#endif

  return rc;
}
