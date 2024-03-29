/*  xbssv.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/


#ifndef __XB_XBSSV_H__
#define __XB_XBSSV_H__

#ifdef  CMAKE_COMPILER_IS_GNUCC
#pragma interface
#endif


namespace xb{

class XBDLLEXPORT xbXBase;

///@cond DOXYOFF
struct XBDLLEXPORT xbErrorMessage{
  xbInt16    iErrorNo;
  const char *sErrorText;
};
///@endcond DOXYOFF


//! @brief Class for handling shared system variables.
/*!
This class defines static variables which are referenced across class instances.

This class is designed to have the variables set when the xbXBase class constructor is called 
(xbXBase is a derived class).  The xbXbase class is designed to be called first in an application 
programs, and it is only called once.  The static values in this class are typically initialized at
program startup and don't require additional updates.

*/


// By design, DBase allows mutliple records in a table all having the same key, but only one entry in a unique index
// XB_HALT_ON_DUP_KEY tells the library to not allow appending records which generate duplicate keys in a unique index
//
#if defined (XB_NDX_SUPPORT) || defined (XB_MDX_SUPPORT)
#define XB_HALT_ON_DUPKEY 0
#define XB_EMULATE_DBASE  1
#endif



class XBDLLEXPORT xbSsv{
 public:
  xbSsv();
  const static char *ErrorCodeText[];

  void       DisplayError              ( xbInt16 ErrorCode ) const;
  xbString&  GetDefaultDateFormat      () const;
  xbString&  GetDataDirectory          () const;
  xbString&  GetTempDirectory          () const;

  void       GetHomeDir                ( xbString &sHomeDirOut );


  xbInt16    GetEndianType             () const;
  const char *GetErrorMessage          ( xbInt16 ErrorCode ) const;
  char       GetPathSeparator          () const;

  void       SetDataDirectory          ( const xbString &sDataDirectory );
  void       SetDefaultDateFormat      ( const xbString &sDefaultDateFormat );
  void       SetTempDirectory          ( const xbString &sTempDirectory );


  xbBool     BitSet                    ( unsigned char c, xbInt16 iBitNo ) const;
  void       BitDump                   ( unsigned char c ) const;
  void       BitDump                   ( char c ) const;

  xbBool     GetDefaultAutoCommit      () const;
  void       SetDefaultAutoCommit      ( xbBool bDefaultAutoCommit );

  xbString&  GetLogDirectory           () const;
  xbString&  GetLogFileName            () const;
  void       SetLogDirectory           ( const xbString &sLogDirectory );
  void       SetLogFileName            ( const xbString &sLogFileName );


  xbBool     GetMultiUser              () const;
  void       SetMultiUser              ( xbBool bMultiUser );

  #if defined (XB_NDX_SUPPORT) || defined (XB_MDX_SUPPORT)
  xbInt16    GetUniqueKeyOpt           () const;
  xbInt16    SetUniqueKeyOpt           ( xbInt16 iUniqueKeyOpt );
  #endif  // (XB_NDX_SUPPORT) || defined (XB_MDX_SUPPORT)

  #ifdef XB_LOCKING_SUPPORT
  xbInt16    GetDefaultLockRetries     () const;
  void       SetDefaultLockRetries     ( xbInt16 iRetryCount );
  xbInt32    GetDefaultLockWait        () const;
  void       SetDefaultLockWait        ( xbInt32 lRetryWait );
  xbInt16    GetDefaultLockFlavor      () const;
  void       SetDefaultLockFlavor      ( xbInt16 iLockFlavor );
  xbBool     GetDefaultAutoLock        () const;
  void       SetDefaultAutoLock        ( xbBool bAutoLock );
  void       EnableDefaultAutoLock     ();
  void       DisableDefaultAutoLock    ();
  #endif  // XB_LOCKING_SUPPORT

  #ifdef XB_MDX_SUPPORT
  xbInt16    GetCreateMdxBlockSize() const;
  xbInt16    SetCreateMdxBlockSize( xbInt16 ulBlockSize );
  #endif  // XB_MDX_SUPPORT

  #ifdef XB_BLOCKREAD_SUPPORT
  xbUInt32   GetDefaultBlockReadSize() const;
  void       SetDefaultBlockReadSize( xbUInt32 ulDfltBlockReadSize );
  #endif  // XB_BLOCKREAD_SUPPORT


 protected:

  void       SetEndianType             ();

  static xbInt16  iEndianType;             // B=Big Endian  L=Little Endian
  static xbString sNullString;             // Null String


 private:

  static xbString sDefaultDateFormat;
  static xbString sDataDirectory;          //Data file directory
  static xbString sTempDirectory;          //Temp file directory

  #ifdef XB_LOGGING_SUPPORT
  static xbString sLogDirectory;           //Default location to store log files
  static xbString sLogFileName;            //Default LogFileName
  #endif

  static xbInt16  iDefaultFileVersion;     // 3 = DBase 3
                                           // 4 = DBase 4
                                           // default version used in CreateTable command
                                           // can be over ridden at the Xbase level, or table level
                                           // Different versions can be open simultaneously

  static xbBool   bDefaultAutoCommit;      // Default dbf auto commit switch

  static xbBool   bMultiUser;              // True if multi user mode is turned on
                                           // Turn this off for better performance in single user mode
                                           // This needs to be turned on or off before any data tables are opened
                                           // turning this on after tables are opened, can result in out of date
                                           // file buffers if multiple users are sharing the files

#ifdef XB_LOCKING_SUPPORT
  static xbInt32  lDefaultLockWait;        // Number of milliseconds between lock retries
  static xbInt16  iDefaultLockRetries;     // Number of times to retry a lock before conceding
  static xbInt16  bDefaultAutoLock;        // Autolocking enabled?
  static xbInt16  iDefaultLockFlavor;      // 1 = DBase
                                           // 2 = Clipper   - not developed yet
                                           // 3 = FoxPro    - not developed yet
                                           // 9 = Xbase64   - not developed yet
#endif


#if defined (XB_NDX_SUPPORT) || defined (XB_MDX_SUPPORT)

  static xbInt16 iUniqueKeyOpt;

  // is one of:
  //    XB_HALT_ON_DUPKEY
  //    XB_EMULATE_DBASE

#endif


#ifdef XB_MDX_SUPPORT
  static xbInt16  iCreateMdxBlockSize;     // System level Mdx Block Size
#endif // XB_MDX_SUPPORT


#ifdef XB_BLOCKREAD_SUPPORT
  static xbUInt32 ulDefaultBlockReadSize;
#endif  // XB_BLOCKREAD_SUPPORT


};

}        /* namespace xb    */
#endif   /* __XB_XBSSV_H__ */