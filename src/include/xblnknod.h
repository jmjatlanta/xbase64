/*  xblnknod.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,20222,2024 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

#ifndef __XB_XBLNKNOD_H__
#define __XB_XBLNKNOD_H__


#ifdef XB_LINKLIST_SUPPORT

namespace xb{

//! @brief xbLinkNode class.
/*!
This class provides a template for defining nodes that are used by the 
xbLinkList and xbLinkListOrd classes.

Nodes can be used for storing any data type, and then linked together with 
one of the linked list classes.

*/


template<class xbNodeType>
class XBDLLEXPORT xbLinkListNode {
  public:
   /*!
      @brief Constructor
      @param nVal Data value stored on this node
   */
   xbLinkListNode( const xbNodeType & nVal );
 
   /*! 
      @brief Constructor
      @param nVal Data value stored on this node.  Used as the sort key in the ordered link list.
      @param sVal User value. Optional string attached to the node.
   */
   xbLinkListNode( const xbNodeType & nVal, const xbString & sVal );

   /*!
      @brief Return the node data value.
   */
   xbNodeType GetKey() const;

   /*! 
      @brief Return optional user defined string value for node.
      @returns User defined xbString
   */
   xbString &GetData();

   /*!
      @brief Return pointer to next node
      @returns Pointer to next node or NULL if last node in list
   */
   xbLinkListNode<xbNodeType> *GetNextNode() const;

   /*!
      @brief Return pointer to previous node
      @returns Pointer to previous node or NULL if last node in list
   */
   xbLinkListNode<xbNodeType> *GetPrevNode() const;

   /*!
      @brief Set next pointer
      @private
   */
   void SetNextNode( xbLinkListNode<xbNodeType> *llNext );

   /*!
      @brief Set previous pointer
      @private
   */
   void SetPrevNode( xbLinkListNode<xbNodeType> *llPrev );

  private:
   xbNodeType     ntKey;
   xbString       sData;
   xbLinkListNode *llNext;
   xbLinkListNode *llPrev;
};

/// @cond

 template<class xbNodeType>
 xbLinkListNode<xbNodeType>::xbLinkListNode( const xbNodeType &key ){
   ntKey  = key;
   llNext = NULL;
   llPrev = NULL;
 }

 template<class xbNodeType>
 xbLinkListNode<xbNodeType>::xbLinkListNode( const xbNodeType &key, const xbString &s ){
   ntKey  = key;
   sData  = s;
   llNext = NULL;
   llPrev = NULL;
 }

 template<class xbNodeType>
 xbNodeType xbLinkListNode<xbNodeType>::GetKey() const {
   return ntKey;
 }

 template<class xbNodeType>
 xbString &xbLinkListNode<xbNodeType>::GetData(){
   return sData;
 }

 template<class xbNodeType>
 xbLinkListNode<xbNodeType> *xbLinkListNode<xbNodeType>::GetNextNode() const {
   return llNext;
 }

 template<class xbNodeType>
 xbLinkListNode<xbNodeType> *xbLinkListNode<xbNodeType>::GetPrevNode() const {
   return llPrev;
 }

 template<class xbNodeType>
 void xbLinkListNode<xbNodeType>::SetNextNode( xbLinkListNode<xbNodeType> *lln ){
   llNext = lln;
 }

 template<class xbNodeType>
 void xbLinkListNode<xbNodeType>::SetPrevNode( xbLinkListNode<xbNodeType> *llp ){
   llPrev = llp;
 }
/// @endcond
}               // namespace
#endif          // XB_LINKLIST_SUPPORT
#endif          // XB_XBLNKNOD_H__


