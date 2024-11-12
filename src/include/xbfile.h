/*  xbfile.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023,2024 Gary A Kunkel

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

/** @brief Class for handling low level file I/O.

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
This class could be used if you want to write a platform independent program that needs R/W file access.<br><br>

If you are writing application programs with the Xbase64 library, you most likely won't need to access the
methods in this class but they are available for use.
*/


class XBDLLEXPORT xbFile : public xbCore {

  public:

    /** @brief Class Constructor.
      @param x Pointer to main xbXbase instance
    */
    xbFile( xbXBase * x );

    //! @brief Class Destructor.
    ~xbFile();

    /** @brief Given a directory and file extension as inputs, create a unique file name.
      @param sDirIn Input Directory
      @param sExtIn Input File Extension
      @param sFqnOut  Output fully qualifed unique file name
      @param iOption 0 - look only for one file for a given directory and extension<br>
                     1 - if file name extension is "dbf" or "DBF", verify unique dbt or DBT (memo) file is also available.<br>
      @returns XB_NO_ERROR or XB_FILE_EXISTS if unable to create a unique file name in 999 attempts.
    */
    xbInt16  CreateUniqueFileName( const xbString &sDirIn, const xbString &sExtIn, xbString &sFqnOut, xbInt16 iOption = 0 );


    /** @brief Determine which version the memo (dbt) file is.
      This routine uses the first byte in the dbf file to determine which memo
      file version is in use.  The main difference between version 3 and 4 is that
      version 4 reuses blocks if they become available.  Version 3 does not.

      @param cFileTypeByte Input one byte field containing file type info. This byte is the first byte in the DBF file.<br>

      @returns
        0 - none<br>
        3 - dBASE III+<br>
        4 - dBASE IV<br>
    */
    xbInt16  DetermineXbaseMemoVersion( unsigned char cFileTypeByte ) const;


    /** @brief Determine xbase dbf version.

     This routine is used to determine which version of the Xbase classes can
     be used for a given DBF file.<br>

     The library attempts to use the highest version compiled into the library.<br>

     References:<br>
       Per dBASE documentation:<br>
       Valid dBASE for Windows table file, bits 0-2 indicate version number: 3 for dBASE Level 5, 4 for dBASE Level 7.<br> 
       Bit 3 and bit 7 indicate presence of a dBASE IV or dBASE for Windows memo file;<br>
       Bits 4-6 indicate the presence of a dBASE IV SQL table;<br>
       Bit 7 indicates the presence of any .DBT memo file (either a dBASE III PLUS type or a dBASE IV or dBASE for Windows memo file).<br><br>
       Bachmann spec (used extensively in library build), page 7 - does not match dBASE documentation<br><br>

      @param cFileTypeByte Input one byte field containing file type info. This byte is the first byte in the DBF file.<br>
      @returns 0 - unknown<br>
               3 - dBASE level 3<br>
               4 - dBASE level 4<br>
               5 - dBASE Level 5     (reserved / future)<br>
               7 - dBASE Level 7     (reserved / future)<br>
               1x - Clipper files    (reserveed / future)<br>
               2x - Foxbase files    (reserved / future)<br>
    */
    xbInt16  DetermineXbaseTableVersion( unsigned char cFileTypeByte ) const;


    /** @brief Get a portable double value.

      This routine returns a double value from an 8 byte character stream, 
      accounting for endian type.
 
      Converts a double (64 bit floating point) value stored at p from a portable 
      format to the machine format.

     @param p - pointer to memory containing the portable double value
     @returns double value
    */
    xbDouble eGetDouble ( const char *p ) const;


    /** @brief Get a portable short int value.

      This routine returns a short int value from a 2 byte character stream, 
      accounting for endian type.

      @param p - pointer to memory containing the portable long value
      @returns short value
    */
    xbInt16  eGetInt16  ( const char *p ) const;

    /** @brief Get a portable long value.

      This routine returns a long int value from a 4 byte character stream, 
      accounting for endian type.

      @param p - pointer to memory containing the portable long value

      @returns long int value
    */
    xbInt32  eGetInt32  ( const char *p ) const;

    /** @brief Get a portable unsigned short value.

      This routine returns a short unsigned int value from a 2 byte character stream, 
      accounting for endian type.

      @param p - pointer to memory containing the portable long value

      @returns short value
    */
    xbUInt16 eGetUInt16 ( const char *p ) const;

    /** @brief Get a portable unsigned long value.

      This routine returns an unsigned long int value from a 4 byte character stream, 
      accounting for endian type.

      @param p - pointer to memory containing the portable long value

      @returns unsigned long int value
    */
    xbUInt32 eGetUInt32 ( const char *p ) const;


    /** @brief 

      Converts a double (64 bit floating point) to a portable format accounting 
      for endian type and stores the converted value in eight bytes of memory 
      referenced by pointer p.

      @param p - Pointer to memory to hold ouput value
      @param d - Input value to be converted
    */
    void     ePutDouble ( char *p, xbDouble d );

    /** @brief 

      Converts a short (16 bit integer) value to a portable format accounting 
      for endian type and stores the converted value in two bytes of memory 
      referenced by pointer p.

      @param p - Pointer to memory to hold output value
      @param i - Input value to be converted
    */
    void     ePutInt16  ( char *p, xbInt16 i );


    /** @brief

      Converts a long (32 bit integer) value to a portable format accounting 
      for endian type and stores the converted value in four bytes of memory 
      referenced by pointer p.

      @param p - Pointer to memory to hold output value
      @param l - Input value to be converted
    */
    void     ePutInt32  ( char *p, xbInt32 l );

   /** @brief
 
     Converts an unsigned short (16 bit integer) value to a portable format accounting 
     for endian type and stores the converted value in two bytes of memory 
     referenced by pointer p.

     @param p - Pointer to memory to hold output value
     @param ui - Input value to be converted
    */
    void     ePutUInt16 ( char *p, xbUInt16 ui );

    /** @brief

      Converts an unsigned long (32 bit integer) value to a portable format accounting 
      for endian type and stores the converted value in four bytes of memory
      referenced by pointer p.

      @param p - Pointer to memory to hold output value
      @param ul - Input value to be converted
    */
    void     ePutUInt32 ( char *p, xbUInt32 ul );


    ///@{

     /** @brief Determines if file specified for this instance exists.

       \returns xbTrue file exists<br>
                xbFalse file does not exist
     */
     xbBool   FileExists () const;

     /** @brief Determines if a file exists.

       @param sFileName - file name to check for
       @returns xbTrue file exists<br>
           xbFalse file does not exist
     */
     xbBool   FileExists ( const xbString &sFileName ) const;

     /** @brief Determines if a file exists.

      @param sFileName - file name to check for
      @param iOption 1 - additionally check for presence of a DBT memo file.<br>
                     0 - don't check for DBT memo file.

      @returns xbTrue if iOption 1 and both files exist or iOption 0 and file name exists<br>
           xbFalse file does not exist
     */


     xbBool   FileExists ( const xbString &sFileName, xbInt16 iOption ) const;

    /** @brief Determines if file specified for this instance exists.

      @param iOption 1 - additionally check for presence of a DBT memo file.<br>
                     0 - don't check for DBT memo file.


      @returns xbTrue if iOption 1 and both files exist or iOption 0 and file name exists<br>
           xbFalse file does not exist
    */
     xbBool FileExists ( xbInt16 iOption ) const;

    ///@}


    /** @brief Determines if file is open.
      @returns xbTrue - file is open<br>
               xbFalse - file is not open
    */
    xbBool FileIsOpen () const;

    /** @brief Get the block size.

    File level - returns the block size in use for a given file, if blocking is used.

     @returns Block Size
    */
    xbUInt32 GetBlockSize   () const;

    /** @brief Get the directory portion of the file name.
      @returns The directory portion of the fully qualified file name.
    */
    const xbString& GetDirectory() const;

    ///@{
     /** @brief Get the directory portion of the file name referenced in sCompleteFileNameIn.
       @param sCompleteFileNameIn - a fully qualfied input file name
       @param sFileDirPartOut - output - directory portion of sCompleteFileNameIn
       @returns XB_NO_ERROR or XB_INVALID_DATA
     */
     xbInt16 GetFileDirPart ( const xbString &sCompleteFileNameIn, xbString &sFileDirPartOut ) const;

     /** @brief Get the directory portion of the file name.
       @param sFileDirPartOut - output - directory portion of the file name.
       @returns XB_NO_ERROR or XB_INVALID_DATA
     */
     xbInt16 GetFileDirPart ( xbString &sFileDirPartOut ) const;
    ///@}

    ///@{
     /** @brief Get the extension portion of the file name.
       @param sCompleteFileNameIn - a fully qualfied input file name
       @param sFileExtPartOut - output - the extention portion of sCompleteFileNameIn
       @returns XB_NO_ERROR or XB_INVALID_DATA
     */
     xbInt16 GetFileExtPart ( const xbString &sCompleteFileNameIn, xbString &sFileExtPartOut ) const;

     /** @brief Get the extension part of the file name.
       @param sFileExtPartOut - output - the extension portion of fully qualified file name.
       @returns XB_NO_ERROR or XB_INVALID_DATA
     */
     xbInt16 GetFileExtPart ( xbString &sFileExtPartOut ) const;
    ///@}

    /** @brief Get the time of last file modification timestamp as reported by the OS.
      @param mTime - output - returned time of last file modification
      @returns XB_NO_ERROR or XB_FILE_NOT_FOUND
    */
    xbInt16 GetFileMtime   ( time_t &mTime );

    /** @brief Get the file name.
      @returns The file name portion of the fully qualified file name.
    */
    const xbString& GetFileName() const;

    ///@{
     /** @brief Get the name part of the file name.
       @param sCompleteFileNameIn - a fully qualified input file name
       @param sFileNamePartOut - output - file name portion out of sCompleteFileNameIn
       @returns XB_NO_ERROR
     */
     xbInt16 GetFileNamePart( const xbString &sCompleteFileNameIn, xbString &sFileNamePartOut ) const;

     /** @brief Get the name part of the file name.
       @param sFileNamePartOut - output - file name portion of the current fully qualified file name
       @returns XB_NO_ERROR
     */
     xbInt16 GetFileNamePart( xbString &sFileNamePartOut ) const;
    ///@}

    /** @brief Get the size of the file as reported by the OS.
      @param ullFileSize - output - unsigned long long file size
      @returns XB_NO_ERROR or XB_SEEK_ERROR
    */
    xbInt16 GetFileSize( xbUInt64 &ullFileSize );

    /** @brief Get the file type (Capitalized file extension)
      @param sFileType - output - the returned extension part out of sCompleteFileNameIn
      @returns XB_NO_ERROR or XB_INVALID_DATA
    */
    xbInt16 GetFileType( xbString &sFileType ) const;

    /** @brief Get the fully qualified file name.
       @returns The fully qualfied name of the file
    */
    const xbString& GetFqFileName() const;

    /** @brief Return the file pointer
       @returns FILE* file pointer.
    */
    FILE * GetFp();


    /** @brief Get the open mode of the file.
      @returns XB_READ - Read Only Mode<br>
           XB_READ_WRITE - Read Write Mode<br>
           XB_WRITE - Write Only Mode
    */
    xbInt16 GetOpenMode() const;

    /** @brief Get the share mode of the file.
         @returns XB_SINGLE_USER - File buffering on, file locking not required, better performance.<br>
                   XB_MULTI_USER - File buffering off, file locking required.
    */ 
    xbInt16 GetShareMode() const;

    ///@{
     /** @brief Get the file type byte and version of the dbf file.

       The first byte of a DBF file has various bit settings which determine the file format.
       This routine pulls the first byte off the DBF file.

       @param sFileName - Name of file to examine
       @param cFileTypeByte - Output - first byte of dbf file
       @returns XB_NO_ERROR, XB_OPEN_ERROR, XB_SEEK_ERROR or XB_READ_ERROR
     */
     xbInt16 GetXbaseFileTypeByte( const xbString &sFileName, unsigned char &cFileTypeByte );

     /** @brief Get the file type byte and version of the dbf file.

       The first byte of a DBF file has various bit settings which determine the file format.
       This routine pulls the first byte off the DBF file and returns version information in iVersion.

       @param sFileName - Name of file to examine
       @param cFileTypeByte - Output - first byte of dbf file
       @param iVersion - Output - file version, 0, 3 or 4
       @returns XB_NO_ERROR, XB_OPEN_ERROR, XB_SEEK_ERROR or XB_READ_ERROR
     */
     xbInt16 GetXbaseFileTypeByte( const xbString &sFileName, unsigned char &cFileTypeByte, xbInt16 &iVersion );

     /** @brief Get the file type byte and version of the dbf file.

       The first byte of a DBF file has various bit settings which determine the file format.
       This routine pulls the first byte off the DBF file and returns version information in iVersion..

       @param sFileName - Name of file to examine
       @param iVersion - Output - file version, 0, 3 or 4
       @returns XB_NO_ERROR, XB_OPEN_ERROR, XB_SEEK_ERROR or XB_READ_ERROR
     */
     xbInt16 GetXbaseFileTypeByte( const xbString &sFileName, xbInt16 &iVersion );
   ///@}

   /** @brief Determines status of file extension.
     @param sFileName - Name of file to examine
     @param iOption - Inspection type<br>
         1 check for dbf or DBF extension<br>
         2 check for ndx or NDX extension<br>
         3 check for mdx or MDX extension<br>
         4 check for ntx or NTX extension
     @returns XB_NO_ERROR or XB_NOT_FOUND

   */
   xbInt16 NameSuffixMissing( const xbString &sFileName, xbInt16 iOption ) const;

   ///@{
    /** @brief Read a block of data from file.

      @param ulBlockNo - block number to read
      @param lReadSize - size of data to read at block location, set to 0 to read blocksize
      @param *buf - pointer to buffer to write output data
      @returns XB_NO_ERROR, XB_INVALID_BLOCK_SIZE, XB_READ_ERROR or XB_SEEK_ERROR
      @note buf is assumed to be previosuly allocated and large enough to contain data
    */
    xbInt16 ReadBlock( xbUInt32 ulBlockNo, size_t lReadSize,  void *buf );

    /** @brief Read a block of data from file.

      @param ulBlockNo - block number to read
      @param ulBlockSize - block size
      @param lReadSize - size of data to read at block location, set to 0 to read blocksize
      @param buf - pointer to buffer to write output data

      @returns XB_NO_ERROR, XB_INVALID_BLOCK_SIZE, XB_READ_ERROR or XB_SEEK_ERROR
      @note buf is assumed to be previosuly allocated and large enough to contain data
     */
     xbInt16 ReadBlock( xbUInt32 ulBlockNo, xbUInt32 ulBlockSize, size_t lReadSize,  void *buf );
    ///@}

    /** @brief Set the block size.

      @param ulBlockSize - unsigned long block size, divisible by 512
      @returns XB_NO_ERROR or XB_INVALID_BLOCK_SIZE
    */
    xbInt16 SetBlockSize( xbUInt32 ulBlockSize );

    /** @brief Set the directory.
      @param sDirectory - Valid directory name
    */
    void SetDirectory( const xbString &sDirectory);

    /** @brief Set the filename.
      This routine builds out two internal variables from the input file name<br>
        sFileName   - the file name part<br>
        sFqFileName - the fully qualified file name<br>

      @param sFileName - Input file name
    */
    void SetFileName( const xbString &sFileName );

   /** @brief Set the fully qualifed filename.
      @param sFqFileName - Fully qualifed input file name.
   */
    void SetFqFileName( const xbString &sFqFileName );

   /** @brief Create xbase64 log, data and temp folders in the home directory for the current user.

     @returns XB_NO_ERROR or XB_DIRECTORY_ERROR
   */
   xbInt16 SetHomeFolders();

   /** @brief Write a block of data to file.
     @param ulBlockNo - block number to write
     @param lWriteSize - size of data to write, set to 0 to write blocksize
     @param *buf - pointer to buffer of data to be written
     @returns XB_NO_ERROR, XB_INVALID_BLOCK_SIZE, XB_SEEK_ERROR or XB_WRITE_ERROR
     @note Not intended to be used by typical application program
   */
   xbInt16 WriteBlock( xbUInt32 ulBlockNo, size_t lWriteSize, void *buf );

   /** @brief Xbase wrapper for standard libary fclose.
    @returns XB_NO_ERROR or XB_CLOSE_ERROR
   */
   xbInt16 xbFclose();

   /** @brief Xbase wrapper for standard libary feof.
     @returns non zero if end-of-file is set for the stream.
   */
    xbInt16 xbFeof();

   /** @brief Xbase wrapper for standard libary fflush.
     @returns XB_NO_ERROR or XB_WRITE_ERROR
   */
   xbInt16 xbFflush();

   ///@{
    /** @brief Xbase wrapper for standard libary fgetc.
      @param c - output integer returned by fgetc
      @returns XB_NO_ERROR or XB_EOF
    */
    xbInt16 xbFgetc( char &c );

    /** @brief Xbase wrapper for standard libary fgetc.
      @param c - output character returned by fgetc
      @returns XB_NO_ERROR or XB_EOF
    */
    xbInt16 xbFgetc( xbInt32 &c );
   ///@}

   /** @brief Xbase wrapper for standard libary fgets.
     @param lSize - reads in at most, one character less than lSize
     @param sLine - output - an xbString containing data returned by fseek
     @returns XB_NO_ERROR or XB_EOF
   */
   xbInt16 xbFgets( size_t lSize, xbString &sLine );

   ///@{
    /** @brief Xbase wrapper for standard libary fopen.

      This routine supports all the standard C library open modes.  The Xbase routines only use "r" and "r+b". 

      @param sMode
      <table>
       <tr><th>Mode<th>Description
       <tr><td>r<td>Reading
       <tr><td>r+<td>Reading and Writing
       <tr><td>w<td>Open for writing. Truncate to zero bytes if it exists
       <tr><td>w+<td>Open for reading and writing, truncate to zero bytes if it exists
       <tr><td>a<td>Open for append
       <tr><td>a+<td>Open for reading and writing (at end).
      </table>
      The mode can also include the letter "b" for binary ie; "r+b". The "b" is ignored on
      POSIX compliant systems, but is included for cross platform compatibility.
     @param sFileName File name to open
     @param iShareMode
      XB_SINGLE_USER<br>
      XB_MULTI_USER<br>
     @returns XB_NO_ERROR or XB_OPEN_ERROR
    */
    xbInt16 xbFopen( const xbString &sMode, const xbString &sFileName, xbInt16 iShareMode );

    /** @brief Xbase wrapper for standard libary fopen.

      This routine supports all the standard C library open modes.  The Xbase routines only use "r" and "r+b". 

      @param sMode
      <table> 
       <tr><th>Mode<th>Description
       <tr><td>r<td>Reading
       <tr><td>r+<td>Reading and Writing
       <tr><td>w<td>Open for writing. Truncate to zero bytes if it exists
       <tr><td>w+<td>Open for reading and writing, truncate to zero bytes if it exists
       <tr><td>a<td>Open for append
       <tr><td>a+<td>Open for reading and writing (at end).
      </table>
      The mode can also include the letter "b" for binary ie; "r+b". The "b" is ignored on
      POSIX compliant systems, but is included for cross platform compatibility.
      @param iShareMode
        XB_SINGLE_USER<br>
        XB_MULTI_USER<br>
      @returns XB_NO_ERROR or XB_OPEN_ERROR
    */
    xbInt16 xbFopen( const xbString &sMode, xbInt16 iShareMode );

    /** @brief Xbase wrapper for standard libary fopen.
     @param iMode
      <table>
       <tr><th>Mode<th>Description
       <tr><td>XB_READ<td>Reading
       <tr><td>XB_READ_WRITE<td>Reading and Writing
       <tr><td>XB_WRITE<td>Open for writing. Truncate to zero bytes if it exists
      </table>
     @param iShareMode
      XB_SINGLE_USER<br>
      XB_MULTI_USER<br>
     @returns XB_NO_ERROR or XB_OPEN_ERROR
    */
    xbInt16 xbFopen( xbInt16 iMode, xbInt16 iShareMode );


   ///@}

   ///@{
    /** @brief Xbase wrapper for standard libary fputc.
      @param c Character to write
      @returns XB_NO_ERROR or XB_WRITE_ERROR
    */
    xbInt16 xbFputc( xbInt32 c );

    /** @brief Xbase wrapper for standard libary fputc.
      @param c Character to write
      @param iNoOfTimes Number of times to write the character
      @returns XB_NO_ERROR or XB_WRITE_ERROR
    */
    xbInt16 xbFputc( xbInt32 c, xbInt32 iNoOfTimes );
   ///@}

   /** @brief Xbase wrapper for standard libary fputs.
     @param s xbString to write to file
     @returns XB_NO_ERROR or XB_WRITE_ERROR
   */
   xbInt16 xbFputs( const xbString &s );

   /** @brief Xbase wrapper for standard libary fread.
     @param p Pointer to data to read
     @param size size of write
     @param nmemb Number of times to read it
     @returns XB_NO_ERROR or XB_READ_ERROR
   */
   xbInt16 xbFread( void *p, size_t size, size_t nmemb );

   /** @brief Xbase wrapper for standard libary fseek.
     @param llOffset Position in file to seek to
     @param iWhence SEEK_SET - from beginning of file<br>
                    SEEK_CUR - from current position<br>
                    SEEK_END - from end of file<br>
     @returns XB_NO_ERROR or XB_SEEK_ERROR
   */
   xbInt16 xbFseek( xbInt64 llOffset, xbInt32 iWhence );

   /** @brief Xbase wrapper for standard libary ftell.
      @returns Current file position.
   */
   size_t xbFtell();

   /** @brief Turn off file buffering. File buffering can not be used while in multi user mode.

     This routine makes a call to routine setvbuf and turns off file buffering in multi user mode.

   */
   // void xbFTurnOffFileBuffering();

   /** @brief Xbase wrapper for standard libary fwrite.
     @param p Pointer to data buffer to write
     @param lSize Size of data to write
     @param lNmemb Number of times to write data buffer
     @returns XB_NO_ERROR or XB_WRITE_ERROR
   */
   xbInt16 xbFwrite( const void *p, size_t lSize, size_t lNmemb );
 
   /** @brief Read file until a particular character is encountered on input stream.
      This routine will read until cDelim is encountered or eof, which ever occurs first.

     @param cDelim Delimiter to stop reading at.
     @param sOut Output xbString containing data read
     @returns XB_NO_ERROR or XB_EOF
   */
   xbInt16 xbReadUntil( const char cDelim, xbString &sOut );

   ///@{
    /** @brief Delete file.
      @returns XB_NO_ERROR or XB_DELETE_FAILED
    */
    xbInt16 xbRemove();

    /** @brief Delete file.
      @param sFileName Name of file to delete
      @returns XB_NO_ERROR or XB_DELETE_FAILED
    */
    xbInt16 xbRemove( const xbString &sFileName );

    /** @brief Delete file.
      @param sFileName Name of file to delete
      @param iOption If Set to 1, assume this is a delete request for a dbf file, and should remove the dbt file also
      @returns XB_NO_ERROR or XB_DELETE_FAILED
    */
    xbInt16 xbRemove( const xbString &sFileName, xbInt16 iOption );
   ///@}

   /** @brief Rename file.
     @param sOldName Original file name
     @param sNewName New file name
     @returns XB_NO_ERROR or XB_RENAME_ERROR
   */
   xbInt16  xbRename      ( const xbString &sOldName, const xbString &sNewName );

   /** @brief Xbase wrapper for rewind.
     Set file pointer at beginning of file.
   */
   void     xbRewind      ();

   /** @brief Xbase wrapper for standard library ftruncate.
     Set file size to llSize
     @param llSize New file size.
     @returns XB_NO_ERROR or XB_WRITE_ERROR
   */
   xbInt16  xbTruncate    ( xbInt64 llSize );


    #ifdef XB_DEBUG_SUPPORT

    /** @brief Debugging routine - dump file internals to log file, stdout or both

     This routine dumps xbFile internals to log file, stdout or both.
     @param sTitle Title to print in the output.
     @param iOutputOption
            0 - Write to Logfile.
            1 - Write to stdout.
            2 - Write to both Logfile and stdout.
     @note Available if XB_DEBUG_SUPPORT compiled into the library.
    */
    void Dump( const char *sTitle = "", xbInt16 iOutputOption = 0 );

    /** @brief Debugging routine - dump a block to a log file.

     This routine dumps a block to a log file named BlockDump.Bnnn where nnn is the block number.<br>
     This is primarily used for debugging and analysis purposes.

     @param ulBlockNo Block number to write
     @param lBlockSize Size of block
     @returns XB_NO_ERROR, XB_OPEN_ERROR, XB_WRITE_ERROR or XB_SEEK_ERROR

     @note Available if XB_DEBUG_SUPPORT compiled into the library.
    */
    xbInt16 DumpBlockToDisk( xbUInt32 ulBlockNo, size_t lBlockSize );

    /** @brief Debugging routine - dump memory to a log file.

     This routine dumps data from memory to a log file called MemDump.txt.<br>
     This is primarily used for debugging and analysis purposes.

     @param p Pointer to data to write
     @param lSize Length of data to write
     @returns XB_NO_ERROR, XB_OPEN_ERROR or XB_WRITE_ERROR

     @note Available if XB_DEBUG_SUPPORT compiled into the library.
    */
    xbInt16 DumpMemToDisk( char *p, size_t lSize );
    #endif


    #ifdef XB_LOCKING_SUPPORT

    /** @brief Return the locking retry setting.
      @returns The lock retry setting for this file or the system default setting if the lock retry for the file
               has not been set.
      @note Availble if XB_LOCKING_SUPPORT compiled into library
    */
    xbInt16 GetLockRetryCount() const;

    /** @brief Set the lock retry count for this specific file.
       @param iLockRetries The number of retries to attempt before returning failure for this file
       @note Availble if XB_LOCKING_SUPPORT compiled into library
    */
    void SetLockRetryCount( xbInt16 iLockRetries );

    /** @brief Lock / unlock file.
      @param iFunction XB_LOCK<br>
                       XB_UNLOCK<br>
      @param llOffset Position in file to lock
      @param stLen Length to lock
      @returns XB_NO_ERROR, XB_INVALID_LOCK_OPTION, XB_LOCK_FAILED or XB_SEEK_ERROR
      @note Availble if XB_LOCKING_SUPPORT compiled into library
    */
    xbInt16 xbLock( xbInt16 iFunction, xbInt64 llOffset, size_t stLen );
    #endif



  protected:

    xbInt16 xbSetVbuf( xbInt16 iOpt );

    xbXBase *xbase;              ///< Pointer to the main xbXbase class instance

  private:

    FILE     *fp;
    char     *pFileBuffer;       /* file buffer set with setvbuf                         */
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

