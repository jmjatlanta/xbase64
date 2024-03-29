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
   xbExpNode();
   xbExpNode( xbString &sNodeText, char cReturnType, char cNodeType  );
   xbExpNode( xbString &sNodeText, char cNodeType  );
   xbExpNode( char cNodeType  );

   virtual   ~xbExpNode();
   xbInt16   AddChild( xbExpNode *n );
   xbExpNode *GetChild( xbUInt32 ulChildNo ) const;
   xbUInt32  GetChildCnt() const;
   xbUInt32  GetChildNo() const;
   xbDbf     *GetDbf() const;
   xbInt16   GetFieldNo() const;
   xbExpNode *GetFirstNode();
   xbExpNode *GetNextNode() const;
   void      GetNodeText( xbString &sNodeText ) const;
   char      GetNodeType() const;
   xbExpNode *GetParent() const;
   xbString  &GetStringResult();
   xbBool    GetBoolResult() const;
   xbDouble  GetNumericResult() const;
   xbUInt32  GetResultLen() const;
   xbExpNode *GetRightSibling() const;
   char      GetReturnType() const;
   xbInt16   GetWeight() const;
   xbBool    HasRightSibling() const;
   xbBool    IsOperator() const;
   xbBool    IsUnaryOperator() const;
   void      RemoveLastChild();
   void      SetResultLen( xbUInt32 ulResultLen );
   void      SetDbfInfo( xbDbf *dbf );
   void      SetDbfInfo( xbDbf *dbf, xbInt16 iFieldNo );
   void      SetNodeText( xbString &sNodeText );
   void      SetNodeType( char cNodeType );
   void      SetParent( xbExpNode *n );
   void      SetResult( xbString &sResult );
   void      SetResult( xbDate   &dtResult );
   void      SetResult( xbBool   bResult );
   void      SetResult( xbDouble dResult );
   void      SetReturnType( char cReturnType );
   void      SetWeight( xbInt16 iWeight );

   #ifdef XB_DEBUG_SUPPORT
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


