/* xb_ex_filter.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2020,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

// This program demonstrates usage of the xbFilter class
// It uses the DBF table and NDX index file that were created by the xb_test_filter program.


#include "xbase.h"

using namespace xb;


int main()
{
  int iRc = 0;

  xbXBase x;
  x.SetDataDirectory( PROJECT_DATA_DIR );

  x.EnableMsgLogging();
  x.SetLogSize( 1000000L );

  #ifdef XB_DBF4_SUPPORT
  xbDbf *myTable = new xbDbf4( &x );
  #else
  xbDbf *myTable = new xbDbf3( &x );
  #endif

  if(( iRc = myTable->Open( "TestFilt.DBF" )) != XB_NO_ERROR ){
    std::cout << "Error opening TestFilt.DBF" << std::endl;
    x.DisplayError( iRc );
    return 1;
  }

  #ifdef XB_BLOCKREAD_SUPPORT
  myTable->EnableBlockReadProcessing();    // speed things up
  #endif


  xbFilter myFilter( myTable );
  myFilter.Set( "ZFLD = 'Z'" );

  std::cout << "\nLoop through table forwards";
  iRc = myFilter.GetFirstRecord();
  while( iRc == XB_NO_ERROR ){
    myTable->DumpRecord( myTable->GetCurRecNo(), 1, 0, ',' );
    iRc = myFilter.GetNextRecord();
  }

  std::cout << "\nLoop through table backwards";
  iRc = myFilter.GetLastRecord();
  while( iRc == XB_NO_ERROR ){
    myTable->DumpRecord( myTable->GetCurRecNo(), 1, 0, ',' );
    iRc = myFilter.GetPrevRecord();
  }

  // Index example
  #ifdef XB_NDX_SUPPORT
  if(( iRc = myTable->OpenIndex( "NDX", "TestFilt.NDX")) != XB_NO_ERROR ){
    std::cout << "Error opening TestFilt.NDX" << std::endl;
    x.DisplayError( iRc );
    return 1;
  }

  xbFilter myFilterIx( myTable );
  myFilterIx.Set( "ZFLD = 'Z'", "TestFilt" );

  std::cout << "\nLoop through table forwards by index";
  iRc = myFilterIx.GetFirstRecordIx();
  while( iRc == XB_NO_ERROR ){
    myTable->DumpRecord( myTable->GetCurRecNo(), 1, 0, ',' );
    iRc = myFilterIx.GetNextRecordIx();
  }

  std::cout << "\nLoop through table backwards by index";
  iRc = myFilterIx.GetLastRecordIx();
  while( iRc == XB_NO_ERROR ){
    myTable->DumpRecord( myTable->GetCurRecNo(), 1, 0, ',' );
    iRc = myFilterIx.GetPrevRecordIx();
  }
  #endif   // XB_NDX_SUPPORT

  x.CloseAllTables();
  return 0;
}
