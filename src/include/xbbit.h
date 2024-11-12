/*  xbbit.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023,2024 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/


#ifndef __XB_XBBIT_H__
#define __XB_XBBIT_H__

#ifdef  CMAKE_COMPILER_IS_GNUCC
#pragma interface
#endif


namespace xb{

/*! @brief Class for handling trivial bit operations. 

    Misc bit handling utility functions included in this class.
*/

class XBDLLEXPORT xbBit{

 public:
 
 /*!
    @brief Check a bit in a one byte field and see if it is set.
    @param c One byte char field to examine.
    @param iBitNo which bit to examine.
    @return xbTrue Bit is set<br>
            xbFalse Bit is not set
  */
  xbBool BitSet( unsigned char c, xbInt16 iBitNo ) const;

  ///@{
   /** 
      @brief Dump bits of <em>c</em> to sysout
      @param c Value to dump bit settings on
   */
   void BitDump( unsigned char c ) const;
   void BitDump( char c )          const;
  ///@}


 
};

}        /* namespace xb    */
#endif   /* __XB_XBBIT_H__ */