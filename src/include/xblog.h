/*  xblog.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2024 Gary A Kunkel

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

<ul>
<li>Error and message logging.
<li>Rolling log files. When log file fills, full file is closed and new file is opened.
<li>Configurable size.
<li>Configurable name.
<li>Configurable location.
</ul>

*/

class XBDLLEXPORT xbLog : public xbFile {
  public:
   //! @brief Constructor.
   xbLog();

   //! @brief Constructor.
   /*!
     @param sLogFileName - Log file name.
   */
   xbLog( const xbString &sLogFileName );

   //! @brief Destructor.
   ~xbLog();

   //! @brief Close the logfile.
   /*!
     @returns <a href="xbretcod_8h.html">Return Codes</a>
   */
   xbInt16 LogClose();

   //! @brief Get the current logging status
   /*!
      @returns xbTrue - Logging turned on.<br>xbFalse - Logging turned off.
   */
   xbInt16 LogGetStatus();


   //! @brief Open the logfile.
   /*!
      @returns <a href="xbretcod_8h.html">Return Codes</a>
   */
   xbInt16 LogOpen();

   //! @brief Set log status.
   /*!
      @param bLogStatus xbTrue - Turn logging on.<br>xbFalse - Turn logging off.
   */
   void LogSetStatus( xbBool bLogStatus );

   //! @brief Set maximum log file size.
   /*!
     When file size is reached, current logfile is closed and new file is opened.
     @param lSize - New maximum log file size.
   */
   void LogSetLogSize( size_t lSize );                  //   { LogSize = size; }

   //! @brief Write a logfile message.
   /*!
      @param sLogEntry Message to write to the logfile.
      @param iOutputOption 0 - Write to logfile.<br>
                           1 - Write to stdout.<br>
                           2 - Write to both logfile and stdout.
      @returns <a href="xbretcod_8h.html">Return Codes</a>
   */
   xbInt16 LogWrite( const xbString &sLogEntry, xbInt16 iOutputOption = 0 );

   //! @brief Write bytes to logfile.
   /*!
      @param ulByteCnt - Number of bytes to write to logfile.
      @param p - Pointer to data to write to logfile.
      @returns XB_NO_ERROR
   */
   xbInt16 LogWriteBytes( xbUInt32 ulByteCnt, const char *p );

   //! @brief Get log file size setting
   /*!
      @returns Log size.
   */
   size_t LogGetLogSize() const;

  private:
   xbBool   bLoggingStatus;        // false = logging off
                                   // true  = logging on
   size_t   lLogSize;
   xbInt16  iShareMode;
};

#endif          // XB_LOGGING_SUPPORT
}               // namespace
#endif          // XB_XBLOG_H__


