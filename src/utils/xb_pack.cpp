/*  xb_pack.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2017,2023 Gary A Kunkel

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
  std::cout << "Usage: xb_pack [-h] [-?] [--help] [-v] [--version] -t filename.DBF"  << std::endl << std::endl;
  std::cout << "This program removes data records that are flagged for deletion in a DBF file and rebuilds indices." << std::endl << std::endl;
}
void PrintVersion();
void PrintVersion(){
  std::cout << "Xbase64 Version: " << xbase_VERSION_MAJOR << "." << xbase_VERSION_MINOR << "." << xbase_VERSION_PATCH << std::endl;
}

int main(int argc, char *argv[] )
{
  xbXBase x;
  xbInt16 iRc;
  xbDbf * MyFile = NULL;

  xbString sParm;
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

  if( !x.GetCmdLineOpt( argc, argv, "-t", sParm ) || sParm == "" ){
    PrintHelp();
    return 1;
  }

  if(( iRc = x.OpenHighestVersion( sParm.Str(), "", &MyFile )) != XB_NO_ERROR ){
    std::cout << "Could not open file iRc = " << iRc  << " file = "  << sParm.Str() << std::endl;
    x.DisplayError( iRc );
    return 1;
  }

  if(( iRc  = MyFile->Pack()) != XB_NO_ERROR ) {
    std::cout << "\nError packing DBF database ==> " << sParm.Str() << std::endl;
    std::cout << " Return Code = " << iRc << std::endl;
    return 1;
  }

  #ifdef XB_INDEX_SUPPORT
  if(( iRc  = MyFile->Reindex( 1 )) != XB_NO_ERROR ) {
    std::cout << "\nError reindexing DBF database ==> " << sParm.Str() << std::endl;
    std::cout << " Return Code = " << iRc << std::endl;
    x.DisplayError( iRc );
    return 1;
  }
  #endif  // XB_INDEX_SUPPORT

  MyFile->Close();
  delete MyFile;

  std::cout << "\nPack Database complete...\n\n";
  return 0;
}



