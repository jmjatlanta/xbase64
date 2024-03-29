/* xbdelete.cpp

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
xbInt16 xbSql::SqlDelete( const xbString &sCmdLine ){

  // expected format:
  //  DELETE FROM tablename.DBF [WHERE expression]


  xbInt16  iRc = 0;
  xbInt16  iRc2 = 0;
  xbInt16  iErrorStop = 0;
  xbString sTableName = "";
  xbUInt32 ulPos = 0;
  xbString sCmd = sCmdLine;
  xbString sNode = "";
  xbString sFilter;
  xbInt16  iDelOpt = 0;

  xbString sTable;
  xbDbf * dbf = NULL;

  try{

//    std::cout << "xbSql::SqlDelete( " << sCmdLine.Str() << " )\n";

    sNode.ExtractElement( sCmd, ' ', 1, 0 );
    sNode.Trim();
    sNode.ToUpperCase();
    if( sNode == "UNDELETE" )
      iDelOpt = 1;

    // pull off the action
    sNode.ExtractElement( sCmd, ' ', 2, 0 );
    sNode.Trim();
    sNode.ToUpperCase();
    if( sNode != "FROM" ){
      iErrorStop = 100;
      iRc = XB_SYNTAX_ERROR;
      throw iRc;
    }

    // pull off the table name
    sTable.ExtractElement( sCmd, ' ', 3, 0 );
    sTable.Trim();

    // pull off the "WHERE" statement if it exists
    sNode.ExtractElement( sCmd, ' ', 4, 0 );
    sNode.Trim();
    sNode.ToUpperCase();
    if( sNode == "WHERE" ){
      ulPos = sCmd.Pos( "WHERE" );
      sFilter = sCmd;
      sFilter.Ltrunc( ulPos + 5 );
    }

    //  if not open, attempt to open it
    dbf = xbase->GetDbfPtr( sTable );
    if( !dbf ){
      if(( iRc = xbase->OpenHighestVersion( sTable, "", &dbf )) != XB_NO_ERROR ){
        iErrorStop = 110;
        throw iRc;
      }
    }
    if( !dbf ){
      iErrorStop = 120;
      iRc = XB_DBF_FILE_NOT_OPEN;
      throw iRc;
    }

    if( sFilter == "" ){
      if(( iRc = dbf->DeleteAll( iDelOpt )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }
    } else {

      xbFilter f( dbf );
      if(( iRc = f.Set( sFilter )) != XB_NO_ERROR ){
        iErrorStop = 150;
        throw iRc;
      }

      iRc2 = f.GetFirstRecord( XB_ALL_RECS );
      while( iRc2 == XB_NO_ERROR ){
        if( iDelOpt == 0 ){
          if( !dbf->RecordDeleted()){
            if(( iRc = dbf->DeleteRecord()) != XB_NO_ERROR ){
              iErrorStop = 160;
              throw iRc;
            }
            if(( iRc = dbf->Commit()) != XB_NO_ERROR ){
              iErrorStop = 170;
              throw iRc;
            }
          }

        } else { // undelete
          if( dbf->RecordDeleted()){
            if(( iRc = dbf->UndeleteRecord()) != XB_NO_ERROR ){
              iErrorStop = 180;
              throw iRc;
            }
            if(( iRc = dbf->Commit()) != XB_NO_ERROR ){
              iErrorStop = 190;
              throw iRc;
            }
          }
        }
        iRc2 = f.GetNextRecord();
      }
    }
  }

  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbSql::SqlDelete() Exception Caught. Error Stop = [%d] rc = [%d] table = [%s]", iErrorStop, iRc, sTableName.Str() );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }

  return iRc;
}

/***********************************************************************/
}              /* namespace       */
#endif         /*  XB_SQL_SUPPORT */

