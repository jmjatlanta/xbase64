/* xbcrix.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

#include "xbase.h"

#ifdef XB_SQL_SUPPORT

namespace xb{


/***********************************************************************/
#ifdef XB_INDEX_SUPPORT
xbInt16 xbSql::SqlCreateIndex( const xbString &sCmdLine ){

  //  std::cout << "CREATE INDEX " << sCmdLine << std::endl;

  // expected format to create an Dbase 3, NDX index:
  //  CREATE INDEX ixname.ndx ON tablename.dbf ( EXPRESSION ) [ASSOCIATE]

  // expected format to create an Dbase 4, tag on an MDX index:
  //  CREATE [UNIQUE] INDEX tagname ON tablename.dbf ( EXPRESSION ) [DESC] [FILTER  .NOT. DELETED()]

  // The ASSOCIATE parameter is specific to Xbase64 library, it is used to associate
  //  a non production (NDX) index file to a dbf file so it will be automatically
  //  opened with the dbf file whenever the dbf file is opened by the xbase64 routines.

  // The [ASSOCIATE] parameter is not used with MDX production indices

  // This method first looks for ".NDX" in the file name to determine if an NDX
  //  index should be created.
  //  if .NDX is not in the filename, it looks in the uda for "IXTYPE" for either 
  //  NDX or MDX to detmermine the index type to create
  //  if IXTYPE not found, create an MDX tag

  // The optional DESC parameter defines an entire index key as descending.  This is
  //  different than other SQL implementations where specific fields can be descending.

  // The optional FILTER parameter is specific to the XBASE64 library, is it used to 
  //  assign a filter to a tag in an MDX style index.  Everything to the right of 
  //  the keyword FILTER is considered part of the filter.

  // The original DBASE indices used to '+' to create an index on more than one field 
  //   ie:  FIELD1+FIELD2+FIELD3
  //  SQL uses commas: ie:   FIELD1, FIELD2, FIELD3
  //  The Xbase library supports either '+' or ',' when creating mutli field indices.

  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;
  xbString sTableName;
  xbString sIxName;
  xbString sIxType;
  xbUInt32 ulPos;
  xbString sCmd = sCmdLine;
  xbString sNode;
  xbBool bUnique = xbFalse;
  xbDbf * dbf = NULL;
  xbBool bTableLocked = xbFalse;

  try{

    // drop off the first node
    ulPos = sCmd.Pos( ' ' );
    sCmd.Ltrunc( ulPos );
    sCmd.Ltrim();

    sNode.ExtractElement( sCmd, ' ', 1, 0 );
    sNode.ToUpperCase();

    if( sNode == "UNIQUE" ){
      //std::cout << "unique ix\n";
      bUnique = xbTrue;
      ulPos = sCmd.Pos( ' ' );
      sCmd.Ltrunc( ulPos );
      sCmd.Ltrim();
    }

    // go past the index keyword
    ulPos = sCmd.Pos( ' ' );
    sCmd.Ltrunc( ulPos );
    sCmd.Ltrim();

    // pull the index name off the cmd line
    sIxName.ExtractElement( sCmd, ' ', 1, 0 );

    #ifdef XB_NDX_SUPPORT
    xbString sTemp = sIxName;
    sTemp.ToUpperCase();
    ulPos = sTemp.Pos( ".NDX" );
    if(ulPos == (sTemp.Len() - 3) )
      sIxType = "NDX";
    #endif // XB_NDX_SUPPORT

    if( sIxType == "" ){
      if(( iRc = uda.GetTokenForKey( "IXTYPE", sIxType )) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      }
    }

    #ifdef XB_NDX_SUPPORT
    if( sIxType == "NDX" ){
      xbFile f( xbase );
      f.SetFileName( sIxName );
      if( f.FileExists()){

        iErrorStop = 110;
        iRc = XB_FILE_EXISTS;
        throw iRc;
      }
    }
    #endif // XB_NDX_SUPPORT

    // skip past index name
    ulPos = sCmd.Pos( ' ' );
    sCmd.Ltrunc( ulPos );
    sCmd.Ltrim();

    // skip past "ON"
    ulPos = sCmd.Pos( ' ' );
    sCmd.Ltrunc( ulPos );
    sCmd.Ltrim();

    // get the table name
    ulPos = sCmd.Pos( '(' );
    sTableName.ExtractElement( sCmd, '(', 1, 0 );
    sTableName.Trim();

    xbFile fDbf( xbase );
    fDbf.SetFileName( sTableName );

    //  if not open, attempt to open it
    dbf = xbase->GetDbfPtr( fDbf.GetFqFileName());

    if( !dbf ){
      if(( iRc = xbase->OpenHighestVersion( sTableName, "", &dbf )) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }
    }

    if( dbf == NULL ){
      iErrorStop = 130;
      iRc = XB_FILE_NOT_FOUND;
      throw iRc;
    }
    sCmd.Ltrunc( ulPos );

    //ulPos = sCmd.GetLastPos( ')' );
    xbString sKeyExpression;
    xbBool   bDone = xbFalse;
    xbUInt32 lPos = 1;
    xbInt16  iParenCtr = 0;

    while( !bDone && lPos < sCmd.Len()){
      if( sCmd[lPos] == '(' ){
        iParenCtr++;
        sKeyExpression.Append( sCmd[lPos] );
      } else if( sCmd[lPos] == ')' ){
        if( iParenCtr > 0 ){
          iParenCtr--;
          sKeyExpression.Append( sCmd[lPos] );
        } else {
          bDone = xbTrue;
        }
      } else if( sCmd[lPos] == ',' && iParenCtr == 0 ){
        sKeyExpression.Append( '+' );
      } else if( sCmd[lPos] != ' ' ){
        sKeyExpression.Append( sCmd[lPos] );
      }
      lPos++;
    }

    // std::cout << "Key Expression =[" << sKeyExpression << "]\n";
    sCmd.Ltrunc( lPos );
    sCmd.Trim();

    xbBool bDesc = xbFalse;
    // std::cout << "sCmd - looking for DESC [" << sCmd << "]\n";
    if( sCmd.Len() > 4 ){
      sNode = sCmd;
      sNode.ToUpperCase();
      ulPos = sNode.Pos( "DESC" );
      if( ulPos > 0 ){
        bDesc = xbTrue;
        sCmd.Ltrunc( 4 );
        sCmd.Trim();
       std::cout << "Descending\n";
      }
    }

    // std::cout << "sCmd - looking for FILTER stuff [" << sCmd << "]\n";
    xbString sFilter;
    if( sCmd.Len() > 6 ){
      sNode = sCmd;
      sNode.ToUpperCase();
      ulPos = sNode.Pos( "FILTER" );
      if( ulPos > 0 ){
        sFilter = sCmd;
        sFilter.Ltrunc( ulPos + 6 );
        sFilter.Trim();
      }
    }
    //  std::cout << "sCmd - FILTER =  [" << sFilter << "]\n";

    #ifdef XB_LOCKING_SUPPORT
    if(( iRc = dbf->LockTable( XB_LOCK )) != XB_NO_ERROR ){
      iErrorStop = 140;
      throw iRc;
    } else {
      bTableLocked = xbTrue;
    }
    #endif  // XB_LOCKING_SUPPORT

    xbIx *pIx;
    void *vpTag;

    if(( iRc = dbf->CreateTag( sIxType, sIxName, sKeyExpression, sFilter, bDesc, bUnique, xbFalse, &pIx, &vpTag )) != XB_NO_ERROR ){
      iErrorStop = 150;
      throw iRc;
    }

    #ifdef XB_NDX_SUPPORT
    xbBool bAssociate = xbFalse;
    if( sIxType == "NDX"){
      sCmd.Ltrunc( ulPos );
      sCmd.Trim();
      if( sCmd.Len() > 0 ){
        sCmd.ToUpperCase();
        if( sCmd.Pos( "ASSOCIATE" )){
          bAssociate = xbTrue;
          if(( iRc = dbf->AssociateIndex( "NDX", sIxName, 0 )) != XB_NO_ERROR ){
            iErrorStop = 160;
            throw iRc;
          }
        }
      }
    }
    #endif // XB_NDX_SUPPORT

    iRc = dbf->Reindex( 2, &pIx, &vpTag );

    #ifdef XB_NDX_SUPPORT
    if( iRc != XB_NO_ERROR && sIxType == "NDX" && bAssociate ){
      xbInt16 iRc2;
      if(( iRc2 = dbf->AssociateIndex( "NDX", sIxName, 1 )) != XB_NO_ERROR ){
        iErrorStop = 180;
        throw iRc2;
      }
      iErrorStop = 190;
      throw iRc;
    }
    #endif  // XB_NDX_SUPPORT

    if( iRc != XB_NO_ERROR ){
      iErrorStop = 200;
      throw iRc;
    }


    #ifdef XB_LOCKING_SUPPORT
    if( bTableLocked ){
      if(( iRc = dbf->LockTable( XB_UNLOCK )) != XB_NO_ERROR ){
        iErrorStop = 210;
        throw iRc;
      } else {
        bTableLocked = xbFalse;
      }
    }
    #endif  // XB_LOCKING_SUPPORT

  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbSql::SqlCreateIndex() Exception Caught. Error Stop = [%d] rc = [%d] table = [%s]", iErrorStop, iRc, sTableName.Str() );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  #ifdef XB_LOCKING_SUPPORT
  if( bTableLocked && dbf )
    dbf->LockTable( XB_UNLOCK );
  #endif  // XB_LOCKING_SUPPORT

  return iRc;
}
#endif  // XB_INDEX_SUPPORT
/***********************************************************************/
}              /* namespace       */
#endif         /*  XB_SQL_SUPPORT */

