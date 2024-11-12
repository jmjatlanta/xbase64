/* xbslttbl.cpp

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
xbInt16 xbSql::SqlAlterTable( const xbString &sCmdLine ){

  // expected format:
  //  ALTER TABLE tablename.DBF RENAME TO newtablename.DBF


  xbInt16  iRc = 0;
  xbInt16  iErrorStop = 0;
  xbString sTableName = "";
  xbUInt32 ulPos = 0;
  xbString sCmd = sCmdLine;
  xbString sNode = "";

  xbString sSrcTbl;
  xbString sTrgTbl;
  xbDbf * dbf = NULL;

  try{

//    std::cout << "xbSql::SqlAlterTable( " << sCmdLine.Str() << " )\n";

    // drop off the first node "DROP"
    ulPos = sCmd.Pos( ' ' );
    sCmd.Ltrunc( ulPos );
    sCmd.Ltrim();

    // drop off the second node "TABLE"
    ulPos = sCmd.Pos( ' ' );
    sCmd.Ltrunc( ulPos );
    sCmd.Ltrim();

    // pull off the source table
    sSrcTbl.ExtractElement( sCmd, ' ', 1, 0 );
    sSrcTbl.Trim();

    // pull off the action
    sNode.ExtractElement( sCmd, ' ', 2, 0 );
    sNode.Trim();
    sNode.ToUpperCase();
    if( sNode != "RENAME" ){
      iErrorStop = 100;
      iRc = XB_SYNTAX_ERROR;
      throw iRc;
    }

    // pull off "TO"
    sNode.ExtractElement( sCmd, ' ', 3, 0 );
    sNode.Trim();
    sNode.ToUpperCase();
    if( sNode != "TO" ){
      iErrorStop = 110;
      iRc = XB_SYNTAX_ERROR;
      throw iRc;
    }

    sTrgTbl.ExtractElement( sCmd, ' ', 4, 0 );
    sTrgTbl.Trim();

//    std::cout << "source table name = " << sSrcTbl.Str() << "\n";
//    std::cout << "target table name = " << sTrgTbl.Str() << "\n";

    //  if not open, attempt to open it
    dbf = xbase->GetDbfPtr( sSrcTbl );

    if( !dbf ){
      //if(( iRc = xbase->OpenHighestVersion( sSrcTbl, "", &dbf )) != XB_NO_ERROR ){
      if(( iRc = xbase->Open( sSrcTbl, "", &dbf )) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }
    }
    if( !dbf ){
      iErrorStop = 130;
      iRc = XB_DBF_FILE_NOT_OPEN;
      throw iRc;
    }

    if(( iRc = dbf->Rename( sTrgTbl )) != XB_NO_ERROR ){
      iErrorStop = 140;
      throw iRc;
    }

  }

  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbSql::SqlAlterTable() Exception Caught. Error Stop = [%d] rc = [%d] table = [%s]", iErrorStop, iRc, sTableName.Str() );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }

  return iRc;
}

/***********************************************************************/
}              /* namespace       */
#endif         /*  XB_SQL_SUPPORT */

