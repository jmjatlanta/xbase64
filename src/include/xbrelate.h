/*  xbrelate.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

#ifndef __XB_RELATE_H__
#define __XB_RELATE_H__

#ifdef  CMAKE_COMPILER_IS_GNUCC
#pragma interface
#endif


namespace xb{

//#define XB_REL_READY 0
//#define XB_REL_FOUND 1
//#define XB_REL_NF_OPT 2
//#define XB_REL_NF_REQ 3


struct XBDLLEXPORT xbRelation{


  xbRelation * pParent;

  //! String value of parent expression for linking the tables
  xbString   * sParentExpression;

  //! Parsed parent Expression for linking the tables
  xbExp      * pParentExp;


  xbUInt32     ulSaveParRecNo;

  xbDbf      * dDbf;
  xbIx       * pIx;
  void       * pTag;
  xbString   * sFilter;
  char         cType;     // M - Master
                          // O - Optional
                          // R - Required

  //! Status from last GetFirst,Next,Prev or Last
  xbInt16      iStatus;

  //! Record position for this node for this rel rec
  xbInt32      ulRecPos;  // 

  //! Query filter
  xbFilter   * pFilt;

  //! Linked list of this nodes children
  xbLinkList<xbRelation *> llChildren;

  //! Pointer to parent node
  xbLinkListNode<xbRelation *> *pParentNp;

};

class XBDLLEXPORT xbRelate {

 public:
  xbRelate( xbXBase *xbase );
  ~xbRelate();

  xbInt16 AddRelation( 
      xbDbf *dParent,                    // parent table
      const xbString &ParentExpression,  // parent expression, needs to match pChildTag
      xbDbf *dChild,                     // child table 
      xbIx *pChildIx,                    // child index
      void *pChildTag,                   // child tag
      char cType = 'O',                  // O = Optional, R = required
      const xbString &sFilter = ""       // optional child record filter
  );

  xbInt16 AddRelation( 
      xbDbf *dParent,                    // parent table
      const xbString &ParentExpression,  // parent expression, needs to match pChildTag
      xbDbf *dChild,                     // child table 
      const xbString &sChildTagName,     // child tag name
      char cType = 'O',                  // O = Optional, R = required
      const xbString &sFilter = ""       // optional child record filter
  );

  xbInt16 CheckTblSts( xbDbf *d );
  xbInt16 GetFirstRelRecord();
  xbInt16 GetLastRelRecord();
  xbInt16 GetNextRelRecord();
  xbInt16 GetPrevRelRecord();

  xbInt16 SetMaster( xbDbf * d, const xbString &sFilter = "" );

  #ifdef XB_DEBUG_SUPPORT
  void DumpRelationList();
  void DumpRelationNode( const xbString &sTitle, xbRelation *pRel );
  void DumpRelationTree();

  xbInt16 CheckRecNo( xbInt16 iPo, xbInt32 lRec0, xbInt32 lRec1 = -1,
                      xbInt32 lRec2 = -1, xbInt32 lRec3 = -1, xbInt32 lRec4 = -1,
                      xbInt32 lRec5 = -1, xbInt32 lRec6 = -1, xbInt32 lRec7 = -1 );

  #endif // XB_DEBUG_SUPPORT

 protected:

  xbInt16 CompareKeys( xbRelation *pRel, xbString &s1, xbString &s2, xbDouble &d1, xbDouble &d2, xbDate &dt1, xbDate &dt2 );
  xbLinkListNode<xbRelation *> *FindNodeForDbf( xbDbf * d );
  xbLinkListNode<xbRelation *> *GetLastTreeNode( xbLinkListNode<xbRelation *> *lln );
  xbLinkListNode<xbRelation *> *GetNextTreeNode( xbLinkListNode<xbRelation *> *lln );
  xbInt16 GetRelRecord( const char cDirection );

  xbInt16 GetFirstRecForNode( xbLinkListNode<xbRelation *> *lln );
  xbInt16 GetLastRecForNode( xbLinkListNode<xbRelation *> *lln );
  xbInt16 GetNextRecForNode( xbLinkListNode<xbRelation *> *lln );
  xbInt16 GetPrevRecForNode( xbLinkListNode<xbRelation *> *lln );
  xbInt16 GetParentRelKeys( xbRelation *pRel, xbString &s, xbDouble &d, xbDate &dt );
  xbInt16 GetChildRelKeys( xbRelation *pRel, xbString &s, xbDouble &d, xbDate &dt );

  xbInt16 InitQuery();

  xbInt16 RelKeySearch( xbRelation *pRel, const char cFL,  xbString &s, xbDouble &d, xbDate &dt );

 private:
  xbXBase    *xbase;       // pointer to main structures

  xbLinkList<xbRelation *> llQryTree;   // pointer to structured tree of linked lists
  xbLinkList<xbRelation *> llQryList;   // linked list of relations used by the query routines

};

}  /* namespace */
#endif  /*  __XB_RELATE_H__  */
