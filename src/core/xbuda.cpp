/*  xbuda.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

This module handles uda (user data area) methods 

*/

#include "xbase.h"


// might need to change thisto XB_EXPRESSION_SUPPORT
#ifdef XB_EXPRESSION_SUPPORT


namespace xb{

/************************************************************************/
xbUda::xbUda() {
  llOrd. SetDupKeys( xbFalse );
}
/************************************************************************/
xbUda::~xbUda() {}
/************************************************************************/
void xbUda::Clear() {
  llOrd.Clear();
}
/************************************************************************/
xbInt16 xbUda::GetTokenCnt() const {
  return llOrd.GetNodeCnt();
}
/************************************************************************/

xbInt16 xbUda::GetTokenForKey( const xbString &sKey, xbString &sToken ) {
  return llOrd.GetDataForKey( sKey, sToken );
}

/************************************************************************/
xbInt16 xbUda::AddTokenForKey( const xbString &sKey, const xbString &sToken ) {
  return llOrd.InsertKey( sKey, sToken );
}
/************************************************************************/

xbInt16 xbUda::UpdTokenForKey( const xbString &sKey, const xbString &sData ) {
  return llOrd.UpdateForKey( sKey, sData );
}

/************************************************************************/
xbInt16 xbUda::DelTokenForKey( const xbString &sKey ) {
  return llOrd.RemoveKey( sKey );  //, sToken );
}


/************************************************************************/

void xbUda::DumpUda() const{

  xbLinkListNode<xbString> *lln = llOrd.GetHeadNode();

  xbInt32 l = 0;
  while( lln ){
    std::cout << ++l << " Key=[" << lln->GetKey() << "] Data=[" << lln->GetData() << "]" << std::endl;
    lln = lln->GetNextNode();
  }
}

/************************************************************************/
}   /* namespace */ 
#endif         /*  XB_EXPRESSION_SUPPORT */