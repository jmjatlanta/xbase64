/*  xblnklstord.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,2019,2022 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/


// Ordered link list



#ifndef __XB_XBLNKLSTORD_H__
#define __XB_XBLNKLSTORD_H__

#ifdef XB_LINKLIST_SUPPORT


namespace xb{


template<class xbNodeType>
class XBDLLEXPORT xbLinkListOrd {
  public:
   xbLinkListOrd();
   ~xbLinkListOrd();

   void    Clear();
   xbLinkListNode<xbNodeType> *GetHeadNode() const;
   xbLinkListNode<xbNodeType> *GetEndNode() const;
   xbLinkListNode<xbNodeType> *GetNodeForKey( const xbString &sKey ) const;

   xbInt16  GetDataForKey  ( const xbNodeType &ntKey, xbString &sData );

   xbBool   GetDupKeys     ();

   xbUInt32 GetNodeCnt     () const;
   xbUInt32 GetNodeCnt     ( const xbString &sNodeKey ) const;
   xbInt16  InsertKey      ( const xbNodeType &ntKey );
   xbInt16  InsertKey      ( const xbNodeType &ntKey, const xbString &sData );
   xbInt16  InsertKey      ( const xbNodeType &ntKey, xbUInt32 ulData );

   xbBool   KeyExists      ( const xbNodeType &ntKey ) const;
   xbInt16  RemoveKey      ( const xbNodeType &ntKey );
   xbInt16  RemoveFromEnd  ( xbNodeType &ntKey );
   xbInt16  RemoveFromFront( xbNodeType &ntKey );
   xbInt16  RemoveFromFront();
   void     SetDupKeys     ( xbBool bAllowDupKeys );
   xbInt16  UpdateForKey   ( const xbNodeType &ntKey, const xbString &sData );
 

  private:
   xbUInt32   ulNodeCnt;
   xbBool     bAllowDupKeys;
   xbLinkListNode<xbNodeType> *llStartPtr;
   xbLinkListNode<xbNodeType> *llEndPtr;

};


template<class xbNodeType>
xbLinkListOrd<xbNodeType>::xbLinkListOrd(){
  bAllowDupKeys = xbTrue;             // default setting - allow duplicate keys
  ulNodeCnt     = 0;
  llStartPtr    = NULL;
  llEndPtr      = NULL;
}

template<class xbNodeType>
xbLinkListOrd<xbNodeType>::~xbLinkListOrd(){
  Clear();
}

template<class xbNodeType>
void xbLinkListOrd<xbNodeType>::Clear(){
  xbLinkListNode<xbNodeType> *cPtr = llStartPtr, *tPtr;
  for( xbUInt32 i = 0; i < ulNodeCnt; i++ ){
    tPtr = cPtr;
    cPtr = cPtr->GetNextNode();

    // next line might cause seg faults
    //   delete tPtr->GetData();

    delete tPtr;
  }
  ulNodeCnt = 0;
  llStartPtr = NULL;
  llEndPtr   = NULL;
}

template<class xbNodeType>
xbLinkListNode<xbNodeType> * xbLinkListOrd<xbNodeType>::GetHeadNode() const{
  return llStartPtr;
}

template<class xbNodeType>
xbLinkListNode<xbNodeType> * xbLinkListOrd<xbNodeType>::GetEndNode() const{
  return llEndPtr;
}

template<class xbNodeType>
xbUInt32 xbLinkListOrd<xbNodeType>::GetNodeCnt() const{
  return ulNodeCnt;
}

template<class xbNodeType>
xbUInt32 xbLinkListOrd<xbNodeType>::GetNodeCnt( const xbString &sNodeKey ) const{

  // won't work if nodekey is not a string
  xbLinkListNode<xbNodeType> *currPtr = llStartPtr;
  // skip to sNodeKey
  while( currPtr && ( sNodeKey > currPtr->GetKey())) {
    currPtr = currPtr->GetNextNode();
  }
  // count entries for sNodeKey
  xbInt16 iKeyCnt = 0;
  while( currPtr && ( sNodeKey == currPtr->GetKey())) {
    iKeyCnt++;
    currPtr = currPtr->GetNextNode();
  }
  return iKeyCnt;
}


template<class xbNodeType>
xbInt16 xbLinkListOrd<xbNodeType>::InsertKey( const xbNodeType &ntKey ){
  xbString s;
  return InsertKey( ntKey, s );
}




template<class xbNodeType>
xbInt16 xbLinkListOrd<xbNodeType>::InsertKey( const xbNodeType &ntKey, xbUInt32 ul ){

  xbString s;
  s.Sprintf( "%ld", ul );
  return InsertKey( ntKey, s );
}


template<class xbNodeType>
xbInt16 xbLinkListOrd<xbNodeType>::InsertKey( const xbNodeType &ntKey, const xbString &sData  ){

  xbLinkListNode<xbNodeType> *p = new xbLinkListNode<xbNodeType>( ntKey, sData );
  if( p == 0 )
    return XB_NO_MEMORY;

  if( ulNodeCnt > 0 ){
    xbLinkListNode<xbNodeType> *currPtr = llStartPtr;
    xbLinkListNode<xbNodeType> *prevPtr = NULL;

    // find location in the chain
    while( currPtr && ntKey > currPtr->GetKey() ){
      prevPtr = currPtr;
      currPtr = currPtr->GetNextNode();
    }
    if( currPtr && ntKey == currPtr->GetKey() && bAllowDupKeys == 0 ){
      delete p;
      return XB_KEY_NOT_UNIQUE;
    }

    if( currPtr == NULL ){
      // std::cout << "at the end of the chain\n";
      llEndPtr = p;
      prevPtr->SetNextNode( p );
      p->SetPrevNode( prevPtr );

    } else if( currPtr->GetPrevNode() == NULL ){
      // std::cout << "at the beginning of the chain\n";
      p->SetNextNode( llStartPtr );
      llStartPtr->SetPrevNode( p );
      llStartPtr = p;

    } else {
      // std::cout << "in the middle of the chain\n";
      p->SetNextNode( currPtr ); 
      p->SetPrevNode( currPtr->GetPrevNode());
      currPtr->SetPrevNode( p );
      prevPtr->SetNextNode( p );
    }
  } else {
    // std::cout << "first addition to the chain\n";
    llStartPtr = p;
    llEndPtr = p;
  }
  ulNodeCnt++;
  return XB_NO_ERROR;
}

template<class xbNodeType>
xbInt16 xbLinkListOrd<xbNodeType>::RemoveKey( const xbNodeType &ntKey ){
  // Remove the first instance of ntKey from the node chain
  xbLinkListNode<xbNodeType> *currPtr = llStartPtr;
  xbLinkListNode<xbNodeType> *prevPtr = NULL;

  while( currPtr && ntKey > currPtr->GetKey() ){
    prevPtr = currPtr;
    currPtr = currPtr->GetNextNode();
  }

  if( currPtr && ntKey == currPtr->GetKey()){
//    ntKey = currPtr->GetKey();
    if( prevPtr == NULL ){                          // this is the first node
      llStartPtr = currPtr->GetNextNode();
      // next line fails
      if( llStartPtr ){
        llStartPtr->SetPrevNode( NULL );
      }
      delete currPtr;
      ulNodeCnt--;
      return XB_NO_ERROR;
    } else if( currPtr->GetNextNode() == NULL ){    // this is the last node
      llEndPtr = prevPtr;
      prevPtr->SetNextNode( NULL );
      delete currPtr;
      ulNodeCnt--;
      return XB_NO_ERROR;
    } else {

      prevPtr->SetNextNode( currPtr->GetNextNode());
      currPtr->GetNextNode()->SetPrevNode( prevPtr );
      delete currPtr;
      ulNodeCnt--;
      return XB_NO_ERROR;
    }
  } else {
    return XB_NOT_FOUND;
  }
}

template<class xbNodeType>
xbInt16 xbLinkListOrd<xbNodeType>::RemoveFromFront( xbNodeType &ntKey ){

  if( ulNodeCnt <= 0 )
    return XB_INVALID_NODELINK;
  xbLinkListNode<xbNodeType> *p = llStartPtr;
  llStartPtr = p->GetNextNode();
  if( llStartPtr )
    llStartPtr->SetPrevNode( NULL );
  ntKey = p->GetKey();
  delete p;
  ulNodeCnt--;
  return XB_NO_ERROR;
}

template<class xbNodeType>
xbInt16 xbLinkListOrd<xbNodeType>::RemoveFromFront(){

  if( ulNodeCnt <= 0 )
    return XB_INVALID_NODELINK;
  xbLinkListNode<xbNodeType> *p = llStartPtr;
  llStartPtr = p->GetNextNode();
  if( llStartPtr )
    llStartPtr->SetPrevNode( NULL );

  if( p->GetKey())
    delete p->GetKey();

  delete p;
  ulNodeCnt--;

  return XB_NO_ERROR;
}


template<class xbNodeType>
xbInt16 xbLinkListOrd<xbNodeType>::RemoveFromEnd( xbNodeType &ntKey ){

  if( ulNodeCnt <= 0 )
    return XB_INVALID_NODELINK;
  xbLinkListNode<xbNodeType> *p = llEndPtr;
  llEndPtr = p->GetPrevNode();
  llEndPtr->SetNextNode( NULL );
  ntKey = p->GetKey();
  delete p;
  ulNodeCnt--;
  return XB_NO_ERROR;
}

template<class xbNodeType>
xbBool xbLinkListOrd<xbNodeType>::GetDupKeys(){
  return bAllowDupKeys;
}

template<class xbNodeType>
void xbLinkListOrd<xbNodeType>::SetDupKeys( xbBool bAllowDupKeys ){
  this->bAllowDupKeys = bAllowDupKeys;
}


template<class xbNodeType>
xbBool xbLinkListOrd<xbNodeType>::KeyExists( const xbNodeType &ntKey ) const {

  xbLinkListNode<xbNodeType> *currPtr = llStartPtr;
  while( currPtr && ntKey > currPtr->GetKey() ){
    currPtr = currPtr->GetNextNode();
  }
  if( currPtr && ntKey == currPtr->GetKey()){
    return xbTrue;
  } else {
    return xbFalse;
  }
}


template<class xbNodeType>
xbInt16 xbLinkListOrd<xbNodeType>::GetDataForKey( const xbNodeType &ntKey, xbString &sData ){

  xbLinkListNode<xbNodeType> *currPtr = llStartPtr;
  while( currPtr && ntKey > currPtr->GetKey() ){
    currPtr = currPtr->GetNextNode();
  }

  if( currPtr && ntKey == currPtr->GetKey()){
    sData = currPtr->GetData();
    return XB_NO_ERROR;
  } else {
    return XB_NOT_FOUND;
  }
}


template<class xbNodeType>
xbInt16 xbLinkListOrd<xbNodeType>::UpdateForKey( const xbNodeType &ntKey, const xbString &sData ){
 
  if( ulNodeCnt == 0 )
    return InsertKey( ntKey, sData );
  xbLinkListNode<xbNodeType> * currPtr = llStartPtr;
  xbLinkListNode<xbNodeType> * prevPtr = NULL;
  while( currPtr && ntKey > currPtr->GetKey() ) {
    prevPtr = currPtr;
    currPtr = currPtr->GetNextNode();
  }

  if( currPtr && ntKey == currPtr->GetKey() ) {
    xbLinkListNode<xbNodeType> *p = new xbLinkListNode<xbNodeType>( ntKey, sData );
    if( prevPtr )
      prevPtr->SetNextNode( p );
    else
      llStartPtr = p;
    p->SetNextNode( currPtr->GetNextNode() );
    p->SetPrevNode( currPtr->GetPrevNode() );
    delete currPtr;
    return XB_NO_ERROR;
  }
  
  return InsertKey( ntKey, sData );

// return 0;
}

}               // namespace

#endif          // XB_LINKLIST_SUPPORT
#endif          // XB_XBLNKLSTORD_H__


