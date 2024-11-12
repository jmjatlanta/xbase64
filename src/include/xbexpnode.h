/*  xbexpnode.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/


#ifndef __XB_EXPNODE_H__
#define __XB_EXPNODE_H__

// #ifdef  CMAKE_COMPILER_IS_GNUCC
// #pragma interface
// #endif

#ifdef XB_EXPRESSION_SUPPORT


namespace xb{

//class XBDLLEXPORT xbDbf;

/************************************************************************/
//! @brief Expression node class for handling expression nodes.
/*!
The xbExpNode class is used by the expression logic for handling specific
nodes within a tree of nodes.<br>

Expressions are parsed into nodes and a logical tree of nodes is created
out of all the individual components within the expression.<br>

This class is used for handling a specific node within a tree.<br>

An application program will typically not need to be concerned with
this class.<br>

*/
class XBDLLEXPORT xbExpNode {
  public:
   //! @brief Constructor
   xbExpNode();
   //! @brief Constructor
   /*!
     @param sNodeText Node text.
     @param cReturnType Return type.
     @param cNodeType Node type.
   */
   xbExpNode( xbString &sNodeText, char cReturnType, char cNodeType  );

   //! @brief Constructor
   /*!
      @param sNodeText Node text.
      @param cNodeType Node type.
   */
   xbExpNode( xbString &sNodeText, char cNodeType  );

   //! @brief Constructor
   /*!
     @param cNodeType Node type.
   */
   xbExpNode( char cNodeType  );

   //! @brief Deconstructor
   virtual   ~xbExpNode();

   //! @brief Add child node to linked list.
   /*!
     @param n Pointer to node to add to linked list.
     @returns <a href="xbretcod_8h.html">Return Codes</a>
   */
   xbInt16   AddChild( xbExpNode *n );


   //! @brief Get pointer to child.
   /*!
      @param ulChildNo - Which child? 1,2 or3 
      @returns Pointer to child node or null if none exists.
   */
   xbExpNode *GetChild( xbUInt32 ulChildNo ) const;

   //! @brief Get child count.
   /*!
      @returns Child count.
   */
   xbUInt32  GetChildCnt() const;

   //! @brief Get the current child number for this node.
   /*!
      @returns Child number: 1, 2 or 3.
   */
   xbUInt32  GetChildNo() const;

   //! @brief Get the dbf pointer.
   /*!
      @returns Pointer to dbf.
   */
   xbDbf     *GetDbf() const;

   //! @brief Get the field number.
   /*!
      @returns Field number for field node.
   */
   xbInt16   GetFieldNo() const;

   //! @brief Get the first node.
   /*!
      @returns Pointer to left most child node or *this if childless.
   */
   xbExpNode *GetFirstNode();

   //! @brief Get the next node.
   /*!
      @returns Pointer to right node or parent if right node does not exist.
   */
   xbExpNode *GetNextNode() const;

   //! @brief Get the node text.
   /*!
      @param sOutText Output string containing node text.
      @returns void
   */
   void      GetNodeText( xbString &sNodeText ) const;

   //! @brief Get the node type.
   /*!
      @returns Node type.
   */
   char      GetNodeType() const;

   //! @brief Get parent.
   /*!
      @returns Pointer to parent node.
   */
   xbExpNode *GetParent() const;

   //! @brief Get string result.
   /*!
      @returns String result.
   */
   xbString  &GetStringResult();

   //! @brief Get boolean result.
   /*!
      @returns Boolean result.
   */
   xbBool    GetBoolResult() const;

   //! @brief Get numeric result.
   /*!
      @returns Numeric result.
   */
   xbDouble  GetNumericResult() const;

   //! @brief Get result length.
   /*!
      @returns Result length.
   */
   xbUInt32  GetResultLen() const;

   //! @brief Get right sibling.
   /*!
      @returns Pointer to right sibling.
   */
   xbExpNode *GetRightSibling() const;

   //! @brief Get result type.
   /*!
      @returns Result type.
   */
   char      GetReturnType() const;

   //! @brief Get node weight.
   /*!
      Each node is assigned a weight used internally to detmerine processing sequence.
      @returns Node weight.
   */
   xbInt16   GetWeight() const;

   //! @brief Determine if node has a right sibling.
   /*!
      @returns xbTrue - Node has right sibling.<br>
               xbFalse - Node has no right sibling.
   */
   xbBool    HasRightSibling() const;

   //! @brief Determine if node is an operator.
   /*!
      @returns xbTrue - Node is an operator.<br>
               xbFalse - Node is not an operator.
   */
   xbBool    IsOperator() const;

   //! @brief Determine if node is a unary operator.
   /*!
      @returns xbTrue - Node is a unary operator.<br>
               xbFalse - Node is not a unary operator.
   */
   xbBool    IsUnaryOperator() const;

   //! @brief Remove last child from node.
   void      RemoveLastChild();

   //! @brief Set result length.
   /*!
      @param ulResultLen Set result length.
      @returns void.
   */
   void      SetResultLen( xbUInt32 ulResultLen );

   //! @brief Set dbf info on node.
   /*!
      @param dbf Pointer to dbf.
      @param iFieldNo Field number of field.
      @returns void.
   */
   void      SetDbfInfo( xbDbf *dbf, xbInt16 iFieldNo );

   //! @brief Set dbf info on node.
   /*!
      @param dbf Pointer to dbf.
      @returns void.
   */
   void      SetDbfInfo( xbDbf *dbf );

   //! @brief Set node text.
   /*!
      @param sNodeText Node text.
      @returns void.
   */
   void      SetNodeText( xbString &sNodeText );

   //! @brief Set node type.
   /*!
      @param cNodeType Node type.
      @returns void.
   */
   void      SetNodeType( char cNodeType );

   //! @brief Set parent.
   /*!
      @param n Pointer to parent.
   */
   void      SetParent( xbExpNode *n );

   //! @brief Set string result.
   /*!
      @param sResult String result.
      @returns void.
   */
   void      SetResult( xbString &sResult );

   //! @brief Set date result.
   /*!
      @param dtResult Date result.
      @returns void.
   */
   void      SetResult( xbDate   &dtResult );

   //! @brief Set boolean result.
   /*!
      @param bResult Boolean result.
      @returns void.
   */
   void      SetResult( xbBool   bResult );

   //! @brief Set numeric result.
   /*!
      @param dResult Double numeric result.
      @returns void.
   */
   void      SetResult( xbDouble dResult );

   //! @brief Set return type.
   /*!
      @param cReturnType Return Type.
      @returns void.
   */
   void      SetReturnType( char cReturnType );

   //! @brief Set weight.
   /*!
      @param iWeight Weight to set this node at.
      @returns void.
   */
   void      SetWeight( xbInt16 iWeight );

   #ifdef XB_DEBUG_SUPPORT
   //! @brief Dump Node.
   /*!
      @param iOption xbTrue - Print child info if they exist.<bt>
                     xbFalse - Don't print child info.
      @note Available if XB_DEBUG_SUPPORT compiled into library.
   */
   void      DumpNode( xbInt16 iOption ) const;     // 0 = no children, 1 = and children
   #endif

  private:
   xbString  sNodeText;                 // expression text
   char      cReturnType;               // one of: XB_EXP_CHAR, XB_EXP_DATE, XB_EXP_LOGICAL, XB_EXP_NUMERIC
   char      cNodeType;                 // one of: XB_EXP_CONSTANT, XB_EXP_FUNCTION, XB_EXP_FIELD, XB_EXP_OPERATOR
   xbString  sResult;                   // char result, and operator
   xbDouble  dResult;                   // numeric and bool results
   xbExpNode * nParent;                 // pointer to parent
   xbLinkList<xbExpNode *> llChildren;  // linked list of descendent nodes
   xbDbf     *dbf;                      // pointer to dbf, used for field, RECNO() and RECCOUNT()
   xbInt16   iFieldNo;                  // field no if DBF field
   xbUInt32  ulResultLen;               // for string results, accumulated length of character operations
                                        // includes the sum of all nodes under this + this
                                        // date = 8, numeric = 4, logical = 1
   xbInt16   iWeight;                   // used for building the tree of nodes, assigned to operators
                                        // the higher the number, the lower it goes on the tree

};

/************************************************************************/
/* Expression handler */

};
#endif        // XB_EXPRESSION_SUPPORT
#endif        // __XB_EXP_H__


