/* xbixndx.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023,2024 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

#include "xbase.h"

#ifdef XB_NDX_SUPPORT

namespace xb{


/***********************************************************************/
//! @brief Class constructor.
/*!
  @param dbf Pointer to dbf instance.
*/

xbIxNdx::xbIxNdx( xbDbf *dbf ) : xbIx( dbf ){
  ndxTag     = (xbNdxTag *) calloc( 1, sizeof( xbNdxTag ));
  SetBlockSize( XB_NDX_BLOCK_SIZE );
  cNodeBuf   = (char *) malloc( XB_NDX_BLOCK_SIZE );
  sIxType    = "NDX";
}
/***********************************************************************/
//! @brief Class Destructor.
xbIxNdx::~xbIxNdx(){
  if( ndxTag ){
    ndxTag->npNodeChain  = FreeNodeChain( ndxTag->npNodeChain );
    if( ndxTag->cpKeyBuf )
      free( ndxTag->cpKeyBuf );
    if( ndxTag->cpKeyBuf2 )
      free( ndxTag->cpKeyBuf2 );
    if( ndxTag->exp ){
      delete ndxTag->exp;
      ndxTag->exp = NULL;
    }
    ndxTag->sKeyExpression.Set( NULL );
    ndxTag->sTagName.Set( NULL );
    free( ndxTag );
    ndxTag = NULL;
  }
  if( cNodeBuf )
    free( cNodeBuf );
}
/***********************************************************************/
//! @brief Add key.
/*!
  Add key. If this is a unique index, this logic assumes the duplicate 
  check logic was already done.

  @param vpTag Tag to update.
  @param ulRecNo Record number to add key for.
  @returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbIxNdx::AddKey( void * vpTag, xbUInt32 ulRecNo ){

  xbNdxTag * npTag;
  vpTag ? npTag = (xbNdxTag *) vpTag : npTag = ndxTag;

  //if( GetUniqueKeyOpt() == XB_EMULATE_DBASE && npTag->bFoundSts )
  if( npTag->iIxTagMode == XB_IX_DBASE_MODE && npTag->bFoundSts )
    return XB_NO_ERROR;

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbInt16 iHeadNodeUpdateOpt = 2;

  try{

     if(( iRc = xbIxNdx::KeySetPosAdd( npTag, ulRecNo )) != XB_NO_ERROR ){
       iErrorStop = 100;
       throw iRc;
     }
     xbInt32 lKeyCnt = GetKeyCount( npTag->npCurNode );
     if( lKeyCnt <  npTag->iKeysPerBlock ){
        // Section A - add key to appropriate position if space available
        if(( iRc = InsertNodeL( npTag, npTag->npCurNode, npTag->npCurNode->iCurKeyNo, npTag->cpKeyBuf, ulRecNo )) != XB_NO_ERROR ){
          iErrorStop = 110;
          throw iRc;
        }

     } else {
       // land here with a full leaf node
       iHeadNodeUpdateOpt = 1;
       // section B - split the leaf node
       xbIxNode * npRightNode = AllocateIxNode( GetBlockSize() + (xbUInt32) npTag->iKeyItemLen, 1 );
       if( !npRightNode ){
         iErrorStop = 120;
         iRc = XB_NO_MEMORY;
         throw iRc;
       }
       if(( iRc = SplitNodeL( npTag, npTag->npCurNode, npRightNode, npTag->npCurNode->iCurKeyNo, npTag->cpKeyBuf, ulRecNo )) != XB_NO_ERROR ){
         iErrorStop = 130;
         throw iRc;
       }
       xbUInt32 ulTempBlockNo = npRightNode->ulBlockNo;

       // section C - go up the tree, splitting nodes as necessary
       xbIxNode * npParent = npTag->npCurNode->npPrev;
       while( npParent && GetKeyCount( npParent ) >= npTag->iKeysPerBlock ){
         npRightNode = FreeNodeChain( npRightNode );
         npRightNode = AllocateIxNode( GetBlockSize() + (xbUInt32) npTag->iKeyItemLen, 1 );
         if( !npRightNode ){
           iErrorStop = 140;
           iRc = XB_NO_MEMORY;
           throw iRc;
         }
         if(( iRc = SplitNodeI( npTag, npParent, npRightNode, npParent->iCurKeyNo, ulTempBlockNo )) != XB_NO_ERROR ){
           iErrorStop = 150;
           throw iRc;
         }
         ulTempBlockNo = npRightNode->ulBlockNo;
         npTag->npCurNode = npParent;
         npParent = npParent->npPrev;
       }

       // section D - if cur node is split root, create new root
       if( npTag->npCurNode->ulBlockNo == npTag->ulRootBlock ){
         // xbase->WriteLogMessage( "Section d" );
         if(( iRc = AddKeyNewRoot( npTag, npTag->npCurNode, npRightNode )) != XB_NO_ERROR ){
             iErrorStop = 160;
             throw iRc;
         }
         npRightNode = FreeNodeChain( npRightNode );

       } else {
         // else section E, put key in parent
         if(( iRc = InsertNodeI( vpTag, npParent, npParent->iCurKeyNo, npRightNode->ulBlockNo )) != XB_NO_ERROR ){
           iErrorStop = 170;
           throw iRc;
         }
         npRightNode = FreeNodeChain( npRightNode );
       }
     }

     // update the header
     if(( iRc = WriteHeadBlock( iHeadNodeUpdateOpt )) != XB_NO_ERROR ){
       iErrorStop = 180;
       throw iRc;
     }

     // ---- free whatever is left of the node chain here, this might not be right, might need to restore it to
     //         the point right after SetKeyPosAdd
     npTag->npNodeChain = FreeNodeChain( ndxTag->npNodeChain );
     npTag->npCurNode = NULL;

  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxNdx::AddKey() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}

/***********************************************************************/
//! @brief Add new root node.
/*!
  @param npTag Tag to update.
  @param npLeft Left node.
  @param npRight Right node.
  @returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbIxNdx::AddKeyNewRoot(  xbNdxTag *npTag, xbIxNode *npLeft, xbIxNode *npRight ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbString sMsg;
  char *pLastKey = NULL;

  try{
    xbIxNode *npRoot = AllocateIxNode( GetBlockSize() + (xbUInt32) npTag->iKeyItemLen, 1 );
    if( !npRoot ){
      iErrorStop = 100;
      iRc = XB_NO_MEMORY;
      throw iRc;
    }
    npTag->ulRootBlock = npRoot->ulBlockNo;
    pLastKey = (char *) malloc( (size_t) ndxTag->iKeyLen );
    if(( iRc = GetLastKeyForBlockNo( npTag, npLeft->ulBlockNo, pLastKey )) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }
    char * pTrg = npRoot->cpBlockData;

    // set no of keys to 1
    ePutUInt32( pTrg, 1 );

    // set the left node number
    pTrg += 4;
    ePutUInt32( pTrg, npLeft->ulBlockNo );

    // set the key
    pTrg+= 8;
    memcpy( pTrg, pLastKey, (size_t) npTag->iKeyLen );

    // set the right node number
    pTrg+= (npTag->iKeyItemLen - 8);
    ePutUInt32( pTrg, npRight->ulBlockNo );

     // write out the block
    if(( iRc = WriteBlock( npRoot->ulBlockNo, GetBlockSize(), npRoot->cpBlockData )) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw iRc;
    }
    if( pLastKey ) 
      free( pLastKey );
    NodeFree( npRoot );
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxNdx::AddKeyNewRoot() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
    if( pLastKey ) 
      free( pLastKey );
  }
  return iRc;
}
/***********************************************************************/
//! @brief Append node to node chain.
/*!
  Append a node to the current node chain for a given tag.

  @param vpTag Tag to update.
  @param npNode Node to add to node chain.
  @returns void
*/
void xbIxNdx::AppendNodeChain( void *vpTag, xbIxNode * npNode ){

    xbNdxTag * npTag;
    vpTag ? npTag = (xbNdxTag *) vpTag : npTag = ndxTag;

    if( npTag->npNodeChain == NULL ){
      npTag->npNodeChain = npNode;
      npTag->npCurNode = npNode;
    } else {
      npNode->npPrev = npTag->npCurNode;
      npTag->npCurNode->npNext = npNode;
      npTag->npCurNode = npNode;
    }
    // time stamp the node chain
    GetFileMtime( npTag->tNodeChainTs );
}

/***********************************************************************/
//! @brief Allocate a node.
/*!
  @param ulBufSize Buffer size.
  @param iOpt 0 - Don't update the node block number on the node.
              1 - Set node block number to the next available block number.
  @returns Pointer to new node.
*/

xbIxNode * xbIxNdx::AllocateIxNode( xbUInt32 ulBufSize, xbInt16 iOpt ){
  xbIxNode *n = xbIx::AllocateIxNode( ulBufSize );
  if( n && iOpt == 1 ) n->ulBlockNo = ndxTag->ulTotalBlocks++;
  return n;
}
/***********************************************************************/
//! @brief Check for duplicate key.
/*!
  @param vpTag Tag to check.
  @returns XB_KEY_NOT_UNIQUE<br>XB_NO_ERROR
*/
xbInt16 xbIxNdx::CheckForDupKey( void *vpTag )
{
  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbNdxTag *npTag = (xbNdxTag *) vpTag;
  npTag->bFoundSts = xbFalse;
  try{
    if( GetUnique()){
      if( npTag->iKeySts == XB_ADD_KEY || npTag->iKeySts == XB_UPD_KEY )
        if( KeyExists( vpTag )){
          //if( GetUniqueKeyOpt() == XB_EMULATE_DBASE ){
          if( npTag->iIxTagMode == XB_IX_DBASE_MODE ){
            npTag->bFoundSts = xbTrue;
            return 0;
          } else {
            return XB_KEY_NOT_UNIQUE;
          }
       }
     }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxNdx::CheckForDupKey() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}

/***********************************************************************/
//! @brief Check tag integrity.
/*!
  Check a tag for accuracy.

  @param vpTag Tag to create key for.
  @param iOpt Output message destination<br>
              0 = Syslog<br>
              1 = Stdout<br>
              2 = Both<br>
  @returns 1 - Unique Index - DBF records exist without corresponding index entries.<br>
           <a href="xbretcod_8h.html">Return Codes<br></a>
*/
xbInt16 xbIxNdx::CheckTagIntegrity( void *vpTag, xbInt16 iOpt ){

  xbInt16  iRc = XB_NO_ERROR;
  xbInt16  iRc2;
  xbInt16  iRc3;
  xbInt16  iErrorStop  = 0;
  xbInt16  iOutOfSync  = 0;
  xbUInt32 ulIxKeyCnt  = 0;
  xbUInt32 ulThisRecNo = 0;
  xbUInt32 ulPrevRecNo = 0;
  xbUInt32 j = 0;

  xbBool bDone = false;
  xbString sMsg;
  char cKeyType;
  char *pPrevKeyBuf = NULL;

  #ifdef XB_BLOCKREAD_SUPPORT
  xbBool bOriginalBlockReadSts = dbf->GetBlockReadStatus();
  #endif


  #ifdef XB_LOCKING_SUPPORT
  xbBool bLocked = xbFalse;
  #endif

  xbNdxTag * npTag;
  vpTag ? npTag = (xbNdxTag *) vpTag : npTag = ndxTag;

  try{

    cKeyType = GetKeyType( vpTag );

    sMsg = "*** Check NDX Tag Integrity ***";
    xbase->WriteLogMessage( sMsg, iOpt );
    sMsg.Sprintf( "TagName             = [%s]", GetTagName( vpTag ).Str());
    xbase->WriteLogMessage( sMsg, iOpt );
    sMsg.Sprintf( "KeyType             = [%c]", cKeyType );
    xbase->WriteLogMessage( sMsg, iOpt );

    if( GetUnique( vpTag ))
      sMsg = "Unique Index        = [ON]";
    else
      sMsg = "Unique Index        = [OFF]";
    xbase->WriteLogMessage( sMsg, iOpt );


    if( npTag->iIxTagMode == XB_IX_DBASE_MODE ){
      sMsg = "Tag Processing Mode = [XB_IX_DBASE_MODE]";
    } else if( npTag->iIxTagMode == XB_IX_XBASE_MODE ){
      sMsg = "Tag Processing Mode = [XB_IX_XBASE_MODE]";
    } else {
      sMsg = "Unknown tag processing option set";
    }
    xbase->WriteLogMessage( sMsg, iOpt );


    #ifdef XB_LOCKING_SUPPORT
    //if( dbf->GetAutoLock() && !dbf->GetTableLocked() ){
    if( dbf->GetMultiUser() && !dbf->GetTableLocked() ){
      if(( iRc = dbf->LockTable( XB_LOCK )) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      }
      bLocked = xbTrue;
    }
    #endif

    #ifdef XB_BLOCKREAD_SUPPORT
    if( !bOriginalBlockReadSts )
      dbf->EnableBlockReadProcessing();
    #endif

    memset( npTag->cpKeyBuf2, 0x00, (size_t) npTag->iKeyLen );
    pPrevKeyBuf = (char *) calloc( 1, (size_t) ndxTag->iKeyLen );

    // for each key in the index, make sure it is trending in the right direction
    iRc = GetFirstKey( vpTag, 0 );
    while( iRc == XB_NO_ERROR && !bDone ){

      ulIxKeyCnt++;
      iRc = GetNextKey( vpTag, 0 );
      if( iRc == XB_NO_ERROR ){
        // compare this key to prev key
        iRc2 = CompareKey( cKeyType, GetKeyData( npTag->npCurNode, npTag->npCurNode->iCurKeyNo, npTag->iKeyItemLen ), 
                          pPrevKeyBuf, (size_t) npTag->iKeyLen );

        if( iRc2 < 0 ){
          sMsg.Sprintf( "Key sequence error at key number [%ld].", ulIxKeyCnt );
          xbase->WriteLogMessage( sMsg, iOpt );
          iErrorStop = 110;
          iRc = XB_INVALID_INDEX;
          throw iRc;
        }

        if( iRc2 == 0 && GetUnique( vpTag )){
          sMsg.Sprintf( "Duplicate key at key number [%ld].", ulIxKeyCnt );
          xbase->WriteLogMessage( sMsg, iOpt );
          iErrorStop = 120;
          iRc = XB_INVALID_INDEX;
          throw iRc;
        }

        ulThisRecNo = 0;
        if(( iRc3 = GetDbfPtr( vpTag, npTag->npCurNode->iCurKeyNo, npTag->npCurNode, ulThisRecNo )) != XB_NO_ERROR ){
          iErrorStop = 130;
          throw iRc3;
        }

        if( iRc2 == 0 && (ulThisRecNo <= ulPrevRecNo )){
          sMsg.Sprintf( "Dbf record number sequence error at key number [%ld].", iOpt );
          xbase->WriteLogMessage( sMsg, iOpt );
          iErrorStop = 140;
          iRc = XB_INVALID_INDEX;
          throw iRc;
        }
        // save this key info to prev key
        memcpy( pPrevKeyBuf, GetKeyData( npTag->npCurNode, npTag->npCurNode->iCurKeyNo, npTag->iKeyItemLen ), (size_t) npTag->iKeyLen );
        ulPrevRecNo = ulThisRecNo;
      }
    }


    xbUInt32 ulDbfRecCnt = 0;
    if(( iRc = dbf->GetRecordCnt( ulDbfRecCnt )) != XB_NO_ERROR ){
      iErrorStop = 150;
      throw iRc;
    }


    // verify each record in the dbf file has a corresponding index entry
    xbUInt32 ulIndexedRecCnt = 0;
    xbUInt32 ulActiveRecCnt  = 0;


    while( j < ulDbfRecCnt ){

      if(( iRc = dbf->GetRecord( ++j )) != XB_NO_ERROR ){
        iErrorStop = 160;
        throw iRc;
      }

      iRc = FindKeyForCurRec( vpTag );
      if( iRc != XB_NO_ERROR && iRc != XB_NOT_FOUND ){
        iErrorStop = 170;
        throw iRc;
      }

      if( !dbf->RecordDeleted())
        ulActiveRecCnt++;

      if( npTag->iIxTagMode == XB_IX_XBASE_MODE ){
         if( dbf->RecordDeleted() && iRc == XB_NO_ERROR ){
           // should not be any index keys for deleted records in XB_IX_XBASE_MODE
           sMsg.Sprintf( "xbNdx::CheckTagIntegrity() Index entry found for deleted record for record number [%ld]", j );
           xbase->WriteLogMessage( sMsg, iOpt );
           ulThisRecNo = j;
           iErrorStop = 170;
           iRc = XB_INVALID_INDEX;
           throw iRc;
         } else if( !dbf->RecordDeleted() && iRc == XB_NOT_FOUND ){
           sMsg.Sprintf( "xbNdx::CheckTagIntegrity() Index entry not found for record number [%ld]", j );
           xbase->WriteLogMessage( sMsg, iOpt );
           ulThisRecNo = j;
           iErrorStop = 180;
           iRc = XB_INVALID_INDEX;
           throw iRc;
         }
         ulIndexedRecCnt++;
      } else {   // XB_IX_DBASE_MODE 
        if( iRc == XB_NOT_FOUND && GetUnique( vpTag ) ) {
          // UniqeKeyOpt is turned on, then missing index entries a possibility
          // if DBF table was updated by non Xbase64 application
          if(( iRc = KeyExists( vpTag )) != 1 ){
            ulThisRecNo = j;
            iErrorStop = 190;
            iRc = XB_INVALID_INDEX;
            throw iRc;
          }
        } else if( iRc == XB_NOT_FOUND ){
          ulThisRecNo = j;
          iErrorStop = 200;
          iRc = XB_INVALID_INDEX;
          throw iRc;
        } else {
          ulIndexedRecCnt++;
        }
      }
    }


    if( npTag->iIxTagMode == XB_IX_XBASE_MODE && ulActiveRecCnt != ulIxKeyCnt ){
      sMsg.Sprintf( "xbNdx::CheckTagIntegrity()  Active Dbf Rec Cnt = [%ld]  Index Entry Cnt = [%ld]", ulActiveRecCnt, ulIxKeyCnt );
      xbase->WriteLogMessage( sMsg, iOpt );
      ulThisRecNo = 0;
      iErrorStop = 210;
      iRc = XB_INVALID_INDEX;
      throw iRc;
    }

    if( npTag->iIxTagMode == XB_IX_DBASE_MODE ){
      if( ulIndexedRecCnt > ulIxKeyCnt && GetUnique() ){
        // DBase handles unique keys by only indicing the first record for a given index key
        // DBase  might have more than one record in a DBF file with the same key, only the first one is in a unique index
        sMsg.Sprintf( "CheckTagIntegrity()  Warning - Index entry count [%ld] does not match dbf record count [%ld]", ulIxKeyCnt, ulIndexedRecCnt );
        xbase->WriteLogMessage( sMsg, iOpt );
        sMsg.Sprintf( "Unique Index with multiple data records per key. Not an unexpected result in dBASE mode.");
        xbase->WriteLogMessage( sMsg, iOpt );
        iOutOfSync = 1;
      } else if( ulIndexedRecCnt != ulIxKeyCnt ){
        sMsg.Sprintf( "xbNdx::CheckTagIntegrity()  Dbf Rec Cnt = [%ld]  Index Entry Cnt = [%ld]", ulIndexedRecCnt, ulIxKeyCnt );
        xbase->WriteLogMessage( sMsg, iOpt );
        ulThisRecNo = 0;
        iErrorStop = 220;
        iRc = XB_INVALID_INDEX;
        throw iRc;
      }
    }
  }


  catch (xbInt16 iRc ){
    sMsg.Sprintf( "xbIxNdx::CheckTagIntegrity() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg, iOpt );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ), iOpt );
    dbf->DumpRecord( j, iOpt, 0 );
  }

  #ifdef XB_LOCKING_SUPPORT
  if( bLocked ){
    dbf->LockTable( XB_UNLOCK );
  }
  #endif

  #ifdef XB_BLOCKREAD_SUPPORT
  if( !bOriginalBlockReadSts )
    dbf->DisableBlockReadProcessing();
  #endif

  if( pPrevKeyBuf )
    free( pPrevKeyBuf );

  if( iRc == XB_NO_ERROR ){
    sMsg.Sprintf( "Tag [%s] OK.", GetTagName( vpTag ).Str());
    xbase->WriteLogMessage( sMsg, iOpt );
  }

  if( iRc )              // if hard error, return error code
    return iRc;
  else                   // return out of sync warning if it exists
    return iOutOfSync;

}
/***********************************************************************/
//! @brief Create key for tag.
/*!
  Append a node to the current node chain for a given tag.

  @param vpTag Tag to create key for.
  @param iOpt 0 = Build a key for FindKey usage, only rec buf 0.<br>
              1 = Append Mode, Create key for an append, only use rec buf 0, set updated switch.<br>
              2 = Update Mode, Create old version and new version keys, check if different, set update switch appropriately.
  @returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbIxNdx::CreateKey( void * vpTag, xbInt16 iOpt ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;

  try{
    xbNdxTag * npTag;
    vpTag ? npTag = (xbNdxTag *) vpTag : npTag = ndxTag;

    char cKeyType = npTag->exp->GetReturnType();

    if(( iRc = npTag->exp->ProcessExpression( 0 )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }

    if( cKeyType == XB_EXP_CHAR ){
      npTag->exp->GetStringResult( npTag->cpKeyBuf, (xbUInt32) npTag->iKeyLen );
    }
    else if( cKeyType == XB_EXP_NUMERIC || cKeyType == XB_EXP_DATE ){
      xbDouble d;
      npTag->exp->GetNumericResult( d );
      memcpy( npTag->cpKeyBuf, &d, 8 );
    }


    if( iOpt == 2 ){
      if(( iRc = npTag->exp->ProcessExpression( 1 )) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }
      if( cKeyType == XB_EXP_CHAR ){
        npTag->exp->GetStringResult( npTag->cpKeyBuf2, (xbUInt32) npTag->iKeyLen );

      } else if( cKeyType == XB_EXP_NUMERIC || cKeyType == XB_EXP_DATE  ){
        xbDouble d;
        npTag->exp->GetNumericResult( d );
        memcpy( npTag->cpKeyBuf2, &d, 8 );
      }
    }

    npTag->iKeySts = 0;

    if( iOpt == 1 ){                 // Append
      //if( GetDropDeletedKeyOpt() && dbf->RecordDeleted( 0 )){
      if( npTag->iIxTagMode == XB_IX_XBASE_MODE  && dbf->RecordDeleted( 0 )){
         // don't add deleted record to index
      } else {
        npTag->iKeySts = XB_ADD_KEY;
      }
    }

    else if( iOpt == 2 ){
      xbBool bOldKeyExists = xbTrue;
      xbBool bNewKeyExists = xbTrue;

      if( npTag->iIxTagMode == XB_IX_XBASE_MODE ){
        if( dbf->RecordDeleted( 1 ))
          bOldKeyExists = xbFalse;

        if( dbf->RecordDeleted( 0 ))
          bNewKeyExists = xbFalse;
      }

      if( bOldKeyExists && bNewKeyExists ){
        if( memcmp( npTag->cpKeyBuf, npTag->cpKeyBuf2, (size_t) npTag->iKeyLen )){
          npTag->iKeySts = XB_UPD_KEY;
        }

      } else if( bOldKeyExists && !bNewKeyExists ){
        npTag->iKeySts = XB_DEL_KEY;

      // if deleted tag key exists, adding instead of updating has potential to cause problems
      } else if( !bOldKeyExists && bNewKeyExists ){
        npTag->iKeySts = XB_ADD_KEY;
      }
    }

/*
    // previous to 4.1.8 below
    if( iOpt == 1 )
      npTag->iKeySts = XB_ADD_KEY;

    else if( iOpt == 2 ){
      if(( iRc = npTag->exp->ProcessExpression( 1 )) != XB_NO_ERROR ){
        iErrorStop = 110;
        throw iRc;
      }
      if( npTag->exp->GetReturnType() == XB_EXP_CHAR ){
        npTag->exp->GetStringResult( npTag->cpKeyBuf2, (xbUInt32) npTag->iKeyLen );
      } else if( npTag->exp->GetReturnType() == XB_EXP_NUMERIC || npTag->exp->GetReturnType() == XB_EXP_DATE  ){
        xbDouble d;
        npTag->exp->GetNumericResult( d );
        memcpy( npTag->cpKeyBuf2, &d, 8 );
      }
      if( memcmp( npTag->cpKeyBuf, npTag->cpKeyBuf2, (size_t) npTag->iKeyLen ))
        npTag->iKeySts = XB_UPD_KEY;
    }
*/

  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxNdx::CreateKey() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  
  return iRc;
}
/***********************************************************************/
//! @brief Create new tag.
/*!
  This routine creates a new tag. Since NDX files have only one tag,
  this creates a new NDX file.

  @param sName Tag Name, including .NDX suffix
  @param sKey Key Expression
  @param sFilter Filter expression.  Not supported by NDX indices.
  @param iDescending Not supported by NDX indices.
  @param iUnique xbtrue - Unique.<br>xbFalse - Not unique.
  @param iOverLay xbTrue - Overlay if file already exists.<br>xbFalse - Don't overlay.
  @param vpTag Output from method Pointer to vptag pointer.
  @returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16  xbIxNdx::CreateTag( const xbString &sName, const xbString &sKey,
                const xbString &, xbInt16, xbInt16 iUnique, xbInt16 iOverLay, void **vpTag ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbNdxTag *npTag = ndxTag;
  *vpTag = ndxTag;

  try{
    //xbString sMsg;
    SetFileName( sName );

    if( FileExists() && !iOverLay )
      return XB_FILE_EXISTS;

    if( FileIsOpen()){
      if(( iRc = xbTruncate(0)) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      }
      if(( iRc = xbFclose()) != XB_NO_ERROR ){
        iErrorStop = 110;
        throw iRc;
      }
      npTag->npNodeChain = FreeNodeChain( npTag->npNodeChain );
      npTag->npCurNode = NULL;
      npTag->sKeyExpression.Set( "" );

      if( npTag->cpKeyBuf ){
        free( npTag->cpKeyBuf );
        npTag->cpKeyBuf = NULL;
      }
      if( npTag->cpKeyBuf2 ){
        free( npTag->cpKeyBuf2 );
        npTag->cpKeyBuf2 = NULL;
      }
    }
    if(( iRc = xbFopen( "w+b", dbf->GetShareMode())) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw iRc;
    }

    //set up the key expression
    npTag->exp = new xbExp( dbf->GetXbasePtr());
    if(( iRc = npTag->exp->ParseExpression( dbf, sKey )) != XB_NO_ERROR ){
      iErrorStop = 130;
      throw iRc;
    }

    switch( npTag->exp->GetReturnType()){
      case XB_EXP_CHAR:
        npTag->cKeyType = 'C';
        npTag->iKeyType = 0;
        npTag->iKeyLen  = npTag->exp->GetResultLen();
        break;

      case XB_EXP_NUMERIC:
        npTag->cKeyType = 'F';
        npTag->iKeyType = 1;
        npTag->iKeyLen  = 8;
        break;

      case XB_EXP_DATE:
        npTag->cKeyType = 'D';
        npTag->iKeyType = 1;
        npTag->iKeyLen  = 8;
        break;

      default:
        iErrorStop = 140;
        iRc = XB_INVALID_INDEX;
        throw iRc;
    }

    npTag->iUnique        = iUnique;
    npTag->ulRootBlock    = 1L;
    //npTag->ulTotalBlocks  = 2l;
    npTag->ulTotalBlocks  = 2L;
    npTag->sKeyExpression = sKey;

    npTag->iIxTagMode     = xbase->GetDefaultIxTagMode();

    GetFileNamePart( npTag->sTagName );

    if( npTag->iKeyLen > 100 ){
      iErrorStop = 150;
      throw iRc;
    }

    npTag->iKeyItemLen    = npTag->iKeyLen + 8;
    while(( npTag->iKeyItemLen % 4 )!= 0 ) npTag->iKeyItemLen++;

    npTag->iKeysPerBlock = (xbInt16) (GetBlockSize() - 8 ) / npTag->iKeyItemLen; 
    ndxTag->cpKeyBuf  = (char *) malloc( (size_t) ndxTag->iKeyLen );
    ndxTag->cpKeyBuf2 = (char *) malloc( (size_t) ndxTag->iKeyLen );

    if(( iRc = WriteHeadBlock(0)) != XB_NO_ERROR ){
      iErrorStop = 160;
      throw iRc;
    }

    //write out block binary zeroes
    char buf[512];
    memset( buf, 0x00, 512 );
    if(( iRc = xbFwrite( buf, 1, 512 )) != XB_NO_ERROR ){
      iErrorStop = 170;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxNdx::CreateTag() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}

/***********************************************************************/
//! @brief Delete a key.
/*!
  This routine deletes a key from a supplied node.
  @param vpTag Tag to delete key on.
  @param npNode Node to delete key on.
  @param iSlotNo Slot number of key to delete.
  @returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbIxNdx::DeleteFromNode( void *vpTag, xbIxNode * npNode, xbInt16 iSlotNo )
{
  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbNdxTag * npTag;
  vpTag ? npTag = (xbNdxTag *) vpTag : npTag = ndxTag;

  try{

    xbInt32 lKeyCnt = GetKeyCount( npNode );
    xbInt16 iLen = (lKeyCnt - iSlotNo - 1) * npTag->iKeyItemLen;
    if( !IsLeaf( vpTag, npNode ))
        iLen += 4;

    if( iLen > 0 ){
      char *pTrg = npNode->cpBlockData;
      pTrg += (4 + (npTag->iKeyItemLen * (iSlotNo)) );  //lTrgPos;
      char *pSrc = pTrg;
      pSrc += npTag->iKeyItemLen;
      memmove( pTrg, pSrc, (size_t) iLen );
    }

    // set the new number of keys 
    ePutUInt32( npNode->cpBlockData, (xbUInt32) lKeyCnt - 1 );

     // write out the block
    if(( iRc = WriteBlock( npNode->ulBlockNo, GetBlockSize(), npNode->cpBlockData )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxNdx::DeleteFromNode() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return XB_NO_ERROR;
}

/***********************************************************************/
//! @brief Delete a key.
/*!
  This routine deletes a key. It assumes the key to delete
  is the current key in the node chain.

  @param vpTag Tag to delete key on.

  @returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbIxNdx::DeleteKey( void *vpTag ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbNdxTag * npTag;
  vpTag ? npTag = (xbNdxTag *) vpTag : npTag = ndxTag;

  // save copy of node chain to reset to after delete completed
  xbIxNode *npSaveNodeChain = npTag->npNodeChain;
  npTag->npNodeChain = NULL;
  xbIxNode * npSaveCurNode = npTag->npCurNode;

  try{

    xbString sMsg;

     if(( iRc = xbIxNdx::KeySetPosDel( npTag )) != XB_NO_ERROR ){
       iErrorStop = 100;
       throw iRc;
     }
     // Delete key needs to handle two scenarios
     // 1 - if the delete is on the only key of a leaf node, then traverse up the tree, trimming as needed
     // 2 - if the last key on a node is deleted, and the key value is not the same as the prev key value
     //     go up the tree looking for an interior node needing updated key value

     xbInt32 lOrigKeyCnt = GetKeyCount( npTag->npCurNode );

     if(( iRc = DeleteFromNode( npTag, npTag->npCurNode, npTag->npCurNode->iCurKeyNo )) != XB_NO_ERROR ){
       iErrorStop = 110;
       throw iRc;
     }


     if( lOrigKeyCnt == 1 ){
       // scenario 1 


       xbBool bDone = xbFalse;
       xbBool bIsLeaf = xbFalse;
       xbInt32 lKeyCnt;
       npTag->npCurNode = npTag->npCurNode->npPrev;



       while( npTag->npCurNode && !bDone ){
         lKeyCnt = GetKeyCount( npTag->npCurNode );
         bIsLeaf = IsLeaf( npTag, npTag->npCurNode );
         if( lKeyCnt > 0 ){
           if(( iRc = DeleteFromNode( npTag, npTag->npCurNode, npTag->npCurNode->iCurKeyNo )) != XB_NO_ERROR ){
             iErrorStop = 120;
             throw iRc;
           }
         }
         if( (bIsLeaf && lKeyCnt > 1) || (!bIsLeaf && lKeyCnt > 0) )
           bDone = xbTrue;
         else
           npTag->npCurNode = npTag->npCurNode->npPrev; 
      }
    } else if( npTag->npCurNode->iCurKeyNo == (xbUInt32) lOrigKeyCnt - 1 ){


      // scenario 2
      // if last two keys identical, then nothing to do, else go up looking for a key to change
      if( memcmp( GetKeyData( npTag->npCurNode, npTag->npCurNode->iCurKeyNo, npTag->iKeyItemLen ),
                  GetKeyData( npTag->npCurNode, npTag->npCurNode->iCurKeyNo-1, npTag->iKeyItemLen ),
                  (size_t) npTag->iKeyLen )){


        xbIxNode *pNode = npTag->npCurNode->npPrev;
        char *pSrc = GetKeyData( npTag->npCurNode, npTag->npCurNode->iCurKeyNo-1, npTag->iKeyItemLen );

        while( pNode && pNode->ulBlockNo != npTag->ulRootBlock && pNode->iCurKeyNo == (xbUInt32) GetKeyCount( pNode ) )
            pNode = pNode->npPrev;
        if( pNode ){


          if( pNode->iCurKeyNo < (xbUInt32) GetKeyCount( pNode )){

            char *pTrg  = pNode->cpBlockData;
            pTrg += 12 + (pNode->iCurKeyNo * (xbUInt32) npTag->iKeyItemLen);
            for( xbInt16 i = 0; i < npTag->iKeyLen; i++ )
              *pTrg++ = *pSrc++;

            // write out the block
            if(( iRc = WriteBlock( pNode->ulBlockNo, GetBlockSize(), pNode->cpBlockData )) != XB_NO_ERROR ){
              iErrorStop = 130;
              throw iRc;
            }
          }
        }
      }
    }

    // restore node chain to pre delete status (which should be post add status)
    npTag->npNodeChain = FreeNodeChain( npTag->npNodeChain );
    npTag->npNodeChain = npSaveNodeChain;
    npTag->npCurNode = npSaveCurNode;

  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxNdx::DeleteKey() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
    if( npSaveNodeChain ){
      npTag->npNodeChain = npSaveNodeChain;
      npSaveNodeChain = FreeNodeChain( npSaveNodeChain );
      npTag->npCurNode = npSaveCurNode;
    }
  }
  return iRc;
}


/***********************************************************************/
//! @brief Delete tag.
/*!
  In the case of an ndx tag, it deletes the ndx file as it contains
  only one tag.
  @returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbIxNdx::DeleteTag( void * ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  //xbNdxTag * npTag;
  //vpTag ? npTag = (xbNdxTag *) vpTag : npTag = ndxTag;

  try{

    // if open, close it
    if( FileIsOpen()){
      if(( iRc = Close()) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      }
    }
    // delete file
    if(( iRc = xbRemove()) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }

  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxNdx::DeleteTag() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
/***********************************************************************/
#ifdef XB_DEBUG_SUPPORT

//! @brief Dump a block for a given tag.
/*!
  Dump blocks for given tag for debugging purposes.
  @param iOpt Output message destination<br>
              0 = stdout<br>
              1 = Syslog<br>
              2 = Both<br>
  @param vpTag - Not required for single tag NDX files.
  @returns void
*/

xbInt16 xbIxNdx::DumpTagBlocks( xbInt16 iOpt, void * ){

  xbInt16  iRc = 0;
  xbInt16  iErrorStop = 0;
  xbUInt32 lNoOfKeys;
  char     *p;
  xbString s;
  xbBool   bIsLeaf = false;

  try{
    if( !FileIsOpen()){
      iRc = XB_NOT_OPEN;
      iErrorStop = 100;
      throw iRc;
    }

    xbUInt32 ulStartBlock;
    xbUInt32 ulEndBlock;
    ulStartBlock = 1;
    ulEndBlock = ndxTag->ulTotalBlocks;

    for( xbUInt32 lBlk = ulStartBlock; lBlk < ulEndBlock; lBlk++ ){

      memset( cNodeBuf, 0x00, XB_NDX_BLOCK_SIZE );
      if(( iRc = ReadBlock( lBlk, XB_NDX_BLOCK_SIZE, cNodeBuf )) != XB_NO_ERROR ){
        iErrorStop = 110;
        throw iRc;
      }
      p = cNodeBuf;
      lNoOfKeys = eGetUInt32( p );

      if( eGetUInt32( p + 4 ) > 0 ){
        bIsLeaf = false;
        s.Sprintf( "Node # %ld - Interior Node - Key Type [%c] Key Count [%ld]", lBlk, ndxTag->cKeyType, lNoOfKeys );
      } else {
        bIsLeaf = true;
        s.Sprintf( "Node # %ld - Leaf Node - Key Type [%c] Key count [%ld]", lBlk, ndxTag->cKeyType, lNoOfKeys );
      }
      xbase->WriteLogMessage( s, iOpt );
      xbase->WriteLogMessage( "Key      Child    Dbf Rec  Key", iOpt );
      p += 4;
      xbUInt32 ulLeftBranch;
      xbUInt32 ulRecNo;
      xbString sKey;
      xbDouble d;

      xbUInt32 l;
      for( l = 0; l < lNoOfKeys; l++ ){
        ulLeftBranch = eGetUInt32( p );
        p+= 4;
        ulRecNo = eGetUInt32( p );
        p+= 4;
        if( ndxTag->cKeyType == 'C' ){
          sKey.Assign( p, 1, (xbUInt32) ndxTag->iKeyLen );
        } else if( ndxTag->cKeyType == 'D' ){
          xbInt32 lDate = (xbInt32) eGetDouble( p );
          xbDate dt( lDate );
          //xbString s2;
          //dt.JulToDate8( lDate, s2 );
          sKey.Sprintf( "%ld - %s", lDate, dt.Str());
        } else {
          d = eGetDouble( p );
          sKey.Sprintf( "%f", d );
        }
        p+= (ndxTag->iKeyItemLen-8);

        s.Sprintf( "%3d  %9d  %9d  %s", l+1, ulLeftBranch, ulRecNo, sKey.Str() );
        xbase->WriteLogMessage( s, iOpt );
      }
      if( !bIsLeaf ){
        ulLeftBranch = eGetUInt32( p );
        s.Sprintf( "%3d  %9d", l+1, ulLeftBranch );
        xbase->WriteLogMessage( s, iOpt );
      }
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxNdx::DumpTagBlocks() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
/***********************************************************************/
//! @brief Dump index file header.
/*!
  Dump a index file header for debugging purposes.
  @param iOpt Output message destination<br>
              0 = stdout<br>
              1 = Syslog<br>
              2 = Both<br>
  @returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbIxNdx::DumpHeader( xbInt16 iOpt, xbInt16 ){
  xbString s;
  xbInt16 iRc;

  if(( iRc = ReadHeadBlock( 1 )) != XB_NO_ERROR )
    return iRc;

  s.Sprintf( "Index Header Node for %s", GetFileName().Str());
  xbase->WriteLogMessage( s, iOpt );
  s.Sprintf( "--------------------------------" );
  xbase->WriteLogMessage( s, iOpt );
  s.Sprintf( "Root block      = %ld",   ndxTag->ulRootBlock );
  xbase->WriteLogMessage( s, iOpt );
  s.Sprintf( "Total blocks    = %ld",   ndxTag->ulTotalBlocks );
  xbase->WriteLogMessage( s, iOpt );
  s.Sprintf( "Key types       = %c,%d", ndxTag->cKeyType, ndxTag->iKeyType );
  xbase->WriteLogMessage( s, iOpt );
  s.Sprintf( "Key Length      = %d",    ndxTag->iKeyLen );
  xbase->WriteLogMessage( s, iOpt );
  s.Sprintf( "Keys Per Block  = %d",    ndxTag->iKeysPerBlock );
  xbase->WriteLogMessage( s, iOpt );
  s.Sprintf( "Key Item Len    = %ld",   ndxTag->iKeyItemLen );
  xbase->WriteLogMessage( s, iOpt);
  s.Sprintf( "Serial No       = %d",    ndxTag->cSerNo );
  xbase->WriteLogMessage( s, iOpt);
  s.Sprintf( "Unique          = %d",    ndxTag->iUnique );
  xbase->WriteLogMessage( s, iOpt );
  s.Sprintf( "KeyExpression   = %s",    ndxTag->sKeyExpression.Str() );
  xbase->WriteLogMessage( s, iOpt );
  return XB_NO_ERROR;
}
/***********************************************************************/
//! @brief Dump the index for a tag.
/*!
  Stub.
  @returns XB_NO_ERROR
*/
xbInt16 xbIxNdx::DumpIxForTag( void *, xbInt16 )
{
  return XB_NO_ERROR;
}
/***********************************************************************/
//! @brief Dump the index node chain.
/*!
  Dump the index node chain for debugging purposes.
  @param vpTag Tag of node chain to dump.
  @param iOpt Output message destination<br>
              0 = stdout<br>
              1 = Syslog<br>
              2 = Both<br>
  @returns void
*/
void xbIxNdx::DumpIxNodeChain( void *vpTag, xbInt16 iOpt ) const
{

  xbNdxTag * npTag;
  vpTag ? npTag = (xbNdxTag *) vpTag : npTag = ndxTag;

  xbString s( "Dump Node Chain" );
  xbase->WriteLogMessage( s, iOpt );

  if( npTag->npNodeChain ){
    xbIxNode *n = npTag->npNodeChain;
    xbInt16 iCtr = 0;
    char cLeaf;
    s.Sprintf( "Cnt\tThis     Prev     Next     CurKeyNo  BlockNo  NoOfKeys  Type" );
    xbase->WriteLogMessage( s, iOpt );
    while( n ){
      IsLeaf( vpTag, n ) ? cLeaf = 'L' : cLeaf = 'I';
      s.Sprintf( "%d\t%08x %08x %08x %08d  %08d %08d  %c", 
                 iCtr++, n, n->npPrev, n->npNext, n->iCurKeyNo, 
                 n->ulBlockNo, eGetUInt32( n->cpBlockData ), cLeaf );
      xbase->WriteLogMessage( s, iOpt );
      n = n->npNext;
    }
  } else {
    s = "Empty Node Chain";
    xbase->WriteLogMessage( s, iOpt );
  }
}
/***********************************************************************/
//! @brief Dump node.
/*!
  Dump a node for debugging purposes.
  @param vpTag Tag of node chain to dump.
  @param pNode Node to dump.
  @param iOpt Output message destination<br>
              0 = stdout<br>
              1 = Syslog<br>
              2 = Both<br>
  @returns XB_INVALID_OBJECT<br>XB_NO_ERROR
*/

xbInt16 xbIxNdx::DumpNode( void *vpTag, xbIxNode *pNode, xbInt16 iOpt ) const
{
  xbString s;
  xbString sKey;
  xbUInt32 lLeftBranch;
  xbUInt32 lRecNo;
  xbDouble d;

  if( !pNode )
    return XB_INVALID_OBJECT;

  xbIx::DumpNode( vpTag, pNode, iOpt );

  xbNdxTag * npTag;
  vpTag ? npTag = (xbNdxTag *) vpTag : npTag = ndxTag;

  xbUInt32 lNoOfKeys = eGetUInt32( pNode->cpBlockData );
  xbBool bIsLeaf = IsLeaf( vpTag, pNode );

  if( bIsLeaf )
    xbase->WriteLogMessage( "Leaf node", iOpt );
  else
    xbase->WriteLogMessage( "Interior node", iOpt );

  s.Sprintf( "Key type = [%c] No Of Keys =[%d] Prev =[%x] Next =[%x]", npTag->cKeyType, lNoOfKeys, pNode->npPrev, pNode->npNext );
  xbase->WriteLogMessage( s, iOpt );

  char *p = pNode->cpBlockData;
  p += 4;

  xbUInt32 l;
  for( l = 0; l < lNoOfKeys; l++ ){

    lLeftBranch = eGetUInt32( p );
    p+= 4;
    lRecNo = eGetUInt32( p );
    p+= 4;

    if( npTag->cKeyType == 'C' ){
      sKey.Assign( p, 1, (xbUInt32) npTag->iKeyLen );
    } else if( npTag->cKeyType == 'D' ){
      xbInt32 lDate = (xbInt32) eGetDouble( p );
      xbDate dt( lDate );
      sKey.Sprintf( "%ld - %s", lDate, dt.Str());
    } else {
      d = eGetDouble( p );
      sKey.Sprintf( "%f", d );
    }
    p+= (npTag->iKeyItemLen-8);
    s.Sprintf( "%3d  %9d  %9d  %s", l+1, lLeftBranch, lRecNo, sKey.Str() );
    xbase->WriteLogMessage( s, iOpt );
  }
  if( !bIsLeaf ){
    lLeftBranch = eGetUInt32( p );
    s.Sprintf( "%3d  %9d", l+1,  lLeftBranch );
    xbase->WriteLogMessage( s.Str(), iOpt );
  }
  return XB_NO_ERROR;
}
#endif
/***********************************************************************/
//! @brief Find key
/*!
  @param vpTag  Pointer to tag to search.
  @param vpKey Void pointer to key data to search on.
  @param lSearchKeyLen Length of key to search for.
  @param iRetrieveSw xbTrue - Retrieve the record if key found.<br>
                     xbFalse - Don't retrieve record, check for key existence only.
  @returns XB_NO_ERROR - Key found.<br>
             XB_NOT_FOUND - Key not found.<br>
             <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbIxNdx::FindKey( void *vpTag, const void *vpKey, xbInt32 lSearchKeyLen,
                          xbInt16 iRetrieveSw ){

  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;
  xbString sMsg;
  // xbInt16 iFindSts;
  try{
    // clean  up any previous table updates before moving on
    if( iRetrieveSw ){
      if( dbf->GetDbfStatus() == XB_UPDATED ){
        if(  dbf->GetAutoCommit() == 1 ){
          if(( iRc = dbf->Commit()) != XB_NO_ERROR ){
            iErrorStop = 100;
            throw iRc;
          }
        } else {
          if(( iRc = dbf->Abort()) != XB_NO_ERROR ){
            iErrorStop = 110;
            throw iRc;
          }
        }
      }
    }

    xbUInt32 ulNoOfKeys;
    xbNdxTag * npTag;
    vpTag ? npTag = (xbNdxTag *) vpTag : npTag = ndxTag;
    char cKeyType = npTag->cKeyType;

    if( npTag->npNodeChain ){

      // determine if the index has been updated since the last time it was used
      time_t tFileTs;
      if(( iRc = GetFileMtime( tFileTs )) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }
      if( npTag->tNodeChainTs < tFileTs ){
        npTag->npNodeChain = FreeNodeChain( npTag->npNodeChain );
        npTag->npCurNode = NULL;
        if(( iRc = ReadHeadBlock( 1 )) != XB_NO_ERROR ){
          iErrorStop = 130;
          throw iRc;
        }
        if(( iRc = GetBlock( npTag, (npTag->ulRootBlock ), 1, (xbUInt32) npTag->iKeyItemLen )) != XB_NO_ERROR ){
          iErrorStop = 140;
          throw iRc;
        }
      } else {
        //  pop up the chain looking for appropriate starting point
        xbBool  bDone = false;
        xbIxNode * TempIxNode;
        while( npTag->npCurNode && !bDone && npTag->npCurNode->ulBlockNo != npTag->ulRootBlock ){ // not root node
            iRc = CompareKey( cKeyType, vpKey, GetKeyData( npTag->npCurNode, 0, npTag->iKeyItemLen ), (size_t) lSearchKeyLen );
            if( iRc <= 0 ){
              TempIxNode = npTag->npCurNode;
              npTag->npCurNode = npTag->npCurNode->npPrev;
              TempIxNode = FreeNodeChain( TempIxNode );
            } else {
              // get the number of keys on the block and compare the key to the rightmost key
              xbUInt32 ulKeyCtr = eGetUInt32( npTag->npCurNode->cpBlockData ) - 1;
              iRc = CompareKey( cKeyType, vpKey, GetKeyData( npTag->npCurNode, ulKeyCtr, npTag->iKeyItemLen), (size_t) lSearchKeyLen );

              if( iRc > 0 ){
                TempIxNode = npTag->npCurNode;
                npTag->npCurNode = npTag->npCurNode->npPrev;
                TempIxNode = FreeNodeChain( TempIxNode );
              } else {
                bDone = true;
              }
            }
          }
       }
    } else {
      if(( iRc = GetBlock( npTag, (npTag->ulRootBlock ), 1, (xbUInt32) npTag->iKeyItemLen )) != XB_NO_ERROR ){
        iErrorStop = 150;
        throw iRc;
      }
    }


    // if cur node is the base node and no keys on this node, then the index is empty
    if( npTag->ulRootBlock == npTag->npCurNode->ulBlockNo ){
      ulNoOfKeys = eGetUInt32( npTag->npCurNode->cpBlockData );
      if( ulNoOfKeys == 0 && IsLeaf( npTag, npTag->npCurNode )){
        // iRc = XB_EMPTY;

        iRc = XB_NOT_FOUND;
        return iRc;
      }
    }

    // should be in the appropriate position in the node chain to continue the search from here
    // run down through the interior nodes
    xbInt16 iSearchRc = 0;
    xbUInt32 ulKeyPtr = 0;

    while( !IsLeaf( npTag, npTag->npCurNode ) ){

      // get the number of keys on the block and compare the key to the rightmost key
      ulNoOfKeys = eGetUInt32( npTag->npCurNode->cpBlockData );
      if( ulNoOfKeys == 0 )   // interior nodes can have zero keys, just a link to the next lower node
        npTag->npCurNode->iCurKeyNo = 0;
      else
      {
        iRc = CompareKey( cKeyType, vpKey, GetKeyData( npTag->npCurNode, ulNoOfKeys - 1, npTag->iKeyItemLen), (size_t) lSearchKeyLen );
        if( iRc > 0 ){
          npTag->npCurNode->iCurKeyNo = ulNoOfKeys;
        } else {
          npTag->npCurNode->iCurKeyNo = (xbUInt32) BSearchBlock( cKeyType, npTag->npCurNode, 
             (xbInt32) npTag->iKeyItemLen, vpKey, (xbInt32) lSearchKeyLen, iSearchRc );
        }
      }

      if(( iRc = GetKeyPtr( npTag, npTag->npCurNode->iCurKeyNo, npTag->npCurNode, ulKeyPtr )) != XB_NO_ERROR ){
        iErrorStop = 160;
        throw iRc;
      }

      if(( iRc = GetBlock( npTag, ulKeyPtr, 1, (xbUInt32) npTag->iKeyItemLen )) != XB_NO_ERROR ){
        iErrorStop = 170;
        throw iRc;
      }
    }

    // should be on a the correct leaf node, it may or may not contain the actual key
    ulNoOfKeys = eGetUInt32( npTag->npCurNode->cpBlockData );
    xbInt16 iCompRc = 0;

    if( ulNoOfKeys == 0 ){
      iRc = XB_NOT_FOUND;
      return iRc;
    } else {

      iRc = BSearchBlock( cKeyType, npTag->npCurNode, npTag->iKeyItemLen, vpKey, lSearchKeyLen, iCompRc );

      // iCompRc 
      //    0 found
      //  < 0 eof encountered, search key > last key in file
      //  > 0 not found, positioned to next key


      // std::cout << "xbIxNdx::FindKey -Rc = " << iRc << " CompRc = " << iCompRc << " NoOfKeys = " << ulNoOfKeys << " blk no = " << npTag->npCurNode->ulBlockNo << "\n";

      if( iCompRc >= 0 ){
        npTag->npCurNode->iCurKeyNo = (xbUInt32) iRc;
        if( iRetrieveSw ){
          xbUInt32 ulKey = npTag->npCurNode->iCurKeyNo;
          if( ulKey >= ulNoOfKeys )   // if position past end of keys, need to go back one and position to last key
            ulKey--;

          if(( iRc = GetDbfPtr( vpTag, ulKey, npTag->npCurNode, ulKeyPtr )) != XB_NO_ERROR ){
            iErrorStop = 180;
            throw iRc;
          }
          if(( iRc = dbf->GetRecord( ulKeyPtr )) != XB_NO_ERROR ){
            iErrorStop = 190;
            throw iRc;
          }
        }
      }
    }

    if( iCompRc == 0 )
      return XB_NO_ERROR;
    else if( iCompRc > 0 )
      return XB_NOT_FOUND;
    else
      return XB_EOF;
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxNdx::FindKey() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
/***********************************************************************/
//! @brief Find key for current record
/*!
  This routine is called when updating a key.

  @param vpTag  Pointer to tag to search.
             XB_NOT_FOUND Key not found.<br>
             <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbIxNdx::FindKeyForCurRec( void * vpTag, xbInt16 iRetrieveSw )
{

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbNdxTag * npTag;
  vpTag ? npTag = (xbNdxTag *) vpTag : npTag = ndxTag;

  try{
    if(( iRc = CreateKey( vpTag, 0 )) < XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }

    // find key
    iRc = FindKey( vpTag, npTag->cpKeyBuf, npTag->iKeyLen, iRetrieveSw );
    if( iRc == XB_NOT_FOUND || iRc == XB_EMPTY || iRc == XB_EOF )
       return iRc;

    if( iRc != XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }

    // if keys are unique, and the recrd number matches, then we are good
    if( GetUnique() )
      return XB_NO_ERROR;

    // get here if key found and not unique, need to move forward looking for correct rec no
    xbUInt32 ulDbfRecNo = dbf->GetCurRecNo();
    xbBool bKeysMatch  = true;     // keys match?
    xbBool bCurRecsMatch = false;  // cur recod number matches?
    xbUInt32 ulIxRecNo = 0;
    char cKeyType = GetKeyType( vpTag );

    if(( iRc = GetDbfPtr( vpTag, npTag->npCurNode->iCurKeyNo, npTag->npCurNode, ulIxRecNo )) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw iRc;
    }
    if( ulIxRecNo == ulDbfRecNo )
      bCurRecsMatch = true;

    xbInt16 iCompRc;
    while( !bCurRecsMatch && bKeysMatch ){

      if(( iRc = GetNextKey( vpTag, 0 )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }
      // do compare key here
      iCompRc = CompareKey( cKeyType, npTag->cpKeyBuf, GetKeyData( npTag->npCurNode, npTag->npCurNode->iCurKeyNo, npTag->iKeyItemLen ), (size_t) npTag->iKeyLen );
      if( iCompRc != 0 )
        bKeysMatch = false;
      else{
        if(( iRc = GetDbfPtr( vpTag, npTag->npCurNode->iCurKeyNo, npTag->npCurNode, ulIxRecNo )) != XB_NO_ERROR ){
          iErrorStop = 140;
          throw iRc;
        }
        if( ulIxRecNo == ulDbfRecNo )
          bCurRecsMatch = true;
      }
    }
    if( ulIxRecNo != ulDbfRecNo ){
      std::cout << "ulDbfRecNo = " << ulDbfRecNo << "\n";
      dbf->DumpRecord( ulDbfRecNo, 1, 0 );
      return XB_NOT_FOUND;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxNdx::FindKeyForCurRec() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return XB_NO_ERROR;
}


/***********************************************************************/
xbInt16 xbIxNdx::GetCurKeyVal( void *vpTag, xbString &s ){

  if( !vpTag )
    return XB_INVALID_TAG;
  xbNdxTag * npTag = (xbNdxTag *) vpTag;
  s.Set( GetKeyData( npTag->npCurNode, npTag->npCurNode->iCurKeyNo, npTag->iKeyItemLen ), (xbUInt16) npTag->iKeyLen );
  return XB_NO_ERROR;
}

/***********************************************************************/
xbInt16 xbIxNdx::GetCurKeyVal( void *vpTag, xbDouble &d ){

  if( !vpTag )
    return XB_INVALID_TAG;
  xbNdxTag * npTag = (xbNdxTag *) vpTag;
  d = eGetDouble( GetKeyData( npTag->npCurNode, npTag->npCurNode->iCurKeyNo, npTag->iKeyItemLen ));
  return XB_NO_ERROR;
}

/***********************************************************************/
xbInt16 xbIxNdx::GetCurKeyVal( void *vpTag, xbDate &dt ){

  if( !vpTag )
    return XB_INVALID_TAG;
  xbNdxTag * npTag = (xbNdxTag *) vpTag;
  xbDouble d = eGetDouble( GetKeyData( npTag->npCurNode, npTag->npCurNode->iCurKeyNo, npTag->iKeyItemLen ));
  dt.JulToDate8( (xbInt32) d );
  return XB_NO_ERROR;
}

/***********************************************************************/
//! @brief Get dbf record number for given key number.
/*!
  @param vpTag Tag to retrieve dbf rec number on.
  @param iKeyNo Key number for retrieval
  @param np Pointer to node
  @param ulDbfPtr- Output dbf record number
  @returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbIxNdx::GetDbfPtr( void *vpTag, xbInt16 iKeyNo, xbIxNode *np, xbUInt32 &ulDbfPtr ) const {

  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;

  try{
    #ifdef XB_DEBUG_SUPPORT
    // turn this off in production mode for better performance
    xbUInt32 ulNoOfKeys = eGetUInt32 ( np->cpBlockData );
    if( iKeyNo < 0 || iKeyNo > (xbInt16) --ulNoOfKeys ){
      iErrorStop = 100;
      throw XB_INVALID_KEYNO;
    }
    #endif

    xbNdxTag * npTag;
    vpTag ? npTag = (xbNdxTag *) vpTag : npTag = ndxTag;
    char *p = ( np->cpBlockData);
    p += (8 + (iKeyNo * npTag->iKeyItemLen));
    ulDbfPtr = eGetUInt32 ( p );
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxNdx::GetDbfPtr() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
/***********************************************************************/
//! @brief Get the first key for the given tag.
/*!
  @param vpTag Tag to retrieve first key on.
  @param iRetrieveSw xbTrue - Retrieve the record on success.<br>
                     xbFalse - Don't retrieve record.
  @returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbIxNdx::GetFirstKey( void *vpTag, xbInt16 iRetrieveSw ){
  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbNdxTag * npTag;
  vpTag ? npTag = (xbNdxTag *) vpTag : npTag = ndxTag;

  try{
    // clear out any history
    if( npTag->npNodeChain ){
      npTag->npNodeChain = FreeNodeChain( npTag->npNodeChain );
      npTag->npCurNode = NULL;
    }
    if(( iRc = ReadHeadBlock( 1 )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }
    // lRootBlock is now available
    if(( iRc = GetBlock( npTag, npTag->ulRootBlock, 1, (xbUInt32) npTag->iKeyItemLen )) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }
    // if no keys on this node, and it's a leaf node then the index is empty
    xbUInt32 ulKeyPtr = eGetUInt32( npTag->npCurNode->cpBlockData );
    if( ulKeyPtr == 0 && IsLeaf( npTag, npTag->npCurNode )){
      iRc = XB_EMPTY;
      return iRc;
    }
    while( !IsLeaf( npTag, npTag->npCurNode ))   // go down the chain looking for a leaf node
    {
      if(( iRc = GetKeyPtr( npTag, 0, npTag->npCurNode, ulKeyPtr )) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }
      if(( iRc = GetBlock( npTag, ulKeyPtr, 1, (xbUInt32) npTag->iKeyItemLen )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }
    }
    if( iRetrieveSw ){
      if(( iRc = GetDbfPtr( npTag, 0, npTag->npCurNode, ulKeyPtr )) != XB_NO_ERROR ){
        iErrorStop = 140;
        throw iRc;
      }
      if(( iRc = dbf->GetRecord( ulKeyPtr )) != XB_NO_ERROR ){
        iErrorStop = 150;
        throw iRc;
      }
    }
  }
  catch( xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxNdx::GetFirstKey() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}

/***********************************************************************/
//! @brief Get the mode for the given tag.
/*!
  @param vpTag Tag to retrieve mode from.
  @returns One of XB_IX_DBASE_MODE or XB_IX_XBASE_MODE
*/

xbInt16 xbIxNdx::GetIxTagMode( const void *vpTag ) const {
  xbNdxTag * npTag;
  vpTag ? npTag = (xbNdxTag *) vpTag : npTag = ndxTag;
  return npTag->iIxTagMode;
}

/***********************************************************************/
//! @brief Get the key expression for the given tag.
/*!
  @param vpTag Tag to retrieve expression from.
  @returns Key expression.
*/

xbString &xbIxNdx::GetKeyExpression( const void * vpTag ) const{
  xbNdxTag * npTag;
  vpTag ? npTag = (xbNdxTag *) vpTag : npTag = ndxTag;
  return npTag->sKeyExpression;
}


/***********************************************************************/
//! @brief Get the key filter for the given tag.
/*!
  NDX index files do not support filters. This returns NULL.
  @returns NULL.
*/

xbString &xbIxNdx::GetKeyFilter( const void * ) const{
  return sNullString;
}
/***********************************************************************/
//! @brief Get the key length for the given tag.
/*!
  @param vpTag Tag to retrieve key length for.
  @returns Length of key.
*/
xbInt32 xbIxNdx::GetKeyLen( const void * vpTag ) const{
  xbNdxTag * npTag;
  vpTag ? npTag = (xbNdxTag *) vpTag : npTag = ndxTag;
  return npTag->iKeyLen;
}
/***********************************************************************/
//! @brief Get child node number for given key number.
/*!
  @param vpTag Tag to retrieve dbf rec number on.
  @param iKeyNo Key number for retrieval
  @param np Pointer to node
  @param ulKeyPtr Output node number
  @returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbIxNdx::GetKeyPtr( void *vpTag, xbInt16 iKeyNo, xbIxNode *np, xbUInt32 &ulKeyPtr ) const {

  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;
  try{
    #ifdef XB_DEBUG_SUPPORT
    // turn this off in production mode for better performance
    xbUInt32 ulNoOfKeys = eGetUInt32 ( np->cpBlockData );
    if( iKeyNo < 0 || iKeyNo > (xbInt16) ulNoOfKeys ){
      iErrorStop = 100;
      throw XB_INVALID_KEYNO;
    }
    #endif
    xbNdxTag * npTag;
    vpTag ? npTag = (xbNdxTag *) vpTag : npTag = ndxTag;
    char *p = ( np->cpBlockData);
    p += (4 + (iKeyNo * npTag->iKeyItemLen));
    ulKeyPtr = eGetUInt32 ( p );
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxNdx::GetKeyPtr() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}

/***********************************************************************/
//! @brief Returns key update status.
/*!
  @param vpTag Tag to check status on.
  @returns XB_UPD_KEY  Key updated.<br>
           XB_DEL_KEY  Key deleted.<br>
           XB_ADD_KEY  Key added.<br>
           0           No key updates

*/
xbInt16 xbIxNdx::GetKeySts( void *vpTag ) const{
  xbNdxTag * npTag;
  vpTag ? npTag = (xbNdxTag *) vpTag : npTag = ndxTag;
  return npTag->iKeySts;
}
/***********************************************************************/
//! @brief Get character key type for given tag.
/*!
  @param vpTag Tag to retrieve key type for.
  @returns Char key type.
*/

char xbIxNdx::GetKeyType( const void * vpTag ) const{
  xbNdxTag * npTag;
  vpTag ? npTag = (xbNdxTag *) vpTag : npTag = ndxTag;
  return npTag->cKeyType;
}

/***********************************************************************/
//! @brief Get numeric key type for given tag.
/*!
  @param vpTag Tag to retrieve first key for.
  @returns Numeric key type.
*/
xbInt16 xbIxNdx::GetKeyTypeN( const void * vpTag ) const{
  xbNdxTag * npTag;
  vpTag ? npTag = (xbNdxTag *) vpTag : npTag = ndxTag;
  return npTag->iKeyType;
}
/***********************************************************************/
//! @brief Get the last key for the given tag.
/*!
  @param vpTag Tag to retrieve last key on.
  @param iRetrieveSw xbTrue - Retrieve the record on success.<br>
                     xbFalse - Don't retrieve record.
  @returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbIxNdx::GetLastKey( void *vpTag, xbInt16 iRetrieveSw ){
  return GetLastKey( 0, vpTag, iRetrieveSw );
//  return GetLastKey( 0, vpTag, 1 );
  
}
/***********************************************************************/
//! @brief Get the last key for the given tag and starting node.
/*!
  @param ulNodeNo Starting node
  @param vpTag Tag to retrieve last key on.
  @param iRetrieveSw xbTrue - Retrieve the record if key found.<br>
                     xbFalse - Don't retrieve record.
  @returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbIxNdx::GetLastKey( xbUInt32 ulNodeNo, void *vpTag, xbInt16 iRetrieveSw ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbUInt32 ulKeyPtr = 0;
  xbUInt32 ulNoOfKeys = 0;
  xbNdxTag * npTag;
  vpTag ? npTag = (xbNdxTag *) vpTag : npTag = ndxTag;

  try{
    // clear out any history
    if( npTag->npNodeChain ){
      npTag->npNodeChain = FreeNodeChain( npTag->npNodeChain );
      npTag->npCurNode = NULL;
    }
    if( ulNodeNo == 0 ){
      if(( iRc = ReadHeadBlock( 1 )) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      }
       // lRootBlock is now available
      if(( iRc = GetBlock( npTag, npTag->ulRootBlock, 1, (xbUInt32) npTag->iKeyItemLen )) != XB_NO_ERROR ){
        iErrorStop = 110;
        throw iRc;
      }
    } else {
      if(( iRc = GetBlock( npTag, ulNodeNo, 1, (xbUInt32) npTag->iKeyItemLen )) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }
    }
    // if no keys on this node, then the index is empty
    ulNoOfKeys = eGetUInt32( npTag->npCurNode->cpBlockData );
    if( ulNoOfKeys == 0 && IsLeaf( npTag, npTag->npCurNode )){
      iRc = XB_EMPTY;
      return iRc;
    }
    npTag->npCurNode->iCurKeyNo = ulNoOfKeys;
    while( !IsLeaf( npTag, npTag->npCurNode ) ){   // go down the chain looking for a leaf node
      npTag->npCurNode->iCurKeyNo = eGetUInt32( npTag->npCurNode->cpBlockData );
      if(( iRc = GetKeyPtr( npTag, npTag->npCurNode->iCurKeyNo, npTag->npCurNode, ulKeyPtr )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }
      if(( iRc = GetBlock( npTag, ulKeyPtr, 1, (xbUInt32) npTag->iKeyItemLen )) != XB_NO_ERROR ){
        iErrorStop = 140;
        throw iRc;
      }
      ulNoOfKeys = eGetUInt32( npTag->npCurNode->cpBlockData );
      npTag->npCurNode->iCurKeyNo = ulNoOfKeys;
    }
    // get here on a leaf node, it has one fewer iCurKeyNo
    npTag->npCurNode->iCurKeyNo--;
    if( iRetrieveSw ){
      ulNoOfKeys = eGetUInt32( npTag->npCurNode->cpBlockData );
      if(( iRc = GetDbfPtr( npTag, ulNoOfKeys - 1, npTag->npCurNode, ulKeyPtr )) != XB_NO_ERROR ){
        iErrorStop = 150;
        throw iRc;
      }
      if(( iRc = dbf->GetRecord( ulKeyPtr )) != XB_NO_ERROR ){
        iErrorStop = 160;
        throw iRc;
      }
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxNdx::GetLastKey() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}

/***********************************************************************/
//! @brief Get the last key for a block number.
/*!
  @param vpTag Tag to retrieve first key on.
  @param ulBlockNo Block number for key retrieval.
  @param cpBuf output buffer for key placement
  @returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbIxNdx::GetLastKeyForBlockNo( void *vpTag, xbUInt32 ulBlockNo, char *cpBuf ){

  // returns the last key for a given block number
  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;

  xbNdxTag * npTag;
  vpTag ? npTag = (xbNdxTag *) vpTag : npTag = ndxTag;

  try{
    xbIxNode * npSaveNodeChain = npTag->npNodeChain;
    xbIxNode * npSaveCurNode = npTag->npCurNode;
    npTag->npNodeChain = NULL;

    if(( iRc = GetLastKey( ulBlockNo, npTag, 0 )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }
    // set the key
    memcpy( cpBuf, GetKeyData( npTag->npCurNode, GetKeyCount( npTag->npCurNode ) - 1, npTag->iKeyItemLen ), (size_t) npTag->iKeyLen );

     // free memory
    npTag->npNodeChain = FreeNodeChain( npTag->npNodeChain );
    npTag->npNodeChain = npSaveNodeChain;
    npTag->npCurNode   = npSaveCurNode;
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxNdx::GetLastKeyForBlockNo() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ) );
  }
  return iRc;
}
/***********************************************************************/
//! @brief Get the next key for the given tag.
/*!
  @param vpTag Tag to retrieve next key on.
  @param iRetrieveSw xbTrue - Retrieve the record on success.<br>
                     xbFalse - Don't retrieve record.
  @returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbIxNdx::GetNextKey( void * vpTag, xbInt16 iRetrieveSw ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbNdxTag * npTag;
  vpTag ? npTag = (xbNdxTag *) vpTag : npTag = ndxTag;

  try{
    if( !npTag->npNodeChain )
      return GetFirstKey( vpTag, iRetrieveSw );

    // more keys on this node?  if yes, get the next one to the right
    xbUInt32 ulKeyPtr;
    if((eGetUInt32( npTag->npCurNode->cpBlockData ) -1) > npTag->npCurNode->iCurKeyNo ){
      npTag->npCurNode->iCurKeyNo++;
      if(( iRc = GetDbfPtr( npTag, npTag->npCurNode->iCurKeyNo, npTag->npCurNode, ulKeyPtr )) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      }
      if( iRetrieveSw ){
        if(( iRc = dbf->GetRecord( ulKeyPtr )) != XB_NO_ERROR ){
          iErrorStop = 110;
          throw iRc;
        } else {
          return iRc;
        }
      } else {
        return iRc;
      }
    }
    // if at end of head node, then eof
    if( npTag->npCurNode->ulBlockNo == npTag->ulRootBlock )
      return XB_EOF;

    // This logic assumes that interior nodes have n+1 left node no's where n is he the number of keys in the node
    xbIxNode * TempIxNode = npTag->npCurNode;
    npTag->npCurNode = npTag->npCurNode->npPrev;
    TempIxNode = FreeNodeChain( TempIxNode );

    while( npTag->npCurNode->iCurKeyNo >= eGetUInt32( npTag->npCurNode->cpBlockData ) &&
           (npTag->npCurNode->ulBlockNo != npTag->ulRootBlock )){
      TempIxNode = npTag->npCurNode;
      npTag->npCurNode = npTag->npCurNode->npPrev;
      TempIxNode = FreeNodeChain( TempIxNode );
    }

    // head node and at end of head node, then eof
    if( npTag->npCurNode->ulBlockNo == npTag->ulRootBlock && 
        npTag->npCurNode->iCurKeyNo == eGetUInt32( npTag->npCurNode->cpBlockData ))
      return XB_EOF;

    // move one to the right
    npTag->npCurNode->iCurKeyNo++;

    if(( iRc = GetKeyPtr( npTag, npTag->npCurNode->iCurKeyNo, npTag->npCurNode, ulKeyPtr )) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw iRc;
    }

    if(( iRc = GetBlock( npTag, ulKeyPtr, 1, (xbUInt32) npTag->iKeyItemLen )) != XB_NO_ERROR ){
      iErrorStop = 130;
      throw iRc;
    }
    while( !IsLeaf( npTag, npTag->npCurNode ))   // go down the chain looking for a leaf node
    {
      if(( iRc = GetKeyPtr( npTag, 0, npTag->npCurNode, ulKeyPtr )) != XB_NO_ERROR ){
        iErrorStop = 140;
        throw iRc;
      }
      if(( iRc = GetBlock( npTag, ulKeyPtr, 1, (xbUInt32) npTag->iKeyItemLen )) != XB_NO_ERROR ){
        iErrorStop = 150;
        throw iRc;
      }
    }
    if( iRetrieveSw ){
      if(( iRc = GetDbfPtr( npTag, 0, npTag->npCurNode, ulKeyPtr )) != XB_NO_ERROR ){
        iErrorStop = 160;
        throw iRc;
      }
      if(( iRc = dbf->GetRecord( ulKeyPtr )) != XB_NO_ERROR ){
        iErrorStop = 170;
        throw iRc;
      }
    }
  }
  catch( xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxNdx::GetNextKey() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
/***********************************************************************/
//! @brief Get the previous key for the given tag.
/*!
  @param vpTag Tag to retrieve previous key on.
  @param iRetrieveSw xbTrue - Retrieve the record on success.<br>
                     xbFalse - Don't retrieve record.
  @returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbIxNdx::GetPrevKey( void * vpTag, xbInt16 iRetrieveSw ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbNdxTag * npTag;
  vpTag ? npTag = (xbNdxTag *) vpTag : npTag = ndxTag;

  // This method assumes last index call landed on a valid key.
  // If last call resulted in an error, this method will returns XB_BOF

  try{
    if( !npTag->npNodeChain )
      return GetLastKey( 0, vpTag, iRetrieveSw );

    xbUInt32 ulKeyPtr;
    if( npTag->npCurNode->iCurKeyNo > 0 ){
      npTag->npCurNode->iCurKeyNo--;

      if(( iRc = GetDbfPtr( npTag, npTag->npCurNode->iCurKeyNo, npTag->npCurNode, ulKeyPtr )) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      }
      if( iRetrieveSw ){
        if(( iRc = dbf->GetRecord( ulKeyPtr )) != XB_NO_ERROR ){
          iErrorStop = 110;
          throw iRc;
        } else {
          return iRc;
        }
      }
    }

    // next two lines might have been an issue
    if( npTag->npCurNode->ulBlockNo == npTag->ulRootBlock && GetKeyCount( npTag->npCurNode ) == 0  && IsLeaf( npTag, npTag->npCurNode ))
      return XB_EMPTY;

    if( npTag->npCurNode->ulBlockNo == npTag->ulRootBlock )
      return XB_BOF;

    // This logic assumes that interior nodes have n+1 left node no's where n is he the number of keys in the node
    xbIxNode * TempIxNode = npTag->npCurNode;
    npTag->npCurNode = npTag->npCurNode->npPrev;
    TempIxNode = FreeNodeChain( TempIxNode );

    while( npTag->npCurNode->iCurKeyNo == 0 &&
           (npTag->npCurNode->ulBlockNo != npTag->ulRootBlock )){
      TempIxNode = npTag->npCurNode;
      npTag->npCurNode = npTag->npCurNode->npPrev;
      TempIxNode = FreeNodeChain( TempIxNode );
    }

    // head node and at end of head node, then bof
    if( npTag->npCurNode->ulBlockNo == npTag->ulRootBlock && 
        npTag->npCurNode->iCurKeyNo == 0 )
      return XB_BOF;

    // move one to the left
    npTag->npCurNode->iCurKeyNo--;

    if(( iRc = GetKeyPtr( npTag, npTag->npCurNode->iCurKeyNo, npTag->npCurNode, ulKeyPtr )) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw iRc;
    }

    if(( iRc = GetBlock( npTag, ulKeyPtr, 1, (xbUInt32) npTag->iKeyItemLen )) != XB_NO_ERROR ){
      iErrorStop = 130;
      throw iRc;
    }

    while( !IsLeaf( npTag, npTag->npCurNode )){   // go down the chain looking for a leaf node
      npTag->npCurNode->iCurKeyNo = eGetUInt32( npTag->npCurNode->cpBlockData );
      if(( iRc = GetKeyPtr( npTag, npTag->npCurNode->iCurKeyNo, npTag->npCurNode, ulKeyPtr )) != XB_NO_ERROR ){
        iErrorStop = 140;
        throw iRc;
      }
      if(( iRc = GetBlock( npTag, ulKeyPtr, 1, (xbUInt32) npTag->iKeyItemLen )) != XB_NO_ERROR ){
        iErrorStop = 150;
        throw iRc;
      }
    }

    npTag->npCurNode->iCurKeyNo = eGetUInt32( npTag->npCurNode->cpBlockData ) - 1;
    if( iRetrieveSw ){
      if(( iRc = GetDbfPtr( npTag, npTag->npCurNode->iCurKeyNo, npTag->npCurNode, ulKeyPtr )) != XB_NO_ERROR ){
        iErrorStop = 160;
        throw iRc;
      }
      if(( iRc = dbf->GetRecord( ulKeyPtr )) != XB_NO_ERROR ){
        iErrorStop = 170;
        throw iRc;
      }
    }
  }

  catch( xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxNdx::GetPrevKey() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
/***********************************************************************/
//! @brief Get the sort order for given tag.
/*!
  Ndx indices only support ascending keys.
  @returns 0
*/
xbBool xbIxNdx::GetSortOrder( void * ) const{
  return 0;
}
/***********************************************************************/
//! @brief Get tag for tag number.
/*!
  @returns Pointer to ndx tag.
*/
void * xbIxNdx::GetTag( xbInt16 ) const{
  return ndxTag;
}
/***********************************************************************/
//! @brief Get tag for tag name.
/*!
  @returns Pointer to ndx tag.
*/
void * xbIxNdx::GetTag( xbString & ) const{
  return ndxTag;
}

/***********************************************************************/
//! @brief Get tag count.
/*!
  NDX index files contain one tag.
  @returns 1
*/

xbInt16 xbIxNdx::GetTagCount() const{
  return 1;
}
/***********************************************************************/
//! @brief Get tag name.
/*!
  @returns Tag name.
*/
xbString &xbIxNdx::GetTagName( void * ) const {
// char * xbIxNdx::GetTagName( void * ) const {

  return ndxTag->sTagName;

}
/***********************************************************************/
//! @brief Get tag name.
/*!
  @returns Tag name.
*/
const char * xbIxNdx::GetTagName( void *, xbInt16 ) const {
  return ndxTag->sTagName;
}

/***********************************************************************/
//! @brief Get the unique setting for given tag.
/*!
  @param vpTag Tag to unique setting on.
  @returns xbTrue - Unique index.<br> xbFalse - Not unique index.
*/
xbBool xbIxNdx::GetUnique( void * vpTag ) const{
  xbNdxTag * npTag;
  vpTag ? npTag = (xbNdxTag *) vpTag : npTag = ndxTag;
  return npTag->iUnique;
}

/***********************************************************************/
//! @brief Insert key into interior node.
/*!
  Insert key into non-full interior node.<br>
  Assumes valid inputs

  @param vpTag Tag in play.
  @param npNode Node for insertion.
  @param iSlotNo Slot number to insert key.
  @param ulPtr Pointer number to insert.
  @returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbIxNdx::InsertNodeI( void *vpTag, xbIxNode *npNode, xbInt16 iSlotNo, xbUInt32 ulPtr ){
  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  char *pTrg;
  xbInt16 iSrcPos;
  char *pLastKey = NULL;
  xbNdxTag * npTag;
  vpTag ? npTag = (xbNdxTag *) vpTag : npTag = ndxTag;

  try{
    // update number of keys on the node
    xbInt32 lKeyCnt = GetKeyCount( npNode ); 
    iSrcPos = 12 + (iSlotNo * npTag->iKeyItemLen);

    char *pSrc = npNode->cpBlockData;
    pSrc += iSrcPos;

    // if not appending to the end of the node, make some room, move things to the right
    if( iSlotNo < lKeyCnt ) {
      xbInt16 iCopyLen = ((lKeyCnt - iSlotNo) * npTag->iKeyItemLen) - 4;
      pTrg = pSrc;
      pTrg += npTag->iKeyItemLen;
      memmove( pTrg, pSrc, (size_t) iCopyLen );
    }

    // get the right most key for the left part of the split node
    xbUInt32 ulKeyPtr2;
    if(( iRc = GetKeyPtr( vpTag, npNode->iCurKeyNo, npNode, ulKeyPtr2 )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }
    // get the new right key value for the freshly split node
    pLastKey = (char *) malloc((size_t) ndxTag->iKeyLen);
    if(( iRc = GetLastKeyForBlockNo( vpTag, ulKeyPtr2, pLastKey )) != XB_NO_ERROR ){
      iRc = 110;
      throw iRc;
    }
    // write the key value
    pTrg = pSrc;
    char *pTrg2 = pSrc;
    pSrc = pLastKey;
    for( xbInt16 i = 0; i < npTag->iKeyLen; i++ )
      *pTrg++ = *pSrc++;

    pTrg2 += (npTag->iKeyItemLen - 8);
    ePutUInt32( pTrg2, ulPtr );
    ePutInt32( npNode->cpBlockData, ++lKeyCnt );

    // write out the updated block to disk
    if(( iRc = WriteBlock( npNode->ulBlockNo, GetBlockSize(), npNode->cpBlockData )) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw iRc;
    }
    if( pLastKey )
      free( pLastKey );
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxNdx::InsertNodeI() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
    if( pLastKey )
      free( pLastKey );
  }
  return iRc;
}
/***********************************************************************/
//! @brief Insert key into leaf node.
/*!
  Insert key into non-full leaf node.<br>
  Assumes valid inputs

  @param vpTag Tag in play.
  @param npNode Node for insertion.
  @param iSlotNo Slot number to insert key.
  @param ulPtr Pointer number to insert.
  @returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbIxNdx::InsertNodeL( void *vpTag, xbIxNode *npNode, xbInt16 iSlotNo, 
                                char * cpKeyBuf, xbUInt32 ulPtr ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  char *pSrc;
  char *pTrg;
  char *pKeyPos;
  xbString sMsg;
  xbNdxTag * npTag;
  vpTag ? npTag = (xbNdxTag *) vpTag : npTag = ndxTag;

  try{
    xbInt32 lKeyCnt = GetKeyCount( npNode ); 
    xbInt16 iKeyPos = 4 + iSlotNo * npTag->iKeyItemLen;
    pKeyPos = npNode->cpBlockData;
    pKeyPos += iKeyPos;

    // if not appending to end, make space, move things right
    if( iSlotNo < lKeyCnt ) {
      xbInt16 iCopyLen = (lKeyCnt - iSlotNo) * npTag->iKeyItemLen;
      pTrg = pKeyPos;
      pTrg += npTag->iKeyItemLen;
      memmove( pTrg, pKeyPos, (size_t) iCopyLen );
    }
    // if leaf, write rec number
    pTrg = pKeyPos;
    memset( pTrg, 0x00, 4 );
    pTrg += 4;
    ePutUInt32( pTrg, ulPtr );
    pTrg += 4;

    // write the key value
    pSrc = cpKeyBuf;
    for( xbInt16 i = 0; i < npTag->iKeyLen; i++ )
      *pTrg++ = *pSrc++;

    // update number of keys on the node
    ePutInt32( npNode->cpBlockData, ++lKeyCnt );

    // write out the updated block to disk
    if(( iRc = WriteBlock( npNode->ulBlockNo, GetBlockSize(), npNode->cpBlockData )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxNdx::InsertNodeL() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
/***********************************************************************/
//! @brief Determine node leaf status
/*!
  @param npNode Node to examine.
  @returns xbTrue - Leaf node.<br> xbFalse - Interior node.
*/
xbBool xbIxNdx::IsLeaf( void *, xbIxNode *npNode ) const {
  xbUInt32 ulBlock = eGetUInt32 ( npNode->cpBlockData+4 );
  if( ulBlock > 0 )    // if the second four bytes are a number, it's an interior node
    return false;
  else
    return true;
}
/***********************************************************************/
//! @brief Determine if key exists.
/*!
  This method assumes the key has already been built and is in either 
  cpKeyBuf or dKey.

  @param vpTag - Pointer to tag.
  @returns xbTrue - Key exists.<br> xbFalse - Key does not exist.
*/
xbInt16 xbIxNdx::KeyExists( void * vpTag ){

  xbNdxTag * npTag;
  vpTag ? npTag = (xbNdxTag *) vpTag : npTag = ndxTag;

  xbInt16 iRc = FindKey( vpTag, npTag->cpKeyBuf, npTag->iKeyLen, 0 );
  if( iRc == 0 )
    return 1;
  else
    return 0;
}

/***********************************************************************/
//! @brief Set position for key add.
/*!
  This routine is called by the AddKey() method and is used to position 
  the node chain to the position the new key should be added to the index.

  @param npTag Pointer to npTag.
  @param ulAddRecNo Record number to add.
  @returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbIxNdx::KeySetPosAdd( xbNdxTag *npTag, xbUInt32 ulAddRecNo ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  try{

     iRc = FindKey( npTag, npTag->cpKeyBuf, npTag->iKeyLen, 0 );
     if( iRc == XB_NOT_FOUND || iRc == XB_EMPTY )
       return XB_NO_ERROR;  // good position

     else if( iRc != XB_NO_ERROR ){
       iErrorStop = 100;
       throw iRc;
     }
     // get here if key was found, get the right most instance of any non unique key for append, find correct spot for update 
     if( GetUnique() == 0 ){
       xbUInt32 ulCurRecNo;
       if(( iRc = GetDbfPtr( npTag, npTag->npCurNode->iCurKeyNo, npTag->npCurNode, ulCurRecNo )) != XB_NO_ERROR ){
         iErrorStop = 110;
         throw iRc;
       }
       xbBool bKeysMatch = xbTrue;
       while( bKeysMatch && ulAddRecNo > ulCurRecNo && iRc == XB_NO_ERROR ){
         if(( iRc = GetNextKey( npTag, 0 )) == XB_NO_ERROR ){
           if( memcmp( GetKeyData( npTag->npCurNode, npTag->npCurNode->iCurKeyNo, npTag->iKeyItemLen ), npTag->cpKeyBuf, (size_t) npTag->iKeyLen ))
             bKeysMatch = xbFalse;
           else{
             if(( iRc = GetDbfPtr( npTag, npTag->npCurNode->iCurKeyNo, npTag->npCurNode, ulCurRecNo )) != XB_NO_ERROR ){
               iErrorStop = 120;
               throw iRc;
             }
           }
         }
       }
     }
     if( iRc == XB_EOF ){   // eof condition
       if(( iRc = GetLastKey( 0, npTag, 0 )) != XB_NO_ERROR ){
         iErrorStop = 130;
         throw iRc;
       }
       npTag->npCurNode->iCurKeyNo++;
       return XB_NO_ERROR;
     }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxNdx::KeySetPos() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}

/***********************************************************************/
//! @brief Set position for key add.
/*!
  This routine is called by the DeleteKey() method and is used to position 
  the node chain to the position the old key should be deleted from the index.

  @param npTag Pointer to npTag.
  @returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbIxNdx::KeySetPosDel( xbNdxTag *npTag ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbString sMsg;

  try{
     iRc = FindKey( NULL, npTag->cpKeyBuf2, npTag->iKeyLen, 0 );
     if( iRc == XB_NOT_FOUND || iRc == XB_EMPTY )
       return XB_NOT_FOUND;

     else if( iRc != XB_NO_ERROR ){
       iErrorStop = 100;
       throw iRc;
     }
     xbUInt32 ulIxRecNo;
     if(( iRc = GetDbfPtr( npTag, npTag->npCurNode->iCurKeyNo, npTag->npCurNode, ulIxRecNo )) != XB_NO_ERROR ){
       iErrorStop = 110;
       throw iRc;
     }

     if( ulIxRecNo == dbf->GetCurRecNo()) 
       return XB_NO_ERROR;
     if( GetUnique() == 1 ){
       iErrorStop = 120;
       iRc = XB_NOT_FOUND;
       throw iRc;
     }

     xbBool bFound = xbFalse;
     xbBool bKeysMatch = xbTrue;

     while( bKeysMatch && !bFound && iRc == XB_NO_ERROR ){

       if(( iRc =  GetNextKey( npTag, 0 )) != XB_NO_ERROR ){
         iErrorStop = 130;
         throw iRc;
       }
       if( memcmp( GetKeyData( npTag->npCurNode, npTag->npCurNode->iCurKeyNo, npTag->iKeyItemLen ), npTag->cpKeyBuf2, (size_t) npTag->iKeyLen )){
         bKeysMatch = xbFalse;
       } else {
         if(( iRc = GetDbfPtr( npTag, npTag->npCurNode->iCurKeyNo, npTag->npCurNode, ulIxRecNo )) != XB_NO_ERROR ){
           iErrorStop = 140;
           throw iRc;
         }
         if( ulIxRecNo == dbf->GetCurRecNo())
           bFound = xbTrue;
       }
     }

     if( bFound )
       return XB_NO_ERROR;
     else
       return XB_NOT_FOUND;
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxNdx::KeySetPosDel() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
/***********************************************************************/
//! @brief Returns key filter status.
/*!
  @param vpTag Tag to check status on.
  @returns xbtrue - Key was updated.<br>xbFalse - Key not updated.

  Always true for NDX style indices.
*/
//inline xbBool xbIxNdx::KeyFiltered( void *vpTag ) const{
//  return xbTrue;
//}

/***********************************************************************/
//! @brief Read head block of index file.
/*!
  @param iOpt 0 - Read in entire block
              1 - Read in only dynamic section of block
  @returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbIxNdx::ReadHeadBlock( xbInt16 iOpt = 0 ) {

  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;
  try{
    if( !FileIsOpen()){
      iRc = XB_NOT_OPEN;
      iErrorStop = 100;
      throw iRc;
    }
    xbInt16 iLen;
    iOpt == 0 ? iLen = 512 : iLen = 21;

    if(( iRc = ReadBlock( (xbUInt32) 0, (size_t) iLen, cNodeBuf )) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }
    char *p        = cNodeBuf;
    ndxTag->ulRootBlock      = eGetUInt32( p ); p+=4;
    ndxTag->ulTotalBlocks    = eGetUInt32( p ); p+=5;
    if( iOpt == 0 ){
      ndxTag->cKeyType       = *p;              p+=3;
      ndxTag->iKeyLen        = eGetInt16( p );  p+=2;
      ndxTag->iKeysPerBlock   = eGetInt16( p ); p+=2;
      ndxTag->iKeyType       = eGetInt16( p );  p+=2;
      ndxTag->iKeyItemLen    = eGetInt16( p );  p+=2;
      ndxTag->cSerNo         = *p;              p+=3;
      ndxTag->iUnique        = *p;              p++;
      ndxTag->sKeyExpression.Set( p );

      // Assume default setting when opening index
      ndxTag->iIxTagMode     = xbase->GetDefaultIxTagMode();

      if( ndxTag->exp )
        delete ndxTag->exp;

      ndxTag->exp = new xbExp( xbase, dbf );
      if(( iRc = ndxTag->exp->ParseExpression( ndxTag->sKeyExpression.Str() )) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }

      if( ndxTag->cpKeyBuf )
        free( ndxTag->cpKeyBuf );
      if( ndxTag->cpKeyBuf2 )
        free( ndxTag->cpKeyBuf2 );

      ndxTag->cpKeyBuf  = (char *) malloc( (size_t) ndxTag->iKeyLen );
      ndxTag->cpKeyBuf2 = (char *) malloc( (size_t) ndxTag->iKeyLen );

      if( ndxTag->sTagName == "" )
        GetFileNamePart( ndxTag->sTagName );

    } else {
      p+= 11;
      ndxTag->cSerNo  = *p;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxNdx::ReadHeadBlock() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}

/***********************************************************************/
//! @brief Reindex a tag.
/*!
  @param vpTag Pointer to tag pointer.
  @returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbIxNdx::ReindexTag( void **vpTag ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbNdxTag *npTag = ndxTag;

  try{
    xbInt16  iUnique   = GetUnique( *vpTag );

    npTag->npNodeChain   = FreeNodeChain( npTag->npNodeChain );
    npTag->npCurNode     = NULL;
    npTag->ulRootBlock   = 1L;
    npTag->ulTotalBlocks = 2L;

    if(( iRc = xbTruncate( 1024 )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }

    if(( iRc = WriteHeadBlock( 1 )) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }
    char buf[512];
    memset( buf, 0x00, 512 );

    if(( iRc = WriteBlock( 1, 0, buf )) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw iRc;
    }

    xbUInt32 ulRecCnt = 0;
    if(( iRc = dbf->GetRecordCnt( ulRecCnt )) != XB_NO_ERROR ){
      iErrorStop = 130;
      throw iRc;
    }

    for( xbUInt32 l = 1; l <= ulRecCnt; l++ ){
      if(( iRc = dbf->GetRecord( l )) != XB_NO_ERROR ){
        iErrorStop = 140;
        throw iRc;
      }

      if(( iRc = CreateKey( npTag, 1 )) != XB_NO_ERROR ){
        iErrorStop = 150;
        throw iRc;
      }

      if( iUnique ){
        // iRc = CheckForDupKey( vpTag2 );
        iRc = CheckForDupKey( npTag );
        if( iRc != 0 ){
          if( iRc < 0 ){
            iErrorStop = 160;
            throw iRc;
          }
          return XB_KEY_NOT_UNIQUE;
        }
      }

      if(( iRc = AddKey( npTag, l )) != XB_NO_ERROR ){
        iErrorStop = 150;
        throw iRc;
      }
    }
    *vpTag = npTag;
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxNdx::Reindex() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
    this->DeleteTag( NULL );     // Don't leave the index in an incomplete state
  }
  return iRc;
}
/***********************************************************************/
//! @brief Set current tag.
/*!
  For ndx indices, there is only one tag.
  @returns XB_NO_ERROR.
*/
xbInt16 xbIxNdx::SetCurTag( xbInt16 ) {
  xbIx::SetCurTag( ndxTag );
  return XB_NO_ERROR;
}
/***********************************************************************/
//! @brief Set current tag.
/*!
  For ndx indices, there is only one tag.
  @returns XB_NO_ERROR.
*/
xbInt16 xbIxNdx::SetCurTag( xbString & ) {
  xbIx::SetCurTag( ndxTag );
  dbf->SetCurTag( "NDX", this, GetTag(0) );
  return XB_NO_ERROR;
}

/***********************************************************************/
//! @brief Set the mode for the given tag.
/*!
  @param vpTag Tag to retrieve mode from.
  @param iMode One of XB_IX_DBASE_MODE or XB_IX_XBASE_MODE
  @returns XB_INVALID_OPTION or XB_NO_ERROR
*/

xbInt16 xbIxNdx::SetIxTagMode( void *vpTag, xbInt16 iMode ){
  if( iMode != XB_IX_DBASE_MODE && iMode != XB_IX_XBASE_MODE )
    return XB_INVALID_OPTION;
  xbMdxTag * mpTag = (xbMdxTag *) vpTag;
  mpTag->iIxTagMode = iMode;
  return XB_NO_ERROR;
}
/***********************************************************************/
//! @brief Split an interior node
/*!

  This routine splits an interior node into two nodes, divided by dSplitFactor.<br>
  This behaves differently than V7 dBASE. V7 does not balance the nodes.<br>
  For V7, if adding a key to the end of a node, it will create a right node 
  with only one key, and the left node is still full.<br><br>

  Possible performance improvement options.<br>
  Two modes when splitting:<br>
  a)  Split nodes in the middle - good for random access applications<br>
  b)  Split off right node with only one key - good for applications with
      expectation of ascending keys added moving forward.<br>

  This routine first inserts the key into the left node in the appropriate location
  then splits the node based on the split factor setting.

  @param vpTag Tag in play.
  @param npLeft Left node to split.
  @param npRight Right node to split.
  @param iSlotNo Slot number for split.
  @param ulPtr Pointer number to insert.
  @returns <a href="xbretcod_8h.html">Return Codes</a>  
*/

xbInt16 xbIxNdx::SplitNodeI( void *vpTag, xbIxNode * npLeft, xbIxNode *npRight, xbInt16 iSlotNo, xbUInt32 ulPtr ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbNdxTag * npTag;
  vpTag ? npTag = (xbNdxTag *) vpTag : npTag = ndxTag;
  xbDouble dSplitFactor = .5;                           // split the nodes 50/50
  xbString sMsg;

  try{
    xbInt32 lKeyCnt = GetKeyCount( npLeft );
    xbInt32 lNewLeftKeyCnt = (xbInt32) ((lKeyCnt + 1) * dSplitFactor) + 1;
    xbInt32 lNewRightKeyCnt = lKeyCnt - lNewLeftKeyCnt;
    xbInt16 iSrcPos;
    xbInt16 iCopyLen;
    char    *pSrc;
    char    *pTrg;

    // insert the key into the left node
    if(( iRc = InsertNodeI( vpTag, npLeft, iSlotNo, ulPtr )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }

    // move the right half of the left node to the right node
    iSrcPos  = ((lNewLeftKeyCnt + 1) * npTag->iKeyItemLen) + 4;
    iCopyLen = (lNewRightKeyCnt * npTag->iKeyItemLen) + 4;
    pSrc = npLeft->cpBlockData;
    pSrc += iSrcPos;
    pTrg = npRight->cpBlockData;
    pTrg += 4;
    memmove( pTrg, pSrc, (size_t) iCopyLen );

    // write the new key counts into the nodes
    pTrg = npLeft->cpBlockData;
    ePutInt32( pTrg, lNewLeftKeyCnt );
    pTrg = npRight->cpBlockData;
    ePutInt32( pTrg, lNewRightKeyCnt );

    // write the new key counts into the nodes
    pTrg = npLeft->cpBlockData;
    ePutInt32( pTrg, lNewLeftKeyCnt );
    pTrg = npRight->cpBlockData;
    ePutInt32( pTrg, lNewRightKeyCnt );

    // write out the block 
    if(( iRc = WriteBlock( npLeft->ulBlockNo, GetBlockSize(), npLeft->cpBlockData )) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }
    // write out the block 
    if(( iRc = WriteBlock( npRight->ulBlockNo, GetBlockSize(), npRight->cpBlockData )) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxNdx::SplitNodeI() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}

/***********************************************************************/
//! @brief Split a leaf node.
/*!
  This routine splits an index leaf into two nodes, divided by dSplitFactor.<br>
  This behaves differently than V7 dBASE. V7 does not balance the nodes.<br>
  For V7, if adding a key to the end of a node, it will create a right node 
  with only one key, and the left node is still full.<br><br>

  Possible performance improvement options.<br>
  Two modes when splitting:<br>
  a)  Split nodes in the middle - good for random access applications<br>
  b)  Split off right node with only one key - good for applications with 
      expectation of ascending keys added moving forward.<br>

  This routine first inserts the key into the left node in the appropriate location
  then splits the node based on the split factor setting.

  @param vpTag Tag in play.
  @param npLeft Left node to split.
  @param npRight Right node to split.
  @param iSlotNo Slot number for split.
  @param ulPtr Pointer number to insert.
  @returns <a href="xbretcod_8h.html">Return Codes</a>  
*/

xbInt16 xbIxNdx::SplitNodeL( void *vpTag, xbIxNode * npLeft, xbIxNode *npRight, 
    xbInt16 iSlotNo, char * cpKeyBuf, xbUInt32 ulPtr ){

  xbInt16  iRc = XB_NO_ERROR;
  xbInt16  iErrorStop = 0;
  xbDouble dSplitFactor = .5;
  xbNdxTag *npTag;
  vpTag ? npTag = (xbNdxTag *) vpTag : npTag = ndxTag;

  xbString sMsg;
  try{
    xbInt32 lKeyCnt = GetKeyCount( npLeft );
    xbInt32 lNewLeftKeyCnt = (xbInt32) ((lKeyCnt + 1) * dSplitFactor) + 1;
    xbInt32 lNewRightKeyCnt = lKeyCnt + 1 - lNewLeftKeyCnt;

    // xbInt16 iSrcPos;
    xbInt16 iLen;
    char *pSrc = npLeft->cpBlockData;
    char *pTrg;

    if(( iRc = InsertNodeL( vpTag, npLeft, iSlotNo, cpKeyBuf, ulPtr )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }

    // move right half off of left node to the right node
    pSrc = npLeft->cpBlockData;
    pSrc += ((lNewLeftKeyCnt * npTag->iKeyItemLen)+4);
    pTrg = npRight->cpBlockData;
    pTrg += 4;
    iLen = lNewRightKeyCnt * npTag->iKeyItemLen;
    memmove( pTrg, pSrc, (size_t) iLen );

    // write the new key counts into the nodes
    pTrg = npLeft->cpBlockData;
    ePutInt32( pTrg, lNewLeftKeyCnt );
    pTrg = npRight->cpBlockData;
    ePutInt32( pTrg, lNewRightKeyCnt );

    // write out the left block 
    if(( iRc = WriteBlock( npLeft->ulBlockNo, GetBlockSize(), npLeft->cpBlockData )) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }
    // write out the right block 
    if(( iRc = WriteBlock( npRight->ulBlockNo, GetBlockSize(), npRight->cpBlockData )) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxNdx::SplitNodeL() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}

/***********************************************************************/
//! @brief UpdateTagKey
/*!
  This routine updates a key or a given tag.
  The file header is considered to be the first 2048 bytes in the file.

  @param cAction A - Add a key.<br>
                 D - Delete a key.<br>
                 R - Revise a key.<br>
  @param vpTg - Pointer to tag.<br>
  @param ulRecNo - Record number association with the action.<br>
  @returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16  xbIxNdx::UpdateTagKey( char cAction, void *vpTag, xbUInt32 ulRecNo ){


  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  // ..xbNdxTag *npTag = (xbMdxTag *) vpTag;

  try{
    // save off any needed fileds for updating
    // xbUInt32 ulTagSizeSave    = mpTag->ulTagSize;
    //xbUInt32 ulLeftChildSave  = mpTag->ulLeftChild;
    //xbUInt32 ulRightChildSave = mpTag->ulRightChild;


    if( cAction == 'D' || cAction == 'R' ){
//      std::cout << "UpdateTagKey delete\n";
      if(( iRc = DeleteKey( vpTag )) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      }
    }

    if( cAction == 'A' || cAction == 'R' ){
      if(( iRc = AddKey( vpTag, ulRecNo )) != XB_NO_ERROR ){
        iErrorStop = 110;
        throw iRc;
      }
    }

  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxNdx::UpdateTagKey() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
/***********************************************************************/
//! @brief Write head block.
/*!
  Commit the index head node to disk.
  @param iOpt 0 - Entire header.<br>
              1 - Update root block, number of blocks and seq number.<br>
              2 - Update sequence number only<br>
  @returns <a href="xbretcod_8h.html">
*/

xbInt16 xbIxNdx::WriteHeadBlock( xbInt16 iOpt ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;

  try{
    if( iOpt == 2 ){

     // increment the serial number
      if( ndxTag->cSerNo >= 0 && ndxTag->cSerNo < 127 )
        ndxTag->cSerNo++;
      else
        ndxTag->cSerNo = 0;

      if(( iRc = xbFseek( 20, SEEK_SET )) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      }
      if(( iRc = xbFputc( ndxTag->cSerNo )) != XB_NO_ERROR ){
        iErrorStop = 110;
        throw iRc;
      }
    } else if( iOpt == 1 ){
      xbRewind();
      char buf[8];
      ePutUInt32( &buf[0], ndxTag->ulRootBlock );
      ePutUInt32( &buf[4], ndxTag->ulTotalBlocks );
      if(( iRc = xbFwrite( buf, 8, 1 )) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }
      return WriteHeadBlock( 2 );

    } else if ( iOpt == 0 ){

      char buf[512];
      memset( buf, 0x00, 512 );
      ePutUInt32( &buf[0], ndxTag->ulRootBlock );
      ePutUInt32( &buf[4], ndxTag->ulTotalBlocks );
      buf[9]  = ndxTag->cKeyType;
      buf[11] = 0x1B;
      ePutInt16( &buf[12], ndxTag->iKeyLen );
      ePutInt16( &buf[14], ndxTag->iKeysPerBlock );
      ePutInt16( &buf[16], ndxTag->iKeyType );
      ePutInt16( &buf[18], ndxTag->iKeyItemLen );
      if( ndxTag-> iUnique ) buf[23] = 0x01;

      for( xbUInt32 i = 0; i < ndxTag->sKeyExpression.Len(); i++ )
        buf[i+24] = ndxTag->sKeyExpression.GetCharacter(i+1);

      xbRewind();
      if(( iRc = xbFwrite( buf, 512, 1 )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }
    } else {
      iRc = XB_INVALID_OPTION;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxNdx::WriteHeadBlock() Exception Caught. Error Stop = [%d] iRc = [%d] option = [%d] ser=[%d]", iErrorStop, iRc, iOpt, ndxTag->cSerNo );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
/***********************************************************************/
}              /* namespace       */
#endif         /*  XB_NDX_SUPPORT */



