/*  xb_execsql.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2023 Gary A Kunkel

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
  std::cout << "Usage: xb_execsql [-h] [-?] [--help] [-v] [--version] -s filename.SQL  -q --quiet"  << std::endl << std::endl;
  std::cout << "This program processes sql commands from input file 'filename.SQL'" << std::endl;
  std::cout << "For more information, see Chapter 6 (xbc6.html) in the included Xbase64 documentation." << std::endl << std::endl;
}
void PrintVersion();
void PrintVersion(){
  std::cout << "Xbase64 Version: " << xbase_VERSION_MAJOR << "." << xbase_VERSION_MINOR << "." << xbase_VERSION_PATCH << std::endl;
}


xbInt16 GetNextSqlCmd( xbFile &f, xbString &sCmd, xbBool bQuiet );
xbInt16 GetNextSqlCmd( xbFile &f, xbString &sCmd, xbBool bQuiet ){
  sCmd = "";
  xbString sLine;
  xbInt16 iRc = XB_NO_ERROR;
  xbUInt32 lPos = 0;
  xbBool bDone = xbFalse;
  while( !bDone ){
    if(( iRc = f.xbFgets( 256, sLine )) != XB_NO_ERROR ){
      bDone = xbTrue;
    } else {

      if( !bQuiet ){
        std::cout << sLine;
        if( sLine.Pos( 0x0a ) == 0 )
          std::cout << std::endl;
      }

      // don't need CR/LF chars
      sLine.ZapChar( 0x0a );
      sLine.ZapChar( 0x0d );

      // if comment, zap out everything to the right of the hash
      lPos = sLine.Pos( '#' );
      if( lPos > 0 )
        sLine.Left( lPos - 1);

      if( sLine.Pos( ';' ) > 0 ){
        bDone = xbTrue;
        sLine.ZapChar( ';' );
      }
    }
    sCmd += sLine;
  }
  return iRc;
}


int main(int argc, char* argv[])
{
  xbXBase  x;
  xbSql    sql( &x );
  xbFile   f( sql.GetXbasePtr() );
  xbInt16  iRc = XB_NO_ERROR;
  xbString sFileName = "";
  xbString sSqlLine  = "";
  xbString sParm     = "";
  xbBool   bQuiet    = xbFalse;

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

  if( !x.GetCmdLineOpt( argc, argv, "-s", sFileName ) || sFileName == "" ){
    PrintHelp();
    return 1;
  }


  if(( iRc = f.xbFopen( "r", sFileName, XB_SINGLE_USER )) != XB_NO_ERROR ){
    xbString sMsg;
    sMsg.Sprintf( "Error opening [%s]\n", sFileName.Str() );
    std::cout << sMsg.Str();
    sql.GetXbasePtr()->DisplayError( iRc );
    return 1;
  }

  while( iRc == XB_NO_ERROR ){
    iRc = GetNextSqlCmd( f, sSqlLine, bQuiet );

    if( iRc == XB_NO_ERROR ){

      sSqlLine.Trim();
      // std::cout << "Processing line [" << sSqlLine.Str() << "]\n";
      iRc = sql.ExecuteNonQuery( sSqlLine );
      if( iRc != XB_NO_ERROR )
        x.DisplayError( iRc );
    }
  }

  f.xbFclose();
  return 0;
}

