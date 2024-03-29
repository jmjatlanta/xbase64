/*  xbfile.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

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
//! @brief Class Constructor.
xbFile::xbFile( xbXBase * x ){
  fp           = NULL;
  bFileOpen    = xbFalse;
  ulBlockSize  = 0;
  iFileNo      = 0;
  xbase        = x;
  if( GetMultiUser() == xbTrue )
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
//! @brief Class Destructor.
xbFile::~xbFile(){
  if( bFileOpen )
    xbFclose();
}

/************************************************************************/
//! @brief Create a unique file name.
/*!
  Given a directory and file extension as inputs, create a unique file name.

  \param sDirIn Directory
  \param sExtIn File Extension
  \param sFqnOut   A fully qualifed unique file name as output
  \param iOption 0 - look only for one file for a given directory and extension<br>
                 1 - if file name extension is "dbf" or "DBF", verify unique dbt or DBT (memo) file is also available<br>
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/


xbInt16 xbFile::CreateUniqueFileName( const xbString & sDirIn, const xbString & sExtIn, xbString &sFqnOut, xbInt16 iOption ){ 

  xbBool   bUniqueFileNameFound = xbFalse;
  xbFile   f( xbase);
  xbInt32  l = 1;
  xbString sMemoFileName;

  xbString sDir = sDirIn;
  char     c = GetPathSeparator();
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
//! @brief Determine which version the memo (dbt) file is.
/*!

  This routine uses the first byte in the dbf file to determine which memo
  file version is in use.  The main difference between version 3 and 4 is that
  version 4 will reuse blocks if they become available.  Version 3 does not.

  \param cFileTypeByte is an output field and is one of:<br>
  <br>
    0 - none<br>
    3 - Dbase III+<br>
    4 - Dbase IV<br>
  
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbFile::DetermineXbaseMemoVersion( unsigned char cFileTypeByte ) const {


  if( BitSet( cFileTypeByte, 3 ) && BitSet( cFileTypeByte, 7 ))
    return 4;
  else if( BitSet( cFileTypeByte, 7 ))
    return 3;

  return 0;
}
/*************************************************************************/
//! @brief Determine xbase dbf version.
/*!

  This routine is used to determine which version of the Xbase classes can
  be used for a given DBF file.<br>

  It attempts to use the highest version compiled into the library.<br>

  References:<br>
    This routine uses the first byte from the dbf file.<br>
    Per DBase documentation:<br>
    Valid dBASE for Windows table file, bits 0-2 indicate version number: 3 for dBASE Level 5, 4 for dBASE Level 7.<br>
    Bit 3 and bit 7 indicate presence of a dBASE IV or dBASE for Windows memo file;<br>
    Bits 4-6 indicate the presence of a dBASE IV SQL table;<br>
    Bit 7 indicates the presence of any .DBT memo file (either a dBASE III PLUS type or a dBASE IV or dBASE for Windows memo file).<br>
<br>
    Bachmann spec (used extensively in library build), page 7 - does not match DBase documentation<br>
<br>
  returns<br>
    0 - unknown<br>
    3 - Dbase level 3<br>
    4 - Dbase level 4<br>
    5 - Dbase Level 5     (future)<br>
    7 - Dbase Level 7     (future)<br>
<br>
    1x - Clipper files    (future)<br>
    2x - Foxbase files    (future)<br>
<br>
*/

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
//! @brief Get a portable double value.
/*!
 
  This routine returns a double value from an 8 byte character stream, 
  accounting for endian type.
 
  Converts a double (64 bit floating point) value stored at p from a portable 
  format to the machine format.
  
  \param p pointer to memory containing the portable double value
  
  \returns the double value.
*/

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
//! @brief Get a portable long value.

/*!
  This routine returns a long int value from a 4 byte character stream, 
  accounting for endian type.

  \param p pointer to memory containing the portable long value

  \returns the long value.
*/

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
//! @brief Get a portable unsigned long value.

/*!
  This routine returns an unsigned long int value from a 4 byte character stream, 
  accounting for endian type.

  \param p pointer to memory containing the portable long value

  \returns the unsigned long value.
*/

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
//! @brief Get a portable short value.

/*!
  This routine returns a short int value from a 2 byte character stream, 
  accounting for endian type.

  \param p pointer to memory containing the portable long value

  \returns the short value.
*/

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
//! @brief Get a portable unsigned short value.

/*!
  This routine returns a short unsigned int value from a 2 byte character stream, 
  accounting for endian type.

  \param p pointer to memory containing the portable long value

  \returns the short value.
*/

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
//! @brief Write a portable double value to memory location.
/*!
  Converts a double (64 bit floating point) value from machine format to a
  portable format and stores the converted value in the memory referenced
  by c.
  
  This routine puts a double value to an 8 byte character stream 

  \param c Pointer to memory to hold converted value
  \param d Input value to be converted
*/

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
//! @brief Write a portable short value to memory location.
/*!
  Converts a short (16 bit integer) value from machine format to a
  portable format and stores the converted value in the memory referenced
  by c.

  This routine puts a short value to a 2 byte character stream 

  \param c Pointer to memory to hold converted value
  \param s Input value to be converted
*/

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
//! @brief Write a portable unsigned short value to memory location.
/*!
  Converts an unsigned short (16 bit integer) value from machine format to a
  portable format and stores the converted value in the memory referenced
  by c.
  
  This routine puts an unsigned short value to a 2 byte character stream 
  
  \param c Pointer to memory to hold converted value
  \param s Input value to be converted
*/

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
//! @brief Write a portable long value to memory location.
/*!
  Converts a long (32 bit integer) value from machine format to a
  portable format and stores the converted value in the memory referenced
  by c.
  
  This routine puts a long value to a 4 byte character stream.
  
  \param c Pointer to memory to hold converted value
  \param l Input value to be converted
*/

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
//! @brief Write a portable unsigned long value to memory location.
/*!
  Converts an unsigned long (32 bit integer) value from machine format to a
  portable format and stores the converted value in the memory referenced
  by c.

  This routine puts an unsigned long value to a 4 byte character stream.

  \param c Pointer to memory to hold converted value
  \param ul Input value to be converted
*/

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
//! @brief Determines if a file exists.
/*!
  \returns xbTrue if file exists<br>
           xbFalse if file does not exist

*/
xbBool xbFile::FileExists() const {
  return FileExists( sFqFileName, 0 );
}
/************************************************************************/
//! @brief Determines if a file exists.
/*!
  \param iOption if 1, assume this is a request for a dbf file and 
    check for the a dbt memo file also, returns true if both files are found

  \returns xbTrue if both files exist<br>
           xbFalse if file does not exist

*/
xbBool xbFile::FileExists( xbInt16 iOption ) const {
  return FileExists( sFqFileName, iOption );
}

/************************************************************************/
//! @brief Determines if a file exists.
/*!
  \param sFileName - file name to check for

  \returns xbTrue if file exists<br>
           xbFalse if file does not exist
*/

xbBool xbFile::FileExists(const xbString &sFileName ) const {
  return FileExists( sFileName, 0 );
}

/************************************************************************/
//! @brief Determines if a file exists.
/*!
  \param sFileName - file name to check for
  \param iOption if 1, assume this is a request for a dbf file and 
    check for the a dbt memo file also, returns true if both files are found

  \returns xbTrue if both dbf and dbt files exist<br>
           xbFalse if file does not exist
*/

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
//! @brief Determines if file is open.
/*!

  \returns xbTrue if file is open<br>
           xbFalse if file is not open
*/

xbBool xbFile::FileIsOpen() const {
  return bFileOpen;
}
/************************************************************************/
//! @brief Get the block size.
/*!
  \returns Block Size
*/

xbUInt32 xbFile::GetBlockSize() const {
  return ulBlockSize;
}

/************************************************************************/
//! @brief Get the directory name.
/*!
  \returns the directory name of the file
*/

const xbString & xbFile::GetDirectory() const {
  return sDirectory;
}

/************************************************************************/
//! @brief Get the directory part of the file name.
/*!
  \param sFileDirPartOut - the returned directory name
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbFile::GetFileDirPart( xbString & sFileDirPartOut ) const {
  return GetFileDirPart( sFqFileName, sFileDirPartOut );
}

/************************************************************************/
//! @brief Get the directory part of the file name.
/*!
  \param sCompleteFileNameIn - a fully qualfied input file name
  \param sFileDirPartOut - the returned directory name part out of sCompleteFileNameIn
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/


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
//! @brief Get the extension part of the file name.
/*!
  \param sFileNameExtOut - the returned extension part out of sCompleteFileNameIn
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbFile::GetFileExtPart( xbString & sFileNameExtOut ) const {
  return GetFileExtPart( sFqFileName, sFileNameExtOut );
}

/************************************************************************/
//! @brief Get the extension part of the file name.
/*!

  \param sCompleteFileNameIn - a fully qualfied input file name

  \param sFileExtPartOut - the returned directory name part out of sCompleteFileNameIn
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/


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
//! @brief Get the time of last file modification timestamp as reported by the OS.
/*!
  \param mtime - returned time of last file modification
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

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
//! @brief Get the file name.
/*!
  \returns the file name portion of the file
*/

const xbString & xbFile::GetFileName() const {
  return sFileName;
}


/************************************************************************/
//! @brief Get the name part of the file name.
/*!
  \param sFileNamePartOut - the returned file name part out of sCompleteFileNameIn
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/


xbInt16 xbFile::GetFileNamePart( xbString & sFileNamePartOut ) const {
  return GetFileNamePart( sFqFileName, sFileNamePartOut );
}

/************************************************************************/
//! @brief Get the name part of the file name.
/*!
  \param sCompleteFileNameIn - a fully qualified input file name
  \param sFileNamePartOut - the returned file name part out of sCompleteFileNameIn
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

//***********  fixme should this be static?????

xbInt16 xbFile::GetFileNamePart( const xbString & sCompleteFileNameIn, xbString & sFileNamePartOut ) const {
  /* extract the file name part out of the string */

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
//! @brief Get the size of the file as reported by the OS.
/*!
  \param ullFileSize - unsigned long long field as output
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

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
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}

/************************************************************************/
//! @brief Get the file type  aka Capitalized file extension
/*!
  \param sFileTypeOut - the returned extension part out of sCompleteFileNameIn
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbFile::GetFileType( xbString & sFileTypeOut ) const {

  xbInt16 iRc = GetFileExtPart( sFqFileName, sFileTypeOut );
  sFileTypeOut.ToUpperCase();
  return iRc;
}
/************************************************************************/
//! @brief Get the fully qualified file name.
/*!
  \returns the fully qualfied name of the file
*/

const xbString & xbFile::GetFqFileName() const {
  return sFqFileName;
}

/************************************************************************/
//! @brief Get the open mode of the file.
/*!
  \returns XB_READ<br>
           XB_READ_WRITE<br>
           XB_WRITE<BR>
*/

xbInt16 xbFile::GetOpenMode() const {
  return iOpenMode;
}

/************************************************************************/
//! @brief Get the share mode of the file.
/*!
  \returns XB_SINGLE_USER - (file buffering on><br>
           XB_MULTI_USER - (file buffering off)<br>
*/

xbInt16 xbFile::GetShareMode() const {
  return iShareMode;
}

/************************************************************************/
//! @brief Get the file type byte and version of the dbf file.
/*!

  Pull the first bye off the DBF file for further inspection.
  First byte has various bits set to determine what the file format is.

  \param sFileName - Name of file to examine
  \param iVersion - Returned file version
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16  xbFile::GetXbaseFileTypeByte( const xbString &sFileName, xbInt16 &iVersion )
{
  unsigned char cFileTypeByte;
  return GetXbaseFileTypeByte( sFileName, cFileTypeByte, iVersion );
}

/************************************************************************/
//! @brief Get the file type byte and version of the dbf file.
/*!
  Pull the first bye off the DBF file for further inspection.
  First byte has various bits set to determine what the file format is.

  \param sFileName - Name of file to examine
  \param cFileTypeByte - Retruned first byte of dbf file
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/


xbInt16  xbFile::GetXbaseFileTypeByte( const xbString &sFileName, unsigned char &cFileTypeByte )
{
  xbInt16 iVersion;
  return GetXbaseFileTypeByte( sFileName, cFileTypeByte, iVersion );
}

/************************************************************************/
//! @brief Get the file type byte and version of the dbf file.
/*!
  Pull the first bye off the DBF file for further inspection.
  First byte has various bits set to determine what the file format is.


  \param sFileName - Name of file to examine
  \param cFileTypeByte - Returned first byte of dbf file
  \param iVersion - Returned file version
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

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
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}

/************************************************************************/
//! @brief Determines status of file extension.
/*!

  \param sFileName - Name of file to examine
  \param iOption - Inspection type<br>
       1 check for DBF<br>
       2 check for NDX<br>
       3 check for MDX<br>
       4 check for NTX<br>

  \returns 0 if suffix found<br>
           1 if suffix not found, lower case<br>
           2 is suffix not found, upper case<br>

*/

xbInt16 xbFile::NameSuffixMissing( const xbString & sFileName, xbInt16 iOption ) const {

  xbUInt32 ulLen = sFileName.Len();
  if( ulLen <= 4 ){
    if( sFileName[ulLen] >= 'A' && sFileName[ulLen] <= 'Z' )
      return 2;
    else
      return 1;
  }
  if( iOption == 1 && sFileName[ulLen-3] == '.' &&
     ( sFileName[ulLen-2] == 'd' || sFileName[ulLen-2] == 'D' ) && 
     ( sFileName[ulLen-1] == 'b' || sFileName[ulLen-1] == 'B' ) && 
     ( sFileName[ulLen]   == 'f' || sFileName[ulLen]   == 'F' )
    )
      return 0;
  if( iOption == 2 && sFileName[ulLen-3] == '.' &&
     ( sFileName[ulLen-2] == 'n' || sFileName[ulLen-2] == 'N' ) && 
     ( sFileName[ulLen-1] == 'd' || sFileName[ulLen-1] == 'D' ) && 
     ( sFileName[ulLen]   == 'x' || sFileName[ulLen]   == 'X' )
     )
      return 0;
  if( iOption == 3 && sFileName[ulLen-3] == '.' &&
     ( sFileName[ulLen-2] == 'm' || sFileName[ulLen-2] == 'M' ) && 
     ( sFileName[ulLen-1] == 'd' || sFileName[ulLen-1] == 'D' ) && 
     ( sFileName[ulLen]   == 'x' || sFileName[ulLen]   == 'X' )
     )
      return 0;
  if( iOption == 4 && sFileName[ulLen-3] == '.' &&
     ( sFileName[ulLen-2] == 'n' || sFileName[ulLen-2] == 'N' ) && 
     ( sFileName[ulLen-1] == 't' || sFileName[ulLen-1] == 'T' ) && 
     ( sFileName[ulLen]   == 'x' || sFileName[ulLen]   == 'X' )
    )
     return 0;
  // next line might be problematic if file naem has mixed case and extension is missing
  if( sFileName[ulLen-4] >= 'A' && sFileName[ulLen-4] <= 'Z' )
    return 2;
  else
    return 1;
}

/***********************************************************************/
//! @brief Read a block of data from file.
/*!

  \param ulBlockNo - block number to read
  \param lReadSize - size of data to read at block location, set to 0 to read blocksize
  \param *buf - pointer to buffer to write output data, assumed to be previosuly allocated
                and large enough to contain data
  \returns <a href="xbretcod_8h.html">Return Codes</a>

*/

xbInt16 xbFile::ReadBlock( xbUInt32 ulBlockNo, size_t lReadSize, void * buf ){
  return ReadBlock( ulBlockNo, ulBlockSize, lReadSize, buf );
}

/***********************************************************************/
//! @brief Read a block of data from file.
/*!

  \param ulBlockNo - block number to read
  \param ulBlockSize - block size
  \param lReadSize - size of data to read at block location, set to 0 to read blocksize
  \param buf - pointer to buffer to write output data, assumed to be previosuly allocated
                and large enough to contain data
  \returns <a href="xbretcod_8h.html">Return Codes</a>

*/
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
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}
/************************************************************************/
//! @brief Set the block size.
/*!

  \param ulBlockSize - unsigned long block size, divisible by 512
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbFile::SetBlockSize( xbUInt32 ulBlockSize ){
  if( ulBlockSize %512 != 0 )
    return XB_INVALID_BLOCK_SIZE;

  this->ulBlockSize = ulBlockSize;
  return XB_NO_ERROR;
}
/************************************************************************/
//! @brief Set the directory.
/*!
  \param sDirectory - Valid directory name
*/

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
//! @brief Set the filename.
/*!
  This routine builds out two internal variables from the input file name<br>
    sFileName   - the file name part<br>
    sFqFileName - the fully qualified file name<br>


  \param sFileName - Input file name
*/
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
      sDirectory = GetDataDirectory();
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
//! @brief Set the fully qualifed filename.
/*!
  \param sFqFileName - Fully qualifed input file name
*/

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
//! @brief Set Home Folders.
/*!
  Create xbase64 log, data and temp folders in the home directory for current usre.

  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbFile::SetHomeFolders(){

  xbInt16 iErrorStop = 0;
  xbInt16 iRc = XB_NO_ERROR;
  xbString sHomeDir;
  char cPathSeparator;
  xbString sDflt;

  try{

    GetHomeDir( sHomeDir );
    // std::cout << "CreateHomeFolders() home dir = [" << sHomeDir.Str() << "]\n";

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
    //std::cout << "logdir = " << sWork.Str() << "\n";

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

    //std::cout << "datadir = " << sWork.Str() << "\n";

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
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}

/************************************************************************/
//! @brief Write a block of data to file.
/*!

  \param ulBlockNo - block number to write
  \param lWriteSize - size of data to write, set to 0 to write blocksize
  \param *buf - pointer to buffer of data to be written
  \returns <a href="xbretcod_8h.html">Return Codes</a>

*/
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
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}

/************************************************************************/
//! @brief Xbase wrapper for standard libary fclose.
/*!
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

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
  return XB_NO_ERROR;
}



/************************************************************************/
//! @brief Xbase wrapper for standard libary feof.
/*!
  \returns non zero if end-of-file is set for the stream.
*/
xbInt16 xbFile::xbFeof(){
  return feof( fp );
}

/************************************************************************/
//! @brief Xbase wrapper for standard libary fflush.
/*!
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbFile::xbFflush() {

  if( fflush( fp ) )
    return XB_WRITE_ERROR;
  else
    return XB_NO_ERROR;
}

/************************************************************************/
//! @brief Xbase wrapper for standard libary fgetc.
/*!
  \param c - output integer returned by fgetc
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbFile::xbFgetc( xbInt32 &c ) {

  int i;

  i = fgetc( fp );
  if( i == EOF )
    return XB_EOF;

  c = i;
  return XB_NO_ERROR;
}

/************************************************************************/
//! @brief Xbase wrapper for standard libary fgetc.
/*!
  \param c - output character returned by fgetc
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbFile::xbFgetc( char &c ) {

  int i;
  i = fgetc( fp );
  if( i == EOF )
    return XB_EOF;

  c = (char) i;
  return XB_NO_ERROR;
}
/************************************************************************/
//! @brief Xbase wrapper for standard libary fgets.
/*!
  \param lSize - reads in at most, one character less than lSize
  \param s - an xbString containing data returned by fseek
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

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
//! @brief Xbase wrapper for standard libary fopen.
/*!

  This routine supports all the standard C library open modes.  The Xbase routines only
  use "r" and "r+b".

  \param sOpenMode
    <table>
    <tr><th>OpenMode<th>Description
    <tr><td>r<td>Reading
    <tr><td>r+<td>Reading and Writing
    <tr><td>w<td>Open for writing. Truncate to zero bytes if it exists
    <tr><td>w+<td>Open for reading and writing, truncate to zero bytes if it exists
    <tr><td>a<td>Open for append
    <tr><td>a+<td>Open for reading and writing (at end).
    </table>
    The mode can also include the letter "b" for binary ie; "r+b". The "b" is ignored on
    POSIX compliant systems, but is included for cross platform compatibility.
  \param sFileName File name to open
  \param iShareMode
    XB_SINGLE_USER<br>
    XB_MULTI_USER<br>
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbFile::xbFopen( const xbString &sOpenMode, const xbString &sFileName, xbInt16 iShareMode ) {
  if( sFileName == "" || sFqFileName == "" )
    SetFileName( sFileName );
  return xbFopen( sOpenMode, iShareMode );
}

/************************************************************************/
//! @brief Xbase wrapper for standard libary fopen.
/*!

  This routine supports all the standard C library open modes.  The Xbase routines only
  use "r" and "r+".


  \param sOpenMode
    <table>
    <tr><th>OpenMode<th>Description
    <tr><td>r<td>Reading
    <tr><td>r+<td>Reading and Writing
    <tr><td>w<td>Open for writing. Truncate to zero bytes if it exists
    <tr><td>w+<td>Open for reading and writing, truncate to zero bytes if it exists
    <tr><td>a<td>Open for append
    <tr><td>a+<td>Open for reading and writing (at end).
    </table>
    The mode can also include the letter "b" for binary ie; "r+b". The "b" is ignored on
    POSIX compliant systems, but is included for cross platform compatibility.
  \param iShareMode
    XB_SINGLE_USER<br>
    XB_MULTI_USER<br>
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbFile::xbFopen( const xbString & sOpenMode, xbInt16 iShareMode ) {

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
    this->iShareMode = iShareMode;

    #ifdef HAVE__FILENO_F
    iFileNo = _fileno( fp );
    #else
    iFileNo = fileno( fp );
    #endif

    #ifdef HAVE_SETENDOFFILE_F
    //used by visual studio, 32 bit
    fHandle = (HANDLE) _get_osfhandle( iFileNo );
    #endif

    #ifdef XB_LOCKING_SUPPORT
    if( iShareMode )
      xbFTurnOffFileBuffering();
    #endif

    return XB_NO_ERROR;
  }
  else
    return XB_OPEN_ERROR;
}
/************************************************************************/
//! @brief Xbase wrapper for standard libary fopen.
/*!
  \param iOpenMode
    XB_READ<br>
    XB_READ_WRITE<br>
  \param iShareMode
    XB_SINGLE_USER<br>
    XB_MULTI_USER<br>
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

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
//! @brief Xbase wrapper for standard libary fputc.
/*!
  \param c Character to write
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/


xbInt16 xbFile::xbFputc( xbInt32 c ) {
  return xbFputc( c, 1 );
}

/************************************************************************/
//! @brief Xbase wrapper for standard libary fputc.
/*!
  \param c Character to write
  \param iNoOfTimes Number of times to write the character
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/


xbInt16 xbFile::xbFputc( xbInt32 c, xbInt32 iNoOfTimes ) {

  for( xbInt32 l = 0; l < iNoOfTimes; l++ )
    if( fputc( c, fp ) != (int) c )
      return XB_WRITE_ERROR;

  return XB_NO_ERROR;
}

/************************************************************************/
//! @brief Xbase wrapper for standard libary fputs.
/*!
  \param s xbString to write to file
   \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbFile::xbFputs( const xbString & s ){
  if( fputs( s.Str(), fp ) < 0 )
    return XB_WRITE_ERROR;
  else
    return XB_NO_ERROR;
}

/************************************************************************/
//! @brief Xbase wrapper for standard libary fread.
/*!
  \param p Pointer to data to write
  \param size size of write
  \param nmemb Number of times to read it
   \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbFile::xbFread( void *p, size_t size, size_t nmemb ) {

  size_t iRc;
  iRc = fread( p, size, nmemb, fp );
  if( iRc == nmemb )
    return XB_NO_ERROR;
  else
    return XB_READ_ERROR;
}

/************************************************************************/
//! @brief Xbase wrapper for standard libary fseek.
/*!
  \param lOffset Position in file to seek to
  \param iWhence SEEK_SET - from beginning of file<br>
                 SEEK_CUR - from current position<br>
                 SEEK_END - from end of file<br>
   \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbFile::xbFseek( xbInt64 lOffset, xbInt32 iWhence ) {

  xbInt32 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  try {

    #if defined(HAVE_FSEEKO_F)
    iRc = fseeko( fp, lOffset, iWhence );
    if( iRc != 0 ){
      iErrorStop = 100;
      throw iRc;
    }

    #elif defined(HAVE__FSEEKI64_F)
    iRc = _fseeki64( fp, lOffset, iWhence );
    if( iRc != 0 ){
      iErrorStop = 110;
      throw iRc;
    }
    #else
      #ifdef XB_PLATFORM_32
      /* if request is larger than 2 gig,this is a part of a locking request,
        assuming offset is less than 4 gig, split the request into 2 fseek calls */
      if( lOffset > 2147483647 && iWhence == SEEK_SET ){
        /* move forward max amt - 2G */
        if(( iRc = fseek( fp, 2147483647, SEEK_SET )) != 0 ){
          iErrorStop = 120;
          throw iRc;
        }
        lOffset -= 2147483647;
        iWhence = SEEK_CUR; 
      }
      #endif
      iRc = fseek( fp, (long) lOffset, iWhence );
      if( iRc != 0 ){
        iErrorStop = 310;
        throw iRc;
      }
    #endif
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbFile::xbFseek() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
    iRc = XB_SEEK_ERROR;
  }
  return iRc;
}

/************************************************************************/
//! @brief Xbase wrapper for standard libary ftell.
/*!
  Returns the current file position.
   \returns Current file position.
*/

size_t xbFile::xbFtell() {
  return (size_t) ftell( fp );
}

/************************************************************************/
//! @brief Turn off file buffering.
/*!
   Turns off file buffering. File buffering can't be used while in multi user mode.

*/

void xbFile::xbFTurnOffFileBuffering() {
  setvbuf( fp, NULL, _IONBF, 0 );
}

/************************************************************************/
//! @brief Xbase wrapper for standard libary fwrite.
/*!
  \param p Pointer to data buffer to write
  \param size Size of data to write
  \param nmemb Number of times to write data buffer
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbFile::xbFwrite( const void *p, size_t size, size_t nmemb ) {

  size_t iRc;
  iRc = fwrite( p, size, nmemb, fp );
  if( iRc == nmemb )
    return XB_NO_ERROR;
  else
    return XB_READ_ERROR;
}

/************************************************************************/
//! @brief Read file until a particular character is encountered on input stream.
/*!
  This routine will read until cDelim is encountered or eof, which ever occurs first.

  \param cDelim Delimiter to stop writing at.
  \param sOut Output xbString containing data read
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

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
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}

/************************************************************************/
//! @brief Delete file.
/*!
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbFile::xbRemove() {
  return xbRemove( sFqFileName.Str(), 0 );
}

/************************************************************************/
//! @brief Delete file.
/*!
  \param sFileNameIn Name of file to delete
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbFile::xbRemove( const xbString & sFileNameIn ) {
  return xbRemove( sFileNameIn, 0 );
}


/************************************************************************/
//! @brief Delete file.
/*!
  \param sFileNameIn Name of file to delete
  \param iOption If Set to 1, assume this is a delete request for a dbf file, and should rename the dbt file also
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
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
//! @brief Rename file.
/*!
  \param sOldName Original file name
  \param sNewName New file name
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbFile::xbRename( const xbString & sOldName, const xbString & sNewName ){
  if( rename( sOldName.Str(), sNewName.Str()))
    return XB_RENAME_ERROR;
  else 
    return XB_NO_ERROR;
}

/************************************************************************/
//! @brief Xbase wrapper for rewind.
/*!
  Set file pointer at beginning of file.
*/

void xbFile::xbRewind() {
  rewind( fp );
}

/************************************************************************/
//! @brief Xbase wrapper for ftruncate.
/*!
  Set file size to llSize
  \param llSize New file size.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
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
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}

/***********************************************************************/

#ifdef XB_LOCKING_SUPPORT

//! @brief Lock / unlock file.
/*!
  \param iFunction XB_LOCK<br>
                  XB_UNLOCK<br>
  \param lOffset Position in file to lock
  \param stLen Length to lock
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbFile::xbLock( xbInt16 iFunction, xbInt64 lOffset, size_t stLen ){

  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;
  xbInt16 iTries = 0;

  try{
    #ifdef HAVE_FCNTL_F
      /* Unix lock function */

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
          xbase->xbSleep( GetDefaultLockWait() );
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
            xbase->xbSleep( GetDefaultLockWait() );
          }
        }
        else if( iFunction == XB_UNLOCK ){
          if(( iRc = UnlockFile( fHandle, lPos.Dword[0], lPos.Dword[1], lLen.Dword[0], lLen.Dword[1] )) == 0 ){
            iTries++;
            xbase->xbSleep( GetDefaultLockWait() );
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
        if(( iRc = _locking( iFileNo, iLockType, stLen )) != 0 ){
        #else
        if(( iRc = locking( iFileNo, iLockType, stLen )) != 0 ){
        #endif
          iTries++;
          xbase->xbSleep( GetDefaultLockWait() );
        }
      } while( iRc != 0 && iTries < GetLockRetryCount());

      if( iRc != 0 )
        iRc = XB_LOCK_FAILED;                // lock failed, don't log an exception
      else
        iRc = XB_NO_ERROR;

    #else

       FATAL ERROR - CANT BUILD LIBRARY IN CURRENT CONFIG - MISSING - no file locking function defined in xbfile.cpp

    #endif

  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbFile::xbLock() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }

  return iRc;


}

//! @brief Return the locking retry setting.
/*!

  \returns The lock retry setting for this file or ths system default setting if the lock retry for the file
  has not been set.
*/

xbInt16 xbFile::GetLockRetryCount() const {

  if( iLockRetries == -1 )
    return xbase->GetDefaultLockRetries();
  else
    return iLockRetries;
}

//! @brief Set the lock retry countr for this specific file.
/*!
  \param iLockRetries The number of retries to attempt before returning failure for this file
*/

void xbFile::SetLockRetryCount( xbInt16 iLockRetries ) {
  this->iLockRetries = iLockRetries;
}

#endif


/***********************************************************************/
#ifdef XB_DEBUG_SUPPORT
//! @brief Debugging routine - dump mem to the log file.
/*!
  This routine dumps data from meemory to the log file. This is
  primarily used for debugging and analysis purposes.

  \param p Pointer to data to write
  \param lBlxkSize Size of block
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbFile::DumpMemToDisk( char *p, size_t lSize ){


  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;
  xbString sDir;
  xbString sFn;

  FILE *fpd = NULL;

  try{

    // sDir = GetLogDirectory();
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
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
    if( fpd )
      fclose( fpd );
  }
  return iRc;
}

/***********************************************************************/
//! @brief Debugging routine - dump a block to the log file.
/*!
  This routine dumps a block to the log file. This is
  primarily used for debugging and analysis purposes.

  \param ulBlockNo Block number to write
  \param lBlxkSize Size of block
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
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

//    sDir = GetLogDirectory();
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
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
    if( buf )
      free( buf );
    if( fpd )
      fclose( fpd );
  }
  return iRc;
}
#endif
/***********************************************************************/
}    /* namespace xb  */



