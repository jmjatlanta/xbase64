/* xbmemo3.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023,2024 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

  This class is used for support dBASE V3 memo files

*/

#include "xbase.h"

#ifdef XB_MEMO_SUPPORT
#ifdef XB_DBF3_SUPPORT

namespace xb{

/***********************************************************************/
xbMemoDbt3::xbMemoDbt3( xbDbf * dbf, xbString const & sFileName ) : xbMemo( dbf, sFileName ){
 iMemoFileType = 3;
 SetBlockSize( 512 );
}
/***********************************************************************/
xbMemoDbt3::~xbMemoDbt3(){}
/***********************************************************************/
xbInt16 xbMemoDbt3::Abort(){
  return XB_NO_ERROR;
}/***********************************************************************/
xbInt16 xbMemoDbt3::Commit(){
  return XB_NO_ERROR;
}
/***********************************************************************/
xbInt16 xbMemoDbt3::CreateMemoFile(){
  xbInt16 iRc = XB_NO_ERROR;
  char cBuf[4];
  if(( iRc = xbFopen( "w+b", dbf->GetShareMode())) != XB_NO_ERROR )
    return iRc;
  ulHdrNextBlock = 1L;
  ePutUInt32( cBuf, ulHdrNextBlock );
  if(( iRc = xbFwrite( cBuf, 4, 1 ))!= XB_NO_ERROR ){
    xbFclose();
    return iRc;
  }
  for(int i = 0; i < 12; i++ )
    xbFputc( 0x00 );
  xbFputc( 0x03 );
  for(int i = 0; i < 495; i++ )
    xbFputc( 0x00 );
  if(( mbb = (void *) malloc( 512 )) == NULL ){
    xbFclose();
    return XB_NO_MEMORY;
  }
  return XB_NO_ERROR;
}
/***********************************************************************/
#ifdef XB_DEBUG_SUPPORT
xbInt16 xbMemoDbt3::DumpMemoFreeChain() {
  std::cout << "Xbase version 3 file - no free block chain" << std::endl;
  return XB_NO_ERROR;
}
#endif   // XB_DEBUG_SUPPORT

xbInt16 xbMemoDbt3::DumpMemoHeader(){
  xbInt16  iRc = XB_NO_ERROR;
  xbUInt64 stFileSize;
  if(( iRc = ReadDbtHeader( 1 )) != XB_NO_ERROR )
    return iRc;
  GetFileSize( stFileSize );
  std::cout << "Version 3 Memo Header Info" << std::endl;
  std::cout << "Memo File Name       = " << GetFqFileName()    << std::endl;
  std::cout << "Next Available Block = " << ulHdrNextBlock     << std::endl;
  std::cout << "Memo File Version    = " << (xbInt16) cVersion << " (";
  xbBit b;
  b.BitDump( cVersion );
  std::cout << ")" << std::endl;
  std::cout << "Block Size           = " << GetBlockSize()     << std::endl;
  std::cout << "File Size            = " << stFileSize         << std::endl;
  std::cout << "Block Count          = " << stFileSize / GetBlockSize() << std::endl;
  return XB_NO_ERROR;
}
/***********************************************************************/
xbInt16 xbMemoDbt3::GetMemoField( xbInt16 iFieldNo, xbString & sMemoData ){

  xbInt16 iErrorStop = 0;
  xbInt16 iRc = XB_NO_ERROR;
  xbUInt32 ulScnt;
  char *sp, *spp;
  xbUInt32  ulBlockNo;
  xbBool  bDone = xbFalse;
  sMemoData = "";
  try{
    if(( iRc = dbf->GetULongField( iFieldNo, ulBlockNo )) < XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }
    if( ulBlockNo == 0L ){
      sMemoData = "";
      return XB_NO_ERROR;
    }
    spp = NULL;
    while( !bDone ){
      if(( iRc = ReadBlock( ulBlockNo++, GetBlockSize(), mbb )) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }
      ulScnt = 0;
      sp = (char *) mbb;
      while( ulScnt < 512 && !bDone ){
        if( *sp == 0x1a && *spp == 0x1a )
          bDone = xbTrue;
        else{
          ulScnt++; spp = sp; sp++;
        }
      }
      sMemoData.Append( (char *) mbb, ulScnt );
    }
    sMemoData.ZapTrailingChar( 0x1a );
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbMemoDbt3::GetMemoField() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
/***********************************************************************/
xbInt16 xbMemoDbt3::GetMemoFieldLen( xbInt16 iFieldNo, xbUInt32 & ulFieldLen ){

  xbInt16   iErrorStop = 0;
  xbInt16   iRc = XB_NO_ERROR;
  xbInt16   iScnt;
  char *sp, *spp;
  xbUInt32  ulBlockNo;
  xbInt16   iNotDone;
  try{
    if(( iRc = dbf->GetULongField( iFieldNo, ulBlockNo )) < XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }
    if( ulBlockNo == 0 ){
      ulFieldLen = 0;
      return XB_NO_ERROR;
    }
    ulFieldLen = 0L;
    spp = NULL;
    iNotDone = 1;
    while( iNotDone ){
      if(( iRc = ReadBlock( ulBlockNo++, GetBlockSize(), mbb )) != XB_NO_ERROR ){
        iErrorStop = 110;
        throw iRc;
      }
      iScnt = 0;
      sp = (char *) mbb;
      while( iScnt < 512 && iNotDone ){
        if( *sp == 0x1a && *spp == 0x1a )
          iNotDone = 0;
        else{
          ulFieldLen++; iScnt++; spp = sp; sp++;
        }
      }
    }
    if( ulFieldLen > 0 ) ulFieldLen--;
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbMemoDbt3::GetMemoFieldLen() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
/***********************************************************************/
xbInt16 xbMemoDbt3::OpenMemoFile() {
  xbInt16 iRc = XB_NO_ERROR;
  if(( iRc = xbFopen( dbf->GetOpenMode(), dbf->GetShareMode())) != XB_NO_ERROR )
    return iRc;
  if(( mbb = (void *) malloc( 512 )) == NULL ){
    xbFclose();
    return XB_NO_MEMORY;
  }
  return XB_NO_ERROR;
}
/***********************************************************************/
xbInt16 xbMemoDbt3::PackMemo( void (*memoStatusFunc ) ( xbUInt32 ulItemNum, xbUInt32 ulNumItems ))
{
  xbInt16 iRc = XB_NO_ERROR;
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

    xbMemoDbt3 *pMemo = new xbMemoDbt3( dbf, sTempMemoName );
    if(( iRc = pMemo->CreateMemoFile()) != XB_NO_ERROR ){
      iErrorStop = 130;
      throw iRc;
    }
    // for dBASE III, block size is always 512, don't need to reset it
    // for each record in dbf
    xbUInt32 ulRecCnt;
    if(( iRc = dbf->GetRecordCnt( ulRecCnt )) != XB_NO_ERROR ){
      iErrorStop = 140;
      throw iRc;
    }
    xbInt32  lFldCnt  = dbf->GetFieldCnt();
    char     cFldType;
    xbString sMemoFldData;

    for( xbUInt32 ulI = 1; ulI <= ulRecCnt; ulI++ ){

      if(( iRc = dbf->GetRecord( ulI )) != XB_NO_ERROR ){
        iErrorStop = 150;
        throw iRc;
      }
      if( (void *) memoStatusFunc )
        (*memoStatusFunc) ( ulI, ulRecCnt );

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

    if(( cBlock = (char *) malloc( (size_t) ulBlkSize )) == NULL ){
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

    //close and delete target
    if(( iRc = pMemo->xbFclose()) != XB_NO_ERROR ){
      iErrorStop = 250;
      throw iRc;
    }

    if(( iRc = pMemo->xbRemove()) != XB_NO_ERROR ){
      iErrorStop = 260;
      throw iRc;
    }
    free( cBlock );
    delete pMemo;
  }
  catch (xbInt16 iRc ){
    free( cBlock );
    xbString sMsg;
    sMsg.Sprintf( "xbMemoDbt3::PackMemo() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, iRc );
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
xbInt16 xbMemoDbt3::ReadDbtHeader( xbInt16 iOption ){
  char *p;
  char MemoBlock[20];
  xbInt16 rc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbUInt32 ulReadSize;

  try{
    if( !FileIsOpen() ){
      iErrorStop = 100;
      rc = XB_NOT_OPEN;
      throw rc;
    }
    if( iOption == 0 )
      ulReadSize = 4;
    else{
      xbUInt64 stFileSize = 0;
      if(( rc = GetFileSize( stFileSize )) != XB_NO_ERROR ){
        iErrorStop = 110;
        throw rc;
      }
      if( stFileSize < 4 ){
        iErrorStop = 120;
        rc = XB_INVALID_BLOCK_NO;
        throw rc;
      }
      else if( stFileSize > 20 )
        ulReadSize = 130;
      else
        ulReadSize = 4;
    }
    if( xbFseek( 0, SEEK_SET )){
      iErrorStop = 140;
      rc = XB_SEEK_ERROR;
      throw rc;
    }
    if(( xbFread( &MemoBlock, ulReadSize, 1 )) != XB_NO_ERROR ){
      iErrorStop = 150;
      rc = XB_READ_ERROR;
      throw rc;
    }
    p = MemoBlock;
    ulHdrNextBlock = eGetUInt32( p );

    if( iOption == 0)
      return XB_NO_ERROR;

    if( ulReadSize >= 20 ){
      p+=16;
      cVersion = *p;
    }
  }
  catch (xbInt16 rc ){
    xbString sMsg;
    sMsg.Sprintf( "xbMemoDbt3::ReadDbtHeader() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, rc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( rc ));
  }
  return rc;
}
/***********************************************************************/
xbInt16 xbMemoDbt3::UpdateHeaderName(){
  return XB_NO_ERROR;
}
/***********************************************************************/
xbInt16 xbMemoDbt3::UpdateMemoField( xbInt16 iFieldNo, const xbString & sMemoData ) {

  xbInt16 iErrorStop = 0;
  xbInt16 iRc = XB_NO_ERROR;
  try{
    if( sMemoData == "" ){
      if(( iRc = dbf->PutField( iFieldNo, "" )) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      }
    } else {
      xbUInt32 ulDataLen      = sMemoData.Len() + 2;
      xbUInt32 ulBlocksNeeded = (ulDataLen / 512) + 1;
      xbUInt32 ulLastDataBlock;
      if(( iRc = CalcLastDataBlock( ulLastDataBlock )) != XB_NO_ERROR ){
        iErrorStop = 110;
        throw iRc;
      }
      if(( iRc = xbFseek( ((xbInt64) ulLastDataBlock * 512), SEEK_SET )) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }
      if(( iRc = xbFwrite( sMemoData.Str(), sMemoData.Len(), 1 )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }
      if(( iRc = xbFputc( 0x1a, 2 )) != XB_NO_ERROR ){
        iErrorStop = 140;
        throw iRc;
      }
      if(( iRc = xbFputc( 0x00, (xbInt32) ( ulBlocksNeeded * 512 ) - (xbInt32) ulDataLen )) != XB_NO_ERROR ){
        iErrorStop = 150;
        throw iRc;
      }
      if(( iRc = dbf->PutULongField( iFieldNo, ulLastDataBlock )) != XB_NO_ERROR ){
        iErrorStop = 160;
        throw iRc;
      }
      ulHdrNextBlock = ulLastDataBlock + ulBlocksNeeded;
      if(( iRc = UpdateHeadNextNode()) != XB_NO_ERROR ){
        iErrorStop = 170;
        throw iRc;
      }
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbMemoDbt3::UpdateMemoField() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
/***********************************************************************/
xbInt16 xbMemoDbt3::Zap(){
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
    if(( iRc != xbTruncate( 512 )) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbMemoDbt3::Zap() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
/***********************************************************************/
}              /* namespace        */
#endif         /*  XB_DBF3_SUPPORT */
#endif         /*  XB_MEMO_SUPPORT  */
