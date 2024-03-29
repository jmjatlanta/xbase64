/*  xbbcd.h

XBase64 Software Library

Copyright (c) 1997,2003,2014, 2018, 2019, 2022 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

#ifndef __XB_BCD_H__
#define __XB_BCD_H__
#ifdef XB_INDEX_SUPPORT

///@cond DOXYOFF

namespace xb{

// structure for bcd value
// next structure superceeded.  Different compilers allocate different
// sizes to structures with bit fields, can't count on the size

/*
struct XBDLLEXPORT xbBcdStrucOld {
  unsigned SigDigits : 8;            // significant digit count
  unsigned Filler    : 2;            // always set to 1
  unsigned EncDigits : 5;            // encoded digit count
  unsigned Sign      : 1;            // +/- sign
  unsigned char Data[10];
};
*/

struct XBDLLEXPORT xbBcdStruct {
  unsigned char cSigDigits;          // significant digit count
  unsigned char cEncDigits;          // encoded digit count
  unsigned char cData[10];           // data
};

struct XBDLLEXPORT xbByteSplit {
  unsigned c1 : 4;
  unsigned c2 : 4;
};

class XBDLLEXPORT xbBcd {
 public:

   xbBcd( const xbString &sIn );
   xbBcd( xbDouble dIn );
   xbBcd( const void *vBcd12In );
   void Set( xbDouble dIn );
   void Set( const void *cBcd12In );
   void Set( const xbString &sIn );


   void ToString( xbString &sOut );
   void ToDouble( xbDouble &dOut );
   void ToBcd( xbBcdStruct &bcdOut );
   void ToChar( char *cOut );

   // const unsigned char * GetBcd() const;
   void StringToBcd( const xbString &sStringIn );

   xbInt16 Compare( const xbBcd &bcdIn );
   xbInt16 Compare( xbDouble d );

   const unsigned char *GetData() const;
   const void * GetBcd() const;


 private:
   void ctor();
   unsigned char GetEncDigitsNoSign() const;
   unsigned GetSign() const;
   unsigned GetSigDigits() const;
   unsigned GetActualSigDigits() const;


//   xbBcdStruc  bcdOld;
   xbBcdStruct bcd;
};

}        /* namespace xb     */


///@endcond DOXYOFF

#endif   /* XB_INDEX_SUPPORT */
#endif   /* __XB_BCD_H__   */
