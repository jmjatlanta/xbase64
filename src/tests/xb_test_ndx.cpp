/* xb_test_ndx.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023,2024 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

// This program tests the class xbIxNdx

// usage:   xb_test_ndx QUITE|NORMAL|VERBOSE

// fix me - this program needs to test GetUnique



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

  xbSchema MyV3Record[] = 
  {
    { "CFLD",       XB_CHAR_FLD,     30, 0 },
    { "DFLD",       XB_DATE_FLD,      8, 0 },
    { "NFLD",       XB_NUMERIC_FLD,  12, 0 },
    { "",0,0,0 }
  };

  xbXBase x;
  xbString sMsg;

  #ifdef XB_LOGGING_SUPPORT
  x.SetLogDirectory( PROJECT_LOG_DIR );
  x.EnableMsgLogging();
  if( iPo ){
    std::cout << "Logfile is [" << x.GetLogFqFileName().Str() << "]" << std::endl;
  }
  sMsg.Sprintf( "Program [%s] initializing...", av[0] );
  x.WriteLogMessage( sMsg );
  x.SetLogSize( 1000000 );
  #endif

  x.SetDataDirectory( PROJECT_DATA_DIR );
  x.SetMultiUser( xbFalse );

  InitTime();

  char c;
  xbString s;
  xbInt32 lRecCnt = 0;
  iRc = 0;


  if( iPo > 0 )
    std::cout << "Default Data Directory is [" << x.GetDataDirectory().Str() << "]" << std::endl;

  x.SetUniqueKeyOpt( XB_HALT_ON_DUPKEY );
  iRc += TestMethod( iPo, "SetUniqueKeyOpt(100)", x.GetUniqueKeyOpt(), XB_HALT_ON_DUPKEY );


  xbDbf *V3Dbf = new xbDbf3( &x );

  xbIx *ixPtr;
  void *ndx;

  iRc2 = V3Dbf->CreateTable( "TestNdx.DBF", "TestNdx", MyV3Record, XB_OVERLAY, XB_MULTI_USER );
  iRc += TestMethod( iPo, "CreateTable(101)", (xbInt32) iRc2, XB_NO_ERROR );

  if( iRc2 )
    x.DisplayError( iRc2 );


  iRc2 = V3Dbf->CreateTag( "NDX", "TestNdxC.NDX", "CFLD", "", 0, 0, XB_OVERLAY, &ixPtr, &ndx );
  iRc += TestMethod( iPo, "CreateTag(102)", (xbInt32) iRc2, XB_NO_ERROR );
  if( iRc2 )
    x.DisplayError( iRc2 );

  iRc2 = V3Dbf->AssociateIndex( "NDX", "TestNdxC.NDX", 0 );
  iRc += TestMethod( iPo, "Associate(103)", (xbInt32) iRc2, XB_NO_ERROR );

  iRc2 = V3Dbf->CreateTag( "NDX", "TestNdxD.NDX", "DFLD", "", 0, 1, XB_OVERLAY, &ixPtr, &ndx );
  iRc += TestMethod( iPo, "CreateTag(104)", (xbInt32) iRc2, XB_NO_ERROR );
  if( iRc2 )
    x.DisplayError( iRc2 );

  iRc2 = V3Dbf->AssociateIndex( "NDX", "TestNdxD.NDX", 0 );
  iRc += TestMethod( iPo, "Associate(105)", (xbInt32) iRc2, XB_NO_ERROR );

  iRc2 = V3Dbf->CreateTag( "NDX", "TestNdxN.NDX", "NFLD", "", 0, 0, XB_OVERLAY, &ixPtr, &ndx );
  iRc += TestMethod( iPo, "CreateTag(106)", (xbInt32) iRc2, XB_NO_ERROR );
  if( iRc2 )
    x.DisplayError( iRc2 );

  iRc2 = V3Dbf->AssociateIndex( "NDX", "TestNdxN.NDX", 0 );
  iRc += TestMethod( iPo, "Associate(107)", (xbInt32) iRc2, XB_NO_ERROR );

  xbInt16 iTagCnt = ixPtr->GetTagCount();
  iRc += TestMethod( iPo, "GetTagCount(108)", (xbInt32) iTagCnt, 1 );

  xbString sTagName;
  sTagName = ixPtr->GetTagName( &ndx );
  iRc += TestMethod( iPo, "GetTagName(110)", sTagName.Str(), "TestNdxN", 8 );


  #ifdef XB_LOCKING_SUPPORT
  iRc += TestMethod( iPo, "LockTable(111)", V3Dbf->LockTable( XB_LOCK ), XB_NO_ERROR );
  #endif

  iRc += TestMethod( iPo, "GetCurTagName(112)", V3Dbf->GetCurTagName().Str(), "TestNdxC", 8 );
  iRc += TestMethod( iPo, "SetCurTag(113)", V3Dbf->SetCurTag( "TestNdxN" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "GetCurTagName(114)", V3Dbf->GetCurTagName().Str(), "TestNdxN", 8 );
  xbDouble dbl = 100;
  iRc += TestMethod( iPo, "Find(115)", V3Dbf->Find( dbl ), XB_NOT_FOUND );

  iRc += TestMethod( iPo, "SetCurTag(116)", V3Dbf->SetCurTag( "TestNdxD" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "GetCurTagName(117)", V3Dbf->GetCurTagName().Str(), "TestNdxD", 8 );
  xbDate dt;
  iRc += TestMethod( iPo, "Find( dt )", V3Dbf->Find( dt ), XB_NOT_FOUND );

  iRc += TestMethod( iPo, "GetFirstKey(118)",    ixPtr->GetFirstKey(), XB_EMPTY );
  iRc += TestMethod( iPo, "GetNextKey(119)",     ixPtr->GetFirstKey(), XB_EMPTY );
  iRc += TestMethod( iPo, "GetLasttKey(120)",    ixPtr->GetLastKey(),  XB_EMPTY );
  iRc += TestMethod( iPo, "GetPrevKey(121)",     ixPtr->GetPrevKey(),  XB_EMPTY );
  iRc += TestMethod( iPo, "FindKey(122)",        ixPtr->FindKey( NULL, "19611101", 8, 0 ), XB_NOT_FOUND );

  iRc += TestMethod( iPo, "BlankRecord(123)",    V3Dbf->BlankRecord(),                     XB_NO_ERROR );
  iRc += TestMethod( iPo, "Putfield(124)",       V3Dbf->PutField( "CFLD", "AAA" ),         XB_NO_ERROR );
  iRc += TestMethod( iPo, "Putfield(125)",       V3Dbf->PutField( "DFLD", "19611109" ),    XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutfieldDouble(126)", V3Dbf->PutDoubleField( "NFLD", 50 ), XB_NO_ERROR );

  iRc += TestMethod( iPo, "AppendRecord(127)",   V3Dbf->AppendRecord(),                    XB_NO_ERROR );

  xbDate d( "19890209" );

  //for( xbUInt16 i = 0; i < 35 && iRc == XB_NO_ERROR; i++ ){
  for( xbUInt16 i = 0; i < 2 && iRc == XB_NO_ERROR; i++ ){
    for( xbUInt16 j = 0; j < 35 && iRc == XB_NO_ERROR; j++ ){ 
      c = j + 65;
      s = c;
      s.PadRight( c, (xbUInt32) i + 1 );
      V3Dbf->BlankRecord();
      V3Dbf->PutField( "CFLD", s );
      V3Dbf->PutLongField( "NFLD", ++lRecCnt );
      V3Dbf->PutField( "DFLD", d.Str() );
      d++;

      iRc = V3Dbf->AppendRecord();
    }
  }

  iRc += TestMethod( iPo, "SetCurTag(200)", V3Dbf->SetCurTag( "TestNdxN" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "GetCurTagName(201)", V3Dbf->GetCurTagName().Str(), "TestNdxN", 8 );

  dbl = 55.0;
  iRc += TestMethod( iPo, "Find( 55.0 )", V3Dbf->Find( dbl ), XB_NO_ERROR );

  iRc += TestMethod( iPo, "SetCurTag(205)", V3Dbf->SetCurTag( "TestNdxC" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "GetCurTagName(206)", V3Dbf->GetCurTagName().Str(), "TestNdxC", 8 );

  s = "AAA";
  iRc += TestMethod( iPo, "Find( 'AAA' )", V3Dbf->Find( s ), XB_NO_ERROR );

  s = "AzAA";
  iRc += TestMethod( iPo, "Find( 'AzAA' )", V3Dbf->Find( s ), XB_NOT_FOUND );

  iRc += TestMethod( iPo, "SetCurTag(210)", V3Dbf->SetCurTag( "TestNdxD" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "GetCurTagName(211)", V3Dbf->GetCurTagName().Str(), "TestNdxD", 8 );

  iRc += TestMethod( iPo, "Find( '19890420' )", V3Dbf->Find(  d ), XB_NOT_FOUND );

  V3Dbf->GetRecord( 26 );
  V3Dbf->PutField( "CFLD", "AAA" );
  V3Dbf->PutRecord( 26 );

  V3Dbf->GetRecord( 14 );
  V3Dbf->PutField( "CFLD", "AAAA" );
  V3Dbf->PutRecord( 14 );

  V3Dbf->GetRecord( 11 );
  V3Dbf->PutField( "CFLD", "III" );
  V3Dbf->PutRecord( 11 );

  V3Dbf->GetRecord( 25 );
  V3Dbf->PutField( "CFLD", "DDD" );
  V3Dbf->PutRecord( 25 );

  V3Dbf->GetRecord( 12 );
  V3Dbf->PutField( "CFLD", "EEE" );
  V3Dbf->PutRecord( 12 );

  V3Dbf->GetRecord( 26 );
  V3Dbf->PutField( "CFLD", "CCC" );
  V3Dbf->PutRecord( 26 );

  V3Dbf->GetRecord( 13 );
  V3Dbf->PutField( "CFLD", "CCCC" );
  V3Dbf->PutRecord( 13 );

  V3Dbf->GetRecord( 27 );
  V3Dbf->PutField( "CFLD", "AAA" );
  V3Dbf->PutRecord( 27 );

  V3Dbf->GetRecord( 28 );
  V3Dbf->PutField( "CFLD", "CCC" );
  V3Dbf->PutRecord( 28 );

  V3Dbf->GetRecord( 24 );
  V3Dbf->PutField( "CFLD", "FFF" );
  V3Dbf->PutRecord( 24 );

  V3Dbf->GetRecord( 10 );
  V3Dbf->PutField( "CFLD", "HHH" );
  V3Dbf->PutRecord( 10 );


  for( xbUInt16 i = 0; i < 35 && iRc == XB_NO_ERROR; i++ ){
    for( xbUInt16 j = 0; j < 14 && iRc == XB_NO_ERROR; j++ ){ 
      c = j + 65;
      s = c;
      s.PadRight( c, (xbUInt32) i + 1 );
      V3Dbf->BlankRecord();
      V3Dbf->PutField( "CFLD", s );
      V3Dbf->PutLongField( "NFLD", ++lRecCnt );
      V3Dbf->PutField( "DFLD", d.Str() );
      d++;
      iRc = V3Dbf->AppendRecord();
    }
  }

  for( xbUInt16 i = 0; i < 2 && iRc == XB_NO_ERROR; i++ ){
    for( xbUInt16 j = 0; j < 14 && iRc == XB_NO_ERROR; j++ ){ 
      c = j + 65;
      s = c;
      s.PadRight( c, (xbUInt32) i + 1 );

      V3Dbf->BlankRecord();
      V3Dbf->PutField( "CFLD", s );
      V3Dbf->PutLongField( "NFLD", ++lRecCnt );
      V3Dbf->PutField( "DFLD", d.Str() );
      d++;

      iRc = V3Dbf->AppendRecord();
    }
  }

  V3Dbf->GetRecord( 26 );
  V3Dbf->PutField( "CFLD", "AAA" );
  V3Dbf->PutRecord( 26 );

  V3Dbf->GetRecord( 14 );
  V3Dbf->PutField( "CFLD", "AAAA" );
  V3Dbf->PutRecord( 14 );

  V3Dbf->GetRecord( 11 );
  V3Dbf->PutField( "CFLD", "III" );
  V3Dbf->PutRecord( 11 );

  V3Dbf->GetRecord( 25 );
  V3Dbf->PutField( "CFLD", "DDD" );
  V3Dbf->PutRecord( 25 );

  V3Dbf->GetRecord( 12 );
  V3Dbf->PutField( "CFLD", "EEE" );
  V3Dbf->PutRecord( 12 );

  V3Dbf->GetRecord( 26 );
  V3Dbf->PutField( "CFLD", "CCC" );
  V3Dbf->PutRecord( 26 );

  V3Dbf->GetRecord( 13 );
  V3Dbf->PutField( "CFLD", "CCCC" );
  V3Dbf->PutRecord( 13 );

  V3Dbf->GetRecord( 27 );
  V3Dbf->PutField( "CFLD", "AAA" );
  V3Dbf->PutRecord( 27 );

  V3Dbf->GetRecord( 28 );
  V3Dbf->PutField( "CFLD", "CCC" );
  V3Dbf->PutRecord( 28 );

  V3Dbf->GetRecord( 24 );
  V3Dbf->PutField( "CFLD", "FFF" );
  V3Dbf->PutRecord( 24 );

  V3Dbf->GetRecord( 10 );
  V3Dbf->PutField( "CFLD", "HHH" );
  V3Dbf->PutRecord( 10 );

  // just in case there are any issues outstanding
  V3Dbf->Abort();

  xbIxList *ixl = V3Dbf->GetIxList();
  xbIxNdx *ix;
  while( ixl ){
    if( *ixl->sFmt == "NDX" ){
      ix = (xbIxNdx *) ixl->ix;
      //ix->GetTagName( 0, sTagName );
      sMsg.Sprintf( "CheckTagIntegrity(220) - [%s]", ix->GetTagName(ix->GetCurTag()).Str());
      iRc += TestMethod( iPo, sMsg, ix->CheckTagIntegrity( ix->GetCurTag(), 0 ), XB_NO_ERROR );
      ixl = ixl->next;
    }
  }

  #ifdef XB_LOCKING_SUPPORT
  iRc += TestMethod( iPo, "LockTable()", V3Dbf->LockTable( XB_UNLOCK ), XB_NO_ERROR );
  #endif

  xbString sDir;
  V3Dbf->GetFileDirPart( sDir );
  xbString sDbfName;
  xbString sInfName;
  sDbfName.Sprintf( "%sTestNdxR.DBF", sDir.Str());
  sInfName.Sprintf( "%sTestNdxR.INF", sDir.Str());

  V3Dbf->xbRemove( sDbfName );
  V3Dbf->xbRemove( sInfName );

  iRc += TestMethod( iPo, "Rename(300)",  V3Dbf->Rename( "TestNdxR.DBF" ), XB_NO_ERROR );

  iRc += TestMethod( iPo, "DeleteTable()", V3Dbf->DeleteTable(), XB_NO_ERROR );
  //  iRc += TestMethod( iPo, "Close()", V3Dbf->Close(), XB_NO_ERROR );  // did a delete instead of close

  delete V3Dbf;

  x.SetUniqueKeyOpt( XB_EMULATE_DBASE );
  iRc += TestMethod( iPo, "SetUniqueKeyOpt(301)", x.GetUniqueKeyOpt(), XB_EMULATE_DBASE );


  V3Dbf = new xbDbf3( &x );

  iRc2 = V3Dbf->CreateTable( "TestNdx.DBF", "TestNdx", MyV3Record, XB_OVERLAY, XB_MULTI_USER );
  iRc += TestMethod( iPo, "CreateTable(302)", (xbInt32) iRc2, XB_NO_ERROR );

  if( iRc2 )
    x.DisplayError( iRc2 );

  iRc2 = V3Dbf->CreateTag( "NDX", "TestNdxC.NDX", "CFLD", "", 0, xbTrue, XB_OVERLAY, &ixPtr, &ndx );
  iRc += TestMethod( iPo, "CreateTag(303)", (xbInt32) iRc2, XB_NO_ERROR );
  if( iRc2 )
    x.DisplayError( iRc2 );

  iRc2 = V3Dbf->AssociateIndex( "NDX", "TestNdxC.NDX", 0 );
  iRc += TestMethod( iPo, "Associate(304)", (xbInt32) iRc2, XB_NO_ERROR );


  iRc += TestMethod( iPo, "BlankRecord(310)",    V3Dbf->BlankRecord(),                     XB_NO_ERROR );
  iRc += TestMethod( iPo, "Putfield(311)",       V3Dbf->PutField( "CFLD", "AAA" ),         XB_NO_ERROR );
  iRc += TestMethod( iPo, "Putfield(312)",       V3Dbf->PutField( "DFLD", "19611109" ),    XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutfieldDouble(313)", V3Dbf->PutDoubleField( "NFLD", 50 ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "AppendRecord(314)",   V3Dbf->AppendRecord(),                    XB_NO_ERROR );

  iRc += TestMethod( iPo, "BlankRecord(320)",    V3Dbf->BlankRecord(),                     XB_NO_ERROR );
  iRc += TestMethod( iPo, "Putfield(321)",       V3Dbf->PutField( "CFLD", "BBB" ),         XB_NO_ERROR );
  iRc += TestMethod( iPo, "Putfield(322)",       V3Dbf->PutField( "DFLD", "19611109" ),    XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutfieldDouble(323)", V3Dbf->PutDoubleField( "NFLD", 50 ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "AppendRecord(324)",   V3Dbf->AppendRecord(),                    XB_NO_ERROR );

  iRc += TestMethod( iPo, "BlankRecord(330)",    V3Dbf->BlankRecord(),                     XB_NO_ERROR );
  iRc += TestMethod( iPo, "Putfield(331)",       V3Dbf->PutField( "CFLD", "BBB" ),         XB_NO_ERROR );
  iRc += TestMethod( iPo, "Putfield(332)",       V3Dbf->PutField( "DFLD", "19611109" ),    XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutfieldDouble(333)", V3Dbf->PutDoubleField( "NFLD", 50 ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "AppendRecord(334)",   V3Dbf->AppendRecord(),                    XB_NO_ERROR );


  ixl = V3Dbf->GetIxList();
  while( ixl ){
    if( *ixl->sFmt == "NDX" ){
      ix = (xbIxNdx *) ixl->ix;
      //ix->GetTagName( 0, sTagName );
      sMsg.Sprintf( "CheckTagIntegrity(340) - [%s]", ix->GetTagName(ix->GetCurTag()).Str());
      iRc += TestMethod( iPo, sMsg, ix->CheckTagIntegrity( ix->GetCurTag(), 0 ), 1 );
    }
    ixl = ixl->next;
  }

  x.SetUniqueKeyOpt( XB_HALT_ON_DUPKEY );
  iRc += TestMethod( iPo, "SetUniqueKeyOpt(341)", x.GetUniqueKeyOpt(), XB_HALT_ON_DUPKEY );

  ixl = V3Dbf->GetIxList();
  while( ixl ){
    if( *ixl->sFmt == "NDX" ){
      ix = (xbIxNdx *) ixl->ix;
      //ix->GetTagName( 0, sTagName );
      sMsg.Sprintf( "CheckTagIntegrity(342) - [%s]", ix->GetTagName(ix->GetCurTag()).Str());
      iRc += TestMethod( iPo, sMsg, ix->CheckTagIntegrity( ix->GetCurTag(), 0 ), XB_INVALID_INDEX );
    }
    ixl = ixl->next;
  }

  x.SetUniqueKeyOpt( XB_EMULATE_DBASE );
  iRc += TestMethod( iPo, "SetUniqueKeyOpt(343)", x.GetUniqueKeyOpt(), XB_EMULATE_DBASE );

  iRc2 = V3Dbf->Reindex( 1 );  // reindex all tags
  iRc += TestMethod( iPo, "Reindex(350)", (xbInt32) iRc2, XB_NO_ERROR );



  iRc += TestMethod( iPo, "DeleteTable(352)", V3Dbf->DeleteTable(), XB_NO_ERROR );

  // test tag delete on unsuccessful reindex

  iRc2 = V3Dbf->CreateTable( "TestNdx.DBF", "TestNdx", MyV3Record, XB_OVERLAY, XB_MULTI_USER );
  iRc += TestMethod( iPo, "CreateTable(360)", (xbInt32) iRc2, XB_NO_ERROR );

  iRc += TestMethod( iPo, "BlankRecord(361)",    V3Dbf->BlankRecord(),                     XB_NO_ERROR );
  iRc += TestMethod( iPo, "Putfield(362)",       V3Dbf->PutField( "CFLD", "AAA" ),         XB_NO_ERROR );
  iRc += TestMethod( iPo, "Putfield(363)",       V3Dbf->PutField( "DFLD", "19611109" ),    XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutfieldDouble(364)", V3Dbf->PutDoubleField( "NFLD", 50 ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "AppendRecord(365)",   V3Dbf->AppendRecord(),                    XB_NO_ERROR );

  iRc += TestMethod( iPo, "BlankRecord(370)",    V3Dbf->BlankRecord(),                     XB_NO_ERROR );
  iRc += TestMethod( iPo, "Putfield(371)",       V3Dbf->PutField( "CFLD", "BBB" ),         XB_NO_ERROR );
  iRc += TestMethod( iPo, "Putfield(372)",       V3Dbf->PutField( "DFLD", "19611109" ),    XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutfieldDouble(373)", V3Dbf->PutDoubleField( "NFLD", 50 ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "AppendRecord(374)",   V3Dbf->AppendRecord(),                    XB_NO_ERROR );

  iRc += TestMethod( iPo, "BlankRecord(375)",    V3Dbf->BlankRecord(),                     XB_NO_ERROR );
  iRc += TestMethod( iPo, "Putfield(376)",       V3Dbf->PutField( "CFLD", "BBB" ),         XB_NO_ERROR );
  iRc += TestMethod( iPo, "Putfield(377)",       V3Dbf->PutField( "DFLD", "19611109" ),    XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutfieldDouble(378)", V3Dbf->PutDoubleField( "NFLD", 50 ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "AppendRecord(379)",   V3Dbf->AppendRecord(),                    XB_NO_ERROR );

  iRc2 = V3Dbf->Reindex( 1 );  // verify reindex works with no tags
  iRc += TestMethod( iPo, "Reindex(400)", (xbInt32) iRc2, XB_NO_ERROR );


  x.SetUniqueKeyOpt( XB_HALT_ON_DUPKEY );
  iRc2 = V3Dbf->CreateTag( "NDX", "TestNdxX.NDX", "CFLD", "", 0, 1, XB_OVERLAY, &ixPtr, &ndx );
  iRc += TestMethod( iPo, "CreateTag(401)", (xbInt32) iRc2, XB_NO_ERROR );

  iRc2 = V3Dbf->Reindex( 1 );  // verify reindex fails with dup key
  iRc += TestMethod( iPo, "Reindex(402)", (xbInt32) iRc2, XB_KEY_NOT_UNIQUE );

  x.CloseAllTables();
//  delete V3Dbf;

  if( iPo > 0 || iRc < 0 )
    fprintf( stdout, "Total Errors = %d\n", iRc * -1 );

  #ifdef XB_LOGGING_SUPPORT
  sMsg.Sprintf( "Program [%s] terminating with [%d] errors...", av[0], iRc * -1 );
  x.WriteLogMessage( sMsg, 2 );
  #endif

  return iRc;
}
