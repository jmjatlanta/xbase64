/* xbsql.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023,2024 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

#include "xbase.h"

#ifdef XB_SQL_SUPPORT

namespace xb{

/***********************************************************************/
xbSql::xbSql( xbXBase *x ){
   xbase = x;
   xbString sIxType;

   #ifdef XB_MDX_SUPPORT
   sIxType = "MDX";
   #endif

   #ifdef XB_NDX_SUPPORT
   if( sIxType == "" )
     sIxType = "NDX";
   #endif

   if( sIxType != "" )
     uda.AddTokenForKey( "IXTYPE", sIxType );

}
/***********************************************************************/
xbSql::~xbSql(){
}

/***********************************************************************/
xbInt16 xbSql::ExecuteNonQuery( const xbString &sCmdLine ) {

//  std::cout << "\n\n\nExecute() " << sCmdLine.Str() << "\n";

  xbInt16 iRc = XB_NO_ERROR;
  xbString sCmd = sCmdLine;

  sCmd.Trim();
  xbString sNode1;
  sNode1.ExtractElement( sCmd.Str(), ' ', 1, 0 );
  sNode1.ToUpperCase();
  sNode1.Trim();

//  std::cout << "node 1 = " << sNode1.Str() << std::endl;

  if( sNode1 == "ALTER" ){
    xbString sNode2;
    sNode2.ExtractElement( sCmd.Str(), ' ', 2, 0 );
    sNode2.ToUpperCase();
    sNode2.Trim();

    if( sNode2 == "TABLE" )
      iRc = SqlAlterTable( sCmd );
    else
      iRc = XB_INVALID_FUNCTION;



  } else if( sNode1 == "CREATE" ){

    xbString sNode2;
    sNode2.ExtractElement( sCmd.Str(), ' ', 2, 0 );
    sNode2.ToUpperCase();
    sNode2.Trim();
    if( sNode2 == "TABLE" )
      iRc = SqlCreateTable( sCmd );

    #ifdef XB_INDEX_SUPPORT
    else if( sNode2 == "INDEX" || sNode2 == "UNIQUE" )
      iRc = SqlCreateIndex( sCmd );
    #endif  // XB_INDEX_SUPPORT

    else
      iRc = XB_INVALID_FUNCTION;

  } else if( sNode1 == "DROP" ){

    xbString sNode2;
    sNode2.ExtractElement( sCmd.Str(), ' ', 2, 0 );
    sNode2.ToUpperCase();
    sNode2.Trim();

    if( sNode2 == "TABLE" )
      iRc = SqlDropTable( sCmd );

    #ifdef XB_INDEX_SUPPORT
    else if( sNode2 == "INDEX" )
      iRc = SqlDropIndex( sCmd );
    #endif  // XB_INDEX_SUPPORT

    else
      iRc = XB_INVALID_FUNCTION;

  } else if( sNode1 == "DELETE" || sNode1 == "UNDELETE" ){

    iRc = SqlDelete( sCmd );


  } else if( sNode1 == "HELP" )
    SqlHelp();

  else if( sNode1 == "INSERT" ){
    iRc = SqlInsert( sCmd );

  // else if( sNode1 == "UPDATE" )
  //    iRc = SqlUpdate( sCmd );
  }

  else if( sNode1 == "USE" )
    iRc = SqlUse( sCmd );

  else if( sNode1 == "SET" )
    iRc = SqlSet( sCmd );

  else
    return XB_INVALID_FUNCTION;

  return iRc;
}

/***********************************************************************/
xbXBase *xbSql::GetXbasePtr() const {
  return xbase;
}


/***********************************************************************/
void xbSql::SqlHelp() const {
  std::cout << "************************"                                                                       << std::endl;
  std::cout << "Valid XBase SQL commands"                                                          << std::endl << std::endl;
  std::cout << "HELP"                                                                                           << std::endl;

  std::cout << "ALTER TABLE tablename.DBF RENAME TO newtablename.DBF"                                           << std::endl;
  std::cout << "CREATE INDEX ixname.NDX ON tablename.dbf ( EXPRESSION ) [ASSOCIATE]"                            << std::endl;
  std::cout << "CREATE [UNIQUE] INDEX tagname ON tablename.dbf ( EXPRESSION ) [DESC] [FILTER  .NOT. DELETED()]" << std::endl;
  std::cout << "CREATE TABLE tablename.DBF (Field1 CHAR(10), INTFLD1 INTEGER, ... )"                            << std::endl;
  std::cout << "DELETE FROM tablename.DBF [WHERE expression]"                                                   << std::endl;
  std::cout << "DROP INDEX [IF EXISTS] ixname.NDX ON tablename.DBF"                                             << std::endl;
  std::cout << "DROP TABLE [IF EXISTS] tablename.DBF"                                                           << std::endl;
  std::cout << "INSERT INTO tablename (field1, field2, field3,...) VALUES ( 'charval', numval, {MM/DD/YY})"     << std::endl;
  std::cout << "SELECT FIELD1,FIELD2...  FROM TABLE.DBF [WHERE expression] [ORDER BY TAG TAGNAME"               << std::endl;
  std::cout << "SET ATTRIBUTE = DATAVALUE"                                                                      << std::endl;
  std::cout << "SET ATTRIBUTE = ^    (to delete an entry)"                                                      << std::endl;
  std::cout << "USE /dir/to/dbf/tables"                                                                         << std::endl;
  std::cout << "CREATE TABLE"                                                         << std::endl << std::endl << std::endl;
}

/***********************************************************************/
xbInt16 xbSql::SqlUse( const xbString &sCmdLine ){

  xbString sNode2;
  sNode2.ExtractElement( sCmdLine.Str(), ' ', 2, 0 );
  sNode2.Trim();
  xbase->SetDataDirectory( sNode2 );
//  std::cout << "USE " << sNode2 << std::endl;
  return XB_NO_ERROR;
}

/***********************************************************************/
}              /* namespace       */
#endif         /*  XB_SQL_SUPPORT */

