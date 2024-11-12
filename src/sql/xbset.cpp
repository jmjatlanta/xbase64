/* xbset.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023,2024 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

#include "xbase.h"

#ifdef XB_SQL_SUPPORT

namespace xb{


/***********************************************************************/
xbInt16 xbSql::SqlSet( const xbString &sCmdLine ){

  // std::cout << "SET [" << sCmdLine << "]\n";
  // expected format:
  //  SET ATTRIBUTE = DATAVALUE
  //  SET ATTRIBUTE = ^     (to delete an entry)

  xbInt16  iRc = 0;
  xbInt16  iErrorStop = 0;
  xbUInt32 ulPos;

  try{

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
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbSql::SqlSet() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }

  return iRc;
}

/***********************************************************************/
}              /* namespace       */
#endif         /*  XB_SQL_SUPPORT */

