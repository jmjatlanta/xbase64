/*  xb_deletall.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2017,2019,2023,2024 Gary A Kunkel

The xb64 software library is covered under 
the terms of the GPL Version 3, 2007 license.

Email Contact:

    xb64-devel@lists.sourceforge.net
    xb64-users@lists.sourceforge.net

This program sets the delete flag on all records in a dbf file

*/


#include <xbase.h>
using namespace xb;

void PrintHelp();
void PrintHelp(){
  std::cout << "Usage: xb_deleteall [-h] [-?] [--help] [-v] [--version] -t myfile.DBF"  << std::endl << std::endl;
  std::cout << "This program flags all the records in a DBF table for deletion.  To physically remove the deleted records, see program xb_pack.";
  std::cout << std::endl << std::endl;
}
void PrintVersion();
void PrintVersion(){
  std::cout << "Xbase64 Version: " << xbase_VERSION_MAJOR << "." << xbase_VERSION_MINOR << "." << xbase_VERSION_PATCH << std::endl;
}

int main(int argc, char *argv[])
{

  xbXBase x;
  xbDbf *MyFile = NULL;
  xbFile f( &x );
  xbInt16 iRc = 0;

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

  // if(( iRc = x.OpenHighestVersion( sParm.Str(), "", &MyFile )) != XB_NO_ERROR ){
  if(( iRc = x.Open( sParm.Str(), "", &MyFile )) != XB_NO_ERROR ){
    std::cout << "Could not open file RC = " << iRc  << " file = "  << sParm.Str() << std::endl;
    x.DisplayError( iRc );
    return 1;
  }

  iRc = MyFile->DeleteAllRecords(); 
  if( iRc != XB_NO_ERROR ) {
    std::cout << "Error Deleting all records - database ==> "  << sParm.Str()  << "\n";
    std::cout << "Return Code = " << iRc;
    x.DisplayError( iRc );
    return 1;
  }
  MyFile->Close();
  delete MyFile;
  return 0;
}