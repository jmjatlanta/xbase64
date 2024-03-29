/*  xbexpnode.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2017,2022,2023 Gary A Kunkel

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
//! @brief Constructor
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
//! @brief Constructor
/*!
  \param sNodeText Node text.
  \param cReturnType Return type.
  \param cNodeType Node type.
*/

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
//! @brief Constructor
/*!
  \param sNodeText Node text.
  \param cNodeType Node type.
*/
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
//! @brief Constructor
/*!
  \param cNodeType Node type.
*/
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
//! @brief Deconstructor
xbExpNode::~xbExpNode(){

  xbExpNode * n;
  while( llChildren.GetNodeCnt() > 0 ){
    llChildren.RemoveFromFront( n );
    delete n;
  }
}

/************************************************************************/
//! @brief Add child node to linked list.
/*!
  \param n Pointer to node to add to linked list.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbExpNode::AddChild( xbExpNode *n ){
  n->SetParent( this );
  return llChildren.InsertAtEnd( n );
}

/************************************************************************/
#ifdef XB_DEBUG_SUPPORT

//! @brief Dump Node.
/*!
  \param iOption xbTrue - Print child info if they exist.<bt>
                 xbFalse - Don't print child info.
  \returns void.
*/
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
//! @brief Get boolean result.
/*!
  \returns Boolean result.
*/

xbBool xbExpNode::GetBoolResult() const {
  return (xbBool) dResult;
}

/************************************************************************/
//! @brief Get pointer to child.
/*!
  \param ulChildNo - Which child? 1,2 or3 
  \returns Pointer to child node or null if none exists.
*/
xbExpNode *xbExpNode::GetChild( xbUInt32 ulChildNo ) const {

  xbLinkListNode<xbExpNode *> *lln = llChildren.GetNodeForNo( ulChildNo );
  if( lln )
    return lln->GetKey();
  else
    return 0x00;
}
/************************************************************************/
//! @brief Get child count.
/*!
  \returns Child count.
*/
xbUInt32 xbExpNode::GetChildCnt() const{
   return llChildren.GetNodeCnt();
}

/************************************************************************/
//! @brief Get the current child number for this node.
/*!
  \returns Child number: 1, 2 or 3.
*/

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
//! @brief Get the dbf pointer.
/*!
  \returns Pointer to dbf.
*/
xbDbf *xbExpNode::GetDbf() const {
  return dbf;
}
/************************************************************************/
//! @brief Get the field number.
/*!
  \returns Field number for field node.
*/

xbInt16 xbExpNode::GetFieldNo() const {
  return iFieldNo;
}

/*************************************************************************/
//! @brief Get the first node.
/*!
  \returns Pointer to left most child node or *this if childless.
*/

xbExpNode *xbExpNode::GetFirstNode() {
  xbExpNode *n = this;
  while( n && n->GetChildCnt() > 0 )
    n = n->GetChild(0);
  return n;
}

/*************************************************************************/
//! @brief Get the next node.
/*!
  \returns Pointer to right node or parent if right node does not exist.
*/

xbExpNode *xbExpNode::GetNextNode() const {

  if( HasRightSibling())
    return GetRightSibling()->GetFirstNode();
  else
    return nParent;
}

/************************************************************************/
//! @brief Get the node text.
/*!
  \param sOutText Output string containing node text.
  \returns void
*/

void xbExpNode::GetNodeText( xbString &sOutText ) const{
  sOutText = sNodeText;
}

/************************************************************************/
//! @brief Get the node type.
/*!
  \returns Node type.
*/

char xbExpNode::GetNodeType() const{
  return cNodeType;
}

/************************************************************************/
//! @brief Get numeric result.
/*!
  \returns Numeric result.
*/

xbDouble xbExpNode::GetNumericResult() const {
  return dResult;
}

/************************************************************************/
//! @brief Get parent.
/*!
  \returns Pointer to parent node.
*/

xbExpNode *xbExpNode::GetParent() const{
  return nParent;
}

/************************************************************************/
//! @brief Get result length.
/*!
  \returns Result length.
*/
xbUInt32 xbExpNode::GetResultLen() const{
  return ulResultLen;
}
/************************************************************************/
//! @brief Get result type.
/*!
  \returns Result type.
*/
char xbExpNode::GetReturnType() const{
  return cReturnType;
}

/*************************************************************************/
//! @brief Get right sibling.
/*!
  \returns Pointer to right sibling.
*/

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
//! @brief Get string result.
/*!
  \returns String result.
*/
xbString &xbExpNode::GetStringResult(){
  return sResult;
}
/************************************************************************/
//! @brief Get node weight.
/*!
  Each node is assigned a weight used internally to detmerine processing sequence.
  \returns Node weight.
*/

xbInt16 xbExpNode::GetWeight() const {
  return iWeight;
}

/*************************************************************************/
//! @brief Determine if node has a right sibling.
/*!
  \returns xbTrue - Node has right sibling.<br>
           xbFalse - Node has no right sibling.
*/

xbBool xbExpNode::HasRightSibling() const {

  // std::cout << "in HasRightSibling [" << sNodeText << "]\n";

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
//! @brief Determine if node is an operator.
/*!
  \returns xbTrue - Node is an operator.<br>
           xbFalse - Node is not an operator.
*/

xbBool xbExpNode::IsOperator() const {

  if( cNodeType == XB_EXP_OPERATOR )
    return xbTrue;
  else
    return xbFalse;
}

/*************************************************************************/
//! @brief Determine if node is a unary operator.
/*!
  \returns xbTrue - Node is a unary operator.<br>
          xbFalse - Node is not a unary operator.
*/
xbBool xbExpNode::IsUnaryOperator() const {

  if( cNodeType == XB_EXP_PRE_OPERATOR || cNodeType == XB_EXP_POST_OPERATOR )
    return xbTrue;
  else if( cNodeType == XB_EXP_OPERATOR && (sNodeText == ".NOT." || sNodeText == "NOT" ))
    return xbTrue;
  else
    return xbFalse;
}
/************************************************************************/
//! @brief Remove last child from node.
/*!
  \returns void.
*/

void xbExpNode::RemoveLastChild(){
  xbExpNode *n;
  llChildren.RemoveFromEnd( n );
}

/************************************************************************/
//! @brief Set dbf info on node.
/*!
  \param dbf Pointer to dbf.
  \param iFieldNo Field number of field.
  \returns void.
*/
void xbExpNode::SetDbfInfo( xbDbf *dbf, xbInt16 iFieldNo ){
  this->dbf      = dbf;
  this->iFieldNo = iFieldNo;
}

/************************************************************************/
//! @brief Set dbf info on node.
/*!
  \param dbf Pointer to dbf.
  \returns void.
*/
void xbExpNode::SetDbfInfo( xbDbf *dbf ){
  this->dbf      = dbf;
}
/************************************************************************/
//! @brief Set node type.
/*!
  \param cNodeType Node type.
  \returns void.
*/
void xbExpNode::SetNodeType( char cNodeType ){
  this->cNodeType = cNodeType;
}

/************************************************************************/
//! @brief Set node text.
/*!
  \param sNodeText Node text.
  \returns void.
*/
void xbExpNode::SetNodeText( xbString & sNodeText ){
  this->sNodeText = sNodeText;
}

/************************************************************************/
//! @brief Set parent.
/*!
  \param n Pointer to parent.
  \returns void.
*/
void xbExpNode::SetParent( xbExpNode *n ){
  this->nParent = n;
}

/************************************************************************/
//! @brief Set date result.
/*!
  \param dtResult Date result.
  \returns void.
*/
void xbExpNode::SetResult( xbDate &dtResult ){
  this->dResult = dtResult.JulianDays();
}

/************************************************************************/
//! @brief Set boolean result.
/*!
  \param bResult Boolean result.
  \returns void.
*/
void xbExpNode::SetResult( xbBool bResult ){
  this->dResult = bResult;
}
/************************************************************************/
//! @brief Set numeric result.
/*!
  \param dResult Double numeric result.
  \returns void.
*/
void xbExpNode::SetResult( xbDouble dResult ){
  this->dResult = dResult;
}
/************************************************************************/
//! @brief Set string result.
/*!
  \param sResult String result.
  \returns void.
*/
void xbExpNode::SetResult( xbString &sResult ){
  this->sResult = sResult;
}
/************************************************************************/
//! @brief Set result length.
/*!
  \param ulResultLen Set result length.
  \returns void.
*/
void xbExpNode::SetResultLen( xbUInt32 ulResultLen ){
  this->ulResultLen = ulResultLen;
}
/************************************************************************/
//! @brief Set return type.
/*!
  \param cReturnType Return Type.
  \returns void.
*/
void xbExpNode::SetReturnType( char cReturnType ){
  this->cReturnType = cReturnType;
}
/************************************************************************/
//! @brief Set weight.
/*!
  \param iWeight Weight to set this node at.
  \returns void.
*/
void xbExpNode::SetWeight( xbInt16 iWeight ){
  this->iWeight = iWeight;
}
/*************************************************************************/
};     // namespace
#endif     // XB_EXPRESSION_SUPPORT
/*************************************************************************/
