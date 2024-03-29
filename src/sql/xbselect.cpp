/* xbset.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

#include "xbase.h"

#ifdef XB_SQL_SUPPORT

namespace xb{


/***********************************************************************/
xbInt16 xbStmt::ExecuteQuery( const xbString &sCmdLine ){

  std::cout << "xbStmt::ExecuteQuery() - SELECT [" << sCmdLine << "]\n";
  // expected format:
  //  SELECT 

  xbInt16  iRc = 0;
  xbInt16  iErrorStop = 0;
  // xbUInt32 ulPos;

  try{

    if(( iRc = ParseStmt( sCmdLine )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }

/*
    xbString sCmd = sCmdLine;
    sCmd.Trim();
    sCmd.ZapTrailingChar( ';' );
    sCmd.Trim();

    // drop off the first node "SET"
    ulPos = sCmd.Pos( ' ' );
    sCmd.Ltrunc( ulPos );
    sCmd.Ltrim();

    xbString sNode1 = sCmd;
    sNode1.ToUpperCase();

    if( sNode1 == "SET" ) {
        uda.DumpUda();

    } else {

      xbString sKey;
      sKey.ExtractElement( sCmd, '=', 1, 0 );
      sKey.Trim();

      xbString sToken;
      sToken.ExtractElement( sCmd, '=', 2, 0 );
      sToken.Trim();


      if( sToken == '^' ){
        if(( iRc = uda.DelTokenForKey( sKey )) != XB_NO_ERROR ){
          iErrorStop = 100;
          throw iRc;
        }
      } else {
        if(( iRc = uda.UpdTokenForKey( sKey, sToken )) != XB_NO_ERROR ){
          iErrorStop = 110;
          throw iRc;
        }
      }
    }
*/

  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbStmt::ExecuteQuery() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }

  return iRc;
}

/***********************************************************************/
}              /* namespace       */
#endif         /*  XB_SQL_SUPPORT */

