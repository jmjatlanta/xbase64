/* xbdrpix.cpp

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

#ifdef XB_INDEX_SUPPORT
/***********************************************************************/
xbInt16 xbSql::SqlDropIndex( const xbString &sCmdLine ){

  // std::cout << "DROP INDEX [" << sCmdLine << "]\n";

  // expected format:
  //  DROP INDEX [IF EXISTS] ixname.ndx ON tablename.dbf


  xbInt16  iRc = 0;
  xbInt16  iErrorStop = 0;
  xbString sTableName;
  xbString sIxName;
  xbUInt32 ulPos;
  xbString sCmd = sCmdLine;
  xbString sNode;
  xbBool   bIfExists = xbFalse;
  xbDbf * dbf = NULL;

  try{

    // drop off the first node "DROP"
    ulPos = sCmd.Pos( ' ' );
    sCmd.Ltrunc( ulPos );
    sCmd.Ltrim();

    // drop off the second node "INDEX"
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

    // get the index name
    sIxName.ExtractElement( sCmd, ' ', 1, 0 );
    sIxName.Trim();

    ulPos = sCmd.Pos( ' ' );
    sCmd.Ltrunc( ulPos );
    sCmd.Ltrim();

    // go past "ON"
    ulPos = sCmd.Pos( ' ' );
    sCmd.Ltrunc( ulPos );
    sCmd.Ltrim();

    sTableName = sCmd;
    sTableName.Trim();

    xbFile fDbf( xbase );
    fDbf.SetFileName( sTableName );

    //  if not open, attempt to open it
    dbf = xbase->GetDbfPtr( fDbf.GetFqFileName());
    if( !dbf ){
      //if(( iRc = xbase->OpenHighestVersion( sTableName, "", &dbf )) != XB_NO_ERROR ){
      if(( iRc = xbase->Open( sTableName, "", &dbf )) != XB_NO_ERROR ){
        iErrorStop = 110;
        throw iRc;
      }
    }
    if( dbf == NULL ){
      iErrorStop = 120;
      iRc = XB_FILE_NOT_FOUND;
      throw iRc;
    }

    #ifdef XB_LOCKING_SUPPORT
    if(( iRc = dbf->LockTable( XB_LOCK )) != XB_NO_ERROR ){
      iErrorStop = 130;
      throw iRc;
    }
    #endif  // XB_LOCKING_SUPPORT

    ulPos = sIxName.Pos( ".NDX" );
    if( ulPos > 0 ){
      iRc = dbf->DeleteTag( "NDX", sIxName );
      if( iRc == XB_FILE_NOT_FOUND && !bIfExists ){
        iErrorStop = 140;
        throw( iRc );
      } else if( iRc != XB_NO_ERROR ){
        iErrorStop = 150;
        throw iRc;
      }
    } else {  // assuming MDX tag
      iRc = dbf->DeleteTag( "MDX", sIxName );
      if( iRc == XB_FILE_NOT_FOUND && !bIfExists ){
        iErrorStop = 160;
        throw( iRc );
      } else if( iRc != XB_NO_ERROR ){
        iErrorStop = 170;
        throw iRc;
      }
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbSql::SqlDropIndex() Exception Caught. Error Stop = [%d] rc = [%d] table = [%s]", iErrorStop, iRc, sTableName.Str() );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }

  #ifdef XB_LOCKING_SUPPORT
  dbf->LockTable( XB_UNLOCK );
  #endif  // XB_LOCKING_SUPPORT

  return iRc;
}
#endif  // XB_INDEX_SUPPORT
/***********************************************************************/
}              /* namespace       */
#endif         /*  XB_SQL_SUPPORT */

