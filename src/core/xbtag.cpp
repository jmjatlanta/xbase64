/*  xbtag.cpp


XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

#include "xbase.h"

#ifdef XB_INDEX_SUPPORT


namespace xb{

/************************************************************************/
//! @brief Constructor
/*!
  @param pIx Pointer to index file instance.
  @param vpTag Pointer to tag structure within file instance.
  @param sType NDX or MDX
  @param sTagName Name of tag.
  @param sExpression Tag key definition.
  @param sFilter MDX only - tag qualification expression.
  @param bUnique xbTrue - Index is unique.<br>xbFalse - Index is not unique.
  @param bSort MDX only<br>xbTrue - Descending.<br>xbFalse - Ascending.
*/

xbTag::xbTag( xbIx *pIx, void *vpTag, xbString &sType, xbString &sTagName, xbString &sExpression, xbString &sFilter, 
xbBool bUnique, xbBool bSort )
{
  this->pIx         = pIx;
  this->vpTag       = vpTag;
  this->sType       = sType;
  this->sTagName    = sTagName;
  this->sExpression = sExpression;
  this->sFilter     = sFilter;
  this->bUnique     = bUnique;
  this->bSort       = bSort;
}


/************************************************************************/
//! @brief Get tag key expression.
/*!
  @returns Tag key expression.
*/
const xbString &xbTag::GetExpression() const {
  return sExpression;
}
/************************************************************************/
//! @brief Get tag filter expression.
/*!
  @returns Tag filter expression (mdx only).
*/

const xbString &xbTag::GetFilter() const {
  return sFilter;
}
/************************************************************************/
//! @brief Get index file pointer.
/*!
  @returns Pointer to index file instance.
*/
xbIx *xbTag::GetIx() const {
  return pIx;
}
/************************************************************************/
//! @brief Get tag ascending setting.
/*!
  @returns Tag sort setting - MDX only.<br>xbTrue - Descending.<br>xbFalse - Ascending.
*/
xbBool xbTag::GetSort() const {
  return bSort;
}
/************************************************************************/
//! @brief Get tag name.
/*!
  @returns Tag name.
*/

const xbString &xbTag::GetTagName() const {
  return sTagName;
}
/************************************************************************/
//! @brief Get tag type.
/*!
  @returns Tag type.
*/
const xbString &xbTag::GetType() const {
  return sType;
}

/************************************************************************/
//! @brief Get tag unique setting.
/*!
  @returns Tag unique setting.<br>xbTrue - Unique.<br>xbFalse - Not unique.
*/

xbBool xbTag::GetUnique() const {
  return bUnique;
}
/************************************************************************/
//! @brief Get tag pointer for tag within index file.
/*!
  @returns Pointer to tag within index file instance.
*/
void *xbTag::GetVpTag() const {
  return vpTag;
}
/************************************************************************/
}   /* namespace */ 
#endif         /*  XB_INDEX_SUPPORT */