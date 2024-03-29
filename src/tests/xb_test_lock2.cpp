/*  xb_test_lock2.cpp

XBase Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

// This program tests the locking functions
//  This program is the child process that is called if used
//  in an environment that supports the CreateProcess library function (ie Windows)

// usage:   xb_test_lock QUITE|NORMAL|VERBOSE



#include "xbase.h"

using namespace xb;

#include "tstfuncs.cpp"

/****************************************************************/
int main( int argCnt, char **av )
{
  xbInt16 iRc = 0;
  xbInt16 iRc2;
  xbInt16 po = 1;      /* print option */
                       /* 0 - QUIET    */
                       /* 1 - NORMAL   */
                       /* 2 - VERBOSE  */

  xbBool   bDone       = xbFalse;
  xbString sLockFile;
  xbString sLockCmd;
  xbString sResult;
  xbInt32  iChildLoop  = 0;

  xbXBase x;
  xbDbf * MyFile;

  xbBool  bTblOpen     = xbFalse;

  if( argCnt > 1 ) {
    if( av[1][0] == 'Q' )
      po = 0;
    else if( av[1][0] == 'V' )
      po = 2;
  }

  #ifdef XB_LOGGING_SUPPORT
  x.SetLogDirectory( PROJECT_LOG_DIR );
  xbString sLogFileName =  x.GetLogFqFileName().Str();
  sLogFileName.Resize( sLogFileName.Len() - 3 );
  sLogFileName += "_l2.txt";
  x.SetLogFileName( sLogFileName );
  x.EnableMsgLogging();
  if( po > 0 ){
    std::cout << "Logfile is [" << x.GetLogFqFileName().Str() << "]" << std::endl;
  }
  xbString sMsg;
  sMsg.Sprintf( "Program [%s] initializing...", av[0] );
  x.WriteLogMessage( sMsg );
  #endif

  x.SetDataDirectory( PROJECT_DATA_DIR );
  InitTime();

  sLockFile.Sprintf( "%slocktest.txt", PROJECT_DATA_DIR );
  // std::cout << "xb_test_lock2 - lockfile = [ " << sLockFile.Str() << "]\n";

  #ifdef XB_DBF4_SUPPORT
  MyFile = new xbDbf4( &x );                /* version 4 dbf file */
  #else
  MyFile = new xbDbf3( &x );                /* version 3 dbf file */
  #endif

  x.xbSleep( 250 );
  while( !bDone ){
    iRc2 = GetCmd( x, sLockFile, sLockCmd, 'C', po );

    #ifdef XB_LOGGING_SUPPORT
    if( sLockCmd != "OK" && sLockCmd != "FAIL" ){
      sMsg.Sprintf( "Program [%s] Child task retrieved command=[%s] RC=[%d]", av[0], sLockCmd.Str(), iRc2 );
      x.WriteLogMessage( sMsg );
    }
    #endif

    if( iRc2 == 0 ){

      if( sLockCmd == "OK" || sLockCmd == "FAIL" )
        x.xbSleep( 250 );

      else if( sLockCmd == "EXIT" ){
        bDone = xbTrue;

      } else if( sLockCmd == "START" && bTblOpen ){
        // came back before the parent task could process the result
        x.xbSleep( 250 );

      } else {

        if( sLockCmd == "START" ){

          // begin the process
          iRc2 =  MyFile->Open( "LockTest.DBF" );
          if( iRc2 != XB_NO_ERROR ){
            sResult = "FAIL";
          } else {
            sResult = "OK";
            bTblOpen = xbTrue;
          }

        } else if( sLockCmd == "TL" ){
          // table lock
          if(( iRc2 = MyFile->LockTable( XB_LOCK )) != XB_NO_ERROR )
            sResult = "FAIL";
          else
            sResult = "OK";

        } else if( sLockCmd == "TU" ){
          // table unlock
          if(( iRc2 = MyFile->LockTable( XB_UNLOCK )) != XB_NO_ERROR )
            sResult = "FAIL";
          else
            sResult = "OK";

        } else if( sLockCmd == "RL" ){
          // record lock
          if(( iRc2 = MyFile->LockRecord( XB_LOCK, 1 )) != XB_NO_ERROR )
            sResult = "FAIL";
          else
            sResult = "OK";


        } else if( sLockCmd == "RU" ){
          // record unlock
          if(( iRc2 = MyFile->LockRecord( XB_UNLOCK, 1 )) != XB_NO_ERROR )
            sResult = "FAIL";
          else
            sResult = "OK";

        } else if( sLockCmd == "ML" ){
          // memo lock
          #ifdef XB_MEMO_SUPPORT
          if(( iRc2 = MyFile->LockMemo( XB_LOCK )) != XB_NO_ERROR )
            sResult = "FAIL";
          else
            sResult = "OK";
          #else
          sLockCmd = "OK";
          #endif

        } else if( sLockCmd == "MU" ){
          // memo unlock
          #ifdef XB_MEMO_SUPPORT
          if(( iRc2 = MyFile->LockMemo( XB_UNLOCK )) != XB_NO_ERROR )
            sResult = "FAIL";
          else
            sResult = "OK";
          #else
          sLockCmd = "OK";
          #endif

        } else if( sLockCmd == "IL" ){
          // index lock
          #ifdef XB_MDX_SUPPORT
          if(( iRc2 = MyFile->LockIndices( XB_LOCK )) != XB_NO_ERROR )
            sResult = "FAIL";
          else
            sResult = "OK";
          #else
          sLockCmd = "OK";
          #endif

        } else if( sLockCmd == "IU" ){
          // index unlock
          #ifdef XB_MDX_SUPPORT
          if(( iRc2 = MyFile->LockIndices( XB_UNLOCK )) != XB_NO_ERROR )
            sResult = "FAIL";
          else
            sResult = "OK";
          #else
          sLockCmd = "OK";
          #endif
        }


        #ifdef XB_LOGGING_SUPPORT
        sMsg.Sprintf( "Program [%s] Child task [%s] Result [%s] RC = [%d]", av[0], sLockCmd.Str(), sResult.Str(), iRc2 );
        x.WriteLogMessage( sMsg );
        #endif
        SetCmd( x, sLockFile, sResult, 'C', po );
      }
    } else {
      iRc = iRc2;
      bDone = xbTrue;
    }

    // std::cout << "clc [" << iChildLoop++ << "][" << bDone << "][" << sLockCmd << "]\n";
    x.xbSleep( 250 );
    if( iChildLoop > 10 )
      bDone = xbTrue;
  }
  MyFile->Close();
  delete MyFile;
  if( po > 0 )
    std::cout << "Exiting child\n";

  remove( sLockFile.Str() );
  #ifdef XB_LOGGING_SUPPORT
  sMsg.Sprintf( "Program [%s] Child task terminating", av[0] );
  x.WriteLogMessage( sMsg );
  #endif

  if( po > 0 || iRc < 0 )
    fprintf( stdout, "Total Errors = %d\n", iRc * -1 );

  #ifdef XB_LOGGING_SUPPORT
  sMsg.Sprintf( "Program [%s] terminating with [%d] errors...", av[0], iRc * -1 );
  x.WriteLogMessage( sMsg );
  #endif

  ExitProcess( (xbUInt32) iRc );
}

