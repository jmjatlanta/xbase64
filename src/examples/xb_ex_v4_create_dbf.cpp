/*  xb_ex_v4_create_dbf.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

    This example demonstrates the creation of a Version IV file and and indices 

    Creates three files in folder "PROJECT_DATA_DIR"
    Address.DBF   -   Table with all the data
    Address.DBT   -   Memo (variable lenght char field) data
    Address.MDX   -   File with index data

*/

#include <xbase.h>

using namespace xb;

int main()
{

#ifdef XB_DBF4_SUPPORT

  xbSchema MyAddressBookRecord[] =
  {
    { "LASTNAME",  XB_CHAR_FLD,     20, 0 },
    { "FIRSTNAME", XB_CHAR_FLD,     15, 0 },
    { "COMPANY",   XB_CHAR_FLD,     20, 0 },
    { "ADDRESS",   XB_CHAR_FLD,     35, 0 },
    { "CITY",      XB_CHAR_FLD,     30, 0 },
    { "STATECD",   XB_CHAR_FLD,      2, 0 },
    { "ZIPCD",     XB_CHAR_FLD,     10, 0 },

    { "BIRTHDATE", XB_DATE_FLD,      8, 0 },

    { "AMOUNT1",   XB_NUMERIC_FLD,   9, 2 },
    { "AMOUNT2",   XB_FLOAT_FLD,    12, 2 },

    { "FRIEND?",   XB_LOGICAL_FLD,   1, 0 },
    { "FAMILY?",   XB_LOGICAL_FLD,   1, 0 },
    { "BUSASSOC?", XB_LOGICAL_FLD,   1, 0 },

    #ifdef XB_MEMO_SUPPORT
    { "NOTES",     XB_MEMO_FLD,     10, 0 },
    #endif
    { "",0,0,0 }
  };

  /* define the classes */
  xbXBase x;                                /* initialize xbase             */
  x.SetDataDirectory( PROJECT_DATA_DIR );   /* where all the tables/files live    */

  xbInt16 iRc;
  xbDbf * MyDbfFile;

  #ifdef XB_MDX_SUPPORT
  xbIx *pIx;
  void *pTag;
  #endif  // XB_MDX_SUPPORT

  MyDbfFile = new xbDbf4( &x );

  if(( iRc = MyDbfFile->CreateTable( "Address.DBF", "Address", MyAddressBookRecord, XB_OVERLAY, XB_MULTI_USER )) != XB_NO_ERROR )
    x.DisplayError( iRc );
  else
  {

    #ifdef XB_MDX_SUPPORT

    /*
     Create a few index tags
     CreateTag( const xbString &sIxType, const xbString &sName, const xbString &sKey, const xbString &sFilter, 
               xbInt16 iDescending, xbInt16 iUnique, xbInt16 iOverLay, xbIx **xbIxOut, void **vpTagOut );
    */

    // std::cout << "Creating three index tags\n";
    if(( iRc = MyDbfFile->CreateTag( "MDX", "NAME",   "LASTNAME+FIRSTNAME", ".NOT. DELETED()", 0, 0, XB_OVERLAY, &pIx, &pTag )) != XB_NO_ERROR )
      x.DisplayError( iRc );
    if(( iRc = MyDbfFile->CreateTag( "MDX", "BDDATE",  "BIRTHDATE", ".NOT. DELETED()", 0, 0, XB_OVERLAY, &pIx, &pTag )) != XB_NO_ERROR )
      x.DisplayError( iRc );
    if(( iRc = MyDbfFile->CreateTag( "MDX", "COMPANY", "COMPANY+LASTNAME+FIRSTNAME", ".NOT. DELETED()", 0, 0, XB_OVERLAY, &pIx, &pTag )) != XB_NO_ERROR )
      x.DisplayError( iRc );

    #endif  // XB_MDX_SUPPORT
  }

  MyDbfFile->Close();   /* Close database and associated indexes */
  delete MyDbfFile;

  #endif // XB_DBF4_SUPPORT
  return 0;
}
