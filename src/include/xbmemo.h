/*  xbmemo.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023,2024 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/


#ifndef __XB_XBMEMO_H__
#define __XB_XBMEMO_H__


// dont use "#pragma interface" with abstract classes
// #ifdef  CMAKE_COMPILER_IS_GNUCC
// #pragma interface
// #endif

#ifdef XB_MEMO_SUPPORT




namespace xb{

//! @brief Base class for memo files.
/*!

The xbMemo class is used as a base class for accessing memo files.

Any DBF file which has any memo fields, will also have a DBT file with the same name.
Memo fields are stored in the DBT file in 512 or 1024 byte sized blocks.

Calls to the memo routines to read and access memo fields are supported from within 
the dbf class via the GetMemoField and UpdateMemoField routines.
From an application program point of view, the application program does not need to 
be concerned with the memo classes.<br>

The main difference between the version 3 and version 4 memo files is that 
the version 4 files will reclaim and reuse blocks if available. The version 3
memo file logic does not.<br>

If you are adding a new style memo table to the logic, you can create a new class
modeled after xbMemoDbt3 or xbDbtMemo4, using the xbMemo class as a base class.

*/

class XBDLLEXPORT xbMemo : public xbFile {
 public:

  //! @brief Class Constructor.
  /*!
     @param dbf Pointer to dbf construct.
     @param sFileName Memo file name.
  */
  xbMemo( xbDbf *dbf, xbString const &sFileName );

  /* virtual methods */
  //! @brief Class Destructor.
  virtual ~xbMemo();
  virtual xbInt16 Abort            () = 0;

  //! @brief Close the memo file.
  /*!
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 CloseMemoFile    ();
  virtual xbInt16 Commit           () = 0;
  virtual xbInt16 CreateMemoFile   () = 0;
  
  virtual xbInt16 DumpMemoHeader   () = 0;


  virtual xbInt16 GetMemoField     ( xbInt16 iFieldNo, xbString &sMemoData ) = 0;
  virtual xbInt16 GetMemoFieldLen  ( xbInt16 iFieldNo, xbUInt32 &ulMemoFieldLen ) = 0;
  virtual xbInt16 OpenMemoFile     () = 0;

  //! @brief Get memo file type.
  /*!
    @returns 3 - Version 3 memo file.<br>
             4 - Version 4 memo file.
  */
  virtual xbInt16 GetMemoFileType  ();


  virtual xbInt16 PackMemo         ( void (*memoStatusFunc)(xbUInt32 ulItemNum, xbUInt32 ulNumItems)) = 0;
  virtual xbInt16 UpdateMemoField  ( xbInt16 iFieldNo, const xbString &sMemoData ) = 0;

  #ifdef XB_DEBUG_SUPPORT
  virtual xbInt16 DumpMemoFreeChain() = 0;
  #endif

  #ifdef XB_LOCKING_SUPPORT
  //! @brief Lock memo file
  /*!
    @param iLockFunction XB_LOCK<br>XB_UNLOCK
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 LockMemo         ( xbInt16 iLockFunction );

  //! @brief Get memo file lock status.
  /*!
    @returns xbTrue - Memo file is locked.<br>
             xbFalse - Memo file is not locked.
  */
  virtual xbBool  GetMemoLocked    () const;
  #endif

  // *********** FIXME *************
  // next two methods should be protected but are called in the xb_test_xxxx programs testing routines
  // so for now, they are kept as public

  //! @brief Calculate the last data block number.
  /*!
    @param ulLastDataBlock Output - Last used block number in the file.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 CalcLastDataBlock( xbUInt32 &ulLastDataBlock );

  //! @brief Get next block available from file header.
  /*!
    @param ulBlockNo Output - Next block number for appending data to memo file.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 GetHdrNextBlock    ( xbUInt32 &ulBlockNo );

 protected:
  friend  class   xbDbf;

  virtual xbInt16 ReadDbtHeader     ( xbInt16 iOption ) = 0;
  virtual xbInt16 UpdateHeaderName  () = 0;

  //! @brief Update Next Node number in file header
  /*!
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 UpdateHeadNextNode();

  virtual xbInt16 Zap            () = 0;

  xbDbf    *dbf;                 /* pointer to related dbf instance                                   */
  char     cVersion;             /* byte 16 off the header block, 0x03 for V3, version IV - ?         */
  xbInt16  iMemoFileType;        /* 3 = version  III, 4 = version IV                                  */
  xbUInt32 ulHdrNextBlock;       /* next available block , bytes 0-3 of the memo header               */
  void *   mbb;                  /* memo block buffer                                                 */


 private:

  #ifdef XB_LOCKING_SUPPORT
  xbBool bFileLocked;           /* memo file locked */
  #endif

};



#ifdef XB_DBF3_SUPPORT
//! @brief Derived class for version 3 memo files.
/*!

The xbMemoDbt3 class is derived from the xbMome class and is used for accessing the 
older Version 3 memo files.

The main difference between the version 3 and version 4 memo files is that 
the version 4 files will reclaim and reuse blocks if available. The version 3
memo file logic does not.<br>

*/


class XBDLLEXPORT xbMemoDbt3 : public xbMemo {
 public:

  //! @brief Class Constructor.
  /*!
    @param dbf Pointer to dbf instance.
    @param sFileName Memo file name.
  */
  xbMemoDbt3( xbDbf *dbf, xbString const &sFileName );

  //! @brief Class Destructor.
  ~xbMemoDbt3();

  //! @brief Abort.
  /*!
    Abort any pending updates to the memo file.
    @returns XB_NO_ERROR
  */
  virtual xbInt16 Abort          ();

  //! @brief Commit changes to memo file.
  /*!
    @returns XB_NO_ERROR.
  */
  virtual xbInt16 Commit         ();

  //! @brief Create memo file.
  /*!
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 CreateMemoFile ();

  //! @brief Dump memo file header.
  /*!
    @returns XB_NO_ERROR
  */
  virtual xbInt16 DumpMemoHeader ();

  //! @brief Get a memo field for a given field number.
  /*!
    @param iFieldNo Field number to retrieve data for.
    @param sMemoData Output - string containing memo field data.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 GetMemoField   ( xbInt16 iFieldNo, xbString &sMemoData );

  //! @brief Get a memo field length for a given field number.
  /*!
    @param iFieldNo Field number to retrieve data for.
    @param ulFieldLen Output - length of memo field data.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 GetMemoFieldLen( xbInt16 iFieldNo, xbUInt32 &ulMemoFieldLen );

  //! @brief Open memo file.
  /*!
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 OpenMemoFile   ();

  //! @brief Pack memo file.
  /*!
    This routine frees up any unused blocks in the file resulting from field updates.
    Version 3 memo files do not reclaim unused space (Version 4 files do). 
    This routine cleans up the unused space.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 PackMemo       ( void (*memoStatusFunc) (xbUInt32 ulItemNum, xbUInt32 ulNumItems));

  //! @brief Update a memo field for a given field number.
  /*!
    @param iFieldNo Field number to update data for.
    @param sMemoData Data to update memo field data with.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 UpdateMemoField( xbInt16 iFieldNo, const xbString &sMemoData );

  #ifdef XB_DEBUG_SUPPORT
  //! @brief Dump memo file header.
  /*!
     @returns XB_NO_ERROR
  */
  virtual xbInt16 DumpMemoFreeChain();
  #endif

 protected:

  //! @brief Read dbt header file.
  /*!
    @param iOption 0 - read only first four bytes<br>
                   1 - read the entire thing
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 ReadDbtHeader( xbInt16 iOption );

  //! @brief Update header name.
  /*!
    @returns XB_NO_ERROR
  */
  virtual xbInt16 UpdateHeaderName();

  //! @brief Empty the memo file.
  /*!
    This routine clears everything out of the file. It does not address the
    block pointers on the dbf file.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 Zap();

 private:
};
#endif


#ifdef XB_DBF4_SUPPORT

//! @brief Derived class for version 4 memo files.
/*!

The xbMemoDbt4 class is derived from the xbMeme class and is used for accessing the 
current Version 4 memo files.

The main difference between the version 3 and version 4 memo files is that 
the version 4 files will reclaim and reuse blocks if available. The version 3
memo file logic does not.<br>
*/

class XBDLLEXPORT xbMemoDbt4 : public xbMemo {
 public:
  //! @brief Class Constructor.
  /*!
    @param dbf Pointer to dbf instance.
    @param sFileName Memo file name.
  */
  xbMemoDbt4( xbDbf *dbf, xbString const &sFileName );

  //! @brief Class Deconstructor.
  ~xbMemoDbt4();

  //! @brief Abort.
  /*!
    Abort any pending updates to the memo file.
    @returns XB_NO_ERROR
  */
  virtual xbInt16 Abort();

  //! @brief Commit changes to memo file.
  /*!
    Commit any pending updates to the memo file.
    @returns XB_NO_ERROR.
  */
  virtual xbInt16 Commit();

  //! @brief Create memo file.
  /*!
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 CreateMemoFile();

  //! @brief Dump memo file header.
  /*!
    @returns XB_NO_ERROR
  */
  virtual xbInt16 DumpMemoHeader();

  //! @brief Get a memo field for a given field number.
  /*!
    @param iFieldNo Field number to retrieve data for.
    @param sMemoData Output - string containing memo field data.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 GetMemoField( xbInt16 iFieldNo, xbString &sMemoData );

  //! @brief Get a memo field length for a given field number.
  /*!
    @param iFieldNo Field number to retrieve data for.
    @param ulMemoFieldLen Output - length of memo field data.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 GetMemoFieldLen( xbInt16 iFieldNo, xbUInt32 &ulMemoFieldLen );

  //! @brief Get a memo field length for a given field number.
  /*!
    @param iFieldNo Field number to retrieve data for.
    @param ulMemoFieldLen Output - length of memo field data.
    @param ulBlockNo Output - Starting block number.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 GetMemoFieldLen( xbInt16 iFieldNo, xbUInt32 &ulMemoFieldLen, xbUInt32 &lBlockNo );

  //! @brief Open memo file.
  /*!
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 OpenMemoFile();

  //! @brief Pack memo file.
  /*!
    This routine frees up any unused blocks in the file resulting from field updates.
    Version 3 memo files do not reclaim unused space (Version 4 files do).
    This routine cleans up the unused space.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 PackMemo( void (*memoStatusFunc) (xbUInt32 ulItemNum, xbUInt32 ulNumItems));

  //! @brief Update a memo field length for a given field number.
  /*!
    @param iFieldNo Field number to update data for.
    @param sMemoData Data to update memo field data with.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 UpdateMemoField( xbInt16 iFieldNo, const xbString &sMemoData );

  #ifdef XB_DEBUG_SUPPORT
  //! @brief Dump memo file header.
  /*!
    @returns XB_NO_ERROR
  */
  virtual xbInt16 DumpMemoFreeChain();

  //! @brief Dump memo internals.
  /*!
    @returns XB_NO_ERROR
  */
  virtual xbInt16 DumpMemoInternals();

  //! @brief Read free block information from header.
  /*!
    This routing pulls any reusable block information for file header.
    Not used with version 3 memo files - stub.

    @param ulBlockNo
    @param ulNextBlock
    @param ulFreeBlockCnt
    @returns XB_NO_ERROR
  */
  virtual xbInt16 ReadFreeBlockHeader( xbUInt32 ulBlockNo, xbUInt32 &ulNextBlock, xbUInt32 &ulFreeBlockCnt );
  #endif

 protected:
  //! @brief Find an empty set of blocks in the free block chain
  /*!
     This routine searches thruugh the free node chain in a dBASE IV type
     memo file searching for a place to grab some free blocks for reuse 

     @param ulBlocksNeeded The size to look in the chain for.
     @param ulLastDataBlock is the last data block in the file, enter 0
                       for the routine to calculate it.
     @param ulLocation The location it finds.
     @param ulPreviousNode Block number of the node immediately previous to this node in the chain.<br>
                        0 if header node
     @param bFound Output xbFalse - Spot not found in chain.<br>
                       xbTrue - Spot found in chain.
     @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 FindBlockSetInChain( xbUInt32 ulBlocksNeeded, xbUInt32 &ulLastDataBlock, xbUInt32 &ulLocation, xbUInt32 &ulPrevNode, xbBool &bFound );

  //! @brief Free a block.
  /*!
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 FreeMemoBlockChain( xbUInt32 ulBlockNo );

  //! @brief Free a block.
  /*!
    @param ulBlockNo The block number being deleted.
    @param ulLastDataBlock Output - Last free block number,prior to this block.
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 FreeMemoBlockChain( xbUInt32 ulBlockNo, xbUInt32 &ulLastDataBlock );


  //! @brief Get a set of blocks from the free block chain.
  /*!
    This routine grabs a set of blocks out of the free block chain.

    @param ulBlocksNeeded The number of blocks requested.
    @param ulLocation
    @param ulPrevNode
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 GetBlockSetFromChain( xbUInt32 ulBlocksNeeded, xbUInt32 ulLocation, xbUInt32 ulPrevNode );

  //! @brief Read dbt header file.
  /*!
    @param iOption 0  -->  read only first four bytes<br>
                   1  -->  read the entire thing
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 ReadDbtHeader( xbInt16 iOption );

  //! @brief Read block header.
  /*!
    @param ulBlockNo Block to read
    @param iOption 1 - Read fields option 1
                   2 - Read fields option 2
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 ReadBlockHeader( xbUInt32 ulBlockNo, xbInt16 iOption );

  //! @brief Update header name.
  /*!
    @returns XB_NO_ERROR
  */
  virtual xbInt16 UpdateHeaderName   ();

  //! @brief Write block header.
  /*!
    @param ulBlockNo Block to read
    @param iOption 1 - Read fields option 1
                   2 - Read fields option 2
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 WriteBlockHeader( xbUInt32 ulBlockNo, xbInt16 iOption );

  //! @brief Empty the memo file.
  /*!
    @returns <a href="xbretcod_8h.html">Return Codes</a>
  */
  virtual xbInt16 Zap();

 private:
  xbString sDbfFileNameWoExt;
  xbUInt32 ulNextFreeBlock;
  xbUInt32 ulFreeBlockCnt;
  xbInt16  iField1;
  xbInt16  iStartPos;
  xbUInt32 ulFieldLen;

  xbLinkList<xbUInt32> llOldBlocks;   // list of previously used memo blocks for field, used by Commit() / Abort()
  xbLinkList<xbUInt32> llNewBlocks;   // list of newly updated memo blocks for field, used by Commit() / Abort()
};
#endif

}        /* namespace xb   */
#endif   /* XB_MEMO_SUPPORT */
#endif   /* __XB_MEMO_H__  */

