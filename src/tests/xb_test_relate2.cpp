/* xb_test_relate2.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023,2024 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

// This program tests the relation logic functions
// usage:   xb_test_relate2 QUITE|NORMAL|VERBOSE


// Foreign Key structure
//  table, record, foreign key



#include "xbase.h"

using namespace xb;

#include "tstfuncs.cpp"



//************************************************************************************************
void DumpRecs( xbXBase *x, xbInt16 iDest, xbInt16 iFmt, xbString sMsgIn, xbDbf *d1 = NULL, xbDbf *d2 = NULL, xbDbf *d3 = NULL, xbDbf *d4 = NULL );
void DumpRecs( xbXBase *x, xbInt16 iDest, xbInt16 iFmt, xbString sMsgIn, xbDbf *d1, xbDbf *d2, xbDbf *d3, xbDbf *d4 ){

// iDest 0 = log, 1 = stdout, 2 = both
// iFmt  0 = field names, 1 = 1 line per record , 2 = 1 line per record with field names

  xbString sMsg;

  if( sMsgIn )
    x->WriteLogMessage( sMsgIn.Str(), iDest );

  if( d1 && d1->GetCurRecNo() > 0 ){
    sMsg.Sprintf( "T1 [%s] Record: %d", d1->GetFileName().Str(), d1->GetCurRecNo());
    x->WriteLogMessage( sMsg.Str(), iDest );
    d1->DumpRecord( d1->GetCurRecNo(), iDest, iFmt );
  }

  if( d2 && d2->GetCurRecNo() > 0 ){
    sMsg.Sprintf( "T2 [%s] Record: %d", d2->GetFileName().Str(), d2->GetCurRecNo());
    x->WriteLogMessage( sMsg.Str(), iDest );
    d2->DumpRecord( d2->GetCurRecNo(), iDest, iFmt );
  }

  if( d3 && d3->GetCurRecNo() > 0 ){
    sMsg.Sprintf( "T3 [%s] Record: %d", d3->GetFileName().Str(), d3->GetCurRecNo());
    x->WriteLogMessage( sMsg.Str(), iDest );
    d3->DumpRecord( d3->GetCurRecNo(), iDest, iFmt );
  }

  if( d4 && d4->GetCurRecNo() > 0 ){
    sMsg.Sprintf( "T4 [%s] Record: %d", d4->GetFileName().Str(), d4->GetCurRecNo());
    x->WriteLogMessage( sMsg.Str(), iDest );
    d4->DumpRecord( d4->GetCurRecNo(), iDest, iFmt );
  }

}

//************************************************************************************************
int main( int argCnt, char **av )
{
  int iRc = 0;
  int iRc2 = 0;
  int iPo = 1;         /* print option */
                       /* 0 - QUIET    */
                       /* 1 - NORMAL   */
                       /* 2 - VERBOSE  */


  if( argCnt > 1 ) {
    if( av[1][0] == 'Q' )
      iPo = 0;
    else if( av[1][0] == 'V' )
      iPo = 2;
  }

  xbXBase x;

  x.SetDataDirectory( PROJECT_DATA_DIR );
  x.SetMultiUser( xbFalse );

  xbDbf * dRel0       = new xbDbf4( &x );   // Root table
  xbDbf * dRel1Mfml   = new xbDbf4( &x );   // Missing first, middle & last tuples
  xbDbf * dRel1Mtch   = new xbDbf4( &x );   // 1:1 match with REL0
  xbDbf * dRel1DK     = new xbDbf4( &x );   // Duplicate key vals
  xbDbf * dRelEmpty   = new xbDbf4( &x );   // Empty table
  xbDbf * dRel2Mtch   = new xbDbf4( &x );   // 1:1 match with dRel1Mtch



  #ifdef XB_LOGGING_SUPPORT
  x.SetLogDirectory( PROJECT_LOG_DIR );
  x.EnableMsgLogging();
  if( iPo ){
    std::cout << "Logfile is [" << x.GetLogFqFileName().Str() << "]" << std::endl;
  }
  xbString sMsg;
  sMsg.Sprintf( "Program [%s] initializing...", av[0] );
  x.WriteLogMessage( sMsg );
  sMsg.Sprintf( "Program [%s] generates error messages -123, -601 and -602 in the log file as part of testing", av[0] );
  x.WriteLogMessage( sMsg );
  #endif

  iRc2 = dRel0->Open( "RL0.DBF" );
  iRc += TestMethod( iPo, "Open(1)", iRc2, 0 );

  iRc2 = dRel1Mtch->Open( "RL1MTCH.DBF" );
  iRc += TestMethod( iPo, "Open(2)", iRc2, 0 );

  iRc2 = dRel1DK->Open( "RL1DK.DBF" );
  iRc += TestMethod( iPo, "Open(3)", iRc2, 0 );

  iRc2 = dRel1Mfml->Open( "RL1MFML.DBF" );
  iRc += TestMethod( iPo, "Open(4)", iRc2, 0 );

  iRc2 = dRelEmpty->Open( "RLEMPTY.DBF" );
  iRc += TestMethod( iPo, "Open(5)", iRc2, 0 );


  iRc2 = dRel2Mtch->Open( "RL2MTCH.DBF" );
  iRc += TestMethod( iPo, "Open(6)", iRc2, 0 );

  if( iPo > 1 ){
    std::cout << "RL0.DBF Current Tag      = [" << dRel0->GetCurTagName().Str()     << "]\n";
    std::cout << "RL1MFML.DBF Current Tag  = [" << dRel1Mfml->GetCurTagName().Str() << "]\n";
    std::cout << "RL1MATCH.DBF Current Tag = [" << dRel1Mtch->GetCurTagName().Str() << "]\n";
    std::cout << "RL1DK.DBF Current Tag    = [" << dRel1DK->GetCurTagName().Str()   << "]\n";
    std::cout << "RLEMPTY.DBF Current Tag  = [" << dRelEmpty->GetCurTagName().Str()   << "]\n";
    std::cout << "RL2MATCH.DBF Current Tag = [" << dRel2Mtch->GetCurTagName().Str() << "]\n";
  }


//***************************************************************************************
// Test one in xb_test_relate1 program

// Test two no filter, no slave relations, loop through master table

  xbRelate * pRel2;
  pRel2  = new xbRelate( &x );

  iRc2 = pRel2->SetMaster( dRel0 );
  iRc += TestMethod( iPo, "SetMaster(200)", iRc2, 0 );

  // iRc2 = pRel2->InitQuery();
  // iRc += TestMethod( iPo, "InitQuery(201)", iRc2, 0 );
  iRc2 = pRel2->GetFirstRelRecord();
  iRc += TestMethod( iPo, "GetFirstRelRecord(202)", iRc2, XB_NO_ERROR );

  xbUInt32 ulRecCnt;
  iRc2 = dRel0->GetRecordCnt( ulRecCnt );
  iRc += TestMethod( iPo, "GetRecordCnt(203)", iRc2, XB_NO_ERROR );
  xbUInt32 iLoopCtr = 1;
  while( iRc2 == XB_NO_ERROR && iLoopCtr++ < ulRecCnt ){
    iRc2 = pRel2->GetNextRelRecord();
    iRc += TestMethod( iPo, "GetNextRelRecord(204)", iRc2, XB_NO_ERROR );
    iRc += TestMethod( iPo, "GetCurRec(205)", (xbInt32) dRel0->GetCurRecNo(), (xbInt32) iLoopCtr );
  }

  iRc2 = pRel2->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(206)", iRc2, XB_EOF );

  iRc2 = pRel2->GetLastRelRecord();
  iRc += TestMethod( iPo, "GetLastRelRecord(207)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(208)", pRel2->CheckRecNo( iPo, 5 ),  0 );

  iLoopCtr = 5;
  while( iRc2 == XB_NO_ERROR && iLoopCtr-- > 1 ){
    iRc2 = pRel2->GetPrevRelRecord();
    iRc += TestMethod( iPo, "GetPrevRelRecord(210)", iRc2, XB_NO_ERROR );
    iRc += TestMethod( iPo, "GetCurRec(211)", (xbInt32) dRel0->GetCurRecNo(), (xbInt32) iLoopCtr );
  }

  iRc2 = pRel2->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(212)", iRc2, XB_BOF );


//***************************************************************************************

// Test three - no filter, one opt slave relation, matches 1:1 - loop through the relation

  xbRelate * pRel3;
  pRel3  = new xbRelate( &x );


  iRc2 = pRel3->SetMaster( dRel0 );
  // test for XB_ALREADY_DEFINED throws a -123 in the logfile, correct response
  iRc += TestMethod( iPo, "SetMaster(300)", iRc2, XB_ALREADY_DEFINED );

  delete pRel2;
  pRel2 = NULL;

  iRc2 = pRel3->SetMaster( dRel0 );
  iRc += TestMethod( iPo, "SetMaster(301)", iRc2, XB_NO_ERROR );

  // test for XB_INVALID_INDEX - throws a -601 in the logfile, correct response
  iRc2 = pRel3->AddRelation( dRel0, "FORGNKEY1", dRel1Mtch, NULL, NULL, 'O' );
  iRc += TestMethod( iPo, "AddRelation(302)", iRc2, XB_INVALID_INDEX );

  // test for XB_INVALID_TAG - throws a -602 in the logfile, correct response
  iRc2 = pRel3->AddRelation( dRel0, "FORGNKEY1", dRel1Mtch, dRel1Mtch->GetCurIx(), NULL, 'O' );
  iRc += TestMethod( iPo, "AddRelation(303)", iRc2, XB_INVALID_TAG );

  iRc2 = pRel3->AddRelation( dRel0, "FORGN_KS2", dRel1Mtch, dRel1Mtch->GetCurIx(), dRel1Mtch->GetCurTag(), 'O' );
  iRc += TestMethod( iPo, "AddRelation(304)", iRc2, XB_NO_ERROR );

  // iRc2 = pRel3->InitQuery();   this is done automatically be the GetFirstRelRecord() method

  iRc2 = pRel3->GetFirstRelRecord();
  iRc += TestMethod( iPo, "GetFirstRelRecord(305)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(306)", pRel3->CheckRecNo( iPo, 1, 1 ),  0 );

  iRc2 = pRel3->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(306)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(307)", pRel3->CheckRecNo( iPo, 2, 2 ),  0 );

  iRc2 = pRel3->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(308)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(309)", pRel3->CheckRecNo( iPo, 3, 3 ),  0 );

  iRc2 = pRel3->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(310)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(311)", pRel3->CheckRecNo( iPo, 4, 4 ),  0 );

  iRc2 = pRel3->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(312)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(313)", pRel3->CheckRecNo( iPo, 5, 5 ),  0 );

  iRc2 = pRel3->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(314)", iRc2, XB_EOF );
  iRc += TestMethod( iPo, "CheckRecNo(315)", pRel3->CheckRecNo( iPo, 0, 0 ),  0 );

  iRc2 = pRel3->GetFirstRelRecord();
  iRc += TestMethod( iPo, "GetFirstRelRecord(316)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(317)", pRel3->CheckRecNo( iPo, 1, 1 ),  0 );

  iRc2 = pRel3->GetLastRelRecord();
  iRc += TestMethod( iPo, "GetLastRelRecord(318)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(318)", pRel3->CheckRecNo( iPo, 5, 5 ),  0 );

  iRc2 = pRel3->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(319)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(320)", pRel3->CheckRecNo( iPo, 4, 4 ),  0 );

  iRc2 = pRel3->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(321)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(322)", pRel3->CheckRecNo( iPo, 3, 3 ),  0 );

  iRc2 = pRel3->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(323)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(324)", pRel3->CheckRecNo( iPo, 2, 2 ),  0 );

  iRc2 = pRel3->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(325)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(326)", pRel3->CheckRecNo( iPo, 1, 1 ),  0 );

  iRc2 = pRel3->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(327)", iRc2, XB_BOF );

  delete pRel3;
  pRel3 = NULL;


//***************************************************************************************

// Test four - no filter, one rqd slave relation, loop through the relation

  xbRelate * pRel4;
  pRel4  = new xbRelate( &x );


  iRc2 = pRel4->SetMaster( dRel0 );
  iRc += TestMethod( iPo, "SetMaster(400)", iRc2, 0 );

  iRc2 = pRel4->AddRelation( dRel0, "FORGN_KS2", dRel1Mtch, dRel1Mtch->GetCurIx(), dRel1Mtch->GetCurTag(), 'R' );
  iRc += TestMethod( iPo, "AddRelation(401)", iRc2, XB_NO_ERROR );

  iRc2 = pRel4->GetFirstRelRecord();
  iRc += TestMethod( iPo, "GetFirstRelRecord(402)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(403)", pRel4->CheckRecNo( iPo, 1, 1 ),  0 );

  iRc2 = pRel4->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(404)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(405)", pRel4->CheckRecNo( iPo, 2, 2 ),  0 );

  iRc2 = pRel4->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(406)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(407)", pRel4->CheckRecNo( iPo, 3, 3 ),  0 );

  iRc2 = pRel4->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(408)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(409)", pRel4->CheckRecNo( iPo, 4, 4 ),  0 );

  iRc2 = pRel4->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(410)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(411)", pRel4->CheckRecNo( iPo, 5, 5 ),  0 );

  iRc2 = pRel4->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(412)", iRc2, XB_EOF );
  iRc += TestMethod( iPo, "CheckRecNo(413)", pRel4->CheckRecNo( iPo, 0, 0 ),  0 );

  iRc2 = pRel4->GetFirstRelRecord();
  iRc += TestMethod( iPo, "GetFirstRelRecord(414)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(415)", pRel4->CheckRecNo( iPo, 1, 1 ),  0 );

  iRc2 = pRel4->GetLastRelRecord();
  iRc += TestMethod( iPo, "GetLastRelRecord(416)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(417)", pRel4->CheckRecNo( iPo, 5, 5 ),  0 );

  iRc2 = pRel4->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(418)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(419)", pRel4->CheckRecNo( iPo, 4, 4 ),  0 );

  iRc2 = pRel4->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(420)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(421)", pRel4->CheckRecNo( iPo, 3, 3 ),  0 );
  iRc2 = pRel4->GetPrevRelRecord();

  iRc += TestMethod( iPo, "GetPrevRelRecord(422)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(423)", pRel4->CheckRecNo( iPo, 2, 2 ),  0 );

  iRc2 = pRel4->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(424)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(425)", pRel4->CheckRecNo( iPo, 1, 1 ),  0 );

  iRc2 = pRel4->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(424)", iRc2, XB_BOF );

  delete pRel4;
  pRel4 = NULL;

//***************************************************************************************
// Test five - no filter, one opt slave relation, slave missing first, last and missing tuple

  xbRelate * pRel5;
  pRel5  = new xbRelate( &x );


  iRc2 = pRel5->SetMaster( dRel0 );
  iRc += TestMethod( iPo, "SetMaster(500)", iRc2, 0 );

  iRc2 = pRel5->AddRelation( dRel0, "FORGN_KS1", dRel1Mfml, dRel1Mfml->GetCurIx(), dRel1Mfml->GetCurTag(), 'O' );
  iRc += TestMethod( iPo, "AddRelation(501)", iRc2, XB_NO_ERROR );

  iRc2 = pRel5->GetFirstRelRecord();
  iRc += TestMethod( iPo, "GetFirstRelRecord(502)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(503)", pRel5->CheckRecNo( iPo, 1, 0 ),  0 );

  iRc2 = pRel5->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(504)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(505)", pRel5->CheckRecNo( iPo, 2, 1 ),  0 );

  iRc2 = pRel5->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(506)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(507)", pRel5->CheckRecNo( iPo, 3, 0 ),  0 );

  iRc2 = pRel5->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(508)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(509)", pRel5->CheckRecNo( iPo, 4, 2 ),  0 );

  iRc2 = pRel5->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(510)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(511)", pRel5->CheckRecNo( iPo, 5, 0 ),  0 );

  iRc2 = pRel5->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(512)", iRc2, XB_EOF );
  iRc += TestMethod( iPo, "CheckRecNo(513)", pRel5->CheckRecNo( iPo, 0, 0 ),  0 );

  iRc2 = pRel5->GetLastRelRecord();
  iRc += TestMethod( iPo, "GetLastRelRecord(514)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(515)", pRel5->CheckRecNo( iPo, 5, 0 ),  0 );

  iRc2 = pRel5->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(516)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(517)", pRel5->CheckRecNo( iPo, 4, 2 ),  0 );

  iRc2 = pRel5->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(518)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(519)", pRel5->CheckRecNo( iPo, 3, 0 ),  0 );

  iRc2 = pRel5->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(520)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(521)", pRel5->CheckRecNo( iPo, 2, 1 ),  0 );

  iRc2 = pRel5->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(522)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(523)", pRel5->CheckRecNo( iPo, 1, 0 ),  0 );

  iRc2 = pRel5->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(524)", iRc2, XB_BOF );

  delete pRel5;
  pRel5 = NULL;

//***************************************************************************************
// Test six - no filter, one req slave relation, slave missing first, last and missing tuple

  xbRelate * pRel6;
  pRel6  = new xbRelate( &x );


  iRc2 = pRel6->SetMaster( dRel0 );
  iRc += TestMethod( iPo, "SetMaster(600)", iRc2, 0 );
  iRc2 = pRel6->AddRelation( dRel0, "FORGN_KS1", dRel1Mfml, dRel1Mfml->GetCurIx(), dRel1Mfml->GetCurTag(), 'R' );
  iRc += TestMethod( iPo, "AddRelation(601)", iRc2, XB_NO_ERROR );

  iRc2 = pRel6->GetFirstRelRecord();
  iRc += TestMethod( iPo, "GetFirstRelRecord(602)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(603)", pRel6->CheckRecNo( iPo, 2, 1 ),  0 );

  iRc2 = pRel6->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(604)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(605)", pRel6->CheckRecNo( iPo, 4, 2 ),  0 );

  iRc2 = pRel6->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(606)", iRc2, XB_EOF );
  iRc += TestMethod( iPo, "CheckRecNo(607)", pRel6->CheckRecNo( iPo, 0, 0 ),  0 );

  iRc2 = pRel6->GetLastRelRecord();
  iRc += TestMethod( iPo, "GetLastRelRecord(608)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(609)", pRel6->CheckRecNo( iPo, 4, 2 ),  0 );

  iRc2 = pRel6->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(610)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(611)", pRel6->CheckRecNo( iPo, 2, 1 ),  0 );

  iRc2 = pRel6->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(614)", iRc2, XB_BOF );


  delete pRel6;
  pRel6 = NULL;


//***************************************************************************************
// Test seven - no filter, all optional  M->C->C

  xbRelate * pRel7;
  pRel7  = new xbRelate( &x );

  iRc2 = pRel7->SetMaster( dRel0 );
  iRc += TestMethod( iPo, "SetMaster(700)", iRc2, 0 );

  iRc2 = pRel7->AddRelation( dRel0, "FORGN_KS2", dRel1Mtch, dRel1Mtch->GetCurIx(), dRel1Mtch->GetCurTag(), 'O' );
  iRc += TestMethod( iPo, "AddRelation(701)", iRc2, XB_NO_ERROR );

  iRc2 = pRel7->AddRelation( dRel1Mtch, "FORGN_KS5", dRel2Mtch, dRel2Mtch->GetCurIx(), dRel2Mtch->GetCurTag(), 'O' );
  iRc += TestMethod( iPo, "AddRelation(702)", iRc2, XB_NO_ERROR );

  iRc2 = pRel7->GetFirstRelRecord();
  iRc += TestMethod( iPo, "GetFirstRelRecord(703)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(704)", pRel7->CheckRecNo( iPo, 1, 1, 1 ),  0 );

  iRc2 = pRel7->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(705)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(706)", pRel7->CheckRecNo( iPo, 2, 2, 2 ),  0 );

  iRc2 = pRel7->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(707)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(708)", pRel7->CheckRecNo( iPo, 3, 3, 3 ),  0 );

  iRc2 = pRel7->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(709)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(710)", pRel7->CheckRecNo( iPo, 4, 4, 4 ),  0 );

  iRc2 = pRel7->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(711)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(712)", pRel7->CheckRecNo( iPo, 5, 5, 5 ),  0 );

  iRc2 = pRel7->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(713)", iRc2, XB_EOF );
  iRc += TestMethod( iPo, "CheckRecNo(714)", pRel7->CheckRecNo( iPo, 0, 0, 0 ),  0 );

  iRc2 = pRel7->GetFirstRelRecord();
  iRc += TestMethod( iPo, "GetFirstRelRecord(714)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(715)", pRel7->CheckRecNo( iPo, 1, 1, 1 ),  0 );

  iRc2 = pRel7->GetLastRelRecord();
  iRc += TestMethod( iPo, "GetLastRelRecord(716)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(717)", pRel7->CheckRecNo( iPo, 5, 5, 5 ),  0 );

  iRc2 = pRel7->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(718)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(719)", pRel7->CheckRecNo( iPo, 4, 4, 4 ),  0 );

  iRc2 = pRel7->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(720)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(721)", pRel7->CheckRecNo( iPo, 3, 3, 3 ),  0 );

  iRc2 = pRel7->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(722)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(723)", pRel7->CheckRecNo( iPo, 2, 2, 2 ),  0 );

  iRc2 = pRel7->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(724)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(725)", pRel7->CheckRecNo( iPo, 1, 1, 1 ),  0 );

  iRc2 = pRel7->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(726)", iRc2, XB_BOF );

  delete pRel7;
  pRel7 = NULL;

//***************************************************************************************
// Test eight - no filter, all required  M->C->C

  xbRelate * pRel8;
  pRel8  = new xbRelate( &x );

  iRc2 = pRel8->SetMaster( dRel0 );
  iRc += TestMethod( iPo, "SetMaster(800)", iRc2, 0 );

  iRc2 = pRel8->AddRelation( dRel0, "FORGN_KS2", dRel1Mtch, dRel1Mtch->GetCurIx(), dRel1Mtch->GetCurTag(), 'R' );
  iRc += TestMethod( iPo, "AddRelation(801)", iRc2, XB_NO_ERROR );

  iRc2 = pRel8->AddRelation( dRel1Mtch, "FORGN_KS5", dRel2Mtch, dRel2Mtch->GetCurIx(), dRel2Mtch->GetCurTag(), 'R' );
  iRc += TestMethod( iPo, "AddRelation(802)", iRc2, XB_NO_ERROR );

  iRc2 = pRel8->GetFirstRelRecord();
  iRc += TestMethod( iPo, "GetFirstRelRecord(803)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(804)", pRel8->CheckRecNo( iPo, 1, 1, 1 ),  0 );

  iRc2 = pRel8->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(805)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(806)", pRel8->CheckRecNo( iPo, 2, 2, 2 ),  0 );

  iRc2 = pRel8->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(807)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(808)", pRel8->CheckRecNo( iPo, 3, 3, 3 ),  0 );

  iRc2 = pRel8->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(809)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(810)", pRel8->CheckRecNo( iPo, 4, 4, 4 ),  0 );

  iRc2 = pRel8->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(810)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(811)", pRel8->CheckRecNo( iPo, 5, 5, 5 ),  0 );

  iRc2 = pRel8->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(812)", iRc2, XB_EOF );
  iRc += TestMethod( iPo, "CheckRecNo(813)", pRel8->CheckRecNo( iPo, 0, 0, 0 ),  0 );

  iRc2 = pRel8->GetFirstRelRecord();
  iRc += TestMethod( iPo, "GetFirstRelRecord(814)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(815)", pRel8->CheckRecNo( iPo, 1, 1, 1 ),  0 );

  iRc2 = pRel8->GetLastRelRecord();
  iRc += TestMethod( iPo, "GetLastRelRecord(816)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(817)", pRel8->CheckRecNo( iPo, 5, 5, 5 ),  0 );

  iRc2 = pRel8->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(820)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(821)", pRel8->CheckRecNo( iPo, 4, 4, 4 ),  0 );

  iRc2 = pRel8->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(822)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(823)", pRel8->CheckRecNo( iPo, 3, 3, 3 ),  0 );

  iRc2 = pRel8->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(824)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(825)", pRel8->CheckRecNo( iPo, 2, 2, 2 ),  0 );

  iRc2 = pRel8->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(826)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(827)", pRel8->CheckRecNo( iPo, 1, 1, 1 ),  0 );

  iRc2 = pRel8->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(828)", iRc2, XB_BOF );

  delete pRel8;
  pRel8 = NULL;

//***************************************************************************************
// Test nine - no filter, master with one optional child having dup recs

  xbRelate * pRel9;
  pRel9  = new xbRelate( &x );


  iRc2 = pRel9->SetMaster( dRel0 );
  iRc += TestMethod( iPo, "SetMaster(900)", iRc2, 0 );
  iRc2 = pRel9->AddRelation( dRel0, "FORGN_KS3", dRel1DK, dRel1DK->GetCurIx(), dRel1DK->GetCurTag(), 'O' );
  iRc += TestMethod( iPo, "AddRelation(601)", iRc2, XB_NO_ERROR );

  iRc2 = pRel9->GetFirstRelRecord();
  iRc += TestMethod( iPo, "GetFirstRelRecord(902)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(903)", pRel9->CheckRecNo( iPo, 1, 1 ),  0 );

  iRc2 = pRel9->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(904)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(905)", pRel9->CheckRecNo( iPo, 2, 2 ),  0 );

  iRc2 = pRel9->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(906)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(907)", pRel9->CheckRecNo( iPo, 2, 3 ),  0 );

  iRc2 = pRel9->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(908)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(909)", pRel9->CheckRecNo( iPo, 3, 4 ),  0 );

  iRc2 = pRel9->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(910)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(911)", pRel9->CheckRecNo( iPo, 3, 5 ),  0 );

  iRc2 = pRel9->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(912)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(913)", pRel9->CheckRecNo( iPo, 3, 6 ),  0 );

  iRc2 = pRel9->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(914)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(915)", pRel9->CheckRecNo( iPo, 4, 7 ),  0 );

  iRc2 = pRel9->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(916)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(917)", pRel9->CheckRecNo( iPo, 5, 8 ),  0 );

  iRc2 = pRel9->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(918)", iRc2, XB_EOF );

  iRc2 = pRel9->GetFirstRelRecord();
  iRc += TestMethod( iPo, "GetFirstRelRecord(919)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(920)", pRel9->CheckRecNo( iPo, 1, 1 ),  0 );

  iRc2 = pRel9->GetLastRelRecord();
  iRc += TestMethod( iPo, "GetLastRelRecord(921)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(922)", pRel9->CheckRecNo( iPo, 5, 8 ),  0 );

  iRc2 = pRel9->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(923)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(924)", pRel9->CheckRecNo( iPo, 4, 7 ),  0 );

  iRc2 = pRel9->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(925)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(926)", pRel9->CheckRecNo( iPo, 3, 6 ),  0 );

  iRc2 = pRel9->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(927)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(928)", pRel9->CheckRecNo( iPo, 3, 5 ),  0 );

  iRc2 = pRel9->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(929)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(930)", pRel9->CheckRecNo( iPo, 3, 4 ),  0 );

  iRc2 = pRel9->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(931)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(932)", pRel9->CheckRecNo( iPo, 2, 3 ),  0 );

  iRc2 = pRel9->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(933)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(934)", pRel9->CheckRecNo( iPo, 2, 2 ),  0 );

  iRc2 = pRel9->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(935)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(936)", pRel9->CheckRecNo( iPo, 1, 1 ),  0 );

  iRc2 = pRel9->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(925)", iRc2, XB_BOF );

  delete pRel9;
  pRel9 = NULL;

//***************************************************************************************
// Test ten - no filter, master with one required child having dup recs

  xbRelate * pRel10;
  pRel10 = new xbRelate( &x );




  iRc2 = pRel10->SetMaster( dRel0 );
  iRc += TestMethod( iPo, "SetMaster(1000)", iRc2, 0 );
  iRc2 = pRel10->AddRelation( dRel0, "FORGN_KS3", dRel1DK, dRel1DK->GetCurIx(), dRel1DK->GetCurTag(), 'R' );
  iRc += TestMethod( iPo, "AddRelation(1001)", iRc2, XB_NO_ERROR );

  iRc2 = pRel10->GetFirstRelRecord();
  iRc += TestMethod( iPo, "GetFirstRelRecord(1002)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1003)", pRel10->CheckRecNo( iPo, 1, 1 ),  0 );

  iRc2 = pRel10->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1004)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1005)", pRel10->CheckRecNo( iPo, 2, 2 ),  0 );

  iRc2 = pRel10->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1006)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1007)", pRel10->CheckRecNo( iPo, 2, 3 ),  0 );

  iRc2 = pRel10->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1008)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1009)", pRel10->CheckRecNo( iPo, 3, 4 ),  0 );

  iRc2 = pRel10->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1010)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1011)", pRel10->CheckRecNo( iPo, 3, 5 ),  0 );

  iRc2 = pRel10->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1012)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1013)", pRel10->CheckRecNo( iPo, 3, 6 ),  0 );

  iRc2 = pRel10->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1014)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1015)", pRel10->CheckRecNo( iPo, 4, 7 ),  0 );

  iRc2 = pRel10->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1016)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1017)", pRel10->CheckRecNo( iPo, 5, 8 ),  0 );

  iRc2 = pRel10->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1018)", iRc2, XB_EOF );

  iRc2 = pRel10->GetFirstRelRecord();
  iRc += TestMethod( iPo, "GetFirstRelRecord(1019)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1020)", pRel10->CheckRecNo( iPo, 1, 1 ),  0 );

  iRc2 = pRel10->GetLastRelRecord();
  iRc += TestMethod( iPo, "GetLastRelRecord(1021)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1022)", pRel10->CheckRecNo( iPo, 5, 8 ),  0 );

  iRc2 = pRel10->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1023)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1024)", pRel10->CheckRecNo( iPo, 4, 7 ),  0 );

  iRc2 = pRel10->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1025)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1026)", pRel10->CheckRecNo( iPo, 3, 6 ),  0 );

  iRc2 = pRel10->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1027)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1028)", pRel10->CheckRecNo( iPo, 3, 5 ),  0 );

  iRc2 = pRel10->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1029)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1030)", pRel10->CheckRecNo( iPo, 3, 4 ),  0 );

  iRc2 = pRel10->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1031)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1032)", pRel10->CheckRecNo( iPo, 2, 3 ),  0 );

  iRc2 = pRel10->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1033)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1034)", pRel10->CheckRecNo( iPo, 2, 2 ),  0 );

  iRc2 = pRel10->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1035)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1036)", pRel10->CheckRecNo( iPo, 1, 1 ),  0 );

  iRc2 = pRel10->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1037)", iRc2, XB_BOF );


  delete pRel10;
  pRel10 = NULL;

//***************************************************************************************
// Test eleven - no filter, master with three optional children
//             - match
//             - missing
//             - dups

  xbRelate * pRel11;
  pRel11 = new xbRelate( &x );


  iRc2 = pRel11->SetMaster( dRel0 );
  iRc += TestMethod( iPo, "SetMaster(1100)", iRc2, 0 );

  iRc2 = pRel11->AddRelation( dRel0, "FORGN_KS1", dRel1Mfml, dRel1Mfml->GetCurIx(), dRel1Mfml->GetCurTag(), 'O' );
  iRc += TestMethod( iPo, "AddRelation(1101)", iRc2, XB_NO_ERROR );

  iRc2 = pRel11->AddRelation( dRel0, "FORGN_KS2", dRel1Mtch, dRel1Mtch->GetCurIx(), dRel1Mtch->GetCurTag(), 'O' );
  iRc += TestMethod( iPo, "AddRelation(1102)", iRc2, XB_NO_ERROR );

  iRc2 = pRel11->AddRelation( dRel0, "FORGN_KS3", dRel1DK, dRel1DK->GetCurIx(), dRel1DK->GetCurTag(), 'O' );
  iRc += TestMethod( iPo, "AddRelation(1103)", iRc2, XB_NO_ERROR );

  iRc2 = pRel11->GetFirstRelRecord();
  iRc += TestMethod( iPo, "GetFirstRelRecord(1104)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1105)", pRel11->CheckRecNo( iPo, 1, 0, 1, 1 ),  0 );

  iRc2 = pRel11->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1106)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1107)", pRel11->CheckRecNo( iPo, 2, 1, 2, 2 ),  0 );

  iRc2 = pRel11->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1108)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1109)", pRel11->CheckRecNo( iPo, 2, 1, 2, 3 ),  0 );

  iRc2 = pRel11->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1110)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1111)", pRel11->CheckRecNo( iPo, 3, 0, 3, 4 ),  0 );

  iRc2 = pRel11->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1112)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1113)", pRel11->CheckRecNo( iPo, 3, 0, 3, 5 ),  0 );

  iRc2 = pRel11->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1114)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1115)", pRel11->CheckRecNo( iPo, 3, 0, 3, 6 ),  0 );

  iRc2 = pRel11->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1116)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1117)", pRel11->CheckRecNo( iPo, 4, 2, 4, 7 ),  0 );

  iRc2 = pRel11->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1118)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1119)", pRel11->CheckRecNo( iPo, 5, 0, 5, 8 ),  0 );

  iRc2 = pRel11->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1120)", iRc2, XB_EOF );

  iRc2 = pRel11->GetFirstRelRecord();
  iRc += TestMethod( iPo, "GetFirstRelRecord(1121)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1121)", pRel11->CheckRecNo( iPo, 1, 0, 1, 1 ),  0 );

  iRc2 = pRel11->GetLastRelRecord();
  iRc += TestMethod( iPo, "GetLastRelRecord(1122)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1123)", pRel11->CheckRecNo( iPo, 5, 0, 5, 8 ),  0 );

  iRc2 = pRel11->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1124)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1125)", pRel11->CheckRecNo( iPo, 4, 2, 4, 7 ),  0 );

  iRc2 = pRel11->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1126", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1127)", pRel11->CheckRecNo( iPo, 3, 0, 3, 6 ),  0 );

  iRc2 = pRel11->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1128)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1129)", pRel11->CheckRecNo( iPo, 3, 0, 3, 5 ),  0 );

  iRc2 = pRel11->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1130)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1131)", pRel11->CheckRecNo( iPo, 3, 0, 3, 4 ),  0 );

  iRc2 = pRel11->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1132)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1133)", pRel11->CheckRecNo( iPo, 2, 1, 2, 3 ),  0 );

  iRc2 = pRel11->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1134)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1135)", pRel11->CheckRecNo( iPo, 2, 1, 2, 2 ),  0 );

  iRc2 = pRel11->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1136)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1137)", pRel11->CheckRecNo( iPo, 1, 0, 1, 1 ),  0 );

  iRc2 = pRel11->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1138)", iRc2, XB_BOF);

  delete pRel11;
  pRel11 = NULL;

//***************************************************************************************
// Test twelve - no filter, master with three required children
//             - match
//             - missing
//             - dups


  xbRelate * pRel12;
  pRel12 = new xbRelate( &x );


  iRc2 = pRel12->SetMaster( dRel0 );
  iRc += TestMethod( iPo, "SetMaster(1200)", iRc2, 0 );

  iRc2 = pRel12->AddRelation( dRel0, "FORGN_KS1", dRel1Mfml, dRel1Mfml->GetCurIx(), dRel1Mfml->GetCurTag(), 'R' );
  iRc += TestMethod( iPo, "AddRelation(1201)", iRc2, XB_NO_ERROR );

  iRc2 = pRel12->AddRelation( dRel0, "FORGN_KS2", dRel1Mtch, dRel1Mtch->GetCurIx(), dRel1Mtch->GetCurTag(), 'R' );
  iRc += TestMethod( iPo, "AddRelation(1202)", iRc2, XB_NO_ERROR );

  iRc2 = pRel12->AddRelation( dRel0, "FORGN_KS3", dRel1DK, dRel1DK->GetCurIx(), dRel1DK->GetCurTag(), 'R' );
  iRc += TestMethod( iPo, "AddRelation(1203)", iRc2, XB_NO_ERROR );

  iRc2 = pRel12->GetFirstRelRecord();
  iRc += TestMethod( iPo, "GetFirstRelRecord(1204)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1205)", pRel12->CheckRecNo( iPo, 2, 1, 2, 2 ),  0 );

  iRc2 = pRel12->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1206)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1207)", pRel12->CheckRecNo( iPo, 2, 1, 2, 3 ),  0 );

  iRc2 = pRel12->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1208)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1209)", pRel12->CheckRecNo( iPo, 4, 2, 4, 7 ),  0 );

  iRc2 = pRel12->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1210)", iRc2, XB_EOF );

  iRc2 = pRel12->GetFirstRelRecord();
  iRc += TestMethod( iPo, "GetFirstRelRecord(1211)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1212)", pRel12->CheckRecNo( iPo, 2, 1, 2, 2 ),  0 );

  iRc2 = pRel12->GetLastRelRecord();
  iRc += TestMethod( iPo, "GetLastRelRecord(1213)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1214)", pRel12->CheckRecNo( iPo, 4, 2, 4, 7 ),  0 );

  iRc2 = pRel12->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1215)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1216)", pRel12->CheckRecNo( iPo, 2, 1, 2, 3 ),  0 );

  iRc2 = pRel12->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1217)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1218)", pRel12->CheckRecNo( iPo, 2, 1, 2, 2 ),  0 );

  iRc2 = pRel12->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1219)", iRc2, XB_BOF );

  delete pRel12;
  pRel12 = NULL;


//***************************************************************************************
// Test thirteen - no filter, master with one optional and one required  children
//             - match
//             - missing

  xbRelate * pRel13;
  pRel13 = new xbRelate( &x );

  iRc2 = pRel13->SetMaster( dRel0 );
  iRc += TestMethod( iPo, "SetMaster(1300)", iRc2, 0 );

  iRc2 = pRel13->AddRelation( dRel0, "FORGN_KS2", dRel1Mtch, dRel1Mtch->GetCurIx(), dRel1Mtch->GetCurTag(), 'O' );
  iRc += TestMethod( iPo, "AddRelation(1301)", iRc2, XB_NO_ERROR );

  iRc2 = pRel13->AddRelation( dRel0, "FORGN_KS1", dRel1Mfml, dRel1Mfml->GetCurIx(), dRel1Mfml->GetCurTag(), 'R' );
  iRc += TestMethod( iPo, "AddRelation(1302)", iRc2, XB_NO_ERROR );

  iRc2 = pRel13->GetFirstRelRecord();
  iRc += TestMethod( iPo, "GetFirstRelRecord(1303)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1304)", pRel13->CheckRecNo( iPo, 2, 2, 1 ),  0 );

  iRc2 = pRel13->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1305)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1306)", pRel13->CheckRecNo( iPo, 4, 4, 2 ),  0 );

  iRc2 = pRel13->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1307)", iRc2, XB_EOF );
  iRc += TestMethod( iPo, "CheckRecNo(1308)", pRel13->CheckRecNo( iPo, 0, 0, 0 ),  0 );

  iRc2 = pRel13->GetLastRelRecord();
  iRc += TestMethod( iPo, "GetLastRelRecord(1309)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1310)", pRel13->CheckRecNo( iPo, 4, 4, 2 ),  0 );

  iRc2 = pRel13->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1311)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1312)", pRel13->CheckRecNo( iPo, 2, 2, 1 ),  0 );

  iRc2 = pRel13->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1313)", iRc2, XB_BOF );
  iRc += TestMethod( iPo, "CheckRecNo(1314)", pRel13->CheckRecNo( iPo, 0, 0, 0 ),  0 );

  delete pRel13;
  pRel13 = NULL;

//***************************************************************************************
// Test fourteen - no filter, one opt slave relation, matches 1:1 on numeric key - loop through the relation


  xbRelate * pRel14;
  pRel14 = new xbRelate( &x );



  iRc2 = pRel14->SetMaster( dRel0 );
  iRc += TestMethod( iPo, "SetMaster(1401)", iRc2, XB_NO_ERROR );

  iRc2 = dRel1Mtch->SetCurTag( "NUM_KS2" );
  iRc += TestMethod( iPo, "SetCurTag(1402)", iRc2, XB_NO_ERROR );

  // std::cout << "RL1MATCH.DBF Current Tag = [" << dRel1Mtch->GetCurTagName().Str() << "]\n";

  iRc2 = pRel14->AddRelation( dRel0, "FORGN_KS2N", dRel1Mtch, dRel1Mtch->GetCurIx(), dRel1Mtch->GetCurTag(), 'O' );
  iRc += TestMethod( iPo, "AddRelation(1403)", iRc2, XB_NO_ERROR );

  iRc2 = pRel14->GetFirstRelRecord();
  iRc += TestMethod( iPo, "GetFirstRelRecord(1404)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1405)", pRel14->CheckRecNo( iPo, 1, 1 ),  0 );

  iRc2 = pRel14->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1406)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1407)", pRel14->CheckRecNo( iPo, 2, 2 ),  0 );

  iRc2 = pRel14->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1408)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1409)", pRel14->CheckRecNo( iPo, 3, 3 ),  0 );

  iRc2 = pRel14->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1410)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1411)", pRel14->CheckRecNo( iPo, 4, 4 ),  0 );

  iRc2 = pRel14->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1412)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1413)", pRel14->CheckRecNo( iPo, 5, 5 ),  0 );

  iRc2 = pRel14->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1414)", iRc2, XB_EOF );
  iRc += TestMethod( iPo, "CheckRecNo(1415)", pRel14->CheckRecNo( iPo, 0, 0 ),  0 );

  iRc2 = pRel14->GetFirstRelRecord();
  iRc += TestMethod( iPo, "GetFirstRelRecord(1416)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1417)", pRel14->CheckRecNo( iPo, 1, 1 ),  0 );

  iRc2 = pRel14->GetLastRelRecord();
  iRc += TestMethod( iPo, "GetLastRelRecord(1418)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1418)", pRel14->CheckRecNo( iPo, 5, 5 ),  0 );

  iRc2 = pRel14->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1419)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1420)", pRel14->CheckRecNo( iPo, 4, 4 ),  0 );

  iRc2 = pRel14->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1421)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1422)", pRel14->CheckRecNo( iPo, 3, 3 ),  0 );

  iRc2 = pRel14->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1423)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1424)", pRel14->CheckRecNo( iPo, 2, 2 ),  0 );

  iRc2 = pRel14->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1425)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1426)", pRel14->CheckRecNo( iPo, 1, 1 ),  0 );

  iRc2 = pRel14->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1427)", iRc2, XB_BOF );

  delete pRel14;
  pRel14 = NULL;


//***************************************************************************************
// Test fifteen - no filter, one opt slave relation, matches 1:1 on date key - loop through the relation

  xbRelate * pRel15;
  pRel15 = new xbRelate( &x );



  iRc2 = pRel15->SetMaster( dRel0 );
  iRc += TestMethod( iPo, "SetMaster(1501)", iRc2, XB_NO_ERROR );

  iRc2 = dRel1Mtch->SetCurTag( "DATE_KS2" );
  iRc += TestMethod( iPo, "SetCurTag(1502)", iRc2, XB_NO_ERROR );

  // std::cout << "RL1MATCH.DBF Current Tag = [" << dRel1Mtch->GetCurTagName().Str() << "]\n";

  iRc2 = pRel15->AddRelation( dRel0, "FORGN_KS2D", dRel1Mtch, dRel1Mtch->GetCurIx(), dRel1Mtch->GetCurTag(), 'O' );
  iRc += TestMethod( iPo, "AddRelation(1503)", iRc2, XB_NO_ERROR );

  iRc2 = pRel15->GetFirstRelRecord();
  iRc += TestMethod( iPo, "GetFirstRelRecord(1504)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1505)", pRel15->CheckRecNo( iPo, 1, 1 ),  0 );

  iRc2 = pRel15->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1506)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1507)", pRel15->CheckRecNo( iPo, 2, 2 ),  0 );

  iRc2 = pRel15->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1508)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1509)", pRel15->CheckRecNo( iPo, 3, 3 ),  0 );

  iRc2 = pRel15->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1510)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1511)", pRel15->CheckRecNo( iPo, 4, 4 ),  0 );

  iRc2 = pRel15->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1512)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1513)", pRel15->CheckRecNo( iPo, 5, 5 ),  0 );

  iRc2 = pRel15->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1514)", iRc2, XB_EOF );
  iRc += TestMethod( iPo, "CheckRecNo(1515)", pRel15->CheckRecNo( iPo, 0, 0 ),  0 );

  iRc2 = pRel15->GetFirstRelRecord();
  iRc += TestMethod( iPo, "GetFirstRelRecord(1516)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1517)", pRel15->CheckRecNo( iPo, 1, 1 ),  0 );

  iRc2 = pRel15->GetLastRelRecord();
  iRc += TestMethod( iPo, "GetLastRelRecord(1518)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1518)", pRel15->CheckRecNo( iPo, 5, 5 ),  0 );

  iRc2 = pRel15->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1519)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1520)", pRel15->CheckRecNo( iPo, 4, 4 ),  0 );

  iRc2 = pRel15->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1521)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1522)", pRel15->CheckRecNo( iPo, 3, 3 ),  0 );

  iRc2 = pRel15->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1523)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1524)", pRel15->CheckRecNo( iPo, 2, 2 ),  0 );

  iRc2 = pRel15->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1525)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1526)", pRel15->CheckRecNo( iPo, 1, 1 ),  0 );

  iRc2 = pRel15->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1527)", iRc2, XB_BOF );

  delete pRel15;
  pRel15 = NULL;

//***************************************************************************************
// Test sixteen - test master filter, no slave relation

  xbRelate * pRel16;
  pRel16 = new xbRelate( &x );


  iRc2 = pRel16->SetMaster( dRel0, "FORGN_KS2N > 202" );
  iRc += TestMethod( iPo, "SetMaster(1600)", iRc2, 0 );

  iRc2 = pRel16->GetFirstRelRecord();
  iRc += TestMethod( iPo, "GetFirstRelRecord(1601)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1602)", pRel16->CheckRecNo( iPo, 3 ),  0 );

  iRc2 = pRel16->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1603)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1604)", pRel16->CheckRecNo( iPo, 4 ),  0 );

  iRc2 = pRel16->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1605)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1606)", pRel16->CheckRecNo( iPo, 5 ),  0 );

  iRc2 = pRel16->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1607)", iRc2, XB_EOF );
  iRc += TestMethod( iPo, "CheckRecNo(1608)", pRel16->CheckRecNo( iPo, 0 ),  0 );

  iRc2 = pRel16->GetLastRelRecord();
  iRc += TestMethod( iPo, "GetLastRelRecord(1609)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1610)", pRel16->CheckRecNo( iPo, 5 ),  0 );

  iRc2 = pRel16->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1611)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1612)", pRel16->CheckRecNo( iPo, 4 ),  0 );

  iRc2 = pRel16->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1613)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1614)", pRel16->CheckRecNo( iPo, 3 ),  0 );

  iRc2 = pRel16->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1615)", iRc2, XB_BOF );
  iRc += TestMethod( iPo, "CheckRecNo(1616)", pRel16->CheckRecNo( iPo, 0 ),  0 );

  delete pRel16;
  pRel16 = NULL;



//***************************************************************************************
// Test seventeen - test one optional slave relation with a filter

  xbRelate * pRel17;
  pRel17 = new xbRelate( &x );

  iRc2 = pRel17->SetMaster( dRel0 );
  iRc += TestMethod( iPo, "SetMaster(1700)", iRc2, 0 );

  iRc2 = dRel1Mtch->SetCurTag( "PRIM_KS2" );
  iRc += TestMethod( iPo, "SetCurTag(1701)", iRc2, XB_NO_ERROR );
  //  std::cout << "RL1MATCH.DBF Current Tag = [" << dRel1Mtch->GetCurTagName().Str() << "]\n";

  iRc2 = pRel17->AddRelation( dRel0, "FORGN_KS2", dRel1Mtch, dRel1Mtch->GetCurIx(), dRel1Mtch->GetCurTag(), 'O', "NUM_KS2 = 202 OR NUM_KS2 = 204" );
  iRc += TestMethod( iPo, "AddRelation(1702)", iRc2, XB_NO_ERROR );

  iRc2 = pRel17->GetFirstRelRecord();
  iRc += TestMethod( iPo, "GetFirstRelRecord(1703)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1704)", pRel17->CheckRecNo( iPo, 1, 0 ),  0 );

  iRc2 = pRel17->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1705)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1706)", pRel17->CheckRecNo( iPo, 2, 2 ),  0 );

  iRc2 = pRel17->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1707)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1708)", pRel17->CheckRecNo( iPo, 3, 0 ),  0 );

  iRc2 = pRel17->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1709)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1710)", pRel17->CheckRecNo( iPo, 4, 4 ),  0 );

  iRc2 = pRel17->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1711)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1712)", pRel17->CheckRecNo( iPo, 5, 0 ),  0 );

  iRc2 = pRel17->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1713)", iRc2, XB_EOF );

  iRc2 = pRel17->GetLastRelRecord();
  iRc += TestMethod( iPo, "GetLastRelRecord(1714)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1715)", pRel17->CheckRecNo( iPo, 5, 0 ),  0 );

  iRc2 = pRel17->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1716)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1717)", pRel17->CheckRecNo( iPo, 4, 4 ),  0 );

  iRc2 = pRel17->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1718)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1719)", pRel17->CheckRecNo( iPo, 3, 0 ),  0 );

  iRc2 = pRel17->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1720)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1721)", pRel17->CheckRecNo( iPo, 2, 2 ),  0 );

  iRc2 = pRel17->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1722)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1723)", pRel17->CheckRecNo( iPo, 1, 0 ),  0 );

  iRc2 = pRel17->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1724)", iRc2, XB_BOF );
  iRc += TestMethod( iPo, "CheckRecNo(1719)", pRel17->CheckRecNo( iPo, 0, 0 ),  0 );

  delete pRel17;
  pRel17 = NULL;


//***************************************************************************************
// Test eighteen - test master filter has a filter, one slave relation with a filter

  xbRelate * pRel18;
  pRel18 = new xbRelate( &x );

  iRc2 = pRel18->SetMaster( dRel0, "FORGN_KS2N > 202" );
  iRc += TestMethod( iPo, "SetMaster(1800)", iRc2, 0 );

  iRc += TestMethod( iPo, "SetCurTag(1801)", iRc2, XB_NO_ERROR );
  // std::cout << "RL1MATCH.DBF Current Tag = [" << dRel1Mtch->GetCurTagName().Str() << "]\n";

//  iRc2 = pRel18->AddRelation( dRel0, "FORGN_KS2", dRel1Mtch, dRel1Mtch->GetCurIx(), dRel1Mtch->GetCurTag(), 'O', "NUM_KS2 = 202 OR NUM_KS2 = 204" );
  
  iRc2 = pRel18->AddRelation( dRel0, "FORGN_KS2", dRel1Mtch, "PRIM_KS2", 'O', "NUM_KS2 = 202 OR NUM_KS2 = 204" );


  iRc += TestMethod( iPo, "AddRelation(1802)", iRc2, XB_NO_ERROR );

  iRc2 = pRel18->GetFirstRelRecord();
  iRc += TestMethod( iPo, "GetFirstRelRecord(1803)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1804)", pRel18->CheckRecNo( iPo, 3, 0 ),  0 );

  iRc2 = pRel18->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1805)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1806)", pRel18->CheckRecNo( iPo, 4, 4 ),  0 );

  iRc2 = pRel18->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1807)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1808)", pRel18->CheckRecNo( iPo, 5, 0 ),  0 );

  iRc2 = pRel18->GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(1809)", iRc2, XB_EOF );

  iRc2 = pRel18->GetLastRelRecord();
  iRc += TestMethod( iPo, "GetLastRelRecord(1810)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1811)", pRel18->CheckRecNo( iPo, 5, 0 ),  0 );

  iRc2 = pRel18->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1812)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1813)", pRel18->CheckRecNo( iPo, 4, 4 ),  0 );

  iRc2 = pRel18->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1814)", iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "CheckRecNo(1815)", pRel18->CheckRecNo( iPo, 3, 0 ),  0 );

  iRc2 = pRel18->GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(1816)", iRc2, XB_BOF );
  iRc += TestMethod( iPo, "CheckRecNo(1817)", pRel18->CheckRecNo( iPo, 0, 0 ),  0 );

  delete pRel18;
  pRel18 = NULL;



// misc stuff that might be useful
//  DumpRecs( &x, 1, 1, "Test 17, get first record", dRel0, dRel1Mtch );
//  pRel16->DumpRelationTree();
//  pRel16->DumpRelationList();
//  DumpRecs( &x, 1, 1, "Test 1, get last record", dRel0, dRel1Mtch );
//  iRc2 = dRel1Mtch->SetCurTag( "PRIM_KS2" );
//  iRc += TestMethod( iPo, "SetCurTag(1601)", iRc2, XB_NO_ERROR );
//  std::cout << "RL1MATCH.DBF Current Tag = [" << dRel1Mtch->GetCurTagName().Str() << "]\n";
//  DumpRecs( &x, 1, 1, "Test 1, get last record", dRel0, dRel1Mfml, dRel1Mtch, dRel1DK );
//  #ifdef XB_DEBUG_SUPPORT
//  pRel4->DumpRelationTree();
//  pRel4->DumpRelationList();
//  #endif  // XB_DEBUG_SUPPORT
//  dRel0->DumpRecord( dRel0->GetCurRecNo() );
//  dRel1Mtch->DumpRecord( dRel1Mtch->GetCurRecNo() );
//  pRel4->DumpRelationTree();
//  pRel4->DumpRelationList();


  x.CloseAllTables();

  delete dRel0;
  delete dRel1Mfml;
  delete dRel1Mtch;
  delete dRel1DK;
  delete dRelEmpty;
  delete dRel2Mtch;

#ifdef XB_LOGGING_SUPPORT
  sMsg.Sprintf( "Program [%s] terminating with [%d] errors...", av[0], iRc * -1 );
  x.WriteLogMessage( sMsg );
  std::cout << sMsg.Str() << std::endl;
#endif

  return iRc;
}
