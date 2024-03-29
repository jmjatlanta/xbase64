/* xbdbf4.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

#include "xbase.h"


#ifdef XB_DBF4_SUPPORT

namespace xb{


/************************************************************************/
//! @brief Constructor.
xbDbf4::xbDbf4(xbXBase * x) : xbDbf( x ) {

  iFileVersion = 4;
  #ifdef XB_MEMO_SUPPORT
  ulCreateMemoBlockSize = 1024;
  #endif
};

/************************************************************************/
//! @brief Destructor.
xbDbf4::~xbDbf4() {};

/************************************************************************/
//! @brief Create Version 4 table.
/*!
    This routine creates a Dbase IV (tm) DBF file.

  \param sTableName DBF table name.
  \param sAlias Table alias
  \param pSchema Pointer to schema structure with field definitions.
  \param iOverlay xbTrue - Overlay.<br> xbFalse - Don't overlay.
  \param iShareMode XB_SINGLE_USER<br>XB_MULTI_USER
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbDbf4::CreateTable( const xbString &sTableName, const xbString &sAlias, xbSchema * pSchema, xbInt16 iOverlay, xbInt16 iShareMode ){

  xbInt16 i, k, k2;
  xbInt16 rc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  iDbfStatus = XB_CLOSED;

  xbString sNfn;

  try{
    sNfn = sTableName;
    xbase->GetLogStatus();

    rc = NameSuffixMissing( sNfn, 1 );
    if( rc > 0 )
      sNfn += ".DBF";

    SetFileName( sNfn );
    this->sAlias = sAlias;

    /* check if the file already exists */
    if( FileExists( 0 )){
      if( !iOverlay ){
        iErrorStop = 100;
        rc = XB_FILE_EXISTS;
        throw rc;
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
    if(( rc = xbase->AddTblToTblList( this, GetFqFileName(), sAlias )) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw rc;
    }

    rc = ValidateSchema( pSchema );
    if( rc < 0 ){
      iErrorStop = 120;
      throw rc;
    } else
      iNoOfFields = rc;

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
      xbUInt32 ulMfnLen = sMfn.Len();
      sMfn.PutAt( ulMfnLen, 'T' );

      // dont overlay the memo file if it exists, and Overlay switch is off
      xbFile fTemp( xbase );
      fTemp.SetFileName( sMfn );
      if( fTemp.FileExists() && !iOverlay ){
        iErrorStop = 130;
        rc = XB_FILE_EXISTS;
        throw rc;
      }

      Memo = new xbMemoDbt4( this, fTemp.GetFqFileName());

      if(( rc = Memo->CreateMemoFile()) != XB_NO_ERROR ){
         iErrorStop = 140;
         throw rc;
      }
    }
    #endif

    /* this is the dBase IV version of the class */
    cVersion = 0x03;    // 0x03 for Dbase level 5
    #ifdef XB_MEMO_SUPPORT
    if( iMemoFieldCnt > 0 ){
//    cVersion = cVersion |= 0x88;   // version IV memos, compiler complains about this
      cVersion |= 0x88;   // version IV memos
    }
    #endif

    if(( rc = xbFopen( "w+b", iShareMode )) != XB_NO_ERROR ){
      iErrorStop = 150;
      throw rc;
    }
    uiRecordLen++;                  /* add one byte for 0x0D    */

    if(( RecBuf = (char *) malloc( uiRecordLen )) == NULL ){
      iErrorStop = 160;
      throw rc;
    }

    if(( RecBuf2 = (char *) malloc( uiRecordLen )) == NULL ){
      iErrorStop = 170;
      rc = XB_NO_MEMORY;
      throw rc;
    }

    /* BlankRecord(); */
    memset( RecBuf, 0x20, uiRecordLen );
    memset( RecBuf2, 0x20, uiRecordLen );
    ulCurRec  = 0L;
    uiHeaderLen = 33 + iNoOfFields * 32;
    xbDate d;
    d.Sysdate();
    cUpdateYY  = (char) (d.YearOf() - 1900);
    cUpdateMM  = (char) d.MonthOf();
    cUpdateDD  = (char) d.DayOf( XB_FMT_MONTH );
    cIndexFlag = 0;
    // Default language driver to 0x1b
    cLangDriver = 0x1b;

    /* write the header prolog */
    if(( rc = WriteHeader( 0, 0 )) != XB_NO_ERROR ){
      iErrorStop = 180;
      rc = XB_WRITE_ERROR;
      throw rc;
    }
    if((SchemaPtr=(xbSchemaRec *)malloc( (size_t) iNoOfFields * sizeof( xbSchemaRec ))) == NULL){
      iErrorStop = 190;
      rc = XB_NO_MEMORY;
      throw rc;
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
        iErrorStop = 110;
        rc = XB_WRITE_ERROR;
        throw rc;
      }

      k2 = k;
      k += SchemaPtr[i].cFieldLen;

      if(( xbFwrite( &SchemaPtr[i].cFieldName, 1, 11 )) != XB_NO_ERROR ) {
        iErrorStop = 200;
        rc = XB_WRITE_ERROR;
        throw rc;
      }

      if(( xbFwrite( &SchemaPtr[i].cType, 1, 1 )) != XB_NO_ERROR ) {
        iErrorStop = 210;
        rc = XB_WRITE_ERROR;
        throw rc;
      }

      for( int j = 0; j < 4; j++ )
        xbFputc( 0x00 );

      if(( xbFwrite( &SchemaPtr[i].cFieldLen, 1, 1 )) != XB_NO_ERROR ) {
        iErrorStop = 220;
        rc = XB_WRITE_ERROR;
        throw rc;
      }

      if(( xbFwrite( &SchemaPtr[i].cNoOfDecs, 1, 1 )) != XB_NO_ERROR ) {
        iErrorStop = 230;
        rc = XB_WRITE_ERROR;
        throw rc;
      }

      /* 14 bytes reserved */
      for( int j = 0; j < 14; j++ )
        xbFputc( 0x00 );

      SchemaPtr[i].pAddress  = RecBuf  + k2;
      SchemaPtr[i].pAddress2 = RecBuf2 + k2;
    }

    /* write the header terminator */
    if(( xbFputc( XB_CHARHDR )) != XB_NO_ERROR ){
      iErrorStop = 240;
      rc = XB_WRITE_ERROR;
      throw rc;
    }
  }
  catch( xbInt16 rc )
  {
    xbString sMsg;
    sMsg.Sprintf( "xbdbf4::CreateTable() Exception Caught Error Stop = [%d] rc = [%d]", iErrorStop, rc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( GetErrorMessage( rc ));

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
    if( rc != XB_FILE_EXISTS )
      xbase->RemoveTblFromTblList( sAlias );
  }

  if( rc == XB_NO_ERROR )
    iDbfStatus = XB_OPEN;

  return rc;
}

/************************************************************************/
//! @brief Get version.
/*!
   The version info can be retrieved to determine
   which class is being used for a given dbf instance.
   \returns 4
*/

xbInt16 xbDbf4::GetVersion() const {
  return 4;
}

/************************************************************************/
//! @brief Open dbf file/table.
/*!
  \param sTableName DBF table name.
  \param sAlias Table alias
  \param iOpenMode XB_READ<br>XB_READ_WRITE
  \param iShareMode XB_SINGLE_USER<br>XB_MULTI_USER
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbDbf4::Open( const xbString & sTableName, const xbString & sAlias, 
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
    //if(( xFileVersion = DetermineXbaseTableVersion( cVersion )) != 4 ){
    if(( cVersion & 0x07 ) != 3 ){
      iErrorStop = 150;
      iRc = XB_FILE_TYPE_NOT_SUPPORTED;
      throw iRc;
    }
    iFileVersion = 4;

    /* calculate the number of fields */
    if( cVersion == (char)0x30 ) {
      iNoOfFields = ( uiHeaderLen - 296 ) / 32 ;
    } else {
      iNoOfFields = ( uiHeaderLen - 33 ) / 32;
    }

    if(( RecBuf = (char *) malloc( uiRecordLen )) == NULL ){
      iErrorStop = 160;
      iRc = XB_NO_MEMORY;
      throw iRc;
    }

    if(( RecBuf2 = (char *) malloc( uiRecordLen )) == NULL ) {
      iErrorStop = 170;
      iRc = XB_NO_MEMORY;
      throw iRc;
    }

    if((SchemaPtr=(xbSchemaRec *)malloc((size_t) iNoOfFields * sizeof( xbSchemaRec ))) == NULL){
      iErrorStop = 180;
      iRc = XB_NO_MEMORY;
      throw iRc;
    }

    memset( SchemaPtr, 0x00, (size_t) iNoOfFields * (size_t) sizeof( xbSchemaRec ));
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
      SchemaPtr[i].cIxFlag   = (unsigned char) *( p + 19 );
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
        sMfn.PutAt( ulMfnLen, 'T' );
      else
        sMfn.PutAt( ulMfnLen, 't' );
      xbFile fTemp( xbase );
      fTemp.SetFileName( sMfn );

      Memo = new xbMemoDbt4( this, fTemp.GetFqFileName());

      if(( iRc = Memo->OpenMemoFile()) != XB_NO_ERROR ){
        iErrorStop = 200;
        throw iRc;
      }
    }
    #endif

   #ifdef XB_MDX_SUPPORT

//   printf( "cIndexFlag [%x]\n", cIndexFlag );

   if( cIndexFlag ){
     // create the file name
     xbString sIxFileName = GetFqFileName();
     sIxFileName.Trim();
     xbUInt32 lLen = sIxFileName.Len();
     sIxFileName.PutAt( lLen-2, 'M' );
     sIxFileName.PutAt( lLen-1, 'D' );
     sIxFileName.PutAt( lLen,   'X' );
     if(( iRc = OpenIndex( "MDX", sIxFileName )) != XB_NO_ERROR ){
       iErrorStop = 210;
       throw iRc;
     }
   }
   #endif

  }
  catch ( xbInt16 iRc )
  {
    xbString sMsg;
    sMsg.Sprintf( "xbdbf4::Open() Exception Caught Error Stop = [%d] iRc = [%d] ShareMode = [%d] OpenMode = [%d]", iErrorStop, iRc, iShareMode, iOpenMode );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
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
#endif
  }

  if( iRc == XB_NO_ERROR )
    iDbfStatus = XB_OPEN;

  return iRc;
}

/************************************************************************/
//! @brief Rename table.
/*!
   This routine renames a table, associated memo, mdx and inf files
  \param sNewName - New file name.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbDbf4::Rename( const xbString sNewName ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;

  xbString sNewDbf;

  #ifdef XB_MEMO_SUPPORT
  xbString sNewDbt;
  xbBool bDbtRenamed = xbFalse;
  xbString sThisDbt;
  #endif

  #ifdef XB_MDX_SUPPORT
  xbString sNewMdx;
  xbBool bMdxRenamed = xbFalse;
  xbString sThisMdx;
  #endif

  #ifdef XB_INF_SUPPORT
  xbString sNewInf;
  xbString sThisInf;
  xbBool bInfRenamed = xbFalse;
  #endif  // XB_INF_SUPPORT

  xbBool bDbfRenamed = xbFalse;

  #ifdef XB_LOCKING_SUPPORT
  xbBool bLocked = xbFalse;
  #endif // XB_LOCKIN_SUPPORT


  try{

    xbString sDir;
    xbString sFile;
    xbString sExt;
    xbString sNewNameWoExt;
    sNewNameWoExt.Set( sNewName );
    if( sNewName.Pos( ".DBF" ) > 0 )
      sNewNameWoExt.Left( sNewName.Len() - 4 );

//    std::cout << "NewName wo ext = [" << sNewNameWoExt.Str() << "]\n";

    GetFileDirPart ( sDir );
    GetFileNamePart( sFile );
    GetFileExtPart ( sExt );

    sNewDbf.Sprintf( "%s%s.DBF", sDir.Str(), sNewNameWoExt.Str());

    #ifdef XB_MEMO_SUPPORT
    sNewDbt.Sprintf( "%s%s.DBT", sDir.Str(), sNewNameWoExt.Str());
    if( FileExists( sNewDbt )) return XB_FILE_EXISTS;
    sThisDbt.Sprintf( "%s%s.DBT", sDir.Str(), sFile.Str());
    #endif

    #ifdef XB_MDX_SUPPORT
    sNewMdx.Sprintf( "%s%s.MDX", sDir.Str(), sNewNameWoExt.Str());
    if( FileExists( sNewMdx )) return XB_FILE_EXISTS;
    sThisMdx.Sprintf( "%s%s.MDX", sDir.Str(), sFile.Str());
    #endif

    #ifdef XB_INF_SUPPORT
    sNewInf.Sprintf( "%s%s.INF", sDir.Str(), sNewNameWoExt.Str());
    if( FileExists( sNewInf )) return XB_FILE_EXISTS;
    sThisInf.Sprintf( "%s%s.INF", sDir.Str(), sFile.Str());
    #endif // XB_INF_SUPPORT


/*
    std::cout << "xbDbf3::Rename  dir = [" << sDir.Str() << "] file = [" << sFile.Str() << "] ext = [" << sExt.Str() << "]\n";
    std::cout << "xbDbf3::Rename new dbf = [" << sNewDbf.Str() << "]\n";
    std::cout << "xbDbf3::Rename new dbt = [" << sNewDbt.Str() << "]\n";
    std::cout << "xbDbf3::Rename new inf = [" << sNewInf.Str() << "]\n";
    std::cout << "xbDbf3::Rename new mdx = [" << sNewMdx.Str() << "]\n";
*/

    if( FileExists( sNewDbf )) return XB_FILE_EXISTS;

    #ifdef XB_LOCKING_SUPPORT
    if( GetAutoLock() ){
      if(( iRc = LockTable( XB_LOCK )) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      }
      bLocked = xbTrue;
    }
    #endif

    xbInt16 iOpenMode = GetOpenMode();
    xbInt16 iShareMode = GetShareMode();
    xbBool  bWasOpen = xbFalse;
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
      if(( iRc = Memo->xbFseek( 8, SEEK_SET )) != XB_NO_ERROR ){
        iErrorStop = 140;
        throw iRc;
      }

      sNewNameWoExt.PadRight( ' ', 8 );
      for( int i = 1; i < 9; i++ )
        Memo->xbFputc( sNewNameWoExt[i] );

      if( bWasOpen ){
        if(( iRc = Memo->xbFclose()) != XB_NO_ERROR ){
          iErrorStop = 150;
          throw iRc;
        }
      }

      Memo->SetFileName( sNewDbt );
      if(( xbRename( sThisDbt.Str(), sNewDbt.Str())) != XB_NO_ERROR ){
        iErrorStop = 160;
        throw iRc;
      }

      if( bWasOpen ){
        if(( iRc = Memo->xbFopen( iOpenMode, iShareMode )) != XB_NO_ERROR ){
          iErrorStop = 150;
          throw iRc;
        }
      }

      bDbtRenamed = xbTrue;
    }
    #endif

    #ifdef XB_MDX_SUPPORT
    if( FileExists( sThisMdx )){
      xbIxMdx *ixMdx;
      xbString s;
      xbBool bMdxFound = xbFalse;
      xbIxList *ixList = GetIxList();
      while( ixList && !bMdxFound ){
        s = ixList->sFmt->Str();
        if( s == "MDX" ){
          ixMdx = (xbIxMdx *) ixList->ix;
          bMdxFound = xbTrue;
        }
      }

      if( bMdxFound ){
        if(( iRc = ixMdx->xbFseek( 4, SEEK_SET )) != XB_NO_ERROR ){
          iErrorStop = 180;
          throw iRc;
        }

        sNewNameWoExt.PadRight( ' ', 8 );
        for( int i = 1; i < 9; i++ )
          ixMdx->xbFputc( sNewNameWoExt[i] );

        if( bWasOpen ){
          if(( iRc = ixMdx->xbFclose()) != XB_NO_ERROR ){
            iErrorStop = 190;
            throw iRc;
          }
        }

        ixMdx->SetFileName( sNewMdx );
        if(( xbRename( sThisMdx.Str(), sNewMdx.Str())) != XB_NO_ERROR ){
          iErrorStop = 200;
          throw iRc;
        }

        if( bWasOpen ){
          if(( iRc = ixMdx->xbFopen( iOpenMode, iShareMode )) != XB_NO_ERROR ){
            iErrorStop = 210;
            throw iRc;
          }
        }
        bMdxRenamed = xbTrue;
      }
    }
    #endif // XB_MDX_SUPPORT

    #ifdef XB_INF_SUPPORT
    if( FileExists( sThisInf )){
      if(( xbRename( sThisInf.Str(), sNewInf.Str())) != XB_NO_ERROR ){
        iErrorStop = 180;
        throw iRc;
      } else {
        bInfRenamed = xbTrue;
      }
    }
    #endif // XB_INF_SUPPORT

    // rename the table in the table list
    xbTblList *tle = xbase->GetTblListEntry( this );
    if( tle ){
      tle->psFqTblName->Set( GetFqFileName().Str());
      tle->psTblAlias->Set( sNewNameWoExt.Str());
    }


  }
  catch ( xbInt16 iRc )
  {
    xbString sMsg;
    sMsg.Sprintf( "xbdbf4::Rename() Exception Caught Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));

    // attempt to reverse things out if unsuccessful
    if( bDbfRenamed ){

      #ifdef XB_MEMO_SUPPORT
      if( bDbtRenamed ){
        xbRename( sNewDbt.Str(), sThisDbt.Str());
      }
      #endif

      #ifdef XB_MDX_SUPPORT
      if( bMdxRenamed ){
        xbRename( sNewMdx.Str(), sThisMdx.Str());
      }
      #endif

      #ifdef XB_INF_SUPPORT
      if( bInfRenamed ){
        xbRename( sNewInf.Str(), sNewInf.Str());
      }
      #endif // XB_INF_SUPPORT
    }

    #ifdef XB_LOCKING_SUPPORT
    if( GetAutoLock() ){
      iRc = LockTable( XB_UNLOCK );
    }
    #endif

  }

  #ifdef XB_LOCKING_SUPPORT
  if( bLocked ){
    LockTable( XB_UNLOCK );
  }
  #endif

  return iRc;
}

/************************************************************************/
#ifdef XB_MEMO_SUPPORT

//! @brief Create memo block size.
/*!
   This routine sets the memo file block size. This value is used when 
   the memo file is created so you if you want to change it, this must be 
   called before creating the table.

   The default size for version 4 is 1024.

  \param ulBlockSize - Block size, must be evenly divisible by 512.
  \returns XB_INVALID_BLOCK_SIZE<br>XB_NO_ERROR
*/

xbInt16 xbDbf4::SetCreateMemoBlockSize( xbUInt32 ulBlockSize ){

  if( ulBlockSize % 512 )
    return XB_INVALID_BLOCK_SIZE;
  else
    ulCreateMemoBlockSize = ulBlockSize;

  return XB_NO_ERROR;
}
#endif // XB_MEMO_SUPPORT
/************************************************************************/
//! @brief Set version.
/*!
   Sets the version to 4.  The version info can be retrieved to determine
   which class is being used for a given dbf instance.
   \returns 4
*/
xbInt16 xbDbf4::SetVersion() {
   iFileVersion = 4;
   return iFileVersion;
}
/************************************************************************/
//! @brief Validate schema
/*!
  This routine verifies the field types are valid for Dbase IV (tm).

  \param s Pointer to schema structure with field definitions.

  \returns Number of fields or XB_INVALID_FIELD_TYPE.
*/


xbInt16 xbDbf4::ValidateSchema( xbSchema * s ){

  xbInt16 iFieldCnt = 0;
  uiRecordLen = 0;

  // Count the number of fields and check paramaters
  xbInt16 i = 0;
  while( s[i].cType != 0 ){
    iFieldCnt++;
    // Version IV field types
    if( s[i].cType != 'C' &&
        s[i].cType != 'N' &&
        s[i].cType != 'F' &&
        s[i].cType != 'D' &&
        #ifdef XB_MEMO_SUPPORT
        s[i].cType != 'M' &&
        #endif /* XB_MEMO_SUPPORT */
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
    else if((s[i].cType == 'N' || s[i].cType == 'F') && s[i].iFieldLen > 19 ){
      return XB_INVALID_FIELD_LEN;
    }

    // field len must be greater then number of decimals
    else if((s[i].cType == 'N' || s[i].cType == 'F') && s[i].iFieldLen < s[i].iNoOfDecs ){
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
#endif   /* XB_DBF4_SUPPORT  */