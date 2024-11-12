/*  xb_import.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2023,2024 Gary A Kunkel

The xb64 software library is covered under 
the terms of the GPL Version 3, 2007 license.

Email Contact:

    xb64-devel@lists.sourceforge.net
    xb64-users@lists.sourceforge.net

*/

#include <xbase.h>
using namespace xb;


struct sFldMap{
  xbUInt32 iRecPos;
  xbInt16  iFldNo;
  char     cFldType;
  sFldMap  *next;
};

void PrintHelp();
void PrintHelp(){
  std::cout << "Usage: xb_execsql [-h] [-?] [--help] [-v] [--version] -i infilename.txt  -d delimeter [TAB] -t table.DBF -q --quiet"  << std::endl << std::endl;
  std::cout << "This program imports data from a delimited text file into a specified DBF file/table." << std::endl << std::endl;
}
void PrintVersion();
void PrintVersion(){
  std::cout << "Xbase64 Version: " << xbase_VERSION_MAJOR << "." << xbase_VERSION_MINOR << "." << xbase_VERSION_PATCH << std::endl;
}



int main(int argc, char* argv[])
{
  xbXBase  x;
  xbSql    sql( &x );
  xbFile   f( sql.GetXbasePtr() );
  xbInt16  iRc        = XB_NO_ERROR;
  xbString sFileName  = "";
  xbString sTableName = "";
  xbString sFldName   = "";
  xbString sSqlLine   = "";
  xbString sParm      = "";
  xbString sMsg;
  xbString sLine;
  xbString sFld;
  xbBool   bQuiet      = xbFalse;
  char     cDelimiter  = ',';
  char     cType       = ' ';
  sFldMap  *fmFldList  = 0;
  sFldMap  *fmTemp     = 0;
  xbInt16  iFldNo      = 0;
  xbUInt32 ulRecCtr    = 0;
  xbUInt32 ulImportCtr = 0;
  xbBool   bRecUpdated;
  x.EnableMsgLogging();


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

  if ( x.GetCmdLineOpt( argc, argv, "-q", sParm ) ||
       x.GetCmdLineOpt( argc, argv, "--quiet", sParm )){
    bQuiet = xbTrue;
  }

  if ( x.GetCmdLineOpt( argc, argv, "-d", sParm )){

    std::cout << "delimiter[" << sParm.Str() << "]\n";

    if( sParm == "TAB" ){
      cDelimiter = '\t';
      std::cout << "tab delimiter[" << sParm.Str() << "]\n";
    }
    else if( sParm.Len() > 0 ){
      cDelimiter = sParm[1];
    }
  }

  if( !x.GetCmdLineOpt( argc, argv, "-i", sFileName ) || sFileName == "" ){
    PrintHelp();
    return 1;
  }

  if( !x.GetCmdLineOpt( argc, argv, "-t", sTableName ) || sTableName == "" ){
    PrintHelp();
    return 1;
  }

  xbDbf *MyFile = NULL;
  // if(( iRc = x.OpenHighestVersion( sTableName.Str(), "", &MyFile )) != XB_NO_ERROR ){
  if(( iRc = x.Open( sTableName.Str(), "", &MyFile )) != XB_NO_ERROR ){
    std::cout << "Could not open table/file RC = " << iRc  << " file = "  << sTableName.Str() << std::endl;
    x.DisplayError( iRc );
    return 1;
  }

  if(( iRc = f.xbFopen( "r", sFileName, XB_SINGLE_USER )) != XB_NO_ERROR ){
    sMsg.Sprintf( "Error opening [%s]\n", sFileName.Str() );
    std::cout << sMsg.Str();
    x.DisplayError( iRc );
    return 1;
  }


  if(( iRc = f.xbFgets( 1024, sLine )) != XB_NO_ERROR ){
    sMsg.Sprintf( "Error reading [%s]\n", sFileName.Str() );
    std::cout << sMsg.Str();
    x.DisplayError( iRc );
    return 1;
  }


  // determine how many fields in a record
  xbUInt32 lFldCnt = sLine.CountChar( cDelimiter, 1 );
  //  std::cout << "in rec = [" << sLine.Str() << "]\n";
  //  std::cout << "fld cnt = [" << lFldCnt << "]\n";

  // do the mapping between field names in source data and field numbers in target table
  for( xbUInt32 l = 0; l < (lFldCnt + 1); l++ ){

    // get the field
    sFld.ExtractElement( sLine.Str(), cDelimiter, l+1, 1 );
    sFld.ZapTrailingChar( 0x0a );   // eliminate CRLF
    sFld.ZapTrailingChar( 0x0d );   // eliminate CRLF


    // do the lookup
    // std::cout << "processing field [" << l << "] [" << sFld.Str() << "]\n";
    // if found, create an entry in the field list structure
    // else if not quiet, display a message

    //iRc = MyFile->GetFieldNo( sFld, &iFldNo );

    if(( iRc = MyFile->GetFieldNo( sFld, iFldNo )) == XB_NO_ERROR ){
      MyFile->GetFieldType( iFldNo, cType );
      fmTemp = (sFldMap *) calloc( 1, sizeof( sFldMap ));
      if( !fmTemp ){
        std::cout << "Memory allocation error\n";
        exit(1);
      } else {
        fmTemp->iRecPos = l;
        fmTemp->iFldNo = iFldNo;
        fmTemp->cFldType = cType;
        fmTemp->next = fmFldList;
        fmFldList = fmTemp;

      }
    } else {
      if( !bQuiet ){
        std::cout << "Field [" << sFld.Str() << "] not found in target table" << std::endl;
      }
    }
  }


  while( f.xbFgets( 1024, sLine ) == XB_NO_ERROR ){
    bRecUpdated = xbFalse;
    // std::cout << sLine.Str() << "\n";

    if(( iRc = MyFile->BlankRecord()) != XB_NO_ERROR ){
      sMsg.Sprintf( "MyFile->BlankRecord() error [%d]\n", iRc );
      std::cout << sMsg.Str();
      x.DisplayError( iRc );
    } else {

      fmTemp = fmFldList;
      while( fmTemp ){ 

        // std::cout << "*** RecPos = " << fmTemp->iRecPos  << "   FldNo  = " << fmTemp->iFldNo  << "  Type = " << fmTemp->cFldType << "\n";

        sFld.ExtractElement( sLine.Str(), cDelimiter, fmTemp->iRecPos+1, 1 );
        sFld.ZapTrailingChar( 0x0a );   // eliminate CRLF
        sFld.ZapTrailingChar( 0x0d );   // eliminate CRLF

        // remove any matching leading and trailing quotes
        if( sFld[1] == '\'' && sFld[sFld.Len()] == '\'' ){
          sFld.ZapTrailingChar( '\'' );
          sFld.ZapLeadingChar ( '\'' );
          //std::cout << "DataNq   = " << sFld.Str()       << "\n";
        } else if( sFld[1] == '"' && sFld[sFld.Len()] == '"' ){
          sFld.ZapTrailingChar( '"' );
          sFld.ZapLeadingChar ( '"' );
        }

        // std::cout << "Data   = " << sFld.Str()       << "\n";
        if( sFld.Len() > 0 ){
          bRecUpdated = xbTrue;
          if( fmTemp->cFldType == 'C' || fmTemp->cFldType == 'L' || fmTemp->cFldType == 'D' || fmTemp->cFldType == 'N' || fmTemp->cFldType == 'F' ){
            iRc = MyFile->PutField( fmTemp->iFldNo, sFld );
          } else if( fmTemp->cFldType == 'M' ){
            iRc = MyFile->UpdateMemoField( fmTemp->iFldNo, sFld );
          } else {
            std::cout << "Field type [" << fmTemp->cFldType << "] not built yet" << std::endl;
          }

          if( iRc != XB_NO_ERROR  && !bQuiet ){
            MyFile->GetFieldName( fmTemp->iFldNo, sFldName );
            std::cout << "Error [" <<  x.GetErrorMessage( iRc ) << "][" << sFld.Str() << "] on field [" << sFldName << "] on record [" << ulRecCtr << "]" << std::endl;
          }
        }
        fmTemp = fmTemp->next;
      }

      if( bRecUpdated ){
        iRc = MyFile->AppendRecord();
        if( iRc != XB_NO_ERROR ){
          if( !bQuiet ){
            std::cout << "Error [" << x.GetErrorMessage( iRc ) << "] on appending record [" << ulRecCtr << "]" << std::endl;
          }
          MyFile->Abort();
        } else {
          iRc = MyFile->Commit();
          if( iRc != XB_NO_ERROR ){
            if( !bQuiet ){
              std::cout << "Error [" << x.GetErrorMessage( iRc ) << "] on appending record [" << ulRecCtr << "]" << std::endl;
            }
            MyFile->Abort();
          } else {
            ulImportCtr++;
          }
        }
      }

    }

    ulRecCtr++;
  }


  if( !bQuiet )
    std::cout << ulImportCtr << " records imported to " << sTableName << "\n";



  f.xbFclose();
  return 0;
}

