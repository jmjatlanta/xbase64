/* xbbit.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023,2024 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

#include "xbase.h"

namespace xb{
/*************************************************************************/
void xbBit::BitDump( unsigned char c ) const {
  for( int i = 7; i >= 0; i-- )
    std::cout << (BitSet( c, i ) ? 1 : 0);
}
void xbBit::BitDump( char c ) const {
  BitDump( (unsigned char) c );
}
/*************************************************************************/
xbBool xbBit::BitSet( unsigned char c, xbInt16 iBitNo ) const {
  return c & 1 << iBitNo;
}

}   /* namespace */