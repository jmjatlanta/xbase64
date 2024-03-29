/*  xb_ex_sql.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

    This example demonstrates the implementation of initial sql functions into the library

*/

#include <xbase.h>
using namespace xb;

int main( int argCnt, char **av )
{
  xbInt16 iRc;
  xbXBase x;                                /* initialize xbase             */

#ifdef XB_LOGGING_SUPPORT
  x.EnableMsgLogging();
  std::cout << "Logfile is [" << x.GetLogFqFileName().Str() << "]" << std::endl;
  xbString sMsg;
  sMsg.Sprintf( "Program [%s] initializing...", av[0] );
  x.WriteLogMessage( sMsg );
#endif

  xbSql sql( &x );
  xbString sSql;


  sSql.Sprintf( "USE %s", PROJECT_DATA_DIR);
  iRc = sql.ExecuteNonQuery( sSql );
  if( iRc != XB_NO_ERROR ){
    std::cout << "USE PROJECT_DATA_DIR error" << std::endl;
    x.DisplayError( iRc );
    return 1;
  } else {
    std::cout << sSql << " successful" << std::endl;
  }

  sSql = "DROP TABLE IF EXISTS TESTTBL.DBF";
  iRc = sql.ExecuteNonQuery( sSql );
  if( iRc != XB_NO_ERROR ){
    std::cout << "DROP TABLE error" << std::endl;
    x.DisplayError( iRc );
    return 1;
  } else {
    std::cout << "DROP TABLE successful" << std::endl;
  }

  std::cout << "Drop table completed\n";

  sSql = "CREATE TABLE TESTTBL.DBF( CHRFLD CHAR(60), DTFLD DATE, INTFLD INTEGER, SMINTFLD SMALLINT, NUMFLD NUMERIC(9,4), DECFLD DECIMAL(9,4), FLTFLD FLOAT(9,4) VCHARFLD VARCHAR, LGFLD LOGICAL)"; 
  iRc = sql.ExecuteNonQuery( sSql );
  if( iRc != XB_NO_ERROR ){
    std::cout << "CREATE TABLE error" << std::endl;
    x.DisplayError( iRc );
    return 1;
  } else {
    std::cout << "CREATE TABLE successful" << std::endl;
  }

  sSql = "CREATE INDEX TAG1 ON TESTTBL.DBF( CHRFLD )";
  iRc = sql.ExecuteNonQuery( sSql );
  if( iRc != XB_NO_ERROR ){
    std::cout << "CREATE INDEX error" << std::endl;
    x.DisplayError( iRc );
    return 1;
  } else {
    std::cout << "CREATE INDEX successful" << std::endl;
  }


  sSql = "INSERT INTO TESTTBL.DBF ( CHRFLD ) VALUES ( 'z' )";
  iRc = sql.ExecuteNonQuery( sSql );
  if( iRc != XB_NO_ERROR ){
    std::cout << "INSERT error" << std::endl;
    x.DisplayError( iRc );
    return 1;
  } else {
    std::cout << "INSERT successful" << std::endl;
  }

  char c;
  xbString s;
  for( xbUInt16 i = 0; i < 3 && iRc == XB_NO_ERROR; i++ ){
    for( xbUInt16 j = 0; j < 5 && iRc == XB_NO_ERROR; j++ ){ 
      c = j + 65;
      s = c;
      s.PadRight( c, (xbUInt32) i + 1 );
      sSql.Sprintf( "INSERT INTO TESTTBL.DBF ( CHRFLD ) VALUES ( '%s' )", s.Str());	  
      std::cout << sSql.Str() << std::endl;
      iRc = sql.ExecuteNonQuery( sSql );
      if( iRc != XB_NO_ERROR ){
        std::cout << "INSERT error" << std::endl;
        x.DisplayError( iRc );
        return 1;
      }
    }
  }

  x.CloseAllTables();
  return 0;
}
