/*  xbfile.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023,2024 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

This module handles all the low level file I/O and is the base class
for the table, memo and index classes

*/

#include "xbase.h"

namespace xb{

/************************************************************************/
xbFile::xbFile( xbXBase * x ){
  fp           = NULL;
  pFileBuffer  = 0x00;
  bFileOpen    = xbFalse;
  ulBlockSize  = 0;
  iFileNo      = 0;
  xbase        = x;
  if( xbase->GetMultiUser() == xbTrue )
    iShareMode = XB_MULTI_USER;
  else
    iShareMode = XB_SINGLE_USER;
  iOpenMode    = 0;
  #ifdef XB_LOCKING_SUPPORT
  iLockRetries = -1;
  #endif
  #ifdef HAVE_SETENDOFFILE_F
  fHandle      = NULL;
  #endif
}
/************************************************************************/
xbFile::~xbFile(){
  if( bFileOpen )
    xbFclose();
}
/************************************************************************/
xbInt16 xbFile::CreateUniqueFileName( const xbString & sDirIn, const xbString & sExtIn, xbString &sFqnOut, xbInt16 iOption ){
  xbBool   bUniqueFileNameFound = xbFalse;
  xbFile   f( xbase);
  xbInt32  l = 1;
  xbString sMemoFileName;
  xbString sDir = sDirIn;
  char     c = xbase->GetPathSeparator();
  if( sDirIn.Len() > 0 && sDirIn[sDirIn.Len()] != c )
    sDir += c;
  while( !bUniqueFileNameFound ){
    sFqnOut.Sprintf( "%sxbTmp%03d.%s", sDir.Str(), l, sExtIn.Str()); 
    if( iOption == 1 && sExtIn == "DBF" ){
      sMemoFileName.Sprintf( "%sxbTmp%03d.DBT", sDirIn.Str(), l ); 
    }
    else if( iOption == 1 && sExtIn == "dbf" ){
      sMemoFileName.Sprintf( "%sxbTmp%03d.dbt", sDirIn.Str(), l ); 
    }
    if( f.FileExists( sFqnOut ) || ( sMemoFileName.Len() > 0 && f.FileExists( sMemoFileName ))){
      l++;
    }
    else
    {
      bUniqueFileNameFound = xbTrue;
    }
    if( l > 999 )
      return XB_FILE_EXISTS;
  }
  return XB_NO_ERROR;
}
/*************************************************************************/
xbInt16 xbFile::DetermineXbaseMemoVersion( unsigned char cFileTypeByte ) const {
  xbBit b;
  if( b.BitSet( cFileTypeByte, 3 ) && b.BitSet( cFileTypeByte, 7 ))
    return 4;
  else if( b.BitSet( cFileTypeByte, 7 ))
    return 3;

  return 0;
}
/*************************************************************************/
xbInt16 xbFile::DetermineXbaseTableVersion( unsigned char cFileTypeByte ) const {
  xbInt16 iMemoVersion = DetermineXbaseMemoVersion(cFileTypeByte);
  char    cDbfLevel = cFileTypeByte & 0x07; 
  #ifdef XB_DBF4_SUPPORT
  if( cDbfLevel == 3 && iMemoVersion != 3 )
    return 4;
  #endif
  #ifdef XB_DBF3_SUPPORT
  if( cDbfLevel == 3 && iMemoVersion != 4 )
    return 3;
  #endif
  return 0;
}
/*************************************************************************/
xbDouble xbFile::eGetDouble( const char *p ) const {
   xbDouble d;
   const char *sp;
   char *tp;
   xbInt16 i;
   tp = (char *) &d;
   sp = p;
   if( iEndianType == 'L' )
      for( i = 0; i < 8; i++ ) *tp++ = *sp++;
   else
   {
      sp+=7;
      for( i = 0; i < 8; i++ )  *tp++ = *sp--;
   } 
   return d;
}
/*************************************************************************/
xbInt16 xbFile::eGetInt16(const char *p) const {
  xbInt16 s, i;
  char *tp;
  tp = (char *) &s;
  if( iEndianType == 'L' )
    for( i = 0; i < 2; i++ ) *tp++ = *p++;
  else{
    p++;
    for( i = 0; i < 2; i++ ) *tp++ = *p--;
  }
  return s;
}
/*************************************************************************/
xbInt32 xbFile::eGetInt32( const char *p ) const {
  xbInt32 l;
  char *tp;
  xbInt16 i;
  tp = (char *) &l;
  if( iEndianType == 'L' )
    for( i = 0; i < 4; i++ ) *tp++ = *p++;
  else {
    p+=3;
    for( i = 0; i < 4; i++ )  *tp++ = *p--;
  }
  return l;
}
/*************************************************************************/
xbUInt16 xbFile::eGetUInt16(const char *p) const {
  xbInt16  i;
  xbUInt16 uI;
  char *tp;

  tp = (char *) &uI;
  if( iEndianType == 'L' )
    for( i = 0; i < 2; i++ ) *tp++ = *p++;
  else{
    p++;
    for( i = 0; i < 2; i++ ) *tp++ = *p--;
  }
  return uI;
}
/*************************************************************************/
xbUInt32 xbFile::eGetUInt32( const char *p ) const {
  xbUInt32 ul;
  xbInt16  i;
  char     *tp;
  tp = (char *) &ul;
  if( iEndianType == 'L' )
    for( i = 0; i < 4; i++ ) *tp++ = *p++;
  else{
    p+=3;
    for( i = 0; i < 4; i++ ) *tp++ = *p--;
  }
  return ul;
}

/*************************************************************************/
void xbFile::ePutDouble( char *c, xbDouble d ){
   const char *sp;
   char *tp;
   xbInt16 i;
   tp = c;
   sp = (const char *) &d;
   if( iEndianType == 'L' )
      for( i = 0; i < 8; i++ ) *tp++ = *sp++;
   else
   {
      sp+=7;
      for( i = 0; i < 8; i++ ) *tp++ = *sp--;
   }
   return;
}
/*************************************************************************/
void xbFile::ePutInt16( char * c, xbInt16 s ){
  const char *sp;
  char *tp;
  xbInt16 i;

  tp = c;
  sp = (const char *) &s;

  if( iEndianType == 'L' )
    for( i = 0; i < 2; i++ ) *tp++ = *sp++;
  else{      /* big endian */
    sp++;
    for( i = 0; i < 2; i++ ) *tp++ = *sp--;
  }
  return;
}
/*************************************************************************/
void xbFile::ePutInt32( char * c, xbInt32 l )
{
  const char *sp;
  char *tp;
  xbInt16 i;
  tp = c;
  sp = (const char *) &l;
  if( iEndianType == 'L' )
    for( i = 0; i < 4; i++ ) *tp++ = *sp++;
  else {
    sp+=3;
    for( i = 0; i < 4; i++ ) *tp++ = *sp--;
  }
  return;
}
/*************************************************************************/
void xbFile::ePutUInt16( char * c, xbUInt16 s ){
  const char *sp;
  char *tp;
  xbInt16 i;
  tp = c;
  sp = (const char *) &s;
  if( iEndianType == 'L' )
    for( i = 0; i < 2; i++ ) *tp++ = *sp++;
  else{      /* big endian */
    sp++;
    for( i = 0; i < 2; i++ ) *tp++ = *sp--;
  }
  return;
}
/*************************************************************************/
void xbFile::ePutUInt32( char * c, xbUInt32 ul )
{
   const char *sp;
   char       *tp;
   xbInt16    i;
   tp = c;
   sp = (const char *) &ul;
   if( iEndianType == 'L' )
      for( i = 0; i < 4; i++ ) *tp++ = *sp++;
   else
   {
      sp+=3;
      for( i = 0; i < 4; i++ ) *tp++ = *sp--;
   }
   return;
}
/************************************************************************/
xbBool xbFile::FileExists() const {
  return FileExists( sFqFileName, 0 );
}
/************************************************************************/
xbBool xbFile::FileExists(const xbString &sFileName ) const {
  return FileExists( sFileName, 0 );
}
/************************************************************************/
xbBool xbFile::FileExists( const xbString & sFileName, xbInt16 iOption ) const {
  struct stat buffer;
  if(( stat( sFileName.Str(), &buffer ) != 0 )){
    return xbFalse;
  }
  #ifdef XB_MEMO_SUPPORT
  if( iOption == 1 ){
    xbString sFileName2 = sFileName;

    if( sFileName2[sFileName2.Len()] == 'F' )
      sFileName2.PutAt( sFileName2.Len(), 'T' );
    else
      sFileName2.PutAt( sFileName2.Len(), 't' );

    if(( stat( sFileName2.Str(), &buffer) != 0 ))
      return xbFalse;
  }
  #endif
  return xbTrue;
}
/************************************************************************/
xbBool xbFile::FileExists( xbInt16 iOption ) const {
  return FileExists( sFqFileName, iOption );
}
/************************************************************************/
xbBool xbFile::FileIsOpen() const {
  return bFileOpen;
}
/************************************************************************/
xbUInt32 xbFile::GetBlockSize() const {
  return ulBlockSize;
}
/************************************************************************/
const xbString & xbFile::GetDirectory() const {
  return sDirectory;
}
/************************************************************************/
xbInt16 xbFile::GetFileDirPart( const xbString & sCompleteFileNameIn, xbString & sFileDirPartOut ) const {
  sFileDirPartOut = sCompleteFileNameIn;
  sFileDirPartOut.SwapChars( '\\', '/' );
  xbUInt32 iPos = sFileDirPartOut.GetLastPos( '/' );
  if( iPos > 0 ){
    xbString sTemp = sFileDirPartOut;
    sFileDirPartOut.Assign( sTemp, 1, iPos );
    return XB_NO_ERROR;
  }
  return XB_INVALID_DATA;
}
/************************************************************************/
xbInt16 xbFile::GetFileDirPart( xbString & sFileDirPartOut ) const {
  return GetFileDirPart( sFqFileName, sFileDirPartOut );
}
/************************************************************************/
xbInt16 xbFile::GetFileExtPart( const xbString & sCompleteFileNameIn, xbString & sFileExtPartOut ) const {
  sFileExtPartOut = sCompleteFileNameIn;
  xbUInt32 iPos = sFileExtPartOut.GetLastPos( '.' );
  if( iPos > 0 ){            /* get rid of the directory part of the name */
    sFileExtPartOut.Ltrunc( iPos );
    return XB_NO_ERROR;
  }
  return XB_INVALID_DATA;
}
/************************************************************************/
xbInt16 xbFile::GetFileExtPart( xbString & sFileNameExtOut ) const {
  return GetFileExtPart( sFqFileName, sFileNameExtOut );
}
/************************************************************************/
xbInt16 xbFile::GetFileMtime( time_t &mtime ){
  struct stat buffer;
  if( stat( sFqFileName.Str(), &buffer ))
    return XB_FILE_NOT_FOUND;
  else{
    mtime = buffer.st_mtime;
    return XB_NO_ERROR;
  }
}
/************************************************************************/
const xbString & xbFile::GetFileName() const {
  return sFileName;
}
/************************************************************************/
xbInt16 xbFile::GetFileNamePart( const xbString & sCompleteFileNameIn, xbString & sFileNamePartOut ) const {
  sFileNamePartOut = sCompleteFileNameIn;
  sFileNamePartOut.SwapChars( '\\', '/' );
  xbUInt32 iPos = sFileNamePartOut.GetLastPos( '/' );
  if( iPos > 0 )            /* get rid of the directory part of the name */
    sFileNamePartOut.Ltrunc( iPos );
  iPos = sFileNamePartOut.Pos( '.' );
  if( iPos > 0 ){            /* get rid of the extension part of the name */
    xbString sTemp = sFileNamePartOut;
    sFileNamePartOut.Assign( sTemp, 1, iPos-1 );
  }
  return XB_NO_ERROR;
}
/************************************************************************/
xbInt16 xbFile::GetFileNamePart( xbString & sFileNamePartOut ) const {
  return GetFileNamePart( sFqFileName, sFileNamePartOut );
}
/************************************************************************/
xbInt16 xbFile::GetFileSize( xbUInt64 &ullFileSize ){
  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;
  try{
    if(( iRc = xbFseek( 0, SEEK_END )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }
    ullFileSize = xbFtell();
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbFile::GetFileSize() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
/************************************************************************/
xbInt16 xbFile::GetFileType( xbString & sFileTypeOut ) const {

  xbInt16 iRc = GetFileExtPart( sFqFileName, sFileTypeOut );
  sFileTypeOut.ToUpperCase();
  return iRc;
}
/************************************************************************/
const xbString & xbFile::GetFqFileName() const {
  return sFqFileName;
}
/************************************************************************/
FILE * xbFile::GetFp(){
  return fp;
}
/************************************************************************/
xbInt16 xbFile::GetOpenMode() const {
  return iOpenMode;
}
/************************************************************************/
xbInt16 xbFile::GetShareMode() const {
  return iShareMode;
}
/************************************************************************/
xbInt16  xbFile::GetXbaseFileTypeByte( const xbString &sFileName, unsigned char &cFileTypeByte )
{
  xbInt16 iVersion;
  return GetXbaseFileTypeByte( sFileName, cFileTypeByte, iVersion );
}
/************************************************************************/
xbInt16  xbFile::GetXbaseFileTypeByte( const xbString &sFileName, unsigned char &cFileTypeByte, xbInt16 &iVersion ){
  xbInt16 iErrorStop = 0;
  xbInt16 iRc = XB_NO_ERROR;
  size_t  stRc;
  FILE *tfp;
  try{
    iVersion = 0;
    cFileTypeByte = 0x00;
    #ifdef HAVE__FSOPEN_F
    // 0x40 is SH_DENYNO or _SH_DENYNO
    if(( tfp = _fsopen( sFileName.Str(), "r", 0x40 )) == NULL ){
     iErrorStop = 100;
     iRc = XB_OPEN_ERROR;
     throw iRc;
    }
    #else
    if(( tfp = fopen( sFileName.Str(), "r" )) == NULL ){
     iErrorStop = 110;
     iRc = XB_OPEN_ERROR;
     throw iRc;
    }
    #endif
    #ifdef HAVE_FSEEKO_F
      iRc = fseeko( tfp, 0, SEEK_SET );
    #else
      iRc = fseek( tfp, 0, SEEK_SET );
    #endif
    if( iRc != 0 ){
      iErrorStop = 120;
      iRc = XB_SEEK_ERROR;
      throw iRc;
    }
    stRc = fread( &cFileTypeByte, (size_t) 1, (size_t) 1, tfp );
    if( stRc != (size_t) 1 ){
      iErrorStop = 130;
      iRc = XB_READ_ERROR;
      throw iRc;
    }
    iRc = XB_NO_ERROR;
    fclose( tfp );
    iVersion = DetermineXbaseTableVersion( cFileTypeByte );
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbFile::GetXbaseFileType() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
/************************************************************************/
xbInt16  xbFile::GetXbaseFileTypeByte( const xbString &sFileName, xbInt16 &iVersion )
{
  unsigned char cFileTypeByte;
  return GetXbaseFileTypeByte( sFileName, cFileTypeByte, iVersion );
}
/************************************************************************/
xbInt16 xbFile::NameSuffixMissing( const xbString & sFileName, xbInt16 iOption ) const {
  xbUInt32 ulLen = sFileName.Len();
  if( ulLen <= 4 )
    return XB_NOT_FOUND;
  if( iOption == 1 && sFileName[ulLen-3] == '.' &&
     ( sFileName[ulLen-2] == 'd' || sFileName[ulLen-2] == 'D' ) && 
     ( sFileName[ulLen-1] == 'b' || sFileName[ulLen-1] == 'B' ) && 
     ( sFileName[ulLen]   == 'f' || sFileName[ulLen]   == 'F' ))
      return 0;
  if( iOption == 2 && sFileName[ulLen-3] == '.' &&
     ( sFileName[ulLen-2] == 'n' || sFileName[ulLen-2] == 'N' ) && 
     ( sFileName[ulLen-1] == 'd' || sFileName[ulLen-1] == 'D' ) && 
     ( sFileName[ulLen]   == 'x' || sFileName[ulLen]   == 'X' ))
      return 0;
  if( iOption == 3 && sFileName[ulLen-3] == '.' &&
     ( sFileName[ulLen-2] == 'm' || sFileName[ulLen-2] == 'M' ) && 
     ( sFileName[ulLen-1] == 'd' || sFileName[ulLen-1] == 'D' ) && 
     ( sFileName[ulLen]   == 'x' || sFileName[ulLen]   == 'X' ))
      return 0;
  if( iOption == 4 && sFileName[ulLen-3] == '.' &&
     ( sFileName[ulLen-2] == 'n' || sFileName[ulLen-2] == 'N' ) && 
     ( sFileName[ulLen-1] == 't' || sFileName[ulLen-1] == 'T' ) && 
     ( sFileName[ulLen]   == 'x' || sFileName[ulLen]   == 'X' ))
     return 0;
  return XB_NOT_FOUND;
}
/***********************************************************************/
xbInt16 xbFile::ReadBlock( xbUInt32 ulBlockNo, size_t lReadSize, void * buf ){
  return ReadBlock( ulBlockNo, ulBlockSize, lReadSize, buf );
}
/***********************************************************************/
xbInt16 xbFile::ReadBlock( xbUInt32 ulBlockNo, xbUInt32 ulBlockSize, size_t lReadSize, void * buf ){

  xbInt16 iErrorStop = 0;
  xbInt16 iRc = XB_NO_ERROR;

  try{
    if( ulBlockSize <= 0 ){
      iErrorStop = 100;
      iRc = XB_INVALID_BLOCK_SIZE;
      throw iRc;
    }

    if(( iRc = xbFseek(((xbInt64) ulBlockNo*ulBlockSize ), SEEK_SET )) != XB_NO_ERROR ){
      iErrorStop = 110;
      iRc = XB_SEEK_ERROR;
      throw iRc;
    }

    if( lReadSize <= 0 )
      lReadSize = ulBlockSize;

    if(( iRc = xbFread( buf, lReadSize, 1 )) != XB_NO_ERROR ){
      iErrorStop = 120;
      iRc = XB_READ_ERROR;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbFile::ReadBlock() Exception Caught. Error Stop = [%d] iRc = [%d] BlkNo=[%ld] BlkSize=[%ld] ReadSize=[%ld]", iErrorStop, iRc, ulBlockNo, ulBlockSize, lReadSize );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
/************************************************************************/
xbInt16 xbFile::SetBlockSize( xbUInt32 ulBlockSize ){
  if( ulBlockSize %512 != 0 )
    return XB_INVALID_BLOCK_SIZE;

  this->ulBlockSize = ulBlockSize;
  return XB_NO_ERROR;
}
/************************************************************************/
void xbFile::SetDirectory( const xbString & sDirectory ){
  this->sDirectory  = sDirectory;
  char cLastChar = sDirectory[sDirectory.Len()];
  if( cLastChar != '/' && cLastChar != '\\' )
    sFqFileName.Sprintf( "%s/%s", sDirectory.Str(), sFileName.Str());
  else
    sFqFileName.Sprintf( "%s%s", sDirectory.Str(), sFileName.Str());
  #ifdef WIN32
    sFqFileName.SwapChars( '/', '\\' );
  #else
    sFqFileName.SwapChars( '\\', '/' );
  #endif
}

/************************************************************************/
void xbFile::SetFileName( const xbString & sFileName ){
  if( sFileName == "" ){
    sFqFileName = "";
    return;
  }
  char cPathSep = sFileName.GetPathSeparator();
  if( cPathSep ){
    xbString sName;
    xbString sExt;
    // GetFileDirPart( this->sDirectory );
    GetFileNamePart( sFileName, sName );
    GetFileExtPart( sFileName, sExt );
    this->sFileName.Sprintf( "%s.%s", sName.Str(), sExt.Str());
    sFqFileName = sFileName;
  } else {
    this->sFileName   = sFileName;
    if( sDirectory.Len() == 0 ){
      sDirectory = sDataDirectory;
      //sDirectory = GetDataDirectory();

      char cLastChar = sDirectory[sDirectory.Len()];
      if( cLastChar != '/' && cLastChar != '\\' )
        sFqFileName.Sprintf( "%s/%s", sDirectory.Str(), sFileName.Str() );
      else
        sFqFileName = sDirectory + sFileName;
    }
    else{
      char cLastChar = sDirectory[sDirectory.Len()];
      if( cLastChar != '/' && cLastChar != '\\' )
        sFqFileName.Sprintf( "%s/%s", sDirectory.Str(), sFileName.Str() );
      else
        sFqFileName = sDirectory + sFileName;
    }
  }
  #ifdef WIN32
    sFqFileName.SwapChars( '/', '\\' );
  #else
    sFqFileName.SwapChars( '\\', '/' );
  #endif
}
/************************************************************************/
void xbFile::SetFqFileName( const xbString & sFqFileName ){
  this->sFqFileName  = sFqFileName;
  xbString sDir;
  xbString sName;
  xbString sExt;
  GetFileDirPart ( sFqFileName, sDir );
  GetFileNamePart( sFqFileName, sName );
  GetFileExtPart ( sFqFileName, sExt );
  sDirectory  = sDir;
  sFileName.Sprintf( "%s.%s", sName.Str(), sExt.Str() );
  #ifdef WIN32
    this->sDirectory.SwapChars ( '/', '\\' );
    this->sFqFileName.SwapChars( '/', '\\' );
  #else
    this->sDirectory.SwapChars ( '\\', '/' );
    this->sFqFileName.SwapChars( '\\', '/' );
  #endif
}
/************************************************************************/
xbInt16 xbFile::SetHomeFolders(){
  xbInt16 iErrorStop = 0;
  xbInt16 iRc = XB_NO_ERROR;
  xbString sHomeDir;
  char cPathSeparator;
  xbString sDflt;
  try{
    xbase->GetHomeDir( sHomeDir );
    if( FileExists( sHomeDir ) == xbFalse ){
      iErrorStop = 100;
      iRc = XB_DIRECTORY_ERROR;
      throw iRc;
    }
    #ifdef WIN32
    cPathSeparator = '\\';
    #else
    cPathSeparator = '/';
    #endif
    sDflt.Sprintf( ".%c", cPathSeparator );
    // set the default folders just in case later steps fail
    xbase->SetDataDirectory( sDflt );
    #ifdef XB_LOGGING_SUPPORT
    xbase->SetLogDirectory( sDflt );
    #endif
    if( sHomeDir[sHomeDir.Len()] != cPathSeparator )
      sHomeDir += cPathSeparator;

    xbString sWork( sHomeDir );
    sWork += "xbase64";
    if( FileExists( sWork ) == xbFalse ){
      #ifdef WIN32
        if( CreateDirectory( sWork.Str(), NULL ) == 0 ){
          iErrorStop = 130;
          iRc = XB_DIRECTORY_ERROR;
          throw iRc;
        }
      #else
        // 0777 mode is correct, the mode will be modified by the user's umask
        if( mkdir( sWork.Str(), 0777 ) == -1 ){
          iErrorStop = 140;
          iRc = XB_DIRECTORY_ERROR;
          throw iRc;
        }
      #endif
    }
    #ifdef XB_LOGGING_SUPPORT
    sWork.Sprintf( "%sxbase64%clogs", sHomeDir.Str(), cPathSeparator );
    if( FileExists( sWork ) == xbFalse ){
      #ifdef WIN32
        if( CreateDirectory( sWork.Str(), NULL ) == 0 ){
          iErrorStop = 110;
          iRc = XB_DIRECTORY_ERROR;
          throw iRc;
        }
      #else
        if( mkdir( sWork.Str(), 0777 ) == -1 ){
          iErrorStop = 120;
          iRc = XB_DIRECTORY_ERROR;
          throw iRc;
        }
      #endif
    }
    xbase->SetLogDirectory( sWork );
    #endif  // XB_LOGGING_SUPPORT
    sWork.Sprintf( "%sxbase64%cdata", sHomeDir.Str(), cPathSeparator );
    if( FileExists( sWork ) == xbFalse ){
      #ifdef WIN32
        if( CreateDirectory( sWork.Str(), NULL ) == 0 ){
          iErrorStop = 130;
          iRc = XB_DIRECTORY_ERROR;
          throw iRc;
        }
      #else
        if( mkdir( sWork.Str(), 0777 ) == -1 ){
          iErrorStop = 140;
          iRc = XB_DIRECTORY_ERROR;
          throw iRc;
        }
      #endif
    }
    sWork.Sprintf( "%sxbase64%ctemp", sHomeDir.Str(), cPathSeparator );
    if( FileExists( sWork ) == xbFalse ){
      #ifdef WIN32
        if( CreateDirectory( sWork.Str(), NULL ) == 0 ){
          iErrorStop = 150;
          iRc = XB_DIRECTORY_ERROR;
          throw iRc;
        }
      #else
        if( mkdir( sWork.Str(), 0777 ) == -1 ){
          iErrorStop = 160;
          iRc = XB_DIRECTORY_ERROR;
          throw iRc;
        }
      #endif
    }
    xbase->SetTempDirectory( sWork );
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbFile::CreateHomeFolders() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
/************************************************************************/
xbInt16 xbFile::WriteBlock( xbUInt32 ulBlockNo, size_t lWriteSize, void * buf ){
  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  try{
    if( ulBlockSize == 0 ){
      iErrorStop = 100;
      iRc = XB_INVALID_BLOCK_SIZE;
      throw iRc;
    }
    if( lWriteSize <= 0 )
      lWriteSize = ulBlockSize;
    if(( iRc = xbFseek(( (xbInt64) ulBlockNo*ulBlockSize), SEEK_SET )) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }
    if(( iRc = xbFwrite( buf, lWriteSize, 1 )) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbFile::WriteBlock() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
/************************************************************************/
xbInt16 xbFile::xbFclose(){
  int iRc = 0;
  if( bFileOpen ){
    iRc = fclose( fp );
    if( iRc != 0 ){
      return XB_CLOSE_ERROR;
    }
    else{
      bFileOpen = xbFalse;
    }
    iFileNo = 0;
  }
  if( pFileBuffer ){
    free( pFileBuffer );
    pFileBuffer = NULL;
  }

  return XB_NO_ERROR;
}
/************************************************************************/
xbInt16 xbFile::xbFeof(){
  return feof( fp );
}
/************************************************************************/
xbInt16 xbFile::xbFflush() {
  if( fflush( fp ) )
    return XB_WRITE_ERROR;
  else
    return XB_NO_ERROR;
}
/************************************************************************/
xbInt16 xbFile::xbFgetc( char &c ) {
  int i;
  i = fgetc( fp );
  if( i == EOF )
    return XB_EOF;

  c = (char) i;
  return XB_NO_ERROR;
}
/************************************************************************/
xbInt16 xbFile::xbFgetc( xbInt32 &c ) {
  int i;
  i = fgetc( fp );
  if( i == EOF )
    return XB_EOF;
  c = i;
  return XB_NO_ERROR;
}
/************************************************************************/
xbInt16 xbFile::xbFgets( size_t lSize, xbString &s ) {
  s = "";
  if( feof( fp ))
    return XB_EOF;
  char *sBuf = (char *) malloc( lSize + 1 );
  if( fgets( sBuf, (xbInt32) lSize, fp ) == NULL ){
    free( sBuf );
    return XB_EOF;
  }
  s.Set( sBuf );
  free( sBuf );
  return XB_NO_ERROR;
}
/************************************************************************/
xbInt16 xbFile::xbFopen( const xbString &sOpenMode, const xbString &sFileName, xbInt16 iMultiUser ) {
  if( sFileName == "" || sFqFileName == "" )
    SetFileName( sFileName );
  return xbFopen( sOpenMode, iMultiUser );
}
/************************************************************************/
xbInt16 xbFile::xbFopen( const xbString & sOpenMode, xbInt16 iMultiUser ) {

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;

  try{

    #ifdef HAVE__FSOPEN_F
    if(( fp = _fsopen( sFqFileName.Str(), sOpenMode.Str(), 0x40 )) != NULL ){
    #else
    if(( fp = fopen( sFqFileName.Str(), sOpenMode.Str())) != NULL ){
    #endif

      if( sOpenMode == "r" )
        iOpenMode = XB_READ;
      else if( sOpenMode == "w" )
        iOpenMode = XB_WRITE;
      else
        iOpenMode = XB_READ_WRITE;

      bFileOpen = xbTrue;
      this->iShareMode = iMultiUser;
      #ifdef HAVE__FILENO_F
      iFileNo = _fileno( fp );
      #else
      iFileNo = fileno( fp );
      #endif
      #ifdef HAVE_SETENDOFFILE_F
      //used by visual studio, 32 bit
      fHandle = (HANDLE) _get_osfhandle( iFileNo );
      #endif

      if(( iRc = xbSetVbuf( iMultiUser )) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      }

      /*  depreciated 4.1.11
      #ifdef XB_LOCKING_SUPPORT
      if( iShareMode )
        xbFTurnOffFileBuffering();
      #endif
      */

    } else {
      iErrorStop = 200;
      iRc = XB_OPEN_ERROR;
      throw iRc;
    }

  }
  catch (xbInt16 iRc2 ){
    xbString sMsg;
    sMsg.Sprintf( "xbFile::xbFseek() Exception Caught. Error Stop = [%d] iRc = [%d] errno = [%d]", iErrorStop, iRc2, errno );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc2 ));
    iRc = XB_SEEK_ERROR;
  }
  return iRc;

}
/************************************************************************/
xbInt16 xbFile::xbFopen( xbInt16 iOpenMode, xbInt16 iShareMode ) {
  this->iOpenMode = iOpenMode;
  if( iOpenMode == XB_READ_WRITE )
    return xbFopen( "r+b", iShareMode );
  else if( iOpenMode == XB_READ )
    return xbFopen( "r", iShareMode );
  else
    return XB_INVALID_OPTION;
}
/************************************************************************/
xbInt16 xbFile::xbFputc( xbInt32 c ) {
  return xbFputc( c, 1 );
}
/************************************************************************/
xbInt16 xbFile::xbFputc( xbInt32 c, xbInt32 iNoOfTimes ) {
  for( xbInt32 l = 0; l < iNoOfTimes; l++ )
    if( fputc( c, fp ) != (int) c )
      return XB_WRITE_ERROR;
  return XB_NO_ERROR;
}
/************************************************************************/
xbInt16 xbFile::xbFputs( const xbString & s ){
  if( fputs( s.Str(), fp ) < 0 )
    return XB_WRITE_ERROR;
  else
    return XB_NO_ERROR;
}
/************************************************************************/
xbInt16 xbFile::xbFread( void *p, size_t size, size_t nmemb ) {

  xbInt16 iErrorStop = 0;
  xbInt16 iRc = XB_NO_ERROR;
  size_t  stRc = 0;

  try {
    stRc = fread( p, size, nmemb, fp );
    if( stRc != nmemb ){
      iRc = XB_READ_ERROR;
      iErrorStop = 100;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbFile::xbFread() Exception Caught. Error Stop = [%d] iRc = [%d] stRc = [%d] size = [%d] nmemb = [%d] errno = [%d]", iErrorStop, iRc, stRc, size, nmemb, errno );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
/************************************************************************/
xbInt16 xbFile::xbFseek( xbInt64 llOffset, xbInt32 iWhence ) {
  xbInt32 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbInt16 iRc2;
  try {
    #if defined(HAVE_FSEEKO_F)
    iRc = fseeko( fp, llOffset, iWhence );
    if( iRc != 0 ){
      iRc2 = XB_SEEK_ERROR;
      iErrorStop = 100;
      throw iRc2;
    }
    #elif defined(HAVE__FSEEKI64_F)
    iRc = _fseeki64( fp, llOffset, iWhence );
    if( iRc != 0 ){
      iRc2 = XB_SEEK_ERROR;
      iErrorStop = 110;
      throw iRc2;
    }
    #else
      #ifdef XB_PLATFORM_32

      /* if request is larger than 2 gig,this is a part of a locking request,
        assuming offset is less than 4 gig, split the request into 2 fseek calls */
      if( llOffset > 2147483647 && iWhence == SEEK_SET ){
        /* move forward max amt - 2G */
        if(( iRc = fseek( fp, 2147483647, SEEK_SET )) != 0 ){
          iRc2 = XB_SEEK_ERROR;
          iErrorStop = 120;
          throw iRc2;
        }
        llOffset -= 2147483647;
        iWhence = SEEK_CUR; 
      }
      #endif

      // iRc = fseek( fp, (xbInt32) llOffset, SEEK_CUR );  // issues with Borland
      iRc = fseek( fp, (long) llOffset, iWhence );
      if( iRc != 0 ){
        iRc2 = XB_SEEK_ERROR;
        iErrorStop = 310;
        throw iRc2;
    }
    #endif
  }
  catch (xbInt16 iRc2 ){
    xbString sMsg;
    sMsg.Sprintf( "xbFile::xbFseek() Exception Caught. Error Stop = [%d] iRc = [%d] errno = [%d]", iErrorStop, iRc2, errno );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc2 ));
    iRc = XB_SEEK_ERROR;
  }
  return iRc;
}
/************************************************************************/
size_t xbFile::xbFtell() {
  return (size_t) ftell( fp );
}

/************************************************************************/
xbInt16 xbFile::xbSetVbuf( xbInt16 iMultiUser ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt32 lRc = 0;
  xbInt16 iErrorStop = 0;

  try{
    if( iMultiUser == xbOn ){
      // turn off file buffering for multi user processing
      if(( lRc = setvbuf( fp, NULL, _IONBF, 0 )) != 0 ){
        iErrorStop = 100;
        iRc = XB_SETVBUF_ERROR;
        throw iRc;
      }
    } else {
      if(( pFileBuffer = (char *) malloc( BUFSIZ )) == NULL ){
        iErrorStop = 200;
        iRc = XB_NO_MEMORY;
        throw iRc;
      }
      if(( lRc = setvbuf( fp, pFileBuffer, _IOFBF, BUFSIZ )) != 0 ){
        iErrorStop = 210;
        iRc = XB_SETVBUF_ERROR;
        throw iRc;
      }
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbFile::xbSetVbuf() Exception Caught. Error Stop = [%d] iRc = [%d] errno = [%d] err2 = [%d]", iErrorStop, iRc, errno, lRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
    iRc = XB_SEEK_ERROR;
  }
  return iRc;
}
/************************************************************************/
/*  9/9/24  replaced with xbSetVBuf
void xbFile::xbFTurnOffFileBuffering() {
  setvbuf( fp, NULL, _IONBF, 0 );
}
*/
/************************************************************************/
xbInt16 xbFile::xbFwrite( const void *p, size_t size, size_t nmemb ) {

  size_t iRc;
  iRc = fwrite( p, size, nmemb, fp );
  if( iRc == nmemb )
    return XB_NO_ERROR;
  else
    return XB_WRITE_ERROR;
}
/************************************************************************/
xbInt16 xbFile::xbReadUntil( const char cDelim, xbString &sOut ){
  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  char    c;
  try{
    sOut = "";
    if(( iRc = xbFgetc( c )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }
    sOut = c;
    while( iRc == XB_NO_ERROR && c != cDelim ){
      if(( iRc = xbFgetc( c )) != XB_NO_ERROR ){
        iErrorStop = 110;
        throw iRc;
      }
      sOut += c; 
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbFile::xbReadUntil() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}

/************************************************************************/
xbInt16 xbFile::xbRemove() {
  return xbRemove( sFqFileName.Str(), 0 );
}
/************************************************************************/

xbInt16 xbFile::xbRemove( const xbString & sFileNameIn ) {
  return xbRemove( sFileNameIn, 0 );
}
/************************************************************************/
xbInt16 xbFile::xbRemove( const xbString & sFileNameIn, xbInt16 iOption ) {
  xbInt32 iRc = remove( sFileNameIn.Str());
  if( iRc != 0 )
    return XB_DELETE_FAILED;
  if( iOption == 1 ){
    xbString sFileName2 = sFileNameIn;
    if( sFileName2[sFileName2.Len()] == 'F' )
      sFileName2.PutAt( sFileName2.Len(), 'T' );
    else
      sFileName2.PutAt( sFileName2.Len(), 't' );
    iRc = remove( sFileName2.Str());
    if( iRc != 0 )
      return XB_DELETE_FAILED;
  }
  return XB_NO_ERROR;
}
/************************************************************************/
xbInt16 xbFile::xbRename( const xbString & sOldName, const xbString & sNewName ){
  if( rename( sOldName.Str(), sNewName.Str()))
    return XB_RENAME_ERROR;
  else 
    return XB_NO_ERROR;
}
/************************************************************************/
void xbFile::xbRewind() {
  rewind( fp );
}
/************************************************************************/
xbInt16 xbFile::xbTruncate( xbInt64 llSize ) {
  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;
  try{
    #ifdef HAVE_FTRUNCATE_F
    if(( iRc = ftruncate( iFileNo, llSize )) != 0 ){
      iErrorStop = 100;
      iRc = XB_WRITE_ERROR;
      throw iRc;
    }
    #elif defined(HAVE_SETENDOFFILE_F)
    if(( iRc = xbFseek( llSize, SEEK_SET )) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }
    if(( iRc = SetEndOfFile( fHandle )) == 0 ){
      iErrorStop = 120;
      iRc = XB_WRITE_ERROR;
      throw iRc;
    } else {
      iRc = XB_NO_ERROR;
    }
    #else
      // check that cmake can find function SetEndOfFile - 
      // cmake could not find for Borland 5.5
      FATAL_COMPILE_ERROR
      CANT_LOCATE_FUNCTION_ftruncate_or_SetEndOfFile

    #endif
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbFile::xbTruncate() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
/***********************************************************************/
#ifdef XB_LOCKING_SUPPORT
xbInt16 xbFile::GetLockRetryCount() const {
  if( iLockRetries == -1 )
    return xbase->GetDefaultLockRetries();
  else
    return iLockRetries;
}

void xbFile::SetLockRetryCount( xbInt16 iLockRetries ) {
  this->iLockRetries = iLockRetries;
}

xbInt16 xbFile::xbLock( xbInt16 iFunction, xbInt64 lOffset, size_t stLen ){
  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;
  xbInt16 iTries = 0;
  try{
    #ifdef HAVE_FCNTL_F
      /* Unix lock function, Mac OS doesn't work very well, problematic */
      if(( iRc = xbFseek( lOffset, SEEK_SET )) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      }
      struct flock fl;
      switch( iFunction ){
        case( XB_LOCK ):
          fl.l_type = F_WRLCK;
          break;
        case( XB_UNLOCK ):
          fl.l_type = F_UNLCK;
          break;
        default:
          iErrorStop = 110;
          iRc = XB_INVALID_LOCK_OPTION;
          throw iRc;
          break;
      }
      fl.l_whence = SEEK_CUR;
      fl.l_start  = 0;
      fl.l_len = (xbInt32) stLen;
      do{
        iRc = fcntl( iFileNo, F_SETLK, &fl );
        if( iRc && (errno == EACCES || errno == EAGAIN )){
          iTries++;
          xbase->xbSleep( xbase->GetDefaultLockWait() );
        } else if( iRc ){
          iErrorStop = 120;
          iRc = XB_LOCK_FAILED;
          throw iRc;
        }
      } while( iRc && iTries < GetLockRetryCount());
      if( iRc )
        iRc = XB_LOCK_FAILED;                // lock failed, don't log an exception

    #elif defined(HAVE_LOCKFILE_F)
      /* Windows 64 byte lock functions */
      /* split a quad word into two double words */
      typedef union{
        size_t  Qword;
        xbUInt32 Dword[2];
      } Qsplit;
      Qsplit lPos;
      Qsplit lLen;
      lPos.Qword = (size_t) lOffset;
      lLen.Qword = stLen;
      do{
        if( iFunction == XB_LOCK ){
          if(( iRc = LockFile( fHandle, lPos.Dword[0], lPos.Dword[1], lLen.Dword[0], lLen.Dword[1] )) == 0 ){
            iTries++;
            xbase->xbSleep( xbase->GetDefaultLockWait() );
          }
        }
        else if( iFunction == XB_UNLOCK ){
          if(( iRc = UnlockFile( fHandle, lPos.Dword[0], lPos.Dword[1], lLen.Dword[0], lLen.Dword[1] )) == 0 ){
            iTries++;
            xbase->xbSleep( xbase->GetDefaultLockWait() );
          }
        }
        else
        {
          iErrorStop = 130;
          iRc = XB_INVALID_LOCK_OPTION;
          throw iRc;
        }
      } while( iRc == 0 && iTries < GetLockRetryCount());
      if( iRc == 0 )
        iRc = XB_LOCK_FAILED;                // lock failed, don't log an exception
      else
        iRc = XB_NO_ERROR;
    #elif defined(HAVE_LOCKING_F) || defined(HAVE__LOCKING_F)
     /* older 32 bit locking functions */
      xbInt32 iLockType;
      if( iFunction == XB_LOCK ){
        iLockType = 2;
      } else if( iFunction == XB_UNLOCK ){
        iLockType = 0;
      } else {
        iErrorStop = 140;
        iRc = XB_INVALID_LOCK_OPTION;
        throw iRc;
      }
      if(( iRc = xbFseek( lOffset, SEEK_SET )) != XB_NO_ERROR ){
          iErrorStop = 150;
          iRc = XB_SEEK_ERROR;
          throw iRc;
      }
      do{
        #ifdef HAVE__LOCKING_F
        if(( iRc = _locking( iFileNo, iLockType, (long) stLen )) != 0 ){
        #else
        if(( iRc = locking( iFileNo, iLockType, stLen )) != 0 ){
        #endif
          iTries++;
          xbase->xbSleep( xbase->GetDefaultLockWait() );
        }
      } while( iRc != 0 && iTries < GetLockRetryCount());

      if( iRc != 0 )
        iRc = XB_LOCK_FAILED;                // lock failed, don't log an exception
      else
        iRc = XB_NO_ERROR;
    #else
       FATAL ERROR - CANT BUILD LIBRARY IN CURRENT CONFIG - MISSING LOCK FUNCTION - no file locking function defined in xbfile.cpp
    #endif
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbFile::xbLock() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
#endif

/***********************************************************************/
#ifdef XB_DEBUG_SUPPORT
void xbFile::Dump( const char * sTitle, xbInt16 iOutputOption ){

  xbString sMsg;

    sMsg = "xbFile::Dump() ----- Begin -----";
    xbase->WriteLogMessage( sMsg, iOutputOption );

    xbase->WriteLogMessage( sTitle, iOutputOption );

    if( fp )
      sMsg.Sprintf( "fp = %p", fp );
    else
      sMsg = "fp is null.\n";
    xbase->WriteLogMessage( sMsg, iOutputOption );

    if( pFileBuffer )
      sMsg.Sprintf( "pFileBuffer = %p", pFileBuffer );
    else
      sMsg = "pFileBuffer is null.";
    xbase->WriteLogMessage( sMsg, iOutputOption );

    sMsg.Sprintf( "Fully qualified file name = [%s]", sFqFileName.Str());
    xbase->WriteLogMessage( sMsg, iOutputOption );

    sMsg.Sprintf( "File name = [%s]", sFileName.Str());
    xbase->WriteLogMessage( sMsg, iOutputOption );

    sMsg.Sprintf( "Directory = [%s]", sDirectory.Str());
    xbase->WriteLogMessage( sMsg, iOutputOption );

    sMsg.Sprintf( "bFileOpen = [%d] BlockSize = [%d] iOpenMode = [%d] iShareMode = [%d] iFileNo = [%d]", bFileOpen, ulBlockSize, iOpenMode, iShareMode, iFileNo );
    xbase->WriteLogMessage( sMsg, iOutputOption );

    #ifdef XB_LOCKING_SUPPORT
    sMsg.Sprintf( "iLockRetries = [%d]", iLockRetries );
    xbase->WriteLogMessage( sMsg, iOutputOption );
    #endif

    xbUInt64 ullFs = 0;
    GetFileSize( ullFs );
    sMsg.Sprintf( "File size = [%d]", ullFs );
    xbase->WriteLogMessage( sMsg, iOutputOption );


    #ifdef HAVE_SETENDOFFILE_F
    // HANDLE   fHandle = null; 

    if( fHandle )
      sMsg.Sprintf( "fHandle = %p", fHandle );
    else
      sMsg = "fHandle is null.\n";
    xbase->WriteLogMessage( sMsg, iOutputOption );
    #endif

    sMsg.Sprintf( "File pos = %ld", ftell( fp ));

    sMsg = "xbFile::Dump() ----- End -----";
    xbase->WriteLogMessage( sMsg, iOutputOption );

}
/***********************************************************************/
xbInt16 xbFile::DumpBlockToDisk( xbUInt32 ulBlockNo, size_t lBlkSize ){
  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;
  xbUInt32 ulStartBlock;
  xbUInt32 ulEndBlock;
  char *p = 0x00;
  xbString sDir;
  xbString sFn;
  char *buf = NULL;
  FILE *fpd = NULL;
  try{

    if( ulBlockNo == 0 ){
      ulStartBlock = 0;
      xbUInt64 ullFileSizeulBlockNo;
      if(( iRc = GetFileSize( ullFileSizeulBlockNo )) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      }
      ulEndBlock = (xbUInt32) (ullFileSizeulBlockNo / lBlkSize);
    } else {
      ulStartBlock = ulBlockNo; 
      ulEndBlock = ulBlockNo;
    }
    if(( buf = (char *) malloc( lBlkSize )) == NULL ){
      iErrorStop = 110;
      iRc = XB_NO_MEMORY;
      throw iRc;
    }
    sDir = xbase->GetLogDirectory();
    char cLastChar = sDir[sDir.Len()];
    for( xbUInt32 l = ulStartBlock; l < ulEndBlock; l++ ){
      if(( iRc = ReadBlock( l, lBlkSize, buf )) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }
      // build logfile name
      if( cLastChar != '/' && cLastChar != '\\' )
        sFn.Sprintf( "%s/BlockDump.B%ld", sDir.Str(), l);
      else
        sFn.Sprintf( "%sBlockDump.%ld", sDir.Str(), l);

      // open the dump file for append
      #ifdef HAVE__FSOPEN_F
      if(( fpd = _fsopen( sFn.Str(), "w+b", 0x40 )) == NULL ){
      #else
      if(( fpd = fopen( sFn.Str(), "w+b")) == NULL ){
      #endif
        iErrorStop = 130;
        iRc = XB_OPEN_ERROR;
        throw iRc;
      }

      // dump the block to the file
      p = buf;
      for( size_t l = 0; l < lBlkSize; l++ ){
        //if( fputc( *p, fpd ) != *p ){
        if( fputc( *p, fpd ) == EOF ){
          iErrorStop = 140;
          iRc = XB_WRITE_ERROR;
          throw iRc;
        }
        p++;
      }
      // close the dump file
      fclose( fpd );
    }
    // free the buffer
    if( buf )
      free( buf );
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbFile::DumpBlockToDisk() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
    if( buf )
      free( buf );
    if( fpd )
      fclose( fpd );
  }
  return iRc;
}
/***********************************************************************/
xbInt16 xbFile::DumpMemToDisk( char *p, size_t lSize ){

  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;
  xbString sDir;
  xbString sFn;
  FILE *fpd = NULL;
  try{
    sDir = xbase->GetLogDirectory();
    char cLastChar = sDir[sDir.Len()];
    // build logfile name
    if( cLastChar != '/' && cLastChar != '\\' )
      sFn.Sprintf( "%s/MemDump.txt", sDir.Str());
    else
      sFn.Sprintf( "%sMemDump.txt", sDir.Str());

    // open the dump file for append
    #ifdef HAVE__FSOPEN_F
    if(( fpd = _fsopen( sFn.Str(), "w+b", 0x40 )) == NULL ){
    #else
    if(( fpd = fopen( sFn.Str(), "w+b")) == NULL ){
    #endif
      iErrorStop = 100;
      iRc = XB_OPEN_ERROR;
      throw iRc;
    }

    int i;
    // dump the block to the file
    for( size_t l = 0; l < lSize; l++ ){
      i = *p;
      if( fputc( i, fpd ) == EOF ){
        iErrorStop = 110;
        iRc = XB_WRITE_ERROR;
        throw iRc;
      }
      p++;
    }
    // close the dump file
    fclose( fpd );
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbFile::DumpBlockToDisk() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
    if( fpd )
      fclose( fpd );
  }
  return iRc;
}
#endif
/***********************************************************************/
}    /* namespace xb  */
