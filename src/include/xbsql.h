/*  xbsql.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2024 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

This class manages a list of open tables, open indices are connected to the open tables

*/


#ifndef __XB_XBSQL_H__
#define __XB_XBSQL_H__

#ifdef  CMAKE_COMPILER_IS_GNUCC
#pragma interface
#endif


#ifdef XB_SQL_SUPPORT

namespace xb{

///@cond

struct XBDLLEXPORT xbSqlFld{
  char     cType;      // F - Database field
                       // L - Literal
                       // E - Expression
                       // T - Group by total
  xbDbf    *dbf;       // pointer to table
  xbInt16  iFldNo;     // Field number if db field
  xbExp *  pExp;       // If cType=E, pointer to parsed expression
  xbString sFldAlias;  // Alias name for query display
  xbString sLiteral;   // For type L - Literal value
  xbSqlFld * Next;     // Next field in list
};

class xbStmt;

class XBDLLEXPORT xbCursor : public xbCore {

  public:
             xbCursor( xbXBase * x );
             ~xbCursor();

  protected:
    xbXBase *xbase;              /* pointer to the main structures                       */

  private:
};



struct XBDLLEXPORT xbTblJoin {            //  structure for defining joined tables in the query

  xbString sTableName;
  xbString sAlias;
  xbString sJoinExp;                      // table join expression
  xbDbf    *pDbf;                         // pointer the the dbf structure for this table
  xbTag    *pTag;                         // pointer to relevant tag for this table, null for record order
  xbExp    *pLinkExp;                     // pointer to parsed linkage expression
  char     cJoinType;                     // M - Master
                                          // L - Left   or Full Left
                                          // R - Right  or Full Right
                                          // I - Inner
                                          // O - Outer  or Full Outer Outer

  xbTblJoin *pMaster;                     // pointer to master table, null if this is the primary master
  xbLinkList<xbTblJoin *> llSubList;      // linked list of subordinate tables
//  xbStmt    *pSubQuery;                 // pointer to sub query
  xbTblJoin   *next;                      // next tbl
};


class XBDLLEXPORT xbStmt : public xbCore{

 public:
  xbStmt( xbXBase *x );
  ~xbStmt();

  #ifdef XB_DEBUG_SUPPORT
  xbInt16 DumpStmtInternals();
  xbInt16 Test();
  #endif

  xbInt16 ExecuteQuery( const xbString &sCmdLine ); 

//  xbInt16 FetchFirst();
//  xbInt16 FetchNext();
//  xbInt16 FetchPrev();
//  xbInt16 FetchLast();
//  xbString &GetField( const xbString sFldName );



 protected:

 private:

  xbInt16  AddQryTbl( const xbString &sTable, const xbString &sAlias, const xbString &sExpression, const char cJoinType );
  xbInt16  CvtSqlExp2DbaseExp( const xbString &sExpIn, xbString &sExpOut );
  xbInt16  GetNextFromSeg( const xbString &sLineIn, xbString &sFromSegOut );
  xbUInt32 GetNextToken( const xbString &sCmdLineIn, xbString &sTokenOut, xbUInt32 ulStartPos );
  xbInt16  GetParenCnt( const xbString &sToken );
  xbInt16  ParseFromStmt( const xbString &sFromLine );
  xbInt16  ParseStmt( const xbString &sCmdLine );
  xbInt16  ProcessFromSeg( const xbString &sFromStmtSeg );
  xbInt16  UpdateJoinList( char cType, const xbString &sTableName, const xbString &sAlias, const xbString &sExp, xbDbf *d, xbExp *e  );   //, xbTag *t);

  xbXBase  *xbase;    // pointer to xbase structure


  xbTblJoin * pTblList;
  xbSqlFld *fl;       // field list in sql select statement
  xbString sStmt;     // complete query statment
  xbString sFields;   // list of fields
  xbString sFrom;     // from statment
  xbString sWhere;    // where clause
  xbString sOrderBy;  // sort order
  xbString sGroupBy;  // group by
  xbString sHaving;   // having
  xbUInt32 ulFromPos;
  xbUInt32 ulWherePos;
  xbUInt32 ulOrderByPos;
  xbUInt32 ulGroupByPos;
  xbUInt32 ulHavingPos;


};
///@endcond



//! @brief xbSql class.
/*!
The xbSql class provides the structure for executing SQL commands against the tables.

Beginning with version 4.1.6, there is limited SQL support.  Additional SQL support will be included
in future releases.

<a href="../include/Sql.html">Supported SQL Commands</a>
*/



class XBDLLEXPORT xbSql : public xbCore {
 public:
   //! @brief Constructor
   xbSql( xbXBase *x );

   //! @brief Destructor
   ~xbSql();

   //! @brief Execute non query SQL command
   /*!
      Execute a non query SQL command
      @param sCmdLine SQL Command
      @returns unsigned long int value

      <a href="../include/Sql.html">Supported SQL Commands</a>

   */
   xbInt16 ExecuteNonQuery( const xbString &sCmdLine );

   //! @brief Get pointer to Xbase structure
   //! @private
   xbXBase *GetXbasePtr() const;

 protected:

 private:

   xbInt16  SqlAlterTable( const xbString &sCmdLine );
   xbInt16  SqlCreateTable( const xbString &sCmdLine );
   xbInt16  SqlDelete( const xbString &sCmdLine );
   xbInt16  SqlDropTable( const xbString &sCmdLine );

   #ifdef XB_INDEX_SUPPORT
   xbInt16  SqlCreateIndex( const xbString &sCmdLine );
   xbInt16  SqlDropIndex( const xbString &sCmdLine );
   #endif  // XB_INDEX_SUPPORT

// xbInt16  SqlCreateView( const xbString &sCmdLine );
// xbInt16  SqlDropView( const xbString &sCmdLine );
// xbInt16  SqlUpdate( const xbString &sCmdLine );
// xbInt16  SqlSelect( const xbString &sCmdLine );

   void     SqlHelp() const;
   xbInt16  SqlInsert( const xbString &sCmLine );
   xbInt16  SqlSet( const xbString &sCmdLine );
   xbInt16  SqlUse( const xbString &sCmdLine );

   xbXBase *xbase;
   xbUda   uda;

};

}        /* namespace xb    */
#endif   /* XB_SQL_SUPPORT  */
#endif   /* __XB_XBSQL_H__  */