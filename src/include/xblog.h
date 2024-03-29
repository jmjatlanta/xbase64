/*  xblog.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

#ifndef __XB_XBLOG_H__
#define __XB_XBLOG_H__

#ifdef  CMAKE_COMPILER_IS_GNUCC
#pragma interface
#endif

namespace xb{

#ifdef XB_LOGGING_SUPPORT

//! @brief xbLog class.
/*!
The xbLog class handles message logging functions.

If logging is enabled in an application, error messages are printed
in the log file.

Once a logfile reaches a predefined size, the logfile is closed, renamed
and opened.

The logging functions can be accessed through the xbXBase class.
Each application has one of those.
*/

class XBDLLEXPORT xbLog : public xbFile {
  public:
   xbLog();
   xbLog( const xbString &sLogFileName );
   ~xbLog();

   xbInt16  LogClose     ();
   xbInt16  LogGetStatus ();
   xbInt16  LogOpen      ();
   void     LogSetStatus ( xbBool bLogStatus );
   void     LogSetLogSize( size_t lSize );                  //   { LogSize = size; }
   xbInt16  LogWrite     ( const xbString &LogEntry, xbInt16 iOutputOption = 0 );
   xbInt16  LogWriteBytes( xbUInt32 lByteCnt, const char *p );
   size_t   LogGetLogSize() const { return lLogSize; }

  private:
   xbBool   bLoggingStatus;        // false = logging off
                                   // true  = logging on
   size_t   lLogSize;
   xbInt16  iShareMode;
};

#endif          // XB_LOGGING_SUPPORT
}               // namespace
#endif          // XB_XBLOG_H__


