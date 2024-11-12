/* xb_test_xbdbf_v3_memos.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

// This program tests the version III memo field logic

// usage:   xb_test_dbf_v3_memos QUITE|NORMAL|VERBOSE


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


  xbSchema MyV3Record[] = 
  {
    { "RECID",      XB_NUMERIC_FLD,   8, 0 },
    { "MEMOFLD1",   XB_MEMO_FLD,     10, 0 },
    { "MEMOFLD2",   XB_MEMO_FLD,     10, 0 },
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

  xbDbf3 V3Dbf( &x );                /* version 3 dbf file */

  rc2 = V3Dbf.CreateTable( "TestV3_memos.DBF", "TestV3", MyV3Record, XB_OVERLAY, XB_MULTI_USER );
  rc += TestMethod( po, "CreateTable()", (xbInt32) rc2, XB_NO_ERROR );


  if( rc2 )
    x.DisplayError( rc2 );
  else{

    xbInt16  fldRecId = V3Dbf.GetFieldNo( "RECID" );
    xbInt16  fldMemo1 = V3Dbf.GetFieldNo( "MEMOFLD1" );
    xbInt16  fldMemo2 = V3Dbf.GetFieldNo( "MEMOFLD2" );
    xbString sData;

    // Record 1
    rc += TestMethod( po, "BlankRecord()", V3Dbf.BlankRecord(), XB_NO_ERROR );
    rc += TestMethod( po, "PutField()", V3Dbf.PutField( fldRecId, "1" ), XB_NO_ERROR );
    sData = "Memo data";
    rc += TestMethod( po, "UpdateMemoField()", V3Dbf.UpdateMemoField( fldMemo1, sData ), XB_NO_ERROR );

    sData = "Some other memo data";
    rc += TestMethod( po, "UpdateMemoField()", V3Dbf.UpdateMemoField( fldMemo2, sData ), XB_NO_ERROR );
    rc += TestMethod( po, "AppendRecord()", V3Dbf.AppendRecord(), XB_NO_ERROR );

    // Record 2
    rc += TestMethod( po, "BlankRecord()", V3Dbf.BlankRecord(), XB_NO_ERROR );
    rc += TestMethod( po, "PutField()", V3Dbf.PutField( fldRecId, "2" ), XB_NO_ERROR );
    sData = "Memo data Rec 2 ..[";
    sData.PadRight( 'Z', 1036 );
    sData += ']';
    rc += TestMethod( po, "UpdateMemoField()", V3Dbf.UpdateMemoField( fldMemo1, sData ), XB_NO_ERROR );
    sData = "Some other memo data Rec 2 ... [";
    sData.PadRight( 'W', 1555 );
    sData += ']';
    rc += TestMethod( po, "UpdateMemoField()", V3Dbf.UpdateMemoField( fldMemo2, sData ), XB_NO_ERROR );
    rc += TestMethod( po, "AppendRecord()", V3Dbf.AppendRecord(), XB_NO_ERROR );

    // Check memo header file
    xbUInt32 ulNextBlock;
    xbMemo *Memo = V3Dbf.GetMemoPtr();
    rc += TestMethod( po, "GetHdrNextBlock()", Memo->GetHdrNextBlock( ulNextBlock ), XB_NO_ERROR );
    rc += TestMethod( po, "GetHdrNextBlock()", (xbInt32) ulNextBlock, 10 );


    // Zap a table with memo fields
    rc += TestMethod( po, "Zap(0)", V3Dbf.Zap(), XB_NO_ERROR );
    xbUInt32 ulRecCnt = 0;
    rc += TestMethod( po, "GetRecCount()", V3Dbf.GetRecordCnt( ulRecCnt ), XB_NO_ERROR );
    rc += TestMethod( po, "GetRecCount() b", (xbInt32) ulRecCnt, 0 );
    Memo = V3Dbf.GetMemoPtr();
    rc += TestMethod( po, "GetHdrNextBlock()", Memo->GetHdrNextBlock( ulNextBlock ), XB_NO_ERROR );
    rc += TestMethod( po, "GetHdrNextBlock()", (xbInt32) ulNextBlock, 1 );



    // Add records back into the table 
    // Record 1
    rc += TestMethod( po, "BlankRecord()", V3Dbf.BlankRecord(), XB_NO_ERROR );
    rc += TestMethod( po, "PutField()", V3Dbf.PutField( fldRecId, "1" ), XB_NO_ERROR );
    sData = "Memo data rec 1 memo 1";
    rc += TestMethod( po, "UpdateMemoField()", V3Dbf.UpdateMemoField( fldMemo1, sData ), XB_NO_ERROR );

    sData = "Memo data rec 1 memo 2";
    rc += TestMethod( po, "UpdateMemoField()", V3Dbf.UpdateMemoField( fldMemo2, sData ), XB_NO_ERROR );
    rc += TestMethod( po, "AppendRecord()", V3Dbf.AppendRecord(), XB_NO_ERROR );

    // Record 2
    rc += TestMethod( po, "BlankRecord()", V3Dbf.BlankRecord(), XB_NO_ERROR );
    rc += TestMethod( po, "PutField()", V3Dbf.PutField( fldRecId, "2" ), XB_NO_ERROR );
    sData = "Memo data Rec 2 memo 1..[";
    sData.PadRight( 'Z', 1036 );
    sData += ']';
    rc += TestMethod( po, "UpdateMemoField()", V3Dbf.UpdateMemoField( fldMemo1, sData ), XB_NO_ERROR );
    sData = "Some other memo data Rec 2 memo 2 ... [";
    sData.PadRight( 'W', 1555 );
    sData += ']';
    rc += TestMethod( po, "UpdateMemoField()", V3Dbf.UpdateMemoField( fldMemo2, sData ), XB_NO_ERROR );
    rc += TestMethod( po, "AppendRecord()", V3Dbf.AppendRecord(), XB_NO_ERROR );

    // Record 3
    rc += TestMethod( po, "BlankRecord()", V3Dbf.BlankRecord(), XB_NO_ERROR );
    rc += TestMethod( po, "PutField()", V3Dbf.PutField( fldRecId, "3" ), XB_NO_ERROR );
    sData = "Memo data Rec 3 memo 1";
    rc += TestMethod( po, "UpdateMemoField()", V3Dbf.UpdateMemoField( fldMemo1, sData ), XB_NO_ERROR );

    sData = "Memo data Rec 3 memo 2";
    rc += TestMethod( po, "UpdateMemoField()", V3Dbf.UpdateMemoField( fldMemo2, sData ), XB_NO_ERROR );
    rc += TestMethod( po, "AppendRecord()", V3Dbf.AppendRecord(), XB_NO_ERROR );


    // Record 4
    rc += TestMethod( po, "BlankRecord()", V3Dbf.BlankRecord(), XB_NO_ERROR );
    rc += TestMethod( po, "PutField()", V3Dbf.PutField( fldRecId, "4" ), XB_NO_ERROR );
    sData = "Memo data Rec 4 memo 1";
    rc += TestMethod( po, "UpdateMemoField()", V3Dbf.UpdateMemoField( fldMemo1, sData ), XB_NO_ERROR );
    sData = "Memo data Rec 4 memo 2";
    rc += TestMethod( po, "UpdateMemoField()", V3Dbf.UpdateMemoField( fldMemo2, sData ), XB_NO_ERROR );
    rc += TestMethod( po, "AppendRecord()", V3Dbf.AppendRecord(), XB_NO_ERROR );


    // Record 5
    rc += TestMethod( po, "BlankRecord()", V3Dbf.BlankRecord(), XB_NO_ERROR );
    rc += TestMethod( po, "PutField()", V3Dbf.PutField( fldRecId, "5" ), XB_NO_ERROR );
    sData = "Memo data Rec 5 memo 1";
    rc += TestMethod( po, "UpdateMemoField()", V3Dbf.UpdateMemoField( fldMemo1, sData ), XB_NO_ERROR );
    sData = "Memo data Rec 5 memo 2";
    rc += TestMethod( po, "UpdateMemoField()", V3Dbf.UpdateMemoField( fldMemo2, sData ), XB_NO_ERROR );
    rc += TestMethod( po, "AppendRecord()", V3Dbf.AppendRecord(), XB_NO_ERROR );


    // Record 6
    rc += TestMethod( po, "BlankRecord()", V3Dbf.BlankRecord(), XB_NO_ERROR );
    rc += TestMethod( po, "PutField()", V3Dbf.PutField( fldRecId, "6" ), XB_NO_ERROR );
    sData = "Memo data Rec 6 memo 1";
    rc += TestMethod( po, "UpdateMemoField()", V3Dbf.UpdateMemoField( fldMemo1, sData ), XB_NO_ERROR );
    sData = "Memo data Rec 6 memo 2";
    rc += TestMethod( po, "UpdateMemoField()", V3Dbf.UpdateMemoField( fldMemo2, sData ), XB_NO_ERROR );
    rc += TestMethod( po, "AppendRecord()", V3Dbf.AppendRecord(), XB_NO_ERROR );


    // Record 7
    rc += TestMethod( po, "BlankRecord()", V3Dbf.BlankRecord(), XB_NO_ERROR );
    rc += TestMethod( po, "PutField()", V3Dbf.PutField( fldRecId, "7" ), XB_NO_ERROR );
    sData = "Memo data Rec 7 memo 1";
    rc += TestMethod( po, "UpdateMemoField()", V3Dbf.UpdateMemoField( fldMemo1, sData ), XB_NO_ERROR );
    sData = "Memo data Rec 7 memo 2";
    rc += TestMethod( po, "UpdateMemoField()", V3Dbf.UpdateMemoField( fldMemo2, sData ), XB_NO_ERROR );
    rc += TestMethod( po, "AppendRecord()", V3Dbf.AppendRecord(), XB_NO_ERROR );

    // Flag 1, 3, 5 and 7 for deletion
    rc += TestMethod( po, "GetRecord()", V3Dbf.GetRecord( 1 ), XB_NO_ERROR );
    rc += TestMethod( po, "DeleteRecord()", V3Dbf.DeleteRecord(), XB_NO_ERROR );

    rc += TestMethod( po, "GetRecord()", V3Dbf.GetRecord( 3 ), XB_NO_ERROR );
    rc += TestMethod( po, "DeleteRecord()", V3Dbf.DeleteRecord(), XB_NO_ERROR );

    rc += TestMethod( po, "GetRecord()", V3Dbf.GetRecord( 5 ), XB_NO_ERROR );
    rc += TestMethod( po, "DeleteRecord()", V3Dbf.DeleteRecord(), XB_NO_ERROR );

    rc += TestMethod( po, "GetRecord()", V3Dbf.GetRecord( 7 ), XB_NO_ERROR );
    rc += TestMethod( po, "DeleteRecord()", V3Dbf.DeleteRecord(), XB_NO_ERROR );

    rc += TestMethod( po, "Pack()", V3Dbf.Pack(), XB_NO_ERROR );

    xbString sDir;
    V3Dbf.GetFileDirPart( sDir );
    xbString sDbfName;
    xbString sDbtName;
    sDbfName.Sprintf( "%snewV3nm.DBF", sDir.Str());
    sDbtName.Sprintf( "%snewV3nm.DBT", sDir.Str());

    //std::cout << "remove [" << sDbfName.Str() << "]\n";
    //std::cout << "remove [" << sDbtName.Str() << "]\n";

    V3Dbf.xbRemove( sDbfName );
    V3Dbf.xbRemove( sDbtName );

    x.DisplayTableList();

    rc += TestMethod( po, "Rename()",  V3Dbf.Rename( "newV3nm.DBF" ), XB_NO_ERROR );
    x.DisplayTableList();


    rc += TestMethod( po, "Close()", V3Dbf.Close(), XB_NO_ERROR );


  }



  if( po > 0 || rc < 0 )
    fprintf( stdout, "Total Errors = %d\n", rc * -1 );

#ifdef XB_LOGGING_SUPPORT
  sMsg.Sprintf( "Program [%s] terminating with [%d] errors...", av[0], rc * -1 );
  x.WriteLogMessage( sMsg );
#endif

  return rc;
}
