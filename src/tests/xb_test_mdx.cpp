/* xb_test_mdx.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

// This program tests the class xbIxNdx

// usage:   xb_test_ndx QUITE|NORMAL|VERBOSE


#include "xbase.h"

using namespace xb;

#include "tstfuncs.cpp"

int main( int argCnt, char **av )
{
  int iRc = 0;
  int iRc2;
  int iPo = 1;          /* print option */
                       /* 0 - QUIET    */
                       /* 1 - NORMAL   */
                       /* 2 - VERBOSE  */

  xbString sMsg;
  char c;
  xbString s;

  if( argCnt > 1 ) {
    if( av[1][0] == 'Q' )
      iPo = 0;
    else if( av[1][0] == 'V' )
      iPo = 2;
  }

  xbSchema MyV4Record[] = 
  {
    { "CITY",   XB_CHAR_FLD,    100, 0 },
    { "STATE",  XB_CHAR_FLD,      2, 0 },
    { "ZIP",    XB_NUMERIC_FLD,   9, 0 },
    { "DATE1",  XB_DATE_FLD,      8, 0 },
    { "",0,0,0 }
  };

  xbXBase x;

  #ifdef XB_LOGGING_SUPPORT
  x.SetLogDirectory( PROJECT_LOG_DIR );
  x.EnableMsgLogging();
  if( iPo ){
    std::cout << "Logfile is [" << x.GetLogFqFileName().Str() << "]" << std::endl;
  }
  sMsg.Sprintf( "Program [%s] initializing...", av[0] );
  x.WriteLogMessage( sMsg );
  #endif

  x.SetDataDirectory( PROJECT_DATA_DIR );
  x.SetMultiUser( xbFalse );
  InitTime();

  if( iPo > 0 )
    std::cout << "Default Data Directory is [" << x.GetDataDirectory().Str() << "]" << std::endl;

  x.SetDefaultIxTagMode( XB_IX_DBASE_MODE );
  iRc += TestMethod( iPo, "GetDefaultIxTagMode(100)", x.GetDefaultIxTagMode(), XB_IX_DBASE_MODE );


//  xbFile f( &x );
  xbIx *pIx0;
  void *pTag0;
  xbIx *pIx1;
  void *pTag1;
  xbIx *pIx2;
  void *pTag2;
  xbIx *pIx5;
  void *pTag5;


  xbDate dt = "19890209";
  xbString sKey;

  xbDbf *V4DbfX1 = new xbDbf4( &x );


  iRc2 = V4DbfX1->CreateTable( "TMDXDB01.DBF", "TestMdxX2", MyV4Record, XB_OVERLAY, XB_MULTI_USER );
  iRc += TestMethod( iPo, "CreateTable(101)", iRc2, 0 );

  /*
   CreateTag( const xbString &sIxType, const xbString &sName, const xbString &sKey, const xbString &sFilter, 
           xbInt16 iDescending, xbInt16 iUnique, xbInt16 iOverLay, xbIx **xbIxOut, void **vpTagOut );
  */

  iRc2 = V4DbfX1->CreateTag( "MDX", "CITY_TAGA",  "CITY", "", 0, 0, XB_OVERLAY, &pIx0, &pTag0 );
  iRc += TestMethod( iPo, "CreateTag(102)", iRc2, 0 );

  iRc2 = V4DbfX1->CreateTag( "MDX", "ZIP_TAG",   "ZIP", "",  xbTrue, 0, XB_OVERLAY, &pIx1, &pTag1 );
  iRc += TestMethod( iPo, "CreateTag(103)", iRc2, 0 );

  iRc2 = V4DbfX1->CreateTag( "MDX", "DATE_TAG",  "DATE1", "", 0, xbTrue, XB_OVERLAY, &pIx2, &pTag2 );
  iRc += TestMethod( iPo, "CreateTag(104)", iRc2, 0 );


  // std::cout << "Cur Tag Name = " << V4DbfX1->GetCurTagName().Str() << "\n";

  iRc2 = V4DbfX1->GetTagPtrs( "DATE_TAG", &pIx2, &pTag2 );
  iRc += TestMethod( iPo, "GetTagPtrs(105)", iRc2, XB_NO_ERROR );

  iRc2 = V4DbfX1->GetTagPtrs( "JUNK_TAG", &pIx5, &pTag5 );
  iRc += TestMethod( iPo, "GetTagPtrs(106)", iRc2, XB_INVALID_TAG );

  iRc += TestMethod( iPo, "CheckTagIntegrity(107)", V4DbfX1->CheckTagIntegrity( 1, 0 ), XB_NO_ERROR );
  iRc2 = V4DbfX1->Reindex( 1 );
  iRc += TestMethod( iPo, "Reindex(108)", iRc2, XB_NO_ERROR );

 // xbDouble d = 4000;
//  iRc2 = V4DbfX1->Find( d );
//  std::cout << iRc2 << "\n";


  xbInt32 uZip = 10000;
  for( xbUInt16 i = 0; i < 35; i++ ){
   for( xbUInt16 j = 0; j < 14; j++ ){ 

      c = j + 65;
      s = c;
      s.PadRight( c, (xbUInt32) i + 1 );
      //std::cout << "*********** adding s=[" << s.Str() << "] length = " << s.Len() << "\n";
      iRc2 = V4DbfX1->BlankRecord();
      if( iRc2 != XB_NO_ERROR )
        iRc += TestMethod( iPo, "BlankRecord()", iRc2, XB_NO_ERROR );

      iRc2 = V4DbfX1->PutField( "CITY", s );
      if( iRc2 != XB_NO_ERROR )
        iRc += TestMethod( iPo, "PutField()", iRc2, XB_NO_ERROR );

      iRc2 = V4DbfX1->PutLongField( "ZIP", uZip++ );
      if( iRc2 != XB_NO_ERROR )
        iRc += TestMethod( iPo, "PutField()", iRc2, XB_NO_ERROR );

      // std::cout << dt.Str() << "\n";
      iRc2 = V4DbfX1->PutDateField( "DATE1", dt );
      dt++;

      if( iRc2 != XB_NO_ERROR )
        iRc += TestMethod( iPo, "PutField()", iRc2, XB_NO_ERROR );

      iRc2 = V4DbfX1->AppendRecord();
      if( iRc2 != XB_NO_ERROR )
        iRc += TestMethod( iPo, "AppendRecord()", iRc2, XB_NO_ERROR );

      iRc2 = V4DbfX1->Commit();
      if( iRc2 != XB_NO_ERROR )
        iRc += TestMethod( iPo, "Commit()", iRc2, XB_NO_ERROR );
    }
  }

  iRc += TestMethod( iPo, "CheckTagIntegrity(200)", V4DbfX1->CheckTagIntegrity( 1, 0 ), XB_NO_ERROR );

  iRc2 = V4DbfX1->Reindex( 2, &pIx0, &pTag0 );
  iRc += TestMethod( iPo, "Reindex(201)", iRc2, XB_NO_ERROR );

  iRc2 = V4DbfX1->Reindex( 2, &pIx1, &pTag1 );
  iRc += TestMethod( iPo, "Reindex(202)", iRc2, XB_NO_ERROR );

  iRc2 = V4DbfX1->Reindex( 2, &pIx2, &pTag2 );
  iRc += TestMethod( iPo, "Reindex(203)", iRc2, XB_NO_ERROR );

  iRc += TestMethod( iPo, "CheckTagIntegrity(204)", V4DbfX1->CheckTagIntegrity( 1, 0 ), XB_NO_ERROR );


  // attempt to add a dup key 
  iRc2 = V4DbfX1->BlankRecord();
  if( iRc2 != XB_NO_ERROR )
    iRc += TestMethod( iPo, "BlankRecord(210)", iRc2, XB_NO_ERROR );

  iRc2 = V4DbfX1->PutField( "CITY", "Tampa" );
  if( iRc2 != XB_NO_ERROR )
    iRc += TestMethod( iPo, "PutField(211)", iRc2, XB_NO_ERROR );

  iRc2 = V4DbfX1->PutLongField( "ZIP", uZip++ );
  if( iRc2 != XB_NO_ERROR )
    iRc += TestMethod( iPo, "PutField(212)", iRc2, XB_NO_ERROR );

  dt.Set( "19890209" );
  iRc2 = V4DbfX1->PutDateField( "DATE1", dt );
  dt++;
  if( iRc2 != XB_NO_ERROR )
    iRc += TestMethod( iPo, "PutField(213)", iRc2, XB_NO_ERROR );

  iRc2 = V4DbfX1->AppendRecord();
  if( iRc2 != XB_NO_ERROR )
    iRc += TestMethod( iPo, "AppendRecord(214)", iRc2, XB_NO_ERROR );

  iRc2 = V4DbfX1->Abort();
  if( iRc2 != XB_NO_ERROR )
    iRc += TestMethod( iPo, "Abort(215)", iRc2, XB_NO_ERROR );

  iRc += TestMethod( iPo, "CheckTagIntegrity(220)", V4DbfX1->CheckTagIntegrity( 1, 0 ), 1 );


  uZip = 10000;
  dt = "19890209";
  for( xbUInt16 i = 0; i < 35; i++ ){
   for( xbUInt16 j = 0; j < 14; j++ ){ 

      c = j + 65;
      s = c;
      s.PadRight( c, (xbUInt32) i + 1 );
      //std::cout << "*********** adding s=[" << s.Str() << "] length = " << s.Len() << "\n";
      iRc2 = V4DbfX1->BlankRecord();
      if( iRc2 != XB_NO_ERROR )
        iRc += TestMethod( iPo, "BlankRecord()", iRc2, XB_NO_ERROR );

      iRc2 = V4DbfX1->PutField( "CITY", s );
      if( iRc2 != XB_NO_ERROR )
        iRc += TestMethod( iPo, "PutField()", iRc2, XB_NO_ERROR );

      iRc2 = V4DbfX1->PutLongField( "ZIP", uZip++ );
      if( iRc2 != XB_NO_ERROR )
        iRc += TestMethod( iPo, "PutField()", iRc2, XB_NO_ERROR );

      // std::cout << dt.Str() << "\n";
      iRc2 = V4DbfX1->PutDateField( "DATE1", dt );
      dt++;

      if( iRc2 != XB_NO_ERROR )
        iRc += TestMethod( iPo, "PutField()", iRc2, XB_NO_ERROR );

      iRc2 = V4DbfX1->AppendRecord();
      if( iRc2 != XB_NO_ERROR )
        iRc += TestMethod( iPo, "AppendRecord()", iRc2, XB_NO_ERROR );

      iRc2 = V4DbfX1->Commit();
      if( iRc2 != XB_NO_ERROR )
        iRc += TestMethod( iPo, "Commit()", iRc2, XB_NO_ERROR );
    }
  }

  iRc += TestMethod( iPo, "CheckTagIntegrity(310)", V4DbfX1->CheckTagIntegrity( 1, 0 ), 1 );
  iRc += TestMethod( iPo, "DeleteTag(320)", V4DbfX1->DeleteTag( V4DbfX1->GetCurIxType(), V4DbfX1->GetCurTagName()), XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckTagIntegrity(321)", V4DbfX1->CheckTagIntegrity( 1, 0 ), 1 );


  // test XB_IX_XBASE_MODE functionality
  x.SetDefaultIxTagMode( XB_IX_XBASE_MODE );
  iRc += TestMethod( iPo, "GetDefaultIxTagMode(501)", x.GetDefaultIxTagMode(), XB_IX_XBASE_MODE );

  xbDbf *V4DbfX2 = new xbDbf4( &x );

  iRc2 = V4DbfX2->CreateTable( "TMDXDB02.DBF", "TestMdxX3", MyV4Record, XB_OVERLAY, XB_MULTI_USER );
  iRc += TestMethod( iPo, "CreateTable(101)", iRc2, 0 );

  iRc2 = V4DbfX2->CreateTag( "MDX", "CITY_TAGA",  "CITY", "", 0, 0, XB_OVERLAY, &pIx0, &pTag0 );
  iRc += TestMethod( iPo, "CreateTag(502)", iRc2, 0 );

  iRc2 = V4DbfX2->CreateTag( "MDX", "ZIP_TAG",   "ZIP", "",  xbTrue, xbTrue, XB_OVERLAY, &pIx1, &pTag1 );
  iRc += TestMethod( iPo, "CreateTag(503)", iRc2, 0 );


  // add a record
  iRc2 = V4DbfX2->BlankRecord();
  if( iRc2 != XB_NO_ERROR )
    iRc += TestMethod( iPo, "BlankRecord(510)", iRc2, XB_NO_ERROR );

  iRc2 = V4DbfX2->PutField( "CITY", "Tampa" );
  if( iRc2 != XB_NO_ERROR )
    iRc += TestMethod( iPo, "PutField(511)", iRc2, XB_NO_ERROR );

  iRc2 = V4DbfX2->PutLongField( "ZIP", uZip++ );
  if( iRc2 != XB_NO_ERROR )
    iRc += TestMethod( iPo, "PutField(512)", iRc2, XB_NO_ERROR );

  dt.Set( "19890209" );
  iRc2 = V4DbfX2->PutDateField( "DATE1", dt );
  dt++;
  if( iRc2 != XB_NO_ERROR )
    iRc += TestMethod( iPo, "PutField(513)", iRc2, XB_NO_ERROR );

  iRc2 = V4DbfX2->AppendRecord();
  if( iRc2 != XB_NO_ERROR )
    iRc += TestMethod( iPo, "AppendRecord(514)", iRc2, XB_NO_ERROR );


  iRc2 = V4DbfX2->AppendRecord();
  if( iRc2 != XB_NO_ERROR )
    iRc += TestMethod( iPo, "AppendRecord(515)", iRc2, XB_KEY_NOT_UNIQUE );

  iRc2 = V4DbfX2->Abort();
  if( iRc2 != XB_NO_ERROR )
    iRc += TestMethod( iPo, "Abort(516)", iRc2, XB_NO_ERROR );


  if( iRc2 != XB_NO_ERROR )
    iRc += TestMethod( iPo, "BlankRecord(520)", iRc2, XB_NO_ERROR );

  iRc2 = V4DbfX2->PutField( "CITY", "Miame" );
  if( iRc2 != XB_NO_ERROR )
    iRc += TestMethod( iPo, "PutField(521)", iRc2, XB_NO_ERROR );

  iRc2 = V4DbfX2->PutLongField( "ZIP", uZip++ );
  if( iRc2 != XB_NO_ERROR )
    iRc += TestMethod( iPo, "PutField(522)", iRc2, XB_NO_ERROR );

  dt.Set( "19890209" );
  iRc2 = V4DbfX2->PutDateField( "DATE1", dt );
  dt++;
  if( iRc2 != XB_NO_ERROR )
    iRc += TestMethod( iPo, "PutField(523)", iRc2, XB_NO_ERROR );

  iRc2 = V4DbfX2->AppendRecord();
  if( iRc2 != XB_NO_ERROR )
    iRc += TestMethod( iPo, "AppendRecord(524)", iRc2, XB_NO_ERROR );

  iRc += TestMethod( iPo, "GetRecord(530)",      V4DbfX2->GetRecord( 1 ),  XB_NO_ERROR );
  iRc += TestMethod( iPo, "DeleteRecord(531)",   V4DbfX2->DeleteRecord(),  XB_NO_ERROR );

  iRc2 = V4DbfX1->Commit();
  if( iRc2 != XB_NO_ERROR )
    iRc += TestMethod( iPo, "Commit()", iRc2, XB_NO_ERROR );

  iRc += TestMethod( iPo, "CheckTagIntegrity(540)", V4DbfX2->CheckTagIntegrity( 1, 0 ), 0 );

  x.CloseAllTables();






  if( iPo > 0 || iRc < 0 )
    fprintf( stdout, "Total Errors = %d\n", iRc * -1 );

#ifdef XB_LOGGING_SUPPORT
  sMsg.Sprintf( "Program [%s] terminating with [%d] errors...", av[0], iRc * -1 );
  x.WriteLogMessage( sMsg );
#endif

  return iRc;
}
