/*  xbtypes.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

#ifndef __XB_XTYPES_H__
#define __XB_XTYPES_H__

namespace xb{


typedef float       xbFloat;
typedef double      xbDouble;
typedef short int   xbBool;
typedef xbUInt64    xbOffT;


#define xbTrue  1
#define xbFalse 0


#ifdef HAVE__FILENO_F
  #define xbFileNo _fileno
#else
  #define xbFileNo fileno
#endif

#ifdef XB_LOCKING_SUPPORT
 #if defined( HAVE_LONG_LONG )
  #define LK4026531839 4026531839LL
  #define LK4026531838 4026531838LL
  #define LK3026531838 3026531838LL
  #define LK1000000000 1000000000LL
 #else
  #define LK4026531839 4026531839L
  #define LK4026531838 4026531838L
  #define LK3026531838 3026531838L
  #define LK1000000000 1000000000L
 #endif
#endif



}  /* namespace */
#endif  /*  __XB_XTYPES_H__  */
