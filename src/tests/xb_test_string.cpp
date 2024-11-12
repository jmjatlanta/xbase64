/* xb_test_string.cpp

XBase63 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

// This program tests the string class xbString

// usage:  xb_test_string QUIET|NORMAL|VERBOSE

#define VERBOSE

#include "xbase.h"

using namespace xb;

#include "tstfuncs.cpp"

int main( int argCnt, char **av = NULL )

//int main( int argCnt, char *argv[] )
{
  int rc = 0;
  int po = 1;                   /* print option   */
                                /* 0 - QUIET      */
                                /* 1 - NORMAL     */
                                /* 2 - VERBOSE    */

  if( argCnt > 1 ) {
    if( av[1][0] == 'Q' )
      po = 0;
    else if( av[1][0] == 'V' )
      po = 2;
  }

  xbXBase x;
  xbString sMsg;

  #ifdef XB_LOGGING_SUPPORT
  x.SetLogDirectory( PROJECT_LOG_DIR );
  x.EnableMsgLogging();
  if( po ){
    std::cout << "Logfile is [" << x.GetLogFqFileName().Str() << "]" << std::endl;
  }
  sMsg.Sprintf( "Program [%s] initializing...", av[0] );
  x.WriteLogMessage( sMsg );
  #endif

  x.SetDataDirectory( PROJECT_DATA_DIR );

  InitTime();

  // create a string, assign a value
  xbString s1;
  s1 = "Test String 1";
  rc += TestMethod( po, "Constructor s1" , s1, "Test String 1", 13 );

  // create another string, copy the value from s1 into it
  xbString s2;
  s2 = s1;
  rc += TestMethod( po, "Operator '=' " , s2, "Test String 1",  13 );

  // create another string with a single character
  xbString s3( 'X' );

  //std::cout << "s3 = [" << s3 << "]" << std::endl;
  rc += TestMethod( po, "Constructor s3" , s3, "X",  1 );

  // create another string and assign data to it yet another way
  xbString s4( "Class constructor test 4" );
  rc += TestMethod( po, "Constructor s4" , s4, "Class constructor test 4",  24 );

  // create another string with a size limit
  xbString s5( "Class constructor test 4", 7 );
  rc += TestMethod( po, "Constructor s5" , s5, "Class c",  7 );

  // create another string from a string
  xbString s6( s5 );
  rc += TestMethod( po, "Constructor s6" , s6, "Class c",  7 );

  // create 15 byte string with nothing in it
  xbString s7( (xbUInt32) 15 );
  rc += TestMethod( po, "Constructor s7" , s7, "",  0 );

  xbString s8( "some test data", 6 );
  rc += TestMethod( po, "Constructor s8" , s8, "some t",  6 );

  xbString s9( "some test data", 30 );
  rc += TestMethod( po, "Constructor s9" , s9, "some test data",  14 );

  // Extract character from a particular position in the string
  rc += TestMethod( po, "Operator [] ", s1[7], 't' );
  rc += TestMethod( po, "Operator [] ", s1.GetCharacter(7), 't' );

  s1[6] = 'X';
  rc += TestMethod( po, "Operator assignment [] ", s1.GetCharacter(6), 'X' );

  // set string 7 to a character
  s7 = 'Z';
  rc += TestMethod( po, "Operator =", s7.Str(), "Z", 1 );

  // Concatenation tests - I
  s1  = " part 1 ";
  s1 += " part 2 ";
  s2  = " part 3 ";
  s1 += s2;
  s1 += 'Z';
  rc += TestMethod( po, "Concatenation test 1", s1, " part 1  part 2  part 3 Z", 25 );

  // Concatenation tests - II
  s1 = "part 1    ";
  s1 -= "part 2    ";
  s1 -= 'X';
  s1 -= s2;
  rc += TestMethod( po, "Concatenation test 2", s1, "part 1part 2X part 3", 20 );

  // Concatenation tests - III
  s1 = "s1data  ";
  s2 = "s2data ";
  s3 = s1 - s2;
  rc += TestMethod( po, "Concatenation test 3", s3, "s1datas2data", 12 );

  // Concatenation tests - IV
  s3 = s1 + s2;
  rc += TestMethod( po, "Concatenation test 4", s3, "s1data  s2data ", 15 );

  // Concatenation tests - V
  s3 = s1 + " (char * data) " + "xyz  " + s2 + 'z';
  rc += TestMethod( po, "Concatenation test 1", s3, "s1data   (char * data) xyz  s2data z", 36 );

  //Operator tests
  s1 = "aaa";
  s2 = "bbb";
  rc  += TestMethod( po, "Operator == ", s1 == s2, 0 );
  rc  += TestMethod( po, "Operator != ", s1 != s2, 1 );
  rc  += TestMethod( po, "Operator < ",  s1 <  s2, 1 );
  rc  += TestMethod( po, "Operator > ",  s1 >  s2, 0 );
  rc  += TestMethod( po, "Operator < ",  s1 <= s2, 1 );
  rc  += TestMethod( po, "Operator > ",  s1 >= s2, 0 );

  s1 = s2;
  rc  += TestMethod( po, "Operator == ", s1 == s2, 1 );
  rc  += TestMethod( po, "Operator != ", s1 == s2, 1 );
  rc  += TestMethod( po, "Operator < ",  s1 <  s2, 0 );
  rc  += TestMethod( po, "Operator > ",  s1 >  s2, 0 );
  rc  += TestMethod( po, "Operator < ",  s1 <= s2, 1 );
  rc  += TestMethod( po, "Operator > ",  s1 >= s2, 1 );

  s1 = "XYZ";
  rc  += TestMethod( po, "Operator * ", (const char *) s1, "XYZ", 3 );

  s1 = 'Z';
  rc += TestMethod( po, "Operator = ", s1, "Z", 1 ); 

  s1 = "ABC,xyz";
  rc += TestMethod( po, "CountChar(c,1) ", (xbInt32) s1.CountChar( ',',1 ), 1 );

  s1 = "ABC,xy,z";
  rc += TestMethod( po, "CountChar(c,1) ", (xbInt32) s1.CountChar( ',',1 ), 2 );

  s1 = "ABC,xy,z'asad,as'adss";
  rc += TestMethod( po, "CountChar(c,1) ", (xbInt32) s1.CountChar( ',',1 ), 2 );

  s1 = "ABADFDSGA";
  rc += TestMethod( po, "CountChar() ", (xbInt32) s1.CountChar( 'A' ), 3 );

  s1.Ltrunc( 4 );
  rc += TestMethod( po, "Ltrunc(4) ", s1, "FDSGA", 5 );

  s1.PutAt( 3, 'Z' );
  rc += TestMethod( po, "PutAt(3,'Z') ", s1, "FDZGA", 5 );

  s1.AddBackSlash( 'Z' );
  rc += TestMethod( po, "AddBackSlash( 'Z' ) ", s1, "FD\\ZGA", 6 );

  s1 = "ABCDEFG";
  rc += TestMethod( po, "s1.Append( 'Z' )", s1.Append( 'Z' ).Str(), "ABCDEFGZ", 8 );
  rc += TestMethod( po, "s1.Append( '999' )", s1.Append( "999" ), "ABCDEFGZ999", 11 );
  s2 = "!@#";
  rc += TestMethod( po, "s1.Append( '!@#' )", s1.Append( s2 ), "ABCDEFGZ999!@#", 14 );

  xbString sAppend1;
  sAppend1.Append( "abc123", 6 );
  rc += TestMethod( po, "Append", sAppend1, "abc123", 6 );

  xbString sAppend2;
  sAppend2.Append( "abc123", 8 );
  rc += TestMethod( po, "Append", sAppend2, "abc123", 6 );

  xbString sAppend3;
  sAppend3.Append( "abc123", 3 );
  rc += TestMethod( po, "Append", sAppend3, "abc", 3 );


  rc += TestMethod( po, "s1.Assign( 'ABCDE', 3, 2 )",  s1.Assign( "ABCDE", 3, 2 ), "CD", 2 );
  rc += TestMethod( po, "s1.Assign( 'ABCDE', 2, 7 )",  s1.Assign( "ABCDE", 2, 7 ), "BCDE", 4 );
  rc += TestMethod( po, "s1.Assign( 'ABCDE', 1, 4 )",  s1.Assign( "ABCDE", 1, 4 ), "ABCD", 4 );
  rc += TestMethod( po, "s1.Assign( 'ABCDE', 5, 5 )",  s1.Assign( "ABCDE", 5, 5 ), "E", 1 );
  rc += TestMethod( po, "s1.Assign( 'ABCDE', 15, 5 )", s1.Assign( "ABCDE", 15, 5 ), "", 0 );

  rc += TestMethod( po, "s1.Assign( 'ABCDE', 1 )",  s1.Assign( "ABCDE", 1 ), "ABCDE", 5 );
  rc += TestMethod( po, "s1.Assign( 'ABCDE', 3 )",  s1.Assign( "ABCDE", 3 ), "CDE", 3 );
  rc += TestMethod( po, "s1.Assign( 'ABCDE', 10 )", s1.Assign( "ABCDE", 10 ), "", 0 );

  s2 = "ABCDE";
  rc += TestMethod( po, "s1.Assign( s2, 3, 2 )",  s1.Assign( s2, 3, 2 ), "CD", 2 );
  rc += TestMethod( po, "s1.Assign( s2, 2, 7 )",  s1.Assign( s2, 2, 7 ), "BCDE", 4 );
  rc += TestMethod( po, "s1.Assign( s2, 1, 4 )",  s1.Assign( s2, 1, 4 ), "ABCD", 4 );
  rc += TestMethod( po, "s1.Assign( s2, 5, 5 )",  s1.Assign( s2, 5, 5 ), "E", 1 );
  rc += TestMethod( po, "s1.Assign( s2, 15, 5 )", s1.Assign( s2, 15, 5 ), "", 0 );

  rc += TestMethod( po, "s1.Assign( s2, 1 )",  s1.Assign( s2, 1 ), "ABCDE", 5 );
  rc += TestMethod( po, "s1.Assign( s2, 3 )",  s1.Assign( s2, 3 ), "CDE", 3 );
  rc += TestMethod( po, "s1.Assign( s2, 10 )", s1.Assign( s2, 10 ), "", 0 );

  s2 = "1234567890";
  s1 = s2.Copy();
  rc += TestMethod( po, "Copy() ", s1, "1234567890", 10 );

  s1 = "0x35";
  char hexChar;
  s1.CvtHexChar( hexChar );
  rc += TestMethod( po, "CvtHexChar(hexChar) ", hexChar, '5' );

  s1 = "0x610x620x630x640x65";
  s1.CvtHexString( s2 );
  rc += TestMethod( po, "CvtHexString() ", s2, "abcde", 5 );

  s1.ExtractElement( "aaaa|bbbb|cccc|dddd", '|', 2, 0 );
  rc += TestMethod( po, "ExtractElement() ", s1, "bbbb", 4 );

  s1.ExtractElement( "aaaa|b'bb|c'ccc|dddd", '|', 3, 1 );
  rc += TestMethod( po, "ExtractElement() ", s1, "dddd", 4 );

  s1 = "aaaa|bbbb|cccc|dddd";
  s2.ExtractElement( s1, '|', 2, 0 );
  rc += TestMethod( po, "ExtractElement() ", s2, "bbbb", 4 );

  s1 = "abcabcabx";
  rc += TestMethod( po, "GetLastPos", (xbInt16) s1.GetLastPos( 'b' ), 8 );
  s1 = "abcabcabx";
  rc += TestMethod( po, "GetLastPos", (xbInt16) s1.GetLastPos( 'x' ), 9 );
  s1 = "abcabcabx";
  rc += TestMethod( po, "GetLastPos", (xbInt16) s1.GetLastPos( '$' ), 0 );
  rc += TestMethod( po, "GetLastPos", (xbInt16) s1.GetLastPos( "ab" ), 7 );

  s1 = "\\ABC\\XYZ";
  rc += TestMethod( po, "GetPathSeparator()", s1.GetPathSeparator(), '\\' );

  s1 = "/ABC/XYZ";
  rc += TestMethod( po, "GetPathSeparator()", s1.GetPathSeparator(), '/' );

  s1 = "123";
  s2 = "ABC";
  rc += TestMethod( po, "HasAlphaChars()", s1.HasAlphaChars(), 0 );
  rc += TestMethod( po, "HasAlphaChars()", s2.HasAlphaChars(), 1 );

  s2 = "";
  rc += TestMethod( po, "IsEmpty()", s2.IsEmpty(), 1 );

  s1.SetNum( (long) 123456 );

  s1 = "abcZZZ123";
  s1.Left( 4 );
  rc += TestMethod( po, "Left(4) ", s1, "abcZ", 4 );

  s1.Left( 1 );
  rc += TestMethod( po, "Left(1) ", s1, "a", 1 );

  s1.Left( 0 );
  rc += TestMethod( po, "Left(0) ", s1, "", 0 );

  // trim tests
  s1 = "   ABC   ";
  rc += TestMethod( po, "Ltrim()", s1.Ltrim(), "ABC   ", 6 );

  s1 = "   ABC   ";
  rc += TestMethod( po, "Rtrim()", s1.Rtrim(), "   ABC", 6 );

  s1 = "   ABC   ";
  rc += TestMethod( po, "Trim() ", s1.Trim(), "ABC", 3 );

  s1 = "ABC   ";
  rc += TestMethod( po, "Ltrim()", s1.Ltrim(), "ABC   ", 6 );

  s1 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  rc += TestMethod( po, "Mid(3,0) ", s1.Mid(3,0), "CDEFGHIJKLMNOPQRSTUVWXYZ", 24 );

  s1 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  rc += TestMethod( po, "Mid(4,5) ", s1.Mid(4,5), "DEFGH", 5 );

  s1 = "123";
  s1.PadLeft( '0', 9 );
  rc += TestMethod( po, "PadLeft() ", s1, "000000123", 9 );

  s1 = "abc";
  s1.PadRight( 'Z', 9 );
  rc += TestMethod( po, "PadRight() ", s1, "abcZZZZZZ", 9 );

  s1.PadRight( 'Z', 4 );
  rc += TestMethod( po, "PadRight() ", s1, "abcZZZZZZ", 9 );



  s1 = "DEFGHI";
  rc += TestMethod( po, "Pos('G') ", (xbInt32) s1.Pos( 'G' ), 4 );
  rc += TestMethod( po, "Pos(\"EFG\") ", (xbInt32) s1.Pos( "EFG" ), 2 );

  rc += TestMethod( po, "Pos('0') ", (xbInt32) s1.Pos( '0' ), 0 );
  rc += TestMethod( po, "Pos(\"000\") ", (xbInt32) s1.Pos( "000" ), 0 );
  rc += TestMethod( po, "Pos(\"DEF\") ", (xbInt32) s1.Pos( "DEF" ), 1 );

  s1 = "ABC.123.abc";
  rc += TestMethod( po, "Pos( '.', 4 )", (xbInt32) s1.Pos( '.', 4 ), 4 );
  rc += TestMethod( po, "Pos( '.', 5 )", (xbInt32) s1.Pos( '.', 5 ), 8 );
  rc += TestMethod( po, "Pos( '.', 9 )", (xbInt32) s1.Pos( '.', 9 ), 0 );



  s1 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  rc += TestMethod( po, "Remove(3,5) ", s1.Remove( 3, 5 ), "ABHIJKLMNOPQRSTUVWXYZ", 21 );
  s1 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  rc += TestMethod( po, "Remove(22,5) ", s1.Remove( 22, 5 ), "ABCDEFGHIJKLMNOPQRSTU", 21 );
  s1 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  rc += TestMethod( po, "Remove(24,5) ", s1.Remove( 24, 5 ), "ABCDEFGHIJKLMNOPQRSTUVW", 23 );

  s1.Set( "abcdef.dbf" );
  s1.Replace( "def", "DEF" );
  rc += TestMethod( po, "Replace", s1, "abcDEF.dbf", 10 );
  s1.Replace( ".dbf", ".DBF" );
  rc += TestMethod( po, "Replace", s1, "abcDEF.DBF", 10 );
  s1.Set( "abcdef.dbf" );
  s1.Replace( "def", "DEFG" );
  rc += TestMethod( po, "Replace", s1, "abcDEFG.dbf", 11 );
  s1.Set( "abcdefdef.dbf" );
  s1.Replace( "def", "DEFG" );
  rc += TestMethod( po, "Replace", s1, "abcDEFGDEFG.dbf", 15 );


  s1.SetNum( (long) 123456 );
  rc += TestMethod( po, "SetNum() ", s1, "123456", 6 );

  xbFloat f = (xbFloat) 12.35;
  // %f format varies depending on compiler
  s1.Sprintf( "%6.2f", f );
  s1.Ltrim();
  rc += TestMethod( po, "s1.Sprintf()/s.Trim()", s1, "12.35", 5 );

  char buf[5];
  buf[0] = 'W';
  buf[1] = 'X';
  buf[2] = 'Y';
  buf[3] = 'Z';
  buf[4] = 0x00;
  xbInt32 l = 1234567;
  s2 = "test string";

  rc += TestMethod( po, "s1.Sprintf()", s1.Sprintf( "%s %d %s %ld", buf, 12, s2.Str(), l  ), "WXYZ 12 test string 1234567", 27 );


  s1 = "ABCABCABZ";
  s1.SwapChars( 'A', '9' );
  rc += TestMethod( po, "SwapChars() ", s1, "9BC9BC9BZ", 9 );

  s1.ToLowerCase();
  rc += TestMethod( po, "ToLowerCase() ", s1, "9bc9bc9bz", 9 );

  s1.ToUpperCase();
  rc += TestMethod( po, "ToUpperCase() ", s1, "9BC9BC9BZ", 9 );

  s1 = "T";
  rc += TestMethod( po, "ValidLogicalValue", s1.ValidLogicalValue(), 1 );

  s1 = "xyz";
  rc += TestMethod( po, "ValidLogicalValue", s1.ValidLogicalValue(), 0 );

  s1 = "-123456.89";
  rc += TestMethod( po, "ValidNumericValue", s1.ValidNumericValue(), 1 );

  s1 = "ABC-123456.89";
  rc += TestMethod( po, "ValidNumericValue", s1.ValidNumericValue(), 0 );


  s1 = "BC9BC99BZ";
  s1.ZapChar( '9' );
  rc += TestMethod( po, "ZapChar('9') ", s1, "BCBCBZ", 6 );

  s1.ZapLeadingChar( 'B' );
  rc += TestMethod( po, "ZapLeadingChar('B') ", s1, "CBCBZ", 5 );

  s1.ZapTrailingChar( 'Z' );
  rc += TestMethod( po, "ZapTrailingChar('Z') ", s1, "CBCB", 4 );
  s1.ZapTrailingChar( 'Z' );
  rc += TestMethod( po, "ZapTrailingChar('Z') ", s1, "CBCB", 4 );


  s1 = ")";
  rc += TestMethod( po, "GetLastPos", (xbInt16) s1.GetLastPos( ')' ), 1 );
  s1 = "))))";
  rc += TestMethod( po, "GetLastPos", (xbInt16) s1.GetLastPos( ')' ), 4 );

  char * p; 
  p = (char *) malloc( 5 );
  p[0] = '1';
  p[1] = '2';
  p[2] = '3';
  p[3] = '4';
  p[4] = 0x00;
  s1.Set( p, 5 );
  free( p );
  rc += TestMethod( po, "Set", s1, "1234", 4 );

  xbDouble d = 12345678.876543;
  xbString sD( d );
  rc += TestMethod( po, "xbDouble Constructor", sD, "12345678.876543", 15 );

  xbString sSet;
  sSet.Set( sD );
  rc += TestMethod( po, "Set", sD, sD, 15 );
  sSet.Set( s2 );
  rc += TestMethod( po, "Set", sSet, s2, 11 );
  sSet.Set( "" );
  rc += TestMethod( po, "Set", sSet, "", 0 );

  s1.Sprintf( "string %d", 1 );
  s2.Sprintf( "string %1.1f", 2.0 );
  s3.Sprintf( "%s and %s", s1.Str(), s2.Str());
  rc += TestMethod( po, "Sprintf", s3, "string 1 and string 2.0", 23 );


/*
  xbInt16 iErrorStop = 10;
  xbInt16 iRc = -100;
  sMsg.Sprintf( "class::method() Exception Caught. Error Stop = [%d] iRc = [%d] Expression = [%s]", iErrorStop, iRc, s3.Str() );
*/
  if( po > 0 || rc < 0 )
    fprintf( stdout, "Total Errors = %d\n", rc * -1 );

  #ifdef XB_LOGGING_SUPPORT
  sMsg.Sprintf( "Program [%s] terminating with [%d] errors...", av[0], rc * -1 );
  x.WriteLogMessage( sMsg );
  #endif

  return rc;
}

