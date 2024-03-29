/*  xbtag.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/


#ifndef __XB_XBTAG_H__
#define __XB_XBTAG_H__

//#ifdef  CMAKE_COMPILER_IS_GNUCC
//#pragma interface
//#endif

namespace xb{


#ifdef XB_INDEX_SUPPORT

class XBDLLEXPORT xbIx;



//! @brief xbTag is used for linking index tags to a table (aka dbf file).
/*!

  Each dbf file (or table) can have zero, one or many tags.
  Each tag is maintained in a linked list of xbTags.

  NDX index files contain only one index tag.<br>
  MDX index files can contain one to 47 index tags.  The production MDX file is opened automatically
  and the tags are  added to the tag list.

*/

class XBDLLEXPORT xbTag {

 public:
  xbTag( xbIx *pIx, void *vpTag, xbString &sType, xbString &sTagName, xbString &sExpression, xbString &sFilter, xbBool bUnique, xbBool bSort );

  xbIx   *GetIx()                  const;
  void   *GetVpTag()               const;
  const  xbString &GetType()       const;
  const  xbString &GetTagName()    const;
  const  xbString &GetExpression() const;
  const  xbString &GetFilter()     const;
  xbBool GetUnique()               const;
  xbBool GetSort()                 const;

 private:
  xbIx     *pIx;
  void     *vpTag;
  xbString sType;
  xbString sTagName;
  xbString sExpression;
  xbString sFilter;
  xbBool   bUnique;
  xbBool   bSort;          // 0 = Ascending, 1 = Descending
};

#endif // XB_INDEX_SUPPORT


}        /* namespace xb    */
#endif   /* __XB_TAG_H__ */
