/*  xb_dumpdbt.cpp

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


void PrintHelp();
void PrintHelp(){
  std::cout << "Usage: xb_dumpdbt [-h] [-?] [--help] [-v] [--version] -t filename.DBF"  << std::endl << std::endl;
  std::cout << "This program dumps memo record data from an xbase formatted DBT file to stdout." << std::endl << std::endl;
}
void PrintVersion();
void PrintVersion(){
  std::cout << "Xbase64 Version: " << xbase_VERSION_MAJOR << "." << xbase_VERSION_MINOR << "." << xbase_VERSION_PATCH << std::endl;
}

int main(int argc, char *argv[] )
{


  #ifdef XB_MEMO_SUPPORT
  xbXBase  x;
  xbInt16  iRc;
  char     cFieldType;
  xbDbf    *MyFile = NULL;
  xbUInt32 ulMemoFieldLen;
  xbString sFldName;
  xbInt32  lBlockPtr;
  xbString sMemoFldData;

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

  //if(( iRc = x.OpenHighestVersion( sParm.Str(), "", &MyFile )) != XB_NO_ERROR ){
  if(( iRc = x.Open( sParm.Str(), "", &MyFile )) != XB_NO_ERROR ){
    std::cout << "Could not open file iRc = " << iRc  << " file = "  << sParm.Str() << std::endl;
    x.DisplayError( iRc );
    return 1;
  }

  if( MyFile->GetMemoFieldCnt() == 0 ) {
    std::cout << "No memo fields exist in " << sParm.Str() << std::endl;
  } else {

    xbUInt32 ulRecCnt = 0;
    iRc = MyFile->GetRecordCnt( ulRecCnt );

    for( xbUInt32 ul = 1; ul <= ulRecCnt; ul++ ){
      MyFile->GetRecord( ul );
      std::cout << "\nRecord # " << MyFile->GetCurRecNo();
      if( MyFile->RecordDeleted())
        std::cout << " (deleted) ";

      for( int j = 0; j < MyFile->GetFieldCnt(); j++ ) {
        MyFile->GetFieldType( j, cFieldType );

        if( cFieldType == 'M' ) {
          MyFile->GetMemoFieldLen( j, ulMemoFieldLen );
          MyFile->GetFieldName( j, sFldName );
          MyFile->GetLongField( j, lBlockPtr );
          std::cout << "\nMemo field [" << sFldName.Str()
              << "] length = [" << ulMemoFieldLen;
          std::cout << "] Head Block = [" << lBlockPtr << "]\n";

          MyFile->GetMemoField( j, sMemoFldData );
          std::cout << sMemoFldData.Str() << "\n";
        }
      }
    }
    std::cout << "\n";
    MyFile->Close();
    delete MyFile;
  }
  #else
  std::cout << "\nXB_MEMO_SUPPORT is OFF\n";
  #endif

  return 0;
}
