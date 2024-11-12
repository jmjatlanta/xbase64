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
xbXBase::xbXBase() {
  SetEndianType();

  xbFile f( this );
  f.SetHomeFolders();

  xbDate d( -2 );      // initialize xbDate static variables

  #ifdef XB_LOGGING_SUPPORT
  xLog = new xbLog();
  #endif

  #ifdef XB_LOCKING_SUPPORT
    SetMultiUser( xbOn );
  #else
    SetMuliUser( xbOff );
  #endif
}
/*************************************************************************/
xbXBase::~xbXBase(){
  CloseAllTables();
  #ifdef XB_LOGGING_SUPPORT
  delete xLog;
  #endif
}
/*************************************************************************/
xbInt16 xbXBase::CloseAllTables(){

  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;
  xbInt16 iOpenTableCnt = GetOpenTableCount();
  try{
    xbDbf *d;
    for( xbInt16 i = 0; i < iOpenTableCnt; i++ ){
      d = (xbDbf *) GetDbfPtr( i+1 );
      if( d ){
        if(( iRc = d->Close()) != XB_NO_ERROR ){
          iErrorStop = 100;
          throw iRc;
        }
        // delete d;
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
void xbXBase::DisplayError( xbInt16 iErrorCode ) const {
  std::cout << (const char *) GetErrorMessage( iErrorCode ) << std::endl;
}
/*************************************************************************/
xbInt16  xbXBase::GetCmdLineOpt( xbInt32 lArgc, char **sArgv, const char *sOptRqst, xbString &sParmOut ){
  xbString sOpt( sOptRqst );
  return GetCmdLineOpt( lArgc, sArgv, sOpt, sParmOut );
}
/*************************************************************************/
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
const char * xbXBase::GetErrorMessage( xbInt16 iErrorCode ) const{

  if( iErrorCode > 0 || iErrorCode <= XB_MAX_ERROR_NO )
    return "";

   xbBool bFound = xbFalse;
   xbInt16 iCtr = 0;
   while( !bFound ){
     if( xbErrorMessages[iCtr].iErrorNo == XB_MAX_ERROR_NO )
       return "Unknown Error";
     if( xbErrorMessages[iCtr].iErrorNo == iErrorCode )
       return xbErrorMessages[iCtr].sErrorText;
     iCtr++;
   }
   return "";
}

/*************************************************************************/
#ifdef XB_LOGGING_SUPPORT
const xbString & xbXBase::GetLogFqFileName() const {
  return xLog->GetFqFileName();
}
xbString & xbXBase::GetLogFileName() const {
  return sLogFileName;
}
void xbXBase::SetLogFileName( const xbString & sLogFileName ){
  this->sLogFileName = sLogFileName;
}
/*************************************************************************/
xbString & xbXBase::GetLogDirectory() const {
  return sLogDirectory;
}
/*************************************************************************/
void xbXBase::SetLogDirectory( const xbString &sLogDirectoryIn ){
  this->sLogDirectory = sLogDirectoryIn;
  #ifdef WIN32
    this->sLogDirectory.SwapChars( '/', '\\' );
  #else
    this->sLogDirectory.SwapChars( '\\', '/' );
  #endif
}
xbBool xbXBase::GetLogStatus() const {
  return xLog->LogGetStatus();
}
void xbXBase:: SetLogSize( size_t lSize ) {
  xLog->LogSetLogSize( lSize );
}
size_t xbXBase::GetLogSize() const {
  return xLog->LogGetLogSize();
}
xbInt16 xbXBase::WriteLogMessage( const xbString & sLogMessage, xbInt16 iOpt ){
  return xLog->LogWrite( sLogMessage, iOpt );
}

xbInt16 xbXBase::WriteLogBytes( xbUInt32 lCnt, const char *p ){
  return xLog->LogWriteBytes( lCnt, p );
}

void xbXBase::EnableMsgLogging() {
  xLog->LogSetStatus( xbTrue );
}

void xbXBase::DisableMsgLogging() {
  xLog->LogSetStatus( xbFalse );
}

xbInt16 xbXBase::FlushLog() {
  return xLog->xbFflush();
}
#else

// if logging not compiled in, these stubs are called with no effect
const xbString & xbXBase::GetLogFqFileName() const {
  return sNullString;
}
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
void xbXBase::xbSleep( xbInt32 lMillisecs ){
  #ifdef WIN32
  Sleep( (xbUInt32) lMillisecs );
  #else
  usleep( (xbInt64) lMillisecs * 1000 );
  #endif

}
/***********************************************************************/
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
xbInt16 xbXBase::Open( const xbString &sTableName, const xbString &sAlias, xbDbf **dbf ){

  xbInt16 iRc;

  *dbf = Open( sTableName, sAlias, XB_READ_WRITE, XB_MULTI_USER, 0, iRc );

  return iRc;
}


/***********************************************************************/
xbDbf* xbXBase::Open( const xbString &sTableName, xbInt16 &iRc ){
  return Open( sTableName, "", XB_READ_WRITE, XB_MULTI_USER, 0, iRc );
}
/***********************************************************************/
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
        // std::cout << "dBASE IV file support not build into library. See XB_DBF4_SUPPORT" << std::endl;
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
        //std::cout << "dBASE III file support not build into library. See XB_DBF3_SUPPORT" << std::endl;
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

/*
xbInt16 xbXBase::OpenHighestVersion( const xbString &sTableName, const xbString &sAlias, xbDbf **dbf )
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
        // std::cout << "dBASE IV file support not build into library. See XB_DBF4_SUPPORT" << std::endl;
        iErrorStop = 130;
        iRc = XB_FILE_TYPE_NOT_SUPPORTED;
        throw iRc;
      #endif

    } else if( f.DetermineXbaseTableVersion( cFileTypeByte ) == 3 ){
      #ifdef XB_DBF3_SUPPORT
        *dbf = new xbDbf3( this );
      #else
        //std::cout << dBASE III file support not build into library. See XB_DBF3_SUPPORT" << std::endl;
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
    sMsg.Sprintf( "xbXBase::OpenHighestVersion() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, iRc );
    WriteLogMessage( sMsg.Str() );
    WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}
*/
/***********************************************************************/
#ifdef XB_LOCKING_SUPPORT

xbInt16 xbXBase::GetDefaultLockRetries() const {
  return iDefaultLockRetries;
}
void xbXBase::SetDefaultLockRetries( xbInt16 iDefaultLockRetries ) {
  this->iDefaultLockRetries = iDefaultLockRetries;
}

//xbBool xbXBase::GetDefaultAutoLock() const {
//  return bDefaultAutoLock;
//}
//void xbXBase::SetDefaultAutoLock( xbBool bDefaultAutoLock ) {
//  this->bDefaultAutoLock = bDefaultAutoLock;
//}
//void xbXBase::EnableDefaultAutoLock() {
//  this->bDefaultAutoLock = xbTrue;
//}
//void xbXBase::DisableDefaultAutoLock() {
//  bDefaultAutoLock = xbFalse;
//}
/***************************************************************************/
xbInt16 xbXBase::GetDefaultLockFlavor() const {
  return iDefaultLockFlavor;
}
/***************************************************************************/
void xbXBase::SetDefaultLockFlavor( xbInt16 iDefaultLockFlavor ) {
  this->iDefaultLockFlavor = iDefaultLockFlavor;
}
/***************************************************************************/
void xbXBase::SetDefaultLockWait( xbInt32 lLockWait ) {
  this->lDefaultLockWait = lLockWait;
}
/***************************************************************************/
xbInt32 xbXBase::GetDefaultLockWait() const {
  return lDefaultLockWait;
}
#endif

/************************************************************************/
#ifdef XB_MDX_SUPPORT
xbInt16 xbXBase::GetCreateMdxBlockSize() const {
  return iCreateMdxBlockSize;
}
/************************************************************************/
xbInt16 xbXBase::SetCreateMdxBlockSize( xbInt16 iBlockSize ){

  if( iBlockSize < 512 || iBlockSize > 16384 || iBlockSize % 512  )
    return XB_INVALID_BLOCK_SIZE;
  else
    iCreateMdxBlockSize = iBlockSize;

  return XB_NO_ERROR;
}
#endif
/*************************************************************************/
xbBool xbXBase::GetDefaultAutoCommit() const {
  return bDefaultAutoCommit;
}
/*************************************************************************/
xbString &xbXBase::GetDataDirectory() const {
  return sDataDirectory;
}
/*************************************************************************/
xbString & xbXBase::GetDefaultDateFormat() const {
  return sDefaultDateFormat;
}
/*************************************************************************/
#ifdef XB_DBF5_SUPPORT
xbString & xbXBase::GetDefaultTimeFormat() const {
  return sDefaultTimeFormat;
}
#endif  // XB_DBF5_SUPPORT
/*************************************************************************/
xbInt16  xbXBase::GetEndianType() const {
  return iEndianType;
}
/*************************************************************************/
void xbXBase::GetHomeDir( xbString &sHomeDirOut ){

  #if defined(HAVE_GETENV_S_F)
    char sPath[MAX_PATH];
    size_t lSize;
    sHomeDirOut = "";
    memset( sPath, 0x00, MAX_PATH );
    getenv_s( &lSize, NULL, 0, "HOMEDRIVE" );
    if( lSize > 0 ){
      getenv_s( &lSize, sPath, lSize, "HOMEDRIVE" );
      sHomeDirOut = sPath;
      memset( sPath, 0x00, MAX_PATH );
    }
    getenv_s( &lSize, NULL, 0, "HOMEPATH" );
    if( lSize > 0 ){
      getenv_s( &lSize, sPath, lSize, "HOMEPATH" );
      sHomeDirOut += sPath;
    }
    if( sHomeDirOut == "" )
      sHomeDirOut = "C:\xbase64";

  #elif defined(WIN32)
    sHomeDirOut.Sprintf( "%s%s", getenv( "HOMEDRIVE" ), getenv( "HOMEPATH" ));

  #else
    sHomeDirOut.Sprintf( "%s", getenv( "HOME" ));
    sHomeDirOut.Trim();
    if( sHomeDirOut == "" )
      sHomeDirOut.Sprintf( "%s", getpwuid( getuid())->pw_dir );
  #endif

  sHomeDirOut.Trim();
}
/*************************************************************************/
void xbXBase::SetTempDirectory( const xbString &sTempDirectory ){
  this->sTempDirectory = sTempDirectory;
  #ifdef WIN32
    this->sTempDirectory.SwapChars( '/', '\\' );
  #else
    this->sTempDirectory.SwapChars( '\\', '/' );
  #endif
}
/*************************************************************************/
void xbXBase::SetDataDirectory( const xbString &sDataDirectory ){
  this->sDataDirectory = sDataDirectory;
  #ifdef WIN32
    this->sDataDirectory.SwapChars( '/', '\\' );
  #else
    this->sDataDirectory.SwapChars( '\\', '/' );
  #endif
}
/***************************************************************************/
xbBool xbXBase::GetMultiUser() const {
  return bMultiUser;
}
xbInt16 xbXBase::SetMultiUser( xbBool bMultiUser ) {

  #ifndef XB_LOCKING_SUPPORT
  if( bMultiUser != xbOff )
    return XB_INVALID_OPTION;
  #endif

  if( bMultiUser != xbOn && bMultiUser != xbOff )
    return XB_INVALID_OPTION;

  //  check for any open files, return XB_NOT_CLOSED if any open tables
  xbInt16 iOpenTableCnt = GetOpenTableCount();

  xbDbf *d;
  for( xbInt16 i = 0; i < iOpenTableCnt; i++ ){
    d = (xbDbf *) GetDbfPtr( i+1 );
    if( d ){
      if( d->GetDbfStatus() != XB_CLOSED ){
        return XB_NOT_CLOSED;
      }
     }
  }

  this->bMultiUser = bMultiUser;
  return XB_NO_ERROR;
}
/*************************************************************************/
char xbXBase::GetPathSeparator() const {
  #ifdef WIN32
  return '\\';
  #else
  return '/';
  #endif
}
/*************************************************************************/
void xbXBase::SetDefaultDateFormat( const xbString &sDefaultDateFormat ) {
  this->sDefaultDateFormat = sDefaultDateFormat;
}
/*************************************************************************/
xbString &xbXBase::GetTempDirectory() const {
  return sTempDirectory;
}
/*************************************************************************/
#ifdef XB_DBF5_SUPPORT
void xbXBase::SetDefaultTimeFormat( const xbString &sDefaultTimeFormat ) {
  this->sDefaultTimeFormat = sDefaultTimeFormat;
}
#endif // XB_DBF5_SUPPORT
/*************************************************************************/
void xbXBase::SetDefaultAutoCommit( xbBool bDefaultAutoCommit ) {
  this->bDefaultAutoCommit = bDefaultAutoCommit;
}
/************************************************************************/
#ifdef XB_BLOCKREAD_SUPPORT
xbUInt32 xbXBase::GetDefaultBlockReadSize() const {
  return ulDefaultBlockReadSize;
}
void xbXBase::SetDefaultBlockReadSize( xbUInt32 ulDfltBlockReadSize ){
   ulDefaultBlockReadSize = ulDfltBlockReadSize;
}
#endif  // XB_BLOCKREAD_SUPPORT
/************************************************************************/
#if defined (XB_NDX_SUPPORT) || defined (XB_MDX_SUPPORT)

xbInt16 xbXBase::GetDefaultIxTagMode() const {
  return iDefaultIxTagMode;
}
xbInt16 xbXBase::SetDefaultIxTagMode( xbInt16 iMode ){

  if( iMode != XB_IX_DBASE_MODE && iMode != XB_IX_XBASE_MODE )
    return XB_INVALID_OPTION;

  iDefaultIxTagMode = iMode;
  return XB_NO_ERROR;
}

#endif  // defined (XB_NDX_SUPPORT) || defined (XB_MDX_SUPPORT)
/*************************************************************************/
void xbXBase::SetEndianType() {
  xbInt16 e = 1;
  iEndianType = *(char *) &e;
  if( iEndianType )
    iEndianType = 'L';
  else
    iEndianType = 'B';
  return;
}



}        /* namespace */
