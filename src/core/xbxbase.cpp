/* xbxbase.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

#include "xbase.h"

namespace xb{

/*************************************************************************/
//! @brief Class Constructor.
xbXBase::xbXBase() {
  SetEndianType();

  xbFile f( this );
  f.SetHomeFolders();

  xbDate d( (xbUInt16) 1);      // initialize xbDate static variables

  #ifdef XB_LOGGING_SUPPORT
  xLog = new xbLog();
  #endif

}
/*************************************************************************/
//! @brief Class Deconstructor.
xbXBase::~xbXBase(){
  CloseAllTables();
  #ifdef XB_LOGGING_SUPPORT
  delete xLog;
  #endif
}
/*************************************************************************/
//! @brief Close all tables / files.
/*!
   This closes everything and deletes references to the associated xbDbf objects.
   \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbXBase::CloseAllTables(){

  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;
  xbInt16 iOpenTableCnt = GetOpenTableCount();
  try{
    xbDbf *d;
    for( xbInt16 i = 0; i < iOpenTableCnt; i++ ){
      d = (xbDbf *) GetDbfPtr( 1 );
      if( d ){
        if(( iRc = d->Close()) != XB_NO_ERROR ){
          iErrorStop = 100;
          throw iRc;
        }
        delete d;
      } else {
        iRc = XB_INVALID_OBJECT;
        iErrorStop = 110;
        throw iRc;
      }
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbxbase::CloseAllTables() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, iRc );
    WriteLogMessage( sMsg.Str() );
    WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}

/************************************************************************/
//! @brief Fully qualified file name from a directory, filename and extension.
/*!
  Given a directory, file name and file extension as inputs, create a fully qualified file name.

  \param sDirIn Directory
  \param sNameIn File Name
  \param sExtIn File Extension
  \param sFqnOut   A fully qualifed unique file name as output
  \returns XB_INVALIED_PARAMETER or XB_NO_ERROR
*/
xbInt16 xbXBase::CreateFqn( const xbString &sDirIn, const xbString &sNameIn, const xbString &sExtIn, xbString &sFqnOut ){

  if( sNameIn == "" || sExtIn == "" )
    return XB_INVALID_PARAMETER;

  sFqnOut = sDirIn;
  #ifdef WIN32
    sFqnOut.SwapChars( '/', '\\' );
    if( sFqnOut[sFqnOut.Len()] != '\\' )
      sFqnOut += '\\';
  #else
    sFqnOut.SwapChars( '\\', '/' );
    if( sFqnOut[sFqnOut.Len()] != '/' )
      sFqnOut += '/';
  #endif
  sFqnOut += sNameIn;
  if( sExtIn != "" ){
    sFqnOut += '.';
    sFqnOut += sExtIn;
  }
  return XB_NO_ERROR;
}

/*************************************************************************/
//! @brief Parse commmand line options for a given parm request
/*!
  \param lArgc Value passed from main( argc, argv[] )
  \param sArgv Valued passed from main
  \param sOptRqst Option to search for in the arguments list
  \param sParmOut String token immediately to the right of the the option request, if found
  \returns 0 - paramater request not found<br> 1 - Parameter found
*/

xbInt16  xbXBase::GetCmdLineOpt( xbInt32 lArgc, char **sArgv, const char *sOptRqst, xbString &sParmOut ){
  xbString sOpt( sOptRqst );
  return GetCmdLineOpt( lArgc, sArgv, sOpt, sParmOut );
}

/*************************************************************************/
//! @brief Parse commmand line options for a given parm request
/*!
  \param lArgc Value passed from main( argc, argv[] )
  \param sArgv Valued passed from main
  \param sOptRqst Option to search for in the arguments list
  \param sParmOut String token immediately to the right of the the option request, if found
  \returns 0 - paramater request not found<br> 1 - Parameter found
*/

xbInt16  xbXBase::GetCmdLineOpt( xbInt32 lArgc, char **sArgv, xbString &sOptRqst, xbString &sParmOut ){

  xbInt16 iFound = 0;
  sParmOut = "";
  if( lArgc < 2 )  // first string is the program name
    return iFound;

  xbInt32 i = 1;
  while( iFound == 0 && i < lArgc ){
    if( sOptRqst == sArgv[i] ){
      iFound = 1;
      if( i < (lArgc-1))
        sParmOut = sArgv[i+1];
    }
    i++;
  }
  return iFound;
}

/*************************************************************************/
#ifdef XB_LOGGING_SUPPORT
//! @brief Get fully qualified log file name.
/*!
   \returns Returns the fully qualified log file name.
*/
const xbString & xbXBase::GetLogFqFileName() const {
  return xLog->GetFqFileName();
}

//! @brief Get the log file name.
/*!
   \returns Returns the log file name.
*/
/*
const xbString & xbXBase::GetLogFileName() const {
  return xLog->GetFileName();
}
*/

//! @brief Get the log directory.
/*!
   \returns Returns the log directory.
*/
/*
const xbString & xbXBase::GetLogDirectory() const {
  return GetLogDirectory();
}
*/
//! @brief Get the log directory.
/*!
   \returns xbTrue - Logging enabled.<br>xbFalse - Logging disables.
*/
xbBool xbXBase::GetLogStatus() const {
  return xLog->LogGetStatus();
}

//! @brief Set the log file name.
/*!
  \param sLogFileName - Log File Name.
  \return void
*/
/*
void xbXBase::SetLogFileName( const xbString & sLogFileName ){

  xLog->SetFileName( sLogFileName );
}
*/
//! @brief Set the log directory.
/*!
  \param sLogDirectory - Log File Directory.
  \return void
*/
/*
void xbXBase::SetLogDirectory( const xbString & sLogDirectory ){
  xLog->SetDirectory( sLogDirectory );
}
*/


//! @brief Set the logfile size.
/*!
  \param lSize - Log File Size.
  \return void
*/
void xbXBase::SetLogSize( size_t lSize ) {
  xLog->LogSetLogSize( lSize );
}

//! @brief Get the logfile size.
/*!
  \return log file size
*/
size_t xbXBase::GetLogSize() const {
  return xLog->LogGetLogSize();
}


//! @brief Write message to logfile.
/*!
  \param sLogMessage - Message to write.
  \param iOpt 0 = stdout<br>
              1 = Syslog<br>
              2 = Both<br>
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbXBase::WriteLogMessage( const xbString & sLogMessage, xbInt16 iOpt ){
  return xLog->LogWrite( sLogMessage, iOpt );
}

//! @brief Write message to logfile.
/*!
  \param lCnt - Number of bytes to write.
  \param p - Pointer to bytes to write to log file.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/
xbInt16 xbXBase::WriteLogBytes( xbUInt32 lCnt, const char *p ){
  return xLog->LogWriteBytes( lCnt, p );
}

//! @brief Enable message logging.
void xbXBase::EnableMsgLogging() {
  xLog->LogSetStatus( xbTrue );
}

//! @brief Disable message logging.
void xbXBase::DisableMsgLogging() {
  xLog->LogSetStatus( xbFalse );
}

//! @brief Flush log file updates to disk.
xbInt16 xbXBase::FlushLog() {
  return xLog->xbFflush();
}
#else

// if logging not compiled in, these stubs are called with no effect
const xbString & xbXBase::GetLogFqFileName() const {
  return sNullString;
}
/*
const xbString & xbXBase::GetLogFileName() const {
  return sNullString;
}
const xbString & xbXBase::GetLogDirectory() const {
  return sNullString;
}
*/
/*
void xbXBase::SetLogFileName( const xbString & sLogFileName ){
  return;
}
void xbXBase::SetLogDirectory( const xbString & sLogDirectory ){
  return;
}
*/
xbBool xbXBase::GetLogStatus() const {
  return xbFalse;
}
xbInt16 xbXBase::WriteLogMessage( const xbString & sLogMessage, xbInt16 ){
  return XB_NO_ERROR;
}
xbInt16 xbXBase::WriteLogBytes( xbUInt32 lCnt, const char *p ){
  return XB_NO_ERROR;
}
void xbXBase::EnableMsgLogging() {
  return;
}
void xbXBase::DisableMsgLogging() {
  return;
}
xbInt16 xbXBase::FlushLog() {
  return XB_NO_ERROR;
}
void xbXBase::SetLogSize( size_t lSize ) {
  return;
}

#endif   // XB_LOGGING_SUPPORT

/*************************************************************************/
#ifdef XB_FUNCTION_SUPPORT

//! @brief Get information regarding expression functions.
/*!
  \param sExpLine An expression beginning with function name.
  \param cReturnType Output - return type of function.
  \param iCalc Used to calculate the function return value is<br>
       1 = use value specified in lReturnLenVal<br>
       2 = use length of operand specified in col 4<br>
       3 = use valued of numeric operand specified in col 4<br>
       4 = length of parm 1 * numeric value parm<br>
       5 = larger length of parm 2 or length of parm 3<br>
       6 = if two or more parms, use numeric value from second parm, otherwise use col4 value
  \param lReturnLenVal Used in combination with iReturnLenCalc.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/

xbInt16 xbXBase::GetFunctionInfo( const xbString &sExpLine, char &cReturnType, xbInt16 &iCalc, xbInt32 &lReturnLenVal ) const{

  xbUInt32 iLen;
  const   char *s;
  if( sExpLine.Len() == 0 )
    return XB_INVALID_FUNCTION;

  s = sExpLine;
  iLen = 0;
  while( *s && *s != '(' && *s != ' ' ) { s++; iLen++; }
  xbString sFunction( sExpLine, iLen );
  cReturnType = 0x00;
  char cFunc1 = sFunction[1];

  if( cFunc1 < 'L' ){
    // std::cout << "less than L\n";
    if( cFunc1 < 'D' ){
       // std::cout << "less than D\n";
      if( sFunction == "ABS" ){
         // { "ABS",       'N',      1,   4 },
        cReturnType   = 'N';
        iCalc         = 1;
        lReturnLenVal = 4;
      } else if( sFunction == "ALLTRIM" ){
        // { "ALLTRIM",   'C',      2,   1 },
        cReturnType   = 'C';
        iCalc         = 2;
        lReturnLenVal = 1;
      } else if( sFunction == "ASC" ){
        // { "ASC",       'N',      1,   4 },
        cReturnType   = 'N';
        iCalc         = 1;
        lReturnLenVal = 4;
      } else if( sFunction == "AT" ){
        // { "AT",        'N',      1,   4 },
        cReturnType   = 'N';
        iCalc         = 1;
        lReturnLenVal = 4;
      } else if( sFunction == "CDOW" ){
        // { "CDOW",      'C',      1,   9 },
        cReturnType   = 'C';
        iCalc         = 1;
        lReturnLenVal = 9;
      } else if( sFunction == "CHR" ){
        // { "CHR",       'C',      1,   1 },
        cReturnType   = 'C';
        iCalc         = 1;
        lReturnLenVal = 1;
      } else if( sFunction == "CMONTH" ){
        // { "CMONTH",    'C',      1,   9 },
        cReturnType   = 'C';
        iCalc         = 1;
        lReturnLenVal = 9;
      } else if( sFunction == "CTOD" ){
        // { "CTOD",      'D',      1,   8 },
        cReturnType   = 'D';
        iCalc         = 1;
        lReturnLenVal = 8;
      }
    } else {
      // std::cout << ">= D\n";
      if( sFunction == "DATE" ){
        // { "DATE",      'D',      1,   8 },
        cReturnType   = 'D';
        iCalc         = 1;
        lReturnLenVal = 8;
      } else if( sFunction == "DAY" ){
        // { "DAY",       'N',      1,   4 },
        cReturnType   = 'N';
        iCalc         = 1;
        lReturnLenVal = 4;
      } else if( sFunction == "DEL" ){
        // { "DEL",       'C',      1,   1 },
        cReturnType   = 'C';
        iCalc         = 1;
        lReturnLenVal = 1;
      } else if( sFunction == "DELETED" ){
        //  { "DELETED",   'L',      1,   1 },
        cReturnType   = 'L';
        iCalc         = 1;
        lReturnLenVal = 1;
      } else if( sFunction == "DESCEND" ){
        // { "DESCEND",   '1',      2,   1 },
        cReturnType   = '1';
        iCalc         = 2;
        lReturnLenVal = 1;
      } else if( sFunction == "DOW" ){
        // { "DOW",       'N',      1,   4 },
        cReturnType   = 'N';
        iCalc         = 1;
        lReturnLenVal = 4;
      } else if( sFunction == "DTOC" ){
        //   { "DTOC",      'C',      1,   8 },
        cReturnType   = 'C';
        iCalc         = 1;
        lReturnLenVal = 8;
      } else if( sFunction == "DTOS" ){
        // { "DTOS",      'C',      1,   8 },
        cReturnType   = 'C';
        iCalc         = 1;
        lReturnLenVal = 8;
      } else if( sFunction == "EXP" ){
        // { "EXP",       'N',      1,   4 },
        cReturnType   = 'N';
        iCalc         = 1;
        lReturnLenVal = 4;
      } else if( sFunction == "IIF" ){
        // { "IIF",       'C',      5,   0 },
        cReturnType   = 'C';
        iCalc         = 5;
        lReturnLenVal = 0;
      } else if( sFunction == "INT" ){
        // { "INT",       'N',      1,   4 },
        cReturnType   = 'N';
        iCalc         = 1;
        lReturnLenVal = 4;
      } else if( sFunction == "ISALPHA" ){
        // { "ISALPHA",   'L',      1,   1 },
        cReturnType   = 'L';
        iCalc         = 1;
        lReturnLenVal = 1;
      } else if( sFunction == "ISLOWER" ){
        // { "ISLOWER",   'L',      1,   1 },
        cReturnType   = 'L';
        iCalc         = 1;
        lReturnLenVal = 1;
      } else if( sFunction == "ISUPPER" ){
        // { "ISUPPER",   'L',      1,   1 },
        cReturnType   = 'L';
        iCalc         = 1;
        lReturnLenVal = 1;
      }
    }
  } else {
    // std::cout << ">= L\n";
    if( cFunc1 < 'R' ) {
      // std::cout << " < R\n";
      if( sFunction == "LEFT" ){
      // { "LEFT",      'C',      3,   2 },
        cReturnType   = 'C';
        iCalc         = 3;
        lReturnLenVal = 2;
      } else if( sFunction == "LEN" ){
        // { "LEN",       'N',      1,   4 },
        cReturnType   = 'N';
        iCalc         = 1;
        lReturnLenVal = 3;
      } else if( sFunction == "LOG" ){
        // { "LOG",       'N',      1,   4 },
        cReturnType   = 'N';
        iCalc         = 1;
        lReturnLenVal = 4;
      } else if( sFunction == "LOWER" ){
        //  { "LOWER",     'C',      2,   1 },
        cReturnType   = 'C';
        iCalc         = 2;
        lReturnLenVal = 1;
      } else if( sFunction == "LTRIM" ){
        //   { "LTRIM",     'C',      2,   1 },
        cReturnType   = 'C';
        iCalc         = 2;
        lReturnLenVal = 1;
      } else if( sFunction == "MAX" ){
        // { "MAX",       'N',      1,   4 },
        cReturnType   = 'N';
        iCalc         = 1;
        lReturnLenVal = 4;
      } else if( sFunction == "MIN" ){
        //   { "MIN",       'N',      1,   4 },
        cReturnType   = 'N';
        iCalc         = 1;
        lReturnLenVal = 4;
      } else if( sFunction == "MONTH" ){
        // { "MONTH",     'N',      1,   4 },
        cReturnType   = 'N';
        iCalc         = 1;
        lReturnLenVal = 4;
      }
    } else if( cFunc1 == 'R' ){
      // std::cout << "==R\n";
      if( sFunction == "RECNO" ){
        //   { "RECNO",     'N',      1,   4 },
        cReturnType   = 'N';
        iCalc         = 1;
        lReturnLenVal = 4;
      } else if( sFunction == "RECCOUNT" ){
        //  { "RECCOUNT",  'N',      1,   4 },
        cReturnType   = 'N';
        iCalc         = 1;
        lReturnLenVal = 4;
      } else if( sFunction == "REPLICATE" ){
        // { "REPLICATE", 'C',      4,   0 },
        cReturnType   = 'C';
        iCalc         = 4;
        lReturnLenVal = 0;
      } else if( sFunction == "RIGHT" ){
        // { "RIGHT",     'C',      3,   2 },
        cReturnType   = 'C';
        iCalc         = 3;
        lReturnLenVal = 2;
      } else if( sFunction == "RTRIM" ){
        //  { "RTRIM",     'C',      2,   1 },
        cReturnType   = 'C';
        iCalc         = 2;
        lReturnLenVal = 1;
      }
    }  else if( cFunc1 == 'S' ){
      // std::cout << "==S\n";
      if( sFunction == "SPACE" ){
        // { "SPACE",     'C',      3,   1 },
        cReturnType   = 'C';
        iCalc         = 3;
        lReturnLenVal = 1;
      } else if( sFunction == "SQRT" ){
        //  { "SQRT",      'N',      1,   4 },
        cReturnType   = 'N';
        iCalc         = 1;
        lReturnLenVal = 4;
      } else if( sFunction == "STOD" ){
        // { "STOD",      'D',      1,   8 },
        cReturnType   = 'D';
        iCalc         = 1;
        lReturnLenVal = 8;
      } else if( sFunction == "STR" ){
        //  { "STR",       'C',      6,  10 },
        cReturnType   = 'C';
        iCalc         = 6;
        lReturnLenVal = 10;
      } else if( sFunction == "STRZERO" ){
        // { "STRZERO",   'C',      3,   2 },
        cReturnType   = 'C';
        iCalc         = 3;
        lReturnLenVal = 2;
      } else if( sFunction == "SUBSTR" ){
        //  { "SUBSTR",    'C',      3,   3 },
        cReturnType   = 'C';
        iCalc         = 3;
        lReturnLenVal = 3;
      }
    }  else {
      //  std::cout << ">S\n";
      if( sFunction == "TRIM" ){
        //  { "TRIM",      'C',      2,   1 },
        cReturnType   = 'C';
        iCalc         = 2;
        lReturnLenVal = 1;
      } else if( sFunction == "UPPER" ){
        // { "UPPER",     'C',      2,   1 },
        cReturnType   = 'C';
        iCalc         = 2;
        lReturnLenVal = 1;
      } else if( sFunction == "VAL" ){
        // { "VAL",       'N',      1,   4 },
        cReturnType   = 'N';
        iCalc         = 1;
        lReturnLenVal = 3;
      } else if( sFunction == "YEAR" ){
        //  { "YEAR",      'N',      1,   4 },
        cReturnType   = 'N';
        iCalc         = 1;
        lReturnLenVal = 4;
      }
    }
  }
  if( cReturnType == 0x00 )
    return XB_INVALID_FUNCTION;
  else
    return XB_NO_ERROR;
}
#endif
/*************************************************************************/
//! @brief Cross platform sleep function.
/*!
  \param lMillisecs Milliseconds to sleep.
*/
void xbXBase::xbSleep( xbInt32 lMillisecs ){
  #ifdef WIN32
  Sleep( (xbUInt32) lMillisecs );
  #else
  usleep( (xbInt64) lMillisecs * 1000 );
  #endif

}
/***********************************************************************/
//! @brief Cross memcmp function.
/*!
  \param s1 Left operand to compare.
  \param s2 Right operand to compare.
  \param n Number of bytes to compare.
  \returns 1 s1 > s2<br>
           0 s1 == s2<br>
           -1 s1 < s2
*/
xbInt16 xbXBase::xbMemcmp( const unsigned char *s1, const unsigned char *s2, size_t n ){
  // The standard memcmp function was found not to behave the same across all platforms
  for( size_t i = 0; i < n; i++ ){
    if(  s1[i] > s2[i] ) 
      return 1;
    else if( s1[i] < s2[i] ) 
      return -1;
  }
  return 0;
}

/***********************************************************************/
//! @brief Open highest qualified class available for dbf file.
/*!
  This routine opens the highest available version of the dbf file.
  Defaults to XB_READ_WRITE and XB_MULTI_USER mode.
  \returns param dbf - Output pointer to dbf file opened or null if error
*/
xbDbf* xbXBase::Open( const xbString &sTableName, xbInt16 &iRc ){
  return Open( sTableName, "", XB_READ_WRITE, XB_MULTI_USER, 0, iRc );
}
/***********************************************************************/
//! @brief Open highest qualified class available for dbf file.
/*!
  This routine can open various versions of the dbf file dependent on the iVersion field

  \param sTableName - Table name to open.
  \param sAlias - Optional alias name.
  \param iOpenMode - XB_READ_WRITE or XB_READ
  \param iShareMode - XB_SINGLE_USER or XB_MULTI_USER
  \param iRequestVersion 0 - Highest available
                         4 - Version four dbf
                         3 - Version three dbf
  \param iRc - Return code from open request
  \returns param dbf - Output pointer to dbf file opened or null if error
*/


xbDbf* xbXBase::Open( const xbString &sTableName, const xbString &sAlias, xbInt16 iOpenMode, 
                      xbInt16 iShareMode, xbInt16 iRequestVersion, xbInt16 &iRc ){

  xbInt16 iErrorStop = 0;
  xbDbf * pDbf = 0;
  iRc = 0;
  xbString sFqFileName;

  try{

    if( sTableName.Len() == 0 ){
      iErrorStop = 100;
      iRc = XB_FILE_NOT_FOUND;
      throw iRc;
    }
    xbFile *f = new xbFile(this);
    f->SetFileName( sTableName );
    if(( iRc = f->FileExists( f->GetFqFileName())) != xbTrue ){
      iErrorStop = 110;
      iRc = XB_FILE_NOT_FOUND;
      throw iRc;
    }
    unsigned char cFileTypeByte;
    if(( iRc = f->GetXbaseFileTypeByte( f->GetFqFileName(), cFileTypeByte )) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw iRc;
    }
    xbInt16 iTblVsn = f->DetermineXbaseTableVersion( cFileTypeByte );
    f->xbFclose();
    sFqFileName.Set( f->GetFqFileName() );
    delete f;

    if( iTblVsn == 4 && ( iRequestVersion == 0 || iRequestVersion == 4 )){
      #ifdef XB_DBF4_SUPPORT
        pDbf = new xbDbf4( this );
        iRc = pDbf->Open( sFqFileName, sAlias, iOpenMode, iShareMode );        
      #else
        // std::cout << "Dbase IV file support not build into library. See XB_DBF4_SUPPORT" << std::endl;
        iErrorStop = 130;
        iRc = XB_FILE_TYPE_NOT_SUPPORTED;
        throw iRc;
      #endif
    }
    else if( iTblVsn == 3 && ( iRequestVersion == 0 || iRequestVersion == 3 )){

      #ifdef XB_DBF3_SUPPORT
        pDbf = new xbDbf3( this );
        iRc = pDbf->Open( sFqFileName, sAlias, iOpenMode, iShareMode );  
      #else
        //std::cout << "Dbase III file support not build into library. See XB_DBF3_SUPPORT" << std::endl;
        iErrorStop = 140;
        iRc = XB_FILE_TYPE_NOT_SUPPORTED;
        throw iRc;
      #endif

    } else {
      iErrorStop = 150;
      iRc = XB_FILE_TYPE_NOT_SUPPORTED;
      throw iRc;
    }

    if( iRc != XB_NO_ERROR ){
      iErrorStop = 160;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbxbase::Open() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, iRc );
    WriteLogMessage( sMsg.Str() );
    WriteLogMessage( GetErrorMessage( iRc ));
  }
  return pDbf;
}

xbInt16 xbXBase::OpenHighestVersion( const xbString &sTableName, const xbString &sAlias, 
    xbDbf **dbf )
{
  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;

  try{
    xbFile f(this);
    if( sTableName.Len() == 0 ){
      iErrorStop = 100;
      iRc = XB_FILE_NOT_FOUND;
      throw iRc;
    }
    f.SetFileName( sTableName );
    if(( iRc = f.FileExists( f.GetFqFileName() )) != xbTrue ){
      iErrorStop = 110;
      iRc = XB_FILE_NOT_FOUND;
      throw iRc;
    }

    unsigned char cFileTypeByte;
    if(( iRc = f.GetXbaseFileTypeByte( f.GetFqFileName(), cFileTypeByte )) != XB_NO_ERROR ){
      iErrorStop = 120;
      return iRc;
    }
    if( f.DetermineXbaseTableVersion( cFileTypeByte ) == 4 ){
      #ifdef XB_DBF4_SUPPORT
        xbDbf *pwDbf = new xbDbf4( this );
        pwDbf->Open( f.GetFqFileName(), sAlias );
        *dbf = pwDbf;
        pwDbf = 0;
      #else
        // std::cout << "Dbase IV file support not build into library. See XB_DBF4_SUPPORT" << std::endl;
        iErrorStop = 130;
        iRc = XB_FILE_TYPE_NOT_SUPPORTED;
        throw iRc;
      #endif

    } else if( f.DetermineXbaseTableVersion( cFileTypeByte ) == 3 ){
      #ifdef XB_DBF3_SUPPORT
        *dbf = new xbDbf3( this );
      #else
        //std::cout << "Dbase III file support not build into library. See XB_DBF3_SUPPORT" << std::endl;
        iErrorStop = 140;
        iRc = XB_FILE_TYPE_NOT_SUPPORTED;
        throw iRc;
      #endif

    } else {
      iErrorStop = 150;
      iRc = XB_FILE_TYPE_NOT_SUPPORTED;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbxbase::OpenHighestVersion() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, iRc );
    WriteLogMessage( sMsg.Str() );
    WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}
/***********************************************************************/
}        /* namespace */
