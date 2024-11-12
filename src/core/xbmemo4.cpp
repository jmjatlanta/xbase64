/* xbmemo4.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023,2024 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

  This class is used for support dBASE V4 memo files

*/

#include "xbase.h"

#ifdef XB_MEMO_SUPPORT
#ifdef XB_DBF4_SUPPORT

namespace xb{

/***********************************************************************/
xbMemoDbt4::xbMemoDbt4( xbDbf * dbf, xbString const & sFileName ) : xbMemo( dbf, sFileName ){
 iMemoFileType = 4;
 SetBlockSize( dbf->GetCreateMemoBlockSize() );
}
/***********************************************************************/
xbMemoDbt4::~xbMemoDbt4(){}
/***********************************************************************/
xbInt16 xbMemoDbt4::Abort(){

  xbInt16  rc = XB_NO_ERROR;
  xbInt16  iErrorStop = 0;
  xbUInt32 ulBlockNo;
  try{
    xbUInt32 ulNodeCnt = llNewBlocks.GetNodeCnt();
    for( xbUInt32 l = 0; l < ulNodeCnt; l++ ){
      if(( rc = llNewBlocks.RemoveFromFront( ulBlockNo )) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw rc;
      }
      if(( rc = FreeMemoBlockChain( ulBlockNo )) != XB_NO_ERROR ){
        iErrorStop = 110;
        throw rc;
      }
    }
    llOldBlocks.Clear();
  }

  catch (xbInt16 rc ){
    xbString sMsg;
    sMsg.Sprintf( "xbMemoDbt4::Abort() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, rc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( rc ));
  }
  return rc;
}
/***********************************************************************/
xbInt16 xbMemoDbt4::Commit(){

  xbInt16  rc = XB_NO_ERROR;
  xbInt16  iErrorStop = 0;
  xbUInt32 ulBlockNo;
  try{
    xbUInt32 ulNodeCnt = llOldBlocks.GetNodeCnt();
    for( xbUInt32 l = 0; l < ulNodeCnt; l++ ){
      if(( rc = llOldBlocks.RemoveFromFront( ulBlockNo )) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw rc;
      }
      if(( rc = FreeMemoBlockChain( ulBlockNo )) != XB_NO_ERROR ){
        iErrorStop = 110;
        throw rc;
      }
    }
    llNewBlocks.Clear();
  }
  catch (xbInt16 rc ){
    xbString sMsg;
    sMsg.Sprintf( "xbMemoDbt4::Commit() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, rc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( rc ));
  }
  return rc;
}
/***********************************************************************/
xbInt16 xbMemoDbt4::CreateMemoFile(){

  xbInt16 iErrorStop = 0;
  xbInt16 rc = XB_NO_ERROR;
  char cBuf[4];
  try{
    if(( rc = xbFopen( "w+b", dbf->GetShareMode() )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw rc;
    }
    ulHdrNextBlock = 1L;
    ePutUInt32( cBuf, ulHdrNextBlock );
    if(( rc = xbFwrite( cBuf, 4, 1 ))!= XB_NO_ERROR ){
      iErrorStop = 110;
      xbFclose();
      throw rc;
    }
    for(int i = 0; i < 4; i++ )
      xbFputc( 0x00 );
    GetFileNamePart( sDbfFileNameWoExt );
    sDbfFileNameWoExt.PadRight( ' ', 8 );                  // need at least eight bytes of name
    sDbfFileNameWoExt = sDbfFileNameWoExt.Mid( 1, 8 );     // need no more than eight bytes of name
    for( int i = 1; i < 9; i++ )
      xbFputc( sDbfFileNameWoExt[i] );

    for(int i = 0; i < 4; i++ )
      xbFputc( 0x00 );

    ePutInt16( cBuf, GetBlockSize());
    if(( rc = xbFwrite( cBuf, 2, 1 ))!= XB_NO_ERROR ){
      iErrorStop = 120;
      xbFclose();
      throw rc;
    }
    for( xbUInt32 i = 0; i < (GetBlockSize() - 22); i++ )
      xbFputc( 0x00 );
    if(( mbb = (void *) malloc( GetBlockSize())) == NULL ){
      rc = XB_NO_MEMORY;
      iErrorStop = 130;
      return XB_NO_MEMORY;
    }
  }
  catch (xbInt16 rc ){
    xbString sMsg;
    sMsg.Sprintf( "xbMemoDbt4::CreateMemoFile() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, rc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( rc ));
    xbFclose();
  }
  return rc;
}
/***********************************************************************/
#ifdef XB_DEBUG_SUPPORT
xbInt16 xbMemoDbt4::DumpMemoFreeChain() 
{
  xbInt16 rc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbUInt32  ulCurBlock, ulLastDataBlock;

  try{
    if(( rc = ReadDbtHeader(1)) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw rc;
    }
    if(( rc = CalcLastDataBlock( ulLastDataBlock )) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw rc;
    }
    ulCurBlock = ulHdrNextBlock;
    std::cout << "**********************************" << std::endl;
    std::cout << "Head Node Next Block = " << ulCurBlock      << std::endl;;
    std::cout << "Total blocks in file = " << ulLastDataBlock << std::endl;
    while( ulCurBlock < ulLastDataBlock ){
      if(( rc = ReadBlockHeader( ulCurBlock, 2 )) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw rc;
      }
      std::cout << "**********************************" << std::endl;
      std::cout << "This Free Block = [" << ulCurBlock      << "] contains [" << ulFreeBlockCnt << "] block(s)" << std::endl;
      std::cout << "Next Free Block = [" << ulNextFreeBlock << "]" << std::endl;
      ulCurBlock = ulNextFreeBlock;
    }
  }
  catch (xbInt16 rc ){
    xbString sMsg;
    sMsg.Sprintf( "xbMemoDbt4::UpdateMemoField() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, rc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( rc ));
  }
  return XB_NO_ERROR;
}

xbInt16 xbMemoDbt4::DumpMemoInternals() {

  xbLinkListNode<xbUInt32> *llPtr;
  xbInt16 iNodeCnt;
  llPtr    = llOldBlocks.GetHeadNode();
  iNodeCnt = llOldBlocks.GetNodeCnt();
  std::cout << "Link List Old Blocks - " << iNodeCnt << " nodes" << std::endl;
  for( xbInt16 i = 0; i < iNodeCnt; i++ ){
    std::cout << llPtr->GetKey() << ",";
    llPtr = llPtr->GetNextNode();
  }
  std::cout << std::endl;
  llPtr    = llNewBlocks.GetHeadNode();
  iNodeCnt = llNewBlocks.GetNodeCnt();
  std::cout << "Link List New Blocks - " << iNodeCnt << " nodes" << std::endl;
  for( xbInt16 i = 0; i < iNodeCnt; i++ ){
    std::cout << llPtr->GetKey() << ",";
    llPtr = llPtr->GetNextNode();
  }
  std::cout << std::endl;
  return XB_NO_ERROR;
}
#endif  // XB_DEBUG_SUPPORT

/***********************************************************************/
xbInt16 xbMemoDbt4::DumpMemoHeader(){

  xbInt16  rc = XB_NO_ERROR;
  xbUInt32 ulLastDataBlock;
  CalcLastDataBlock( ulLastDataBlock );

  if(( rc = ReadDbtHeader( 1 )) != XB_NO_ERROR )
    return rc;
  std::cout << "Version 4 Memo Header Info" << std::endl;
  std::cout << "Memo File Name       = "    << GetFqFileName()         << std::endl;
  std::cout << "Hdr Next Avail Block = "    << ulHdrNextBlock          << std::endl;
  std::cout << "Block Size           = "    << GetBlockSize()          << std::endl;
  std::cout << "Dbf File Name wo Ext = "    << sDbfFileNameWoExt.Str() << std::endl;
  std::cout << "Last Data Block      = "    << ulLastDataBlock         << std::endl;
  return rc;
}

/************************************************************************/
xbInt16 xbMemoDbt4::FindBlockSetInChain( xbUInt32 ulBlocksNeeded,
    xbUInt32 &ulLastDataBlock, xbUInt32 &ulLocation, xbUInt32 &ulPrevNode, xbBool &bFound ){
  xbInt16  rc = XB_NO_ERROR;
  xbInt16  iErrorStop = 0;
  xbUInt32 ulCurNode;

  try{
    ulPrevNode = 0L;
    if( ulLastDataBlock == 0 ){
      /* Determine last good data block */
      if(( rc = CalcLastDataBlock( ulLastDataBlock )) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw rc;
      }
    }
    if( ulHdrNextBlock < ulLastDataBlock ){
      ulCurNode = ulHdrNextBlock;

      if(( rc = ReadBlockHeader( ulHdrNextBlock, 2 )) != XB_NO_ERROR ){
        iErrorStop = 110;
        throw rc;
      }
      while( ulBlocksNeeded > ulFreeBlockCnt && ulNextFreeBlock < ulLastDataBlock ){
        ulPrevNode = ulCurNode;
        ulCurNode  = ulNextFreeBlock;
        if(( rc = ReadBlockHeader( ulNextFreeBlock, 2 )) != XB_NO_ERROR ){
          iErrorStop = 120;
          throw rc;
        }
      }
      if( ulBlocksNeeded <= ulFreeBlockCnt ){
        ulLocation = ulCurNode;
        //    PreviousNode = lPrevNode;
        bFound = xbTrue;
      } else {   // no data found and at end of chain 
        ulPrevNode = ulCurNode;
        bFound = xbFalse;
      }
    } else {
      bFound = xbFalse;
    }
  }
   catch (xbInt16 rc ){
    xbString sMsg;
    sMsg.Sprintf( "xbMemoDbt4::FindBlockSetInChain() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, rc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( rc ));
  }
  return rc;
}
/***********************************************************************/
xbInt16 xbMemoDbt4::FreeMemoBlockChain( xbUInt32 ulBlockNo ){
  xbUInt32 ulLastDataBlock;
  return FreeMemoBlockChain( ulBlockNo, ulLastDataBlock );
}
/***********************************************************************/
xbInt16 xbMemoDbt4::FreeMemoBlockChain( xbUInt32 ulBlockNo, xbUInt32 &ulLastDataBlock )
{
  xbInt16 rc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbUInt32 ulNoOfFreedBlocks;
  xbUInt32 ulLastFreeBlock = 0;
  xbUInt32 ulLastFreeBlockCnt = 0;
  xbUInt32 ulSaveNextFreeBlock;

  // iFieldNo - The field no m\bing deleted
  // iBlockNo - The block number being deleted
  // iNoOfFreeBlocks - The number of blocks being freed with this delete
  // iLastDataBlock - The next block number to allocate if more blocks needed
  // iHdrNextBlock - The head pointer in the main header block
  // iNextFreeBlock - The block that is immediately following the current free block to be added
  // iLastFreeBlock - Last free block number,prior to this block
  // iLastFreeBlockCnt - Last free block number of blocks

  try{

    if( ulBlockNo <= 0 ){
      iErrorStop = 100;
      rc =XB_INVALID_BLOCK_NO;
      throw rc;
    }
    /* Load the first block */
    if(( rc = ReadBlockHeader( ulBlockNo, 1 )) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw rc;
    }
    if( (ulFieldLen) % GetBlockSize() )
      ulNoOfFreedBlocks = ((ulFieldLen) / GetBlockSize()) + 1L;
    else
      ulNoOfFreedBlocks = (ulFieldLen) / GetBlockSize();

    /* Determine last good data block */
    if(( rc = CalcLastDataBlock( ulLastDataBlock )) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw rc;
    }
    if(( rc = ReadDbtHeader( 0 )) != XB_NO_ERROR ){
      iErrorStop = 130;
      throw rc;
    }
    // Not an empty node chain, position to correct location in chain
    ulNextFreeBlock = ulHdrNextBlock;
    while( ulBlockNo > ulNextFreeBlock && ulBlockNo < ulLastDataBlock ){
      ulLastFreeBlock = ulNextFreeBlock;
      if(( rc = ReadBlockHeader( ulNextFreeBlock, 2 )) != XB_NO_ERROR ){
        iErrorStop = 140;
        return rc;
      }
      ulLastFreeBlockCnt = ulFreeBlockCnt;
    }

    // One of two outcomes at this point
    //  A)  This block is combined with the next free block chain, and points to the free chain after the next free block
    //  B)  This block is not combined with the next free block chain, and points to the next block 
    //      (which could be the last block
    // should next block should be concatonated onto the end of this set?
    ulSaveNextFreeBlock = ulNextFreeBlock;
    if(( ulBlockNo + ulNoOfFreedBlocks ) == ulNextFreeBlock && ulNextFreeBlock < ulLastDataBlock ){
      if(( rc = ReadBlockHeader( ulNextFreeBlock, 2 )) != XB_NO_ERROR ){
        iErrorStop = 150;
        throw rc;
      }
      ulNoOfFreedBlocks += ulFreeBlockCnt;
      ulSaveNextFreeBlock = ulNextFreeBlock;
    }

    // if this is the first set of free blocks
    if( ulLastFreeBlock == 0 ){
      // 1 - write out the current block
      // 2 - update header block
      // 3 - write header block
      // 4 - update data field

      ePutUInt32( (char *) mbb,   ulSaveNextFreeBlock );
      ePutUInt32( (char *) mbb+4, ulNoOfFreedBlocks );
      if(( rc = WriteBlock( ulBlockNo, 8, mbb )) != XB_NO_ERROR ){
        iErrorStop = 160;
        throw rc;
      }

      ulHdrNextBlock = ulBlockNo;
      if(( rc = UpdateHeadNextNode()) != XB_NO_ERROR ){
        iErrorStop = 170;
        throw rc;
      }
      return XB_NO_ERROR; 
    }

    /* determine if this block set should be added to the previous set */
    if(( ulLastFreeBlockCnt + ulLastFreeBlock ) == ulBlockNo ){
      if(( rc = ReadBlockHeader( ulLastFreeBlock, 2 )) != XB_NO_ERROR ){
        iErrorStop = 180;
        throw rc;
      }
      ulFreeBlockCnt += ulNoOfFreedBlocks;

      ePutUInt32( (char *) mbb,   ulSaveNextFreeBlock );
      ePutUInt32( (char *) mbb+4, ulFreeBlockCnt );
      if(( rc = WriteBlock( ulLastFreeBlock, 8, mbb )) != XB_NO_ERROR ){
        iErrorStop = 190;
        throw rc;
      }
      return XB_NO_ERROR;
    }

    /* insert into the chain */
    /* 1 - set the next bucket on the current node         */
    /* 2 - write this node                                 */
    /* 3 - go to the previous node                         */
    /* 4 - insert this nodes id into the previous node set */
    /* 5 - write previous node                             */

    ePutUInt32( (char *) mbb,   ulSaveNextFreeBlock );
    ePutUInt32( (char *) mbb+4, ulNoOfFreedBlocks );
    if(( rc = WriteBlock( ulBlockNo, 8, mbb )) != XB_NO_ERROR ){
      iErrorStop = 200;
      throw rc;
    }

    if(( rc = ReadBlockHeader( ulLastFreeBlock, 2 )) != XB_NO_ERROR ){
      iErrorStop = 210;
      throw rc;
    }

    ePutUInt32( (char *) mbb,   ulBlockNo );
    if(( rc = WriteBlock( ulLastFreeBlock, 8, mbb )) != XB_NO_ERROR ){
      iErrorStop = 220;
      throw rc;
    }
  }
  catch (xbInt16 rc ){
    xbString sMsg;
    sMsg.Sprintf( "xbMemoDbt4::DeleteMemoField() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, rc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( rc ));
  }
  return rc;
}
/************************************************************************/
xbInt16 xbMemoDbt4::GetBlockSetFromChain( xbUInt32 ulBlocksNeeded,
   xbUInt32 ulLocation, xbUInt32 ulPrevNode )
{
  xbInt16 rc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbUInt32  ulNextFreeBlock2;
  xbUInt32  ulNewFreeBlocks;
  xbUInt32  ulSaveNextFreeBlock;

  try{
    if(( rc = ReadBlockHeader( ulLocation, 2 )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw rc;
    }

    if( ulBlocksNeeded == ulFreeBlockCnt ){  // grab this whole set of blocks
      if( ulPrevNode == 0 ){                // first in the chain
        ulHdrNextBlock = ulNextFreeBlock;
        if(( rc = UpdateHeadNextNode()) != XB_NO_ERROR ){
          iErrorStop = 110;
          throw rc;
        }
      }
      else                                // remove out of the middle or end 
      {
        ulNextFreeBlock2 = ulNextFreeBlock;
        if(( rc = ReadBlockHeader( ulPrevNode, 2 )) != XB_NO_ERROR ){
          iErrorStop = 120;
          throw rc;
        }
        ulNextFreeBlock = ulNextFreeBlock2;
        if(( rc = WriteBlockHeader( ulPrevNode, 2 )) != XB_NO_ERROR ){
          iErrorStop = 130;
          throw rc;
        }
      }
    } else {                               // only take a portion of this set 
      if( ulPrevNode == 0 ){                // first in the set 
        ulHdrNextBlock = ulLocation + ulBlocksNeeded;
        if(( rc = UpdateHeadNextNode()) != XB_NO_ERROR ){
          iErrorStop = 140;
          throw rc;
        }
        ulFreeBlockCnt -= ulBlocksNeeded;
        if(( rc = WriteBlockHeader( ulHdrNextBlock, 2 )) != XB_NO_ERROR ){
          iErrorStop = 150;
          throw rc;
        }
      }
       else {                             // remove out of the middle or end
        ulNewFreeBlocks     = ulFreeBlockCnt - ulBlocksNeeded;
        ulSaveNextFreeBlock = ulNextFreeBlock; 
        ulNextFreeBlock2    = ulLocation + ulBlocksNeeded;

        if(( rc = ReadBlockHeader( ulPrevNode, 2 )) != XB_NO_ERROR ){
          iErrorStop = 160;
          throw rc;
        }
        ulNextFreeBlock = ulNextFreeBlock2;
        if(( rc = WriteBlockHeader( ulPrevNode, 2 )) != XB_NO_ERROR ){
          iErrorStop = 170;
          throw rc;
        }
        ulFreeBlockCnt  = ulNewFreeBlocks;
        ulNextFreeBlock = ulSaveNextFreeBlock;
        if(( rc = WriteBlockHeader( ulNextFreeBlock2, 2 )) != XB_NO_ERROR ){
          iErrorStop = 180;
          throw rc;
        }
      }
    }
  }
  catch (xbInt16 rc ){
    xbString sMsg;
    sMsg.Sprintf( "xbMemoDbt4::GetBlockSetFromChain() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, rc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( rc ));
  }
  return rc;
}
/***********************************************************************/
xbInt16 xbMemoDbt4::GetMemoField( xbInt16 iFieldNo, xbString & sMemoData ){

  xbUInt32 ulBlockNo;
  xbUInt32 ulMemoFieldLen;
  xbUInt32 ulMemoFieldDataLen;
  xbInt16  rc = XB_NO_ERROR;
  xbInt16  iErrorStop = 0;
  char     *p = NULL;

  try{
    if(( rc = GetMemoFieldLen( iFieldNo, ulMemoFieldLen, ulBlockNo )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw rc;
    }

    if( ulBlockNo == 0L || ulMemoFieldLen == 0L )
      sMemoData = "";
    else{
      ulMemoFieldDataLen = ulMemoFieldLen - 8;

      if(( p = (char *)calloc(1, ulMemoFieldDataLen+1)) == NULL ){
        iErrorStop = 110;
        rc = XB_NO_MEMORY;
        xbString sMsg;
        sMsg.Sprintf( "xbMemoDbt4::GetMemoField() lBlockNo = %ld Data Len = [%ld]", ulBlockNo, ulMemoFieldDataLen + 1 );
        xbase->WriteLogMessage( sMsg.Str() );
        throw rc;
      }

      // go to the first block of the memo field, skip past the first 8 bytes
      if(( xbFseek( ( ulBlockNo * GetBlockSize() + 8 ), SEEK_SET )) != XB_NO_ERROR ){
        iErrorStop = 120;
        rc = XB_SEEK_ERROR;
        throw rc;
      }

      // read the memo file data into buffer pointed to by "p"
      if(( rc = xbFread( p, ulMemoFieldDataLen, 1 )) != XB_NO_ERROR ){
        iErrorStop = 130;
        rc = XB_READ_ERROR;
        throw rc;
      }
      // null terminate the string
      char *p2;
      p2 = p + ulMemoFieldDataLen;
      *p2 = 0x00;

      // save it to the string
      sMemoData.Set( p, ulMemoFieldDataLen + 1 );
      free( p );
    }
  }
  catch (xbInt16 rc ){
    xbString sMsg;
    sMsg.Sprintf( "xbMemoDbt4::GetMemoField() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, rc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( rc ));
    if( p )
      free( p );
  }
  return rc;
}
/***********************************************************************/
xbInt16 xbMemoDbt4::GetMemoFieldLen( xbInt16 iFieldNo, xbUInt32 &ulMemoFieldLen ){
  xbUInt32 ulBlockNo;
  return GetMemoFieldLen( iFieldNo, ulMemoFieldLen, ulBlockNo );
}
/***********************************************************************/
xbInt16 xbMemoDbt4::GetMemoFieldLen( xbInt16 iFieldNo, xbUInt32 &ulMemoFieldLen, xbUInt32 &ulBlockNo ){

  xbInt16 rc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  char    cFieldType;
  try{
    if(( rc = dbf->GetFieldType( iFieldNo, cFieldType )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw rc;
    }
    if( cFieldType != 'M' ){
      iErrorStop = 110;
      rc = XB_INVALID_MEMO_FIELD;
      throw rc;
    }
    if(( rc = dbf->GetULongField( iFieldNo, ulBlockNo )) < XB_NO_ERROR ){
      iErrorStop = 120;
      throw rc;
    }
    if( ulBlockNo < 1 ){
      ulMemoFieldLen = 0;
      return XB_NO_ERROR;
    }
    if(( rc = ReadBlockHeader( ulBlockNo, 1 )) != XB_NO_ERROR ){
      iErrorStop = 130;
      throw rc;
    }
    ulMemoFieldLen = ulFieldLen;
  }
  catch (xbInt16 rc ){
    xbString sMsg;
    sMsg.Sprintf( "xbMemoDbt4::GetMemoFieldLen() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, rc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( rc ));
  }
  return rc;
}
/***********************************************************************/
xbInt16 xbMemoDbt4::OpenMemoFile() {

  xbInt16 iErrorStop = 0;
  xbInt16 rc = XB_NO_ERROR;
  try{
    if(( rc = xbFopen( dbf->GetOpenMode(), dbf->GetShareMode())) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw rc;
    }
    if(( rc = ReadDbtHeader( 1 )) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw rc;
    }
    if(( mbb = (void *) malloc( GetBlockSize())) == NULL ){
      xbFclose();
      iErrorStop = 120;
      rc = XB_NO_MEMORY;
      throw rc;
    }
  }
  catch (xbInt16 rc ){
    xbString sMsg;
    sMsg.Sprintf( "xbMemoDbt4::OpenMemoFile() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, rc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( rc ));
  }
  return rc;
}
/***********************************************************************/
xbInt16 xbMemoDbt4::PackMemo( void (*memoStatusFunc ) ( xbUInt32 ulItemNum, xbUInt32 ulNumItems )){
  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;
  char *  cBlock = NULL;

  #ifdef XB_LOCKING_SUPPORT
  xbBool  bTableLocked = xbFalse;
  xbBool  bMemoLocked = xbFalse;
  #endif

  try{
    #ifdef XB_LOCKING_SUPPORT
    //if( dbf->GetAutoLock() && !dbf->GetTableLocked() ){
    if( dbf->GetMultiUser() && !dbf->GetTableLocked() ){
      if(( iRc = dbf->LockTable( XB_LOCK )) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      } else {
        bTableLocked = xbTrue;
      }
      if(( iRc = LockMemo( XB_LOCK )) != XB_NO_ERROR ){
        iErrorStop = 110;
        throw iRc;
      } else {
        bMemoLocked = xbTrue;
      }
    }
    #endif

    // create temp file
    xbString sTempMemoName;
    //if(( iRc = CreateUniqueFileName( GetDirectory(), "dbt", sTempMemoName )) != XB_NO_ERROR ){
    if(( iRc = CreateUniqueFileName( xbase->GetTempDirectory(), "DBT", sTempMemoName )) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw iRc;
    }

    xbMemoDbt4 *pMemo = new xbMemoDbt4( dbf, sTempMemoName );
    if(( iRc = pMemo->CreateMemoFile()) != XB_NO_ERROR ){
      iErrorStop = 130;
      throw iRc;
    }
    // for dBASE III, block size is always 512, don't need to reset it
    // for each record in dbf
    xbUInt32 ulRecCnt;
    if(( iRc = dbf->GetRecordCnt( ulRecCnt)) != XB_NO_ERROR ){
      iErrorStop = 140;
      throw iRc;
    }
    xbInt32  lFldCnt  = dbf->GetFieldCnt();
    char     cFldType;
    xbString sMemoFldData;

    for( xbUInt32 ulI = 1; ulI <= ulRecCnt; ulI++ ){
      if(( iRc = dbf->GetRecord( ulI  )) != XB_NO_ERROR ){
        iErrorStop = 150;
        throw iRc;
      }

      if( (void *) memoStatusFunc)
        (*memoStatusFunc)(ulI, ulRecCnt );

      // for each memo field 
      for( xbInt32 lFc = 0; lFc < lFldCnt; lFc++ ){
        if(( iRc = dbf->GetFieldType( lFc, cFldType )) != XB_NO_ERROR ){
          iErrorStop = 160;
          throw iRc;
        }
          if( cFldType == 'M' ){
          // copy it to work field
          if(( iRc = dbf->GetMemoField( lFc, sMemoFldData )) != XB_NO_ERROR ){
            iErrorStop = 170;
            throw iRc;
          }
          // write it to new field
          if(( iRc = pMemo->UpdateMemoField( lFc, sMemoFldData )) != XB_NO_ERROR ){
            iErrorStop = 180;
            throw iRc;
          }
        }
      }
    }
    //copy target back to source
    xbUInt32 ulBlkSize = GetBlockSize();
    xbUInt64 ullFileSize;
    if(( iRc = pMemo->GetFileSize( ullFileSize )) != XB_NO_ERROR ){
      iErrorStop = 190;
      throw iRc;
    }
    // file size should be evenly divisible by block size
    xbUInt32 ulBlkCnt;

    if( ullFileSize % ulBlkSize ){
      iErrorStop = 200;
      throw iRc;
    } else {
      ulBlkCnt = (xbUInt32) (ullFileSize / ulBlkSize);
    }
    if(( iRc = xbTruncate( 0 )) != XB_NO_ERROR ){
      iErrorStop = 210;
      throw iRc;
    }
    if(( cBlock = (char *) malloc( ulBlkSize )) == NULL ){
      iErrorStop = 220;
      throw iRc;
    }

    // can't rename files in a multiuser, cross platform environment, causes issues
    // copy work table back to source table

    for( xbUInt32 ulBc = 0; ulBc < ulBlkCnt; ulBc++ ){
      if(( iRc = pMemo->ReadBlock( ulBc, ulBlkSize, cBlock )) != XB_NO_ERROR ){
        iErrorStop = 230;
        throw iRc;
      }
      if(( iRc = WriteBlock( ulBc, ulBlkSize, cBlock )) != XB_NO_ERROR ){
        iErrorStop = 240;
        throw iRc;
      }
    }

    if(( xbFseek( 8, SEEK_SET )) != XB_NO_ERROR ){
        iErrorStop = 250;
        iRc = XB_SEEK_ERROR;
        throw iRc;
    }

    for( int i = 1; i < 9; i++ )
      xbFputc( sDbfFileNameWoExt[i] );

    //close and delete target
    if(( iRc = pMemo->xbFclose()) != XB_NO_ERROR ){
      iErrorStop = 260;
      throw iRc;
    }
    if(( iRc = pMemo->xbRemove()) != XB_NO_ERROR ){
      iErrorStop = 270;
      throw iRc;
    }
    free( cBlock );
    delete pMemo;

  }
  catch (xbInt16 iRc ){
    free( cBlock );

    xbString sMsg;
    sMsg.Sprintf( "xbMemoDbt4::PackMemo() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  #ifdef XB_LOCKING_SUPPORT
   if( bTableLocked )
     dbf->LockTable( XB_UNLOCK );
   if( bMemoLocked )
      LockMemo( XB_UNLOCK );
  #endif
  return iRc;
}
/***********************************************************************/
xbInt16 xbMemoDbt4::ReadBlockHeader( xbUInt32 ulBlockNo, xbInt16 iOption ) {

  xbInt16 rc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  try{
    if(( rc = ReadBlock( ulBlockNo, 8, mbb )) != XB_NO_ERROR ){
      iErrorStop = 100;
      rc = XB_READ_ERROR;
    }
    if( iOption == 1 ){
      iField1    = eGetInt16((char *) mbb );
      iStartPos  = eGetInt16((char *) mbb+2);
      ulFieldLen = eGetUInt32((char *) mbb+4);
    }
    else if( iOption == 2 ){
      ulNextFreeBlock = eGetUInt32((char *) mbb );
      ulFreeBlockCnt  = eGetUInt32((char *) mbb+4 );
    }
    else{
      iErrorStop = 110;
      rc = XB_INVALID_OPTION;
      throw rc;
    }
  }
  catch (xbInt16 rc ){
    xbString sMsg;
    sMsg.Sprintf( "xbMemoDbt4::ReadBlockHeader() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, rc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( rc ));
  }
  return rc;
}
/***********************************************************************/
xbInt16 xbMemoDbt4::ReadDbtHeader( xbInt16 iOption ) {

  xbInt16 iErrorStop = 0;
  xbInt16 rc = XB_NO_ERROR;
  xbInt16 iReadLen = 0;
  char    *p;
  char    MemoBlock[22];
  try{
    if( !FileIsOpen() ){
      iErrorStop = 100;
      rc = XB_NOT_OPEN;
      throw rc;
    }
    if( xbFseek( 0, SEEK_SET )){
      iErrorStop = 110;
      rc = XB_SEEK_ERROR;
      throw rc;
    }
    if( iOption )
      iReadLen = 22;
    else
      iReadLen = 4;
    if(( xbFread( &MemoBlock, (size_t) iReadLen, 1 )) != XB_NO_ERROR ){
      iErrorStop = 120;
      rc = XB_READ_ERROR;
      throw rc;
    }
    p = MemoBlock;
    ulHdrNextBlock = eGetUInt32( p );
    if( iOption == 0)
      return XB_NO_ERROR;
    p += 8;
    sDbfFileNameWoExt = "";
    for( int i = 0; i < 8; i++ )
      sDbfFileNameWoExt += *p++;
    p += 4;
    SetBlockSize( (xbUInt32) eGetInt16( p ));
    cVersion = MemoBlock[16];
  }
  catch (xbInt16 rc ){

    xbString sMsg;
    sMsg.Sprintf( "xbMemoDbt4::ReadDbtHeader() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, rc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( rc ));
  }
  return rc;
}

/***********************************************************************/
#ifdef XB_DEBUG_SUPPORT
xbInt16 xbMemoDbt4::ReadFreeBlockHeader( xbUInt32 ulBlockNo, xbUInt32 &ulNextBlock, xbUInt32 &ulFreeBlockCount ){

  xbInt16 rc = XB_NO_ERROR;
  rc = ReadBlockHeader( ulBlockNo, 2 );
  ulNextBlock = ulNextFreeBlock;
  ulFreeBlockCount = ulFreeBlockCnt;
  return rc;
}
#endif
/***********************************************************************/
xbInt16 xbMemoDbt4::UpdateHeaderName() {

  xbInt16 rc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  GetFileNamePart( sDbfFileNameWoExt );
  sDbfFileNameWoExt.PadRight( ' ', 8 );                  // need at least eight bytes of name
  sDbfFileNameWoExt = sDbfFileNameWoExt.Mid( 1, 8 );     // need no more than eight bytes of name
  try{
    if(( rc = xbFseek( 8, SEEK_SET )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw rc;
    }

    for( int i = 1; i < 9; i++ ){
      if(( rc = xbFputc( sDbfFileNameWoExt[i] )) != XB_NO_ERROR ){
        iErrorStop = 110;
        throw rc;
      }
    }
  }
  catch (xbInt16 rc ){
    xbString sMsg;
    sMsg.Sprintf( "xbMemoDbt4::UpdateHeaderName() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, rc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( rc ));
  }
  return rc;
}
/***********************************************************************/
xbInt16 xbMemoDbt4::UpdateMemoField( xbInt16 iFieldNo, const xbString & sMemoData ) {

  xbInt16  iErrorStop = 0;
  xbInt16  rc = XB_NO_ERROR;
  xbUInt32 ulBlockNo;
  try{
    if(( rc = dbf->GetULongField( iFieldNo, ulBlockNo )) < XB_NO_ERROR ){
      iErrorStop = 100;
      throw rc;
    }
    if( sMemoData == "" ){
      if( ulBlockNo == 0 ){
        /* if nothing to do, return */
        return XB_NO_ERROR;
      } else {
        // if this is in the new blocks link list already, then this is not the first update for this memo field
        // this would be second or third update on the field since the original change and not commited
        // Since it won't be needed in either a Commmit() or Abort(), can be freed immediately

        //std::cout << "cp1 ulBlockNo = " << ulBlockNo << " rc = " << llNewBlocks.SearchFor( ulBlockNo ) << "\n";
        if( llNewBlocks.SearchFor( ulBlockNo ) == XB_NO_ERROR ){

        // if( llNewBlocks.SearchFor( ulBlockNo ) > 0 ){

          if(( rc = FreeMemoBlockChain( ulBlockNo )) != XB_NO_ERROR ){
            iErrorStop = 110;
            throw rc;
          }
          if(( llNewBlocks.RemoveByVal( ulBlockNo )) < XB_NO_ERROR ){
            iErrorStop = 120;
            throw rc;
          }
        } else {
          // first revision, save what it was in case of Abort() command
          if(( llOldBlocks.InsertAtFront( ulBlockNo )) != XB_NO_ERROR ){
            iErrorStop = 130;
            throw rc;
          }
        }
        if(( rc = dbf->PutField( iFieldNo, "" )) != XB_NO_ERROR ){
          iErrorStop = 140;
          throw rc;
        }
      }
    } else {
      // free up the old space
      xbUInt32 ulLastDataBlock = 0L;

      if( ulBlockNo > 0 ){

        // std::cout << "cp2 ulBlockNo = " << ulBlockNo << " rc = " << llNewBlocks.SearchFor( ulBlockNo ) << "\n";
        // if( llNewBlocks.SearchFor( ulBlockNo ) > 0 ){
        if( llNewBlocks.SearchFor( ulBlockNo ) == XB_NO_ERROR ){

          if(( rc = FreeMemoBlockChain( ulBlockNo, ulLastDataBlock )) != XB_NO_ERROR ){
            iErrorStop = 150;
            throw rc;
          }
        } else {
          // first revision, save what it was in case of Abort() command
          if(( rc = llOldBlocks.InsertAtFront( ulBlockNo )) != XB_NO_ERROR ){
            iErrorStop = 160;
            throw rc;
          }
        }
      }
      // should next line be unsigned 32 bit int?
      xbUInt32 ulTotalLen = 8 + sMemoData.Len();
      xbUInt32 ulBlocksNeeded;
      if( ulTotalLen % GetBlockSize())
        ulBlocksNeeded = ulTotalLen / GetBlockSize() + 1;
      else
        ulBlocksNeeded = ulTotalLen / GetBlockSize();

      xbBool bUsedBlockFound;
      xbUInt32 ulHeadBlock;
      xbUInt32 ulPrevNode;
      if(( rc = FindBlockSetInChain( ulBlocksNeeded, ulLastDataBlock, ulHeadBlock, ulPrevNode, bUsedBlockFound )) != XB_NO_ERROR ){
        iErrorStop = 170;
        throw rc;
      }
      iField1   = -1;
      iStartPos = 8;
      ulFieldLen = sMemoData.Len() + 8;

      if( bUsedBlockFound ){

        if(( rc = GetBlockSetFromChain( ulBlocksNeeded, ulHeadBlock, ulPrevNode )) != XB_NO_ERROR ){
          iErrorStop = 180;
          throw rc;
        }

        if(( rc = WriteBlockHeader( ulHeadBlock, 1 )) != XB_NO_ERROR ){
          iErrorStop = 190;
          throw rc;
        }

        if(( rc = xbFwrite( sMemoData.Str(), sMemoData.Len(), 1 )) != XB_NO_ERROR ){
          iErrorStop = 200;
          throw rc;
        }
      } else { // append to the end 

        if(( rc = WriteBlockHeader( ulLastDataBlock, 1 )) != XB_NO_ERROR ){
          iErrorStop = 220;
          throw rc;
        }

        if(( rc = xbFwrite( sMemoData.Str(), sMemoData.Len(), 1 )) != XB_NO_ERROR ){
          iErrorStop = 230;
          throw rc;
        }

        if(( rc = xbFputc( 0x00, (xbInt32)((ulBlocksNeeded * GetBlockSize()) - (sMemoData.Len() + 8)))) != XB_NO_ERROR ){
          iErrorStop = 240;
          throw rc;
        }

        if( ulLastDataBlock == ulHdrNextBlock ){                   // this is first node to be added to the node chain
          ulHdrNextBlock += ulBlocksNeeded;
          ulHeadBlock = ulLastDataBlock;
          if(( rc = UpdateHeadNextNode()) != XB_NO_ERROR ){
            iErrorStop = 250;
            throw rc;
          }

        } else {                                                // adding memo data to the end of the file, but chain exists

          ulNextFreeBlock = ulLastDataBlock + ulBlocksNeeded;
          ulHeadBlock = ulLastDataBlock;
          if(( rc =  WriteBlockHeader( ulPrevNode, 2 )) != XB_NO_ERROR ){
            iErrorStop = 260;
            throw rc;
          }
        }
      }

      if(( rc = llNewBlocks.InsertAtFront( ulHeadBlock )) != XB_NO_ERROR ){  // In case of Abort(), this block needs to be freed
        iErrorStop = 270;
        throw rc;
      }
      if(( rc = dbf->PutLongField( iFieldNo, (xbInt32) ulHeadBlock )) != XB_NO_ERROR ){
        iErrorStop = 280;
        throw rc;
      }
    }
  }

  catch (xbInt16 rc ){
    xbString sMsg;
    sMsg.Sprintf( "xbMemoDbt4::UpdateMemoField() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, rc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( rc ));
  }
  return rc;
}
/***********************************************************************/
xbInt16 xbMemoDbt4::WriteBlockHeader( xbUInt32 ulBlockNo, xbInt16 iOption ) {

  xbInt16 rc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  try{
    if( iOption == 1 ){
      ePutInt16 ((char *) mbb,   iField1 );
      ePutInt16 ((char *) mbb+2, iStartPos );
      ePutUInt32((char *) mbb+4, ulFieldLen );
    }
    else if( iOption == 2 ){
      ePutUInt32((char *) mbb,   ulNextFreeBlock );
      ePutUInt32((char *) mbb+4, ulFreeBlockCnt );
    }
    else{
      iErrorStop = 100;
      rc = XB_INVALID_OPTION;
      throw rc;
    }
    if(( rc = WriteBlock( ulBlockNo, 8, mbb )) != XB_NO_ERROR ){
      iErrorStop = 110;
      rc = XB_READ_ERROR;
    }
  }
  catch (xbInt16 rc ){
    xbString sMsg;
    sMsg.Sprintf( "xbMemoDbt4::WriteHeader() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, rc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( rc ));
  }
  return rc;
}
/***********************************************************************/
xbInt16 xbMemoDbt4::Zap(){

  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;
  char    cBuf[4];
  try{
    ulHdrNextBlock = 1L;
    ePutUInt32( cBuf, ulHdrNextBlock );
    if(( iRc != xbFseek( 0, SEEK_SET )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }
    if(( iRc != xbFwrite( cBuf, 4, 1 ))!= XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }
    if(( iRc != xbTruncate( GetBlockSize())) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbMemoDbt4::Zap() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}/***********************************************************************/
}              /* namespace        */
#endif         /*  XB_DBF4_SUPPORT */
#endif         /*  XB_MEMO_SUPPORT  */

