/* xbixmdx.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

   XDB-devel@lists.sourceforge.net
   XDB-users@lists.sourceforge.net


   MDX indices are comprised of blocks and pages.
   A page is 512 bytes.
   A Block is one or more pages.
   The default block size is two 512 byte pages per block.
   Nodes are used for storing block images in memory

*/

#include "xbase.h"

#ifdef XB_MDX_SUPPORT

namespace xb{

/***********************************************************************/
xbIxMdx::xbIxMdx( xbDbf *dbf ) : xbIx( dbf ){
   Init();
}
/***********************************************************************/
void xbIxMdx::Init( xbInt16 ){

   cVersion          = 0;
   cCreateYY         = 0;
   cCreateMM         = 0;
   cCreateDD         = 0;
   sFileName         = "";
   iBlockFactor      = 0;
   cProdIxFlag       = 0;
   cTagEntryCnt      = 0;
   iTagLen           = 0;
   iTagUseCnt        = 0;
   cNextTag          = 0;
   c1B               = 0x0b;
   ulPageCnt         = 0;
   ulFirstFreePage   = 0;
   ulNoOfBlockAvail  = 0;
   cUpdateYY         = 0;
   cUpdateMM         = 0;
   cUpdateDD         = 0;
   mdxTagTbl         = NULL;
   cNodeBuf          = NULL;
   bReuseEmptyNodes  = xbTrue;
   sIxType           = "MDX";
}
/***********************************************************************/
xbIxMdx::~xbIxMdx(){

  if( cNodeBuf )
    free( cNodeBuf );

  if( FileIsOpen())
    Close();

}
/***********************************************************************/
//! @brief Add key.
/*!
  Add key. If this is a unique index, this logic assumes the duplicate 
  check logic was already done.

  \param vpTag Tag to update.
  \param ulRecNo Record number to add key for.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbIxMdx::AddKey( void * vpTag, xbUInt32 ulRecNo ){

  xbMdxTag * npTag = (xbMdxTag *) vpTag;
  if( GetUniqueKeyOpt() == XB_EMULATE_DBASE && npTag->bFoundSts )
    return XB_NO_ERROR;

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbInt16 iHeadNodeUpdateOpt = 2;
  xbIxNode * npRightNode = NULL;
  xbUInt32 ulNewRightChild = 0;

  try{
     if(( iRc = xbIxMdx::KeySetPosAdd( npTag, ulRecNo )) != XB_NO_ERROR ){
       iErrorStop = 100;
       throw iRc;
     }
     xbInt32 lKeyCnt = GetKeyCount( npTag->npCurNode );
     if( lKeyCnt <  npTag->iKeysPerBlock ){
        // Section A - add key to appropriate position if space available
        // std::cout << "AddKey Section A begin\n";
        if(( iRc = InsertNodeL( npTag, npTag->npCurNode, npTag->npCurNode->iCurKeyNo, npTag->cpKeyBuf, ulRecNo )) != XB_NO_ERROR ){
          iErrorStop = 110;
          throw iRc;
        }
     } else {
       // land here with a full leaf node
       iHeadNodeUpdateOpt = 1;
       npRightNode = AllocateIxNode( npTag, GetBlockSize() + (xbUInt32) npTag->iKeyItemLen, npTag->npCurNode->ulBlockNo * (xbUInt32) iBlockFactor );
       if( !npRightNode ){
         iErrorStop = 120;
         iRc = XB_NO_MEMORY;
         throw iRc;
       }

       if(( npTag->npCurNode->ulBlockNo * (xbUInt32) iBlockFactor ) == npTag->ulRightChild ){
         ulNewRightChild = npRightNode->ulBlockNo * (xbUInt32) iBlockFactor;
       }

       if(( iRc = xbIxMdx::SplitNodeL( npTag, npTag->npCurNode, npRightNode, npTag->npCurNode->iCurKeyNo, npTag->cpKeyBuf, ulRecNo )) != XB_NO_ERROR ){
         iErrorStop = 130;
         throw iRc;
       }
       xbUInt32 ulTempBlockNo = npRightNode->ulBlockNo;

       // section C - go up the tree, splitting nodes as necessary
       xbIxNode * npParent = npTag->npCurNode->npPrev;
       while( npParent && GetKeyCount( npParent ) >= npTag->iKeysPerBlock ){
         //std::cout << "Section C begin  interior node is full\n";
         npRightNode = FreeNodeChain( npRightNode );
         npRightNode = AllocateIxNode( npTag, GetBlockSize() + (xbUInt32) npTag->iKeyItemLen, npParent->ulBlockNo * (xbUInt32) iBlockFactor );
         //std::cout << "Section C - B new right node block number for interior node split= " << npRightNode->ulBlockNo << "\n";

         if( !npRightNode ){
           iErrorStop = 140;
           iRc = XB_NO_MEMORY;
           throw iRc;
         }
         //std::cout << "Section C - going to split interior node C\n";

         if(( iRc = SplitNodeI( npTag, npParent, npRightNode, npParent->iCurKeyNo, BlockToPage( ulTempBlockNo ))) != XB_NO_ERROR ){
           iErrorStop = 150;
           throw iRc;
         }
         // std::cout << "Section C - interior node split \n";
         ulTempBlockNo = npRightNode->ulBlockNo;
         npTag->npCurNode = npParent;
         npParent = npParent->npPrev;
       }

       // section D - if cur node is split root, create new root
       if(( npTag->npCurNode->ulBlockNo * (xbUInt32) iBlockFactor ) == npTag->ulRootPage ){
         if(( iRc = AddKeyNewRoot( npTag, npTag->npCurNode, npRightNode )) != XB_NO_ERROR ){
             iErrorStop = 160;
             throw iRc;
         }
         if( npRightNode )
           npRightNode = FreeNodeChain( npRightNode );

       } else {

         // std::cout << "Section E, put key in parent\n";
         if(( iRc = InsertNodeI( (void *) vpTag, (xbIxNode *) npParent, (xbInt16) npParent->iCurKeyNo, BlockToPage( npRightNode->ulBlockNo ))) != XB_NO_ERROR ){
           iErrorStop = 170;
           throw iRc;
         }
       }
     }

     // update the header
     if(( iRc = WriteHeadBlock( iHeadNodeUpdateOpt )) != XB_NO_ERROR ){
       iErrorStop = 180;
       throw iRc;
     }

     // if adding the first key, set the cHasKeys field
     if( !npTag->cHasKeys ){
       npTag->cHasKeys = 0x01;
       if(( iRc = xbFseek( ((npTag->ulTagHdrPageNo * 512) + 246), SEEK_SET )) != XB_NO_ERROR ){
         iErrorStop = 190;
         throw iRc;
       }
       if(( iRc = xbFwrite( &npTag->cHasKeys, 1, 1 )) != XB_NO_ERROR ){
         iErrorStop = 200;
         throw iRc;
       }
     }

     if( ulNewRightChild > 0 ){

       char cBuf[4];
       ePutUInt32( cBuf, ulNewRightChild );
       if(( iRc = xbFseek( ((npTag->ulTagHdrPageNo * 512) + 252), SEEK_SET )) != XB_NO_ERROR ){
         iErrorStop = 210;
         throw iRc;
       }
       if(( iRc = xbFwrite( &cBuf, 4, 1 )) != XB_NO_ERROR ){
         iErrorStop = 220;
         throw iRc;
       }
       npTag->ulRightChild = ulNewRightChild;
     }

     if( npRightNode )
       npRightNode = FreeNodeChain( npRightNode );
     npTag->npNodeChain = FreeNodeChain( npTag->npNodeChain );
     npTag->npCurNode = NULL;

  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxMdx::AddKey() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}
/***********************************************************************/
void xbIxMdx::AppendNodeChain( void *vpTag, xbIxNode * npNode ){
    xbMdxTag * mdxTag = (xbMdxTag *) vpTag;
    if( mdxTag->npNodeChain == NULL ){
      mdxTag->npNodeChain = npNode;
      mdxTag->npCurNode = npNode;
    } else {
      npNode->npPrev = mdxTag->npCurNode;
      mdxTag->npCurNode->npNext = npNode;
      mdxTag->npCurNode = npNode;
    }
    // time stamp the node chain
    GetFileMtime( mdxTag->tNodeChainTs );
}

/***********************************************************************/
//! @brief Add new root node.
/*!
  \param mpTag Tag to update.
  \param npLeft Left node.
  \param npRight Right node.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbIxMdx::AddKeyNewRoot(  xbMdxTag *npTag, xbIxNode *npLeft, xbIxNode *npRight ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  char *pLastKey = NULL;

  try{
    xbIxNode *npRoot = AllocateIxNode( npTag, GetBlockSize() + (xbUInt32) npTag->iKeyItemLen, npRight->ulBlockNo * (xbUInt32) iBlockFactor );
    if( !npRoot ){
      iErrorStop = 100;
      iRc = XB_NO_MEMORY;
      throw iRc;
    }
    npTag->ulRootPage = npRoot->ulBlockNo;

    pLastKey = (char *) malloc( (size_t) npTag->iKeyLen );
    if(( iRc = GetLastKeyForBlockNo( npTag, npLeft->ulBlockNo, pLastKey )) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }
    char * pTrg = npRoot->cpBlockData;

    // set no of keys to 1
    ePutUInt32( pTrg, 1 );

    // set the left node number
    pTrg += 8;
    ePutUInt32( pTrg, npLeft->ulBlockNo * (xbUInt32) iBlockFactor );

    // set the key
    pTrg+= 4;
    memcpy( pTrg, pLastKey, (size_t) npTag->iKeyLen );
    pTrg+= npTag->iKeyItemLen - 4;
    ePutUInt32( pTrg, npRight->ulBlockNo * (xbUInt32) iBlockFactor );

     // write out the new block
    if(( iRc = WriteBlock( npRoot->ulBlockNo, GetBlockSize(), npRoot->cpBlockData )) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw iRc;
    }

    // write out the new root node number in the tag header
    // position the file
    xbUInt32 ulPagePos = npTag->ulTagHdrPageNo * 512;

    // save the number to a buffer
    char cBuf[4];
    ePutUInt32( cBuf, npRoot->ulBlockNo * ((xbUInt32) iBlockFactor ));

    if(( iRc = xbFseek( ulPagePos, SEEK_SET )) != XB_NO_ERROR ){
      iErrorStop = 130;
      throw iRc;
    }
    if(( iRc = xbFwrite( &cBuf, 4, 1 )) != XB_NO_ERROR ){
      iErrorStop = 140;
      throw iRc;
    }

    if( pLastKey ) 
      free( pLastKey );

    npRoot = FreeNodeChain( npRoot );

  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxMdx::AddKeyNewRoot() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
    if( pLastKey ) 
      free( pLastKey );
  }
  return iRc;
}



/***********************************************************************/
//! @brief Allocate a node.
/*!
  \param mpTag Pointer to mdx tag
  \param ulBufSize Buffer size.
  \param ulBlock2 Value to load in ulBlock2 field, bytes 4-7 in the first page of the block
  \returns Pointer to new node.
*/
xbIxNode * xbIxMdx::AllocateIxNode( xbMdxTag * mpTag, xbUInt32 ulBufSize, xbUInt32 ulBlock2 ){

  xbInt16  iRc = XB_NO_ERROR;
  xbInt16  iErrorStop = 0;
  xbIxNode *n = NULL;

  try{
    if(( n = xbIx::AllocateIxNode( ulBufSize )) == NULL ){
      iRc = XB_NO_MEMORY;
      iErrorStop = 100;
      throw iRc;
    }
    char *p = n->cpBlockData;
    p += 4;

    if( ulFirstFreePage > 0 && bReuseEmptyNodes ){
      // have an empty node we can reuse
      n->ulBlockNo = PageToBlock( ulFirstFreePage );
      if(( iRc = ReadBlock( n->ulBlockNo, GetBlockSize(), n->cpBlockData )) != XB_NO_ERROR ){
        iRc = 110;
        throw iRc;
      }
      // update ulFirstFreePage
      ulFirstFreePage = eGetUInt32( p );
      if(( iRc = xbFseek( 36, SEEK_SET )) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }
      if(( iRc = xbFwrite( p, 4, 1 )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }
      // memset cpBlockData to zeroes
      memset( n->cpBlockData, 0x00, GetBlockSize());

    } else {
      n->ulBlockNo     =  ulPageCnt / (xbUInt32) iBlockFactor;
      ulPageCnt        += (xbUInt32) iBlockFactor;
    }

    mpTag->ulTagSize += (xbUInt32) iBlockFactor;
    if( ulBlock2 > 0 ){
      ePutUInt32( p, ulBlock2 );
    }
  }
  catch( xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxMdx::AllocateIxNode() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
    if( n ) 
      n = FreeNodeChain( n );
  }
  return n;
}
/***********************************************************************/
//! @brief Calculate B-tree pointers.
/*!
  Set binary tree pointer value.  The MDX tags are stored with binary 
  tree positions.  This routine calculates the value in memory.
  \returns void
*/

void xbIxMdx::CalcBtreePointers(){

  xbInt16 iaLeftChild[48];
  xbInt16 iaRightChild[48];
  xbInt16 iaParent[48];

  for( xbInt16 i = 0; i < 48; i++ ){
    iaLeftChild[i] = 0;
    iaRightChild[i] = 0;
    iaParent[i] = 0;
  }

  // anything to do?
  if( iTagUseCnt > 1 ){
    xbString sBaseTag;
    xbString sThisTag;
    xbString sWorkTag;
    xbInt16  iWorkTagNo;
    xbBool   bDone;
    sBaseTag = GetTagName( GetTag( 0 ));

    for( xbInt16 iThisTagNo = 1; iThisTagNo < iTagUseCnt; iThisTagNo++ ){
      iWorkTagNo = 0;
      sWorkTag.Set( sBaseTag );
      sThisTag = GetTagName( GetTag( iThisTagNo ));
      bDone = xbFalse;
      while( !bDone ){
        if( sThisTag < sWorkTag ){
          if( iaLeftChild[iWorkTagNo] == 0 ) {
            iaLeftChild[iWorkTagNo] = iThisTagNo + 1;
            iaParent[iThisTagNo] = iWorkTagNo + 1;
            bDone = xbTrue;
          } else {
             iWorkTagNo = iaLeftChild[iWorkTagNo]-1;
             sWorkTag = GetTagName( GetTag( iWorkTagNo));
          }
        } else {
          if( iaRightChild[iWorkTagNo] == 0 ) {
            iaRightChild[iWorkTagNo] = iThisTagNo + 1;
            iaParent[iThisTagNo] = iWorkTagNo + 1;
            bDone = xbTrue;
          } else {
             iWorkTagNo = iaRightChild[iWorkTagNo]-1;
             sWorkTag = GetTagName( GetTag( iWorkTagNo ));
          }
        }
      }
    }
  }

  xbString s;
  xbMdxTag *mpTag = mdxTagTbl;
  for( xbInt16 i = 0;  i < iTagUseCnt; i++ ){
    mpTag->cLeftChild  = (char ) iaLeftChild[i];
    mpTag->cRightChild = (char ) iaRightChild[i];
    mpTag->cParent     = (char ) iaParent[i];
    mpTag = mpTag->next;
  }
}

/**************************************************************************************************************/
//! @brief Calculate the page number for a given block
/*!
  This routine is called by any function needing to calculate the page number for a given block.
  Page numbers are stored internally in the physical file, and the library reads and writes in
  blocks of one or more pages.

  Assumes valid data input

  \param ulBlockNo  Block Number
  \returns Calculated page number.
*/

inline xbUInt32 xbIxMdx::BlockToPage( xbUInt32 ulBlockNo ){
  return ulBlockNo * (xbUInt32) iBlockFactor;
}
/***********************************************************************/
char xbIxMdx::CalcTagKeyFmt( xbExp &exp ){

  xbExpNode *n = exp.GetTreeHandle();
  if( n->GetChildCnt() == 0 && n->GetNodeType() == XB_EXP_FIELD )
    return 0x01;
  else
    return 0;
}
/***********************************************************************/
//! @brief Check for duplicate key.
/*!
  \param vpTag Tag to check.
  \returns XB_KEY_NOT_UNIQUE<br>XB_NO_ERROR
*/

xbInt16 xbIxMdx::CheckForDupKey( void *vpTag )
{
  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbMdxTag *mpTag = (xbMdxTag *) vpTag;
  mpTag->bFoundSts = xbFalse;
  try{
    if( GetUnique( mpTag )){
      if( mpTag->iKeySts == XB_ADD_KEY || mpTag->iKeySts == XB_UPD_KEY )
        if( KeyExists( mpTag )){
          if( GetUniqueKeyOpt() == XB_EMULATE_DBASE ){
            mpTag->bFoundSts = xbTrue;
            return 0;
          } else {
            return XB_KEY_NOT_UNIQUE;
          }
        }
     }
     return 0;
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxMdx::CheckForDupKey() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}
/***********************************************************************/
//! @brief Check tag integrity.
/*!
  Check a tag for accuracy.

  \param vpTag Tag to create key for.
  \param iOpt Output message destination<br>
              0 = stdout<br>
              1 = Syslog<br>
              2 = Both<br>
  \returns 1 - Unique Index - DBF records exist without corresponding index entries.<br>
           <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbIxMdx::CheckTagIntegrity( void *vpTag, xbInt16 iOpt ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iRc2;
  xbInt16 iRc3;
  xbInt16 iErrorStop   = 0;
  xbInt16 iOutOfSync   = 0;
  xbUInt32 ulIxCnt     = 0;
  xbUInt32 ulThisRecNo = 0;
  xbUInt32 ulPrevRecNo = 0;
  xbBool bDone = false;
  xbString sMsg;
  char cKeyType;
  char *pPrevKeyBuf = NULL;
  xbMdxTag *npTag = (xbMdxTag *) vpTag;
  xbBool bDescending = npTag->cKeyFmt2 & 0x08;

  #ifdef XB_LOCKING_SUPPORT
  xbBool bLocked = xbFalse;
  #endif

  try{

    #ifdef XB_LOCKING_SUPPORT
    if( dbf->GetAutoLock() && !dbf->GetTableLocked() ){
      if(( iRc = dbf->LockTable( XB_LOCK )) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      }
      bLocked = xbTrue;
    }
    #endif

    memset( npTag->cpKeyBuf2, 0x00, (size_t) npTag->iKeyLen );
    cKeyType = GetKeyType( vpTag );

    pPrevKeyBuf = (char *) calloc( 1, (size_t) npTag->iKeyLen );
    iRc = GetFirstKey( vpTag, 0 );

    memcpy( pPrevKeyBuf, GetKeyData( npTag->npCurNode, npTag->npCurNode->iCurKeyNo, npTag->iKeyItemLen ), (size_t) npTag->iKeyLen );

    // for each key in the index, make sure it is trending in the right direction
    while( iRc == XB_NO_ERROR && !bDone ){
      ulIxCnt++;

      iRc = GetNextKey( vpTag, 0 );
      if( iRc == XB_NO_ERROR ){

        // compare this key to prev key
        iRc2 = CompareKey( cKeyType, GetKeyData( npTag->npCurNode, npTag->npCurNode->iCurKeyNo, npTag->iKeyItemLen ), 
                          pPrevKeyBuf, (size_t) npTag->iKeyLen );

        if(( iRc2 < 0 && !bDescending ) || ( iRc2 > 0 && bDescending )){
          sMsg.Sprintf( "Key sequence error at key number [%ld].", ulIxCnt );
          xbase->WriteLogMessage( sMsg, iOpt );
          iErrorStop = 110;
          iRc = XB_INVALID_INDEX;
          throw iRc;
        }

        ulThisRecNo = 0;
        if(( iRc3 = GetDbfPtr( vpTag, npTag->npCurNode->iCurKeyNo, npTag->npCurNode, ulThisRecNo )) != XB_NO_ERROR ){
          iErrorStop = 120;
          throw iRc3;
        }

        if( iRc2 == 0 && (ulThisRecNo <= ulPrevRecNo )){
          sMsg.Sprintf( "Dbf record number sequence error at key number [%ld].", iOpt );
          xbase->WriteLogMessage( sMsg, iOpt );
          iErrorStop = 130;
          iRc = XB_INVALID_INDEX;
          throw iRc;
        }
        // save this key info to prev key
        memcpy( pPrevKeyBuf, GetKeyData( npTag->npCurNode, npTag->npCurNode->iCurKeyNo, npTag->iKeyItemLen ), (size_t) npTag->iKeyLen );
        ulPrevRecNo = ulThisRecNo;
      }
    }

    xbUInt32 ulDbfCnt = 0;
    if(( iRc = dbf->GetRecordCnt( ulDbfCnt )) != XB_NO_ERROR ){
      iErrorStop = 140;
      throw iRc;
    }

    xbUInt32 ulFiltCnt = 0;
    xbBool bFiltered = xbTrue;
    // verify each record in the dbf file has a corresponding index entry
    xbUInt32 j = 0;
    while( j < ulDbfCnt ){

      if(( iRc = dbf->GetRecord( ++j )) != XB_NO_ERROR ){
        iErrorStop = 150;
        throw iRc;
      }
      if( npTag->cHasFilter ){
        if(( iRc = npTag->filter->ProcessExpression( 0 )) != XB_NO_ERROR ){
          iErrorStop = 160;
          throw iRc;
        }
        if(( iRc = npTag->filter->GetBoolResult( bFiltered )) != XB_NO_ERROR ){
          iErrorStop = 170;
          throw iRc;
        }
      }

      if( bFiltered ){

        if( GetUniqueKeyOpt() == XB_EMULATE_DBASE && GetUnique( vpTag )) {
          if(( iRc = KeyExists( vpTag )) != 1 ){
            ulThisRecNo = j;
            iErrorStop = 180;
            throw iRc;
          }
        } else {
          if(( iRc = FindKeyForCurRec( vpTag )) != XB_NO_ERROR ){
            ulThisRecNo = j;
            iErrorStop = 190;
            throw iRc;
          }
        }
        ulFiltCnt++;
      }
    }


    if( ulIxCnt > ulFiltCnt ){
      // should not be more records in index file than valid record count
      sMsg.Sprintf( "CheckTagIntegrity()  Index tag entry count [%ld] greater than record count [%ld]", ulIxCnt, ulFiltCnt );
      iErrorStop = 200;
      throw iRc;
    }


    if((GetUniqueKeyOpt() == XB_EMULATE_DBASE) && (GetUnique( vpTag ))){
      // can't compare counts if using XB_EMULATE_DBASE and it's a unique index
      if( ulIxCnt != ulFiltCnt ){
        // DBase handles unique keys by only indicing the first record for a given index key
        // DBase  might have more than one record in a DBF file with the same key, only the first one is in a unique index
        sMsg.Sprintf( "CheckTagIntegrity()  Warning - Index entry count [%ld] does not match dbf record count [%ld]", ulIxCnt, ulDbfCnt );
        xbase->WriteLogMessage( sMsg, iOpt );
        sMsg.Sprintf( "Unique Index with multiple data records per key. Not an unexpected result in XB_EMULATE_DBASE mode.");
        xbase->WriteLogMessage( sMsg, iOpt );
        iOutOfSync = 1;
      }
    } else {
      if( ulIxCnt != ulFiltCnt && GetUniqueKeyOpt() == XB_HALT_ON_DUPKEY ){
        if( npTag->cHasFilter )
          sMsg.Sprintf( "xbIxMdx::CheckTagIntegrity()  Filtered index entry count [%ld] does not match dbf record count [%ld] for tag [%s]", ulIxCnt, ulFiltCnt, npTag->cTagName );
        else
          sMsg.Sprintf( "xbIxMdx::CheckTagIntegrity()  Index entry count [%ld] does not match dbf record count [%ld] for tag [%s]", ulIxCnt, ulFiltCnt, npTag->cTagName );

        xbase->WriteLogMessage( sMsg, iOpt );
        iErrorStop = 210;
        iRc = XB_INVALID_INDEX;
        throw iRc;
      }
      if( npTag->cHasFilter )
        sMsg.Sprintf( "xbIxMdx::CheckTagIntegrity()  Filtered index entry count [%ld]  matches dbf record count [%ld] for tag [%s]", ulIxCnt, ulFiltCnt, npTag->cTagName );
      else
        sMsg.Sprintf( "xbIxMdx::CheckTagIntegrity()  Index entry count [%ld]  matches dbf record count [%ld] for tag [%s]", ulIxCnt, ulFiltCnt, npTag->cTagName );
      xbase->WriteLogMessage( sMsg, iOpt );
    }

    if( pPrevKeyBuf )
      free( pPrevKeyBuf );
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxMdx::CheckTagIntegrity() Exception Caught. Error Stop = [%d] iRc = [%d] Tag = [%s]", iErrorStop, iRc, npTag->cTagName );
    xbase->WriteLogMessage( sMsg, iOpt );
    xbase->WriteLogMessage( GetErrorMessage( iRc ), iOpt );
    if( pPrevKeyBuf )
      free( pPrevKeyBuf );

    if( iErrorStop == 160 ){
      sMsg.Sprintf( "xbIxMdx::CheckTagIntegrity() Missing index entry for record [%d]", ulThisRecNo );
      xbase->WriteLogMessage( sMsg, iOpt );
    }
  }
  #ifdef XB_LOCKING_SUPPORT
  if( bLocked ){
    dbf->LockTable( XB_UNLOCK );
  }
  #endif

  if( iRc )              // if hard error, return error code
    return iRc;
  else                   // return out of sync warning if it exists
    return iOutOfSync;
}

/***********************************************************************/
xbMdxTag *xbIxMdx::ClearTagTable(){

  // clear the list of tags
  xbMdxTag *tt  = mdxTagTbl;
  xbMdxTag *tt2;
  while( tt ){
    tt2 = tt;
    tt = tt->next;
    tt2->npNodeChain = FreeNodeChain( tt2->npNodeChain );
    tt2->npCurNode = NULL;
    if( tt2->cpKeyBuf )
      free( tt2->cpKeyBuf );
    if( tt2->cpKeyBuf2 )
      free( tt2->cpKeyBuf2 );
    if( tt2->exp )
      delete tt2->exp;
    if( tt2->filter )
      delete tt2->filter;
    if( tt2->sKeyExp )
      delete tt2->sKeyExp;
    if( tt2->sTagName )
      delete tt2->sTagName;
    if( tt2->sFiltExp )
      delete tt2->sFiltExp;
    free( tt2 );
  }
  return NULL;
}
/***********************************************************************/
xbInt16 xbIxMdx::Close(){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;

  try{
    mdxTagTbl = ClearTagTable();
    if(( iRc = xbIx::Close()) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxMdx::Close() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}

/***********************************************************************/
//! @brief Create key.
/*!

  \param vpTag Tag
  \param iOpt 1 = Append, 2 = Update
  \returns XB_KEY_NOT_UNIQUE<br>XB_NO_ERROR

  iKeySts     0 - No Updates
              1 - Add Key        XB_ADD_KEY
              2 - Update Key     XB_UPD_KEY
              3 - Delete Key     XB_DEL_KEY

  bKeyFiltered xbFalse - Key filtered out
               xbTrue  - Key filtered in

  cpKeyBuf  - Key buffer for add
  cpKeyBuf2 - Key buffer for delete

*/

xbInt16 xbIxMdx::CreateKey( void *vpTag, xbInt16 iOpt ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;

  xbBool  bFilter0 = xbFalse;   // filter against RecBuf,  updated record buffer
  xbBool  bFilter1 = xbFalse;   // filter against recBuf2, original record buffer

  try{

    xbMdxTag *npTag = (xbMdxTag *) vpTag;
    npTag->iKeySts = 0;

    // do tag filter logic
    if( npTag->cHasFilter ){
      if(( iRc = npTag->filter->ProcessExpression( 0 )) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      }
      if(( iRc = npTag->filter->GetBoolResult( bFilter0 )) != XB_NO_ERROR ){
        iErrorStop = 110;
        throw iRc;
      }
    } else {
      bFilter0 = xbTrue;
    }

    // if add request and filtered out, we're done
    if( iOpt == 1 && !bFilter0 )
      return XB_NO_ERROR;

    if(( iRc = npTag->exp->ProcessExpression( 0 )) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw iRc;
    }
    if( npTag->exp->GetReturnType() == XB_EXP_CHAR )
      npTag->exp->GetStringResult( npTag->cpKeyBuf, (xbUInt32) npTag->iKeyLen );
    else if( npTag->exp->GetReturnType() == XB_EXP_NUMERIC ){
       xbDouble d;
       npTag->exp->GetNumericResult( d );
       xbBcd bcd( d );
       bcd.ToChar( npTag->cpKeyBuf );
    }
    else if( npTag->exp->GetReturnType() == XB_EXP_DATE ){
       xbDouble d;
       npTag->exp->GetNumericResult( d );
       memcpy( npTag->cpKeyBuf, &d, 8 );
    }

    if( iOpt == 1 )                 // Append
      npTag->iKeySts = XB_ADD_KEY;

    else if( iOpt == 2 ){           // Update
      if( npTag->cHasFilter ){
        if(( iRc = npTag->filter->ProcessExpression( 1 )) != XB_NO_ERROR ){
          iErrorStop = 130;
          throw iRc;
        }
        if(( iRc = npTag->filter->GetBoolResult( bFilter1 )) != XB_NO_ERROR ){
          iErrorStop = 140;
          throw iRc;
        }
      } else {
        bFilter1 = xbTrue;
      }

      if(( iRc = npTag->exp->ProcessExpression( 1 )) != XB_NO_ERROR ){
        iErrorStop = 150;
        throw iRc;
      }

      if( npTag->exp->GetReturnType() == XB_EXP_CHAR ){
        npTag->exp->GetStringResult( npTag->cpKeyBuf2, (xbUInt32) npTag->iKeyLen );

      } else if( npTag->exp->GetReturnType() == XB_EXP_NUMERIC ){
        xbDouble d;
        npTag->exp->GetNumericResult( d );
        xbBcd bcd( d );
        bcd.ToChar( npTag->cpKeyBuf2 );

      } else if( npTag->exp->GetReturnType() == XB_EXP_DATE  ){
        xbDouble d;
        npTag->exp->GetNumericResult( d );
        memcpy( npTag->cpKeyBuf2, &d, 8 );

      }

      if( bFilter1 ){     // original key was indexed
        if( bFilter0 ){   // new key s/b indexed, update it if changed
          if( memcmp( npTag->cpKeyBuf, npTag->cpKeyBuf2, (size_t) npTag->iKeyLen )){
            npTag->iKeySts = XB_UPD_KEY;
          }
        } else {          // original key indexed, new key not indexed, delete it
          npTag->iKeySts = XB_DEL_KEY;
        }
      } else {            // original key not indexed
        if( bFilter0 )
          npTag->iKeySts = XB_ADD_KEY;
      }
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxMdx::CreateKey() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}
/***********************************************************************/
//! @brief Create new tag.
/*!
  This routine creates a new tag.   When complete, sets the cur tag pointer to 
  the newly created tag.


  \param sName Tag Name, including .MDX suffix
  \param sKey Key Expression
  \param sFilter Filter expression.
  \param iDescending 
  \param iUnique xbtrue - Unique.<br>xbFalse - Not unique.
  \param iOverLay xbTrue - Overlay if file already exists.<br>xbFalse - Don't overlay.
  \param vpTag Output from method Pointer to vptag pointer.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/


xbInt16  xbIxMdx::CreateTag( const xbString &sName, const xbString &sKey, const xbString &sFilter, xbInt16 iDescending, xbInt16 iUnique, xbInt16 iOverlay, void **vpTag ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbMdxTag *tte = NULL;

  try{
    // verify room for new tag
    if( !( iTagUseCnt < 47 )){
      iErrorStop = 100;
      iRc = XB_LIMIT_REACHED;
      throw iRc;
    }

    // verify valid tag name
    xbString sWorker = sName;
    sWorker.Trim();
    if( sWorker.Len() > 10 ){
      iErrorStop = 110;
      iRc = XB_INVALID_TAG;
      throw iRc;
    }

    // verify tag not already defined
    if( iTagUseCnt > 0 ){
      if( GetTag( sWorker )){
        iErrorStop = 120;
        iRc = XB_INVALID_TAG;
        throw iRc;
      }
    }

    // allocate a tag structure here
    if(( tte = (xbMdxTag *) calloc( 1, (size_t) sizeof( xbMdxTag ))) == NULL ){
      iErrorStop = 130;
      iRc = XB_NO_MEMORY;
      throw iRc;
    }
    *vpTag = tte;
    tte->sTagName = new xbString( sWorker );

    //set up the key expression
    sWorker = sFilter;
    sWorker.Trim();
    if( sWorker.Len() > 0 ){
      if( sWorker.Len() == 0 || sWorker.Len() > 220 ){
        iRc = XB_INVALID_TAG;
        iErrorStop = 140;
        throw iRc;
      }
      tte->sFiltExp = new xbString( sWorker );
      tte->filter = new xbExp( dbf->GetXbasePtr());
      if(( iRc = tte->filter->ParseExpression( dbf, sWorker )) != XB_NO_ERROR ){
        iErrorStop = 150;
        throw iRc;
      }

      if((tte->filter->GetReturnType()) != 'L' ){
        iRc = XB_INVALID_TAG;
        iErrorStop = 160;
        throw iRc;
      }
      tte->cHasFilter = 0x01;
    }

    //set up the key expression
    sWorker = sKey;
    sWorker.Trim();
    if( sWorker.Len() == 0 || sWorker.Len() > 100 ){
      iRc = XB_INVALID_TAG;
      iErrorStop = 170;
      throw iRc;
    }
    tte->sKeyExp = new xbString( sWorker );
    tte->exp = new xbExp( dbf->GetXbasePtr());
    if(( iRc = tte->exp->ParseExpression( dbf, sWorker )) != XB_NO_ERROR ){
      iErrorStop = 180;
      throw iRc;
    }

    xbDate d;
    d.Sysdate();
    if( iTagUseCnt == 0 ){
      // first tag, new mdx file
      // create the file name
      xbString sIxFileName = dbf->GetFqFileName();
      sIxFileName.Trim();
      xbUInt32 lLen = sIxFileName.Len();
      sIxFileName.PutAt( lLen-2, 'M' );
      sIxFileName.PutAt( lLen-1, 'D' );
      sIxFileName.PutAt( lLen,   'X' );

      // copy the file name to the class variable
      this->SetFileName( sIxFileName );
      if( FileExists() && !iOverlay ){
        iErrorStop = 190;
        iRc = XB_FILE_EXISTS;
        throw iRc;
      }

      // first tag, need to create the file
      if(( iRc = xbFopen( "w+b", dbf->GetShareMode())) != XB_NO_ERROR ){
        iErrorStop = 200;
        throw iRc;
      }
      cVersion         = 2;
      cCreateYY        = (char) d.YearOf() - 1900;
      cCreateMM        = (char) d.MonthOf();
      cCreateDD        = (char) d.DayOf( XB_FMT_MONTH );

      GetFileNamePart( sFileName );
      sFileName.ToUpperCase();

      SetBlockSize( (xbUInt32) dbf->GetCreateMdxBlockSize());
      iBlockFactor     = GetBlockSize() / 512;

      cProdIxFlag      = 1;
      cTagEntryCnt     = 48;
      iTagLen          = 32;
      ulPageCnt        = 4;
      ulFirstFreePage  = 0;
      ulNoOfBlockAvail = 0;
      cNextTag         = 1;
      c1B              = 0x1B;
      cUpdateYY        = cCreateYY;
      cUpdateMM        = cCreateMM;
      cUpdateDD        = cCreateDD;

      if(( iRc = WriteHeadBlock( 0 )) != XB_NO_ERROR ){
        iErrorStop = 210;
        throw iRc;
      }
    }

    // populate the tag table entry structure
    tte->ulTagHdrPageNo = ulPageCnt;
    ulPageCnt += (xbUInt32) iBlockFactor;
    tte->sTagName->strncpy( tte->cTagName, 10 );

    // cKeyFmt is always 0x10;
    // tested 2+ZIPCD  CITY+STATE  or just standalone field - always 0x10
    tte->cKeyFmt = 0x10;        //  = CalcTagKeyFmt( *tte->exp );

    switch( tte->exp->GetReturnType()){
      case XB_EXP_CHAR:
        tte->cKeyType    = 'C';
        tte->iKeyLen     = tte->exp->GetResultLen();
        tte->iSecKeyType = 0;
        break;

      case XB_EXP_NUMERIC:
        tte->cKeyType    = 'N';
        tte->iKeyLen     = 12;
        tte->iSecKeyType = 0;
        break;

      case XB_EXP_DATE:
        tte->cKeyType    = 'D';
        tte->iKeyLen     = 8;
        tte->iSecKeyType = 1;
        break;

      default:
        iErrorStop = 200;
        iRc = XB_INVALID_INDEX;
        throw iRc;
    }

    tte->cpKeyBuf  = (char *) malloc( (size_t) tte->iKeyLen + 1 );
    tte->cpKeyBuf2 = (char *) malloc( (size_t) tte->iKeyLen + 1 );

    // write the new tte entry here
    char tteBuf[21];
    memset( tteBuf, 0x00, 21 );

    ePutUInt32( &tteBuf[0], tte->ulTagHdrPageNo );
    for( xbUInt32 l = 0; l < tte->sTagName->Len() && l < 10; l++ ){
      tteBuf[l+4] = tte->sTagName->GetCharacter(l+1);
    }
    tteBuf[15] = tte->cKeyFmt;
    tteBuf[19] = 0x02;                // appears to always be a 0x02
    tteBuf[20] = tte->cKeyType;

    if(( iRc = xbFseek( (iTagUseCnt * 32) + 544, SEEK_SET )) != XB_NO_ERROR ){
      iErrorStop = 210;
      throw iRc;
    }
    if(( iRc = xbFwrite( tteBuf, 21, 1 )) != XB_NO_ERROR ){
      iErrorStop = 220;
      throw iRc;
    }

    // Begin Tag Header
    tte->ulRootPage   = ulPageCnt;
    tte->ulTagSize    = (xbUInt32) iBlockFactor;
    ulPageCnt        += 2;
    tte->cKeyFmt2     = 0x10;
    if( iDescending )
      tte->cKeyFmt2  += 0x08;
    if( iUnique ){
      tte->cKeyFmt2  += 0x40;
      tte->cUnique    = 0x01;
    }

    tte->cTag11       = 0x1B;    // always 0x1b ?
    tte->cSerialNo    = 0x01;    // version incremented with each tag update
    tte->ulLeftChild  = tte->ulRootPage;
    tte->ulRightChild = tte->ulRootPage;

    tte->cTagYY       = (char) d.YearOf() - 1900;
    tte->cTagMM       = (char) d.MonthOf();
    tte->cTagDD       = (char) d.DayOf( XB_FMT_MONTH );

    tte->cKeyType2    = tte->cKeyType;
    tte->iKeyItemLen  = tte->iKeyLen + 4;
    while(( tte->iKeyItemLen % 4 ) != 0 ) tte->iKeyItemLen++;

    tte->iKeysPerBlock = (xbInt16) (GetBlockSize() - 12) / tte->iKeyItemLen;
    tte->cKeyFmt3 = CalcTagKeyFmt( *tte->exp );

    char *pBuf;
    if(( pBuf = (char *) calloc( 1, (size_t) GetBlockSize())) == NULL ){
      iErrorStop = 230;
      iRc = XB_NO_MEMORY;
      throw iRc;
    }
    char *wPtr;
    wPtr = pBuf;
    ePutUInt32( wPtr, tte->ulRootPage );

    wPtr  += 4;
    ePutUInt32( wPtr, tte->ulTagSize );

    wPtr  += 4;
    *wPtr  = tte->cKeyFmt2;

    wPtr++;
    *wPtr  = tte->cKeyType2;

    wPtr  += 2;
    *wPtr  = tte->cTag11;

    wPtr  += 1;
    ePutInt16( wPtr, tte->iKeyLen );

    wPtr += 2;
    ePutInt16( wPtr, tte->iKeysPerBlock );

    wPtr += 2;
    ePutInt16( wPtr, tte->iSecKeyType );

    wPtr += 2;
    ePutInt16( wPtr, tte->iKeyItemLen );

    wPtr += 2;
    *wPtr = tte->cSerialNo;

    wPtr += 3;
    *wPtr = tte->cUnique;

    wPtr++;
    for( xbUInt32 l = 0; l < tte->sKeyExp->Len(); l++ )
      *wPtr++ = tte->sKeyExp->GetCharacter(l+1);

    wPtr = pBuf;

    tte->cHasKeys = 0x00;
    pBuf[246] = tte->cHasKeys;

    wPtr += 248;
    ePutUInt32( wPtr, tte->ulLeftChild );
    wPtr += 4;
    ePutUInt32( wPtr, tte->ulRightChild );

    pBuf[257] = tte->cTagYY;
    pBuf[258] = tte->cTagMM;
    pBuf[259] = tte->cTagDD;
    pBuf[480] = tte->cKeyFmt3;

    if( sFilter.Len() > 0 ){
      pBuf[245] = tte->cHasFilter;
      wPtr = pBuf;
      wPtr += 762;
      for( xbUInt32 l = 0; l < sFilter.Len(); l++ )
        *wPtr++ = sFilter.GetCharacter(l+1);
    }

    if(( iRc = xbFseek( tte->ulTagHdrPageNo * 512, SEEK_SET )) != XB_NO_ERROR ){
      iErrorStop = 240;
      throw iRc;
    }

    if(( iRc = xbFwrite( pBuf, GetBlockSize(), 1 )) != XB_NO_ERROR ){
      iErrorStop = 250;
      throw iRc;
    }

    memset( pBuf, 0x00, GetBlockSize() );
    if(( iRc = xbFwrite( pBuf, GetBlockSize(), 1 )) != XB_NO_ERROR ){
      iErrorStop = 260;
      throw iRc;
    }

    iTagUseCnt++;
    cNextTag++;

    if(( iRc = WriteHeadBlock( 1 )) != XB_NO_ERROR ){
      iErrorStop = 270;
      throw iRc;
    }

    // update the dbf file if needed - discreet field, has no filter
    // 10/15/22 - dbase 7 does not update this field on index creation
    if( tte->cKeyFmt3 == 0x01 && !tte->cHasFilter ){
      // printf( "cKeyFmt3 = [%x]\n", tte->cKeyFmt3 );
      xbInt16 iFldNo;
      if(( iRc = dbf->GetFieldNo( sKey, iFldNo )) != XB_NO_ERROR ){
        iErrorStop = 280;
        throw iRc;
      }
      xbInt64 lOffset =  31 + ((iFldNo + 1) * 32 );

      if(( iRc = dbf->xbFseek( lOffset, SEEK_SET )) != XB_NO_ERROR ){
        iErrorStop = 290;
        iRc = XB_SEEK_ERROR;
        throw iRc;
      }
      char cBuf[2];
      cBuf[0] = 0x01;
      cBuf[1] = 0x00;

      if(( iRc = dbf->xbFwrite( cBuf, 1, 1 )) != XB_NO_ERROR ){
        iErrorStop = 300;
        throw iRc;
      }
      dbf->UpdateSchemaIxFlag( iFldNo, 0x01 );
    }

    // add the new entry to the end of the list of tags
    if( mdxTagTbl == NULL ){
      mdxTagTbl = tte;
    } else {
      xbMdxTag *tteL = mdxTagTbl;
      while( tteL->next )
        tteL = tteL->next;
      tteL->next = tte;
    }

    /* update the btree pointers */
    CalcBtreePointers();
    char bBuf[3];
    xbMdxTag *tteWork = mdxTagTbl;

    if(( iRc = xbFseek( 560, SEEK_SET )) != XB_NO_ERROR ){
      iErrorStop = 310;
      throw iRc;
    }
    while( tteWork ){
      bBuf[0] = tteWork->cLeftChild;
      bBuf[1] = tteWork->cRightChild;
      bBuf[2] = tteWork->cParent;

      if(( iRc = xbFwrite( bBuf, 3, 1 )) != XB_NO_ERROR ){
        iErrorStop = 320;
        throw iRc;
      }
      if( tteWork->next ){
        if(( iRc = xbFseek( 29, SEEK_CUR )) != XB_NO_ERROR ){
          iErrorStop = 330;
          throw iRc;
        }
      }
      tteWork = tteWork->next;
    }
    free( pBuf );
  }

  catch (xbInt16 iRc ){
    if( tte ){
      if( tte->cpKeyBuf )
        free( tte->cpKeyBuf );
      if( tte->cpKeyBuf2 )
        free( tte->cpKeyBuf2 );
      if( tte->exp )
        delete tte->exp;
      if( tte->filter )
        delete tte->filter;
      if( tte->sKeyExp )
        delete tte->sKeyExp;
      if( tte->sFiltExp )
        delete tte->sFiltExp;
      if( tte->sTagName )
        delete tte->sTagName;
      free( tte );
    }
    xbString sMsg;
    sMsg.Sprintf( "xbIxMdx::CreateTag() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }

  return iRc;
};

/***********************************************************************/
//! @brief Delete a key from a node.
/*!
  This routine deletes a key from a supplied node.
  \param vpTag Tag to delete key on.
  \param npNode Node to delete key on.
  \param iSlotNo Slot number of key to delete.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbIxMdx::DeleteFromNode( void *vpTag, xbIxNode * npNode, xbInt16 iSlotNo )
{
  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbMdxTag * npTag = (xbMdxTag *) vpTag;

  try{
    xbInt32 lKeyCnt = GetKeyCount( npNode );
    xbInt16 iLen = (lKeyCnt - iSlotNo - 1) * npTag->iKeyItemLen;
    xbBool bLeaf = IsLeaf( vpTag, npNode );
    if( !bLeaf )
      iLen += 4;

    char *pTrg = npNode->cpBlockData;
    if( iLen > 0 ){
      pTrg += (8 + (npTag->iKeyItemLen * (iSlotNo)) );  //lTrgPos;
      // std::cout << "TrgSpot = " << (8 + (npTag->iKeyItemLen * (iSlotNo)) ) << "\n";
      char *pSrc = pTrg;
      pSrc += npTag->iKeyItemLen;
      memmove( pTrg, pSrc, (size_t) iLen );
    }

    // zap out the right most key
    pTrg = npNode->cpBlockData;
    if( bLeaf ){
      pTrg += (8 + (npTag->iKeyItemLen * ( lKeyCnt-1 )));

    } else {
      pTrg += (12 + (npTag->iKeyItemLen * ( lKeyCnt-1 )));

    }

    for( xbInt16 i = 0; i < npTag->iKeyItemLen; i++ )
      *pTrg++ = 0x00;

    // set the new number of keys 
    ePutUInt32( npNode->cpBlockData, (xbUInt32) lKeyCnt - 1 );

    // if node empty, add it to the free node chain
    if( lKeyCnt < 2 ){
      if( bReuseEmptyNodes ){
        if( bLeaf && lKeyCnt == 1 ){
          if(( iRc =  HarvestEmptyNode( npTag, npNode, 0 )) != XB_NO_ERROR ){
            iErrorStop = 100;
            throw iRc;
          }
        }
      }
    }

     // write out the block
    if(( iRc = WriteBlock( npNode->ulBlockNo, GetBlockSize(), npNode->cpBlockData )) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }

  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxMdx::DeleteFromNode() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return XB_NO_ERROR;
}

/***********************************************************************/
//! @brief Delete a key.
/*!
  This routine deletes a key. It assumes the key to delete
  is the current key in the node chain.

  \param vpTag Tag to delete key on.

  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbIxMdx::DeleteKey( void *vpTag ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbMdxTag * npTag = (xbMdxTag *) vpTag;

  // save copy of node chain to reset to after delete completed
  xbIxNode *npSaveNodeChain = npTag->npNodeChain;
  npTag->npNodeChain = NULL;
  xbIxNode * npSaveCurNode = npTag->npCurNode;

  try{
    xbString sMsg;

     if(( iRc = xbIxMdx::KeySetPosDel( npTag )) != XB_NO_ERROR ){
       iErrorStop = 100;
       throw iRc;
     }
     // Delete key needs to handle two scenarios
     // 1 - if delete is on the only key of leaf, 
     //     traverse up tree, trim as needed
     // 2 - if last key on node is deleted, and key value is not the same 
     //     as prev key, ascend tree looking for an interior node needing 
     //     updated key value

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
         } else if(( iRc =  HarvestEmptyNode( npTag, npTag->npCurNode, 1 )) != XB_NO_ERROR ){
            iErrorStop = 130;
            throw iRc;
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

        while( pNode && pNode->ulBlockNo != npTag->ulRootPage && pNode->iCurKeyNo == (xbUInt32) GetKeyCount( pNode ) )
            pNode = pNode->npPrev;

        if( pNode ){
          if( pNode->iCurKeyNo < (xbUInt32) GetKeyCount( pNode )){
            char *pTrg  = pNode->cpBlockData;
            pTrg += 12 + (pNode->iCurKeyNo * (xbUInt32) npTag->iKeyItemLen);
            for( xbInt16 i = 0; i < npTag->iKeyLen; i++ )
              *pTrg++ = *pSrc++;
            // write out the block
            if(( iRc = WriteBlock( pNode->ulBlockNo, GetBlockSize(), pNode->cpBlockData )) != XB_NO_ERROR ){
              iErrorStop = 140;
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

    // update the serial number
    if(( iRc = TagSerialNo( 3, npTag )) != XB_NO_ERROR ){
      iErrorStop = 150;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxMdx::DeleteKey() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
    if( npSaveNodeChain ){
      npTag->npNodeChain = npSaveNodeChain;
      npSaveNodeChain = FreeNodeChain( npSaveNodeChain );
      npTag->npCurNode = npSaveCurNode;
    }
  }
  return iRc;
}

/***********************************************************************/
//! @brief Delete a given tag
/*!
  \param vpTag Input tag ptr for tag to be deleted<br>
  \returns <a href="xbretcod_8h.html">Return Codes</a><br>
            1 = Deleted entire MDX file, only had one tag

*/

xbInt16 xbIxMdx::DeleteTag( void *vpTag ){

  xbInt16  iRc        = 0;
  xbInt16  iErrorStop = 0;
  xbMdxTag * mpTag    = (xbMdxTag *) vpTag;
  xbIxNode *n         = NULL;
  xbBool   bLoneTag   = xbFalse;
  xbString sMsg;

  try{

    if( !vpTag ){
      iErrorStop = 100;
      iRc = XB_INVALID_TAG;
      throw iRc;
    }

    sMsg.Sprintf( "Deleting Tag [%s]", mpTag->sTagName->Str());
    xbase->WriteLogMessage( sMsg.Str() );

    char cSaveHasFilter = mpTag->cHasFilter;
    char cSaveKeyFmt3   = mpTag->cKeyFmt3;
    xbString sSaveKey   = mpTag->sKeyExp->Str();

    if( iTagUseCnt == 1 ){
      // std::cout << "xbIxMdx::DeleteTag - one tag found, delete the mdx file\n";

      // close the mdx file
      if(( iRc = xbIxMdx::Close()) != XB_NO_ERROR ){
        iErrorStop = 110;
        throw iRc;
      }

      // delete the file
      xbRemove();

      // init variables - needed?
      // Init();
      // iRc > 0 defines this as the only tag in an MDX file, MDX file deleted.
      //         signals to the calling process to drop the MDX file from the
      //         list of updateable indices.
      bLoneTag = xbTrue;

    } else {

      //    harvest tag nodes
      if(( iRc = HarvestTagNodes( mpTag, xbTrue )) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }

      // remove an entry from tag table
      //   which tag is this?
      xbInt16  iTagNo  = 0;
      xbMdxTag *mp     = mdxTagTbl;
      xbMdxTag *mpPrev = NULL;
      while( mp && mp->ulTagHdrPageNo != mpTag->ulTagHdrPageNo ){
        iTagNo++;
        mpPrev = mp;
        mp = mp->next;
      }

      // remove it from the linked list of tags
      if( !mpPrev ){
        mdxTagTbl = mp->next;
      } else {
        mpPrev->next = mp->next;
      }
      if( mp ){
        if( mp->cpKeyBuf )  free(  mp->cpKeyBuf );
        if( mp->cpKeyBuf2 ) free(  mp->cpKeyBuf2 );
        if( mp->exp )       delete mp->exp;
        if( mp->filter )    delete mp->filter;
        if( mp->sKeyExp )   delete mp->sKeyExp;
        if( mp->sFiltExp )  delete mp->sFiltExp;
        if( mp->sTagName )  delete mp->sTagName;
        free( mp );
      }
      xbInt32 iTarg = iTagNo * 32;
      xbInt32 iSrc  = iTarg  + 32;
      xbInt32 iLen  = (iTagUseCnt - iTagNo) * 32;

      if(( iRc = xbFseek( 544, SEEK_SET )) != XB_NO_ERROR ){
        iErrorStop = 160;
        throw iRc;
      }
      char Buf[1536];  // 47 tags + 1 in case tag #47 is deleted
      memset( Buf, 0x00, 1536 );
      if(( iRc = xbFread( Buf, 1504, 1 )) != XB_NO_ERROR ){
        iErrorStop = 170;
        throw iRc;
      }

      /* rewrite the 47 item tag table */
      char *pTrg = Buf;
      pTrg += iTarg;
      char *pSrc = Buf;
      pSrc += iSrc;
      for( xbInt32 i = 0; i < iLen; i++ )
        *pTrg++ = *pSrc++;

      if(( iRc = xbFseek( 544, SEEK_SET )) != XB_NO_ERROR ){
        iErrorStop = 180;
        throw iRc;
      }
      if(( iRc = xbFwrite( Buf, 1504, 1 )) != XB_NO_ERROR ){
        iErrorStop = 190;
        throw iRc;
      }

      cNextTag--;
      iTagUseCnt--;
      if(( iRc = WriteHeadBlock( 1 )) != XB_NO_ERROR ){
        iErrorStop = 200;
        throw iRc;
      }

      /* update the btree pointers */
      CalcBtreePointers();
      char bBuf[3];
      xbMdxTag *tteWork = mdxTagTbl;

      if(( iRc = xbFseek( 560, SEEK_SET )) != XB_NO_ERROR ){
        iErrorStop = 210;
        throw iRc;
      }
      while( tteWork ){
        bBuf[0] = tteWork->cLeftChild;
        bBuf[1] = tteWork->cRightChild;
        bBuf[2] = tteWork->cParent;

        if(( iRc = xbFwrite( bBuf, 3, 1 )) != XB_NO_ERROR ){
          iErrorStop = 320;
          throw iRc;
        }
        if( tteWork->next ){
          if(( iRc = xbFseek( 29, SEEK_CUR )) != XB_NO_ERROR ){
            iErrorStop = 330;
            throw iRc;
          }
        }
        tteWork = tteWork->next;
      }
    }

    // update the dbf file if needed, if discreet field with no filter
    // printf( "cSaveKeyFmt3 = [%x] cSaveHasFilter=[%x] SaveKey = [%s]\n", cSaveKeyFmt3, cSaveHasFilter, sSaveKey.Str());

    if( cSaveKeyFmt3 == 0x01 && !cSaveHasFilter ){
      xbInt16 iFldNo;
      if(( iRc = dbf->GetFieldNo( sSaveKey, iFldNo )) != XB_NO_ERROR ){
        iErrorStop = 340;
        throw iRc;
      }
      xbInt64 lOffset =  31 + ((iFldNo + 1) * 32 );

      if(( iRc = dbf->xbFseek( lOffset, SEEK_SET )) != XB_NO_ERROR ){
        iErrorStop = 350;
        iRc = XB_SEEK_ERROR;
        throw iRc;
      }
      char cBuf[2];
      cBuf[0] = 0x00;
      cBuf[1] = 0x00;
      if(( iRc = dbf->xbFwrite( cBuf, 1, 1 )) != XB_NO_ERROR ){
        iErrorStop = 360;
        throw iRc;
      }
      dbf->UpdateSchemaIxFlag( iFldNo, 0x00 );
    }
  }
  catch (xbInt16 iRc ){
    sMsg.Sprintf( "xbIxMdx::DeleteTag() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
    if( n )
      free( n );
  }

  if( bLoneTag && !iRc )
    return 1;
  else
    return iRc;
}

/***********************************************************************/
#ifdef XB_DEBUG_SUPPORT

//! @brief Dump a given block for a tag
/*!
  \param iOpt Output message destination<br>
              0 = stdout<br>


              1 = Syslog<br>
              2 = Both<br>
  \param ulBlockNo Block number to dump
  \param mpTag Index tag pointer
  \returns <a href="xbretcod_8h.html">Return Codes</a>

*/

xbInt16 xbIxMdx::DumpBlock( xbInt16 iOpt, xbUInt32 ulBlockNo, xbMdxTag *mpTag ){

  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;
  xbString s, s2;
  xbBool bLeaf;
  char *p;

  try{
    if(( iRc = GetBlock( mpTag, ulBlockNo, 0 )) != XB_NO_ERROR ){
      iErrorStop = 10;
      throw iRc;
    }
    p = cNodeBuf;
    xbInt32 lNoOfKeys = eGetInt32( p );
    p+=4;
    xbUInt32 ulNode2 = eGetUInt32( p );

    if( !mpTag ){
      // if no tag info, print what is available without tag info and exit
      s.Sprintf( "--- BlkNo = %ld  Page = %ld NoOfKeys = %ld Node2 (opt NextFreePage) = %ld", ulBlockNo, BlockToPage( ulBlockNo ), lNoOfKeys, ulNode2 );
      xbase->WriteLogMessage( s, iOpt );
      return XB_NO_ERROR;
    }

    p+=4;
    p+= mpTag->iKeyItemLen * lNoOfKeys;
    if( eGetUInt32( p ) == 0 ){
      bLeaf = xbTrue;
      // std::cout << "No of keys = " << lNoOfKeys << "\n";
      s.Sprintf( "--- Leaf Node      KeyCnt %d\t  Page %d\t Block %d", lNoOfKeys, BlockToPage( ulBlockNo ), ulBlockNo ); 
    } else {
      bLeaf = xbFalse;
      s.Sprintf( "--- Interior Node  KeyCnt %d\t  Page %d\t Block %d", lNoOfKeys, BlockToPage( ulBlockNo ), ulBlockNo );
    }
    if( ulNode2 > 0 )
      s.Sprintf( "%s  Node2 (opt NextFreePage) = %d", s.Str(), ulNode2 );

    xbase->WriteLogMessage( s, iOpt );

    xbInt32 l;
    for( l = 0; l < lNoOfKeys; l++ ){
      p = cNodeBuf + (8 + (l * mpTag->iKeyItemLen ));
      s.Sprintf( "%08ld\t", eGetUInt32( p ));
      p+=4;
      if( mpTag->cKeyType2 == 'C' ){         //CHAR
        for( xbInt32 l = 0; l < (mpTag->iKeyItemLen-4); l++ )
        s += *p++;
        s.Trim();
      } else if( mpTag->cKeyType2 == 'N' ){  // NUMERIC
        xbBcd bcd( p );
        bcd.ToString( s2 );
        s += s2;
      } else if( mpTag->cKeyType2 == 'D' ){  // DATE
        xbInt32 lDate = (xbInt32) eGetDouble( p );
        xbDate d( lDate );
        s.Sprintf( "%s\t%ld\t(%s)", s.Str(), lDate, d.Str());
      } else {
        s.Sprintf( "Unknown key type [%c]", mpTag->cKeyType2 );
      }
      xbase->WriteLogMessage( s, iOpt );
    }
    if( !bLeaf ){
      // interior node has one extra key at the right most position
      p = cNodeBuf + (8 + (l * mpTag->iKeyItemLen ));
      s.Sprintf( "\t%08ld", eGetUInt32( p ));
      xbase->WriteLogMessage( s, iOpt );
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxMdx::DumpBlock() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}

/**************************************************************************************************************/
//! @brief Dump free blocks.
/*!
  Dump free blocks for index debugging purposes.

  \param iOpt Output message destination<br>
              0 = stdout<br>
              1 = Syslog<br>
              2 = Both<br>
  \returns <a href="xbretcod_8h.html">Return Codes</a>

*/
xbInt16 xbIxMdx::DumpFreeBlocks( xbInt16 iOpt ){

  xbInt16  iRc = XB_NO_ERROR;
  xbInt16  iErrorStop = 0;
  xbString s;
  char     *pBuf = NULL;
  char     *pNextPage;
  xbUInt32 ulNextPage;

  try{

    if(( iRc = ReadHeadBlock( 1 )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }

    xbUInt32 ulLastBlock  = PageToBlock( ulPageCnt );

    pBuf = (char *) malloc( (size_t) GetBlockSize());
    if( !pBuf ){
      iErrorStop = 110;
      iRc = XB_NO_MEMORY;
      throw iRc;
    }

    if( ulFirstFreePage > 0 ){
      xbUInt32 ulThisFreePage = ulFirstFreePage;
      xbUInt32 ulNextFreePage = 0;
      xbUInt32 ulCnt = 0;
      xbase->WriteLogMessage( "*** Free Blocks ***", iOpt );
      s.Sprintf( "File Header - FirstFreePage = %ld  Block = %ld", ulFirstFreePage, PageToBlock( ulFirstFreePage ));
      xbase->WriteLogMessage( s, iOpt );
      while( ulThisFreePage > 0 ){
        if(( iRc = ReadBlock( PageToBlock( ulThisFreePage ), GetBlockSize(), pBuf )) != XB_NO_ERROR ){
          iErrorStop = 120;
          throw iRc;
        }
        pNextPage = pBuf;
        pNextPage+=4;
        ulNextFreePage = eGetUInt32( pNextPage );
        s.Sprintf( "Free Page# = %ld\t(Block# = %ld)\tNext Free Page = %ld\t(Block = %ld)", ulThisFreePage, PageToBlock( ulThisFreePage ), ulNextFreePage, PageToBlock( ulNextFreePage ));
        xbase->WriteLogMessage( s, iOpt );
        ulThisFreePage = ulNextFreePage;
        ulCnt++;
      }
      s.Sprintf( "%ld free blocks (%ld pages)", ulCnt, BlockToPage( ulCnt ));
      xbase->WriteLogMessage( s, iOpt );
      xbase->WriteLogMessage( "*** End Of Free Blocks ***", iOpt );
    }

    pNextPage = pBuf;
    pNextPage+=4;

    s = "*** Beginning of Block2 Info ***";
    xbase->WriteLogMessage( s, iOpt );
    s = "ulBlock2 info.  ulBlock2 is either one of a linked list of free nodes, or the id of the original node that this node was split from.";
    xbase->WriteLogMessage( s, iOpt );
    s = "Stored in physical file as pages, processed in blocks";
    xbase->WriteLogMessage( s, iOpt );

    xbUInt32 ulFirstBlock = 3;

    for( xbUInt32 ul = ulFirstBlock; ul < ulLastBlock; ul++ ){
      if(( iRc = ReadBlock( ul, GetBlockSize(), pBuf )) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }
      ulNextPage = eGetUInt32( pNextPage );
      if( ulNextPage > 0 ){
        s.Sprintf( " Block# = %ld\tPage# = %ld\tulBlock2 = %ld\tulBlock2(Page) = %ld", ul, BlockToPage( ul ), PageToBlock( ulNextPage ), ulNextPage );
        xbase->WriteLogMessage( s, iOpt );
      }
    }
    s = "*** End of Block2 Info ***";
    xbase->WriteLogMessage( s, iOpt );

    if( pBuf ) free( pBuf );
  }

  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxMdx::DumpFreeBlocks() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
    if( pBuf ) free( pBuf );
  }
  return iRc;
}

/**************************************************************************************************************/
//! @brief Dump interior and leaf blocks for a given tag.
/*!
  Dump blocks for given tag for index debugging purposes.

  A page is 512 bytes<br>
  A block is one or more pages<br>
  The default mdx block size is 2 pages, or 1024 bytes<br>
  The first four pages or header pages<br>

  \param iOpt Output message destination<br>
              0 = stdout<br>
              1 = Syslog<br>
              2 = Both<br>
  \param vpTag Index tag pointer, defaults to all tags if null.
  \returns <a href="xbretcod_8h.html">Return Codes</a>

*/

xbInt16 xbIxMdx::DumpTagBlocks( xbInt16 iOpt, void * vpTag ){

  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;
  xbInt16 iCurTag = 0;
  xbString s;
  xbInt16 iBlockCtr = 0;

  try{

    xbMdxTag * mpTag;
    if( vpTag == NULL )
      mpTag = (xbMdxTag *) GetTag( iCurTag++ );
    else
      mpTag = (xbMdxTag *) vpTag;

    if( mpTag == NULL ){
      iErrorStop = 100;
      iRc = XB_INVALID_TAG;
      throw iRc;
    }

    xbIxNode *n;
    xbString s;
    xbString s2;
    xbBool bDone = xbFalse;

    xbUInt32 ulBlkNo;
    xbLinkListOrd<xbUInt32> ll;
    xbLinkListNode<xbUInt32> * llN;

    ll.SetDupKeys( xbFalse );

    s.Sprintf( "%s Root Page %ld  (Block %ld)", mpTag->cTagName, mpTag->ulRootPage, PageToBlock( mpTag->ulRootPage ) );
    xbase->WriteLogMessage( s, iOpt );

    // for each tag
    while( !bDone ){

      // clear out any history
      ll.Clear();
      if( mpTag->npNodeChain ){
        mpTag->npNodeChain = FreeNodeChain( mpTag->npNodeChain );
        mpTag->npCurNode   = NULL;
      }

      while( GetNextKey( mpTag, 0 ) == 0 ){
        n = mpTag->npNodeChain;

        while(n){
          ll.InsertKey( n->ulBlockNo, (xbUInt32) n->iCurKeyNo );
          n = n->npNext;
        }
      }
      llN = ll.GetHeadNode();

      while( llN ){

        ulBlkNo = llN->GetKey();
        xbIxMdx::DumpBlock( iOpt, ulBlkNo, mpTag );
        llN = llN->GetNextNode();
        iBlockCtr++;
      }

      if( vpTag || iCurTag >= GetTagCount())
        bDone = xbTrue;
      else
        mpTag = (xbMdxTag *) GetTag( iCurTag++ );
    }

    s.Sprintf( "\nTotal Blocks: %d", iBlockCtr );
    xbase->WriteLogMessage( s, iOpt );

    if( mpTag->npNodeChain ){
        mpTag->npNodeChain = FreeNodeChain( mpTag->npNodeChain );
        mpTag->npCurNode   = NULL;
    }

  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxMdx::DumpTagBlocks() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}
/***********************************************************************/
//! @brief Dump index file header.
/*!
  Dump a index file header for debugging purposes.
  \param iOpt Output message destination<br>
              0 = stdout<br>
              1 = Syslog<br>
              2 = Both<br>
  \param iFmtOpt Output Format<br>
              0, 1 = Header info only<br>
              2 = Tag info<br>
              3 = Header && Tag info<br>
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbIxMdx::DumpHeader( xbInt16 iOpt, xbInt16 iFmtOpt )
{

  xbInt16 iRc = XB_NO_ERROR;
  xbString s;
  if(( iRc = ReadHeadBlock( 1 )) != XB_NO_ERROR )
    return iRc;

  char c, tfv, cDisplayMask = 1;
  cDisplayMask = cDisplayMask << 7;
  if( iFmtOpt != 2 && iFmtOpt != 4 ){
    s = "*** MDX Index Header ***";
    xbase->WriteLogMessage( s, iOpt );
    s = "Version           = ";
    tfv = cVersion;
    for( c = 1; c<= 8; c++ ){
      //std::cout << (tfv & cDisplayMask ? '1' : '0');
      s+= (tfv & cDisplayMask ? '1' : '0');
      tfv <<= 1;
    }
    xbase->WriteLogMessage( s, iOpt );

    s.Sprintf( "Create Date       = %d/%d/%d", (int) cCreateMM, (int) cCreateDD, (int) cCreateYY % 100 );
    xbase->WriteLogMessage( s, iOpt );

    s.Sprintf( "File Name         = %s", sFileName.Str() );
    xbase->WriteLogMessage( s, iOpt );

    s.Sprintf( "Block Factor      = %d", iBlockFactor );
    xbase->WriteLogMessage( s, iOpt );

    s.Sprintf( "Block Size        = %d", GetBlockSize() );
    xbase->WriteLogMessage( s, iOpt );

    s.Sprintf( "Prod Ix Flag      = %d", (xbInt16) cProdIxFlag );
    xbase->WriteLogMessage( s, iOpt );

    s.Sprintf( "Tag Entry Cnt     = %d", (xbInt16) cTagEntryCnt );
    xbase->WriteLogMessage( s, iOpt );

    s.Sprintf( "Tag Len           = %d", iTagLen );
    xbase->WriteLogMessage( s, iOpt );

    s.Sprintf( "Next Tag          = %d", (xbInt16) cNextTag );
    xbase->WriteLogMessage( s, iOpt );

    s.Sprintf( "Tag Use Cnt       = %d", iTagUseCnt );
    xbase->WriteLogMessage( s, iOpt );

    s.Sprintf( "Page Cnt          = %d", ulPageCnt );
    xbase->WriteLogMessage( s, iOpt );

    s.Sprintf( "First Free Page   = %d", ulFirstFreePage );
    xbase->WriteLogMessage( s, iOpt );

    s.Sprintf( "No Of Block Avail = %d\n", ulNoOfBlockAvail );
    xbase->WriteLogMessage( s, iOpt );

    s.Sprintf( "Last update date  = %d/%d/%d", (int) cCreateMM, (int) cCreateDD, (int) cCreateYY % 100 );
    xbase->WriteLogMessage( s, iOpt );

    if( ulFirstFreePage > 0 ){
      xbString s;
      xbUInt32 ulNfp = ulFirstFreePage;   // next free page
      xbInt16 lc = 0;
      while( ulNfp && lc++ < 5 ){
        if( s.Len() > 0 )
          s += ",";
        s.Sprintf( "%s%ld", s.Str(), ulNfp );
        if(( iRc = GetBlock( NULL, (xbUInt32) (ulNfp / (xbUInt32) iBlockFactor), 0 )) != 0 )
          return iRc;
        ulNfp = eGetUInt32( cNodeBuf+4 );
      }
      xbase->WriteLogMessage( s, iOpt );
    }
  }
  if( iFmtOpt > 1 ){
    xbMdxTag *tt = mdxTagTbl;
    xbString s;
    xbInt16 i = 0;

    if( tt ){
      while( tt ){
        i++;
        if(( iRc = LoadTagDetail( 2, tt )) != XB_NO_ERROR )
          return iRc;

        s.Sprintf( "TTE (%d)\tName         HdrPage\tFormat\tLeftChild\tRightChild\tParent\tKeyType", i );
        xbase->WriteLogMessage( s, iOpt );
        s.Sprintf( "TTE (%d)\t%-12s %d\t\t%d\t%d\t\t%d\t\t%d\t%c\n", i, tt->cTagName,  tt->ulTagHdrPageNo, tt->cKeyFmt, tt->cLeftChild, tt->cRightChild, tt->cParent, tt->cKeyType );
        xbase->WriteLogMessage( s, iOpt );

        s.Sprintf( "TTH (%d)\tRoot\tTagSize\tKeyFmt2\tType2\tKeyLen\tKeysPerBlock\tSecType\tKeyItemLen\tSerial#\tHasKeys\tFilter\tDesc\tUnique\tLchild\tRchild\tKeyFmt3\tTagDate", i );
        xbase->WriteLogMessage( s, iOpt );

        s.Sprintf( "TTH (%d)\t%d\t%d\t%d\t%c\t%d\t%d\t\t%d\t%d\t\t%x\t%x\t%d\t%d\t%d\t%d\t%d\t%d\t%d/%d/%d", 
           i, tt->ulRootPage, tt->ulTagSize, tt->cKeyFmt2, tt->cKeyType2, tt->iKeyLen, tt->iKeysPerBlock, tt->iSecKeyType, tt->iKeyItemLen, tt->cSerialNo, tt->cHasKeys, tt->cHasFilter,
           (((tt->cKeyFmt2 & 0x08) > 0) ? 1 : 0),  // descending?
           tt->cUnique, tt->ulLeftChild, tt->ulRightChild, tt->cKeyFmt3, (int) tt->cTagMM,   (int) tt->cTagDD, (int) tt->cTagYY % 100 );

        xbase->WriteLogMessage( s, iOpt );

        s.Sprintf( "Key (%d) %s", i, tt->sKeyExp->Str());
        xbase->WriteLogMessage( s, iOpt );

        if( tt->cHasFilter ){
          s.Sprintf( "Flt (%d) %s", i, tt->sFiltExp->Str());
          xbase->WriteLogMessage( s, iOpt );
        }
        xbase->WriteLogMessage( "", iOpt );
        tt = tt->next;

      }
    }
  }
  return iRc;
}

/***********************************************************************/
xbInt16 xbIxMdx::DumpIxForTag( void *vpTag, xbInt16 iOutputOpt )
{
  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbInt16 iDepth = 0;
  xbUInt32 lKeyCtr = 0;
  xbInt32 iMinDepth = 999999;
  xbInt32 iMaxDepth = 0;

  try{
  /* 
    get first node
    while interior node  
      print the left key
      level++
      go down one on the left
  */

    xbMdxTag * mpTag = (xbMdxTag *) vpTag;
    if( mpTag->npNodeChain ){
      mpTag->npNodeChain = FreeNodeChain( mpTag->npNodeChain );
      mpTag->npCurNode   = NULL;
    }

    // Get the root
    if(( iRc = LoadTagDetail( 2, mpTag )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }
    // lRootBlock is now available
    if(( iRc = GetBlock( vpTag, (mpTag->ulRootPage / (xbUInt32) iBlockFactor), 1 )) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }
    // if no keys on this node, then the index is empty
    xbUInt32 ulKeyPtr = eGetUInt32( mpTag->npCurNode->cpBlockData );
    if( ulKeyPtr == 0 ){
      iErrorStop = 120;
      iRc = XB_EMPTY;
      throw iRc;
    }
    while( !IsLeaf( vpTag, mpTag->npCurNode )){   // go down the chain looking for a leaf node
      PrintKey( vpTag, mpTag->npCurNode , 0, iDepth++,  'I', iOutputOpt );
      if(( iRc = GetKeyPtr( vpTag, 0, mpTag->npCurNode, ulKeyPtr )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }
      if(( iRc = GetBlock( vpTag, (ulKeyPtr / (xbUInt32) iBlockFactor), 1 )) != XB_NO_ERROR ){
        iErrorStop = 140;
        throw iRc;
      }
    }
    if(( iRc = GetKeyPtr( vpTag, 0, mpTag->npCurNode, ulKeyPtr )) != XB_NO_ERROR ){
      iErrorStop = 150;
      throw iRc;
    }
    // loop through the leaf entries of left most leaf
    if( iDepth < iMinDepth ) iMinDepth = iDepth;
    if( iDepth > iMaxDepth ) iMaxDepth = iDepth;
    xbUInt32 ulNoOfKeys = eGetUInt32( mpTag->npCurNode->cpBlockData );
    for( xbUInt32 ul = 0; ul < ulNoOfKeys; ul++ ){
      PrintKey( vpTag, mpTag->npCurNode , ul, iDepth, 'L', iOutputOpt );
      lKeyCtr++;
    }

    // if head node = start node, return
    if( mpTag->npCurNode->ulBlockNo == ( mpTag->ulRootPage / (xbUInt32) iBlockFactor ))
      return XB_NO_ERROR;

    xbBool bEof = false;
    while( !bEof  ){

      // go up the chain, looking for an interior node with more keys on it
      xbIxNode * TempIxNode = mpTag->npCurNode;
      mpTag->npCurNode = mpTag->npCurNode->npPrev;
      mpTag->npCurNode->npNext = NULL;
      TempIxNode->npPrev = NULL;
      TempIxNode = FreeNodeChain( TempIxNode );
      iDepth--;

      while( mpTag->npCurNode->iCurKeyNo >= eGetUInt32( mpTag->npCurNode->cpBlockData ) &&
             mpTag->npCurNode->ulBlockNo != ( mpTag->ulRootPage / (xbUInt32) iBlockFactor) ){
        TempIxNode = mpTag->npCurNode;
        mpTag->npCurNode = mpTag->npCurNode->npPrev; 
        mpTag->npCurNode->npNext = NULL;
        TempIxNode->npPrev = NULL;
        TempIxNode = FreeNodeChain( TempIxNode );
        iDepth--;
      }
      // if head node && right most key, return
      if( mpTag->npCurNode->ulBlockNo == (mpTag->ulRootPage / (xbUInt32) iBlockFactor) &&
          mpTag->npCurNode->iCurKeyNo == eGetUInt32( mpTag->npCurNode->cpBlockData ))
        bEof = true;

      if( !bEof ){
        mpTag->npCurNode->iCurKeyNo++;
        if(( iRc = GetKeyPtr( vpTag,  mpTag->npCurNode->iCurKeyNo, mpTag->npCurNode, ulKeyPtr )) != XB_NO_ERROR ){
          iErrorStop = 160;
          throw iRc;
        }
        PrintKey( vpTag, mpTag->npCurNode , mpTag->npCurNode->iCurKeyNo, iDepth++,  'I', iOutputOpt );

        if(( iRc = GetBlock( vpTag, (ulKeyPtr / (xbUInt32) iBlockFactor), 1 )) != XB_NO_ERROR ){
          iErrorStop = 170;
          throw iRc;
        }

        // traverse down the left side of the tree
        while( !IsLeaf( vpTag, mpTag->npCurNode ))   // go down the chain looking for a leaf node
        {
          PrintKey( vpTag, mpTag->npCurNode , 0, iDepth++,  'I', iOutputOpt );
          if(( iRc = GetKeyPtr( vpTag, 0, mpTag->npCurNode, ulKeyPtr )) != XB_NO_ERROR ){
            iErrorStop = 180;
            throw iRc;
          }
          if(( iRc = GetBlock( vpTag, (ulKeyPtr / (xbUInt32) iBlockFactor), 1 )) != XB_NO_ERROR ){
            iErrorStop = 190;
            throw iRc;
          }
        }
        if( iDepth < iMinDepth ) iMinDepth = iDepth;
        if( iDepth > iMaxDepth ) iMaxDepth = iDepth;
        ulNoOfKeys = eGetUInt32( mpTag->npCurNode->cpBlockData );
        for( xbUInt32 ul = 0; ul < ulNoOfKeys; ul++ ){
          PrintKey( vpTag, mpTag->npCurNode , ul, iDepth, 'L', iOutputOpt );
          lKeyCtr++;
        }
      }
    }
    xbString s;
    s.Sprintf( "Total keys = [%ld] Min Depth = [%d] Max Depth = [%d]", lKeyCtr, iMinDepth, iMaxDepth );
    xbase->WriteLogMessage( s.Str(), 2 );
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxMdx::DumpIxForTag() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}
/**************************************************************************************************************/
void xbIxMdx::DumpIxNodeChain( void *vpTag, xbInt16 iOutputOpt ) const
{
  xbMdxTag * mpTag = (xbMdxTag *) vpTag;
  xbString s( "Dump Node Chain" );
  xbase->WriteLogMessage( s, 2 );

  if( mpTag->npNodeChain ){
    xbIxNode *n = mpTag->npNodeChain;
    xbInt16 iCtr = 0;
    char cLeaf;
    s.Sprintf( "Cnt\tThis     Prev     Next     CurKeyNo  BlockNo  Page      NoOfKeys    Type" );
    xbase->WriteLogMessage( s, iOutputOpt );
    while( n ){
      IsLeaf( vpTag, n ) ? cLeaf = 'L' : cLeaf = 'I';
      s.Sprintf( "%d\t%08x %08x %08x %08d  %08d %08d  %08ld    %c",
                 iCtr++, n, n->npPrev, n->npNext, n->iCurKeyNo,
                 n->ulBlockNo, n->ulBlockNo * (xbUInt32) iBlockFactor,
                 eGetUInt32( n->cpBlockData ), cLeaf );
      xbase->WriteLogMessage( s, 2 );
      n = n->npNext;
    }
  } else {
    s = "Empty Node Chain";
    xbase->WriteLogMessage( s, 2 );
  }
}
#endif

/***********************************************************************************************/
xbInt16 xbIxMdx::FindKey( void *vpTag, xbDouble dKey, xbInt16 iRetrieveSw ){

  xbMdxTag * mpTag = (xbMdxTag *) vpTag;
  if( mpTag->cKeyType2 == 'N' ){   // mdx indices store numeric keys as bcd values
    xbBcd bcd( dKey );
    return xbIx::FindKey( vpTag, bcd, iRetrieveSw );
  } else                           // this would be a julian date inquiry
    return FindKey( vpTag, &dKey, 8, iRetrieveSw );
}

/***********************************************************************************************/
// iRetrieveSw = 1 - position db file to index position
//               0 - do not position dbf file

xbInt16 xbIxMdx::FindKey( void *vpTag, const void * vpKey,
      xbInt32 lSearchKeyLen, xbInt16 iRetrieveSw ){

  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;

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
    xbMdxTag * mpTag = (xbMdxTag *) vpTag;
    char cKeyType = GetKeyType( vpTag );
    xbBool bDescending = mpTag->cKeyFmt2 & 0x08;

    if( mpTag->npNodeChain ){

      // determine if the index has been updated since the last time it was used
      time_t tFileTs;
      if(( iRc = GetFileMtime( tFileTs )) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }
      if( mpTag->tNodeChainTs < tFileTs ){
        mpTag->npNodeChain = FreeNodeChain( mpTag->npNodeChain );
        mpTag->npCurNode   = NULL;

      } else {
        //  pop up the chain looking for appropriate starting point
        xbBool  bDone = false;
        xbIxNode * TempIxNode;
        while( mpTag->npCurNode  && !bDone && 
          (mpTag->npCurNode->ulBlockNo != ( mpTag->ulRootPage / (xbUInt32) iBlockFactor ))){ // not root node
            //if no keys on the node, pop up one
            ulNoOfKeys = eGetUInt32( mpTag->npCurNode->cpBlockData );
            if( ulNoOfKeys == 0 ){
              TempIxNode = mpTag->npCurNode;
              mpTag->npCurNode = mpTag->npCurNode->npPrev;
              TempIxNode = FreeNodeChain( TempIxNode );

            } else {

              iRc = CompareKey( cKeyType, vpKey, GetKeyData( mpTag->npCurNode, 0, mpTag->iKeyItemLen ), (size_t) lSearchKeyLen );
              if( (!bDescending && iRc <= 0) || (bDescending && iRc >= 0 )){
                TempIxNode = mpTag->npCurNode;
                mpTag->npCurNode = mpTag->npCurNode->npPrev;
                TempIxNode = FreeNodeChain( TempIxNode );
              } else {
                // get the number of keys on the block and compare the key to the rightmost key
                xbUInt32 ulKeyCtr = eGetUInt32( mpTag->npCurNode->cpBlockData ) - 1;     // IsLeaf( vpTag, mpTag->npCurNode );
                iRc = CompareKey( cKeyType, vpKey, GetKeyData( mpTag->npCurNode, ulKeyCtr, mpTag->iKeyItemLen), (size_t) lSearchKeyLen );

                if( (!bDescending && iRc > 0) || (bDescending && iRc < 0 )){
                  TempIxNode = mpTag->npCurNode;
                  mpTag->npCurNode = mpTag->npCurNode->npPrev;
                  TempIxNode = FreeNodeChain( TempIxNode );
                } else {
                  bDone = true;
                }
             }
           }
        }
      }
    } 


    // either started empty, or cleared due to file time diff
    if( !mpTag->npNodeChain ){
      // Get the root
      if(( iRc = LoadTagDetail( 2, mpTag )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }
      // lRootBlock is now available
      if(( iRc = GetBlock( vpTag, (mpTag->ulRootPage / (xbUInt32) iBlockFactor), 1 )) != XB_NO_ERROR ){
        iErrorStop = 140;
        throw iRc;
      }
      // if this is a leaf node and no keys on this node, then the index is empty
      ulNoOfKeys = eGetUInt32( mpTag->npCurNode->cpBlockData );
      if( ulNoOfKeys == 0 && IsLeaf( vpTag, mpTag->npCurNode )){
//        iRc = XB_EMPTY;
        iRc = XB_NOT_FOUND;
        return iRc;
      }
    }

    // should be in the appropriate position in the node chain to continue the search from here
    // run down through the interior nodes
    xbInt16 iSearchRc = 0;
    xbUInt32 ulKeyPtr = 0;


    while( vpTag && !IsLeaf( vpTag, mpTag->npCurNode ) ){

      // get the number of keys on the block and compare the key to the rightmost key
      ulNoOfKeys = eGetUInt32( mpTag->npCurNode->cpBlockData );

      if( ulNoOfKeys == 0 ){
        mpTag->npCurNode->iCurKeyNo = 0;

      } else {

        iRc = CompareKey( cKeyType, vpKey, GetKeyData( mpTag->npCurNode, ulNoOfKeys - 1, mpTag->iKeyItemLen), (size_t) lSearchKeyLen );
        if( (!bDescending && iRc > 0) || (bDescending && iRc < 0)){
          mpTag->npCurNode->iCurKeyNo = ulNoOfKeys;
        }
        else
        {
          mpTag->npCurNode->iCurKeyNo = (xbUInt32) BSearchBlock( cKeyType, mpTag->npCurNode, 
              (xbInt32) mpTag->iKeyItemLen, vpKey, (xbInt32) lSearchKeyLen, iSearchRc, bDescending );
        }

      }

      if(( iRc = GetKeyPtr( vpTag, mpTag->npCurNode->iCurKeyNo, mpTag->npCurNode, ulKeyPtr )) != XB_NO_ERROR ){
        iErrorStop = 150;
        throw iRc;
      }

      if(( iRc = GetBlock( vpTag, (ulKeyPtr / (xbUInt32)iBlockFactor), 1 )) != XB_NO_ERROR ){
        iErrorStop = 160;
        throw iRc;
      }
    }

    ulNoOfKeys = eGetUInt32( mpTag->npCurNode->cpBlockData );
    xbInt16 iCompRc = 0;

    if( ulNoOfKeys == 0 ){

      // iCompRc = -1;
      // iRc = XB_EMPTY;
      iRc = XB_NOT_FOUND;
      return iRc;

    } else {

      iRc = BSearchBlock( cKeyType, mpTag->npCurNode, mpTag->iKeyItemLen, vpKey, lSearchKeyLen, iCompRc, bDescending );
      // iCompRc 
      //    0 found
      //  < 0 eof encountered, search key > last key in file
      //  > 0 not found, positioned to next key


      if( iCompRc >= 0 ){
        mpTag->npCurNode->iCurKeyNo = (xbUInt32) iRc;

        if( iRetrieveSw ){

          xbUInt32 ulKey = mpTag->npCurNode->iCurKeyNo;

          if( ulKey >= ulNoOfKeys )   // if position past end of keys, need to go back one and position to last key
            ulKey--;

          // if(( iRc = GetKeyPtr( vpTag, mpTag->npCurNode->iCurKeyNo, mpTag->npCurNode, ulKeyPtr )) != XB_NO_ERROR ){

          if(( iRc = GetKeyPtr( vpTag, ulKey, mpTag->npCurNode, ulKeyPtr )) != XB_NO_ERROR ){
            iErrorStop = 170;
            throw iRc;
          }

          if(( iRc = dbf->GetRecord( ulKeyPtr )) != XB_NO_ERROR ){
            iErrorStop = 180;
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
    sMsg.Sprintf( "xbIxMdx::FindKey() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}

/***********************************************************************/
xbInt16 xbIxMdx::FindKeyForCurRec( void * vpTag )
{

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbInt16 i = 0;

  try{
    if(( iRc = CreateKey( vpTag, 0 )) < XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxMdx::FindKeyForCurRec() Exception Caught. Error Stop = [%d] iRc = [%d] Tag=[%d]", iErrorStop, iRc, i );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return XB_NO_ERROR;
}

/***********************************************************************/
xbInt16 xbIxMdx::GetCurKeyVal( void *vpTag, xbString &s ){

  if( !vpTag )
    return XB_INVALID_TAG;
  xbMdxTag * mpTag = (xbMdxTag *) vpTag;
  s.Set( GetKeyData( mpTag->npCurNode, mpTag->npCurNode->iCurKeyNo, mpTag->iKeyItemLen ), (xbUInt16) mpTag->iKeyLen );
  return XB_NO_ERROR;
}
/***********************************************************************/
xbInt16 xbIxMdx::GetCurKeyVal( void *vpTag, xbDouble &d ){

  if( !vpTag )
    return XB_INVALID_TAG;
  xbMdxTag * mpTag = (xbMdxTag *) vpTag;
  xbBcd bcd( GetKeyData( mpTag->npCurNode, mpTag->npCurNode->iCurKeyNo, mpTag->iKeyItemLen ));
  bcd.ToDouble( d );
  return XB_NO_ERROR;
}
/***********************************************************************/
xbInt16 xbIxMdx::GetCurKeyVal( void *vpTag, xbDate &dt ){

  if( !vpTag )
    return XB_INVALID_TAG;

  xbMdxTag * mpTag = (xbMdxTag *) vpTag;
  xbDouble d = eGetDouble( GetKeyData( mpTag->npCurNode, mpTag->npCurNode->iCurKeyNo, mpTag->iKeyItemLen ));
  dt.JulToDate8( (xbInt32) d );
  return XB_NO_ERROR;
}
/***********************************************************************/
//! @brief Get dbf record number for given key number.
/*!
  \param vpTag Tag to retrieve dbf rec number on.
  \param iKeyNo Key number for retrieval
  \param np Pointer to node
  \param ulDbfPtr- Output dbf record number
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbIxMdx::GetDbfPtr( void *vpTag, xbInt16 iKeyNo, xbIxNode *np, xbUInt32 &ulDbfPtr ) const {

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

    xbMdxTag * mpTag = (xbMdxTag *) vpTag;
    char *p = ( np->cpBlockData);
    p += (8 + (iKeyNo * mpTag->iKeyItemLen));
    ulDbfPtr = eGetUInt32 ( p );

  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxMdx::GetDbfPtr() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}

/***********************************************************************/
/*!
  \param vpTag Tag to retrieve first key on.
  \param iRetrieveSw xbTrue - Retrieve the record on success.<br>
                     xbFalse - Don't retrieve record.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbIxMdx::GetFirstKey( void *vpTag, xbInt16 iRetrieveSw = 0 ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;

// std::cout << "gfk cp0\n";

  // convert the tag pointer to mdx tag pointer
  xbMdxTag * mpTag = (xbMdxTag *) vpTag;


//std::cout << "gfk cp1 " << mpTag << "\n";



  try{
  
  //std::cout << "gfk cp2\n";
  
    // clear out any history
    if( mpTag->npNodeChain ){
      mpTag->npNodeChain = FreeNodeChain( mpTag->npNodeChain );
      mpTag->npCurNode   = NULL;
    }


  //std::cout << "gfk cp3\n";
  

    // Get the root 
    if(( iRc = LoadTagDetail( 2, mpTag )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }


  //std::cout << "gfk cp4\n";


    // lRootPage is available
    if(( iRc = GetBlock( vpTag, (mpTag->ulRootPage / (xbUInt32) iBlockFactor), 1 )) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }

    // if no keys on this node, then the index is empty
    // this is not true


  //std::cout << "gfk cp5\n";


    xbUInt32 ulKeyPtr = eGetUInt32( mpTag->npCurNode->cpBlockData );
    if( ulKeyPtr == 0 && IsLeaf( vpTag, mpTag->npCurNode )){
      iRc = XB_EMPTY;
      return iRc;
    }


  //std::cout << "gfk cp6\n";


    while( !IsLeaf( vpTag, mpTag->npCurNode )){   // go down the chain looking for a leaf node
      if(( iRc = GetKeyPtr( vpTag, 0, mpTag->npCurNode, ulKeyPtr )) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }

      if(( iRc = GetBlock( vpTag, (ulKeyPtr / (xbUInt32) iBlockFactor), 1 )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }
    }


  //std::cout << "gfk cp8\n";


    // retrieve record to data buf 
    if( iRetrieveSw ){
      if(( iRc = GetKeyPtr( vpTag, 0, mpTag->npCurNode, ulKeyPtr )) != XB_NO_ERROR ){
        iErrorStop = 140;
        throw iRc;
      }
      if(( iRc = dbf->GetRecord( ulKeyPtr )) != XB_NO_ERROR ){
        iErrorStop = 150;
        throw iRc;
      }
    }


  //std::cout << "gfk cp9\n";


  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxMdx::GetFirstKey() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}

/***********************************************************************/
//xbBool xbIxMdx::GetIndexUpdated() const {
//  std::cout << "xbIxMdx::GetIndexUpdate() FIX ME \n";
//  return xbFalse;
//}

/***********************************************************************/
//! @brief Get the key expression for the given tag.
/*!
  \param vpTag Tag to retrieve key expression from tag.
  \returns Key expression.
*/

xbString &xbIxMdx::GetKeyExpression( const void * vpTag ) const{
  xbMdxTag *mpTag = (xbMdxTag *) vpTag;
  return *mpTag->sKeyExp;
}

/***********************************************************************/
//! @brief Get the key expression for the given tag.
/*!
  \param vpTag Tag to retrieve filter expression from tag (if it exists).
  \returns Key filter.
*/

xbString &xbIxMdx::GetKeyFilter( const void * vpTag ) const{

  xbMdxTag * mpTag = (xbMdxTag *) vpTag;

  if( mpTag->sFiltExp )
    return *mpTag->sFiltExp;
  else
    return sNullString;

}
/**************************************************************************************************/
xbInt16 xbIxMdx::GetKeyPtr( void *vpTag, xbInt16 iKeyNo, xbIxNode *np, xbUInt32 &ulKeyPtr ) const {

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;

  try{

    xbMdxTag *mdxTag = (xbMdxTag *) vpTag;
    char *p = np->cpBlockData;
    xbUInt32 ulKeyCnt = eGetUInt32( p );
    if( iKeyNo < 0 || iKeyNo > (xbInt16) ulKeyCnt ){
      iErrorStop = 100;
      iRc = XB_INVALID_KEYNO;
      throw iRc;
    }
    p+=8;  // skip past first two four byte numeric fields
    p+= (iKeyNo * mdxTag->iKeyItemLen);
    ulKeyPtr = eGetUInt32( p );
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxMdx::GetKeyPtr() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}
/***********************************************************************/
//! @brief Returns key update status.
/*!
  \param vpTag Tag to check status on.
  \returns XB_UPD_KEY  Key updated.<br>
           XB_DEL_KEY  Key deleted.<br>
           XB_ADD_KEY  Key added.<br>
           0           No key updates

*/
inline xbInt16 xbIxMdx::GetKeySts( void *vpTag ) const{

  xbMdxTag *mpTag = (xbMdxTag *) vpTag;
  return mpTag->iKeySts;
}

/***********************************************************************/
char xbIxMdx::GetKeyType( const void *vpTag ) const {
  xbMdxTag * mpTag = (xbMdxTag *) vpTag;
  return mpTag->cKeyType;
}

/***********************************************************************/
//! @brief Get the last key for the given tag.
/*!
  \param vpTag Tag to retrieve last key on.
  \param iRetrieveSw xbTrue - Retrieve the record on success.<br>
                     xbFalse - Don't retrieve record.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbIxMdx::GetLastKey( void *vpTag, xbInt16 iRetrieveSw ){
  return GetLastKey( 0, vpTag, iRetrieveSw );
}

/***********************************************************************/
//! @brief Get the last key for the given tag and starting node.
/*!
  \param ulBlockNo Starting node
  \param vpTag Tag to retrieve last key on.
  \param iRetrieveSw xbTrue - Retrieve the record if key found.<br>
                     xbFalse - Don't retrieve record.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbIxMdx::GetLastKey( xbUInt32 ulBlockNo, void *vpTag, xbInt16 iRetrieveSw ){

  // if UlNodeNo is zero, start at head node, otherwise start at ulNodeNo
  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;

  // convert the tag pointer to mdx tag pointer
  xbMdxTag * mpTag = (xbMdxTag *) vpTag;

  try{
    xbUInt32 ulNoOfKeys = 0;
    // clear out any history
    if( mpTag->npNodeChain ){
       mpTag->npNodeChain = FreeNodeChain( mpTag->npNodeChain );
       mpTag->npCurNode   = NULL;
    }
    // Get the root 
    if( ulBlockNo == 0 ){
      if(( iRc = LoadTagDetail( 2, mpTag )) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      }
      //if(( iRc = GetBlock( vpTag, (mpTag->ulRootPage / (xbUInt32) iBlockFactor), 1 )) != XB_NO_ERROR ){
      if(( iRc = GetBlock( vpTag, PageToBlock( mpTag->ulRootPage ), 1 )) != XB_NO_ERROR ){
        iErrorStop = 110;
        throw iRc;
      }

    } else {
      if(( iRc = GetBlock( vpTag, ulBlockNo, 1 )) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }
    }

    ulNoOfKeys = eGetUInt32( mpTag->npCurNode->cpBlockData );

    if( ulNoOfKeys == 0 && IsLeaf( vpTag, mpTag->npCurNode )){
      iRc = XB_EMPTY;
      return iRc;
    }

    mpTag->npCurNode->iCurKeyNo = ulNoOfKeys;
    ulNoOfKeys = eGetUInt32( mpTag->npCurNode->cpBlockData );

    xbUInt32 ulKeyPtr = 0;
    while( !IsLeaf( vpTag, mpTag->npCurNode )){   // go down the chain looking for a leaf node
      // std::cout << "Considered an interior node\n";
      if(( iRc = GetKeyPtr( vpTag, ulNoOfKeys, mpTag->npCurNode, ulKeyPtr )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }
      if(( iRc = GetBlock( vpTag, (ulKeyPtr / (xbUInt32) iBlockFactor), 1 )) != XB_NO_ERROR ){
        iErrorStop = 140;
        throw iRc;
      }

      ulNoOfKeys = eGetUInt32( mpTag->npCurNode->cpBlockData );
      mpTag->npCurNode->iCurKeyNo = ulNoOfKeys;
    }
    // leaf node has one fewer keys than the interior node
    mpTag->npCurNode->iCurKeyNo--;
    ulNoOfKeys = eGetUInt32( mpTag->npCurNode->cpBlockData );

    // retrieve record to data buf 
    if( iRetrieveSw ){
      if(( iRc = GetKeyPtr( vpTag, (ulNoOfKeys-1), mpTag->npCurNode, ulKeyPtr )) != XB_NO_ERROR ){
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
    sMsg.Sprintf( "xbIxMdx::GetLastKey() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}

/***********************************************************************/
//! @brief Get the last key for a block number.
/*!
  \param vpTag Tag to retrieve first key on.
  \param ulBlockNo Block number for key retrieval.
  \param cpBuf output buffer for key placement
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbIxMdx::GetLastKeyForBlockNo( void *vpTag, xbUInt32 ulBlockNo, char *cpBuf ){

  // returns the last key for a given block number
  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;

  xbMdxTag * npTag = (xbMdxTag *) vpTag;

  try{
    xbIxNode * npSaveNodeChain = npTag->npNodeChain;
    xbIxNode * npSaveCurNode = npTag->npCurNode;
    npTag->npNodeChain = NULL;

    if(( iRc = GetLastKey( ulBlockNo, npTag, 0 )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }
    memcpy( cpBuf, GetKeyData( npTag->npCurNode, GetKeyCount( npTag->npCurNode ) - 1, npTag->iKeyItemLen ), (size_t) npTag->iKeyLen );

     // free memory
    FreeNodeChain( npTag->npNodeChain );
    npTag->npNodeChain = npSaveNodeChain;
    npTag->npCurNode = npSaveCurNode;

  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxMdx::GetLastKeyForBlockNo() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ) );
  }
  return iRc;
}
/***********************************************************************/
//! @brief Get the next key for the given tag.
/*!
  \param vpTag Tag to retrieve next key on.
  \param iRetrieveSw xbTrue - Retrieve the record on success.<br>
                     xbFalse - Don't retrieve record.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbIxMdx::GetNextKey( void * vpTag, xbInt16 iRetrieveSw ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;

  // convert the tag pointer to mdx tag pointer
  xbMdxTag * mpTag = (xbMdxTag *) vpTag;

//std::cout << "gnk cp1 vpTag = " << vpTag << "  mpTag->npCurNode = " << mpTag->npCurNode << "\n";

  try{
    if( !mpTag->npCurNode ){

// std::cout << "gnk cp2 going to GetFirstKey\n";

      return GetFirstKey( vpTag, iRetrieveSw );
    }



    // more keys on this node?
    xbUInt32 ulKeyPtr;
    if( (eGetUInt32( mpTag->npCurNode->cpBlockData ) - 1) > mpTag->npCurNode->iCurKeyNo ){
      mpTag->npCurNode->iCurKeyNo++;

      if(( iRc = GetKeyPtr( vpTag, mpTag->npCurNode->iCurKeyNo, mpTag->npCurNode, ulKeyPtr )) != XB_NO_ERROR ){
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

    // if at end head node, then at eof
    if( mpTag->npCurNode->ulBlockNo == ( mpTag->ulRootPage / (xbUInt32) iBlockFactor ))
      return XB_EOF;

    // This logic assumes that interior nodes have n+1 left node no's where n is the number of keys in the node
    xbIxNode * TempIxNode = mpTag->npCurNode;
    mpTag->npCurNode = mpTag->npCurNode->npPrev;
    TempIxNode = FreeNodeChain( TempIxNode );

    // While no more right keys && not head node, pop up one node
    while( mpTag->npCurNode->iCurKeyNo >= eGetUInt32( mpTag->npCurNode->cpBlockData ) &&
           mpTag->npCurNode->ulBlockNo != ( mpTag->ulRootPage / (xbUInt32) iBlockFactor) ){

      TempIxNode = mpTag->npCurNode;
      mpTag->npCurNode = mpTag->npCurNode->npPrev; 
      TempIxNode = FreeNodeChain( TempIxNode );
    }

    // if head node && right most key, return eof
    if( mpTag->npCurNode->ulBlockNo == (mpTag->ulRootPage / (xbUInt32) iBlockFactor) &&  
        mpTag->npCurNode->iCurKeyNo == eGetUInt32( mpTag->npCurNode->cpBlockData ))
      return XB_EOF;

    // move one to the right
    mpTag->npCurNode->iCurKeyNo++;
    if(( iRc = GetKeyPtr( vpTag,  mpTag->npCurNode->iCurKeyNo, mpTag->npCurNode, ulKeyPtr )) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw iRc;
    }

    if(( iRc = GetBlock( vpTag, (ulKeyPtr / (xbUInt32) iBlockFactor), 1 )) != XB_NO_ERROR ){
      iErrorStop = 130;
      throw iRc;
    }

    // traverse down the left side of the tree
    while( !IsLeaf( vpTag, mpTag->npCurNode ))   // go down the chain looking for a leaf node
    {
      if(( iRc = GetKeyPtr( vpTag, 0, mpTag->npCurNode, ulKeyPtr )) != XB_NO_ERROR ){
        iErrorStop = 140;
        throw iRc;
      }
      if(( iRc = GetBlock( vpTag, (ulKeyPtr / (xbUInt32) iBlockFactor), 1 )) != XB_NO_ERROR ){
        iErrorStop = 150;
        throw iRc;
      }
    }
    // retrieve record to data buf 
    if( iRetrieveSw ){
      if(( iRc = GetKeyPtr( vpTag, 0, mpTag->npCurNode, ulKeyPtr )) != XB_NO_ERROR ){
        iErrorStop = 160;
        throw iRc;
      }
      if(( iRc = dbf->GetRecord( ulKeyPtr )) != XB_NO_ERROR ){
        iErrorStop = 170;
        throw iRc;
      }
    } else {
      return iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxMdx::GetNextKey() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}

/***********************************************************************/
//! @brief Get the previous key for the given tag.
/*!
  \param vpTag Tag to retrieve previous key on.
  \param iRetrieveSw xbTrue - Retrieve the record on success.<br>
                     xbFalse - Don't retrieve record.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbIxMdx::GetPrevKey( void * vpTag, xbInt16 iRetrieveSw ){

  xbString s;

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;

  // convert the tag pointer to mdx tag pointer
  xbMdxTag * mpTag = (xbMdxTag *) vpTag;

  try{
    if( !mpTag->npCurNode ){
      return GetLastKey( 0, vpTag, iRetrieveSw );
    }

    xbUInt32 ulKeyPtr = 0;
    // more keys on this assumed-leaf node?

    if( mpTag->npCurNode->iCurKeyNo > 0 ){

      mpTag->npCurNode->iCurKeyNo--;

      if(( iRc = GetKeyPtr( vpTag, mpTag->npCurNode->iCurKeyNo, mpTag->npCurNode, ulKeyPtr )) != XB_NO_ERROR ){
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

    //if head node = start node, at eof
    if( mpTag->npCurNode->ulBlockNo == ( mpTag->ulRootPage / (xbUInt32) iBlockFactor ))
      return XB_BOF;

    // This logic assumes that interior nodes have n+1 left node no's where n is the number of keys in the node
    xbIxNode * TempIxNode = mpTag->npCurNode;
    mpTag->npCurNode = mpTag->npCurNode->npPrev;
    TempIxNode = FreeNodeChain( TempIxNode );

    // While no more left keys && not head node, pop up one node
    while( mpTag->npCurNode->iCurKeyNo == 0 &&
           mpTag->npCurNode->ulBlockNo != ( mpTag->ulRootPage / (xbUInt32) iBlockFactor) ){
      TempIxNode = mpTag->npCurNode;
      mpTag->npCurNode = mpTag->npCurNode->npPrev; 
      TempIxNode = FreeNodeChain( TempIxNode );
    }

    //if head node && left most key, return bof
    if( mpTag->npCurNode->ulBlockNo == ( mpTag->ulRootPage / (xbUInt32) iBlockFactor) && mpTag->npCurNode->iCurKeyNo == 0 )
      return XB_BOF;

    // move one to the left
    mpTag->npCurNode->iCurKeyNo--;
    if(( iRc = GetKeyPtr( vpTag,  mpTag->npCurNode->iCurKeyNo, mpTag->npCurNode, ulKeyPtr )) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw iRc;
    }

    if(( iRc = GetBlock( vpTag, (ulKeyPtr / (xbUInt32) iBlockFactor), 1 )) != XB_NO_ERROR ){
      iErrorStop = 130;
      throw iRc;
    }

    // traverse down the right side of the tree
    xbUInt32 ulNoOfKeys;
    while( !IsLeaf( vpTag, mpTag->npCurNode ))   // go down the chain looking for a leaf node
    {
      ulNoOfKeys = eGetUInt32( mpTag->npCurNode->cpBlockData );
      mpTag->npCurNode->iCurKeyNo = ulNoOfKeys;

      if(( iRc = GetKeyPtr( vpTag,  ulNoOfKeys, mpTag->npCurNode, ulKeyPtr )) != XB_NO_ERROR ){
        iErrorStop = 140;
        throw iRc;
      }
      if(( iRc = GetBlock( vpTag, (ulKeyPtr / (xbUInt32) iBlockFactor), 1 )) != XB_NO_ERROR ){
        iErrorStop = 150;
        throw iRc;
      }
    }

    // ulNoOfKeys = eGetUInt32( mpTag->npCurNode->cpBlockData );
    mpTag->npCurNode->iCurKeyNo = eGetUInt32( mpTag->npCurNode->cpBlockData ) - 1;

    // retrieve record to data buf 
    if( iRetrieveSw ){
      if(( iRc = GetKeyPtr( vpTag, mpTag->npCurNode->iCurKeyNo, mpTag->npCurNode, ulKeyPtr )) != XB_NO_ERROR ){
        iErrorStop = 160;
        throw iRc;
      }
      if(( iRc = dbf->GetRecord( ulKeyPtr )) != XB_NO_ERROR ){
        iErrorStop = 170;
        throw iRc;
      }
    } else {
      return iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxMdx::GetPrevKey() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}

/***********************************************************************/
//! @brief GetReuseEmptyNode swuitch setting.
/*!
  \returns xbFalse - Do not reuse empty MDX index nodes (Dbase 6. behavior).
           xbTrue  - Reuse empty MDX index nodes.
*/

xbBool xbIxMdx::GetReuseEmptyNodesSw() const {
  return bReuseEmptyNodes;
}
/***********************************************************************/
xbBool xbIxMdx::GetSortOrder( void *vpTag ) const {

  // return true if descending
  xbMdxTag *mTag = (xbMdxTag *) vpTag;  
  if( mTag->cKeyFmt2 & 0x08 )
    return 0x01;
  else
    return 0x00;
}

/***********************************************************************/
//! @brief Get tag for tag number.
/*!
  \param iTagNo - Zero based, which tag to retrieve.
  \returns Pointer to mdx tag for a given tag number.
*/

void * xbIxMdx::GetTag( xbInt16 iTagNo ) const {

  xbMdxTag *tt = mdxTagTbl;
  xbInt16 i = 0;

  while( i < iTagNo && tt->next ){
    tt = tt->next;
    i++;
  }
  if( i == iTagNo )
    return (void *) tt;
  else
    return NULL;
}

/***********************************************************************/
//! @brief Get tag for tag name.
/*!
  \sTagName - Tag name to retrieve.
  \returns Pointer to mdx tag for a given tag number.
*/

void * xbIxMdx::GetTag( xbString &sTagName ) const {

  xbMdxTag *tt = mdxTagTbl;

  while( sTagName != tt->cTagName && tt->next ){
    tt = tt->next;
  }

  if( sTagName == tt->cTagName )
    return (void *) tt;
  else
    return NULL;
}
/***********************************************************************/
xbInt16 xbIxMdx::GetTagCount() const {
  return iTagUseCnt;
}

/***********************************************************************/
void xbIxMdx::GetTagName( void *vpTag, xbString &sTagName ){

  xbMdxTag *mpTag = (xbMdxTag *) vpTag;
  sTagName = mpTag->sTagName->Str();
}

/***********************************************************************/
//const char *xbIxMdx::GetTagName( void *vpTag, xbInt16 iOpt ) const {

const char *xbIxMdx::GetTagName( void *vpTag, xbInt16 ) const {
  xbMdxTag *mpTag = (xbMdxTag *) vpTag;
  return mpTag->cTagName;
}

/***********************************************************************/
xbString &xbIxMdx::GetTagName( void *vpTag ) const {

  xbMdxTag *mpTag = (xbMdxTag *) vpTag;
  return *mpTag->sTagName;
}

/***********************************************************************/
void *xbIxMdx::GetTagTblPtr() const {
  return (void *) mdxTagTbl;
}


/***********************************************************************/
xbBool xbIxMdx::GetUnique( void *vpTag ) const {
//! @brief Determine unique setting for given tag.
/*!
  \param vpTag Tag to retrieve expression from.
  \returns xbTrue if unique key.
*/
  xbMdxTag *mTag = (xbMdxTag *) vpTag;
  return mTag->cUnique;
}

/***********************************************************************/
//! @brief Harvest Empty Node.
/*!
  Harvest empty MDX node and add it to the chain of link nodes

  \param mpTag Tag to harvest.
  \param iOpt - 0 Don't write the node info to disk, handled elsewhere (don't write it twice)
                1 Write the update into to disk
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbIxMdx::HarvestEmptyNode( xbMdxTag *mpTag, xbIxNode *npNode, xbInt16 iOpt, xbBool bHarvestRoot ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbBool  bRootPage = xbFalse;
  xbInt32 iOffset = 0;

  try{

    if( mpTag->ulRootPage == BlockToPage( npNode->ulBlockNo ) && !bHarvestRoot ){
      bRootPage = xbTrue;
    }
    memset( npNode->cpBlockData, 0x00, GetBlockSize());

    char *pTrg = npNode->cpBlockData;
    if( !bRootPage ){
      pTrg += 4;
      ePutUInt32( pTrg, ulFirstFreePage );
    }

    if( bRootPage ){
      if( mpTag->cHasKeys ){

        // std::cout << "setting has keys\n";

        mpTag->cHasKeys = 0x00;
        if(( iRc = xbFseek( ((mpTag->ulTagHdrPageNo * 512) + 246), SEEK_SET )) != XB_NO_ERROR ){
          iErrorStop = 100;
          throw iRc;
        }
        if(( iRc = xbFwrite( &mpTag->cHasKeys, 1, 1 )) != XB_NO_ERROR ){
          iErrorStop = 110;
          throw iRc;
        }
        // might need to update left sibling and right sibling here.
        // Fields don't seem to be updated consistently by other xbase tools, 
        // for now, not updating
      }

    } else {

      // update header
      // seek to position byte 13
      ulFirstFreePage = BlockToPage( npNode->ulBlockNo );
      if(( iRc = xbFseek( 36, SEEK_SET )) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }
      // write it
      char c4[4];
      ePutUInt32( c4, ulFirstFreePage );
      if(( iRc = xbFwrite( c4, 4, 1 )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }
    }

    if( iOpt == 1 ){
      if(( iRc = xbFseek( (xbInt64) ((npNode->ulBlockNo * GetBlockSize() )) + iOffset, SEEK_SET )) != XB_NO_ERROR ){
        iErrorStop = 140;
        throw iRc;
      }
      // write out the block
      if(( iRc = xbFwrite( npNode->cpBlockData, GetBlockSize(), 1 )) != XB_NO_ERROR ){
        iErrorStop = 150;
        throw iRc;
      }
    }
  }

  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxMdx::HarvestEmptyNodeI() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}

/***********************************************************************/
//! @brief Harvest Tag Nodes.
/*!
  Save all nodes for a given tag into the free node chain.
  Used for reindexing or deleting a given tag.

  \param mpTag Tag for harvesting nodes
  \param bHarvestRoot Set to True when deleting tag
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbIxMdx::HarvestTagNodes( xbMdxTag *mpTag, xbBool bHarvestRoot ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;

  xbUInt32 ulBlkNo;
  xbLinkListOrd<xbUInt32> ll;
  xbLinkListNode<xbUInt32> * llN;
  xbIxNode * n;

  try{

    ll.SetDupKeys( xbFalse );

    // clear out any history
    if( mpTag->npNodeChain ){
      mpTag->npNodeChain = FreeNodeChain( mpTag->npNodeChain );
      mpTag->npCurNode   = NULL;
    }

    while( GetNextKey( mpTag, 0 ) == 0 ){

      // std::cout << "htn15 ";
      n = mpTag->npNodeChain;
      // std::cout << "htn16  ";

      while(n){
        ll.InsertKey( n->ulBlockNo, (xbUInt32) n->iCurKeyNo );
        n = n->npNext;
      }

    }

    if( bHarvestRoot )
      ll.InsertKey( PageToBlock( mpTag->ulTagHdrPageNo ), 0 );

    llN = ll.GetHeadNode();
    if(( n = xbIx::AllocateIxNode( GetBlockSize())) == NULL ){
      iErrorStop = 100;
      iRc = XB_NO_MEMORY;
      throw iRc;
    }

    while( llN ){
      ulBlkNo = llN->GetKey();

      // read in a block for the block number
      if(( iRc = ReadBlock( ulBlkNo, GetBlockSize(), n->cpBlockData )) != XB_NO_ERROR ){
        iErrorStop = 110;
        throw iRc;
      }
      // harvest it
      n->ulBlockNo = ulBlkNo;
      if(( iRc = HarvestEmptyNode( mpTag, n, 1, bHarvestRoot )) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }
      llN = llN->GetNextNode();
    }


// std::cout << "htn4\n";


    n = FreeNodeChain( n );
    mpTag->npNodeChain = FreeNodeChain( mpTag->npNodeChain );
    mpTag->npCurNode   = NULL;

//std::cout << "htn5\n";


  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxMdx::HarvestTagNodes() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }


// std::cout << "htn6\n";


  return iRc;
}

/***********************************************************************/
//! @brief Insert key into interior node.
/*!
  Insert key into non-full interior node.<br>
  Assumes valid inputs

  \param vpTag Tag in play.
  \param npNode Node for insertion.
  \param iSlotNo Slot number to insert key.
  \param ulPtr Page number to insert.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbIxMdx::InsertNodeI( void *vpTag, xbIxNode *npNode, xbInt16 iSlotNo, xbUInt32 ulPtr ){
  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  char *pNewKeyPos;
  char *pTrg;
  char *pLastKey = NULL;
  xbMdxTag * npTag;
  npTag = (xbMdxTag *) vpTag;
  xbInt16 iCopyLen;
  xbInt16 iNewKeyPos = 8;

  try{
    xbInt32 lKeyCnt = GetKeyCount( npNode ); 
    iNewKeyPos += (iSlotNo * npTag->iKeyItemLen);
    char *pSrc = npNode->cpBlockData;

    if( iSlotNo < lKeyCnt )
      iCopyLen = ((lKeyCnt - iSlotNo) * npTag->iKeyItemLen) + 4;
    else
      iCopyLen = 0;

    xbUInt32 ulRqdBufSize = (xbUInt32) ((lKeyCnt + 1) * npTag->iKeyItemLen) + 12;
    if( ulRqdBufSize > npNode->ulBufSize ){
      npNode->ulBufSize += (xbUInt32) npTag->iKeyItemLen;
      npNode->cpBlockData = (char *) realloc( npNode->cpBlockData, (size_t) npNode->ulBufSize );
      if( !npNode->cpBlockData ){
        iErrorStop = 100;
        iRc = XB_NO_MEMORY;
        throw iRc;
      }
    }

    // if not appending to the end of the node, make some room, move things to the right
    pNewKeyPos = npNode->cpBlockData;
    pNewKeyPos += iNewKeyPos;

    if( iSlotNo < lKeyCnt ){
      pTrg = pNewKeyPos;
      pTrg += npTag->iKeyItemLen;
      memmove( pTrg, pNewKeyPos, (size_t) iCopyLen );
    }

    // get the right most key for the left part of the split node
    xbUInt32 ulKeyPtr2;
    if(( iRc = GetKeyPtr( vpTag, npNode->iCurKeyNo, npNode, ulKeyPtr2 )) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }

    // get the new right key value for the freshly split node
    pLastKey = (char *) malloc((size_t) npTag->iKeyLen);
    if(( iRc = GetLastKeyForBlockNo( vpTag, PageToBlock( ulKeyPtr2 ), pLastKey )) != XB_NO_ERROR ){
      iRc = 120;
      throw iRc;
    }

    // write the key values
    pTrg = pNewKeyPos;
    pTrg += 4;
    pSrc = pLastKey;
    for( xbInt16 i = 0; i < npTag->iKeyLen; i++ )
      *pTrg++ = *pSrc++;

    pTrg = pNewKeyPos;
    //pTrg+= npTag->iKeyItemLen - 4;
    pTrg+= npTag->iKeyItemLen;

    ePutUInt32( pTrg, ulPtr);
    ePutInt32( npNode->cpBlockData, ++lKeyCnt );

    // write out the updated block to disk
    if(( iRc = WriteBlock( npNode->ulBlockNo, GetBlockSize(), npNode->cpBlockData )) != XB_NO_ERROR ){
      iErrorStop = 130;
      throw iRc;
    }
    if( pLastKey )
      free( pLastKey );
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxMdx::InsertNodeI() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
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

  \param vpTag Tag in play.
  \param npNode Node for insertion.
  \param iSlotNo Slot number to insert key.
  \param ulPtr Pointer number to insert.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbIxMdx::InsertNodeL( void *vpTag, xbIxNode *npNode, xbInt16 iSlotNo, char * cpKeyBuf, xbUInt32 ulPtr ){

  // format of block data is
  //   4 bytes number of keys on block
  //   4 bytes - next free block or split block num
  //   repeating
  //      4 bytes record number
  //      x bytes key data

  //  Special processing note:  when splitting node, new key is first inserted into full left node before
  //  the node is split.  This routine will make additional room in the buffer for that scenario


  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  char *pNewKeyPos;              // pointer to position in record for new key composite
  char *pTrg;

  xbInt16 iNewKeyPos = 8;        // position in data block where new key begins.
                                 // is the position of the record number, where the fmt is 
                                 // [four byte rec number][actual key data] repeats

  xbMdxTag * npTag = (xbMdxTag *) vpTag;
  xbInt16 iCopyLen;

  try{
    xbInt32 lKeyCnt = GetKeyCount( npNode ); 
    iNewKeyPos += (iSlotNo * npTag->iKeyItemLen);

    // length of number of keys that need to be moved to the right
    if( iSlotNo < lKeyCnt )
      iCopyLen = (lKeyCnt - iSlotNo) * npTag->iKeyItemLen;
    else
      iCopyLen = 0;

    // +8 is to include the first two 4 byte fields in the block
    xbUInt32 ulRqdBufSize = (xbUInt32) ((lKeyCnt + 1) * npTag->iKeyItemLen) + 8;

    if( ulRqdBufSize > npNode->ulBufSize ){

      npNode->ulBufSize += (xbUInt32) npTag->iKeyItemLen;
      npNode->cpBlockData = (char *) realloc( npNode->cpBlockData, (size_t) npNode->ulBufSize );

      // init the newly acquired buffer space
      char *p = npNode->cpBlockData;
      p += (npNode->ulBufSize - (xbUInt32) npTag->iKeyItemLen);
      memset( p, 0x00, (size_t) npTag->iKeyItemLen );
 
      if( !npNode->cpBlockData ){
        iErrorStop = 100;
        iRc = XB_NO_MEMORY;
        throw iRc;
      }
    }

    // if not appending to end, move things right

    pNewKeyPos =  npNode->cpBlockData;
    pNewKeyPos += iNewKeyPos;

    if( iSlotNo < lKeyCnt ) {
      pTrg       =  pNewKeyPos;
      pTrg       += npTag->iKeyItemLen;
      memmove( pTrg, pNewKeyPos, (size_t) iCopyLen );

    }

    // write rec number
    ePutUInt32( pNewKeyPos, ulPtr );

    // write the key value
    pTrg = pNewKeyPos;
    pTrg += 4;
    char * pSrc = cpKeyBuf;
    for( xbInt16 i = 0; i < npTag->iKeyLen; i++ )
      *pTrg++ = *pSrc++;

    // update number of keys on the node
    ePutInt32( npNode->cpBlockData, ++lKeyCnt );

    // determine length of node, zap everything to the right of it
    xbUInt32 iStartPos = 8 + ((xbUInt32) lKeyCnt * (xbUInt32) npTag->iKeyItemLen );
    xbUInt32 iClearLen = npNode->ulBufSize - iStartPos;

    char *p = npNode->cpBlockData;
    p += iStartPos;
    memset( p, 0x00, iClearLen );

    // write out the updated block to disk
    if(( iRc = WriteBlock( npNode->ulBlockNo, GetBlockSize(), npNode->cpBlockData )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxMdx::InsertNodeL() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}
/***********************************************************************/
inline xbBool xbIxMdx::IsLeaf( void *vpTag, xbIxNode *npNode ) const{

  // for performance reasons, does no data checking
  // will result in potentially hard to find segfaults if passing invalid npNode

  xbMdxTag *mTag = (xbMdxTag *) vpTag;
  char *p = npNode->cpBlockData;

  xbInt32 lNoOfKeys = eGetInt32( p );
  // mdx interior nodes have a sibling number to the right of the right most key in the node
  p+=8;
  p+= mTag->iKeyItemLen * lNoOfKeys;

  if( eGetUInt32( p ) == 0 ){
    // std::cout << "leaf node\n";
    return true;
  } else {
    // std::cout << "interior node\n";
    return false;
  }
}

/***********************************************************************/
xbInt16 xbIxMdx::KeyExists( void * vpTag )
{
  // this method assumes the key has already been built 

  xbMdxTag *mpTag = (xbMdxTag *) vpTag;
  xbInt16 iRc = FindKey( vpTag, mpTag->cpKeyBuf, mpTag->iKeyLen, 0 );

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

  \param npTag Pointer to npTag.
  \param ulAddRecNo Record number to add.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbIxMdx::KeySetPosAdd( xbMdxTag *mpTag, xbUInt32 ulAddRecNo ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  try{

     iRc = FindKey( mpTag, mpTag->cpKeyBuf, mpTag->iKeyLen, 0 );
     if( iRc == XB_NOT_FOUND || iRc == XB_EMPTY )
       return XB_NO_ERROR;  // good position
     else if( iRc != XB_NO_ERROR ){
       iErrorStop = 100;
       throw iRc;
     }
     // get here if key was found, get the right most instance of any non unique key for append, find correct spot for update 
     if( GetUnique( mpTag ) == 0 ){

       xbUInt32 ulCurRecNo;
       if(( iRc = GetDbfPtr( mpTag, mpTag->npCurNode->iCurKeyNo, mpTag->npCurNode, ulCurRecNo )) != XB_NO_ERROR ){
         iErrorStop = 110;
         throw iRc;
       }
       xbBool bKeysMatch = xbTrue;

       while( bKeysMatch && ulAddRecNo > ulCurRecNo && iRc == XB_NO_ERROR ){
         if(( iRc = GetNextKey( mpTag, 0 )) == XB_NO_ERROR ){
           if( memcmp( GetKeyData( mpTag->npCurNode, mpTag->npCurNode->iCurKeyNo, mpTag->iKeyItemLen ), mpTag->cpKeyBuf, (size_t) mpTag->iKeyLen ))
             bKeysMatch = xbFalse;
           else{
             if(( iRc = GetDbfPtr( mpTag, mpTag->npCurNode->iCurKeyNo, mpTag->npCurNode, ulCurRecNo )) != XB_NO_ERROR ){
               iErrorStop = 120;
               throw iRc;
             }
           }
         }
       }
     }
     if( iRc == XB_EOF ){   // eof condition
       if(( iRc = GetLastKey( 0, mpTag, 0 )) != XB_NO_ERROR ){
         iErrorStop = 130;
         throw iRc;
       }
       mpTag->npCurNode->iCurKeyNo++;
       return XB_NO_ERROR;
     }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxMdx::KeySetPosAdd() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}

/***********************************************************************/
//! @brief Set position for key delete.
/*!
  This routine is called by the DeleteKey() method and is used to position 
  the node chain to the position the old key should be deleted from the index.

  \param npTag Pointer to npTag.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbIxMdx::KeySetPosDel( xbMdxTag *npTag ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbString sMsg;

  try{

     iRc = FindKey( npTag, npTag->cpKeyBuf2, npTag->iKeyLen, 0 );
     if( iRc == XB_NOT_FOUND || iRc == XB_EMPTY )
       return XB_NO_ERROR;  // good pos ition
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

     if( GetUnique( npTag ) == 1 ){
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
    sMsg.Sprintf( "xbIxMdx::KeySetPosDel() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}

/***********************************************************************/
//! @brief Returns key update status.
/*!
  \param vpTag Tag to check status on.
  \returns xbtrue - Key was updated.<br>xbFalse - Key not updated.
*/
/*
inline xbBool xbIxMdx::KeyFiltered( void *vpTag ) const{

  xbMdxTag *mpTag = (xbMdxTag *) vpTag;
  return mpTag->bKeyFiltered;
}
*/
/***********************************************************************/
xbInt16 xbIxMdx::LoadTagDetail( xbInt16 iOption, xbMdxTag *tte ){

  // option 1 - Load the entire tag detail
  // option 2 - Load the dynamic variables only

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  size_t iReadSize;
  char *buf = NULL;
  char *p;

  try{
    // set the read length based on the option
    if( iOption == 1 )
      iReadSize = 1024;

    else if( iOption == 2 )
      iReadSize = 260;

    else{
      iRc = XB_INVALID_OPTION;
      iErrorStop = 100;
      throw iRc;
    }

    if(( buf = (char *) calloc( 1, (size_t) iReadSize )) == NULL ){
      iErrorStop = 110;
      iRc = XB_NO_MEMORY;
      throw iRc;
    }

    if(( iRc = ReadBlock( tte->ulTagHdrPageNo,(xbUInt32) (GetBlockSize() / (xbUInt16) iBlockFactor),
        iReadSize, buf )) != XB_NO_ERROR ){
      free( buf );
      buf = NULL;
      iErrorStop = 30;
      throw iRc;
    }

    p = buf;
    tte->ulRootPage = eGetUInt32( p );

    if( iOption == 1 ){
      p+=4;
      tte->ulTagSize    = eGetUInt32( p );
      p+=4;
      tte->cKeyFmt2     = *p;
      p++;
      tte->cKeyType2    = *p;
      p+=3;
      tte->iKeyLen      = eGetInt16( p );
      p+=2;
      tte->iKeysPerBlock = *p;
      p+=2;
      tte->iSecKeyType  = eGetInt16( p );
      p+=2;
      tte->iKeyItemLen  = eGetInt16( p );
      p+=2;
      tte->cSerialNo    = *p;
      p+=3;
      tte->cUnique      = *p;
      p++;

      // next line assumes expression is a null terminated string in the block
      tte->sKeyExp = new xbString();
      tte->sKeyExp->Sprintf( "%s", p );

      p+=221;
      tte->cHasFilter   = *p;
      p+=1;
      tte->cHasKeys     = *p;
      p+=2;
      tte->ulLeftChild  = eGetUInt32( p );
      p+=4;
      tte->ulRightChild = eGetUInt32( p );
      p+=5;
      tte->cTagYY       = *p;
      p++;
      tte->cTagMM       = *p;
      p++;
      tte->cTagDD       = *p;
      // p+=223;

      p+=221;
      tte->cKeyFmt3     = *p;

      if( tte->cHasFilter ){
        p+=282;
        tte->sFiltExp = new xbString();
        tte->sFiltExp->Sprintf( "%s", p );
        tte->filter = new xbExp( xbase, dbf );
        if(( iRc = tte->filter->ParseExpression( tte->sFiltExp->Str())) != XB_NO_ERROR ){
          iErrorStop = 120;
          throw iRc;
        }
      }
      tte->npNodeChain = NULL;
      tte->npCurNode   = NULL;
      tte->cpKeyBuf    = (char *) malloc( (size_t) tte->iKeyLen + 1 );
      tte->cpKeyBuf2   = (char *) malloc( (size_t) tte->iKeyLen + 1 );
      tte->exp = new xbExp( xbase, dbf );
      if(( iRc = tte->exp->ParseExpression( tte->sKeyExp->Str() )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }
    } else if( iOption == 2 ){

      // refresh the dynamic tag variables
      p+=4;
      tte->ulTagSize    = eGetUInt32( p );
      p+= 16;
      tte->cSerialNo    = *p;
      p+= 226;
      tte->cHasKeys     = *p;
      p+=2;
      tte->ulLeftChild  = eGetUInt32( p );
      p+=4;
      tte->ulRightChild = eGetUInt32( p );
      p+=5;
      tte->cTagYY       = *p;
      p++;
      tte->cTagMM       = *p;
      p++;
      tte->cTagDD       = *p;
    }

    if( buf )
      free( buf );

  }
  catch (xbInt16 iRc ){
    if( buf )
      free( buf );
    xbString sMsg;
    sMsg.Sprintf( "xbIxMdx::LoadTagDetail() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}

/***********************************************************************/
xbInt16 xbIxMdx::LoadTagTable()
{
  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  char * buf = NULL;

  //std::cout << "xbIxMdx::LoadTagTable() tag use cnt = " << iTagUseCnt << "\n";

  try{

    if( iTagUseCnt > 46 ){
      iErrorStop = 100;
      iRc = XB_INVALID_INDEX;
      throw iRc;
    }

    xbInt16 iBufSize = (xbInt16) iTagLen * iTagUseCnt;

    if(( buf = (char *) malloc( (size_t) iBufSize )) == NULL ){
      iErrorStop = 110;
      iRc = XB_NO_MEMORY;
      throw iRc;
    }

    if(( iRc = xbFseek( 544, SEEK_SET )) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw iRc;
    }

    if(( iRc = xbFread( buf, (size_t) iBufSize, 1 )) != XB_NO_ERROR ){
      iErrorStop = 130;
      throw iRc;
    }

    xbInt16 iPos;
    char    *p;
    xbMdxTag *tte;
    xbMdxTag *ttel = NULL;

    for( xbInt16 i = 0; i < iTagUseCnt; i++ ){
      iPos = i * iTagLen;
      p = buf + iPos;

      if(( tte = (xbMdxTag *) calloc( 1, (size_t) sizeof( xbMdxTag ))) == NULL ){
        iErrorStop = 140;
        iRc = XB_NO_MEMORY;
        throw iRc;
      }

      // set the current tag to the first tag in the table
      if( !vpCurTag )
        xbIx::SetCurTag( (void *) tte );

      if( mdxTagTbl )
        ttel->next = tte;
      else
        mdxTagTbl = tte;

      ttel = tte;
      tte->next = NULL;
      tte->ulTagHdrPageNo = eGetUInt32( p );

      p += 4;
      for( xbUInt32 i = 0; i < 11; i ++ )
        tte->cTagName[i] = *p++;

      tte->cTagName[11] = 0x00;
      tte->cKeyFmt        = *p++;
      tte->cLeftChild     = *p++;
      tte->cRightChild    = *p++;
      tte->cParent        = *p++;
      tte->c2             = *p++;
      tte->cKeyType       = *p;
      tte->sTagName = new xbString();
      tte->sTagName->Set( tte->cTagName );
      tte->sTagName->Trim();

      if(( iRc = LoadTagDetail( 1, tte )) != XB_NO_ERROR ){
        iErrorStop = 150;
        throw iRc;
      }
    }
    if( buf )
      free( buf );
  }
  catch (xbInt16 iRc ){
    if( buf )
      free( buf );
    xbString sMsg;
    sMsg.Sprintf( "xbIxMdx::LoadTagTable() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    if( iErrorStop == 100 ){
      sMsg.Sprintf( "xbIxMdx::LoadTagTable() Invalid Tag Count: %d", iTagUseCnt );
      xbase->WriteLogMessage( sMsg.Str());
    }
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}

/**************************************************************************************************************/
//! @brief Calculate the block number for a given page.
/*!
  This routine is called by any function needing to calculate the block number for a given page.
  Page numbers are stored internally in the physical file, and the library reads and writes in
  blocks of one or more pages.

  Assumes valid data input

  \param ulPageNo Page Number
  \returns Calculated block number.
*/

inline xbUInt32 xbIxMdx::PageToBlock( xbUInt32 ulPageNo ){
  return ulPageNo / (xbUInt32) iBlockFactor;
}



/**************************************************************************************************************/
#ifdef XB_DEBUG_SUPPORT
xbInt16 xbIxMdx::PrintKey( void *vpTag, xbIxNode *npNode, xbInt16 iKeyNo, xbInt16 iDepth, char cType, xbInt16 iOutputOpt ){

  xbString sPre;
  sPre.Sprintf( "%c ", cType );
  for( xbInt16 i = 0; i < iDepth; i++ )
    sPre += "|";

  xbString sPost;
  sPost.Sprintf( "\tThisBlock=[%ld] KeyNo=[%d] Depth=[%d]", npNode->ulBlockNo, iKeyNo, iDepth );

  xbMdxTag * mpTag = (xbMdxTag *) vpTag;
  char *p = npNode->cpBlockData + (8 + (iKeyNo * mpTag->iKeyItemLen ));

  xbString sKeyPtr;
  xbUInt32 ulNoOfKeys = 0;
  if( cType == 'I' ) {       // interior
    sKeyPtr.Sprintf( " ptr=[%ld]", eGetUInt32( p ));
    ulNoOfKeys = eGetUInt32( mpTag->npCurNode->cpBlockData );
  }
  else if( cType == 'L' )   // leaf
    sKeyPtr.Sprintf( " rec=[%ld]", eGetUInt32( p ));
  p += 4;

  xbString s;
  if(( cType == 'I' && iKeyNo < (xbInt16) ulNoOfKeys) || cType == 'L' ){
    if( mpTag->cKeyType2 == 'C' ){         //CHAR
      for( xbInt32 l = 0; l < (mpTag->iKeyItemLen-4); l++ )
        s += *p++;

    } else if( mpTag->cKeyType2 == 'N' ){  // NUMERIC
      xbBcd bcd( p );
      xbString s2;
      bcd.ToString( s2 );
      s += s2;

    } else if( mpTag->cKeyType2 == 'D' ){  // DATE
      xbInt32 lDate = (xbInt32) eGetDouble( p );
      xbDate d( lDate );
      //xbString s2;
      //d.JulToDate8( lDate, s2 );
      s.Sprintf( "%s%s", s.Str(), d.Str());
    }
  } else {
    s = "Rightmost InteriorNode Pointer";
  }

  xbString sOut( sPre );
  sOut += s;
  sOut += sPost;
  sOut += sKeyPtr;

  xbase->WriteLogMessage( sOut, iOutputOpt );
  return XB_NO_ERROR;
}
#endif

/***********************************************************************/
//! @brief ReadHeadBlock.
/*!
  Read values off head block in MDX file
  \param iOpt 0 - Read entire block, initialize as needed.<br>
              1 - Read in only dynamic section of block<br>
  \returns <a href="xbretcod_8h.html">Return Codes</a>

*/

xbInt16 xbIxMdx::ReadHeadBlock( xbInt16 iOpt )
{
  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;

  try{
    if( !FileIsOpen()){
      iRc = XB_NOT_OPEN;
      iErrorStop = 100;
      throw iRc;
    }
    char sBuf[48];
    memset( sBuf, 0x00, 48 );

    if( iOpt == 0 ){
      if(( iRc = xbFseek( 0, SEEK_SET )) != XB_NO_ERROR ){
        iErrorStop = 110;
        throw iRc;
      }

      if(( iRc = xbFread( sBuf, 47, 1 )) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }
    } else {

      if(( iRc = xbFseek( 28, SEEK_SET )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }

      if(( iRc = xbFread( sBuf, 19, 1 )) != XB_NO_ERROR ){
        iErrorStop = 140;
        throw iRc;
      }
    }

    char *p         = sBuf;
    if( iOpt == 0 ){
      cVersion         = *p++;
      cCreateYY        = *p++;
      cCreateMM        = *p++;
      cCreateDD        = *p++;
      sFileName.Assign( p, 1, 16 );
      p+=16;
      iBlockFactor     = eGetInt16( p );
      p+=2;
      SetBlockSize( (xbUInt32) eGetInt16( p ));
      p+=2;
      cProdIxFlag      = *p++;
      cTagEntryCnt     = *p++;
      iTagLen          = *p;
      p+=2;

      iTagUseCnt       = eGetInt16( p );
      //lTagUseCnt       = eGetInt32( p );
      //p+=4;
      p+=2;
      cNextTag         = *p++;
      c1B              = *p++;

      ulPageCnt        = eGetUInt32( p );
      p+=4;
      ulFirstFreePage  = eGetUInt32( p );
      p+=4;
      ulNoOfBlockAvail = eGetUInt32( p );
      p+=4;
      cUpdateYY        = *p++;
      cUpdateMM        = *p++;
      cUpdateDD        = *p;

      if( cNodeBuf )
        free( cNodeBuf );

      if(( cNodeBuf = (char *) malloc( (size_t) GetBlockSize())) == NULL ){
        iErrorStop = 150;
        throw XB_NO_MEMORY;
      }

      if(( iRc = xbIxMdx::LoadTagTable()) != XB_NO_ERROR ){
        iErrorStop = 160;
        throw iRc;
      }

    } else {
      iTagUseCnt       = eGetInt16( p );
      p+=4;
      ulPageCnt        = eGetUInt32( p );
      p+=4;
      ulFirstFreePage  = eGetUInt32( p );
      p+=4;
      ulNoOfBlockAvail = eGetUInt32( p );
      p+=4;
      cUpdateYY        = *p++;
      cUpdateMM        = *p++;
      cUpdateDD        = *p;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxMdx::ReadHeadBlock() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
    if( cNodeBuf )
      free( cNodeBuf );
  }
  return iRc;
}

/***********************************************************************/
//! @brief Reindex
/*!
  Reindex specifed tag or all tags 
  \param **vpTag &tag - Tag to reindex.<br>
                 NULL - Reindex all tags<br>
  \returns <a href="xbretcod_8h.html">Return Codes</a>

  If this method fails, the index is left in an undefined state

*/

/*
xbInt16 xbIxMdx::Reindex( void **vpTag ){

  xbInt16 iRc        = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbMdxTag * mpTag;
//  #ifdef XB_LOCKING_SUPPORT
//  xbBool  bLocked = xbFalse;
//  #endif

  if( vpTag )
    mpTag = (xbMdxTag *) *vpTag;
  else
    mpTag = NULL;

  struct tagInfo{
    xbBool   bUnique;
    xbBool   bDesc;
    char     sTagName[11];
    xbString *sKeyExp;
    xbString *sFiltExp;
    tagInfo  *next;
  };
  tagInfo *ti = NULL;

  try{


    #ifdef XB_LOCKING_SUPPORT
    if( dbf->GetAutoLock() && !dbf->GetTableLocked() ){
      if(( iRc = dbf->LockTable( XB_LOCK )) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      }
      bLocked = xbTrue;
    }
    #endif

    if( mpTag == NULL ){
      // do all tags
      xbMdxTag *tt = mdxTagTbl;
      tagInfo *pHead = NULL;
      tagInfo *pEnd  = NULL;

      if( tt ){
        while( tt ){
          ti = (tagInfo *) calloc( 1, sizeof( tagInfo ));
          ti->bUnique = tt->cUnique ? 1 : 0;
          ti->bDesc   = (((tt->cKeyFmt2 & 0x08) > 0) ? 1 : 0);
          memcpy( ti->sTagName, tt->cTagName, 11 );
          ti->sKeyExp = new xbString( tt->sKeyExp->Str());
          if( tt->cHasFilter )
            ti->sFiltExp = new xbString( tt->sFiltExp->Str());
          else
            ti->sFiltExp = new xbString( "" );
          if( !pHead )
            pHead = ti;
          else
            pEnd->next = ti;
          pEnd = ti;
          tt = tt->next;
        }
      }

      // get the file name and save it
      xbString sMdxFileName = GetFqFileName();

      // close the mdx file
      if(( iRc = xbIxMdx::Close()) != XB_NO_ERROR ){
        iErrorStop = 110;
        throw iRc;
      }

      // delete the file
      xbRemove();

      // init variables
      Init();


      tagInfo *p = pHead;
      tagInfo *pDel;

      // create new file & add the tags
      while( p ){

        if(( iRc = CreateTag( p->sTagName, p->sKeyExp->Str(), p->sFiltExp->Str(), p->bDesc, p->bUnique, xbTrue, vpTag )) != XB_NO_ERROR ){
          iErrorStop = 120;
          throw iRc;
        }
        delete p->sKeyExp;
        delete p->sFiltExp;
        pDel = p;
        p = p->next;
        free( pDel );
      }
    } else {

      if(( iRc = HarvestTagNodes( mpTag )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }
    }

    xbInt16 iErrorCnt = 0;
    if( vpTag ){
      if(( iRc = ReindexTag( vpTag )) != XB_NO_ERROR ){
        iErrorStop = 140;
        throw iRc;
      }

    } else {

      void *vpTag;
      void **vppTag;

      xbInt16 iCurTag = 0;
      while( iCurTag < GetTagCount()){
        vpTag = GetTag( iCurTag );

        if( !vpTag ){
          iRc = XB_INVALID_INDEX;
          iErrorStop = 150;
          throw iRc;
        }
        vppTag = &vpTag;
        iRc = ReindexTag( vppTag );

        // temp code to exit on error
        if( iRc != XB_NO_ERROR ){
          iErrorStop = 999;
          throw iRc;

        }

        if( iRc == XB_NO_ERROR )
          iCurTag++;
        else
          iErrorCnt++;

      }
    }



  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxMdx::Reindex() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }


  #ifdef XB_LOCKING_SUPPORT
  if( bLocked ){
    dbf->LockTable( XB_UNLOCK );
  }
  #endif


  return iRc;
}
*/
/***********************************************************************/
xbInt16 xbIxMdx::ReindexTag( void **vpTag ){

  xbInt16  iRc        = XB_NO_ERROR;
  xbInt16  iErrorStop = 0;
  xbMdxTag * mpTag    = NULL;
  xbString sMsg;

  try{

    if( !vpTag )
      return XB_INVALID_TAG;

    mpTag  = (xbMdxTag *) *vpTag;

    sMsg.Sprintf( "Reindex Tag for Table [%s] Tag [%s]", dbf->GetTblAlias().Str(), mpTag->cTagName );
    xbase->WriteLogMessage( sMsg.Str());

    xbUInt32 ulRecCnt = 0;
    if(( iRc = dbf->GetRecordCnt( ulRecCnt )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }

//    if( ulRecCnt == 0 )
//      return XB_NO_ERROR;


    if(( iRc = HarvestTagNodes( mpTag )) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }


    xbBool bAddKey;
    for( xbUInt32 ulRec = 1; ulRec <= ulRecCnt; ulRec++ ){
      if(( iRc = dbf->GetRecord( ulRec )) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }

      // do the tag things
      // CreateKey
      if(( iRc = CreateKey( mpTag, 1 )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }

      if( mpTag->iKeySts == XB_ADD_KEY ){
        bAddKey = xbTrue;
        if( mpTag->cUnique ){

          iRc = CheckForDupKey( mpTag );
          if( GetUniqueKeyOpt() == XB_EMULATE_DBASE && mpTag->bFoundSts == xbTrue ){
            bAddKey = xbFalse;
          } else if( GetUniqueKeyOpt() == XB_HALT_ON_DUPKEY && iRc == XB_KEY_NOT_UNIQUE ){
            iErrorStop = 140;
            throw iRc;
          }
        }
        if( bAddKey ){
          if(( iRc = AddKey( mpTag, ulRec )) != XB_NO_ERROR ){
            iErrorStop = 150;
            throw iRc;
          }
        }
      }
    }
  }
  catch (xbInt16 iRc ){
    // xbString sMsg;
    sMsg.Sprintf( "xbIxMdx::ReindexTag() Exception Caught. Error Stop = [%d] iRc = [%d] Tag: [%s]  Key Expression: [%s] Rec No: [%ld]", 
      iErrorStop, iRc, mpTag->cTagName, mpTag->sKeyExp->Str(), dbf->GetCurRecNo());
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}
/***********************************************************************/
xbInt16 xbIxMdx::SetCurTag( xbString &sTagName ) {

  xbMdxTag *tt = (xbMdxTag *) GetTag( sTagName );
  if( tt ){
      xbIx::SetCurTag((void *) tt );
      return XB_NO_ERROR;
  } else
    return XB_INVALID_TAG;
}

/***********************************************************************/
xbInt16 xbIxMdx::SetCurTag( xbInt16 iTagNo ) {

  xbMdxTag *tt = (xbMdxTag *) GetTag( iTagNo );
  if( tt ){
      xbIx::SetCurTag((void *) tt );
      return XB_NO_ERROR;
  } else
    return XB_INVALID_TAG;
}

/***********************************************************************/
//! @brief SetReuseEmptyNode switch setting.
/*!
  \param  bEmptyNodeSw  xbFalse - Do not reuse empty MDX index nodes (Dbase 6. behavior).
                        xbTrue  - Reuse empty MDX index nodes.
*/

void xbIxMdx::SetReuseEmptyNodesSw( xbBool bEmptyNodesSw ) {
  bReuseEmptyNodes = bEmptyNodesSw;
}

/***********************************************************************/
//! @brief Split an interior node
/*!

  This routine splits an interior node into two nodes, divided by dSplitFactor.<br>
  This behaves differently than V7 Dbase. V7 does not balance the nodes.<br>
  For V7, if adding a key to the end of a node, it will create a right node 
  with only one key, and the left node is still full.<br><br>

  Possible performance improvement options.<br>
  Two modes when splitting:<br>
  a)  Split nodes in the middle - good for random access applications<br>
  b)  Split off right node with only one key - good for applications with
      expectation of ascending keys added moving forward.<br>

  This routine first inserts the key into the left node in the appropriate location
  then splits the node based on the split factor setting.

  \param vpTag Tag in play.
  \param npLeft Left node to split.
  \param npRight Right node to split.
  \param iSlotNo Slot number for split.
  \param ulPtr Pointer number to insert.
  \returns <a href="xbretcod_8h.html">Return Codes</a>  
*/

xbInt16 xbIxMdx::SplitNodeI( void *vpTag, xbIxNode * npLeft, xbIxNode *npRight, xbInt16 iSlotNo, xbUInt32 ulPtr ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbMdxTag * npTag = (xbMdxTag *) vpTag;
  xbDouble dSplitFactor = .5;                           // split the nodes 50/50
  xbInt16 iLen;
  char    *pSrc;
  char    *pTrg;

  try{
    xbInt32 lKeyCnt = GetKeyCount( npLeft );
    xbInt32 lNewLeftKeyCnt = (xbInt32) ((lKeyCnt + 1) * dSplitFactor);
    xbInt32 lNewRightKeyCnt = lKeyCnt - lNewLeftKeyCnt;
    if(( iRc = InsertNodeI( vpTag, npLeft, iSlotNo, ulPtr )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }

    // move the right half of the left node to the right node
    pSrc = npLeft->cpBlockData;
    pSrc += 8 + ((lNewLeftKeyCnt+1) * npTag->iKeyItemLen);
    pTrg = npRight->cpBlockData;
    pTrg += 8;
    iLen = (lNewRightKeyCnt * npTag->iKeyItemLen) + 4;
    memmove( pTrg, pSrc, (size_t) iLen );

    // eliminate chattle on the right
    iLen = 12 + (lNewLeftKeyCnt * npTag->iKeyItemLen);
    pSrc = npLeft->cpBlockData;
    pSrc += iLen;
    memset( pSrc, 0x00, npLeft->ulBufSize - (xbUInt32) iLen );

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
    sMsg.Sprintf( "xbIxMdx::SplitNodeI() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}

/***********************************************************************/
//! @brief Split a leaf node.
/*!
  This routine splits an index leaf into two nodes, divided by dSplitFactor.<br>
  
  Possible performance improvement options.<br>
  Two modes when splitting:<br>
  a)  Split nodes in the middle - good for random access applications<br>
  b)  Split off right node with only one key - good for applications with 
      expectation of ascending keys added moving forward.<br>

  This routine first inserts the key into the left node in the appropriate location
  then splits the node based on the split factor setting.

  \param vpTag Tag in play.
  \param npLeft Left node to split.
  \param npRight Right node to split.
  \param iSlotNo Slot number for split.
  \param ulPtr Pointer number to insert.
  \returns <a href="xbretcod_8h.html">Return Codes</a>  
*/

xbInt16 xbIxMdx::SplitNodeL( void *vpTag, xbIxNode * npLeft, xbIxNode *npRight,
    xbInt16 iSlotNo, char * cpKeyBuf, xbUInt32 ulPtr ){

  xbInt16  iRc = XB_NO_ERROR;
  xbInt16  iErrorStop = 0;
  xbDouble dSplitFactor = .5;             // can adjust performance with this number
  xbMdxTag *mpTag = (xbMdxTag *) vpTag;
  xbString sMsg;

  xbInt16 iLen;
  char *pSrc;
  char *pTrg;

  // std::cout << "In xbIxMdx::SplitNodeL()\n";
  try{
    xbInt32 lKeyCnt = GetKeyCount( npLeft );
    xbInt32 lNewLeftKeyCnt = (xbInt32) ((lKeyCnt + 1) * dSplitFactor) + 1;
    xbInt32 lNewRightKeyCnt = lKeyCnt + 1 - lNewLeftKeyCnt;

    if(( iRc = InsertNodeL( vpTag, npLeft, iSlotNo, cpKeyBuf, ulPtr )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }

    // move right half off of left node to the right node
    pSrc = npLeft->cpBlockData;
    pSrc += 8 + (lNewLeftKeyCnt * mpTag->iKeyItemLen);
    pTrg = npRight->cpBlockData;
    pTrg += 8;
    iLen = lNewRightKeyCnt * mpTag->iKeyItemLen;
    memmove( pTrg, pSrc, (size_t) iLen );

    // write the new key counts into the nodes
    pTrg = npLeft->cpBlockData;
    ePutInt32( pTrg, lNewLeftKeyCnt );
    pTrg = npRight->cpBlockData;
    ePutInt32( pTrg, lNewRightKeyCnt );

    // zero out the next key number so this node is not confused with interior node
    iLen = 8 + (lNewLeftKeyCnt * mpTag->iKeyItemLen);
    pSrc = npLeft->cpBlockData;
    pSrc += iLen;
    memset( pSrc, 0x00, npLeft->ulBufSize - (xbUInt32) iLen );

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
    sMsg.Sprintf( "xbIxMdx::SplitNodeL() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}
/**************************************************************************************************************/
//! @brief TagSerialNo.
/*!
  This routine is used internally for reading or updating the serial number on a given tag when the tag.

  \param iOption 1 - Read tag serial number off disk, save in structure<br>
                 2 - Write serial number from memory to disk<br>
                 3 - Read serial number off disk, increment, write updated number to disk<br>
         mpTag - Pointer to tag for serial number update
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbIxMdx::TagSerialNo( xbInt16 iOption, xbMdxTag * mpTag ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;

  try{
    xbInt64 lPos = (mpTag->ulTagHdrPageNo * 512) + 20;

    if( iOption != 2 ){
      if(( iRc = xbFseek( lPos, SEEK_SET )) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      }
      if(( iRc = xbFgetc( mpTag->cSerialNo )) != XB_NO_ERROR ){
        iErrorStop = 110;
        throw iRc;
      }
    }

    if( iOption == 3 )
      mpTag->cSerialNo++;

    if( iOption != 1 ){
      if(( iRc = xbFseek( lPos, SEEK_SET )) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }
      if(( iRc = xbFwrite( &mpTag->cSerialNo, 1, 1 )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxMdx::UpdateSerialNo() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}

/***********************************************************************/
//! @brief UpdateTagKey
/*!
  This routine updates a key or a given tag.
  The file header is considered to be the first 2048 bytes in the file.

  \param cAction A - Add a key.<br>
                 D - Delete a key.<br>
                 R - Revise a key.<br>
  \param vpTg - Pointer to tag.<br>
  \param ulRecNo - Record number association with the action.<br>
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16  xbIxMdx::UpdateTagKey( char cAction, void *vpTag, xbUInt32 ulRecNo ){


  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbMdxTag *npTag = (xbMdxTag *) vpTag;

  try{
    // save off any needed fields for updating
    xbUInt32 ulTagSizeSave    = npTag->ulTagSize;

    if( cAction == 'D' || cAction == 'R' ){
      // std::cout << "UpdateTagKey-delete going to DeleteKey \n";
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
    if( ulTagSizeSave != npTag->ulTagSize ){
      if(( iRc = UpdateTagSize( npTag, npTag->ulTagSize )) != XB_NO_ERROR) {
        iErrorStop = 120;
        throw iRc;
      }
    }
    // update the serial number
    if(( iRc = TagSerialNo( 3, npTag )) != XB_NO_ERROR ){
      iErrorStop = 130;
      throw iRc;
    }

  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxMdx::UpdateTagKey() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}

/**************************************************************************************************************/
//! @brief Write head block.
/*!
  This routine updates the MDX file header and commits changes to disk.
  The file header is considered to be the first 2048 bytes in the file.

  \param iOption 0 - Entire 2048 byte header, used for creating a new mdx file.<br>
                 1 - Bytes 28 through 46, used when adding or deleting a tag.<br>
                 2 - Bytes 32 through 46, used after updating keys in the file.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/


xbInt16 xbIxMdx::WriteHeadBlock( xbInt16 iOption ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;

  try{
    xbDate d;
    d.Sysdate();   // set to system date, today
    cUpdateYY = (char) d.YearOf() - 1900;
    cUpdateMM = (char) d.MonthOf();
    cUpdateDD = (char) d.DayOf( XB_FMT_MONTH );

    if( iOption >  0 ){
      char buf[48];
      memset( buf, 0x00, 48 );
      xbUInt32 ulStartPos = 0;
      xbUInt32 ulLen = 0;

      if( iOption == 1 ){
        ePutInt16( &buf[28], iTagUseCnt );
        buf[30] = cNextTag;
        buf[31] = 0x1b;
        ulStartPos = 28;
        ulLen = 19;
      } else {
        ulStartPos = 32;
        ulLen = 16;
      }

      ePutUInt32( &buf[32], ulPageCnt );
      ePutUInt32( &buf[36], ulFirstFreePage );
      ePutUInt32( &buf[40], ulNoOfBlockAvail );
      buf[44]   = cUpdateYY;
      buf[45]   = cUpdateMM;
      buf[46]   = cUpdateDD;

      if(( iRc = xbFseek( ulStartPos, SEEK_SET )) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      }

      if(( iRc = xbFwrite( &buf[ulStartPos], ulLen, 1 )) != XB_NO_ERROR ){
        iErrorStop = 110;
        throw iRc;
      }

    } else if( iOption == 0 ){
      char buf[2048];
      memset( buf, 0x00, 2048 );

      buf[0] = cVersion;
      cCreateYY = cUpdateYY;
      cCreateMM = cUpdateMM;
      cCreateDD = cUpdateDD;
      buf[1] = cCreateYY;
      buf[2] = cCreateMM;
      buf[3] = cCreateDD;


      for( xbUInt32 l = 0; l < sFileName.Len() && l < 10; l++ ){
        buf[l+4] = sFileName[l+1];
      }

      ePutInt16( &buf[20], iBlockFactor );
      ePutInt16( &buf[22], (xbInt16) GetBlockSize() );

      buf[24]   = cProdIxFlag;
      buf[25]   = cTagEntryCnt;
      ePutInt16 ( &buf[26], iTagLen );
      ePutInt16 ( &buf[28], iTagUseCnt );
      buf[30]   = cNextTag;
      buf[31]   = c1B;
      ePutUInt32( &buf[32], ulPageCnt );
      ePutUInt32( &buf[36], ulFirstFreePage );
      ePutUInt32( &buf[40], ulNoOfBlockAvail );
      buf[44]   = cUpdateYY;
      buf[45]   = cUpdateMM;
      buf[46]   = cUpdateDD;

      // not sure what the following "1" is for in a sea of zeroes....
      // maybe it's current tag or default tag or something along those lines?
      buf[529]  = 0x01;

      xbRewind();
      if(( iRc = xbFwrite( buf, 2048, 1 )) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }
    } else {
      iErrorStop = 130;
      iRc = XB_INVALID_OPTION;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxMdx::WriteHeadBlock() Exception Caught. Error Stop = [%d] iRc = [%d] option = [%d]", iErrorStop, iRc, iOption );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}
/***********************************************************************/
xbInt16 xbIxMdx::UpdateTagSize( xbMdxTag *mpTag, xbUInt32 ulTagSz ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  char    buf[4];
  try{
    ePutUInt32( &buf[0], ulTagSz );
    if(( iRc = xbFseek( (xbInt64) ((mpTag->ulTagHdrPageNo *512 )+ 4), SEEK_SET )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }
    if(( iRc = xbFwrite( &buf[0], 4, 1 )) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxMdx::UpdateTagSize() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}


/***********************************************************************/
//void xbIxMdx::TestStub( char *s, void *vpTag ){
void xbIxMdx::TestStub( char *, void * ){
}
/***********************************************************************/
}              /* namespace       */
#endif         /*  XB_MDX_SUPPORT */

