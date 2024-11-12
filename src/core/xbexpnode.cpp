/*  xbexpnode.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2017,2022,2023,2024 Gary A Kunkel

The xb64 software library is covered under 
the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

#include "xbase.h"

#ifdef XB_EXPRESSION_SUPPORT

namespace xb{
/************************************************************************/
xbExpNode::xbExpNode(){
  sNodeText      = "";
  cReturnType    = 0;
  cNodeType      = 0;
  dResult        = 0;
  iFieldNo       = 0;
  ulResultLen    = 0;
  iWeight        = 0;
  nParent        = NULL;
  dbf            = NULL;
}
/************************************************************************/
xbExpNode::xbExpNode( xbString &sNodeText, char cReturnType, char cNodeType ){
  this->sNodeText   = sNodeText;
  this->cReturnType = cReturnType;
  this->cNodeType   = cNodeType;
  dResult           = 0;
  iFieldNo          = 0;
  ulResultLen       = 0;
  iWeight           = 0;
  nParent           = NULL;
  dbf               = NULL;
}
/************************************************************************/
xbExpNode::xbExpNode( xbString &sNodeText, char cNodeType ){
  this->sNodeText   = sNodeText;
  this->cReturnType = 0x00;
  this->cNodeType   = cNodeType;
  dResult           = 0;
  iFieldNo          = 0;
  ulResultLen       = 0;
  iWeight           = 0;
  nParent           = NULL;
  dbf               = NULL;
}
/************************************************************************/
xbExpNode::xbExpNode( char cNodeType ){
  this->cReturnType = 0x00;
  this->cNodeType   = cNodeType;
  dResult           = 0;
  iFieldNo          = 0;
  ulResultLen       = 0;
  iWeight           = 0;
  nParent           = NULL;
  dbf               = NULL;
}
/************************************************************************/
xbExpNode::~xbExpNode(){
  xbExpNode * n;
  while( llChildren.GetNodeCnt() > 0 ){
    llChildren.RemoveFromFront( n );
    delete n;
  }
}
/************************************************************************/
xbInt16 xbExpNode::AddChild( xbExpNode *n ){
  n->SetParent( this );
  return llChildren.InsertAtEnd( n );
}
/************************************************************************/
#ifdef XB_DEBUG_SUPPORT
void xbExpNode::DumpNode( xbInt16 iOption ) const {
  xbString sMsg;
  std::cout << "xbExpNode::DumpNode() Me=[" << this << "] ";
  if( nParent )
    std::cout << "Par=[" << nParent << "] ";
  std::cout << "nTyp=[" << cNodeType  << "] ";
  std::cout << "dTyp=[" << cReturnType << "] ";
  if( iWeight != 0 )
    std::cout << "W=[" << iWeight << "] ";
  if( cNodeType == XB_EXP_FIELD )
    std::cout << "FieldNo=[" << iFieldNo << "] ";
  std::cout << "Txt=[" << sNodeText << "] ";
  if( sResult != "" )
    std::cout << "sVal=[" << sResult << "] ";
  if( ulResultLen > 0 )
    std::cout << "Len=[" << ulResultLen << "] ";
  if( cReturnType == XB_EXP_DATE ){
    xbDate d( (xbInt32) dResult );
    std::cout << "dtVal=[" << d.Str() << "] ";
  }
  if( cReturnType == XB_EXP_DATE || cReturnType == XB_EXP_NUMERIC ){
    sMsg.Sprintf( "dVal=[%f]\n", dResult );
    std::cout << sMsg.Str();
  }
  if( cReturnType == XB_EXP_LOGICAL ){
    sMsg.Sprintf( "lVal=[%d]\n", (xbInt32) dResult );
    std::cout << sMsg.Str();
  }
  if( iOption ){
    xbLinkListNode<xbExpNode *> *lln = llChildren.GetHeadNode();
    xbExpNode *n;
    if( lln ){
      std::cout << " Children: ";
      while( lln ){
        n = lln->GetKey();
        std::cout << " [" << n << "]";
        lln = lln->GetNextNode();
      }
      std::cout << std::endl;

      lln = llChildren.GetHeadNode();
      while( lln ){
        n = lln->GetKey();
        n->DumpNode( iOption );
        lln = lln->GetNextNode();
      }
    }
  }
  std::cout << std::endl;
}
#endif
/************************************************************************/
xbBool xbExpNode::GetBoolResult() const {
  return (xbBool) dResult;
}
/************************************************************************/
xbExpNode *xbExpNode::GetChild( xbUInt32 ulChildNo ) const {
  xbLinkListNode<xbExpNode *> *lln = llChildren.GetNodeForNo( ulChildNo );
  if( lln )
    return lln->GetKey();
  else
    return 0x00;
}
/************************************************************************/
xbUInt32 xbExpNode::GetChildCnt() const{
   return llChildren.GetNodeCnt();
}
/************************************************************************/
xbUInt32 xbExpNode::GetChildNo() const {
  if( !nParent )
    return 0;
  for( xbUInt32 i = 0; i < nParent->GetChildCnt(); i++ ){
    if( this == nParent->GetChild( i )){
      // std::cout << "get child no [" << this << "][" << nParent->GetChild(i) << "]\n";
      return i;
    }
  }
  return 0;
}
/************************************************************************/
xbDbf *xbExpNode::GetDbf() const {
  return dbf;
}
/************************************************************************/
xbInt16 xbExpNode::GetFieldNo() const {
  return iFieldNo;
}
/*************************************************************************/
xbExpNode *xbExpNode::GetFirstNode() {
  xbExpNode *n = this;
  while( n && n->GetChildCnt() > 0 )
    n = n->GetChild(0);
  return n;
}
/*************************************************************************/
xbExpNode *xbExpNode::GetNextNode() const {
  if( HasRightSibling())
    return GetRightSibling()->GetFirstNode();
  else
    return nParent;
}
/************************************************************************/
void xbExpNode::GetNodeText( xbString &sOutText ) const{
  sOutText = sNodeText;
}
/************************************************************************/
char xbExpNode::GetNodeType() const{
  return cNodeType;
}
/************************************************************************/
xbDouble xbExpNode::GetNumericResult() const {
  return dResult;
}
/************************************************************************/
xbExpNode *xbExpNode::GetParent() const{
  return nParent;
}
/************************************************************************/
xbUInt32 xbExpNode::GetResultLen() const{
  return ulResultLen;
}
/************************************************************************/
char xbExpNode::GetReturnType() const{
  return cReturnType;
}
/*************************************************************************/
xbExpNode *xbExpNode::GetRightSibling() const {

  xbExpNode * nParent;
  if(( nParent = GetParent()) == NULL )
    return NULL;

  if( nParent->GetChildCnt() <= 0 )
    return NULL;

  xbUInt32 ulChildNo = GetChildNo();

  if( ulChildNo < (nParent->GetChildCnt() - 1))
    return nParent->GetChild( ulChildNo + 1 );
  else
    return NULL;
}
/************************************************************************/
xbString &xbExpNode::GetStringResult(){
  return sResult;
}
/************************************************************************/
xbInt16 xbExpNode::GetWeight() const {
  return iWeight;
}
/*************************************************************************/
xbBool xbExpNode::HasRightSibling() const {
  if( nParent == NULL )
    return xbFalse;
  xbUInt32 ulChildNo = GetChildNo();
  if( ulChildNo < (nParent->GetChildCnt() - 1)){
    // std::cout << "Has Right Sibling = " << iChildNo << "] of [" << nParent->GetChildCnt() << "]\n";
    return xbTrue;
}
  else
    return xbFalse;
}
/*************************************************************************/
xbBool xbExpNode::IsOperator() const {

  if( cNodeType == XB_EXP_OPERATOR )
    return xbTrue;
  else
    return xbFalse;
}
/*************************************************************************/
xbBool xbExpNode::IsUnaryOperator() const {

  if( cNodeType == XB_EXP_PRE_OPERATOR || cNodeType == XB_EXP_POST_OPERATOR )
    return xbTrue;
  else if( cNodeType == XB_EXP_OPERATOR && (sNodeText == ".NOT." || sNodeText == "NOT" ))
    return xbTrue;
  else
    return xbFalse;
}
/************************************************************************/
void xbExpNode::RemoveLastChild(){
  xbExpNode *n;
  llChildren.RemoveFromEnd( n );
}
/************************************************************************/
void xbExpNode::SetDbfInfo( xbDbf *dbf, xbInt16 iFieldNo ){
  this->dbf      = dbf;
  this->iFieldNo = iFieldNo;
}

/************************************************************************/
void xbExpNode::SetDbfInfo( xbDbf *dbf ){
  this->dbf      = dbf;
}
/************************************************************************/
void xbExpNode::SetNodeText( xbString & sNodeText ){
  this->sNodeText = sNodeText;
}
/************************************************************************/
void xbExpNode::SetNodeType( char cNodeType ){
  this->cNodeType = cNodeType;
}
/************************************************************************/
void xbExpNode::SetParent( xbExpNode *n ){
  this->nParent = n;
}
/************************************************************************/
void xbExpNode::SetResult( xbDate &dtResult ){
  this->dResult = dtResult.JulianDays();
}
/************************************************************************/
void xbExpNode::SetResult( xbBool bResult ){
  this->dResult = bResult;
}
/************************************************************************/
void xbExpNode::SetResult( xbDouble dResult ){
  this->dResult = dResult;
}
/************************************************************************/
void xbExpNode::SetResult( xbString &sResult ){
  this->sResult = sResult;
}
/************************************************************************/
void xbExpNode::SetResultLen( xbUInt32 ulResultLen ){
  this->ulResultLen = ulResultLen;
}
/************************************************************************/
void xbExpNode::SetReturnType( char cReturnType ){
  this->cReturnType = cReturnType;
}
/************************************************************************/
void xbExpNode::SetWeight( xbInt16 iWeight ){
  this->iWeight = iWeight;
}
/*************************************************************************/
};     // namespace
#endif     // XB_EXPRESSION_SUPPORT
/*************************************************************************/
