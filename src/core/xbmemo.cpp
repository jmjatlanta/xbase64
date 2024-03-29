/* xbmemo.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

   Base memo class
*/

#include "xbase.h"

#ifdef XB_MEMO_SUPPORT

namespace xb{

/***********************************************************************/
//! @brief Class Constructor.
/*!
  \param dbf Pointer to dbf construct.
  \param sFileName Memo file name.
*/

xbMemo::xbMemo( xbDbf * dbf, xbString const &sFileName ) : xbFile( dbf->GetXbasePtr() ) {
  this->dbf = dbf;              /* pointer to the associated dbf class instance */
  // xbase = dbf->GetXbasePtr();   /* pointer to the engine */
  SetDirectory( dbf->GetDirectory());
  SetFileName( sFileName );
  mbb = NULL;
  #ifdef XB_LOCKING_SUPPORT
  bFileLocked = xbFalse;
  #endif
}
/***********************************************************************/
//! @brief Class Destructor.
xbMemo::~xbMemo(){
 if( mbb )
    free( mbb );
}
/***********************************************************************/
//! @brief Calculate the last data block number.
/*!
  \param ulLastDataBlock Output - Last used block number in the file.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbMemo::CalcLastDataBlock( xbUInt32 & ulLastDataBlock ){

  xbInt16 iRc = XB_NO_ERROR;
  if(( iRc = xbFseek( 0, SEEK_END )) != XB_NO_ERROR )
    return iRc;

  ulLastDataBlock = (xbUInt32) xbFtell() / (xbUInt32) GetBlockSize();
  return XB_NO_ERROR;
}
/***********************************************************************/
//! @brief Close the memo file.
/*!
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbMemo::CloseMemoFile(){

  if( mbb ){
    free( mbb );
    mbb = NULL;
  }
  return xbFclose();
}


/***********************************************************************/
//! @brief Get memo file type.
/*!
  \returns 3 - Version 3 memo file.<br>
           4 - Version 4 memo file.
*/
xbInt16 xbMemo::GetMemoFileType(){
  return iMemoFileType;
}

/***********************************************************************/
//! @brief Get next block available from file header.
/*!
  \param ulBlockNo Output - Next block number for appending data to memo file.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbMemo::GetHdrNextBlock( xbUInt32 & ulBlockNo ){
  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  try{
    if(( iRc = ReadDbtHeader( 0 )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }
    ulBlockNo = ulHdrNextBlock;
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbmemo::GetNextAvailableBlock() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}
/***********************************************************************/
#ifdef XB_LOCKING_SUPPORT
//! @brief Lock memo file
/*!

  \param iLockFunction XB_LOCK<br>XB_UNLOCK
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbMemo::LockMemo( xbInt16 iLockFunction ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;

  try{
    if( iLockFunction == XB_LOCK ){

      if( bFileLocked )       // already locked
        return XB_NO_ERROR;

      if( dbf->GetLockFlavor() == LK_DBASE ){
        iRc = xbLock( XB_LOCK, LK4026531838, 1 );
        if( iRc != XB_NO_ERROR ){
          if( iRc == XB_LOCK_FAILED )
            return iRc;
          else {
            iErrorStop = 100;
            throw iRc;
          }
        } else {
          bFileLocked = xbTrue;
        }
      }
    } else if( iLockFunction == XB_UNLOCK ){

      if( !bFileLocked )  // already unlocked
        return XB_NO_ERROR;

      if( dbf->GetLockFlavor() == LK_DBASE ){
        iRc = xbLock( XB_UNLOCK, LK4026531838, 1 );
        if( iRc != XB_NO_ERROR ){
          if( iRc == XB_LOCK_FAILED )
            return iRc;
          else {
            iErrorStop = 110;
            throw iRc;
          }
        } else {
          bFileLocked = xbFalse;
        }
      }
    } else {
      iErrorStop = 120;
      iRc = XB_INVALID_OPTION;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbmemo::LockMemoFile() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}

/***********************************************************************/
//! @brief Get memo file lock status.
/*!
  \returns xbTrue - Memo file is locked.<br>
           xbFalse - Memo file is not locked.
*/
xbBool xbMemo::GetMemoLocked() const {
  return bFileLocked;
}
#endif

/***********************************************************************/
//! @brief Update Next Node number in file header
/*!
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbMemo::UpdateHeadNextNode(){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  try{
    char buf[4];
    ePutUInt32( buf, ulHdrNextBlock );
    if(( iRc = xbFseek( 0, SEEK_SET )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }
    if(( iRc = xbFwrite( &buf, 4, 1 )) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbmemo::UpdateHeadeNextNode() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}
/***********************************************************************/
}              /* namespace       */
#endif         /*  XB_MEMO_SUPPORT */

