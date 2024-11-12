/*  xbblkread.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2024 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

#ifndef __XB_BLKREAD_H__
#define __XB_BLKREAD_H__

namespace xb{
#ifdef XB_BLOCKREAD_SUPPORT



//! @brief xbBlockRead class.
/*!
The xbBlockRead class can be used to read blocks of records at one time
rather than one record at a time for perfomance improvements when reading a DBF file sequentially.<br>
<br>
See program xb_dumprecs.cpp for an example on how to use block read processing.
*/


class XBDLLEXPORT xbBlockRead {

  public:

    /**
     @brief Constructor
     @param d Pointer to dbf table object to apply block processing on.
    */
    xbBlockRead( xbDbf * d );

    /**
     @brief Destructor
    */
    ~xbBlockRead();

    #ifdef XB_DEBUG_SUPPORT
    /** @brief Dump internals to stdout.
     @note Availabe if XB_DEBUG_SUPPORT compiled in.
    */
    void     DumpReadBlockInternals();
    #endif

    /** @brief Retrieve the first record number in the current block.
      @returns First record number in the current block.
    */
    xbUInt32 GetBlockFirstRecNo() const;

    /** @brief Retrieve the number of records loaded in the curent block.
      @return Record count loaded in current block.
    */
    xbUInt32 GetBlockRecCnt() const;

    /** @brief Retrieve the current block size.
      @return Current Block Size.
    */
    xbUInt32 GetBlockSize() const;

    /** @brief Get record for specified record number.

      This method retrieves a record from read block buffer and copies it into the record buffer.
      If the record is not in the current block, the routine calls GetBlockForRecNo to load the 
      currect block from disk.

      @param ulRecNo - Record number to retrieve.
      @return XB_NO_ERROR, XB_SEEK_ERROR, XB_LOCK_FAILED, XB_FILE_NOT_FOUND or XB_INVALID_OPTION
      @warning For performance reasons, this method assumes a valid record number has been passed 
      and does no error checking on ulRecNo.
    */
    xbInt16  GetRecord( xbUInt32 ulRecNo );

    /** @brief Init the block processing for a given DBF file.
      This routine may adjust the block size as needed to eliminate unused
      memory or adjust it bigger if too small.

      @param ulBlockSize - Block size to allocate. If 0 or missing, it uses default block size of 32K.
      @return XB_NO_ERROR or XB_MEMORY_ERROR
    */
    xbInt16  Init( xbUInt32 ulBlockSize = 0 );

   /** @brief Set the block size for this instance.<br>
     @param ulBlkSize - Block Size.
   */
    void  SetBlockSize( xbUInt32 ulBlkSize );


  private:

    xbInt16  GetBlockForRecNo( xbUInt32 ulRecNo );    // retrieve block from disk for a given record number

    char     *pBlock;             // block pointer
    xbUInt32 ulBlkSize;           // block size in bytes
    xbUInt32 ulFirstBlkRec;       // first recod number in the block
    xbUInt32 ulRecCnt;            // number of records in block
    xbUInt32 ulMaxRecs;           // max number of records block can handle
    xbBool   bEof;                // EOF flag
    xbDbf    *dbf;                // reference to associated dbf file
    time_t   tFmTime;             // file modify time at time of block read

};

#endif // XB_BLOCKREAD
}  /* namespace */
#endif  /*  __XB_BLOCKREAD_H__  */
