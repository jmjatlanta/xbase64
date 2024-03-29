/*  xb_test_lock.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

// This program tests the locking functions of xbase
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

  xbInt16  iErrorStop  = 0;
  xbString sLockFile;
  xbString sLockFile2;
  xbString sLockCmd;
  xbString sResult;

  xbXBase x;
  xbDbf * MyFile;

  if( argCnt > 1 ) {
    if( av[1][0] == 'Q' )
      po = 0;
    else if( av[1][0] == 'V' )
      po = 2;
  }

  xbSchema MyRecord[] = 
  {
    { "LOCKTEST",      XB_CHAR_FLD,      5, 0 },
    #ifdef XB_MEMO_SUPPORT
    { "MEMOTEST",      XB_MEMO_FLD,     10, 0 },
    #endif
    { "",0,0,0 }
  };


  #ifdef XB_LOGGING_SUPPORT
  x.SetLogDirectory( PROJECT_LOG_DIR );
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
  #ifdef XB_DBF4_SUPPORT
  MyFile = new xbDbf4( &x );                // version 4 dbf file
  #else
  MyFile = new xbDbf3( &x );                // version 3 dbf file
  #endif

  iRc2 = MyFile->CreateTable( "LockTest.DBF", "LockTest", MyRecord, XB_OVERLAY, XB_MULTI_USER );
  iRc += TestMethod( po, "CreateTable()",  iRc2, XB_NO_ERROR );

  #ifdef XB_MDX_SUPPORT
  xbIx *ixPtr;
  void *pTag;
  iRc2 = MyFile->CreateTag( "MDX", "LockTag", "LOCKTEST", "", 0, 0, XB_OVERLAY, &ixPtr, &pTag );
  iRc += TestMethod( po, "CreateTag()", iRc2,  XB_NO_ERROR );
  #endif  // XB_MDX_SUPPORT


  iRc += TestMethod( po, "PutField()",     MyFile->PutField( "LOCKTEST", "TEST" ),   XB_NO_ERROR );
  iRc += TestMethod( po, "AppendRecord()", MyFile->AppendRecord(),                   XB_NO_ERROR );
  iRc += TestMethod( po, "Close()",        MyFile->Close(),                          XB_NO_ERROR );

  sLockFile.Sprintf( "%slocktest.txt", PROJECT_DATA_DIR );
  // std::cout << "xb_test_lock - lockfile = [ " << sLockFile.Str()  << "]\n";

  remove( sLockFile.Str() );

  #if defined (HAVE_FORK_F)
  pid_t pid;
  if(( pid = fork()) < 0 ){
    std::cout << "fork error\n";
    exit(1);
  }

  if( pid == 0 ){
    // child task
    xbDbf    * MyFileChld;
    xbInt16  iRcChld      = 0;
    xbBool   bTblOpenChld = xbFalse;
    xbInt32  iChildLoop   = 0;
    xbBool   bDone        = xbFalse;
    xbString sLastLockCmd;

    #ifdef XB_DBF4_SUPPORT
    MyFileChld = new xbDbf4( &x );                // version 4 dbf file
    #else
    MyFileChld = new xbDbf3( &x );                // version 3 dbf file
    #endif

    x.xbSleep( 250 );

    while( !bDone ){

      iRc2 = GetCmd( x, sLockFile, sLockCmd, 'C', po );

      if( sLockCmd == sLastLockCmd )
        iChildLoop++;
      else
        sLastLockCmd = sLockCmd;

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

        } else if( sLockCmd == "START" && bTblOpenChld ){
          // came back before the parent task could process the result
          x.xbSleep( 250 );

        } else {
          if( sLockCmd == "START" ){
            // begin the process
            iRcChld =  MyFileChld->Open( "LockTest.DBF" );
            if( iRcChld != XB_NO_ERROR ){
              sResult = "FAIL";
            } else {
              sResult = "OK";
              bTblOpenChld = xbTrue;
            }

          } else if( sLockCmd == "TL" ){
            // table lock
            if(( iRcChld = MyFileChld->LockTable( XB_LOCK )) != XB_NO_ERROR )
              sResult = "FAIL";
            else
              sResult = "OK";

          } else if( sLockCmd == "TU" ){
            // table unlock
            if(( iRcChld = MyFileChld->LockTable( XB_UNLOCK )) != XB_NO_ERROR )
              sResult = "FAIL";
            else
              sResult = "OK";

          } else if( sLockCmd == "RL" ){
            // record lock
            if(( iRcChld = MyFileChld->LockRecord( XB_LOCK, 1 )) != XB_NO_ERROR )
              sResult = "FAIL";
            else
              sResult = "OK";

          } else if( sLockCmd == "RU" ){
            // record unlock
            if(( iRcChld = MyFileChld->LockRecord( XB_UNLOCK, 1 )) != XB_NO_ERROR )
              sResult = "FAIL";
            else
              sResult = "OK";

          } else if( sLockCmd == "ML" ){
            // memo lock
            #ifdef XB_MEMO_SUPPORT
            if(( iRcChld = MyFileChld->LockMemo( XB_LOCK )) != XB_NO_ERROR )
              sResult = "FAIL";
            else
              sResult = "OK";
            #else
            sLockCmd = "OK";
            #endif

          } else if( sLockCmd == "MU" ){
            // memo unlock
            #ifdef XB_MEMO_SUPPORT
            if(( iRcChld = MyFileChld->LockMemo( XB_UNLOCK )) != XB_NO_ERROR )
              sResult = "FAIL";
            else
              sResult = "OK";
            #else
            sLockCmd = "OK";
            #endif

          } else if( sLockCmd == "IL" ){
            // index lock
            #ifdef XB_MDX_SUPPORT
            if(( iRcChld = MyFileChld->LockIndices( XB_LOCK )) != XB_NO_ERROR )
              sResult = "FAIL";
            else
              sResult = "OK";
            #else
            sLockCmd = "OK";
            #endif   // XB_MDX_SUPPORT

          } else if( sLockCmd == "IU" ){
            // index unlock
            #ifdef XB_MDX_SUPPORT
            if(( iRcChld = MyFileChld->LockIndices( XB_UNLOCK )) != XB_NO_ERROR )
              sResult = "FAIL";
            else
              sResult = "OK";
            #else
            sLockCmd = "OK";
            #endif   //  XB_MDX_SUPPORT
          }


          #ifdef XB_LOGGING_SUPPORT
          sMsg.Sprintf( "Program [%s] Child task [%s] Result [%s] RC = [%d]", av[0], sLockCmd.Str(), sResult.Str(), iRcChld );
          x.WriteLogMessage( sMsg );
          #endif
          SetCmd( x, sLockFile, sResult, 'C', po );
          if( sResult == "FAIL" ){
            bDone = xbTrue;
            MyFileChld->Close();
            delete MyFileChld;
          }
        }
      } else {
        iRc = iRc2;
        bDone = xbTrue;
      }
      //std::cout << "clc [" << iChildLoop++ << "][" << bDone << "][" << sLockCmd.Str() << "]\n";
      x.xbSleep( 250 );
      if( iChildLoop > 15 )
        bDone = xbTrue;
    }
    MyFileChld->Close();
    delete MyFile;
    delete MyFileChld;

    remove( sLockFile );

    if( po > 0 )
      std::cout << "Exiting child\n";

    #ifdef XB_LOGGING_SUPPORT
    sMsg.Sprintf( "Program [%s] Child task terminating", av[0] );
    x.WriteLogMessage( sMsg );
    #endif


  } else {

    // parent logic
    xbInt16 iLoopCtr = 0;

    try{
      // start
      #ifdef XB_LOGGING_SUPPORT
      sMsg.Sprintf( "Program [%s] Parent task issuing START command", av[0] );
      x.WriteLogMessage( sMsg );
      #endif

      sLockCmd = "START";
      SetCmd( x, sLockFile, sLockCmd, 'P', po );
      sResult = "";

      GetCmd( x, sLockFile, sResult, 'P', po );
      while( sResult != "OK" && sResult != "FAIL" && iLoopCtr < 30 ){
        GetCmd( x, sLockFile, sResult, 'P', po );
        x.xbSleep( 250 );
        iLoopCtr++;
      }

      #ifdef XB_LOGGING_SUPPORT
      sMsg.Sprintf( "Program [%s] Parent task retrieved result [%s]", av[0], sResult.Str() );
      x.WriteLogMessage( sMsg );
      #endif

      if( sResult != "OK" ){
        iErrorStop = 100;
        iRc2 = -1;
        throw iRc2;
      }

      // table lock
      #ifdef XB_LOGGING_SUPPORT
      sMsg.Sprintf( "Program [%s] Parent task issuing TL command", av[0] );
      x.WriteLogMessage( sMsg );
      #endif

      sLockCmd = "TL";
      SetCmd( x, sLockFile, sLockCmd, 'P', po );
      sResult = "";

      GetCmd( x, sLockFile, sResult, 'P', po );
      while( sResult != "OK" && sResult != "FAIL" && iLoopCtr < 30 ){
        GetCmd( x, sLockFile, sResult, 'P', po );
        x.xbSleep( 250 );
        iLoopCtr++;
      }

      #ifdef XB_LOGGING_SUPPORT
      sMsg.Sprintf( "Program [%s] Parent task retrieved result [%s]", av[0], sResult.Str() );
      x.WriteLogMessage( sMsg );
      #endif

      if( sResult != "OK" ){
        iErrorStop = 110;
        iRc2 = -1;
        throw iRc2;
      }

      // attempt to lock table, should fail
      if(( iRc2 =  MyFile->Open( "LockTest.DBF" )) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc2;
      }

      if(( iRc2 =  MyFile->LockTable( XB_LOCK )) == XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc2;
      }

      if( po > 0 )
        std::cout << "[PASS] LockTable Test 1\n";

      // table unlock
      #ifdef XB_LOGGING_SUPPORT
      sMsg.Sprintf( "Program [%s] Parent task issuing TU command", av[0] );
      x.WriteLogMessage( sMsg );
      #endif

      sLockCmd = "TU";
      SetCmd( x, sLockFile, sLockCmd, 'P', po );
      sResult = "";

      GetCmd( x, sLockFile, sResult, 'P', po );
      while( sResult != "OK" && sResult != "FAIL" && iLoopCtr < 30 ){
        GetCmd( x, sLockFile, sResult, 'P', po );
        x.xbSleep( 250 );
        iLoopCtr++;
      }

      #ifdef XB_LOGGING_SUPPORT
      sMsg.Sprintf( "Program [%s] Parent task retrieved result [%s]", av[0], sResult.Str() );
      x.WriteLogMessage( sMsg );
      #endif

      if( sResult != "OK" ){
        iErrorStop = 140;
        iRc2 = -1;
        throw iRc2;
      }

      if(( iRc2 =  MyFile->LockTable( XB_LOCK )) != XB_NO_ERROR ){
        iErrorStop = 150;
        throw iRc2;
      }
      if( po > 0 )
        std::cout << "[PASS] LockTable Test 2\n";

      if(( iRc2 =  MyFile->LockTable( XB_UNLOCK )) != XB_NO_ERROR ){
        iErrorStop = 160;
        throw iRc2;
      }

      // record lock
      #ifdef XB_LOGGING_SUPPORT
      sMsg.Sprintf( "Program [%s] Parent task issuing RL command", av[0] );
      x.WriteLogMessage( sMsg );
      #endif

      sLockCmd = "RL";
      SetCmd( x, sLockFile, sLockCmd, 'P', po );
      sResult = "";

      GetCmd( x, sLockFile, sResult, 'P', po );
      while( sResult != "OK" && sResult != "FAIL" && iLoopCtr < 30 ){
        GetCmd( x, sLockFile, sResult, 'P', po );
        x.xbSleep( 250 );
        iLoopCtr++;
      }

      #ifdef XB_LOGGING_SUPPORT
      sMsg.Sprintf( "Program [%s] Parent task retrieved result [%s]", av[0], sResult.Str() );
      x.WriteLogMessage( sMsg );
      #endif

      if( sResult != "OK" ){
        iErrorStop = 170;
        iRc2 = -1;
        throw iRc2;
      }

      if(( iRc2 =  MyFile->LockRecord( XB_LOCK, 1 )) == XB_NO_ERROR ){
        iErrorStop = 180;
        throw iRc2;
      }
      if( po > 0 )
        std::cout << "[PASS] LockRecord Test 1\n";

      // record unlock
      #ifdef XB_LOGGING_SUPPORT
      sMsg.Sprintf( "Program [%s] Parent task issuing RU command", av[0] );
      x.WriteLogMessage( sMsg );
      #endif

      sLockCmd = "RU";
      SetCmd( x, sLockFile, sLockCmd, 'P', po );
      sResult = "";

      GetCmd( x, sLockFile, sResult, 'P', po );
      while( sResult != "OK" && sResult != "FAIL" && iLoopCtr < 30 ){
        GetCmd( x, sLockFile, sResult, 'P', po );
        x.xbSleep( 250 );
        iLoopCtr++;
      }

      #ifdef XB_LOGGING_SUPPORT
      sMsg.Sprintf( "Program [%s] Parent task retrieved result [%s]", av[0], sResult.Str() );
      x.WriteLogMessage( sMsg );
      #endif

      if( sResult != "OK" ){
        iErrorStop = 190;
        iRc2 = -1;
        throw iRc2;
      }

      if(( iRc2 =  MyFile->LockRecord( XB_LOCK, 1 )) != XB_NO_ERROR ){
        iErrorStop = 200;
        throw iRc2;
      }

      std::cout << "[PASS] LockRecord Test 2\n";
      if(( iRc2 =  MyFile->LockRecord( XB_UNLOCK, 1 )) != XB_NO_ERROR ){
        iErrorStop = 210;
        throw iRc2;
      }


      //  memo lock
      #ifdef XB_MEMO_SUPPORT
      #ifdef XB_LOGGING_SUPPORT
      sMsg.Sprintf( "Program [%s] Parent task issuing ML command", av[0] );
      x.WriteLogMessage( sMsg );
      #endif // XB_LOGGING_SUPPORT

      sLockCmd = "ML";
      SetCmd( x, sLockFile, sLockCmd, 'P', po );
      sResult = "";

      GetCmd( x, sLockFile, sResult, 'P', po );
      while( sResult != "OK" && sResult != "FAIL" && iLoopCtr < 30 ){
        GetCmd( x, sLockFile, sResult, 'P', po );
        x.xbSleep( 250 );
        iLoopCtr++;
      }

      #ifdef XB_LOGGING_SUPPORT
      sMsg.Sprintf( "Program [%s] Parent task retrieved result [%s]", av[0], sResult.Str() );
      x.WriteLogMessage( sMsg );
      #endif  // XB_LOGGING_SUPPORT

      if( sResult != "OK" ){
        iErrorStop = 220;
        iRc2 = -1;
        throw iRc2;
      }

      if(( iRc2 =  MyFile->LockMemo( XB_LOCK )) == XB_NO_ERROR ){
        iErrorStop = 230;
        throw iRc2;
      }
      if( po > 0 )
        std::cout << "[PASS] LockMemo Test 1\n";

      // memo unlock
      #ifdef XB_LOGGING_SUPPORT
      sMsg.Sprintf( "Program [%s] Parent task issuing MU command", av[0] );
      x.WriteLogMessage( sMsg );
      #endif // XB_LOGGING_SUPPORT

      sLockCmd = "MU";
      SetCmd( x, sLockFile, sLockCmd, 'P', po );
      sResult = "";

      GetCmd( x, sLockFile, sResult, 'P', po );
      while( sResult != "OK" && sResult != "FAIL" && iLoopCtr < 30 ){
        GetCmd( x, sLockFile, sResult, 'P', po );
        x.xbSleep( 250 );
        iLoopCtr++;
      }

      #ifdef XB_LOGGING_SUPPORT
      sMsg.Sprintf( "Program [%s] Parent task retrieved result [%s]", av[0], sResult.Str() );
      x.WriteLogMessage( sMsg );
      #endif  // XB_LOGGING_SUPPORT

      if( sResult != "OK" ){
        iErrorStop = 240;
        iRc2 = -1;
        throw iRc2;
      }

      if(( iRc2 =  MyFile->LockMemo( XB_LOCK )) != XB_NO_ERROR ){
        iErrorStop = 250;
        throw iRc2;
      }

      std::cout << "[PASS] LockMemo Test 2\n";
      if(( iRc2 =  MyFile->LockMemo( XB_UNLOCK )) != XB_NO_ERROR ){
        iErrorStop = 260;
        throw iRc2;
      }
      #endif  // XB_MEMO_SUPPORT

      //  index lock
      #ifdef XB_MDX_SUPPORT
      #ifdef XB_LOGGING_SUPPORT
      sMsg.Sprintf( "Program [%s] Parent task issuing IL command", av[0] );
      x.WriteLogMessage( sMsg );
      #endif // XB_LOGGING_SUPPORT

      sLockCmd = "IL";
      SetCmd( x, sLockFile, sLockCmd, 'P', po );
      sResult = "";

      GetCmd( x, sLockFile, sResult, 'P', po );
      while( sResult != "OK" && sResult != "FAIL" && iLoopCtr < 30 ){
        GetCmd( x, sLockFile, sResult, 'P', po );
        x.xbSleep( 250 );
        iLoopCtr++;
      }

      #ifdef XB_LOGGING_SUPPORT
      sMsg.Sprintf( "Program [%s] Parent task retrieved result [%s]", av[0], sResult.Str() );
      x.WriteLogMessage( sMsg );
      #endif  // XB_LOGGING_SUPPORT

      if( sResult != "OK" ){
        iErrorStop = 270;
        iRc2 = -1;
        throw iRc2;
      }

      if(( iRc2 =  MyFile->LockIndices( XB_LOCK )) == XB_NO_ERROR ){
        iErrorStop = 280;
        throw iRc2;
      }
      if( po > 0 )
        std::cout << "[PASS] LockIndex Test 1\n";

      // memo unlock
      #ifdef XB_LOGGING_SUPPORT
      sMsg.Sprintf( "Program [%s] Parent task issuing IU command", av[0] );
      x.WriteLogMessage( sMsg );
      #endif // XB_LOGGING_SUPPORT

      sLockCmd = "IU";
      SetCmd( x, sLockFile, sLockCmd, 'P', po );
      sResult = "";

      GetCmd( x, sLockFile, sResult, 'P', po );
      while( sResult != "OK" && sResult != "FAIL" && iLoopCtr < 30 ){
        GetCmd( x, sLockFile, sResult, 'P', po );
        x.xbSleep( 250 );
        iLoopCtr++;
      }

      #ifdef XB_LOGGING_SUPPORT
      sMsg.Sprintf( "Program [%s] Parent task retrieved result [%s]", av[0], sResult.Str() );
      x.WriteLogMessage( sMsg );
      #endif  // XB_LOGGING_SUPPORT

      if( sResult != "OK" ){
        iErrorStop = 290;
        iRc2 = -1;
        throw iRc2;
      }

      if(( iRc2 =  MyFile->LockIndices( XB_LOCK )) != XB_NO_ERROR ){
        iErrorStop = 300;
        throw iRc2;
      }

      std::cout << "[PASS] LockIndex Test 2\n";
      if(( iRc2 =  MyFile->LockMemo( XB_UNLOCK )) != XB_NO_ERROR ){
        iErrorStop = 310;
        throw iRc2;
      }
      #endif  // XB_MDX_SUPPORT



      // exit
      sLockCmd = "EXIT";
      SetCmd( x, sLockFile, sLockCmd, 'P', po );



    } catch (xbInt16 iRc3 ){
      iRc = iRc3;
      if( po > 0 )
        std::cout << "Parent lock task exiting on failure [" << sLockCmd.Str() << "][" << iErrorStop << "]\n";


      #ifdef XB_LOGGING_SUPPORT
      sMsg.Sprintf( "Program [%s] Parent task terminating with errors [%s][%d][%d][%d]...", av[0], sLockCmd.Str(), iErrorStop, iLoopCtr, iRc3 );
      x.WriteLogMessage( sMsg );
      #endif

    }

    #ifdef XB_LOGGING_SUPPORT
    sMsg.Sprintf( "Program [%s] Parent task terminating", av[0] );
    x.WriteLogMessage( sMsg );
    #endif

    if( po > 0 )
      std::cout << "Exiting parent\n";

    sLockCmd = "EXIT";
    SetCmd( x, sLockFile, sLockCmd, 'P', po );
    MyFile->Close();
    delete MyFile;

  }
  #elif defined (HAVE_CREATEPROCESSW_F)

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory( &si, sizeof( si ));
    si.cb = sizeof( si );
    ZeroMemory( &pi, sizeof( pi ));

    xbString strCmdLine = "xb_test_lock2";
    if( argCnt > 1 ){
      strCmdLine += " ";
      strCmdLine += av[1];
    }

    char sCmdLineBuf[25];
    memset( sCmdLineBuf, 0x00, 25 );
    for( xbUInt32 i = 0; i < strCmdLine.Len(); i++ )
      sCmdLineBuf[i] = strCmdLine[i+1];

    if( !CreateProcess( NULL, sCmdLineBuf, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi )){
      sMsg.Sprintf( "Program [%s] error in CreateProcess call.  Processing aborted" );
      #ifdef XB_LOGGING_SUPPORT
      x.WriteLogMessage( sMsg );
      #endif
      std::cout << sMsg.Str() << "\n";
      iRc -=1;
    } else {

      xbInt16 iLoopCtr = 0;

      try{
        #ifdef XB_LOGGING_SUPPORT
        sMsg.Sprintf( "Program [%s] Parent task issuing START command", av[0] );
        x.WriteLogMessage( sMsg );
        #endif


        sLockCmd = "START";
        SetCmd( x, sLockFile, sLockCmd, 'P', po );
        sResult = "";

        GetCmd( x, sLockFile, sResult, 'P', po );
        while( sResult != "OK" && sResult != "FAIL" && iLoopCtr < 10 ){
          GetCmd( x, sLockFile, sResult, 'P', po );
          x.xbSleep( 300 );
          iLoopCtr++;
        }

        #ifdef XB_LOGGING_SUPPORT
        sMsg.Sprintf( "Program [%s] Parent task retrieved result [%s]", av[0], sResult.Str() );
        x.WriteLogMessage( sMsg );
        #endif

        // table lock
        #ifdef XB_LOGGING_SUPPORT
        sMsg.Sprintf( "Program [%s] Parent task issuing TL command", av[0] );
        x.WriteLogMessage( sMsg );
        #endif

        sLockCmd = "TL";
        SetCmd( x, sLockFile, sLockCmd, 'P', po );
        sResult = "";

        GetCmd( x, sLockFile, sResult, 'P', po );
        while( sResult != "OK" && sResult != "FAIL" && iLoopCtr < 10 ){
          GetCmd( x, sLockFile, sResult, 'P', po );
          x.xbSleep( 310 );
          iLoopCtr++;
        }

        #ifdef XB_LOGGING_SUPPORT
        sMsg.Sprintf( "Program [%s] Parent task retrieved result [%s]", av[0], sResult.Str() );
        x.WriteLogMessage( sMsg );
        #endif

        if( sResult != "OK" ){
          iErrorStop = 320;
          iRc2 = -1;
          throw iRc2;
        }

        // attempt to lock table, should fail
        if(( iRc2 = MyFile->Open( "LockTest.DBF" )) != XB_NO_ERROR ){
          iErrorStop = 330;
          throw iRc2;
        }

        if(( iRc2 =  MyFile->LockTable( XB_LOCK )) == XB_NO_ERROR ){
          iErrorStop = 340;
          throw iRc2;
        }

        if( po > 0 ){
          std::cout << "[PASS] LockTable Test 1\n";

          #ifdef XB_LOGGING_SUPPORT
          sMsg.Sprintf( "Program [%s] Parent task LockTable Test 1 Success.", av[0] );
          x.WriteLogMessage( sMsg );
          #endif
        }

        // table unlock
        #ifdef XB_LOGGING_SUPPORT
        sMsg.Sprintf( "Program [%s] Parent task issuing TU command", av[0] );
        x.WriteLogMessage( sMsg );
        #endif

        sLockCmd = "TU";
        SetCmd( x, sLockFile, sLockCmd, 'P', po );
        sResult = "";

        GetCmd( x, sLockFile, sResult, 'P', po );
        while( sResult != "OK" && sResult != "FAIL" && iLoopCtr < 10 ){
          GetCmd( x, sLockFile, sResult, 'P', po );
          x.xbSleep( 250 );
          iLoopCtr++;
        }

        #ifdef XB_LOGGING_SUPPORT
        sMsg.Sprintf( "Program [%s] Parent task retrieved result [%s]", av[0], sResult.Str() );
        x.WriteLogMessage( sMsg );
        #endif

        if( sResult != "OK" ){
          iErrorStop = 350;
          iRc2 = -1;
          throw iRc2;
        }

        if(( iRc2 =  MyFile->LockTable( XB_LOCK )) != XB_NO_ERROR ){
          iErrorStop = 360;
          throw iRc2;
        }
        if( po > 0 ){
          std::cout << "[PASS] LockTable Test 2\n";
          #ifdef XB_LOGGING_SUPPORT
          sMsg.Sprintf( "Program [%s] Parent task LockTable Test 1 Success.", av[0] );
          x.WriteLogMessage( sMsg );
          #endif
        }

        if(( iRc2 =  MyFile->LockTable( XB_UNLOCK )) != XB_NO_ERROR ){
          iErrorStop = 370;
          throw iRc2;
        }

        // record lock
        #ifdef XB_LOGGING_SUPPORT
        sMsg.Sprintf( "Program [%s] Parent task issuing RL command", av[0] );
        x.WriteLogMessage( sMsg );
        #endif

        sLockCmd = "RL";
        SetCmd( x, sLockFile, sLockCmd, 'P', po );
        sResult = "";

        GetCmd( x, sLockFile, sResult, 'P', po );
        while( sResult != "OK" && sResult != "FAIL" && iLoopCtr < 10 ){
          GetCmd( x, sLockFile, sResult, 'P', po );
          x.xbSleep( 380 );
          iLoopCtr++;
        }

        #ifdef XB_LOGGING_SUPPORT
        sMsg.Sprintf( "Program [%s] Parent task retrieved result [%s]", av[0], sResult.Str() );
        x.WriteLogMessage( sMsg );
        #endif

        if( sResult != "OK" ){
          iErrorStop = 390;
          iRc2 = -1;
          throw iRc2;
        }

        if(( iRc2 =  MyFile->LockRecord( XB_LOCK, 1 )) == XB_NO_ERROR ){
          iErrorStop = 400;
          throw iRc2;
        }
        if( po > 0 )
          std::cout << "[PASS] LockRecord Test 1\n";

        // record unlock
        #ifdef XB_LOGGING_SUPPORT
        sMsg.Sprintf( "Program [%s] Parent task issuing RU command", av[0] );
        x.WriteLogMessage( sMsg );
        #endif

        sLockCmd = "RU";
        SetCmd( x, sLockFile, sLockCmd, 'P', po );
        sResult = "";

        GetCmd( x, sLockFile, sResult, 'P', po );
        while( sResult != "OK" && sResult != "FAIL" && iLoopCtr < 10 ){
          GetCmd( x, sLockFile, sResult, 'P', po );
          x.xbSleep( 250 );
          iLoopCtr++;
        }

        #ifdef XB_LOGGING_SUPPORT
        sMsg.Sprintf( "Program [%s] Parent task retrieved result [%s]", av[0], sResult.Str() );
        x.WriteLogMessage( sMsg );
        #endif

        if( sResult != "OK" ){
          iErrorStop = 410;
          iRc2 = -1;
          throw iRc2;
        }

        if(( iRc2 =  MyFile->LockRecord( XB_LOCK, 1 )) != XB_NO_ERROR ){
          iErrorStop = 420;
          throw iRc2;
        }

        std::cout << "[PASS] LockRecord Test 2\n";
        if(( iRc2 =  MyFile->LockRecord( XB_UNLOCK, 1 )) != XB_NO_ERROR ){
          iErrorStop = 430;
          throw iRc2;
        }

        //  memo lock
        #ifdef XB_MEMO_SUPPORT
        #ifdef XB_LOGGING_SUPPORT
        sMsg.Sprintf( "Program [%s] Parent task issuing ML command", av[0] );
        x.WriteLogMessage( sMsg );
        #endif

        sLockCmd = "ML";
        SetCmd( x, sLockFile, sLockCmd, 'P', po );
        sResult = "";

        GetCmd( x, sLockFile, sResult, 'P', po );
        while( sResult != "OK" && sResult != "FAIL" && iLoopCtr < 10 ){
          GetCmd( x, sLockFile, sResult, 'P', po );
          x.xbSleep( 250 );
          iLoopCtr++;
        }

        #ifdef XB_LOGGING_SUPPORT
        sMsg.Sprintf( "Program [%s] Parent task retrieved result [%s]", av[0], sResult.Str() );
        x.WriteLogMessage( sMsg );
        #endif

        if( sResult != "OK" ){
          iErrorStop = 440;
          iRc2 = -1;
          throw iRc2;
        }

        if(( iRc2 =  MyFile->LockMemo( XB_LOCK )) == XB_NO_ERROR ){
          iErrorStop = 450;
          throw iRc2;
        }
        if( po > 0 )
          std::cout << "[PASS] LockMemo Test 1\n";

        // memo unlock
        #ifdef XB_LOGGING_SUPPORT
        sMsg.Sprintf( "Program [%s] Parent task issuing MU command", av[0] );
        x.WriteLogMessage( sMsg );
        #endif

        sLockCmd = "MU";
        SetCmd( x, sLockFile, sLockCmd, 'P', po );
        sResult = "";

        GetCmd( x, sLockFile, sResult, 'P', po );
        while( sResult != "OK" && sResult != "FAIL" && iLoopCtr < 10 ){
          GetCmd( x, sLockFile, sResult, 'P', po );
          x.xbSleep( 250 );
          iLoopCtr++;
        }

        #ifdef XB_LOGGING_SUPPORT
        sMsg.Sprintf( "Program [%s] Parent task retrieved result [%s]", av[0], sResult.Str() );
        x.WriteLogMessage( sMsg );
        #endif

        if( sResult != "OK" ){
          iErrorStop = 460;
          iRc2 = -1;
          throw iRc2;
        }

        if(( iRc2 =  MyFile->LockMemo( XB_LOCK )) != XB_NO_ERROR ){
          iErrorStop = 470;
          throw iRc2;
        }

        std::cout << "[PASS] LockMemo Test 2\n";
        if(( iRc2 =  MyFile->LockMemo( XB_UNLOCK )) != XB_NO_ERROR ){
          iErrorStop = 480;
          throw iRc2;
        }
        #endif

        //  index lock
        #ifdef XB_MDX_SUPPORT
        #ifdef XB_LOGGING_SUPPORT
        sMsg.Sprintf( "Program [%s] Parent task issuing IL command", av[0] );
        x.WriteLogMessage( sMsg );
        #endif

        sLockCmd = "IL";
        SetCmd( x, sLockFile, sLockCmd, 'P', po );
        sResult = "";

        GetCmd( x, sLockFile, sResult, 'P', po );
        while( sResult != "OK" && sResult != "FAIL" && iLoopCtr < 10 ){
          GetCmd( x, sLockFile, sResult, 'P', po );
          x.xbSleep( 490 );
          iLoopCtr++;
        }

        #ifdef XB_LOGGING_SUPPORT
        sMsg.Sprintf( "Program [%s] Parent task retrieved result [%s]", av[0], sResult.Str() );
        x.WriteLogMessage( sMsg );
        #endif

        if( sResult != "OK" ){
          iErrorStop = 500;
          iRc2 = -1;
          throw iRc2;
        }

        if(( iRc2 =  MyFile->LockIndices( XB_LOCK )) == XB_NO_ERROR ){
          iErrorStop = 510;
          throw iRc2;
        }
        if( po > 0 )
          std::cout << "[PASS] LockIndex Test 1\n";

        // index unlock
        #ifdef XB_LOGGING_SUPPORT
        sMsg.Sprintf( "Program [%s] Parent task issuing IU command", av[0] );
        x.WriteLogMessage( sMsg );
        #endif

        sLockCmd = "IU";
        SetCmd( x, sLockFile, sLockCmd, 'P', po );
        sResult = "";

        GetCmd( x, sLockFile, sResult, 'P', po );
        while( sResult != "OK" && sResult != "FAIL" && iLoopCtr < 10 ){
          GetCmd( x, sLockFile, sResult, 'P', po );
          x.xbSleep( 250 );
          iLoopCtr++;
        }

        #ifdef XB_LOGGING_SUPPORT
        sMsg.Sprintf( "Program [%s] Parent task retrieved result [%s]", av[0], sResult.Str() );
        x.WriteLogMessage( sMsg );
        #endif

        if( sResult != "OK" ){
          iErrorStop = 520;
          iRc2 = -1;
          throw iRc2;
        }

        if(( iRc2 =  MyFile->LockIndices( XB_LOCK )) != XB_NO_ERROR ){
          iErrorStop = 530;
          throw iRc2;
        }

        std::cout << "[PASS] LockIndex Test 2\n";
        if(( iRc2 =  MyFile->LockIndices( XB_UNLOCK )) != XB_NO_ERROR ){
          iErrorStop = 540;
          throw iRc2;
        }
        #endif

        // exit
        sLockCmd = "EXIT";
        SetCmd( x, sLockFile, sLockCmd, 'P', po );


      } catch (xbInt16 iRc3 ){
        if( po > 0 )
          std::cout << "Parent lock task exiting on failure [" << sLockCmd.Str() << "][" << iErrorStop << "]\n";

        #ifdef XB_LOGGING_SUPPORT
        sMsg.Sprintf( "Program [%s] Parent task terminating with errors [%s][%d][%d][%d]...", av[0], sLockCmd.Str(), iErrorStop, iLoopCtr, iRc3 );
        x.WriteLogMessage( sMsg );
        #endif

        sLockCmd = "EXIT";
        SetCmd( x, sLockFile, sLockCmd, 'P', po );
        MyFile->Close();
        delete MyFile;
      }
  }

  #else

    iRc--;
    sMsg.Sprintf( "Program [%s] not executed.  Library does not support 'fork' or 'CreateProcess' function call", av[0] );
    #ifdef XB_LOGGING_SUPPORT
    x.WriteLogMessage( sMsg );
    #endif
    if( po > 0 )
      std::cout << sMsg.Str() << "\n";
  #endif

  if( po > 0 || iRc < 0 )
    fprintf( stdout, "[%s] Total Errors = %d\n", av[0], iRc * -1 );

  #ifdef XB_LOGGING_SUPPORT
  sMsg.Sprintf( "Program [%s] terminating with [%d] errors...", av[0], iRc * -1 );
  x.WriteLogMessage( sMsg ); 
  #endif

  return iRc;
}

