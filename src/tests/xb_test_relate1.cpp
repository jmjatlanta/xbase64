/* xb_test_relate1.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

// This program tests builds the database files that the subsequent relateion test programs use
// usage:   xb_test_relate1 QUITE|NORMAL|VERBOSE


// Foreign Key structure
//  table, record, foreign key



#include "xbase.h"

using namespace xb;

#include "tstfuncs.cpp"


  xbSchema RL0Schema[] =
  {
    { "PRIM_KS0",   XB_NUMERIC_FLD,   8, 0 },
    { "FORGN_KS1",  XB_CHAR_FLD,      3, 0 },
    { "FORGN_KS2",  XB_CHAR_FLD,      3, 0 },
    { "FORGN_KS2N", XB_NUMERIC_FLD,   3, 0 },
    { "FORGN_KS2D", XB_DATE_FLD,      8, 0 },
    { "FORGN_KS3",  XB_CHAR_FLD,      3, 0 },
    { "FORGN_KS4",  XB_CHAR_FLD,      3, 0 },
    { "",0,0,0 }
  };

  xbSchema RL1MfmlSchema[] =
  {
    { "PRIM_KS1",   XB_CHAR_FLD,     3, 0 },
    { "",0,0,0 }
  };

  xbSchema RL1MtchSchema[] =
  {
    { "PRIM_KS2",   XB_CHAR_FLD,     3, 0 },
    { "NUM_KS2",    XB_NUMERIC_FLD,  3, 0 },
    { "DATE_KS2",   XB_DATE_FLD,     3, 0 },
    { "FORGN_KS5",  XB_CHAR_FLD,     3, 0 },
    { "FORGN_KS6",  XB_CHAR_FLD,     3, 0 },
    { "",0,0,0 }
  };

  xbSchema RL1DupKeysSchema[] =
  {
    { "PRIM_KS3",   XB_CHAR_FLD,     3, 0 },    // not really a prime key in the truest since as it has dups
    { "",0,0,0 }
  };


  xbSchema RLxEmptySchema[] =
  {
    { "PRIM_KS4",   XB_CHAR_FLD,     3, 0 },    // not really a prime key in the truest since as it has dups
    { "",0,0,0 }
  };

  xbSchema RL2MtchSchema[] =
  {
    { "PRIM_KS5",   XB_CHAR_FLD,     3, 0 },
    { "FORGN_KS7",  XB_CHAR_FLD,     3, 0 },
    { "FORGN_KS8",  XB_CHAR_FLD,     3, 0 },
    { "",0,0,0 }
  };


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

  xbDbf4 dRel0( &x );
  xbDbf4 dRel1Mfml( &x );
  xbDbf4 dRel1Mtch( &x );
  xbDbf4 dRel1DK( &x );
  xbDbf4 dRelEmpty( &x );
  xbDbf4 dRel2Mtch( &x );

  xbIx *pIx1;
  xbIx *pIx2;
  xbIx *pIx3;
  xbIx *pIx3a;
  xbIx *pIx3b;
  xbIx *pIx4;
  xbIx *pIx5;
  xbIx *pIx6;

  void *pTag1;
  void *pTag2;
  void *pTag3;
  void *pTag3a;
  void *pTag3b;
  void *pTag4;
  void *pTag5;
  void *pTag6;

  xbRelate relate1( &x );

  #ifdef XB_LOGGING_SUPPORT
  x.SetLogDirectory( PROJECT_LOG_DIR );
  x.EnableMsgLogging();
  if( iPo ){
    std::cout << "Logfile is [" << x.GetLogFqFileName().Str() << "]" << std::endl;
  }
  xbString sMsg;
  sMsg.Sprintf( "Program [%s] initializing...", av[0] );
  x.WriteLogMessage( sMsg );
  #endif


  iRc2 = dRel0.CreateTable( "RL0.DBF", "RL0", RL0Schema, XB_OVERLAY, XB_SINGLE_USER );
  iRc += TestMethod( iPo, "CreateTable(1)", iRc2, 0 );

/*
   CreateTag( const xbString &sIxType, const xbString &sName, const xbString &sKey, const xbString &sFilter, 
           xbInt16 iDescending, xbInt16 iUnique, xbInt16 iOverLay, xbIx **xbIxOut, void **vpTagOut );
*/

  iRc2 = dRel0.CreateTag( "MDX", "PRIM_KS0",  "PRIM_KS0", "", 0, 0, XB_OVERLAY, &pIx1, &pTag1 );
  iRc += TestMethod( iPo, "CreateTag(1)", iRc2, 0 );

  iRc2 = dRel1Mfml.CreateTable( "RL1MFML.DBF", "TestRelMfml", RL1MfmlSchema, XB_OVERLAY, XB_SINGLE_USER );
  iRc += TestMethod( iPo, "CreateTable(6)", iRc2, 0 );
  iRc2 = dRel1Mfml.CreateTag( "MDX", "PRIM_KS1",  "PRIM_KS1", "", 0, 0, XB_OVERLAY, &pIx2, &pTag2 );
  iRc += TestMethod( iPo, "CreateTag(7)", iRc2, 0 );

  iRc2 = dRel1Mtch.CreateTable( "RL1MTCH.DBF", "TestRel1Mtch", RL1MtchSchema, XB_OVERLAY, XB_SINGLE_USER );
  iRc += TestMethod( iPo, "CreateTable(8)", iRc2, 0 );
  iRc2 = dRel1Mtch.CreateTag( "MDX", "PRIM_KS2",  "PRIM_KS2", "", 0, 0, XB_OVERLAY, &pIx3, &pTag3 );
  iRc += TestMethod( iPo, "CreateTag(9)", iRc2, 0 );

  iRc2 = dRel1Mtch.CreateTag( "MDX", "NUM_KS2",  "NUM_KS2", "", 0, 0, XB_OVERLAY, &pIx3a, &pTag3a );
  iRc += TestMethod( iPo, "CreateTag(10)", iRc2, 0 );
  iRc2 = dRel1Mtch.CreateTag( "MDX", "DATE_KS2",  "DATE_KS2", "", 0, 0, XB_OVERLAY, &pIx3b, &pTag3b );
  iRc += TestMethod( iPo, "CreateTag(11)", iRc2, 0 );

  iRc2 = dRel1DK.CreateTable( "RL1DK.DBF", "TestRelDupKeys", RL1DupKeysSchema, XB_OVERLAY, XB_SINGLE_USER );
  iRc += TestMethod( iPo, "CreateTable(12)", iRc2, 0 );
  iRc2 = dRel1DK.CreateTag( "MDX", "PRIM_KS3",  "PRIM_KS3", "", 0, 0, XB_OVERLAY, &pIx4, &pTag4 );
  iRc += TestMethod( iPo, "CreateTag(13)", iRc2, 0 );

  iRc2 = dRelEmpty.CreateTable( "RLEMPTY.DBF", "TestRelEmpty", RLxEmptySchema, XB_OVERLAY, XB_SINGLE_USER );
  iRc += TestMethod( iPo, "CreateTable(14)", iRc2, 0 );
  iRc2 = dRelEmpty.CreateTag( "MDX", "PRIM_KS4",  "PRIM_KS4", "", 0, 0, XB_OVERLAY, &pIx5, &pTag5 );
  iRc += TestMethod( iPo, "CreateTag(15)", iRc2, 0 );

  iRc2 = dRel2Mtch.CreateTable( "RL2MTCH.DBF", "TestRel2Mtch", RL2MtchSchema, XB_OVERLAY, XB_SINGLE_USER );
  iRc += TestMethod( iPo, "CreateTable(16)", iRc2, 0 );
  iRc2 = dRel2Mtch.CreateTag( "MDX", "PRIM_KS5",  "PRIM_KS5", "", 0, 0, XB_OVERLAY, &pIx6, &pTag6 );
  iRc += TestMethod( iPo, "CreateTag(17)", iRc2, 0 );


  iRc2 = relate1.SetMaster( &dRel0 );
  iRc += TestMethod( iPo, "SetMaster(100)", iRc2, 0 );

  // iRc2 = relate1.InitQuery();
  // iRc += TestMethod( iPo, "InitQuery(101)", iRc2, 0 );

  iRc2 = relate1.GetFirstRelRecord();
  iRc += TestMethod( iPo, "GetFirstRelRecord(102)", iRc2, XB_EMPTY );

  iRc2 = relate1.GetNextRelRecord();
  iRc += TestMethod( iPo, "GetNextRelRecord(103)", iRc2, XB_EMPTY );

  iRc2 = relate1.GetPrevRelRecord();
  iRc += TestMethod( iPo, "GetPrevRelRecord(104)", iRc2, XB_EMPTY );

  iRc2 = relate1.GetLastRelRecord();
  iRc += TestMethod( iPo, "GetLastRelRecord(105)", iRc2, XB_EMPTY );

  iRc2 = dRel0.BlankRecord();
  iRc += TestMethod( iPo, "BlankRecord(200)",  iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(201)",     dRel0.PutLongField( "PRIM_KS0", 1 ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(202)",     dRel0.PutField( "FORGN_KS1", "101" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(203)",     dRel0.PutField( "FORGN_KS2", "201" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(204)",     dRel0.PutLongField( "FORGN_KS2N", 201 ),  XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(205)",     dRel0.PutField( "FORGN_KS2D", "19890201" ),  XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(205)",     dRel0.PutField( "FORGN_KS3", "301" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(206)",     dRel0.PutField( "FORGN_KS4", "401" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "AppendRecord(207)", dRel0.AppendRecord(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "Commit(208)",       dRel0.Commit(), XB_NO_ERROR );

  iRc2 = dRel0.BlankRecord();
  iRc += TestMethod( iPo, "BlankRecord(210)",  iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(211)",     dRel0.PutLongField( "PRIM_KS0", 2 ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(212)",     dRel0.PutField( "FORGN_KS1", "102" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(213)",     dRel0.PutField( "FORGN_KS2", "202" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(214)",     dRel0.PutLongField( "FORGN_KS2N", 202 ),  XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(215)",     dRel0.PutField( "FORGN_KS2D", "19890202" ),  XB_NO_ERROR ); 
  iRc += TestMethod( iPo, "PutField(216)",     dRel0.PutField( "FORGN_KS3", "302" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(217)",     dRel0.PutField( "FORGN_KS4", "402" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "AppendRecord(218)", dRel0.AppendRecord(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "Commit(219)",       dRel0.Commit(), XB_NO_ERROR );

  iRc2 = dRel0.BlankRecord();
  iRc += TestMethod( iPo, "BlankRecord(220)",  iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(221)",     dRel0.PutLongField( "PRIM_KS0", 3 ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(222)",     dRel0.PutField( "FORGN_KS1", "103" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(223)",     dRel0.PutField( "FORGN_KS2", "203" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(224)",     dRel0.PutLongField( "FORGN_KS2N", 203 ),  XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(225)",     dRel0.PutField( "FORGN_KS2D", "19890203" ),  XB_NO_ERROR ); 
  iRc += TestMethod( iPo, "PutField(226)",     dRel0.PutField( "FORGN_KS3", "303" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(227)",     dRel0.PutField( "FORGN_KS4", "403" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "AppendRecord(228)", dRel0.AppendRecord(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "Commit(229)",       dRel0.Commit(), XB_NO_ERROR );

  iRc2 = dRel0.BlankRecord();
  iRc += TestMethod( iPo, "BlankRecord(230)",  iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(231)",     dRel0.PutLongField( "PRIM_KS0", 4 ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(232)",     dRel0.PutField( "FORGN_KS1", "104" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(233)",     dRel0.PutField( "FORGN_KS2", "204" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(234)",     dRel0.PutLongField( "FORGN_KS2N", 204 ),  XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(245)",     dRel0.PutField( "FORGN_KS2D", "19890204" ),  XB_NO_ERROR ); 
  iRc += TestMethod( iPo, "PutField(236)",     dRel0.PutField( "FORGN_KS3", "304" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(237)",     dRel0.PutField( "FORGN_KS4", "404" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "AppendRecord(238)", dRel0.AppendRecord(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "Commit(239)",       dRel0.Commit(), XB_NO_ERROR );

  iRc2 = dRel0.BlankRecord();
  iRc += TestMethod( iPo, "BlankRecord(240)",  iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(241)",     dRel0.PutLongField( "PRIM_KS0", 5 ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(242)",     dRel0.PutField( "FORGN_KS1", "105" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(243)",     dRel0.PutField( "FORGN_KS2", "205" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(244)",     dRel0.PutLongField( "FORGN_KS2N", 205 ),  XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(245)",     dRel0.PutField( "FORGN_KS2D", "19890205" ),  XB_NO_ERROR ); 
  iRc += TestMethod( iPo, "PutField(246)",     dRel0.PutField( "FORGN_KS3", "305" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(247)",     dRel0.PutField( "FORGN_KS4", "405" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "AppendRecord(248)", dRel0.AppendRecord(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "Commit(249)",       dRel0.Commit(), XB_NO_ERROR );


  iRc2 = dRel1Mfml.BlankRecord();
  iRc += TestMethod( iPo, "BlankRecord(250)",  iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(251)",     dRel1Mfml.PutField( "PRIM_KS1",  "102" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "AppendRecord(252)", dRel1Mfml.AppendRecord(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "Commit(253)",       dRel1Mfml.Commit(), XB_NO_ERROR );

  iRc2 = dRel1Mfml.BlankRecord();
  iRc += TestMethod( iPo, "BlankRecord(260)",  iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(261)",     dRel1Mfml.PutField( "PRIM_KS1",  "104" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "AppendRecord(262)", dRel1Mfml.AppendRecord(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "Commit(263)",       dRel1Mfml.Commit(), XB_NO_ERROR );


  iRc2 = dRel1Mtch.BlankRecord();
  iRc += TestMethod( iPo, "BlankRecord(270)",  iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(271)",     dRel1Mtch.PutField( "PRIM_KS2", "201" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(272)",     dRel1Mtch.PutLongField( "NUM_KS2", 201 ),  XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(273)",     dRel1Mtch.PutField( "DATE_KS2", "19890201" ),  XB_NO_ERROR ); 
  iRc += TestMethod( iPo, "PutField(274)",     dRel1Mtch.PutField( "FORGN_KS5",  "501" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(275)",     dRel1Mtch.PutField( "FORGN_KS6",  "601" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "AppendRecord(276)", dRel1Mtch.AppendRecord(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "Commit(277)",       dRel1Mtch.Commit(), XB_NO_ERROR );

  iRc2 = dRel1Mtch.BlankRecord();
  iRc += TestMethod( iPo, "BlankRecord(280)",  iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(281)",     dRel1Mtch.PutField( "PRIM_KS2", "202" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(282)",     dRel1Mtch.PutLongField( "NUM_KS2", 202 ),  XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(283)",     dRel1Mtch.PutField( "DATE_KS2", "19890202" ),  XB_NO_ERROR ); 
  iRc += TestMethod( iPo, "PutField(284)",     dRel1Mtch.PutField( "FORGN_KS5",  "502" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(285)",     dRel1Mtch.PutField( "FORGN_KS6",  "602" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "AppendRecord(286)", dRel1Mtch.AppendRecord(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "Commit(287)",       dRel1Mtch.Commit(), XB_NO_ERROR );

  iRc2 = dRel1Mtch.BlankRecord();
  iRc += TestMethod( iPo, "BlankRecord(290)",  iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(291)",     dRel1Mtch.PutField( "PRIM_KS2", "203" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(292)",     dRel1Mtch.PutLongField( "NUM_KS2", 203 ),  XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(293)",     dRel1Mtch.PutField( "DATE_KS2", "19890203" ),  XB_NO_ERROR ); 
  iRc += TestMethod( iPo, "PutField(294)",     dRel1Mtch.PutField( "FORGN_KS5",  "503" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(295)",     dRel1Mtch.PutField( "FORGN_KS6",  "603" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "AppendRecord(296)", dRel1Mtch.AppendRecord(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "Commit(297)",       dRel1Mtch.Commit(), XB_NO_ERROR );

  iRc2 = dRel1Mtch.BlankRecord();
  iRc += TestMethod( iPo, "BlankRecord(300)",  iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(301)",     dRel1Mtch.PutField( "PRIM_KS2", "204" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(302)",     dRel1Mtch.PutLongField( "NUM_KS2", 204 ),  XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(303)",     dRel1Mtch.PutField( "DATE_KS2", "19890204" ),  XB_NO_ERROR ); 
  iRc += TestMethod( iPo, "PutField(304)",     dRel1Mtch.PutField( "FORGN_KS5",  "504" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(305)",     dRel1Mtch.PutField( "FORGN_KS6",  "604" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "AppendRecord(306)", dRel1Mtch.AppendRecord(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "Commit(307)",       dRel1Mtch.Commit(), XB_NO_ERROR );

  iRc2 = dRel1Mtch.BlankRecord();
  iRc += TestMethod( iPo, "BlankRecord(310)",  iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(311)",     dRel1Mtch.PutField( "PRIM_KS2", "205" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(312)",     dRel1Mtch.PutLongField( "NUM_KS2", 205 ),  XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(313)",     dRel1Mtch.PutField( "DATE_KS2", "19890205" ),  XB_NO_ERROR ); 
  iRc += TestMethod( iPo, "PutField(314)",     dRel1Mtch.PutField( "FORGN_KS5",  "505" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(315)",     dRel1Mtch.PutField( "FORGN_KS6",  "605" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "AppendRecord(316)", dRel1Mtch.AppendRecord(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "Commit(317)",       dRel1Mtch.Commit(), XB_NO_ERROR );


  iRc2 = dRel1DK.BlankRecord();
  iRc += TestMethod( iPo, "BlankRecord(320)",  iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(321)",     dRel1DK.PutField( "PRIM_KS3",  "301" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "AppendRecord(322)", dRel1DK.AppendRecord(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "Commit(323)",       dRel1DK.Commit(), XB_NO_ERROR );
  iRc2 = dRel1DK.BlankRecord();

  iRc += TestMethod( iPo, "BlankRecord(325)",  iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(326)",     dRel1DK.PutField( "PRIM_KS3",  "302" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "AppendRecord(327)", dRel1DK.AppendRecord(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "Commit(328)",       dRel1DK.Commit(), XB_NO_ERROR );
  iRc2 = dRel1DK.BlankRecord();

  iRc += TestMethod( iPo, "BlankRecord(330)",  iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(331)",     dRel1DK.PutField( "PRIM_KS3",  "302" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "AppendRecord(332)", dRel1DK.AppendRecord(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "Commit(333)",       dRel1DK.Commit(), XB_NO_ERROR );
  iRc2 = dRel1DK.BlankRecord();

  iRc += TestMethod( iPo, "BlankRecord(335)",  iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(336)",     dRel1DK.PutField( "PRIM_KS3",  "303" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "AppendRecord(337)", dRel1DK.AppendRecord(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "Commit(338)",       dRel1DK.Commit(), XB_NO_ERROR );

  iRc += TestMethod( iPo, "BlankRecord(340)",  iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(341)",     dRel1DK.PutField( "PRIM_KS3",  "303" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "AppendRecord(342)", dRel1DK.AppendRecord(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "Commit(343)",       dRel1DK.Commit(), XB_NO_ERROR );

  iRc += TestMethod( iPo, "BlankRecord(345)",  iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(346)",     dRel1DK.PutField( "PRIM_KS3",  "303" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "AppendRecord(347)", dRel1DK.AppendRecord(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "Commit(348)",       dRel1DK.Commit(), XB_NO_ERROR );

  iRc += TestMethod( iPo, "BlankRecord(350)",  iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(351)",     dRel1DK.PutField( "PRIM_KS3",  "304" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "AppendRecord(352)", dRel1DK.AppendRecord(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "Commit(353)",       dRel1DK.Commit(), XB_NO_ERROR );

  iRc += TestMethod( iPo, "BlankRecord(355)",  iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(356)",     dRel1DK.PutField( "PRIM_KS3",  "305" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "AppendRecord(357)", dRel1DK.AppendRecord(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "Commit(358)",       dRel1DK.Commit(), XB_NO_ERROR );

  iRc2 = dRel2Mtch.BlankRecord();
  iRc += TestMethod( iPo, "BlankRecord(360)",  iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(361)",     dRel2Mtch.PutField( "PRIM_KS5", "501" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(362)",     dRel2Mtch.PutField( "FORGN_KS7",  "701" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(363)",     dRel2Mtch.PutField( "FORGN_KS8",  "801" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "AppendRecord(364)", dRel2Mtch.AppendRecord(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "Commit(365)",       dRel2Mtch.Commit(), XB_NO_ERROR );

  iRc2 = dRel2Mtch.BlankRecord();
  iRc += TestMethod( iPo, "BlankRecord(370)",  iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(371)",     dRel2Mtch.PutField( "PRIM_KS5", "502" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(372)",     dRel2Mtch.PutField( "FORGN_KS7",  "702" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(373)",     dRel2Mtch.PutField( "FORGN_KS8",  "802" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "AppendRecord(374)", dRel2Mtch.AppendRecord(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "Commit(375)",       dRel2Mtch.Commit(), XB_NO_ERROR );

  iRc2 = dRel2Mtch.BlankRecord();
  iRc += TestMethod( iPo, "BlankRecord(380)",  iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(381)",     dRel2Mtch.PutField( "PRIM_KS5", "503" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(382)",     dRel2Mtch.PutField( "FORGN_KS7",  "703" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(383)",     dRel2Mtch.PutField( "FORGN_KS8",  "803" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "AppendRecord(384)", dRel2Mtch.AppendRecord(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "Commit(385)",       dRel2Mtch.Commit(), XB_NO_ERROR );

  iRc2 = dRel2Mtch.BlankRecord();
  iRc += TestMethod( iPo, "BlankRecord(390)",  iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(391)",     dRel2Mtch.PutField( "PRIM_KS5", "504" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(392)",     dRel2Mtch.PutField( "FORGN_KS7",  "704" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(393)",     dRel2Mtch.PutField( "FORGN_KS8",  "804" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "AppendRecord(394)", dRel2Mtch.AppendRecord(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "Commit(395)",       dRel2Mtch.Commit(), XB_NO_ERROR );

  iRc2 = dRel2Mtch.BlankRecord();
  iRc += TestMethod( iPo, "BlankRecord(400)",  iRc2, XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(401)",     dRel2Mtch.PutField( "PRIM_KS5", "505" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(402)",     dRel2Mtch.PutField( "FORGN_KS7",  "705" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "PutField(403)",     dRel2Mtch.PutField( "FORGN_KS8",  "805" ), XB_NO_ERROR );
  iRc += TestMethod( iPo, "AppendRecord(404)", dRel2Mtch.AppendRecord(), XB_NO_ERROR );
  iRc += TestMethod( iPo, "Commit(405)",       dRel2Mtch.Commit(), XB_NO_ERROR );



  dRel0.Close();

//  x.CloseAllTables();


#ifdef XB_LOGGING_SUPPORT
  sMsg.Sprintf( "Program [%s] terminating with [%d] errors...", av[0], iRc * -1 );
  x.WriteLogMessage( sMsg );
  std::cout << sMsg.Str() << std::endl;
#endif

  return iRc;
}
