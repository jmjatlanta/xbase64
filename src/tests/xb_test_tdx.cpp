/* xb_test_tdx.cpp

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
    { "DATE1",  XB_DATE_FLD,      8, 0  },
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
  x.SetTempDirectory( PROJECT_TEMP_DIR );
  x.SetMultiUser( xbFalse );
  InitTime();

  if( iPo > 0 )
    std::cout << "Default Data Directory is [" << x.GetDataDirectory().Str() << "]" << std::endl;


  xbFile f( &x );
  xbIx *pIx;
  void *pTag;
  xbDate dt = "19890209";
  xbString sKey;

  xbDbf *V4DbfX1 = new xbDbf4( &x );


  iRc2 = V4DbfX1->CreateTable( "TTDXDB01.DBF", "TestMdxX2", MyV4Record, XB_OVERLAY, XB_MULTI_USER );
  iRc += TestMethod( iPo, "CreateTable(1)", iRc2, 0 );

  iRc2 = V4DbfX1->CreateTag( "TDX", "CITY_TAGA",  "CITY", "", 0, 0, XB_OVERLAY, &pIx, &pTag );
  iRc += TestMethod( iPo, "CreateTag(1)", iRc2, 0 );

  std::cout << "Cur Tag Name = " << V4DbfX1->GetCurTagName().Str() << "\n";

  xbString sFqFileName = pIx->GetFqFileName();
  iRc2 = V4DbfX1->DeleteTag( "TDX", V4DbfX1->GetCurTagName() );

  iRc += TestMethod( iPo, "DeleteTag(3)", iRc2, 0 );

  // only one tag, file should not exist anymore
  iRc2 = pIx->FileExists(sFqFileName);
  iRc += TestMethod( iPo, "FileExists()", iRc2, 0 );

  iRc2 = V4DbfX1->CreateTag( "TDX", "ZIP_TAG",   "ZIP", "",  xbTrue, 0, XB_OVERLAY, &pIx, &pTag );
  iRc += TestMethod( iPo, "CreateTag(2)", iRc2, 0 );

  xbInt32 uZip = 10000;
  for( xbUInt16 i = 0; i < 5; i++ ){
   for( xbUInt16 j = 0; j < 4; j++ ){ 

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


  iRc += TestMethod( iPo, "CheckTagIntegrity(1)", V4DbfX1->CheckTagIntegrity( 1, 2 ), XB_NO_ERROR );


  x.CloseAllTables();

//  delete V4DbfX1;
//  delete V4DbfX2;

  if( iPo > 0 || iRc < 0 )
    fprintf( stdout, "Total Errors = %d\n", iRc * -1 );

#ifdef XB_LOGGING_SUPPORT
  sMsg.Sprintf( "Program [%s] terminating with [%d] errors...", av[0], iRc * -1 );
  x.WriteLogMessage( sMsg );
#endif

  return iRc;
}
