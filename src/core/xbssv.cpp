/* xbssv.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

#include "xbase.h"

namespace xb{

const xbErrorMessage xbErrorMessages[] = {
  { XB_NO_ERROR,                "No Error" },
  { XB_NO_MEMORY,               "No Memory" },
  { XB_INVALID_OPTION,          "Invalid Option" },
  { XB_INVALID_PARAMETER,       "Invalid Parameter" },
  { XB_DUP_TABLE_OR_ALIAS,      "Duplicate Alias/Table Name" },
  { XB_INVALID_NODELINK,        "Invalid Node Link" },
  { XB_KEY_NOT_UNIQUE,          "Key Not Unique" },
  { XB_MEMCPY_ERROR,            "Memory copy failure" },
  { XB_ALREADY_DEFINED,         "Object already defined" },
  { XB_FILE_EXISTS,             "File Already Exists" },
  { XB_ALREADY_OPEN,            "Database already open" },
  { XB_DBF_FILE_NOT_OPEN,       "DBF File Not Open" },
  { XB_FILE_NOT_FOUND,          "File not found" },
  { XB_FILE_TYPE_NOT_SUPPORTED, "Not an Xbase type database" },
  { XB_RENAME_ERROR,            "Unable to rename file" },
  { XB_INVALID_OBJECT,          "Invalid Object" },
  { XB_NOT_OPEN,                "Database not open" },
  { XB_NOT_FOUND,               "Not Found" },
  { XB_OPEN_ERROR,              "Open Error" },
  { XB_CLOSE_ERROR,             "Close Error" },
  { XB_SEEK_ERROR,              "Seek Error" },
  { XB_READ_ERROR,              "Read Error" },
  { XB_WRITE_ERROR,             "Error writing to disk drive" },
  { XB_EOF,                     "End Of File" },
  { XB_BOF,                     "Beginning Of File" },
  { XB_INVALID_BLOCK_SIZE,      "Invalid Block Size" },
  { XB_INVALID_BLOCK_NO,        "Invalid Block Number" },
  { XB_INVALID_RECORD,          "Invalid Record Number" },
  { XB_DELETE_FAILED,           "Delete Failed" },
  { XB_INVALID_TABLE_NAME,      "Invalid Table Name" },
  { XB_EMPTY,                   "Empty Table or Index" },
  { XB_LIMIT_REACHED,           "Limit Reached" },
  { XB_BLOCKREAD_NOT_ENABLED,   "Block Read Mode is not enabled" },
  { XB_DIRECTORY_ERROR,         "Directory Read/Write error" },
  { XB_INVALID_FIELD_TYPE,      "Unknown Field Type" },
  { XB_INVALID_FIELD_NO,        "Invalid Field Number" },
  { XB_INVALID_DATA,            "Invalid Data" },
  { XB_INVALID_FIELD_NAME,      "Invalid Field Name" },
  { XB_INVALID_MEMO_FIELD,      "Not a Memo field" },
  { XB_INVALID_FIELD,           "Invalid Field" },
  { XB_INVALID_FIELD_LEN,       "Invalid Field Length" },
  { XB_INVALID_DATE,            "Invalid Date" },
  { XB_INVALID_LOCK_OPTION,     "Invalid Lock Option" },
  { XB_LOCK_FAILED,             "Lock Failed" },
  { XB_TABLE_NOT_LOCKED,        "Table Not Locked" },
  { XB_PARSE_ERROR,             "Parse Error" },
  { XB_INVALID_FUNCTION,        "Invalid or Undefined Function" },
  { XB_INVALID_PARM,            "Invalid Parm" },
  { XB_INCONSISTENT_PARM_LENS,  "Inconsistent parm lengths" },
  { XB_INCOMPATIBLE_OPERANDS,   "Incompatible operands" },
  { XB_UNBALANCED_PARENS,       "Unbalanced Parens" },
  { XB_UNBALANCED_QUOTES,       "Unbalanced Quotes" },
  { XB_INVALID_EXPRESSION,      "Invalid expression" },
  { XB_INVALID_KEYNO,           "Invalid Key Number" },
  { XB_INVALID_INDEX,           "Index File Error" },
  { XB_INVALID_TAG,             "Invalid index tag" },
  { XB_SYNTAX_ERROR,            "Invalid SQL Syntax" },
  { XB_MAX_ERROR_NO,            "End of Error List" }
};
// see also xbretcod.h

xbInt16  xbSsv::iEndianType             = 0;
xbString xbSsv::sDefaultDateFormat      = "MM/DD/YY";

xbInt16  xbSsv::iDefaultFileVersion     = 4;
xbString xbSsv::sNullString             = "";
xbBool   xbSsv::bDefaultAutoCommit      = xbTrue;

xbString xbSsv::sDataDirectory          = PROJECT_DATA_DIR;
xbString xbSsv::sTempDirectory          = PROJECT_TEMP_DIR;

#ifdef XB_LOGGING_SUPPORT
xbString xbSsv::sLogDirectory           = PROJECT_LOG_DIR;
xbString xbSsv::sLogFileName            = PROJECT_DFLT_LOGFILE;
#endif  // XB_LOGGING_SUPPORT

#ifdef XB_LOCKING_SUPPORT
xbInt32  xbSsv::lDefaultLockWait        = 100;
xbInt16  xbSsv::iDefaultLockRetries     = 3;
xbBool   xbSsv::bDefaultAutoLock        = xbTrue;
xbInt16  xbSsv::iDefaultLockFlavor      = 1;
xbBool   xbSsv::bMultiUser              = xbTrue;
#else
xbBool   xbSsv::bMultiUser              = xbFalse;
#endif  // XB_LOCKING_SUPPORT

#if defined (XB_NDX_SUPPORT) || defined (XB_MDX_SUPPORT)
xbInt16  xbSsv::iUniqueKeyOpt = XB_HALT_ON_DUPKEY;
  // is one of XB_HALT_ON_DUPKEY || XB_EMULATE_DBASE
#endif  // defined (XB_NDX_SUPPORT) || defined (XB_MDX_SUPPORT)


#ifdef XB_MDX_SUPPORT
xbInt16  xbSsv::iCreateMdxBlockSize     = 1024;   // 1024 is DBase 7 default size for MDX index blocks
#endif  // XB_MDX_SUPPORT

#ifdef XB_BLOCKREAD_SUPPORT
xbUInt32 xbSsv::ulDefaultBlockReadSize  = 32768;  // 32K buffer for block DBF datafile reads
#endif  // XB_BLOCKREAD_SUPPORT

/*************************************************************************/
//! @brief Class Constructor.
xbSsv::xbSsv(){}
/*************************************************************************/
void xbSsv::BitDump( unsigned char c ) const {
  for( int i = 7; i >= 0; i-- )
    std::cout << (BitSet( c, i ) ? 1 : 0);
}
void xbSsv::BitDump( char c ) const {
  BitDump( (unsigned char) c );
}
/*************************************************************************/
//! @brief Check a bit in a one byte field and see if it is set.
/*!
    \param c One byte char field to examine.
    \param iBitNo which bit to examine.
    \returns xbTrue Bit is set<br>
             xbFalse Bit is not set
*/
xbBool xbSsv::BitSet( unsigned char c, xbInt16 iBitNo ) const {
  return c & 1 << iBitNo;
}
/*************************************************************************/
//! @brief Display error message on console for a given error number.
/*!
   \param iErrorCode Error number to reference
*/
void xbSsv::DisplayError( xbInt16 iErrorCode ) const {
  std::cout << (const char *) GetErrorMessage( iErrorCode ) << std::endl;
}
/*************************************************************************/
//! @brief Get the default auto commit setting.
/*!

   When auto commit is enabled, the library will automatically post any updates 
   when moving off an updated record or closing files. 
   If auto commit is disabled, the application program will need to explicitly
   update the tables using  using dbf->Put() and dbf->AppendRecord().

   \returns xbTrue if auto commit is turned on<br>
            xbFalse is auto commit is turned off
*/

xbBool xbSsv::GetDefaultAutoCommit() const {
  return bDefaultAutoCommit;
}
/*************************************************************************/
//! @brief Get the current data directory.
/*!
   \returns xbString containing the current data directory
            where the database files are stored.
*/

xbString &xbSsv::GetDataDirectory() const {
  return sDataDirectory;
}
/*************************************************************************/
//! @brief Get the default date format.
/*!
   \returns xbString containing the default date format. 
*/

xbString & xbSsv::GetDefaultDateFormat() const {
  return sDefaultDateFormat;
}
/*************************************************************************/
//! @brief Get the Endian type.
/*!
   \returns B - Big endian<br>
            L - Little endian<br>
*/
xbInt16  xbSsv::GetEndianType() const {
  return iEndianType;
}
/*************************************************************************/
//! @brief Get an error message.
/*!
   \param iErrorCode  ErrorCode is the error number of description to be returned.
   \returns Returns a pointer to a string containing a text description for the error code.
*/

const char * xbSsv::GetErrorMessage( xbInt16 iErrorCode ) const{

  if( iErrorCode > 0 || iErrorCode <= XB_MAX_ERROR_NO )
    return "";

   xbBool bFound = xbFalse;
   xbInt16 iCtr = 0;
   while( !bFound ){
     if( xbErrorMessages[iCtr].iErrorNo == XB_MAX_ERROR_NO )
       return "Unknown Error";
     if( xbErrorMessages[iCtr].iErrorNo == iErrorCode )
       return xbErrorMessages[iCtr].sErrorText;
     iCtr++;
   }
   return "";
}
/*************************************************************************/
//! @brief Get home directory.
/*!
   \param sHomeDirOut - Output home directory for current user.
*/

void xbSsv::GetHomeDir( xbString &sHomeDirOut ){

  #ifdef HAVE_GETENV_S_F

    char sPath[MAX_PATH];
    size_t lSize;

    sHomeDirOut = "";
    memset( sPath, 0x00, MAX_PATH );

    getenv_s( &lSize, NULL, 0, "HOMEDRIVE" );
    if( lSize > 0 ){
      getenv_s( &lSize, sPath, lSize, "HOMEDRIVE" );
      sHomeDirOut = sPath;
      memset( sPath, 0x00, MAX_PATH );
    }

    getenv_s( &lSize, NULL, 0, "HOMEPATH" );
    if( lSize > 0 ){
      getenv_s( &lSize, sPath, lSize, "HOMEPATH" );
      sHomeDirOut += sPath;
    }
    if( sHomeDirOut == "" )
      sHomeDirOut = "C:\xbase64";

  #elif defined(WIN32)
    sHomeDirOut.Sprintf( "%s%s", getenv( "HOMEDRIVE" ), getenv( "HOMEPATH" ));

  #else
    sHomeDirOut.Sprintf( "%s", getenv( "HOME" ));
    sHomeDirOut.Trim();
    if( sHomeDirOut == "" )
      sHomeDirOut.Sprintf( "%s", getpwuid( getuid())->pw_dir );
  #endif

  sHomeDirOut.Trim();
}


/*************************************************************************/
//! @brief Set the data directory.
/*!
   \param sDataDirectory Set the data directory.
*/

void xbSsv::SetDataDirectory( const xbString &sDataDirectory ){
  this->sDataDirectory = sDataDirectory;

  #ifdef WIN32
    this->sDataDirectory.SwapChars( '/', '\\' );
  #else
    this->sDataDirectory.SwapChars( '\\', '/' );
  #endif

}

/*************************************************************************/
//! @brief Set the default date format.
/*!
   \param sDefaultDateFormat Set the default date format.
*/

void xbSsv::SetDefaultDateFormat( const xbString &sDefaultDateFormat ) {
  this->sDefaultDateFormat = sDefaultDateFormat;
}

/*************************************************************************/
//! @brief Set the default auto commit.
/*!

   Disabling auto commit requires the application execute explicit updates 
   using dbf->Put() and dbf->AppendRecord().  With auto commit on, the library 
   posts updates automatically when moving off the current record or closing 
   a file.

   \param bDefaultAutoCommit xbTrue - Enable default auto commit.<br>
                            xbFalse - Disable default auto commit.<br> 
*/

void xbSsv::SetDefaultAutoCommit( xbBool bDefaultAutoCommit ) {
  this->bDefaultAutoCommit = bDefaultAutoCommit;
}
/*************************************************************************/
//! @brief Set the endian type
/*!
    This routine determines the Endian-ness at run time instead of
    compile time as some processers (ie; Sparc,ARM) can be switched either way.
    This routine is called automatically by the library at startup and does not
    need to be called in an application program.

*/

void xbSsv::SetEndianType() {
  xbInt16 e = 1;
  iEndianType = *(char *) &e;
  if( iEndianType )
    iEndianType = 'L';
  else
    iEndianType = 'B';
  return;
}

/*************************************************************************/
//! @brief Set the temp directory.
/*!
   \param sTempDirectory Set the data direcroty.
*/

void xbSsv::SetTempDirectory( const xbString &sTempDirectory ){
  this->sTempDirectory = sTempDirectory;

  #ifdef WIN32
    this->sTempDirectory.SwapChars( '/', '\\' );
  #else
    this->sTempDirectory.SwapChars( '\\', '/' );
  #endif

}

/*************************************************************************/
//! @brief Get the OS dependent path separator.
/*!
   \returns Returns '\' for windows environment, otherwise returns '/'.
*/

char xbSsv::GetPathSeparator() const {
  #ifdef WIN32
  return '\\';
  #else
  return '/';
  #endif
}

/*************************************************************************/
//! @brief Get the current temp directory.
/*!
   \returns xbString containing the current data directory
            where the database files are stored.
*/

xbString &xbSsv::GetTempDirectory() const {
  return sTempDirectory;
}

/*************************************************************************/
#ifdef XB_LOGGING_SUPPORT

//! @brief Get the default log file name.
/*!
   \returns Returns the log file name.
*/

xbString & xbSsv::GetLogFileName() const {
  return sLogFileName;
}

/*************************************************************************/
//! @brief Get the default log directory.
/*!
   \returns Returns the log directory.
*/


xbString & xbSsv::GetLogDirectory() const {
  return sLogDirectory;
}
/*************************************************************************/
//! @brief Set the default log directory name.
/*!
   \param sLogDirectory  Name of desired log directory.

*/

void xbSsv::SetLogDirectory( const xbString &sLogDirectoryIn ){

  this->sLogDirectory = sLogDirectoryIn;
  #ifdef WIN32
    this->sLogDirectory.SwapChars( '/', '\\' );
  #else
    this->sLogDirectory.SwapChars( '\\', '/' );
  #endif

}

//! @brief Set the log file name.
/*!
  \param sLogFileName - Log File Name.
  \return void
*/

void xbSsv::SetLogFileName( const xbString & sLogFileName ){

  this->sLogFileName = sLogFileName;
}


#else

xbString & xbSsv::GetLogFileName() const {
  return sNullString;
}


xbString & xbSsv::GetLogDirectory() const {

std::cout << "xbSsv::GetLogDirectory() returning null\n";

  return sNullString;
}

void xbSsv::SetLogDirectory( const xbString &sLogDirectory ){
  return;
}

void xbSsv::SetLogFileName( const xbString & sLogFileName ){
  return;
}

#endif

/*************************************************************************/

#ifdef XB_LOCKING_SUPPORT

//! @brief Get the default lock retries.
/*!
   This is the number of lock attempts the libary will make before returning
   failure if the file can not be locked.
   \returns Default lock retry count.
*/
xbInt16 xbSsv::GetDefaultLockRetries() const {
  return iDefaultLockRetries;
}

//! @brief Set the default lock retries.
/*!
   \param iDefaultLockRetries - Number of lock attempts before returning failure.
*/
void xbSsv::SetDefaultLockRetries( xbInt16 iDefaultLockRetries ) {
  this->iDefaultLockRetries = iDefaultLockRetries;
}

//! @brief Get the default auto lock setting.
/*!
   When auto locking is turned on, the library automatically locks and unlocks
   files and indices as needed in a multi user environment.
   \returns Number of lock attempt settings.
*/
xbBool xbSsv::GetDefaultAutoLock() const {
  return bDefaultAutoLock;
}


//! @brief Set the default auto lock setting.
/*!
   When auto locking is turned on, the library automatically locks and unlocks
   files and indices as needed in a multi user environment.  Locking is not required
   in single a single user environment.

   \param bDefaultAutoLock xbTrue - Turn autolocking on<br>
                           xbFalse - Turn autolocking off<br>
*/
void xbSsv::SetDefaultAutoLock( xbBool bDefaultAutoLock ) {
  this->bDefaultAutoLock = bDefaultAutoLock;
}

//! @brief Enable default auto locking.
/*!
   When auto locking is turned on, the library automatically locks and unlocks
   files and indices as needed in a multi user environment.
*/
void xbSsv::EnableDefaultAutoLock() {
  this->bDefaultAutoLock = xbTrue;
}

//! @brief Disable defalt auto locking.
/*!
   When auto locking is turned off, the library does not automatically lock 
   and unlock files and indices as needed in a multi user environment.
   Locking is not needed in a single user environment.

*/
void xbSsv::DisableDefaultAutoLock() {
  this->bDefaultAutoLock = xbFalse;
}
/***************************************************************************/
//! @brief Get default lock flavor
/*!
  Currently one flavor.  This routine is part of the structure to support
  future additional locking scenarios for Clipper and Foxpro.
   \returns 1
*/
xbInt16 xbSsv::GetDefaultLockFlavor() const {
  return iDefaultLockFlavor;
}

/***************************************************************************/
//! @brief Set default lock flavor
/*!
  Currently one flavor.  This routine is part of the structure to support
  future additional locking scenarios for Clipper and Foxpro.
*/
void xbSsv::SetDefaultLockFlavor( xbInt16 iDefaultLockFlavor ) {
  this->iDefaultLockFlavor = iDefaultLockFlavor;
}

/***************************************************************************/
//! @brief Set default lock wait
/*!
   \param lLockWait Set default lock wait in milliseconds.
*/
void xbSsv::SetDefaultLockWait( xbInt32 lLockWait ) {
  this->lDefaultLockWait = lLockWait;
}

/***************************************************************************/
//! @brief Get default lock wait
/*!
  \returns Lock wait time in milliseconds.
  
*/
xbInt32 xbSsv::GetDefaultLockWait() const {
  return lDefaultLockWait;
}
#endif

/***************************************************************************/
//! @brief Get the multi user setting.
/*!
  \returns xbTrue - Multi user mode turned on.<br>
           xbFalse - Multi user mode turned off.<br>
*/
xbBool xbSsv::GetMultiUser() const {
  return bMultiUser;
}

//! @brief Get the multi user setting.
/*!
  \param bMultiUser xbTrue - Turn on Multi user mode.<br>
                    xbFalse - Turn off Multi user mode.<br>
*/
void xbSsv::SetMultiUser( xbBool bMultiUser ) {
  this->bMultiUser = bMultiUser;
}



/************************************************************************/
#ifdef XB_MDX_SUPPORT
//! @brief Get the mdx file block size used when creating a memo file. 
/*!
  \returns system default setting for MDX block size.
*/
xbInt16 xbSsv::GetCreateMdxBlockSize() const {
  return iCreateMdxBlockSize;
}

/************************************************************************/
//! @brief Create mdx block size.
/*!
   This routine sets the mdx file block size at the system level. This value is 
   used when the mdx index file is initially created so if you want to change it, 
   this must be called before creating the table.

  \param iBlockSize - Block size, must be evenly divisible by 512 and <= 16384
  \returns XB_INVALID_BLOCK_SIZE<br>XB_NO_ERROR
*/

xbInt16 xbSsv::SetCreateMdxBlockSize( xbInt16 iBlockSize ){

  if( iBlockSize < 512 || iBlockSize > 16384 || iBlockSize % 512  )
    return XB_INVALID_BLOCK_SIZE;
  else
    iCreateMdxBlockSize = iBlockSize;

  return XB_NO_ERROR;
}
#endif

/************************************************************************/
#if defined (XB_NDX_SUPPORT) || defined (XB_MDX_SUPPORT)
//! @brief Get Unique Key Opt
/*!
  This routine returns the Unique Key Processing Option which is one of:
     XB_HALT_ON_DUPKEY
     XB_EMULATE_DBASE
*/

xbInt16 xbSsv::GetUniqueKeyOpt() const {
  return iUniqueKeyOpt;
}

//! @brief Set Unique Key Opt
/*! @brief Set Unique Key Opt
  This routine Sets the Unique Key Processing Option which is one of:
     XB_HALT_ON_DUPKEY
     XB_EMULATE_DBASE
*/
xbInt16 xbSsv::SetUniqueKeyOpt( xbInt16 iOpt ){
  if( iOpt == XB_HALT_ON_DUPKEY || iOpt == XB_EMULATE_DBASE ){
    iUniqueKeyOpt = iOpt;
    return XB_NO_ERROR;
  } else {
    return XB_INVALID_OPTION;
  }
}
#endif
/************************************************************************/
#ifdef XB_BLOCKREAD_SUPPORT

//! @brief Get Default Read Block Size
/*!
  This routine returns the default read block size used when allocating
  buffer space for block reads of table data.  Initial setting is 32768 bytes.
*/
xbUInt32 xbSsv::GetDefaultBlockReadSize() const {
  return ulDefaultBlockReadSize;
}


//! @brief Set Default Read Block Size
/*!
  This routine sets the default read block size used when allocating
  buffer space for block reads of table data.  Initial setting is 32768 bytes.
*/
void xbSsv::SetDefaultBlockReadSize( xbUInt32 ulDfltBlockReadSize ){
   ulDefaultBlockReadSize = ulDfltBlockReadSize;
}

#endif  // XB_BLOCKREAD_SUPPORT
/************************************************************************/




}   /* namespace */