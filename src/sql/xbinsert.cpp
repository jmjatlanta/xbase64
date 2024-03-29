/* xbinsert.cpp

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
xbInt16 xbSql::SqlInsert( const xbString &sCmdLine ){

  // expected format:
  //  INSERT INTO tablename (field1, field2, field3,...) VALUES ( 'charval', numval, 'what is the correct odbc date format to use? CCYYMMDD');

  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;
  xbString sTableName;
  xbDbf * dbf = NULL;

  xbString sWork1;
  xbString sFieldList;
  xbString sDataList;
  xbString sFieldName;
  xbString sFieldData;

  // queue the memo data to post after the append occurs
  // dbase does not support usage of memo fields with insert commands
  xbLinkList <xbInt16>  llMemoFieldNos;
  xbLinkList <xbString> llMemoFieldData;

  try{
    //  retrieve table name
    sTableName.ExtractElement( sCmdLine, ' ', 3, 0 );
    sTableName.Trim();

    //  if not open, attempt to open it
    dbf = xbase->GetDbfPtr( sTableName );

    if( !dbf ){
      if(( iRc = xbase->OpenHighestVersion( sTableName, "", &dbf )) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      }
    }

    if( !dbf ){
      iErrorStop = 110;
      iRc = XB_FILE_NOT_FOUND;
      throw iRc;
    }

    //  blank the record buffer
    dbf->BlankRecord();

    sWork1.ExtractElement( sCmdLine, ')', 1, 0 );
    sFieldList.ExtractElement( sWork1, '(', 2, 0 );

    sDataList.ExtractElement( sCmdLine, '(', 3, 0 );
    sDataList.Trim();
    sDataList.ZapTrailingChar( ')' );

    xbUInt32 iFldCnt = sFieldList.CountChar( ',' );
    xbUInt32 iDataCnt = sDataList.CountChar( ',', 1 );

    // verify there are the same count in the field list and values list
    if( iFldCnt != iDataCnt ){
      iErrorStop = 120;
      iRc = XB_SYNTAX_ERROR;
      throw iRc;
    }

    iFldCnt++;
    xbInt16 iFldNo = -1;
    char cFldType = 0x00;
    for( xbUInt32 i = 1; i <= iFldCnt; i++ ){
      sFieldName.ExtractElement( sFieldList, ',', i, 0 );
      sFieldName.Trim();
      if(( iRc = dbf->GetFieldNo( sFieldName, iFldNo )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }
      if(( iRc = dbf->GetFieldType( iFldNo, cFldType )) != XB_NO_ERROR ){
        iErrorStop = 140;
        throw iRc;
      }

      // get the field data here
      sFieldData.ExtractElement( sDataList, ',', i, 1 );
      sFieldData.Trim();

      // remove beginning and ending quotes
      if(( sFieldData[1] == '\'' && sFieldData[sFieldData.Len()] == '\'') || (sFieldData[1] == '"' && sFieldData[sFieldData.Len()] == '"' )){
        sFieldData.Remove( sFieldData.Len(), 1 );
        sFieldData.Remove( 1, 1 );
      }

      switch( cFldType ){
        case 'C':
        case 'N':
        case 'L':
          if(( iRc = dbf->PutField( iFldNo, sFieldData )) != XB_NO_ERROR ){
            iErrorStop = 150;
            throw iRc;
          }
          break;

        case 'D':
          // assumes input date format of yyyy-mm-dd
          if( sFieldData.Len() != 10 || sFieldData[5] != '-' || sFieldData[8] != '-' ){
            iErrorStop = 160;
            iRc = XB_INVALID_DATA;
            throw iRc;
          }
          sWork1 = sFieldData;
          sWork1.Remove( 8, 1 );
          sWork1.Remove( 5, 1 );
          if(( iRc = dbf->PutField( iFldNo, sWork1 )) != XB_NO_ERROR ){
            iErrorStop = 170;
            throw iRc;
          }
          break;

        case 'M':
          llMemoFieldNos.InsertAtFront( iFldNo );
          llMemoFieldData.InsertAtFront( sFieldData );
          break;

        default:
          iErrorStop= 180;
          iRc = XB_INVALID_FIELD_TYPE;
          throw iRc;
      }
    }

    if(( iRc = dbf->AppendRecord()) != XB_NO_ERROR ){
      iErrorStop = 190;
      throw iRc;
    }

    // Add any memo fields
    xbLinkListNode<xbInt16>  * llN = llMemoFieldNos.GetHeadNode();
    xbLinkListNode<xbString> * llD = llMemoFieldData.GetHeadNode();
    xbUInt32 ulCnt = llMemoFieldNos.GetNodeCnt();
    for( xbUInt32 i = 0; i < ulCnt; i++ ){
      iFldNo = llN->GetKey();
      sFieldData = llD->GetKey();
      if(( iRc = dbf->UpdateMemoField( iFldNo, sFieldData )) != XB_NO_ERROR ){
        iErrorStop = 200;
        throw iRc;
       }
      llN = llN->GetNextNode();
      llD = llD->GetNextNode();
    }

    if(( iRc = dbf->Commit()) != XB_NO_ERROR ){
      iErrorStop = 210;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    xbase->WriteLogMessage( sCmdLine );
    sMsg.Sprintf( "xbSql::SqlInsert() Exception Caught. Error Stop = [%d] rc = [%d] table = [%s] field = [%s] data = [%s]", iErrorStop, iRc, sTableName.Str(), sFieldName.Str(), sFieldData.Str() );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
    if( dbf )
      dbf->Abort();
  }
  return iRc;

}

/***********************************************************************/
}              /* namespace       */
#endif         /*  XB_SQL_SUPPORT */

