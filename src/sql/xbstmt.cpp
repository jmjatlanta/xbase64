/* xbsql.cpp

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
xbStmt::xbStmt( xbXBase *x ){
   xbase        = x;
   pTblList     = NULL;
   fl           = NULL;
   ulFromPos    = 0;
   ulWherePos   = 0;
   ulOrderByPos = 0;
   ulGroupByPos = 0;
   ulHavingPos  = 0;
}

/***********************************************************************/
xbStmt::~xbStmt(){

  std::cout << "xbStmt::~xbStmt()  - need to release all allocated structures and memory here\n";

}

/***********************************************************************/
xbInt16  xbStmt::CvtSqlExp2DbaseExp( const xbString &sExpIn, xbString &sExpOut ){

  // convert Ansi SQL expression to a DBase compatible expression

  xbInt16  iRc        = 0;
  xbInt16  iErrorStop = 0;

  try{

    // convert   tablename.fieldname   to   tablename->fieldname
    sExpOut = sExpIn[1];
    xbUInt32 ulPos = 2;
    while( ulPos < sExpIn.Len() ){
      if( sExpIn[ulPos] == '.' && sExpIn[ulPos-1] != ' ' && sExpIn[ulPos+1] != ' ' )
        sExpOut += "->";
      else
        sExpOut += sExpIn[ulPos];
      ulPos++;
    }
    sExpOut += sExpIn[ulPos];

    sExpOut.Replace( " AND ", " .AND. ", 0 );
    sExpOut.Replace( " and ", " .AND. ", 0 );
    sExpOut.Replace( " OR ",  " .OR. ",  0 );
    sExpOut.Replace( " or ",  " .OR. ",  0 );
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbStmt::CvtSqlExp2DbaseExp() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}


/***********************************************************************/
#ifdef XB_DEBUG_SUPPORT
xbInt16 xbStmt::DumpStmtInternals(){

  std::cout << "************** Dump Statment Internals ***************" << std::endl;
  std::cout << "Statement [" << sStmt    << "]\n";
  std::cout << "Fields    [" << sFields  << "]\n";
  std::cout << "From      [" << sFrom    << "]\n";
  std::cout << "Where     [" << sWhere   << "]\n";
  std::cout << "Order By  [" << sOrderBy << "]\n";
  std::cout << "Group By  [" << sGroupBy << "]\n";
  std::cout << "Having    [" << sHaving  << "]\n";

  xbTblJoin *p = pTblList;
  if( p ){
    std::cout << "Tbl List\n";
    std::cout << "Type,Name,Alias,Exp\n";
    while( p ){
      std::cout << p->cJoinType << "," << p->sTableName.Str() << "," << p->sAlias.Str() << "," << p->sJoinExp.Str() << "\n";
      p = p->next;
    }
  }
  std::cout << "*************** End of Internals ************" << std::endl;
  return 0;
}

xbInt16 xbStmt::Test(){
  xbString s;

  s = "(ABC)";
  std::cout << "[" << s.Str() << "][" << GetParenCnt( s ) << "]\n";
  s = "(ABC) )";
  std::cout << "[" << s.Str() << "][" << GetParenCnt( s ) << "]\n";
  s = "(";
  std::cout << "[" << s.Str() << "][" << GetParenCnt( s ) << "]\n";
  s = "( (";
  std::cout << "[" << s.Str() << "][" << GetParenCnt( s ) << "]\n";
  s = "aaa)";
  std::cout << "[" << s.Str() << "][" << GetParenCnt( s ) << "]\n";
  s = "aaa";
  std::cout << "[" << s.Str() << "][" << GetParenCnt( s ) << "]\n";

  return 0;

}
#endif  // XB_DEBUG_SUPPORT


/***********************************************************************/
xbInt16 xbStmt::GetNextFromSeg( const xbString &sLineIn, xbString &sFromSegOut ){

  xbInt16  iRc        = 0;
  xbInt16  iErrorStop = 0;
  xbInt16  iParenCtr  = 0;
  xbUInt32 ulPos      = 1;
  xbUInt32 ulTsp      = 0;            // token start position
  xbBool   bDone      = xbFalse;
  xbString sToken;

  try{
    sFromSegOut = "";
    while( !bDone ){
      ulTsp = GetNextToken( sLineIn, sToken, ulPos );
      if( sToken == "" ){
        bDone = xbTrue;

      } else {
        iParenCtr += GetParenCnt( sToken );
        if( iParenCtr == 0 && ulPos > 1  ){
          sToken.ToUpperCase();
          if( sToken == "LEFT" || sToken == "RIGHT" || sToken == "FULL" ||
              sToken == "INNER" || sToken == "OUTER" || sToken[1] == '(' ){
            bDone = xbTrue;
          }
        }
      }
      ulPos += (sToken.Len() + 1);
    }
    if( ulTsp > 1 )
      sFromSegOut.Assign( sLineIn, 1, ulTsp - 2 );
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbStmt::GetNextFromSeg() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}

/***********************************************************************/

xbUInt32 xbStmt::GetNextToken( const xbString &sCmdLineIn, xbString &sTokenOut, xbUInt32 ulStartPos ){

  // input 
  //   sCmdLine   - Entire Command line
  //   ulStartPos - Where to start searching for the next token
  // returns the position of the beginning of the token in the main command line

  xbUInt32 ul = ulStartPos;
  xbUInt32 ulTokenStartPos = 0;
  sTokenOut = "";

  while( ul <= sCmdLineIn.Len() && sCmdLineIn[ul] == ' ' )
    ul++;
  ulTokenStartPos = ul;

  while( ul <= sCmdLineIn.Len() && sCmdLineIn[ul] != ' ' ){
    sTokenOut += sCmdLineIn[ul];
    ul++;
  }

  return ulTokenStartPos;
}
/***********************************************************************/
xbInt16 xbStmt::GetParenCnt( const xbString &sToken ){

  xbInt16 iParenCnt = 0;

  for( xbUInt32 i = 1; i <= sToken.Len(); i++ ){
    if( sToken[i] == '(' )
      iParenCnt++;
    else if( sToken[i] == ')' )
      iParenCnt--;
  }
  return iParenCnt;
}


/***********************************************************************/
xbInt16 xbStmt::ParseFromStmt( const xbString &sFromLine ){

  xbInt16  iRc           = 0;
  xbInt16  iErrorStop    = 0;
  xbBool   bDone         = xbFalse;
  xbString sFromSeg;
  xbString sLine = sFromLine;

  try{
    std::cout << "ParseFromSeg [" << sFromLine.Str() << "]\n";

    while( !bDone ){
      if(( iRc = GetNextFromSeg( sLine, sFromSeg)) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      }
      if( sFromSeg.Len() == 0 ){
        bDone = xbTrue;
      } else {
        if(( iRc = ProcessFromSeg( sFromSeg )) != XB_NO_ERROR ){
          iErrorStop = 110;
          throw iRc;
        }
        sLine.Ltrunc( sFromSeg.Len() );
        sLine.Ltrim();
      }
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbStmt::ParseFromSeg() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }

  return iRc;
}
/***********************************************************************/
xbInt16 xbStmt::ParseStmt( const xbString &sCmdLine ){

  xbInt16  iRc           = 0;
  xbInt16  iErrorStop    = 0;
  xbInt16  iParenCnt     = 0;
  xbUInt32 ulStartPos    = 7;
  xbUInt32 ulLen         = 0;


  xbBool   bFromFound    = xbFalse;
  xbBool   bWhereFound   = xbFalse;
  xbBool   bOrderByFound = xbFalse;
  xbBool   bGroupByFound = xbFalse;
  xbBool   bHavingFound  = xbFalse;
  xbBool   bDone         = xbFalse;

  xbString sToken;

  sStmt.Set( sCmdLine );
  sStmt.Trim();

  try{

    std::cout << "ParseStmt - [" << sStmt << "]\n";

    xbUInt32 ulPos = ulStartPos;

    while( !bFromFound && !bDone ){
      ulPos = GetNextToken( sStmt, sToken, ulPos );
      if( sToken == "" ){
        bDone = xbTrue;
      } else {
        iParenCnt += GetParenCnt( sToken );
        if( iParenCnt == 0 ){
          sToken.ToUpperCase();
          if( sToken == "FROM" ){
            bFromFound = xbTrue;
            ulFromPos = ulPos;
            sFrom.Assign( sStmt, ulFromPos );
          }
        }
        ulPos += sToken.Len() + 1;
      }
    }

    // look for the where clause
    if( bFromFound )
      ulPos = ulFromPos + 5;
    else
      ulPos = ulStartPos;

    bDone = xbFalse;
    while( !bWhereFound && !bDone ){
      ulPos = GetNextToken( sStmt, sToken, ulPos );
      if( sToken == "" ){
        bDone = xbTrue;
      } else {
        iParenCnt += GetParenCnt( sToken );
        if( iParenCnt == 0 ){
          sToken.ToUpperCase();
          if( sToken == "WHERE" ){
            bWhereFound = xbTrue;
            ulWherePos = ulPos;
            sWhere.Assign( sStmt, ulWherePos+6 );
          }
        }
        ulPos += sToken.Len() + 1;
      }
    }


    // look for the order clause
    if( bWhereFound )
      ulPos = ulWherePos + 6;
    else if( bFromFound )
      ulPos = ulFromPos + 5;
    else
      ulPos = ulStartPos;

    bDone = xbFalse;
    while( !bOrderByFound && !bDone ){
      ulPos = GetNextToken( sStmt, sToken, ulPos );
      if( sToken == "" ){
        bDone = xbTrue;
      } else {
        iParenCnt += GetParenCnt( sToken );
        if( iParenCnt == 0 ){
          sToken.ToUpperCase();
          if( sToken == "ORDER" ){
            xbString sToken2;
            xbUInt32 ulPos2 = GetNextToken( sStmt, sToken2, ulPos + 6 );
            if( sToken2 != "" ){
              sToken2.ToUpperCase();
              if( sToken2 == "BY" ){
                bOrderByFound = xbTrue;
                ulOrderByPos = ulPos;
                sOrderBy.Assign( sStmt, ulPos2 + 3 );
              }
            }
          }
        }
        ulPos += sToken.Len() + 1;
      }
    }


    bDone = xbFalse;
    while( !bGroupByFound && !bDone ){
      ulPos = GetNextToken( sStmt, sToken, ulPos );
      if( sToken == "" ){
        bDone = xbTrue;
      } else {
        iParenCnt += GetParenCnt( sToken );
        if( iParenCnt == 0 ){
          sToken.ToUpperCase();
          if( sToken == "GROUP" ){
            xbString sToken2;
            xbUInt32 ulPos2 = GetNextToken( sStmt, sToken2, ulPos + 6 );
            if( sToken2 != "" ){
              sToken2.ToUpperCase();
              if( sToken2 == "BY" ){
                bGroupByFound = xbTrue;
                ulGroupByPos = ulPos;
                sGroupBy.Assign( sStmt, ulPos2 + 3 );
              }
            }
          }
        }
        ulPos += sToken.Len() + 1;
      }
    }

    bDone = xbFalse;
    while( !bHavingFound && !bDone ){
      ulPos = GetNextToken( sStmt, sToken, ulPos );
      if( sToken == "" ){
        bDone = xbTrue;
      } else {
        iParenCnt += GetParenCnt( sToken );
        if( iParenCnt == 0 ){
          sToken.ToUpperCase();
          if( sToken == "HAVING" ){
            bHavingFound = xbTrue;
            ulHavingPos = ulPos;
            sHaving.Assign( sStmt, ulHavingPos + 7 );
          }
        }
        ulPos += sToken.Len() + 1;
      }
    }


    // do the fields part
    if( bFromFound )
      ulLen = ulFromPos - 7;
    else if( bWhereFound )
      ulLen = ulWherePos - 7;
    else if( bOrderByFound )
      ulLen = ulOrderByPos - 7;
    else if( bGroupByFound )
      ulLen = ulGroupByPos - 7;
    else if( bHavingFound )
      ulLen = ulHavingPos - 7;
    else 
      ulLen = sStmt.Len() - 7;
    sFields.Assign( sStmt, 7, ulLen );
    sFields.Trim();

    // do the FROM part
    if( bFromFound ){
      if( bWhereFound )
        ulLen = ulWherePos - ulFromPos;
      else if( bOrderByFound )
        ulLen = ulOrderByPos - ulFromPos;
      else if( bGroupByFound )
        ulLen = ulGroupByPos - ulFromPos;
      else if( bHavingFound )
        ulLen = ulHavingPos - ulFromPos;
      else 
        ulLen = sStmt.Len() - ulFromPos;
      sFrom.Resize( ulLen );
      sFrom.Trim();

    }


    // do the WHERE part
    if( bWhereFound ){
      ulLen = 0;
      if( bOrderByFound )
        ulLen = ulOrderByPos - ulWherePos - 6;
      else if( bGroupByFound )
        ulLen = ulGroupByPos - ulWherePos - 6;
      else if( bHavingFound )
        ulLen = ulHavingPos - ulWherePos - 6;

     if( ulLen > 0 )
        sWhere.Resize( ulLen );
      sWhere.Trim();
    }

    // FIXME if there is more than one space between ORDER and BY then this doesn't work quite right
    // do the ORDER BY part
    if( bOrderByFound ){
      if( bGroupByFound )
        ulLen = ulGroupByPos - ulOrderByPos - 9;
      else if( bHavingFound )
        ulLen = ulHavingPos - ulOrderByPos - 9;
      else 
        ulLen = sStmt.Len() - ulOrderByPos - 9;
      sOrderBy.Resize( ulLen );
      sOrderBy.Trim();
    }

    // FIXME if there is more than one space between ORDER and BY then this doesn't work quite right
    // do the GROUP BY part
    if( bGroupByFound ){
      if( bHavingFound )
        ulLen = ulHavingPos - ulGroupByPos - 9;
      else 
        ulLen = sStmt.Len() - ulGroupByPos - 9;
      sGroupBy.Resize( ulLen );
      sGroupBy.Trim();
    }

    if( bFromFound ){
      if(( iRc = ParseFromStmt( sFrom )) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      }
    }

  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbStmt::ParseStmt() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }

  return iRc;
}

/***********************************************************************/
xbInt16 xbStmt::ProcessFromSeg( const xbString &sFromSeg ){


  xbInt16  iRc        = 0;
  xbInt16  iErrorStop = 0;
  char     cType      = ' ';
  xbString sToken;
  xbString sTable;
  xbString sAlias;
  xbString sWork;
  xbString sExp;
  xbUInt32 iPos;
  xbBool   bDone;
  xbExp    * pJoinExp = NULL;

  try{

    std::cout << "ProcessFromSeg for segment [" << sFromSeg.Str() << "]\n";

    GetNextToken( sFromSeg, sToken, 1 );
    sToken.ToUpperCase();

    std::cout << "sToken1 = [" << sToken.Str() << "]\n";

    if( sToken == "FROM" ){
      // FROM has to be the first statement, and exist only once
      if( pTblList ){
        iErrorStop = 100;
        iRc = XB_PARSE_ERROR;
        throw iRc;
      }
      cType = 'M';
    } else if( sToken == "LEFT" )
      cType = 'L';
    else if( sToken == "RIGHT" )
      cType = 'R';
    else if( sToken == "RIGHT" )
      cType = 'R';
    else if( sToken == "INNER" )
      cType = 'I';
    else if( sToken == "OUTER" || sToken == "FULL" )
      cType = 'O';
    else if( sToken[1] == '(' )
      cType = 'Q';
    else{
      iErrorStop = 100;
      iRc = XB_PARSE_ERROR;
      throw iRc;
    }


    if( cType == 'M' ){
      // expecting "FROM table"  or "FROM table alias"
      iPos = GetNextToken( sFromSeg, sToken, 5 );
      sTable = sToken;

      iPos = GetNextToken( sFromSeg, sToken, iPos + sToken.Len() + 1 );
      if( sToken.Len() > 0 )
        sAlias = sToken;
      else
        sAlias = "";

      sWork = sToken;
      sWork.ToUpperCase();
      iPos = sWork.Pos( ".DBF" );
      if( iPos == 0 )
        sTable += ".DBF";

   //   std::cout << "table = [" << sTable.Str() << "] alias = [" << sAlias.Str() << "]\n";

    } else if( cType != 'Q' ){
      sWork = sFromSeg;
      sWork.ToUpperCase();
      iPos = sWork.Pos( " ON " );
      if( iPos == 0 ){
        iErrorStop = 110;
        iRc = XB_PARSE_ERROR;
        throw iRc;
      }
      bDone = xbFalse;

      sWork.Assign( sFromSeg, iPos + 4 );
      CvtSqlExp2DbaseExp( sWork, sExp );
      sExp.Trim();
      std::cout << "ON = [" << sExp.Str() << "]\n";


      // need to get past the keywords LEFT, FULL, OUTER, RIGHT, INNER and get the table name here
      iPos = sToken.Len() + 1;
      while( !bDone ){
        iPos = GetNextToken( sFromSeg, sToken, iPos );
        sWork = sToken;
        sWork.ToUpperCase();
        if( sWork != "LEFT" && sWork != "RIGHT" && sWork != "INNER" && sWork != "OUTER" && sWork != "FULL" && sWork != "JOIN" )
          bDone = xbTrue;
        else
          iPos += (sWork.Len() + 1);
      }

      sTable = sToken;
      GetNextToken( sFromSeg, sAlias, iPos + sTable.Len() + 1 );
      sWork.ToUpperCase();
      iPos = sWork.Pos( ".DBF" );
      if( iPos == 0 )
        sTable += ".DBF";
      sWork = sAlias;
      sWork.ToUpperCase();
      if( sWork == "ON" )
        sAlias = "";
    }


    // open table
    //std::cout << "attempting to open table " << sTable.Str() << "\n";
    xbDbf *d = NULL;
    if(( iRc = xbase->OpenHighestVersion( sTable, sAlias, &d )) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw iRc;
    }



    // build out table linkage expression
    if( cType == 'M' )
      sExp = "MASTER";
    else if( cType == 'L' ){
      pJoinExp = new xbExp( xbase );

      //  std::cout << "Parsing expression [" << sExp.Str() << "]\n";
      if(( iRc = pJoinExp->ParseExpression( d, sExp )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }
   }


    // attach to correct location in list


    // update the UpdateJoinList routine as appropriate
//    if(( iRc = UpdateJoinList( cType, sTable, sAlias, NULL, NULL, NULL, NULL )) != XB_NO_ERROR ){

    if(( iRc = UpdateJoinList( cType, sTable, sAlias, sExp, d, pJoinExp )) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }


  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbStmt::ProcessFromSeg() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}

/***********************************************************************/
xbInt16  xbStmt::UpdateJoinList( char cType, const xbString &sTableName, const xbString &sAlias, const xbString &sExp, xbDbf *d, xbExp *e ){
//  xbTag *t ){


  std::cout << "Update join list for table " << sTableName.Str() << "\n";

  xbTblJoin *pTj;
  if(( pTj = (xbTblJoin *) calloc( 1, sizeof( xbTblJoin ))) == NULL )
    return XB_NO_MEMORY;

  pTj->cJoinType = cType;
  pTj->sTableName.Set( sTableName );
  pTj->sAlias.Set( sAlias );
  pTj->sJoinExp.Set( sExp );
  pTj->pDbf = d;
  pTj->pLinkExp = e;

/*
  
  pTj->pTag = t;
*/

  if( cType == 'M' )
    pTblList = pTj;

  return 0;
}
/***********************************************************************/
}              /* namespace       */
#endif         /*  XB_SQL_SUPPORT */

