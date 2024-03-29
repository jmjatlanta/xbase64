/* xb_test_linklist.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

// This program tests the class xblnklst and xblnknod
// usage:   xb_test_linklist QUITE|NORMAL|VERBOSE


#include "xbase.h"

using namespace xb;

#include "tstfuncs.cpp"



int main( int argCnt, char **av )
{
  int rc = 0;

  int po = 1;          /* print option */
                       /* 0 - QUIET    */
                       /* 1 - NORMAL   */
                       /* 2 - VERBOSE  */

  if( argCnt > 1 ) {
    if( av[1][0] == 'Q' )
      po = 0;
    else if( av[1][0] == 'V' )
      po = 2;
  }

  xbXBase x;
  #ifdef XB_LOGGING_SUPPORT
  x.SetLogDirectory( PROJECT_LOG_DIR );
  x.EnableMsgLogging();
  if( po ){
    std::cout << "Logfile is [" << x.GetLogFqFileName().Str() << "]" << std::endl;
  }
  xbString sMsg;
  sMsg.Sprintf( "Program [%s] initializing...", av[0] );
  x.WriteLogMessage( sMsg );
  #endif

  InitTime();

  xbLinkList<xbString> ll;
  ll.InsertAtFront( "A" );
  ll.InsertAtFront( "B" );
  ll.InsertAtFront( "C" );
  ll.InsertAtFront( "D" );
  ll.InsertAtFront( "E" );
  ll.InsertAtFront( "F" );
  ll.InsertAtFront( "G" );
  ll.InsertAtFront( "H" );
  ll.InsertAtFront( "I" );
  ll.InsertAtFront( "J" );
  ll.InsertAtFront( "K" );
  ll.InsertAtFront( "L" );
  ll.InsertAtFront( "M" );
  ll.InsertAtFront( "N" );
  ll.InsertAtFront( "O" );

  xbLinkListNode<xbString> * llN = ll.GetHeadNode();
  xbString s;
  xbString s2;
  xbUInt32 ulCnt = ll.GetNodeCnt();
  for( xbUInt32 i = 0; i < ulCnt; i++ ){
    s2 = llN->GetKey();
    s.Sprintf( "%s%s", s.Str(), s2.Str() );
    llN = llN->GetNextNode();
  }
  rc += TestMethod( po, "InsertAtFront()", s, "ONMLKJIHGFEDCBA", 15 );

  s = "";
  llN = ll.GetHeadNode();
  while( llN ){
    s2 = llN->GetKey();
    s.Sprintf( "%s%s", s.Str(), s2.Str() );
    llN = llN->GetNextNode();
  }
  rc += TestMethod( po, "GetNextNode()", s, "ONMLKJIHGFEDCBA", 15 );

  s = "";
  llN = ll.GetEndNode();
  while( llN ){
    s2 = llN->GetKey();
    s.Sprintf( "%s%s", s.Str(), s2.Str() );
    llN = llN->GetPrevNode();
  }
  rc += TestMethod( po, "GetNextNode()", s, "ABCDEFGHIJKLMNO", 15 );


  ll.RemoveFromFront( s );
  rc += TestMethod( po, "RemoveFromFront()", s, "O", 1 );

  llN = ll.GetHeadNode();
  ulCnt = ll.GetNodeCnt();
  rc += TestMethod( po, "RemoveFromFront()", (xbInt32) ulCnt, 14 );

  s = "";
  for( xbUInt32 i = 0; i < ulCnt; i++ ){
    s2 = llN->GetKey();
    s.Sprintf( "%s%s", s.Str(), s2.Str() );
    llN = llN->GetNextNode();
  }
  rc += TestMethod( po, "RemoveFromFront()", s, "NMLKJIHGFEDCBA", 14 );


  s = "";
  llN = ll.GetHeadNode();
  while( llN ){
    s2 = llN->GetKey();
    s.Sprintf( "%s%s", s.Str(), s2.Str() );
    llN = llN->GetNextNode();
  }
  rc += TestMethod( po, "RemoveFromFront()", s, "NMLKJIHGFEDCBA", 14 );


  s = "";
  llN = ll.GetEndNode();
  while( llN ){
    s2 = llN->GetKey();
    s.Sprintf( "%s%s", s.Str(), s2.Str() );
    llN = llN->GetPrevNode();
  }
  rc += TestMethod( po, "RemoveFromFront()", s, "ABCDEFGHIJKLMN", 14 );


  ll.RemoveFromEnd( s );
  rc = TestMethod( po, "RemoveFromEnd()", s, "A", 1 );
  ulCnt = ll.GetNodeCnt();
  rc += TestMethod( po, "RemoveFromFront()", (xbInt32) ulCnt, 13 );

  s = "";
  llN = ll.GetHeadNode();
  for( xbUInt32 i = 0; i < ulCnt; i++ ){
    s2 = llN->GetKey();
    s.Sprintf( "%s%s", s.Str(), s2.Str() );
    llN = llN->GetNextNode();
  }
  rc += TestMethod( po, "RemoveFromEnd()", s, "NMLKJIHGFEDCB", 13 );

  s = "";
  llN = ll.GetHeadNode();
  while( llN ){
    s2 = llN->GetKey();
    s.Sprintf( "%s%s", s.Str(), s2.Str() );
    llN = llN->GetNextNode();
  }
  rc += TestMethod( po, "RemoveFromEnd()", s, "NMLKJIHGFEDCB", 13 );

  s = "";
  llN = ll.GetEndNode();
  while( llN ){
    s2 = llN->GetKey();
    s.Sprintf( "%s%s", s.Str(), s2.Str() );
    llN = llN->GetPrevNode();
  }
  rc += TestMethod( po, "RemoveFromEnd()", s, "BCDEFGHIJKLMN", 13 );

  s = "J";
  ll.RemoveByVal( s );
  rc += TestMethod( po, "RemoveByVal()", s, "J", 1 );
  ulCnt = ll.GetNodeCnt();
  rc += TestMethod( po, "RemoveByVal()", (xbInt32) ulCnt, 12 );

  llN = ll.GetHeadNode();
  s = "";
  for( xbUInt32 i = 0; i < ulCnt; i++ ){
    s2 = llN->GetKey();
    s.Sprintf( "%s%s", s.Str(), s2.Str() );
    llN = llN->GetNextNode();
  }
  rc += TestMethod( po, "RemoveByVal()", s, "NMLKIHGFEDCB", 12 );

  s = "";
  llN = ll.GetHeadNode();
  while( llN ){
    s2 = llN->GetKey();
    s.Sprintf( "%s%s", s.Str(), s2.Str() );
    llN = llN->GetNextNode();
  }
  rc += TestMethod( po, "RemoveByVal()", s, "NMLKIHGFEDCB", 12 );

  s = "";
  llN = ll.GetEndNode();
  while( llN ){
    s2 = llN->GetKey();
    s.Sprintf( "%s%s", s.Str(), s2.Str() );
    llN = llN->GetPrevNode();
  }
  rc += TestMethod( po, "RemoveByVal()", s, "BCDEFGHIKLMN", 12 );


// ordered link list 
  xbLinkListOrd<xbString> llO;
  llO.SetDupKeys( 0 );
  rc += TestMethod( po, "InsertKey( J )", llO.InsertKey( "J" ), 0);
  rc += TestMethod( po, "InsertKey( J )", llO.InsertKey( "J" ), XB_KEY_NOT_UNIQUE );
  rc += TestMethod( po, "InsertKey( M )", llO.InsertKey( "M" ), 0 );
  rc += TestMethod( po, "InsertKey( C )", llO.InsertKey( "C" ), 0 );
  rc += TestMethod( po, "InsertKey( F )", llO.InsertKey( "F" ), 0 );
  rc += TestMethod( po, "InsertKey( Q )", llO.InsertKey( "Q" ), 0 );
  rc += TestMethod( po, "InsertKey( A )", llO.InsertKey( "A" ), 0 );
  rc += TestMethod( po, "InsertKey( T )", llO.InsertKey( "T" ), 0 );

  if( po > 1 ){
    std::cout << "dumping node chain from beginning to end\n";
    llN = llO.GetHeadNode();
    while( llN ){
      s2 = llN->GetKey();
      std::cout << "node = " <<  s2.Str() << "\n";
      llN = llN->GetNextNode();
    }

    std::cout << "dumping node chain from beginning to beginning\n";
    llN = llO.GetHeadNode();
    while( llN ){
      s2 = llN->GetKey();
      std::cout << "node = " <<  s2.Str() << "\n";
      llN = llN->GetPrevNode();
    }

    std::cout << "dumping node chain from end to end\n";
    llN = llO.GetEndNode();
    while( llN ){
      s2 = llN->GetKey();
      std::cout << "node = " <<  s2.Str() << "\n";
      llN = llN->GetNextNode();
    }

    std::cout << "dumping node chain from end to beginning\n";
    llN = llO.GetEndNode();
    while( llN ){
      s2 = llN->GetKey();
      std::cout << "node = " <<  s2.Str() << "\n";
      llN = llN->GetPrevNode();
    }


  }

  // xbLinkListNode<xbString> * psN;
  s2 = "A";
  rc += TestMethod( po, "KeyExists('A')", llO.KeyExists( s2 ), xbTrue );
  rc += TestMethod( po, "RemoveKey('A')", llO.RemoveKey( s2 ), 0 );
  s2 = "J";
  rc += TestMethod( po, "RemoveKey('J')", llO.RemoveKey( s2 ), 0 );
  s2 = "T";
  rc += TestMethod( po, "RemoveKey('T')", llO.RemoveKey( s2 ), 0 );
  rc += TestMethod( po, "RemoveKey('T')", llO.RemoveKey( s2 ), XB_NOT_FOUND );
  rc += TestMethod( po, "KeyExists('T')", llO.KeyExists( s2 ), xbFalse );



  xbLinkListOrd<xbUInt32> lloi;
  lloi.SetDupKeys( 0 );
  xbUInt32 i = 3;

  rc += TestMethod( po, "InsertKey( 1 )", lloi.InsertKey( i ), 0 );
  rc += TestMethod( po, "RemoveKey( 1 )", lloi.RemoveKey( i ), 0 );
  rc += TestMethod( po, "RemoveKey( 1 )", lloi.RemoveKey( i ), XB_NOT_FOUND );


  if( po > 1 ){
    std::cout << "dumping node chains after delete\n";
    std::cout << "dumping node chain from beginning to end\n";
    llN = llO.GetHeadNode();
    while( llN ){
      s2 = llN->GetKey();
      std::cout << "node = " <<  s2.Str() << "\n";
      llN = llN->GetNextNode();
    }

    std::cout << "dumping node chain from beginning to beginning\n";
    llN = llO.GetHeadNode();
    while( llN ){
      s2 = llN->GetKey();
      std::cout << "node = " <<  s2.Str() << "\n";
      llN = llN->GetPrevNode();
    }

    std::cout << "dumping node chain from end to end\n";
    llN = llO.GetEndNode();
    while( llN ){
      s2 = llN->GetKey();
      std::cout << "node = " <<  s2.Str() << "\n";
      llN = llN->GetNextNode();
    }

    std::cout << "dumping node chain from end to beginning\n";
    llN = llO.GetEndNode();
    while( llN ){
      s2 = llN->GetKey();
      std::cout << "node = " <<  s2.Str() << "\n";
      llN = llN->GetPrevNode();
    }
  }


  xbLinkListOrd<xbUInt32> ll32;
  xbUInt32 ul3 = 0;
  ll32.InsertKey( ul3, ul3 );
  ll.Clear();



#ifdef XB_LOGGING_SUPPORT
  sMsg.Sprintf( "Program [%s] terminating with [%d] errors...", av[0], rc * -1 );
  x.WriteLogMessage( sMsg );
#endif

  if( po > 0 || rc < 0 )
    fprintf( stdout, "Total Errors = %d\n", rc * -1 );

  return rc;
}




