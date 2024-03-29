/*  xb_dumprecs.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2019,2023 Gary A Kunkel

The xb64 software library is covered under 
the terms of the GPL Version 3, 2007 license.

Email Contact:

    xb64-devel@lists.sourceforge.net
    xb64-users@lists.sourceforge.net

*/

#include <xbase.h>

using namespace xb;

void PrintHelp();
void PrintHelp(){
  std::cout << "Usage: xb_dumprecs [-h] [-?] [--help] [-v] [--version] [-q] [--quiet] -t filename.DBF -d D"  << std::endl << std::endl;
  std::cout << "This program dumps data records from an xbase formatted DBF file to stdout in delimited format." << std::endl;
  std::cout << "Memo data (variable length) is not included in this export.  See program xb_dumpdpt for memo data." << std::endl;
  std::cout << "Quiet mode (-q) writes all output to the log file." << std::endl;
  std::cout << "The default delimiter is a comma.  Use the -d option to use a differnt delimter." << std::endl << std::endl;
}
void PrintVersion();
void PrintVersion(){
  std::cout << "Xbase64 Version: " << xbase_VERSION_MAJOR << "." << xbase_VERSION_MINOR << "." << xbase_VERSION_PATCH << std::endl;
}

int main(int argc, char *argv[] )
{
  xbXBase  x;
  xbInt16  iRc;
  xbInt16  iOutputDest = 2;
  xbString sParm;
  char     cDelimiter = ',';
  xbString sInDbf;
  xbString s;

  x.EnableMsgLogging();
  x.SetLogSize( 1000000L );

  if (argc < 2 || x.GetCmdLineOpt( argc, argv, "-h", sParm ) ||
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

  if( !x.GetCmdLineOpt( argc, argv, "-t", sInDbf ) || sInDbf == "" ){
    PrintHelp();
    return 1;
  }

  if ( x.GetCmdLineOpt( argc, argv, "-q", sParm ) ||
       x.GetCmdLineOpt( argc, argv, "--quiet", sParm )){
    iOutputDest = 0;
  }

  if ( x.GetCmdLineOpt( argc, argv, "-d", sParm )){
    if( sParm.Len() > 0 ){
      cDelimiter = sParm[1];
    }
  }


  xbDbf *MyFile = NULL;
  if(( iRc = x.OpenHighestVersion( sInDbf.Str(), "", &MyFile )) != XB_NO_ERROR ){
    s.Sprintf( "Could not open file RC = %d file = %s", iRc, sInDbf.Str());
    x.WriteLogMessage( s, iOutputDest );
    x.DisplayError( iRc );
    return 1;
  }

  //  std::cout << "Processing file sequentially from beginning..." << std::endl;

  #ifdef XB_BLOCKREAD_SUPPORT
  MyFile->EnableBlockReadProcessing();
  #endif

  xbUInt32 j = 0;
  xbUInt32 ulRecCnt = 0;

  iRc = MyFile->GetRecordCnt( ulRecCnt );

  if( iRc < XB_NO_ERROR )
    return iRc;

  s.Sprintf( "Table [%s]", sInDbf.Str() );
  x.WriteLogMessage( s, iOutputDest );

  while( j < ulRecCnt ){
    if( j == 0 )
      iRc = MyFile->DumpRecord(++j, iOutputDest, 2, cDelimiter );   // header + data
    else
      iRc = MyFile->DumpRecord(++j, iOutputDest, 1, cDelimiter );   // data only
    if( iRc != XB_NO_ERROR ){
      x.DisplayError( iRc );
      return 1;
    }
  }
  s.Sprintf( "%d Records processed.", j );
  x.WriteLogMessage( s, iOutputDest );

  MyFile->Close();
  return 0;
}

