/*  xbxbase.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023,2024 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/


#ifndef __XB_XBDBLIST_H__
#define __XB_XBDBLIST_H__

#ifdef  CMAKE_COMPILER_IS_GNUCC
#pragma interface
#endif

namespace xb{

class XBDLLEXPORT xbLog;

/************************************************************************/
/* 
    Xbase functions

   Min Parm Count is the minimum number of input parms needed by the function

   Return Type
       C        Char or string
       D        Date
       L        Logical
       N        Numeric
       1        Varies - pull return type from first sibling
       2        Varies - pull return type from second sibling
*/

#ifdef XB_FUNCTION_SUPPORT

/*
struct XBDLLEXPORT xbFunctionInfo {
  const   char * FuncName;          // function name
  char    ReturnType;               // return type of function
  xbInt16 iReturnLenCalc;           // used to calculate the function return value is
                                    // 1 = use value specified in iReturnLenVal
                                    // 2 = use length of operand specified in col 4
                                    // 3 = use valued of numeric operand specified in col 4
                                    // 4 = length of parm 1 * numeric value parm
                                    // 5 = larger length of parm 2 or length of parm 3
                                    // 6 = if two or more parms, use numeric value from second parm,
                                    //     otherwise use col4 value
  xbInt16 iReturnLenVal;            // Used in combination with iReturnLenCalc

};
*/
/*
static xbFunctionInfo FunctionList[] =
//    Func       Return  -Rtrn Len-
//    Name        Type   -Calc Val-
{
   { "ABS",       'N',      1,   4 },
   { "ALLTRIM",   'C',      2,   1 },
   { "ASC",       'N',      1,   4 },
   { "AT",        'N',      1,   4 },
   { "CDOW",      'C',      1,   9 },
   { "CHR",       'C',      1,   1 },
   { "CMONTH",    'C',      1,   9 },
   { "CTOD",      'D',      1,   8 },
   { "DATE",      'D',      1,   8 },
   { "DAY",       'N',      1,   4 },
   { "DEL",       'C',      1,   1 },
   { "DELETED",   'L',      1,   1 },
   { "DESCEND",   '1',      2,   1 },
   { "DOW",       'N',      1,   4 },
   { "DTOC",      'C',      1,   8 },
   { "DTOS",      'C',      1,   8 },
   { "EXP",       'N',      1,   4 },
   { "IIF",       'C',      5,   0 },
   { "INT",       'N',      1,   4 },
   { "ISALPHA",   'L',      1,   1 },
   { "ISLOWER",   'L',      1,   1 },
   { "ISUPPER",   'L',      1,   1 },
   { "LEFT",      'C',      3,   2 },
   { "LEN",       'N',      1,   4 },
   { "LOG",       'N',      1,   4 },
   { "LOWER",     'C',      2,   1 },
   { "LTRIM",     'C',      2,   1 },
   { "MAX",       'N',      1,   4 },
   { "MIN",       'N',      1,   4 },
   { "MONTH",     'N',      1,   4 },
   { "RECNO",     'N',      1,   4 },
   { "RECCOUNT",  'N',      1,   4 },
   { "REPLICATE", 'C',      4,   0 },
   { "RIGHT",     'C',      3,   2 },
   { "RTRIM",     'C',      2,   1 },
   { "SPACE",     'C',      3,   1 },
   { "SQRT",      'N',      1,   4 },
   { "STOD",      'D',      1,   8 },
   { "STR",       'C',      6,  10 },
   { "STRZERO",   'C',      3,   2 },
   { "SUBSTR",    'C',      3,   3 },
   { "TRIM",      'C',      2,   1 },
   { "UPPER",     'C',      2,   1 },
   { "VAL",       'N',      1,   4 },
   { "YEAR",      'N',      1,   4 },
   { 0, 0, 0, 0 },
};
*/
#endif



//! @brief xbXbase class.
/*!
  The xbXBase class is the core class that needs to be declared in every application program.
*/

class XBDLLEXPORT xbXBase : public xbTblMgr{
 public:

  //! @brief Class Constructor.
  xbXBase();

  //! @brief Class Deconstructor.
  ~xbXBase();

  //! @brief Close all tables / files.
  /*!
    This closes everything and deletes references to the associated xbDbf objects.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 CloseAllTables();

  //! @brief Create a fully qualified file name from a directory, filename and extension.
  /*!
    Given a directory, file name and file extension as inputs, create a fully qualified file name.

    @param sDirIn Directory
    @param sNameIn File Name
    @param sExtIn File Extension
    @param sFqfnOut A fully qualifed unique file name as output
    @returns XB_INVALID_PARAMETER or XB_NO_ERROR
  */
  xbInt16 CreateFqn( const xbString &sDirIn, const xbString &sNameIn, const xbString &sExtIn, xbString &sFqfnOut );

  //! @brief Display error message on console for a given error number.
  /*!
    @param iErrorCode Error number to reference
  */
  void DisplayError( xbInt16 iErrorCode ) const;

  //! @brief Disable message logging.
  void DisableMsgLogging();

  //! @brief Enable message logging.
  void EnableMsgLogging ();

  //! @brief Flush log file updates to disk.
  xbInt16 FlushLog();

  //! @brief Get the default date format.
  /*!
    @returns xbString containing the default date format. 
  */
  xbString& GetDefaultDateFormat() const;

  //! @brief Get the Endian type.
  /*!
    Endianness is the byte order used by the computer architecture.
    Big endian stores the most signficant byte first whereas little endian 
    stores the least signficant byte first.  The DBF file formats utilize
    the little endian format and the Xbase64 library automatically handles
    the endianness of the archticture the software is running on to store the
    data in little endian format.
  
    @returns B - Big endian<br>
             L - Little endian<br>
  */
  xbInt16 GetEndianType() const;

  //! @brief Get an error message.
  /*!
    @param iErrorCode  ErrorCode is the error number of description to be returned.
    @returns Returns a pointer to a string containing a text description for the error code.
  */
  const char *GetErrorMessage( xbInt16 iErrorCode ) const;

  //! @brief Get the OS dependent path separator.
  /*!
    @returns Returns '\' for windows environment, otherwise returns '/'.
  */
  char GetPathSeparator() const;

  //! @brief Set the data directory.
  /*!
    @param sDataDirectory Set the data directory.
  */
  void SetDataDirectory( const xbString &sDataDirectory );

  //! @brief Set the default date format.
  /*!
    @param sDefaultDateFormat Set the default date format.
  */
  void SetDefaultDateFormat( const xbString &sDefaultDateFormat );

  //! @brief Set the temp directory.
  /*!
    @param sTempDirectory Set the data direcroty.
  */
  void SetTempDirectory( const xbString &sTempDirectory );

  //! @brief Get the current data directory.
  /*!
    @returns xbString containing the current data directory
             where the database files are stored.
  */
  xbString& GetDataDirectory() const;

  //! @brief Get the current temp directory.
  /*!
    @returns xbString containing the current data directory
             where temporary files are stored.
  */
  xbString& GetTempDirectory() const;

  //! @brief Get home directory.
  /*!
    @param sHomeDirOut - Output home directory for current user.
  */
  void GetHomeDir( xbString &sHomeDirOut );

  //! @brief Get the default auto commit setting.
  /*!
    When auto commit is enabled, the library will automatically post any updates 
    when moving off an updated record or closing files. 
    If auto commit is disabled, the application program will need to explicitly
    update the tables using  using dbf->Put() and dbf->AppendRecord().

    @returns xbTrue if auto commit is turned on<br>
             xbFalse is auto commit is turned off
  */
  xbBool GetDefaultAutoCommit() const;


  //! @brief Set the default auto commit.
  /*!

    With auto commit on, the library posts updates automatically when moving 
    off the current record or closing file.

    Disabling auto commit requires the application execute explicit updates 
    using dbf->Put() and dbf->AppendRecord().

    @param bDefaultAutoCommit xbTrue - Enable default auto commit.<br>
                              xbFalse - Disable default auto commit.<br> 
  */
  void SetDefaultAutoCommit( xbBool bDefaultAutoCommit );


  //! @brief Get the default log directory.
  /*!
    @returns Returns the log directory.
  */
  xbString& GetLogDirectory() const;

  //! @brief Get log file name.
  /*!
    @returns Returns the log file name.
  */
  xbString& GetLogFileName() const;


  //! @brief Set the default log directory name.
  /*!
    @param sLogDirectory  Name of desired log directory.
  */
  void SetLogDirectory( const xbString &sLogDirectory );

  //! @brief Set the log file name.
  /*!
    @param sLogFileName - Log File Name.
  */
  void SetLogFileName( const xbString &sLogFileName );

  //! @brief Get the multi user setting.
  /*!

    If XB_LOCKING_SUPPORT is compiled into the library, this is on by default but can be updated and turned off.<br>
    If XB_LOCKING_SUPPORT is not compiled into the library, this is always off and can't be turned on.

    @returns xbTrue or xbOn - Multi user mode turned on.<br>
             xbFalse or xbOff - Multi user mode turned off.<br>
  */
  xbBool GetMultiUser() const;

  //! @brief Set the default multi user setting.
  /*!
    This is the system level setting. Locking at the table level can also be set which overrides the 
    system level setting.  See xbDbf::SetMultiUser().

    If XB_LOCKING_SUPPORT is compiled into the library, this is on by default but can be updated and turned off.<br>
    If XB_LOCKING_SUPPORT is not compiled into the library, this is always off and can't be turned on.

    All tables must be closed to change this setting.

    @param bMultiUser xbTrue or xbOn - Turn on Multi user mode.<br>
                      xbFalse or xbOff - Turn off Multi user mode.<br>
    @returns XB_INVALID_OPTION<br>
             XB_NO_ERROR<br>
             XB_NOT_CLOSED - There are open tables.  Close all tables to change the setting.
  */
  xbInt16 SetMultiUser( xbBool bMultiUser );

  //! @brief Get fully qualified log file name.
  /*!
    @returns Returns the fully qualified log file name.
  */
  const xbString &GetLogFqFileName() const;

  //! @brief Get the logging status.
  /*!
    @returns xbTrue - Logging enabled.<br>xbFalse - Logging disables.
  */
  xbBool GetLogStatus() const;



  //! Open DBF file using highest version class available.
  /*!
    This routine opens the highest available version of the dbf file.
    Defaults to XB_READ_WRITE and XB_MULTI_USER mode.
    @param sTableName - Table name to open.
    @param sAlias - Optional alias name.
    @param dbf Output - pointer to dbf file or null if error.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 Open( const xbString &sTableName, const xbString &sAlias, xbDbf **dbf );

  //! @brief Open DBF file using highest version class available.
  /*!
    This routine can open various versions of the dbf file dependent on the iVersion field

    @param sTableName - Table name to open.
    @param sAlias - Optional alias name.
    @param iOpenMode - XB_READ_WRITE or XB_READ
    @param iShareMode - XB_SINGLE_USER or XB_MULTI_USER
    @param iVersion 0 - Highest available<br>
                    4 - Version four dbf<br>
                    3 - Version three dbf<br>
    @param iRc - Return code from open request
    @returns param dbf - Output pointer to dbf file opened or null if error
  */
  xbDbf * Open( const xbString &sTableName, const xbString &sAlias, xbInt16 iOpenMode, xbInt16 iShareMode, xbInt16 iVersion, xbInt16 &iRc );


  //! @brief Open DBF file using highest version class available.
  /*!
    This routine opens the highest available version of the dbf file.
    Defaults to XB_READ_WRITE and XB_MULTI_USER mode.
    @param sTableName - Table name to open.
    @param iRc - Return code from open request
    @returns param dbf - Output pointer to dbf file opened or null if error
  */
  xbDbf * Open( const xbString &sTableName, xbInt16 &iRc );


  //! @brief Set the logfile size.
  /*!
    When the logfile grows to this size, it will close the current logfile
    and roll to a new logfile.

    @param lSize - Log File Size.
  */
  void SetLogSize( size_t lSize );

  //! @brief Get the logfile size.
  /*!
   Logfile size is the setting at which the logfile will roll over and start a new 
   logfile.
   @returns log file size
  */
  size_t GetLogSize() const;

  //! @brief Write message to logfile.
  /*!
    @param sLogMessage - Message to write.
    @param iOutputOpt 0 = stdout<br>
                      1 = Syslog<br>
                      2 = Both<br>
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 WriteLogMessage( const xbString &sLogMessage, xbInt16 iOutputOpt = 0 );

  //! @brief Write message to logfile.
  /*!
    @param lCnt - Number of bytes to write.
    @param p - Pointer to bytes to write to log file.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 WriteLogBytes( xbUInt32 lCnt, const char *p );

  //! @brief Cross platform sleep function.
  /*!
    @param lMillisecs Milliseconds to sleep.
  */
  void xbSleep( xbInt32 lMillisecs );

  //! @brief Parse commmand line options for a given parm request
  /*!
    @param lArgc Value passed from main( argc, argv[] )
    @param sArgv Valued passed from main
    @param sOptRqst Option to search for in the arguments list
    @param sParmOut String token immediately to the right of the the option request, if found
    @returns 0 - paramater request not found<br> 1 - Parameter found
  */
  xbInt16 GetCmdLineOpt( xbInt32 lArgc, char **sArgv, const char *sOptRqst, xbString &sParmOut );

  //! @brief Parse commmand line options for a given parm request
  /*!
    @param lArgc Value passed from main( argc, argv[] )
    @param sArgv Valued passed from main
    @param sOptRqst Option to search for in the arguments list
    @param sParmOut String token immediately to the right of the the option request, if found
    @returns 0 - paramater request not found<br> 1 - Parameter found
  */
  xbInt16 GetCmdLineOpt( xbInt32 lArgc, char **sArgv, xbString &sOptRqst, xbString &sParmOut );

  #ifdef XB_MDX_SUPPORT
  //! @brief Get the mdx file block size used when creating a memo file. 
  /*!
    @returns System default setting for MDX block size.
  */
  xbInt16 GetCreateMdxBlockSize() const;

  //! @brief Create mdx block size.
  /*!
    This routine sets the mdx file block size at the system level. This value is 
    used when the mdx index file is initially created so if you want to change it, 
    this must be called before creating the table.

    @param ulBlockSize - Block size, must be evenly divisible by 512 and <= 16384
    @returns XB_INVALID_BLOCK_SIZE<br>XB_NO_ERROR
  */
  xbInt16 SetCreateMdxBlockSize( xbInt16 ulBlockSize );
  #endif  // XB_MDX_SUPPORT


  #if defined (XB_NDX_SUPPORT) || defined (XB_MDX_SUPPORT)

  /*!
     @brief Get the default index tag mode

     XB_IX_DBASE_MODE - DBASE Compatibility mode<br>
       1)  For index tags defined as unique, dBASE allows mutliple records in a table all having the same key, 
           but only the first entry in the table with the unique key will be referenced in the unique index.<br>
       2)  Deleted DBF records are not deleted from the associated tag file.<br>
       3)  This is the system default.<br>

     XB_IX_XBASE_MODE - XBase mode<br>
       1)  For index tags defined as unique,  the library will throw an error when an attempt is made to 
           add/update a record with a duplicate key.<br>
       2)  Deleted DBF records are deleted from the associated index tag.<br>

     @note When switching between modes, reindex any affected index tags.
  */
  xbInt16 GetDefaultIxTagMode() const;

  /*!
     @brief Set the default index tag mode

     XB_IX_DBASE_MODE - DBASE Compatibility mode<br>
       1)  For index tags defined as unique, dBASE allows mutliple records in a table all having the same key, 
           but only the first entry in the table with the unique key will be referenced in the unique index.<br>
       2)  Deleted DBF records are not deleted from the associated tag file.<br>
       3)  This is the system default.<br>

     XB_IX_XBASE_MODE - XBase mode<br>
       1)  For index tags defined as unique,  the library will throw an error when an attempt is made to 
           add/update a record with a duplicate key.<br>
       2)  Deleted DBF records are deleted from the associated index tag.<br>

     @note When switching between modes, reindex any affected index tags.
  */
  xbInt16 SetDefaultIxTagMode( xbInt16 iIndexMode = XB_IX_DBASE_MODE );
  #endif  // (XB_NDX_SUPPORT) || defined (XB_MDX_SUPPORT)


  #ifdef XB_DBF5_SUPPORT
  //! @brief Get the default time format.  Available if XB_DBF5_SUPPORT is on.
  /*!
    @returns xbString containing the default time format.
  */
  xbString& GetDefaultTimeFormat() const;

  //! @brief Set the default time format.  Available if XB_DBF5_SUPPORT is on.
  /*!
    @param sDefaultTimeFormat Set the default time format.
  */
  void SetDefaultTimeFormat( const xbString &sDefaultTimeFormat );
  #endif // XB_DBF5_SUPPORT


  #ifdef XB_BLOCKREAD_SUPPORT
  //! @brief Get Default Read Block Size
  /*!
    This routine returns the default read block size used when allocating
    buffer space for block reads of table data.  Initial setting is 32768 bytes.
  */
  xbUInt32 GetDefaultBlockReadSize() const;

  //! @brief Set Default Read Block Size
  /*!
    This routine sets the default read block size used when allocating
    buffer space for block reads of table data.  Initial setting is 32768 bytes.
  */
  void SetDefaultBlockReadSize( xbUInt32 ulDfltBlockReadSize );
  #endif  // XB_BLOCKREAD_SUPPORT


  #ifdef XB_LOCKING_SUPPORT


  //! @brief Disable defalt auto locking.
  /*!
    When auto locking is turned off, the library does not automatically lock 
    and unlock files and indices as needed in a multi user environment.

    Locking is optional in a single user environment.<br>
    Locking is required in a multi user envrionment.
  */
  void       DisableDefaultAutoLock    ();

  //! @brief Enable default auto locking.
  /*!
    When auto locking is turned on, the library automatically locks and unlocks
    files and indices as needed in a multi user environment.
  */
  void       EnableDefaultAutoLock     ();

  //! @brief Get the default auto lock setting.
  /*!
    When auto locking is turned on, the library automatically locks and unlocks
    files and indices as needed in a multi user environment.
    @returns Number of lock attempt settings.
  */
  xbBool     GetDefaultAutoLock        () const;

  //! @brief Get default lock flavor
  /*!
    Currently one flavor.  This routine is part of the structure to support
    future additional locking scenarios for Clipper and Foxpro.
    @returns 1
  */
  xbInt16    GetDefaultLockFlavor      () const;

  //! @brief Get the default lock retries.
  /*!
    This is the number of lock attempts the libary will make before returning
    failure if the lock request can not be completed.
    @returns Default lock retry count.
  */
  xbInt16    GetDefaultLockRetries     () const;

  //! @brief Get default lock wait.
  /*!
    The lock wait time is the time the library waits between lock requests.
    @returns Lock wait time in milliseconds.
  */
  xbInt32    GetDefaultLockWait        () const;

  //! @brief Set the default auto lock setting.
  /*!
    When auto locking is turned on, the library automatically locks and unlocks
    files and indices as needed in a multi user environment.  Locking is not required
    in single a single user environment.

    @param bAutoLock xbTrue - Turn autolocking on<br>
                     xbFalse - Turn autolocking off<br>
  */
  void       SetDefaultAutoLock        ( xbBool bAutoLock );

  //! @brief Set default lock flavor
  /*!
    Currently one flavor.  This routine is part of the structure to support
    future additional locking scenarios for Clipper and Foxpro.
  */
  void       SetDefaultLockFlavor      ( xbInt16 iLockFlavor );

  //! @brief Set the default lock retries.
  /*!
    @param iRetryCount - Number of lock attempts before returning failure.
  */
  void       SetDefaultLockRetries     ( xbInt16 iRetryCount );

  //! @brief Set default lock wait
  /*!
    @param lRetryWait Set default lock wait in milliseconds.
  */
  void       SetDefaultLockWait        ( xbInt32 lRetryWait );
  #endif

  /* xbase functions */
  #ifdef XB_FUNCTION_SUPPORT
  //! @brief Calculate absolute value of a numeric expression.
  /*!
    Expression function ABS().
    @param dIn Input - Numeric expression.
    @param dOut Output - Absolute value.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 ABS( xbDouble dIn, xbDouble &dOut );

  //! @brief Trim leading and trailing white space from a string.
  /*!
    Expression function ALLTRIM().
    @param sIn Input - Input string to trim.
    @param sOut Output - Trimmed string.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 ALLTRIM( const xbString &sIn, xbString &sOut );

  //! @brief Return ASCII code for the first character in a string.
  /*!
    Expression function ASC().
    @param sIn Input - Input character string.
    @param dAscOut Output - Ascii code of first character in string.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 ASC( const xbString &sIn, xbDouble &dAscOut );

  //! @brief Return number indicating starting position of string within a string.
  /*!
    Expression function AT().
    @param sSrchFor Input - Input string to search for.
    @param sBase Input - Input string to search.
    @param dPos Output - Position of string s1 within s2.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 AT( const xbString &sSrchFor, const xbString &sBase, xbDouble &dPos );


  //! @brief Return character weekday name for date.
  /*!
    Expression function CDOW().
    @param dInDate Input - Input date.
    @param sOutDow Output - Character day of week.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 CDOW( xbDate &dInDate, xbString &sOutDow );

  //! @brief Convert numeric expression to a character.
  /*!
    Expression function CHR().
    @param dAsciCd Input - Numeric expression.
    @param sOut Output - Character result.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 CHR( xbDouble dAsciCd, xbString &sOut );

  //! @brief Return character month  name for date.
  /*!
    Expression function CMONTH().
    @param dInDate Input - Input date.
    @param sOutMonth Output - Character month.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 CMONTH( xbDate &dInDate, xbString &sOutMonth );

  //! @brief Return date from character input date.
  /*!
    Expression function CTOD().
    @param sInDate Input - Input date in MM/DD/YY format.
    @param dOutDate Output - Output date.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 CTOD( const xbString &sInDate, xbDate &dOutDate );

  //! @brief Return system date.
  /*!
    Expression function DATE().
    @param dOutDate Output - Output date.
    @returns XB_NO_ERROR
  */
  xbInt16 DATE( xbDate &dOutDate );

  //! @brief Return the day of the month from a date.
  /*!
    Expression function DAY().
    @param dInDate Input - Input date.
    @param dOutDay Output - Output day of month.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 DAY( const xbDate &dInDate, xbDouble &dOutDay );

  //! @brief Return record deletion status for current record.
  /*!
    Expression function DEL().
    @param dbf Input - Table to check current record deletion status.
    @param iRecBufSw Input - Which buffer.<br>
                         0 - Current record buffer<br> 
                         1 - Original record buffer
    @param sOut Output - '* if record is deleted, otherise space.
    @returns XB_NO_ERROR<br>XB_PARSE_ERROR.
  */
  xbInt16 DEL( xbDbf * dbf, xbString &sOut, xbInt16 iRecBufSw = 0 );

  //! @brief Return record deletion status for current record.
  /*!
    Expression function DELETED().
    @param dbf Input - Table to check current record deletion status for.
    @param iRecBufSw Input - Which buffer.<br>
                         0 - Current record buffer<br>
                         1 - Original record buffer
    @param bOut Output - xbTrue if record is deleted.<br>
                         xbFalse if record is not deleted.
    @returns XB_NO_ERROR<br>XB_PARSE_ERROR.
  */
  xbInt16 DELETED( xbDbf * dbf, xbBool &bOut, xbInt16 iRecBufSw = 0 );

  //! @brief Clipper DESCEND function.
  /*!
    Expression function DESCEND().
    @param dtInDate Input - Input date.
    @param dtOutDate Output - Output date.
    @returns XB_NO_ERROR.
  */
  xbInt16 DESCEND( const xbDate &dtInDate, xbDate &dtOutDate );

  //! @brief Clipper DESCEND function.
  /*!
    Expression function DESCEND().
    @param sIn Input - Input string.
    @param sOut Output - Output string.
    @returns XB_NO_ERROR.
  */
  xbInt16 DESCEND( const xbString &sIn, xbString &sOut );

  //! @brief Clipper DESCEND function.
  /*!
    Expression function DESCEND().
    @param dIn Input - Input number.
    @param dOut Output - Output number.
    @returns XB_NO_ERROR.
  */
  xbInt16 DESCEND( const xbDouble dIn, xbDouble &dOut );

  //! @brief Return number of day of week.
  /*!
    Expression function DOW().
    @param dtInDate Input - Input date.
    @param dDowOut Output - Output day of week.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 DOW( const xbDate &dtInDate, xbDouble &dDowOut );


  //! @brief Return character date from input date.
  /*!
    Expression function DTOC().
    @param dInDate Input - Input date.
    @param sOutFmtDate Output - Output date.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 DTOC( xbDate &dInDate, xbString &sOutFmtDate );

  //! @brief Return char CCYYMMDD date from input date.
  /*!
    Expression function DTOS().
    @param dtInDate Input - Input date.
    @param sOutFmtDate Output - Output date.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 DTOS( xbDate &dtInDate, xbString &sOutFmtDate );

  //! @brief Return exponent value.
  /*!
    Expression function EXP().
    This function returns e**x where e is approximately 2.71828 and x is dIn.

    @param dIn Input - exp value.
    @param dOut Output - value.
    @returns XB_NO_ERROR
  */
  xbInt16 EXP( xbDouble dIn, xbDouble &dOut );

  //! @brief Immediate if.
  /*!
    Expression function IIF().
    @param bResult Input - boolean expression.
    @param sTrueResult Input - value if boolean expression is true.
    @param sFalseResult Input - value if boolean expression is false.
    @param sResult Output - sTrueResult or sFalseResult depending on bResultvalue.
    @returns XB_NO_ERROR
  */
  xbInt16 IIF( xbBool bResult, const xbString &sTrueResult, const xbString &sFalseResult, xbString &sResult );

  //! @brief Convert number to integer, truncate any decimals.
  /*!
    Expression function INT().
    @param dIn Input - Input number.
    @param dOut Output - integer.
    @returns XB_NO_ERROR
  */
  xbInt16 INT( xbDouble dIn, xbDouble &dOut );

  //! @brief Check if string begins with alpha character.
  /*!
    Expression function ISALPHA().
    @param sIn Input - Input string.
    @param bResult Output - xbTrue if string begins with alpha character.<br>xbFalse if string does not begin with alpha character.
    @returns XB_NO_ERROR
  */
  xbInt16 ISALPHA( const xbString &sIn, xbBool &bResult );

  //! @brief Check if string begins with lower case alpha character.
  /*!
    Expression function ISLOWER().
    @param sIn Input - Input string.
    @param bResult Output - xbTrue if string begins with lower case alpha character.<br>
                            xbFalse if string does not begin with lower case alpha character.
    @returns XB_NO_ERROR
  */
  xbInt16 ISLOWER( const xbString &sIn, xbBool &bResult );

  //! @brief Check if string begins with upper case alpha character.
  /*!
    Expression function ISUPPER().
    @param sIn Input - Input string.
    @param bResult Output - xbTrue if string begins with upper case alpha character.<br>
                            xbFalse if string does not begin with upper case alpha character.
    @returns XB_NO_ERROR
  */
  xbInt16 ISUPPER( const xbString &sIn, xbBool &bResult );

  //! @brief Return left characters from string.
  /*!
    Expression function LEFT().
    @param sIn Input - string.
    @param ulCharCnt Input - number of characters to extract from string.
    @param sOut Output - resultant string.
    @returns XB_NO_ERROR
  */
  xbInt16 LEFT( const xbString &sIn, xbUInt32 ulCharCnt, xbString &sOut );

  //! @brief Return length of string.
  /*!
    Expression function LEN().
    @param sIn Input - Input string.
    @param dLen Output - string length.
    @returns XB_NO_ERROR
  */
  xbInt16 LEN( const xbString &sIn, xbDouble &dLen );

  //! @brief Calculate logarithm.
  /*!
    Expression function LOG().
    @param dIn Input - numeric expression.
    @param dOut Output - numeric log value.
    @returns XB_NO_ERROR
  */
  xbInt16 LOG( xbDouble dIn, xbDouble &dOut );

  //! @brief Convert upper case to lower case.
  /*!
    Expression function LOWER().
    @param sIn Input - string.
    @param sOut Output - string result.
    @returns XB_NO_ERROR
  */
  xbInt16 LOWER( const xbString &sIn, xbString &sOut );

  //! @brief Trim white space from left side of string.
  /*!
    Expression function LTRIM().
    @param sIn Input - string.
    @param sOut Output - string result.
    @returns XB_NO_ERROR
  */
  xbInt16 LTRIM( const xbString &sIn, xbString & sOut );

  //! @brief Return higher of two values.
  /*!
    Expression function MAX().
    @param dIn1 Input - Numeric value 1.
    @param dIn2 Input - Numeric value 2.
    @param dOut Output - Higher of d1 or d2.
    @returns XB_NO_ERROR
  */
  xbInt16 MAX( xbDouble dIn1, xbDouble dIn2, xbDouble &dOut );

  //! @brief Return lessor of two values.
  /*!
    Expression function MIN().
    @param dIn1 Input - Numeric value 1.
    @param dIn2 Input - Numeric value 2.
    @param dOut Output - Lessor of d1 or d2.
    @returns XB_NO_ERROR
  */
  xbInt16 MIN( xbDouble dIn1, xbDouble dIn2, xbDouble &dOut );

  //! @brief Return number of month for a given date.
  /*!
    Expression function MONTH().
    @param dtInDate Input date.
    @param dOutMonth - Month number.
    @returns XB_NO_ERROR.<br>XB_PARSE_ERROR.
  */
  xbInt16 MONTH( xbDate &dtInDate, xbDouble &dOutMonth );

  //! @brief Return number of records in a given table.
  /*!
    Expression function RECCOUNT().
    @param dbf - Table.
    @param dRecOut - Number of records.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 RECCOUNT( xbDbf * dbf, xbDouble &dRecOut );

  //! @brief Return current record number for a given table.
  /*!
    Expression function RECNO().
    @param dbf  - Table.
    @param dRecOut - Record number.
    @returns XB_NO_ERROR.<br>XB_PARSE_ERROR.
  */
  xbInt16 RECNO( xbDbf * dbf, xbDouble &dRecOut );

  //! @brief Repeat character expression N times.
  /*!
    Expression function REPLICATE().
    @param sIn Inout - String to replicate.
    @param ulRepCnt Input - Number of times to repeat.
    @param sOut Output - String result.
    @returns XB_NO_ERROR.<br>XB_PARSE_ERROR.
  */
  xbInt16 REPLICATE( const xbString &sIn, xbUInt32 ulRepCnt, xbString &sOut );

  //! @brief Return right characters from string.
  /*!
    Expression function RIGHT().
    @param sIn Input - string.
    @param ulCharCnt Input - number of characters to extract from string.
    @param sOut Output - resultant string.
    @returns XB_NO_ERROR
  */
  xbInt16 RIGHT( const xbString &sIn, xbUInt32 ulCharCnt, xbString &sOut );

  //! @brief Trim white space from right side of string.
  /*!
    Expression function RTRIM().
    @param sIn Input - string.
    @param sOut Output - string result.
    @returns XB_NO_ERROR
  */
  xbInt16 RTRIM( const xbString &sIn, xbString &sOut );

  //! @brief Generate a string of N spaces.
  /*!
    Expression function SPACE().
    @param lCnt Input - Number of spaces.
    @param sOut Output - Output string consisting of specified number of spaces.
    @returns XB_NO_ERROR.
  */
  xbInt16 SPACE( xbInt32 lCnt, xbString &sOut );

  //! @brief Calculate a square root.
  /*!
    Expression function SQRT().
    @param dBase Input - Base number.
    @param dSqrRoot Output - Square root.
    @returns XB_NO_ERROR.
  */
  xbInt16 SQRT( xbDouble dBase, xbDouble &dSqrRoot );

  //! @brief Converts a valid 8 byte (CCYYMMDD) input date into a date class.
  /*!
    Expression function STOD().
    @param sInDate Input - Input date.
    @param dtOutDate Output - Output date.
    @returns XB_NO_ERROR.<br>XB_INVALID_DATE.
  */
  xbInt16 STOD( const xbString &sInDate, xbDate &dtOutDate );

  //! @brief Convert number to a character string.
  /*!
    Expression function STR().
    @param dIn Input - Number.
    @param sOut Output - String.
    @returns XB_NO_ERROR.
  */
  xbInt16 STR( xbDouble dIn, xbString &sOut );

  //! @brief Convert number to a character string.
  /*!
    Expression function STR().
    @param dIn Input - Number.
    @param ulLen Input - Length.
    @param sOut Output - String.
    @returns XB_NO_ERROR.
  */
  xbInt16 STR( xbDouble dIn, xbUInt32 ulLen, xbString &sOut );

  //! @brief Convert number to a character string.
  /*!
    Expression function STR().
    @param dIn Input - Number.
    @param ulLen Input - Length.
    @param ulDec Input - Number of decimals.
    @param sOut Output - String.
    @returns XB_NO_ERROR.
  */
  xbInt16 STR( xbDouble dIn, xbUInt32 ulLen, xbUInt32 ulDec, xbString &sOut );

  //! @brief Convert number to a character string.
  /*!
    Expression function STR().
    @param dIn Input - Number.
    @param ulLen Input - Length.
    @param ulDec Input - Number of decimals.
    @param sPadChar Input - Left pad character, typically zero or space.
    @param sOut Output - String.
    @returns XB_NO_ERROR.
  */
  xbInt16 STR( xbDouble dIn, xbUInt32 ulLen, xbUInt32 ulDec, xbString &sPadChar, xbString &sOut );

  //! @brief Convert number to a character string with leading zeroes.
  /*!
    Expression function STRZERO().
    @param dIn Input - Number.
    @param ulLen Input - Length.
    @param ulDec Input - Number of decimals.
    @param sOut Output - String.
    @returns XB_NO_ERROR.
  */
  xbInt16 STRZERO( xbDouble dIn, xbUInt32 ulLen, xbUInt32 ulDec, xbString &sOut );

  //! @brief Extract a portion of a string from another string.
  /*!
    Expression function SUBSTR().
    @param sIn Input - Source string.
    @param ulStartPos Input - Starting position for string extraction.
    @param ulLen Input - Number of characters to extract.
    @param sOut Output - String.
    @returns XB_NO_ERROR.
  */
  xbInt16 SUBSTR( const xbString &sIn, xbUInt32 ulStartPos, xbUInt32 ulLen, xbString &sOut );

  //! @brief Trim white space from left and right sides of string.
  /*!
    Expression function TRIM().
    @param sIn Input - string.
    @param sOut Output string result.
    @returns XB_NO_ERROR
  */
  xbInt16 TRIM( const xbString &sIn, xbString &sOut );

  //! @brief Convert lower case to upper case.
  /*!
    Expression function UPPER().
    @param sIn Input - string.
    @param sOut Output - string result.
    @returns XB_NO_ERROR
  */
  xbInt16 UPPER( const xbString &sIn, xbString &sOut );

  //! @brief Convert numeric characters to number.
  /*!
    Expression function VAL().
    @param sIn Input - string.
    @param dOut Output - numeric result.
    @returns XB_NO_ERROR
  */
  xbInt16 VAL( const xbString &sIn, xbDouble &dOut );

  //! @brief Return year for a given date.
  /*!
    Expression function YEAR().
    @param dInDate Input date.
    @param dOutYear - Year.
    @returns XB_NO_ERROR.<br>XB_PARSE_ERROR.
  */
  xbInt16 YEAR( xbDate &dInDate, xbDouble &dOutYear );
  #endif

 protected:

  ///@cond
  friend class xbBcd;
  friend class xbExp;

  #ifdef XB_BLOCKREAD_SUPPORT
  friend class xbBlockRead;
  #endif // XB_BLOCKREAD_SUPPORT

  //! @brief Get information regarding expression functions.
  /*!
    @param sExpLine An expression beginning with function name.
    @param cReturnType Output - return type of function.
    @param iCalc Used to calculate the function return value is<br>
        1 = use value specified in lReturnLenVal<br>
        2 = use length of operand specified in col 4<br>
        3 = use valued of numeric operand specified in col 4<br>
        4 = length of parm 1 * numeric value parm<br>
        5 = larger length of parm 2 or length of parm 3<br>
        6 = if two or more parms, use numeric value from second parm, otherwise use col4 value
    @param lReturnLenVal Used in combination with iReturnLenCalc.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 GetFunctionInfo( const xbString &sExpLine, char &cReturnType, xbInt16 &iReturnLenCalc, xbInt32 &lReturnLenVal ) const;

  //! @brief Set the endian type
  /*!
    This routine determines the Endian-ness at run time instead of
    compile time as some processers (ie; Sparc,ARM) can be switched either way.
    This routine is called automatically by the library at startup and does not
    need to be called in an application program.
  */
  void    SetEndianType             ();

  //! @brief Cross memcmp function.
  /*!
    @param s1 Left operand to compare.
    @param s2 Right operand to compare.
    @param n Number of bytes to compare.
    @returns 1 s1 > s2<br>
             0 s1 == s2<br>
            -1 s1 < s2
  */
  static  xbInt16 xbMemcmp( const unsigned char *s1, const unsigned char *s2, size_t n );

  ///@endcond

 private:

  #ifdef XB_LOGGING_SUPPORT
  xbLog *xLog;
  #endif

};

}        /* namespace xb    */
#endif   /* __XB_DBLIST_H__ */