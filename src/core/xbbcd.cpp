/* xbbcd.cpp 

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

   BCD class

*/

#include "xbase.h"

#ifdef XB_INDEX_SUPPORT
///@cond DOXYOFF
namespace xb{



xbBool bcdBitSet( unsigned char c, xbInt16 iBitNo ){
  return c & 1 << iBitNo;
}
void bcdBitDump( unsigned char c ){
  for( int i = 7; i >= 0; i-- )
    std::cout << (bcdBitSet( c, i ) ? 1 : 0);
}
void bcdBitDump( char c ){
  bcdBitDump( (unsigned char) c );
}


/***********************************************************************/
void xbBcd::ctor(){
  memset( &bcd, 0x00, sizeof( xbBcdStruct ));
}
/***********************************************************************/
xbBcd::xbBcd( xbDouble d ) {
  Set( d );
}
/***********************************************************************/
xbBcd::xbBcd( const xbString &sNumIn ) {
  Set( sNumIn );
}
/***********************************************************************/
xbBcd::xbBcd( const void *vBcdIn ) {
  memcpy( &bcd, vBcdIn, sizeof( xbBcdStruct ));
}
/***********************************************************************/
void xbBcd::Set( xbDouble d ) {
  ctor();
  xbString s( d );
  StringToBcd( s );
}
/***********************************************************************/
void xbBcd::Set( const xbString &sNumIn ) {
  ctor();
  StringToBcd( sNumIn );
}
/***********************************************************************/
void xbBcd::Set( const void *vBcdIn ) {
  memcpy( &bcd, vBcdIn, sizeof( xbBcdStruct ));
}
/***********************************************************************/
void xbBcd::StringToBcd( const xbString &sIn )
{

  unsigned char cEdc = 0;         // encoded digit count
  xbUInt32 iPos;                  // current position in source string
  xbBool   bDecHit = xbFalse;     // decimal position hit?
  unsigned char *p = bcd.cData;   // pointer to cData
  xbInt16  iBytePos = 0;          // next load position in xbs structure
  xbInt16  iBcdDataPos = 0;       // current position in output structure
  xbByteSplit xbs;                // combiner


  ctor();
  xbString sNum( sIn );
  xbBool bSign = xbFalse;

  sNum.Trim();
  if( sNum[1] == '-' ){
    bSign = 1;
    sNum.ZapLeadingChar( '-' );
  }
  sNum.ZapLeadingChar( '0' );

  xbInt16 iDecPos = sNum.Pos( '.' );
  if( iDecPos > 0 ){
    sNum.ZapTrailingChar( '0' );
    cEdc = (unsigned char) (sNum.Len() - 1);
  } else {
    cEdc = (unsigned char) (sNum.Len());
  }
  if( cEdc > 31 ) cEdc = 31;  // max 5 bit number


  if( sNum[1] == '.' ){
    iPos = 2;
    bDecHit = xbTrue;
    while( sNum[iPos] == '0' && iPos <= sNum.Len()){
      bcd.cSigDigits--;
      iPos++;
    }
  } else {
    iPos = 1;
  }

  while( iPos <= sNum.Len() ){
    if( sNum[iPos] == '.' )
      bDecHit = true;
    else{
      if( !bDecHit ){
        bcd.cSigDigits++;
      }
      if( iBytePos++ == 0 ){
        xbs.c2 = (unsigned) sNum[iPos] - 0x30;
      } else {
        xbs.c1 = (unsigned) sNum[iPos] - 0x30;
        iBytePos = 0;
        if( iBcdDataPos++ < 10 ){
          memcpy( p++, &xbs, 1 );
          xbs.c1 = 0x00;
        }
      }
    }
    iPos++;
  }

  if( iBytePos == 1 && iBcdDataPos < 10 ){
    memcpy( p, &xbs, 1 );
  }

  bcd.cSigDigits += 52;
  bcd.cEncDigits = cEdc << 2;
  bcd.cEncDigits = bcd.cEncDigits | 0x01;
  if( bSign )
    bcd.cEncDigits = bcd.cEncDigits | 0x80;

}
/***********************************************************************/
void xbBcd::ToChar( char * cOut ){

  memcpy( cOut, &bcd, sizeof( xbBcdStruct )); 
}
/***********************************************************************/
void xbBcd::ToDouble( xbDouble &d ){
  xbString s;
  ToString( s );
  d = atof( s.Str());
}
/***********************************************************************/
void xbBcd::ToString( xbString &sStringOut ){

//  printf( "\n\n\nToString " );
//   printf( "Sig digits [%d]  EncodedDigits [%d] sign [%d] sizeof struct [%d]\n", 
//  bcd.cSigDigits - 52 , bcd.EncDigits, bcd.Sign, sizeof( xbBcdStruct ));

// this routine converts a bcd numeric key value to a base 10 number in a string


  xbBool bHasDot = false;
  xbInt16 iSig = bcd.cSigDigits - 52;
  xbByteSplit xbs;
  unsigned char *p = bcd.cData;
  unsigned char c;

//  xbInt16 iEnc = bcd.cEncDigits;
  xbInt16 iEnc = GetEncDigitsNoSign();

  // set the sign
  // if( bcd.Sign )
  if( bcd.cEncDigits >> 7 )
    sStringOut = "-";
  else
    sStringOut = "";

  // do add any needed zeroes after the decimal
  if( iSig <= 0 ){
    if( iEnc > 0 ){
      sStringOut.Append( "." );
      bHasDot = true;
    }
    for( xbInt16 i = iSig; i < 0; i++ )
      sStringOut.Append( "0" );
  }

  // do the encoded digits
  while( iEnc > 0 ){
    if( iSig == 0 && !bHasDot )
      sStringOut.Append( "." );
    c = *p++;
    memcpy( &xbs, &c, 1 );
    c = xbs.c2 + 0x30;
    sStringOut.Append((char) c );
    iSig--;
    iEnc--;
    if( iEnc > 0 ){
      if( iSig == 0 && !bHasDot )
        sStringOut.Append( "." );
      c = xbs.c1 + 0x30;
      sStringOut.Append((char) c );
      iSig--;
      iEnc--;
    }
  }
  // do the trailing zeroes
  while( iSig-- > 0 )
    sStringOut.Append( "0" );
  if( sStringOut == "" )
    sStringOut = "0";

}

/***********************************************************************/
xbInt16 xbBcd::Compare( xbDouble d ){
  xbBcd bcdIn( d );

  xbString s;
  bcdIn.ToString( s );
  return Compare( bcdIn );
}

/***********************************************************************/
xbInt16 xbBcd::Compare( const xbBcd &bcdIn ){
  // if this == bcdIn return 0
  // if this  < bcdIn return -1
  // if this  > bcdIn return  1


  xbInt16 iRc = 0;
//  if( bcd.Sign != bcdIn.GetSign() ){
//    bcd.Sign > 0 ? iRc = -1 : iRc = 1;

  if( (unsigned)(bcd.cEncDigits >> 7 ) != bcdIn.GetSign() ){
    (bcd.cEncDigits >> 7 ) > 0 ? iRc = -1 : iRc = 1;
    return iRc;
  }

  if( (unsigned) bcd.cSigDigits != bcdIn.GetSigDigits()){
    // if( !bcd.Sign ){                            // positive numbers
   if( !(bcd.cEncDigits >> 7 )){                            // positive numbers
      if( (unsigned) bcd.cSigDigits > bcdIn.GetSigDigits())
        return 1;
      else
        return -1;
    } else {                                   // negative numbers
      if( (unsigned) bcd.cSigDigits > bcdIn.GetSigDigits())
        return -1;
      else
        return 1;
    }
  }

//  iRc = xbXBase::xbMemcmp( bcd.cData, bcdIn.GetData(), (size_t)((bcd.cEncDigits + 1) / 2) );
  iRc = xbXBase::xbMemcmp( bcd.cData, bcdIn.GetData(), (size_t)((GetEncDigitsNoSign() + 1) / 2) );
  if( iRc == 0 )
    return 0;
  // else if((!bcd.Sign && iRc > 0) || (bcd.Sign && iRc < 0 ))
  else if((!(bcd.cEncDigits >> 7) && iRc > 0) || ((bcd.cEncDigits >> 7) && iRc < 0 ))
    return 1;
  else 
    return -1;
}

/***********************************************************************/
unsigned char xbBcd::GetEncDigitsNoSign() const {
  unsigned char c = bcd.cEncDigits << 1;
  return c >> 3;
}
/***********************************************************************/
unsigned xbBcd::GetSign() const {
  //return bcd.Sign;
  return (unsigned) bcd.cEncDigits >> 7;
}
/***********************************************************************/
unsigned xbBcd::GetSigDigits() const {
  return bcd.cSigDigits;
}
/***********************************************************************/
unsigned xbBcd::GetActualSigDigits() const {
  return bcd.cSigDigits - (xbUInt32) 52;
}
/***********************************************************************/
const unsigned char * xbBcd::GetData() const {
  const unsigned char *p = bcd.cData;
  return p;
}
/***********************************************************************/
const void * xbBcd::GetBcd() const {
  return &bcd;
}
}              /* namespace       */
///@endcond DOXYOFF
#endif         /*  XB_INDEX_SUPPORT */

