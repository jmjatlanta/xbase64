/*  xbfilter.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2024 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

This class manages the user data area (UDA)

*/


#ifndef __XB_XBFILTER_H__
#define __XB_XBFILTER_H__

#ifdef  CMAKE_COMPILER_IS_GNUCC
#pragma interface
#endif


#ifdef XB_FILTER_SUPPORT


namespace xb{


//! @brief xbFilter class.
/*!

The xFilter class is used to establish a data filter on a given table (DBF file).
<ol>
<li>A filter can be established to limit selected records in a query.
<li>Similar to a <em>WHERE</em> clause in SQL.
<li>The filter logic uses the expression module for processing.
<li>Any of the expression routines that return a logical value can be used to establish a filter condition.
<li>See Chapter four or the xb_ex_expression sample program for more information on valid expressions.
</ol>
*/


class XBDLLEXPORT xbFilter {

 public:
  //! @brief Constructor
  /*!
  Create instance of xbFilter.
    @param dbf Pointer to associated dbf structure.
  */
  xbFilter( xbDbf *dbf );

  //! @brief Destructor
  /*!
    Destructor for xbFilter.
  */
  ~xbFilter();

  //! @brief Set filter options.
  /*!
    Set the filter string.

    @param sFilterExpression filter expression
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 Set( const xbString &sFilterExpression );

  #ifdef XB_INDEX_SUPPORT

  //! @brief Set filter options.
  /*!
     Set the filter string and index tag.

     @param sFilterExpression filter expression
     @param pIx pointer to index file
     @param vpTag pointer to tag
     @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 Set( const xbString &sFilterExpression, xbIx *ix, void *vpTag );

  //! @brief Set filter options.
  /*!
     Set the filter string and index tag

     @param sFilterExpression filter expression
     @param sTagName name of tag
     @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 Set( const xbString &sFilterExpression, const xbString &sTagName );

  //! @brief Set tag for filter.
  /*!
     Set the filter string and index tag

     @param pIx pointer to index file
     @param vpTag pointer to tag
     @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 SetIxTag( xbIx *ix, void *vpTag );

  //! @brief Set tag for filter.
  /*!
     Set the filter string and index tag

     @param sTagName name of tag
     @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 SetIxTag( const xbString &sTagName );
  #endif // XB_INDEX_SUPPORT

  //! @brief Get first record for filter
  /*!

    @param iOption XB_ALL_RECS - Get the first filtered record, deleted or not.<br>
                   XB_ACTIVE_RECS - Get the first filtered active record. Default setting.<br>
                   XB_DELETED_RECS - Get the first filtered deleted record.<br>

    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 GetFirstRecord( xbInt16 iOption = XB_ACTIVE_RECS );

  //! @brief Get next record for filter
  /*!

    @param iOption XB_ALL_RECS - Get the next filtered record, deleted or not.<br>
                   XB_ACTIVE_RECS - Get the next active filtered record. Default setting.<br>
                   XB_DELETED_RECS - Get the next deleted filtered record.<br>

    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 GetNextRecord ( xbInt16 iOption = XB_ACTIVE_RECS );

  //! @brief Get previous record for filter
  /*!

    @param iOption XB_ALL_RECS - Get the previous filtered record, deleted or not.<br>
                   XB_ACTIVE_RECS - Get the previous active filtered record. Default setting.<br>
                   XB_DELETED_RECS - Get the previous deleted filtered record.<br>

    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 GetPrevRecord ( xbInt16 iOption = XB_ACTIVE_RECS );

  //! @brief Get last record for filter
  /*!
    @param iOption XB_ALL_RECS - Get the last filtered record, deleted or not.<br>
                   XB_ACTIVE_RECS - Get the last active filtered record. Default setting.<br>
                   XB_DELETED_RECS - Get the last deleted filtered record.<br>

    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 GetLastRecord ( xbInt16 iOption = XB_ACTIVE_RECS );

  //! @brief Set Limit
  /*!
    Sets the limit for the number of records for a given query.

    @param ulLimit max number of records for a query.
           Zero (unlimited) is default value.
  */
  void    SetLimit( xbInt32 ulLimit );


  //! @brief Get current limit setting
  /*!
      @returns Current limit value.
  */
  xbInt32 GetLimit() const;

  //! @brief Reset Query Count
  /*!
    Reset the Query count to zero.
  */
  void    ResetQryCnt();

  //! @brief Get Query Count
  /*!
    Return the number of records returned for a given query.
      @returns number of query records.
      @note Setting of zero is unlimited.
  */
  xbInt32 GetQryCnt() const;

  #ifdef XB_INDEX_SUPPORT

  //! @brief Find a filtered record for a given xbString key.
  /*!

     @param sKey Key value to search for.
     @param iOption XB_ALL_RECS - Find key for filtered record, deleted or not.<br>
                    XB_ACTIVE_RECS - Find key for active filtered record. Default setting.<br>
                    XB_DELETED_RECS - Find key for deleted filtered record.<br>

     @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 Find( const xbString &sKey,  xbInt16 iOpt = XB_ACTIVE_RECS  );

  //! @brief Find a filtered record for a given xbDate key.
  /*!

    @param sKey Key value to search for.
    @param iOption XB_ALL_RECS - Find key for filtered record, deleted or not.<br>
                   XB_ACTIVE_RECS - Find key for active filtered record. Default setting.<br>
                   XB_DELETED_RECS - Find key for deleted filtered record.<br>

    @returns XB_NO_ERROR if found<br>
             <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 Find( const xbDate   &dtKey, xbInt16 iOpt = XB_ACTIVE_RECS  );


  //! @brief Find a filtered record for a given xbDouble (numeric) key.
  /*!

    @param sKey Key value to search for.
    @param iOption XB_ALL_RECS - Find key for filtered record, deleted or not.<br>
                   XB_ACTIVE_RECS - Find key for active filtered record. Default setting.<br>
                   XB_DELETED_RECS - Find key for deleted filtered record.<br>

    @returns XB_NO_ERROR if found<br>
             <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 Find( const xbDouble &dKey,  xbInt16 iOpt = XB_ACTIVE_RECS  );

  //! @brief Get First Record for filter, by index tag
  /*!

    @param iOption XB_ALL_RECS - Get the first filtered record, deleted or not.<br>
                   XB_ACTIVE_RECS - Get the first filtered active record. Default setting.<br>
                   XB_DELETED_RECS - Get the first filtered deleted record.<br>

    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 GetFirstRecordIx( xbInt16 iOpt = XB_ACTIVE_RECS );

  //! @brief Get Next Record for filter, by index tag
  /*!

    @param iOption XB_ALL_RECS - Get the first filtered record, deleted or not.<br>
                   XB_ACTIVE_RECS - Get the first filtered active record. Default setting.<br>
                   XB_DELETED_RECS - Get the first filtered deleted record.<br>

    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 GetNextRecordIx ( xbInt16 iOpt = XB_ACTIVE_RECS );

  //! @brief Get Previous Record for filter, by index tag
  /*!

    @param iOption XB_ALL_RECS - Get the first filtered record, deleted or not.<br>
                   XB_ACTIVE_RECS - Get the first filtered active record. Default setting.<br>
                   XB_DELETED_RECS - Get the first filtered deleted record.<br>

    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 GetPrevRecordIx ( xbInt16 iOpt = XB_ACTIVE_RECS );

  //! @brief Get Last Record for filter, by index tag
  /*!

    @param iOption XB_ALL_RECS - Get the first filtered record, deleted or not.<br>
         XB_ACTIVE_RECS - Get the first filtered active record. Default setting.<br>
         XB_DELETED_RECS - Get the first filtered deleted record.<br>

    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 GetLastRecordIx ( xbInt16 iOpt = XB_ACTIVE_RECS );
  #endif  // XB_INDEX_SUPPORT

 private:
  xbDbf    *dbf;
  xbExp    *exp;

  xbInt32  lLimit;           // max number rows returned
  xbInt32  lCurQryCnt;       // current count of rows returned, neg# is moving from bottom to top
                             //                                 pos# is moving from top to bottom

  #ifdef XB_INDEX_SUPPORT
  xbIx     *pIx;             // optional index tag
  void     *vpTag;
  #endif  // XB_INDEX_SUPPORT

};
}        /* namespace */
#endif   /* XB_FILTER_SUPPORT */
#endif   /* __XBFILTER_H__ */