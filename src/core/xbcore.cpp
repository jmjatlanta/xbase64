/* xbcore.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023,2024 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

#include "xbase.h"

namespace xb{


xbInt16  xbCore::iDefaultFileVersion     = 4;
xbInt16  xbCore::iEndianType             = 0;
xbString xbCore::sDataDirectory          = PROJECT_DATA_DIR;
xbBool   xbCore::bDefaultAutoCommit      = xbTrue;
xbString xbCore::sDefaultDateFormat      = "MM/DD/YY";
xbString xbCore::sNullString             = "";
xbString xbCore::sTempDirectory          = PROJECT_TEMP_DIR;


#ifdef XB_BLOCKREAD_SUPPORT
xbUInt32 xbCore::ulDefaultBlockReadSize  = 32768;  // 32K buffer for block DBF datafile reads
#endif  // XB_BLOCKREAD_SUPPORT

#ifdef XB_DBF5_SUPPORT
xbString xbCore::sDefaultTimeFormat      = "%H:%M:%S";
#endif  // XB_DBF5_SUPPORT

#ifdef XB_LOCKING_SUPPORT
//xbBool   xbCore::bDefaultAutoLock        = xbTrue;
xbBool   xbCore::bMultiUser              = xbTrue;
xbInt16  xbCore::iDefaultLockFlavor      = 1;
xbInt16  xbCore::iDefaultLockRetries     = 3;
xbInt32  xbCore::lDefaultLockWait        = 100;
#else
xbBool   xbCore::bMultiUser              = xbFalse;
#endif  // XB_LOCKING_SUPPORT

#ifdef XB_LOGGING_SUPPORT
xbString xbCore::sLogDirectory           = PROJECT_LOG_DIR;
xbString xbCore::sLogFileName            = PROJECT_DFLT_LOGFILE;
#endif  // XB_LOGGING_SUPPORT

#ifdef XB_MDX_SUPPORT
xbInt16  xbCore::iCreateMdxBlockSize     = 1024;   // 1024 is dBASE 7 default size for MDX index blocks
#endif  // XB_MDX_SUPPORT

#if defined (XB_NDX_SUPPORT) || defined (XB_MDX_SUPPORT)
xbInt16  xbCore::iDefaultIxTagMode       = XB_IX_DBASE_MODE;
#endif  // defined (XB_NDX_SUPPORT) || defined (XB_MDX_SUPPORT)

/*************************************************************************/
//! @brief Class Constructor.
//xbCore::xbCore(){}
/*************************************************************************/


}   /* namespace */