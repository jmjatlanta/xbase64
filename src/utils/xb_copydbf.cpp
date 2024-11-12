/*  xb_copydbf.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2019,2023,2024 Gary A Kunkel

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
  std::cout << "Usage: xb_copydbf [-h] [-?] [--help] [-v] [--version] -i input.DBF -o output.DBF"  << std::endl << std::endl;
  std::cout << "This program copies the structure of a table defined in input.DBF to output.DBF.  Data and indices are not included.";
  std::cout << std::endl << std::endl;
}
void PrintVersion();
void PrintVersion(){
  std::cout << "Xbase64 Version: " << xbase_VERSION_MAJOR << "." << xbase_VERSION_MINOR << "." << xbase_VERSION_PATCH << std::endl;
}


int main(int argc, char *argv[])
{

  xbXBase x;
  xbInt16 iRc;
  xbDbf *dSrc = NULL;
  xbDbf *dTrg = NULL;

  xbString sParm;
  xbString sSrcDbf;
  xbString sTrgDbf;

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

  if( !x.GetCmdLineOpt( argc, argv, "-i", sSrcDbf ) || sSrcDbf == "" ){
    PrintHelp();
    return 1;
  }

  if( !x.GetCmdLineOpt( argc, argv, "-o", sTrgDbf ) || sTrgDbf == "" ){
    PrintHelp();
    return 1;
  }

  #ifdef XB_LOGGING_SUPPORT
  x.EnableMsgLogging();
  xbString sMsg;
  sMsg.Sprintf( "Program [%s] initializing...", argv[0] );
  x.WriteLogMessage( sMsg );
  #endif

  //if(( iRc = x.OpenHighestVersion( sSrcDbf, "", &dSrc )) != XB_NO_ERROR ){

  if(( iRc = x.Open( sSrcDbf, "", &dSrc )) != XB_NO_ERROR ){
    std::cout << "Could not open file iRc = " << iRc  << " file = "  << sSrcDbf.Str() << std::endl;
    x.DisplayError( iRc );
    return 1;
  }

  #ifdef XB_DBF4_SUPPORT
  dTrg = new xbDbf4( &x );
  #else
  dTrg = new xbDbf3( &x );
  #endif

  if(( iRc = dSrc->CopyDbfStructure( dTrg, sTrgDbf, sTrgDbf, XB_DONTOVERLAY, XB_SINGLE_USER )) != XB_NO_ERROR ){
      std::cout << "Could not copy file " << sTrgDbf.Str() << " Error = " << iRc  << "\n";
      x.DisplayError( iRc );
      return 1;
  }

  x.CloseAllTables();
  return 0;
}





