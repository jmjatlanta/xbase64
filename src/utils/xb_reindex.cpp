/*  xb_reindex.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2017,2023,2024 Gary A Kunkel

The xb64 software library is covered under 
the terms of the GPL Version 3, 2007 license.

Email Contact:

    xb64-devel@lists.sourceforge.net
    xb64-users@lists.sourceforge.net
*/

#include <xbase.h>
using namespace xb;


void PrintMode( xbInt16 iMode );
void PrintMode( xbInt16 iMode ){

  if( iMode == XB_IX_DBASE_MODE )
    std::cout << "Ix Mode:  [XB_IX_DBASE_MODE]" << std::endl;
  else if( iMode == XB_IX_XBASE_MODE )
    std::cout << "Ix Mode:  [XB_IX_XBASE_MODE]" << std::endl;
  else
    std::cout << "Ix Mode:  [Unknwon]" << std::endl;
}

void PrintHelp();
void PrintHelp(){
  std::cout << "Usage: xb_reindex [-h] [-?] [--help] [-v] [--version] -t filename.DBF [-n index.NDX] -c -u"  << std::endl << std::endl;
  std::cout << "This program reindexes the indices for a specific .DBF file.\n";
  std::cout << "If -n is used, only the specified NDX file is reindexed.\n";
  std::cout << "If -n is not specified, then all tags in the production MDX index and any indices included ";
  std::cout << "in the associated INF file are reindexed." << std::endl;
  std::cout << "-c is XB_IX_DBASE_MODE  - DBase compatibility mode (DBase duplicate key && deleted row handling )" << std::endl;
  std::cout << "-x is XB_IX_XBASE_MODE  - Enforce Unique Index Keys, removed deleted recs from indices." << std::endl;
  
}
void PrintVersion();
void PrintVersion(){
  std::cout << "Xbase64 Version: " << xbase_VERSION_MAJOR << "." << xbase_VERSION_MINOR << "." << xbase_VERSION_PATCH << std::endl;
}

int main(int argc, char *argv[] )
{
  xbXBase x;
  xbInt16 iRc;
  xbString sNdx;

  xbDbf * MyFile = NULL;
  xbIx *pIx      = NULL;
  void *pTag     = NULL;

  xbInt16 iReindexOpt = 1;

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
//    PrintMode( x.GetUniqueKeyOpt());
    return 1;
  }

  if( !x.GetCmdLineOpt( argc, argv, "-t", sParm ) || sParm == "" ){
    PrintHelp();
    return 1;
  }

  if( x.GetCmdLineOpt( argc, argv, "-c", sParm ) && x.GetCmdLineOpt( argc, argv, "-u", sParm )){
    std::cout << "Options -c and -u are mutually exlusive.  Select only one." << std::endl << std::endl;
    PrintHelp();
    return 1;
  }

  if( x.GetCmdLineOpt( argc, argv, "-c", sParm )){
    x.SetDefaultIxTagMode( XB_IX_DBASE_MODE );
    PrintMode( XB_IX_DBASE_MODE );
  } else if( x.GetCmdLineOpt( argc, argv, "-x", sParm )){
    x.SetDefaultIxTagMode( XB_IX_XBASE_MODE );
    PrintMode( XB_IX_XBASE_MODE );
  }

  x.GetCmdLineOpt( argc, argv, "-n", sNdx );

  // if(( iRc = x.OpenHighestVersion( sParm.Str(), "", &MyFile )) != XB_NO_ERROR ){
  if(( iRc = x.Open( sParm.Str(), "", &MyFile )) != XB_NO_ERROR ){
    std::cout << "Could not open file iRc = " << iRc  << " file = "  << sParm.Str() << std::endl;
    x.DisplayError( iRc );
    return 1;
  }

  if( sNdx.Len() > 0 ){
    iReindexOpt = 0;

    // verify valid NDX file name
    if( sNdx.Pos( ".NDX" ) == 0 ){
      std::cout << "\nError with NDX index file.\n";
      std::cout << "File name must end with .NDX\n";
      return 1;
    }

    // drop the ndx extension 
    xbString sTagName = sNdx;

    sTagName.Resize( sTagName.Len() - 3 );
    std::cout << "NDX file name = [" << sNdx.Str() << "] Tag Name = [" << sTagName.Str() << "]\n";

    // see if already in use
    if(( iRc = MyFile->GetTagPtrs( sTagName, &pIx, &pTag )) == XB_INVALID_TAG ){

      //  std::cout << "tag not found, opening tag file\n";
      if(( iRc = MyFile->OpenIndex( "NDX", sNdx )) != XB_NO_ERROR ){
        std::cout << "Error opening index file " << sNdx.Str() << "\n";
        x.DisplayError( iRc );
        return 1;
      }
    }
  }

  if(( iRc  = MyFile->Reindex( iReindexOpt )) != XB_NO_ERROR ) {
    std::cout << "\nError reindexing DBF database ==> " << sParm.Str() << std::endl;
    std::cout << " Return Code = " << iRc << std::endl;
    x.DisplayError( iRc );
    return 1;
  }

  MyFile->Close();
  delete MyFile;

  std::cout << "\nReindex complete...\n\n";
  return 0;
}



