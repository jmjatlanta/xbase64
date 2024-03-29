/* xb_ex_relation.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

// This program demonstrates how to use the xbRelation class
// usage:   xb_ex_relation



#include "xbase.h"

using namespace xb;


  xbSchema RlMaster[] =
  {
    { "PRIM_KS0",   XB_NUMERIC_FLD,  8, 0 },
    { "FORGN_KS1",  XB_CHAR_FLD,     3, 0 },	// foreign key 1
    { "FORGN_KS2",  XB_CHAR_FLD,     3, 0 },	// foreign key 2
    { "CHAR_DATA3", XB_CHAR_FLD,     3, 0 },
    { "",0,0,0 }
  };

  xbSchema RlSlave1[] =
  {
    { "PRIM_KS1",   XB_CHAR_FLD,     3, 0 },
    { "FORGN_KS3",  XB_CHAR_FLD,     3, 0 },	// foreign key 3
    { "CHAR_DATA4", XB_CHAR_FLD,     4, 0 },
    { "",0,0,0 }
  };

  xbSchema RlSlave2[] =
  {
    { "PRIM_KS2",   XB_CHAR_FLD,     3, 0 },
    { "CHAR_DATA5", XB_CHAR_FLD,     5, 0 },
    { "",0,0,0 }
  };

  xbSchema RlSlave3[] =
  {
    { "PRIM_KS3",   XB_CHAR_FLD,     3, 0 },
    { "CHAR_DATA6", XB_CHAR_FLD,     6, 0 },
    { "",0,0,0 }
  };


int main()
{
  int iRc = 0;

  xbXBase x;

  x.SetDataDirectory( PROJECT_DATA_DIR );
  x.SetMultiUser( xbFalse );

  xbDbf4 dMaster( &x );
  xbDbf4 dSlave1( &x );
  xbDbf4 dSlave2( &x );
  xbDbf4 dSlave3( &x );

  xbIx *pIx2;
  xbIx *pIx3;
  xbIx *pIx4;

  void *pTag2;
  void *pTag3;
  void *pTag4;

  xbRelate relate1( &x );


  // define tables and indices
  if(( iRc = dMaster.CreateTable( "MASTER.DBF", "Master", RlMaster, XB_OVERLAY, XB_SINGLE_USER )) != XB_NO_ERROR ){
    x.DisplayError( iRc );
  }
  if(( iRc = dSlave1.CreateTable( "SLAVE1.DBF", "Slave1", RlSlave1, XB_OVERLAY, XB_SINGLE_USER )) != XB_NO_ERROR ){
    x.DisplayError( iRc );
  }

  if(( iRc = dSlave1.CreateTag( "MDX", "PRIM_KS1",  "PRIM_KS1", "", 0, 0, XB_OVERLAY, &pIx2, &pTag2 )) != XB_NO_ERROR ){
    x.DisplayError( iRc );
  }

  if(( iRc = dSlave2.CreateTable( "SLAVE2.DBF", "Slave2", RlSlave2, XB_OVERLAY, XB_SINGLE_USER )) != XB_NO_ERROR ){
    x.DisplayError( iRc );
  }

  if(( iRc = dSlave2.CreateTag( "MDX", "PRIM_KS2",  "PRIM_KS2", "", 0, 0, XB_OVERLAY, &pIx3, &pTag3 )) != XB_NO_ERROR ){
    x.DisplayError( iRc );
  }

  if(( iRc = dSlave3.CreateTable( "SLAVE3.DBF", "Slave3", RlSlave3, XB_OVERLAY, XB_SINGLE_USER )) != XB_NO_ERROR ){
    x.DisplayError( iRc );
  }
  if(( iRc = dSlave3.CreateTag( "MDX", "PRIM_KS3",  "PRIM_KS3", "", 0, 0, XB_OVERLAY, &pIx4, &pTag4 )) != XB_NO_ERROR ){
    x.DisplayError( iRc );
  }

  // add some data to the tables
  dMaster.BlankRecord();
  dMaster.PutLongField( "PRIM_KS0", 1 );
  dMaster.PutField( "FORGN_KS1", "101" );
  dMaster.PutField( "FORGN_KS2", "201" );
  dMaster.PutField( "CHAR_DATA3", "AAA" );
  dMaster.AppendRecord();
  dMaster.Commit();

  dMaster.BlankRecord();
  dMaster.PutLongField( "PRIM_KS0", 2 );
  dMaster.PutField( "FORGN_KS1", "102" );
  dMaster.PutField( "FORGN_KS2", "202" );
  dMaster.PutField( "CHAR_DATA3", "BBB" );
  dMaster.AppendRecord();
  dMaster.Commit();

  dMaster.BlankRecord();
  dMaster.PutLongField( "PRIM_KS0", 3 );
  dMaster.PutField( "FORGN_KS1", "103" );
  dMaster.PutField( "FORGN_KS2", "203" );
  dMaster.PutField( "CHAR_DATA3", "CCC" );
  dMaster.AppendRecord();
  dMaster.Commit();

  dMaster.BlankRecord();
  dMaster.PutLongField( "PRIM_KS0", 4 );
  dMaster.PutField( "FORGN_KS1", "104" );
  dMaster.PutField( "FORGN_KS2", "204" );
  dMaster.PutField( "CHAR_DATA3", "DDD" );
  dMaster.AppendRecord();
  dMaster.Commit();

  dMaster.BlankRecord();
  dMaster.PutLongField( "PRIM_KS0", 5 );
  dMaster.PutField( "FORGN_KS1", "105" );
  dMaster.PutField( "FORGN_KS2", "205" );
  dMaster.PutField( "CHAR_DATA3", "EEE" );
  dMaster.AppendRecord();
  dMaster.Commit();

  dSlave1.BlankRecord();
  dSlave1.PutField( "PRIM_KS1",   "101" );
  dSlave1.PutField( "FORGN_KS3",  "301" );
  dSlave1.PutField( "CHAR_DATA4", "AAAA" );
  dSlave1.AppendRecord();
  dSlave1.Commit();

  dSlave1.BlankRecord();
  dSlave1.PutField( "PRIM_KS1",   "102" );
  dSlave1.PutField( "FORGN_KS3",  "302" );
  dSlave1.PutField( "CHAR_DATA4", "BBBB" );
  dSlave1.AppendRecord();
  dSlave1.Commit();

  dSlave1.BlankRecord();
  dSlave1.PutField( "PRIM_KS1",   "103" );
  dSlave1.PutField( "FORGN_KS3",  "303" );
  dSlave1.PutField( "CHAR_DATA4", "CCCC" );
  dSlave1.AppendRecord();
  dSlave1.Commit();

  dSlave1.BlankRecord();
  dSlave1.PutField( "PRIM_KS1",   "104" );
  dSlave1.PutField( "FORGN_KS3",  "304" );
  dSlave1.PutField( "CHAR_DATA4", "DDDD" );
  dSlave1.AppendRecord();
  dSlave1.Commit();

  dSlave1.BlankRecord();
  dSlave1.PutField( "PRIM_KS1",   "105" );
  dSlave1.PutField( "FORGN_KS3",  "305" );
  dSlave1.PutField( "CHAR_DATA4", "EEEE" );
  dSlave1.AppendRecord();
  dSlave1.Commit();

  dSlave2.BlankRecord();
  dSlave2.PutField( "PRIM_KS2",   "201" );
  dSlave2.PutField( "CHAR_DATA5", "AAAAA" );
  dSlave2.AppendRecord();
  dSlave2.Commit();

  dSlave2.BlankRecord();
  dSlave2.PutField( "PRIM_KS2",   "202" );
  dSlave2.PutField( "CHAR_DATA5", "BBBBB" );
  dSlave2.AppendRecord();
  dSlave2.Commit();

  dSlave2.BlankRecord();
  dSlave2.PutField( "PRIM_KS2",   "203" );
  dSlave2.PutField( "CHAR_DATA5", "CCCCC" );
  dSlave2.AppendRecord();
  dSlave2.Commit();

  dSlave2.BlankRecord();
  dSlave2.PutField( "PRIM_KS2",   "204" );
  dSlave2.PutField( "CHAR_DATA5", "DDDDD" );
  dSlave2.AppendRecord();
  dSlave2.Commit();

  dSlave2.BlankRecord();
  dSlave2.PutField( "PRIM_KS2",   "205" );
  dSlave2.PutField( "CHAR_DATA5", "EEEEE" );
  dSlave2.AppendRecord();
  dSlave2.Commit();

  dSlave3.BlankRecord();
  dSlave3.PutField( "PRIM_KS3",   "301" );
  dSlave3.PutField( "CHAR_DATA6", "AAAAAA" );
  dSlave3.AppendRecord();
  dSlave3.Commit();

  dSlave3.BlankRecord();
  dSlave3.PutField( "PRIM_KS3",   "302" );
  dSlave3.PutField( "CHAR_DATA6", "BBBBBB" );
  dSlave3.AppendRecord();
  dSlave3.Commit();

  dSlave3.BlankRecord();
  dSlave3.PutField( "PRIM_KS3",   "304" );
  dSlave3.PutField( "CHAR_DATA6", "DDDDDD" );
  dSlave3.AppendRecord();
  dSlave3.Commit();

  dSlave3.BlankRecord();
  dSlave3.PutField( "PRIM_KS3",   "305" );
  dSlave3.PutField( "CHAR_DATA6", "EEEEEE" );
  dSlave3.AppendRecord();
  dSlave3.Commit();


  if(( iRc = relate1.SetMaster( &dMaster )) != XB_NO_ERROR ){
    x.DisplayError( iRc );
  }

  // example using tag name for relation
  if(( iRc = relate1.AddRelation( &dMaster, "FORGN_KS1", &dSlave1, "PRIM_KS1", 'O' )) != XB_NO_ERROR ){
    x.DisplayError( iRc );
  }

  // example using tag pointers for relation
  if(( iRc = relate1.AddRelation( &dMaster, "FORGN_KS2", &dSlave2, pIx3, pTag3, 'R' )) != XB_NO_ERROR ){
    x.DisplayError( iRc );
  }

  if(( iRc = relate1.AddRelation( &dSlave1, "FORGN_KS3", &dSlave3, "PRIM_KS3", 'R' )) != XB_NO_ERROR ){
    x.DisplayError( iRc );
  }


  #ifdef XB_DEBUG_SUPPORT	// look at the internals
  relate1.GetFirstRelRecord();
  relate1.DumpRelationTree();
  relate1.DumpRelationList();
  #endif // XB_DEBUG_SUPPORT


  // loop forward
  iRc = relate1.GetFirstRelRecord();
  while( iRc == XB_NO_ERROR ){
    std::cout << "GetNext example - do something with m rec# = " << dMaster.GetCurRecNo() 
         << " s1 rec# = " << dSlave1.GetCurRecNo() 
         << " s2 rec# = " << dSlave2.GetCurRecNo() 
         << " s3 rec# = " << dSlave3.GetCurRecNo() << "\n";
    iRc = relate1.GetNextRelRecord();
  }

  std::cout << std::endl;  // insert a blank line
  // loop backwards
  iRc = relate1.GetLastRelRecord();
  while( iRc == XB_NO_ERROR ){
    std::cout << "GetPrev example - do something with m rec# = " << dMaster.GetCurRecNo()  
         << " s1 rec# = " << dSlave1.GetCurRecNo() 
         << " s2 rec# = " << dSlave2.GetCurRecNo() 
         << " s3 rec# = " << dSlave3.GetCurRecNo() << "\n";
    iRc = relate1.GetPrevRelRecord();
  }


  return iRc;
}
