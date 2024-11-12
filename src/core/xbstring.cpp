/* xbstring.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2021,2022,2023,2024 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

//#ifdef __GNU LesserG__
//  #pragma implementation "xbstring.h"
//#endif

#include "xbase.h"


namespace xb{

XBDLLEXPORT const char * xbString::NullString = "";
XBDLLEXPORT       char   xbString::cNullByte  = 0x00;

/************************************************************************/
xbString::xbString(xbUInt32 ulSize) {
  data = (char *)calloc(1, ulSize);
  this->size = ulSize;
}
/************************************************************************/
xbString::xbString(char c) {  
  data = (char *)calloc(1, 2);
  data[0] = c;
  data[1] = 0;
  size = 2;
}
/************************************************************************/
xbString::xbString( const char *s ) {

  if( s == NULL ){
    size = 0;
    data = NULL;
  } else {
    size = (xbUInt32) (strlen(s) + 1 );
    data = (char *) calloc( 1, size );
    xb_strcpy( data, s );
  }
}
/************************************************************************/
xbString::xbString( xbDouble d ) {
  data = NULL;
  size = 0;
  Sprintf("%f", d);
}
/************************************************************************/
xbString::xbString( const char *s, xbUInt32 ulMaxLen ) {
  xbUInt32 sSize = (xbUInt32) strlen( s );
  if( sSize < ulMaxLen )
    size = sSize;
  else
    size = ulMaxLen;
  data = (char *)calloc(1, size+1);
  for( xbUInt32 i = 0; i < size; i++ )
    data[i] = s[i];
  data[size] = '\0';
  size++;    // account for null trailing byte
  return;
}
/************************************************************************/
xbString::xbString( const xbString &s ) {
  ctor(s.Str());
}
/************************************************************************/
xbString::~xbString(){
  if (data != NULL)
    free(data);
}
/************************************************************************/
xbString &xbString::AddBackSlash( char c ) {

  xbUInt32 lCnt = CountChar( c );
  if( lCnt == 0 )
    return *this;
  char *p;
  if(( p = (char *)calloc( 1, size + lCnt )) == NULL )
    return *this;

  char *p2 = p;
  for( xbUInt32 lS = 0; lS < size; lS++ ){
    if( data[lS] == c )
      *p2++ = '\\';
    *p2++ = data[lS];
  }
  if( data )
    free( data );
  data = p;

  //  size += lCnt;
  size == 0 ? size += (lCnt+1) : size += lCnt;

  return *this;
}
/************************************************************************/
xbString &xbString::Append( const xbString &s ) {
 *this += s;
  return *this;
}
/************************************************************************/
xbString &xbString::Append( const char *s ) {
  *this += s;
  return *this;
}
/************************************************************************/
xbString &xbString::Append( char c ) {
  *this += c;
  return *this;
}
/************************************************************************/
xbString &xbString::Append( const char *s, xbUInt32 ulByteCount ) {

  if ( s == NULL || !*s || ulByteCount == 0)
    return (*this);

  xbUInt32 ulOrigLen = this->Len();

  // s might not be null byte at the end, can't use strlen
  //  xbUInt32 ulAddLen = strlen( s );
  xbUInt32 ulAddLen = 0;
  const char *p = s;

  while( ulAddLen < ulByteCount && *p ){
    p++;
    ulAddLen++;
  }

  if( ulAddLen > ulByteCount )
    ulAddLen = ulByteCount;

  size = ulOrigLen + ulAddLen + 1;
  data = (char *) realloc( data, size );

  for( xbUInt32 i = 0; i < ulAddLen; i++ )
    data[i+ulOrigLen] = s[i];

  data[size-1] = 0x00;
  return (*this);
}

/************************************************************************/
xbString &xbString::Assign(const char * sStr, xbUInt32 ulStartPos, xbUInt32 ulCopyLen){
  if(data){
    free(data);
    data = 0;
    size = 0;
  }
  xbUInt32 lLen = (xbUInt32) strlen( sStr );
  if( ulStartPos > lLen ){
    size = 0;
    return( *this );
  }
  if((( ulCopyLen - 1) + ulStartPos ) > lLen )
    ulCopyLen = lLen - ulStartPos + 1;
  data = (char *)calloc(1, ulCopyLen + 1);

  //size = ulCopyLen + 1;
  size == 0 ? size += (ulCopyLen+1) : size += ulCopyLen;

  for( xbUInt32 i = 0; i < ulCopyLen; i++ )
    data[i] = sStr[i + ulStartPos - ((xbUInt32) 1)];
  data[ulCopyLen] = '\0';
  return (*this);
}
/************************************************************************/
xbString &xbString::Assign(const char * sStr, xbUInt32 ulStartPos){
  if(data){
    free(data);
    data = 0;
    size = 0;
  }
  xbUInt32 ulSrcLen = (xbUInt32) strlen( sStr );
  if( ulStartPos > ulSrcLen ){
    size = 0;
    return( *this );
  }
  xbUInt32 ulCopyLen;
  ulCopyLen = ulSrcLen - ulStartPos + 1;
  data = (char *)calloc(1, ulCopyLen + 1);

  size = ulCopyLen + 1;

  for( xbUInt32 i = 0; i < ulCopyLen; i++ )
    data[i] = sStr[i + ulStartPos - ((xbUInt32) 1)];
  data[ulCopyLen] = '\0';
  return (*this);
}

/************************************************************************/
xbString &xbString::Assign(const xbString& sStr, xbUInt32 ulStartPos, xbUInt32 ulCopyLen){
 if(data){
    free(data);
    data = 0;
    size = 0;
  }
  xbUInt32 ulSrcLen = sStr.Len();
  if( ulStartPos > ulSrcLen ){
    size = 0;
    return( *this );
  }
  if((( ulCopyLen - 1) + ulStartPos ) > ulSrcLen )
    ulCopyLen = ulSrcLen - ulStartPos + 1;  
  data = (char *)calloc(1, ulCopyLen + 1);
  size = ulCopyLen + 1;
  for( xbUInt32 i = 0; i < ulCopyLen; i++ )
    data[i] = sStr[i + ulStartPos];
  data[ulCopyLen] = '\0';
  return (*this);
}
/************************************************************************/
xbString &xbString::Assign(const xbString& sStr, xbUInt32 ulStartPos){
  if(data){
    free(data);
    data = 0;
    size = 0;
  }
  xbUInt32 ulSrcLen = sStr.Len();
  if( ulStartPos > ulSrcLen ){
    size = 0;
    return( *this );
  }
  xbUInt32 ulCopyLen;
  ulCopyLen = ulSrcLen - ulStartPos + 1;
  data = (char *)calloc(1, ulCopyLen + 1);
  size = ulCopyLen;
  for( xbUInt32 i = 0; i < ulCopyLen; i++ )
    data[i] = sStr[i + ulStartPos];
  data[ulCopyLen] = '\0';
  size++;
  return (*this);
}


/************************************************************************/
xbString xbString::Copy() const {
  return( *this );
}
/************************************************************************/
xbUInt32 xbString::CountChar( char c, xbInt16 iOpt ) const {
  if( iOpt == 0 )
    return CountChar( c );
  else{
    xbBool bSingleQuote = xbFalse;
    xbBool bDoubleQuote = xbFalse;
    char cPrevChar = 0x00;
    xbUInt32 i,j;
    for( i = 0, j = 0; i < size; i++ ){
      if( bSingleQuote && data[i] == '\'' && cPrevChar != '\\' ){
        bSingleQuote = xbFalse;
      }
      else if( bDoubleQuote && data[i] == '"' && cPrevChar != '\\' ){
        bDoubleQuote = xbFalse;
      }
      else if( data[i] == '\'' && cPrevChar != '\\' && !bDoubleQuote ){
        bSingleQuote = xbTrue;
      }
      else if( data[i] == '"' && cPrevChar != '\\' && !bSingleQuote ){
        bDoubleQuote = xbTrue;
      }
      else if( !bDoubleQuote && !bSingleQuote && data[i] == c ){
        j++;
      }
      cPrevChar = data[i];
    }
    return j;
  }
}
/************************************************************************/
xbUInt32 xbString::CountChar( char c ) const {
  xbUInt32 i,j;
  for( i = 0,j = 0; i < size; i++ )
    if( data[i] == c )
      j++;
  return j;
}
/************************************************************************/
void xbString::ctor( const char *s ) {

  // this routine assumes it was called by one of the constructors.
  if (s == NULL) {
    data = NULL;
    size = 0;
    return;
  }

  size = (xbUInt32) (strlen(s) + 1);
  data = (char *) calloc( 1, size);

  xb_strcpy(data, s);
}
/************************************************************************/
xbInt16 xbString::CvtHexChar( char &cOut ){

  int  j, k;
  char c;

  if( Len() < 4 || data[0] != '0' || (data[1]!='X' && data[1]!='x' ))
    return XB_INVALID_PARM;

  c = (char) toupper( data[2] );
  j = ( c > '9' ? c - 'A' + 10 : c - '0' );
  c = (char)toupper( data[3] );
  k = ( c > '9' ? c - 'A' + 10 : c - '0' );
  j = ( j << 4 ) + k;

  cOut = ( char ) j;
  return XB_NO_ERROR;
}
/************************************************************************/
xbInt16 xbString::CvtHexString( xbString &sOut ){
  char c;
  xbString ws;
  ws = data;
  sOut = "";
  xbInt16 iRc;
  while( ws.Len()){
    if(( iRc= ws.CvtHexChar( c )) != XB_NO_ERROR )
      return iRc;
    sOut += c;
    ws.Ltrunc( 4 );
  }
  return XB_NO_ERROR;
}
/************************************************************************/
xbInt16 xbString::CvtULongLong( xbUInt64 &ullOut ){

  // version 1 - fast, but no data checking
  ullOut = 0;
  char *s = data;
  int i = 0;
  while( *s ){
    ullOut *= 10;
    ullOut += (xbUInt64) *s - '0';
    s++;
    i++;
  }
  return XB_NO_ERROR;
}
/************************************************************************/
xbInt16 xbString::CvtLongLong( xbInt64 &llOut ){

  // version 1 - fast, but no data checking
  llOut = 0;
  char *s = data;
  int i = 0;
  while( *s ){
    llOut *= 10;
    llOut += (xbInt64) *s - '0';
    s++;
    i++;
  }
  return XB_NO_ERROR;
}
/************************************************************************/
#ifdef XB_DEBUG_SUPPORT
void xbString::Dump( const char * sTitle ) const {
  Dump( sTitle, 0 );
}
void xbString::Dump( const char * sTitle, xbBool bHexOption ) const {
  fprintf(stdout, "%s StringSize[%d] DataLen=[%d] data=[%s]\n", sTitle, size, Len(), data );
  if( bHexOption ){
    std::cout << "Hex values" << std::endl;
    for( xbUInt32 i = 0; i < strlen( data ); i++ )
      printf( " %x", data[i] );
    std::cout << std::endl;
  }
}
void xbString::Dump( const xbString &sTitle ) const {
  Dump( sTitle.Str(), 0 );
}
void xbString::Dump( const xbString &sTitle, xbInt16 bHexOption ) const {
  Dump( sTitle.Str(), bHexOption );
}
void xbString::DumpHex( const xbString &sTitle ) const {
  Dump( sTitle.Str(), 1 );
}
void xbString::DumpHex( const char * sTitle ) const {
  Dump( sTitle, 1 );
}
#endif

/************************************************************************/
xbString &xbString::ExtractElement( xbString &sSrc, char cDelim, xbUInt32 lSkipCnt, xbInt16 iOpt )
{
  return ExtractElement( sSrc.Str(), cDelim, lSkipCnt, iOpt );
}

/************************************************************************/
xbString &xbString::ExtractElement( const char *sSrc, char cDelim, xbUInt32 lSkipCnt, xbInt16 iOpt )
{
  /* opt values
     0 - ignore single and double quotes
     1 - ignore delimiters between single or double quotes
  */

  xbUInt32 lLen;
  xbUInt32 lCurCnt = 0;
  xbBool bInSingleQuotes = xbFalse;
  xbBool bInDoubleQuotes = xbFalse;
  char cPrevChar = 0x00;
  const char *s = sSrc;
  const char *pAnchor;

  /* skip past skipcnt delimiters */
  while( *s && lCurCnt < (lSkipCnt-1) ){
    if( iOpt == 0 ){
      if( *s == cDelim ) 
        lCurCnt++;
    } else {
      if( *s == cDelim && !bInSingleQuotes && !bInDoubleQuotes ){
        lCurCnt++;
      } else if( *s == '\'' && !bInDoubleQuotes && cPrevChar != '\\' ){
         if( bInSingleQuotes == xbTrue ) 
           bInSingleQuotes = xbFalse; 
         else 
           bInSingleQuotes = xbTrue;
      } else if( *s == '"' && !bInSingleQuotes && cPrevChar != '\\' ){
         if( bInDoubleQuotes == xbTrue ) 
           bInDoubleQuotes = xbFalse; 
         else 
           bInDoubleQuotes = xbTrue;
      }
    }
    cPrevChar = *s;
    s++;
  }

  /* at the beginning of the field */
  pAnchor = s;
  xbBool bDone = xbFalse;
  while( *s && !bDone ){
    if( iOpt == 0 ){
      if( *s == cDelim )
        bDone = xbTrue;
    } else {
      if( *s == cDelim && !bInSingleQuotes && !bInDoubleQuotes ){
        bDone = xbTrue;
      } else if( *s == '\'' && !bInDoubleQuotes && cPrevChar != '\\' ){
        if( bInSingleQuotes == xbTrue ) 
          bInSingleQuotes = xbFalse;
        else 
          bInSingleQuotes = xbTrue;
      } else if( *s == '"' && !bInSingleQuotes && cPrevChar != '\\' ){
        if( bInDoubleQuotes == xbTrue ) 
          bInDoubleQuotes = xbFalse; 
        else 
          bInDoubleQuotes = xbTrue;
      }
    }
    cPrevChar = *s;
    s++;
  }

  // if at end of string, go back one and drop the delimiter
  if( *s ) s--;

  lLen = (xbUInt32)(s - pAnchor);

  /* copy data */
  data = (char *) realloc( data, lLen+1 );
  memcpy( data, pAnchor, lLen );
  data[lLen] = 0;
  this->size = lLen+1;
  return *this;
}
/************************************************************************/
char xbString::GetCharacter( xbUInt32 n ) const {
  if( n > 0 && n <= size )
    return data[n-1];
  else
    return 0x00;
}
/************************************************************************/
xbUInt32 xbString::GetLastPos(char c) const {

  if (data == NULL)
    return 0;

  char *p = data;
  xbUInt32 iPos = 0;
  xbUInt32 hPos = 0;
  while( *p && iPos++ < ( size - 1 )){
    if( *p == c )
      hPos = iPos;
    p++;
  }
  if( hPos )
    return hPos;
  else
    return 0;
}
/************************************************************************/
xbUInt32 xbString::GetLastPos(const char* s) const{

  if (data == NULL)
    return 0;

  char *p     = data;
  char *saveP = NULL;
  while( p ){
    p = strstr( p, s);
    if( p ){
      saveP = p;
      p++;
    }
  }
  if (saveP == NULL)
    return 0;
  return (xbUInt32)(saveP - data) + 1;
}
/************************************************************************/
char xbString::GetPathSeparator() const {

  if (data == NULL)
    return 0x00;
  char *p = data;
  while( *p ){
    if( *p == '\\' || *p == '/' )
      return *p;
    else
      p++;
  }
  return 0x00;
}
/************************************************************************/
xbUInt32 xbString::GetSize() const {
  return size;
}
/************************************************************************/
xbBool xbString::HasAlphaChars() const {
  for( xbUInt32 i = 0; i < size; i++ )
    if( isalpha( data[i] ))
      return xbTrue;
  return xbFalse;
}
/************************************************************************/
xbBool xbString::IsEmpty() const {
  if( data == NULL )
    return true;
  if( data[0] == 0 )
    return xbTrue;
  return xbFalse;
}
/************************************************************************/
xbBool xbString::IsNull() const {
  return( data == NULL );
}
/************************************************************************/
xbString &xbString::Left( xbUInt32 ulLen ) {
  if( ulLen == 0 )
    return Set( "" );
  else
    return Mid( 1, ulLen );
}
/************************************************************************/
xbUInt32 xbString::Len() const {
  return( data ? (xbUInt32) strlen(data) : 0 );
}
/************************************************************************/
xbString &xbString::Ltrim(){
  if( !data )
    return *this;
  char *p = data;
  if( !*p || (*p && *p != ' ') )
    return *this;   /* nothing to do */
  xbUInt32 s = 0;
  while( *p && *p == ' ' ){
    p++;
    s++;
    size--;
  }
  xbUInt32 i;
  for( i = 0; i < size; i++ )
    data[i] = data[i+s];
  data[i] = 0x00;
  data = (char *) realloc( data, size );
  return *this;
}
/************************************************************************/
xbString &xbString::Ltrunc( xbUInt32 ulCnt ){
  if( ulCnt >= size ){
    if( size > 0 ){
      free( data );
      data = NULL;
      size = 0;
    }
    return *this;
  }
  char * ndata;
  char * p;
  ndata = (char *) calloc( 1, size - ulCnt );
  p = data;
  p += ulCnt;
  xb_strcpy( ndata, p );
  free( data );
  data = ndata;
  size = size - ulCnt;
  return *this;
}
/************************************************************************/
xbString &xbString::Mid( xbUInt32 ulStartPos, xbUInt32 ulTargLen ){
  if( ulStartPos == 0 )
    return *this;
  if( data == NULL )
    return( *this );
  if( data[0] == 0 )
    return( *this );
  if( ulStartPos > Len() )
    return( *this );
  if( ulTargLen == 0 || ((ulTargLen + ulStartPos) > Len()) )
    ulTargLen = Len() - ulStartPos + 1;
  char * newData = (char *) calloc( 1, ulTargLen + 1 );
  char *pTarg = newData;
  char *pSrc = data + ulStartPos - 1;
  for( xbUInt32 l = 0; l < ulTargLen; l++ )
    *pTarg++ = *pSrc++;
  *pTarg = 0x00;

  free( data );
  data = newData;
  size = ulTargLen + 1;
  return *this;
}
/************************************************************************/
xbString::operator const char *() const {
  return data ? data : NullString;
}
/************************************************************************/
//! @brief operator !=
/*!
    @param s String to compare 
    @returns xbTrue - Strings don't match.<br>
             xbFalse - Strings match.<br>
*/
xbBool xbString::operator!=( const xbString &s ) const {
  if( data == NULL || data[0] == 0 ) {
    if( s.data == NULL || s.data[0] == 0 )
      return xbFalse;                                           // NULL != NULL
    return xbTrue;                                              // NULL != !NULL
  } else {
    if( s.data == NULL || s.data[0] == 0 )
      return xbTrue;                                            // !NULL != NULL
    return( strcmp( data, s.data ) != 0 ? xbTrue : xbFalse );   // !NULL != !NULL
  }
}
/************************************************************************/
//! @brief operator !=
/*!
    @param s String to compare 
    @returns xbTrue - Strings don't match.<br>
         zbFalse - Strings match.<br>
*/
xbBool xbString::operator!=( const char *s ) const {
  if( s == NULL || s[0] == 0 ) {
    if( data == NULL || data[0] == 0 )
      return xbFalse;                                      // NULL != NULL
    return xbTrue;                                         // NULL != !NULL
  } else {
    if( s == NULL || s[0] == 0 )
      return xbTrue;                                       // !NULL != NULL
    return( strcmp( data, s ) != 0 ? xbTrue : xbFalse );   // !NULL != !NULL
  }
}
/************************************************************************/
xbString xbString::operator+( const char *s1) {
  xbString tmp( data );
  tmp += s1;
  return tmp;
}
/************************************************************************/
xbString xbString::operator+( const char c) {
  xbString tmp( data );
  tmp += c;
  return tmp;
}
/************************************************************************/
xbString xbString::operator+( const xbString &s1) {
  xbString tmp( data );
  tmp += s1;
  return tmp;
}

/************************************************************************/
xbString &xbString::operator+=( char c ) {
  xbUInt32 Len = 1;
  xbUInt32 oldLen = this->Len();
  data = (char *)realloc(data, oldLen+Len+1);
  data[oldLen] = c;
  data[oldLen+1] = 0;
  // size++;
  size == 0 ? size += 2 : size++;
  return (*this);
}
/************************************************************************/
xbString &xbString::operator+=( const char *s ) {
  if (s == NULL)
    return (*this);
  xbUInt32 Len = (xbUInt32) strlen(s);
  xbUInt32 oldLen = this->Len();
  xbUInt32 newLen = Len + oldLen;
  data = (char *)realloc(data, newLen+1);
  if(oldLen == 0)
    data[0] = 0;
  for( xbUInt32 i = 0; i < Len; i++ )
    data[i+oldLen] = s[i];
  data[newLen] = '\0';
  // size += Len;
  size == 0 ? size+= (Len + 1) : size += Len;
  return (*this);
}
/************************************************************************/
xbString &xbString::operator+=( const xbString &s ) {
  if (s.IsNull())
    return (*this);
  xbUInt32 Len = s.Len();
  xbUInt32 oldLen = this->Len();
  xbUInt32 newLen = Len + oldLen;
  data = (char *)realloc(data, newLen+1);
  if( !data )
    return (*this);
  if(oldLen == 0)
    data[0] = 0;
  char *t = data;
  t+= oldLen;
  for( xbUInt32 i = 0; i < Len; i++ )
    *t++ = s.GetCharacter(i+1);
  data[newLen] = '\0';
  size == 0 ? size += (Len + 1) : size += Len;
  return (*this);
}
/************************************************************************/
xbString xbString::operator-(const xbString &s1) {
  xbString tmp( data );
  tmp -= s1;
  return tmp;
}
/************************************************************************/
xbString &xbString::operator-=(const char c) {
  Rtrim();
  xbUInt32 oldSize = size;
  size == 0 ? size += 2 : size += 1;
  data = (char *)realloc( data, size );
  if( oldSize == 0 ) data[0] = 0;
  data[size-2] = c;
  data[size-1] = 0;
  Trim(); 
  return (*this);
}
/************************************************************************/
xbString &xbString::operator-=(const char *s) {
  Rtrim();
  if (s == NULL)
    return (*this);
  xbUInt32 Len = (xbUInt32) strlen(s);
  xbUInt32 oldLen = this->Len();
  xbUInt32 newLen = Len + oldLen;
  data = (char *)realloc(data, newLen+1);
  if(oldLen == 0)
    data[0] = 0;
  for( xbUInt32 i = 0; i < Len; i++ )
    data[i+oldLen] = s[i];
  data[newLen] = '\0';
  size == 0 ? size += (Len+1) : size += Len;
  Rtrim(); 
  return (*this);
}
/************************************************************************/
xbString &xbString::operator-=( const xbString &s ) {
  Rtrim();
  if (s.IsNull())
    return (*this);
  xbUInt32 Len = s.Len();
  xbUInt32 oldLen = this->Len();
  xbUInt32 newLen = Len + oldLen;
  data = (char *)realloc(data, newLen+1);
  if(oldLen == 0)
    data[0] = 0;
  for( xbUInt32 i = 0; i < Len; i++ )
    data[i+oldLen] = s.GetCharacter(i+1);
  data[newLen] = '\0';
  size == 0 ? size += (Len+1) : size += Len;
  Rtrim();
  return (*this);
}
/************************************************************************/
xbBool xbString::operator< ( const xbString &s ) const {
  if( data == NULL || data[0] == 0 ) {
    if( s.data == NULL || s.data[0] == 0 )
      return false;
    return true;
  } else {
    if( s.data == NULL || s.data[0] == 0 )
      return false;
    return ( strcmp(data, s.data) < 0 ? xbTrue : xbFalse );
  }
}
/************************************************************************/
xbBool xbString::operator<=( const xbString &s ) const {
  if( data == NULL || data[0] == 0 ) {
    if( s.data == NULL || s.data[0] == 0 )
      return true;
    return true;
  } else {
    if( s.data == NULL || s.data[0] == 0 )
      return false;
    return( strcmp(data,s.data) <= 0 ? xbTrue : xbFalse );
  }
}
/************************************************************************/
xbString &xbString::operator=( const char *s ) {
  return Set(s);
}
/************************************************************************/
xbString &xbString::operator=( const xbString &s ) {
  return Set(s);
}
/************************************************************************/
xbBool xbString::operator==( const char *s ) const {

  if (s == NULL) {
    if ( data == NULL)
      return true;
    return false;
  }
  if ((s[0] == 0) && data == NULL)
      return true;
  if ( data == NULL)
    return false;
  return( strcmp( data, s) == 0 ? xbTrue : xbFalse );
}
/************************************************************************/
xbBool xbString::operator==( const xbString &s ) const {

  if( data == NULL || data[0] == 0 ) {
    if( s.data == NULL || s.data[0] == 0 )
      return true;
    return false;
  } else {
    if( s.data == NULL || s.data[0] == 0 )
      return false;
    return( strcmp(data,s.data) == 0 ? xbTrue : xbFalse );
  }
}
/************************************************************************/
xbBool xbString::operator> ( const xbString &s ) const {
  if( data == NULL || data[0] == 0 ) {
    if( s.data == NULL || s.data[0] == 0 )
      return false;
    return false;
  } else {
    if( s.data == NULL || s.data[0] == 0 )
      return true;
    return( strcmp(data,s.data) > 0 ? xbTrue : xbFalse );
  }
}
/************************************************************************/
xbBool xbString::operator>=( const xbString &s ) const {
  if( data == NULL || data[0] == 0 ) {
    if( s.data == NULL || s.data[0] == 0 )
      return true;
    return false;
  } else {
    if( s.data == NULL || s.data[0] == 0 )
      return true;
    return( strcmp(data, s.data) >= 0 ? xbTrue : xbFalse );
  }
}
/************************************************************************/
char &xbString::operator[]( xbUInt32 n ) const {
//char xbString::operator[]( xbUInt32 n ) const {
  if( n > 0 && n <= size )
    return data[n-1];
  else
    return cNullByte;
}
/************************************************************************/
char &xbString::operator[]( xbInt32 n ) const {
  if( n > 0 && n <= (xbInt32) size )
    return data[n-1];
  else
    return cNullByte;
}
/************************************************************************/
xbString &xbString::PadLeft( char c, xbUInt32 ulNewLen ){

  xbUInt32 srcLen;
  if( data )
    srcLen = (xbUInt32) strlen( data );
  else
    srcLen = 0;
  if( srcLen >= ulNewLen )
    return *this;
  char * newData = (char *) calloc( 1, ulNewLen + 1 );
  xbUInt32 i;
  for( i = 0; i < ulNewLen - srcLen; i++ )
    newData[i] = c;
  char *targ = &newData[i];
  xb_strcpy( targ, data );
  free( data );
  data = newData;
  size = ulNewLen + 1;
  return *this;
}
/************************************************************************/
xbString &xbString::PadRight( char c, xbUInt32 ulNewLen ){
  xbUInt32 srcLen = (xbUInt32) strlen( data );
  if( srcLen >= ulNewLen )
    return *this;
  data = (char *) realloc( data, ulNewLen + 1 );
  xbUInt32 i;
  for( i = srcLen; i < ulNewLen; i++ )
    data[i] = c;
  data[i] = 0x00;
  size = ulNewLen + 1;
  return *this;
}
/************************************************************************/
xbUInt32 xbString::Pos(char c) const {

  if (data == NULL)
    return 0;
  char *p = data;
  xbUInt32 iPos = 0;
  int  iFound = 0;
  while( *p && !iFound && iPos < ( size - 1 )){
    if( *p == c )
      iFound = 1;
    else {
      iPos++;
      p++;
    }
  }
  if( iFound )
    return iPos + 1;
  else
    return 0;
}
/************************************************************************/
xbUInt32 xbString::Pos(char c, xbUInt32 ulStartPos ) const {
  if (data == NULL)
    return 0;
  char *p = data;
  if( ulStartPos >= size )
    return 0;
  xbUInt32 iPos = 0;
  while( (iPos+1) < ulStartPos ){
    p++;
    iPos++;
  }
  xbBool bFound = 0;
  while( *p && !bFound && iPos < ( size - 1 )){
    if( *p == c )
      bFound = 1;
    else {
      iPos++;
      p++;
    }
  }
  if( bFound )
    return iPos + 1;
  else
    return 0;
}
/************************************************************************/
xbUInt32 xbString::Pos(const char* s) const{

  if (data == NULL)
    return 0;

  char *p = strstr(data, s);
  if ( p == NULL)
    return 0;

  return (xbUInt32)(p - data + 1);
}
/************************************************************************/
xbString &xbString::PutAt(xbUInt32 ulPos, char c){
   if((ulPos-1) > Len() )
      return *this;
   data[ulPos-1] = c;
   return *this;
}
/************************************************************************/
xbString &xbString::Remove(xbUInt32 ulStartPos, xbUInt32 ulDelSize ) {
  if( data == NULL )
    return( *this );
  if( data[0] == 0 )
    return( *this );
  xbUInt32 srcLen = Len();
  if( ulStartPos > srcLen || ulStartPos < 1 || ulDelSize < 1 )
    return( *this );
  if(( ulStartPos + ulDelSize - 1) >= size ){
    data[ulStartPos-1] = 0x00;
    size = ulStartPos;
    return( *this );
  }
  char *t;
  char *s;
  t = data + (ulStartPos - 1);
  s = t + ulDelSize;
  size -= ulDelSize;
  while( *s )
    *t++ = *s++;
  *t = 0x00;
  return( *this );
}
/************************************************************************/
xbString &xbString::Replace( const char *sReplace, const char *sReplaceWith, xbInt16 iOption ){

  xbBool bDone = xbFalse;
  xbUInt32 ulPos;
  xbUInt32 ulNewLen;
  xbUInt32 ulReplaceWithLen;
  xbUInt32 ulRsLen;    // size of right side of string after replaced data
  xbUInt32 ulSp2;
  char *sBuf2;

  const char *s;        // source ptr
  char *t;              // target ptr

  while( !bDone ){
    ulPos = Pos( sReplace );
    if( ulPos == 0 ){
      bDone = xbTrue;
    } else {

      ulReplaceWithLen = (xbUInt32) strlen( sReplaceWith );
      ulNewLen = this->size + ulReplaceWithLen - (xbUInt32) strlen( sReplace );
      sBuf2 = (char *) calloc( 1, ulNewLen );

      // copy part1
      t = sBuf2;
      s = data;
      for( xbUInt32 ul = 0; ul < ulPos-1; ul++ )
        *t++ = *s++;

      // copy part2
      s = sReplaceWith;
      for( xbUInt32 ul = 0; ul < ulReplaceWithLen; ul++ )
        *t++ = *s++;

      // copy part3
      ulSp2 = ulPos + (xbUInt32) strlen( sReplace );
      s = data;
      s+= (ulSp2 - 1);
      ulRsLen = (xbUInt32) strlen( s );
      for( xbUInt32 ul = 0; ul < ulRsLen; ul++ )
        *t++ = *s++;

      if( iOption )
        bDone = xbTrue;

      free(data);
      data = sBuf2;
      size = ulNewLen;
    }
  }
  return *this;
}

xbString  &xbString::Replace( const char *sReplace, const xbString &sReplaceWith, xbInt16 iOption ){
  return Replace( sReplace, sReplaceWith.Str(), iOption );
}

xbString  &xbString::Replace( const xbString &sReplace, const char *sReplaceWith, xbInt16 iOption ){
  return Replace( sReplace.Str(), sReplaceWith, iOption );
}

xbString  &xbString::Replace( const xbString &sReplace, const xbString &sReplaceWith, xbInt16 iOption ){
  return Replace( sReplace.Str(), sReplaceWith.Str(), iOption );
}
/************************************************************************/
xbString &xbString::Resize(xbUInt32 ulSize) {

  data = (char *) realloc( data, ulSize );

  if( ulSize > 0 )
    data[ulSize-1] = 0;
  this->size = ulSize;
  return *this;
}
/************************************************************************/
xbString &xbString::Rtrim(){
  xbUInt32 l = Len();
  if( l == 0 )
    return *this;
  xbUInt32 ulOrigSize = size;
  l--;
  for(;;) {
    if( data[l] != ' ' )
      break;
    data[l] = 0;
    size--;
    if( l == 0 )
      break;
    l--;
  }
  if( ulOrigSize != size )
    data = (char * ) realloc( data, size );
  return *this;
}
/************************************************************************/
xbString &xbString::Set( const char *s ) {
  if(data != NULL){
    free(data);
    data = NULL;
  }
  if( s == NULL || !*s ) {
    if( data )
      free( data );
    data = NULL;
    size = 0;
  } else {
    //data = (char *)calloc(1, strlen(s) + 1 );
    data = (char *) realloc( data, strlen(s) + 1 );
    xb_strcpy(data, s);
    size = (xbUInt32) (strlen(data) + 1);
  }
  return (*this);
}
/************************************************************************/
xbString &xbString::Set( const xbString &s ) {
  if( s.Str() == NULL ){
    if( data ) free( data );
    data = NULL;
    size = 0;
  } else {
    xbUInt32 ulLen = s.Len();
    char *p = (char *) calloc( 1, ulLen + 1 );
    xb_strcpy( p, s.Str());
    size = ulLen + 1;
    if( data ) free( data );
    data = p;
  }
  return (*this );
}
/************************************************************************/
xbString &xbString::Set(const char *s, xbUInt32 ulSize) {
  if( data != NULL )
    free( data );
  if(s == NULL) {
    data = NULL;
    size = 0;
    return (*this);
  }
  data = (char *) calloc( 1, ulSize+1 );
  char *pTarget = data;
  for( xbUInt32 i = 0; i < ulSize; i++ ){
    *pTarget = *s;
    pTarget++;
    s++;
  }
  this->size = ulSize + 1;
  return *this;
}
/************************************************************************/
xbString &xbString::SetNum(xbInt32 lNum) {
  Sprintf("%ld", lNum);
  return *this;
}

/************************************************************************/
xbString &xbString::Sprintf( const char *sFormat, ...) {
  xbInt32 iRc;
  va_list ap;
  char *t;

  #if defined(HAVE__VSNPRINTF_S_F)

  va_start( ap, sFormat );
  size = (xbUInt32) _vsnprintf_s( NULL, 0, sFormat, ap ) + 1;
  va_end( ap );

  t = (char *) malloc( size );
  if( !t ){
    size = 0;
    return( *this );
  }

  va_start( ap, sFormat );
  iRc  = _vsnprintf_s( t, size, sFormat, ap );
  va_end( ap );

  #else
  #ifdef HAVE_VSPRINTF_S_F

  va_start( ap, sFormat );
  // size = (xbUInt32) vsprintf_s( NULL, 0, sFormat, ap ) + 1;
  size = (xbUInt32) _vscprintf( sFormat, ap ) + 1;
  va_end( ap );

  t = (char *) malloc( size );
  if( !t ){
    size = 0;
    return( *this );
  }

  va_start( ap, sFormat );
  iRc  = vsprintf_s( t, size, sFormat, ap );
  va_end( ap );

  #else
  #ifdef HAVE_VSNPRINTF_F

  va_start( ap, sFormat );
  size = (xbUInt32) vsnprintf( NULL, 0, sFormat, ap) + 1;
  va_end( ap );

  t = (char *) calloc( 1, size );
  if( !t ){
    size = 0;
    return( *this );
  }
  va_start( ap, sFormat );
  iRc  = vsnprintf( t, size, sFormat, ap );
  va_end( ap );

  #  else
  #      error "Fatal error building [xbstring.cpp] - You have neither _vsnprintf_s nor vsnprintf_s."
  #  endif
  #endif
  #endif

  if( iRc < 0 ){
    if( data )
      free( data );
    data = NULL;
    size = 0;
  } else {
    if( data )
      free( data );
    data = t;
  }
  return( *this );
}
/************************************************************************/
const char *xbString::Str() const {
  return data ? data : NullString;
}
/************************************************************************/
char *xbString::strncpy( char * cDest, xbUInt32 n ) const {
  xbUInt32 i;
  xbUInt32 ulLen;
  n > (size-1) ? ulLen = size-1 : ulLen = n;
  memset( cDest, 0x00, ulLen );
  for( i = 0; i < ulLen; i++ )
    cDest[i] = data[i];
  return cDest;
}
/************************************************************************/
xbString &xbString::SwapChars( char cFrom, char cTo ){
  xbUInt32 i;
  for( i = 0; i < size; i++ )
    if( data[i] == cFrom )
       data[i] = cTo;
  return *this;
}
/************************************************************************/
xbString &xbString::ToLowerCase(){
  xbUInt32 Len = this->Len();
  for (xbUInt32 i=0; i<Len; i++)
    data[i] = (char)tolower(data[i]);
  return *this;
}
/************************************************************************/
xbString &xbString::ToUpperCase(){
  xbUInt32 Len = this->Len();
  for (xbUInt32 i=0;i<Len;i++)
    data[i] = (char)toupper(data[i]);
  return *this;
}
/************************************************************************/
xbString &xbString::Trim(){
  Rtrim();
  Ltrim();
  return *this;
}
/************************************************************************/

xbBool xbString::ValidLogicalValue() const {
  if( Len() == 1 )
    if( data[0] == 'T' || data[0] == 'F' || data[0] == 'Y' || data[0] == 'N' || data[0] == '?' )
      return xbTrue;
  return xbFalse;
}
/************************************************************************/
xbBool xbString::ValidNumericValue() const {
  const char *p;
  p = data;
  while( *p ){
    if( *p != '+' && *p != '-' && *p != '.' && *p != '0' && *p != '1' &&
        *p != '2' && *p != '3' && *p != '4' && *p != '5' && *p != '6' &&
        *p != '7' && *p != '8' && *p != '9' )
      return xbFalse;
    else
      p++;
  }
  return xbTrue;
}
/************************************************************************/
//! @brief Private function used for copying a string
/*!
    For performance reasons, this is an internal function that does no 
    memory checking and assumes a valid buffer area is available to be copied.

    This function is marked as private because of the above reason and
    is used by "stronger" calling functions.

    @param sTarget Target destination of copied string
    @param sSource Source string to copy
    @returns Reference to string.
*/
char * xbString::xb_strcpy( char *sTarget, const char *sSource ){
  char *temp = sTarget;
  while( *sSource != '\0')
    *sTarget++ = *sSource++;
  *sTarget= '\0';
  return temp;
}
/************************************************************************/
xbString &xbString::ZapChar( char c ){
  if( data == NULL )
    return *this;
  if( data[0] == 0 )
    return *this;
  char *s;
  char *t;
  s = data;
  t = data;
  while( *s ){
    if( *s == c ){
      s++;
      size--;
    } else {
      *t++ = *s++;
    }
 }
 *t = 0x00; 
 data = (char *) realloc( data, size );
 return *this;
}
/************************************************************************/
xbString &xbString::ZapLeadingChar( char c ){
  /* left truncate all of character c */
  xbUInt32 iLen = 0;
  char *p;
  p = data;
  while( *p && *p == c ){
    iLen++;
    p++;
  }
  if( iLen )
    Ltrunc( iLen );
  return *this;
}
/************************************************************************/
xbString &xbString::ZapTrailingChar( char c ){

  xbUInt32 l = Len();
  if( l == 0 )
    return *this;
  xbUInt32 ulOrigSize = size;
  l--;
  for(;;) {
    if( data[l] != c )
      break;
    data[l] = 0;
    size--;
    if( l == 0 )
      break;
    l--;
  }
  if( ulOrigSize != size )
    data = (char *) realloc( data, size );
  return *this;
}
/************************************************************************/
//! @brief Stream insertion operator <<
/*!
    std::cout << MyString << std::endl;

    @param os Output stream
    @param s String to send to output stream
*/
std::ostream& operator<< ( std::ostream& os, const xbString & s ) {
  return os << s.Str();
}



}   /* namespace */