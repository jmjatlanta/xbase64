/*  xbfile.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

#ifndef __XB_FILE_H__
#define __XB_FILE_H__

#ifdef  CMAKE_COMPILER_IS_GNUCC
#pragma interface
#endif

/*****************************/
/* File Open Modes           */
#define XB_READ       0
#define XB_READ_WRITE 1
#define XB_WRITE      2

/*****************************/
/* File Access Modes         */
#define XB_SINGLE_USER  0      // file buffering on
#define XB_MULTI_USER   1      // file buffering off

namespace xb{

/*! @brief Class for handling low level file I/O.
*/

/*!
The xbFile class is used as an abstraction layer to isolate the library from the nuances 
of different operating systems and machine architectures.

This module handles low level file I/O and is a base class
for the (dbf) table, (dbt) memo and (ndx,mdx) index classes.
Files are opened and manipulated via methods in the xbFile class.<br>
This class handles:<br>
1) Big and little endian reads/writes<br>
2) 32 or 64 bit system calls, depending on OS<br>
3) Various different c/c++ calls to open/close/read/write based on OS and compiler version<br>
<br>
If you are wanting to port this library to a new platform, start with this class.
This class could be used if you want to write a platform independent program that needs R/W file access.
*/


class XBDLLEXPORT xbFile : public xbSsv {

  public:
             xbFile( xbXBase * x );
             ~xbFile();

    const    xbString& GetDirectory() const;
    const    xbString& GetFileName() const;
    const    xbString& GetFqFileName() const;


    xbInt16  CreateUniqueFileName( const xbString &sDirIn, const xbString &sExtIn, xbString &sFqnOut, xbInt16 iOption = 0 );

    xbInt16  DetermineXbaseTableVersion( unsigned char cFileTypeByte ) const;
    xbInt16  DetermineXbaseMemoVersion( unsigned char cFileTypeByte ) const;

    xbDouble eGetDouble ( const char *p ) const;
    xbInt32  eGetInt32  ( const char *p ) const;
    xbUInt32 eGetUInt32 ( const char *p ) const;
    xbInt16  eGetInt16  ( const char *p ) const;
    xbUInt16 eGetUInt16 ( const char *p ) const;
    void     ePutDouble ( char *p, xbDouble d );
    void     ePutInt32  ( char *p, xbInt32 l );
    void     ePutUInt32 ( char *p, xbUInt32 ul );
    void     ePutInt16  ( char *p, xbInt16 s );
    void     ePutUInt16 ( char *p, xbUInt16 s );

    xbBool   FileExists () const;
    xbBool   FileExists ( xbInt16 iOption ) const;
    xbBool   FileExists ( const xbString &sFileName ) const;
    xbBool   FileExists ( const xbString &sFileName, xbInt16 iOption ) const;
    xbBool   FileIsOpen () const;

    xbUInt32 GetBlockSize   () const;

    xbInt16  GetFileDirPart ( xbString &sFileDirPartOut ) const;
    xbInt16  GetFileDirPart ( const xbString &sCompleteFileNameIn, xbString &sFileDirPartOut ) const;
    xbInt16  GetFileExtPart ( xbString &sFileExtPartOut ) const;
    xbInt16  GetFileExtPart ( const xbString &sCompleteFileNameIn, xbString &sFileExtPartOut ) const;
    xbInt16  GetFileMtime   ( time_t &mtime );
    xbInt16  GetFileNamePart( xbString &sFileNamePartOut ) const;
    xbInt16  GetFileNamePart( const xbString &sCompleteFileNameIn, xbString &sFileNamePartOut ) const;
    xbInt16  GetFileSize    ( xbUInt64 &ullFileSize );
    xbInt16  GetFileType    ( xbString &sFileType ) const;


    xbInt16  GetOpenMode    () const;
    xbInt16  GetShareMode   () const;


    xbInt16  GetXbaseFileTypeByte( const xbString &sFileName, xbInt16 &iVersion );
    xbInt16  GetXbaseFileTypeByte( const xbString &sFileName, unsigned char &cFileTypeByte );
    xbInt16  GetXbaseFileTypeByte( const xbString &sFileName, unsigned char &cFileTypeByte, xbInt16 &iVersion );

    xbInt16  NameSuffixMissing( const xbString &sFileName, xbInt16 iOption ) const;

    xbInt16  ReadBlock     ( xbUInt32 ulBlockNo, size_t readSize,  void *buf );
    xbInt16  ReadBlock     ( xbUInt32 ulBlockNo, xbUInt32 ulBlockSize, size_t readSize,  void *buf );

    xbInt16  SetBlockSize  ( xbUInt32 ulBlockSize );
    void     SetDirectory  ( const xbString &sDirectory);
    void     SetFileName   ( const xbString &sFileName );
    void     SetFqFileName ( const xbString &sFqName );
    xbInt16  SetHomeFolders();

    xbInt16  WriteBlock    ( xbUInt32 ulBlockNo, size_t writeSize, void *buf );

    xbInt16  xbFclose      ();
    xbInt16  xbFeof        ();
    xbInt16  xbFflush      ();
    xbInt16  xbFgetc       ( xbInt32 &c );
    xbInt16  xbFgetc       ( char &c );
    xbInt16  xbFgets       ( size_t lSize, xbString &sLine );


    xbInt16  xbFopen       ( xbInt16 iOpenMode );
    xbInt16  xbFopen       ( const xbString &sOpenMode, xbInt16 iShareMode );
    xbInt16  xbFopen       ( xbInt16 iOpenMode, xbInt16 iShareMode );
    xbInt16  xbFopen       ( const xbString &sMode, const xbString &sFileName, xbInt16 iShareMode );

    xbInt16  xbFputc       ( xbInt32 c );
    xbInt16  xbFputc       ( xbInt32 c, xbInt32 iNoTimes );
    xbInt16  xbFputs       ( const xbString &s );
    xbInt16  xbFread       ( void *ptr, size_t size, size_t nmemb );
    xbInt16  xbFseek       ( xbInt64 llOffset, xbInt32 whence );
    size_t   xbFtell       ();
    void     xbFTurnOffFileBuffering();
 
    xbInt16  xbReadUntil   ( const char cDelim, xbString &sOut );
    xbInt16  xbRemove      ( const xbString &sFileName, xbInt16 iOption );
    xbInt16  xbRemove      ( const xbString &sFileName );
    xbInt16  xbRemove      ();

    xbInt16  xbRename      ( const xbString &sOldName, const xbString &sNewName );
    void     xbRewind      ();

    xbInt16  xbFwrite      ( const void *ptr, size_t lSize, size_t lNmemb );

    xbInt16  xbTruncate    ( xbInt64 llSize );


    #ifdef XB_LOCKING_SUPPORT
    xbInt16  xbLock           ( xbInt16 iFunction, xbInt64 llOffset, size_t stLen );
    xbInt16  GetLockRetryCount() const;
    void     SetLockRetryCount( xbInt16 iLockRetries );
    #endif

    #ifdef XB_DEBUG_SUPPORT
    xbInt16  DumpBlockToDisk( xbUInt32 ulBlockNo, size_t lBlockSize );
    xbInt16  DumpMemToDisk  ( char *p, size_t lBlockSize );
    #endif


  protected:

    xbXBase *xbase;              /* pointer to the main structures                       */

  private:
    FILE     *fp;
    xbString sFqFileName;        /* fully qualified file name                            */
    xbString sFileName;          /* file name                                            */
    xbString sDirectory;         /* directory, ends with / or \                          */
    xbBool   bFileOpen;          /* true if file is open                                 */
    xbUInt32 ulBlockSize;        /* used for memo and index files                        */

    xbInt16  iOpenMode;          /* XB_READ || XB_READ_WRITE || XB_WRITE                 */
    xbInt16  iShareMode;         /* XB_SINGLE_USER || XB_MULTI_USER - set file buffering */
    xbInt32  iFileNo;            /* Library File No                                      */

    #ifdef XB_LOCKING_SUPPORT
    xbInt16  iLockRetries;       /* file override number of lock attempts                */
    #endif

    #ifdef HAVE_SETENDOFFILE_F
    HANDLE   fHandle;
    #endif


};

}        /* namespace     */
#endif   /*  __XBFILE_H__ */

