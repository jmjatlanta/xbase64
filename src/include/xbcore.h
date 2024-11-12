/*  xbcore.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023,2024 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/


#ifndef __XB_XBCORE_H__
#define __XB_XBCORE_H__

#ifdef  CMAKE_COMPILER_IS_GNUCC
#pragma interface
#endif


namespace xb{

class XBDLLEXPORT xbXBase;


//! @brief Class for handling shared system variables.
/*!
This class defines static variables which are referenced across class instances.

This class is designed to have the variables set when the xbXBase class constructor is called 
(xbXBase is a derived class).  The xbXbase class is designed to be called first in an application 
programs, and it is only called once.  The static values in this class are typically initialized at
program startup and don't require additional updates.


   In dBASE Compatibility mode   -  XB_IX_DBASE_MODE
     1)  dBASE allows mutliple records in a table all having the same key, but only one entry in a unique index
     2)  Deleted DBF records are not deleted from the associated tag file

   In Xbase mode - XB_IX_XBASE_MODE 
     1)  Index tags defined as unique - this option throws an error when ever an attempt is made to add/update a record with a duplicate key
     2)  Deleted DBF records are deleted from the associated tag file
*/



#if defined (XB_NDX_SUPPORT) || defined (XB_MDX_SUPPORT)
#define XB_IX_DBASE_MODE  0
#define XB_IX_XBASE_MODE  1
#endif


class XBDLLEXPORT xbCore{
 public:

  const static char *ErrorCodeText[];

 protected:

  static xbBool   bMultiUser;              // xbOn if multi user mode is turned on
                                           // xbOff turn off for better performance in single user mode
                                           // This needs to be turned on or off before any data tables are opened
                                           // turning this on after tables are opened, can result in out of date
                                           // file buffers if multiple users are sharing the files

  static xbBool   bDefaultAutoCommit;      // Default dbf auto commit switch

  static xbInt16  iDefaultFileVersion;     // 3 = dBASE 3
                                           // 4 = dBASE 4
                                           // default version used in CreateTable command
                                           // can be over ridden at the Xbase level, or table level
                                           // Different versions can be open simultaneously

  static xbInt16  iEndianType;             // B=Big Endian  L=Little Endian

  static xbString sDataDirectory;          //Data file directory

  static xbString sNullString;             // Null String

  static xbString sTempDirectory;          //Temp file directory

  static xbString sDefaultDateFormat;

  #ifdef XB_BLOCKREAD_SUPPORT
  static xbUInt32 ulDefaultBlockReadSize;
  #endif  // XB_BLOCKREAD_SUPPORT

  #ifdef XB_DBF5_SUPPORT
  static xbString sDefaultTimeFormat;
  #endif // XB_DBF5_SUPPORT

  #ifdef XB_LOCKING_SUPPORT
  //static xbInt16  bDefaultAutoLock;        // Autolocking enabled?
  static xbInt16  iDefaultLockFlavor;      // 1 = dBASE
                                           // 2 = Clipper   - not developed yet
                                           // 3 = FoxPro    - not developed yet
                                           // 9 = Xbase64   - not developed yet
  static xbInt16  iDefaultLockRetries;     // Number of times to retry a lock before conceding
  static xbInt32  lDefaultLockWait;        // Number of milliseconds between lock retries
  #endif

  #ifdef XB_MDX_SUPPORT
  static xbInt16  iCreateMdxBlockSize;     // System level Mdx Block Size
  #endif // XB_MDX_SUPPORT

  #if defined (XB_NDX_SUPPORT) || defined (XB_MDX_SUPPORT)
  static xbInt16 iDefaultIxTagMode;
  #endif

  #ifdef XB_LOGGING_SUPPORT
  static xbString sLogDirectory;           //Default location to store log files
  static xbString sLogFileName;            //Default LogFileName
  #endif

};

}        /* namespace xb    */
#endif   /* __XB_XBCORE_H__ */