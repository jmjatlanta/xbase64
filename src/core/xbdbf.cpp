/* xbdbf.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023,2024 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

#include "xbase.h"


namespace xb{

/************************************************************************/
xbDbf::xbDbf( xbXBase * x ) : xbFile( x ){
  xbase             = x;
  SchemaPtr         = NULL;
  RecBuf            = NULL;
  RecBuf2           = NULL;

  #ifdef XB_BLOCKREAD_SUPPORT
  pRb               = NULL;
  bBlockReadEnabled = xbFalse;              // batch read switch, if xbTrue, then ON
  #endif // XB_BLOCKREAD_SUPPORT

  InitVars();
}
/************************************************************************/
void xbDbf::InitVars()
{
  iNoOfFields      = 0;
  iDbfStatus       = XB_CLOSED;
  ulCurRec         = 0L;
  cVersion         = 0x00;
  cUpdateYY        = 0x00;
  cUpdateMM        = 0x00;
  cUpdateDD        = 0x00;
  ulNoOfRecs       = 0L;
  uiHeaderLen      = 0x00;
  uiRecordLen      = 0x00;
  cTransactionFlag = 0x00;
  cEncryptionFlag  = 0x00;
  cIndexFlag       = 0x00;
  cLangDriver      = 0x00;
  iFileVersion     = 0;            /* Xbase64 file version */
  iAutoCommit      = -1;
  iMultiUser       = xbSysDflt;

  SetFileName  ( "" );
  sAlias.Set   ( "" ); 
  SetDirectory ( xbase->GetDataDirectory());

  #ifdef XB_LOCKING_SUPPORT
  iLockFlavor      = -1;
  bTableLocked     = xbFalse;
  bHeaderLocked    = xbFalse;
  ulAppendLocked   = 0;
  // SetAutoLock( -1 );
  lloRecLocks.SetDupKeys( xbFalse );
  #endif // XB_LOCKING_SUPPORT

  #ifdef XB_INDEX_SUPPORT
  ixList        = NULL;
  pCurIx        = NULL;
  vpCurIxTag    = NULL;
  sCurIxType    = "";
  ClearTagList();
  #endif // XB_INDEX_SUPPORT

  #ifdef XB_MEMO_SUPPORT
  Memo          = NULL;
  #endif // XB_MEMO_SUPPORT

  #ifdef XB_INF_SUPPORT
  llInfData.Clear();
  #endif   // XB_INF_SUPPORT
}

/************************************************************************/
xbDbf::~xbDbf(){

  // is there is an uncommited update, discard it.
  // as we don't know if it is an append or an update
  if( iDbfStatus == XB_UPDATED )
    Abort();

  if( iDbfStatus != XB_CLOSED )
    Close();

  if( SchemaPtr ){
    free( SchemaPtr );
    SchemaPtr = NULL;
  }
  if( RecBuf ){
    free( RecBuf );
    RecBuf = NULL;
  }
  if( RecBuf2){
    free( RecBuf2 );
    RecBuf2 = NULL;
  }
  if( RecBuf ){
    free( RecBuf );
    RecBuf = NULL;
  }

  #ifdef XB_BLOCKREAD_SUPPORT
  if( bBlockReadEnabled )
    DisableBlockReadProcessing();
  #endif   // XB_BLOCKREAD_SUPPORT

  Close();
}
/************************************************************************/
xbInt16 xbDbf::Abort(){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;

  try{
    if( iDbfStatus == XB_UPDATED ){
      #ifdef XB_MEMO_SUPPORT
      if( MemoFieldsExist()){
        if(( iRc = Memo->Abort()) != XB_NO_ERROR ){
          iErrorStop = 100;
          throw iRc;
        }
      }
      #endif
      memcpy( RecBuf, RecBuf2, uiRecordLen );
      iDbfStatus = XB_OPEN;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbdbf::Abort() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}

/************************************************************************/
#ifdef XB_INDEX_SUPPORT
xbInt16 xbDbf::AddIndex( xbIx * ixIn, const xbString &sFmt ){

  xbIxList *ixt;   // this
  if(( ixt = (xbIxList *) malloc( sizeof( xbIxList ))) == NULL )
    return XB_NO_ERROR;

  ixt->ix   = ixIn;
  ixt->next = NULL;
  ixt->sFmt = new xbString( sFmt );
  ixt->sFmt->ToUpperCase();

  if( ixList ){
    xbIxList *ixn = ixList;   // next
    while( ixn->next ){
      ixn = ixn->next;
    }
    ixn->next = ixt;
  } else {
    ixList = ixt;
  }
  return XB_NO_ERROR;
}
#endif // XB_INDEX_SUPPORT
/************************************************************************/
xbInt16 xbDbf::AppendRecord(){
  xbInt16 iErrorStop = 0;
  xbInt16 iRc = XB_NO_ERROR;
  xbUInt32 ulSaveCurRec = 0;

  try{
    #ifdef XB_INDEX_SUPPORT
    xbIxList *ixList = GetIxList();
    // do this step first before anything is locked, reduce lock time as much as possible
    while( ixList ){
      if(( iRc = ixList->ix->CreateKeys( 1 )) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      }

      ixList = ixList->next;
    }
    #endif // XB_INDEX_SUPPORT

    // lock everything up for an update
    #ifdef XB_LOCKING_SUPPORT

    if( (GetMultiUser() == xbOn) && !bTableLocked ){
      if(( iRc = LockHeader( XB_LOCK )) != XB_NO_ERROR ){
        if( iRc == XB_LOCK_FAILED )  {
          return iRc;
        } else {
          iErrorStop = 110;
          throw iRc;
        }
      }

      if(( iRc = LockAppend( XB_LOCK )) != XB_NO_ERROR ){
        if( iRc == XB_LOCK_FAILED ){
          LockHeader( XB_UNLOCK );
          return iRc;
        } else {
         iErrorStop = 120;
         throw iRc;
        }
      }

      #ifdef XB_INDEX_SUPPORT
      if(( iRc = LockIndices( XB_LOCK )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }
      #endif // XB_INDEX_SUPPORT

    }
    #endif // XB_LOCKING_SUPPORT
    if(( iRc = ReadHeader( 1, 1 )) != XB_NO_ERROR ){
      iErrorStop = 140;
      throw iRc;
    }
    #ifdef XB_INDEX_SUPPORT

    ixList = GetIxList();
    while( ixList ){
      if(( iRc = ixList->ix->CheckForDupKeys()) != 0 ){
        if( iRc < 0 ){
          iErrorStop = 150;
          throw iRc;
        }
        return XB_KEY_NOT_UNIQUE;
      }
      ixList = ixList->next;
    }
    #endif // XB_INDEX_SUPPORT

    // calculate the latest header information
    xbDate d;
    d.Sysdate();
    cUpdateYY = (char) d.YearOf() - 1900;
    cUpdateMM = (char) d.MonthOf();
    cUpdateDD = (char) d.DayOf( XB_FMT_MONTH );
    ulSaveCurRec = ulCurRec;
    ulNoOfRecs++;
    ulCurRec  = ulNoOfRecs;

    #ifdef XB_INDEX_SUPPORT

    ixList = GetIxList();
    while( ixList ){
      if(( iRc = ixList->ix->AddKeys( ulCurRec )) != XB_NO_ERROR ){
        iErrorStop = 160;
        throw iRc;
      }
      ixList = ixList->next;
    }

    #ifdef XB_LOCKING_SUPPORT
    //if( iAutoLock ){
    if( GetMultiUser() == xbOn ){
      if(( iRc = LockIndices( XB_UNLOCK )) != XB_NO_ERROR ){
        iErrorStop = 170;
        throw iRc;
      }
    }
    #endif     // XB_LOCKING_SUPPORT
    #endif     // XB_INDEX_SUPPORT

    // rewrite the header record
    if(( iRc = WriteHeader( 1, 1 )) != XB_NO_ERROR ){
      iErrorStop = 180;
      throw iRc;
    }

    #ifdef XB_LOCKING_SUPPORT
    // if( iAutoLock ){
    if( GetMultiUser() == xbOn ){
      if(( iRc = LockHeader( XB_UNLOCK )) != XB_NO_ERROR ){
        iErrorStop = 190;
        throw iRc;
      }
    }
    #endif

    // write the last record
    if(( iRc = xbFseek( (uiHeaderLen+((xbInt64)(ulNoOfRecs-1)*uiRecordLen)), 0 )) != XB_NO_ERROR ){
      iErrorStop = 200;
      throw iRc;
    }

    if(( iRc = xbFwrite( RecBuf, uiRecordLen, 1 )) != XB_NO_ERROR ){
      iErrorStop = 210;
      throw iRc;
    }

    // write the end of file marker
    if(( iRc = xbFputc( XB_CHAREOF )) != XB_NO_ERROR ){
      iErrorStop = 220;
      throw iRc;
    }

    #ifdef XB_LOCKING_SUPPORT
    if( GetMultiUser() == xbOn ){
      if(( iRc = LockAppend( XB_UNLOCK )) != XB_NO_ERROR ){
         iErrorStop = 230;
         throw( iRc );
      }
    }
    #endif // XB_LOCKING_SUPPORT

  }
  catch (xbInt16 iRc ){
    if( ulSaveCurRec != 0 ){
      ulCurRec = ulSaveCurRec;
      ulNoOfRecs--;
    }
    #ifdef XB_LOCKING_SUPPORT
    //if( iAutoLock ){
    if( GetMultiUser() == xbOn ){
      #ifdef XB_INDEX_SUPPORT
      LockIndices( XB_UNLOCK );
      #endif // XB_INDEX_SUPPORT
      LockAppend( XB_UNLOCK );
      LockHeader( XB_UNLOCK );
    }
    #endif // XB_LOCKING_SUPPORT

    if( iRc != XB_LOCK_FAILED && iRc != XB_KEY_NOT_UNIQUE ){
      xbString sMsg;
      sMsg.Sprintf( "xbdbf::Append() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
      xbase->WriteLogMessage( sMsg );
      xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
    }
  }

  if( iRc == XB_NO_ERROR )
    iDbfStatus = XB_OPEN;
  return iRc;
}
/************************************************************************/
#ifdef XB_INF_SUPPORT
xbInt16 xbDbf::AssociateIndex( const xbString &sIxType, const xbString &sIxName, xbInt16 iOpt ){

  xbInt16  iRc = 0;
  xbInt16  iErrorStop = 0;

  try{
    xbString sIxTypeIn = sIxType;
    sIxTypeIn.Trim();
    xbString sIxNameIn = sIxName;
    sIxNameIn.Trim();

    if( sIxTypeIn != "NDX" || sIxName == "" )
      return XB_INVALID_INDEX;

    if(( iRc = LoadInfData()) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }

    // check if entry exists
    xbLinkListNode<xbString> * llN = llInfData.GetHeadNode();
    xbBool bFound = xbFalse;
    xbString s;

    while( llN && !bFound ){
      s = llN->GetKey();
      if( s.Len() > 0 ){
        if( sIxNameIn == s )
          bFound = xbTrue;
      }
      llN = llN->GetNextNode();
    }

    xbBool bUpdated = xbFalse;
    if( iOpt == 0 && !bFound ){
      s.Sprintf( "%s%c%c", sIxName.Str(), 0x0d, 0x0a );
      llInfData.InsertAtEnd( s );
      bUpdated = xbTrue;

    } else if( iOpt == 1 && bFound ){
      llInfData.RemoveByVal( s );
      bUpdated = xbTrue;
    }

    if( bUpdated ){
      if(( iRc = SaveInfData()) != XB_NO_ERROR ){
        iErrorStop = 110;
        throw iRc;
      }
    }

 } catch( xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbdbf::AssociateIndex() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
#endif // XB_INF_SUPPORT

/************************************************************************/
xbInt16 xbDbf::BlankRecord()
{
  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;

  try{

    if( iDbfStatus == XB_CLOSED ){
      iErrorStop = 100;
      iRc = XB_NOT_OPEN;
      throw iRc;
    }

    if( iDbfStatus == XB_UPDATED ){
      if(  GetAutoCommit() == 1 ){
        if(( iRc = Commit()) != XB_NO_ERROR ){
          iErrorStop = 110;
          throw iRc;
        }
      } else {
        if(( iRc = Abort()) != XB_NO_ERROR ){
          iErrorStop = 120;
          throw iRc;
        }
      }
    }
    ulCurRec = 0;
    memset( RecBuf,  0x20, uiRecordLen );
    memset( RecBuf2, 0x20, uiRecordLen );
  }

  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbdbf::BlankRecord() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }

  return iRc;
}
/************************************************************************/
#ifdef XB_INDEX_SUPPORT
/*!
  This method is used to check an index tag's intgerity.

  @param iTagOpt 0 - Check current tag<br>
                 1 - Check all tags<br>

  @param iOutputOpt Output message destination<br>
                 0 = stdout<br>
                 1 = Syslog<br>
                 2 = Both<br>

  @returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbDbf::CheckTagIntegrity( xbInt16 iTagOpt, xbInt16 iOutputOpt ){
  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;

  try{
    if( iTagOpt == 0 ){
      if( pCurIx )
        return pCurIx->CheckTagIntegrity( vpCurIxTag, iOutputOpt );
      else
        return XB_INVALID_TAG;

    } else {

      xbLinkListNode<xbTag *> *llN = GetTagList();
      xbTag *pTag;

      while( llN ){
        pTag = llN->GetKey();
        if(( iRc = pTag->GetIx()->CheckTagIntegrity( pTag->GetVpTag(), iOutputOpt )) < XB_NO_ERROR ){
          iErrorStop = 110;
          throw iRc;
        }
        llN = llN->GetNextNode();
      }
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbdbf::CheckTagIntegrity() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}

/************************************************************************/
xbInt16 xbDbf::Reindex( xbInt16 iTagOpt, xbIx **ppIx, void **vppTag ){
  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iRc2 = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  void    *vp;
  xbBool  bLocked = xbFalse;

  xbString sType;
  xbString sTagName;

  if( iTagOpt < 0 || iTagOpt > 2 || (iTagOpt == 2 && (!ppIx || !vppTag )))
    return XB_INVALID_OPTION;

  #ifdef XB_BLOCKREAD_SUPPORT
  xbBool bOriginalBlockReadSts = GetBlockReadStatus();
  #endif

  try{

    #ifdef XB_LOCKING_SUPPORT
    //if( GetAutoLock() && !GetTableLocked() ){
    if( GetMultiUser() && !GetTableLocked() ){
      if(( iRc = LockTable( XB_LOCK )) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      }
      bLocked = xbTrue;
    }
    #endif

    #ifdef XB_BLOCKREAD_SUPPORT
    if( !bOriginalBlockReadSts )
      EnableBlockReadProcessing();
    #endif

    if( iTagOpt == 0 ){

      if( pCurIx ){
        if(( iRc = pCurIx->ReindexTag( &vpCurIxTag )) != XB_NO_ERROR ){
          if(( iRc2 = DeleteTag( sCurIxType, pCurIx->GetTagName( vpCurIxTag ))) != XB_NO_ERROR ){
            iErrorStop = 110;
            throw iRc2;
          }
          iErrorStop = 120;
          throw iRc;
        }
        return iRc;
      } else {
        return XB_INVALID_TAG;
      }

    } else if( iTagOpt == 1 ) {

      xbLinkListNode<xbTag *> *llN = GetTagList();
      xbTag *pTag;

      while( llN ){

        pTag = llN->GetKey();
        vp   = pTag->GetVpTag();

        if(( iRc = pTag->GetIx()->ReindexTag( &vp )) != XB_NO_ERROR ){

          if(( iRc2 = DeleteTag( pTag->GetType(), pTag->GetTagName())) != XB_NO_ERROR ){
            iErrorStop = 130;
            throw iRc2;
          }

          iErrorStop = 140;
          throw iRc;
        }
        llN = llN->GetNextNode();
      }
    } else if( iTagOpt == 2 ){

      xbIx *pIx = *ppIx;
      void *vpTag = *vppTag;

      if(( iRc = pIx->ReindexTag( &vpTag )) != XB_NO_ERROR ){
        if(( iRc2 = DeleteTag( pIx->GetType(), pIx->GetTagName( vpTag ))) != XB_NO_ERROR ){
          iErrorStop = 150;
          throw iRc2;
        }

        iErrorStop = 160;
        throw iRc;
      }
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbdbf::Reindex() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  #ifdef XB_BLOCKREAD_SUPPORT
  if( !bOriginalBlockReadSts )
    DisableBlockReadProcessing();
  #endif

  #ifdef XB_LOCKING_SUPPORT
  if( bLocked ){
    LockTable( XB_UNLOCK );
  }
  #endif
  return iRc;
}

/************************************************************************/
void xbDbf::ClearTagList(){
  xbTag *pTag;
  xbBool bDone = xbFalse;
  while( llTags.GetNodeCnt() > 0 && !bDone ){
    if( llTags.RemoveFromFront( pTag ) != XB_NO_ERROR ){
      bDone = xbTrue;
    } else {
      if( pTag )
        delete pTag;
    }
  }
}
#endif // XB_INDEX_SUPPORT
/************************************************************************/
xbInt16 xbDbf::Close(){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  try{
    if(iDbfStatus == XB_CLOSED)
      return XB_NO_ERROR;

    else if( iDbfStatus == XB_UPDATED ){
      if(  GetAutoCommit() == 1 ){
        if(( iRc = Commit()) != XB_NO_ERROR ){
          iErrorStop = 100;
          throw iRc;
        }
      } else {
        if(( iRc = Abort()) != XB_NO_ERROR ){
          iErrorStop = 110;
          throw iRc;
        }
     }
   }

    if(SchemaPtr){
      free( SchemaPtr );
      SchemaPtr = NULL;
    }
    if(RecBuf){
      free( RecBuf );
      RecBuf = NULL;
    }
    if(RecBuf2){
      free( RecBuf2 );
      RecBuf2 = NULL;
    }

    #ifdef XB_MEMO_SUPPORT
    if( iMemoFieldCnt > 0 ){
      Memo->CloseMemoFile();
      delete Memo;
      Memo = NULL;
    }
    #endif

    // close any open index files, remove from the ix list
    #ifdef XB_INDEX_SUPPORT
    while( ixList ){
      ixList->ix->Close();
      RemoveIndex( ixList->ix );
    }
    #endif

    if(( iRc = xbase->RemoveTblFromTblList( this )) != XB_NO_ERROR ){
      xbString sMsg;
      sMsg.Sprintf( "Alias = [%s]", sAlias.Str());
      xbase->WriteLogMessage( sMsg.Str() );
      iErrorStop = 120;
      throw iRc;
    }
    xbFclose();
    InitVars();
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbdbf::Close() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
/************************************************************************/
#ifdef XB_INDEX_SUPPORT
xbInt16 xbDbf::CloseIndexFile( xbIx *pIx ){

  xbInt16 iErrorStop = 0;
  xbInt16 iRc = XB_NO_ERROR;

  try{
    // verify index is open and in the list
    xbBool bFound = xbFalse;
    xbIxList *p = GetIxList();
    while( p && !bFound ){
      if( pIx == p->ix )
        bFound = xbTrue;
      p = p->next;
    }
    if( !bFound ){
      iErrorStop = 100;
      iRc = XB_NOT_OPEN;
      throw iRc;
    }
    //  close it
    if(( iRc = pIx->Close()) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }
    //  remove it from the list
    if(( iRc = RemoveIndex( pIx )) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw iRc;
    }
    // refresh the tag list
    if(( iRc = UpdateTagList()) != XB_NO_ERROR ){
      iErrorStop = 130;
      throw iRc;
    }
    if( pIx == pCurIx ){
      pCurIx     = NULL;
      vpCurIxTag = NULL;
      sCurIxType = "";
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbdbf::CloseIndexFile() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
#endif // XB_INDEX_SUPPORT

/************************************************************************/
xbInt16 xbDbf::Commit(){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  try{

    if( iDbfStatus == XB_UPDATED ){
      if( ulCurRec == 0 ){
        if(( iRc = AppendRecord()) != XB_NO_ERROR ){
          iErrorStop = 100;
          throw iRc;
        }
      } else {
        if(( iRc = PutRecord( ulCurRec )) != XB_NO_ERROR ){
          iErrorStop = 110;
          throw iRc;
        }
      }
    }
  }
  catch (xbInt16 iRc ){
    if( iRc != XB_LOCK_FAILED ){
      xbString sMsg;
      sMsg.Sprintf( "xbdbf::Commit() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
      xbase->WriteLogMessage( sMsg.Str() );
      xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
    }
  }
  return iRc;
}
/************************************************************************/
xbInt16 xbDbf::CopyDbfStructure( xbDbf * dNewTable, const xbString &sNewTableName,
                 const xbString & sNewTableAlias, xbInt16 iOverlay, xbInt16 iShareMode ) {

  xbInt16  iRc = XB_NO_ERROR;
  xbInt16  iErrorStop = 0;
  xbSchema *newTableSchema = NULL;

  try{
    if( iDbfStatus == XB_CLOSED ){
      iErrorStop = 100;
      iRc = XB_DBF_FILE_NOT_OPEN;
      throw iRc;
    }

    if( !dNewTable ){
      iErrorStop = 110;
      iRc = XB_INVALID_OBJECT;
      throw iRc;
    }

    // Get the number of schema entries for this table
    xbInt32 lSchemaRecCnt = GetFieldCnt() + 1;

    // Allocate a Schema = No Of Fields + 1
    if((newTableSchema=(xbSchema *)malloc( (size_t) lSchemaRecCnt * sizeof(xbSchema)))==NULL){
      iErrorStop = 120;
      iRc = XB_NO_MEMORY;
      throw iRc;
    }

    //  Populate the Schema
    xbInt32 l;
    for( l = 0; l < lSchemaRecCnt-1; l++ ){
      memset( newTableSchema[l].cFieldName, 0x00, 11 );
      for( int x = 0; x < 10 && SchemaPtr[l].cFieldName[x]; x++ )
        newTableSchema[l].cFieldName[x] = SchemaPtr[l].cFieldName[x];
      newTableSchema[l].cType     = SchemaPtr[l].cType;
      newTableSchema[l].iFieldLen = SchemaPtr[l].cFieldLen;
      newTableSchema[l].iNoOfDecs = SchemaPtr[l].cNoOfDecs;
    }

    // set the last one to zeroes
    memset( newTableSchema[l].cFieldName, 0x00, 11 );
    newTableSchema[l].cType     = 0;
    newTableSchema[l].iFieldLen = 0;
    newTableSchema[l].iNoOfDecs = 0;

    dNewTable->SetVersion();
    #ifdef XB_MEMO_SUPPORT
    if( MemoFieldsExist())
      dNewTable->SetCreateMemoBlockSize( Memo->GetBlockSize() );
    #endif

    // Call the create a table function
    if(( iRc = dNewTable->CreateTable( sNewTableName, sNewTableAlias, newTableSchema, iOverlay, iShareMode )) != XB_NO_ERROR ){
      iErrorStop = 130;
      throw iRc;
    }
  }

  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbDbf::CopyDbfStructure() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }

  if( newTableSchema )
    free( newTableSchema );

  return iRc;
}

/************************************************************************/
#ifdef XB_INDEX_SUPPORT
xbInt16  xbDbf::CreateTag( const xbString &sIxType, const xbString &sName, const xbString &sKey, const xbString &sFilter, 
                   xbInt16 iDescending, xbInt16 iUnique, xbInt16 iOverLay, xbIx **pIxOut, void **vpTagOut ){

  xbInt16 iErrorStop = 0;
  xbInt16 iRc = XB_NO_ERROR;

  #ifdef XB_LOCKING_SUPPORT
  xbBool   bLocked = xbFalse;
  #endif

  try{
    xbString sType = sIxType;
    sType.ToUpperCase();

    #ifdef XB_LOCKING_SUPPORT
    //if( iAutoLock && !bTableLocked ){
    if(( GetMultiUser() == xbOn ) && !bTableLocked ){
      if(( iRc = LockTable( XB_LOCK )) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw( iRc );
      }
      bLocked = xbTrue;
    }
    #endif // XB_LOCKING_SUPPORT

    if( sIxType == "" ){
      iErrorStop = 110;
      iRc = XB_INVALID_OPTION;
      throw iRc;

    #ifdef XB_NDX_SUPPORT
    } else if( sIxType == "NDX" ){
      xbIxNdx *ixNdx = new xbIxNdx( this );

      if(( iRc = ixNdx->CreateTag( sName, sKey, sFilter, iDescending, iUnique, iOverLay, vpTagOut )) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }

      if(( iRc = AddIndex( ixNdx, sIxType )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }
      *pIxOut = ixNdx;

      // Set the current tag if one not already set
      if( sCurIxType == "" ){
        sCurIxType = "NDX";
        pCurIx = ixNdx;
        vpCurIxTag = ixNdx->GetTag(0);
      }

    #endif

    #ifdef XB_MDX_SUPPORT
    } else if( sIxType == "MDX" ){

      if( GetVersion() == 3 ){     // MDX indexes were version 4 and higher
        iErrorStop = 140;
        iRc = XB_INVALID_INDEX;
        throw iRc;
      }

      xbIxMdx *ixMdx;
      xbString s;
      //  look through the index list and see if there is an mdx pointer we can grab
      xbBool bMdxFound = xbFalse;
      xbIxList *ixList = GetIxList();
      while( ixList && !bMdxFound ){
        s = ixList->sFmt->Str();
        if( s == "MDX" ){
          ixMdx = (xbIxMdx *) ixList->ix;
          bMdxFound = xbTrue;
        }
        ixList = ixList->next;
      }

      if( !bMdxFound )
        ixMdx = new xbIxMdx( this );

      if(( iRc = ixMdx->CreateTag( sName, sKey, sFilter, iDescending, iUnique, iOverLay, vpTagOut )) != XB_NO_ERROR ){
        iErrorStop = 150;
        throw iRc;
      }

      if( !bMdxFound ){
        if(( iRc = AddIndex( ixMdx, "MDX" )) != XB_NO_ERROR ){
          iErrorStop = 160;
          throw iRc;
        }

        cIndexFlag = 0x01;
        if(( iRc = WriteHeader( 1, 0 )) != XB_NO_ERROR ){
          iErrorStop = 170;
          throw iRc;
        }
      }
      *pIxOut = ixMdx;

      // set the current tag if one not already set
      if( sCurIxType == "" ){
        sCurIxType = "MDX";
        pCurIx = ixMdx;
        vpCurIxTag = ixMdx->GetTag(0);
      }
    #endif

    #ifdef XB_TDX_SUPPORT
    } else if( sIxType == "TDX" ){

      if( GetVersion() == 3 ){     // TDX indexes were version 4 and higher
        iErrorStop = 140;
        iRc = XB_INVALID_INDEX;
        throw iRc;
      }

      xbIxTdx *ixTdx;
      xbString s;
      //  look through the index list and see if there is an mdx pointer we can grab
      xbBool bTdxFound = xbFalse;
      xbIxList *ixList = GetIxList();
      while( ixList && !bTdxFound ){
        s = ixList->sFmt->Str();
        if( s == "TDX" ){
          ixTdx = (xbIxTdx *) ixList->ix;
          bTdxFound = xbTrue;
        }
        ixList = ixList->next;
      }
      if( !bTdxFound )
        ixTdx = new xbIxTdx( this );

      if(( iRc = ixTdx->CreateTag( sName, sKey, sFilter, iDescending, iUnique, iOverLay, vpTagOut )) != XB_NO_ERROR ){
        iErrorStop = 150;
        throw iRc;
      }
      if( !bTdxFound ){
        if(( iRc = AddIndex( ixTdx, "TDX" )) != XB_NO_ERROR ){
          iErrorStop = 160;
          throw iRc;
        }
      }
      *pIxOut = ixTdx;

      // set the current tag if one not already set
      if( sCurIxType == "" ){
        sCurIxType = "TDX";
        pCurIx = ixTdx;
        vpCurIxTag = ixTdx->GetTag(0);
      }
    #endif

    } else {
      iErrorStop = 200;
      iRc = XB_INVALID_OPTION;
      throw iRc;
    }

    if(( iRc = UpdateTagList()) != XB_NO_ERROR ){
      iErrorStop = 300;
      throw iRc;
    }

  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbdbf::CreateTag() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }

  #ifdef XB_LOCKING_SUPPORT
  if( bLocked )
    LockTable( XB_UNLOCK );
  #endif // XB_LOCKING_SUPPORT

  return iRc;
}
#endif // XB_INDEX_SUPPORT

/************************************************************************/
xbInt16 xbDbf::DeleteAll( xbInt16 iOption )
{
  xbInt16 iErrorStop = 0;
  xbInt16 iRc = XB_NO_ERROR;
  xbUInt32 ulRecCnt;

  try{
    if(( iRc = GetRecordCnt( ulRecCnt )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }
    if( ulRecCnt == 0 ) 
      return XB_NO_ERROR;
    for( xbUInt32 ul = 0; ul < ulRecCnt; ul++ ){
      if(( iRc = GetRecord( ul+1 )) != XB_NO_ERROR ){
        iErrorStop = 110;
        throw iRc;
      }
      if( iOption == 0 ){   /* delete all option */
        if( !RecordDeleted()){
          if(( iRc = DeleteRecord()) != XB_NO_ERROR ){
            iErrorStop = 120;
            throw iRc;
          }
          if(( iRc = Commit()) != XB_NO_ERROR ){
            iErrorStop = 130;
            throw iRc;
          }
        }
      } else {   /* undelete all option */
        if( RecordDeleted()){
          if(( iRc = UndeleteRecord()) != XB_NO_ERROR ){
            iErrorStop = 140;
            throw iRc;
          }
          if(( iRc = Commit()) != XB_NO_ERROR ){
            iErrorStop = 150;
            throw iRc;
          }
        }
      }
    }
  }
  catch (xbInt16 iRc ){
    if( iRc != XB_LOCK_FAILED ){
      xbString sMsg;
      sMsg.Sprintf( "xbDbf::DeleteAll() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
      xbase->WriteLogMessage( sMsg.Str() );
      xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
    }
  }
  return iRc;
}

/************************************************************************/
#ifdef XB_INDEX_SUPPORT
xbInt16 xbDbf::DeleteAllIndexFiles(){

  xbInt16  iRc = 0;
  xbInt16  iErrorStop = 0;

  #ifdef XB_LOCKING_SUPPORT
  xbBool   bLocked = xbFalse;
  #endif  // XB_LOCKING_SUPPORT

  #ifdef XB_INF_SUPPORT
  xbString sIxName;
  #endif // XB_INF_SUPPORT

  try{
    #ifdef XB_LOCKING_SUPPORT
//    if( iAutoLock && !bTableLocked ){
    if((GetMultiUser() == xbOn ) && !bTableLocked ){
      if(( iRc = LockTable( XB_LOCK )) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw( iRc );
      }
      bLocked = xbTrue;
    }
    #endif // XB_LOCKING_SUPPORT

    // close any open index files, delete it, remove from the ix list
    while( ixList ){

      // next two lines for debugging
      ixList->ix->GetFileNamePart( sIxName );
      ixList->ix->Close();
      if(( iRc = ixList->ix->xbRemove()) != XB_NO_ERROR ){
        iErrorStop = 110;
        throw iRc;
      }
      #ifdef XB_INF_SUPPORT
      // if XB_INF_SUPPORT is enabled, all open non prod indices should be in here
      if( *ixList->sFmt != "MDX" && *ixList->sFmt != "TDX" ){    // production and temp indices not stored in .INF dataset
        if(( iRc = ixList->ix->GetFileNamePart( sIxName )) != XB_NO_ERROR ){
          iErrorStop = 120;
          throw iRc;
        }
        if(( iRc = AssociateIndex( *ixList->sFmt, sIxName, 1 )) != XB_NO_ERROR ){
          iErrorStop = 130;
          throw iRc;
        }
      }
      #endif
      RemoveIndex( ixList->ix );
    }
 } catch( xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbdbf::DeleteAllIndexFiles() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  #ifdef XB_LOCKING_SUPPORT
  if( bLocked )
    LockTable( XB_UNLOCK );
  #endif // XB_LOCKING_SUPPORT
  return iRc;
}
#endif   // XB_INDEX_SUPPORT

/************************************************************************/
xbInt16 xbDbf::DeleteAllRecords(){
  return DeleteAll( 0 );
}
/************************************************************************/
#ifdef XB_INF_SUPPORT
xbInt16 xbDbf::DeleteInfData(){
  xbInt16  iRc = 0;
  xbInt16  iErrorStop = 0;

  try{
    xbString sInfFileName;
    if(( iRc = GetInfFileName( sInfFileName )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }
    xbFile f( xbase );
    f.SetFileName( sInfFileName );
    if( f.FileExists()){
      if(( iRc = f.xbRemove()) != XB_NO_ERROR ){
        iErrorStop = 110;
        throw iRc;
      }
    }
  } catch( xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbdbf::DeleteInfData() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
#endif // XB_INF_SUPPORT
/************************************************************************/
xbInt16 xbDbf::DeleteRecord(){
  if( RecBuf && ulCurRec > 0 ){
    if( RecBuf[0] != 0x2a){
      if( iDbfStatus != XB_UPDATED ){
        iDbfStatus = XB_UPDATED;
        memcpy( RecBuf2, RecBuf, uiRecordLen );  // save off original before making any updates
      }
      RecBuf[0] = 0x2a;
    }
    return XB_NO_ERROR;
  }
  else
    return XB_INVALID_RECORD;
}
/************************************************************************/
xbInt16 xbDbf::DeleteTable(){

  xbInt16  iRc = 0;
  xbInt16  iErrorStop = 0;
  #ifdef   XB_LOCKING_SUPPORT
  xbBool   bTableLocked = xbFalse;
  #endif
  try{
    #ifdef XB_LOCKING_SUPPORT
    //if( iAutoLock && !bTableLocked ){
    if((GetMultiUser() == xbOn) && !bTableLocked ){
      if(( iRc = LockTable( XB_LOCK )) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      } else {
        bTableLocked = xbTrue;
      }
    }
    #endif  // XB_LOCKING_SUPPORT

    #ifdef XB_INDEX_SUPPORT
    if(( iRc = DeleteAllIndexFiles()) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }

    #ifdef XB_INF_SUPPORT
    if(( iRc = DeleteInfData()) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw iRc;
    }
    #endif // XB_INF_SUPPORT
    #endif // XB_INDEX_SUPPORT

    #ifdef XB_MEMO_SUPPORT
    xbInt16 iMemoFldCnt = GetMemoFieldCnt();
    xbString sMemoFileName;
    if(iMemoFldCnt > 0 ){
      sMemoFileName = Memo->GetFqFileName();
    }
    #endif // XB_MEMO_SUPPORT

    if(( iRc = Close()) != XB_NO_ERROR ){
      iErrorStop = 130;
      throw iRc;
    }

    if(( iRc = xbRemove()) != XB_NO_ERROR ){
      iErrorStop = 140;
      throw iRc;
    }

    #ifdef XB_MEMO_SUPPORT
    if( iMemoFieldCnt > 0 ){
      xbFile f( xbase );
      if(( iRc = f.xbRemove( sMemoFileName )) != XB_NO_ERROR ){
        iErrorStop = 150;
        throw iRc;
      }
    }
    #endif // XB_MEMO_SUPPORT

  } catch( xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbdbf::DeleteTable() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  #ifdef XB_LOCKING_SUPPORT
  if( bTableLocked )
    LockTable( XB_UNLOCK );
  #endif // XB_LOCKING_SUPPORT
  return iRc;
}

/************************************************************************/
#ifdef XB_INDEX_SUPPORT
xbInt16 xbDbf::DeleteTag( const xbString &sIxType, const xbString &sName ){

  xbInt16 iErrorStop = 0;
  xbInt16 iRc = XB_NO_ERROR;
  xbIx *pIx = NULL;

  #ifdef XB_LOCKING_SUPPORT
  xbBool bTableLocked = xbFalse;
  #endif

  try{

    #ifdef XB_LOCKING_SUPPORT
    //if( iAutoLock && !GetTableLocked() ){
    if((GetMultiUser() == xbOn) && !GetTableLocked() ){
      if(( iRc = LockTable( XB_LOCK )) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      } else {
        bTableLocked = xbTrue;
      }
    }
    #endif   // XB_LOCKING_SUPPORT

    if( sIxType == "" ){
      iErrorStop = 110;
      iRc = XB_INVALID_OPTION;
      throw iRc;

    #ifdef XB_NDX_SUPPORT
    } else if( sIxType == "NDX" ){

      xbIxList *ixl = ixList;
      xbBool bDone = xbFalse;
      while( ixl && !bDone ){

        if( ixl->ix->GetTagName( NULL ) == sName ){
          bDone = xbTrue;

          // remove from .INF if it's there
          #ifdef XB_INF_SUPPORT 
          if(( iRc = AssociateIndex( "NDX", sName, 1 )) != XB_NO_ERROR ){
            iErrorStop = 120;
            throw iRc;
          }
          #endif // XB_INF_SUPPORT

          if(( iRc = ixl->ix->DeleteTag( ixl->ix->GetTag( 0 ))) != XB_NO_ERROR ){
            iErrorStop = 130;
            throw iRc;
          }

          if(( iRc = RemoveIndex( ixl->ix )) != XB_NO_ERROR ){
            iErrorStop = 140;
            throw iRc;
          }

          if( !ixList || ixl->ix == pCurIx )
            SetCurTag( "", NULL, NULL );

        }
        ixl = ixl->next;
      }
    #endif

    #ifdef XB_MDX_SUPPORT
    } else if( sIxType == "MDX" ){
      xbIxList *ixl = ixList;
      xbIxList *ixlNext;
      xbIxList *ixlPrev = NULL;
      xbBool bDone = xbFalse;
      xbIxMdx  *pMdx;
      xbMdxTag *pMdxTag;
      xbInt16 iTagCnt = 0;

      while( ixl && !bDone ){
        ixlNext = ixl->next;
        pMdx = (xbIxMdx *) ixl->ix;
        iTagCnt = pMdx->GetTagCount();
        for( xbInt16 i = 0; i < iTagCnt && !bDone; i++ ){
          pMdxTag = (xbMdxTag *) pMdx->GetTag( i );
          if( pMdx->GetTagName( pMdxTag ) == sName ){
            bDone = xbTrue;
            iRc = pMdx->DeleteTag( pMdxTag );
            if( iRc > 0 ){
              // Successful delete of only tag in production mdx file - need to remove it from the list, update the dbf header
              cIndexFlag = 0x00;
              if(( iRc = WriteHeader( 1, 0 )) != XB_NO_ERROR ){
                iErrorStop = 150;
                throw iRc;
              }
              if(( iRc = RemoveIndex( ixl->ix )) != XB_NO_ERROR ){
                iErrorStop = 160;
                throw iRc;
              }
              if( ixlPrev == NULL ){
                // std::cout << "setting ixList to null or should be\n";
                ixList = ixlNext;
              } else {
                ixlPrev = ixlNext;
              }
            } else if( iRc < 0 ){
              iErrorStop = 170;
              throw iRc;
            }
            if( !ixList || ixl->ix == pCurIx )
              SetCurTag( "", NULL, NULL );
          }
        }
        ixlPrev = ixl;
        ixl = ixlNext;
      }

      if( !bDone )
        return XB_INVALID_TAG;
    #endif

    #ifdef XB_TDX_SUPPORT
    } else if( sIxType == "TDX" ){
      xbIxList *ixl = ixList;
      xbIxList *ixlNext;
      xbIxList *ixlPrev = NULL;
      xbBool bDone = xbFalse;
      xbIxTdx  *pTdx;
      xbMdxTag *pMdxTag;
      xbInt16 iTagCnt = 0;

      while( ixl && !bDone ){
        ixlNext = ixl->next;
        pTdx = (xbIxTdx *) ixl->ix;
        iTagCnt = pTdx->GetTagCount();
        for( xbInt16 i = 0; i < iTagCnt && !bDone; i++ ){
          pMdxTag = (xbMdxTag *) pTdx->GetTag( i );
          if( pTdx->GetTagName( pMdxTag ) == sName ){
            bDone = xbTrue;
            iRc = pTdx->DeleteTag( pMdxTag );
            if( iRc > 0 ){
              // Successful delete of only tag in production mdx file - need to remove it from the list, update the dbf header
              cIndexFlag = 0x00;
              if(( iRc = WriteHeader( 1, 0 )) != XB_NO_ERROR ){
                iErrorStop = 150;
                throw iRc;
              }
              if(( iRc = RemoveIndex( ixl->ix )) != XB_NO_ERROR ){
                iErrorStop = 160;
                throw iRc;
              }
              if( ixlPrev == NULL ){
                // std::cout << "setting ixList to null or should be\n";
                ixList = ixlNext;
              } else {
                ixlPrev = ixlNext;
              }
            } else if( iRc < 0 ){
              iErrorStop = 170;
              throw iRc;
            }
            if( ixList )
              std::cout << "ixlist not null\n";
            else
              std::cout << "ixlist null\n";

            if( !ixList  || ixl->ix == pCurIx )
              SetCurTag( "", NULL, NULL );
          }
        }
        ixlPrev = ixl;
        ixl = ixlNext;
      }

      if( !bDone )
        return XB_INVALID_TAG;

    #endif

    } else {
      iErrorStop = 180;
      iRc = XB_INVALID_OPTION;
      throw iRc;
    }

    if(( iRc = UpdateTagList()) != XB_NO_ERROR ){
      iErrorStop = 190;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    if( pIx ) delete pIx;
    xbString sMsg;
    sMsg.Sprintf( "xbdbf::DeleteTag() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));

    #ifdef XB_LOCKING_SUPPORT
    //if( iAutoLock && GetTableLocked() ){
    if((GetMultiUser() == xbOn) && GetTableLocked() ){
      LockTable( XB_UNLOCK );
    }
    #endif   // XB_LOCKING_SUPPORT
  }

  #ifdef XB_LOCKING_SUPPORT
  if( bTableLocked ){
    LockTable( XB_UNLOCK );
  }
  #endif   // XB_LOCKING_SUPPORT


  return iRc;
}
#endif // XB_INDEX_SUPPORT
/************************************************************************/
xbInt16 xbDbf::DumpHeader( xbInt16 iOption ){
  int i;
  int iMemoCtr = 0;

  if( iOption < 1 || iOption > 4 )
    return XB_INVALID_OPTION;

  xbInt16 iRc = ReadHeader( xbTrue, 0 );
  if( iRc != XB_NO_ERROR )
    return iRc;

  std::cout << "\nDatabase file " << GetFqFileName() << std::endl << std::endl;

  if( iOption != 2 ){
    std::cout << "File header data:" << std::endl;

    xbInt16 sVer = DetermineXbaseTableVersion( cVersion );

    if( sVer == 3 )
      std::cout << "dBASE III file"  << std::endl;
    else if ( sVer == 4 )
      std::cout << "dBASE IV file"   << std::endl << std::endl;
    else if ( sVer == 5 )
      std::cout << "dBASE V file"    << std::endl << std::endl;
    else if ( sVer == 7 )
      std::cout << "dBASE VII file"  << std::endl << std::endl;
    else
      std::cout << "Unknown Version" << std::endl;

    /* display the bit stream */
    unsigned char c, tfv, displayMask = 1 << 7;
    tfv = cVersion;
    std::cout << "File descriptor bits = ";
    for( c = 1; c<= 8; c++ ){
      std::cout << (tfv & displayMask ? '1' : '0');
      tfv <<= 1;
    }
    std::cout << std::endl;

    std::cout << "Descriptor bits legend:"           << std::endl;
    std::cout << "  0-2 = version number"            << std::endl;
    std::cout << "    3 = presence of dBASE IV memo" << std::endl;
    std::cout << "  4-6 = SQL table presence"        << std::endl;
    std::cout << "    7 = Presence of any memo file (dBASE III PLUS or dBASE IV)" << std::endl << std::endl;

    std::cout << "Last update date     = " 
      << (int) cUpdateMM << "/" << (int) cUpdateDD   << "/" << (int) cUpdateYY % 100 << std::endl;  

    std::cout << "Header length        = " << uiHeaderLen << std::endl;
    std::cout << "Record length        = " << uiRecordLen << std::endl;
    std::cout << "Records in file      = " << ulNoOfRecs  << std::endl << std::endl << std::endl;

    std::cout << "Transaction Flag     = ";
    xbBit b;
    b.BitDump( cTransactionFlag );
    std::cout << std::endl;

    std::cout << "Encryption Flag      = ";
    b.BitDump( cEncryptionFlag );
    std::cout << std::endl;

    std::cout << "Index Flag           = ";
    b.BitDump( cIndexFlag );
    std::cout << std::endl;

    std::cout << "Lang Driver          = " << (int) cIndexFlag << " - ";
    b.BitDump( cIndexFlag );
    std::cout << std::endl;
    #ifdef XB_INDEX_SUPPORT
    std::cout << "Open Index Files     = " << GetPhysicalIxCnt() << std::endl;
    #endif  // XB_INDEX_SUPPORT
  }

  if( iOption != 1 ){
    char c;
    std::cout << "Field Name   Type  Length  Decimals IxFlag" << std::endl;
    std::cout << "----------   ----  ------  -------- ------" << std::endl;
    for( i = 0; i < iNoOfFields; i++ ){

      SchemaPtr[i].cIxFlag ? c = 'Y' : c = ' ';

      if( SchemaPtr[i].cType == 'C' && SchemaPtr[i].cNoOfDecs > 0 )
        printf( "%10s    %1c     %4d    %4d      %c\n", SchemaPtr[i].cFieldName,
               SchemaPtr[i].cType, SchemaPtr[i].cFieldLen, 0, c );
      else
        printf( "%10s    %1c     %4d    %4d      %c\n", SchemaPtr[i].cFieldName,
               SchemaPtr[i].cType, SchemaPtr[i].cFieldLen, SchemaPtr[i].cNoOfDecs, c );

      if( SchemaPtr[i].cType == 'M' )
        iMemoCtr++;
    }
  }
  std::cout << std::endl;

#ifdef XB_MEMO_SUPPORT
  if( iOption > 3 && iMemoCtr > 0 )
    Memo->DumpMemoHeader();
#endif

  return XB_NO_ERROR;
}
/************************************************************************/
xbInt16 xbDbf::DumpRecord( xbUInt32 ulRecNo, xbInt16 iOutputDest, xbInt16 iOutputFmt, char cDelim ) {
  int i, iRc = XB_NO_ERROR;

  xbString sTemp;
  xbString s2;
  if( ulRecNo == 0 || ulRecNo > ulNoOfRecs )
    return XB_INVALID_RECORD;

  if( ulCurRec != ulRecNo ){
    iRc = GetRecord( ulRecNo );
    if( iRc != XB_NO_ERROR )
      return iRc;
  }

  if( iOutputFmt >= 1 ){
    if( iOutputFmt == 2 ){
      sTemp = "RecNo";
      sTemp += cDelim;
      sTemp += "DEL";
      for( i = 0; i < iNoOfFields; i++ ){
        s2 = SchemaPtr[i].cFieldName;
        s2.Trim();
        sTemp += cDelim;
        sTemp += s2;
      }
      xbase->WriteLogMessage( sTemp.Str(), iOutputDest );
    }

    if( RecordDeleted() )
      s2.Sprintf( "%ld%cDEL", ulRecNo, cDelim );
    else
      s2.Sprintf( "%ld%c", ulRecNo, cDelim );

    for( i = 0; i < iNoOfFields; i++ ){
      GetField( i, sTemp );
      sTemp.Trim();
      s2.Sprintf( "%s%c'%s'", s2.Str(), cDelim, sTemp.Str());
    }
    xbase->WriteLogMessage( s2.Str(),iOutputDest );
    return XB_NO_ERROR;
  }

  sTemp.Sprintf( "\n%s Rec Number: %ld", GetFileName().Str(), ulRecNo );
  xbase->WriteLogMessage( sTemp.Str(),iOutputDest);
  if( RecordDeleted())
    xbase->WriteLogMessage( "Record flagged as deleted", iOutputDest );

  #ifdef XB_MEMO_SUPPORT
  xbString sMemo;
  #endif

  for( i = 0; i < iNoOfFields; i++ ){

    #ifdef XB_MEMO_SUPPORT
    GetField( i, sTemp );
    sTemp.Trim();

    if(SchemaPtr[i].cType == 'M'){
      GetMemoField( i, sMemo );
    if( sMemo.Len() > 70 )
      sMemo.Resize( 70 );
    s2.Sprintf ( "%c  %s  = '%s'", SchemaPtr[i].cType, SchemaPtr[i].cFieldName, sMemo.Str());
    xbase->WriteLogMessage( s2.Str(), iOutputDest);

    } else {
      s2.Sprintf ( "%c  %s  = '%s'", SchemaPtr[i].cType, SchemaPtr[i].cFieldName, sTemp.Str());
      xbase->WriteLogMessage( s2.Str(), iOutputDest);
    }
    #else
    GetField( i, sTemp );
    sTemp.Trim();
    s2.Sprintf( "%s = '%s'", SchemaPtr[i].cFieldName, sTemp.Str());
    xbase->WriteLogMessage( s2.Str(), iOutputDest);
    #endif

  }
  return XB_NO_ERROR;
}


/************************************************************************/
#ifdef XB_DEBUG_SUPPORT


#ifdef XB_LOCKING_SUPPORT

void xbDbf::DumpTableLockStatus() const {

  std::cout << "File Lock Retry Count = [" << GetLockRetryCount() << "]" << std::endl;
  std::cout << "File Lock Flavor      = [";
  switch (GetLockFlavor()){
    case 1:
      std::cout << "dBASE]" << std::endl;
      break;
    case 2:
      std::cout << "Clipper]" << std::endl;
      break;
    case 3:
      std::cout << "Fox]" << std::endl;
      break;
    case 9:
      std::cout << "Xbase64]" << std::endl;
      break;
    default:
      std::cout << "Unknown]" << std::endl;
      break;
  }
  std::cout << "File Multi User Setting = [";
  if( GetMultiUser())
    std::cout << "ON]" << std::endl;
  else
    std::cout << "OFF]" << std::endl;

  if( GetHeaderLocked())
    std::cout << "Header Locked         = [TRUE]\n";
  else
    std::cout << "Header Locked         = [FALSE]\n";

  if( GetTableLocked())
    std::cout << "Table Locked          = [TRUE]\n";
  else
    std::cout << "Table Locked          = [FALSE]\n";

  if( GetAppendLocked() > 0 )
    std::cout << "Append Locked         = [" << GetAppendLocked() << "]\n";
  else
    std::cout << "Append Locked         = [FALSE]\n";

  #ifdef XB_MEMO_SUPPORT
  if( GetMemoLocked())
    std::cout << "Memo Locked           = [TRUE]\n";
  else
    std::cout << "Memo Locked           = [FALSE]\n";
  #endif // XB_MEMO_SUPPORT

  xbLinkListNode<xbUInt32> * llN = GetFirstRecLock();
  if( llN ){
    while( llN ){
      std::cout << "Record Locked         = [" << llN->GetKey() << "]\n";
      llN = llN->GetNextNode();
    }
  } else {
      std::cout << "Record Locked         = [None]\n";
  }
}
#endif  // XB_LOCKING_SUPPORT
#endif //  XB_DEBUG_SUPPORT

/************************************************************************/
#ifdef XB_LOCKING_SUPPORT
xbUInt32 xbDbf::GetAppendLocked() const {
  return this->ulAppendLocked;
}
#endif // XB_LOCKING_SUPPORT

/************************************************************************/
xbInt16 xbDbf::GetAutoCommit() const {
  return GetAutoCommit( 1 );
}

/************************************************************************/
xbInt16 xbDbf::GetAutoCommit( xbInt16 iOpt ) const {
  if( iOpt == 1 && iAutoCommit == -1 )
    return xbase->GetDefaultAutoCommit();
  else
    return iAutoCommit;
}
/************************************************************************/
#ifdef XB_MEMO_SUPPORT
xbUInt32 xbDbf::GetCreateMemoBlockSize() const {
  return ulCreateMemoBlockSize;
}
#endif  // XB_MEMO_SUPPORT

/************************************************************************/
#ifdef XB_INDEX_SUPPORT
xbIx *xbDbf::GetCurIx() const {
  return pCurIx;
}
/************************************************************************/
//! @brief Get pointer to current tag for the current index.
void *xbDbf::GetCurTag() const {
  return vpCurIxTag;
}
/************************************************************************/
//! @brief Get the current index type.
/*!
  @returns NDX for single tag index.<br>
           MDX for production multi tag index.<br>
           TDX for temporary tag index.
*/
const xbString &xbDbf::GetCurIxType() const {
  return sCurIxType;
}
/************************************************************************/
xbInt16 xbDbf::GetTagPtrs( const xbString &sTagNameIn,  xbIx **pIxOut, void **pTagOut ) {

  xbLinkListNode<xbTag *> *llN = GetTagList();
  xbTag *pTag;
  while( llN ){
    pTag = llN->GetKey();
    if( pTag->GetTagName() == sTagNameIn ){
      *pIxOut = pTag->GetIx();
      *pTagOut = pTag->GetVpTag();
      return XB_NO_ERROR;
    }
    llN = llN->GetNextNode();
  }
  *pIxOut  = NULL;
  *pTagOut = NULL;
  return XB_INVALID_TAG;
}
/************************************************************************/
const xbString &xbDbf::GetCurTagName() const {

  if( pCurIx )
    return pCurIx->GetTagName( vpCurIxTag );
   else
    return sNullString;
}
/************************************************************************/
xbInt16 xbDbf::GetFirstKey(){
  if( pCurIx )
    return pCurIx->GetFirstKey( vpCurIxTag, 1 );
  else
    return XB_INVALID_TAG;
}
/************************************************************************/
xbInt16 xbDbf::GetFirstKey( xbIx *pIx, void *vpTag ){
  if( pIx )
    return pIx->GetFirstKey( vpTag, 1 );
  else
    return XB_INVALID_TAG;
}
/************************************************************************/
xbUInt16 xbDbf::GetHeaderLen() const {
  return uiHeaderLen;
}
/************************************************************************/
xbInt16 xbDbf::GetLastKey(){
  if( pCurIx )
    return pCurIx->GetLastKey( vpCurIxTag, 1 );
  else
    return XB_INVALID_TAG;
}
/************************************************************************/
xbInt16 xbDbf::GetLastKey( xbIx *pIx, void *vpTag ){
  if( pIx )
    return pIx->GetLastKey( vpTag, 1 );
  else
    return XB_INVALID_TAG;
}
/************************************************************************/
xbInt16 xbDbf::GetNextKey(){
  if( pCurIx )
    return pCurIx->GetNextKey( vpCurIxTag, 1 );
  else
    return XB_INVALID_TAG;
}
/************************************************************************/
xbInt16 xbDbf::GetNextKey( xbIx *pIx, void *vpTag ){
  if( pIx )
    return pIx->GetNextKey( vpTag, 1 );
  else
    return XB_INVALID_TAG;
}
/************************************************************************/
xbInt16 xbDbf::GetPrevKey(){
  if( pCurIx )
    return pCurIx->GetPrevKey( vpCurIxTag, 1 );
  else
    return XB_INVALID_TAG;
}
/************************************************************************/
xbInt16 xbDbf::GetPrevKey( xbIx *pIx, void *vpTag ){

  if( pIx )
    return pIx->GetPrevKey( vpTag, 1 );
  else
    return XB_INVALID_TAG;
}
/************************************************************************/
xbInt16 xbDbf::Find( xbString &sKey ){
  if( pCurIx )
    return pCurIx->FindKey( vpCurIxTag, sKey.Str(), (xbInt32) sKey.Len(), 1 );
  else
    return XB_INVALID_TAG;
}
/************************************************************************/
xbInt16 xbDbf::Find( xbDate &dtKey ){
 if( pCurIx )
    return pCurIx->FindKey( vpCurIxTag, dtKey, 1 );
  else
    return XB_INVALID_TAG;
}
/************************************************************************/
xbInt16 xbDbf::Find( xbDouble &dKey ){
 if( pCurIx )
    return pCurIx->FindKey( vpCurIxTag, dKey, 1 );
  else
    return XB_INVALID_TAG;
}
/************************************************************************/
xbInt16 xbDbf::Find( xbIx *pIx, void *vpTag, const xbString &sKey ){
  if( pIx ){
    return pIx->FindKey( vpTag, sKey.Str(), (xbInt32) sKey.Len(), 1 );
  } else
    return XB_INVALID_TAG;
}
/************************************************************************/
xbInt16 xbDbf::Find( xbIx *pIx, void *vpTag, const xbDate &dtKey ){
 if( pIx )
    return pIx->FindKey( vpTag, dtKey, 1 );
  else
    return XB_INVALID_TAG;
}
/************************************************************************/
xbInt16 xbDbf::Find( xbIx *pIx, void *vpTag, const xbDouble &dKey ){
 if( pIx )
    return pIx->FindKey( vpTag, dKey, 1 );
  else
    return XB_INVALID_TAG;
}
#endif // XB_INDEX_SUPPORT

/************************************************************************/
xbBool xbDbf::GetBof() {
  xbUInt32 ulRecCnt;
  xbInt16 iRc = GetRecordCnt( ulRecCnt );
  if( iRc != XB_NO_ERROR || ulRecCnt == 0 || GetCurRecNo() == 1 )
    return xbTrue;
  else
    return xbFalse;
}
/************************************************************************/
xbBool xbDbf::GetEof() {
  xbUInt32 ulRecCnt;
  xbInt16 iRc = GetRecordCnt( ulRecCnt );
  if( iRc != XB_NO_ERROR || ulRecCnt == 0 || GetCurRecNo() == ulRecCnt )
    return xbTrue;
  else
    return xbFalse;
}
/************************************************************************/
xbUInt32 xbDbf::GetCurRecNo() const {
  return ulCurRec;
}
/************************************************************************/
xbInt16 xbDbf::GetDbfStatus() const {
  return iDbfStatus;
}
/************************************************************************/
xbInt32 xbDbf::GetFieldCnt() const {
  return iNoOfFields;
}
/************************************************************************/
#ifdef XB_LOCKING_SUPPORT
xbLinkListNode<xbUInt32> * xbDbf::GetFirstRecLock() const {
  return lloRecLocks.GetHeadNode();
}
#endif // XB_LOCKING_SUPPORT
/************************************************************************/
xbInt16 xbDbf::GetFirstRecord()
{
  return GetFirstRecord( XB_ACTIVE_RECS );
}
/************************************************************************/
xbInt16 xbDbf::GetFirstRecord( xbInt16 iOption )
{
  if( ulNoOfRecs == 0 )
    return XB_EMPTY;

  xbInt16 iRc = GetRecord( 1L );
  while(  iRc == XB_NO_ERROR && 
        ((RecordDeleted() && iOption == XB_ACTIVE_RECS) || 
        (!RecordDeleted() && iOption == XB_DELETED_RECS)))
    if( ulCurRec < ulNoOfRecs )
      iRc = GetRecord( ulCurRec + 1 );
    else
      return XB_EOF;

  return iRc;
}
/************************************************************************/
#ifdef XB_LOCKING_SUPPORT
xbBool xbDbf::GetHeaderLocked() const {
  return this->bHeaderLocked;
}
#endif   // XB_LOCKING_SUPPORT

#ifdef XB_INDEX_SUPPORT

xbIxList *xbDbf::GetIxList() const{
  return ixList;
}
#endif // XB_INDEX_SUPPORT
/************************************************************************/
xbInt16 xbDbf::GetLastRecord()
{
  return GetLastRecord( XB_ACTIVE_RECS );
}
/************************************************************************/
xbInt16 xbDbf::GetLastRecord( xbInt16 iOption )
{
  if( ulNoOfRecs == 0 )
    return XB_EMPTY;
  xbInt16 iRc = GetRecord( ulNoOfRecs );
  while(  iRc == XB_NO_ERROR &&
        ((RecordDeleted() && iOption == XB_ACTIVE_RECS) || 
        (!RecordDeleted() && iOption == XB_DELETED_RECS)))
    if( ulCurRec > 1 )
      iRc = GetRecord( ulCurRec - 1 );
    else
      return XB_EOF;
  return iRc;
}
/************************************************************************/
#ifdef XB_LOCKING_SUPPORT
xbInt16 xbDbf::GetLockFlavor() const{
  if( iLockFlavor == -1 )
    return xbase->GetDefaultLockFlavor();
  else
    return iLockFlavor;
}
#endif   // XB_LOCKING_SUPPORT

/************************************************************************/
#ifdef XB_MEMO_SUPPORT
#ifdef XB_LOCKING_SUPPORT
xbBool xbDbf::GetMemoLocked() const {
  if( MemoFieldsExist())
    return Memo->GetMemoLocked();
  else
    return xbFalse;
}
#endif  // XB_LOCKING_SUPPORT
/************************************************************************/
xbMemo * xbDbf::GetMemoPtr(){
  return Memo;
}
#endif  // XB_MEMO_SUPPORT
/************************************************************************/
#ifdef XB_INF_SUPPORT
xbInt16 xbDbf::GetInfFileName( xbString &sInfFileName ){

  sInfFileName = GetFqFileName();
  xbUInt32 lLen = sInfFileName.Len();
  if( lLen < 5 )
    return XB_FILE_NOT_FOUND;
  sInfFileName.PutAt(lLen-2, 'I');
  sInfFileName.PutAt(lLen-1, 'N');
  sInfFileName.PutAt(lLen,   'F');
  return XB_NO_ERROR;
}
/************************************************************************/
xbLinkListNode<xbString> * xbDbf::GetInfList() const{
  return llInfData.GetHeadNode();
}
#endif  // XB_INF_SUPPORT
/************************************************************************/
xbInt16 xbDbf::GetMultiUser( xbInt16 iOpt ) const{

  if( iOpt == 0 && this->iMultiUser == xbSysDflt )
    return xbase->GetMultiUser();
  else
    return this->iMultiUser;
}
/************************************************************************/
xbInt16 xbDbf::GetNextRecord(){
  return GetNextRecord( XB_ACTIVE_RECS );
}
/************************************************************************/
xbInt16 xbDbf::GetNextRecord( xbInt16 iOption ){
  if( ulNoOfRecs == 0 ) 
    return XB_EMPTY;
  else if( ulCurRec >= ulNoOfRecs ) 
    return XB_EOF;
  xbInt16 iRc = GetRecord( ulCurRec + 1 );
  while(  iRc == XB_NO_ERROR && 
        ((RecordDeleted() && iOption == XB_ACTIVE_RECS) || 
        (!RecordDeleted() && iOption == XB_DELETED_RECS)))
    if( ulCurRec < ulNoOfRecs )
      iRc = GetRecord( ulCurRec + 1 );
    else
      return XB_EOF;
  return iRc;
}
/************************************************************************/
xbInt16 xbDbf::GetNextRecord( xbInt16 iOption , xbUInt32 ulStartRec ){
  if( iOption == 0 )
    return GetNextRecord();
  else if( iOption == 1 ){
    if( ulStartRec > 0 )
      ulCurRec = ulStartRec;
    xbInt16 iRc = GetNextRecord();
    while( iRc == XB_NO_ERROR && RecordDeleted())
      iRc = GetNextRecord();
    return iRc;
  }
  else
    return XB_INVALID_OPTION;
}
/************************************************************************/
#ifdef XB_INDEX_SUPPORT
xbInt32 xbDbf::GetPhysicalIxCnt() const {

  xbInt32 lCnt = 0;
  #ifdef XB_INDEX_SUPPORT
  xbIxList *p = ixList;
  while( p ){
    lCnt++;
    p = p->next;
  }
  #endif
  return lCnt;
}
#endif  // XB_INDEX_SUPPORT
/************************************************************************/
xbInt16 xbDbf::GetPrevRecord()
{
  return GetPrevRecord( XB_ACTIVE_RECS );
}
/************************************************************************/
xbInt16 xbDbf::GetPrevRecord( xbInt16 iOption ){
  if( ulNoOfRecs == 0 ) 
    return XB_EMPTY;
  else if( ulCurRec <= 1L ) 
    return XB_BOF;
  xbInt16 iRc = GetRecord( ulCurRec - 1 );
  while(  iRc == XB_NO_ERROR && 
        ((RecordDeleted() && iOption == XB_ACTIVE_RECS) || 
        (!RecordDeleted() && iOption == XB_DELETED_RECS)))
    if( ulCurRec > 1 )
      iRc = GetRecord( ulCurRec - 1 );
    else
      return XB_BOF;

  return iRc;
}
/************************************************************************/
xbInt16 xbDbf::GetRecord( xbUInt32 ulRecNo ){
  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;

  try{
    /* verify the file is open */
    if( iDbfStatus == XB_CLOSED ){
      iErrorStop = 100;
      iRc = XB_NOT_OPEN;
      throw iRc;
    }
    if( iDbfStatus == XB_UPDATED ){
      if(  GetAutoCommit() == 1 ){
        if(( iRc = Commit()) != XB_NO_ERROR ){
          iErrorStop = 110;
          throw iRc;
        }
      } else {
        if(( iRc = Abort()) != XB_NO_ERROR ){
          iErrorStop = 120;
          throw iRc;
        }
      }
    }
    if( ulRecNo > ulNoOfRecs || ulRecNo == 0L ){
      iErrorStop = 130;
      iRc = XB_INVALID_RECORD;
      throw iRc;
    }
    #ifdef XB_BLOCKREAD_SUPPORT
    if( bBlockReadEnabled )
      return pRb->GetRecord( ulRecNo );
    #endif // XB_BLOCK_READ_SUPPORT

    if(( xbFseek( (uiHeaderLen+(( (xbInt64) ulRecNo-1L ) * uiRecordLen )), SEEK_SET )) != XB_NO_ERROR ){
      iErrorStop = 140;
      iRc = XB_SEEK_ERROR;
      throw iRc;
    }
    if( xbFread( RecBuf, uiRecordLen, 1 ) != XB_NO_ERROR ){
      iErrorStop = 150;
      iRc = XB_READ_ERROR;
      throw iRc;
    }
    ulCurRec = ulRecNo;
  }

  catch (xbInt16 iRc ){
    if( iRc != XB_LOCK_FAILED ){
      xbString sMsg;
      sMsg.Sprintf( "xbDbf::GetRecord()  Exception Caught. Error Stop = [%d] iRc = [%d] record = [%d]", iErrorStop, iRc, ulRecNo );
      xbase->WriteLogMessage( sMsg.Str() );
      xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
    }
  }
  return iRc;
}
/************************************************************************/
char * xbDbf::GetRecordBuf( xbInt16 iOpt ) const {
  if( iOpt )
    return RecBuf2;
  else
    return RecBuf;
}

/************************************************************************/
xbInt16 xbDbf::GetRecordCnt( xbUInt32 & ulRecCnt ) {
  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;

  #ifdef XB_LOCKING_SUPPORT
  xbBool  bLocked = xbFalse;
  #endif  // XB_LOCKING_SUPPORT

  try{
    #ifdef XB_LOCKING_SUPPORT
    //if( iAutoLock && !bTableLocked ){
    if((GetMultiUser() == xbOn) && !bTableLocked ){
      if(( iRc = LockHeader( XB_LOCK )) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      } else 
        bLocked = xbTrue;
    }
    #endif // XB_LOCKING_SUPPORT

    if((iRc = ReadHeader(1,1)) != XB_NO_ERROR){
      iErrorStop = 110;
      throw iRc;
    }
  }
  catch( xbInt16 iRc ){
    if( iRc != XB_LOCK_FAILED ){
      xbString sMsg;
      sMsg.Sprintf( "xbDbf::GetRecordCnt()  Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
      xbase->WriteLogMessage( sMsg.Str() );
      xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
    }
  }

  #ifdef XB_LOCKING_SUPPORT
  if( bLocked ){
    LockHeader( XB_UNLOCK );
  }
  #endif

  ulRecCnt = ulNoOfRecs;
  return iRc;
}
/************************************************************************/
xbUInt16 xbDbf::GetRecordLen() const {
  return uiRecordLen;
}
/************************************************************************/
#ifdef XB_LOCKING_SUPPORT
xbBool xbDbf::GetTableLocked() const {
  return this->bTableLocked;
}
#endif  // XB_LOCKING_SUPPORT
/************************************************************************/
#ifdef XB_INDEX_SUPPORT
xbLinkListNode<xbTag *> *xbDbf::GetTagList() const {
  return llTags.GetHeadNode();
}
#endif  // XB_INDEX_SUPPORT
/************************************************************************/
const xbString & xbDbf::GetTblAlias() const {
  return this->sAlias;
}
/************************************************************************/
xbXBase * xbDbf::GetXbasePtr() const {
  return xbase;
}
/************************************************************************/
#ifdef XB_INF_SUPPORT
xbInt16 xbDbf::LoadInfData(){

  xbInt16  iRc = 0;
  xbInt16  iErrorStop = 0;

  try{
    // create file name
    xbString sInfFileName;
    if(( iRc = GetInfFileName( sInfFileName )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }
    // if file does not exist, return no error
    xbFile fMd( xbase );
    if( !fMd.FileExists( sInfFileName ))
      return XB_NO_ERROR;

    // open file file in read only mode
    if(( iRc = fMd.xbFopen( "r", sInfFileName, GetShareMode())) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }
    // clear the linked list
    llInfData.Clear();

    // for each entry in the file, add a linked list item
    xbString sRec;
    xbString sLeft3;
    xbString sFn;

    while( iRc == XB_NO_ERROR ){
      sRec = "";
      if(( iRc = fMd.xbFgets( 132, sRec )) == XB_NO_ERROR ){
        sLeft3 = sRec;
        sLeft3.Left( 3 );
        sLeft3.ToUpperCase();
        if( sLeft3 == "NDX"){
          sFn.ExtractElement(sRec.Str(), '=', 2 );
          sFn.ZapChar( 0x0d );
          sFn.ZapChar( 0x0a );
          llInfData.InsertAtEnd( sFn );  
        }
      }
    }
    // close the file
    if(( iRc = fMd.xbFclose()) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw iRc;
    }

 } catch( xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbdbf::LoadInfData() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
#endif  // XB_INF_SUPPORT

/************************************************************************/
#ifdef XB_LOCKING_SUPPORT
xbInt16 xbDbf::LockAppend( xbInt16 iLockFunction )
{
  xbInt16  iRc = 0;
  xbInt16  iErrorStop = 0;
  xbUInt32 ulAppendRec;

  try{
    if( iLockFunction == XB_LOCK ){
      iErrorStop = 100;
      if( ulAppendLocked > 0 )         /* already have an append lock */
        return XB_NO_ERROR;

      ulAppendRec = ulNoOfRecs + 1;    /* record number needing to be locked */
      if( GetLockFlavor() == LK_DBASE ){
        iRc = xbLock( XB_LOCK, LK4026531839, 1 );
        if( iRc != XB_NO_ERROR ){
          if( iRc == XB_LOCK_FAILED )
            return iRc;
          else{
            iErrorStop = 110;
            throw iRc;
          }
        }

        xbInt64 llAppendRecLockByte = (xbInt64) LK4026531838 - ulAppendRec;
        iRc = xbLock( XB_LOCK, llAppendRecLockByte, 1 );
        if( iRc != XB_NO_ERROR ){
          xbLock( XB_UNLOCK, LK4026531839, 1 );
          if( iRc == XB_LOCK_FAILED ){
            return iRc;
          } else {
            iErrorStop = 120;
            throw iRc;
          }
        }
        ulAppendLocked = ulAppendRec;    /* set the append lock switch */


   // } else { - other lock-table flavor options go here Clipper, Fox, etc - }

      } else {
        iErrorStop = 130;
        iRc = XB_INVALID_LOCK_OPTION;
        throw iRc;
      }

    } else if( iLockFunction == XB_UNLOCK ){
      iErrorStop = 200;

      if( ulAppendLocked == 0 )        /* verify we have an active append lock */
        return XB_NO_ERROR;

      if( GetLockFlavor() == LK_DBASE ){
        xbInt64 llAppendRecLockByte =(xbInt64) LK4026531838 - ulAppendLocked;
        iRc = xbLock( XB_UNLOCK,  llAppendRecLockByte, 1 );
        if( iRc != XB_NO_ERROR ){
          xbLock( XB_UNLOCK, LK4026531839, 1 );
          iErrorStop = 220;
          throw iRc;
        }
        iRc = xbLock( XB_UNLOCK, LK4026531839, 1 );
        if( iRc != XB_NO_ERROR ){
          iErrorStop = 230;
          throw iRc;
        }

        ulAppendLocked = 0;             /* release the append lock switch */

   // } else { - other unlock-table flavor options go here Clipper, Fox, etc - }

      } else {
        iErrorStop = 290;
        iRc = XB_INVALID_LOCK_OPTION;
        throw iRc;
      }
    } else {
      iErrorStop = 300;
      iRc = XB_INVALID_LOCK_OPTION;
      throw iRc;
    }
  } catch( xbInt16 iRc ){
    if( iRc != XB_LOCK_FAILED ){
      xbString sMsg;
      sMsg.Sprintf( "xbdbf::LockAppendBytes() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
      xbase->WriteLogMessage( sMsg.Str() );
      xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
    }
  }
  return iRc;
}
/************************************************************************/
xbInt16 xbDbf::LockHeader( xbInt16 iLockFunction )
{
  xbInt16  iRc = 0;
  xbInt16  iErrorStop = 0;
  try{

    if( iLockFunction == XB_LOCK ){
      iErrorStop = 100;
      if( GetHeaderLocked())
        return XB_NO_ERROR; 

      iErrorStop = 110;
      if( GetLockFlavor() == LK_DBASE ){

        iRc = xbLock( XB_LOCK, LK4026531838, 1 );

        if( iRc != XB_NO_ERROR ){
          if( iRc == XB_LOCK_FAILED )
            return iRc;
          else{
            iErrorStop = 120;
            throw iRc;
          }
        }
      } else {
        iErrorStop = 130;
        iRc = XB_INVALID_LOCK_OPTION;
        throw iRc;
      }

      SetHeaderLocked( xbTrue );

    } else if( iLockFunction == XB_UNLOCK ){

      iErrorStop = 200;
      if( !GetHeaderLocked())
        return XB_NO_ERROR;

      iErrorStop = 210;
      if( GetLockFlavor() == LK_DBASE ){
        iRc = xbLock( XB_UNLOCK, LK4026531838, 1 );
        if( iRc != XB_NO_ERROR ){
          iErrorStop = 220;
          throw iRc;
        }
      } else {
        iErrorStop = 230;
        iRc = XB_INVALID_LOCK_OPTION;
        throw iRc;
      }
      SetHeaderLocked( xbFalse );
    } else {
      iErrorStop = 300;
      iRc = XB_INVALID_LOCK_OPTION;
      throw iRc;
    }
  } catch (xbInt16 iRc ){
    if( iRc != XB_LOCK_FAILED ){
      xbString sMsg;
      sMsg.Sprintf( "xbdbf::LockHeader() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
      xbase->WriteLogMessage( sMsg.Str() );
      xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
    }
  }
  return iRc;
}
/************************************************************************/
#ifdef XB_INDEX_SUPPORT
xbInt16 xbDbf::LockIndices( xbInt16 iLockFunction )
{
  // this function doesn't take into account any Lock Flavors other than dBASE,
  //  would need updated to support other lock flavors - Clipper, FoxPro etc

  xbInt16  iRc = 0;
  xbInt16  iErrorStop = 0;

  try{
    xbIxList *ixLI = GetIxList();   // index list item

    while( ixLI ){
      if( iLockFunction == XB_LOCK ){

        #ifdef XB_NDX_SUPPORT
        if( *ixLI->sFmt == "NDX" ){
          if( !ixLI->ix->GetLocked()){
            if(( iRc = ixLI->ix->xbLock( XB_LOCK, LK4026531838, 1 )) != XB_NO_ERROR ){
              ixLI->ix->xbLock( XB_UNLOCK, LK4026531838, 1 );
              iErrorStop = 100;
              throw iRc;
            }
            ixLI->ix->SetLocked( xbTrue );
          }
        }
        #endif

        #ifdef XB_MDX_SUPPORT
        if( *ixLI->sFmt == "MDX" ){
          if( !ixLI->ix->GetLocked()){
            if(( iRc = ixLI->ix->xbLock( XB_LOCK, LK4026531838, 1 )) != XB_NO_ERROR ){
              ixLI->ix->xbLock( XB_UNLOCK, LK4026531838, 1 );
              iErrorStop = 100;
              throw iRc;
            }
            ixLI->ix->SetLocked( xbTrue );
          }
        }
        #endif

        #ifdef XB_TDX_SUPPORT
        if( *ixLI->sFmt == "TDX" ){
          if( !ixLI->ix->GetLocked()){
            if(( iRc = ixLI->ix->xbLock( XB_LOCK, LK4026531838, 1 )) != XB_NO_ERROR ){
              ixLI->ix->xbLock( XB_UNLOCK, LK4026531838, 1 );
              iErrorStop = 100;
              throw iRc;
            }
            ixLI->ix->SetLocked( xbTrue );
          }
        }
        #endif

      } else if( iLockFunction == XB_UNLOCK ){

        #ifdef XB_NDX_SUPPORT
        if( *ixLI->sFmt == "NDX" ){
          if( ixLI->ix->GetLocked()){
            if(( iRc = ixLI->ix->xbLock( XB_UNLOCK, LK4026531838, 1 )) != XB_NO_ERROR ){
              iErrorStop = 100;
              throw iRc;
            }
            ixLI->ix->SetLocked( xbFalse );
          }
        }
        #endif

        #ifdef XB_MDX_SUPPORT
        if( *ixLI->sFmt == "MDX" ){
          if( ixLI->ix->GetLocked()){
            if(( iRc = ixLI->ix->xbLock( XB_UNLOCK, LK4026531838, 1 )) != XB_NO_ERROR ){
              iErrorStop = 100;
              throw iRc;
            }
            ixLI->ix->SetLocked( xbFalse );
          }
        }
        #endif

        #ifdef XB_TDX_SUPPORT
        if( *ixLI->sFmt == "MDX" ){
          if( ixLI->ix->GetLocked()){
            if(( iRc = ixLI->ix->xbLock( XB_UNLOCK, LK4026531838, 1 )) != XB_NO_ERROR ){
              iErrorStop = 100;
              throw iRc;
            }
            ixLI->ix->SetLocked( xbFalse );
          }
        }
        #endif

      }
      ixLI = ixLI->next;
    }

  } catch( xbInt16 iRc ){
    if( iRc != XB_LOCK_FAILED ){
      xbString sMsg;
      sMsg.Sprintf( "xbdbf::LockIndices() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
      xbase->WriteLogMessage( sMsg.Str() );
      xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
    }
  }
  return iRc;
}
#endif // XB_INDEX_SUPPORT
/************************************************************************/
#ifdef XB_MEMO_SUPPORT
xbInt16 xbDbf::LockMemo( xbInt16 iLockFunction ){
  if( MemoFieldsExist())
    return Memo->LockMemo( iLockFunction );
  else
    return XB_NO_ERROR;
}
#endif  // XB_MEMO_SUPPORT
/************************************************************************/
xbInt16 xbDbf::LockRecord( xbInt16 iLockFunction, xbUInt32 ulRecNo )
{
  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;

  try{

    if( ulRecNo > ulNoOfRecs )
      return XB_INVALID_RECORD;

    if( iLockFunction == XB_LOCK ){
      iErrorStop = 100;

      if( lloRecLocks.KeyExists( ulRecNo ))
        return XB_NO_ERROR;

      if( GetLockFlavor() == LK_DBASE ){

        iRc = xbLock( XB_LOCK, LK4026531838 - ulRecNo, 1 );
        if( iRc != XB_NO_ERROR ){
          if( iRc == XB_LOCK_FAILED ){
            return iRc;
          } else {
            iErrorStop = 110;
            throw iRc;
          }
        } 
        // other lock-table flavor options go here Clipper, Fox, etc

      } else {
        iErrorStop = 120;
        iRc = XB_INVALID_LOCK_OPTION;
        throw iRc;
      }

      // add the record lock info to the linked list chain of record locks
      iRc = lloRecLocks.InsertKey( ulRecNo );
      if( iRc != XB_NO_ERROR ){
        xbLock( XB_UNLOCK,  LK4026531838 - ulRecNo, 1 );
        iErrorStop = 130;
        throw iRc;
      }

    } else if( iLockFunction == XB_UNLOCK ){

      iErrorStop = 200;

      if( !lloRecLocks.KeyExists( ulRecNo ) )
        return XB_NO_ERROR;

      if( GetLockFlavor() == LK_DBASE ){
        iRc = xbLock( XB_UNLOCK,  LK4026531838 - ulRecNo, 1 );
        if( iRc != XB_NO_ERROR ){
          iErrorStop = 210;
          throw iRc;
        }
      } else {
        iErrorStop = 220;
        iRc = XB_INVALID_LOCK_OPTION;
        throw iRc;
      }

      // remove the record lock info to the linked list chain of record locks
      // next line is crashing
      iRc = lloRecLocks.RemoveKey( ulRecNo );
      if( iRc != XB_NO_ERROR ){
        iErrorStop = 230;
        throw iRc;
      }

    } else {
      iErrorStop = 300;
      iRc = XB_INVALID_LOCK_OPTION;
      throw iRc;
    }

  } catch( xbInt16 iRc ){
    if( iRc != XB_LOCK_FAILED ){
      xbString sMsg;
      sMsg.Sprintf( "xbdbf::LockRecord() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
      xbase->WriteLogMessage( sMsg.Str() );
      xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
    }
  }
  return iRc;
}
/************************************************************************/
xbInt16 xbDbf::LockTable( xbInt16 iLockFunction )
{
  xbInt16  iRc = 0;
  xbInt16  iErrorStop = 0;

  try{
    if( iLockFunction == XB_LOCK ){
      iErrorStop = 100;
      if( GetTableLocked())
        return XB_NO_ERROR;  // table already locked 

      iErrorStop = 110;
      if( GetLockFlavor() == LK_DBASE ){

        // lOffset = LK4026531838;
        // iLen = 2;
        iRc = xbLock( XB_LOCK, LK4026531838, 2 );
        if( iRc != XB_NO_ERROR ){
          if( iRc == XB_LOCK_FAILED )
            return iRc;
          else{
            iErrorStop = 120;
            throw iRc;
          }
        }

        // lOffset = LK3026531838;
        // iLen = LK1000000000;
        iRc = xbLock( XB_LOCK, LK3026531838, LK1000000000);
        if( iRc != XB_NO_ERROR ){

          // lOffset = LK4026531838;
          // iLen = 2;
          xbLock( XB_UNLOCK, LK4026531838, 2 );
          if( iRc == XB_LOCK_FAILED ){
            return iRc;
          } else {
            iErrorStop = 130;
            throw iRc;
          }
        }

        // iRc = xbLock( XB_UNLOCK, lOffset, iLen );
        iRc = xbLock( XB_UNLOCK, LK3026531838, LK1000000000);
        if( iRc != XB_NO_ERROR ){
          // lOffset = LK4026531838;
          // iLen = 2;
          xbLock( XB_UNLOCK, LK4026531838, 2 );
          iErrorStop = 140;
          throw iRc;
        }

        // other lock-table flavor options go here Clipper, Fox, etc

      } else {
        iErrorStop = 190;
        iRc = XB_INVALID_LOCK_OPTION;
        throw iRc;
      }
      SetTableLocked( xbTrue );

    } else if( iLockFunction == XB_UNLOCK ){

      iErrorStop = 200;
      if( !GetTableLocked())
        return XB_NO_ERROR;  // table already unlocked 

      iErrorStop = 210;
      if( GetLockFlavor() == LK_DBASE ){

        // lOffset = LK4026531838;
        // iLen = 2;
        iRc = xbLock( XB_UNLOCK, LK4026531838, 2 );
        if( iRc != XB_NO_ERROR ){
          iErrorStop = 220;
          throw iRc;
        }
      } else {
        iErrorStop = 290;
        iRc = XB_INVALID_LOCK_OPTION;
        throw iRc;
      }
      SetTableLocked( xbFalse );

    } else {
      iErrorStop = 300;
      iRc = XB_INVALID_LOCK_OPTION;
      throw iRc;
    }

  } catch (xbInt16 iRc ){
    if( iRc != XB_LOCK_FAILED ){
      xbString sMsg;
      sMsg.Sprintf( "xbdbf::LockFile() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
      xbase->WriteLogMessage( sMsg.Str() );
      xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
    }
  }
  return iRc;

}
#endif  // XB_LOCKING_SUPPORT
/************************************************************************/
xbBool xbDbf::MemoFieldsExist() const {
#ifdef XB_MEMO_SUPPORT
  if( iMemoFieldCnt > 0 )
    return xbTrue;
#endif
  return xbFalse;
}

/************************************************************************/
xbInt16 xbDbf::Open( const xbString & sTableName ) {
  return Open( sTableName, sTableName );
}
/************************************************************************/
xbInt16 xbDbf::Open( const xbString & sTableName, const xbString & sAlias ){

  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;

  try{
    if(( iRc = Open( sTableName, sAlias, XB_READ_WRITE, XB_MULTI_USER )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }

    // do any .INF data things on the file, like open indices
    #ifdef XB_INF_SUPPORT
    if(( iRc = LoadInfData()) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }
    xbUInt32 llNodeCnt = llInfData.GetNodeCnt();
    if( llNodeCnt > 0 ){
      xbString s2;
      xbLinkListNode<xbString> * llN = llInfData.GetHeadNode();
      for( xbUInt32 i = 0; i < llNodeCnt; i++ ){
        s2 = llN->GetKey();
        #ifdef XB_NDX_SUPPORT
        if(( iRc = OpenIndex( "NDX", s2 )) != XB_NO_ERROR ){
          iErrorStop = 120;
          throw iRc ;
        }
        #endif  // XB_NDX_SUPPORT
        llN = llN->GetNextNode();
      }
    }
    #endif // XB_INF_SUPPORT
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbdbf::Open() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
/************************************************************************/
#ifdef XB_INDEX_SUPPORT

xbInt16 xbDbf::OpenIndex( const xbString &sIxType, const xbString &sFileName ){

  // this routine is used to open indices and set up linkages

  xbInt16 iErrorStop = 0;
  xbInt16 iRc = XB_NO_ERROR;
  xbIx *pIx = NULL;

  try{
    xbString sType = sIxType;
    sType.ToUpperCase();

    if( sType == "" ){
      iErrorStop = 100;
      iRc = XB_INVALID_OPTION;
      throw iRc;

    #ifdef XB_NDX_SUPPORT
    } else if( sType == "NDX" ){
      pIx = new xbIxNdx( this );
      if(( iRc = pIx->Open( sFileName )) != XB_NO_ERROR ){
        iErrorStop = 110;
        throw iRc;
      }
    #endif

    #ifdef XB_MDX_SUPPORT
    } else if( sType == "MDX" ){
      pIx = new xbIxMdx( this );
      if(( iRc = pIx->Open( sFileName )) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }

    #endif

    } else {
      iErrorStop = 130;
      iRc = XB_INVALID_OPTION;
      throw iRc;
    }

    if(( iRc = AddIndex( pIx, sIxType )) != XB_NO_ERROR ){
      iErrorStop = 140;
      throw iRc;
    }
    if(( iRc = UpdateTagList()) != XB_NO_ERROR ){
      iErrorStop = 150;
      throw iRc;
    }
    pCurIx = pIx;
    sCurIxType = sIxType;
    vpCurIxTag = pIx->GetTag( 0 );
  }
  catch (xbInt16 iRc ){
    if( pIx ) delete pIx;
    xbString sMsg;
    sMsg.Sprintf( "xbdbf::OpenIndex() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
#endif // XB_INDEX_SUPPORT
/************************************************************************/
xbInt16 xbDbf::Pack()
{
  xbUInt32 ulDeletedRecCnt;
  return Pack( ulDeletedRecCnt );
}
/************************************************************************/
xbInt16 xbDbf::Pack( xbUInt32 &ulDeletedRecCnt )
{
  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;
  xbUInt32 ulLastMovedRec = 0;
  xbUInt32 ulStartPos = 0;
  xbUInt32 ulLastPackedRec = 0;
  xbUInt32 ulMoveRec = 0;
  xbUInt32 ulRecCnt = 0;
  ulDeletedRecCnt = 0;

  #ifdef XB_LOCKING_SUPPORT
  xbBool bLocked = xbFalse;
  #endif // XB_LOCKING_SUPPORT

  try{
    if( !FileIsOpen() ){
      iErrorStop = 100;
      iRc = XB_DBF_FILE_NOT_OPEN;
      throw iRc;
    }

    #ifdef XB_LOCKING_SUPPORT
    //if( iAutoLock && !bTableLocked ){
    if((GetMultiUser() == xbOn) && !bTableLocked ){
      if(( iRc = LockTable( XB_LOCK )) != XB_NO_ERROR ){
        iErrorStop = 110;
        throw iRc;
      } else {
        bLocked = xbTrue;
      }
    }
    #endif

    if(( iRc = GetRecordCnt( ulRecCnt )) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw iRc;
    }

    xbBool bDone = xbFalse;
    for( xbUInt32 ulI = 1; ulI <= ulRecCnt && !bDone; ulI++ ){

      if(( iRc = GetRecord( ulI )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }
      if( RecordDeleted()){
        ulDeletedRecCnt++;
        if( ulI > ulLastMovedRec )
          ulStartPos = ulI;
        else
          ulStartPos = ulLastMovedRec;

        iRc = GetNextRecord( 1, ulStartPos );

        if( iRc == XB_NO_ERROR ){
          ulMoveRec = ulCurRec;
        }
        else if( iRc == XB_EOF ){
          ulMoveRec = 0;
          bDone = xbTrue;
        }
        else{
          iErrorStop = 140;
          throw iRc;
        }
        if( ulMoveRec > 0 ){
          if(( iRc = DeleteRecord()) != XB_NO_ERROR ){
            iErrorStop = 150;
            throw iRc;
          }
          if(( iRc = PutRecord( ulMoveRec )) != XB_NO_ERROR ){
            iErrorStop = 160;
            throw iRc;
          }
          if(( iRc = UndeleteRecord()) != XB_NO_ERROR ){
            iErrorStop = 170;
            throw iRc;
          }
          if(( iRc = PutRecord( ulI )) != XB_NO_ERROR ){
            iErrorStop = 180;
            throw iRc;
          }
          ulLastPackedRec = ulI;
        }

      } else {
        ulLastPackedRec = ulI;
      }
    }

    if( ulLastPackedRec < ulRecCnt ){
      // update header record count

      xbDate d;
      d.Sysdate();
      cUpdateYY = (char) d.YearOf() - 1900;
      cUpdateMM = (char) d.MonthOf();
      cUpdateDD = (char) d.DayOf( XB_FMT_MONTH );
      ulNoOfRecs = ulLastPackedRec;

      // rewrite the header record 
      if(( iRc = WriteHeader( 1, 1 )) != XB_NO_ERROR ){
        iErrorStop = 190;
        throw iRc;
      }

      // truncate the file to the new size 
      if(( iRc = xbTruncate( uiHeaderLen + uiRecordLen * ulLastPackedRec )) != XB_NO_ERROR ){
        iErrorStop = 200;
        throw iRc;
      }
    }

    if( ulNoOfRecs > 0 ){
      if(( iRc = GetRecord( 1 )) != XB_NO_ERROR ){
        iErrorStop = 210;
        throw iRc;
      }
    }  else {
      BlankRecord();
      ulCurRec = 0;
    }

    #ifdef XB_MEMO_SUPPORT
    if( iMemoFieldCnt > 0 ){
      if(( iRc = Memo->PackMemo( 0 )) != XB_NO_ERROR ){
        iErrorStop = 220;
        throw iRc;
      }
    }
    #endif // XB_MEMO_SUPPORT
  }
  catch (xbInt16 iRc ){
    if( iRc != XB_LOCK_FAILED ){
      xbString sMsg;
      sMsg.Sprintf( "xbdbf::Pack() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
      xbase->WriteLogMessage( sMsg.Str() );
      xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
    }
  }
  #ifdef XB_LOCKING_SUPPORT
  if( bLocked ){
    LockTable( XB_UNLOCK );
  }
  #endif
  return iRc;
}

/************************************************************************/
xbInt16 xbDbf::PutRecord() {
   return PutRecord(ulCurRec);
}
/************************************************************************/
xbInt16 xbDbf::PutRecord(xbUInt32 ulRecNo) 
{
  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  try{
    if( ulRecNo < 1 ){
      iErrorStop = 100;
      throw XB_INVALID_RECORD;
    }
    xbUInt32 ulRecCnt;
    if(( iRc = GetRecordCnt( ulRecCnt )) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }
    if( ulRecNo > ulRecCnt ){
      iErrorStop = 120;
      throw XB_INVALID_RECORD;
    }
    if( iDbfStatus == XB_CLOSED ){
      iErrorStop = 130;
      iRc = XB_NOT_OPEN;
      throw iRc;
    }
    /* lock the database */
    #ifdef XB_LOCKING_SUPPORT
    if( (GetMultiUser() == xbOn) && !bTableLocked ){
      if(( iRc = LockHeader( XB_LOCK )) != XB_NO_ERROR ){
         throw iRc;
      }
    }
    #endif     // XB_LOCKING_SUPPORT

    if(( iRc = ReadHeader( 1, 1 )) != XB_NO_ERROR ){
      iErrorStop = 150;
      throw iRc;
    }

    // verify valid record number request
    if( ulRecNo > ulNoOfRecs || ulRecNo == 0L ){
      iErrorStop = 160;
      iRc = XB_INVALID_RECORD;
      throw iRc;
    }

    #ifdef XB_LOCKING_SUPPORT
    if(( GetMultiUser() == xbOn) && !bTableLocked ){
      if(( iRc = LockRecord( XB_LOCK, ulRecNo )) != XB_NO_ERROR ){
        iErrorStop = 170;
        throw iRc;
        }

      #ifdef XB_INDEX_SUPPORT
      if(( iRc = LockIndices( XB_LOCK )) != XB_NO_ERROR ){
        iErrorStop = 180;
        throw iRc;
      }
      #endif  // XB_INDEX_SUPPORT
    }
    #endif  // XB_LOCKING_SUPPORT

    // build keys, check for duplicate keys, add keys
    #ifdef XB_INDEX_SUPPORT
    xbIxList *ixList = GetIxList();

    while( ixList ){
      if(( iRc = ixList->ix->CreateKeys( 2 )) != XB_NO_ERROR ){
        iErrorStop = 190;
        throw iRc;
      }
      iRc = ixList->ix->CheckForDupKeys();
      if( iRc != 0 ){
        if( iRc < 0 ){
          iErrorStop = 200;
          throw iRc;
        }
        throw XB_KEY_NOT_UNIQUE;
      }
      ixList = ixList->next;
    }

    ixList = GetIxList();
    while( ixList ){

      if(( iRc = ixList->ix->AddKeys( ulCurRec )) != XB_NO_ERROR ){
        iErrorStop = 210;
        throw iRc;
      }
      ixList = ixList->next;
    }

    ixList = GetIxList();
    while( ixList ){
      if(( iRc = ixList->ix->DeleteKeys()) != XB_NO_ERROR ){
        iErrorStop = 220;
        throw iRc;
      }
      ixList = ixList->next;
    }
    #endif     // XB_INDEX_SUPPORT

    // update latest header date if changed
    xbDate d;
    d.Sysdate();
    if( (cUpdateYY != (char)(d.YearOf() - 1900)) || (cUpdateMM != (char) d.MonthOf()) || (cUpdateDD != (char)d.DayOf( XB_FMT_MONTH))){
      cUpdateYY = (char) d.YearOf() - 1900;
      cUpdateMM = (char) d.MonthOf();
      cUpdateDD = (char) d.DayOf( XB_FMT_MONTH );
      // rewrite the header record - first 8 bytes
      if(( iRc = WriteHeader( 1, 1 )) != XB_NO_ERROR ){
        iErrorStop = 70;
        throw iRc;
      }
    }

    // update record
    iRc = xbFseek( (uiHeaderLen+(( (xbInt64) ulRecNo-1L ) * uiRecordLen )),0 );
    if( iRc != XB_NO_ERROR ){
      iErrorStop = 240;
      throw iRc;
    }

    if( xbFwrite( RecBuf, uiRecordLen, 1 ) != XB_NO_ERROR ){
      iErrorStop = 250;
      iRc = XB_WRITE_ERROR;
      throw iRc;
    }

    #ifdef XB_MEMO_SUPPORT
    if( MemoFieldsExist() ){
      if(( iRc = Memo->Commit()) != XB_NO_ERROR ){
        iErrorStop = 260;
        throw iRc;
      }
    }
    #endif

    ulCurRec = ulRecNo;
    iDbfStatus = XB_OPEN;
  }

  catch (xbInt16 iRc ){
    if( iRc != XB_LOCK_FAILED && iRc != XB_KEY_NOT_UNIQUE ){
      xbString sMsg;
      sMsg.Sprintf( "xbdbf::PutRecord() Exception Caught. Error Stop = [%d] iRc = [%d] record = [%d]", iErrorStop, iRc, ulRecNo );
      xbase->WriteLogMessage( sMsg.Str() );
      xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
    }
  }

  #ifdef XB_LOCKING_SUPPORT
  if( GetMultiUser() == xbOn ){
    LockHeader( XB_UNLOCK );
    LockAppend( XB_UNLOCK );
    LockRecord( XB_UNLOCK, ulRecNo );
    #ifdef XB_INDEX_SUPPORT
    LockIndices( XB_UNLOCK );
    #endif   //  XB_INDEX_SUPPORT
  }
  #endif   // XB_LOCKING_SUPPORT

  return iRc;
}

/************************************************************************/
xbInt16 xbDbf::ReadHeader( xbInt16 iPositionOption, xbInt16 iReadOption ){

  char buf[32];
  size_t iReadSize;

  if(iPositionOption)
    xbRewind();
  if( iReadOption == 1 )
    iReadSize = 8;
  else
    iReadSize = 32;

  if(xbFread(buf, iReadSize, 1) != XB_NO_ERROR)
    return XB_READ_ERROR;
  memcpy(&cVersion, buf, 4);
  ulNoOfRecs  = eGetUInt32(&buf[4]);
  if( iReadOption == 1 )
    return XB_NO_ERROR;

  uiHeaderLen      = eGetUInt16(&buf[8]);
  uiRecordLen      = eGetUInt16(&buf[10]);
  cTransactionFlag = buf[14];
  cEncryptionFlag  = buf[15];
  cIndexFlag       = buf[28];
  cLangDriver      = buf[29];
  return XB_NO_ERROR;
}

/************************************************************************/
xbInt16 xbDbf::RecordDeleted( xbInt16 iOpt ) const {
  if( !iOpt && RecBuf && RecBuf[0] == 0x2a )
    return xbTrue;
  else if( iOpt && RecBuf2 && RecBuf2[0] == 0x2a )
    return xbTrue;
  else
    return xbFalse;
}
/************************************************************************/
#ifdef XB_INDEX_SUPPORT
xbInt16 xbDbf::RemoveIndex( xbIx * ixIn ){

  xbIxList *p = ixList;
  // if index is the first entry in the list
  if( ixList->ix == ixIn ){
    ixList = ixList->next;
    delete p->sFmt;
    delete p->ix;
    free( p );
    return XB_NO_ERROR;
  }

  // spin down to the correct ix
  xbIxList *p2 = NULL;
  while( p && p->ix != ixIn ){
    p2 = p;
    p = p->next;
  }
  if( p ){
    p2->next = p->next;
    delete p->sFmt;
    delete p->ix;
    free( p );
  }
  return XB_NO_ERROR;
}
#endif // XB_INDEX_SUPPORT

/************************************************************************/
void xbDbf::ResetNoOfRecords() {
  ulNoOfRecs = 0UL;
}
/************************************************************************/
#ifdef XB_INF_SUPPORT
xbInt16 xbDbf::SaveInfData(){

  xbInt16  iRc = 0;
  xbInt16  iErrorStop = 0;
  xbFile fMd( xbase );

  try{
    xbUInt32 llNodeCnt = llInfData.GetNodeCnt();
    xbString sInfFileName;
    if(( iRc = GetInfFileName( sInfFileName )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }
    // open the file
    if(( iRc = fMd.xbFopen( "w", sInfFileName, GetShareMode())) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }
    xbString s1;
    xbString s2;
    s2.Sprintf( "[dBASE]%c%c", 0x0d, 0x0a );
    if(( iRc = fMd.xbFputs( s2 )) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw iRc;
    }

    // for each entry in the linked list, write a line
    xbLinkListNode<xbString> * llN = llInfData.GetHeadNode();
    for( xbUInt32 i = 0; i < llNodeCnt; i++ ){
      s2 = llN->GetKey();
      if( i > 0 )
        s1.Sprintf( "NDX%d=%s%c%c", i, s2.Str(), 0x0d, 0x0a );
          else
        s1.Sprintf( "NDX=%s%c%c", s2.Str(), 0x0d, 0x0a );

      if(( iRc = fMd.xbFputs( s1 )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }
      llN = llN->GetNextNode();
    }

    // close the file
    if(( iRc = fMd.xbFclose()) != XB_NO_ERROR ){
      iErrorStop = 140;
      throw iRc;
    }

  } catch( xbInt16 iRc ){
    if( fMd.FileIsOpen())
      fMd.xbFclose();
    xbString sMsg;
    sMsg.Sprintf( "xbdbf::SaveInfData() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
#endif // XB_INF_SUPPORT
/************************************************************************/
xbInt16 xbDbf::SetAutoCommit( xbBool iAutoCommit ) {
  if( iAutoCommit > 1 || iAutoCommit < -1 )
    return XB_INVALID_OPTION;
  this->iAutoCommit = iAutoCommit;
  return XB_NO_ERROR;
}
/************************************************************************/
xbInt16 xbDbf::SetMultiUser( xbInt16 iMultiUserOption ){

  if( iMultiUserOption != xbOn && iMultiUserOption != xbOff && iMultiUserOption != xbSysDflt )
    return XB_INVALID_OPTION;

  if( iDbfStatus != XB_CLOSED )
    return XB_NOT_CLOSED;

  this->iMultiUser = iMultiUserOption;
  return XB_NO_ERROR;
}

/************************************************************************/
#ifdef XB_INDEX_SUPPORT
xbInt16 xbDbf::SetCurTag( const xbString &sTagName ){
  if( sTagName == "" ){
    SetCurTag( "", 0, 0 );
    return XB_NO_ERROR;
  }  else {
    xbLinkListNode<xbTag *> *llN = GetTagList();
    xbTag *pTag;
    while( llN ){
      pTag = llN->GetKey();
      if( pTag->GetTagName() == sTagName ){
        SetCurTag( pTag->GetType(), pTag->GetIx(), pTag->GetVpTag());
        return XB_NO_ERROR;
      }
      llN = llN->GetNextNode();
    }
  }
  return XB_INVALID_TAG;
}
/************************************************************************/
void xbDbf::SetCurTag( const xbString &sIxType, xbIx *pIx, void *vpTag ){
    pCurIx     = pIx;
    vpCurIxTag = vpTag;
    sCurIxType.Set( sIxType );
}
#endif // XB_INDEX_SUPPORT

/************************************************************************/
#ifdef XB_LOCKING_SUPPORT
void xbDbf::SetHeaderLocked( xbBool bHeaderLocked ){
  this->bHeaderLocked = bHeaderLocked;
}
#endif

/************************************************************************/
#ifdef XB_LOCKING_SUPPORT
void xbDbf::SetLockFlavor( xbInt16 iLockFlavor ){
  this->iLockFlavor = iLockFlavor;
}
#endif

/************************************************************************/
#ifdef XB_LOCKING_SUPPORT
void xbDbf::SetTableLocked( xbBool bTableLocked ){
  this->bTableLocked = bTableLocked;
}
#endif
/************************************************************************/
xbInt16 xbDbf::UndeleteAllRecords(){
  return DeleteAll( 1 );
}
/************************************************************************/
xbInt16 xbDbf::UndeleteRecord()
{
  if( RecBuf && ulCurRec > 0 ){
    if( RecBuf[0] != 0x20 ){
      if( iDbfStatus != XB_UPDATED ){
        iDbfStatus = XB_UPDATED;
        memcpy( RecBuf2, RecBuf, uiRecordLen );  // save off original before making updates
      }
      RecBuf[0] = 0x20;
    }
    return XB_NO_ERROR;
  }
  else
    return XB_INVALID_RECORD;
}
/************************************************************************/
#ifdef XB_MEMO_SUPPORT
xbInt16 xbDbf::UpdateMemoField( xbInt16 iFieldNo, const xbString &sMemoData ){
  return Memo->UpdateMemoField( iFieldNo, sMemoData );
}
/************************************************************************/
xbInt16 xbDbf::UpdateMemoField( const xbString & sFieldName, const xbString & sMemoData ){
  return Memo->UpdateMemoField( GetFieldNo( sFieldName ), sMemoData );
}
#endif
/************************************************************************/
#ifdef XB_INDEX_SUPPORT

void xbDbf::UpdateSchemaIxFlag( xbInt16 iFldNo, unsigned char cVal ){
  if( cVal != 0x00 || cVal != 0x01 )
    SchemaPtr[iFldNo].cIxFlag = cVal;
}
/************************************************************************/
xbInt16 xbDbf::UpdateTagList(){

  xbInt16 iErrorStop = 0;
  xbInt16 iRc = XB_NO_ERROR;
  xbInt32 lTagCnt;

  try{
   ClearTagList();

   // For each active index
   xbIxList *p = GetIxList();
   xbIx *ixp;
   while( p ){
     ixp = p->ix;
     // for each tag within the file
     lTagCnt = ixp->GetTagCount();
     for( xbInt32 l = 0; l < lTagCnt; l++ ){
       xbTag *pTag = new xbTag( ixp, ixp->GetTag( l ), *p->sFmt, ixp->GetTagName( ixp->GetTag( l )),
            ixp->GetKeyExpression( ixp->GetTag( l )), ixp->GetKeyFilter( ixp->GetTag( l )),
            ixp->GetUnique( ixp->GetTag( l )), ixp->GetSortOrder( ixp->GetTag( l )));

       // append it to the llTags list
       llTags.InsertAtEnd( pTag );
      }
      p = p->next;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbdbf::UpdateTagList() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
#endif   // XB_INDEX_SUPPORT

/************************************************************************/
// @brief Write Header 
xbInt16 xbDbf::WriteHeader( xbInt16 iPositionOption, xbInt16 iWriteOption )
{
   char buf[32];
   xbInt32 lWriteLen;

   if(iPositionOption)
     xbRewind();

   memset(buf, 0, 32);
   if( iWriteOption == 1 )
     lWriteLen = 8;
   else{
     lWriteLen = 32;
     ePutUInt16( &buf[8],  uiHeaderLen );
     ePutUInt16( &buf[10], uiRecordLen );
     buf[14] = cTransactionFlag;
     buf[15] = cEncryptionFlag;
     buf[28] = cIndexFlag;
     buf[29] = cLangDriver;
   }
   memcpy(&buf[0],  &cVersion, 4);
   ePutUInt32( &buf[4],  ulNoOfRecs);
   if(xbFwrite(buf, (size_t) lWriteLen, 1) != XB_NO_ERROR)
     return XB_WRITE_ERROR;

   return XB_NO_ERROR;
}
/************************************************************************/
//! @brief Zap (remove) everything from the file,
xbInt16 xbDbf::Zap(){

  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;

  #ifdef XB_LOCKING_SUPPORT
  xbBool bLocked = xbFalse;
  #endif // XB_LOCKING_SUPPORT

  try{
    if( iDbfStatus != XB_OPEN ){
      iErrorStop = 100;
      iRc = XB_DBF_FILE_NOT_OPEN;
      throw iRc;
    }
    #ifdef XB_LOCKING_SUPPORT
    //if( iAutoLock ){
    if( GetMultiUser() == xbOn ){
      if(( iRc = LockTable( XB_LOCK )) != XB_NO_ERROR ){
        iErrorStop = 110;
        throw iRc;
      }
    }
    #endif

    xbDate d;
    d.Sysdate();
    cUpdateYY = (char) d.YearOf() - 1900;
    cUpdateMM = (char) d.MonthOf();
    cUpdateDD = (char) d.DayOf( XB_FMT_MONTH );
    ulNoOfRecs = 0;

    // rewrite the header record 
    if(( iRc = WriteHeader( 1, 1 )) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw iRc;
    }

    // truncate the file to the new size 
    if(( iRc = xbTruncate( uiHeaderLen  )) != XB_NO_ERROR ){
      iErrorStop = 130;
      throw iRc;
    }
    BlankRecord();
    ulCurRec = 0;

    #ifdef XB_MEMO_SUPPORT
    if( iMemoFieldCnt ){
      if(( iRc = Memo->Zap()) != XB_NO_ERROR ){
        iErrorStop = 140;
        throw iRc;
      }
    }
    #endif
    #ifdef XB_INDEX_SUPPORT
    xbLinkListNode<xbTag *> *llN = GetTagList();
    xbTag *pTag;
    xbIx  *pIx;
    void  *vpTag;
    while( llN ){
      pTag  = llN->GetKey();
      pIx   = pTag->GetIx();
      vpTag = pTag->GetVpTag();
      if(( iRc = pIx->ReindexTag( &vpTag )) != XB_NO_ERROR ){
        iErrorStop = 60;
        throw iRc;
      }
      llN = llN->GetNextNode();
    }

    #endif // XB_INDEX_SUPPORT
  }
  catch (xbInt16 iRc ){
    if( iRc != XB_LOCK_FAILED ){
      xbString sMsg;
      sMsg.Sprintf( "xbdbf::Zap() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
      xbase->WriteLogMessage( sMsg.Str() );
      xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
    }
  }
  #ifdef XB_LOCKING_SUPPORT
  if( bLocked ){
    LockTable( XB_UNLOCK );
  }
  #endif // XB_LOCKING_SUPPORT
  return iRc;
}
/************************************************************************/
#ifdef XB_BLOCKREAD_SUPPORT
xbInt16 xbDbf::DisableBlockReadProcessing(){

  if( bBlockReadEnabled ){
    bBlockReadEnabled = xbFalse;
    delete pRb;
    pRb = NULL;
  }
  return XB_NO_ERROR;
}

xbBool xbDbf::GetBlockReadStatus() const {
  return bBlockReadEnabled;
}

xbInt16 xbDbf::EnableBlockReadProcessing(){

  xbInt16  iRc = 0;
  xbInt16  iErrorStop = 0;

  try{

    if( !bBlockReadEnabled ){
      if( iDbfStatus == XB_UPDATED ){
        if(  GetAutoCommit() == 1 ){
          if(( iRc = Commit()) != XB_NO_ERROR ){
            iErrorStop = 100;
            throw iRc;
          }
        } else {
          if(( iRc = Abort()) != XB_NO_ERROR ){
            iErrorStop = 110;
            throw iRc;
          }
        }
      }
      pRb = new xbBlockRead( this );
      if( !pRb ){
        iErrorStop = 120;
        iRc = XB_NO_MEMORY;
        throw iRc;
      }
      if(( iRc = pRb->Init()) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }

      bBlockReadEnabled = xbTrue;
    }

  } catch( xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbdbf::EnableBlockReadProcessing() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}

#endif // XB_BLOCKREAD_SUPPORT
/************************************************************************/
}   /* namespace */