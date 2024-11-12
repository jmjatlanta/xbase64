/*  xbfilter.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net


This module handles uda (user data area) methods 

*/

#include "xbase.h"

#ifdef XB_FILTER_SUPPORT

namespace xb{

/************************************************************************/
xbFilter::xbFilter( xbDbf *dbf ) {
  this->dbf    = dbf;
  this->exp    = NULL;
  lLimit       = 0;       // max number of responses
  lCurQryCnt   = 0;       // current number, this query  + = moving fwd
                          //                             - = moving backwards

  #ifdef XB_INDEX_SUPPORT
  pIx          = NULL;    // if index is set, the class uses the index tag, otherwise table
  vpTag        = NULL;
  #endif  // XB_INDEX_SUPPORT

}
/************************************************************************/
xbFilter::~xbFilter() {
  if( exp )
    delete exp;
}
/************************************************************************/
xbInt32 xbFilter::GetLimit() const {
  return lLimit;
}
/************************************************************************/
xbInt32 xbFilter::GetQryCnt() const {
  return lCurQryCnt;
}
/************************************************************************/
void xbFilter::SetLimit( xbInt32 lLimit ){
  this->lLimit = lLimit;
}
/************************************************************************/
void xbFilter::ResetQryCnt(){
  this->lCurQryCnt = 0;
}
/************************************************************************/
xbInt16 xbFilter::Set( const xbString &sFilter, xbIx *pIx, void *vpTag ) {
  this->pIx   = pIx;
  this->vpTag = vpTag;
  return Set( sFilter );
}
/************************************************************************/
xbInt16 xbFilter::Set( const xbString &sFilter, const xbString &sTagName ) {

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  try{
    if(( iRc = SetIxTag( sTagName )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }
    if(( iRc = Set( sFilter )) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbFilter::Set(sFilter,sTagName) Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    dbf->GetXbasePtr()->WriteLogMessage( sMsg.Str() );
    dbf->GetXbasePtr()->WriteLogMessage( dbf->GetXbasePtr()->GetErrorMessage( iRc ));
  }
  return iRc;
}
/************************************************************************/
xbInt16 xbFilter::Set( const xbString &sFilter ) {

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  try{

    if( exp )
      delete exp;

    exp = new xbExp( dbf->GetXbasePtr(), dbf );
    if(( iRc = exp->ParseExpression( sFilter.Str() )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }
    if( exp->GetReturnType() != XB_EXP_LOGICAL ){
      iErrorStop = 110;
      iRc = XB_INVALID_EXPRESSION;
      delete exp;
      exp = NULL;
      throw iRc;
    }
    if( !pIx && !vpTag ){
      pIx   = dbf->GetCurIx();
      vpTag = dbf->GetCurTag();
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbFilter::SetExpression() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    dbf->GetXbasePtr()->WriteLogMessage( sMsg.Str() );
    dbf->GetXbasePtr()->WriteLogMessage( dbf->GetXbasePtr()->GetErrorMessage( iRc ));
  }
  return iRc;
}
/************************************************************************/
xbInt16 xbFilter::SetIxTag( xbIx *pIx, void *vpTag ) {
  this->pIx   = pIx;
  this->vpTag = vpTag;
  return XB_NO_ERROR;
}
/************************************************************************/
xbInt16 xbFilter::SetIxTag( const xbString &sTagName ) {

  xbInt16    iRc        = XB_NO_ERROR;
  xbInt16    iErrorStop = 0;
  try{
    if(( iRc = dbf->GetTagPtrs( sTagName,  &this->pIx, &this->vpTag )) != XB_NO_ERROR ) {
      iErrorStop = 100;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbFilter::SetIx(1) Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, iRc );
    dbf->GetXbasePtr()->WriteLogMessage( sMsg.Str() );
    dbf->GetXbasePtr()->WriteLogMessage( dbf->GetXbasePtr()->GetErrorMessage( iRc ));
  }
  return iRc;
}
/************************************************************************/
xbInt16 xbFilter::GetFirstRecord( xbInt16 iOption ) { 

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  try{

    if( !exp ){
      iErrorStop = 100;
      throw iRc;
    }
    lCurQryCnt = 0;
    if(( iRc = dbf->GetFirstRecord( iOption )) != XB_NO_ERROR ){
      if( iRc == XB_EMPTY || iRc == XB_EOF )
        return iRc;
      else{
        iErrorStop = 110;
        throw iRc;
      }
    }
    xbBool bFound = xbFalse;
    while( !bFound && iRc == XB_NO_ERROR ){
      if(( iRc = exp->ProcessExpression()) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }
      if(( iRc = exp->GetBoolResult( bFound )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }
      if( !bFound ){
        if(( iRc = dbf->GetNextRecord( iOption )) != XB_NO_ERROR ){
          if( iRc == XB_EOF ){
            return iRc;
          } else {
            iErrorStop = 140;
            throw iRc;
          }
        }
      }
    }
    lCurQryCnt++;
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbFilter::GetFirstRecord() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    dbf->GetXbasePtr()->WriteLogMessage( sMsg.Str() );
    dbf->GetXbasePtr()->WriteLogMessage( dbf->GetXbasePtr()->GetErrorMessage( iRc ));
  }
  return iRc;
}
/************************************************************************/
xbInt16 xbFilter::GetNextRecord( xbInt16 iOption ){ 

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  try{
    if( !exp ){
      iErrorStop = 100;
      throw iRc;
    }
    if( lLimit != 0 && abs( lCurQryCnt ) >= lLimit )
      return XB_LIMIT_REACHED;

    if(( iRc = dbf->GetNextRecord( iOption )) != XB_NO_ERROR ){
      if( iRc == XB_EOF )
        return iRc;
      else{
        iErrorStop = 110;
        throw iRc;
      }
    }
    xbBool bFound = xbFalse;
    while( !bFound && iRc == XB_NO_ERROR ){
      if(( iRc = exp->ProcessExpression()) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }
      if(( iRc = exp->GetBoolResult( bFound )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }
      if( !bFound ){
        if(( iRc = dbf->GetNextRecord( iOption )) != XB_NO_ERROR ){
          if( iRc == XB_EOF ){
            return iRc;
          } else {
            iErrorStop = 140;
            throw iRc;
          }
        }
      }
    }
    lCurQryCnt++;
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbFilter::GetNextRecord() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    dbf->GetXbasePtr()->WriteLogMessage( sMsg.Str() );
    dbf->GetXbasePtr()->WriteLogMessage( dbf->GetXbasePtr()->GetErrorMessage( iRc ));
  }
  return iRc;
}
/************************************************************************/
xbInt16 xbFilter::GetPrevRecord( xbInt16 iOption ){ 

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  try{

    if( !exp ){
      iErrorStop = 100;
      throw iRc;
    }
    if( lLimit != 0 && abs( lCurQryCnt ) >= lLimit )
      return XB_LIMIT_REACHED;

    if(( iRc = dbf->GetPrevRecord( iOption )) != XB_NO_ERROR ){
      if( iRc == XB_BOF )
        return iRc;
      else{
        iErrorStop = 110;
        throw iRc;
      }
    }
    xbBool bFound = xbFalse;
    while( !bFound && iRc == XB_NO_ERROR ){
      if(( iRc = exp->ProcessExpression()) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }
      if(( iRc = exp->GetBoolResult( bFound )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }
      if( !bFound ){
        if(( iRc = dbf->GetPrevRecord( iOption )) != XB_NO_ERROR ){
          if( iRc == XB_BOF ){
            return iRc;
          } else {
            iErrorStop = 140;
            throw iRc;
          }
        }
      }
    }
    lCurQryCnt--;
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbFilter::GetPrevRecord() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    dbf->GetXbasePtr()->WriteLogMessage( sMsg.Str() );
    dbf->GetXbasePtr()->WriteLogMessage( dbf->GetXbasePtr()->GetErrorMessage( iRc ));
  }
  return iRc;
}
/************************************************************************/
xbInt16 xbFilter::GetLastRecord( xbInt16 iOption ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  try{

    if( !exp ){
      iErrorStop = 100;
      throw iRc;
    }
    lCurQryCnt = 0;
    if(( iRc = dbf->GetLastRecord( iOption )) != XB_NO_ERROR ){
      if( iRc == XB_EOF )
        return iRc;
      else{
        iErrorStop = 110;
        throw iRc;
      }
    }
    xbBool bFound = xbFalse;
    while( !bFound && iRc == XB_NO_ERROR ){
      if(( iRc = exp->ProcessExpression()) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }
      if(( iRc = exp->GetBoolResult( bFound )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }
      if( !bFound ){
        if(( iRc = dbf->GetPrevRecord( iOption )) != XB_NO_ERROR ){
          if( iRc == XB_BOF ){
            return iRc;
          } else {
            iErrorStop = 140;
            throw iRc;
          }
        }
      }
    }
    lCurQryCnt--;
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbFilter::GetLastRecord() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    dbf->GetXbasePtr()->WriteLogMessage( sMsg.Str() );
    dbf->GetXbasePtr()->WriteLogMessage( dbf->GetXbasePtr()->GetErrorMessage( iRc ));
  }
  return iRc;
}

/************************************************************************/
#ifdef XB_INDEX_SUPPORT

xbInt16 xbFilter::Find( const xbString &sKey, xbInt16 iOption ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  try{

    if( !exp ){
      iErrorStop = 100;
      throw iRc;
    }
    lCurQryCnt = 0;

    if(( iRc = dbf->Find( pIx, vpTag, sKey )) != XB_NO_ERROR ){
      if( iRc == XB_NOT_FOUND || iRc == XB_EMPTY || iRc == XB_EOF )
        return iRc;
      else{
        iErrorStop = 110;
        throw iRc;
      }
    }

    xbString sSaveKey( sKey );
    xbBool bFound = xbFalse;
    while( !bFound && iRc == XB_NO_ERROR && sKey == sSaveKey ){
      if(( iRc = exp->ProcessExpression()) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }
      if(( iRc = exp->GetBoolResult( bFound )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }
      if( !bFound || 
          (dbf->RecordDeleted() && iOption == XB_ACTIVE_RECS) ||
          (!dbf->RecordDeleted() && iOption == XB_DELETED_RECS)){

        if(( iRc = dbf->GetNextKey( pIx, vpTag )) != XB_NO_ERROR ){
          if( iRc == XB_EOF ){
            return iRc;
          } else {
            iErrorStop = 140;
            throw iRc;
          }
        }
        if(( iRc = pIx->GetCurKeyVal( vpTag, sSaveKey )) != XB_NO_ERROR ){
          iErrorStop = 150;
          throw iRc;
        }
      }
    }
    if( bFound )
      lCurQryCnt++;
    else
      iRc = XB_NOT_FOUND;
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbFilter::Find(xbString) Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    dbf->GetXbasePtr()->WriteLogMessage( sMsg.Str() );
    dbf->GetXbasePtr()->WriteLogMessage( dbf->GetXbasePtr()->GetErrorMessage( iRc ));
  }
  return iRc;
}
/************************************************************************/
xbInt16 xbFilter::Find( const xbDate &dtKey, xbInt16 iOption ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  try{

    if( !exp ){
      iErrorStop = 100;
      throw iRc;
    }
    lCurQryCnt = 0;

    if(( iRc = dbf->Find( pIx, vpTag, dtKey )) != XB_NO_ERROR ){
      if( iRc == XB_NOT_FOUND || iRc == XB_EMPTY || iRc == XB_EOF )
        return iRc;
      else{
        iErrorStop = 110;
        throw iRc;
      }
    }

    xbDate dtSaveKey( dtKey );
    xbBool bFound = xbFalse;
    while( !bFound && iRc == XB_NO_ERROR && dtKey == dtSaveKey ){
      if(( iRc = exp->ProcessExpression()) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }
      if(( iRc = exp->GetBoolResult( bFound )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }
      if( !bFound || 
          (dbf->RecordDeleted() && iOption == XB_ACTIVE_RECS) ||
          (!dbf->RecordDeleted() && iOption == XB_DELETED_RECS)){
        if(( iRc = dbf->GetNextKey( pIx, vpTag )) != XB_NO_ERROR ){
          if( iRc == XB_EOF ){
            return iRc;
          } else {
            iErrorStop = 140;
            throw iRc;
          }
        }
      }
      if(( iRc = pIx->GetCurKeyVal( vpTag, dtSaveKey )) != XB_NO_ERROR ){
        iErrorStop = 150;
        throw iRc;
      }
    }
    if( bFound )
      lCurQryCnt++;
    else
      iRc = XB_NOT_FOUND;
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbFilter::Find(xbDate) Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    dbf->GetXbasePtr()->WriteLogMessage( sMsg.Str() );
    dbf->GetXbasePtr()->WriteLogMessage( dbf->GetXbasePtr()->GetErrorMessage( iRc ));
  }
  return iRc;
}
/************************************************************************/
xbInt16 xbFilter::Find( const xbDouble &dKey, xbInt16 iOption ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  try{

    if( !exp ){
      iErrorStop = 100;
      throw iRc;
    }
    lCurQryCnt = 0;

    if(( iRc = dbf->Find( pIx, vpTag, dKey )) != XB_NO_ERROR ){
      if( iRc == XB_NOT_FOUND || iRc == XB_EMPTY || iRc == XB_EOF )
        return iRc;
      else{
        iErrorStop = 110;
        throw iRc;
      }
    }

    xbDouble dSaveKey = dKey;
    xbBool bFound = xbFalse;
    while( !bFound && iRc == XB_NO_ERROR && dKey == dSaveKey ){
      if(( iRc = exp->ProcessExpression()) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }
      if(( iRc = exp->GetBoolResult( bFound )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }
      if( !bFound || 
          (dbf->RecordDeleted() && iOption == XB_ACTIVE_RECS) ||
          (!dbf->RecordDeleted() && iOption == XB_DELETED_RECS)){
        if(( iRc = dbf->GetNextKey( pIx, vpTag )) != XB_NO_ERROR ){
          if( iRc == XB_EOF ){
            return iRc;
          } else {
            iErrorStop = 140;
            throw iRc;
          }
        }
      }
      if(( iRc = pIx->GetCurKeyVal( vpTag, dSaveKey )) != XB_NO_ERROR ){
        iErrorStop = 150;
        throw iRc;
      }
    }
    if( bFound )
      lCurQryCnt++;
    else
      iRc = XB_NOT_FOUND;
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbFilter::Find(xbDouble) Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    dbf->GetXbasePtr()->WriteLogMessage( sMsg.Str() );
    dbf->GetXbasePtr()->WriteLogMessage( dbf->GetXbasePtr()->GetErrorMessage( iRc ));
  }
  return iRc;
}
/************************************************************************/
xbInt16 xbFilter::GetFirstRecordIx( xbInt16 iOption ) { 

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  try{

    if( !exp ){
      iErrorStop = 100;
      throw iRc;
    }

    lCurQryCnt = 0;
    if(( iRc = dbf->GetFirstKey( pIx, vpTag )) != XB_NO_ERROR ){
      if( iRc == XB_EMPTY || iRc == XB_EOF )
        return iRc;
      else{
        iErrorStop = 110;
        throw iRc;
      }
    }

    xbBool bFound = xbFalse;
    while( !bFound && iRc == XB_NO_ERROR ){
      if(( iRc = exp->ProcessExpression()) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }
      if(( iRc = exp->GetBoolResult( bFound )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }
      if( !bFound || 
          (dbf->RecordDeleted() && iOption == XB_ACTIVE_RECS) ||
          (!dbf->RecordDeleted() && iOption == XB_DELETED_RECS)){
        if(( iRc = dbf->GetNextKey( pIx, vpTag )) != XB_NO_ERROR ){
          if( iRc == XB_EOF ){
            return iRc;
          } else {
            iErrorStop = 140;
            throw iRc;
          }
        }
      }
    }
    lCurQryCnt++;
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbFilter::GetFirstRecordIx() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    dbf->GetXbasePtr()->WriteLogMessage( sMsg.Str() );
    dbf->GetXbasePtr()->WriteLogMessage( dbf->GetXbasePtr()->GetErrorMessage( iRc ));
  }
  return iRc;
}
/************************************************************************/
xbInt16 xbFilter::GetNextRecordIx( xbInt16 iOption ){ 

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  try{

    if( !exp ){
      iErrorStop = 100;
      throw iRc;
    }

    if( lLimit != 0 && abs( lCurQryCnt ) >= lLimit )
      return XB_LIMIT_REACHED;

    if(( iRc = dbf->GetNextKey( pIx, vpTag )) != XB_NO_ERROR ){
      if( iRc == XB_EOF )
        return iRc;
      else{
        iErrorStop = 110;
        throw iRc;
      }
    }

    xbBool bFound = xbFalse;
    while( !bFound && iRc == XB_NO_ERROR ){
      if(( iRc = exp->ProcessExpression()) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }
      if(( iRc = exp->GetBoolResult( bFound )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }
      if( !bFound || 
          (dbf->RecordDeleted() && iOption == XB_ACTIVE_RECS) ||
          (!dbf->RecordDeleted() && iOption == XB_DELETED_RECS)){
        if(( iRc = dbf->GetNextKey( pIx, vpTag )) != XB_NO_ERROR ){
          if( iRc == XB_EOF ){
            return iRc;
          } else {
            iErrorStop = 140;
            throw iRc;
          }
        }
      }
    }
    lCurQryCnt++;
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbFilter::GetNextRecordIx() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    dbf->GetXbasePtr()->WriteLogMessage( sMsg.Str() );
    dbf->GetXbasePtr()->WriteLogMessage( dbf->GetXbasePtr()->GetErrorMessage( iRc ));
  }
  return iRc;
}
/************************************************************************/
xbInt16 xbFilter::GetPrevRecordIx( xbInt16 iOption ){ 

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  try{

    if( !exp ){
      iErrorStop = 100;
      throw iRc;
    }

    if( lLimit != 0 && abs( lCurQryCnt ) >= lLimit )
      return XB_LIMIT_REACHED;

    if(( iRc = dbf->GetPrevKey( pIx, vpTag )) != XB_NO_ERROR ){    
      if( iRc == XB_BOF )
        return iRc;
      else{
        iErrorStop = 110;
        throw iRc;
      }
    }

    xbBool bFound = xbFalse;
    while( !bFound && iRc == XB_NO_ERROR ){
      if(( iRc = exp->ProcessExpression()) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }
      if(( iRc = exp->GetBoolResult( bFound )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }
      if( !bFound || 
          (dbf->RecordDeleted() && iOption == XB_ACTIVE_RECS) ||
          (!dbf->RecordDeleted() && iOption == XB_DELETED_RECS)){
        if(( iRc = dbf->GetPrevKey( pIx, vpTag )) != XB_NO_ERROR ){
          if( iRc == XB_BOF ){
            return iRc;
          } else {
            iErrorStop = 140;
            throw iRc;
          }
        }
      }
    }
    lCurQryCnt--;
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbFilter::GetPrevRecordIx() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    dbf->GetXbasePtr()->WriteLogMessage( sMsg.Str() );
    dbf->GetXbasePtr()->WriteLogMessage( dbf->GetXbasePtr()->GetErrorMessage( iRc ));
  }
  return iRc;
}
/************************************************************************/
xbInt16 xbFilter::GetLastRecordIx( xbInt16 iOption ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  try{

    if( !exp ){
      iErrorStop = 100;
      throw iRc;
    }
    lCurQryCnt = 0;
    if(( iRc = dbf->GetLastKey( pIx, vpTag )) != XB_NO_ERROR ){
      if( iRc == XB_EOF )
        return iRc;
      else{
        iErrorStop = 110;
        throw iRc;
      }
    }
    xbBool bFound = xbFalse;
    while( !bFound && iRc == XB_NO_ERROR ){
      if(( iRc = exp->ProcessExpression()) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }
      if(( iRc = exp->GetBoolResult( bFound )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }
      if( !bFound || 
          (dbf->RecordDeleted() && iOption == XB_ACTIVE_RECS) ||
          (!dbf->RecordDeleted() && iOption == XB_DELETED_RECS)){
        if(( iRc = dbf->GetPrevKey( pIx, vpTag )) != XB_NO_ERROR ){
          if( iRc == XB_BOF ){
            return iRc;
          } else {
            iErrorStop = 140;
            throw iRc;
          }
        }
      }
    }
    lCurQryCnt--;
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbFilter::GetLastRecordIx() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    dbf->GetXbasePtr()->WriteLogMessage( sMsg.Str() );
    dbf->GetXbasePtr()->WriteLogMessage( dbf->GetXbasePtr()->GetErrorMessage( iRc ));
  }
  return iRc;
}
#endif  // XB_INDEX_SUPPORT
/************************************************************************/
}   /* namespace */ 
#endif         /*  XB_FILTER_SUPPORT */