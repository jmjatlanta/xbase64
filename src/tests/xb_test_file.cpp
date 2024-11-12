/* xb_test_file.cpp

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

// This program tests the file functions

// usage:   xb_test_file QUITE|NORMAL|VERBOSE


#include "xbase.h"



using namespace xb;

#include "tstfuncs.cpp"

int main( int argCnt, char **av )
{
  int iRc = 0;
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
  xbXBase x;

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
  xbFile  f( &x );
  xbString sWrkStr;
  xbString sWrkStr2;
  sWrkStr = PROJECT_DATA_DIR;
  x.SetDataDirectory( PROJECT_DATA_DIR );

  #ifdef WIN32
    sWrkStr.SwapChars( '/', '\\' );
  #else
    sWrkStr.SwapChars( '\\', '/' );
  #endif

  iRc += TestMethod( iPo, "Set/GetDataDirectory()", x.GetDataDirectory(), sWrkStr, sWrkStr.Len());

  f.SetFileName( "TestFile.txt" );
  sWrkStr = "TestFile.txt";
  iRc += TestMethod( iPo, "Set/GetFileName()", f.GetFileName(), sWrkStr, sWrkStr.Len());

  f.GetFileType( sWrkStr );
  iRc += TestMethod( iPo, "GetFileType()", sWrkStr, "TXT", 3 );

  #ifdef WIN32
  sWrkStr = "\\my\\directory\\";
  #else
  sWrkStr = "/my/directory/";
  #endif

  f.SetDirectory( sWrkStr );
  iRc += TestMethod( iPo, "Set/GetDirectory()", f.GetDirectory(), sWrkStr, sWrkStr.Len());

  sWrkStr += "TestFile.txt";
  iRc += TestMethod( iPo, "GetFqFileName()", f.GetFqFileName(), sWrkStr, 26 );

  #ifdef WIN32
  sWrkStr  = "\\some\\directory\\myfile.dat";
  sWrkStr2 = "\\some\\directory\\";
  #else
  sWrkStr  = "/some/directory/myfile.dat";
  sWrkStr2 = "/some/directory/";
  #endif

  f.SetFqFileName( sWrkStr );
  iRc += TestMethod( iPo, "GetDirectory()", f.GetDirectory(), sWrkStr2, 16 );
  iRc += TestMethod( iPo, "GetFileName()", f.GetFileName(), "myfile.dat", 10 );

  iRc += TestMethod( iPo, "NameSuffixMissing()", f.NameSuffixMissing( "myfile.dbf", 1 ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "NameSuffixMissing()", f.NameSuffixMissing( "myfile", 1 ), XB_NOT_FOUND );
  iRc += TestMethod( iPo, "NameSuffixMissing()", f.NameSuffixMissing( "MYFILE", 1 ), XB_NOT_FOUND );

  f.SetDirectory( PROJECT_DATA_DIR );
  f.SetFileName( "xbfile.txt" );

  iRc += TestMethod( iPo, "xbFopen()", f.xbFopen( "w+b", XB_MULTI_USER ), XB_NO_ERROR );

  xbString sTest;
  sTest = "Test Data";
  iRc += TestMethod( iPo, "xbWrite()", f.xbFwrite( sTest.Str(), 9, 1 ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "xbFclose()", f.xbFclose(), XB_NO_ERROR );

  iRc += TestMethod( iPo, "xbFopen()", f.xbFopen( "r+b", XB_MULTI_USER ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "xbFseek()", f.xbFseek( 0, SEEK_SET ), XB_NO_ERROR );
  char buf[10];
  for( int i = 0; i < 10; i++ )
    buf[i] = 0x00;
  iRc += TestMethod( iPo, "xbFread()", f.xbFread( buf, 5, 1 ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "xbFread()", buf, "Test ", 5 );

  iRc += TestMethod( iPo, "xbFclose()", f.xbFclose(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "xbRemove()", f.xbRemove(), XB_NO_ERROR );
  xbInt16 iWork = 100;
  char cBuf[9];
  char *p = cBuf;
  f.ePutInt16( cBuf, iWork );
  iRc += TestMethod( iPo, "Put/GetShort()", f.eGetInt16( p ), 100 );

  xbInt32 lWork = 10101;
  f.ePutInt32( p, lWork );
  iRc += TestMethod( iPo, "Put/GetLong()", f.eGetInt32( p ), 10101 );

  lWork = 2147483647;
  f.ePutInt32( p, lWork );
  iRc += TestMethod( iPo, "Put/GetLong()", f.eGetInt32( p ), 2147483647 );
  iRc += TestMethod( iPo, "Put/GetLong()", (xbInt32) f.eGetUInt32( p ), 2147483647 );

  xbDouble d = 123456.789;
  f.ePutDouble( p, d );
  iRc += TestMethod( iPo, "Put/GetDouble()", f.eGetDouble( p ), 123456.789 );

  xbString sFqnS;
  xbString sFqnT;
  xbFile f2( &x );
  iRc += TestMethod( iPo, "CreateUniqueFileName()", f2.CreateUniqueFileName( PROJECT_DATA_DIR, "dbf", sFqnS ), XB_NO_ERROR );

  iRc += TestMethod( iPo, "FileExists()", f2.FileExists( sFqnS ), xbFalse );
  iRc += TestMethod( iPo, "xbFopen()",  f2.xbFopen( "w+b", sFqnS, XB_SINGLE_USER ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "xbFclose()", f2.xbFclose(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "FileExists()", f2.FileExists( sFqnS ), xbTrue );


  iRc += TestMethod( iPo, "CreateUniqueFileName()", f2.CreateUniqueFileName( PROJECT_DATA_DIR, "dbf", sFqnT ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "xbRename()", f2.xbRename( sFqnS, sFqnT ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "xbRemove()", f.xbRemove( sFqnT ), XB_NO_ERROR );

  xbString sFn;
  iRc += TestMethod( iPo, "GetFileNamePart()", f2.GetFileNamePart( sFqnS , sFn ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "GetFileExtPart()",  f2.GetFileExtPart( sFqnS , sFn ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "GetFileExtPart()",  f2.GetFileDirPart( sFqnS , sFn ), XB_NO_ERROR );

  iRc += TestMethod( iPo, "SetBlockSize()",    f.SetBlockSize( 100 ), XB_INVALID_BLOCK_SIZE );
  iRc += TestMethod( iPo, "SetBlockSize()",    f.SetBlockSize( 512 ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "GetBlockSize()",    (xbInt32) f.GetBlockSize(), 512 );

  char BlockBuf[513];
  memset( BlockBuf, 0x00, 513 );
  iRc += TestMethod( iPo, "xbFopen()", f.xbFopen( "w+b", XB_SINGLE_USER ), XB_NO_ERROR );

  for( int i = 0; i < 512; i++ )
    BlockBuf[i] = 'A';
  iRc += TestMethod( iPo, "WriteBlock()", f.WriteBlock( 0L, 512, BlockBuf ), XB_NO_ERROR );

  xbUInt64 uulFs = 0;
  iRc += TestMethod( iPo, "GetFileSize(1)", f.GetFileSize( uulFs ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "GetFileSize(2)", uulFs, (xbUInt64) 512 );

  for( int i = 0; i < 512; i++ )
    BlockBuf[i] = 'B';
  iRc += TestMethod( iPo, "WriteBlock()", f.WriteBlock( 1L, 512, BlockBuf ), XB_NO_ERROR );

  iRc += TestMethod( iPo, "GetFileSize(3)", f.GetFileSize( uulFs ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "GetFileSize(4)", uulFs, (xbUInt64) 1024 );


  for( int i = 0; i < 512; i++ )
    BlockBuf[i] = 'C';
  iRc += TestMethod( iPo, "WriteBlock()", f.WriteBlock( 2L, 512, BlockBuf ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "GetFileSize(5)", f.GetFileSize( uulFs ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "GetFileSize(6)", uulFs, (xbUInt64) 1536 );

  char BlockBuf2[513];
  memset( BlockBuf2, 0x00, 513 );
  iRc += TestMethod( iPo, "ReadBlock()", f.ReadBlock( 2L, 512, BlockBuf2 ), XB_NO_ERROR );

  xbString s1 = BlockBuf;
  xbString s2 = BlockBuf2;

  iRc += TestMethod( iPo, "ReadBlock()", s1, s2, 512 );
  iRc += TestMethod( iPo, "xbTruncate()", f.xbTruncate( 1000 ), XB_NO_ERROR );

  xbUInt64 ullFsize;
  iRc += TestMethod( iPo, "GetFileSize()", f.GetFileSize( ullFsize ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "xbGetFileSize()", (xbInt32) ullFsize, 1000 );
  iRc += TestMethod( iPo, "xbFclose()", f.xbFclose(), XB_NO_ERROR );


  if( iPo > 0 || iRc < 0 )
    fprintf( stdout, "Total Errors = %d\n", iRc * -1 );

  #ifdef XB_LOGGING_SUPPORT
  sMsg.Sprintf( "Program [%s] terminating with [%d] errors...", av[0], iRc * -1 );
  x.WriteLogMessage( sMsg );
  #endif

  return iRc;
}

