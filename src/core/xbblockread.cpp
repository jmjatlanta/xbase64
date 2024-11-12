/*  xbblockread.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023,2024 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

This module handles block read methods.  Block reading is used for performance improvement
during sequential access processing.

*/

#include "xbase.h"

#ifdef XB_BLOCKREAD_SUPPORT

namespace xb{

/************************************************************************/
xbBlockRead::xbBlockRead(  xbDbf * d ) {
  pBlock        = NULL;
  ulBlkSize     = 0;
  ulFirstBlkRec = 0;
  ulRecCnt      = 0;
  ulMaxRecs     = 0;
  bEof          = xbFalse;
  this->dbf     = d;
  tFmTime       = 0;
}
/************************************************************************/
xbBlockRead::~xbBlockRead(){
  if( pBlock ){
    free( pBlock );
    pBlock = NULL;
  }
}
/************************************************************************/
#ifdef XB_DEBUG_SUPPORT
void xbBlockRead::DumpReadBlockInternals(){

  xbUInt32 ulRecCnt;
  dbf->GetRecordCnt( ulRecCnt );

  std::cout << "------- DumpBlockInternals ---------" << std::endl;
  std::cout << "Dflt Blk Size    = [" << dbf->GetXbasePtr()->GetDefaultBlockReadSize()
                                                               << "]" << std::endl;
  std::cout << "Dbf Record Count = [" << ulRecCnt              << "]" << std::endl;
  std::cout << "Dbf Record Len   = [" << dbf->GetRecordLen()   << "]" << std::endl;
  std::cout << "ulBlkSize        = [" << ulBlkSize             << "]" << std::endl;
  std::cout << "ulMaxRecs        = [" << ulMaxRecs             << "]" << std::endl;
  std::cout << "ulFirstBlkRec    = [" << ulFirstBlkRec         << "]" << std::endl;
  std::cout << "ulRecCnt         = [" << ulRecCnt              << "]" << std::endl;
  std::cout << "bEof             = [" << bEof                  << "]" << std::endl;
}
#endif  // XB_DEBUG_SUPPORT
/************************************************************************/
xbInt16 xbBlockRead::Init(xbUInt32 ulBlockSize ){

  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;

  try{
    // calculate the block size
    if( ulBlockSize == 0 )
      ulBlkSize = dbf->GetXbasePtr()->GetDefaultBlockReadSize();

    // if not big enough to handle more than one record, bump it up to something meaningful
    if( ulBlkSize < (xbUInt32)(dbf->GetRecordLen() * 2 ))
      ulBlkSize = (xbUInt32) dbf->GetRecordLen() * 10;

    ulMaxRecs = (xbUInt32) ulBlkSize / dbf->GetRecordLen();
    ulBlkSize = ulMaxRecs * dbf->GetRecordLen();

    // allocate memory for the block
    if(( pBlock = (char *) calloc( 1, ulBlkSize )) == NULL ){
      iErrorStop = 100;
      iRc = XB_NO_MEMORY;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbBlockRead::Init() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    dbf->GetXbasePtr()->WriteLogMessage( sMsg.Str() );
    dbf->GetXbasePtr()->WriteLogMessage( dbf->GetXbasePtr()->GetErrorMessage( iRc ));
  }
  return iRc;
}
/************************************************************************/
xbUInt32 xbBlockRead::GetBlockFirstRecNo() const{
  return ulFirstBlkRec;
}
/************************************************************************/
xbUInt32 xbBlockRead::GetBlockRecCnt() const {
  return ulRecCnt;
}
/************************************************************************/
xbUInt32 xbBlockRead::GetBlockSize() const{
  return ulBlkSize;
}
/************************************************************************/
xbInt16  xbBlockRead::GetRecord( xbUInt32 ulRecNo ){
  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;
  try{
    if( !( ulRecNo >= ulFirstBlkRec && ulRecNo < (ulFirstBlkRec + ulRecCnt))){
      if(( iRc = GetBlockForRecNo( ulRecNo )) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      }
    }
    char *s = pBlock;
    s += (ulRecNo - ulFirstBlkRec) * dbf->GetRecordLen();
    char *t = dbf->RecBuf;
    xbUInt32 ulRecLen = dbf->GetRecordLen();
    for( xbUInt32 l = 0; l < ulRecLen; l++ ){
      *t = *s;
      t++;
      s++;
    }
    dbf->ulCurRec = ulRecNo;
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbBlockRead::GetBlockForRecNo() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    dbf->GetXbasePtr()->WriteLogMessage( sMsg.Str() );
    dbf->GetXbasePtr()->WriteLogMessage( dbf->GetXbasePtr()->GetErrorMessage( iRc ));
  }
  return iRc;
}
/************************************************************************/
void xbBlockRead::SetBlockSize( xbUInt32 ulBlkSize ){
  this->ulBlkSize = ulBlkSize;
}
/************************************************************************/
//! @brief Get record for specified record number.
/*!
  Retrieve a block containing specified record.  This routine calculates the
  correct block in the DBF file, updates the internal block fields and retrieves 
  the block of records from disk and loads into the block buffer.<br><br>

  \param ulRecNo - Record number to retrieve.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16  xbBlockRead::GetBlockForRecNo( xbUInt32 ulRecNo ){

  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;

  try{
    xbUInt32 ulDbfRecCnt;

    if(( iRc = dbf->GetRecordCnt( ulDbfRecCnt )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }

    // calc to determine block number for the requested record, 0 based offset
    xbUInt32 ulBlockNo = (xbUInt32)(ulRecNo / ulMaxRecs);
    if( ulRecNo % ulMaxRecs == 0 ) ulBlockNo--;

    // calc the first record
    ulFirstBlkRec = (ulBlockNo * ulMaxRecs);

    // calc the record count
    if(( ulFirstBlkRec + ulMaxRecs) > ulDbfRecCnt ){
      ulRecCnt = ulDbfRecCnt - ulFirstBlkRec;
      bEof     = xbTrue;
    } else {
      ulRecCnt = ulMaxRecs;
      bEof     = xbFalse;
    }

    // position accordingly
    xbInt64 ulStartPos = dbf->GetHeaderLen() + ((xbInt64) ulFirstBlkRec * dbf->GetRecordLen());
    if(( iRc = dbf->xbFseek( ulStartPos, SEEK_SET )) != XB_NO_ERROR ){
      iErrorStop = 110;
      iRc = XB_SEEK_ERROR;
      throw iRc;
    }

    if(( iRc = dbf->GetFileMtime( tFmTime )) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw iRc;
    }
    // read it
    if(( iRc = dbf->xbFread( (void *) pBlock, ulRecCnt * dbf->GetRecordLen(), 1 )) != XB_NO_ERROR ){
      iErrorStop = 130;
      throw iRc;
    }

    ulFirstBlkRec++;    // zero offset in the routine, regular record number from ths point forward
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbBlockRead::GetBlockForRecNo() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    dbf->GetXbasePtr()->WriteLogMessage( sMsg.Str() );
    dbf->GetXbasePtr()->WriteLogMessage( dbf->GetXbasePtr()->GetErrorMessage( iRc ));
  }
  return iRc;
}
/************************************************************************/
}   /* namespace */ 
#endif         /*  XB_BLOCKREAD_SUPPORT */