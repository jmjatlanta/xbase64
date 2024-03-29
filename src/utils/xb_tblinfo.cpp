/*  xb_tblinfo.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2021,2023 Gary A Kunkel

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
  std::cout << "Usage: xb_tblinfo [-h] [-?] [--help] [-v] [--version] -t filename.DBF"  << std::endl << std::endl;
  std::cout << "This program dumps table definition information from a DBF file and associated indices." << std::endl << std::endl;
}
void PrintVersion();
void PrintVersion(){
  std::cout << "Xbase64 Version: " << xbase_VERSION_MAJOR << "." << xbase_VERSION_MINOR << "." << xbase_VERSION_PATCH << std::endl;
}

int main(int argc, char *argv[] ){

  xbXBase  x;
  xbInt16  iRc;
  xbString sTagName;
  xbDbf    *MyFile = NULL;

  x.EnableMsgLogging();
  x.SetLogSize( 1000000L );

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
  MyFile->DumpHeader( 4 );



  #ifdef XB_INDEX_SUPPORT
  xbIxList *ixl;
  xbIx     *ixp;
  xbString sFileType;

  #ifdef XB_MDX_SUPPORT
  ixl = MyFile->GetIxList();
  while( ixl ){
    ixp = ixl->ix;
    ixp->GetFileType( sFileType );
    if( sFileType == "MDX" )
      ixp->DumpHeader( 1, 3 );
    ixl = ixl->next;
  }
  #endif // XB_MDX_SUPPORT


  #ifdef XB_NDX_SUPPORT
  ixl = MyFile->GetIxList();
  while( ixl ){
    ixp = ixl->ix;
    ixp->GetFileType( sFileType );
    if( sFileType == "NDX" )
      ixp->DumpHeader( 1, 3 );
    ixl = ixl->next;
  }
  #endif // XB_NDX_SUPPORT

  #endif // XB_INDEX_SUPPORT

  MyFile->Close();
  delete MyFile;
  return 0;
}
