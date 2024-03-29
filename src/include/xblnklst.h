/*  xblnklst.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/


#ifndef __XB_XBLNKLST_H__
#define __XB_XBLNKLST_H__

#ifdef XB_LINKLIST_SUPPORT

namespace xb{


template<class xbNodeType>
class XBDLLEXPORT xbLinkList {
  public:
   xbLinkList();
   ~xbLinkList();

   xbLinkListNode<xbNodeType> *GetHeadNode() const;
   xbLinkListNode<xbNodeType> *GetEndNode() const;
   xbLinkListNode<xbNodeType> *GetNodeForNo( xbUInt32 ulNodeNo ) const;

   void        Clear();
   xbUInt32    GetNodeCnt     () const;
   xbInt16     InsertAtEnd    ( const xbNodeType &xbLLN );
   xbInt16     InsertAtEnd    ( const xbNodeType & ntKey, xbLinkListNode<xbNodeType> **np  );

   xbInt16     InsertAtFront  ( const xbNodeType &xbLLN );
   xbInt16     RemoveByVal    ( const xbNodeType &xbLLN );
   xbInt16     RemoveFromEnd  ();
   xbInt16     RemoveFromEnd  ( xbNodeType &xbLLN );
   xbInt16     RemoveFromFront( xbNodeType &xbLLN );
   xbInt16     SearchFor      ( const xbNodeType &xbLLN );

  private:
   xbUInt32 ulNodeCnt;
   xbLinkListNode<xbNodeType> *llStartPtr;
   xbLinkListNode<xbNodeType> *llEndPtr;
};

template<class xbNodeType>
xbLinkList<xbNodeType>::xbLinkList(){
  ulNodeCnt  = 0;
  llStartPtr = NULL;
  llEndPtr   = NULL;
}

template<class xbNodeType>
xbLinkList<xbNodeType>::~xbLinkList(){
  Clear();
}

template<class xbNodeType>
void xbLinkList<xbNodeType>::Clear(){
  xbLinkListNode<xbNodeType> *cPtr = llStartPtr, *tPtr;
  for( xbUInt32 i = 0; i < ulNodeCnt; i++ ){
    tPtr = cPtr;
    cPtr = cPtr->GetNextNode();
    delete tPtr;
  }
  ulNodeCnt  = 0;
  llStartPtr = NULL;
  llEndPtr   = NULL;
}


template<class xbNodeType>
xbLinkListNode<xbNodeType> *xbLinkList<xbNodeType>::GetHeadNode() const{
  return llStartPtr;
}

template<class xbNodeType>
xbLinkListNode<xbNodeType> *xbLinkList<xbNodeType>::GetEndNode() const{
  return llEndPtr;
}


template<class xbNodeType>
xbLinkListNode<xbNodeType> *xbLinkList<xbNodeType>::GetNodeForNo( xbUInt32 ulNo ) const{

  xbLinkListNode<xbNodeType> *cPtr = llStartPtr;
  xbUInt32 i;
  for( i = 0; i < ulNo && i < ulNodeCnt; i++ )
    cPtr = cPtr->GetNextNode();

  if( i == ulNo )
    return cPtr;
  else 
    return 0;
}



template<class xbNodeType>
xbUInt32 xbLinkList<xbNodeType>::GetNodeCnt() const{
  return ulNodeCnt;
}

template<class xbNodeType>
xbInt16 xbLinkList<xbNodeType>::InsertAtFront( const xbNodeType & ntKey ){

  xbLinkListNode<xbNodeType> *p = new xbLinkListNode<xbNodeType>( ntKey );
  if( p == 0 )
    return XB_NO_MEMORY;

  if( ulNodeCnt > 0 ){
    llStartPtr->SetPrevNode( p );
    p->SetNextNode( llStartPtr );
  } else {
    llEndPtr = p;
  }

  llStartPtr = p;
  ulNodeCnt++;
  return XB_NO_ERROR;
}

template<class xbNodeType>
xbInt16 xbLinkList<xbNodeType>::InsertAtEnd( const xbNodeType & ntKey ){

  xbLinkListNode<xbNodeType> *p = new xbLinkListNode<xbNodeType>( ntKey );
  if( p == 0 )
    return XB_NO_MEMORY;

  if( ulNodeCnt > 0 ){
    llEndPtr->SetNextNode( p );
    p->SetPrevNode( llEndPtr );
    llEndPtr = p;
  } else {
    llStartPtr = p;
  }

  llEndPtr = p;
  ulNodeCnt++;
  return XB_NO_ERROR;
}


template<class xbNodeType>
xbInt16 xbLinkList<xbNodeType>::InsertAtEnd( const xbNodeType & ntKey, xbLinkListNode<xbNodeType> **np  ){

  xbLinkListNode<xbNodeType> *p = new xbLinkListNode<xbNodeType>( ntKey );
  if( p == 0 )
    return XB_NO_MEMORY;

  *np = p;
  if( ulNodeCnt > 0 ){
    llEndPtr->SetNextNode( p );
    p->SetPrevNode( llEndPtr );
    llEndPtr = p;
  } else {
    llStartPtr = p;
  }

  llEndPtr = p;
  ulNodeCnt++;
  return XB_NO_ERROR;
}



template<class xbNodeType>
xbInt16 xbLinkList<xbNodeType>::RemoveByVal( const xbNodeType & ntKey ){
  // Remove the first instance of ntKey from the node chain
  xbLinkListNode<xbNodeType> *currPtr = llStartPtr;
  xbLinkListNode<xbNodeType> *prevPtr = NULL;

  for( xbUInt32 i = 0; i < ulNodeCnt; i++ ){
    if( currPtr->GetKey() == ntKey ){
      if( prevPtr == NULL ){                   //then this is the first node
        llStartPtr = currPtr->GetNextNode();
        if( llStartPtr )                      // if more than one link in the linked list
          llStartPtr->SetPrevNode( NULL );
        delete currPtr;
        ulNodeCnt--;
        return i + 1;
      }
      else {
        prevPtr->SetNextNode( currPtr->GetNextNode());
        if( currPtr->GetNextNode())
          currPtr->GetNextNode()->SetPrevNode( prevPtr );
        delete currPtr;
        ulNodeCnt--;
        return i + 1;
      }
    }
    prevPtr = currPtr;
    currPtr = currPtr->GetNextNode();
  }
  return XB_NOT_FOUND;
}


template<class xbNodeType>
xbInt16 xbLinkList<xbNodeType>::RemoveFromFront( xbNodeType & ntKey ){

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
xbInt16 xbLinkList<xbNodeType>::RemoveFromEnd( xbNodeType & ntKey ){

  if( ulNodeCnt <= 0 )
    return XB_INVALID_NODELINK;
  xbLinkListNode<xbNodeType> *p = llEndPtr;
  if( p->GetPrevNode()){
    llEndPtr = p->GetPrevNode();
    llEndPtr->SetNextNode( NULL );
  } else {
    // there are no more nodes
    llStartPtr = NULL;
    llEndPtr   = NULL;
  }
  ntKey = p->GetKey();
  delete p;
  ulNodeCnt--;
  return XB_NO_ERROR;
}


template<class xbNodeType>
xbInt16 xbLinkList<xbNodeType>::RemoveFromEnd(){

  if( ulNodeCnt <= 0 )
    return XB_INVALID_NODELINK;
  xbLinkListNode<xbNodeType> *p = llEndPtr;
  if( p->GetPrevNode()){
    llEndPtr = p->GetPrevNode();
    llEndPtr->SetNextNode( NULL );
  } else {
    // there are no more nodes
    llStartPtr = NULL;
    llEndPtr   = NULL;
  }
  delete p->GetKey();
  delete p;
  ulNodeCnt--;
  return XB_NO_ERROR;
}


template<class xbNodeType>
xbInt16 xbLinkList<xbNodeType>::SearchFor( const xbNodeType & ntKey ){

  xbLinkListNode<xbNodeType> *cPtr = llStartPtr;
  for( xbUInt32 i = 0; i < ulNodeCnt; i++ ){
    if( cPtr->GetKey() == ntKey )
      return i+1;
    cPtr = cPtr->GetNextNode();
  }
  return XB_NO_ERROR;

}
}               // namespace

#endif          // XB_LINKLIST_SUPPORT
#endif          // XB_XBLNKLST_H__


