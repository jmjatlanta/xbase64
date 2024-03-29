/*  xbfilter.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022 Gary A Kunkel

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


class XBDLLEXPORT xbFilter {

 public:
  xbFilter( xbDbf *dbf );
  ~xbFilter();
  xbInt16 Set( const xbString &sFilterExpression );

  #ifdef XB_INDEX_SUPPORT
  xbInt16 Set( const xbString &sFilterExpression, xbIx *ix, void *vpTag );
  xbInt16 Set( const xbString &sFilterExpression, const xbString &sTagName );

  xbInt16 SetIxTag( xbIx *ix, void *vpTag );
  xbInt16 SetIxTag( const xbString &sTagName );

  #endif // XB_INDEX_SUPPORT

  xbInt16 GetFirstRecord( xbInt16 iOpt = XB_ACTIVE_RECS );
  xbInt16 GetNextRecord ( xbInt16 iOpt = XB_ACTIVE_RECS );
  xbInt16 GetPrevRecord ( xbInt16 iOpt = XB_ACTIVE_RECS );
  xbInt16 GetLastRecord ( xbInt16 iOpt = XB_ACTIVE_RECS );
  void    SetLimit( xbInt32 ulLimit );
  xbInt32 GetLimit() const;
  void    ResetQryCnt();
  xbInt32 GetQryCnt() const;

  #ifdef XB_INDEX_SUPPORT
  xbInt16 Find( const xbString &sKey,  xbInt16 iOpt = XB_ACTIVE_RECS  );
  xbInt16 Find( const xbDate   &dtKey, xbInt16 iOpt = XB_ACTIVE_RECS  );
  xbInt16 Find( const xbDouble &dKey,  xbInt16 iOpt = XB_ACTIVE_RECS  );
  xbInt16 GetFirstRecordIx( xbInt16 iOpt = XB_ACTIVE_RECS );
  xbInt16 GetNextRecordIx ( xbInt16 iOpt = XB_ACTIVE_RECS );
  xbInt16 GetPrevRecordIx ( xbInt16 iOpt = XB_ACTIVE_RECS );
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