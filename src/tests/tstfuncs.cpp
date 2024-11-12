/*  tstfuncs.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

time_t timer;

xbDouble ChronTime();
void InitTime();

xbBool dblEquals( xbDouble a, xbDouble b, xbDouble epsilon );
xbInt16 TestMethod( xbInt16 PrintOption, const char * title, const xbString &result, const char * expectedResult, size_t expectedLen );
xbInt16 TestMethod( xbInt16 PrintOption, const char * title, char result, char expectedResult );
xbInt16 TestMethod( xbInt16 PrintOption, const char * title, xbInt32 result, xbInt32 expectedResult );
//xbInt16 TestMethod( xbInt16 PrintOption, const char * title, xbUInt32 result, xbUInt32 expectedResult );
xbInt16 TestMethod( xbInt16 PrintOption, const char * title, xbUInt64 result, xbUInt64 expectedResult );
xbInt16 TestMethod( xbInt16 PrintOption, const char * title, xbFloat result, xbFloat expectedResult );
xbInt16 TestMethod( xbInt16 PrintOption, const char * title, xbDouble result, xbDouble expectedResult );
xbInt16 TestMethod( xbInt16 PrintOption, const char * title, xbDouble result, xbDouble expectedResult, xbDouble dEpsilon );


// xbInt16 FileCompare( xbXBase &x, xbInt16 iPo, const xbString &sFile1, const xbString &sFile2, const xbString &sSkipBytes );


#ifdef XB_LOCKING_SUPORT
xbInt16 SetCmd( xbXBase &x, const xbString &sFileName, const xbString &sCmd, const char cSrc, xbInt16 iPo );
xbInt16 GetCmd( xbXBase &x, const xbString &sFileName, xbString &sCmd, const char cSrc, xbInt16 iPo );
#endif

#if defined( XB_DBF4_SUPPORT ) && defined( XB_MEMO_SUPPORT )

xbInt16 TestDbt4Method( xbInt16 PrintOption, const char * title, xbMemo * m, xbUInt32 ulHdrNext, xbUInt32 ulBlockCnt, xbString sNodeChain );
xbInt16 TestDbt4Method( xbInt16 PrintOption, const char * title, xbMemo * m, xbUInt32 ulHdrNext, xbUInt32 ulBlockCnt, xbString sNodeChain ){

  xbUInt32 ulLastDataBlock = 0L;
  xbUInt32 ulHdrNextBlock = 0L;
  xbInt16  iRc = 0;

  iRc = m->GetHdrNextBlock( ulHdrNextBlock );
  if( iRc != XB_NO_ERROR ){
    std::cout << std::endl << "[FAIL 1] " << title << std::endl;
    std::cout << "Error [" << iRc << "] in GetHdrNextBlock" << std::endl;
    return -1;
  }

  iRc = m->CalcLastDataBlock( ulLastDataBlock );
  if( iRc != XB_NO_ERROR ){
    std::cout << std::endl << "[FAIL 2] " << title << std::endl;
    std::cout << "Error [" << iRc << "] in CalcLastDataBlock" << std::endl;
    return -1;
  }

  if( ulHdrNext > 0 ){
    if( ulHdrNextBlock != ulHdrNext ){
      std::cout << std::endl << "[FAIL 3] " << title << std::endl;
      std::cout << "Expected Header Next Block = [" << ulHdrNext << "] Actual = [" << ulHdrNextBlock << "]" << std::endl;
      return -1;
    }
    if( PrintOption == 2 ){
      xbString sMsg;
      sMsg.Sprintf( "[PASS][%3.4f] %s Expected matches actual Len=[%ld] Data=[%lds]\n", 
         ChronTime(), title, ulHdrNextBlock, ulHdrNext );
     std::cout << sMsg.Str();
     // std::cout << "[PASS][" << ChronTime() << "] " << title << " Expected matches actual HdrBlock=[" << ulHdrNextBlock << "] Data=[" << ulHdrNext << "]" << std::endl;
    } else if( PrintOption == 1 )
      std::cout << "[PASS] " << title << std::endl;
  }

  if( ulBlockCnt > 0 ){
    if( ulLastDataBlock != ulBlockCnt ){
      std::cout << std::endl << "[FAIL 4] " << title << std::endl;
      std::cout << "Expected Block Count = [" << ulBlockCnt << "] Actual = [" << ulLastDataBlock << "]" << std::endl;
      return -1;
    }
    if( PrintOption == 2 ){
      xbString sMsg;
      sMsg.Sprintf( "[PASS][%3.4f] %s Expected matches actual Len=[%ld] Data=[%lds]\n",
         ChronTime(), title, ulLastDataBlock, ulBlockCnt );
     std::cout << sMsg.Str();
     // std::cout << "[PASS][" << ChronTime() << "] " << title << " Expected matches actual LastBlock=[" << ulLastDataBlock << "] Data=[" << ulBlockCnt << "]" << std::endl;
    } else if( PrintOption == 1 )
      std::cout << "[PASS] " << title << std::endl;
  }

  #ifdef XB_DEBUG_SUPPORT
  xbMemoDbt4 *m4 = (xbMemoDbt4 *) m;
  if( sNodeChain != "-1" ){
    xbUInt32 ulNode = ulHdrNextBlock;
    xbUInt32 ulNextBlock;
    xbUInt32 ulFreeBlockCnt;
    xbString sActualNodeChain;

    while( ulNode < ulLastDataBlock ){
      iRc = m4->ReadFreeBlockHeader( ulNode, ulNextBlock, ulFreeBlockCnt ); 
      if( iRc != XB_NO_ERROR ){
        std::cout << std::endl << "[FAIL 5] " << title << std::endl;
        std::cout << "Error [" << iRc << "] in CalcLastDataBlock" << std::endl;
        return -1;
      }
      if( sActualNodeChain != "" )
        sActualNodeChain += ",";
      sActualNodeChain.Sprintf( "%s%ld,%ld,%ld", sActualNodeChain.Str(), ulNode, ulFreeBlockCnt, ulNextBlock );
      ulNode = ulNextBlock;
    }
  if( sNodeChain != sActualNodeChain ){
    std::cout << std::endl << "[FAIL 6] " << title << std::endl;
    std::cout << "Expected Block Chain = [" << sNodeChain.Str() << "] Actual = [" << sActualNodeChain.Str() << "]" << std::endl;
    return -1;
  }
  if( PrintOption == 2 ){
    xbString sMsg;
    sMsg.Sprintf( "[PASS][%3.4f] %s Expected matches actual Len=[%d] Data=[%s]\n", ChronTime(), title, sNodeChain.Str() );
    std::cout << sMsg.Str();
    //std::cout << "[PASS][" << ChronTime() << "] " << title << " Expected matches actual BlockChain=[" << sNodeChain.Str() << "]" << std::endl;
  } else if( PrintOption == 1 )
    std::cout << "[PASS] " << title << std::endl;
  }
  #endif  // XB_DEBUG_SUPPORT"

  return 0;
}
#endif



xbInt16 TestMethod( xbInt16 PrintOption, const char * title, const xbString &sResult, const char * expectedResult, size_t expectedLen ){
  xbInt16 rc = 0;

  if( sResult == expectedResult && sResult.Len() == expectedLen ){
    if( PrintOption == 2 ){
     xbString sMsg;
     sMsg.Sprintf( "[PASS][%3.4f] %s Expected matches actual Len=[%d] Data=[%s]\n", ChronTime(), title, expectedLen, expectedResult );
     std::cout << sMsg.Str();
    } else if( PrintOption == 1 ) {
      std::cout << "[PASS] " << title << std::endl;
    }
  } else {
    std::cout << std::endl << "[FAIL 1] " << title << std::endl;
    std::cout << "  Expected Len = [" << expectedLen  << "] Expected Data = [" <<  expectedResult << "]" << std::endl;
    std::cout << "  Actual Len   = [" << sResult.Len() << "] Actual Data   = [" << sResult.Str() << "]" << std::endl;

    if( sResult.Len() == expectedLen )
      printf( "lengths match\n" );
    else
      printf( "lengths dont match\n" );

    if( sResult == expectedResult )
      printf( "result matches\n" );
    else
      printf( "result does not match\n" );

    rc = -1;
  }
  return rc;
}

xbInt16 TestMethod( xbInt16 PrintOption, const char * title, char result, char expectedResult ){
  xbInt16 rc = 0;
  if( result == expectedResult ){
    if( PrintOption == 2 ){
      xbString sMsg;
      sMsg.Sprintf( "[PASS][%3.4f] %s Expected matches actual Data=[%c]\n", ChronTime(), title,  expectedResult );
      std::cout << sMsg.Str();
      // std::cout << "[PASS][" << ChronTime() << "] " << title << " Expected matches actual Data=[" << expectedResult << "]" << std::endl;
    } else if( PrintOption == 1 )
      std::cout << "[PASS] " << title << std::endl;
    rc = 0;
  } else {
    std::cout << std::endl << "[FAIL 2] " << title << std::endl;
    std::cout << "  Expected Data = [" <<  expectedResult << "]" << std::endl;
    std::cout << "  Actual Data   = [" << result << "]" << std::endl;
    rc = -1;
  }
  return rc;
}

xbInt16 TestMethod( xbInt16 PrintOption, const char * title, xbInt32 result, xbInt32 expectedResult ){
  xbInt16 rc = 0;
  if( result == expectedResult ){
    if( PrintOption == 2 ){
      xbString sMsg;
      //sMsg.Sprintf( "[PASS][%3.4f] %s Expected matches actual Data=[%d]\n", ChronTime(), title,  expectedResult );

      sMsg.Sprintf( "[PASS] %s Expected matches actual Data=[%d]\n", title,  expectedResult );
      std::cout << sMsg.Str();
      //  std::cout << "[PASS][" << ChronTime() << "] "  << title << " Expected matches actual Data=[" << expectedResult << "]" << std::endl;
    } else if( PrintOption == 1 )
      std::cout << "[PASS] " << title << std::endl;
    rc = 0;
  } else {
    std::cout << std::endl << "[FAIL 3] " << title << std::endl;
    std::cout << "  Expected Data = [" <<  expectedResult << "]" << std::endl;
    std::cout << "  Actual Data   = [" << result << "]" << std::endl;
    rc = -1;
  }
  return rc;
}

xbInt16 TestMethod( xbInt16 PrintOption, const char * title, xbUInt64 result, xbUInt64 expectedResult ){
  xbInt16 rc = 0;
  if( result == expectedResult ){
    if( PrintOption == 2 ){
      xbString sMsg;
      //sMsg.Sprintf( "[PASS][%3.4f] %s Expected matches actual Data=[%d]\n", ChronTime(), title,  expectedResult );

      sMsg.Sprintf( "[PASS] %s Expected matches actual Data=[%d]\n", title,  expectedResult );
      std::cout << sMsg.Str();
      //  std::cout << "[PASS][" << ChronTime() << "] "  << title << " Expected matches actual Data=[" << expectedResult << "]" << std::endl;
    } else if( PrintOption == 1 )
      std::cout << "[PASS] " << title << std::endl;
    rc = 0;
  } else {
    std::cout << std::endl << "[FAIL 4] " << title << std::endl;
    std::cout << "  Expected Data = [" <<  expectedResult << "]" << std::endl;
    std::cout << "  Actual Data   = [" << result << "]" << std::endl;
    rc = -1;
  }
  return rc;
}


/*
xbInt16 TestMethod( xbInt16 PrintOption, const char * title, xbUInt32 result, xbUInt32 expectedResult ){
  xbInt16 rc = 0;
  if( result == expectedResult ){
    if( PrintOption == 2 )
      std::cout << "[PASS][" << ChronTime() << "] "  << title << " Expected matches actual Data=[" << expectedResult << "]" << std::endl;
    else if( PrintOption == 1 )
      std::cout << "[PASS] " << title << std::endl;
    rc = 0;
  } else {
    std::cout << std::endl << "[FAIL 3] " << title << std::endl;
    std::cout << "  Expected Data = [" <<  expectedResult << "]" << std::endl;
    std::cout << "  Actual Data   = [" << result << "]" << std::endl;
    rc = -1;
  }
  return rc;
}
*/


xbInt16 TestMethod( xbInt16 PrintOption, const char * title, xbFloat result, xbFloat expectedResult ){
  xbInt16 rc = 0;
  if( result == expectedResult ){
    if( PrintOption == 2 ){ 
      xbString sMsg;
      sMsg.Sprintf( "[PASS][%f] %3.4s Expected matches actual Data=[%f]\n", ChronTime(), title, expectedResult );
      std::cout << sMsg.Str();
      // std::cout << "[PASS][" << ChronTime() << "] " << title << " Expected matches actual Data=[" << expectedResult << "]" << std::endl;
    } else if( PrintOption == 1 )
      std::cout << "[PASS] " << title << std::endl;
    rc = 0;
  } else {
    std::cout << std::endl << "[FAIL 5] " << title << std::endl;
    std::cout << "  Expected Data = [" <<  expectedResult << "]" << std::endl;
    std::cout << "  Actual Data   = [" << result << "]" << std::endl;
    rc = -1;
  }
  return rc;
}

xbInt16 TestMethod( xbInt16 PrintOption, const char * title, xbDouble result, xbDouble expectedResult ){
  xbInt16 rc = 0;
  if( result == expectedResult ){
  // if( dblEquals( result, expectedResult ) == 0 ){
    if( PrintOption == 2 ){ 
      xbString sMsg;
      sMsg.Sprintf( "[PASS][%3.4f] %s Expected matches actual Data=[%f]\n", ChronTime(), title, expectedResult );
      std::cout << sMsg.Str();
      //std::cout << "[PASS][" << ChronTime() << "] " << title << " Expected matches actual Data=[" << expectedResult << "]" << std::endl;
    } else if( PrintOption == 1 )
      std::cout << "[PASS] " << title << std::endl;
    rc = 0;
  } else {
    std::cout << std::endl << "[FAIL 6] " << title << std::endl;
    std::cout << "  Expected Data = [" <<  expectedResult << "]" << std::endl;
    std::cout << "  Actual Data   = [" << result << "]" << std::endl;
    rc = -1;
  }
  return rc;
}


// next three routines used to do a compare on double values
xbDouble dblMax( xbDouble a, xbDouble b ){
  if( a > b )
    return a;
  else
    return b;
}
xbDouble dblMin( xbDouble a, xbDouble b ){
  if( a < b )
    return a;
  else
    return b;
}
xbBool dblEquals( xbDouble a, xbDouble b, xbDouble epsilon = 0.0001 ){

  // std::cout << "a=[" << a << "] b=[ " << b << "] dblmax [" << dblAbsMax( a, b ) << "] min [" << dblAbsMin( a, b ) << "]\n";
  if( a < 0 && b >= 0 )
    return xbFalse;
  else if( a >= 0 && b < 0 )
    return xbFalse;
  if( (dblMax( a ,b ) - dblMin( a, b )) < epsilon )
    return xbTrue;
  else
  { 
//    std::cout << "no match a=[" << a << "] b=[" << b << "] dblmax [" << dblAbsMax( a, b ) << "] min [" << dblAbsMin( a, b ) << "]\n";
    return xbFalse;
  }
}

xbInt16 TestMethod( xbInt16 PrintOption, const char * title, xbDouble result, xbDouble expectedResult, xbDouble dEpsilon ){
  xbInt16 rc;
  if( dblEquals( result, expectedResult, dEpsilon ) == xbTrue ){
    if( PrintOption == 2 ){
     xbString sMsg;
     sMsg.Sprintf( "[PASS][%3.4f] %s Expected matches actual Data=[%f]\n", ChronTime(), title, expectedResult );
     std::cout << sMsg.Str();
      //std::cout << "[PASS][" << ChronTime() << "] " << title << " Expected matches actual Data=[" << expectedResult << "]" << std::endl;
    } else if( PrintOption == 1 )
      std::cout << "[PASS] " << title << std::endl;
    rc = 0;
  } else {
    std::cout << std::endl << "[FAIL 6] " << title << std::endl;
    std::cout << "  Expected Data = [" <<  expectedResult << "]" << std::endl;
    std::cout << "  Actual Data   = [" << result << "]" << std::endl;
    rc = -1;
  }
  return rc;
}


#ifdef XB_LOCKING_SUPPORT

xbInt16 GetCmd( xbXBase &x, const xbString &sFileName, xbString &sCmd, const char cSrc, xbInt16 iPo ){
  xbInt16  iTryCnt = 0;
  xbInt16  iMaxTries = 10;
  char     cInBuf[256];
  FILE     *f;
  xbBool   bDone = xbFalse;
  xbString sMsg;

  while( !bDone && iTryCnt < iMaxTries ){

    #ifdef HAVE__FSOPEN_F
    // 0x40 is SH_DENYNO or _SH_DENYNO
    if(( f = _fsopen( sFileName.Str(), "r", 0x40 )) == NULL){
      sMsg.Sprintf( "GetCmd _fsopen() error opening [%s]", sFileName.Str() );
      x.WriteLogMessage( sMsg );
      std::cout << sMsg.Str() << "\n";
      x.xbSleep( 250 );
      iTryCnt++;
    }

    #else

    if(( f = fopen( sFileName.Str(), "r" )) == NULL ){
      sMsg.Sprintf( "GetCmd fopen() error opening [%s]", sFileName.Str() );
      x.WriteLogMessage( sMsg );
      std::cout << sMsg.Str() << "\n";
      x.xbSleep( 250 );
      iTryCnt++;
    }
    #endif

    else {
      memset( cInBuf, 0x00, 256 );
      fgets( cInBuf, 256, f );
      fclose( f );
      sCmd = cInBuf;
      if( sCmd != "" )
        bDone = xbTrue;
      else{
        x.xbSleep( 250 );
      }
    }
  }

  #ifdef XB_LOGGING_SUPPORT
  if( iPo > 0 ){
    sMsg.Sprintf( "[%c] GetCmd [%s]", cSrc, sCmd.Str());
    x.WriteLogMessage( sMsg );
    x.FlushLog();
  }
  #endif

  if( iTryCnt >= iMaxTries )
    return -1;
  else
    return 0;
}

xbInt16 SetCmd( xbXBase &x, const xbString &sFileName, const xbString &sCmd, const char cSrc, xbInt16 iPo ){

  xbInt16 iTryCnt = 0;
  FILE    *f;
  xbBool  bDone = xbFalse;

  #ifdef XB_LOGGING_SUPPORT
  if( iPo > 0 ){
    xbString sMsg;
    sMsg.Sprintf( "[%c] SetCmd [%s] FileName [%s]", cSrc, sCmd.Str(), sFileName.Str());
    x.WriteLogMessage( sMsg );
    x.FlushLog();
  }
  #endif

  while( !bDone && iTryCnt < 10 ){

    #ifdef HAVE__FSOPEN_F
    // 0x40 is SH_DENYNO or _SH_DENYNO
    if(( f = _fsopen( sFileName.Str(), "w", 0x40 )) == NULL){
      x.xbSleep( 250 );
      iTryCnt++;
    }

    #else
    if(( f = fopen( sFileName.Str(), "w" )) == NULL ){
      x.xbSleep( 250 );
      iTryCnt++;
    }
    #endif

    else {
      fputs( sCmd.Str(), f );
      fflush( f );
      fclose( f );
      bDone = xbTrue;
    }
  }
  if( iTryCnt == 10 )
    return -1;
  else{
    x.xbSleep( 250 );
    return 0;
  }
}



#endif
void InitTime(){
  time( &timer );
}


xbDouble ChronTime(){
  time_t tWork = timer;
  time( &timer );
  return difftime( timer, tWork );

}


/*
xbInt16 FileCompare( xbXBase &x, xbInt16 iPo, const xbString &sFile1, const xbString &sFile2, const xbString &sSkipBytes ){


//  iPo print option
//  sFile1 - File 1
//  sFile2 - File 2
//  sSkipBytes - comma separated list of bytes to skip the compare on


  xbInt16  iRc = 0;
  xbInt16  iErrorStop = 0;
  xbInt16  iErrorCnt = 0;
  xbFile   f1( &x );
  xbFile   f2( &x );
  xbString sMsg;
  char     c1;
  char     c2;

  if( iPo > 0 ){
    std::cout << "FileCompare\n";
    std::cout << "Skip bytes = " << sSkipBytes.Str() << std::endl;
    std::cout << sFile1.Str() << std::endl;
    std::cout << sFile2.Str() << std::endl;
  }

  xbLinkListOrd<xbInt32> llO;
  llO.SetDupKeys( 0 );
  if( sSkipBytes.Len() > 0 ){
    xbString sNode;
    xbUInt32 iCommaCnt = sSkipBytes.CountChar( ',' );
    for( xbUInt32 i = 0; i < (iCommaCnt+1); i++ ){
      sNode.ExtractElement( sSkipBytes, ',', i+1, 0 );
      //std::cout << "Adding key = " << atol( sNode.Str()) << std::endl;
      llO.InsertKey( atol( sNode.Str()));
    }
  }

  xbInt32 iPos = 0;
  try{

    if(( iRc = f1.xbFopen( "r", sFile1, XB_SINGLE_USER )) != XB_NO_ERROR ){
      iErrorStop = 10;
      iErrorCnt++;
      throw iRc;
    }

    if(( iRc = f2.xbFopen( "r", sFile2, XB_SINGLE_USER )) != XB_NO_ERROR ){
      iErrorStop = 20;
      iErrorCnt++;
      throw iRc;
    }

    xbUInt64 uiFs1;
    xbUInt64 uiFs2;

    if(( iRc = f1.GetFileSize( uiFs1 )) != XB_NO_ERROR ){
      iErrorStop = 30;
      iErrorCnt++;
      throw iRc;
    }

    if(( iRc = f2.GetFileSize( uiFs2 )) != XB_NO_ERROR ){
      iErrorStop = 40;
      iErrorCnt++;
      throw iRc;
    }

   // std::cout << "file size " <<  uiFs1 << " " <<  uiFs2 << "\n";

    f1.xbRewind();
    f2.xbRewind();

    while( !f1.xbFeof() && !f2.xbFeof()){
      f1.xbFgetc( c1 );
      f2.xbFgetc( c2 );
      if( c1 != c2 ){
        if( !llO.KeyExists( iPos )){
          iErrorCnt++;
          if( iPo > 0 ){
            sMsg.Sprintf( "Diff  %ld   %x  %x\n", iPos, c1, c2 );
            std::cout << sMsg.Str();
          }
        }
      }
      iPos++;
    }
  }

  catch( xbInt16 ){
    if( iPo > 0 ){
      std::cout << "File Compare Error Count = " << iErrorCnt << std::endl;

      switch( iErrorStop ){
        case 10:
          std::cout << "Error opening file = " << sFile1.Str() << std::endl;
          break;
        case 20:
          std::cout << "Error opening file = " << sFile2.Str() << std::endl;
          break;
        case 30:
          std::cout << "GetFileSize() error " << sFile1.Str() << std::endl;
          break;
        case 40:
          std::cout << "GetFileSize() error " << sFile2.Str() << std::endl;
          break;
        default:
          break;
      }
    }
  }

  if( f1.FileIsOpen() )
      f1.xbFclose();
  if( f1.FileIsOpen() )
      f1.xbFclose();

  return iRc;
}
*/



