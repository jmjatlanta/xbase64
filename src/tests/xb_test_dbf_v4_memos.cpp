/* xb_test_xbdbf_v4_memos.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

// This program tests the version III memo field logic
// usage:   xb_test_dbf_v4_memos QUITE|NORMAL|VERBOSE

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
    { "RECID",      XB_NUMERIC_FLD,   8, 0 },
    { "MEMOFLD1",   XB_MEMO_FLD,     10, 0 },
    { "MEMOFLD2",   XB_MEMO_FLD,     10, 0 },
    { "MEMOFLD3",   XB_MEMO_FLD,     10, 0 },
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

  //#ifdef XB_LOCKING_SUPPORT
  //x.DisableDefaultAutoLock();
  //#endif // XB_LOCKING_SUPPORT

  InitTime();


  if( po > 0 )
    std::cout << "Default Data Directory is [" << x.GetDataDirectory().Str() << "]" << std::endl;

  xbDbf4 V4Dbf( &x );
  rc += TestMethod( po, "SetCreateMemoBlockSize", V4Dbf.SetCreateMemoBlockSize( 1024 ), XB_NO_ERROR );
  rc2 = V4Dbf.CreateTable( "TestV4_memos.DBF", "TestV4", MyV4Record, XB_OVERLAY, XB_MULTI_USER );

  xbUInt64 ullDbfOrigFileSize = 0;
  xbUInt64 ullDbtOrigFileSize = 0;
  rc = V4Dbf.GetFileSize( ullDbfOrigFileSize );
  rc = V4Dbf.GetMemoPtr()->GetFileSize( ullDbtOrigFileSize );

  rc += TestMethod( po, "CreateTable()", (xbInt32) rc2, XB_NO_ERROR );
  if( rc2 )
    x.DisplayError( rc2 );
  else{

    xbInt16  fldRecId = V4Dbf.GetFieldNo( "RECID" );
    xbInt16  fldMemo1 = V4Dbf.GetFieldNo( "MEMOFLD1" );
    xbInt16  fldMemo2 = V4Dbf.GetFieldNo( "MEMOFLD2" );
    xbInt16  fldMemo3 = V4Dbf.GetFieldNo( "MEMOFLD3" );
    xbString sData;




    // Record 1
    rc += TestMethod( po, "BlankRecord()", V4Dbf.BlankRecord(), XB_NO_ERROR );
    rc += TestMethod( po, "PutField()", V4Dbf.PutField( fldRecId, "1" ), XB_NO_ERROR );
    sData = "Memo data";

    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo1, sData ), XB_NO_ERROR );
    rc += TestDbt4Method( po, "Add one block", V4Dbf.GetMemoPtr(), 2, 2, "" );

    sData = "Some other memo data";
    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo2, sData ), XB_NO_ERROR );
    rc += TestDbt4Method( po, "Add one block", V4Dbf.GetMemoPtr(), 3, 3, "" );
    rc += TestMethod( po, "AppendRecord()", V4Dbf.AppendRecord(), XB_NO_ERROR );




    // Record 2
    rc += TestMethod( po, "BlankRecord()", V4Dbf.BlankRecord(), XB_NO_ERROR );
    rc += TestMethod( po, "PutField()", V4Dbf.PutField( fldRecId, "2" ), XB_NO_ERROR );
    sData = "Memo data Rec 2 ..[";
    sData.PadRight( 'Z', 1036 );
    sData += ']';
    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo1, sData ), XB_NO_ERROR );
    sData = "Some other memo data Rec 2 ... [";
    sData.PadRight( 'W', 1555 );
    sData += ']';
    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo2, sData ), XB_NO_ERROR );
    rc += TestDbt4Method( po, "Add 7 blocks", V4Dbf.GetMemoPtr(), 7, 7, "" );

    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo1, "" ), XB_NO_ERROR );
    rc += TestDbt4Method( po, "Delete 2 blocks", V4Dbf.GetMemoPtr(), 3, 7, "3,2,7" );
    rc += TestMethod( po, "AppendRecord()", V4Dbf.AppendRecord(), XB_NO_ERROR );

    // Record 3
    rc += TestMethod( po, "BlankRecord()", V4Dbf.BlankRecord(), XB_NO_ERROR );
    rc += TestMethod( po, "PutField()", V4Dbf.PutField( fldRecId, "3" ), XB_NO_ERROR );
    sData = "Memo data r3f1";
    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo1, sData ), XB_NO_ERROR );
    rc += TestDbt4Method( po, "Add one block", V4Dbf.GetMemoPtr(), 4, 7, "4,1,7" );
    sData = "Memo data r3f2";
    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo2, sData ), XB_NO_ERROR );
    rc += TestDbt4Method( po, "Add one block", V4Dbf.GetMemoPtr(), 7, 7, "" );
    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo1, "" ), XB_NO_ERROR );
    rc += TestDbt4Method( po, "Delete one block", V4Dbf.GetMemoPtr(), 3, 7, "3,1,7" );
    rc += TestMethod( po, "AppendRecord()", V4Dbf.AppendRecord(), XB_NO_ERROR );

    // Record 4
    rc += TestMethod( po, "BlankRecord()", V4Dbf.BlankRecord(), XB_NO_ERROR );
    rc += TestMethod( po, "PutField()", V4Dbf.PutField( fldRecId, "4" ), XB_NO_ERROR );
    sData = "Memo data Rec 4 fld1 ..[";
    sData.PadRight( 'Q', 1036 );
    sData += ']';
    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo1, sData ), XB_NO_ERROR );
    rc += TestDbt4Method( po, "Update Memo Field", V4Dbf.GetMemoPtr(), 3, 9, "3,1,9" );
    rc += TestMethod( po, "AppendRecord()", V4Dbf.AppendRecord(), XB_NO_ERROR );

    // go back a record and delete a memo field and test if all blocks filled in
    rc += TestMethod( po, "GetPrevRecord()", V4Dbf.GetPrevRecord(), XB_NO_ERROR );
    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo2, "" ), XB_NO_ERROR );
    rc += TestMethod( po, "Commit()", V4Dbf.Commit(), XB_NO_ERROR );
    rc += TestDbt4Method( po, "Delete one block", V4Dbf.GetMemoPtr(), 3, 9, "3,2,9" );

    // Record 5
    rc += TestMethod( po, "BlankRecord()", V4Dbf.BlankRecord(), XB_NO_ERROR );
    rc += TestMethod( po, "PutField()", V4Dbf.PutField( fldRecId, "5" ), XB_NO_ERROR );
    rc += TestMethod( po, "AppendRecord()", V4Dbf.AppendRecord(), XB_NO_ERROR );

    // Zap a table with memo fields
    rc += TestMethod( po, "Zap()", V4Dbf.Zap(), XB_NO_ERROR );
    xbUInt32 ulRecCnt = 0;
    rc += TestMethod( po, "GetRecCount()", V4Dbf.GetRecordCnt( ulRecCnt ), XB_NO_ERROR );
    rc += TestMethod( po, "GetRecCount()b", (xbInt32) ulRecCnt, 0 );

    // Add records back into the table, and verify the block chains created as designed
    // Record 1
    rc += TestMethod( po, "BlankRecord()", V4Dbf.BlankRecord(), XB_NO_ERROR );
    rc += TestMethod( po, "PutField()", V4Dbf.PutField( fldRecId, "1" ), XB_NO_ERROR );
    sData = "Memo data rec 1 memo 1";
    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo1, sData ), XB_NO_ERROR );
    rc += TestMethod( po, "AppendRecord()", V4Dbf.AppendRecord(), XB_NO_ERROR );
    rc += TestDbt4Method( po, "Update Memo Field", V4Dbf.GetMemoPtr(), 2, 2, "" );

    // Revise the memo field
    sData = "Updated memo data rec1 memo 1";
    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo1, sData ), XB_NO_ERROR );
    rc += TestMethod( po, "Commit()", V4Dbf.Commit(), XB_NO_ERROR );
    rc += TestDbt4Method( po, "UpdateMemoField", V4Dbf.GetMemoPtr(), 2, 2, "" );

    sData = "Memo data rec 1 memo 2";
    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo2, sData ), XB_NO_ERROR );
    rc += TestMethod( po, "Commit()", V4Dbf.Commit(), XB_NO_ERROR );
    rc += TestDbt4Method( po, "UpdateMemoField", V4Dbf.GetMemoPtr(), 3, 3, "" );

    // Record 2
    rc += TestMethod( po, "BlankRecord()", V4Dbf.BlankRecord(), XB_NO_ERROR );
    rc += TestMethod( po, "PutField()", V4Dbf.PutField( fldRecId, "2" ), XB_NO_ERROR );
    sData = "Memo data rec 2 memo 1";
    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo1, sData ), XB_NO_ERROR );
    sData = "Memo data rec 2 memo 2";
    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo2, sData ), XB_NO_ERROR );
    rc += TestMethod( po, "AppendRecord()", V4Dbf.AppendRecord(), XB_NO_ERROR );
    rc += TestDbt4Method( po, "Add one block", V4Dbf.GetMemoPtr(), 5, 5, "" );
    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo1, "" ), XB_NO_ERROR );
    rc += TestMethod( po, "Commit()", V4Dbf.Commit(), XB_NO_ERROR );
    rc += TestDbt4Method( po, "UpdateMemoField", V4Dbf.GetMemoPtr(), 3, 5, "3,1,5" );

    sData = "Test the abort code";
    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo1, sData ), XB_NO_ERROR );
    rc += TestMethod( po, "Abort()", V4Dbf.Abort(), XB_NO_ERROR );
    rc += TestDbt4Method( po, "UpdateMemoField", V4Dbf.GetMemoPtr(), 3, 5, "3,1,5" );

    sData = "Test mulitple updates before commit";
    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo1, sData ), XB_NO_ERROR );
    sData = "Test mulitple updates before commit2";
    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo1, sData ), XB_NO_ERROR );
    rc += TestDbt4Method( po, "UpdateMemoField", V4Dbf.GetMemoPtr(), 5, 5, "" );
    sData = "Test mulitple updates before commit3";

    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo1, sData ), XB_NO_ERROR );
    rc += TestMethod( po, "Commit()", V4Dbf.Commit(), XB_NO_ERROR );
    rc += TestDbt4Method( po, "UpdateMemoField", V4Dbf.GetMemoPtr(), 5, 5, "" );

//    V4Dbf.GetMemoPtr()->DumpMemoInternals();

    rc += TestMethod( po, "Zap(0)", V4Dbf.Zap(), XB_NO_ERROR );

    xbUInt64 ullDbfPostZapFileSize = 0;
    xbUInt64 ullDbtPostZapFileSize = 0;
    rc += V4Dbf.GetFileSize( ullDbfPostZapFileSize );
    rc += V4Dbf.GetMemoPtr()->GetFileSize( ullDbtPostZapFileSize );

    rc += TestMethod( po, "Zap()", ullDbfPostZapFileSize, ullDbfOrigFileSize );
    rc += TestMethod( po, "Memo->Zap()", ullDbtPostZapFileSize, ullDbtOrigFileSize );
    rc += TestDbt4Method( po, "Zap() block check", V4Dbf.GetMemoPtr(), 1, 1, "" );

    rc += TestMethod( po, "GetRecCount()", V4Dbf.GetRecordCnt( ulRecCnt ), XB_NO_ERROR );
    rc += TestMethod( po, "GetRecCount()", (xbInt32) ulRecCnt, 0 );

    rc += TestMethod( po, "BlankRecord()", V4Dbf.BlankRecord(), XB_NO_ERROR );
    rc += TestMethod( po, "PutField()", V4Dbf.PutField( fldRecId, "1" ), XB_NO_ERROR );
    sData = "Memo data rec 1 memo 1";
    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo1, sData ), XB_NO_ERROR );
    sData = "Memo data rec 1 memo 2";
    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo2, sData ), XB_NO_ERROR );
    sData = "Memo data rec 1 memo 3";
    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo3, sData ), XB_NO_ERROR );
    rc += TestMethod( po, "AppendRecord()", V4Dbf.AppendRecord(), XB_NO_ERROR );

    sData = "Memo data rec 1 memo 1 A";
    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo1, sData ), XB_NO_ERROR );
    sData = "Memo data rec 1 memo 2 A";
    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo2, sData ), XB_NO_ERROR );
    sData = "Memo data rec 1 memo 3A ";
    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo3, sData ), XB_NO_ERROR );
    rc += TestMethod( po, "Commit()", V4Dbf.Commit(), XB_NO_ERROR );
    rc += TestDbt4Method( po, "Update Memo Field", V4Dbf.GetMemoPtr(), 4, 4, "" );

    // Record 2
    rc += TestMethod( po, "BlankRecord()", V4Dbf.BlankRecord(), XB_NO_ERROR );
    rc += TestMethod( po, "PutField()", V4Dbf.PutField( fldRecId, "2" ), XB_NO_ERROR );
    sData = "Memo data rec 2 memo 1";
    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo1, sData ), XB_NO_ERROR );
    sData = "Memo data rec 2 memo 2";
    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo2, sData ), XB_NO_ERROR );
    rc += TestMethod( po, "AppendRecord()", V4Dbf.AppendRecord(), XB_NO_ERROR );
    rc += TestDbt4Method( po, "Add two blocks", V4Dbf.GetMemoPtr(), 6, 6, "" );

    // Record 3
    rc += TestMethod( po, "BlankRecord()", V4Dbf.BlankRecord(), XB_NO_ERROR );
    rc += TestMethod( po, "PutField()", V4Dbf.PutField( fldRecId, "3" ), XB_NO_ERROR );
    sData = "Memo data rec 3 memo 1";
    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo1, sData ), XB_NO_ERROR );
    sData = "Memo data rec 3 memo 2";
    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo2, sData ), XB_NO_ERROR );
    rc += TestMethod( po, "AppendRecord()", V4Dbf.AppendRecord(), XB_NO_ERROR );
    rc += TestDbt4Method( po, "Add two blocks", V4Dbf.GetMemoPtr(), 8, 8, "" );

    // Record 4
    rc += TestMethod( po, "BlankRecord()", V4Dbf.BlankRecord(), XB_NO_ERROR );
    rc += TestMethod( po, "PutField()", V4Dbf.PutField( fldRecId, "4" ), XB_NO_ERROR );
    sData = "Memo data rec 4 memo 1";
    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo1, sData ), XB_NO_ERROR );
    sData = "Memo data rec 4 memo 2";
    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo2, sData ), XB_NO_ERROR );
    rc += TestMethod( po, "AppendRecord()", V4Dbf.AppendRecord(), XB_NO_ERROR );
    rc += TestDbt4Method( po, "Add two blocks", V4Dbf.GetMemoPtr(), 10, 10, "" );

    // Record 5
    rc += TestMethod( po, "BlankRecord()", V4Dbf.BlankRecord(), XB_NO_ERROR );
    rc += TestMethod( po, "PutField()", V4Dbf.PutField( fldRecId, "5" ), XB_NO_ERROR );
    sData = "Memo data rec 5 memo 1";
    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo1, sData ), XB_NO_ERROR );
    sData = "Memo data rec 5 memo 2";
    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo2, sData ), XB_NO_ERROR );
    rc += TestMethod( po, "AppendRecord()", V4Dbf.AppendRecord(), XB_NO_ERROR );
    rc += TestDbt4Method( po, "Add two blocks", V4Dbf.GetMemoPtr(), 12, 12, "" );

    // Record 6
    rc += TestMethod( po, "BlankRecord()", V4Dbf.BlankRecord(), XB_NO_ERROR );
    rc += TestMethod( po, "PutField()", V4Dbf.PutField( fldRecId, "6" ), XB_NO_ERROR );
    sData = "Memo data rec 6 memo 1";
    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo1, sData ), XB_NO_ERROR );
    sData = "Memo data rec 6 memo 2";
    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo2, sData ), XB_NO_ERROR );
    rc += TestMethod( po, "AppendRecord()", V4Dbf.AppendRecord(), XB_NO_ERROR );
    rc += TestDbt4Method( po, "Add two blocks", V4Dbf.GetMemoPtr(), 14, 14, "" );

    // Record 7
    rc += TestMethod( po, "BlankRecord()", V4Dbf.BlankRecord(), XB_NO_ERROR );
    rc += TestMethod( po, "PutField()", V4Dbf.PutField( fldRecId, "7" ), XB_NO_ERROR );
    sData = "Memo data rec 6 memo 1";
    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo1, sData ), XB_NO_ERROR );
    sData = "Memo data rec 6 memo 2";
    rc += TestMethod( po, "UpdateMemoField()", V4Dbf.UpdateMemoField( fldMemo2, sData ), XB_NO_ERROR );
    rc += TestMethod( po, "AppendRecord()", V4Dbf.AppendRecord(), XB_NO_ERROR );
    rc += TestDbt4Method( po, "Add two blocks", V4Dbf.GetMemoPtr(), 16, 16, "" );

    // delete records 1, 3, 5, 7
    rc += TestMethod( po, "GetRecord()", V4Dbf.GetRecord( 1 ), XB_NO_ERROR );
    rc += TestMethod( po, "DeleteRecord()", V4Dbf.DeleteRecord(), XB_NO_ERROR );
    rc += TestMethod( po, "GetRecord()", V4Dbf.GetRecord( 3 ), XB_NO_ERROR );
    rc += TestMethod( po, "DeleteRecord()", V4Dbf.DeleteRecord(), XB_NO_ERROR );
    rc += TestMethod( po, "GetRecord()", V4Dbf.GetRecord( 5 ), XB_NO_ERROR );
    rc += TestMethod( po, "DeleteRecord()", V4Dbf.DeleteRecord(), XB_NO_ERROR );
    rc += TestMethod( po, "GetRecord()", V4Dbf.GetRecord( 7 ), XB_NO_ERROR );
    rc += TestMethod( po, "DeleteRecord()", V4Dbf.DeleteRecord(), XB_NO_ERROR );

    rc += TestMethod( po, "Pack()", V4Dbf.Pack(), XB_NO_ERROR );

    xbString sDir;
    V4Dbf.GetFileDirPart( sDir );
    xbString sDbfName;
    xbString sDbtName;
    sDbfName.Sprintf( "%snew4name.DBF", sDir.Str());
    sDbtName.Sprintf( "%snew4name.DBT", sDir.Str());

//    std::cout << "remove [" << sDbfName.Str() << "\n";
//    std::cout << "remove [" << sDbtName.Str() << "\n";

    V4Dbf.xbRemove( sDbfName );
    V4Dbf.xbRemove( sDbtName );

    x.DisplayTableList();

    rc += TestMethod( po, "Rename()",  V4Dbf.Rename( "new4name.DBF" ), XB_NO_ERROR );

    x.DisplayTableList();


    rc += TestMethod( po, "Close()", V4Dbf.Close(), XB_NO_ERROR );
  }

  if( po > 0 || rc < 0 )
    fprintf( stdout, "Total Errors = %d\n", rc * -1 );

#ifdef XB_LOGGING_SUPPORT
  sMsg.Sprintf( "Program [%s] terminating with [%d] errors...", av[0], rc * -1 );
  x.WriteLogMessage( sMsg );
#endif

  return rc;
}
