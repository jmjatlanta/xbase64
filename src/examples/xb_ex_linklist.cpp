/* xb_ex_linklist.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023,2024 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net
*/

// This program demonstrates usage of the linked list functionality conatined within the library

#include "xbase.h"
using namespace xb;

//int main( int argCnt, char **av )
int main()
{
  int iRc = 0;
  xbXBase x;
  xbString s;

  // set up a linked list of strings
  xbLinkList<xbString> ll;

  // add a bunch of stuff to the front
  ll.InsertAtFront( "A" );
  ll.InsertAtFront( "B" );
  ll.InsertAtFront( "C" );
  ll.InsertAtFront( "D" );
  ll.InsertAtFront( "E" );
  ll.InsertAtFront( "F" );
  ll.InsertAtFront( "G" );
  ll.InsertAtFront( "H" );

  // add a bunch of stuff to the end
  ll.InsertAtEnd( "1" );
  ll.InsertAtEnd( "2" );
  ll.InsertAtEnd( "3" );
  ll.InsertAtEnd( "4" );
  ll.InsertAtEnd( "5" );
  ll.InsertAtEnd( "6" );
  ll.InsertAtEnd( "7" );
  ll.InsertAtEnd( "8" );

  std::cout << "iterate list begin to end" << std::endl;
  xbLinkListNode<xbString> * llN = ll.GetHeadNode();
  xbUInt32 ulCnt = ll.GetNodeCnt();
  for( xbUInt32 i = 0; i < ulCnt; i++ ){
    s = llN->GetKey();
    std::cout << s.Str() << " ";
    llN = llN->GetNextNode();
  }

  std::cout << std::endl << "iterate list end to begin" << std::endl;
  llN = ll.GetEndNode();
  for( xbUInt32 i = 0; i < ulCnt; i++ ){
    s = llN->GetKey();
    std::cout << s.Str() << " ";
    llN = llN->GetPrevNode();
  }

  std::cout << std::endl;

  ll.RemoveFromFront( s );
  std::cout << "Removed [" << s.Str() << "] from front" << std::endl;

  ll.RemoveFromEnd( s );
  std::cout << "Removed [" << s.Str() << "] from end" << std::endl;

  std::cout << "Node count before clear = " << ll.GetNodeCnt() << std::endl;
  ll.Clear();
  std::cout << "Node count after clear = " << ll.GetNodeCnt() << std::endl;

  // ordered link list 
  xbLinkListOrd<xbString> llO;
  llO.SetDupKeys( 0 );
  llO.InsertKey( "J" );

  if(( iRc = llO.InsertKey( "J" )) != XB_KEY_NOT_UNIQUE )
    std::cout << "This should error, no dups allowed" << std::endl;

  llO.InsertKey( "M" );
  llO.InsertKey( "C" );
  llO.InsertKey( "F" );
  llO.InsertKey( "Q" );
  llO.InsertKey( "A" );
  llO.InsertKey( "T" );

  std::cout << "dumping ordered list begin to end\n";
  llN = llO.GetHeadNode();
  while( llN ){
    s = llN->GetKey();
    std::cout << s.Str() << " ";
    llN = llN->GetNextNode();
  }

  std::cout << std::endl << "dumping node chain from end to begine\n";
  llN = llO.GetEndNode();
  while( llN ){
    s = llN->GetKey();
    std::cout << s.Str() << " ";
    llN = llN->GetPrevNode();
  }

  std::cout << std::endl;

  s = "A";
  if( llO.KeyExists( s )){
    std::cout << s.Str() << " exists, deleting." << std::endl;
    llO.RemoveKey( s );
  }
  if( llO.KeyExists( s ))
    std::cout << s.Str() << " exists" << std::endl;
  else
    std::cout << s.Str() << " does not exist" << std::endl;

  return iRc;
}




