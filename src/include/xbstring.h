/*  xbstring.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

#ifndef __XBSTRING_H__
#define __XBSTRING_H__

#ifdef  CMAKE_COMPILER_IS_GNUCC
#pragma interface
#endif

#include <stdlib.h>
#include <iostream>

namespace xb{


//! @brief Class for handling string data.

/*!
This class defines a basic string class with all the functions one would expect in a string class.

For purposes of the xbString class, a string is defined as a variable sized array of one byte
characters terminated with a null (0x00 or \0) byte.<br><br>

This version of the xbString class does not support wide (wchar_t) characters.  Perhaps you would 
be interested in creating a class for supporting wide characters.<br><br>

This string class handle strings in a 1-based (not 0 based) fashion.
Any string routines taking an offset use a 1-based value. That is, the first position of 
the string is position 1, not 0.<br><br>  

Position 1 (not 0) is considered the first position in a string.  
A return of 0 would indicate a not found condition. A return of 1, would be the
first byte.
*/


class XBDLLEXPORT xbString {

 public:
  //Various constructors
  xbString(xbUInt32 size);
  xbString(char cIn);
  xbString(const char *sIn, xbUInt32 ulMaxLen);
  xbString(const xbString &sIn);
  xbString(const char *sIn = "");
  xbString( xbDouble d );
  ~xbString();

  //operators
  xbString &operator= (const xbString &s);
  xbString &operator= (const char *s);

  operator const char *() const;

  char &operator[](xbUInt32 n) const;
  char &operator[](xbInt32 n) const;

  xbString &operator+=(const xbString &s);
  xbString &operator+=(const char *s);
  xbString &operator+=(char c);
  xbString &operator-=(const xbString &s);
  xbString &operator-=(const char *s);
  xbString &operator-=(char c);

  xbBool operator == ( const xbString& ) const;
  xbBool operator == ( const char * ) const;
  xbBool operator != ( const xbString& ) const;
  xbBool operator != ( const char * ) const;

  xbBool operator <  ( const xbString& ) const;
  xbBool operator >  ( const xbString& ) const;
  xbBool operator <= ( const xbString& ) const;
  xbBool operator >= ( const xbString& ) const;

  xbString  operator-( const xbString &s );
  xbString  operator+( const char *s );
  xbString  operator+( const xbString &s );
  xbString  operator+( const char c );

  xbString  &AddBackSlash( char c );
  xbString  &Append(const xbString &s);
  xbString  &Append(const char *s);
  xbString  &Append(const char *s, xbUInt32 ulByteCount );
  xbString  &Append(char c);
  xbString  &Assign(const char *srcStr, xbUInt32 ulStartPos, xbUInt32 ulCopyLen );
  xbString  &Assign(const char *srcStr, xbUInt32 ulStartPos );
  xbString  &Assign(const xbString &srcStr, xbUInt32 ulStartPos, xbUInt32 ulCopyLen );
  xbString  &Assign(const xbString &srcStr, xbUInt32 ulStartPos );

  xbString  Copy() const;
  xbUInt32  CountChar( char c ) const;
  xbUInt32  CountChar( char c, xbInt16 iOpt ) const;
  xbInt16   CvtHexChar( char &cOut );
  xbInt16   CvtHexString( xbString &sOut );
  xbInt16   CvtULongLong( xbUInt64 &ullOut );
  xbInt16   CvtLongLong( xbInt64 &llOut );

  #ifdef XB_DEBUG_SUPPORT
  void      Dump( const char *title ) const;
  void      Dump( const char *title, xbInt16 iOption ) const;
  void      DumpHex( const char *title ) const;
  #endif

  xbString  &ExtractElement(xbString &s, char delim, xbUInt32 iCnt, xbInt16 iOpt = 0 );
  xbString  &ExtractElement(const char *src, char delim, xbUInt32 iCnt, xbInt16 iOpt = 0 );

  char      GetCharacter( xbUInt32 ulPos ) const;
  xbUInt32  GetLastPos(char c) const;
  xbUInt32  GetLastPos(const char *s) const;
  char      GetPathSeparator() const;
  xbUInt32  GetSize() const;

  xbBool    HasAlphaChars() const;
  xbBool    IsEmpty() const;
  xbBool    IsNull() const;

  xbString  &Left( xbUInt32 ulLen );
  xbUInt32  Len() const;                     // returns the length of the string
  xbString  &Ltrim();
  xbString  &Ltrunc( xbUInt32 ulCnt );

  xbString  &Mid(xbUInt32 ulPos, xbUInt32 lLen );
  xbString  &PadLeft( char c, xbUInt32 ulLen );
  xbString  &PadRight( char c, xbUInt32 ulLen );

  xbUInt32  Pos(char c, xbUInt32 ulStartPos ) const;
  xbUInt32  Pos(char c) const;
  xbUInt32  Pos(const char *s) const;
  xbString  &PutAt(xbUInt32 ulPos, char c);

  xbString  &Remove( xbUInt32 ulPos, xbUInt32 ulN );
  xbString  &Replace( const char *sReplace, const char *sReplaceWith, xbInt16 iOpt = 0 );
  xbString  &Resize( xbUInt32 lSize );
  xbString  &Rtrim();

  xbString  &Set( const char *s );
  xbString  &Set( const xbString &s );
  xbString  &Set( const char *s, xbUInt32 ulSize );
  xbString  &SetNum( xbInt32 lNum );
  xbString  &Sprintf(const char *format, ...);

  const char *Str() const;
  char      *strncpy( char * cDest, xbUInt32 n ) const;
  xbString  &SwapChars( char from, char to );

  xbString  &ToLowerCase();
  xbString  &ToUpperCase();
  xbString  &Trim();

  xbBool    ValidLogicalValue() const;
  xbBool    ValidNumericValue() const;
  xbString  &ZapChar( char c );
  xbString  &ZapLeadingChar( char c );
  xbString  &ZapTrailingChar( char c );

  XBDLLEXPORT friend std::ostream& operator<< ( std::ostream& os, const xbString& s );

 private:

  static const char * NullString;
  static       char   cJunkBuf;

  char     *data;          // pointer to actual string data
  xbUInt32 size;           // size of string plus null terminating byte

  void     ctor(const char *s);
//  xbUInt32 CalcSprintfBufSize(const char *format, ...);

//  char *   xb_realloc( char *pIn, xbUInt32 iLen );

  // next routine could result in buffer over runs if used with improperly sized buffers
  char *   xb_strcpy ( char *target, const char *source);

};

}        /* namespace */
#endif   /* __XBSTRING_H__ */
