/*  xblnklstord.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,2019,2022,2024 Gary A Kunkel

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

//! @brief xbLinkListOrd class.
/*!
<ul>
<li>Template based class, supports an ordered list of any object type
<li>Items are placed in the list based on key value
<li>xbLLN is a linked list node of type xbNodeType
<li>Class is available if XB_LINKLIST_SUPPORT is compiled in library
</ul>
*/


template<class xbNodeType>
class XBDLLEXPORT xbLinkListOrd {
  public:
   /*!
      @brief Constructor
   */
   xbLinkListOrd();
 
   /*!
      @brief Destructor
   */
   ~xbLinkListOrd();

   /*!
      @brief Clear the linked list and free memory
   */
   void    Clear();

   /*!
      @brief Return the user string for a given key value
      @param ntKey Key value to search on
      @param sData Output - xbString for found node
      @returns XB_NO_ERROR or XB_NOT_FOUND
   */
   xbInt16 GetDataForKey( const xbNodeType &ntKey, xbString &sData );

   /*!
      @brief Return dup keys setting
      @returns xbTrue Duplicate keys allowed<br>
               xbFalse Duplicate keys not allowed
   */
   xbBool GetDupKeys();

   /*!
      @brief Return the end node of the list
      @returns Reference to the end node
   */
   xbLinkListNode<xbNodeType> *GetEndNode() const;

   /*!
      @brief Return the head node of the list
      @returns Reference to the head node
   */
   xbLinkListNode<xbNodeType> *GetHeadNode() const;


   /*!
      @brief Get the total node count in the linked list
      @returns Node count
   */
   xbUInt32 GetNodeCnt() const;

   /*!
      @brief Get the total node count in the linked list for a given key
      @returns Node count
      @note Current version only works if for node type of strings.
             Feel free to expand this and send in your updates for inclusion in the library
   */
   xbUInt32 GetNodeCnt( const xbString &sNodeKey ) const;

   /*!
      @brief Insert a node into the list
      @param ntKey Node with data
      @return XB_NO_ERROR, XB_NO_MEMORY or XB_KEY_NOT_UNIQUE
      @note Library inserts key into correct location
   */
   xbInt16 InsertKey( const xbNodeType &ntKey );

   /*!
      @brief Insert a node into the list with user string
      @param ntKey Node with data
      @param sData User defined string data
      @return XB_NO_ERROR, XB_NO_MEMORY or XB_KEY_NOT_UNIQUE
      @note Library inserts key into correct location
   */
   xbInt16 InsertKey( const xbNodeType &ntKey, const xbString &sData );

   /*!
      @brief Insert a node into the list with user numeric data
      @param ntKey Node with data
      @param ulData User defined numeric data
      @return XB_NO_ERROR, XB_NO_MEMORY or XB_KEY_NOT_UNIQUE
      @note Library inserts key into correct location
   */
   xbInt16 InsertKey( const xbNodeType &ntKey, xbUInt32 ulData );

   /*!
      @brief Check list for existence of given key
      @returns xbTrue Key found<br>
               xbFalse Key not found
   */
   xbBool KeyExists( const xbNodeType &ntKey ) const;

   /*!
      @brief Remove node for a given key
      @param ntKey Key value
      @returns XB_NO_ERROR or XB_NOT_FOUND
      @note If multiple nodes with same key, this routine removes the first one
   */
   xbInt16 RemoveKey( const xbNodeType &ntKey );

   /*!
      @brief Remove node from end of linked list
      @param ntKey Output key value in end node
      @returns XB_NO_ERROR or XB_INVALID_NODELINK
   */
   xbInt16 RemoveFromEnd( xbNodeType &ntKey );

   /*!
      @brief Remove node from front of linked list
      @param ntKey Output key value in first node
      @returns XB_NO_ERROR or XB_INVALID_NODELINK
   */
   xbInt16 RemoveFromFront( xbNodeType &ntKey );

   /*!
      @brief Remove node from front of linked list
      @returns XB_NO_ERROR or XB_INVALID_NODELINK
   */
   xbInt16 RemoveFromFront();

   /*!
      @brief Set duplicate key switch
      @param bAllowDupKeys xbTrue Allow duplicate keys in linked list<br>
                           xbFalse Don't allow duplicate keys in linked list
   */
   void SetDupKeys( xbBool bAllowDupKeys );

   /*!
      @brief Update user string value for a given key
      @param ntKey Key value to retrieve for update
      @param sData New data value to be associated with key
      @returns XB_NO_ERROR or XB_NO_MEMORY
      @note If key is not found, new node is added to the list
   */
   xbInt16 UpdateForKey( const xbNodeType &ntKey, const xbString &sData );

  private:
   xbUInt32   ulNodeCnt;
   xbBool     bAllowDupKeys;
   xbLinkListNode<xbNodeType> *llStartPtr;
   xbLinkListNode<xbNodeType> *llEndPtr;

};

/// @cond

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

/// @endcond

}               // namespace

#endif          // XB_LINKLIST_SUPPORT
#endif          // XB_XBLNKLSTORD_H__


