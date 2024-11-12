/*  xbuda.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023,20224 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

This class manages the user data area (UDA)

*/


#ifndef __XB_XBUDA_H__
#define __XB_XBUDA_H__

#ifdef  CMAKE_COMPILER_IS_GNUCC
#pragma interface
#endif


#ifdef XB_EXPRESSION_SUPPORT


namespace xb{

class XBDLLEXPORT xbUda {

 public:
  xbUda();
  ~xbUda();

  xbInt16 AddTokenForKey( const xbString &sKey, const xbString &sToken );
  void    Clear();
  xbInt16 DelTokenForKey( const xbString &sKey );
  xbInt16 GetTokenCnt() const;
  xbInt16 GetTokenForKey( const xbString &sKey, xbString &sData );
  xbInt16 UpdTokenForKey( const xbString &sKey, const xbString &sToken );
  void    DumpUda() const;

 private:
   xbLinkListOrd<xbString> llOrd;

};
}        /* namespace */
#endif   /* XB_EXPRESSION_SUPPORT */
#endif   /* __XBUDA_H__ */