/* xbcrtbl.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2019,2022 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

#include "xbase.h"

#ifdef XB_SQL_SUPPORT

namespace xb{


/***********************************************************************/
xbInt16 xbSql::SqlCreateTable( const xbString &sCmdLine ){


  // std::cout << "CREATE TABLE " << sCmdLine << std::endl;

  // expected format:
  // CREATE TABLE tablename.dbf (Field1 CHAR(10), INTFLD1 INTEGER, ... )

  // supported field types
  //
  //  SQL TYPE      XBASE Field Type
  //  --------      ----------------
  //  SMALLINT      NUMERIC(6,0)
  //  INTEGER       NUMERIC(11,0)
  //  DECIMAL(x,y)  NUMERIC(x+1,y)
  //  NUMERIC(x,y)  NUMERIC(x,y)
  //  FLOAT(x,y)    FLOAT(x,y)
  //  CHAR(n)       CHARACTER(n)
  //  DATE          DATE
  //  VARCHAR       MEMO
  //  LOGICAL       LOGICAL

  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;
  xbString sTableName;
  xbString sFieldName;
  xbString sDataType;
  xbString sAttrib1;
  xbString sAttrib2;
  xbString sLlEntry;
  xbInt16  iType = 0;
  xbInt16  iAttribCnt;  // number of attributes for a given data type
  xbString s;
  xbUInt32 lPos = 0;
  xbUInt32 lSpPos = 0;    // space position
  xbUInt32 lCmPos = 0;    // comma position
  xbUInt32 lLpPos = 0;    // left paren position
  xbLinkList<xbString> ll;
  xbSchema *schema = NULL;
  xbString sMsg;
  xbString sWork;

  try{
    //  retrieve table name
    s.ExtractElement( sCmdLine, '(', 1, 0 );
    sTableName.ExtractElement( s, ' ', 3, 0 );
    sTableName.Trim();

//    std::cout << "Create table - Tablename = [" << sTableName.Str() << "]\n";
//    std::cout << "Cm line = [" << sCmdLine.Str() << "]\n";

    // determine if it already exists
    xbFile f( xbase );
    f.SetFileName( sTableName );
    if( f.FileExists() ){
      iErrorStop = 100;
      iRc = XB_FILE_EXISTS;
      throw iRc;
    }

    // build out table structure with parms from the sql string
    lPos = sCmdLine.Pos( '(' );
    s = sCmdLine;
    s.Ltrunc( lPos );
    s.Trim();

    // remove the last byte, should be a )
    s.Remove( s.Len(), 1 );

    s.Trim();

    xbBool bDone = xbFalse;
    xbInt16 iLoop = 0;
    while( !bDone && iLoop++ < 255 ){
      sFieldName.ExtractElement( s, ' ', 1 , 0 );
      lPos = sFieldName.Len();
      sFieldName.Trim();
      if( sFieldName.Len() > 10 )
        sFieldName.Mid( 1, 10 );   // shrink to 10 buytes if too big

      //std::cout << "field name=[" << sFieldName << "]\n"; 
      s.Ltrunc( lPos + 1 );
      s.Ltrim();
      //std::cout << "remainder after field name removed = [" << s << "]\n";

      // Data type is delimited with either a space, comma or left paren
      lPos = 9999999;
      lSpPos = s.Pos( ' ' );
      lCmPos = s.Pos( ',' );
      lLpPos = s.Pos( '(' );
      if( lSpPos != 0 ) lPos = lSpPos;
      if( lCmPos != 0 && lCmPos < lPos ) lPos = lCmPos;
      if( lLpPos != 0 && lLpPos < lPos ) lPos = lLpPos;

      //sMsg.Sprintf( "SpPos=[%d] CmPos=[%d] LpPos=[%d] lPos=[%d]", lSpPos, lCmPos, lLpPos, lPos );
      //std::cout << sMsg << "\n";

      sDataType.Assign( s, 1, lPos-1 );

      //std::cout << "DataType=[" << sDataType << "]\n";
      if( sDataType == "CHAR" )
        iAttribCnt = 1;
      else if( sDataType == "DECIMAL" || sDataType == "NUMERIC" || sDataType == "FLOAT" )
        iAttribCnt = 2;
      else if( sDataType == "SMALLINT" || sDataType == "INTEGER" || sDataType == "DATE" || sDataType == "VARCHAR" || sDataType == "LOGICAL" )
        iAttribCnt = 0;
      else{
        iErrorStop = 110;
        iRc = XB_INVALID_FIELD_TYPE;
        throw iRc;
      }

      sAttrib1 = "";
      sAttrib2 = "0";

      if( iAttribCnt == 0 ){
        s.Ltrunc( sDataType.Len());

      } else if( iAttribCnt > 0 ){
        lPos = s.Pos( '(' );
        if( lPos <= 0 ){
          iErrorStop = 120;
          iRc = XB_INVALID_FIELD_LEN;
          throw iRc;
        }
        s.Ltrunc( lPos );

        lPos = s.Pos( ')' );
        if( lPos <= 0 ){
          iErrorStop = 130;
          iRc = XB_INVALID_FIELD_LEN;
          throw iRc;
        }
        sWork.Assign( s, 1, lPos - 1);
        sWork.Trim();

        if( iAttribCnt == 1 ){
          sAttrib1 = sWork;
        } else {

          lCmPos = sWork.Pos( ',' );
          if( lCmPos <= 0 ){
            iErrorStop = 140;
            iRc = XB_INVALID_FIELD_LEN;
            throw iRc;
          }

          sAttrib1.Assign( sWork, 1, lCmPos - 1);
          sAttrib1.Trim();

          sWork.Ltrunc( lCmPos );
          sAttrib2 = sWork;
          sAttrib2.Trim();

        }
        s.Ltrunc( lPos );
      }

      s.Ltrim();
      s.ZapLeadingChar( ',' );
      s.Ltrim();

      if( sDataType == "CHAR" ){
        iType = XB_CHAR_FLD;
      } else if( sDataType == "DECIMAL" ){
        xbInt32 lVal = atol( sAttrib1.Str()) + 1;
        sAttrib1.Sprintf( "%d", lVal );
        iType = XB_NUMERIC_FLD;
      } else if( sDataType == "SMALLINT" ){
        sAttrib1 = "6";
        iType = XB_NUMERIC_FLD;
      } else if( sDataType == "INTEGER" ){
        sAttrib1 = "11";
        iType = XB_NUMERIC_FLD;
      } else if( sDataType == "NUMERIC" ){
        iType = XB_NUMERIC_FLD;
      } else if( sDataType == "FLOAT" ) {
        iType = XB_FLOAT_FLD;
      } else if( sDataType == "DATE" ){
        iType = XB_DATE_FLD;
        sAttrib1 = "8";
      } else if( sDataType == "VARCHAR" ){
        iType = XB_MEMO_FLD;
        sAttrib1 = "10";
      } else if( sDataType == "LOGICAL" ){
        iType = XB_LOGICAL_FLD;
        sAttrib1 = "1";
      }
      sLlEntry.Sprintf( "%s,%s,%c,%s,%s", sFieldName.Str(), sDataType.Str(), iType, sAttrib1.Str(), sAttrib2.Str());
      ll.InsertAtEnd( sLlEntry );

      if( s.Len() == 0 )
        bDone = xbTrue;
    }

    schema = (xbSchema *) calloc( ll.GetNodeCnt()+1, sizeof( xbSchema ));
    xbLinkListNode<xbString> * llN = ll.GetHeadNode();
    xbUInt32 ulCnt = ll.GetNodeCnt();

    char *pTrg;
    for( xbUInt32 i = 0; i < ulCnt; i++ ){
      s = llN->GetKey();
      sFieldName.ExtractElement( s, ',', 1 , 0 );
      pTrg = schema[i].cFieldName;
      for( xbUInt32 j = 0; j < sFieldName.Len(); j++ )
        *pTrg++ = sFieldName[j+1];
      sDataType.ExtractElement( s, ',', 3, 0 );
      schema[i].cType = sDataType[1];
      sAttrib1.ExtractElement( s, ',', 4, 0 );
      schema[i].iFieldLen = atoi( sAttrib1.Str());
      sAttrib2.ExtractElement( s, ',', 5, 0 );
      schema[i].iNoOfDecs = atoi( sAttrib2.Str());
      llN = llN->GetNextNode();
    }

    // create the table
    xbDbf * dbf = NULL;
    #ifdef XB_DBF4_SUPPORT
    dbf = new xbDbf4( xbase );
    #elif defined (XB_DBF3_SUPPORT)
    dbf = new xbDbf3( xbase );
    #endif

    if(( iRc = dbf->CreateTable( sTableName, "", schema, 0, XB_MULTI_USER )) != XB_NO_ERROR ){
      iErrorStop = 150;
      throw iRc;
    }
    free( schema );

    ll.Clear();

  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    if( sFieldName.Len() > 0 )
      sMsg.Sprintf( "xbSql::SqlCreateTbl() Exception Caught. Error Stop = [%d] rc = [%d] table = [%s] field = [%s]", iErrorStop, iRc, sTableName.Str(), sFieldName.Str() );
    else
      sMsg.Sprintf( "xbSql::SqlCreateTbl() Exception Caught. Error Stop = [%d] rc = [%d] table = [%s]", iErrorStop, iRc, sTableName.Str() );

    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
    if( schema ) free( schema );
  }

  return iRc;
}

/***********************************************************************/
}              /* namespace       */
#endif         /*  XB_SQL_SUPPORT */

