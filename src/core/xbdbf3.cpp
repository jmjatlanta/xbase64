/* xbdbf3.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023,2024 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net
*/

#include "xbase.h"

#ifdef XB_DBF3_SUPPORT

namespace xb{
/************************************************************************/
//! @brief Constructor.

xbDbf3::xbDbf3(xbXBase * x) : xbDbf( x ) {
  #ifdef XB_MEMO_SUPPORT
  ulCreateMemoBlockSize = 512;
  #endif
  iFileVersion = 3;
};
/************************************************************************/
//! @brief Destructor.
xbDbf3::~xbDbf3() {};
/************************************************************************/
xbInt16 xbDbf3::CreateTable( const xbString & sTableName, const xbString & sAlias, xbSchema *pSchema, xbInt16 iOverlay, xbInt16 iShareMode ){

  xbInt16 i, k, k2;
  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;
  iDbfStatus = XB_CLOSED;

  xbString sNfn;
  try{
    sNfn = sTableName;
    xbase->GetLogStatus();

    iRc = NameSuffixMissing( sNfn, 1 );
    if( iRc == XB_NOT_FOUND )
      sNfn += ".DBF";

    SetFileName( sNfn );
    this->sAlias = sAlias;

    /* check if the file already exists */
    if( FileExists( 0 )){
      if( !iOverlay ){
        iErrorStop = 100;
        iRc = XB_FILE_EXISTS;
        throw iRc;
      }

      // remove other files if they exist
      xbString sMname = sNfn;
      xbUInt32 iMnameLen = sMname.Len();
      sMname.PutAt( iMnameLen-2, 'I' );
      sMname.PutAt( iMnameLen-1, 'N' );
      sMname.PutAt( iMnameLen,   'F' );
      xbFile fTemp( xbase );
      fTemp.SetFileName( sMname );
      if( fTemp.FileExists() )
        fTemp.xbRemove();

      sMname.PutAt( iMnameLen-2, 'D' );
      sMname.PutAt( iMnameLen-1, 'B' );
      sMname.PutAt( iMnameLen,   'T' );
      fTemp.SetFileName( sMname );
      if( fTemp.FileExists() )
        fTemp.xbRemove();

      sMname.PutAt( iMnameLen-2, 'M' );
      sMname.PutAt( iMnameLen-1, 'D' );
      sMname.PutAt( iMnameLen,   'X' );
      fTemp.SetFileName( sMname );
      if( fTemp.FileExists() )
        fTemp.xbRemove();
    }

    /* check if we already have a file with this alias  */
    if(( iRc = xbase->AddTblToTblList( this, GetFqFileName(), sAlias )) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }

    iRc = ValidateSchema( pSchema );
    if( iRc < 0 ){
      iErrorStop = 120;
      throw iRc;
    } else
      iNoOfFields = iRc;

    #ifdef XB_MEMO_SUPPORT
    // if we have memo fields
    iMemoFieldCnt = 0;
    i = 0;
    while( pSchema[i].cType != 0){
      if( pSchema[i].cType == 'M' )
        iMemoFieldCnt++;         /* number of memo fields in the incoming definition */
      i++;
    }

    if( iMemoFieldCnt > 0 ){
      xbString sMfn = sNfn;        /* memo file name, same as filename except ends with a "t", not an "f" */
      xbUInt32 iMfnLen = sMfn.Len();
      sMfn.PutAt( iMfnLen, 'T' );

      // dont overlay the memo file if it exists, and Overlay switch is off
      xbFile fTemp( xbase );
      fTemp.SetFileName( sMfn );
      if( fTemp.FileExists() && !iOverlay ){
        iErrorStop = 130;
        iRc = XB_FILE_EXISTS;
        throw iRc;
      }

      Memo = new xbMemoDbt3( this, fTemp.GetFqFileName());

      if(( iRc = Memo->CreateMemoFile()) != XB_NO_ERROR ){
         iErrorStop = 140;
         throw iRc;
      }
    }
    #endif

    /* this is the dBASE III version of the class */
    cVersion = 0x03;     // 0x03 for dBASE level 5
    #ifdef XB_MEMO_SUPPORT
    if( iMemoFieldCnt > 0 ){
//    cVersion = cVersion |= 0x80;   // Version III memo, compiler complaints
      cVersion |= 0x80;   // Version III memo
    }
    #endif

    if(( iRc = xbFopen( "w+b", iShareMode )) != XB_NO_ERROR ){
      iErrorStop = 150;
      throw iRc;
    }
    uiRecordLen++;                  /* add one byte for 0x0D    */

    if(( RecBuf = (char *) malloc( uiRecordLen )) == NULL ){
      iErrorStop = 160;
      throw iRc;
    }

    if(( RecBuf2 = (char *) malloc( uiRecordLen )) == NULL ){
      iErrorStop = 170;
      iRc = XB_NO_MEMORY;
      throw iRc;
    }

    /* BlankRecord(); */
    memset( RecBuf, 0x20, uiRecordLen );
    memset( RecBuf2, 0x20, uiRecordLen );
    ulCurRec  = 0L;
    uiHeaderLen = 33 + iNoOfFields * 32;
    xbDate d;
    d.Sysdate();
    cUpdateYY = (char) (d.YearOf() - 1900);
    cUpdateMM = (char) d.MonthOf();
    cUpdateDD = (char) d.DayOf( XB_FMT_MONTH );

    /* write the header prolog */
    if(( iRc = WriteHeader( 0, 0 )) != XB_NO_ERROR ){
      iErrorStop = 180;
      iRc = XB_WRITE_ERROR;
      throw iRc;
    }
    if((SchemaPtr = (xbSchemaRec *) malloc( (size_t) iNoOfFields * sizeof( xbSchemaRec ))) == NULL){
      iErrorStop = 190;
      iRc = XB_NO_MEMORY;
      throw iRc;
    }

    /* write the field information into the header */
    for( i = 0, k = 1; i < iNoOfFields; i++ ){

      memset( SchemaPtr[i].cFieldName, 0x00, 11 );
      for( int x = 0; x < 10 && pSchema[i].cFieldName[x]; x++ )
        SchemaPtr[i].cFieldName[x] = pSchema[i].cFieldName[x];

      SchemaPtr[i].cType = pSchema[i].cType;
      SchemaPtr[i].cFieldLen = (unsigned char) pSchema[i].iFieldLen;
      SchemaPtr[i].cNoOfDecs = (unsigned char) pSchema[i].iNoOfDecs;

      if( SchemaPtr[i].cNoOfDecs > SchemaPtr[i].cFieldLen ){
        iErrorStop = 200;
        iRc = XB_WRITE_ERROR;
        throw iRc;
      }

      k2 = k;
      k += SchemaPtr[i].cFieldLen;

      if(( xbFwrite( &SchemaPtr[i].cFieldName, 1, 11 )) != XB_NO_ERROR ) {
        iErrorStop = 210;
        iRc = XB_WRITE_ERROR;
        throw iRc;
      }

      if(( xbFwrite( &SchemaPtr[i].cType, 1, 1 )) != XB_NO_ERROR ) {
        iErrorStop = 220;
        iRc = XB_WRITE_ERROR;
        throw iRc;
      }

      for( int j = 0; j < 4; j++ )
        xbFputc( 0x00 );

      if(( xbFwrite( &SchemaPtr[i].cFieldLen, 1, 1 )) != XB_NO_ERROR ) {
        iErrorStop = 230;
        iRc = XB_WRITE_ERROR;
        throw iRc;
      }

      if(( xbFwrite( &SchemaPtr[i].cNoOfDecs, 1, 1 )) != XB_NO_ERROR ) {
        iErrorStop = 240;
        iRc = XB_WRITE_ERROR;
        throw iRc;
      }

      /* 14 bytes reserved */
      for( int j = 0; j < 14; j++ )
        xbFputc( 0x00 );

      SchemaPtr[i].pAddress  = RecBuf  + k2;
      SchemaPtr[i].pAddress2 = RecBuf2 + k2;
    }

    /* write the header terminator */
    if(( xbFputc( XB_CHARHDR )) != XB_NO_ERROR ){
      iErrorStop = 250;
      iRc = XB_WRITE_ERROR;
      throw iRc;
    }
  }
  catch( xbInt16 iRc )
  {
    xbString sMsg;
    sMsg.Sprintf( "xbdbf3::CreateTable() Exception Caught Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));

    sMsg.Sprintf( "Table Name = [%s]", GetFqFileName().Str());
    xbase->WriteLogMessage( sMsg );   
    sMsg.Sprintf( "Alias Name = [%s]", sAlias.Str());
    xbase->WriteLogMessage( sMsg );

    xbFclose();
    if( RecBuf ){
      free( RecBuf );
      RecBuf = NULL;
    }
    if( RecBuf2 ){
      free( RecBuf2 );
      RecBuf2 = NULL;
    }
    if( SchemaPtr ){
      free( SchemaPtr );
      SchemaPtr = NULL;
    }

    InitVars();
    if( iRc != XB_FILE_EXISTS )
      xbase->RemoveTblFromTblList( sAlias );
  }

  if( iRc == XB_NO_ERROR )
    iDbfStatus = XB_OPEN;

  return iRc;
}

/************************************************************************/
xbInt16 xbDbf3::GetVersion() const {
  return 3;
}
/************************************************************************/
xbInt16 xbDbf3::Open( const xbString & sTableName, const xbString & sAlias, 
                         xbInt16 iOpenMode, xbInt16 iShareMode ){
  xbInt16 i, j, iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  char buf[33];
  char *p;

  #ifdef XB_MEMO_SUPPORT
  iMemoFieldCnt = 0;
  #endif

  try{
    /* verify the file is not already open */
    if( iDbfStatus != XB_CLOSED ){
      iErrorStop = 100;
      iRc = XB_ALREADY_OPEN;
      throw iRc;
    }
    /* copy the file name to the class variable */
    SetFileName( sTableName );
    this->sAlias = sAlias;

    if( !FileExists()){
      iErrorStop = 110;
      iRc = XB_FILE_NOT_FOUND;
      throw iRc;
    }

    if(( iRc = xbase->AddTblToTblList( this, GetFqFileName(), sAlias )) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw iRc;
    }

    /* open the file */
    if(( iRc = xbFopen( iOpenMode, iShareMode )) != XB_NO_ERROR ){
      iErrorStop = 130;
      throw iRc;
    }

    /* copy the header into memory */
    if(( iRc = ReadHeader( 1, 0 )) != XB_NO_ERROR ){
      iErrorStop = 140;
      throw iRc;
    }

    /* check the version */
    if(( cVersion & 0x07 ) != 3 ){
    // if( xFileVersion != 3 && xFileVersion != 4 ){ 
      iErrorStop = 150;
      iRc = XB_FILE_TYPE_NOT_SUPPORTED;
      throw iRc;
    }
    iFileVersion = 3;

    /* calculate the number of fields */
    if( cVersion == (char)0x30 ) {
      iNoOfFields = ( uiHeaderLen - 296 ) / 32 ;
    } else {
      iNoOfFields = ( uiHeaderLen - 33 ) / 32;
    }

    if(( RecBuf = (char *) malloc( (size_t) uiRecordLen )) == NULL ){
      iErrorStop = 160;
      iRc = XB_NO_MEMORY;
      throw iRc;
    }

    if(( RecBuf2 = (char *) malloc( uiRecordLen )) == NULL ) {
      iErrorStop = 170;
      iRc = XB_NO_MEMORY;
      throw iRc;
    }

    if((SchemaPtr=(xbSchemaRec *)malloc( (size_t) iNoOfFields * sizeof( xbSchemaRec ))) == NULL){
      iErrorStop = 180;
      iRc = XB_NO_MEMORY;
      throw iRc;
    }

    memset( SchemaPtr, 0x00, ( (size_t) iNoOfFields * sizeof(xbSchemaRec) ));
    /* copy field info into memory */
    for( i = 0, j = 1; i < iNoOfFields; i++ ){
      xbFseek( ((xbInt64)i*32+32), SEEK_SET );
      xbFread( &buf, 1, 32 );
      p = buf;
      for( int x = 0; x < 10 && buf[x]; x++ ){
        SchemaPtr[i].cFieldName[x] = buf[x];
      }
      p = buf + 11;
      SchemaPtr[i].cType = *p++;
      SchemaPtr[i].pAddress  = RecBuf + j;
      SchemaPtr[i].pAddress2 = RecBuf2 + j;
      SchemaPtr[i].cFieldLen = (unsigned char) *( p + 4 );
      SchemaPtr[i].cNoOfDecs = (unsigned char) *( p + 5 );
      j += SchemaPtr[i].cFieldLen;
      #ifdef XB_MEMO_SUPPORT
      if( (SchemaPtr[i].cType == 'M' || SchemaPtr[i].cType == 'B' || SchemaPtr[i].cType == 'O' ))
        iMemoFieldCnt++;
      #endif
    }
    ulCurRec = 0L;
    iDbfStatus = XB_OPEN;
    if(( iRc = BlankRecord()) != XB_NO_ERROR ){
      iErrorStop = 190;
      throw iRc;
    }

    #ifdef XB_MEMO_SUPPORT
    if( iMemoFieldCnt > 0 ){   /* does this table have memo fields ? */

      // build the file name
      xbString sMfn   = GetFqFileName();        /* memo file name, same as filename except ends with a "t", not an "f" */
      xbUInt32 ulMfnLen = sMfn.Len();
      if( sMfn[ulMfnLen] == 'F' )
        sMfn.PutAt( ulMfnLen,  'T' );
      else
        sMfn.PutAt( ulMfnLen, 't' );
      xbFile fTemp( xbase );
      fTemp.SetFileName( sMfn );;

      Memo = new xbMemoDbt3( this, fTemp.GetFqFileName());

      if(( iRc = Memo->OpenMemoFile()) != XB_NO_ERROR ){
        iErrorStop = 200;
        throw iRc;
      }
    }
    #endif // XB_MEMO_SUPPORT

  }
  catch ( xbInt16 iRc )
  {
    xbString sMsg;
    sMsg.Sprintf( "xbdbf3::Open() Exception Caught Error Stop = [%d] iRc = [%d] ShareMode = [%d] OpenMode = [%d]", iErrorStop, iRc, iShareMode, iOpenMode );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
    xbFclose();
    if( RecBuf ){
      free( RecBuf );
      RecBuf = NULL;
    }
    if( RecBuf2 ){
      free( RecBuf2 );
      RecBuf2 = NULL;
    }
    if( SchemaPtr ){
      free( SchemaPtr );
      SchemaPtr = NULL;
    }
    InitVars();

    #ifdef XB_MEMO_SUPPORT
    if( Memo ){
      Memo->CloseMemoFile();
      delete Memo;
      Memo = NULL;
    }
    #endif  // XB_MEMO_SUPPORT
  }

  if( iRc == XB_NO_ERROR )
    iDbfStatus = XB_OPEN;

  return iRc;
}
/************************************************************************/
xbInt16 xbDbf3::Rename( const xbString sNewName ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;

  xbString sNewDbf;

  xbBool bDbfRenamed = xbFalse;

  #ifdef XB_INF_SUPPORT
  xbString sNewInf;
  xbString sThisInf;
  xbBool bInfRenamed = xbFalse;
  #endif

  #ifdef XB_MEMO_SUPPORT
  xbString sNewDbt;
  xbString sThisDbt;
  xbBool bDbtRenamed = xbFalse;
  #endif // XB_MEMO_SUPPORT

  #ifdef XB_LOCKING_SUPPORT
  xbBool bLocked = xbFalse;
  #endif

  try{

    xbString sDir;
    xbString sFile;
    xbString sExt;

    xbString sNewNameWoExt;
    sNewNameWoExt.Set( sNewName );
    if( sNewName.Pos( ".DBF" ) > 0 )
      sNewNameWoExt.Left( sNewName.Len() - 4 );

    GetFileDirPart ( sDir );
    GetFileNamePart( sFile );
    GetFileExtPart ( sExt );

    if( FileExists( sNewDbf )) return XB_FILE_EXISTS;
    sNewDbf.Sprintf( "%s%s.DBF", sDir.Str(), sNewNameWoExt.Str());

    #ifdef XB_MEMO_SUPPORT
    sNewDbt.Sprintf( "%s%s.DBT", sDir.Str(), sNewNameWoExt.Str());
    if( FileExists( sNewDbt )) return XB_FILE_EXISTS;
    sThisDbt.Sprintf( "%s%s.DBT", sDir.Str(), sFile.Str());
    #endif

    #ifdef XB_INF_SUPPORT
    sNewInf.Sprintf( "%s%s.INF", sDir.Str(), sNewNameWoExt.Str());
    if( FileExists( sNewInf )) return XB_FILE_EXISTS;
    sThisInf.Sprintf( "%s%s.INF", sDir.Str(), sFile.Str());
    #endif // XB_INF_SUPPORT

    #ifdef XB_LOCKING_SUPPORT
    //if( GetAutoLock() && GetTableLocked() ){
    if( GetMultiUser() && GetTableLocked() ){
      if(( iRc = LockTable( XB_LOCK )) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      }
      bLocked = xbTrue;
    }
    #endif

    xbInt16 iOpenMode  = GetOpenMode();
    xbInt16 iShareMode = GetShareMode();
    xbBool  bWasOpen   = xbFalse;
    if( FileIsOpen() ){
      bWasOpen = xbTrue;
      if(( iRc = xbFclose()) != XB_NO_ERROR ){
        iErrorStop = 110;
        throw iRc;
      }
    }

    if(( iRc = xbRename( GetFqFileName().Str(), sNewDbf.Str())) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw iRc;
    } else {
      bDbfRenamed = xbTrue;
    }
    xbString sNameWext;
    sNameWext.Sprintf( "%s.DBF", sNewNameWoExt.Str());
    SetFileName( sNameWext );

    if( bWasOpen ){
      if(( iRc = xbFopen( iOpenMode, iShareMode )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }
    }

    #ifdef XB_MEMO_SUPPORT
    if( FileExists( sThisDbt )){
      if( bWasOpen ){
        if(( iRc = Memo->xbFclose()) != XB_NO_ERROR ){
          iErrorStop = 140;
          throw iRc;
        }
      }
      if(( xbRename( sThisDbt.Str(), sNewDbt.Str())) != XB_NO_ERROR ){
        iErrorStop = 150;
        throw iRc;
      }
      Memo->SetFileName( sNewDbt );
      if( bWasOpen ){
        if(( iRc = Memo->xbFopen( iOpenMode, iShareMode )) != XB_NO_ERROR ){
          iErrorStop = 160;
          throw iRc;
        }
      }
      bDbtRenamed = xbTrue;
    }
    #endif   // XB_MEMO_SUPPORT

    #ifdef XB_INF_SUPPORT
    if( FileExists( sThisInf )){
      if(( iRc = xbRename( sThisInf.Str(), sNewInf.Str())) != XB_NO_ERROR ){
        iErrorStop = 170;
        throw iRc;
      } else {
        bInfRenamed = xbTrue;
      }
    }
    #endif  // XB_INF_SUPPORT

    // rname the table in the table list
    xbTblList *tle = xbase->GetTblListEntry( this );
    if( tle ){
      // std::cout << "setting [" << GetFqFileName().Str() << "][" << sNewNameWoExt.Str() << "]\n";
      tle->psFqTblName->Set( GetFqFileName().Str());
      tle->psTblAlias->Set( sNewNameWoExt.Str());
    }
  }
  catch ( xbInt16 iRc )
  {
    xbString sMsg;
    sMsg.Sprintf( "xbdbf3::Rename() Exception Caught Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));

    // attempt to reverse things out if unsuccessful
    if( bDbfRenamed ){
      #ifdef XB_MEMO_SUPPORT
      if( bDbtRenamed ){
        xbRename( sNewDbt.Str(), sThisDbt.Str());
      }
      #endif // XB_MEMO_SUPPORT
      #ifdef XB_INF_SUPPORT
      if( bInfRenamed ){
        xbRename( sNewInf.Str(), sNewInf.Str());
      }
      #endif  // XB_INF_SUPPORT
    }

    #ifdef XB_LOCKING_SUPPORT
    if( bLocked ){
      LockTable( XB_UNLOCK );
    }
    #endif

  }
  return iRc;
}

/************************************************************************/
#ifdef XB_MEMO_SUPPORT
xbInt16 xbDbf3::SetCreateMemoBlockSize( xbUInt32 ulBlockSize ){

  if( ulBlockSize != 512 )
    return XB_INVALID_BLOCK_SIZE;
  else
    ulCreateMemoBlockSize = 512;

  return XB_NO_ERROR;
}
#endif

/************************************************************************/
xbInt16 xbDbf3::SetVersion() {
   iFileVersion = 3;
   return iFileVersion;
}
/************************************************************************/
xbInt16 xbDbf3::ValidateSchema( xbSchema * s ){

  xbInt16 iFieldCnt = 0;
  uiRecordLen = 0;

  /* count the number of fields and check paramaters */
  xbInt16 i = 0;
  while( s[i].cType != 0 ){
    iFieldCnt++;
    // Version 3 field types
    if( s[i].cType != 'C' &&
        s[i].cType != 'N' &&
        s[i].cType != 'D' &&
        #ifdef XB_MEMO_SUPPORT
        s[i].cType != 'M' &&
        #endif // XB_MEMO_SUPPORT 
        s[i].cType != 'L' ){
      return XB_INVALID_FIELD_TYPE;
    }

    if(s[i].cType == 'D'){
      s[i].iFieldLen = 8;
      s[i].iNoOfDecs = 0;
    }

    else if(s[i].cType == 'C')
      s[i].iNoOfDecs = 0;

    // check for numeric fields which are too long
    else if( s[i].cType == 'N'  && s[i].iFieldLen > 19 ){
      return XB_INVALID_FIELD_LEN;
    }
    // field len must be >= no of decimals
    else if( s[i].cType == 'N'  && s[i].iFieldLen < s[i].iNoOfDecs ){
      return XB_INVALID_FIELD_LEN;
    }

    #ifdef XB_MEMO_SUPPORT
    else if(s[i].cType == 'M'){
      s[i].iFieldLen = 10;
      s[i].iNoOfDecs = 0;
    }
    #endif // XB_MEMO_SUPPORT

    uiRecordLen += s[i].iFieldLen;
    i++;
  }
  return iFieldCnt;
}

}        /* namespace        */
#endif   /* XB_DBF3_SUPPORT  */