/*  xbrelate.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023,2024 Gary A Kunkel

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

///@cond DOXYOFF

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
///@endcond DOXYOFF


//! @brief Class for handling table relation logic
/*!
The xbRelate class is used define master / slave relationships between tables

<ul>
<li>Similar in functionality to SQL joins
<li>The highest level has one entry, the main master/parent node which can have one or more slave/child nodes.
<li>Slave/child tables can be the master table in lower level master/slave relationships
<li>There are two types of master/slave relationship - Optional (think left join) or Required (think inner join).
<li>Handled internally as a tree of linked lists. The tree is processed using a depth first algorith for traversing the tree.
<li>There is a one to many relationship between master and slave relationships.
<li>After the relationships are established, using the GetFirstRelRecord(), GetNextRelRecord(), GetLastRelRecord() and GetPrevRelRecord() 
are used to navigate thtough the data.

</ul>

*/


class XBDLLEXPORT xbRelate {

 public:
  //! @brief Constructor
  xbRelate( xbXBase *xbase );

  //! @brief Destructor
  ~xbRelate();

  //! @brief Add a relationship to an xbRelation.
  /*!
    This routine is called after the SetMaster() method is called.

    @param dParent Pointer to parent/master table.
    @param sParentExpression Parent table expression that links to the child table.
    @param dChild Pointer to child/slave table.
    @param sChildTagName Name of Child tag to use.
    @param cType O = Optional (left join)<br>R = Required (inner join)
    @param sFilter Optional child tag filter (place holder for future version of software).
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 AddRelation( 
      xbDbf *dParent,                     // parent table
      const xbString &sParentExpression,  // parent expression, needs to match pChildTag
      xbDbf *dChild,                      // child table 
      const xbString &sChildTagName,      // child tag name
      char cType = 'O',                   // O = Optional, R = required
      const xbString &sFilter = ""        // optional child record filter
  );

  //! @brief Add a relationship to an xbRelation.
  /*!
    This routine is called after the SetMaster() method is called.

    @param dParent Pointer to parent/master table.
    @param sParentExpression Parent table expression that links to the child table.
    @param dChild Pointer to child/slave table.
    @param pChildIx Pointer to the child index file that is used to link the child to the parent table.
    @param pChildTag Pointer to the tag in the index file that is used to link the child to the parent table.
    @param cType O = Optional (left join)<br>R = Required (inner join)
    @param sFilter = Optional child tag filter (place holder for future version of software).
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 AddRelation( 
      xbDbf *dParent,                     // parent table
      const xbString &sParentExpression,  // parent expression, needs to match pChildTag
      xbDbf *dChild,                      // child table 
      xbIx *pChildIx,                     // child index
      void *pChildTag,                    // child tag
      char cType = 'O',                   // O = Optional, R = required
      const xbString &sFilter = ""        // optional child record filter
  );

  //! @brief Check Table Status.
  /*!
    @private
    Used to determine if a give table pointer is already included elsewhere in an expression.

    @param d - Pointer to table to check on.
    @returns XB_NO_ERROR - Table not already defined<br>
             XB_ALREADY_DEFINED - Table is already defiend in another relation
  */
  xbInt16 CheckTblSts( xbDbf *d );

  //! @brief Get First Rel Record.
  /*!

    Used to position to the first composite record defined for the relation.
    After execution, all tables are positioned to the appropriate record.
    If the linkage is defined as optional and there is no associated record
    for a given table, the table will be positioned to blank record 0.

    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 GetFirstRelRecord();

  //! @brief Get Last Rel Record.
  /*!

    Used to position to the last composite record defined for the relation.
    After execution, all tables are positioned to the appropriate record.
    If the linkage is defined as optional and there is no associated record
    for a given table, the table will be positioned to blank record 0.

    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 GetLastRelRecord();

  //! @brief Get Next Rel Record.
  /*!

    Used to position to the next composite record defined for the relation.
    After execution, all tables are positioned to the appropriate record.
    If the linkage is defined as optional and there is no associated record
    for a given table, the table will be positioned to blank record 0.

    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 GetNextRelRecord();


  //! @brief Get Prev Rel Record.
  /*!

    Used to position to the previous composite record defined for the relation.
    After execution, all tables are positioned to the appropriate record.
    If the linkage is defined as optional and there is no associated record
    for a given table, the table will be positioned to blank record 0.

    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 GetPrevRelRecord();

  //! @brief Set Master Table for an xbRelation.
  /*!

    Set the master table for an xbRelation.  This routine is called first before 
    any of the  AddRelation() or any of the Get methods are called.

    @param dTbl - Pointer to parent/master table.
    @param sFilter = Optional filter (place holder for future version of software).
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 SetMaster( xbDbf * dTbl, const xbString &sFilter = "" );

  #ifdef XB_DEBUG_SUPPORT

  ///@cond
  void DumpRelationList();
  void DumpRelationNode( const xbString &sTitle, xbRelation *pRel );
  void DumpRelationTree();
  xbInt16 CheckRecNo( xbInt16 iPo, xbInt32 lRec0, xbInt32 lRec1 = -1,
                      xbInt32 lRec2 = -1, xbInt32 lRec3 = -1, xbInt32 lRec4 = -1,
                      xbInt32 lRec5 = -1, xbInt32 lRec6 = -1, xbInt32 lRec7 = -1 );
  ///@endcond

  #endif // XB_DEBUG_SUPPORT

 protected:

  ///@cond
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
  ///@endcond

 private:
  xbXBase    *xbase;       // pointer to main structures

  xbLinkList<xbRelation *> llQryTree;   // pointer to structured tree of linked lists
  xbLinkList<xbRelation *> llQryList;   // linked list of relations used by the query routines

};

}  /* namespace */
#endif  /*  __XB_RELATE_H__  */
