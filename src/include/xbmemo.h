/*  xbmemo.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022 Gary A Kunkel

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

  xbMemo( xbDbf *dbf, xbString const &sFileName );

  /* virtual methods */
  virtual ~xbMemo();
  virtual xbInt16 Abort            () = 0;
  virtual xbInt16 CloseMemoFile    ();
  virtual xbInt16 Commit           () = 0;
  virtual xbInt16 CreateMemoFile   () = 0;
  
  virtual xbInt16 DumpMemoHeader   () = 0;


  virtual xbInt16 GetMemoField     ( xbInt16 iFieldNo, xbString &sMemoData ) = 0;
  virtual xbInt16 GetMemoFieldLen  ( xbInt16 iFieldNo, xbUInt32 &ulMemoFieldLen ) = 0;
  virtual xbInt16 OpenMemoFile     () = 0;
  virtual xbInt16 GetMemoFileType  ();
  virtual xbInt16 PackMemo         ( void (*memoStatusFunc)(xbUInt32 ulItemNum, xbUInt32 ulNumItems)) = 0;
  virtual xbInt16 UpdateMemoField  ( xbInt16 iFieldNo, const xbString &sMemoData ) = 0;

  #ifdef XB_DEBUG_SUPPORT
  virtual xbInt16 DumpMemoFreeChain() = 0;
  #endif

  #ifdef XB_LOCKING_SUPPORT
  virtual xbInt16 LockMemo         ( xbInt16 iLockFunction );
  virtual xbBool  GetMemoLocked    () const;
  #endif



  // *********** FIXME *************
  // next two methods should be protected but are called in the xb_test_xxxx programs testing routines
  // so for now, they are kept as public
  virtual xbInt16 CalcLastDataBlock  ( xbUInt32 &lLastDataBlock );
  virtual xbInt16 GetHdrNextBlock    ( xbUInt32 &ulBlockNo );

 protected:
  friend  class   xbDbf;

  virtual xbInt16 ReadDbtHeader     ( xbInt16 iOption ) = 0;
  virtual xbInt16 UpdateHeaderName  () = 0;
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
  xbMemoDbt3( xbDbf *dbf, xbString const &sFileName );
  ~xbMemoDbt3();
  virtual xbInt16 Abort          ();
  virtual xbInt16 Commit         ();
  virtual xbInt16 CreateMemoFile ();
  virtual xbInt16 DumpMemoHeader ();
  virtual xbInt16 GetMemoField   ( xbInt16 iFieldNo, xbString &sMemoData );
  virtual xbInt16 GetMemoFieldLen( xbInt16 iFieldNo, xbUInt32 &ulMemoFieldLen );
  virtual xbInt16 OpenMemoFile   ();
  virtual xbInt16 PackMemo       ( void (*memoStatusFunc) (xbUInt32 ulItemNum, xbUInt32 ulNumItems));
  virtual xbInt16 UpdateMemoField( xbInt16 iFieldNo, const xbString &sMemoData );

  #ifdef XB_DEBUG_SUPPORT
  virtual xbInt16 DumpMemoFreeChain();
  #endif

 protected:
  virtual xbInt16 ReadDbtHeader( xbInt16 iOption );
  virtual xbInt16 UpdateHeaderName();
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
  xbMemoDbt4( xbDbf *dbf, xbString const &sFileName );
  ~xbMemoDbt4();
  virtual xbInt16 Abort              ();
  virtual xbInt16 Commit             ();
  virtual xbInt16 CreateMemoFile     ();
  virtual xbInt16 DumpMemoHeader     ();
  virtual xbInt16 GetMemoField       ( xbInt16 iFieldNo, xbString &sMemoData );
  virtual xbInt16 GetMemoFieldLen    ( xbInt16 iFieldNo, xbUInt32 &ulMemoFieldLen );
  virtual xbInt16 GetMemoFieldLen    ( xbInt16 iFieldNo, xbUInt32 &ulMemoFieldLen, xbUInt32 &lBlockNo );
  virtual xbInt16 OpenMemoFile       ();
  virtual xbInt16 PackMemo           ( void (*memoStatusFunc) (xbUInt32 ulItemNum, xbUInt32 ulNumItems));
  virtual xbInt16 UpdateMemoField    ( xbInt16 iFieldNo, const xbString &sMemoData );

  #ifdef XB_DEBUG_SUPPORT
  virtual xbInt16 DumpMemoFreeChain  ();
  virtual xbInt16 DumpMemoInternals  ();
  virtual xbInt16 ReadFreeBlockHeader( xbUInt32 ulBlockNo, xbUInt32 &ulNextBlock, xbUInt32 &ulFreeBlockCnt );
  #endif

 protected:
  virtual xbInt16 FindBlockSetInChain( xbUInt32 ulBlocksNeeded, xbUInt32 &ulLastDataBlock, xbUInt32 &ulLocation, xbUInt32 &ulPrevNode, xbBool &bFound );
  virtual xbInt16 FreeMemoBlockChain( xbUInt32 ulBlockNo );
  virtual xbInt16 FreeMemoBlockChain( xbUInt32 ulBlockNo, xbUInt32 &ulLastDataBlock );
  virtual xbInt16 GetBlockSetFromChain( xbUInt32 ulBlocksNeeded, xbUInt32 ulLocation, xbUInt32 ulPrevNode );
  virtual xbInt16 ReadDbtHeader( xbInt16 iOption );
  virtual xbInt16 ReadBlockHeader( xbUInt32 ulBlockNo, xbInt16 iOption );
  virtual xbInt16 UpdateHeaderName   ();
  virtual xbInt16 WriteBlockHeader( xbUInt32 ulBlockNo, xbInt16 iOption );
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

