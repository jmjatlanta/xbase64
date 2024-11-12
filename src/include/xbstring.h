/*  xbstring.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023,2024 Gary A Kunkel

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


/*!
xbString is a basic string class with all the functionality one would expect in a string class.

For purposes of the xbString class, a string is defined as a variable sized array of one byte
characters terminated with a null (0x00 or \0) byte.<br>

This version of the xbString class does not support wide (wchar_t) characters at this time.<br>

This string class handle strings in a 1-based (not 0 based) fashion.
Any string routines taking an offset use a 1-based value. That is, the first position of 
the string is position 1, not 0.  Position 1 (not 0) is considered the first position in a string.
A return of 0 would indicate a not found condition. A return of 1, would be the
first byte.

*/


class XBDLLEXPORT xbString {

 public:
  ///@{
   /**
   @brief Constructor
   @param ulSize Size to allocate for a string.
   */
   xbString(xbUInt32 ulSize);

   /**
   @brief Constructor
   @param cIn Character value to set as a string.
   */
   xbString(char cIn);

   /**
   @brief Constructor
   @param sIn String value to set as a string.
   @param ulMaxLen Max Length for new string.
   */
   xbString(const char *sIn, xbUInt32 ulMaxLen);

   /**
   @brief Constructor
   @param sIn String value to set as a string.
   */
   xbString(const xbString &sIn);

   /**
   @brief Constructor
   @param sIn String value to set as a string.
   */
   xbString(const char *sIn = "");

   /**
   @brief Constructor
   @param dNum numeric valus to set as string.
   */
   xbString( xbDouble dNum );
  ///@}

  //! @brief Destructor.
  ~xbString();

  //! @brief Add a prefixing back slash to specified characters in the string.
  /*!
    @param c Character to prefix with a backslash.
    @returns Reference to this string.
  */
  xbString &AddBackSlash( char c );

  ///@{
   //! @brief Append data to string.
   /*!
     @param s String data to append.
     @returns Reference to this string.
   */
   xbString &Append(const xbString &s);

   //! @brief Append data to string.
   /*!
     @param s String data to append.
     @returns Reference to this string.
   */
   xbString &Append(const char *s);

   //! @brief Append data to string.
   /*!
     @param s String data to append.  
     @param ulByteCount Maximum number of bytes to append.
     @returns Reference to this string.
   */
   xbString &Append(const char *s, xbUInt32 ulByteCount );

   //! @brief Append data to string.
   /*!
     @param c char data to append.
     @returns Reference to this string.
   */
   xbString &Append(char c);
  ///@}

  ///@{
   //! @brief Assign portion of string.
   /*!
     @param sSrc - Null terminated source string for copy operation.
     @param ulStartPos - Starting position within source string.
     @param ulCopyLen - Length of data to copy.
     @returns Reference to this string.
   */
   xbString &Assign(const char *sSrc, xbUInt32 ulStartPos, xbUInt32 ulCopyLen );

   //! @brief Assign portion of string.
   /*!
     @param sSrc - Null terminated source string for copy operation.
     @param ulStartPos - Starting position within source string.
     @returns Reference to this string.
   */
   xbString &Assign(const char *sSrc, xbUInt32 ulStartPos );

   //! @brief Assign portion of string.
   /*!
     @param sSrc - Source xbString object for copy operation.
     @param ulStartPos - Starting position within source string.
     @param ulCopyLen - Length of data to copy.
     @returns Reference to this string.
   */
   xbString &Assign(const xbString &sSrc, xbUInt32 ulStartPos, xbUInt32 ulCopyLen );

   //! @brief Assign portion of string.
   /*!
     @param sSrc - Source xbString object for copy operation.
     @param ulStartPos - Starting position within source string.
     @returns Reference to this string.
   */
   xbString &Assign(const xbString &sSrc, xbUInt32 ulStartPos );
  ///@}

  //! @brief Copy a string
  /*!
     @returns xbString.
  */
  xbString  Copy() const;


  ///@{
   //! @brief Count the number of characters in the string.
   /*!
     @param c Character to count.
     @returns The number of characters.
   */
   xbUInt32  CountChar( char c ) const;

   //! @brief Count the number of characters in the string.
   /*!
     @param c Character to count.
     @param iOpt 0 - Count the number of characters.<br>
                 1 - Count the number of characters not between single or double quotes.
     @returns The number of characters.
   */
   xbUInt32  CountChar( char c, xbInt16 iOpt ) const;
  ///@}



  //! @brief Convert hex character to string. 
  /*!
     This routine converts a four byte string in the format of 0x00 to a one byte char value.
     The first four bytes of the string must be in the format 0x00.
     Anything past the first four bytes is disregarded.

     @param cOut Output character.
     @returns  XB_INVALID_PARM<br>
               XB_NO_ERROR
  */
  xbInt16   CvtHexChar( char &cOut );

  //! @brief Convert string of hex characters to string. 
  /*!
    This routine converts a string of one or more four byte sequences 0x00 to a string of one byte chars.
    @param sOut Output string of converted characters.
    @returns  XB_INVALID_PARM<br>
              XB_NO_ERROR.
  */
  xbInt16   CvtHexString( xbString &sOut );

  //! @brief Convert string to xbUInt64 number
  /*!
     @param ullOut - output xbUInt64 (unsigned long long) number.
     @returns XB_NO_ERROR
     @warning Current version has no data checking.
  */
  xbInt16   CvtULongLong( xbUInt64 &ullOut );

  //! @brief Convert string to xbInt64 number
  /*!
    @param llOut - output xbInt64 (long long) number.
    @returns XB_NO_ERROR
    @warning Current version has no data checking.
  */
  xbInt16   CvtLongLong( xbInt64 &llOut );



  #ifdef XB_DEBUG_SUPPORT
  ///@{
   //! @brief Dump string
   /*!
      @param sTitle Title to print in output.
      @note This method is only available if the XB_DEBUG_SUPPORT option is compiled into libarary.
   */
   void      Dump( const char *sTitle ) const;

   //! @brief Dump string
   /*!
      @param sTitle Title to print in output.
      @param bHexOption xbFalse Don't include hex output in dump.<br>xbTrue Include hex output in dump.
      @note This method is only available if the XB_DEBUG_SUPPORT option is compiled into libarary.
   */
   void      Dump( const char *sTitle, xbBool bHexOption ) const;

   //! @brief Dump string
   /*!
      @param sTitle Title to print in output.
      @note This method is only available if the XB_DEBUG_SUPPORT option is compiled into libarary.
   */
   void      Dump( const xbString &sTitle ) const;

   //! @brief Dump string
   /*!
      @param sTitle Title to print in output.
      @param bHexOption xbFalse Don't include hex output in dump.<br>xbTrue Include hex output in dump.
      @note This method is only available if the XB_DEBUG_SUPPORT option is compiled into libarary.
   */
   void      Dump( const xbString &sTitle, xbBool bHexOption ) const;
  ///@}

  ///@{
   //! @brief Dump Hex 
   /*!
      @param sTitle Dump hex values for a string.
      @note This method is only available if the XB_DEBUG_SUPPORT option is compiled into libarary.
   */
   void      DumpHex( const xbString & sTitle ) const;
   //! @brief Dump Hex 
   /*!
      @param sTitle Dump hex values for a string.
      @note This method is only available if the XB_DEBUG_SUPPORT option is compiled into libarary.
   */
   void      DumpHex( const char *sTitle ) const;
  ///@}
  #endif

  ///@{
   //! @brief Extract an element out of a delimited string.
   /*!
     @param sSrc Source string.
     @param cDelim Delimiter.
     @param lSkipCnt Number of delimiters to skip.
     @param iOpt 0 - ignore single and double quotes. (Default)<br>
                 1 - ignore delimiters between single or double quotes.
     @returns Reference to string extracted from element.
   */
   xbString  &ExtractElement(xbString &sSrc, char cDelim, xbUInt32 lSkipCnt, xbInt16 iOpt = 0 );

   //! @brief Extract an element out of a delimited string.
   /*!
      @param sSrc Source string.
      @param cDelim Delimiter.
      @param lSkipCnt Number of delimiters to skip.
      @param iOpt 0 - ignore single and double quotes. (Default)<br>
                  1 - ignore delimiters between single or double quotes.
      @returns Reference to string extracted from element.
   */
   xbString  &ExtractElement(const char *sSrc, char cDelim, xbUInt32 lSkipCnt, xbInt16 iOpt = 0 );

  //! @brief Get a character by position 
  /*!
     @param ulPos Position in string to extract. First position is 1 (not 0).
     @returns Character from position n, or null.
  */
  char GetCharacter( xbUInt32 ulPos ) const;

  ///@{
   //! @brief Get the position of the last occurrence of a given character.
   /*!
      @param c - Character to search for.
      @returns Last position of character in the string.
   */
   xbUInt32 GetLastPos(char c) const;
 
   //! @brief Get the position of the last occurrence of a given string.
   /*!
      @param s - String to search for.
      @returns Last position of character in the string.
   */
   xbUInt32  GetLastPos(const char *s) const;
  ///@}

  //! @brief Get the path separator out of the string.
  /*!
    This method assumes the string is a valid path name.
    If it is, it returns either / or \.
    @returns  /, \ or null.
  */
  char GetPathSeparator() const;

  //! @brief Retrieve the size of the string buffer.
  /*!
     @returns Size of string buffer including the null terminating byte.
  */
  xbUInt32  GetSize() const;

  //! @brief Determine if the string has any alpha characters
  /*!
     @returns xbTrue String contains one or more alpha characters.<br>
              xbFalse String contains no alpha characters.
  */
  xbBool    HasAlphaChars() const;

  //! @brief Determine if string is empty
  /*!
    @returns xbTrue if string is empty.<br>
            xbFalse if string is not empty.
  */
  xbBool    IsEmpty() const;

  //! @brief Determine if string is NULL
  /*!
    @returns xbTrue if string is NULL.<br>
             xbFalse if string is not NULL.
  */
  xbBool    IsNull() const;

  //! @brief Retain left part of string, drop rightmost characters.
  /*!
     @param ulLen New string length, truncate rightmost excess.
     @returns Reference to string.
  */
  xbString  &Left( xbUInt32 ulLen );

  //! @brief Retrieve length of current string object.
  /*!
     @returns String length, excluding the terminating null byte.
  */
  xbUInt32  Len() const;

  //! @brief Left trim white space from string. 
  /*!
     @returns Reference to this string.
  */
  xbString &Ltrim();

  //! @brief Left truncate string 
  /*!
     @param ulCnt Number of bytes to remove from the left.
     @returns Reference to this string.
  */
  xbString  &Ltrunc( xbUInt32 ulCnt );

  //! @brief Extract portion of data from string
  /*!
     @param ulStartPos Starting position
     @param ulTargLen Length to copy. If 0 copy everything after ulStartPos.
     @returns Reference to string

     @note This is a 1 based routine (first position in string is 1).
     @note If ulStartPos > string length, ulStartPos + TargLen > String Length or the string is null,
     the routine returns without making any updates.
  */
  xbString  &Mid(xbUInt32 ulStartPos, xbUInt32 ulTargLen = 0 );


  /*! @brief Operator const char *
     @returns Pointer to string data.
  */
  operator const char *() const;

  ///@{
   /**
     @brief Compare for inequality operator
     @param s - Compare string <em>s</em> to the string object.
     @returns xbTrue - Strings don't match.<br>xbFalse - Strings match.
   */
   xbBool operator != ( const xbString& s ) const;
   /**
     @brief Compare for inequality operator
     @param s - Compare string <em>s</em> to the string object.
     @returns xbTrue - Strings don't match.<br>xbFalse - Strings match.
   */
   xbBool operator != ( const char * s ) const;
  ///@}

  ///@{
   /**
     @brief Concatonate operator +<br>Concatonate left string with right string returning new string.
     @param s Right string operator.
     @returns Resultant string.
   */
   xbString  operator+( const char *s );

   /**
     @brief Concatonate operator +<br>Concatonate left string with char value returning new string.
     @param c Right string operator.
     @returns Resultant string.
   */
   xbString  operator+( const char c );

   /**
     @brief Concatonate operator +<br>Concatonate left string with right string returning new string.
     @param s Right string operator.
     @returns Resultant string.
    */
   xbString  operator+( const xbString &s );
  ///@}

  ///@{
   /**
     @brief Append operator +=
     @param c - Append char c to the string object.
   */
   xbString &operator+=(char c);

   /**
     @brief Append operator +=
     @param s - Append null terminated string s to the string object.
   */
   xbString &operator+=(const char *s);

   /**
     @brief Append operator +=
     @param s - Append xbString s to the string object.
   */
   xbString &operator+=(const xbString &s);
  ///@}

  //! @brief Concatonate operator -
  /*!
      Concatonate left string with right string returning new string.
      Both strings are trimmed.
      @param s Right string operator.
  */
  xbString  operator-( const xbString &s );

  ///@{
   /**
     @brief Append operator += Append to the right of the right trimmed string object.
     @param c Append string.
   */
   xbString &operator-=(char c);

   /**
     @brief Append operator += Append to the right of the string, right trimming both strings.
     @param s Append null terminated char string.
   */
   xbString &operator-=(const char *s);

   /**
     @brief Append operator += Append to the right of the string, right trimming both strings.
     @param s Append string.
   */
   xbString &operator-=(const xbString &s);
  ///@}


  //! @brief operator <
  /*!
    @param s String to compare 
    @returns xbTrue - Left string is less than the right string.<br>
         zbFalse - Left string is not less than the right string.<br>
  */
  xbBool operator <  ( const xbString& s ) const;

  //! @brief operator <=
  /*!
    @param s String to compare 
    @returns xbTrue Left string is less than or equal to the right string.<br>
         xbFalse Left string is not less than or equal to the right string.<br>
  */
  xbBool operator <= ( const xbString& s ) const;

  ///@{
   /**
    @brief operator =<br>Assignment operator
    @param s Null terminated char string.
   */
   xbString &operator= (const char *s );

   /**
    @brief operator =<br>Assignment operator
    @param s xBbString to assign.
   */
   xbString &operator= (const xbString &s );
  ///@}

  ///@{
   /**
    @brief Compare for equality operator
    @param s Null terminated char string <em>s</em>.
    @returns xbTrue Strings match.<br>xbFalse Strings don't match.
   */
   xbBool operator == ( const char * s ) const;

   /**
    @brief Compare for equality operator
    @param s xbString object <em>s</em>.
    @returns xbTrue Strings match.<br>xbFalse Strings don't match.
   */
   xbBool operator == ( const xbString& s ) const;
  ///@}

  /**
    @brief Operator ><br>Greater than compare.
    @param s xbString for compare operation.
    @returns xbTrue - Left string greater than right string.<br>xbFalse - Left string not greater than right string.
  */
  xbBool operator > ( const xbString& s ) const;

  /**
    @brief Operator >=<br>Greater than or equal compare.
    @param s xbString for compare operation.
    @returns xbTrue - Left string greater than or equal to right string.<br>xbFalse - Left string not greater than or equal to right string.
  */
  xbBool operator >= ( const xbString& s ) const;

  ///@{
   /**
     @brief Subscript operator[]<br>Return char value for position <em>iPos</em>.
     @param iPos 1 based offset for string character retrieval. Position 1 is the first byte in the string.
     @return Character at position <em>iPos</em>.
   */
   char &operator[](xbUInt32 iPos) const;

   /**
     @brief Subscript operator[]<br>Return char value for position <em>iPos</em>.
     @param iPos 1 based offset for string character retrieval.  Position 1 is the first byte in the string.
     @return Character at position <em>iPos</em>.
   */
    char &operator[](xbInt32 iPos) const;
  ///@}


  //! @brief Left pad string 
  /*!
    @param c Padding character.
    @param ulNewLen New string length.
    @returns Reference to this string.
  */
  xbString &PadLeft( char c, xbUInt32 ulNewLen );

  //! @brief Right pad string
  /*!
    @param c Padding character.
    @param ulNewLen New string length.
    @returns Reference to this string.
  */
  xbString  &PadRight( char c, xbUInt32 ulNewLen );


  ///@{
   /**
     @brief Determine position of a given character within a string object.
     @param c Seek character.
     @returns Position within string or 0 if not found.
   */
   xbUInt32  Pos(char c) const;

   /**
     @brief Determine position of a given character within a string object starting at position <em>ulStartPos</em>.
     @param c Seek character.
     @param ulStartPos starting position for search, first position is 1.
     @returns Position within string or 0 if not found.
   */
   xbUInt32  Pos(char c, xbUInt32 ulStartPos ) const;

   /**
     @brief Determine position of a given substring within a string object.
     @param s Substring to search for.
     @returns Position within string or 0 if not found.
   */
   xbUInt32  Pos(const char *s) const;
  ///@}

  //! @brief Insert character into string
  /*!
    @param ulPos Insertion position. First string position is 1.
    @param c Character to insert.
    @returns Reference to this string.
  */
  xbString  &PutAt(xbUInt32 ulPos, char c);

  //! @brief Remove portion of string.
  /*!
    @param ulStartPos Starting position for removal operation.
    @param ulDelSize Size of deletion.
    @returns Reference to string.
  */
  xbString  &Remove( xbUInt32 ulStartPos, xbUInt32 ulDelSize );

  ///@{
   /**
     @brief Replace a value within a string with another value
     @param sReplace - Character string to replace.
     @param sReplaceWith - Character string to replace with
     @param iOption 0 = All occurrences (default), 1 = first occurrence
     @returns Reference to this string.
   */
   xbString  &Replace( const char *sReplace, const char *sReplaceWith, xbInt16 iOption = 0 );
   xbString  &Replace( const xbString &sReplace, const xbString &sReplaceWith, xbInt16 iOption = 0 );
   xbString  &Replace( const char *sReplace, const xbString &sReplaceWith, xbInt16 iOption = 0 );
   xbString  &Replace( const xbString &sReplace, const char *sReplaceWith, xbInt16 iOption = 0 );
  ///@}

  //! @brief Resize a string
  /*!
    @param ulSize - New string size, including null termination byte.
    @returns Reference to this string.
  */
  xbString  &Resize( xbUInt32 ulSize );

  //! @brief Right trim the string by removing any white space,
  /*!
    @returns Reference to string.
  */
  xbString  &Rtrim();

  ///@{
   /**
    @brief Set the value of the string.
    @param s Value to set the string.
    @returns Reference to string.
    @note  This routine fails if you try to set the string to itself or some part of itself.
   */
   xbString  &Set( const char *s );

   /**
    @brief Set the value of the string.
    @param s Value to set the string.
    @param ulSize Max size of resultant string.
    @returns Reference to string.
    @note  This routine fails if you try to set the string to itself or some part of itself.
   */
   xbString  &Set( const char *s, xbUInt32 ulSize );
   xbString  &Set( const xbString &s );
  ///@}

  //! @brief Set the string to long integer numeric value. 
  /*!
    @param lNum Value to set the string
    @returns Reference to this string.
  */
  xbString  &SetNum( xbInt32 lNum );

  //! @brief Printf routine for formatting a string.
  /*!
    See documentation on the standard C printf function for format specifiers for your platform.<br><br>
    Example: <em>MyString.Sprintf( "a number [%d] and some text [%s]", 100, "my test text data" );</em>
    @param sFormat A format specifier
    @returns Reference to a formatted string
  */
  xbString &Sprintf(const char *sFormat, ...);


  //! @brief Return string data
  /*!
    @returns char * to string data or NULL if string is empty
  */
  const char *Str() const;

  //! @brief Copy all or part of string to character array
  /*!
    @param cDest pointer to destination buffer.
    @param n Number of bytes to copy.
    @returns char * to result
    @note It is the responsibility of the application program to verify the buffer is large enough to hold string contents.
  */
  char      *strncpy( char * cDest, xbUInt32 n ) const;

  //! @brief Swap characters
  /*!
    @param cFrom character to replace.
    @param cTo character to replace with.
    @returns Reference to this string.
  */
  xbString  &SwapChars( char cFrom, char cTo );

  //! @brief Replace all upper case characters with lower case characters
  /*!
    @returns Reference to this string.
  */
  xbString  &ToLowerCase();

  //! @brief Replace all lower case characters with upper case characters
  /*!
    @returns Reference to this string.
  */
  xbString  &ToUpperCase();

  //! @brief Trim all leading and trailing white space from string.
  /*!
    @returns Reference to string.
  */
  xbString  &Trim();

  //! @brief Check for valid logical field data
  /*!
    Valid logical data is one 'T', 'F', 'N' or 'Y'.<br>
    @returns xbTrue if the data is valid logical data.<br>
             xbFalse if not valid logical data.
  */
  xbBool    ValidLogicalValue() const;

  //!  @brief This function returns true if the data is valid numeric data
  /*!
    @returns xbTrue if valid numeric data.<br>
            xbFalse if not valid numeric daata.
  */
  xbBool    ValidNumericValue() const;

  //!  @brief Remove every instance of a character from a string.
  /*!
    @param c character to remove from string.
    @returns Reference to this string.
  */
  xbString  &ZapChar( char c );

  //!  @brief Remove leading character from a string.
  /*!
    @param c character to remove from beginning of string.
    @returns Reference to this string.
  */
  xbString  &ZapLeadingChar( char c );

  //!  @brief Remove trailing character from a string.
  /*!
    @param c character to remove from end of string.
    @returns Reference to this string.
  */
  xbString  &ZapTrailingChar( char c );

  //! @brief Stream insertion operator <<
  /*!
    std::cout << MyString << std::endl;
    @param os Output stream
    @param s String to send to output stream
  */

  XBDLLEXPORT friend std::ostream& operator<< ( std::ostream& os, const xbString& s );

 private:

  static const char     *NullString;
  static       char     cNullByte;

               char     *data;          // pointer to actual string data
               xbUInt32 size;           // size of string plus null terminating byte

  void     ctor(const char *s);

  // next routine could result in buffer over runs if used with improperly sized buffers
  char *   xb_strcpy ( char *target, const char *source);

};

}        /* namespace */
#endif   /* __XBSTRING_H__ */
