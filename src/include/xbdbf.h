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
  unsigned char cIxFlag;     /* DBase IV Index field flag               */
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


//! @brief Base class for handling dbf files/tables.
/*!
The xbDbf class is used as a base class for accessing dbf files.  
In line with relational theory, a each dbf file can be considered as a table.
The documentation uses the terms dbf and table interchangeably.<br>

This module handles methods for accessing and updating dbf files.

<br>
The class is designed to support additional file layouts with a minimal amount of effort.
If you are wanting to update the library to support a new dbf file type not currently supported 
by the library, create a derived class using xbDbf as a base class and modify methods needed
to support the new dbf file version.<br>
See the following for examples on how to start on this:<br>
xbDbf3 is a derived class from xbDbf and supports the original Dbase III+ file version.<br>
xbDbf4 is a derived class from xbDbf and supports the original Dbase IV file version.<br>

*/


class XBDLLEXPORT xbDbf : public xbFile {

 public:
  xbDbf( xbXBase *x );
  virtual ~xbDbf();

  virtual xbInt16  Abort();
  virtual xbInt16  AppendRecord();
  virtual xbInt16  BlankRecord();
  virtual xbInt16  Commit();
  virtual xbInt16  Close();
  virtual xbInt16  CopyDbfStructure( xbDbf *dNewTable, const xbString &sTableName, const xbString &sTableAlias, xbInt16 iOverlay, xbInt16 iShareMode );
  virtual xbInt16  CreateTable      ( const xbString &sTableName, const xbString &sAlias, xbSchema *pSchema, xbInt16 iOverlay, xbInt16 iShareMode ) = 0;
  virtual xbInt16  DeleteTable      ();
  virtual xbInt16  DeleteAll        ( xbInt16 iOption );
  virtual xbInt16  DeleteAllRecords ();
  virtual xbInt16  DeleteRecord     ();
  virtual xbInt16  DumpHeader       ( xbInt16 iOption );
  virtual xbInt16  DumpRecord       ( xbUInt32 ulRecNo, xbInt16 iOutputDest = 0, xbInt16 iOutputFmt = 0, char cDelim = ',' );
  virtual xbInt16  GetAutoCommit    () const;
  virtual xbInt16  GetAutoCommit    ( xbInt16 iOption ) const;
  virtual xbBool   GetBof           ();
  virtual xbUInt32 GetCurRecNo      () const;
  virtual xbInt16  GetDbfStatus     () const;
  virtual xbBool   GetEof           ();
  virtual xbInt32  GetFieldCnt      () const;
  virtual xbInt16  GetFirstRecord   ();
  virtual xbInt16  GetFirstRecord   ( xbInt16 iOption );
  virtual xbUInt16 GetHeaderLen     () const;
  virtual xbInt16  GetLastRecord    ();
  virtual xbInt16  GetLastRecord    ( xbInt16 iOption );

  virtual xbInt16  GetNextRecord    ();
  virtual xbInt16  GetNextRecord    ( xbInt16 iOption );
  virtual xbInt16  GetNextRecord    ( xbInt16 iOption, xbUInt32 ulStartRec );
  virtual xbInt16  GetPrevRecord    ();
  virtual xbInt16  GetPrevRecord    ( xbInt16 iOption );
  virtual xbInt16  GetRecord        ( xbUInt32 ulRecNo );

  // virtual xbUInt32 GetRecordCount   ();
  virtual xbInt16  GetRecordCnt     ( xbUInt32 & ulRecCnt );

  virtual char *   GetRecordBuf     ( xbInt16 iOpt = 0 ) const;
  virtual xbUInt16 GetRecordLen     () const;
  virtual const    xbString &GetTblAlias() const;
  virtual xbInt16  GetVersion       () const = 0; 
  virtual xbXBase  *GetXbasePtr     () const;   // return xbase pointer


  virtual xbBool   MemoFieldsExist  () const;

  virtual xbInt16  Open             ( const xbString &sTableName );
  virtual xbInt16  Open             ( const xbString &sTableName, const xbString &sAlias );
  virtual xbInt16  Open             ( const xbString &sTableName, const xbString &sAlias, xbInt16 iOpenMode, xbInt16 iShareMode ) = 0;
  virtual xbInt16  Pack             ();
  virtual xbInt16  Pack             ( xbUInt32 &ulDeletedRecCnt );


  virtual xbInt16  PutRecord        ();     // Put record to current location
  virtual xbInt16  PutRecord        ( xbUInt32 ulRecNo );
//  virtual xbInt16  ReadHeader       ( xbInt16 iFilePositionOption, xbInt16 iReadOption );
  virtual xbInt16  RecordDeleted    ( xbInt16 iOpt = 0 ) const;
  virtual xbInt16  Rename           ( const xbString sNewName ) = 0;

  virtual xbInt16  SetAutoCommit    ( xbInt16 iAutoCommit );

  virtual xbInt16  UndeleteAllRecords();
  virtual xbInt16  UndeleteRecord   ();
  virtual xbInt16  Zap              ();


  /* field methods */
  //virtual xbInt16  GetRawField( xbInt16 iFieldNo, char *Buf, xbUInt32 BufSize, xbInt16 iRecBufSw ) const;
  virtual xbInt16  GetField(xbInt16 iFieldNo, xbString &sFieldValue, xbInt16 iRecBufSw ) const;
  virtual xbInt16  GetField(xbInt16 iFieldNo, xbString &sFieldValue) const;
  virtual xbInt16  GetField(const xbString &sFieldName, xbString &sFieldValue) const;
  virtual xbInt16  GetFieldDecimal( xbInt16 iFieldNo, xbInt16 &iFieldDecimal ) const;
  virtual xbInt16  GetFieldDecimal( const xbString &sFieldName, xbInt16 &iFieldDecimal ) const;
  virtual xbInt16  GetFieldLen( xbInt16 iFieldNo, xbInt16 &iFieldLen ) const;
  virtual xbInt16  GetFieldLen( const xbString &sFieldName, xbInt16 &iFieldLen ) const;
  virtual xbInt16  GetFieldName( xbInt16 iFieldNo, xbString &sFieldName ) const;

  virtual xbInt16  GetFieldNo( const xbString &sFieldName, xbInt16 &iFieldNo ) const;
  virtual xbInt16  GetFieldNo( const xbString &sFieldName ) const;

  virtual xbInt16  GetFieldType( xbInt16 iFieldNo, char &cFieldType ) const;
  virtual xbInt16  GetFieldType( const xbString &sFieldName, char &cFieldType ) const;
  virtual xbInt16  PutField( const xbString &sFieldName, const xbString &sFieldData );
  virtual xbInt16  PutField( xbInt16 iFieldNo, const xbString &sFieldData );

  virtual xbInt16  PutLogicalField( xbInt16 iFieldNo, const xbString &sFieldData );
  virtual xbInt16  PutLogicalField( const xbString &sFieldName, const  xbString &sFieldData );
  virtual xbInt16  GetLogicalField( xbInt16 iFieldNo, xbString &sFieldData ) const;
  virtual xbInt16  GetLogicalField( const xbString &sFieldName, xbString &sFieldData) const;

  virtual xbInt16  PutLogicalField( xbInt16 iFieldNo, xbBool bFieldData );
  virtual xbInt16  PutLogicalField( const xbString &sFieldName, xbBool bFieldData );
  virtual xbInt16  GetLogicalField( xbInt16 iFieldNo, xbBool &bFieldData ) const;
  virtual xbInt16  GetLogicalField( xbInt16 iFieldNo, xbBool &bFieldData, xbInt16 iRecBufSw ) const;
  virtual xbInt16  GetLogicalField( const xbString &sFieldName, xbBool &bFieldData) const;

  virtual xbInt16  GetLongField( xbInt16 iFieldNo, xbInt32 &lFieldValue ) const;
  virtual xbInt16  GetLongField( const xbString &sFieldName, xbInt32 &lFieldValue ) const;
  virtual xbInt16  PutLongField( xbInt16 iFieldNo, xbInt32 lFieldValue );
  virtual xbInt16  PutLongField( const xbString &sFieldName, xbInt32 lFieldValue );

  virtual xbInt16  GetULongField( xbInt16 iFieldNo, xbUInt32 &lFieldValue ) const;
  virtual xbInt16  GetULongField( const xbString &sFieldName, xbUInt32 &lFieldValue ) const;
  virtual xbInt16  PutULongField( xbInt16 iFieldNo, xbUInt32 lFieldValue );
  virtual xbInt16  PutULongField( const xbString &sFieldNo, xbUInt32 lFieldValue );

  virtual xbInt16  GetDoubleField( xbInt16 FieldNo, xbDouble &dFieldValue ) const;
  virtual xbInt16  GetDoubleField( xbInt16 FieldNo, xbDouble &dFieldValue, xbInt16 iRecBufSw ) const;
  virtual xbInt16  GetDoubleField( const xbString &sFieldName, xbDouble &dFieldValue ) const;
  virtual xbInt16  PutDoubleField( xbInt16 FieldNo, xbDouble dFieldValue );
  virtual xbInt16  PutDoubleField( const xbString &FieldName, xbDouble dFieldValue );

  virtual xbInt16  GetFloatField( xbInt16 iFieldNo, xbFloat &fFieldValue ) const;
  virtual xbInt16  GetFloatField( const xbString &sFieldName, xbFloat &fFieldValue ) const;
  virtual xbInt16  PutFloatField( xbInt16 iFieldNo, xbFloat fFieldValue );
  virtual xbInt16  PutFloatField( const xbString &sFieldName, xbFloat fFieldValue );

  virtual xbInt16  GetDateField( xbInt16 iFieldNo, xbDate &dt ) const;
  virtual xbInt16  GetDateField( const xbString &sFieldName, xbDate &dt ) const;
  virtual xbInt16  PutDateField( xbInt16 iFieldNo, const xbDate &dt );
  virtual xbInt16  PutDateField( const xbString &sFieldName, const xbDate &dt );

  virtual xbInt16  GetNullSts( xbInt16 iFieldNo, xbBool &bIsNull ) const;
  virtual xbInt16  GetNullSts( const xbString &sFieldName, xbBool &bIsNull ) const;
  virtual xbInt16  GetNullSts( xbInt16 iFieldNo, xbBool &bIsNull, xbInt16 iRecBufSw ) const;



  #ifdef XB_MEMO_SUPPORT
  virtual xbInt16  GetMemoFieldCnt   () const;
  virtual xbMemo   *GetMemoPtr       ();
  virtual xbUInt32 GetCreateMemoBlockSize() const;
  virtual xbInt16  GetMemoField      ( xbInt16 iFldNo, xbString &sMemoData );
  virtual xbInt16  GetMemoField      ( const xbString & sFldName, xbString &sMemoData );
  virtual xbInt16  GetMemoFieldLen   ( xbInt16 iFldNo, xbUInt32 &ulMemoFieldLen );
  virtual xbInt16  GetMemoFieldLen   ( const xbString & sFldName, xbUInt32 &ulMemoFieldLen );
  virtual xbBool   MemoFieldExists   ( xbInt16 iFieldNo ) const;
  virtual xbBool   MemoFieldExists   ( const xbString &sFieldName ) const;
  virtual xbInt16  SetCreateMemoBlockSize( xbUInt32 ulBlockSize ) = 0;
  virtual xbInt16  UpdateMemoField   ( xbInt16 iFldNo, const xbString &sMemoData );
  virtual xbInt16  UpdateMemoField   ( const xbString & sFldName, const xbString &sMemoData );
  #endif // XB_MEMO_SUPPORT


  #ifdef XB_LOCKING_SUPPORT
  virtual xbInt16 LockTable         ( xbInt16 iLockFunction );
  virtual xbInt16 LockRecord        ( xbInt16 iLockFunction, xbUInt32 ulRecNo );
  virtual xbInt16 LockAppend        ( xbInt16 iLockFunction );
  virtual xbInt16 LockHeader        ( xbInt16 iLockFunction );

  #ifdef XB_MEMO_SUPPORT
  virtual xbInt16 LockMemo          ( xbInt16 iLockFunction );
  virtual xbBool  GetMemoLocked     () const; 
  #endif  // XB_MEMO_LOCK

  #ifdef XB_INDEX_SUPPORT
  virtual xbInt16 LockIndices       ( xbInt16 iLockFunction );
  #endif // XB_INDEX_SUPPORT

  xbInt16         GetAutoLock       () const;
  void            SetAutoLock       ( xbInt16 iAutoLock );
  xbInt16         GetLockFlavor     () const;
  void            SetLockFlavor     ( xbInt16 iLockFlavor );
  xbBool          GetTableLocked    () const;
  xbBool          GetHeaderLocked   () const;
  xbUInt32        GetAppendLocked   () const;
  xbLinkListNode<xbUInt32> *  GetFirstRecLock   () const;

  #ifdef XB_DEBUG_SUPPORT
  void            DumpTableLockStatus() const;
  #endif  // XB_DEBUG_SUPPORT
  #endif  // XB_LOCKING_SUPPORT


  #ifdef XB_INDEX_SUPPORT
  virtual xbInt16  CheckTagIntegrity( xbInt16 iTagOpt, xbInt16 iOutputOpt );
  virtual xbInt16  CloseIndexFile( xbIx *pIx );
  virtual xbInt16  CreateTag( const xbString &sIxType, const xbString &sName, const xbString &sKey, const xbString &sFilter, 
                   xbInt16 iDescending, xbInt16 iUnique, xbInt16 iOverLay, xbIx **xbIxOut, void **vpTagOut );
  virtual xbInt16  DeleteTag( const xbString &sIxType, const xbString &sName );
  virtual xbInt16  DeleteAllIndexFiles();
  virtual xbInt16  Find( xbString &sKey );
  virtual xbInt16  Find( xbDate   &dtKey );
  virtual xbInt16  Find( xbDouble &dKey );
  virtual xbInt16  Find( xbIx *pIx, void *vpTag, const xbString &sKey );
  virtual xbInt16  Find( xbIx *pIx, void *vpTag, const xbDate   &dtKey );
  virtual xbInt16  Find( xbIx *pIx, void *vpTag, const xbDouble &dKey );
  virtual xbIx     *GetCurIx() const;
  virtual void     *GetCurTag() const;

  virtual xbInt16  GetTagPtrs( const xbString &sTagName,  xbIx **pIx, void **pTag );

  virtual xbInt16  GetFirstKey();
  virtual xbInt16  GetNextKey();
  virtual xbInt16  GetPrevKey();
  virtual xbInt16  GetLastKey();

  virtual xbInt16  GetFirstKey( xbIx *pIx, void *vpTag );
  virtual xbInt16  GetNextKey( xbIx *pIx, void *vpTag );
  virtual xbInt16  GetPrevKey( xbIx *pIx, void *vpTag );
  virtual xbInt16  GetLastKey( xbIx *pIx, void *vpTag );

  virtual const    xbString &GetCurIxType() const;
  virtual const    xbString &GetCurTagName() const;
  virtual xbIxList *GetIxList() const;
  virtual xbInt32  GetPhysicalIxCnt () const;
  xbLinkListNode<xbTag *> *GetTagList   () const;

  virtual xbInt16  OpenIndex( const xbString &sIxType, const xbString &sIndexName );
  virtual xbInt16  Reindex( xbInt16 iTagOpt = 0, xbIx **pIx = NULL, void **vpTag = NULL );
  virtual xbInt16  SetCurTag( const xbString &sTagName );
  virtual void     SetCurTag( const xbString &sIxType, xbIx *pIx, void *vpTag );

  #endif  // XB_INDEX_SUPPORT

  #ifdef XB_INF_SUPPORT
  virtual xbInt16  AssociateIndex( const xbString &sType, const xbString &sName, xbInt16 iOption );
  xbLinkListNode<xbString> *GetInfList() const;
  #endif   // XB_INF_SUPPORT

  //#ifdef XB_MDX_SUPPORT
  //virtual xbInt16  GetCreateMdxBlockSize() const;
  //virtual xbInt16  SetCreateMdxBlockSize( xbInt16 ulBlockSize );
  //#endif

  #ifdef XB_BLOCKREAD_SUPPORT
  xbInt16 DisableBlockReadProcessing();
  xbInt16 EnableBlockReadProcessing();
  xbBool  GetBlockReadStatus() const;
  #endif  // XB_BLOCKREAD_SUPPORT

 protected:
  #ifdef XB_INDEX_SUPPORT
  friend class xbIx;
  friend class xbIxMdx;
  friend class xbIxTdx;

           xbInt16  AddIndex( xbIx *ix, const xbString &sFmt );
           void     ClearTagList();
           xbInt16  RemoveIndex( xbIx * ix );
           void     UpdateSchemaIxFlag( xbInt16 iFldNo, unsigned char cVal );


  virtual  xbInt16  UpdateTagList      ();
  #endif   // XB_INDEX_SUPPORT

  virtual xbInt16  ReadHeader       ( xbInt16 iFilePositionOption, xbInt16 iReadOption );


  #ifdef XB_INF_SUPPORT
  virtual  xbInt16  GetInfFileName( xbString &sNdxIdxFileName );
  #endif   // XB_INF_SUPPORT

  #ifdef XB_BLOCKREAD_SUPPORT
  friend class xbBlockRead;
  #endif // XB_BLOCKREAD_SUPPORT


  #ifdef XB_LOCKING_SUPPORT
  void     SetHeaderLocked   ( xbBool bTableLocked );
  void     SetTableLocked    ( xbBool bTableLocked );
  #endif // XB_LOCKING_SUPORT

  #ifdef XB_MEMO_SUPPORT
  xbInt16  iMemoFieldCnt;                 // Memo field cnt in the table
  xbMemo   *Memo;                         // Pointer to memo class
  xbUInt32 ulCreateMemoBlockSize;         // blocksize to use when creating dbt file
  #endif

  #ifdef XB_INF_SUPPORT
  virtual xbInt16  DeleteInfData();
  virtual xbInt16  LoadInfData();
  virtual xbInt16  SaveInfData();
  #endif // XB_INF_SUPPORT

/*
  #ifdef XB_MDX_SUPPORT
  xbInt16  iCreateMdxBlockSize;
  #endif // XB_MDX_SUPPORT
*/

  virtual  xbInt16  GetRawField( xbInt16 iFieldNo, char *Buf, xbUInt32 BufSize, xbInt16 iRecBufSw ) const;
  virtual  void     InitVars(); 
  virtual  xbInt16  SetVersion() = 0;
  virtual  xbInt16  ValidateSchema( xbSchema * s ) = 0;
  xbInt16  WriteHeader( xbInt16 iPositionOption, xbInt16 iWriteOption );


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


/* Next several variables are database header fields, up through dbase V */
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
  char     cLangDriver;                   // dbase 7 sets to 1B



 private:
  void     ResetNoOfRecords(); 


  #ifdef XB_LOCKING_SUPPORT
  xbInt16  iAutoLock;                     // 0 - autolock off, 1 - autolock on
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
  virtual xbInt16 CreateTable   ( const xbString &sTableName, const xbString &sAlias, xbSchema *, xbInt16 iOverlay, xbInt16 iShareMode );
  virtual xbInt16 GetVersion    () const;
  virtual xbInt16 Open          ( const xbString &sTableName, const xbString &sAlias, xbInt16 iOpenMode, xbInt16 iShareMode );
  virtual xbInt16 Rename        ( const xbString sNewName );

  #ifdef XB_MEMO_SUPPORT
  virtual xbInt16 SetCreateMemoBlockSize( xbUInt32 iBlockSize );
  #endif

 protected:
  xbInt16 SetVersion    ();
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

  virtual xbInt16 CreateTable     ( const xbString &sTableName, const xbString &sAlias, xbSchema *, xbInt16 iOverlay, xbInt16 iShareMode );
  virtual xbInt16 GetVersion      () const;
  virtual xbInt16 Open            ( const xbString &sTableName, const xbString &sAlias, xbInt16 iOpenMode, xbInt16 iShareMode );
  virtual xbInt16 Rename          ( const xbString sNewName );

  #ifdef XB_MEMO_SUPPORT
  virtual xbInt16 SetCreateMemoBlockSize( xbUInt32 iBlockSize );
  #endif

 protected:
  // void    InitVars  ();
  xbInt16 SetVersion();
  virtual xbInt16 ValidateSchema  ( xbSchema *s );

 private:

};

#endif   /* XB_DBF4_SUPPORT */

}        /* namespace xb    */
#endif   /* __XB_DBF_H__ */
