/*  xbindex.h

XBase64 Software Library

Copyright (c) 1997,2003,2014, 2018, 2022 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/


#ifndef __XB_INDEX_H__
#define __XB_INDEX_H__

#ifdef XB_INDEX_SUPPORT


#define XB_ADD_KEY 1
#define XB_UPD_KEY 2
#define XB_DEL_KEY 3

namespace xb{



///@cond DOXYOFF
// structure for index nodes, each node contains information regarding one block
struct XBDLLEXPORT xbIxNode {
  xbIxNode *npPrev;                     // pointer to previous node in chain
  xbIxNode *npNext;                     // pointer to next node in chain
  xbUInt32 iCurKeyNo;                   // current key number in the node, 0 offset
  xbUInt32 ulBlockNo;                   // this block number
  xbUInt32 ulBufSize;                   // size of cpBlockData
  char     *cpBlockData;                // pointer to memory version of block data stored in file
};
///@endcond DOXYOFF


//! @brief Base class for handling dbf indices.
/*!

The xbIx class is used as a base class for accessing index files.
Each index file can have one or more tags.

Calls to the index routines to perform index updates are handled automatically by the dbf class. 
The application program does not need to be concerned with index updates.

If there is a production MDX index, it is opened automatically when the dbf file is opened.
If there is an ndx file, that has been associated with the dbf file with the metadata routines,
it will be opened automatically when the dbf file is opened.
If there are non prod ndx indices that are not associated with the dbf file, the application
program will need to open as appropriate.
The meta data association logic is specific to the Xbase64 library and is not applicable to
other available tools that handle ndx indices.
All index files are automatically closed when the dbf file is closed.


<br>
The class is designed to support the addition of additional indices with a minimal amount of effort
needed to integrate into the library.
If you are looking at adding an new index type to the library, create a derived class using xbIx as a 
base class and modify methods needed to support the new index file version. 
The xbDbf class (and derived classes) perform the needed calls to the index routines for updates.<br>
See the following for examples on how to start on this:<br>
xbIxNdx is a derived class and supports a single tag.<br>
xbIxMdx is a derived class and supports multiple tags.<br>



<br>
How data fields are stored in index files:
<table>
<tr><th>Field Type<th>Stored in DBF as<th>Stored in NDX as<th>Stored in MDX as</tr>
<tr><td>C<td>char<td>char<td>char
<tr><td>F<td>text numbers<td>xbDouble<td>xbBcd
<tr><td>N<td>text numbers<td>xbDouble<td>xbBcd
<tr><td>D<td>text CCYYMMDD<td>xbDouble - julian<td>xbDouble - julian
</table>


<br>
Pages Vs Blocks
<br>
For purposes of the Xbase index classes, a page is considered to be 512 bytes of data
and a block is made up of one or more 512 byte pages.
<br>Default block sixe of NDX indices is one 512 byte page.
<br>Default block size of MDX indices is two 512 byte pages or 1024 bytes.

<br>The WriteBlock and GetBlock functions calculate the physical position in the 
file based on a combination of Block Number and Block Size.  Block size is set at
time of index file creation, default is 1024 or two pages.

<br>Page numbers are stored in the physical file, but block reads and writes 
are performed.

<br>
Duplicate Keys
<br>
With the original DBase unique indexing option, if a table has multiple records with the
same key value, DBase would allow multiple records in the table, but only the first
record would be found in the index.
<br>
XBase64 can be configured to support the original DBase duplicate key implementation,
or can be configured to halt with a DUPLICATE_KEY error on the insertion of a record
with a duplicate key.
<br>
<table>
<tr><th>Option<th>Description</tr>
<tr><td>XB_HALT_ON_DUPKEY</td><td>Return error XB_KEY_NOT_UNIQUE when attempting to append record with duplicate key</td></tr>
<tr><td>XB_EMULATE_DBASE</td><td>Emulate DBase, allow duplicate records with the same key, only the first record is indexed</td></tr>
</table>
*/


class XBDLLEXPORT xbIx : public xbFile {
 public:
   xbIx( xbDbf * d );
   virtual ~xbIx();

   virtual xbInt16  CheckTagIntegrity( void *vpTag, xbInt16 iOpt ) = 0;
   virtual xbInt16  Close();
   virtual xbInt16  CreateTag( const xbString &sName, const xbString &sKey, const xbString &sFilter, xbInt16 iDescending, xbInt16 iUnique, xbInt16 iOverlay, void **vpTag ) = 0;
   virtual xbInt16  FindKey( void *vpTag, const xbString &sKey, xbInt16 iRetrieveSw );
   virtual xbInt16  FindKey( void *vpTag, const char * cKey, xbInt32 lKeyLen, xbInt16 iRetrieveSw );
   virtual xbInt16  FindKey( void *vpTag, const xbBcd &bcd, xbInt16 iRetrieveSw );
   virtual xbInt16  FindKey( void *vpTag, const xbDate &dtKey, xbInt16 iRetrieveSw );
   virtual xbInt16  FindKey( void *vpTag, xbDouble dKey, xbInt16 iRetrieveSw );
   virtual xbInt16  FindKey( void *vpTag, const void *vKey, xbInt32 lKeyLen, xbInt16 iRetrieveSw ) = 0;
   virtual void     *GetCurTag() const;

   virtual xbInt16  GetCurKeyVal( void *vpTag, xbString &s ) = 0;
   virtual xbInt16  GetCurKeyVal( void *vpTag, xbDouble &d ) = 0;
   virtual xbInt16  GetCurKeyVal( void *vpTag, xbDate &dt ) = 0;

   virtual xbDbf    *GetDbf() const;

   virtual xbString &GetKeyExpression( const void *vpTag ) const = 0;
   virtual xbString &GetKeyFilter( const void *vpTag ) const = 0;
   virtual char     GetKeyType( const void *vpTag ) const = 0;
   virtual xbBool   GetLocked() const;

   virtual xbInt16  GetFirstKey( void *vpTag, xbInt16 iRetrieveSw ) = 0;
   virtual xbInt16  GetFirstKey( void *vpTag );
   virtual xbInt16  GetFirstKey();

   virtual xbInt16  GetLastKey( void *vpTag, xbInt16 lRetrieveSw ) = 0;
   virtual xbInt16  GetLastKey( void *vpTag );
   virtual xbInt16  GetLastKey();

   virtual xbInt16  GetNextKey( void *vpTag, xbInt16 iRetrieveSw ) = 0;
   virtual xbInt16  GetNextKey( void *vpTag );
   virtual xbInt16  GetNextKey();

   virtual xbInt16  GetPrevKey( void *vpTag, xbInt16 iRetrieveSw ) = 0;
   virtual xbInt16  GetPrevKey( void *vpTag );
   virtual xbInt16  GetPrevKey();

   virtual void     *GetTag( xbInt16 iTagNo ) const = 0;
   virtual void     *GetTag( xbString &sTagName ) const = 0;
   virtual xbInt16  GetTagCount() const = 0;

   virtual xbString &GetTagName( void *vpTag ) const = 0;
   virtual const char * GetTagName( void *vpTag, xbInt16 iOpt ) const = 0;
   virtual void     GetTagName( void *vpTag, xbString &sTagName ) {};
   virtual const    xbString &GetType() const;

   virtual xbBool   GetUnique( void *vpTag ) const = 0;
   virtual xbBool   GetSortOrder( void *vpTag ) const = 0;

   virtual xbInt16  Open( const xbString &sFileName );
   virtual xbInt16  SetCurTag( xbInt16 iTagNo ) = 0;
   virtual xbInt16  SetCurTag( xbString &sTagName ) = 0;
   virtual void     SetCurTag( void * vpCurTag );
   virtual void     SetLocked( xbBool bLocked );

   virtual void     TestStub( char *s, void *vpTag ) {};

   // virtual xbInt16  ReindexTag( void **vpTag ) = 0;


   #ifdef XB_DEBUG_SUPPORT
   virtual xbInt16 DumpFreeBlocks( xbInt16 iOpt = 0 ) { return XB_NO_ERROR; }
   virtual xbInt16 DumpHeader( xbInt16 iDestOpt = 0, xbInt16 iFmtOpt = 0 ) = 0;
   virtual xbInt16 DumpIxForTag( void *vpTag, xbInt16 iOutputOpt ) = 0;
   virtual void    DumpIxNodeChain( void *vpTag, xbInt16 iOutputOpt ) const = 0;
   virtual xbInt16 DumpNode( void * vpTag, xbIxNode * pNode, xbInt16 iOption ) const;
   virtual xbInt16 DumpTagBlocks( xbInt16 iOpt = 1, void *vpTag = NULL  ) = 0;
   #endif

 protected:
   friend  class    xbDbf;

   virtual xbInt16  AddKey( void *vpTag, xbUInt32 ulRecNo ) = 0;
   virtual xbInt16  AddKeys( xbUInt32 ulRecNo );
   virtual xbIxNode *AllocateIxNode( xbUInt32 ulBufSize = 0, xbInt16 iOption = 0 );
   virtual xbInt16  BSearchBlock( char cKeyType, xbIxNode *npNode, xbInt32 lKeyLen, const void *vpKey, xbInt32 lSearchKeyLen, xbInt16 &iCompRc, xbBool bDescending = xbFalse ) const;
   virtual xbInt16  CheckForDupKeys();
   virtual xbInt16  CheckForDupKey( void *vpTag ) = 0;
   virtual xbInt16  CompareKey( char cKeyType, const void *v1, const void *v2, size_t lKeyLen ) const;
   virtual xbInt16  CreateKeys( xbInt16 iOpt );
   virtual xbInt16  CreateKey( void * vpTag, xbInt16 iOpt ) = 0;
   virtual xbInt16  DeleteFromNode( void *vpTag, xbIxNode * npNode, xbInt16 iSlotNo ) = 0;
   virtual xbInt16  DeleteKeys();
   virtual xbInt16  DeleteKey( void *vpTag ) = 0;
   virtual xbInt16  DeleteTag( void *vpTag ) = 0;

   virtual xbInt16  FindKeyForCurRec( void *vpTag ) = 0;
   virtual xbIxNode *FreeNodeChain( xbIxNode *np );
   virtual xbInt16  GetBlock( void *vpTag, xbUInt32 ulBlockNo, xbInt16 iOpt, xbUInt32 ulAddlBuf = 0 );
   virtual xbInt32  GetKeyCount( xbIxNode *npNode ) const;
   virtual char     *GetKeyData( xbIxNode *npNode, xbInt16 iKeyNo, xbInt16 iKeyItemLen ) const;
   virtual xbInt16  GetKeySts( void *vpTag ) const = 0;
   virtual xbInt16  GetLastKey( xbUInt32 ulNodeNo, void *vpTag, xbInt16 lRetrieveSw ) = 0;
   virtual xbInt16  InsertNodeL( void *vpTag, xbIxNode * npNode, xbInt16 iSlotNo, char * cpKeyBuf, xbUInt32 uiPtr ) = 0;
   virtual xbInt16  InsertNodeI( void *vpTag, xbIxNode * npNode, xbInt16 iSlotNo, xbUInt32 uiPtr ) = 0;
   virtual xbInt16  KeyExists( void * ) = 0;
   virtual void     NodeFree( xbIxNode * ixNode );
   virtual xbInt16  ReadHeadBlock( xbInt16 iOpt = 0 ) = 0;
   virtual xbInt16  ReindexTag( void **vpTag ) = 0;
   virtual void     SetDbf( xbDbf *dbf );
   virtual xbInt16  SplitNodeL( void *vpTag, xbIxNode * npLeft, xbIxNode *npRight, xbInt16 iSlotNo, char *cpKeyBuf, xbUInt32 uiPtr ) = 0;
   virtual xbInt16  SplitNodeI( void *vpTag, xbIxNode * npLeft, xbIxNode *npRight, xbInt16 iSlotNo, xbUInt32 uiPtr ) = 0;
   virtual xbInt16  UpdateTagKey( char cAction, void *vpTag, xbUInt32 ulRecNo = 0 ) = 0;
   virtual xbInt16  WriteHeadBlock( xbInt16 iOption ) = 0;

   xbDbf    *dbf;
   char     *cNodeBuf;               // pointer to memory for processing in a block of index data
   void     *vpCurTag;               // pointer to active tag.  Single index files have only one tag
   xbString sIxType;


 private:
   virtual void     AppendNodeChain( void *vpTag, xbIxNode *npNode ) = 0;
   virtual xbInt16  GetKeyPtr( void *vpTag, xbInt16 iKeyNo, xbIxNode *npNode, xbUInt32 &ulKeyPtr ) const = 0;
   virtual xbBool   IsLeaf( void *vpTag, xbIxNode *npNode ) const = 0;
   // virtual void     SetCurNode( void *vpTag, xbIxNode *npNode ) = 0;

   xbBool   bLocked;               // index file locked?
};

#ifdef XB_NDX_SUPPORT

#define XB_NDX_BLOCK_SIZE 512


///@cond DOXYOFF
struct XBDLLEXPORT xbNdxTag {

   // NDX File Header Fields
   xbUInt32 ulRootBlock;     // header node is 0
   xbUInt32 ulTotalBlocks;   // includes header node
   char     cKeyType;        // C = Char, F = Numeric, D = Date
   xbInt16  iKeyLen;         // length of key data
   xbInt16  iKeysPerBlock;   // max number keys per block  <=100
   xbInt16  iKeyType;        // 00 = Char, 01 = Numeric
   xbInt16  iKeyItemLen;     // KeyLen + 8 bytes
   char     cSerNo;          // rolling incrementing serial number +1 on each index update
   xbInt16  iUnique;         // True if unique
   xbString sKeyExpression;  // index expression
   // end of NDX Header field

   xbExp     *exp;           // pointer to expression for expression keys
   time_t    tNodeChainTs;   // node chain time stamp
   xbIxNode  *npNodeChain;
   xbIxNode  *npCurNode;
   char      *cpKeyBuf;     // key buffer, for searches and adds
   char      *cpKeyBuf2;    // key buffer, for deletes
   xbString  sTagName;      // tag name - is the file name without the extension
   xbBool    bFoundSts;     // key found?  used to determine if new key should be added in XB_EMULATE_DBASE mode in AddKey


   xbInt16   iKeySts;       // key updated?  set in method CreateKey, checked in AddKey and DeleteKey routines
                            //   old key filtered     new key filtered   iKeySts
                            //       Y                      Y              XB_UPD_KEY  2 - update key if changed (delete and add)
                            //       Y                      N              XB_DEL_KEY  3 - delete key
                            //       N                      Y              XB_ADD_KEY  1 - add key
                            //       N                      N                          0 - no update

};
///@endcond DOXYOFF

//! @brief Class for handling NDX single tag indices.
/*!

The xbIxNdx class is derived from the xbIx base class and is specific to handling NDX single tag index files.
Each NDX index file can have only one tag, but the methods are set up to take an argument for a specific tag.
This was done in order to provide a consistant interface across index types.

Calls to the ndx index routines to perform index updates are handled automatically be the dbf class after
the ndx file has been opened.

Xbase64 provides a mechanism to automatically open ndx files when a dbf file is opened.
If the ndx file has been associated with the dbf file with the metadata routines,
it will be opened automatically when the dbf file is opened.
If there are non prod ndx indices that are not associated with the dbf file, the application
program will need to open as appropriate.
The meta data association logic is specific to the Xbase64 library and is not applicable to
other available tools that handle ndx indices.

*/

class XBDLLEXPORT xbIxNdx : public xbIx {
 public:
   xbIxNdx( xbDbf * d );
   ~xbIxNdx();
   xbInt16  CheckTagIntegrity( void *vpTag, xbInt16 iOpt );
   xbInt16  CreateTag( const xbString &sName, const xbString &sKey, const xbString &sFilter, xbInt16 iDescending, xbInt16 iUnique, xbInt16 iOverlay, void **vpTag );
   xbInt16  FindKey( void *vpTag, const void *vpKey, xbInt32 lKeyLen, xbInt16 iRetrieveSw );


   xbInt16  GetCurKeyVal( void *vpTag, xbString &s );
   xbInt16  GetCurKeyVal( void *vpTag, xbDouble &d );
   xbInt16  GetCurKeyVal( void *vpTag, xbDate &dt );



   xbInt16  GetFirstKey( void *vpTag, xbInt16 iRetrieveSw );

   xbInt16  GetLastKey( void *vpTag, xbInt16 iRetrieveSw = 1 );
   xbInt16  GetNextKey( void *vpTag, xbInt16 iRetrieveSw = 1 );
   xbInt16  GetPrevKey( void *vpTag, xbInt16 iRetrieveSw = 1 );
   xbInt32  GetKeyLen  ( const void *vpTag ) const;
   char     GetKeyType ( const void *vpTag ) const;
   xbString &GetKeyExpression( const void *vpTag ) const;
   xbString &GetKeyFilter( const void *vpTag ) const;
   void     *GetTag( xbInt16 iTagNo ) const;
   void     *GetTag( xbString &sTagName ) const;
   xbString &GetTagName( void *vpTag ) const;
   const char * GetTagName( void *vpTag, xbInt16 iOpt ) const;
   xbInt16  GetTagCount() const;

   xbBool   GetUnique( void *vpTag = NULL ) const;
   xbBool   GetSortOrder( void *vpTag ) const;
   xbInt16  SetCurTag( xbInt16 iTagNo );
   xbInt16  SetCurTag( xbString &sTagName );

   #ifdef XB_DEBUG_SUPPORT
   xbInt16 DumpTagBlocks( xbInt16 iOpt = 1, void *vpTag = NULL  );
   xbInt16 DumpHeader( xbInt16 iOpt = 0, xbInt16 iFmt = 0 );
   xbInt16 DumpIxForTag( void *vpTag, xbInt16 iOutputOpt );
   void    DumpIxNodeChain( void *vpTag, xbInt16 iOutputOpt ) const;
   xbInt16 DumpNode( void * vpTag, xbIxNode * pNode, xbInt16 iOption ) const;
   #endif

   // xbInt16  ReindexTag( void **vpTag );
 

 protected:
   friend   class xbDbf;
   xbInt16  AddKey( void *vpTag, xbUInt32 ulRecNo );
   xbIxNode *AllocateIxNode( xbUInt32 ulBufSize = 0, xbInt16 iOption = 0 );
   xbInt16  CheckForDupKey( void *vpTag );
   xbIxNode *CreateIxNode( xbUInt32 ulBufSize );
   xbInt16  CreateKey( void * vpTag, xbInt16 iOpt );
   xbInt16  DeleteFromNode( void *vpTag, xbIxNode * npNode, xbInt16 iSlotNo );
   xbInt16  DeleteKey( void *vpTag );
   xbInt16  DeleteTag( void *vpTag );
   xbInt16  FindKeyForCurRec( void *vpTag );
   xbInt16  GetKeyTypeN( const void *vpTag ) const;
   xbInt16  GetKeySts( void *vpTag ) const;
   xbInt16  GetLastKey( xbUInt32 ulNodeNo, void *vpTag, xbInt16 iRetrieveSw = 1 );
   xbInt16  InsertNodeI( void *vpTag, xbIxNode * npNode, xbInt16 iSlotNo, xbUInt32 uiPtr );
   xbInt16  InsertNodeL( void *vpTag, xbIxNode * npNode, xbInt16 iSlotNo, char * cpKeyBuf, xbUInt32 uiPtr );
   xbInt16  KeyExists( void *vpTag = NULL );
   xbInt16  ReadHeadBlock(xbInt16 iOpt);   // read the header node of the disk NDX file
   xbInt16  ReindexTag( void **vpTag );
   xbInt16  SplitNodeI( void *vpTag, xbIxNode * npLeft, xbIxNode *npRight, xbInt16 iSlotNo, xbUInt32 uiPtr );
   xbInt16  SplitNodeL( void *vpTag, xbIxNode * npLeft, xbIxNode *npRight, xbInt16 iSlotNo, char *cpKeyBuf, xbUInt32 uiPtr );
   xbInt16  UpdateTagKey( char cAction, void *vpTag, xbUInt32 ulRecNo = 0 );
   xbInt16  WriteHeadBlock( xbInt16 iOption );

 private:
   xbInt16  AddKeyNewRoot( xbNdxTag *npTag, xbIxNode *npLeft, xbIxNode *npRight );
   void     AppendNodeChain( void *vpTag, xbIxNode *npNode );
   xbInt16  GetDbfPtr( void *vpTag, xbInt16 iKeyNo, xbIxNode *npNode, xbUInt32 &ulDbfPtr ) const;
   xbInt16  GetKeyPtr( void *vpTag, xbInt16 iKeyNo, xbIxNode *npNode, xbUInt32 &ulKeyPtr ) const;
   xbInt16  GetLastKeyForBlockNo( void *vpTag, xbUInt32 ulBlockNo, char *cpKeyBuf );
   xbBool   IsLeaf( void *vpTag, xbIxNode *npNode ) const;
   xbInt16  KeySetPosAdd( xbNdxTag *npTag, xbUInt32 ulAddKeyRecNo );
   xbInt16  KeySetPosDel( xbNdxTag *npTag );
   xbNdxTag *ndxTag;
};

#endif   /* XB_NDX_SUPPORT */


#ifdef XB_MDX_SUPPORT
//#define XB_MDX_BLOCK_SIZE 1024

struct XBDLLEXPORT xbMdxTag {

  // next 7 fields comprise the tag table entry
  xbUInt32      ulTagHdrPageNo;   // 512 byte page number, NOT block number
  char          cTagName[11];
  char          cKeyFmt;          // always 0x10 w/ DBase V7
  char          cLeftChild;       // cFwdTagThread
  char          cRightChild;      // cFwdTagThread2
  char          cParent;          // cBwdTagThread
  char          c2;
  char          cKeyType;         // C,D,N

  xbUInt32      ulRootPage;       // 512 byte page number, NOT block number
  xbUInt32      ulTagSize;        // Number of 512 byte pages allocated to the tag.   Tag size of two is a single 1024 block

  char          cKeyFmt2;         // 0x10 - base
                                  // 0x08 - descending
                                  // 0x40 - unique
  char          cKeyType2;
  // one unused byte fits here

  char          cTag11;          // dbase sets to 0x1B
  xbInt16       iKeyLen;
  xbInt16       iKeysPerBlock;
  xbInt16       iSecKeyType;
  xbInt16       iKeyItemLen;    // iKeyLen + 4

  char          cSerialNo;      // Increments +1 for each tag update
  char          cUnique;
  xbString      *sKeyExp;       // Key expression
  char          cHasFilter;     // 0x00 or 0x01
  char          cHasKeys;       // 0x00 or 0x01
  xbUInt32      ulLeftChild;    // dbase 7 sets this to the root page on tag creation
  xbUInt32      ulRightChild;   // dbase 7 sets this to the root page on tag creation

  char          cTagYY;
  char          cTagMM;
  char          cTagDD;

  char          cKeyFmt3;      // dbase 7 sets this 0x01 if discreet field or 0x00 if calculated or combination field key expression on tag creation

  xbString      *sFiltExp;     // Filter expression

  time_t        tNodeChainTs;
  xbIxNode      *npNodeChain;
  xbIxNode      *npCurNode;
  xbExp         *exp;          // pointer to expression for expression based tags
  xbExp         *filter;       // pointer to expression for index filter

  char          *cpKeyBuf;     // key buffer
  char          *cpKeyBuf2;    // key buffer
  xbString      *sTagName;     // string tag name


  xbMdxTag      *next;
  xbBool        bFoundSts;     // key found?  used to determine if new key should be added in XB_EMULATE_DBASE mode in AddKey


  xbInt16       iKeySts;       // key updated?  set in method CreateKey, checked in AddKey and DeleteKey routines
                               //   old key filtered     new key filtered   iKeySts
                               //       Y                      Y              XB_UPD_KEY  2 - update key if changed (delete and add)
                               //       Y                      N              XB_DEL_KEY  3 - delete key
                               //       N                      Y              XB_ADD_KEY  1 - add key
                               //       N                      N                          0 - no update


};


class XBDLLEXPORT xbIxMdx : public xbIx {
 public:
   xbIxMdx( xbDbf * d );
   ~xbIxMdx();
   virtual xbInt16  CheckTagIntegrity( void *vpTag, xbInt16 iOpt );
   virtual xbInt16  CreateTag( const xbString &sName, const xbString &sKey, const xbString &sFilter, xbInt16 iDescending, xbInt16 iUnique, xbInt16 iOverlay, void **vpTag );
   virtual xbInt16  FindKey( void *vpTag, const void *vKey, xbInt32 lKeyLen, xbInt16 iRetrieveSw );
   virtual xbInt16  FindKey( void *vpTag, xbDouble dKey, xbInt16 iRetrieveSw );

   virtual xbInt16  GetCurKeyVal( void *vpTag, xbString &s );
   virtual xbInt16  GetCurKeyVal( void *vpTag, xbDouble &d );
   virtual xbInt16  GetCurKeyVal( void *vpTag, xbDate &dt );

   virtual xbInt16  GetFirstKey( void *vpTag, xbInt16 lRetrieveSw );
   virtual xbString &GetKeyExpression( const void *vpTag ) const;
   virtual xbString &GetKeyFilter( const void *vpTag ) const;
   virtual char     GetKeyType( const void *vpTag ) const;
   virtual xbInt16  GetLastKey( void *vpTag, xbInt16 lRetrieveSw );
   virtual xbInt16  GetNextKey( void *vpTag, xbInt16 lRetrieveSw );
   virtual xbInt16  GetPrevKey( void *vpTag, xbInt16 lRetrieveSw );
   virtual xbBool   GetReuseEmptyNodesSw() const;
   virtual xbBool   GetSortOrder( void *vpTag ) const;
   virtual void     *GetTag( xbInt16 iTagNo ) const;
   virtual void     *GetTag( xbString &sTagName ) const;
   virtual xbInt16  GetTagCount() const;

   virtual xbString &GetTagName( void *vpTag ) const;
   virtual const char * GetTagName( void *vpTag, xbInt16 iOpt ) const;
   virtual void     GetTagName( void *vpTag, xbString &sTagName );

   virtual xbInt16  GetUnique( void *vpTag ) const;
   virtual xbInt16  SetCurTag( xbInt16 iTagNo );
   virtual xbInt16  SetCurTag( xbString &sTagName );
           void     SetReuseEmptyNodesSw( xbBool bReuse );

           void     TestStub( char *s, void *vpTag );


   //virtual xbInt16  ReindexTag( void **vpTag );


 protected:
   friend   class xbDbf;

   xbInt16  AddKey( void *vpTag, xbUInt32 ulRecNo );
   xbIxNode *AllocateIxNode( xbMdxTag * mpTag, xbUInt32 ulBufSize, xbUInt32 ulBlock2 );
   void     CalcBtreePointers();
   char     CalcTagKeyFmt( xbExp &exp );

   xbInt16  CheckForDupKey( void *vpTag );
   virtual  xbInt16  Close();
   xbInt16  CreateKey( void * vpTag, xbInt16 iOpt );
   xbInt16  DeleteFromNode( void *vpTag, xbIxNode * npNode, xbInt16 iSlotNo );
   xbInt16  DeleteKey( void *vpTag );

   virtual  xbInt16  DeleteTag( void *vpTag );

   xbInt16  FindKeyForCurRec( void *vpTag );
   xbInt16  GetKeySts( void *vpTag ) const;
   xbInt16  GetLastKey( xbUInt32 ulBlockNo, void *vpTag, xbInt16 lRetrieveSw );
   void     *GetTagTblPtr() const;

   xbInt16  HarvestTagNodes( xbMdxTag *mpTag, xbBool bRecycleRoot = xbFalse );
   void     Init( xbInt16 iOpt = 0 );
   xbInt16  InsertNodeI( void *vpTag, xbIxNode *npNode, xbInt16 iSlotNo, xbUInt32 uiPtr );
   xbInt16  InsertNodeL( void *vpTag, xbIxNode *npNode, xbInt16 iSlotNo, char *cpKeyBuf, xbUInt32 uiPtr );
   xbInt16  KeyExists( void * );
   xbInt16  LoadTagTable();
   xbInt16  ReadHeadBlock(xbInt16 iOpt);   // read the header node of the disk file
   virtual  xbInt16  ReindexTag( void **vpTag );
   xbInt16  SplitNodeI( void *vpTag, xbIxNode * npLeft, xbIxNode *npRight, xbInt16 iSlotNo, xbUInt32 uiPtr );
   xbInt16  SplitNodeL( void *vpTag, xbIxNode * npLeft, xbIxNode *npRight, xbInt16 iSlotNo, char *cpKeyBuf, xbUInt32 uiPtr );
   xbInt16  UpdateTagKey( char cAction, void *vpTag, xbUInt32 ulRecNo = 0 );
   xbInt16  WriteHeadBlock( xbInt16 iOption );

   #ifdef XB_DEBUG_SUPPORT
   xbInt16  DumpTagBlocks( xbInt16 iOpt = 1, void *vpTag = NULL  );
   xbInt16  DumpFreeBlocks( xbInt16 iOpt = 0 );
   xbInt16  DumpHeader( xbInt16 iOpt = 0, xbInt16 iFmtOpt = 0 );
   xbInt16  DumpIxForTag( void *vpTag, xbInt16 iOutputOpt );
   void     DumpIxNodeChain( void *vpTag, xbInt16 iOutputOpt ) const;
   #endif


   xbMdxTag *mdxTagTbl;

// MDX File Header Fields
   char     cVersion;
   char     cCreateYY;
   char     cCreateMM;
   char     cCreateDD;
   xbString sFileName;
   xbInt16  iBlockFactor;          // 1-32 #of 512 byte segments in a block

   // use file version
   // xbInt16  iBlockSize;         // Stored at the xbFile level

   char     cProdIxFlag;
   char     cTagEntryCnt;
   xbInt16  iTagLen;
   xbInt16  iTagUseCnt;
   char     cNextTag;             // byte 28 +1
   char     c1B;                  // always 0x1B
   xbUInt32 ulPageCnt;            // number of 512 byte pages in the mdx file
   xbUInt32 ulFirstFreePage;      // page number corresponding to the next free block
   xbUInt32 ulNoOfBlockAvail;     // might be improperly named??  not sure how it is used
   char     cUpdateYY;
   char     cUpdateMM;
   char     cUpdateDD;
   // end of MDX Header fields


   xbBool    bReuseEmptyNodes;     // Reuese empty MDX nodes when all keys deleted?
                                   // DBase 7.x and MS ODBC drivers do not reuse empty nodes, leaves them stranded in the file
                                   // Codebase 6.x reuses empty nodes.
                                   // Setting this to True will reuse empty nodes in the same manner Codebase 6.x reuses them.


 private:
   xbInt16  AddKeyNewRoot( xbMdxTag *npTag, xbIxNode *npLeft, xbIxNode *npRight );
   void     AppendNodeChain( void *vpTag, xbIxNode *npNode );
   xbUInt32 BlockToPage( xbUInt32 ulBlockNo );
   xbMdxTag *ClearTagTable();
   xbInt16  DumpBlock( xbInt16 iOpt, xbUInt32 ulBlockNo, xbMdxTag * mpTag );
   xbInt16  GetDbfPtr( void *vpTag, xbInt16 iKeyNo, xbIxNode *npNode, xbUInt32 &ulDbfPtr ) const;
   xbInt16  GetKeyPtr( void *vpTag, xbInt16 iKeyNo, xbIxNode *npNode, xbUInt32 &ulKeyPtr ) const;
   xbInt16  GetLastKeyForBlockNo( void *vpTag, xbUInt32 ulBlockNo, char *cpKeyBuf );
   xbInt16  HarvestEmptyNode( xbMdxTag *mpTag, xbIxNode *npNode, xbInt16 iOpt, xbBool bHarvestRoot = xbFalse );
   xbBool   IsLeaf( void *vpTag, xbIxNode *npNode ) const;
   xbInt16  KeySetPosAdd( xbMdxTag *mpTag, xbUInt32 ulAddKeyRecNo );
   xbInt16  KeySetPosDel( xbMdxTag *mpTag );
   xbInt16  LoadTagDetail( xbInt16 iOption, xbMdxTag *tte );
   xbUInt32 PageToBlock( xbUInt32 ulPageNo );
   xbInt16  TagSerialNo( xbInt16 iOption, xbMdxTag *mpTag );
   xbInt16  UpdateTagSize( xbMdxTag *mpTag, xbUInt32 ulTagSz );

   #ifdef XB_DEBUG_SUPPORT
   xbInt16  PrintKey( void *vpTag, xbIxNode *npNode, xbInt16 iKeyNo, xbInt16 iDepth, char cType, xbInt16 iOutputOpt );
   #endif


    };
   #endif   /* XB_MDX_SUPPORT */


#ifdef XB_TDX_SUPPORT

class XBDLLEXPORT xbIxTdx : public xbIxMdx {
 public:
   xbIxTdx( xbDbf * d );
   ~xbIxTdx();

   xbInt16  CreateTag( const xbString &sName, const xbString &sKey, const xbString &sFilter, xbInt16 iDescending, xbInt16 iUnique, xbInt16 iOverlay, void **vpTag );

 protected:
   friend   class xbDbf;
   xbInt16  Close();
   xbInt16  DeleteTag( void *vpTag );

 private:
};

#endif   /* XB_TDX_SUPPORT  */


  }         /* namespace xb     */
 #endif     /* XB_INDEX_SUPPORT */
#endif      /* __XB_INDEX_H__   */
