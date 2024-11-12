/*  xbtblmgr.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2024 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

This class manages a list of open tables, open indices are connected to the open tables

*/


#ifndef __XB_XBMGR_H__
#define __XB_XBMGR_H__

#ifdef  CMAKE_COMPILER_IS_GNUCC
#pragma interface
#endif

namespace xb{

class XBDLLEXPORT xbDbf;

#ifdef XB_RELATE_SUPPORT
class XBDLLEXPORT xbRelate;
#endif


/* this structure is a linked list of open tables */
struct XBDLLEXPORT xbTblList{
  xbTblList *pNext;
  xbString  *psFqTblName;          // Fully qualified name of table - same table can be opened multiple times, but must have unique alias
  xbString  *psTblName;            // Table name without path, without extension
  xbString  *psTblAlias;           // TblAliasName must be unique, same as TblNAme if Alias not provided
  xbDbf     *pDbf;
};


#ifdef XB_RELATE_SUPPORT
struct XBDLLEXPORT xbRelList{
  xbRelList  *pNext;
  xbRelate   *pRel;
};
#endif


class XBDLLEXPORT xbTblMgr : public xbCore {
 public:
  xbTblMgr();
  ~xbTblMgr();
  xbInt16     AddTblToTblList        ( xbDbf *d, const xbString &sFqTblName );
  xbInt16     AddTblToTblList        ( xbDbf *d, const xbString &sFqTblName, const xbString &sTblAlias );
  xbInt16     DisplayTableList       () const;
  xbDbf *     GetDbfPtr              ( const xbString &sTblAlias ) const;
  xbDbf *     GetDbfPtr              ( xbInt16 sItemNo ) const;
  xbTblList * GetTblListEntry        ( xbDbf *d );
  xbInt16     GetOpenTableCount      () const;
  xbInt16     RemoveTblFromTblList   ( const xbString &sTblAlias );
  xbInt16     RemoveTblFromTblList   ( xbDbf *d );


  #ifdef XB_RELATE_SUPPORT
  xbInt16     AddRelToRelList        ( xbRelate *pRel );
  void        RemoveRelFromRelList   ( xbRelate *pRel );
  xbInt16     CheckRelListForTblUse  ( xbDbf *d );
  #endif

 protected:

 private:
  xbTblList  * TblList;          // List of open database tables
  xbInt16    iOpenTableCount;    // Number of open tables


  #ifdef XB_RELATE_SUPPORT
  xbRelList  * pRelList;         // Pointer to list of active Relations
  #endif

};

}        /* namespace xb   */

#endif   /* __XB_XBMGR_H__ */