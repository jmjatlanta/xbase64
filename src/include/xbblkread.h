/*  xbblkread.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022 Gary A Kunkel

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

*/


class XBDLLEXPORT xbBlockRead {

  public:
    xbBlockRead( xbDbf * d );
    ~xbBlockRead();

    #ifdef XB_DEBUG_SUPPORT
    void     DumpReadBlockInternals();
    #endif

    xbInt16  Init( xbUInt32 ulBlkSize = 0 );

    xbUInt32 GetBlockFirstRecNo() const;
    xbUInt32 GetBlockRecCnt() const;
    xbUInt32 GetBlockSize() const;

    xbInt16  GetRecord( xbUInt32 ulRecNo );            // retrieve a data record from a block to RecBuf
    xbInt16  SetBlockSize( xbUInt32 ulBlkSize );


  private:

    xbInt16  GetBlockForRecNo( xbUInt32 ulRecNo );    // retrieve block from disk for a given record number

    char     *pBlock;             // block pointer
    xbUInt32 ulBlkSize;           // block size in bytes
    xbUInt32 ulFirstBlkRec;       // first recod number in the block
    xbUInt32 ulRecCnt;            // number of records in block
    xbUInt32 ulMaxRecs;           // max number of records block can handle
    xbBool   bEof;                // EOF flag
    xbDbf    *dbf;                // reference to associated dbf file
//    xbXBase  *xbase;              // reference to main xbXBase structure
    time_t   tFmTime;             // file modify time at time of block read

};

#endif // XB_BLOCKREAD
}  /* namespace */
#endif  /*  __XB_BLOCKREAD_H__  */
