/*  xblnknod.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/


#ifndef __XB_XBLNKNOD_H__
#define __XB_XBLNKNOD_H__



#ifdef XB_LINKLIST_SUPPORT

namespace xb{

template<class xbNodeType>
class XBDLLEXPORT xbLinkListNode {
  public:
   xbLinkListNode( const xbNodeType & );
   xbLinkListNode( const xbNodeType &, const xbString & );
   xbNodeType GetKey() const;
   xbString   &GetData();
   xbLinkListNode<xbNodeType> *GetNextNode() const;
   xbLinkListNode<xbNodeType> *GetPrevNode() const;
   void       SetNextNode( xbLinkListNode<xbNodeType> *llNext );
   void       SetPrevNode( xbLinkListNode<xbNodeType> *llPrev );

  private:
   xbNodeType     ntKey;
   xbString       sData;
   xbLinkListNode *llNext;
   xbLinkListNode *llPrev;
};

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

}               // namespace
#endif          // XB_LINKLIST_SUPPORT
#endif          // XB_XBLNKNOD_H__


