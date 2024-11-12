/* xbdrptbl.cpp

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
xbInt16 xbSql::SqlDropTable( const xbString &sCmdLine ){

  // std::cout << "DROP TABLE [" << sCmdLine << "]\n";
  // expected format:
  //  DROP TABLE [IF EXISTS] tablename.dbf


  xbInt16  iRc = 0;
  xbInt16  iErrorStop = 0;
  xbString sTableName = "";
  xbUInt32 ulPos = 0;
  xbString sCmd = sCmdLine;
  xbString sNode = "";
  xbBool   bIfExists = xbFalse;
  xbDbf * dbf = NULL;

  try{

    // drop off the first node "DROP"
    ulPos = sCmd.Pos( ' ' );
    sCmd.Ltrunc( ulPos );
    sCmd.Ltrim();

    // drop off the second node "TABLE"
    ulPos = sCmd.Pos( ' ' );
    sCmd.Ltrunc( ulPos );
    sCmd.Ltrim();

    sNode.ExtractElement( sCmd, ' ', 1, 0 );
    sNode.Trim();
    sNode.ToUpperCase();

    if( sNode == "IF" ){
      ulPos = sCmd.Pos( ' ' );
      sCmd.Ltrunc( ulPos );
      sCmd.Ltrim();
      sNode.ExtractElement( sCmd, ' ', 1, 0 );
      sNode.Trim();
      sNode.ToUpperCase();
      if( sNode != "EXISTS" ){
        iErrorStop = 100;
        iRc = XB_SYNTAX_ERROR;
        throw iRc;
      } else {
        ulPos = sCmd.Pos( ' ' );
        sCmd.Ltrunc( ulPos );
        sCmd.Ltrim();
        bIfExists = xbTrue;
      }
    }

    // get the table name
    sTableName.ExtractElement( sCmd, ' ', 1, 0 );
    sTableName.Trim();

    xbFile fDbf( xbase );
    fDbf.SetFileName( sTableName );

    if( bIfExists && ! fDbf.FileExists())
      return XB_NO_ERROR;

    //  if not open, attempt to open it
    dbf = xbase->GetDbfPtr( fDbf.GetFqFileName());

    if( !dbf ){

      //iRc = xbase->OpenHighestVersion( sTableName, "", &dbf );

      iRc = xbase->Open( sTableName, "", &dbf );
      if( iRc != XB_NO_ERROR ){
        if( iRc == XB_FILE_NOT_FOUND && bIfExists ){
          return XB_NO_ERROR;
        } else {
          iErrorStop = 110;
          throw iRc;
        }
      }
    }
    if( dbf == NULL ){
      iErrorStop = 120;
      iRc = XB_OPEN_ERROR;
      throw iRc;
    }

    if(( dbf->DeleteTable()) != XB_NO_ERROR ){
      iErrorStop = 130;
      throw iRc;
    }

    delete dbf;
    // dbf = NULL;
  }

  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbSql::SqlDropTable() Exception Caught. Error Stop = [%d] rc = [%d] table = [%s]", iErrorStop, iRc, sTableName.Str() );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }

  return iRc;
}

/***********************************************************************/
}              /* namespace       */
#endif         /*  XB_SQL_SUPPORT */

