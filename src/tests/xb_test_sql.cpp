/* xb_test_sql.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014, 2022,2023, 2024 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    xb64-devel@lists.sourceforge.net
    xb64-users@lists.sourceforge.net

*/

// This program tests the SQL functions

// usage:   xb_test_sql QUITE|NORMAL|VERBOSE



#include "xbase.h"

using namespace xb;

#include "tstfuncs.cpp"


int main( int argCnt, char **av )
{
  int iRc = 0;
  int iRc2 = 0;
  int po = 1;          /* print option */
                       /* 0 - QUIET    */
                       /* 1 - NORMAL   */
                       /* 2 - VERBOSE  */

  xbString sSql;

  if( argCnt > 1 ) {
    if( av[1][0] == 'Q' )
      po = 0;
    else if( av[1][0] == 'V' )
      po = 2;
  }

/*
  xbSchema MyAddressRecord[] = 
  {
    { "ADDRESS",     XB_CHAR_FLD,     30, 0 },
    { "CITY",        XB_CHAR_FLD,     30, 0 },
    { "STATE",       XB_CHAR_FLD,      2, 0 },
    { "ZIPCODE",     XB_NUMERIC_FLD,   9, 0 },
    { "NOTES",       XB_MEMO_FLD,     10, 0 },
    { "LASTUPDATE",  XB_DATE_FLD,      8, 1 },
    { "ACTIVE",      XB_LOGICAL_FLD,   1, 0 },
    { "",0,0,0 }
  };

  above structure corresponds to sql below

  sSql = "CREATE TABLE Address.DBF ( ADDRESS CHAR(30), CITY CHAR(30), STATE CHAR(2), ZIPCODE NUMERIC(9,0), NOTES VARCHAR, LASTUPDATE DATE, ACTIVE LOGICAL )";
*/

/*
  xbSchema MyZipRecord[] = 
  {
    { "ZIPCODE",     XB_NUMERIC_FLD,   9, 0 },
    { "CITY",        XB_CHAR_FLD,     30, 0 },
    { "STATE",       XB_CHAR_FLD,      2, 0 },
    { "",0,0,0 }
  };
*/

  xbXBase x;
  #ifdef XB_LOGGING_SUPPORT
  x.SetLogDirectory( PROJECT_LOG_DIR );
  x.EnableMsgLogging();
  x.SetDefaultIxTagMode( XB_IX_XBASE_MODE );
  if( po ){
    std::cout << "Logfile is [" << x.GetLogFqFileName().Str() << "]" << std::endl;
  }
  xbString sMsg;
  sMsg.Sprintf( "Program [%s] initializing...", av[0] );
  x.WriteLogMessage( sMsg );
  #endif


  x.SetDataDirectory( PROJECT_DATA_DIR );

  xbSql sql( &x );

  if( po > 0 )
    std::cout << "Default Data Directory is [" << x.GetDataDirectory().Str() << "]" << std::endl;

/*
  // verify a delete on a non existant table doesn't crash things
  sSql = "DROP TABLE IF EXISTS NoTable.DBF";
  iRc2 = sql.ExecuteNonQuery( sSql );
  iRc += TestMethod( po, "DropTable(100)", (xbInt32) iRc2, XB_NO_ERROR );
  if( iRc2 )
    x.DisplayError( iRc2 );

  sSql = "DROP TABLE IF EXISTS Address.DBF";
  iRc2 = sql.ExecuteNonQuery( sSql );
  iRc += TestMethod( po, "DropTable(101)", (xbInt32) iRc2, XB_NO_ERROR );
  if( iRc2 )
    x.DisplayError( iRc2 );

  sSql = "CREATE TABLE Address.DBF ( ADDRESS CHAR(30), CITY CHAR(30), STATE CHAR(2), ZIPCODE NUMERIC(9,0), NOTES VARCHAR, LASTUPDATE DATE, ACTIVE LOGICAL   )";
  iRc2 = sql.ExecuteNonQuery( sSql );
  iRc += TestMethod( po, "CreateTable()", (xbInt32) iRc2, XB_NO_ERROR );
  if( iRc2 )
    x.DisplayError( iRc2 );

  #ifdef XB_MDX_SUPPORT
  sSql = "CREATE INDEX tag1 ON Address.DBF( CITY, STATE, DTOS( LASTUPDATE )) FILTER .NOT. DELETED()";
  // xbString sSql = "CREATE INDEX tag1 ON Address.DBF( CITY, STATE )";

  iRc2 = sql.ExecuteNonQuery( sSql );
  iRc += TestMethod( po, "SqL CreateIndex(102)", (xbInt32) iRc2, XB_NO_ERROR );
  if( iRc2 )
    x.DisplayError( iRc2 );
  #endif // XB_MDX_SUPPORT

  sSql = "INSERT INTO Address (CITY, STATE, ZIPCODE, NOTES, LASTUPDATE, ACTIVE ) VALUES ( 'San Diego', 'CA', 92007, 'San Diego is a cool place', '1989-02-09', 'Y')";
  iRc2 = sql.ExecuteNonQuery( sSql );
  iRc += TestMethod( po, "SqlInsert(103)", (xbInt32) iRc2, XB_NO_ERROR );
  if( iRc2 )
    x.DisplayError( iRc2 );

  sSql = "INSERT INTO Address (CITY, STATE, ZIPCODE, NOTES, LASTUPDATE, ACTIVE ) VALUES ( 'Dallas', 'TX', 76248, 'Dallas is hot in the summer', '1989-02-09', 'N')";
  iRc2 = sql.ExecuteNonQuery( sSql );
  iRc += TestMethod( po, "SqlInsert()", (xbInt32) iRc2, XB_NO_ERROR );
  if( iRc2 )
    x.DisplayError( iRc2 );

  sSql = "DROP TABLE IF EXISTS AddressR.DBF";
  iRc2 = sql.ExecuteNonQuery( sSql );
  iRc += TestMethod( po, "DropTable(104)", (xbInt32) iRc2, XB_NO_ERROR );
  if( iRc2 )
    x.DisplayError( iRc2 );

  sSql = "ALTER TABLE Address.DBF RENAME TO AddressR.DBF";
  iRc2 = sql.ExecuteNonQuery( sSql );
  iRc += TestMethod( po, "SqlAlterTable(105)", (xbInt32) iRc2, XB_NO_ERROR );
  if( iRc2 )
    x.DisplayError( iRc2 );

  sSql = "DELETE FROM AddressR.DBF WHERE CITY='San Diego'";
  iRc2 = sql.ExecuteNonQuery( sSql );
  iRc += TestMethod( po, "SqlDelete(106)", (xbInt32) iRc2, XB_NO_ERROR );
  if( iRc2 )
    x.DisplayError( iRc2 );

  sSql = "UNDELETE FROM AddressR.DBF WHERE CITY='San Diego'";
  iRc2 = sql.ExecuteNonQuery( sSql );
  iRc += TestMethod( po, "SqlUndelete(107)", (xbInt32) iRc2, XB_NO_ERROR );
  if( iRc2 )
    x.DisplayError( iRc2 );

  sSql = "DELETE FROM AddressR.DBF";
  iRc2 = sql.ExecuteNonQuery( sSql );
  iRc += TestMethod( po, "SqlDelete(108)", (xbInt32) iRc2, XB_NO_ERROR );
  if( iRc2 )
    x.DisplayError( iRc2 );

  sSql = "UNDELETE FROM AddressR.DBF";
  iRc2 = sql.ExecuteNonQuery( sSql );
  iRc += TestMethod( po, "SqlDelete(109)", (xbInt32) iRc2, XB_NO_ERROR );
  if( iRc2 )
    x.DisplayError( iRc2 );

  sSql = "DELETE FROM AddressR.DBF WHERE BAD='EXPRESSION'";
  iRc2 = sql.ExecuteNonQuery( sSql );
  iRc += TestMethod( po, "SqlDelete(110)", (xbInt32) iRc2, XB_INVALID_FIELD_NAME );


  sSql = "DROP TABLE IF EXISTS AddressR.DBF";
  iRc += TestMethod( po, "Drop Table(111)", sql.ExecuteNonQuery( sSql ), XB_NO_ERROR );
  iRc += TestMethod( po, "Drop Table(112)", sql.ExecuteNonQuery( sSql ), XB_NO_ERROR );

*/


  sSql = "DROP TABLE IF EXISTS ZipCode.DBF";
  iRc2 = sql.ExecuteNonQuery( sSql );
  iRc += TestMethod( po, "DropTable(120)", (xbInt32) iRc2, XB_NO_ERROR );
  if( iRc2 )
    x.DisplayError( iRc2 );


  sSql = "CREATE TABLE ZipCode.DBF ( ZIPCODE NUMERIC(9,0), CITY CHAR(30), STATE CHAR(2) )";
  iRc2 = sql.ExecuteNonQuery( sSql );
  iRc += TestMethod( po, "CreateTable(121)", (xbInt32) iRc2, XB_NO_ERROR );
  if( iRc2 )
    x.DisplayError( iRc2 );

  sSql = "INSERT INTO ZipCode.DBF ( ZIPCODE, CITY, STATE ) VALUES ( 75087, 'Rockwall', 'TX' )";
  iRc2 = sql.ExecuteNonQuery( sSql );

  sSql = "INSERT INTO ZipCode.DBF ( ZIPCODE, CITY, STATE ) VALUES ( 75087, 'Rockwall', 'TX' )";
  iRc2 = sql.ExecuteNonQuery( sSql );

  sSql = "INSERT INTO ZipCode.DBF ( ZIPCODE, CITY, STATE ) VALUES ( 44256, 'Medina', 'OH' )";
  iRc2 = sql.ExecuteNonQuery( sSql );

  sSql = "INSERT INTO ZipCode.DBF ( ZIPCODE, CITY, STATE ) VALUES ( 44287, 'Salem', 'OH' )";
  iRc2 = sql.ExecuteNonQuery( sSql );

  sSql = "INSERT INTO ZipCode.DBF ( ZIPCODE, CITY, STATE ) VALUES ( 26554, 'Fairmont', 'WV' )";
  iRc2 = sql.ExecuteNonQuery( sSql );

  sSql = "INSERT INTO ZipCode.DBF ( ZIPCODE, CITY, STATE ) VALUES ( 74743, 'Hugo', 'OK' )";
  iRc2 = sql.ExecuteNonQuery( sSql );

  sSql = "INSERT INTO ZipCode.DBF ( ZIPCODE, CITY, STATE ) VALUES ( 29073, 'Lexington', 'SC' )";
  iRc2 = sql.ExecuteNonQuery( sSql );


/*

  sSql = "CREATE INDEX ZipCode1.NDX ON ZipCode.DBF( ZIPCODE ) ASSOCIATE";
  iRc2 = sql.ExecuteNonQuery( sSql );
  if( iRc2 )
    x.DisplayError( iRc2 );
  iRc += TestMethod( po, "Create Index(122)", (xbInt32) iRc2, XB_NO_ERROR );
*/


  sSql = "CREATE UNIQUE INDEX ZipCode2.NDX ON ZipCode.DBF( ZIPCODE ) ASSOCIATE";
  iRc2 = sql.ExecuteNonQuery( sSql );
  iRc += TestMethod( po, "Create Index(123)", (xbInt32) iRc2, XB_KEY_NOT_UNIQUE );

/*

  // 10/18/24 segfaults
  sSql = "DROP INDEX IF EXISTS ZipCode3.NDX";
  iRc2 = sql.ExecuteNonQuery( sSql );
  iRc += TestMethod( po, "Drop Index(124)", (xbInt32) iRc2, XB_NO_ERROR );
*/


/*
  // 10/18/24 throws an error if the index file already exists
  sSql = "CREATE INDEX ZipCode3.NDX ON ZipCode.DBF( ZIPCODE ) ASSOCIATE";
  iRc2 = sql.ExecuteNonQuery( sSql );
  iRc += TestMethod( po, "Create Index(125)", (xbInt32) iRc2, XB_NO_ERROR );
*/

/*
  sSql = "DROP TABLE IF EXISTS ZipCode.DBF";
  iRc2 = sql.ExecuteNonQuery( sSql );
  iRc += TestMethod( po, "DropTable(125)", (xbInt32) iRc2, XB_NO_ERROR );
  if( iRc2 )
    x.DisplayError( iRc2 );
*/


//  sSql = "INSERT INTO ZipCode.DBF ( ZIPCODE, CITY, STATE ) VALUES ( 75087, 'Rockwall', 'TX' )";
//  iRc2 = sql.ExecuteNonQuery( sSql );


  x.DisplayTableList();




/*
  sSql = "CREATE INDEX ZipCode.NDX ON Addres.DBF( ZIPCODE )";

  iRc2 = sql.ExecuteNonQuery( sSql );
  iRc += TestMethod( po, "CreateTable(130)", (xbInt32) iRc2, XB_NO_ERROR );
  if( iRc2 )
    x.DisplayError( iRc2 );
*/

//  sSql = "DROP TABLE IF EXISTS AddressR.DBF";
//  iRc += TestMethod( po, "Drop Table(140)", sqlQry1.ExecuteQuery( sSql ), XB_NO_ERROR );




  // x.DisplayTableList();

  x.CloseAllTables();

  if( po > 0 || iRc < 0 )
    fprintf( stdout, "Total Errors = %d\n", iRc * -1 );

#ifdef XB_LOGGING_SUPPORT
  sMsg.Sprintf( "Program [%s] terminating with [%d] errors...", av[0], iRc * -1 );
  x.WriteLogMessage( sMsg );
#endif

  return iRc;
}

 