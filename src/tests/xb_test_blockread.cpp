/*  xb_test_blockread.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

// This program tests the class xbBlockRead
// usage:   xb_test_blockread QUITE|NORMAL|VERBOSE


#include "xbase.h"

using namespace xb;

#include "tstfuncs.cpp"

  xbSchema MyDbfRec[] = 
  {
    { "NFLD",       XB_NUMERIC_FLD,   10, 0 },
    { "CFLD",       XB_CHAR_FLD,      89, 0 },
    { "",0,0,0 }
  };


int main( int argCnt, char **av )
{
  xbInt16 iRc  = 0;
  xbInt16 iRc2 = 0;
  int iPo = 1;         /* print option */
                       /* 0 - QUIET    */
                       /* 1 - NORMAL   */
                       /* 2 - VERBOSE  */

  if( argCnt > 1 ) {
    if( av[1][0] == 'Q' )
      iPo = 0;
    else if( av[1][0] == 'V' )
      iPo = 2;
  }

  xbXBase x;
  x.SetDataDirectory( PROJECT_DATA_DIR );

  xbDbf4 dbf( &x );

  #ifdef XB_LOGGING_SUPPORT
  x.SetLogDirectory( PROJECT_LOG_DIR );
  x.EnableMsgLogging();
  if( iPo ){
    std::cout << "Logfile is [" << x.GetLogFqFileName().Str() << "]" << std::endl;
  }
  xbString sMsg;
  sMsg.Sprintf( "Program [%s] initializing...", av[0] );
  x.WriteLogMessage( sMsg );
  #endif


  InitTime();


  iRc2 = dbf.CreateTable( "BLOCKRD.DBF", "BlockRead", MyDbfRec, XB_OVERLAY, XB_MULTI_USER );
  iRc += TestMethod( iPo, "CreateTable()", (xbInt32) iRc2, XB_NO_ERROR );
  if( iRc2 )
    x.DisplayError( iRc2 );


  // fill the file with several blocks of data
  for( xbInt32 i = 0; i < 50; i++ ){
    iRc2 = dbf.BlankRecord();
    if( iRc2 != XB_NO_ERROR )
      iRc += TestMethod( iPo, "BlankRecord()", (xbInt32) iRc2, XB_NO_ERROR );

    iRc2 = dbf.PutLongField( 0, i+1 );
    if( iRc2 != XB_NO_ERROR )
      iRc += TestMethod( iPo, "PutLongField()", (xbInt32) iRc2, XB_NO_ERROR );

    iRc2 = dbf.AppendRecord();
    if( iRc2 != XB_NO_ERROR )
      iRc += TestMethod( iPo, "AppendRecord()", (xbInt32) iRc2, XB_NO_ERROR );

    iRc2 = dbf.Commit();
    if( iRc2 != XB_NO_ERROR )
      iRc += TestMethod( iPo, "Commit()", (xbInt32) iRc2, XB_NO_ERROR );
  }


  dbf.SetDefaultBlockReadSize( 2000 );

  // enable block read for this table
  iRc += TestMethod( iPo, "EnableBlockReadProcessing()", dbf.EnableBlockReadProcessing(), XB_NO_ERROR );

  xbUInt32 ulFld = 0;
  xbUInt32 ulCtr = 1;

  iRc2 = XB_NO_ERROR;
  while( iRc2 == XB_NO_ERROR ){
    //    std::cout << ulCtr << "\n";
    iRc2 += dbf.GetRecord( ulCtr );
    if( iRc2 != XB_NO_ERROR && iRc != XB_INVALID_RECORD )
      TestMethod( iPo, "GetRecord()", iRc, XB_NO_ERROR );

    if( iRc2 == XB_NO_ERROR ){
      dbf.GetULongField( "NFLD", ulFld );
      if( ulFld != ulCtr ){
        std::cout << "ulFld = " << ulFld << " ulCtr = " << ulCtr << "\n";
        iRc += TestMethod( iPo, "Field Compare", (xbDouble) ulFld, (xbDouble) ulCtr );
      }
    }
    ulCtr++;
  }

  // std::cout << "**********Delete every other record\n";
  iRc2 = dbf.GetFirstRecord();
  while( iRc2 == XB_NO_ERROR ){
    if( (dbf.GetCurRecNo() % 2) != 0 ){
      dbf.DeleteRecord();
      dbf.Commit();
    }
    iRc2 = dbf.GetNextRecord();
  }


  // test filter for deleted records
  #ifdef XB_FILTER_SUPPORT
  xbFilter f( &dbf );
  xbString sFilt = ".NOT. DELETED()";
  f.Set( sFilt );
  iRc2 = f.GetFirstRecord();
  while( iRc2 == XB_NO_ERROR ){
    dbf.GetULongField( "NFLD", ulFld );
    if( (ulFld % 2) != 0 ){
      iRc += TestMethod( iPo, "Filter GetNextRecord()", ulFld % 2, 0 );
    }
    iRc2 = f.GetNextRecord();
  }
  #endif  // XB_FILTER_SUPPORT


  iRc += TestMethod( iPo, "DisableBlockReadProcessing()", dbf.DisableBlockReadProcessing(), XB_NO_ERROR );
  //iRc2 = dbf.DeleteTable();

  iRc2 = dbf.Close();
  iRc += TestMethod( iPo, "Close()", (xbInt32) iRc2, XB_NO_ERROR );
  if( iRc2 )
    x.DisplayError( iRc2 );


  #ifdef XB_LOGGING_SUPPORT
  sMsg.Sprintf( "Program [%s] terminating with [%d] errors...", av[0], iRc * -1 );
  x.WriteLogMessage( sMsg );
  #endif


  if( iPo > 0 || iRc < 0 )
    fprintf( stdout, "Total Errors = %d\n", iRc * -1 );

  return iRc;
}

