/*  xb_dbfutil.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2021,2022,2023 Gary A Kunkel

The xb64 software library is covered under 
the terms of the GPL Version 3, 2007 license.

Email Contact:

    xb64-devel@lists.sourceforge.net
    xb64-users@lists.sourceforge.net

*/

#include "xbase.h"

#ifdef HAVE_DIRENT_H
#include <dirent.h>
#endif

#ifdef WIN32
#include <windows.h>
#endif

using namespace xb;

///@cond DOXYOFF
class xbUtil{
 public:
   xbUtil( xbXBase *x );
   ~xbUtil();
   xbDbf  *GetTablePtr( const char * cTitle );
   void    DisplayActiveTable() const;

   void    Help();

   // menus
   xbInt16 GetOption();
   xbInt32 GetLong();
   void    ProcessOption( const xbString & sOption );
   void    MainMenu();
   void    SystemMenu();
   void    FileMenu();
   void    RecordMenu();
   void    FieldMenu();
   void    IndexMenu();
   void    LockingMenu();
   void    ExpressionMenu();
   void    DebugMenu();
   void    FilterMenu();

   #ifdef XB_SQL_SUPPORT
   void    SqlMenu();
   #endif // XB_SQL_MENU

   #ifdef XB_INF_SUPPORT
   void    InfFileMenu();
   #endif // XB_INF_SUPPORT

   // 2 - SystemMenu options
   void    ListSystemSettings();
   void    UpdateDataDirectory();
   void    ToggleDefaultAutoCommit();
   #ifdef XB_MEMO_SUPPORT
   void    UpdateDefaultMemoBlockSize();
   #endif
   void    UpdateLogDirectory();
   void    UpdateLogFileName();
   void    ToggleLoggingStatus();
   void    WriteLogMessage();
   #ifdef XB_LOCKING_SUPPORT
   void    UpdateDefaultLockRetries();
   void    ToggleDefaultAutoLock();
   void    UpdateDefaultLockFlavor();
   void    UpdateDefaultLockWait();
   void    ToggleMultiUserMode();
   #endif

   #if defined(XB_NDX_SUPPORT) || defined (XB_MDX_SUPPORT)
   void    ToggleIndexOpt();
   #endif

   // 3 - FileMenu options
   void    ListFilesInDataDirectory();
   void    Open();
   void    Close();
   void    CloseAllTables();
   void    DisplayTableStats();
   void    Pack();
   void    ZapTable();
   void    CopyDbfStructure();
   void    UpdateTableAutoCommit();
   void    DisplayTableInfo();
   void    RenameTable();
   void    DeleteTable();

   // 4 - RecordMenu options
   void    GetRecord();
   void    BlankRecord();
   void    AppendRecord();
   void    PutRecord();
   void    DeleteRecord();
   void    UndeleteRecord();
   void    SelectActiveTable();
   void    GetFirstRecord();
   void    GetNextRecord();
   void    GetPrevRecord();
   void    GetLastRecord();
   void    DumpRecord();
   void    AbortRecord();
   void    CommitRecord();

   #ifdef XB_FILTER_SUPPORT
   void    SetFilter();
   void    GetFirstFilterRec();
   void    GetNextFilterRec();
   void    GetPrevFilterRec();
   void    GetLastFilterRec();
   #endif  //  XB_FILTER_SUPPORT

   // 5 - FieldMenu options
   void    ListFieldInfo(); 
   void    UpdateFieldData();
   #ifdef XB_MEMO_SUPPORT
   void    ShowMemoFieldData();
   void    UpdateMemoFieldData();
   void    DeleteMemoField();
   #endif

   // 6 - LockMenu options
   #ifdef XB_LOCKING_SUPPORT
   void    DisplayFileLockSettings(); 
   void    UpdateFileLockRetryCount();
   void    UpdateFileLockFlavor();
   void    UpdateFileAutoLock();
   void    LockDbf();
   void    UnlockDbf();
   void    LockRecord();
   void    UnlockRecord();
   void    LockAppend();
   void    UnlockAppend();
   #ifdef XB_INDEX_SUPPORT
   void    LockIndices();
   void    UnlockIndices();
   #endif  // XB_INDEX_SUPPORT

   #ifdef XB_MEMO_SUPPORT
   void    LockMemo();
   void    UnlockMemo();
   #endif  // XB_MEMO_SUPPORT

   void    LockHeader();
   void    UnlockHeader();
   void    xbFileLock();
   void    xbFileUnlock();
   #endif  // XB_LOCKING_SUPPORT

   // 7 - Expression Menu options
   #ifdef XB_EXPRESSION_SUPPORT
   void    ParseExpression( xbInt16 iOption );
   void    ProcessParsedExpression( xbInt16 iOption );
   void    ParseAndProcessExpression();
   void    JulToDate8();
   void    Date8ToJul();
   void    IsLeapYear();
   #ifdef XB_DEBUG_SUPPORT
   void    DumpExpressionTree();
   #endif  // XB_DEBUG_SUPPORT
   #endif  // XB_EXPRESSION_SUPPORT

   // 8 - Index Menu Options
   #ifdef XB_INDEX_SUPPORT
   void   DisplayOpenIndexFiles();
   void   DisplaySupportedIndexTypes();
   void   SelectTag();
   void   OpenIndex();
   void   CloseIndexFile();
   void   CreateIndexTag();

   void   GetFirstKey();
   void   GetNextKey();
   void   GetPrevKey();
   void   GetLastKey();
   void   FindKey();

   void   CheckIntegrity();
   void   Reindex();
   void   DeleteTag();

   void   DumpIxHeader();
   void   DumpIxNode();
   void   DumpIxNodeChain();
   void   DumpRecsByIx( xbInt16 iOpt );
   void   DumpFreeBlocks();

   #endif // XB_INDEX_SUPPORT


   // 9 - SQL Menu Options
   #ifdef XB_SQL_SUPPORT
   void   ExecSqlNonQuery();
   void   ExecSqlQuery();
   #endif // XB_SQL_SUPPORT


   // 10 - DebugMenu options
   #ifdef XB_MEMO_SUPPORT
   void    DumpDbtHeader();
   void    DumpMemoFreeChain();
   #endif

   // 11 - InfFileMenu options
   #ifdef XB_INF_SUPPORT
   void    ListInfFileData();
   void    AddInfFileData();
   void    DeleteInfFileData();
   void    InfFileHelp();
   #endif


 private:
   xbXBase *x;
   xbDbf   *dActiveTable;

   #ifdef XB_EXPRESSION_SUPPORT
   xbExp *exp;
   #endif // XB_EXPRESSION_SUPPORT

   #ifdef XB_SQL_SUPPORT
   xbSql *sql;
   #endif // XB_SQL_SUPPORT

   #ifdef XB_FILTER_SUPPORT
   xbFilter *filt;
   #endif // XB_FILTER_SUPPORT

};

/*************************************************************************************/
xbUtil::xbUtil( xbXBase *x )
{
  this->x = x;
  dActiveTable = NULL;
  x->EnableMsgLogging();
  x->SetLogSize( 10000000L );

  #ifdef XB_EXPRESSION_SUPPORT
  exp = NULL;
  #endif

  #ifdef XB_SQL_SUPPORT
  sql = new xbSql( x );
  #endif // XB_SQL_SUPPORT

  #ifdef XB_FILTER_SUPPORT
  filt = NULL;
  #endif // XB_FILTER_SUPPORT
}


/*************************************************************************************/
xbUtil::~xbUtil(){


  if( dActiveTable ) 
    delete dActiveTable;

  x->CloseAllTables();

  #ifdef XB_SQL_SUPPORT
  if( sql )
    delete sql;
  #endif // XB_SQL_SUPPORT

  #ifdef XB_FILTER_SUPPORT
  if( filt )
    delete filt;
  #endif // XB_FILTER_SUPPORT
}


/*************************************************************************************/
void xbUtil::Help(){
  std::cout << "Program xb_dbfutil provides a menu driven application for accessing" << std::endl;
  std::cout << "the xbase64 library functions." << std::endl << std::endl;
  std::cout << "Menu choices can be selected directly with out accessing the given menus" << std::endl;
  std::cout << "by entering a '=' followed by the menu choices.  For example, entering '=3.4'" << std::endl;
  std::cout << "would be the same as going to menu #3 and entering a 4, which is the sequence" << std::endl;
  std::cout << "for opening a file." << std::endl;
}

/*************************************************************************************/
#ifdef XB_INF_SUPPORT

void xbUtil::ListInfFileData()
{
  xbLinkListNode<xbString> *llN = dActiveTable->GetInfList();

  xbString s;
  while( llN ){
    s = llN->GetKey();
    std::cout << s.Str() << std::endl;
    llN = llN->GetNextNode();
  }

}

void xbUtil::AddInfFileData()
{
  char cBuf[128];
  std::cout << "Enter NDX index file name (FILENAME.NDX)" << std::endl;
  std::cin.getline( cBuf, 128 );
  xbInt16 iRc = dActiveTable->AssociateIndex( "NDX", cBuf, 0 );
  x->DisplayError( iRc );
}

void xbUtil::DeleteInfFileData()
{
  char cBuf[128];
  std::cout << "Enter NDX index file name (FILENAME.NDX)" << std::endl;
  std::cin.getline( cBuf, 128 );
  xbInt16 iRc = dActiveTable->AssociateIndex( "NDX", cBuf, 1 );
  x->DisplayError( iRc );
}

void xbUtil::InfFileHelp()
{
  std::cout << std::endl;
  std::cout << "Xbase64 uses an .INF file to link non production (NDX) index files to their associated DBF data file" << std::endl;
}
#endif  // XB_INF_SUPPORT


/*************************************************************************************/
#ifdef XB_INDEX_SUPPORT
#ifdef XB_LOCKING_SUPPORT

void xbUtil::LockIndices(){

  // verify active table selected, if not, select one
 if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }
  xbInt16 iRc = dActiveTable->LockIndices( XB_LOCK );
  x->DisplayError( iRc );
}

void xbUtil::UnlockIndices(){

  // verify active table selected, if not, select one
 if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }
  xbInt16 iRc = dActiveTable->LockIndices( XB_UNLOCK );
  x->DisplayError( iRc );
}

#endif   // XB_LOCKING_SUPPORT
#endif   // XB_INDEX_SUPPORT


/*************************************************************************************/
#ifdef XB_EXPRESSION_SUPPORT
void xbUtil::ParseExpression( xbInt16 iOption ){

  if( iOption == 0 )
    std::cout << "ParseExpression()\n";

  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }
  xbInt16 iRc;
  char sExpression[256];
  memset( sExpression, 0x00, 256 );

  std::cout << "Enter expresion:\n";
  std::cin.getline( sExpression, 255 );

  if( strlen( sExpression ) == 0 ){
    std::cout << "Invalid expression" << std::endl;
    return;
  }

  if( exp ){
    delete exp;
    exp = NULL;
  }

  exp = new xbExp( x, dActiveTable );
  iRc = exp->ParseExpression( sExpression );

  if( iOption == 0 )
    x->DisplayError( iRc );
}

/*************************************************************************************/
void xbUtil::ProcessParsedExpression( xbInt16 iOption ){

 if( iOption == 0 )
   std::cout << "ProcessParsedExpression()\n";
 if( !exp )
   ParseExpression( iOption );
 if( !exp )
   return;

 // std::cout << "Dump from w/i dbfutil before processing expression\n";
 //exp->DumpTree( xbTrue );
 //std::cout << "--  end of dumptree in dbfutil before processExpression\n";

 xbInt16 iRc = exp->ProcessExpression();
 if( iRc != XB_NO_ERROR ){
   x->DisplayError( iRc );
   return;
 }

 //std::cout << "Dump from w/i dbfutil after processing expression\n";
 //exp->DumpTree( xbTrue );
 //std::cout << "--  end of dumptree in dbfutil\n";

 xbString sResult;
 xbDouble dResult;
 xbDate   dtResult;
 xbBool   bResult;

 switch (  exp->GetReturnType()){
   case XB_EXP_CHAR:
     exp->GetStringResult( sResult );
     std::cout << sResult.Str() << "\n";
     break;
   case XB_EXP_DATE:
     exp->GetDateResult( dtResult );
     std::cout << dtResult.Str() << "\n";
     break;
   case XB_EXP_LOGICAL:
     exp->GetBoolResult( bResult );
     std::cout << bResult << "\n";
     break;
   case XB_EXP_NUMERIC:
     exp->GetNumericResult( dResult );
     std::cout << dResult << "\n";
     break;
   default:
     std::cout << "Unknown result type [" <<  exp->GetReturnType() << std::endl;
     break;
  }
  return;
}

/*************************************************************************************/
void xbUtil::ParseAndProcessExpression(){
   ParseExpression( 1 );
   if( exp )
     ProcessParsedExpression( 1 );
}
/*************************************************************************************/
#ifdef XB_DEBUG_SUPPORT
void xbUtil::DumpExpressionTree(){

  if( exp )
    exp->DumpTree( xbTrue );
  else
   std::cout << "No expression defined\n";
}
#endif    // XB_DEBUG_SUPPORT

void xbUtil::JulToDate8(){
  std::cout << "Convert Julian Date to Date8 (CCYYMMDD) format" << std::endl;
  std::cout << "Enter Julian Value" << std::endl;
  xbInt32 l = GetLong();
  xbDate d( l );
  std::cout << "Date8 value = [" << d.Str() << "]" << std::endl;
}

void xbUtil::Date8ToJul(){
  char cLine[12];
  std::cout << "Convert Date8 (CCYYMMDD) format to Julian Date value" << std::endl;
  std::cout << "Enter Date8 value:" << std::endl;
  memset( cLine, 0x00, 12 );
  std::cin.getline( cLine, 9 );

  if( strlen( cLine ) == 8 ){
    xbDate d( cLine );
    std::cout << "Julian Value = [" << d.JulianDays() << "]" << std::endl;
  } else {
    std::cout << "Invalid length, expecting 8 characters" << std::endl;
  }
}

void xbUtil::IsLeapYear(){
   std::cout << "Check leap year status for a given year" << std::endl;
   std::cout << "Enter a four digit year" << std::endl;
   xbInt32 l = GetLong();
   xbDate d( l );
   if( d.IsLeapYear( (xbInt16) l ))
    std::cout << l << " is a leap year" << std::endl;
   else
    std::cout << l << " is not a leap year" << std::endl;
}
#endif    // XB_EXPRESSION_SUPPORT
/*************************************************************************************/
#ifdef XB_SQL_SUPPORT
void xbUtil::ExecSqlNonQuery(){

  xbSql sql( x );
  std::cout << "ExecSqlNonQuery\n";

  char sSql[2048];
  std::cout << "Enter an SQL command (2K max byte max)" << std::endl;
  std::cin.getline( sSql, 2048 );

  xbInt16 iRc = sql.ExecuteNonQuery( sSql );
  x->DisplayError( iRc );
}

void xbUtil::ExecSqlQuery(){
  std::cout << "ExecSqlQuery\n";
}
#endif  // XB_SQL_SUPPORT

/*************************************************************************************/
#ifdef XB_LOCKING_SUPPORT
void xbUtil::DisplayFileLockSettings(){

  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }
  std::cout << "File Lock Retry Count = [" << dActiveTable->GetLockRetryCount() << "]" << std::endl;
  std::cout << "File Lock Flavor      = [";
  switch (dActiveTable->GetLockFlavor()){
    case 1:
      std::cout << "Dbase]" << std::endl;
      break;
    case 2:
      std::cout << "Clipper]" << std::endl;
      break;
    case 3:
      std::cout << "Fox]" << std::endl;
      break;
    case 9:
      std::cout << "Xbase64]" << std::endl;
      break;
    default:
      std::cout << "Unknown]" << std::endl;
      break;
  }
  std::cout << "File Auto Lock        = [";

  if( dActiveTable->GetAutoLock())
   std::cout << "ON]" << std::endl;
  else
   std::cout << "OFF]" << std::endl;
  if( dActiveTable->GetHeaderLocked())
   std::cout << "Header Locked         = [TRUE]\n";
  else
   std::cout << "Header Locked         = [FALSE]\n";

  if( dActiveTable->GetTableLocked())
   std::cout << "Table Locked          = [TRUE]\n";
  else
   std::cout << "Table Locked          = [FALSE]\n";

  if( dActiveTable->GetAppendLocked() > 0 )
   std::cout << "Append Locked         = [" << dActiveTable->GetAppendLocked() << "]\n";
  else
   std::cout << "Append Locked         = [FALSE]\n";

  #ifdef XB_MEMO_SUPPORT
  if( dActiveTable->GetMemoLocked())
   std::cout << "Memo Locked           = [TRUE]\n";
  else
   std::cout << "Memo Locked           = [FALSE]\n";
  #endif

  xbLinkListNode<xbUInt32> * llN = dActiveTable->GetFirstRecLock();
  if( llN ){
    while( llN ){
     std::cout << "Record Locked         = [" << llN->GetKey() << "]\n";
      llN = llN->GetNextNode();
    }
  } else {
     std::cout << "Record Locked         = [None]\n";
  }
}

void xbUtil::UpdateFileLockRetryCount(){

  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }

  char cBuf[15];
  std::cout << std::endl << "Enter new File Lock Retry Count: " << std::endl;
  std::cin.getline( cBuf, 15 );
  if( strlen( cBuf ) == 0 ){
    std::cout << "File Lock Retry Count not updated" << std::endl;
    return;
  } else {
    dActiveTable->SetLockRetryCount( atoi( cBuf ));
    std::cout << "File Lock Retry Count updated to ["
              << dActiveTable->GetLockRetryCount()  << "]" << std::endl;
  }
}

void xbUtil::UpdateFileLockFlavor(){

  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }

  std::cout << std::endl;
  std::cout << "Enter new File Lock Flavor: "       << std::endl;
  std::cout << "1 = DBase"                          << std::endl;
  std::cout << "2 = Clipper  (not implemented yet)" << std::endl;
  std::cout << "3 = Fox      (not implemented yet)" << std::endl;
  std::cout << "9 = XBase64  (not implemented yet)" << std::endl;

  char cBuf[15];
  std::cin.getline( cBuf, 15 );
  if( strlen( cBuf ) == 0 ){
    std::cout << "File Lock Flavor not updated" << std::endl;
    return;
  } else {
    dActiveTable->SetLockFlavor( atoi( cBuf ));
    std::cout << "File Lock Flavor updated to ["
              << dActiveTable->GetLockFlavor() << "]" << std::endl;
  }
}

void xbUtil::UpdateFileAutoLock(){

  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }

  std::cout << "Enter new File Auto Lock: "               << std::endl;
  std::cout << "-1 = Use System Default"                  << std::endl;
  std::cout << " 0 = Disable Auto Lock for this DBF file" << std::endl;
  std::cout << " 1 = Enable Auto Lock for this DBF file"  << std::endl;

  char cBuf[15];
  std::cin.getline( cBuf, 15 );
  if( strlen( cBuf ) == 0 ){
    std::cout << "File Auto Lock Flavor not updated" << std::endl;
    return;
  }
  xbInt16 iOption = atoi( cBuf );
  if( iOption < -1 || iOption > 1 ){
    std::cout << "Must be one of:" << std::endl;
    std::cout << " -1 = Use Default Auto Lock"       << std::endl;
    std::cout << "  0 = File Auto Lock Off"          << std::endl;
    std::cout << "  1 = File Auto Lock On"           << std::endl;
    std::cout << "File Auto Lock Flavor not updated" << std::endl;
    return;
  } else {
    dActiveTable->SetAutoLock( iOption );
    std::cout << "File Auto Lock updated to ["
              << dActiveTable->GetAutoLock() << "]" << std::endl;
  }
}

void xbUtil::LockDbf(){

  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }
  xbInt16 iRc;
  std::cout << std::endl << "Lock Table"  << std::endl;
  iRc = dActiveTable->LockTable( XB_LOCK );
  x->DisplayError( iRc );
}

void xbUtil::LockAppend(){

  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }
  xbInt16 iRc;
  std::cout << std::endl << "Lock Append"  << std::endl;
  iRc = dActiveTable->LockAppend( XB_LOCK );
  x->DisplayError( iRc );
}

void xbUtil::UnlockAppend(){

  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }
  xbInt16 iRc;
  std::cout << std::endl << "Unlock Append"  << std::endl;
  iRc = dActiveTable->LockAppend( XB_UNLOCK );
  x->DisplayError( iRc );
}
#ifdef XB_MEMO_SUPPORT
void xbUtil::LockMemo(){

  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }
  xbInt16 iRc;
  std::cout << std::endl << "Lock Memo"  << std::endl;
  iRc = dActiveTable->LockMemo( XB_LOCK );
  x->DisplayError( iRc );
}

void xbUtil::UnlockMemo(){

  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }
  xbInt16 iRc;
  std::cout << std::endl << "Unlock Memo"  << std::endl;
  iRc = dActiveTable->LockMemo( XB_UNLOCK );
  x->DisplayError( iRc );
}
#endif

void xbUtil::LockRecord(){
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }

  xbInt16 iRc;
  std::cout << std::endl << "Lock Record"                    << std::endl;
  std::cout << "Enter record number to lock specific record" << std::endl;

  char cBuf[15];
  std::cin.getline( cBuf, 15 );
  //iRc = dActiveTable->LockRecord( XB_LOCK, atol( cBuf ));
  iRc = dActiveTable->LockRecord( XB_LOCK, strtoul( cBuf, NULL, 0 ));

  x->DisplayError( iRc );
}

void xbUtil::UnlockRecord(){
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }

  xbInt16 iRc;
  std::cout << std::endl << "Unlock Record"                    << std::endl;
  std::cout << "Enter record number to unlock specific record" << std::endl;

  char cBuf[15];
  std::cin.getline( cBuf, 15 );
  //iRc = dActiveTable->LockRecord( XB_UNLOCK, atol( cBuf ));
  iRc = dActiveTable->LockRecord( XB_UNLOCK, strtoul( cBuf, NULL, 0 ));
  x->DisplayError( iRc );
}

void xbUtil::UnlockDbf(){

  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }

  xbInt16 iRc;
  std::cout << std::endl << "Unlock Table" << std::endl;
  iRc = dActiveTable->LockTable( XB_UNLOCK );
  x->DisplayError( iRc );
}

void xbUtil::UnlockHeader(){

  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }

  xbInt16 iRc;
  std::cout << std::endl << "Unlock Table Header" << std::endl;
  iRc = dActiveTable->LockHeader( XB_UNLOCK );
  x->DisplayError( iRc );
}

void xbUtil::LockHeader(){
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );
  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }

  xbInt16 iRc;
  std::cout << std::endl << "Lock Table Header" << std::endl;
  iRc = dActiveTable->LockHeader( XB_LOCK );
  x->DisplayError( iRc );
}

#ifdef XB_DEBUG_SUPPORT

void xbUtil::xbFileLock(){
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }

  char cBufOffset[30];
  char cBufLen[30];

  std::cout << std::endl << "Enter new File Lock Offset (in bytes from BOF): " << std::endl;
  std::cin.getline( cBufOffset, 30 );
  std::cout << std::endl << "Enter new File Lock Len (in Bytes): " << std::endl;
  std::cin.getline( cBufLen, 30 );

  if( strlen( cBufOffset ) == 0 || strlen( cBufLen ) == 0 ){
    std::cout << "Offset and length required." << std::endl;
    return;
  }

  xbInt64 llSpos;
  xbInt64 llLen;
  xbString s1 = cBufOffset;
  s1.CvtLongLong( llSpos );
  s1 = cBufLen;
  s1.CvtLongLong( llLen );
  xbInt16 iRc = dActiveTable->xbLock( XB_LOCK, llSpos, (size_t) llLen );
  x->DisplayError( iRc );
}

void xbUtil::xbFileUnlock(){

  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }
  char cBufOffset[30];
  char cBufLen[30];
  std::cout << std::endl << "Enter new File Lock Offset (in bytes from BOF): " << std::endl;
  std::cin.getline( cBufOffset, 30 );
  std::cout << std::endl << "Enter new File Lock Len (in Bytes): " << std::endl;
  std::cin.getline( cBufLen, 30 );
  if( strlen( cBufOffset ) == 0 || strlen( cBufLen ) == 0 ){
    std::cout << "Offset and length required." << std::endl;
    return;
  }
  xbInt16 iRc;
  iRc = dActiveTable->xbLock( XB_UNLOCK, atol( cBufOffset ), (size_t) atol( cBufLen ));
  x->DisplayError( iRc );
}

#endif
#endif
/*************************************************************************************/
#ifdef XB_DEBUG_SUPPORT
#ifdef XB_MEMO_SUPPORT

void xbUtil::DumpDbtHeader(){

  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }

  if( !dActiveTable->MemoFieldsExist()){
    std::cout << "Table has no memo fields" << std::endl;
    return;
  }

  xbMemo * mPtr;
  mPtr = dActiveTable->GetMemoPtr();
  mPtr->DumpMemoHeader();

}
#endif
#endif

/*************************************************************************************/
#ifdef XB_DEBUG_SUPPORT
#ifdef XB_MEMO_SUPPORT

void xbUtil::DumpMemoFreeChain(){

  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );
  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }
  if( !dActiveTable->MemoFieldsExist()){
    std::cout << "Table has no memo fields" << std::endl;
    return;
  }
  xbMemo * mPtr;
  mPtr = dActiveTable->GetMemoPtr();
  mPtr->DumpMemoFreeChain();
}
#endif
#endif

/*************************************************************************************/
void xbUtil::ListFieldInfo(){
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );
  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }
  DisplayActiveTable();
  dActiveTable->DumpHeader( 2 );
}
/*************************************************************************************/
void xbUtil::UpdateFieldData(){
  xbInt16 rc;
  char    cFldName[40];
  char    cFldData[256];
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );
  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }
  std::cout << "Enter Field Name " << std::endl;
  std::cin.getline( cFldName,  40 );

  std::cout << "Enter Field Data " << std::endl;
  std::cin.getline( cFldData,  256 );

  if(( rc = dActiveTable->PutField( cFldName, cFldData )) != XB_NO_ERROR ){
    x->DisplayError( rc );
    return;
  }
  std::cout << "Success" << std::endl;
}
/*************************************************************************************/
#ifdef XB_MEMO_SUPPORT
void xbUtil::ShowMemoFieldData(){
  xbInt16  rc;
  char     cFldName[40];
  char     cBuf[15];
  xbString sMemoData;
  xbUInt32 ulRecNo;
  xbUInt32 ulFldLen;
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );
  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }
  if( !dActiveTable->MemoFieldsExist()){
    std::cout << "Table has no memo fields" << std::endl;
    return;
  }
  if( dActiveTable->GetCurRecNo() == 0 ){
    std::cout << "Enter Record number" << std::endl;
    std::cin.getline( cBuf,  15 );
    if( strlen( cBuf ) == 0 ){
      std::cout << "Record number not entered" << std::endl;
      return;
    }
    //ulRecNo = atol( cBuf );
    ulRecNo = strtoul( cBuf, NULL, 0 );
    if(( rc = dActiveTable->GetRecord( ulRecNo )) != XB_NO_ERROR ){
      x->DisplayError( rc );
      return;
    }
  }
  std::cout << "Enter Memo Field Name " << std::endl;
  std::cin.getline( cFldName,  40 );
  if(( rc = dActiveTable->GetMemoField( cFldName, sMemoData )) != XB_NO_ERROR ){
    std::cout << "rc = " << rc << std::endl;
    x->DisplayError( rc );
    return;
  }
  if(( rc = dActiveTable->GetMemoFieldLen( cFldName, ulFldLen )) != XB_NO_ERROR ){
    std::cout << "rc = " << rc << std::endl;
    x->DisplayError( rc );
    return;
  }
  std::cout << sMemoData.Str() << std::endl;
  std::cout << "Data length = [" << ulFldLen << "]" << std::endl;
}
#endif
/*************************************************************************************/
#ifdef XB_MEMO_SUPPORT
void xbUtil::UpdateMemoFieldData(){

  xbInt16  rc;
  char     cFldName[40];
  char     cBuf[15];
  char     cMemoData[2048];
  xbUInt32  ulRecNo;
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );
  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }
  if( !dActiveTable->MemoFieldsExist()){
    std::cout << "Table has no memo fields" << std::endl;
    return;
  }
  if( dActiveTable->GetCurRecNo() == 0 ){
    std::cout << "Enter Record number" << std::endl;
    std::cin.getline( cBuf,  15 );
    if( strlen( cBuf ) == 0 ){
      std::cout << "Record number not entered" << std::endl;
      return;
    }
    //ulRecNo = atol( cBuf );
    ulRecNo = strtoul( cBuf, NULL, 0 );
    if(( rc = dActiveTable->GetRecord( ulRecNo )) != XB_NO_ERROR ){
      x->DisplayError( rc );
      return;
    }
  }
  std::cout << "Enter Memo Field Name " << std::endl;
  std::cin.getline( cFldName,  40 );
  std::cout << "Enter Memo Data (2048 bytes max) " << std::endl;
  std::cin.getline( cMemoData,  2048 );
  if(( rc = dActiveTable->UpdateMemoField( cFldName, cMemoData )) != XB_NO_ERROR ){
    std::cout << "rc = " << rc << std::endl;
    x->DisplayError( rc );
    return;
  }

  if(( rc = dActiveTable->PutRecord( dActiveTable->GetCurRecNo())) != XB_NO_ERROR ){
    std::cout << "rc = " << rc << std::endl;
    x->DisplayError( rc );
    return;
  }
}
#endif

/*************************************************************************************/
#ifdef XB_MEMO_SUPPORT
void xbUtil::DeleteMemoField(){

  xbInt16  rc;
  char     cFldName[40];
  char     cBuf[15];
  xbUInt32 ulRecNo;
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );
  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }

  if( !dActiveTable->MemoFieldsExist()){
    std::cout << "Table has no memo fields" << std::endl;
    return;
  }

  if( dActiveTable->GetCurRecNo() == 0 ){
    std::cout << "Enter Record number" << std::endl;
    std::cin.getline( cBuf,  15 );
    if( strlen( cBuf ) == 0 ){
      std::cout << "Record number not entered" << std::endl;
      return;
    }
    //ulRecNo = atol( cBuf );
    ulRecNo = strtoul( cBuf, NULL, 0 );
    if(( rc = dActiveTable->GetRecord( ulRecNo )) != XB_NO_ERROR ){
      x->DisplayError( rc );
      return;
    }
  }
  std::cout << "Enter Memo Field Name " << std::endl;
  std::cin.getline( cFldName,  40 );
  if(( rc = dActiveTable->UpdateMemoField( cFldName, "" )) != XB_NO_ERROR ){
    std::cout << "rc = " << rc << std::endl;
    x->DisplayError( rc );
    return;
  }
  if(( rc = dActiveTable->PutRecord( dActiveTable->GetCurRecNo())) != XB_NO_ERROR ){
    std::cout << "rc = " << rc << std::endl;
    x->DisplayError( rc );
    return;
  }
}
#endif
/*************************************************************************************/
void xbUtil::SelectActiveTable(){

  char cBuf[15];
  xbInt16 iLineNo;
  if( x->GetOpenTableCount() == 0 ){
    std::cout << "No open tables" << std::endl;
    std::cout << "Use menu option =3.4 to open a table" << std::endl;
    return;
  }

  x->DisplayTableList();
  std::cout << std::endl << "Enter line number:" << std::endl;
  std::cin.getline( cBuf,  15 );
  if( strlen( cBuf ) == 0 ){
    std::cout << "Table not selected" << std::endl;
    return;
  }
  iLineNo = atoi( cBuf );
  dActiveTable = x->GetDbfPtr( iLineNo );
}

/*************************************************************************************/
void xbUtil::CommitRecord(){

  xbInt16 rc = XB_NO_ERROR;
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }

  DisplayActiveTable();

  if(( rc = dActiveTable->Commit()) == XB_NO_ERROR )
    std::cout << "Success" << std::endl;
  else
    x->DisplayError( rc );
}
/*************************************************************************************/
void xbUtil::AbortRecord(){

  xbInt16 rc = XB_NO_ERROR;
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }
  DisplayActiveTable();


  if(( rc = dActiveTable->Abort()) == XB_NO_ERROR )
    std::cout << "Success" << std::endl;
  else
    x->DisplayError( rc );
}
/*************************************************************************************/
void xbUtil::DumpRecord(){

  char    cBuf[15];
  xbInt16 rc;
  xbUInt32 ulRecNo;
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );
  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }
  DisplayActiveTable();

  if( dActiveTable->GetCurRecNo() == 0 ){
    std::cout << "Enter Record number" << std::endl;
    std::cin.getline( cBuf,  15 );
    if( strlen( cBuf ) == 0 ){
      std::cout << "Record number not entered" << std::endl;
      return;
    }
    //lRecNo = atol( cBuf );
    ulRecNo = strtoul( cBuf, NULL, 0 );
    if(( rc = dActiveTable->GetRecord( ulRecNo )) != XB_NO_ERROR ){
      x->DisplayError( rc );
      return;
    }
  }

  if(( rc = dActiveTable->DumpRecord( dActiveTable->GetCurRecNo(), 2 )) == XB_NO_ERROR )
    std::cout << "Success" << std::endl;
  else
    x->DisplayError( rc );
}
/*************************************************************************************/
void xbUtil::GetFirstRecord(){

  xbInt16 iRc;

  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }

  if(( iRc = dActiveTable->GetFirstRecord()) == XB_NO_ERROR )
    dActiveTable->DumpRecord( dActiveTable->GetCurRecNo(), 2);
  else
    x->DisplayError( iRc );
}
/*************************************************************************************/
void xbUtil::GetNextRecord(){

  xbInt16 iRc;
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );
  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }
  if(( iRc = dActiveTable->GetNextRecord()) == XB_NO_ERROR )
    dActiveTable->DumpRecord( dActiveTable->GetCurRecNo(), 2);
  else
    x->DisplayError( iRc );
}
/*************************************************************************************/
void xbUtil::GetPrevRecord(){

  xbInt16 iRc;
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );
  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }
  if(( iRc = dActiveTable->GetPrevRecord()) == XB_NO_ERROR )
    dActiveTable->DumpRecord( dActiveTable->GetCurRecNo(), 2);
  else
    x->DisplayError( iRc );
}
/*************************************************************************************/
void xbUtil::GetLastRecord(){

  xbInt16 iRc;

  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }
  if(( iRc = dActiveTable->GetLastRecord()) == XB_NO_ERROR )
    dActiveTable->DumpRecord( dActiveTable->GetCurRecNo(),2);
  else
    x->DisplayError( iRc );
}

/*************************************************************************************/
void xbUtil::UndeleteRecord(){

  char     cBuf[15];
  xbInt16  rc;
  xbUInt32 ulRecNo;

  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }
  DisplayActiveTable();
  if( dActiveTable->GetCurRecNo() == 0 ){
    std::cout << "Enter Record number" << std::endl;
    std::cin.getline( cBuf,  15 );
    if( strlen( cBuf ) == 0 ){
      std::cout << "Record number not entered" << std::endl;
      return;
    }
    //lRecNo = atol( cBuf );
    ulRecNo = strtoul( cBuf, NULL, 0 );
    if(( rc = dActiveTable->GetRecord( ulRecNo )) != XB_NO_ERROR ){
      x->DisplayError( rc );
      return;
    }
  }
  if(( dActiveTable->RecordDeleted()) == xbFalse )
    std::cout << "Record is not flagged for deletion" << std::endl;
  else{
    if(( rc = dActiveTable->UndeleteRecord()) == XB_NO_ERROR )
      std::cout << "Success" << std::endl;
    else
      x->DisplayError( rc );
  }
}

/*************************************************************************************/
void xbUtil::DeleteRecord(){

  char     cBuf[15];
  xbInt16  rc;
  xbUInt32 ulRecNo;

  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );
  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }
  DisplayActiveTable();
  if( dActiveTable->GetCurRecNo() == 0 ){
    std::cout << "Enter Record number" << std::endl;
    std::cin.getline( cBuf,  15 );
    if( strlen( cBuf ) == 0 ){
      std::cout << "Record number not entered" << std::endl;
      return;
    }
    //ulRecNo = atol( cBuf );

    ulRecNo = strtoul( cBuf, NULL, 0 );
    if(( rc = dActiveTable->GetRecord( ulRecNo )) != XB_NO_ERROR ){
      x->DisplayError( rc );
      return;
    }
  }

  if(( dActiveTable->RecordDeleted()) == xbTrue )
   std::cout << "Record is already flagged for deletion" << std::endl;
  else{
    if(( rc = dActiveTable->DeleteRecord()) == XB_NO_ERROR )
      std::cout << "Success" << std::endl;
    else
      x->DisplayError( rc );
  }
}

/*************************************************************************************/
void xbUtil::PutRecord(){

  char     cBuf[15];
  xbInt16  rc;
  xbUInt32 ulRecNo;

  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }
  DisplayActiveTable();

  std::cout << "Enter Record number" << std::endl;
  std::cin.getline( cBuf,  15 );
  if( strlen( cBuf ) == 0 ){
    std::cout << "Record number not entered" << std::endl;
    return;
  }
  //lRecNo = atol( cBuf );
  ulRecNo = strtoul( cBuf, NULL, 0 );
  if(( rc = dActiveTable->PutRecord( ulRecNo )) == XB_NO_ERROR )
    std::cout << "Success" << std::endl;
  else
    x->DisplayError( rc );
}

/*************************************************************************************/
void xbUtil::AppendRecord(){

  xbInt16 rc;
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );
  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }
  if(( rc = dActiveTable->AppendRecord()) == XB_NO_ERROR )
    std::cout << "Success" << std::endl;
  else
    x->DisplayError( rc );
}

/*************************************************************************************/
void xbUtil::BlankRecord(){

  xbInt16 rc;
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );
  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }
  if(( rc = dActiveTable->BlankRecord()) == XB_NO_ERROR )
    std::cout << "Success" << std::endl;
  else
    x->DisplayError( rc );
}

/*************************************************************************************/
void xbUtil::DisplayTableInfo(){

  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }
  dActiveTable->DumpHeader( 1 );
}

/*************************************************************************************/
void xbUtil::DeleteTable(){

  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }

  dActiveTable->DeleteTable();
  dActiveTable = NULL;
  std::cout << "Table deleted.\n";

}


/*************************************************************************************/
void xbUtil::RenameTable(){

  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }

  char cBuf[128];
  std::cout << "Enter new tablefile name (filename.DBF)" << std::endl;
  std::cin.getline( cBuf, 128 );

  if( cBuf[0] ){
    dActiveTable->Rename( cBuf );
    dActiveTable->Close();
    dActiveTable = NULL;
    std::cout << "Table closed.  Reopen if needed.\n";
  }
}

/*************************************************************************************/
void xbUtil::GetRecord(){

  char     cBuf[15];
  xbInt16  iRc;
  xbUInt32 ulRecNo;

  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }

  DisplayActiveTable();

  std::cout << "Enter Record number" << std::endl;
  std::cin.getline( cBuf,  15 );
  if( strlen( cBuf ) == 0 ){
    std::cout << "Record number not entered" << std::endl;
    return;
  }
  //lRecNo = atol( cBuf );
  ulRecNo = strtoul( cBuf, NULL, 0 );
  if(( iRc = dActiveTable->GetRecord( ulRecNo )) == XB_NO_ERROR )
    dActiveTable->DumpRecord( dActiveTable->GetCurRecNo(), 2);
  else
    x->DisplayError( iRc );
}
/*************************************************************************************/
void xbUtil::DisplayActiveTable() const{
  if( dActiveTable ){
    std::cout << "Active Table = ["   << dActiveTable->GetTblAlias().Str()  << "] ";
    xbUInt32 ulRecCnt = 0;
    xbInt16 iRc;
    if(( iRc = dActiveTable->GetRecordCnt( ulRecCnt )) == XB_NO_ERROR ){
      std::cout << "Total Records = ["  << ulRecCnt << "] ";
      std::cout << "Current Record = [" << dActiveTable->GetCurRecNo()  << "] "; 

      if( dActiveTable->GetAutoCommit())
        std::cout << " Auto Commit = [Enabled]";
      else
        std::cout << " Auto Commit = [Disabled]";

      std::cout   << std::endl;
      #ifdef XB_INDEX_SUPPORT
      xbIx *pIx = dActiveTable->GetCurIx();
      if( pIx ){
        void *vpTag = dActiveTable->GetCurTag();
        std::cout << "Active Tag   = [" << pIx->GetTagName( vpTag ).Str() << "] Type = [" << dActiveTable->GetCurIxType().Str() <<
                   "] \tFile Name = [" << pIx->GetFileName().Str() << "] Key = [" << pIx->GetKeyExpression( vpTag ).Str() << "]" <<  std::endl;
      }
      #endif // XB_INDEX_SUPPORT
      std::cout   <<  std::endl;
    } else {
      x->DisplayError( iRc );
    }
  }
}
/*************************************************************************************/
xbDbf * xbUtil::GetTablePtr( const char * cTitle ){

  xbDbf   *d;
  xbInt16 iOpenTableCnt = x->GetOpenTableCount();
  char    cBuf[15];
  xbInt16 iLineNo;

  if( iOpenTableCnt == 0 ){
    std::cout << "No open tables" << std::endl;
    return NULL;
  } else if( iOpenTableCnt == 1 ){
    d = x->GetDbfPtr( 1 );
  } else {
    std::cout << "Select file/table " << cTitle << std::endl;
    x->DisplayTableList();
    std::cout << std::endl << "Enter line number:" << std::endl;
    memset( cBuf, 0x00, 15 );
    std::cin.getline( cBuf,  15 );
    if( strlen( cBuf ) == 0 ){
      std::cout << "Table not selected" << std::endl;
      return NULL;
    }
    iLineNo = atoi( cBuf );
    if( iLineNo < 1 || iLineNo > iOpenTableCnt ){
      std::cout << "Invalid selection.  Valid line numbers are 1 through " << iOpenTableCnt << std::endl;
      return NULL;
    }
    d = x->GetDbfPtr( iLineNo );
  }
  return d;
}
/*************************************************************************************/
void xbUtil::UpdateTableAutoCommit(){
  xbDbf   *d;
  d = GetTablePtr( "" );
  if( d ){
    std::cout << "Xbase64 AutoCommit is functionality to determine if table updates should be posted"             << std::endl;
    std::cout << "                   to the table automatically, even if no xbDbf::PutRecord explicitly executed" << std::endl;
    std::cout << "                   If unsure, leave the option turned on at the DBMS level (default)"           << std::endl;
    std::cout << "                   and don't over ride the setting at the table level"             << std::endl << std::endl;
    std::cout << " -1 ==> Use DBMS setting which is currently [";
    if( x->GetDefaultAutoCommit() )
      std::cout << "ON]" << std::endl;
    else
      std::cout << "OFF]" << std::endl;
    std::cout << "  0 ==> Disable Auto Commit for table" << std::endl;
    std::cout << "  1 ==> Enable Auto Commit for table"  << std::endl;
    std::cout << "Current setting is [" << d->GetAutoCommit() << "]" << std::endl;
    char cBuf[15];
    xbInt16 iAuto;
    std::cout << "Enter new Table Auto Commit: " << std::endl;
    std::cin.getline( cBuf, 15 );
    if( strlen( cBuf ) == 0 ) {
      std::cout << "Not updated"  << std::endl;
      return;
    }
    iAuto = atoi( cBuf );
    if( iAuto < -1 || iAuto > 1 ){
      std::cout << "Invalid value.  Must be one of -1, 0 or 1"  << std::endl;
      return;
    }
    d->SetAutoCommit( iAuto );
    std::cout << "Auto Commit set to [" << d->GetAutoCommit(0) << "]" << std::endl;
    if( d->GetAutoCommit() )
      std::cout << "Auto Commit enabled for table" << std::endl;
    else
      std::cout << "Auto Commit disabled for table" << std::endl;
  }
}
/*************************************************************************************/
void xbUtil::CopyDbfStructure(){
  xbDbf   *d;
  xbInt16 rc;
  char    filename[128];
  xbFile  f( x );
  xbDbf   *dNewTable;
  d = GetTablePtr( "" );

  if( d ){

    if( d->GetVersion() == 3 ){
      #ifdef XB_DBF3_SUPPORT
      dNewTable = new xbDbf3( x );
      #else
      std::cout << "Dbase III file support not build into library. See XB_DBF3_SUPPORT" << std::endl;
      return;
      #endif
    } else if( d->GetVersion() == 4 ){
      #ifdef XB_DBF4_SUPPORT
      dNewTable = new xbDbf4( x );
      #else
      std::cout << "Dbase IV file support not build into library. See XB_DBF4_SUPPORT" << std::endl;
      return;
      #endif
    } else {
      std::cout << "Unsupported file type file = " << d->GetVersion() << std::endl;
      return;
    }
    std::cout << "Copy Table" << std::endl;
    std::cout << "Enter new DBF file name (ie; MYFILE.DBF): ";
    std::cin.getline( filename,  128 );
    f.SetFileName( filename );
    if( strlen( filename ) == 0 ){
      std::cout << "No file name entered" << std::endl;
      return;
    }
    if(( rc = f.FileExists( f.GetFqFileName() )) == xbTrue ){
      std::cout << "File [" << f.GetFqFileName().Str() << "] already exists " << std::endl;
      return;
    }
    if(( rc = d->CopyDbfStructure( dNewTable, filename, filename, 0, XB_MULTI_USER )) != XB_NO_ERROR ){
      std::cout << "Error " << rc << " creating new file" << std::endl;
      x->DisplayError( rc );
      return;
    }
    std::cout << "Table " << f.GetFqFileName().Str() << " created" << std::endl;
    if(( rc = dNewTable->Close()) != XB_NO_ERROR ){
      std::cout << "Error " << rc << " closing new file" << std::endl;
      x->DisplayError( rc );
      return;
    }
  }
}
/*************************************************************************************/
void xbUtil::ZapTable(){

  xbInt16 iRc;
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );
  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }
  DisplayActiveTable();

  iRc = dActiveTable->Zap();
  if( iRc == XB_NO_ERROR )
    std::cout << "Table Zapped (all rows deleted)" << std::endl;
  else
    x->DisplayError( iRc );
}
/*************************************************************************************/
void xbUtil::Pack(){

  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );
  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }
  DisplayActiveTable();
  xbInt16 iRc;
  iRc = dActiveTable->Pack();
  if( iRc == XB_NO_ERROR )
    std::cout << "Table Packed" << std::endl;
  else
    x->DisplayError( iRc );
}
/*************************************************************************************/
void xbUtil::DisplayTableStats(){

  xbDbf *d;
  char    cBuf[15];
  xbInt16 iOptionNo;
  d = GetTablePtr( "" );
  if( d ){
    std::cout << "Enter option"                                    << std::endl;
    std::cout << "1 - Header data only"                            << std::endl;
    std::cout << "2 - Field data only"                             << std::endl;
    std::cout << "3 - Header and Field data"                       << std::endl;
    std::cout << "4 - Header, Field and Memo Header if applicable" << std::endl;
    std::cout << "5 - DBMS Settings (not stored in the file)"      << std::endl << std::endl;
    std::cin.getline( cBuf,  15 );
    if( strlen( cBuf ) == 0 ){
      std::cout << "Option not entered" << std::endl;
      return;
    }
    iOptionNo = atoi( cBuf );
    if( iOptionNo < 1 || iOptionNo > 5 ){
      std::cout << "Invalid option [" << cBuf << "] entered. Defaulting to 1" << std::endl;
      iOptionNo = 1;
    }
    if( iOptionNo < 5 ) {
      // d->ReadHeader( xbTrue, 0 );  moved to DumpHeader routine
      d->DumpHeader( iOptionNo );
    } else {
    // DBMS settings
      if( d->GetAutoCommit( 0 ) == -1 ){
        std::cout << "Table Auto Commit     = [Use DBMS Setting]"  << std::endl;
        if( x->GetDefaultAutoCommit() )
          std::cout << "DBMS Auto Commit      = [ON]"  << std::endl;
        else
          std::cout << "DBMS Auto Commit      = [OFF]"  << std::endl;
      }
      else if( d->GetAutoCommit( 0 ) == 0 )
        std::cout << "Table Auto Commit     = [OFF]" << std::endl;
      else
        std::cout << "Table Auto Commit     = [ON]" << std::endl;
    }
  }
}
/*************************************************************************************/
void xbUtil::CloseAllTables(){

  xbInt16 sOpenTableCnt = x->GetOpenTableCount();
  xbInt16 rc;
  if( sOpenTableCnt == 0 ){
    std::cout << "No open tables" << std::endl;
    return;
  }
  rc = x->CloseAllTables();
  if( rc == XB_NO_ERROR )
   std::cout << "All open tables closed" << std::endl;
  else
    x->DisplayError( rc );
  dActiveTable = NULL;
}

/*************************************************************************************/
void xbUtil::Close(){
  xbDbf   *d;
//  xbInt16 sOpenTableCnt = x->GetOpenTableCount();

  d = GetTablePtr( "to close" );
  if( d ){
    d->Close();
    std::cout << "Table closed" << std::endl;
    if( d == dActiveTable )
      dActiveTable = NULL;
  }
  else
    printf( "Can't identify table\n");
}
/*************************************************************************************/
void xbUtil::Open(){
  xbInt16  rc;
  xbFile   f( x );
  xbDbf    *dTable;
  char     filename[128];
  unsigned char cFileTypeByte;

  std::cout << "Open Table" << std::endl;
  std::cout << "Enter DBF file name (.DBF): ";
  std::cin.getline( filename,  128 );
  f.SetFileName( filename );

  if( strlen( filename ) == 0 ){
    std::cout << "No file name entered" << std::endl;
    return;
  }
  if(( rc = f.FileExists( f.GetFqFileName() )) != xbTrue ){
    std::cout << "File [" << f.GetFqFileName().Str() << "] does not exist " << std::endl;
    return;
  }
  if(( rc = f.GetXbaseFileTypeByte( f.GetFqFileName(), cFileTypeByte )) != XB_NO_ERROR ){
    std::cout << "Could not open file or determine file type rc = " << rc << " file = " << filename << std::endl;
    x->DisplayError( rc );
    return;
  }
  std:: cout << "File Type Byte ";
  x->BitDump( cFileTypeByte );
  std::cout << "\n";
  std::cout << "Table Type = [" << f.DetermineXbaseTableVersion( cFileTypeByte ) << "]\n";
  std::cout << "Memo Type  = [" << f.DetermineXbaseMemoVersion( cFileTypeByte ) << "]\n";

  if( f.DetermineXbaseTableVersion( cFileTypeByte ) == 4 ){
    #ifdef XB_DBF4_SUPPORT
      dTable = new xbDbf4( x );
    #else
      std::cout << "Dbase IV file support not build into library. See XB_DBF4_SUPPORT" << std::endl;
      return;
    #endif

  } else if( f.DetermineXbaseTableVersion( cFileTypeByte ) == 3 ){
    #ifdef XB_DBF3_SUPPORT
      dTable = new xbDbf3( x );
    #else
      std::cout << "Dbase III file support not build into library. See XB_DBF3_SUPPORT" << std::endl;
      return;
    #endif
  } else {
    std::cout << "Unsupported file type file = " << filename << " type = ";
    x->BitDump( cFileTypeByte );
    std::cout << std::endl;
    return;
  }

  if(( rc =  dTable->Open( filename )) != 0 ){
    std::cout << "Could not open file rc = " << rc  << " file = "  << filename << std::endl;
    x->DisplayError( rc );
    return;
  }

  dActiveTable = dTable;
  dActiveTable->GetFirstRecord();

  if( dActiveTable )
    std::cout << dActiveTable->GetTblAlias().Str() << " opened" << std::endl;
  else
    std::cout << "dActiveTable not set" << std::endl;
}
/*************************************************************************************/
#ifdef XB_LOCKING_SUPPORT
void xbUtil::UpdateDefaultLockRetries(){
  char cBuf[15];
  std::cout << std::endl << "Enter new Default Lock Retry Count: " << std::endl;
  std::cin.getline( cBuf, 15 );
  if( strlen( cBuf ) == 0 ){
    std::cout << "Default Lock Retry Count not updated" << std::endl;
    return;
  } else {
    x->SetDefaultLockRetries( atoi( cBuf ));
    std::cout << "Default Lock Retry Count updated to ["
              << x->GetDefaultLockRetries() << "]" << std::endl;
  }
}

void xbUtil::UpdateDefaultLockWait(){
  char cBuf[15];
  std::cout << std::endl << "Enter new Default Lock Wait Time (in millisecs 1000=1 second): " << std::endl;
  std::cin.getline( cBuf, 15 );
  if( strlen( cBuf ) == 0 ){
    std::cout << "Default Lock Wait Time not updated" << std::endl;
    return;
  } else {
    x->SetDefaultLockWait( atoi( cBuf ));
    std::cout << "Default Lock Wait Time updated to ["
              << x->GetDefaultLockWait() << "]" << std::endl;
  }
}

void xbUtil::UpdateDefaultLockFlavor(){
  char cBuf[15];
  std::cout << std::endl;
  std::cout << "Enter new Default Lock Flavor: "    << std::endl;
  std::cout << "1 = DBase"                          << std::endl;
  std::cout << "2 = Clipper  (not implemented yet)" << std::endl;
  std::cout << "3 = Fox      (not implemented yet)" << std::endl;
  std::cout << "9 = XBase64  (not implemented yet)" << std::endl;
  std::cin.getline( cBuf, 15 );
  if( strlen( cBuf ) == 0 ){
    std::cout << "Default Lock Flavor not updated" << std::endl;
    return;
  } else {
    x->SetDefaultLockFlavor( atoi( cBuf ));
    std::cout << "Default Lock Flavor updated to ["
              << x->GetDefaultLockFlavor() << "]" << std::endl;
  }
}
void xbUtil::ToggleDefaultAutoLock(){
  if( x->GetDefaultAutoLock()){
    x->DisableDefaultAutoLock();
    x->WriteLogMessage( "xb_dbfutil - Default Auto Lock disabled" );
    std::cout << "Default Auto Lock disabled" << std::endl;
  } else {
    x->EnableDefaultAutoLock();
    x->WriteLogMessage( "xb_dbfutil - Default Auto Lock enabled" );
    std::cout << "Default Auto Lock enabled" << std::endl;
  }
} 

void xbUtil::ToggleMultiUserMode(){
  if( x->GetMultiUser()){
    x->SetMultiUser( xbFalse );
    x->WriteLogMessage( "xb_dbfutil - Multi user mode disabled" );
    std::cout << "Multi user mode disabled" << std::endl;
  } else {
    x->SetMultiUser( xbTrue );
    x->WriteLogMessage( "xb_dbfutil - Multu user mode enabled" );
    std::cout << "Multi user mode enabled" << std::endl;
  }
}
#endif


#if defined(XB_NDX_SUPPORT) || defined (XB_MDX_SUPPORT)
void xbUtil::ToggleIndexOpt(){

  std::cout << "XBase64 supports two modes for processing index files that are defined as Unique" << std::endl << std::endl;
  std::cout << "XB_EMULATE_DBASE  - For unique index tags, DBase (TM) allows multiple records in a given" << std::endl;
  std::cout << "                    table to have the same key but only the first record for said key " << std::endl;
  std::cout << "                    is included in the index tag." << std::endl << std::endl;
  std::cout << "XB_HALT_ON_DUPKEY - The Xbase64 routines will halt whenever a duplicate key is" << std::endl;
  std::cout << "                    encountered and throw a XB_KEY_NOT_UNIQUE (-121) error." << std::endl << std::endl;
  std::cout << "Choose XB_EMULATE_DBASE  if you need to be compatible with DBase (TM)." << std::endl << std::endl;
  std::cout << "Choose XB_HALT_ON_DUPKEY if you want the library to behave like other modern RDBMS environments where" << std::endl;
  std::cout << "                         duplicate key values are not allowed and dup records are not allowed." << std::endl << std::endl;
  

  if( x->GetUniqueKeyOpt() == XB_HALT_ON_DUPKEY ){
    x->SetUniqueKeyOpt( XB_EMULATE_DBASE );
    std::cout << "Unique Key Option set to [XB_EMULATE_DBASE]" << std::endl;
  } else {
    x->SetUniqueKeyOpt( XB_HALT_ON_DUPKEY );
    std::cout << "Unique Key Option set to [XB_HALT_ON_DUPKEY]" << std::endl;
  }
}
#endif

/*************************************************************************************/
void xbUtil::ListFilesInDataDirectory(){
  std::cout << "List files for [" << x->GetDataDirectory().Str() << "]" << std::endl << std::endl;
  xbString s1;
  xbInt16 iCnt = 0;

#ifdef WIN32

  WIN32_FIND_DATA fData;
  HANDLE hFile;

  xbString sSearchPath = x->GetDataDirectory();
  xbUInt32 l = sSearchPath.Len();
  char cPathSeperator = sSearchPath.GetPathSeparator();
  char cLastChar = sSearchPath.GetCharacter( l );

  if( cLastChar == cPathSeperator )
    sSearchPath += "*.*";
  else if( cPathSeperator ){
    sSearchPath += cPathSeperator;
    sSearchPath += "*.*";
  }
  else
   sSearchPath += "\\*.*";

  hFile = FindFirstFile( sSearchPath.Str(), &fData );
  if( hFile == INVALID_HANDLE_VALUE ){
    std::cout << "Could not open directory" << std::endl;
    return;
  }

  do{
    s1 = fData.cFileName;
    if( s1 != "." && s1 != ".." ){
      std::cout << fData.cFileName << std::endl;
      iCnt++;
    }
  } while( FindNextFile( hFile, &fData ));
    std::cout << std::endl << iCnt << " entries" << std::endl;
#else
  DIR *dir;
  struct dirent *ent;

  if(( dir = opendir( x->GetDataDirectory() )) != NULL ){
    while(( ent = readdir( dir )) != NULL ){
      s1 = ent->d_name;
      if( s1 != "." && s1 != ".." ){
        std::cout << ent->d_name << std::endl;
        iCnt++;
      }
    }
    std::cout << std::endl << iCnt << " entries" << std::endl;
    closedir( dir );
  }
  else
    std::cout << "Could not open directory" << std::endl;
#endif
}
/*************************************************************************************/
void xbUtil::UpdateLogDirectory(){

  char cNewDir[256];
  cNewDir[0] = 0x00;
  std::cout << std::endl << "Update Log Directory" << std::endl;
  std::cout << "Current Log File Directory = [" << x->GetLogDirectory().Str() << "]" << std::endl;
  std::cout << "Enter '1' to erase the Log File Directory" << std::endl;
  std::cout << "Enter '2' to change Log File Directory to [" << x->GetLogDirectory().Str() << "]" << std::endl;
  std::cout << "Enter new log directory.  Enter for no updates." << std::endl;
  std::cin.getline( cNewDir, 256 );
  if( strlen( cNewDir ) > 0 ){
    if( cNewDir[0] == '1' )
      x->SetLogDirectory( "" );
    else if( cNewDir[0] == '2' )
      x->SetLogDirectory( x->GetLogDirectory());
    else 
      x->SetLogDirectory( cNewDir );

    std::cout << "Log File Directory is [" << x->GetLogDirectory().Str() << "]" << std::endl;
  }
  else
   std::cout << "Log Directory not updated" << std::endl;
}
/*************************************************************************************/
void xbUtil::UpdateLogFileName(){
  char cNewFile[256];
  cNewFile[0] = 0x00;

  std::cout << std::endl << "Update Log File Name" << std::endl;
  std::cout << "Current Log File Name = [" << x->GetLogFileName().Str() << "]" << std::endl;
  std::cout << "Enter '1' to change change Log File Name to default [" << x->GetLogFileName().Str() << "]" << std::endl;
  std::cout << "Enter new Log File Name" << std::endl;
  std::cin.getline( cNewFile, 256 );
  if( strlen( cNewFile ) > 0 ){
    if( cNewFile[0] == '1' )
      x->SetLogFileName( x->GetLogFileName());
    else
      x->SetLogFileName( cNewFile );

    std::cout << "Log File Name is [" << x->GetLogFileName().Str() << "]" << std::endl;
  } else
    std::cout << "Log File Name not updated" << std::endl;
}
/*************************************************************************************/
void xbUtil::WriteLogMessage(){
  if( x->GetLogStatus()) {
    char cMsg[256];
    std::cout << "Enter a message to write to the log file (256 byte max)" << std::endl;
    std::cin.getline( cMsg, 256 );
    x->WriteLogMessage( cMsg );
  } else 
    std::cout << "Logging disabled" << std::endl;
}
/*************************************************************************************/
void xbUtil::ToggleLoggingStatus(){
  if( x->GetLogStatus()){
    x->DisableMsgLogging();
    x->WriteLogMessage( "xb_dbfutil - Logging disabled" );
    std::cout << "Logging disabled" << std::endl;
  } else {
    x->EnableMsgLogging();
    x->WriteLogMessage( "xb_dbfutil - Logging enabled" );
    std::cout << "Logging enabled" << std::endl;
  }
}
/*************************************************************************************/
void xbUtil::ToggleDefaultAutoCommit(){
  if( x->GetDefaultAutoCommit()){
    x->SetDefaultAutoCommit( xbFalse );
    x->WriteLogMessage( "xb_dbfutil - Default Auto Commit disabled" );
    std::cout << "Default Auto Commit disabled" << std::endl;
  } else {
    x->SetDefaultAutoCommit( xbTrue );
    x->WriteLogMessage( "xb_dbfutil - Default Auto Commit enabled" );
    std::cout << "Default Auto Commit enabled" << std::endl;
  }
}
/*************************************************************************************/
void xbUtil::UpdateDataDirectory(){
 char cNewDir[256];
  cNewDir[0] = 0x00;

  xbString sWork;
  xbString sHomeDir;
  char cPathSeparator;

  #ifdef WIN32
  cPathSeparator = '\\';
  #else
  cPathSeparator = '/';

  #endif

  x->GetHomeDir( sHomeDir );
  sWork.Sprintf( "%s%cxbase64%cdata", sHomeDir.Str(), cPathSeparator, cPathSeparator );

  std::cout << std::endl << "Update Default Data Directory" << std::endl;
  std::cout << "Current Data Directory = [" << x->GetDataDirectory().Str() << "]" << std::endl;
  std::cout << "Enter '1' to erase the Default Data Directory" << std::endl;
  std::cout << "Enter '2' to change Default Data Directory to [" << PROJECT_DATA_DIR << "]" << std::endl;
  std::cout << "Enter '3' to change Default Data Directory to [" << sWork.Str() << "]" << std::endl;

  std::cout << "Enter new directory" << std::endl;
  std::cin.getline( cNewDir, 256 );
  if( strlen( cNewDir ) > 0 ){
    if( cNewDir[0] == '1' )
      x->SetDataDirectory( "" );
    else if( cNewDir[0] == '2' )
      x->SetDataDirectory( PROJECT_DATA_DIR );
    else if( cNewDir[0] == '3' )
      x->SetDataDirectory( sWork.Str());
    else
      x->SetDataDirectory( cNewDir );
    std::cout << "Default Data Directory is [" << x->GetDataDirectory().Str() << "]" << std::endl;
  }
  else
    std::cout << "Default Data Directory not updated" << std::endl;
}
/*************************************************************************************/
void xbUtil::ListSystemSettings(){
  std::cout << std::endl << "List System Settings" << std::endl;
  std::cout << "Default Data Directory  = [" << x->GetDataDirectory().Str()    << "]" << std::endl;
//  std::cout << "Default File Version    = [" << x->GetDefaultFileVersion()      << "]" << std::endl;
  #ifdef XB_LOCKING_SUPPORT
  std::cout << "Default Auto Locking    = [";
  if( x->GetDefaultAutoLock())
    std::cout << "ON]" << std::endl;
  else
    std::cout << "OFF]" << std::endl;

  std::cout << "Default Lock Retries    = [" << x->GetDefaultLockRetries()      << "]" << std::endl;
  std::cout << "Default Lock Wait Time  = [" << x->GetDefaultLockWait()         << "] (millisecs)" << std::endl;

  std::cout << "Default Lock Flavor     = [";
  switch (x->GetDefaultLockFlavor()){
    case 1:
      std::cout << "Dbase]" << std::endl;
      break;
    case 2:
      std::cout << "Clipper]" << std::endl;
      break;
    case 3:
      std::cout << "Fox]" << std::endl;
      break;
    case 9:
      std::cout << "Xbase64]" << std::endl;
      break;
    default:
      std::cout << "Unknown]" << std::endl;
      break;
  } 
  #endif
  std::cout << "Log Directory           = [" << x->GetLogDirectory().Str()     << "]" << std::endl;
  std::cout << "Logfile Name            = [" << x->GetLogFileName().Str()      << "]" << std::endl;
  std::cout << "Default Auto Commit     = [";
  if( x->GetDefaultAutoCommit())
    std::cout << "ON]" << std::endl;
  else
    std::cout << "OFF]" << std::endl;

  std::cout << "Logging Status          = [";
  if( x->GetLogStatus())
    std::cout << "ON]" << std::endl;
  else
    std::cout << "OFF]" << std::endl;
  std::cout << "Endian Type             = [";
  if( x->GetEndianType() == 'L' )
    std::cout << "Little Endian]" << std::endl;
  else
    std::cout << "Big Endian]" << std::endl;

  if( x->GetMultiUser())
    std::cout << "Multi User Mode         = [ON]" << std::endl;
  else
    std::cout << "Multi User Mode         = [OFF]" << std::endl;

  #if defined(XB_NDX_SUPPORT) || defined (XB_MDX_SUPPORT)
  if( x->GetUniqueKeyOpt() == XB_HALT_ON_DUPKEY )
    std::cout << "Duplicate Index Key Opt = [XB_HALT_ON_DUPKEY]" << std::endl;
  else
    std::cout << "Duplicate Index Key Opt = [XB_EMULATE_DBASE]" << std::endl;
  #endif
}
/*************************************************************************************/
/* This method handles all the complex menu option commands */
void xbUtil::ProcessOption( const xbString &sOption ){

  if( sOption[1] == '=' ){
    if( sOption == "=0" )
      MainMenu();
    else if( sOption == "=0.99" )
      // exit right now, now cleanup or termination of anything
      exit(0);
    else if( sOption == "=1" )
      Help();
    else if( sOption == "=2" )
      SystemMenu();
    else if( sOption == "=2.1" )
      ListSystemSettings();
    else if( sOption == "=2.2"  )
      UpdateDataDirectory();
    else if( sOption == "=2.3" )
      ToggleDefaultAutoCommit();
    else if( sOption == "=2.4" )
      UpdateLogDirectory();
    else if( sOption == "=2.5" )
      UpdateLogFileName();
    else if( sOption == "=2.6" )
      ToggleLoggingStatus();
    else if( sOption == "=2.7" )
      WriteLogMessage();
    #ifdef XB_LOCKING_SUPPORT
    else if( sOption == "=2.8" )
      UpdateDefaultLockRetries();
    else if( sOption == "=2.9" )
      ToggleDefaultAutoLock();
    else if( sOption == "=2.10" )
      UpdateDefaultLockFlavor();
    else if( sOption == "=2.11" )
      UpdateDefaultLockWait();
    else if( sOption == "=2.12" )
      ToggleMultiUserMode();
    #endif

    #if defined(XB_NDX_SUPPORT) || defined (XB_MDX_SUPPORT)
    else if( sOption == "=2.13" )
      ToggleIndexOpt();
    #endif

    else if( sOption == "=3" )
      FileMenu();
    else if( sOption == "=3.1" )
      ListFilesInDataDirectory();
    else if( sOption == "=3.2" )
      UpdateDataDirectory();
    else if( sOption == "=3.3" )
      x->DisplayTableList();
    else if( sOption == "=3.4" )
      Open();
    else if( sOption == "=3.5" )
      Close();
    else if( sOption == "=3.6" )
      CloseAllTables();
    else if( sOption == "=3.7" )
      SelectActiveTable();
    else if( sOption == "=3.8" )
      DisplayTableStats();
    else if( sOption == "=3.10" )
      Pack();
    else if( sOption == "=3.11" )
      ZapTable();
    else if( sOption == "=3.12" )
      CopyDbfStructure();
    else if( sOption == "=3.13" )
      UpdateTableAutoCommit();
    else if( sOption == "=3.14" )
      DisplayTableInfo();
    else if( sOption == "=3.15" )
      RenameTable();
    else if( sOption == "=3.16" )
      DeleteTable();
    else if( sOption == "=4" )
      RecordMenu();
    else if( sOption == "=4.1" )
      SelectActiveTable();
    else if( sOption == "=4.2" )
      GetRecord();
    else if( sOption == "=4.3" )
      BlankRecord();
    else if( sOption == "=4.4" )
      AppendRecord();
    else if( sOption == "=4.5" )
      PutRecord();
    else if( sOption == "=4.6" )
      DeleteRecord();
    else if( sOption == "=4.7" )
      UndeleteRecord();
    else if( sOption == "=4.8" )
      GetFirstRecord();
    else if( sOption == "=4.9" )
     GetNextRecord();
    else if( sOption == "=4.10" )
      GetPrevRecord();
    else if( sOption == "=4.11" )
      GetLastRecord();
    else if( sOption == "=4.12" )
      DumpRecord();
    else if( sOption == "=4.13" )
      AbortRecord();
    else if( sOption == "=4.14" )
      CommitRecord();
    #ifdef XB_FILTER_SUPPORT
    else if( sOption == "=4.20" )
      SetFilter();
    else if( sOption == "=4.21" )
      GetFirstFilterRec();
    else if( sOption == "=4.22" )
      GetNextFilterRec();
    else if( sOption == "=4.23" )
      GetPrevFilterRec();
    else if( sOption == "=4.24" )
      GetLastFilterRec();
    #endif  //  XB_FILTER_SUPPORT
    else if( sOption == "=5" )
      FieldMenu();
    else if( sOption == "=5.1" )
      SelectActiveTable();
    else if( sOption == "=5.2" )
      ListFieldInfo();
    else if( sOption == "=5.3" )
      DumpRecord();
    else if( sOption == "=5.4" )
      UpdateFieldData();
    #ifdef XB_MEMO_SUPPORT
    else if( sOption == "=5.5" )
      ShowMemoFieldData();
    else if( sOption == "=5.6" )
      UpdateMemoFieldData();
    else if( sOption == "=5.7" )
      DeleteMemoField();
    #endif

    #ifdef XB_LOCKING_SUPPORT
    else if( sOption == "=6" )
      LockingMenu();
    else if( sOption == "=6.1" )
      DisplayFileLockSettings();
    else if( sOption == "=6.2" )
      UpdateFileLockRetryCount();
    else if( sOption == "=6.3" )
      UpdateFileLockFlavor();
    else if( sOption == "=6.4" )
      UpdateFileAutoLock();
    else if( sOption == "=6.5" )
      LockDbf();
    else if( sOption == "=6.6" )
      UnlockDbf();
    else if( sOption == "=6.7" )
      LockRecord();
    else if( sOption == "=6.8" )
      UnlockRecord();
    else if( sOption == "=6.9" )
      LockAppend();
    else if( sOption == "=6.10" )
      UnlockAppend();
    else if( sOption == "=6.11" )
      LockHeader();
    else if( sOption == "=6.12" )
      UnlockHeader();
    #ifdef XB_MEMO_SUPPORT
    else if( sOption == "=6.13" )
      LockMemo();
    else if( sOption == "=6.14" )
      UnlockMemo();
    #endif

    #ifdef XB_DEBUG_SUPPORT
    else if( sOption == "=6.20" )
      xbFileLock();
    else if( sOption == "=6.21" )
      xbFileUnlock();
    #endif  // XB_DEBUG_SUPPORT
    #endif  // XB_LOCKING_SUPPORT

    #ifdef XB_EXPRESSION_SUPPORT
    else if( sOption == "=7" )
      ExpressionMenu();
    else if( sOption == "=7.1" )
      ParseExpression( 0 );
    else if( sOption == "=7.2" )
      ProcessParsedExpression( 0 );
    else if( sOption == "=7.3" )
      ParseAndProcessExpression();

    #ifdef XB_DEBUG_SUPPORT
    else if (sOption == "=7.4" )
      DumpExpressionTree();
    #endif // XB_DEBUG_SUPPORT
    else if (sOption == "=7.10" )
      Date8ToJul();
    else if (sOption == "=7.11" )
      JulToDate8();
    else if (sOption == "=7.12" )
      IsLeapYear();
    #endif  // XB_EXPRESSION_SUPPORT

    #ifdef XB_INDEX_SUPPORT
    else if( sOption == "=8" )
      IndexMenu();
    else if( sOption == "=8.1" )
      SelectActiveTable();
    else if( sOption == "=8.2" )
      DisplayOpenIndexFiles();
    else if( sOption == "=8.3" )
      DisplaySupportedIndexTypes();
    else if( sOption == "=8.4" )
      SelectTag();
    else if( sOption == "=8.5" )
      OpenIndex();
    else if( sOption == "=8.6" )
      CloseIndexFile();
    else if( sOption == "=8.7" )
      CreateIndexTag();
    else if( sOption == "=8.8" )
      GetFirstKey();
    else if( sOption == "=8.9" )
      GetPrevKey();
    else if( sOption == "=8.10" )
      GetNextKey();
    else if( sOption == "=8.11" )
      GetLastKey();
    else if( sOption == "=8.12" )
      FindKey();

    else if( sOption == "=8.13" )
      CheckIntegrity();
    else if( sOption == "=8.14" )
      Reindex();
    else if( sOption == "=8.15" )
      DeleteTag();

    #ifdef XB_INF_SUPPORT
    else if( sOption == "=8.16" )
      AddInfFileData();
    #endif


    #ifdef XB_DEBUG_SUPPORT
    else if( sOption == "=8.20" )
      DumpIxHeader();
    else if( sOption == "=8.21" )
      DumpIxNode();
    else if( sOption == "=8.22" )
      DumpIxNodeChain();
    else if( sOption == "=8.23" )
      DumpRecsByIx(0);
    else if( sOption == "=8.24" )
      DumpRecsByIx(1);
    else if( sOption == "=8.25" )
      DumpFreeBlocks();
    #endif // XB_DEBUG_SUPPORT
    #endif // XB_INDEX_SUPPORT

    #ifdef XB_SQL_SUPPORT
    else if( sOption == "=10" )
      SqlMenu();
    else if( sOption == "=10.1" )
      ExecSqlNonQuery();
    else if( sOption == "=10.2" )
      ExecSqlQuery();
    #endif // XB_SQL_SUPPORT

    #ifdef XB_INF_SUPPORT
    else if( sOption == "=11" )
      InfFileMenu();
    else if( sOption == "=11.1" )
      ListInfFileData();
    else if( sOption == "=11.2" )
      AddInfFileData();
    else if( sOption == "=11.3" )
      DeleteInfFileData();
    else if( sOption == "=11.10" )
      InfFileHelp();
    #endif // XB_INF_SUPPORT

    #ifdef XB_DEBUG_SUPPORT
    else if( sOption == "=20" )
      DebugMenu();
    else if( sOption == "=20.1" )
      SelectActiveTable();
/*
    #ifdef XB_MEMO_SUPPORT
    else if( sOption == "=10.2" )
      DumpDbtHeader();
    else if( sOption == "=10.3" )
      DumpMemoFreeChain();
    #endif
*/

    #endif
    else
      return;
  }
}

/*************************************************************************************/
xbInt32 xbUtil::GetLong(){
  char cLine[256];
  xbString sLine;
  memset( cLine, 0x00, 256 );
  std::cin.getline( cLine, 256 );
  sLine = cLine;
  sLine.Trim();
  return atol( cLine );
}
/*************************************************************************************/
xbInt16 xbUtil::GetOption(){
  char cLine[256];
  xbString sLine;
  memset( cLine, 0x00, 256 );
  std::cin.getline( cLine, 256 );
  sLine = cLine;
  sLine.Trim();

  if( sLine[1] == '=' || sLine.Pos(".") != 0 ){
    ProcessOption( sLine );
    return 0;
  }
  else
    return atoi( cLine );
}
/************************************************************************/
#ifdef XB_INDEX_SUPPORT
void xbUtil::IndexMenu()
{
  int option = 0;

  while( option != 99 ) {

    std::cout << std::endl << std::endl << " 8 - Index Menu" << std::endl;
    std::cout << "---------------------" << std::endl;
    DisplayActiveTable();

    std::cout << "  1 - Select Active Table"             << std::endl;
    std::cout << "  2 - Display Open Index Files"        << std::endl;
    std::cout << "  3 - Display Supported Index Types"   << std::endl;
    std::cout << "  4 - Select Active Tag"               << std::endl;
    std::cout << "  5 - Open Index File"                 << std::endl;
    std::cout << "  6 - Close Index File"                << std::endl;
    std::cout << "  7 - Create Index Tag"                << std::endl;
    std::cout << "  8 - Get First Key"                   << std::endl;
    std::cout << "  9 - Get Next Key"                    << std::endl;
    std::cout << " 10 - Get Prev Key"                    << std::endl;
    std::cout << " 11 - Get Last Key"                    << std::endl;
    std::cout << " 12 - Find Key"                        << std::endl;
    std::cout << " 13 - Check Index Integrity"           << std::endl;
    std::cout << " 14 - Reindex"                         << std::endl;
    std::cout << " 15 - Delete Tag"                      << std::endl;
    #ifdef XB_INF_SUPPORT
    std::cout << " 16 - Associate NDX file"              << std::endl;
    #endif


    #ifdef XB_DEBUG_SUPPORT
    std::cout << std::endl;
    std::cout << " 20 - Dump Header"                     << std::endl;
    std::cout << " 21 - Dump Node(s)"                    << std::endl;
    std::cout << " 22 - Dump Node Chain"                 << std::endl;
    std::cout << " 23 - Dump Recs by Tag Fwd"            << std::endl;
    std::cout << " 24 - Dump Recs by Tag Bwd"            << std::endl;
    std::cout << " 25 - Dump Free Blocks / MDX ulBlock2" << std::endl;
    #endif // XB_DEBUG_SUPPORT

    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << " 99 - Exit Menu"                       << std::endl;
    option = GetOption();

    switch( option ){
      case 0: break;
      case 1:  SelectActiveTable();          break;
      case 2:  DisplayOpenIndexFiles();      break;
      case 3:  DisplaySupportedIndexTypes(); break;
      case 4:  SelectTag();                  break;
      case 5:  OpenIndex();                  break;
      case 6:  CloseIndexFile();             break;
      case 7:  CreateIndexTag();             break;

      case 8:  GetFirstKey();                break;
      case 9:  GetNextKey();                 break;
      case 10: GetPrevKey();                 break;
      case 11: GetLastKey();                 break;

      case 12: FindKey();                    break;
      case 13: CheckIntegrity();             break;
      case 14: Reindex();                    break;
      case 15: DeleteTag();                  break;
      #ifdef XB_INF_SUPPORT
      case 16: AddInfFileData();             break;
      #endif

      #ifdef XB_DEBUG_SUPPORT
      case 20: DumpIxHeader();               break;
      case 21: DumpIxNode();                 break;
      case 22: DumpIxNodeChain();            break;
      case 23: DumpRecsByIx(0);              break;
      case 24: DumpRecsByIx(1);              break;
      case 25: DumpFreeBlocks();             break;
      #endif

      case 99: break;
      default: std::cout << "Invalid option" << std::endl; break;
    }
  }
}
#endif
/************************************************************************/
#ifdef XB_EXPRESSION_SUPPORT
void xbUtil::ExpressionMenu()
{
  int option = 0;

  while( option != 99 ) {
    std::cout << std::endl << std::endl << " 7 - Expression Menu" << std::endl;
    std::cout << " --------------" << std::endl;
    DisplayActiveTable();
    std::cout << "  1 - Parse Expression"             << std::endl;
    std::cout << "  2 - Process Parsed Expression"    << std::endl;
    std::cout << "  3 - Parse and Process Expression" << std::endl;
    #ifdef XB_DEBUG_SUPPORT
    std::cout << "  4 - Dump Expression Internals"    << std::endl;
    #endif
    std::cout << " 10 - Date8 To Julian Date"         << std::endl;
    std::cout << " 11 - Julian Date to Date8"         << std::endl;
    std::cout << " 12 - Check Leap Year"              << std::endl;
    std::cout << " 99 - Exit Menu"                    << std::endl;
    option = GetOption();
   switch( option ){
    case 0:                                break;
    case 1:  ParseExpression( 0 );         break;
    case 2:  ProcessParsedExpression( 0 ); break;
    case 3:  ParseAndProcessExpression();  break;
    #ifdef XB_DEBUG_SUPPORT
    case 4:  DumpExpressionTree();         break;
    #endif
    case 10: Date8ToJul();                 break;
    case 11: JulToDate8();                 break;
    case 12: IsLeapYear();                 break;
    case 99: break;
    default:  std::cout << "Invalid option" << std::endl; break;
   }
 }
}
#endif
/************************************************************************/
#ifdef XB_LOCKING_SUPPORT
void xbUtil::LockingMenu()
{
  int option = 0;

  while( option != 99 ) {
    std::cout << std::endl << std::endl << " 6 - Locking Menu" << std::endl;
    std::cout << "-------------" << std::endl;
    DisplayActiveTable();

    std::cout << "  1 - Display File Specific Settings"    << std::endl;
    std::cout << "  2 - Update File Retry Count"           << std::endl;
    std::cout << "  3 - Update Locking Flavor"             << std::endl;
    std::cout << "  4 - Update Auto Lock"                  << std::endl;
    std::cout << "  5 - Lock table (dbf file)"             << std::endl;
    std::cout << "  6 - Unlock table (dbf file)"           << std::endl;
    std::cout << "  7 - Lock Record"                       << std::endl;
    std::cout << "  8 - Unlock Record"                     << std::endl;
    std::cout << "  9 - Lock Append"                       << std::endl;
    std::cout << " 10 - Unlock Append"                     << std::endl;
    std::cout << " 11 - Lock Header"                       << std::endl;
    std::cout << " 12 - Unlock Header"                     << std::endl;


   #ifdef XB_MEMO_SUPPORT
   std::cout << " 13 - Lock Memo File"               << std::endl;
   std::cout << " 14 - Unlock Memo File"             << std::endl;
   #endif  // XB_MEMO_SUPPORT

   #ifdef XB_INDEX_SUPPORT
   std::cout << " 15 - Lock Index File(s)"           << std::endl;
   std::cout << " 16 - Unlock Index File(s)"         << std::endl;
   #endif  // XB_INDEX_SUPPORT

   #ifdef XB_DEBUG_SUPPORT
   std::cout << " 20 - Native xbFile - Lock Bytes"   << std::endl;
   std::cout << " 21 - Native xbFile - Unlock Bytes" << std::endl;
   #endif  // XB_DEBUG_SUPPORT

   std::cout << " 99 - Exit Menu"                    << std::endl;
   option = GetOption();

   switch( option ) {
    case 0:                                break;
    case 1:  DisplayFileLockSettings();    break;
    case 2:  UpdateFileLockRetryCount();   break;
    case 3:  UpdateFileLockFlavor();       break;
    case 4:  UpdateFileAutoLock();         break;
    case 5:  LockDbf();                    break;
    case 6:  UnlockDbf();                  break;
    case 7:  LockRecord();                 break;
    case 8:  UnlockRecord();               break;
    case 9:  LockAppend();                 break;
    case 10: UnlockAppend();               break;
    case 11: LockHeader();                 break;
    case 12: UnlockHeader();               break;

    #ifdef XB_MEMO_SUPPORT
    case 13: LockMemo();                   break;
    case 14: UnlockMemo();                 break;
    #endif  // XB_MEMO_SUPPORT

    #ifdef XB_INDEX_SUPPORT
    case 15: LockIndices();                break;
    case 16: UnlockIndices();              break;
    #endif // XB_INDEX_SUPPORT

    #ifdef XB_DEBUG_SUPPORT
    case 20:  xbFileLock();                break;
    case 21:  xbFileUnlock();              break;
    #endif // XB_DEBUG_SUPPORT

    case 99: break;
    default:  std::cout << "Invalid Option" << std::endl;
   }
 }
}
#endif

/************************************************************************/
#ifdef XB_SQL_SUPPORT
void xbUtil::SqlMenu()
{
  int option = 0;

  while( option != 99 ) {
    std::cout << std::endl << std::endl << "9 - Sql Menu"         << std::endl;
    std::cout << "-------------" << std::endl;
    DisplayActiveTable();

    std::cout << " 1 - Execute SQL Non Query" << std::endl;
    std::cout << " 2 - Execute SQL Query"     << std::endl;
    std::cout << "99 - Exit Menu"                               << std::endl;

   option = GetOption();
   switch( option ){
    case 0: break;
    case 1:  ExecSqlNonQuery();  break;
    case 2:  ExecSqlQuery();     break;
    case 99: break;
    default:  std::cout << "Invalid option" << std::endl; break;
   }
  }
}
#endif   //  XB_SQL_SUPPORT

/************************************************************************/
#ifdef XB_DEBUG_SUPPORT
void xbUtil::DebugMenu()
{
  int option = 0;

  while( option != 99 ) {
    std::cout << std::endl << std::endl << "10 - Debug Menu"         << std::endl;
    std::cout << "-------------" << std::endl;
    DisplayActiveTable();

    std::cout << " 1 - Select Active Table" << std::endl;
    #ifdef XB_MEMO_SUPPORT
    std::cout << " 2 - Dislay Memo Header Info"                 << std::endl;
    std::cout << " 3 - Dump Memo Free Chain"                    << std::endl;
    #endif

//  //std::cout << "4 - Dump index node chains to file xbase.dmp" << std::endl;
//  //std::cout << "5 - Dump index node chain"                    << std::endl;
    std::cout << "99 - Exit Menu"                               << std::endl;
    option = GetOption();
    switch( option ){
     case 0: break;
     case 1:  SelectActiveTable();  break;
     #ifdef XB_MEMO_SUPPORT
     case 2:  DumpDbtHeader();      break;
     case 3:  DumpMemoFreeChain();  break;
     #endif
     case 99: break;
     default:  std::cout << "Invalid option" << std::endl; break;
   }
  }
}
#endif   //  XBASE_DEBUG
/************************************************************************/
void xbUtil::FieldMenu()
{
  int option = 0;

  while( option != 99 ) {
    std::cout << std::endl << std::endl << " 5 - Field Menu" << std::endl;
    std::cout << " --------------" << std::endl;
    DisplayActiveTable();
    std::cout << " 1 - Select Active Table" << std::endl;
    std::cout << " 2 - List Field Info"   << std::endl;
    std::cout << " 3 - Show Field Data (non memo field)"   << std::endl;
    std::cout << " 4 - Update Field Data" << std::endl;
    #ifdef XB_MEMO_SUPPORT
    std::cout << " 5 - Show Memo Field Data" << std::endl;
    std::cout << " 6 - Update Memo Field" << std::endl;
    std::cout << " 7 - Delete Memo Field" << std::endl;
    #endif

    std::cout << "99 - Exit Menu" << std::endl;
    option = GetOption();
    switch( option ){
     case 0:                          break;
     case 1:  SelectActiveTable();    break;
     case 2:  ListFieldInfo();        break;
     case 3:  DumpRecord();           break;
     case 4:  UpdateFieldData();      break;
     #ifdef XB_MEMO_SUPPORT
     case 5:  ShowMemoFieldData();    break;
     case 6:  UpdateMemoFieldData();  break;
     case 7:  DeleteMemoField();      break;
     #endif
     case 99: break;
     default:  std::cout << "Invalid option" << std::endl; break;
   }
  }
}
/************************************************************************/
void xbUtil::RecordMenu()
{
  int option = 0;
  while( option != 99 ) {
   std::cout << std::endl << std::endl << " 4 - Record Menu" << std::endl;
   std::cout << " ---------------" << std::endl;
   DisplayActiveTable();
   std::cout << " 1 - Select Active Table"   << std::endl;
   std::cout << " 2 - Get Record"            << std::endl;
   std::cout << " 3 - Blank Record"          << std::endl;
   std::cout << " 4 - Append Record"         << std::endl;
   std::cout << " 5 - Put Record"            << std::endl;
   std::cout << " 6 - Delete Record"         << std::endl;
   std::cout << " 7 - Undelete Record"       << std::endl;
   std::cout << " 8 - First Record"          << std::endl;
   std::cout << " 9 - Next Record"           << std::endl;
   std::cout << "10 - Prev Record"           << std::endl;
   std::cout << "11 - Last Record"           << std::endl;
   std::cout << "12 - Dump Record"           << std::endl;
   std::cout << "13 - Abort Record Updates"  << std::endl;
   std::cout << "14 - Commit Record Updates" << std::endl;

   #ifdef XB_FILTER_SUPPORT
   std::cout << "20 - Set Filter"            << std::endl;
   std::cout << "21 - Get First Filter Rec"  << std::endl;
   std::cout << "22 - Get Next Filter Rec"   << std::endl;
   std::cout << "23 - Get Prev Filter Rec"   << std::endl;
   std::cout << "24 - Get Last Filter Rec"   << std::endl;
   #endif  //  XB_FILTER_SUPPORT
   
  std::cout << "99 - Exit Menu"       << std::endl;
   option = GetOption();

   switch( option ){
    case 0:                       break;
    case 1:  SelectActiveTable(); break;
    case 2:  GetRecord();         break;
    case 3:  BlankRecord();       break;
    case 4:  AppendRecord();      break;
    case 5:  PutRecord();         break;
    case 6:  DeleteRecord();      break;
    case 7:  UndeleteRecord();    break;
    case 8:  GetFirstRecord();    break;
    case 9:  GetNextRecord();     break;
    case 10: GetPrevRecord();     break;
    case 11: GetLastRecord();     break;
    case 12: DumpRecord();        break;
    case 13: AbortRecord();       break;
    case 14: CommitRecord();      break;

    #ifdef XB_FILTER_SUPPORT
    case 20: SetFilter();         break;
    case 21: GetFirstFilterRec(); break;
    case 22: GetNextFilterRec();  break;
    case 23: GetPrevFilterRec();  break;
    case 24: GetLastFilterRec();  break;
    #endif  //  XB_FILTER_SUPPORT

    case 99:                      break;
    default: std::cout << "Invalid option" << std::endl; break;
   }
  }
}

/************************************************************************/
void xbUtil::FileMenu()
{
  int option = 0;

  while( option != 99 ) {
    std::cout << std::endl << std::endl << " 3 - File / Table Menu"      << std::endl;
    std::cout << " ---------------------" << std::endl;
    DisplayActiveTable();
    std::cout << " 1 - List files in Current Data Directory" << std::endl;
    std::cout << " 2 - Update Current Data Directory"        << std::endl;
    std::cout << " 3 - List Open Tables/Files"               << std::endl;
    std::cout << " 4 - Open Table/File"                      << std::endl;
    std::cout << " 5 - Close Table/File"                     << std::endl;
    std::cout << " 6 - Close All Tables/Files"               << std::endl;
    std::cout << " 7 - Select Active Table"                  << std::endl;
    std::cout << " 8 - Table/File Information"               << std::endl;
    std::cout << "10 - Pack"                                 << std::endl;
    std::cout << "11 - Zap Database"                         << std::endl;
    std::cout << "12 - Copy Dbf Structure"                   << std::endl;
    std::cout << "13 - Update Table Auto Commit Setting"     << std::endl;
    std::cout << "14 - Display Table Info"                   << std::endl;
    std::cout << "15 - Rename Table"                         << std::endl;
    std::cout << "16 - Delete Table"                         << std::endl;

    std::cout << "99 - Exit Menu"        << std::endl;
    option = GetOption();

    switch( option ){
     case 0:                                      break;
     case 1:  ListFilesInDataDirectory();         break;
     case 2:  UpdateDataDirectory();              break;
     case 3:  x->DisplayTableList();              break;
     case 4:  Open();                             break;
     case 5:  Close();                            break;
     case 6:  CloseAllTables();                   break;
     case 7:  SelectActiveTable();                break;
     case 8:  DisplayTableStats();                break;
     case 10: Pack();                             break;
     case 11: ZapTable();                         break;
     case 12: CopyDbfStructure();                 break;
     case 13: UpdateTableAutoCommit();            break;
     case 14: DisplayTableInfo();                 break;
     case 15: RenameTable();                      break;
     case 16: DeleteTable();                      break;
     case 99: break;

     default:  std::cout << "Invalid Option" << std::endl;
   }
 }
}
/************************************************************************/
#ifdef XB_INF_SUPPORT
void xbUtil::InfFileMenu()
{


  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "No table selected" << std::endl;
    return;
  }

  int option = 0;
  while( option != 99 ) {
    std::cout << std::endl << std::endl << " 11 - InfFileData Menu"      << std::endl;
    std::cout << " ---------------------" << std::endl;
    DisplayActiveTable();
    std::cout << " 1 - List Inf Contents"                  << std::endl;
    std::cout << " 2 - Add NDX File association to ["      << dActiveTable->GetTblAlias().Str() << "]" << std::endl;
    std::cout << " 3 - Delete NDX File association from [" << dActiveTable->GetTblAlias().Str() << "]" << std::endl;
    std::cout << "10 - Inf File Help"                      << std::endl;
    std::cout << "99 - Exit Menu"                          << std::endl;
    option = GetOption();

    switch( option ){
      case 0:                       break;
      case 1:  ListInfFileData();   break;
      case 2:  AddInfFileData();    break;
      case 3:  DeleteInfFileData(); break;
      case 10: InfFileHelp();       break;
      case 99: break; 
      default: std::cout << "Invalid Option" << std::endl;
    }
  }
}
#endif // XB_INF_SUPPORT

/************************************************************************************/
void xbUtil::SystemMenu()
{
  int option = 0;
  while( option != 99 ) {
    std::cout << std::endl << std::endl << " 2 - System Menu"  << std::endl;
    std::cout << " ---------------"                            << std::endl;
    DisplayActiveTable();
    std::cout << " 1 - Display System Settings"                << std::endl;
    std::cout << " 2 - Update Default Data Directory"          << std::endl;

    std::cout << " 3 - Toggle Default Auto Commit"             << std::endl;
    std::cout << " 4 - Update Default Log Directory"           << std::endl;
    std::cout << " 5 - Update Default Log File Name"           << std::endl;
    std::cout << " 6 - Toggle Logging Status"                  << std::endl;
    std::cout << " 7 - Write Test Log Message"                 << std::endl;
    #ifdef XB_LOCKING_SUPPORT
    std::cout << " 8 - Update Default Lock Retries"            << std::endl;
    std::cout << " 9 - Toggle Default Auto Lock"               << std::endl;
    //std::cout << "10 - Update Lock Flavor"                   << std::endl;  3/20/17, only one flavor working
    std::cout << "11 - Update Default Lock Wait Time"          << std::endl;
    std::cout << "12 - Toggle Multi User Mode"                 << std::endl;
    #endif

    #if defined(XB_NDX_SUPPORT) || defined (XB_MDX_SUPPORT)
    std::cout << "13 - Toggle Index Dup Key Processing Option" << std::endl;
    #endif

    std::cout << "99 - Exit Menu"                              << std::endl;
    option = GetOption();

    switch( option ) {
     case 0:                                 break;
     case 1:  ListSystemSettings();          break;
     case 2:  UpdateDataDirectory();         break;
     case 3:  ToggleDefaultAutoCommit();     break;
     case 4:  UpdateLogDirectory();          break;
     case 5:  UpdateLogFileName();           break;
     case 6:  ToggleLoggingStatus();         break;
     case 7:  WriteLogMessage();             break;
     #ifdef XB_LOCKING_SUPPORT
     case 8:  UpdateDefaultLockRetries();    break;
     case 9:  ToggleDefaultAutoLock();       break;
     case 10: UpdateDefaultLockFlavor();     break;
     case 11: UpdateDefaultLockWait();       break;
     case 12: ToggleMultiUserMode();         break;
     #endif

     #if defined (XB_NDX_SUPPORT) || defined (XB_MDX_SUPPORT)
     case 13: ToggleIndexOpt();              break;
     #endif


     case 99: break;
     default:  std::cout << "Invalid Option" << std::endl;
    }
  }
}
/************************************************************************************/
void xbUtil::MainMenu()
{
  int option = 0;
  std::cout << std::endl<< std::endl << "XBase64 Utility Program   " <<
     xbase_VERSION_MAJOR << "." << xbase_VERSION_MINOR << "." <<
     xbase_VERSION_PATCH << "\n";

  DisplayActiveTable();
  while( option != 99 ) {
    std::cout << std::endl << std::endl << " 0 - Main Menu" << std::endl;
    std::cout << " -------------" << std::endl;
    std::cout << " 1 - Help"                << std::endl;
    std::cout << " 2 - System Menu"         << std::endl;
    std::cout << " 3 - File / Table Menu"   << std::endl;
    std::cout << " 4 - Record Menu"         << std::endl; 
    std::cout << " 5 - Field Menu"          << std::endl;
    //std::cout << " 6  - Index Menu"          << std::endl;
    #ifdef XB_LOCKING_SUPPORT
    std::cout << " 6 - Locking Menu"      << std::endl;
    #endif
    #ifdef XB_EXPRESSION_SUPPORT
    std::cout << " 7 - Expression Menu"   << std::endl;
    #endif
    #ifdef XB_INDEX_SUPPORT
    std::cout << " 8 - Index Menu"        << std::endl;
    #endif

    #ifdef XB_FILTERS
    std::cout << " 9 - Filter Menu"       << std::endl;
    #endif

    #ifdef XB_SQL_SUPPORT
    std::cout << "10 - SQL Menu"          << std::endl;
    #endif // XB_SQL_SUPPORT

    #ifdef XB_INF_SUPPORT
    std::cout << "11 - INF File Menu"     << std::endl;
    #endif // XB_INF_SUPPORT

    #ifdef XB_DEBUG_SUPPORT
    std::cout << "20 - Debug Menu"        << std::endl;
    #endif

    std::cout << "99 - Exit"                << std::endl;
    option = GetOption();
    switch( option ){
     case 0:                       break;
     case 1: Help();               break;
     case 2: SystemMenu();         break;
     case 3: FileMenu();           break;
     case 4: RecordMenu();         break;
     case 5: FieldMenu();          break;

     #ifdef XB_LOCKING_SUPPORT
     case 6:  LockingMenu();       break;
     #endif

     #ifdef XB_EXPRESSION_SUPPORT
     case 7:  ExpressionMenu();    break;
     #endif

     #ifdef XB_INDEX_SUPPORT
     case 8: IndexMenu();          break;
     #endif

     #ifdef XB_FILTERS
     case 9:  FilterMenu();        break;
     #endif

     #ifdef XB_SQL_SUPPORT
     case 10: SqlMenu();           break;
     #endif

     #ifdef XB_INF_SUPPORT
     case 11: InfFileMenu();       break;
     #endif

     #ifdef XB_DEBUG_SUPPORT
     case 20:  DebugMenu();        break;
     #endif

     case 99:  std::cout << "Bye!! - Thanks for using XBase64" << std::endl; break;
     default: 
        std::cout << option << " - Invalid function" << std::endl; 
        break;
    }
  }
}
/*************************************************************************************/

#ifdef XB_INDEX_SUPPORT
void xbUtil::DisplayOpenIndexFiles(){

  // verify active table selected, if not, select one
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "Table not selected" << std::endl;
    return;
  }
   // if not active indices, display no open NDX indices
   xbInt32 lIxCnt = dActiveTable->GetPhysicalIxCnt();
   if( lIxCnt == 0 ){
    std::cout << "No open index files for table"  << std::endl;
     return;
   } else {
     std::cout << "  Open Index Files = [" << lIxCnt << "]" << std::endl;
     std::cout << "  FileName\tFile Type\n";
     std::cout << "  ========\t=========\n";
   }
   // For each active index
   //  display File Name, Type, Key
   xbIxList *p = dActiveTable->GetIxList();
   xbIx *ixp;
   while( p ){
     ixp = p->ix;
     std::cout << "  " << ixp->GetFileName().Str() << "\t" << p->sFmt->Str() << std::endl;
     p = p->next;
   }
}

void xbUtil::DisplaySupportedIndexTypes(){

  std::cout << "Supported Index Type" << std::endl;
  std::cout << "Type  MaxTags  Asc/Dsc   Filters  Description" << std::endl;
  std::cout << "----  -------  --------  -------  --------------------------------" << std::endl;
  #ifdef XB_NDX_SUPPORT
  std::cout << "NDX      1     ASC        No      Dbase III single tag index file"  << std::endl; 
  #endif
  #ifdef XB_MDX_SUPPORT
  std::cout << "MDX     47     ASC/DSC    Yes     Dbase IV multiple tag index file" << std::endl; 
  #endif
}

void xbUtil::SelectTag(){
  std::cout << "SelectTag" << std::endl;
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "Table not selected" << std::endl;
    return;
  }

  xbLinkListNode<xbTag *> *llN = dActiveTable->GetTagList();
  xbTag *pTag;
  xbInt16 i = 1;
  std::cout << "Line\tType\tUnique\tSort\tName\t\tKey Expression\t\tFilter" << std::endl;

  char cUnique;
  char cSort;
  xbString s;
  while( llN ){
     pTag = llN->GetKey();
     pTag->GetUnique()  ? cUnique = 'Y' : cUnique = 'N';
     pTag->GetSort()    ? cSort = 'D'   : cSort = 'A';
     s.Sprintf( "%d\t%s\t%c\t%c\t%-12s\t%-20s\t%s  \n", i++, pTag->GetType().Str(), cUnique, cSort, pTag->GetTagName().Str(), pTag->GetExpression().Str(), pTag->GetFilter().Str() );
     std::cout << s.Str();
//     std::cout << i++ << "\t" << pTag->GetType() << "\t " << cUnique << "\t " << cSort << "\t" << pTag->GetTagName() << "\t" << pTag->GetExpression() << "\t" << pTag->GetFilter() << std::endl;
     llN = llN->GetNextNode();
  }
  char cBuf[128];
  std::cout << std::endl << "Enter Line No:" << std::endl;
  std::cin.getline( cBuf, 128 );

  xbInt32 iSelection = atol( cBuf );
  if( iSelection < 1 || iSelection > i ){
    std::cout << "Invalid selection [" << iSelection << "]" << std::endl;
  } else {
    llN = dActiveTable->GetTagList();
    i = 1;
    for( i = 1; i < iSelection; i++ )
      llN = llN->GetNextNode();
    pTag = llN->GetKey();
    dActiveTable->SetCurTag( pTag->GetType(), pTag->GetIx(), pTag->GetVpTag() );
  }
}

void xbUtil::OpenIndex(){
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "Table not selected" << std::endl;
    return;
  }
  xbInt16 iRc;

  char cBuf[128];
  std::cout << "Enter index type (NDX or MDX)" << std::endl;
  std::cin.getline( cBuf, 128 );
  xbString sIxType = cBuf;

  std::cout << "Enter File Name" << std::endl;
  std::cin.getline( cBuf, 128 );
  xbString sIxFileName = cBuf;

  iRc = dActiveTable->OpenIndex( sIxType, sIxFileName );
  x->DisplayError( iRc );
}

void xbUtil::CloseIndexFile(){

  std::cout << "CloseIndex\n";

  // verify active table selected, if not, select one
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "Table not selected" << std::endl;
    return;
  }

  xbIx *pIx = dActiveTable->GetCurIx();
  if( !pIx )
    SelectTag();

  pIx = dActiveTable->GetCurIx();
  if( !pIx ){
    std::cout << "Tag not selected" << std::endl;
    return;
  }

  xbInt16 iRc = dActiveTable->CloseIndexFile( pIx );
  x->DisplayError( iRc );
}

void xbUtil::CreateIndexTag(){

  std::cout << "CreateIndexTag\n";

  // verify active table selected, if not, select one
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "Table not selected" << std::endl;
    return;
  }

  char cBuf[512];
  xbString sPrompt = "Enter Index Type (";
  #ifdef XB_NDX_SUPPORT 
  sPrompt += "NDX";
  #endif // XB_NDX_SUPPORT
  #ifdef XB_MDX_SUPPORT
  sPrompt += " MDX";
  #endif // XB_MDX_SUPPORT
  sPrompt += "):";
  std::cout << sPrompt.Str() << std::endl;

  std::cin.getline( cBuf, 128 );
  xbString sIxType = cBuf;

  if( sIxType == "NDX" )
    std::cout << "Enter File Name" << std::endl;
  else if( sIxType == "MDX" )
    std::cout << "Enter Tag Name" << std::endl;

  std::cin.getline( cBuf, 128 );
  xbString sIxName = cBuf;

  if( sIxType != "NDX" && sIxType != "MDX" ){
    std::cout << "Invalid tag type" << std::endl;
    return;
  }

  xbString s;
  if( sIxType == "NDX" ){
    s = sIxName;
    s.ToUpperCase();
    xbUInt32 lPos = s.Pos( ".NDX" );
    if( lPos == 0 )
      sIxName += ".NDX";
  }
  if( sIxType == "MDX" ){
    if( sIxName.Len() > 12 ){
      std::cout << "Tag name [" << sIxName.Str() << "] to long.  Must be 12 bytes or less" << std::endl;
      return;
    }
  }

  xbInt16  iDescending = 0;
  xbInt16  iUnique = 0;

  std::cout << "Enter Key Expression:" << std::endl;
  std::cin.getline( cBuf, 512 );
  xbString sKeyExpression = cBuf;

  xbString sFilter;
  if( sIxType == "MDX" ){
    std::cout << "Enter Filter (or enter for none):" << std::endl;
    std::cin.getline( cBuf, 512 );
    sFilter = cBuf;

    std::cout << "Descending? (Enter Y for yes):" << std::endl;
    std::cin.getline( cBuf, 12 );
    if( cBuf[0] == 'Y' )
      iDescending = 1;
  }

  std::cout << "Unique Keys? (Enter Y for yes):" << std::endl;
  std::cin.getline( cBuf, 12 );
  if( cBuf[0] == 'Y' )
    iUnique = 1;

  xbIx *pIx;
  void *vpTag;

  xbInt16 iRc = dActiveTable->CreateTag( sIxType, sIxName, sKeyExpression, sFilter, iDescending, iUnique, 0, &pIx, &vpTag );

  if( iRc != XB_NO_ERROR ){
    x->DisplayError( iRc );
  } else {
    // iRc = pIx->Reindex( &vpTag );
    iRc = dActiveTable->Reindex( 2, &pIx, &vpTag );
    if( iRc != XB_NO_ERROR )
      x->DisplayError( iRc );
    else
      std::cout << "Index created" << std::endl;

    #ifdef XB_INF_SUPPORT
    iRc = dActiveTable->AssociateIndex( "NDX", sIxName, 0 );
    if( iRc != XB_NO_ERROR )
      x->DisplayError( iRc );
    #endif
  }
}

void xbUtil::GetFirstKey(){
  std::cout << "GetFirstKey\n";
  // verify active table selected, if not, select one
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );
  if( !dActiveTable ){
    std::cout << "Table not selected" << std::endl;
    return;
  }
  xbIx *pIx = dActiveTable->GetCurIx();
  if( !pIx )
    SelectTag();
  pIx = dActiveTable->GetCurIx();
  if( !pIx ){
    std::cout << "Tag not selected" << std::endl;
    return;
  }
  void *vpCurTag = dActiveTable->GetCurTag();
  xbInt16 iRc = pIx->GetFirstKey( vpCurTag, 1 );
  if( iRc == XB_NO_ERROR )
      dActiveTable->DumpRecord( dActiveTable->GetCurRecNo(), 2);
  else
      x->DisplayError( iRc );
}

void xbUtil::GetNextKey(){
  std::cout << "GetNextKey\n";
  // verify active table selected, if not, select one
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );
  if( !dActiveTable ){
    std::cout << "Table not selected" << std::endl;
    return;
  }
  xbIx *pIx = dActiveTable->GetCurIx();
  if( !pIx )
    SelectTag();
  pIx = dActiveTable->GetCurIx();
  if( !pIx ){
    std::cout << "Tag not selected" << std::endl;
    return;
  }
  void *vpCurTag = dActiveTable->GetCurTag();
  xbInt16 iRc = pIx->GetNextKey( vpCurTag, 1 );
  if( iRc == XB_NO_ERROR )
    dActiveTable->DumpRecord( dActiveTable->GetCurRecNo(), 2);
  else
    x->DisplayError( iRc );
}

void xbUtil::GetPrevKey(){
  std::cout << "GetPrevKey\n";
  // verify active table selected, if not, select one
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "Table not selected" << std::endl;
    return;
  }
  xbIx *pIx = dActiveTable->GetCurIx();
  if( !pIx )
    SelectTag();
  pIx = dActiveTable->GetCurIx();
  if( !pIx ){
    std::cout << "Tag not selected" << std::endl;
    return;
  }
  void *vpCurTag = dActiveTable->GetCurTag();
  xbInt16 iRc = pIx->GetPrevKey( vpCurTag, 1 );
  if( iRc == XB_NO_ERROR )
    dActiveTable->DumpRecord( dActiveTable->GetCurRecNo(), 2);
  else
    x->DisplayError( iRc );
}

void xbUtil::GetLastKey(){
  std::cout << "GetLastKey\n";
  // verify active table selected, if not, select one
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );
  if( !dActiveTable ){
    std::cout << "Table selected" << std::endl;
    return;
  }
  xbIx *pIx = dActiveTable->GetCurIx();
  if( !pIx )
    SelectTag();
  pIx = dActiveTable->GetCurIx();
  if( !pIx ){
    std::cout << "Tag not selected" << std::endl;
    return;
  }
  void *vpCurTag = dActiveTable->GetCurTag();
  xbInt16 iRc = pIx->GetLastKey( vpCurTag, 1 );
  if( iRc == XB_NO_ERROR )
    dActiveTable->DumpRecord( dActiveTable->GetCurRecNo(), 2);
  else
    x->DisplayError( iRc );
}

void xbUtil::FindKey(){
  std::cout << "FindKey\n";
  // verify active table selected, if not, select one
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );
  if( !dActiveTable ){
    std::cout << "Table not selected" << std::endl;
    return;
  }
  xbIx *pIx = dActiveTable->GetCurIx();
  if( !pIx )
    SelectTag();
  pIx = dActiveTable->GetCurIx();
  if( !pIx ){
    std::cout << "Tag not selected" << std::endl;
    return;
  }
  void *vpCurTag = dActiveTable->GetCurTag();
  char cKeyType = pIx->GetKeyType( vpCurTag );

  switch( cKeyType ){
    case 'C':
      std::cout << "Enter character key value:";
      break;
    case 'F':
    case 'N':
      std::cout << "Enter numeric key value:";
      break;
    case 'D':
      std::cout << "Enter date (YYYYMMDD) key value:";
      break;
    default:
      std::cout << "Unknown key type [" << cKeyType << "]" << std::endl;
      return;
      // break;
  }

  char cBuf[128];
  std::cin.getline( cBuf, 128 );
  xbInt16 iRc = 0;

  if( cKeyType == 'C' ){
    iRc = pIx->FindKey( vpCurTag, cBuf, (xbInt32) strlen( cBuf ), 1 );

  } else if( cKeyType == 'F' || cKeyType == 'N' ){
    xbDouble d = atof( cBuf );
    iRc = pIx->FindKey( vpCurTag, d, 1 );

  } else if( cKeyType == 'D' ){
    xbDate dt( cBuf );
    iRc = pIx->FindKey( vpCurTag, dt, 1 );
  }

  if( iRc == XB_NO_ERROR )
      dActiveTable->DumpRecord( dActiveTable->GetCurRecNo(), 2);
  else
      x->DisplayError( iRc );
}

void   xbUtil::CheckIntegrity(){
  std::cout << "CheckIntegrity\n";
  // verify active table selected, if not, select one
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );
  if( !dActiveTable ){
    std::cout << "Table not selected" << std::endl;
    return;
  }
  xbIx *pIx = dActiveTable->GetCurIx();
  if( !pIx )
    SelectTag();
  pIx = dActiveTable->GetCurIx();
  if( !pIx ){
    std::cout << "Tag not selected" << std::endl;
    return;
  }
  void *vpCurTag = dActiveTable->GetCurTag();
  
  xbInt16 iRc = pIx->CheckTagIntegrity( vpCurTag, 2 );
  x->DisplayError( iRc );
}

void   xbUtil::Reindex(){
  std::cout << "Reindex\n";

  // verify active table selected, if not, select one
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );
  if( !dActiveTable ){
    std::cout << "Table not selected" << std::endl;
    return;
  }
  xbIx *pIx = dActiveTable->GetCurIx();
  if( !pIx )
    SelectTag();
  pIx = dActiveTable->GetCurIx();
 
  xbInt16 iOption = 0;
  if( !pIx ){
    iOption = 2;
    //    std::cout << "Tag not selected" << std::endl;
    // return;
  } else {
    while( iOption != 1 && iOption != 2 && iOption != 9 ){
      std::cout << "1 - Reindex current tag" << std::endl;
      std::cout << "2 - Reindex all tags"    << std::endl;
      std::cout << "9 - Exit"                << std::endl;
      iOption = GetOption();
    }
  }

  xbInt16 iRc = 0;
  if( iOption == 9 )
    return;
  else if ( iOption == 1 ){
    std::cout << "Rebuilding current index tag." << std::endl;
    iRc = dActiveTable->Reindex( 0 );
  } else if ( iOption == 2 ){
    std::cout << "Rebuilding all index tags." << std::endl;
    iRc = dActiveTable->Reindex( 1 );
  }
  x->DisplayError( iRc );
}

void   xbUtil::DeleteTag(){
  std::cout << "DeleteTag\n";
  // verify active table selected, if not, select one
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );
  if( !dActiveTable ){
    std::cout << "Table not selected" << std::endl;
    return;
  }
  xbIx *pIx = dActiveTable->GetCurIx();
  if( !pIx )
    SelectTag();
  pIx = dActiveTable->GetCurIx();
  if( !pIx ){
    std::cout << "Tag not selected" << std::endl;
    return;
  }
  void *vpCurTag = dActiveTable->GetCurTag();
  xbInt16 iRc = dActiveTable->DeleteTag( dActiveTable->GetCurIxType(), pIx->GetTagName( vpCurTag ));
  x->DisplayError( iRc );
}


#ifdef XB_DEBUG_SUPPORT
void xbUtil::DumpRecsByIx( xbInt16 iOpt ){
 //  iDirection =  0 - Forward - MDX
 //             =  1 - Reverse - MDX

  std::cout << "DumpRecsByIx\n";
  // verify active table selected, if not, select one
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );
  if( !dActiveTable ){
    std::cout << "Table not selected" << std::endl;
    return;
  }
  xbIx *pIx = dActiveTable->GetCurIx();
  if( !pIx )
    SelectTag();
  pIx = dActiveTable->GetCurIx();
  if( !pIx ){
    std::cout << "Tag not selected" << std::endl;
    return;
  }
  std::cout << "Select destination:" << std::endl;
  std::cout << "0 - Logfile" << std::endl;
  std::cout << "1 - Screen"  << std::endl;
  std::cout << "2 - Both"    << std::endl;
  xbInt16 iDispOpt = GetOption();
  std::cout << "Select format:"         << std::endl;
  std::cout << "0 - With Field Names"   << std::endl;
  std::cout << "1 - 1 line per record"  << std::endl;
  xbInt16 iDispFmt = GetOption();

  x->WriteLogMessage( "--------- Dump Recs By Index -------------", iDispOpt );
  void *vpCurTag = dActiveTable->GetCurTag();
  xbUInt32 lRecCnt = 0;
  xbInt16 iRc = 0;

  if( iOpt == 0 ){
    iRc = pIx->GetFirstKey( vpCurTag, 1 );
    if( iRc == XB_NO_ERROR ){
      lRecCnt++;
      dActiveTable->DumpRecord( dActiveTable->GetCurRecNo(), iDispOpt, iDispFmt );
      while(( iRc =  pIx->GetNextKey( vpCurTag, 1 )) == XB_NO_ERROR ){
        dActiveTable->DumpRecord( dActiveTable->GetCurRecNo(), iDispOpt, iDispFmt );
        lRecCnt++;
      }
    } else {
      x->DisplayError( iRc );
    }
  } else if( iOpt == 1 ) {
    iRc = pIx->GetLastKey( vpCurTag, 1 );
    if( iRc == XB_NO_ERROR ){
      lRecCnt++;
      dActiveTable->DumpRecord( dActiveTable->GetCurRecNo(), iDispOpt, iDispFmt );
      while(( iRc =  pIx->GetPrevKey( vpCurTag, 1 )) == XB_NO_ERROR ){
        dActiveTable->DumpRecord( dActiveTable->GetCurRecNo(), iDispOpt, iDispFmt );
        lRecCnt++;
      }
    }
  } else {
    std::cout << "Invalid option " << iOpt << std::endl;
    return;
  }
  std::cout << lRecCnt << " record(s) dumped" << std::endl;
}

void   xbUtil::DumpIxHeader(){
  std::cout << "DumpIxHeader\n";
  // verify active table selected, if not, select one
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );
  if( !dActiveTable ){
    std::cout << "Table not selected" << std::endl;
    return;
  }
  xbIx *pIx = dActiveTable->GetCurIx();
  if( !pIx )
    SelectTag();
  pIx = dActiveTable->GetCurIx();
  if( !pIx ){
    std::cout << "Tag not selected" << std::endl;
    return;
  }
  std::cout << "0 - Logfile" << std::endl;
  std::cout << "1 - Screen"  << std::endl;
  std::cout << "2 - Both"    << std::endl;
  xbInt16 iDispOpt = GetOption();
  pIx->DumpHeader( iDispOpt, 3 );

}

void   xbUtil::DumpIxNode(){
  std::cout << "DumpIxNode\n";
  // verify active table selected, if not, select one
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "Table not selected" << std::endl;
    return;
  }

  xbIx *pIx = dActiveTable->GetCurIx();
  if( !pIx )
    SelectTag();

  pIx = dActiveTable->GetCurIx();
  if( !pIx ){
    std::cout << "Tag not selected" << std::endl;
    return;
  }
  std::cout << "0 - Logfile" << std::endl;
  std::cout << "1 - Screen"  << std::endl;
  std::cout << "2 - Both"    << std::endl;
  xbInt16 iDispOpt = GetOption();
//  std::cout << std::endl << "0 - All Blocks"  << std::endl;
//  std::cout << "NNN - Specific block number, 0=All"  << std::endl;
//  xbUInt32 iBlockOpt = (xbUInt32) GetOption();
//  if( iBlockOpt == 0 ) // dump the header if dumping all blocks
 //   pIx->DumpHeader( iDispOpt );
  pIx->DumpTagBlocks( iDispOpt, dActiveTable->GetCurTag() );
}

void   xbUtil::DumpIxNodeChain(){

  std::cout << "DumpIxNodeChain\n";

  // verify active table selected, if not, select one
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "Table not selected" << std::endl;
    return;
  }

  xbIx *pIx = dActiveTable->GetCurIx();
  if( !pIx )
    SelectTag();

  pIx = dActiveTable->GetCurIx();
  if( !pIx ){
    std::cout << "Tag not selected" << std::endl;
    return;
  }

  std::cout << "0 - Logfile" << std::endl;
  std::cout << "1 - Screen"  << std::endl;
  std::cout << "2 - Both"    << std::endl;
  xbInt16 iDispOpt = GetOption();

  void *vpCurTag = dActiveTable->GetCurTag();
  pIx->DumpIxNodeChain( vpCurTag, iDispOpt );
}


void   xbUtil::DumpFreeBlocks(){
  std::cout << "Dump ulBlock2 - (Free Block or Split From Page) for MDX Index file\n";
  // verify active table selected, if not, select one
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );
  if( !dActiveTable ){
    std::cout << "Table not selected" << std::endl;
    return;
  }
  xbIx *pIx = dActiveTable->GetCurIx();
  if( !pIx )
    SelectTag();
  pIx = dActiveTable->GetCurIx();
  if( !pIx ){
    std::cout << "Tag not selected" << std::endl;
    return;
  }
  std::cout << "0 - Logfile" << std::endl;
  std::cout << "1 - Screen"  << std::endl;
  std::cout << "2 - Both"    << std::endl;
  xbInt16 iDispOpt = GetOption();
  pIx->DumpFreeBlocks( iDispOpt );

}



#endif // XB_DEBUG_SUPPORT
#endif // XB_INDEX_SUPPORT

#ifdef XB_FILTER_SUPPORT
void xbUtil::SetFilter()
{
  // verify active table selected, if not, select one
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "Table not selected" << std::endl;
    return;
  }

  char cBuf[128];
  std::cout << "Enter filter expression:" << std::endl;
  std::cin.getline( cBuf, 128 );
  xbString s( cBuf );

  if( filt )
    delete filt;

  filt = new xbFilter( dActiveTable );
  xbInt16 iRc = filt->Set( s );

  x->DisplayError( iRc );
}

void xbUtil::GetFirstFilterRec()
{
  // verify active table selected, if not, select one
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "Table not selected" << std::endl;
    return;
  }

  xbInt16 iRc;
  if(( iRc = filt->GetFirstRecord()) == XB_NO_ERROR )
    dActiveTable->DumpRecord( dActiveTable->GetCurRecNo(), 2);
  else
    x->DisplayError( iRc );
}

void xbUtil::GetNextFilterRec()
{
  // verify active table selected, if not, select one
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "Table not selected" << std::endl;
    return;
  }
  xbInt16 iRc;
  if(( iRc = filt->GetNextRecord()) == XB_NO_ERROR )
    dActiveTable->DumpRecord( dActiveTable->GetCurRecNo(), 2);
  else
    x->DisplayError( iRc );
}

void xbUtil::GetPrevFilterRec()
{
  // verify active table selected, if not, select one
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "Table not selected" << std::endl;
    return;
  }
  xbInt16 iRc;
  if(( iRc = filt->GetPrevRecord()) == XB_NO_ERROR )
    dActiveTable->DumpRecord( dActiveTable->GetCurRecNo(), 2);
  else
    x->DisplayError( iRc );
}

void xbUtil::GetLastFilterRec()
{
  // verify active table selected, if not, select one
  if( !dActiveTable )
    dActiveTable = GetTablePtr( " - select table" );

  if( !dActiveTable ){
    std::cout << "Table not selected" << std::endl;
    return;
  }
  xbInt16 iRc;
  if(( iRc = filt->GetLastRecord()) == XB_NO_ERROR )
    dActiveTable->DumpRecord( dActiveTable->GetCurRecNo(), 2);
  else
    x->DisplayError( iRc );
}
#endif  //  XB_FILTER_SUPPORT

///@endcond DOXYOFF


/*************************************************************************************/
void PrintHelp();
void PrintHelp(){
  std::cout << "Usage: xb_dbfutil [-h] [-?] [--help] [-v] [--version]"  << std::endl << std::endl;
  std::cout << "This program provides a menu driven tool for accessing the functionality of the xbase64 library" << std::endl;
  std::cout << "and provides access to DBF, MDX, NDX and MDT xbase formatted data files." << std::endl << std::endl;
}
void PrintVersion();
void PrintVersion(){
  std::cout << "Xbase64 Version: " << xbase_VERSION_MAJOR << "." << xbase_VERSION_MINOR << "." << xbase_VERSION_PATCH << std::endl;
}
/*************************************************************************************/
int main(int argc, char *argv[] )
{

  // std::cout << "xb_dbfutil initializing" << std::endl;
  xbXBase x;

  xbString sParm;
  if (x.GetCmdLineOpt( argc, argv, "-h", sParm ) ||
      x.GetCmdLineOpt( argc, argv, "-?", sParm ) ||
      x.GetCmdLineOpt( argc, argv, "--help", sParm )){
    PrintHelp();
    return 1;
  }

  if ( x.GetCmdLineOpt( argc, argv, "-v", sParm ) ||
       x.GetCmdLineOpt( argc, argv, "--version", sParm )){
    PrintVersion();
    return 1;
  }

  xbUtil u( &x );
  u.MainMenu();
  return 0;
}
