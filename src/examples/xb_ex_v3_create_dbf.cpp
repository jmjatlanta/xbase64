/*  xb_ex_v3_create_dbf.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

    This example demonstrates the creation of a Version III file and and indices 

*/

#include <xbase.h>

using namespace xb;

int main()
{

#ifdef XB_DBF3_SUPPORT

  xbSchema MyRecord[] =
  {
    { "FIRSTNAME", XB_CHAR_FLD,     15, 0 },
    { "LASTNAME",  XB_CHAR_FLD,     20, 0 },
    { "BIRTHDATE", XB_DATE_FLD,      8, 0 },
    { "AMOUNT",    XB_NUMERIC_FLD,   9, 2 },
    { "RETIRED?",  XB_LOGICAL_FLD,   1, 0 },
    { "ZIPCODE",   XB_NUMERIC_FLD,   5, 0 },
#ifdef XB_MEMO_SUPPORT
    { "MEMO1",     XB_MEMO_FLD,     10, 0 },
#endif
    { "",0,0,0 }
  };

  /* define the classes */
  xbXBase x;                                       /* initialize xbase             */
  x.SetDataDirectory( PROJECT_DATA_DIR );          /* where all the tables live    */

  xbDbf *MyDbfFile;                                /* Pointer to dbf class         */
  MyDbfFile = new xbDbf3(&x);                      /* Create Version 3 instance    */


//  Create Dbase3  NDX style indices if support compiled in
  #ifdef XB_NDX_SUPPORT
  xbIxNdx MyIndex1( MyDbfFile );    /* class for index 1 */
  xbIxNdx MyIndex2( MyDbfFile );    /* class for index 2 */
  xbIxNdx MyIndex3( MyDbfFile );    /* class for index 3 */
  #endif


  // fixme
  // Create Clipper NTX style indices if support compiled in  - bring this back to life in a future release
  #ifdef XB_INDEX_NTX
  xbNtx MyIndex4( &MyDbfFile );    /* class for index 4 */
  xbNtx MyIndex5( &MyDbfFile );    /* class for index 5 */
  #endif

  xbInt16 rc;

  if(( rc = MyDbfFile->CreateTable( "MyV3Table1", "MyV3ExampleTableAlias", MyRecord, XB_OVERLAY, XB_MULTI_USER )) != XB_NO_ERROR )
    x.DisplayError( rc );
  else
  {

    #ifdef XB_NDX_SUPPORT

    xbIx *pIx;
    void *pTag;

    /*
     Create a few index tags
     CreateTag( const xbString &sIxType, const xbString &sName, const xbString &sKey, const xbString &sFilter, 
               xbInt16 iDescending, xbInt16 iUnique, xbInt16 iOverLay, xbIx **xbIxOut, void **vpTagOut );
    */

     /* define a simple index */
     if(( rc = MyDbfFile->CreateTag  ( "NDX", "MYINDEX1.NDX", "LASTNAME", "",  0, 1, XB_OVERLAY, &pIx, &pTag )) != XB_NO_ERROR )
        x.DisplayError( rc );


     /* define a multi-field index "LASTNAME            FIRSTNAME" */
     if(( rc = MyDbfFile->CreateTag(  "NDX", "MYINDEX2.NDX", "LASTNAME+FIRSTNAME", "", 0, 1, XB_OVERLAY, &pIx, &pTag )) != XB_NO_ERROR )
        x.DisplayError( rc );

     /* define a numeric index "ZIPCODE" */
     if(( rc = MyDbfFile->CreateTag( "NDX", "MYINDEX3.NDX", "ZIPCODE", "", 0, 0, XB_OVERLAY, &pIx, &pTag )) != XB_NO_ERROR )
        x.DisplayError( rc );

     std::cout << "Tag Count in MYINDEX3.NDX = " << pIx->GetTagCount() << "\n";
     xbString sTagName;
     sTagName = pIx->GetTagName( &pTag );
     std::cout << "Tag Name in MYINDEX3.NDX = " << sTagName.Str() << "\n";


     #endif

  }

  MyDbfFile->Close();   /* Close database and associated indexes */
  delete MyDbfFile;

  #endif // XB_DBF3_SUPPORT
  return 0;
}
