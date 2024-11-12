/*  xbdbf.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/


#ifndef __XB_XBDBF_H__
#define __XB_XBDBF_H__

//#ifdef  CMAKE_COMPILER_IS_GNUCC
//#pragma interface
//#endif

namespace xb{

/*****************************/
/* Field Types               */

#define XB_CHAR_FLD      'C'
#define XB_LOGICAL_FLD   'L'
#define XB_NUMERIC_FLD   'N'
#define XB_DATE_FLD      'D'
#define XB_MEMO_FLD      'M'
#define XB_FLOAT_FLD     'F'

/*****************************/
/* File Status Codes         */

#define XB_CLOSED  0
#define XB_OPEN    1
#define XB_UPDATED 2


/*****************************/
/* File Access Modes         */
// #define XB_SINGLE_USER  0      // file buffering on
// #define XB_MULTI_USER   1      // file buffering off

/*****************************/
/* File Lock Functions       */
#define XB_LOCK       0
#define XB_UNLOCK     1

#define LK_DBASE      1
#define LK_CLIPPER    2		// future
#define LK_FOX        3		// future
#define LK_XBASE64    9		// future

/*****************************/
/* Record retrieve options   */
#define XB_ALL_RECS     0
#define XB_ACTIVE_RECS  1
#define XB_DELETED_RECS 2


/*****************************/
/* Other defines             */

#define XB_OVERLAY     1
#define XB_DONTOVERLAY 0
#define XB_CHAREOF  '\x1A'         /* end of DBF        */
#define XB_CHARHDR  '\x0D'         /* header terminator */


//! @brief Schema used for defining tables with CreateTable methods.
/*!
  See program xb_ex_v3_create_dbf.cpp or xb_ex_v4_create_dbf.cpp 
  as examples on how to use.

\code
xbSchema MyRecord[] = {
//  FieldName,   Type,          Len, Dec
  { "FIRSTNAME", XB_CHAR_FLD,     15, 0 },
  { "LASTNAME",  XB_CHAR_FLD,     20, 0 },
  { "BIRTHDATE", XB_DATE_FLD,      8, 0 },
  { "AMOUNT",    XB_NUMERIC_FLD,   9, 2 },
  { "RETIRED?",  XB_LOGICAL_FLD,   1, 0 },
  { "ZIPCODE",   XB_NUMERIC_FLD,   5, 0 },
  { "NUMFLD1",   XB_FLOAT_FLD,    12, 2 },
  { "NUMFLD2",   XB_FLOAT_FLD,    14, 2 },
  { "MEMO1",     XB_MEMO_FLD,     10, 0 },
  { "",0,0,0 }};
\endcode
*/
struct XBDLLEXPORT xbSchema {
  char      cFieldName[11];
  char      cType;
  xbInt16   iFieldLen;      /* fields are stored as one byte on record*/
  xbInt16   iNoOfDecs;
};

///@cond DOXYOFF
struct XBDLLEXPORT xbSchemaRec {
  char     cFieldName[11];   /* ASCIIZ field name                       */
  char     cType;            /* field type                              */
  char     *pAddress;        /* pointer to field in record buffer 1     */
  unsigned char cFieldLen;   /* fields are stored as one byte on record */
  unsigned char cNoOfDecs;
  char     *pAddress2;       /* pointer to field in record buffer 2     */
  xbInt16  iLongFieldLen;    /* to handle long field lengths            */
  unsigned char cIxFlag;     /* dBASE IV Index field flag               */
};
///@endcond DOXYOFF



///@cond DOXYOFF
#ifdef XB_INDEX_SUPPORT
class XBDLLEXPORT xbIx;
// structure for file list, each open DBF file can have one or more index files
struct XBDLLEXPORT xbIxList {
  xbIxList *next;
  xbIx     *ix;
  xbString *sFmt;
};
#endif // XB_INDEX_SUPPORT
///@endcond DOXYOFF


//! @brief xbDbf is a base class for accessing and updating dbf files/tables.  Classes xbDbf3 and xbDbf4 are derived from xbDbf.
/*!
In line with relational theory, each dbf file can be considered a table.
The documentation uses the terms dbf and table interchangeably.<br>

See <a href="../include/TableBasics.html">Table Basics</a> for more information.

*/


class XBDLLEXPORT xbDbf : public xbFile {

 public:
  //! @brief Constructor
  /*!
    \param x Pointer to xbXbase
  */
  xbDbf( xbXBase *x );

  //! @brief Destructor
  virtual ~xbDbf();

  //! @brief Abort any uncommited changes from the current record buffer.
  /*!
      @returns XB_NO_ERROR if no error<br>
               XB_INVALID_NODE_LINK<br>
               XB_INVALID_BLOCK_NO<br>
               XB_INVALID_BLOCK_SIZE<br>
               XB_INVALID_OPTION<br>
               XB_NOT_OPEN<br>
               XB_READ_ERROR<br>
               XB_SEEK_ERROR<br>
               XB_WRITE_ERROR<br>
  */
  virtual xbInt16  Abort();

  //! @brief Append the current record buffer to the data file.
  /*!
    This method attempts to append the contents of the current record buffer
    to the end of the DBF file, update the file date, number of records in the file
    and updates any open indices associated with this data file.<br>

    To add a record, an application would typically blank the record buffer,
    update various fields in the record buffer, then append the record.<br>

    The append method performs the following tasks:<br>
    1)  Create new index key values<br>
    2)  Lock the table<br>
    3)  Lock append bytes<br>
    4)  Lock indices<br>
    5)  Read the dbf header<br>
    6)  Check for dup keys<br>
    7)  Calc last update date, no of recs<br>
    8)  Add keys<br>
    9)  Unlock indices<br>
    10) Update file header<br>
    11) Unlock file header<br>
    12) Append record<br>
    13) Unlock append bytes<br>

    Note: Locking memo files is not required as the memo file updates are handled outside of the append method.<br>

      @returns XB_NO_ERROR if no error<br>
               XB_INVALID_BLOCK_SIZE<br>
               XB_INVALID_FIELD<br>
               XB_INVALID_FIELD_NAME<br>
               XB_INVALID_FIELD_NO<br>
               XB_INVALID_FUNCTION<br>
               XB_INVALID_OPTION<br>
               XB_INVALID_KEYNO<br>
               XB_INVALID_LOCK_OPTION<br>
               XB_INVALID_RECORD<br>
               XB_KEY_NOT_UNIQUE<br>
               XB_LOCK_FAILED<br>
               XB_NO_MEMORY<br>
               XB_NOT_FOUND<br>
               XB_NOT_OPEN<br>
               XB_PARSE_ERROR<br>
               XB_READ_ERROR<br>
               XB_SEEK_ERROR<br>
               XB_UNBALANCED_PARENS<br>
               XB_UNBALANCED_QUOTES<br>
               XB_WRITE_ERROR<br>
  */
  virtual xbInt16 AppendRecord();


  //! @brief Blank the record buffer.
  /*!

    This method would typically be called to initialize the record buffer before
    updates are applied to append a new record.<br>

    If there are pending updates for a given record buffer and auto-update is turned
    on, the library will attempt to update the database and index files with the current
    updates.<br>

    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 BlankRecord();


  //! @brief Close DBF file/table.
  /*!
    This routine flushes any remaining updates to disk, closes the DBF file and
    any associated memo and index files.

    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 Close();

  //! @brief Commit updates to disk 
  /*!

    This routine commits any pending updates to disk.
    \returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 Commit();


  //! @brief Copy table (dbf) file structure.
  /*!

   This routine will copy the structure of a dbf file and if successful
    return a pointer to the new table in an open state.

    @param dNewTable Output - Reference to new table object.
    @param sNewTableName New table (dbf) name.
    @param sNewTableAlias Alias name of new table.
    @param iOverlay xbTrue - Overlay existing file.<br>
                    xbFalse - Don't overlay existing file.
    @param iShareMode XB_SINGLE_USER<br>
                      XB_MULTI_USER
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 CopyDbfStructure( xbDbf *dNewTable, const xbString &sNewTableName, const xbString &sNewTableAlias, xbInt16 iOverlay, xbInt16 iShareMode );

  //! @brief Create new table.
  /*!
    This routine creates a dBASE IV (tm) DBF file and possibly an associated DBT memo file.

    @param sTableName DBF table name.
    @param sAlias Table alias
    @param pSchema Pointer to schema structure with field definitions.
    @param iOverlay xbTrue - Overlay.<br> xbFalse - Don't overlay.
    @param iShareMode XB_SINGLE_USER<br>XB_MULTI_USER
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 CreateTable( const xbString &sTableName, const xbString &sAlias, xbSchema *pSchema, xbInt16 iOverlay, xbInt16 iShareMode ) = 0;

  //! @brief Set the delete byte for all records in a dbf file (table). 
  /*!
    The xbase file format contains a leading one byte character in each physical record 
    used for flagging a record as deleted.<br><br>
    When a record is flagged as deleted the first byte is set to value 0x2A.
    If the record is not deleted, the first byte is set to 0x20 (space).<br><br>
    This routine updates the first byte of all the records depending on how iOption is valued. It does not physically remove the records from the file.<br><br>
    See the Pack() routine for physical removal of deleted records.

    @param iOption 0 - Delete all records.<br>
                   1 - Un-delete all deleted records.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 DeleteAll( xbInt16 iOption );

  //! @brief Delete all records.
  /*!
    This routine flags all records in a table as deleted. See the DeleteAll() routine for
    more information.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16  DeleteAllRecords ();

  //! @brief Delete the current record.
  /*!
    This routine flags the current record for deletion if it's not already flagged.
    @returns XB_NO_ERROR<br>
             XB_INVALID_RECORD
  */
  virtual xbInt16 DeleteRecord();

  //! @brief Delete a table.
  /*!
    This routine deletes a given table, associated index files if any, the
    memo file if any and the .INF file if any.<br><br>
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 DeleteTable();

  //! @brief Dump dbf file header. 
  /*!
    This routine dumps dbf header information to the console.

    @param iOption  1 = Print header only<br>
                    2 = Field data only<br>
                    3 = Header and Field data<br> 
                    4 = Header, Field and Memo header data if applicable
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 DumpHeader( xbInt16 iOption );

  //! Dump record
  /*!
    Dump the contents of the specified record


    @param ulRecNo Record number of record to be dumped.
    @param iOutputDest 0 = logfile<br>
                       1 = stdout<br>
                       2 = Both<br>

    @param iOutputFmt  0 = with field names<br>
                       1 = 1 line per rec, no field names<br>
                       2 = 1 line per rec, first line is a list of field names.
    @param cDelim Optional one byte delimter.  Default is comma ','

    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 DumpRecord( xbUInt32 ulRecNo, xbInt16 iOutputDest = 0, xbInt16 iOutputFmt = 0, char cDelim = ',' );

  //! @brief Get auto commit setting for table.
  /*!

    @returns  -1 Use the xbase system level setting.<br>
               0 No auto update for this table, regardless of system level setting.<br>
               1 Auto update enabled for this table, regardless of system level setting


    <a href="../include/AutoCommit.html">Auto commit information</a>

  */
  virtual xbInt16 GetAutoCommit() const;


  //! @brief Get auto commit setting for table.
  /*!

    @param iOption  1 Look at both table level and system level settings and determine auto commit status for this table.<br>
                    0 Same as GetAutoCommit()

    @returns xbTrue - Auto commit on for this table<br>
             xbFalse - Auto commit is off for this table

    <a href="../include/AutoCommit.html">Auto commit information</a>

  */
  virtual xbInt16 GetAutoCommit( xbInt16 iOption ) const;


  //! @brief Return true if dbf file empty or positioned to the first record
  /*!
    @returns xbTrue - DBF file is empty or positioned on the first record<br>
             xbFalse - Not on first record
  */
  virtual xbBool GetBof();

  //! @brief Return the current record number. 
  /*!
    @returns Returns the current record number.
  */
  virtual xbUInt32 GetCurRecNo() const;

  //! @brief Return the current dbf status. 
  /*!
    @returns  0 = Closed<br>
              1 = Open<br>
              2 = Open with updates pending<br>
  */
  virtual xbInt16  GetDbfStatus     () const;

  //! @brief Return true if dbf file empty or positioned to the last record
  /*!
    @returns xbTrue - DBF file is empty or positioned on the last record<br>
             xbFalse - not at end of file
  */
  virtual xbBool   GetEof           ();

  //! @brief Return the number of fields in the table. 
  /*!
    @returns The number of fields in the table.
  */
  virtual xbInt32  GetFieldCnt      () const;

  //! @brief Retrieve the first record from the table.
  /*!
    Get the first not deleted record.  This routines skips over any deleted records.

    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16  GetFirstRecord   ();

  //! @brief Get the first record.
  /*!
    @param iOption XB_ALL_RECS - Get the first record, deleted or not.<br>
           XB_ACTIVE_RECS - Get the first active record.<br>
           XB_DELETED_RECS - Get the first deleted record.<br>
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16  GetFirstRecord   ( xbInt16 iOption );

  //! @brief GetHeaderLen for DBF file
  /*!
    Returns the length of the header portion of the DBF file
    @returns Length of header portion of DBF file
  */

  virtual xbUInt16 GetHeaderLen     () const;

  //! @brief Get the last record.
  /*!
    Get the last not deleted record.  This routines skips over any deleted records.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16  GetLastRecord    ();

  //! @brief Get the last record.
  /*!
    @param iOption XB_ALL_RECS - Get the last record, deleted or not.<br>
           XB_ACTIVE_RECS - Get the last active record.<br>
           XB_DELETED_RECS - Get the last deleted record.<br>
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16  GetLastRecord    ( xbInt16 iOption );

  //! @brief Get table level multi user setting.
  /*!
      @param iOpt - 0 Return the derived setting for this table, taking into account the 
                      the system level default and the table level setting if valued.<br>
                    1 Return the table level setting

      @returns xbOn - Table is set for multi user mode
               xbOff - Table is not set for multi user mode
               xbSysDflt - Use system default setting.  This value only returned if iOpt is 1
  */
  virtual xbInt16 GetMultiUser( xbInt16 iOpt = 0 ) const;

  //! @brief Get the next record.
  /*!
    Get the next not deleted record.  This routines skips over any deleted records.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 GetNextRecord();

  //! @brief Get the next record.
  /*!
    @param iOption XB_ALL_RECS - Get the next record, deleted or not.<br>
           XB_ACTIVE_RECS - Get the next active record.<br>
           XB_DELETED_RECS - Get the next deleted record.<br>
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16  GetNextRecord    ( xbInt16 iOption );

  //! @brief Get the next record.
  /*!
    @param iOption XB_ALL_RECS - Get the next record, deleted or not.<br>
           XB_ACTIVE_RECS - Get the next active record.<br>
           XB_DELETED_RECS - Get the next deleted record.<br>
    @param ulStartRec Get next record, starting from ulStartRec.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 GetNextRecord( xbInt16 iOption, xbUInt32 ulStartRec );


  //! @brief Get the previous record.
  /*!
    Get the previous not deleted record.  This routine skips over any deleted records.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 GetPrevRecord();

  //! @brief Get the previous record.
  /*!
    @param iOption XB_ALL_RECS - Get the previous record, deleted or not.<br>
           XB_ACTIVE_RECS - Get the previous active record.<br>
           XB_DELETED_RECS - Get the previous deleted record.<br>
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 GetPrevRecord( xbInt16 iOption );

  //! @brief Get record for specified record number.
  /*!
    Retrieve a record from disk and load it into the record buffer.  If auto commit
    is enabled and there are pending updates, this routine will flush the updates
    to disk before proceeding to ulRecNo.

    @param ulRecNo - Record number to retrieve.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 GetRecord( xbUInt32 ulRecNo );

  //! @brief Get the current number of records in the DBF data file.
  /*!
    @param ulRecCnt Output number of records in file.
    @returns XB_NO_ERROR<br>
             XB_INVALID_LOCK_OPTION<br>
             XB_INVALID_OPTION<br>
             XB_LOCK_FAILED<br>
             XB_READ_ERROR<br>
             XB_SEEK_ERROR
  */
  virtual xbInt16 GetRecordCnt( xbUInt32 & ulRecCnt );

  //! @brief Get pointer to record buffer 
  /*!
    @param iOpt 0 for RecBuf (current data, includes pending file updates)<br>
                1 for RecBuf2 (original contents)

    @returns Pointer to record buffer.
  */
  virtual char * GetRecordBuf( xbInt16 iOpt = 0 ) const;

  //! @brief Get the DBF record length. 
  /*!
    @returns Record length.
  */
  virtual xbUInt16 GetRecordLen() const;

  //! @brief Get the table alias. 
  /*!
    This routine returns the table alias.
    @returns Table alias
  */
  virtual const xbString &GetTblAlias() const;

  //! @brief Get the class version.
  /*!
    @returns class xbDbf3 returns 3<br>
             class xbDbf4 returns 4
  */
  virtual xbInt16 GetVersion() const = 0; 

  //! @brief Get the pointer to the xbXbase structure,
  /*!
    @returns Pointer to xbXbase structure.
  */
  virtual xbXBase *GetXbasePtr() const;   // return xbase pointer

  //! @brief Check for existence of any memo fields. 
  /*!
    @returns xbTrue - Memo fields exist.<br>
             xbFalse - Memo fields don't exist.
  */
  virtual xbBool MemoFieldsExist() const;

  //! @brief Open a table/DBF file. 
  /*!
    This routine sets the alias name to the same as the table name.
    @param sTableName Fully qualified file name (table) to open, Include the .dbf or .DBF extension. 
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 Open( const xbString &sTableName );

  //! @brief Open a table/DBF file. 
  /*!
    @param sTableName Fully qualified file name (table) to open, Include the .dbf or .DBF extension. 
    @param sAlias Alias name to assign to this entry.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 Open( const xbString &sTableName, const xbString &sAlias );

  //! @brief Open a table/DBF file. 
  /*!
    @param sTableName Fully qualified file name (table) to open, Include the .dbf or .DBF extension. 
    @param sAlias Alias name to assign to this entry.
    @param iOpenMode XB_READ or XB_READ_WRITE
    @param iShareMode XB_SINGLE_USER or XB_MULTI_USER
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 Open( const xbString &sTableName, const xbString &sAlias, xbInt16 iOpenMode, xbInt16 iShareMode ) = 0;

  //! @brief Pack DBF file. 
  /*!
    This routine eliminates all deleted records from the file and clears
    out any unused blocks in the memo file if one exists.

    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 Pack();

  //! @brief Pack DBF file. 
  /*!
    This routine eliminates all deleted records from the file and clears
    out any unused blocks in the memo file if one exists.

    @param ulDeletedRecCnt - Output - number of recrods removed from the file.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 Pack( xbUInt32 &ulDeletedRecCnt );

  //! @brief Write the current record to disk.
  /*!
    This routine is used to write any updates to the current record buffer to disk.

    Indices are automatically updated.

    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 PutRecord();

  //! @brief Write record to disk.
  /*!
    This routine is used to write a copy of the current record buffer to disk
    for a given record number.

    Indices are automatically updated.

    @param ulRecNo Record number to update.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16  PutRecord        ( xbUInt32 ulRecNo );

  //! @brief Return record deletion status.
  /*!
    This routine returns the record deletion status.
    @param iOpt 0 = Current record buffer, 1 = Original record buffer
    @returns xbTrue - Record deleted.<br>xbFalse - Record not deleted.
  */
  virtual xbInt16  RecordDeleted    ( xbInt16 iOpt = 0 ) const;

  //! @brief Rename table.
  /*!
     This routine renames a given table, associated DBT (memo), INF (NDX linkages) and MDX (Production index) files
     @param sNewName - New file name.
     @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 Rename( const xbString sNewName ) = 0;

  //! @brief Set auto commit.
  /*!
    This routine sets the auto commit setting for this table.

    @param iOpt -1 Use DBMS default<br>
                0  No auto update on this table, regardless of DBMS setting<br>
                1  Auto update on this table, regardless of DBMS setting<br>

    @returns XB_NO_ERROR or XB_INVALID_OPTION;
  */
  virtual xbInt16  SetAutoCommit    ( xbInt16 iOpt );

  //! @brief Undelete all records. 
  /*!
    This routine will remove the deletion flag on any deleted records in the table.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16  UndeleteAllRecords();

  //! @brief Undelete one record. 
  /*!
    This routine will undelete the current record, if it is deleted.
    @returns XB_NO_ERROR<br>XB_INVALID_RECORD
  */
  virtual xbInt16  UndeleteRecord   ();

  //! @brief  Clear the table
  /*!
  This routine eliminates everything from the DBF file and dbt memo file.
  @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16  Zap              ();


  /* field methods */
  //! @brief Get field data for field number.
  /*!
    @param iFieldNo Number of field to retrieve.
    @param sFieldValue Output field value.
    @param iRecBufSw 0 - Record buffer with any updates.<br>1 - Record buffer with original data.
    @returns XB_NO_ERROR<br>
             XB_INVALID_FIELD_NO
  */
  virtual xbInt16  GetField(xbInt16 iFieldNo, xbString &sFieldValue, xbInt16 iRecBufSw ) const;

  //! @brief Get field data for field number.
  /*!
    @param iFieldNo Number of field to retrieve.
    @param sFieldValue Output field value.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
             XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16  GetField(xbInt16 iFieldNo, xbString &sFieldValue) const;

  //! @brief Get xbString field for field name.
  /*!
    @param sFieldName Name of field to retrieve
    @param sFieldValue Output field value.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
             XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16  GetField(const xbString &sFieldName, xbString &sFieldValue) const;

  //! @brief Get decimal for field number.
  /*!
    This routine retreives a field's decimal length.
    @param iFieldNo Number of field to retrieve
    @param iFieldDecimal Output field value.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
             XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16  GetFieldDecimal( xbInt16 iFieldNo, xbInt16 &iFieldDecimal ) const;

  //! @brief Get decimal for field name.
  /*!
    This routine retreives a field's decimal length.
    @param sFieldName Name of field to retrieve
    @param iFieldDecimal Output field value.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
             XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16  GetFieldDecimal( const xbString &sFieldName, xbInt16 &iFieldDecimal ) const;


  //! @brief Get field length for field number.
  /*!
    This function retrieves a field's length.

    @param iFieldNo Name of field to retrieve
    @param iFieldLen Output field length value.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
      XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16  GetFieldLen( xbInt16 iFieldNo, xbInt16 &iFieldLen ) const;

  //! @brief Get field length for field name.
  /*!
    This function retrieves a field's length.

    @param sFieldName Name of field to retrieve
    @param iFieldLen Output field length value.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
             XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16  GetFieldLen( const xbString &sFieldName, xbInt16 &iFieldLen ) const;

  //! @brief Returns the name of the specified field.
  /*! Returns name for the field specified by iFieldNo.

      @param iFieldNo Number of field.
      @param sFieldName Output variable containing the field name.
      @returns XB_NO_ERROR<br>XB_INVALID_FIELD_NO
  */
  virtual xbInt16  GetFieldName( xbInt16 iFieldNo, xbString &sFieldName ) const;

  //! @brief  Get the field number for name.
  /*! Returns the field number for the named field.

     All field get/put methods require either a field number or field name as
     one of the parameters.  Using the methods that take the field numbers will
     yield slightly better performance because the methods that take a name, have 
     to look up the number.

     @param sFieldName Name of field.
     @param iFieldNo Output field number for the given name.
     @returns XB_NO_ERROR<br>XB_INVALID_FIELD_NAME
  */
  virtual xbInt16 GetFieldNo( const xbString &sFieldName, xbInt16 &iFieldNo ) const;

  //! Get field ID number for a given field name.
  /*! Returns the field number for the named field.

    @param sFieldName Name of field.
    @returns Number of field or XB_INVALID_FIELD_NAME.
  */
  virtual xbInt16 GetFieldNo( const xbString &sFieldName ) const;

  //! Get field type for field name.
  /*!
    @param sFieldName Field name.
    @param cFieldType Output field type.
    @returns XB_NO_ERROR<br>
             XB_INVALID_FIELD_NO

    <a href="..\include\FieldTypes.html">Field Types</a>
  */
  virtual xbInt16 GetFieldType( const xbString &sFieldName, char &cFieldType ) const;

  //! Get field type for field number.
  /*!
    @param iFieldNo Field number.
    @param cFieldType Output field type.
    @returns XB_NO_ERROR<br>
             XB_INVALID_FIELD_NO

    <a href="..\include\FieldTypes.html">Field Types</a>
  */
  virtual xbInt16 GetFieldType( xbInt16 iFieldNo, char &cFieldType ) const;


  //! @brief Put field for field name.
  /*!
    @param sFieldName Field name.
    @param sFieldValue Field value.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
             XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16 PutField( const xbString &sFieldName, const xbString &sFieldValue );

  //! @brief Put field for field number.
  /*!
    @param iFieldNo Number of field to update.
    @param sFieldValue Field value.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
             XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16 PutField( xbInt16 iFieldNo, const xbString &sFieldValue );

  //! @brief Put logical field for field number.
  /*!
    @param iFieldNo Number of field to update.
    @param sFieldValue Field value.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
             XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16 PutLogicalField( xbInt16 iFieldNo, const xbString &sFieldValue );

  //! @brief Put logical field for field name.
  /*!
    @param sFieldName Name of field to update.
    @param sFieldValue Field value.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
             XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16 PutLogicalField( const xbString &sFieldName, const  xbString &sFieldValue );

  //! @brief Get logical field for field number.
  /*!
    @param iFieldNo Number of field to retrieve.
    @param sFieldValue Output field value.
    @returns XB_NO_ERROR<br>XB_INVALID_FIELD_NO<br>
      XB_INVALID_FIELD_TYPE
  */
  virtual xbInt16  GetLogicalField( xbInt16 iFieldNo, xbString &sFieldValue ) const;

  //! @brief Get logical field for field name.
  /*!
    @param sFieldName Name of field to retrieve.
    @param sFieldValue Output field value.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
             XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16  GetLogicalField( const xbString &sFieldName, xbString &sFieldValue ) const;

  //! @brief Get logical field for field number.
  /*!
    @param iFieldNo Number of field to retrieve.
    @param bFieldValue Output field value.
    @returns XB_NO_ERROR<br>XB_INVALID_FIELD_NO<br>
      XB_INVALID_FIELD_TYPE
  */
  virtual xbInt16  GetLogicalField( xbInt16 iFieldNo, xbBool &bFieldValue ) const;

  //! @brief Put logical field for field number.
  /*!
    @param iFieldNo Number of field to update.
    @param bFieldValue Field value.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
      XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16  PutLogicalField( xbInt16 iFieldNo, xbBool bFieldValue );

  //! @brief Put logical field for field name.
  /*!
    @param sFieldName Name of field to update.
    @param bFieldValue Field value.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16  PutLogicalField( const xbString &sFieldName, xbBool bFieldValue );



  //! @brief Get logical field for field number.
  /*!
    @param iFieldNo Number of field to retrieve.
    @param bFieldValue Output field value.
    @param iRecBufSw 0 - Record buffer with any updates.<br>1 - Record buffer with original data.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
      XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16  GetLogicalField( xbInt16 iFieldNo, xbBool &bFieldValue, xbInt16 iRecBufSw ) const;

  //! @brief Get logical field for field name.
  /*!
    @param sFieldName Name of field to retrieve.
    @param bFieldValue Output field value.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
             XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16  GetLogicalField( const xbString &sFieldName, xbBool &bFieldValue ) const;

  //! @brief Get long field for field number.
  /*!
    @param iFieldNo Number of field to retrieve.
    @param lFieldValue Output field value.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
             XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16  GetLongField( xbInt16 iFieldNo, xbInt32 &lFieldValue ) const;

  //! @brief Get long field for field number.
  /*!
    @param sFieldName Name of field to retrieve.
    @param lFieldValue Output field value.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
             XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16  GetLongField( const xbString &sFieldName, xbInt32 &lFieldValue ) const;

  //! @brief Put long field for field number.
  /*!
    @param iFieldNo Number of field to update.
    @param lFieldValue Field value.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
             XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16  PutLongField( xbInt16 iFieldNo, xbInt32 lFieldValue );

  //! @brief Put long field for field name.
  /*!
    @param sFieldName Name of field to update.
    @param lFieldValue Field value.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
             XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16  PutLongField( const xbString &sFieldName, xbInt32 lFieldValue );

  //! @brief Get unsigned long field for field number.
  /*!
    @param iFieldNo Number of field to retrieve.
    @param ulFieldValue Output field value.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
             XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16  GetULongField( xbInt16 iFieldNo, xbUInt32 &ulFieldValue ) const;

  //! @brief Get unsigned long field for field name.
  /*!
    @param sFieldName Number of field to retrieve.
    @param ulFieldValue Output field value.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
             XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16  GetULongField( const xbString &sFieldName, xbUInt32 &ulFieldValue ) const;

  //! @brief Put unsigned long field for field number.
  /*!
    @param iFieldNo Number of field to update.
    @param ulFieldValue Field value.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
             XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16  PutULongField( xbInt16 iFieldNo, xbUInt32 ulFieldValue );

  //! @brief Put unsigned long field for field name.
  /*!
    @param sFieldName Name of field to update.
    @param ulFieldValue Field value.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
             XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16  PutULongField( const xbString &sFieldName, xbUInt32 ulFieldValue );


  //! @brief Get xbDouble field for field number.
  /*!
    @param iFieldNo Number of field to retrieve.
    @param dFieldValue Output field value.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
             XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16  GetDoubleField( xbInt16 iFieldNo, xbDouble &dFieldValue ) const;

  //! @brief Get xbDouble field for field number.
  /*!
    @param iFieldNo Number of field to retrieve.
    @param dFieldValue Output field value.
    @param iRecBufSw 0 - Record buffer with any updates.<br>1 - Record buffer with original data.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
           XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16  GetDoubleField( xbInt16 iFieldNo, xbDouble &dFieldValue, xbInt16 iRecBufSw ) const;

  //! @brief Get xbDouble field for field name.
  /*!
    @param sFieldName Name of field to retrieve.
    @param dFieldValue Output field value.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
             XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16  GetDoubleField( const xbString &sFieldName, xbDouble &dFieldValue ) const;

  //! @brief Put double field for field number.
  /*!
    @param iFieldNo Number of field to update.
    @param dFieldValue Field value.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
           XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16 PutDoubleField( xbInt16 iFieldNo, xbDouble dFieldValue );

  //! @brief Put double field for field name.
  /*!
    @param sFieldName Name of field to update.
    @param dFieldValue Field value.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
             XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16  PutDoubleField( const xbString &sFieldName, xbDouble dFieldValue );

  //! @brief Get xbFloat field for field number.
  /*!
    @param iFieldNo Number of field to retrieve.
    @param fFieldValue Output field value.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
      XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16  GetFloatField( xbInt16 iFieldNo, xbFloat &fFieldValue ) const;

  //! @brief Get xbFloat field for field name.
  /*!
    @param sFieldName Number of field to retrieve.
    @param fFieldValue Output field value.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
             XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16  GetFloatField( const xbString &sFieldName, xbFloat &fFieldValue ) const;

  //! @brief Put float field for field number.
  /*!
    @param iFieldNo Number of field to update.
    @param fFieldValue Field value.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
      XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16  PutFloatField( xbInt16 iFieldNo, xbFloat fFieldValue );

  //! @brief Put float field for field name.
  /*!
    @param sFieldName Name of field to update.
    @param fFieldValue Field value.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
             XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16  PutFloatField( const xbString &sFieldName, xbFloat fFieldValue );

  //! @brief Get xbDate field for field number.
  /*!
    @param iFieldNo Number of field to retrieve.
    @param dtFieldValue Output field value.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
             XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16  GetDateField( xbInt16 iFieldNo, xbDate &dtFieldValue ) const;

  //! @brief Get xbDate field for field name.
  /*!
    @param sFieldName Name of field to retrieve.
    @param dtFieldValue Output field value.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
             XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16  GetDateField( const xbString &sFieldName, xbDate &dtFieldValue ) const;

  //! @brief Put date field for field number.
  /*!
    @param iFieldNo Number of field to update.
    @param dtFieldValue Field value.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
             XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16 PutDateField( xbInt16 iFieldNo, const xbDate &dtFieldValue );

  //! @brief Put field for field name.
  /*!
    @param sFieldName Name of field to update.
    @param dtFieldValue Field value.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
             XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16 PutDateField( const xbString &sFieldName, const xbDate &dtFieldValue );


  //! @brief Get field null status
  /*!
    @param iFieldNo Number of field to retrieve.
    @param bIsNull Output field value.  If field is all spaces on record buffer, returns true.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
             XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16  GetNullSts( xbInt16 iFieldNo, xbBool &bIsNull ) const;

  //! @brief Get field null status
  /*!
    @param sFieldName Field Name of field to retrieve.
    @param bIsNull Output field value.  If field is all spaces on record buffer, returns true.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
             XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16  GetNullSts( const xbString &sFieldName, xbBool &bIsNull ) const;

  //! @brief Get field null status
  /*!
    @param iFieldNo Number of field to retrieve.
    @param bIsNull Output field value.  If field is all spaces on record buffer, returns true.
    @param iRecBufSw 0 - Record buffer with any updates.<br>1 - Record buffer with original data.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
             XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16  GetNullSts( xbInt16 iFieldNo, xbBool &bIsNull, xbInt16 iRecBufSw ) const;



  #ifdef XB_MEMO_SUPPORT
  //! @brief Get the memo field count for this table.
  /*!
      @returns Returns the number of memo fields in the table,
  */
  virtual xbInt16  GetMemoFieldCnt   () const;

  //! @brief Get pointer to Memo object.
  /*!
    @returns This routine returns the pointer to the memo object.
  */
  virtual xbMemo   *GetMemoPtr       ();

  //! @brief Get the memo file block size used when creating a memo file. 
  /*!
    @returns Memo block size.
  */
  virtual xbUInt32 GetCreateMemoBlockSize() const;

  //! @brief Get memo field for field number.
  /*!
    @param iFieldNo Number of field to retrieve.
    @param sMemoValue Output field value.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
             XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16 GetMemoField( xbInt16 iFieldNo, xbString &sMemoValue );

  //! @brief Get memo field for field name.
  /*!
    @param sFieldName Name of field to retrieve.
    @param sMemoValue Output field value.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
             XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16 GetMemoField( const xbString & sFieldName, xbString &sMemoValue );

  //! @brief Get memo field length for field number.
  /*!
    @param iFieldNo Number of field to retrieve. 
    @param ulMemoFieldLen Output memo field value length.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
             XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16 GetMemoFieldLen( xbInt16 iFieldNo, xbUInt32 &ulMemoFieldLen );

  //! @brief Get memo field length for field name.
  /*!
    @param sFieldName Name of field to retrieve.
    @param ulMemoFieldLen Output memo field value length.
    @returns XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
             XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
  */
  virtual xbInt16  GetMemoFieldLen   ( const xbString & sFieldName, xbUInt32 &ulMemoFieldLen );

  //! @brief Check if memo field exists for field number.
  /*!
    @param iFieldNo Number of field to check.
    @returns xbTrue Field exists<br> xbFalse Field does not exist
  */
  virtual xbBool   MemoFieldExists   ( xbInt16 iFieldNo ) const;

  //! @brief Check if memo field exists for field name.
  /*!
    @param sFieldName Name of field to check.
    @returns xbTrue Field exists<br> xbFalse Field does not exist
  */
  virtual xbBool   MemoFieldExists   ( const xbString &sFieldName ) const;


  //! @brief Set memo block size
  /*!

   This routine sets the memo file block size. This value is used when 
   the memo file is created so you if you need to change it, this must be 
   set before creating the table.

   Version 3 is set to 512 and can't be changed.

   The default size for version 4 is 1024 and can be updated in 512 byte increments.

   @returns XB_NO_ERROR or XB_INVALID_BLOCK_SIZE
  */
  virtual xbInt16  SetCreateMemoBlockSize( xbUInt32 ulBlockSize ) = 0;

  //! @brief Update memo field
  /*!
    This routine updates a memo field.
    @param iFieldNo - Memo field number to update.
    @param sMemoData - Memo data for update.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16  UpdateMemoField   ( xbInt16 iFieldNo, const xbString &sMemoData );

  //! @brief Update memo field
  /*!
    This routine updates a memo field.
    @param sFieldName - Memo field name to update.
    @param sMemoData - Memo data for update.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16  UpdateMemoField   ( const xbString & sFieldName, const xbString &sMemoData );
  #endif // XB_MEMO_SUPPORT


  #ifdef XB_LOCKING_SUPPORT
  //! @brief Lock table. 
  /*!
    This routine locks the table for updates.

    If auto locking is enabled, the library handles all needed locking activity.

    @param iLockFunction XB_LOCK<br>XB_UNLOCK
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 LockTable         ( xbInt16 iLockFunction );

  //! @brief Lock Record 
  /*!
    This routine locks a record for update.

    If auto locking is enabled, the library handles all needed locking activity.

    @param iLockFunction XB_LOCK<br>XB_UNLOCK
    @param ulRecNo Record number to lock
    @returns XB_NO_RECORD<br>
             XB_INVALID_OPTION<br>
             XB_INVALID_RECORD<br>
             XB_LOCK_FAILED
  */
  virtual xbInt16 LockRecord        ( xbInt16 iLockFunction, xbUInt32 ulRecNo );

  //! @brief Lock append bytes.
  /*!
    This routine locks the append bytes and is used by the AppendRecord function.

    If auto locking is enabled, the library handles all needed locking activity.

    @param iLockFunction XB_LOCK<br>XB_UNLOCK
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 LockAppend        ( xbInt16 iLockFunction );

  //! @brief Lock Header 
  /*!
    This routine locks the file header.

    If auto locking is enabled, the library handles all needed locking activity.

    @param iLockFunction XB_LOCK<br>
                         XB_UNLOCK
    @returns XB_NO_ERROR<br>
             XB_LOCK_FAILED<br>
             XB_INVALID_OPTION
  */
  virtual xbInt16 LockHeader        ( xbInt16 iLockFunction );

  #ifdef XB_MEMO_SUPPORT

  //! @brief Lock Memo file.
  /*!
    This routine locks the memo file for updates.

    If auto locking is enabled, the library handles all needed locking activity.

    @param iLockFunction XB_LOCK<br>XB_UNLOCK
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 LockMemo          ( xbInt16 iLockFunction );

  //! @brief  Get the lock status of the memo file.
  /*!
    @returns Lock status of memo file.
  */
  virtual xbBool  GetMemoLocked     () const; 
  #endif  // XB_MEMO_LOCK

  #ifdef XB_INDEX_SUPPORT
  //! @brief Lock Index files.
  /*!
    This routine locks all the index files.

    If auto locking is enabled, the library handles all needed locking activity.

    @param iLockFunction XB_LOCK<br>XB_UNLOCK
    @returns XB_INVALID_LOCK_OPTION<br>
             XB_LOCK_FAILED<br>
             XB_SEEK_ERROR
  */
  virtual xbInt16 LockIndices       ( xbInt16 iLockFunction );
  #endif // XB_INDEX_SUPPORT


  //! @brief Set table level multi user setting.
  /*!

      @param iMultiUserOption xbOn  Set table for multi user mode.<br>
                     xbOff  Set table for single user mode.<br>
                     xbSysDflt (minus one) Use system default.<br>

      @returns XB_NO_ERROR
               XB_INVALID_OPTION
               XB_NOT_CLOSED
  */
  virtual xbInt16 SetMultiUser( xbInt16 iMultiUserOption );


  //! @brief Get lock flavor.
  /*!
    This routine is currently in place to provide structure for future locking
    schemes that may differ from the legacy dBASE (TM) locking scheme.
    @returns Always 1.
  */
  xbInt16 GetLockFlavor()const;

  void SetLockFlavor( xbInt16 iLockFlavor );

  //! @brief Get table locked status 
  /*!
    @returns xbTrue  Header is locked<br>
               xbFalse Header is unlocked
  */
  xbBool GetTableLocked()const;

  //! @brief Return lock status of the table header 
  /*! @returns xbTrue  Header is locked<br>
               xbFalse Header is unlocked
  */
  xbBool GetHeaderLocked() const;

  //! @brief Get the append locked bytes status 
  /*!
    @returns The record number of the new record for the append lock operation.
  */
  xbUInt32 GetAppendLocked() const;

  //! @brief Get the first first record lock.
  /*!
    Get the first record lock from a linked list of record locks.
    @returns First record lock.
  */
  xbLinkListNode<xbUInt32> * GetFirstRecLock() const;

  #ifdef XB_DEBUG_SUPPORT
  //! @brief Dump the table lock status 
  /*!
    Debugging routine.  Dumps the table lock status to the console.//! @brief GetHeaderLen for DBF
  */
  void DumpTableLockStatus() const;
  #endif  // XB_DEBUG_SUPPORT
  #endif  // XB_LOCKING_SUPPORT


  #ifdef XB_INDEX_SUPPORT
  virtual xbInt16  CheckTagIntegrity( xbInt16 iTagOpt, xbInt16 iOutputOpt );
  //! @brief Close an open index file
  /*!
     All index files are automatically closed when the DBF file is closed.
     Under normal conditions, it is not necessary to explicitly close an index file
     with this routine.  Any updates posted to a DBF file while an index is closed 
     will not be reflected in the closed index file.

    @param pIx Pointer to index object to close.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16  CloseIndexFile( xbIx *pIx );

  //! @brief Create a new tag (index) for this DBF file (table).
  /*!
    This routine creates a new tag (index) on a DBF file.  The library currently supports NDX, MDX ans TDX.
    indices.  If you don't have a specific need for an NDX file, use MDX.

    @param sIxType "MDX", "NDX" or "NTX".
    @param sName Index or tag name.
    @param sKey Index key expression or field name.
    @param sFilter Filter expression.  Not applicable for NDX indices.
    @param iDescending xbTrue for descending.  Not available for NDX indices.<br>
                       xbFalse - ascending
    @param iUnique xbTrue - Unique index<br>xbFalse - Not unique index.
    @param iOverLay xbTrue - Overlay if exists<br> 
                    xbFalse - Don't overlay if it exists.
    @param pIxOut Pointer to pointer of output index object.
    @param vpTagOut Pointer to pointer of newly created tag,
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 CreateTag( const xbString &sIxType, const xbString &sName, const xbString &sKey, const xbString &sFilter, 
                   xbInt16 iDescending, xbInt16 iUnique, xbInt16 iOverLay, xbIx **pIxOut, void **vpTagOut );

  //! @brief Delete an index tag.
  /*!
    This routine deletes an index tag.  If it is a single tag file, or the only tag in a multi tag file,
    the file is also deleted.
    @param sIxType Either "NDX", "MDX" or "TDX".<br>
    @param sName Tag name to delete.<br>
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16  DeleteTag( const xbString &sIxType, const xbString &sName );

  //! @brief 
  /*!
    This routine deletes all indices associated with the DBF file.

    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16  DeleteAllIndexFiles();

  //! @brief Find record for string key for current tag.
  /*!
    Find a key and position to record if key found

    @param sKey  String key to find
    @returns <a href="xbretcod_8h.html">Return Codes</a>

  */
  virtual xbInt16 Find( xbString &sKey );

  //! @brief Find record for date key for current tag.
  /*!
    Find a key and position to record if key found

    @param dtKey  Date key to find
    @returns <a href="xbretcod_8h.html">Return Codes</a>

  */
  virtual xbInt16 Find( xbDate   &dtKey );

  //! @brief Find record for numeric key for current tag.
  /*!
    Find a key and position to record if key found

    @param dKey  Numeric key to find
    @returns <a href="xbretcod_8h.html">Return Codes</a>

  */
  virtual xbInt16 Find( xbDouble &dKey );

  //! @brief Find record for string key for given tag.
  /*!

    Find a string key and position to record if found.

    @param pIx   Pointer to index file
    @param vpTag Pointer to index tag
    @param sKey  String key to find
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 Find( xbIx *pIx, void *vpTag, const xbString &sKey );

  //! @brief Find record for date key for given tag.
  /*!

    Find a date key and position to record if found.

    @param pIx   Pointer to index file
    @param vpTag Pointer to index tag
    @param dtKey Date key to find
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 Find( xbIx *pIx, void *vpTag, const xbDate &dtKey );

  //! @brief Find record for numeric key for given tag.
  /*!

    Find a numeric key and position to record if found.

    @param pIx   Pointer to index file
    @param vpTag Pointer to index tag
    @param dKey  Numeric key to find
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 Find( xbIx *pIx, void *vpTag, const xbDouble &dKey );


  //! @brief Get a pointer to the current index object.
  /*!
    @returns Pointer to current index.
  */
  virtual xbIx *GetCurIx() const;

  /*!
    An index file can have one or more tags
    NDX index files have one tag per file.
    MDX index files can can have up to 47 tags per file.
    TDX index files can can have up to 47 tags per file.
    @returns Pointer to current tag.
  */
  virtual void *GetCurTag() const;

  //! @brief Get index pointer and tag pointer for tag name.
  /*!

    Get index pointer and tag pointer for tag name.

    @param sTagName  Input - Tag Name to find pointers for
    @param pIx       Output - Pointer to index tag file structure
    @param vpTag     Output - Pointer to index tag
    @returns XB_NOT_FOUND<br>
             XB_INVALID_TAG
  */
  virtual xbInt16 GetTagPtrs( const xbString &sTagName, xbIx **pIx, void **vpTag );

  //! @brief GetFirstKey for last current tag.
  /*!
    Position to the first key for the current tag
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 GetFirstKey();

  //! @brief GetNextKey for last current tag.
  /*!
    Position to the next key for the current tag
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 GetNextKey();

  //! @brief GetPrevKey for last current tag.
  /*!
    Position to the prev//! @brief Find record for key for current tag.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 GetPrevKey();

  //! @brief GetLastKey for last current tag.
  /*!
    Position to the last key for the current tag
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 GetLastKey();

  //! @brief GetFirstKey for current tag.
  /*!
    Position to the first key for the current tag
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */

  //! @brief GetFirstKey for specified tag.
  /*!
    Position to the first key for the specified tag
    @param pIx Pointer to index file instance
    @param vpTag Pointer to tag instance
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 GetFirstKey( xbIx *pIx, void *vpTag );

  //! @brief GetNextKey for specified tag.
  /*!
    Position to the next key for the specified tag
    @param pIx Pointer to index file instance
    @param vpTag Pointer to tag instance
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 GetNextKey( xbIx *pIx, void *vpTag );

  //! @brief GetPrevKey for specified tag.
  /*!
    Position to the prev key for the specified tag
    @param pIx Pointer to index file instance
    @param vpTag Pointer to tag instance
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 GetPrevKey( xbIx *pIx, void *vpTag );

  //! @brief GetLastKey for given tag.
  /*!
    Position to the last key for the specified tag
    @param pIx Pointer to index file instance
    @param vpTag Pointer to tag instance
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */

  virtual xbInt16 GetLastKey( xbIx *pIx, void *vpTag );

  virtual const xbString &GetCurIxType() const;

  //! @brief Get the current tag name.
  /*!
    @returns Current Tag Name.<br>
  */
  virtual const xbString &GetCurTagName() const;

  //! @brief Return pointer to list of index files for the table.
  /*!
    @returns Returns an xbIxList * pointer to list of open index files.
  */
  virtual xbIxList *GetIxList() const;

  //! @brief Physical count of open index files.
  /*!
    Returns a physical count of open index files for the DBF file. An index file
    can contain one or more tags.
    @returns Count of open index files.
  */
  virtual xbInt32 GetPhysicalIxCnt() const;

  //! @brief Get tag list for DBF file. 
  /*!
     This routine returns a list of tags for the file.<br>

     The library is structured to support one or more files of the same or differing 
     index types (NDX/MDX/TDX), with  each file supporting one or more index tags.<br>

     @returns Tag list for the file/table.
  */
  xbLinkListNode<xbTag *> *GetTagList() const;

  //! @brief Open an index.
  /*!
    Open an index file for the DBF file.

    @param sIxType - "NDX" or "MDX"
    @param sIndexName - File name of index,
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 OpenIndex( const xbString &sIxType, const xbString &sIndexName );

  /*! @brief Reindex / rebuild index tag.
    @param iTagOpt 0 - Reindex current tag<br>
                   1 - Reindex all tags<br>
                   2 - Reindex for tag identified by vpTag
    @param pIx   if option 2 used, point to index file to reindex
    @param vpTag if option 2 used, pointer to tag to reindex

    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 Reindex( xbInt16 iTagOpt = 0, xbIx **pIx = NULL, void **vpTag = NULL );

  //! @brief Set the current tag for the DBF file.
  /*!
    @param sTagName - Tag Name
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 SetCurTag( const xbString &sTagName );

  //! @brief Set the current tag for the DBF file.
  /*!
    Method assumes valid pointer addresses used.

    @param sIxType  One of NDX, MDX or TDX (temporary)
    @param pIx Pointer to index object
    @param vpTag Pointer to tag object
  */
  virtual void SetCurTag( const xbString &sIxType, xbIx *pIx, void *vpTag );

  #endif  // XB_INDEX_SUPPORT

  #ifdef XB_INF_SUPPORT
  //! @brief Asscoiate a non production index to a DBF file. 
  /*!

    The original dBASE (TM) software supported non production indices (NDX) and production indices (MDX).
    The production indices are opened automatically when the DBF file is opened but the non-production
    indices are not.  This method is specific to the Xbas64 library and providex a means to link non production
    NDX index files to the DBF file so they will be opened automatically when the DBF file is opened.<br>

    This routine requires INF support be enabled when building the library.<br>
    This routine creates a file with the same name as the DBF file, but with an extension of INF.<br>


    @param sType Currently only NDX. Future versions will support additional non prod index types.
    @param sName The index name.
    @param iOpt 0 - Add index to .INF if not already there<br>
                1 - Remove index from .INF if exists

    @returns XB_NO_ERROR if no error<br>
             XB_CLOSE_ERROR<br>
             XB_FILE_NOT_FOUND<br>
             XB_INVALID_INDEX<br>
             XB_OPEN_ERROR<br>
             XB_WRITE_ERROR
  */
  virtual xbInt16  AssociateIndex( const xbString &sType, const xbString &sName, xbInt16 iOpt );

  //! @brief Return first node of linked list of .INF items.
  /*!
    @returns List of .INF entries.
    @note Available if XB_BLOCKREAD_SUPPORT compiled into the library.
  */
  xbLinkListNode<xbString> *GetInfList() const;

  #endif   // XB_INF_SUPPORT


  #ifdef XB_BLOCKREAD_SUPPORT
  //! @brief Disable block read processing
  /*!
      Block read processing is designed to rapidly retrieve records from a .DBF file in sequential order.<br>

      It is not designed for read/write processing or data retrieval based on a tag sort.

      This method disables the block read functionality for a given table.

      @returns XB_NO_ERROR

      @note Available if XB_BLOCKREAD_SUPPORT compiled into the library.
  */
  xbInt16 DisableBlockReadProcessing();

  //! @brief Enable block read processing
  /*!
      Block read processing is designed to rapidly retrieve records from a .DBF file in sequential order.<br>

      It is not designed for read/write processing or data retrieval based on a tag sort.

      This method enables the block read functionality for a given table.

      @returns XB_NO_ERROR

      @note Available if XB_BLOCKREAD_SUPPORT compiled into the library.
      
      <a href="../include/BlockRead.html>Block Read Info</a>
  */

  xbInt16 EnableBlockReadProcessing();


  //! @brief Get block read status
  /*!
  
      Block read processing is designed to rapidly retrieve records from a .DBF file in sequential order.<br>

      This method returns the block read status for a given table.

      @returns xbTrue - Block read enabled for this table<br>
               xbFalse - Block read disabled for this table

      @note Available if XB_BLOCKREAD_SUPPORT compiled into the library.

      <a href="../include/BlockRead.html">Block Read Info</a>
  */
  xbBool  GetBlockReadStatus() const;
  #endif  // XB_BLOCKREAD_SUPPORT

 protected:
  #ifdef XB_INDEX_SUPPORT
  friend class xbIx;
  friend class xbIxMdx;
  friend class xbIxTdx;

           //! @brief Add an index to the internal list of indices for this table.
           /*!
               @private

               The index list is used during any table update process to update any open
               index file.  Index files can contain one or more tags.  Temporary tags
               are not included here because they are created after a table is open
               and will be deleted when the table is closed.

               @param ixIn Pointer to index object for a given index file.
               @param sFmt NDX, MDX or TDX.
               @returns <a href="xbretcod_8h.html">Return Codes</a>
           */
           xbInt16  AddIndex( xbIx *ix, const xbString &sFmt );
           // @brief Clear the index tag list.
           /*
                Protected method. Clears the list inf index tags.
           */
           void     ClearTagList();

           //! @brief Remove an index from the internal list of indices for this table 
           /*
               The index list is used during any table update process to update any open
               index file.  Index files can contain one or more tags.

               @param ixIn Pointer to index object for a given index file.
               @returns <a href="xbretcod_8h.html">Return Codes</a>
           */
           xbInt16  RemoveIndex( xbIx * ix );

           //! @brief Update SchemaIxFlag
           /*!
              This routine can be called from the DeleteTag routine if a tag has been deleted and the flag needs reset
              @param iFldNo - Which field the index flag needs changed on
              @param cVal   - Value to change it to
           */
           void     UpdateSchemaIxFlag( xbInt16 iFldNo, unsigned char cVal );



  //! @brief Update tag list. 
  /*!
    @private
    This routine updates the internal tag list of open index tags.
    @reeturns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual  xbInt16  UpdateTagList      ();

  #endif   // XB_INDEX_SUPPORT

  #ifdef XB_BLOCKREAD_SUPPORT
  friend class xbBlockRead;
  #endif // XB_BLOCKREAD_SUPPORT


  //! @brief Get raw field data for field number.
  /*!
    @private
    This is a protected method, used by other methods and not intended
    to be directly used by application programs.
    @param iFieldNo Number of field to retrieve.
    @param cBuf Pointer to buffer area provided by calling application program.
    @param ulBufSize Size of data to copy
    @param iRecBufSw 0 - Record buffer with any updates.<br>1 - Record buffer with original data.
    @returns The field routines return one of:<br>
      XB_NO_ERROR<br>
      XB_INVALID_FIELD_NO
  */
  virtual  xbInt16  GetRawField( xbInt16 iFieldNo, char *cBuf, xbUInt32 ulBufSize, xbInt16 iRecBufSw ) const;


  //! @brief Init class variables.
  /*! Not intended to be called within an application program
      @private
  */

  virtual  void     InitVars(); 
  //! @brief Read DBF file header information.
  /*!
     @private
     This method assumes the header has been locked appropriately
     in a multi user environment

     @param iPositionOption 0 - Don't fseek to beginning of file before read.<br>
                            1 - Start from beginning of file.

     @param iReadOption 0 - Read entire 32 byte header<br>
                        1 - Read first eight bytes which includes the last update date and number of records.

     @returns XB_NO_ERROR<br>
              XB_READ_ERROR
  */
  virtual  xbInt16  ReadHeader       ( xbInt16 iPositionOption, xbInt16 iReadOption );

  //! Set the version number
  //! @private
  virtual  xbInt16  SetVersion() = 0;

  //! Check schema for errors
  //! @private
  virtual  xbInt16  ValidateSchema( xbSchema * s ) = 0;

  //! @brief Write table header
  /*!
    @private

    Protected method.

    @param iPositionOption  0 - Don't fseek to beginning of file before read.<br>
                          1 - Go to beginning of file before read.
    @param iWriteOption     0 - Write entire 32 byte header.<br>
                          1 - Write first eight bytes needed for table updates - last update date and number of records.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  xbInt16 WriteHeader( xbInt16 iPositionOption, xbInt16 iWriteOption );


  #ifdef XB_INF_SUPPORT
  //! @brief Delete .INF File
  /*!
    @private

    The original dBASE (TM) software supported non production indices (.NDX files) and production indices (.MDX files).

    By default, production indices are opened automatically when the DBF file is opened but non-production indices are not.

    This method is part of the INF logic that is specific to the Xbas64 library which is used to provide a means to link 
    non production NDX index files to the DBF file so they will be opened automatically when the DBF file is opened.

    This routine deletes the .INF file associated with the given .DBF data file.

    @returns <a href="xbretcod_8h.html">Return Codes</a>

    @note Available if XB_INF_SUPPORT is compiled into the library.
  */
  virtual xbInt16  DeleteInfData();

  //! @brief Return the .INF file name
  /*!
    @private

    If INF support is enabled in the library and a non production (NDX) index
    has been associated with the DBF file, the .INF file name can be retrieved 
    with this routine.

    @param sInfFileName Output string containing .INF file name.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
    @note Available if XB_INF_SUPPORT is compiled into the library.

  */
  virtual xbInt16  GetInfFileName( xbString &sInfFileName );

  //! @brief Load .INF data file,
  /*!
    @private

    Protected method.  This routine loads the ndx inf file.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16  LoadInfData();


  // @brief Update .INF data file.
  /*!
    @private
    Protected method.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16  SaveInfData();
  #endif // XB_INF_SUPPORT


  #ifdef XB_LOCKING_SUPPORT
  //! @brief Set the header locked flag
  //! @private
  void     SetHeaderLocked   ( xbBool bTableLocked );
  //! @brief Set the table locked flag
  //! @private
  void     SetTableLocked    ( xbBool bTableLocked );
  #endif // XB_LOCKING_SUPORT

  #ifdef XB_MEMO_SUPPORT
  xbInt16  iMemoFieldCnt;                 // Memo field cnt in the table
  xbMemo   *Memo;                         // Pointer to memo class
  xbUInt32 ulCreateMemoBlockSize;         // blocksize to use when creating dbt file
  #endif




  xbUInt32 ulCurRec;               // Current record or zero
  xbInt16  iAutoCommit;            // Auto commit updates if not explicitly performed before moving off record?
                                   //  -1  -->  Use DBMS default
                                   //   0  -->  No auto update on this table, regardless of DBMS setting
                                   //   1  -->  Auto update on this table, regardless of DBMS setting

  xbInt16  iFileVersion;           // xBase  file version - which class is in play


  xbString sAlias;                 // table alias
  xbInt16  iNoOfFields;
  xbInt16  iDbfStatus;             // 0 = closed
                                   // 1 = open
                                   // 2 = updates pending

  xbSchemaRec *SchemaPtr;          // Pointer to field data
  char     *RecBuf;                // Pointer to record buffer
  char     *RecBuf2;               // Pointer to original rec buf allocation


/* Next several variables are database header fields, up through dBASE V */
  unsigned char cVersion;
  char     cUpdateYY;
  char     cUpdateMM;
  char     cUpdateDD;
  xbUInt32 ulNoOfRecs;
  xbUInt16 uiHeaderLen;
  xbUInt16 uiRecordLen;
  char     cTransactionFlag;
  char     cEncryptionFlag;
  char     cIndexFlag;
  char     cLangDriver;                   // dBASE 7 sets to 1B


 private:
  // @brief Reset number of records.
  /*
    Protected method.  Resets number of records to 0.
  */
  void     ResetNoOfRecords(); 

  xbInt16  iMultiUser;                    //  xbOn      - table is shared
                                          //  xbOff     - table not shared
                                          //  xbSysDflt - table status determined by ssytem default setting

  #ifdef XB_LOCKING_SUPPORT
 // xbInt16  iAutoLock;                     // 0 - autolock off, 1 - autolock on
  xbInt16  iLockFlavor;
  xbBool   bTableLocked;                  // is the table locked
  xbBool   bHeaderLocked;                 // is the header locked
  xbUInt32 ulAppendLocked;                // record number of the new record for the append lock operation
  xbLinkListOrd<xbUInt32> lloRecLocks;    // ordered link list of locked records
  #endif

  #ifdef XB_INDEX_SUPPORT
  xbIxList *ixList;                      // pointer to a list of indices associated with the table
  xbIx     *pCurIx;                      // Pointer to current index class
  void     *vpCurIxTag;                  // Pointer to current tag
  xbString sCurIxType;                   // Current index type
  xbLinkList<xbTag *> llTags;            // linked list of open tags
  #endif // XB_INDEX_SUPPORT

  #ifdef XB_INF_SUPPORT
  xbLinkList<xbString> llInfData;        // linked list of strings containing ndx file entries
  #endif   // XB_INF_SUPPORT

  #ifdef XB_BLOCKREAD_SUPPORT
  xbBlockRead *pRb;
  xbBool      bBlockReadEnabled;        // if true, then block read mode is on
  #endif



};


#ifdef XB_DBF3_SUPPORT
//! @brief Derived class for handling dbf version III files/tables.
/*!
The xbDbf class is used as a base class for accessing dbf files.
The xbDbf3 class is derived from the xbDbf class and is designed to handle the
orginal version 3 type files.
*/

class XBDLLEXPORT xbDbf3 : public xbDbf {
 public:
  xbDbf3(xbXBase *x);
  ~xbDbf3();

  //! @brief Create Version 3 table.
  /*!
    This routine creates a dBASE III Plus (tm) DBF file.

    @param sTableName DBF table name.
    @param sAlias Table alias
    @param pSchema Pointer to schema structure with field definitions.
    @param iOverlay xbTrue - Overlay.<br> xbFalse - Don't overlay.
    @param iShareMode XB_SINGLE_USER<br>XB_MULTI_USER
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 CreateTable   ( const xbString &sTableName, const xbString &sAlias, xbSchema *pSchema, xbInt16 iOverlay, xbInt16 iShareMode );

  //! @brief Get version.
  /*!
   The version info can be retrieved to determine
   which class is being used for a given dbf instance.
   @returns 3
  */
  virtual xbInt16 GetVersion    () const;

  //! @brief Open dbf file/table.
  /*!
    @param sTableName DBF table name.
    @param sAlias Table alias
    @param iOpenMode XB_READ<br>XB_READ_WRITE
    @param iShareMode XB_SINGLE_USER<br>XB_MULTI_USER
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 Open          ( const xbString &sTableName, const xbString &sAlias, xbInt16 iOpenMode, xbInt16 iShareMode );

  //! @brief Rename table.
  /*!
     This routine renames a given table, associated DBT (memo) and INF (NDX linkages).
     @param sNewName - New file name.
     @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 Rename        ( const xbString sNewName );

  #ifdef XB_MEMO_SUPPORT

  //! @brief Create memo block size.
  /*!
     This routine sets the memo file block size. This value is used when 
     the memo file is created so you if you want to change it, this must be 
     called before creating the table.

     Version 3 uses a block size of 512.

     Version 4 can be adjusted and must be evenly divisible by 512.

     @param ulBlockSize - Ignored for version 3 memo files, set to 512.
     @returns XB_NO_ERROR or XB_INVALID_BLOCK_SIZE<br>XB_NO_ERROR
  */
  virtual xbInt16 SetCreateMemoBlockSize( xbUInt32 ulBlockSize );
  #endif

 protected:

  //! @brief Set version.
  /*!
     @private
     Sets the version to 3.  The version info can be retrieved to determine
     which class is being used for a given dbf instance.
     @returns 3
  */
  xbInt16 SetVersion    ();

  //! @brief Validate schema
  /*!
    @private
    This routine verifies the field types are valid for dBASE III Plus (tm).

    This routine validates an input schema<br>
    Negative return value is an error<br>
    Positive return value is the number of fields<br>
    On success, the class variable uiRecordLen will be updated with the record length of the combined total of the fields<br>

    @param s Pointer to schema structure with field definitions.

    @returns Number of fields or XB_INVALID_FIELD_TYPE.
  */
  virtual xbInt16 ValidateSchema( xbSchema *s );

 private:

};
#endif   /* XB_DBF3_SUPPORT */


#ifdef XB_DBF4_SUPPORT

//! @brief Derived class for handling dbf version IV files/tables.
/*!
The xbDbf class is used as a base class for accessing dbf files.
The xbDbf4 class is derived from the xbDbf class and is designed to handle the
orginal version 4 type files.
*/

class XBDLLEXPORT xbDbf4 : public xbDbf {
 public:
  xbDbf4( xbXBase *x );

  ~xbDbf4();
  //! @brief Create Version 4 table.
  /*!
    This routine creates a dBASE IV (tm) DBF file.

    @param sTableName DBF table name.
    @param sAlias Table alias
    @param pSchema Pointer to schema structure with field definitions.
    @param iOverlay xbTrue - Overlay.<br> xbFalse - Don't overlay.
    @param iShareMode XB_SINGLE_USER<br>XB_MULTI_USER
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 CreateTable     ( const xbString &sTableName, const xbString &sAlias, xbSchema *pSchema, xbInt16 iOverlay, xbInt16 iShareMode );

  //! @brief Get version.
  /*!
     The version info can be retrieved to determine
     which class is being used for a given dbf instance.
     @returns 4
  */
  virtual xbInt16 GetVersion      () const;

  //! @brief Open dbf file/table.
  /*!
    @param sTableName DBF table name.
    @param sAlias Table alias
    @param iOpenMode XB_READ<br>XB_READ_WRITE
    @param iShareMode XB_SINGLE_USER<br>XB_MULTI_USER
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 Open            ( const xbString &sTableName, const xbString &sAlias, xbInt16 iOpenMode, xbInt16 iShareMode );

  //! @brief Rename table.
  /*!
     This routine renames a table, associated memo, mdx and inf files
     @param sNewName - New file name.
     @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 Rename          ( const xbString sNewName );

  #ifdef XB_MEMO_SUPPORT


  //! @brief Create memo block size.
  /*!
     This routine sets the memo file block size. This value is used when 
     the memo file is created so you if you want to change it, this must be 
     called before creating the table.

     The default size for version 4 is 1024.

     @param ulBlockSize - Block size, must be evenly divisible by 512.
     @returns XB_INVALID_BLOCK_SIZE<br>XB_NO_ERROR
  */

  virtual xbInt16 SetCreateMemoBlockSize( xbUInt32 ulBlockSize );
  #endif

 protected:

  //! @brief Set version.
  /*!
     @private
     Sets the version to 4.  The version info can be retrieved to determine
     which class is being used for a given dbf instance.
     @returns 4
  */
  xbInt16 SetVersion();

  //! @brief Validate schema
  /*!
    @private
    This routine verifies the field types are valid for dBASE IV (tm).

    @param s Pointer to schema structure with field definitions.

    @returns Number of fields or XB_INVALID_FIELD_TYPE.
  */
  virtual xbInt16 ValidateSchema  ( xbSchema *s );

 private:

};

#endif   /* XB_DBF4_SUPPORT */

}        /* namespace xb    */
#endif   /* __XB_DBF_H__ */
