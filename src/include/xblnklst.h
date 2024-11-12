/*  xblnklst.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2024 Gary A Kunkel

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

//! @brief xbLinkList class.
/*!
<ul>
<li>Template based class, supports a list of any object type
<li>Supports additions at the beginning or end of the list
<li>Support deletions at the beginning or end of the list
<li>Can be used to support a <em>stack data structure</em> by always adding to the end and removing from the end
<li>xbLLN is a linked list node of type xbNodeType
<li>Class is available if XB_LINKLIST_SUPPORT is compiled in library
</ul>
*/


class XBDLLEXPORT xbLinkList {
  public:

   //! @brief Constructor
   xbLinkList();

   //! @brief Destructor
   ~xbLinkList();

   //! @brief Return head node from linked list
   xbLinkListNode<xbNodeType> *GetHeadNode() const;

   //! @brief Return end node from linked list
   xbLinkListNode<xbNodeType> *GetEndNode() const;

   //! @brief Return node for specified node number
   /*!
     @param ulNodeNo Number of requested node
     @returns Requested node or NULL
   */
   xbLinkListNode<xbNodeType> *GetNodeForNo( xbUInt32 ulNodeNo ) const;

   //! @brief Clear the linked list
   void Clear();

   //! @brief Return the total number of nodes in the linked list
   xbUInt32 GetNodeCnt() const;

   /*! 
     @brief Insert a node at the end of the list2
     @param xbLLN Linked list node to append to end of list
     @param *np  Output - pointer to pointer of added node
     @returns XB_NO_ERROR or XB_NO_MEMORY
   */
   xbInt16 InsertAtEnd( const xbNodeType & xbLLN, xbLinkListNode<xbNodeType> **np  );


   /*! 
     @brief Insert a node at the end of the list
     @param xbLLN Linked list node to append to end of list
     @returns XB_NO_ERROR or XB_NO_MEMORY
   */
   xbInt16 InsertAtEnd( const xbNodeType &xbLLN );


   /*! 
     @brief Insert a node at the beginning of the list
     @param xbLLN Linked list node to insert at front of list
     @returns XB_NO_ERROR or XB_NO_MEMORY
   */
   xbInt16 InsertAtFront( const xbNodeType &xbLLN );

   /*!
     @brief Remove the first node found for specified value and return where it was found in the list.
     @param ntKey Key to find and remove.
     @param ulNodeOut Output node number of where the node was found before removal (0 based).
     @returns XB_NOT_FOUND or XB_NO_ERROR if successful.
   */
   xbInt16 RemoveByVal( const xbNodeType &ntKey, xbUInt32 &ulNodeOut );

   /*!
     @brief Remove the first node found for specified value.
     @param ntKey Key to find and remove.
     @returns XB_NOT_FOUND or XB_NO_ERROR if successful.
   */
   xbInt16 RemoveByVal( const xbNodeType &ntKey );

   //! @brief Remove node from end of list
   xbInt16 RemoveFromEnd();

   /*! 
     @brief Remove node from end of list, returning node
     @param xbLLN Output - removed node from end of list
     @returns XB_NO_ERROR or XB_INVALID_NODELINK
   */
   xbInt16 RemoveFromEnd( xbNodeType &xbLLN );

   /*!
     @brief Remove node from front f list, returning node
     @param xbLLN Output - removed node from front of list
     @returns XB_NO_ERROR or XB_INVALID_NODELINK
   */
   xbInt16 RemoveFromFront( xbNodeType &xbLLN );

   /*!
     @brief Search list for a given key value.
     @param ntKey Key value to search for.
     @returns XB_NOT_FOUND or XB_NO_ERROR if found
   */
   xbInt16 SearchFor( const xbNodeType &ntKey );

   /*!
     @brief Search list for given key value and return placement in list if found.
     @param ntKey Key value to search for.
     @param ulNodeOut Output - placement in the linked list where found (0 based).
     @returns XB_NOT_FOUND or XB_NO_ERROR if found.
   */
   xbInt16 SearchFor( const xbNodeType &ntKey, xbUInt32 &ulNodeOut );

  private:
   xbUInt32 ulNodeCnt;
   xbLinkListNode<xbNodeType> *llStartPtr;
   xbLinkListNode<xbNodeType> *llEndPtr;
};



/// @cond
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
xbInt16 xbLinkList<xbNodeType>::RemoveByVal( const xbNodeType & ntKey, xbUInt32 &ulNodeNoOut ){
  // Remove the first instance of ntKey from the node chain
  xbLinkListNode<xbNodeType> *currPtr = llStartPtr;
  xbLinkListNode<xbNodeType> *prevPtr = NULL;
  ulNodeNoOut = 0;

  for( xbUInt32 i = 0; i < ulNodeCnt; i++ ){
    if( currPtr->GetKey() == ntKey ){
      if( prevPtr == NULL ){                   //then this is the first node
        llStartPtr = currPtr->GetNextNode();
        if( llStartPtr )                      // if more than one link in the linked list
          llStartPtr->SetPrevNode( NULL );
        delete currPtr;
        ulNodeCnt--;
        ulNodeNoOut = i;
        return XB_NO_ERROR;
        //return i + 1;
      }
      else {
        prevPtr->SetNextNode( currPtr->GetNextNode());
        if( currPtr->GetNextNode())
          currPtr->GetNextNode()->SetPrevNode( prevPtr );
        delete currPtr;
        ulNodeCnt--;
        ulNodeNoOut = i;
        return XB_NO_ERROR;
        // return i + 1;
      }
    }
    prevPtr = currPtr;
    currPtr = currPtr->GetNextNode();
  }
  return XB_NOT_FOUND;
}

template<class xbNodeType>
xbInt16 xbLinkList<xbNodeType>::RemoveByVal( const xbNodeType & ntKey ){

  xbUInt32 ulNodeNo;
  return RemoveByVal( ntKey, ulNodeNo );
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

  xbUInt32 ulNodeNo;
  return SearchFor( ntKey, ulNodeNo );
}

template<class xbNodeType>
xbInt16 xbLinkList<xbNodeType>::SearchFor( const xbNodeType & ntKey, xbUInt32 &ulNodeOut ){

  ulNodeOut = 0;
  xbLinkListNode<xbNodeType> *cPtr = llStartPtr;
  for( xbUInt32 i = 0; i < ulNodeCnt; i++ ){
    if( cPtr->GetKey() == ntKey ){
      ulNodeOut = i;
      return XB_NO_ERROR;
    }
    cPtr = cPtr->GetNextNode();
  }
  return XB_NOT_FOUND;
}


/// @endcond

}               // namespace


#endif          // XB_LINKLIST_SUPPORT
#endif          // __XB_XBLNKLST_H__


