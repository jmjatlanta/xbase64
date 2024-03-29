/*  xb_ex_string.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2021,2022 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

// This demonstrates the string class


#include "xbase.h"

using namespace xb;

int main()
{

  // create a string, assign a value, print it
  xbString s1;
  s1 = "Test String 1";
  fprintf( stdout, "s1 = [%s]\n", s1.Str());

  // create another string, copy the value from s1 into it, print it
  xbString s2;
  s2 = s1;
  std::cout << "s2 = [" << s2.Str() << "]" << std::endl;

  // create another string with and print it
  xbString s3( 'X' );
  std::cout << "s3 = [" << s3.Str() << "]" << std::endl;  

  // create another string with and print it and print it out yet another way
  xbString s4( "Class constructor test 4" );
  printf( "s4 = [%s]\n", s4.Str() );

  // create another string with a size limit and print it out
  xbString s5( "Class constructor test 4", 7 );
  printf( "s5 = [%s]\n", s5.Str() );

  // create another string from a string
  xbString s6( s5 );
  printf( "s6 = [%s]\n", s6.Str() );

  // create 100 byte string with nothing in it
  xbString s7( (xbUInt32) 100 );
  printf( "s7 = [%s]\n", s7.Str() );

  // Extract character from a particular position in the string
  printf( "[] test -- Position 7 (starts from 1) from String 1 = [%c]\n", s1[7] );
  // or use the getCharacter method
  printf( "getCharacter() test -- Position 7 (starts from 1) from String 1 = [%c]\n", s1.GetCharacter(7) );

  // set string 7 to a character
  s7 = 'Z';
  printf( "updated s7 = [%s]\n", s7.Str() );

  // trim methods
  s3 = "   abc   ";
  s3.Ltrim();
  #ifdef XB_DEBUG_SUPPORT
  s3.Dump( "LTrim test" );
  #else
  std::cout << s3.Str() << std::endl;
  #endif

  s3 = "   abc   ";
  s3.Rtrim();
  std::cout << "RTrim test - " << s3.Str() << std::endl;

  s3.Trim();
  std::cout << "Trim test - " << s3.Str() << std::endl;
  printf( "s3 Len = [%d]\n", s3.Len() );

  // Concatenation tests - I
  s1 = "Concatenation test1 part 1    ";
  s1 += "Concatenation test1 part 2    ";
  s2 = " s2 data ";
  s1 += s2;
  s1 += 'z';

  // Concatenation tests - II
  s1 = "Concatenation test1 part 1    ";
  s1 -= "Concatenation test1 part 2    ";
  s1 -= 'X';
  s1 -= s2;
  std::cout << "Concatenation test 2 - " << s1.Str() << std::endl; 

  // Concatenation tests - III
  s1 = "s1data  ";
  s2 = "s2data ";

  s3 = s1 - s2;
  std::cout << "Concatenation test 3a - " << s3.Str() << std::endl;

  s3 = s1 + s2;
  std::cout << "Concatenation test 3b - " << s3.Str() << std::endl;

  s3 = s1 + " char * data ";
  std::cout << "Concatenation test 3c - " << s3.Str() << std::endl;

  s3 = s1 + 'Z';
  std::cout << "Concatenation test 3d - " << s3.Str() << std::endl;

  s3 = 'A';

  std::cout << s3.Str() << std::endl;
  s3 += s1;

  std::cout << s3.Str() << std::endl;

  // The following compiles and runs, but is not valid
  // s3 = 'A' + s1;

  std::cout << std::endl << "== operator tests"  << std::endl;
  if( s1 == s2 )
    std::cout << s1.Str() << " == " << s2.Str() << std::endl;
  else
    std::cout << s1.Str() << " != " << s2.Str() << std::endl;

  s1 = s2;
  if( s1 == s2 )
    std::cout << s1.Str() << " == " << s2.Str() << std::endl;
  else
    std::cout << s1.Str() << " != " << s2.Str() << std::endl;

  if( s1 == "sometestdata" )
    std::cout << s1.Str() << " == sometestdata" << s2.Str() << std::endl;
  else
    std::cout << s1.Str() << " != sometestdata" << s2.Str() << std::endl;

  std::cout << std::endl << "!= operator tests"  << std::endl;
  s2 = "abc123";
  std::cout << "s1 - " << s1.Str() << std::endl;
  std::cout << "s2 - " << s2.Str() << std::endl;
  
  if( s1 != s2 )
    std::cout << s1.Str() << " != " << s2.Str() << std::endl;
  else
    std::cout << s1.Str() << " == " << s2.Str() << std::endl;

  s1 = s2;
  if( s1 != s2 )
    std::cout << s1.Str() << " != " << s2.Str() << std::endl;
  else
    std::cout << s1.Str() << " == " << s2.Str() << std::endl;

  if( s1 != "sometestdata" )
    std::cout << s1.Str() << " != [sometestdata]" << std::endl;
  else
    std::cout << s1.Str() << " == [sometestdata]" << std::endl;

  std::cout << std::endl << "< operator tests" << std::endl;
  s1 = "AAA";
  s2 = "BBB";

  if( s1 < s2 )
    std::cout << s1.Str() << " < " << s2.Str() << std::endl;
  else
    std::cout << s1.Str() << " >= " << s2.Str() << std::endl;

  s1 = "BBB";
  if( s1 < s2 )
    std::cout << s1.Str() << " < " << s2.Str() << std::endl;
  else
    std::cout << s1.Str() << " >= " << s2.Str() << std::endl;

  s1 = "CCC";  
  if( s1 < s2 )
    std::cout << s1.Str() << " < " << s2.Str() << std::endl;
  else
    std::cout << s1.Str() << " >= " << s2.Str() << std::endl;

  std::cout << std::endl << "> operator tests" << std::endl;
  s1 = "AAA";
  s2 = "BBB";

  if( s1 > s2 )
    std::cout << s1.Str() << " > " << s2.Str() << std::endl;
  else
    std::cout << s1.Str() << " <= " << s2.Str() << std::endl;

  s1 = "BBB";
  if( s1 > s2 )
    std::cout << s1.Str() << " > " << s2.Str() << std::endl;
  else
    std::cout << s1.Str() << " <= " << s2.Str() << std::endl;

  s1 = "CCC";
  if( s1 > s2 )
    std::cout << s1.Str() << " > " << s2.Str() << std::endl;
  else
    std::cout << s1.Str() << " <= " << s2.Str() << std::endl;

  std::cout << std::endl << "<= operator tests" << std::endl;
  s1 = "AAA";
  s2 = "BBB";

  if( s1 <= s2 )
    std::cout << s1.Str() << " <= " << s2.Str() << std::endl;
  else
    std::cout << s1.Str() << " > " << s2.Str() << std::endl;

  s1 = "BBB";
  if( s1 <= s2 )
    std::cout << s1.Str() << " <= " << s2.Str() << std::endl;
  else
    std::cout << s1.Str() << " > " << s2.Str() << std::endl;

  s1 = "CCC";  
  if( s1 <= s2 )
    std::cout << s1.Str() << " <= " << s2.Str() << std::endl;
  else
    std::cout << s1.Str() << " > " << s2.Str() << std::endl;

  std::cout << std::endl << ">= operator tests" << std::endl;
  s1 = "AAA";
  s2 = "BBB";

  if( s1 >= s2 )
    std::cout << s1.Str() << " >= " << s2.Str() << std::endl;
  else
    std::cout << s1.Str() << " < " << s2.Str() << std::endl;

  s1 = "BBB";
  if( s1 >= s2 )
    std::cout << s1.Str() << " >= " << s2.Str() << std::endl;
  else
    std::cout << s1.Str() << " < " << s2.Str() << std::endl;

  s1 = "CCC";  
  if( s1 >= s2 )
    std::cout << s1.Str() << " >= " << s2.Str() << std::endl;
  else
    std::cout << s1.Str() << " < " << s2.Str() << std::endl;

  std::cout << "(const char *) " << (const char *) s2.Str() << std::endl;

  std::cout << std::endl << "CountChar() test" << std::endl;
  s1 = "ABADFDSGA";
  xbUInt32 i = s1.CountChar( 'A' );
  std::cout << "There are " << i << " 'A's in " << s1.Str() << std::endl;

  s1.Ltrunc( 4 );
  std::cout << "lTunc(4) test s1 = [" << s1.Str() << "]" << std::endl;

  std::cout << std::endl << "PutAt() test" << std::endl;
  s1.PutAt( 3, 'Z' );
  std::cout << "Third char should be a 'Z' = " << s1.Str() << std::endl;

  std::cout << std::endl << "AddBackSlash() test" << std::endl;
  s1.AddBackSlash( 'Z' );
  std::cout << "Should be a backslash before the 'Z' = " << s1.Str() << std::endl;

  std::cout << std::endl << "Assign() test" << std::endl;
  s2 = "1234567890";
  std::cout << "s2 = " << s2.Str() << std::endl;
  s1.Assign( s2, 4, 5 );
  std::cout << "assign( s2, 4, 5 ) results = " << s1.Str() << std::endl;
  s1.Assign( s2, 4, 15 );
  std::cout << "assign( s2, 4, 15 ) results = " << s1.Str() << std::endl;

  s1.Assign( s2, 5 );
  std::cout << "Assign( s2, 5 ) results = " << s1.Str() << std::endl;
  s1.Assign( s2, 15 );
  std::cout << "Assign( s2, 15 ) results = " << s1.Str() << std::endl;

  std::cout << std::endl << "s1.copy() test" << std::endl;
  s1 = "ABC";
  std::cout << "s1 = " << s1.Str() << std::endl;
  std::cout << "s2 = " << s2.Str() << std::endl;

  s1 = s2.Copy();
  std::cout << "s1.Copy() results"  << s1.Str() << std::endl;

  s1 = "0x35";
  char hexChar;
  s1.CvtHexChar( hexChar );
  std::cout << "CvtHexChar test [" << s1.Str() << "] converts to [" << hexChar << "]" << std::endl;
  s1 = "0x65";
  s1.CvtHexChar( hexChar );
  std::cout << "cvHexChar test [" << s1.Str() << "] converts to [" << hexChar << "]" << std::endl;

  s1 = "0x610x620x630x640x65";
  s1.CvtHexString( s2 );
  std::cout << "CvtHexString [" << s1.Str() << "] converts to [" << s2.Str() << "]" << std::endl;

  s1.ExtractElement( "aaaa|bbbb|cccc|dddd", '|', 2, 0 );
  std::cout << "ExtractElement() " << s1.Str() << std::endl; 

  s1 = "123";
  s2 = "ABC";
  std::cout << "HasAlphaChars( " << s1.Str() << " ) = " << s1.HasAlphaChars() << std::endl;   
  std::cout << "HasAlphaChars( " << s2.Str() << " ) = " << s2.HasAlphaChars() << std::endl;

  s2 = "";
  std::cout << "IsEmpty( " << s1.Str() << " ) = " << s1.IsEmpty() << std::endl;
  std::cout << "IsEmpty( " << s2.Str() << " ) = " << s2.IsEmpty() << std::endl;

  s1 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  std::cout << s1.Str() << " s1.Mid( 3,5 ) = [" << s1.Mid( 3, 5 ).Str() << "]"  << std::endl;
  std::cout << s1.Str() << " s1.Mid( 25, 10 ) = [" << s1.Mid( 25, 10 ).Str() << "]"  << std::endl;

  std::cout << s1.Str() << " s1.Pos('G') = " << s1.Pos( 'G' ) << std::endl;
  std::cout << s1.Str() << " s1.Pos(\"JKL\") = " << s1.Pos( "JKL" ) << std::endl;

  std::cout << "Remove(  3,  5 ) before " << s1.Str() << std::endl;
  std::cout << s1.Str() << " s1.Remove( 3, 5 ) = [" << s1.Remove( 3, 5 ).Str() << "]" << std::endl; 
  std::cout << "Remove(  3,  5 ) after  " << s1.Str() << std::endl;
  s1 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  std::cout << "s1.Remove( 20, 10 ) = [" << s1.Remove( 20, 10 ).Str() << "]" << std::endl;
  
  std::cout << "Remove( 20, 10 ) - " << s1.Str() << std::endl;
  

  s1.Sprintf( "%d", 12345 );
  std::cout << "Sprintf( %d, 12345 ) " << s1.Str() << std::endl;

  s1.SetNum( (long) 123456 );
  std::cout << "s1.SetNum( 123456 ) = " << s1.Str()  << std::endl;

  s1.Set( "Yet another way to set a string value" );
  std::cout << "Set - " << s1.Str() << std::endl;

  s1 = "ABCABCABZ";
  std::cout << "SwapChars( 'A', '9' ) before - " << s1.Str() << std::endl;
  s1.SwapChars( 'A', '9' );
  std::cout << "SwapChars( 'A', '9' ) after - " << s1.Str() << std::endl;

  s1.ToLowerCase();
  std::cout << "ToLowerCase - " << s1.Str() << std::endl;

  s1.ToUpperCase();
  std::cout << "ToUpperCase - " << s1.Str() << std::endl;

  s1.ZapChar( '9' );
  std::cout << "ZapChar( '9' )"  << s1.Str() << std::endl;

  s1.ZapLeadingChar( 'B' );
  std::cout << "ZapLeadingChar( 'B' )"  << s1.Str() << std::endl;

  s1.ZapTrailingChar( 'Z' );
  std::cout << "ZapLeadingChar( 'Z' ) - " << s1.Str() << std::endl;

  s1 = "123";
  s1.PadLeft( '0', 9 );
  std::cout << "s1.PadLeft('0', 9 ) - " << s1.Str() << std::endl;

  s1 = "abc";
  s1.PadRight( 'Z', 9 );
  std::cout << "s1.PadRight('Z', 9 ) " << s1.Str() << std::endl;

  xbString sNullString;
  if( sNullString.IsNull())
    std::cout << "sNullString is null" << std::endl;
  else
    std::cout << "sNullString is not null" << std::endl;

  xbString tstS( "ZZZZZZZZZ" );
  tstS = s1.Left( 5 );

  std::cout << "tstS = " << tstS.Str() << "\n";
  std::cout << "s1 = " << s1.Str() << "\n";

  tstS = "1234567890";
  std::cout << "mid result = " << tstS.Mid( 3, 3 ).Str() << std::endl;

  tstS = "1234567890";
  std::cout << "left result = " << tstS.Left( 3 ).Str() << std::endl;


  return 0;
}
