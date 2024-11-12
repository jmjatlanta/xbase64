/* xbtblmgr.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

#include "xbase.h"

namespace xb{

/*************************************************************************/
xbTblMgr::xbTblMgr(){
  TblList = NULL;
  iOpenTableCount = 0;

  #ifdef XB_RELATE_SUPPORT
  pRelList = NULL;
  #endif

}

/*************************************************************************/
xbTblMgr::~xbTblMgr(){

  // clear list of tables
  xbTblList *l;
  if( TblList ){
    while( TblList ){
      l = TblList;
      TblList = TblList->pNext;
      delete l->psFqTblName;
      delete l->psTblName;
      delete l->psTblAlias;
      free( l );
    }
  }

  // clear list of relations
  #ifdef XB_RELATE_SUPPORT
  xbRelList *pRl  = pRelList;
  xbRelList *pRlN = pRelList;
  while( pRl ){
    pRlN = pRl->pNext;
    free( pRl );
    pRl = pRlN;
  }
  #endif

}

/*************************************************************************/
xbInt16 xbTblMgr::AddTblToTblList( xbDbf *d, const xbString & sFqTblName ){
  return AddTblToTblList( d, sFqTblName, "" );
}

/*************************************************************************/
xbInt16 xbTblMgr::AddTblToTblList( xbDbf *d, const xbString & sFqTblName, const xbString & sTblAlias ) {

  // Set the Fq (fully qualified name)
  // Pull the table name from the FQ name
  // Set the Alias to the table name if the alias name is not provided


  xbTblList *i, *s, *t;
  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;
  xbString sTblName;
  xbString sAlias;
  xbString sTemp;
  xbString sFqTemp;
  xbUInt32 iSlashPos;


  try{
    if( sFqTblName.Len() == 0 ){
      iErrorStop = 100;
      iRc = XB_INVALID_TABLE_NAME;
      throw iRc;
    }

    sTblName = sFqTblName;
    sTblName.SwapChars( '\\', '/' );
    iSlashPos = sTblName.GetLastPos( '/' );

    if( iSlashPos > 0 ){
      sTblName.Ltrunc( iSlashPos );           // remove the directory part from the table name
      sFqTemp = sFqTblName;
    } else{
//      sFqTemp.Sprintf( "%s%s", GetDataDirectory().Str(), sFqTblName.Str());   // add the dir part to the FQ name
      sFqTemp.Sprintf( "%s%s", sDataDirectory.Str(), sFqTblName.Str());   // add the dir part to the FQ name
    }

    xbUInt32 iDbfPos = sFqTemp.Pos( ".DBF" );
    if( iDbfPos == 0 )
      sFqTemp += ".DBF";
    else
      sTblName.Resize( sTblName.Len() - 3 );

    if( sTblAlias.Len() == 0 )
      sAlias = sTblName;
    else
      sAlias = sTblAlias;

    if((i = (xbTblList *) calloc(1, sizeof(xbTblList))) == NULL){
      iErrorStop = 110;
      iRc = XB_NO_MEMORY;
      throw iRc;
    }
    i->psFqTblName  = new xbString( sFqTemp );
    i->psTblName    = new xbString( sTblName );
    i->psTblAlias   = new xbString( sAlias );
    i->pDbf         = d;
    i->pNext        = NULL;

    // insert new table into the list of open tables, sorted by table name 
    s = NULL;
    t = TblList;

    while(t && (strcmp( t->psTblAlias->Str(), sAlias.Str()) < 0 )){
      s = t;
      t = t->pNext;
    }

    if( t && (strcmp( t->psTblAlias->Str(), sAlias.Str()) == 0 )){
      iErrorStop = 120;
      delete i->psFqTblName;
      delete i->psTblName;
      delete i->psTblAlias;
      free( i );
      iRc = XB_DUP_TABLE_OR_ALIAS;
      throw iRc;
    }
    i->pNext = t;
    if (s == NULL)
      TblList = i;
    else
      s->pNext = i;
  }
  catch (xbInt16 iRc ){
    if( iErrorStop != 120 ){
      xbString sMsg;
      sMsg.Sprintf( "xbTblMgr::AddTblToTblList() Exception Caught. Error Stop = [%d] iRc = [%d] Tbl Name = [%s] Alias = [%s]", iErrorStop, iRc, sTblName.Str(), sTblAlias.Str() );
    }
  }
  if( iRc == XB_NO_ERROR )
    iOpenTableCount++;
  return iRc;
}

/*************************************************************************/
xbInt16 xbTblMgr::DisplayTableList() const {
  xbInt16 iTblCnt = 0;
  xbTblList * l = TblList;
  std::cout << "-- Open Table List --" << std::endl;
  if( l == NULL )
    std::cout << "Table list is empty" << std::endl;
  else{
    while( l ){
      iTblCnt++;
      std::cout << iTblCnt << " FqFileName=[" << l->psFqTblName->Str() << "]  TableName=[" << l->psTblName->Str() << "] Alias=[" << l->psTblAlias->Str() << "]" << std::endl;
      l = l->pNext;
    }
  }
  return iTblCnt;
}
/*************************************************************************/
/* Get pointer to named dbf.
  Looks up an open DBF file by Name.

  returns A pointer to the xbDbf class instance if found or NULL if not found.

  // looks for a match as an alias first, if not found as an alias, looks at the name

*/

xbDbf *xbTblMgr::GetDbfPtr(const xbString& sTblAlias) const {


  xbTblList *t;
  t = TblList;
  xbString s;
  xbUInt32 ui = sTblAlias.Pos( ".DBF" );
  if( ui > 0 )
    s.Assign( sTblAlias.Str(), 1, ui - 1 );
  else
    s.Set( sTblAlias );

  while( t ){
    if( s == t->psTblAlias->Str()){
       return t->pDbf;
    }
    t = t->pNext;
  }

  t = TblList;
  while( t ){
    //  std::cout << "s = [" << s.Str() << "] tbl name = [" << t->psTblName->Str() << "]\n";
    if( s == t->psTblName->Str()){
      return t->pDbf;
    }
    t = t->pNext;
  }


  t = TblList;
  while( t ){
    if( sTblAlias == t->psFqTblName->Str())
      return t->pDbf;
    t = t->pNext;
  }
  return NULL;
}
/*************************************************************************/
/* Get pointer to named dbf.
  Looks up an open DBF file by Name.

  returns pointer to the xbDbf class instance if found or NULL if not found.
*/

xbDbf *xbTblMgr::GetDbfPtr(xbInt16 iItemNo ) const {

  xbTblList *t;
  t = TblList;
  xbInt16 iCnt = 1;

  if( iItemNo < 1 || iItemNo > iOpenTableCount )
    return NULL;

  while( t && iCnt < iItemNo ){
    t = t->pNext;
    iCnt++;
  }
  if( t )
    return t->pDbf;
  else
    return NULL;
}
/*************************************************************************/
xbInt16 xbTblMgr::GetOpenTableCount() const {
  return iOpenTableCount;
}
/*************************************************************************/
xbTblList * xbTblMgr::GetTblListEntry( xbDbf *pTbl ){

  xbTblList * i = TblList;
  while( i ){
    if( i->pDbf == pTbl ) 
      return i;
    i = i->pNext;
  }
  return NULL;
}
/*************************************************************************/
xbInt16 xbTblMgr::RemoveTblFromTblList( const xbString & sTblAlias ) {
  xbTblList *i, *s;

  i = TblList;
  s = NULL;

  while( i ){

    if( strcmp( i->psTblAlias->Str(), sTblAlias.Str()) == 0 ) {
      if(s)
        s->pNext = i->pNext;
      else
        TblList = i->pNext;

      delete i->psFqTblName;
      delete i->psTblName;
      delete i->psTblAlias;
      free( i );
      iOpenTableCount--;
      return XB_NO_ERROR;
    } else {
      s = i;
      i = i->pNext;
    }
  }
  return XB_NOT_FOUND;
}
/*************************************************************************/
xbInt16 xbTblMgr::RemoveTblFromTblList( xbDbf *pTbl ) {
  xbTblList *i, *s;

  i = TblList;
  s = NULL;

  while( i ){

    if( i->pDbf == pTbl ) {
      if(s)
        s->pNext = i->pNext;
      else
        TblList = i->pNext;

      delete i->psFqTblName;
      delete i->psTblName;
      delete i->psTblAlias;
      free( i );
      iOpenTableCount--;
      return XB_NO_ERROR;
    } else {
      s = i;
      i = i->pNext;
    }
  }
  return XB_NOT_FOUND;
}

/*************************************************************************/
#ifdef XB_RELATE_SUPPORT
xbInt16 xbTblMgr::AddRelToRelList( xbRelate *pRelI ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  try{

    // verify structure doesn't already exist in list
    xbRelList *pRl = pRelList;

    while( pRl ){
      if( pRl->pRel == pRelI ){
        iErrorStop = 100;
        iRc = XB_ALREADY_DEFINED;
        throw iRc;
      }
      pRl = pRl->pNext;
    }

    xbRelList *pR;
    if(( pR = (xbRelList *) calloc(1, sizeof(xbRelList))) == NULL){
      iErrorStop = 110;
      iRc = XB_NO_MEMORY;
      throw iRc;
    }
    pR->pNext = pRelList;
    pR->pRel  = pRelI;
    pRelList  = pR;

  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbTblMgr::AddRelToRelList() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
  }
  return iRc;
}
/*************************************************************************/
void xbTblMgr::RemoveRelFromRelList( xbRelate *pRelI ){

  xbBool bDone        = xbFalse;
  xbRelList *pRl      = pRelList;
  xbRelList *pPrevRel = NULL;

  while( pRl && !bDone ){
    if( pRl->pRel == pRelI ){

      if( pPrevRel )
        pPrevRel->pNext = pRl->pNext;
      else
        pRelList        = pRl->pNext;

      bDone = xbTrue;
      free( pRl );

    } else {
      pRl = pRl->pNext;

    }
  }
}
/*************************************************************************/
xbInt16 xbTblMgr::CheckRelListForTblUse( xbDbf *d ){

  // returns 
  //   0 if not not found
  //   XB_ALREADY_DEFINED if found

  xbRelList  *pRl   = pRelList;
  xbRelate   *pRelate;
  xbInt16 iRc = XB_NO_ERROR;

  while( pRl ){
    pRelate = pRl->pRel;
    if(( iRc = pRelate->CheckTblSts( d )) != XB_NO_ERROR )
      return iRc;
    pRl = pRl->pNext;
  }
  return iRc;
}
#endif    // XB_RELATE_SUPPORT

/*************************************************************************/
}   /* namespace */
