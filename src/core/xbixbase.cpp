/* xbixbase.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

   Base index class

*/

#include "xbase.h"
#ifdef XB_INDEX_SUPPORT

namespace xb{

/***********************************************************************/
//! @brief Class constructor.
/*!
  /param dbf Pointer to dbf instance.
*/
xbIx::xbIx( xbDbf *dbf ) : xbFile( dbf->GetXbasePtr()) {
  this->dbf = dbf;
  vpCurTag  = NULL;
  cNodeBuf  = NULL;
  bLocked   = xbFalse;
}
/***********************************************************************/
//! @brief Class Destructor.
xbIx::~xbIx(){}


/***********************************************************************/
//! @brief Add Keys for record number
/*!
  For a given a record number, add keys to each tag in the index file
  if it was updated

  \param ulRecNo Record number to add keys for
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbIx::AddKeys( xbUInt32 ulRecNo ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbInt16 i = 0;
  xbInt16 iKeySts;

  try{
    void * vpTag;
    xbInt16 iTagCount = GetTagCount();

    for( i = 0; i < iTagCount; i++ ){
      vpTag = GetTag( i );
      iKeySts = GetKeySts( vpTag );

      if( iKeySts == 1 || iKeySts == 2 ){
        if(( iRc = UpdateTagKey( 'A', vpTag, ulRecNo )) != XB_NO_ERROR ){
          iErrorStop = 100;
          throw iRc;
        }
      }
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIx::AddKeys() Exception Caught. Error Stop = [%d] iRc = [%d] Tag=[%d]", iErrorStop, iRc, i );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}

/***********************************************************************/
//! @brief Allocate memory for index node.
/*!
  Allocate an index node.

  \param ulBufSize Size of buffer to allocate
  \returns null on error<br>Pointer to newly allocated xbIxNode on success
*/
xbIxNode * xbIx::AllocateIxNode( xbUInt32 ulBufSize, xbInt16 )
{
  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;

  try{
    xbIxNode * pNode = (xbIxNode *) calloc( 1, sizeof( xbIxNode ));
    if( pNode == NULL ){
      iErrorStop = 100;
      iRc = XB_NO_MEMORY;
      throw iRc;
    }
    if( ulBufSize == 0 )
      ulBufSize = GetBlockSize();

    pNode->ulBufSize = ulBufSize;
    pNode->cpBlockData = (char *) calloc( 1, ulBufSize );
    if( pNode->cpBlockData == NULL ){
      free( pNode );
      iErrorStop = 110;
      iRc = XB_NO_MEMORY;
      throw iRc;
    }
    return pNode;
 }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIx::AllocateIxNode() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return NULL;
}


/***********************************************************************/
//! @brief Binary search for given value on an index node.
/*!

  Binary search for key lookups
  \param cKeyType Key type
  \param npNode Pointer to index node for search
  \param lKeyItemLen Lenth of key plus pointer values
  \param vpKey Pointer to key value
  \param lSearchKeyLen length of key to search
  \param iCompRc output return code from the CompareKey routine.  CompareKey returns an 
          integer value less than, equal to or greater than zero in when comparing values

  \param bDescending xbTrue for descending index key lookup.<br>
                     xbFalse for ascending index key lookup.
  \return The position in the node the key was found, if multiples it returns the first occurrence.
          If the key is not found, it returns the slot it should be in.
*/

xbInt16 xbIx::BSearchBlock( char cKeyType, xbIxNode *npNode, xbInt32 lKeyItemLen, const void *vpKey, 
                        xbInt32 lSearchKeyLen, xbInt16 &iCompRc, xbBool bDescending ) const {
  xbInt32 lLo = 0;
  xbInt32 lHi  = 0;
  xbInt32 lMid = 0;
  xbInt32 lKeyCnt = GetKeyCount( npNode );


  if( !bDescending ){
    lHi = lKeyCnt - 1;

    while( lLo <= lHi ){
      lMid = (lLo + lHi) / 2;
      iCompRc = CompareKey( cKeyType, GetKeyData( npNode, lMid, lKeyItemLen ), vpKey, (size_t) lSearchKeyLen );
      if( iCompRc > 0 ) 
        lHi = lMid - 1;
      else if( iCompRc < 0 )
        lLo = lMid + 1; 
      else{  // found  match, look for leftmost occurrence

        xbInt32 lFoundPos = lMid;
        lMid--;
        while( lMid >= 0 && CompareKey( cKeyType, GetKeyData( npNode, lMid, lKeyItemLen ), vpKey, (size_t) lSearchKeyLen ) == 0 ){
          lFoundPos = lMid;
          lMid--;
        }
        iCompRc = 0;
        lLo = lFoundPos;
        lHi = -1;
      }
    }
    // update the compare key results
    if( lMid != lLo ){
      if( lLo >= lKeyCnt )
        iCompRc = 1;
      else
        iCompRc = CompareKey( cKeyType, GetKeyData( npNode, lLo, lKeyItemLen ), vpKey, (size_t) lSearchKeyLen );
    }
    return (xbInt16) lLo;

  }  else {   // descending key

    lLo = lKeyCnt - 1;
    while( lLo >= lHi && lHi != -1 ){
      lMid = (lLo + lHi) / 2;
      iCompRc = CompareKey( cKeyType, GetKeyData( npNode, lMid, lKeyItemLen ), vpKey, (size_t) lSearchKeyLen );

      if( iCompRc > 0 ) {
        lHi = lMid + 1;
      }
      else if( iCompRc < 0) {
        lLo = lMid - 1;
      }
      else{  // found  match, look for leftmost occurrence

        xbInt32 lFoundPos = lMid;
        lMid--;
        while( lMid >= 0 && CompareKey( cKeyType, GetKeyData( npNode, lMid, lKeyItemLen ), vpKey, (size_t) lSearchKeyLen ) == 0 ){
          lFoundPos = lMid;
          lMid--;
        }
        iCompRc = 0;
        lHi = lFoundPos;
        lLo = -1;
      }
    }

    // std::cout << "BSB1 lo = " << lLo << " mid = " << lMid << " hi = " << lHi << " keycnt = " << lKeyCnt << " iCompRc = " << iCompRc << "\n";    // key=" << (char *) vpKey << "\n";
    if( lLo < 0 && iCompRc < 0 ) 
      iCompRc = 1;
    else if( iCompRc != 0 ) {
      iCompRc = CompareKey( cKeyType, GetKeyData( npNode, (lLo < 0 ? 0 : lLo), lKeyItemLen ), vpKey, (size_t) lSearchKeyLen );
    }
    // std::cout << "BSB2 lo = " << lLo << " mid = " << lMid << " hi = " << lHi << " keycnt = " << lKeyCnt << " iCompRc = " << iCompRc << "\n";    // key=" << (char *) vpKey << "\n";
    return (xbInt16) lHi;
  }

  // should never get here
  //  return (xbInt16) 0;
}

/***********************************************************************/
//! @brief Check for duplicate keys.
/*!
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbIx::CheckForDupKeys(){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbInt16 i = 0;

  try{
    void * vpTag;
    xbInt16 iTagCount = GetTagCount();
    for( i = 0; i < iTagCount; i++ ){
      vpTag = GetTag( i );
      if(( iRc = CheckForDupKey( vpTag )) < XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      }
    }
  }
  catch (xbInt16 iRc ){
    if( iRc != XB_KEY_NOT_UNIQUE ){
      xbString sMsg;
      sMsg.Sprintf( "xbIxBase::CheckForDupKeys() Exception Caught. Error Stop = [%d] iRc = [%d] Tag=[%d]", iErrorStop, iRc, i );
      xbase->WriteLogMessage( sMsg );
      xbase->WriteLogMessage( GetErrorMessage( iRc ));
    }
  }
  return iRc;
}
/***********************************************************************/
//! @brief Close index file.
/*!
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbIx::Close(){

  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;

  try{
    if(( iRc = xbFclose()) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIx::Close() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}

/***********************************************************************/
//! @brief Compare keys.
/*!
  \param cKeyType C - Character compare.<br>
                  N - Numeric BCD compare.<br>
                  D - Numeric compare.<br>
                  F - Numeric compare.<br>
  \param v1 Left compare.<br>v2 - Right Compare.
  \param iSearchKeyLen Length of key compare.
  \returns 1 - Left operand is greater then right operand.<br>
           0 - Left operand is equal to right operand.<br>
          -1 - Left operand is less than right operand.
*/
inline xbInt16 xbIx::CompareKey( char cKeyType, const void *v1, const void *v2, size_t iSearchKeyLen ) const{
  if( cKeyType == 'C' ){                // character compare
    return memcmp( v1, v2, iSearchKeyLen );
  } else if( cKeyType == 'N' ){         // numeric bcd compare, mdx bcd numeric indices
    xbBcd bcdk1( v1 );
    return bcdk1.Compare( v2 );
  } else if( cKeyType == 'D' || cKeyType == 'F' ){         // julian date compare, ndx float numeric indices
    xbDouble *d1 = (xbDouble *) v1;
    xbDouble *d2 = (xbDouble *) v2;
    if( *d1 < *d2 )
      return -1;
    else if( *d1 > *d2 )
      return 1;
    else
      return 0;
  } else {
//    std::cout << "Unhandled key type [" << cKeyType << "]\n";
  }
  return XB_NO_ERROR;
}
/***********************************************************************/
//! @brief Create Keys for record number
/*!
  \param iOpt 0 Build a key for FindKey usage, only rec buf 0.<br>
              1 Append Mode, Create key for an append, only use rec buf 0, set updated switch.<br>
              2 Update Mode, Create old version and new version keys, check if different, set update switch appropriately.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbIx::CreateKeys( xbInt16 iOpt ) {
  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbInt16 i = 0;

  try{
    void * vpTag;
    xbInt16 iTagCount = GetTagCount();

    for( i = 0; i < iTagCount; i++ ){
      vpTag = GetTag( i );
      if(( iRc = CreateKey( vpTag, iOpt )) < XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      }
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIx::CreateKeys() Exception Caught. Error Stop = [%d] iRc = [%d] Tag=[%d]", iErrorStop, iRc, i );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return XB_NO_ERROR;
}
/***********************************************************************/
//! @brief Delete keys for record number
/*!
  Delete keys to each tag in the index file if it was updated as determined
  by CreateKeys function

  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
//xbInt16 xbIx::DeleteKeys( xbUInt32 ulRecNo ){

xbInt16 xbIx::DeleteKeys(){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbInt16 i = 0;

  try{
    void * vpTag;
    xbInt16 iTagCount = GetTagCount();

    for( i = 0; i < iTagCount; i++ ){
      vpTag = GetTag( i );
      if( GetKeySts( vpTag ) > 1 ){    // 0 = no update 1 = add 2 = update, 3 = delete
        if(( iRc = UpdateTagKey( 'D', vpTag )) != XB_NO_ERROR ){
          iErrorStop = 100;
          throw iRc;
        }
      }
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIx::DeleteKeys() Exception Caught. Error Stop = [%d] iRc = [%d] Tag=[%d]", iErrorStop, iRc, i );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}

/***********************************************************************/
#ifdef XB_DEBUG_SUPPORT
//! @brief Dump anode for debug purposes.
/*!
  \param pNode Pointer to node to dump.
  \param iOption 0 = stdout<br>
                 1 = Syslog<br>
                 2 = Both<br>
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbIx::DumpNode( void *, xbIxNode *pNode, xbInt16 iOption ) const
{
  xbString s;
  s.Sprintf( "Dump Node Block=[%d] CurKey=[%d]", pNode->ulBlockNo, pNode->iCurKeyNo );
  xbase->WriteLogMessage( s, iOption );

  return XB_NO_ERROR;
}
#endif   // XB_DEBUG_SUPPORT

/***********************************************************************/
//! @brief Find double key
/*!
  \param vpTag Pointer to tag to search.
  \param dKey Double value to search for.
  \param iRetrieveSw xbTrue - Retrieve the record if key found.<br>
               xbFalse - Don't retrieve record, check for key existence only.
  \returns XB_NO_ERROR - Key found.<br>
           XB_NOT_FOUND - Key not found.<br>
           <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbIx::FindKey( void *vpTag, xbDouble dKey, xbInt16 iRetrieveSw ){
  return FindKey( vpTag, &dKey, 8, iRetrieveSw );
}
/***********************************************************************/
//! @brief Find string key
/*!
  \param vpTag Pointer to tag to search.
  \param sKey String data to search for.
  \param iRetrieveSw xbTrue - Retrieve the record if key found.<br>
               xbFalse - Don't retrieve record, check for key existence only.
  \returns XB_NO_ERROR - Key found.<br>
           XB_NOT_FOUND - Key not found.<br>
           <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16  xbIx::FindKey( void *vpTag, const xbString &sKey, xbInt16 iRetrieveSw ){

  return FindKey( vpTag, sKey.Str(), (xbInt32) sKey.Len(), iRetrieveSw );
}
/***********************************************************************/
//! @brief Find character key
/*!
  \param vpTag Pointer to tag to search.
  \param cKey String data to search for.
  \param lKeyLen Length of key to search for.
  \param iRetrieveSw xbTrue - Retrieve the record if key found.<br>
               xbFalse - Don't retrieve record, check for key existence only.
  \returns XB_NO_ERROR - Key found.<br>
           XB_NOT_FOUND - Key not found.<br>
           <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16  xbIx::FindKey( void *vpTag, const char *cKey, xbInt32 lKeyLen, xbInt16 iRetrieveSw ){


  //std::cout << "find string key *********222** retrieve sw - " << iRetrieveSw << "\n";


  return FindKey( vpTag, (void *) cKey, lKeyLen, iRetrieveSw );


//  xbInt16 iRc = FindKey( vpTag, (void *) cKey, lKeyLen, iRetrieveSw );
//  std::cout << "xbixbase::Find(line 460) rc = " << iRc << " cur rec = " << dbf->GetCurRecNo() << " fn = [" << dbf->GetFileName().Str() << "]\n";
//  return iRc;

}
/***********************************************************************/
//! @brief Find bcd key
/*!
  \param vpTag Pointer to tag to search.
  \param bcd BCD data to search for.
  \param iRetrieveSw xbTrue - Retrieve the record if key found.<br>
               xbFalse - Don't retrieve record, check for key existence only.

  \returns XB_NO_ERROR - Key found.<br>
           XB_NOT_FOUND - Key not found.<br>
           <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16  xbIx::FindKey( void *vpTag, const xbBcd &bcd, xbInt16 iRetrieveSw ){
  return FindKey( vpTag, bcd.GetBcd(), 12, iRetrieveSw );
}
/***********************************************************************/
//! @brief Find date key
/*!
  \param vpTag Pointer to tag to search.
  \param dtKey Date data to search for.
  \param iRetrieveSw xbTrue - Retrieve the record if key found.<br>
               xbFalse - Don't retrieve record, check for key existence only.
  \returns XB_NO_ERROR - Key found.<br>
           XB_NOT_FOUND - Key not found.<br>
           <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16  xbIx::FindKey( void *vpTag, const xbDate &dtKey, xbInt16 iRetrieveSw ){
  xbDouble d = (xbDouble) dtKey.JulianDays();
  return FindKey( vpTag, &d, 8, iRetrieveSw );
}
/***********************************************************************/
//! @brief Free all nodes in a linked list.
/*!
  \param np Pointer to first node in linked list to free.
  \returns NULL.
*/
xbIxNode *xbIx::FreeNodeChain( xbIxNode *np ){

  // routine returns NULL
  if( np ){
    // free memory for a given chain of nodes
    xbIxNode * np2;

    // Clear the previous node's next pointer
    if( np->npPrev )
      np->npPrev->npNext = NULL;

    // Clear out the tree
    while( np ){
      np2 = np->npNext;
      NodeFree( np );
      np = NULL;
      np = np2;
    }
  }
  return NULL;
}
/***********************************************************************/
//! @brief Read block for block number.
/*!
  Routine to read a node/block out of an index file and store in xbIxNode structure
  \param vpTag Pointer to tag.
  \param ulBlockNo Block number to read off disk.
  \param iOpt 
    0 = Node is read into block buffer, not added to the node chain<br>
    1 = Node is read into new xbIxNode, then added to the node chain, and sets CurNode with new node<br>
    2 = Node is read into new xbIxNode, not added to the node chain<br>
        CurNode points to new node<br>
   \param ulAddlBuf Additional buffer size added to memory 
   \returns <a href="xbretcod_8h.html">Return Codes</a>

*/

xbInt16 xbIx::GetBlock( void *vpTag, xbUInt32 ulBlockNo, xbInt16 iOpt, xbUInt32 ulAddlBuf ){
  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;
  xbIxNode *np = NULL;
  try{

    if( !vpTag && iOpt == 1 ){
      iErrorStop = 100;
      throw iRc;
    }

    // set target location of block read to read
    char  *cp;
    if( iOpt == 0 )
      cp = cNodeBuf;
    else{
      if(( np = AllocateIxNode(GetBlockSize() + ulAddlBuf )) == NULL ){
        iErrorStop = 110;
        iRc = XB_NO_MEMORY;
        throw( iRc );
      }
      cp = np->cpBlockData;
    }
    if(( iRc = ReadBlock( ulBlockNo, GetBlockSize(), cp )) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw iRc;
    }
    if( iOpt == 0 )
      return iRc;
    np->ulBlockNo = ulBlockNo;
    np->iCurKeyNo = 0;
    if( iOpt == 1 )
      AppendNodeChain( vpTag, np );
    else if( iOpt == 2 ){
      std::cout << "Future use stub. xbIxbase::GetBlock() option 2 not coded.\n";
      iErrorStop = 130;
      iRc = XB_INVALID_OPTION;
      throw iRc;
      // SetCurNode( vpTag, np );
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIx::GetBlock() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
    if( np ) NodeFree( np );
  }
  return iRc;
}
/***********************************************************************/
//! @brief Get pointer to current tag.
/*!
  \returns Pointer to current tag.
*/

void *xbIx::GetCurTag() const {
  return vpCurTag;
}
/***********************************************************************/
//! @brief Get pointer to dbf.
/*!
  \returns Pointer to dbf.
*/
xbDbf *xbIx::GetDbf() const {
  return this->dbf;
}
/***********************************************************************/
//! @brief Get the first key for the current tag.
/*!
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbIx::GetFirstKey(){
  return GetFirstKey( vpCurTag, 0 );
}

/***********************************************************************/
//! @brief Get the first key for a given tag.
/*!
  \param vpTag Tag for get first key operation.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbIx::GetFirstKey( void *vpTag ){
  return GetFirstKey( vpTag, 0 );
}
/***********************************************************************/
//! @brief Get the last key for the current tag.
/*!
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbIx::GetLastKey(){
  return GetLastKey( 0, vpCurTag, 0 );
}
/***********************************************************************/
//! @brief Get the last key for a given tag.
/*!
  \param vpTag Tag for get last key operation.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbIx::GetLastKey( void *vpTag ){
  return GetLastKey( 0, vpTag, 0 );
}

/***********************************************************************/
//! @brief Get the file lock status.
/*!
  \returns xbTrue - Index file is locked.<br>xbFalse - Index file is not locked.
*/
xbBool xbIx::GetLocked() const {
  return bLocked;
}

/***********************************************************************/
//! @brief Get the key count for number of keys on a node.
/*!
  \param np Given node for key count.
  \returns Number of keys on the node.
*/
xbInt32 xbIx::GetKeyCount( xbIxNode *np ) const {
  // assumes the first four bytes of the block is a four byte number
  // representing the number of keys contained on the block
  return eGetInt32( np->cpBlockData );
}
/***********************************************************************/
//! @brief Get key data for a given key number.
/*!
  \param np Given node for key retrieval.
  \param iKeyNo Which key to pull.
  \param iKeyItemLen Length of key plus pointers.
  \returns Pointer to a given key.
*/
char * xbIx::GetKeyData( xbIxNode *np, xbInt16 iKeyNo, xbInt16 iKeyItemLen ) const {
  if( !np ) return NULL;
  char *p = np->cpBlockData;
  xbUInt32 ulKeyCnt = eGetUInt32( p );
  if( iKeyNo < 0 || iKeyNo > (xbInt16) ulKeyCnt ) return NULL;
  xbInt16 iOffset = 12 + (iKeyNo * iKeyItemLen);
  p+=iOffset;
  return p;
}
/***********************************************************************/
//! @brief Get the next key for the current tag.
/*!
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbIx::GetNextKey(){
  return GetNextKey( vpCurTag, 0 );
}
/***********************************************************************/
//! @brief Get the next key for the given tag.
/*!
  \param vpTag Tag for next key operation.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbIx::GetNextKey( void *vpTag ){
  return GetNextKey( vpTag, 0 );
}
/***********************************************************************/
//! @brief Get the prev key for the current tag.
/*!
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbIx::GetPrevKey(){
  return GetPrevKey( vpCurTag, 0 );
}
/***********************************************************************/
//! @brief Get the previous key for the given tag.
/*!
  \param vpTag Tag for previous key operation.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbIx::GetPrevKey( void *vpTag ){

  return GetPrevKey( vpTag, 0 );
}

/***********************************************************************/
//! @brief Get the index type.
/*!
  \returns MDX, NDX or TDX.
*/

const xbString &xbIx::GetType() const {
  return sIxType;
}


/***********************************************************************/
//! @brief Free an index node
/*!
  \param ixNode Pointer to index node to free.
  \returns void
*/
void xbIx::NodeFree( xbIxNode *ixNode ){
  if( ixNode ){
    if( ixNode->cpBlockData ){
      free( ixNode->cpBlockData );
      ixNode->cpBlockData = NULL;
    }
    free( ixNode );
    ixNode = NULL;
  }
}
/***********************************************************************/
//! @brief Open an index file.
/*!
  MDX files are opened automatically and don't need opened.
  NDX files that are associated with the DBF file are opened automatically.

  Non production indexes that haven't been opened will need to be opened to be used.
  \param sFileName Index file name to open.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbIx::Open( const xbString & sFileName ){

  // There are no locking requirements when opening an NDX index
  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;

  try{
    /* copy the file name to the class variable */
    this->SetFileName( sFileName );

    if( !FileExists()){
      iErrorStop = 100;
      iRc = XB_FILE_NOT_FOUND;
      throw iRc;
    }
    /* open the file */
    if(( iRc = xbFopen( dbf->GetOpenMode(), dbf->GetShareMode())) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }
    if(( iRc = ReadHeadBlock()) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw iRc;
    }
    SetCurTag( (xbInt16) 0 );     // default the first tag as the current tag
  }

  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIx::Open( %s ) Exception Caught. Error Stop = [%d] iRc = [%d]", sFileName.Str(), iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}
/***********************************************************************/
//! @brief Set the current tag.
/*!
  \param vpCurTag Pointer to tag to set as current.
  \returns void
*/
void xbIx::SetCurTag( void *vpCurTag ){
  this->vpCurTag = vpCurTag;
}
/***********************************************************************/
//! @brief Set the dbf pointer.
/*!
  \param dbf Dbf pointer to set.
  \returns void
*/
void xbIx::SetDbf( xbDbf *dbf ){
  this->dbf = dbf;
}
/***********************************************************************/
//! @brief Set the file lock status.
/*!
  \param bLocked xbTrue - Set to locked.<br>xbFalse - Set to unlocked.
  \returns void
*/
void xbIx::SetLocked( xbBool bLocked ){
  this->bLocked = bLocked;
}
/***********************************************************************/
}              /* namespace       */
#endif         /*  XB_INDEX_SUPPORT */
