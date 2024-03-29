/*  xbblockread.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

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
//! @brief Dump read block internals to stdout.
/*!
  Dump the current read block internals to stdout.
*/


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
//! @brief Get the first record number in the current block.
/*!
  Retrieve the first record numer in the current block.<br>

  \returns First record number in the current block.
*/

xbUInt32 xbBlockRead::GetBlockFirstRecNo() const{
  return ulFirstBlkRec;
}
/************************************************************************/
//! @brief Get record for specified record number.
/*!
  Retrieve a record from read block buffer and copy it into the record buffer.
  If the current record is not in the current block, the routine calls 
  GetBlockForRecNo to load the currect block from disk.<br><br>
  For performance reasons, this method assumes a valid record number has been 
  passed.<br><br>

  \param ulRecNo - Record number to retrieve.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16  xbBlockRead::GetRecord( xbUInt32 ulRecNo ){
  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;

  // std::cout << "xbBlockRead::GetRecord( " << ulRecNo << ")\n";

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
    if(( dbf->xbFseek( ulStartPos, SEEK_SET )) != XB_NO_ERROR ){
      iErrorStop = 110;
      iRc = XB_SEEK_ERROR;
      throw iRc;
    }

    if(( dbf->GetFileMtime( tFmTime )) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw iRc;
    }
    // read it
    if(( dbf->xbFread( (void *) pBlock, ulRecCnt * dbf->GetRecordLen(), 1 )) != XB_NO_ERROR ){
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
//! @brief Get the current block size.
/*!
  Retrieve the current block size.<br><br>

  \returns Current Block Size.
*/

xbUInt32 xbBlockRead::GetBlockSize() const{
  return ulBlkSize;
}

/************************************************************************/
//! @brief Get the current block record count.
/*!
  Retrieve the current number of records loaded in the block.<br><br>

  \returns Current Block Size.
*/

xbUInt32 xbBlockRead::GetBlockRecCnt() const {
  return ulRecCnt;
}

/************************************************************************/
//! @brief Init the block processing for a iven DBF file.
/*!
  Initialize the settings for a given DBF file.<br>
  This routine may adjust the block size as needed to eliminate unused
  memory or adjust it bigger if too small.

  \param ulRecNo - ulBlockSize - Block size to allocate. If 0 or missing, it uses default block size of 32K.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

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
//! @brief Set the block size.
/*!
  St block size for this DBF file.<br>.

  \param ulBlkSize - Block Size.
  \returns XB_NO_ERROR
*/

xbInt16 xbBlockRead::SetBlockSize( xbUInt32 ulBlkSize ){
  this->ulBlkSize = ulBlkSize;
  return XB_NO_ERROR;
}

/************************************************************************/
}   /* namespace */ 
#endif         /*  XB_BLOCKREAD_SUPPORT */