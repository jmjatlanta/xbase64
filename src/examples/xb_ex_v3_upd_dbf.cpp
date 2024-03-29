/*  xb_ex_v3_upd_dbf.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2021,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

    This example demonstrates how to open the DBase III table created by xb_ex_v3_create_dbf
    and apply various updates to the table.

*/

#include <xbase.h>

using namespace xb;

int main()
{

#ifdef XB_DBF3_SUPPORT

  xbInt16  fld_FIRSTNAME;
  xbInt16  fld_LASTNAME;
  xbInt16  fld_BIRTHDATE;
  xbInt16  fld_AMOUNT;
  xbInt16  fld_RETIRED;
  xbInt16  fld_ZIPCODE;

  #ifdef XB_MEMO_SUPPORT
  xbInt16  fld_MEMO1;
  xbString sMemoData;
  #endif


  /* define the classes */
  xbXBase x;                                /* initialize xbase             */
  x.SetDataDirectory( PROJECT_DATA_DIR );   /* where all the tables live    */
  x.EnableMsgLogging();
  x.WriteLogMessage( "Program [xb_ex_v3_upd_dbf] initializing..." );


  xbDbf * MyTable = new xbDbf3( &x );       /* class for V3 table */

  xbString sSearchKey;                      /* string for doing an index lookup */

  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;

  try{

    if(( iRc = MyTable->Open( "MyV3Table1.DBF" )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }


    #ifdef XB_NDX_SUPPORT
    // V3 NDX style indices can be opened manually  (vs production MDX index files opened automatically)


    if(( iRc = MyTable->OpenIndex( "NDX", "MYINDEX1.NDX")) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }


/*
    if(( iRc = MyTable->OpenIndex( "NDX", "MYINDEX2.NDX" )) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw iRc;
    }

    if(( iRc = MyTable->OpenIndex( "NDX", "MYINDEX3.NDX" )) != XB_NO_ERROR ){ 
      iErrorStop = 130;
      throw iRc;
    }
*/

    std::cout << "Current tag = [" << MyTable->GetCurTagName().Str() << "]\n";
    #endif

    // get the field numbers for all the fields in the table
    fld_FIRSTNAME = MyTable->GetFieldNo( "FIRSTNAME" );
    fld_LASTNAME  = MyTable->GetFieldNo( "LASTNAME" );
    fld_BIRTHDATE = MyTable->GetFieldNo( "BIRTHDATE" );
    fld_AMOUNT    = MyTable->GetFieldNo( "AMOUNT" );
    fld_RETIRED   = MyTable->GetFieldNo( "RETIRED?" );
    fld_ZIPCODE   = MyTable->GetFieldNo( "ZIPCODE" );



    // do an index lookup for (key does not exist in this example)
    sSearchKey = "abc123";
    if(( iRc = MyTable->Find( sSearchKey )) != XB_NOT_FOUND ){
      iErrorStop = 140;
      throw iRc;
    }
    std::cout << "RC = " << iRc << "\n";



    #ifdef XB_MEMO_SUPPORT
    fld_MEMO1     = MyTable->GetFieldNo( "MEMO1" );
    #endif



    // Blank the record buffer
    if(( iRc = MyTable->BlankRecord()) != XB_NO_ERROR ){
      iErrorStop = 140;
      throw iRc;
    }


    // put field examples - using field numbers
    if(( iRc = MyTable->PutField( fld_LASTNAME, "JONES" )) != XB_NO_ERROR ){
      iErrorStop = 150;
      throw iRc;
    }


    if(( iRc = MyTable->PutField( fld_FIRSTNAME, "JERRY" )) != XB_NO_ERROR ){
      iErrorStop = 160;
      throw iRc;
    }


    if(( iRc = MyTable->PutField( fld_AMOUNT, "12.35" )) != XB_NO_ERROR ){
      iErrorStop = 170;
      throw iRc;
    }

    if(( iRc = MyTable->PutField( fld_BIRTHDATE, "19880208" )) != XB_NO_ERROR ){
      iErrorStop = 180;
      throw iRc;
    }

    if(( iRc = MyTable->PutLogicalField( fld_RETIRED, "Y" )) != XB_NO_ERROR ){
      iErrorStop = 190;
      throw iRc;
    }

    if(( iRc = MyTable->PutLongField( fld_ZIPCODE, 12345 )) != XB_NO_ERROR ){
      iErrorStop = 200;
      throw iRc;
    }

    #ifdef XB_MEMO_SUPPORT
    sMemoData = "Memo data record 1";
    if(( iRc = MyTable->UpdateMemoField( fld_MEMO1, sMemoData )) != XB_NO_ERROR ){
      iErrorStop = 210;
      throw iRc;
    }
    #endif

    // Append the first record
    if(( iRc = MyTable->AppendRecord()) != XB_NO_ERROR ){
      // here is where you would address any errors.
      // in this program, we simply abort and continue
      MyTable->Abort();
    }

    // put field to the record buffer using field name (slightly less efficient than using field numbers)
    // Blank the record buffer
    if(( iRc = MyTable->BlankRecord()) != XB_NO_ERROR ){
      iErrorStop = 220;
      throw iRc;
    }

    if(( iRc = MyTable->PutField( "LASTNAME", "EINSTIEN" )) != XB_NO_ERROR ){
      iErrorStop = 230;
      throw iRc;
    }

    if(( iRc = MyTable->PutField( "FIRSTNAME", "ALBERT" )) != XB_NO_ERROR ){
      iErrorStop = 240;
      throw iRc;
    }

    if(( iRc = MyTable->PutField( "AMOUNT", "987.55" )) != XB_NO_ERROR ){
      iErrorStop = 250;
      throw iRc;
    }

    if(( iRc = MyTable->PutField( fld_BIRTHDATE, "19890209" )) != XB_NO_ERROR ){
      iErrorStop = 260;
      throw iRc;
    }

    if(( iRc = MyTable->PutLogicalField( "RETIRED?", "N" )) != XB_NO_ERROR ){
      iErrorStop = 270;
      throw iRc;
    }

    if(( iRc = MyTable->PutLongField( "ZIPCODE", 44256 )) != XB_NO_ERROR ){
      iErrorStop = 280;
      throw iRc;
    }

    #ifdef XB_MEMO_SUPPORT
    sMemoData = "Memo data record 2";
    if(( iRc = MyTable->UpdateMemoField( fld_MEMO1, sMemoData )) != XB_NO_ERROR ){
      iErrorStop = 290;
      throw iRc;
    }
    #endif

    // Append the second record 
    if(( iRc = MyTable->AppendRecord()) != XB_NO_ERROR ){
      // here is where you would address any errors.
      // in this program, we simply abort and continue
      MyTable->Abort();
    }


    if(( iRc = MyTable->GetRecord( 1 )) != XB_NO_ERROR ){
      iErrorStop = 300;
      throw iRc;
    }
    // get a field with a field number
    xbString sFirstName;
    if(( iRc = MyTable->GetField( fld_FIRSTNAME, sFirstName )) < 0 ){
      iErrorStop = 310;
      throw iRc;
    }
    std::cout << "First Name is [" << sFirstName.Str() << "]" << std::endl;

    xbString sLastName;
    if(( iRc = MyTable->GetField( "LASTNAME", sLastName )) < 0 ){
      iErrorStop = 320;
      throw iRc;
    }
    std::cout << "Last Name is [" << sLastName.Str() << "]" << std::endl;

    xbInt16 iNoOfDecimals;
    if(( iRc = MyTable->GetFieldDecimal( "AMOUNT", iNoOfDecimals )) != XB_NO_ERROR ){
      iErrorStop = 330;
      throw iRc;
    }
    std::cout << "There are " << iNoOfDecimals << " decimals in the AMOUNT field" << std::endl;

    xbString FieldName;
    if(( iRc = MyTable->GetFieldName( 4, FieldName )) != XB_NO_ERROR ){
      iErrorStop = 340;
      throw iRc;
    }
    std::cout << "Field #4 name is " << FieldName.Str() << std::endl;

    xbString sRetired;
    if(( iRc = MyTable->GetLogicalField( "RETIRED?", sRetired )) < 0 ){
      iErrorStop = 350;
      throw iRc;
    }
    std::cout << "Switch value = [" << sRetired.Str() << "]" << std::endl;

    xbInt32 lZip;
    if(( iRc = MyTable->GetLongField( "ZIPCODE", lZip )) < 0  ){
      iErrorStop = 360;
      throw iRc;
    }
    std::cout << "Long value = [" << lZip << "]" << std::endl;

    // Initialize the record buffer in preparation for another record
    if(( iRc = MyTable->BlankRecord()) != XB_NO_ERROR ){
      iErrorStop = 370;
      throw iRc;
    }

    // Append another record (it will be blank)
    if(( iRc = MyTable->AppendRecord()) != XB_NO_ERROR ){
      // here is where you would address any errors.
      // in this program, we simply abort and continue
      MyTable->Abort();
    };

    // mark record 1 for deletion
    if(( iRc = MyTable->GetRecord( 1 )) != XB_NO_ERROR ){
      iErrorStop = 300;
      throw iRc;
    }

    if(( iRc = MyTable->DeleteRecord()) != XB_NO_ERROR ){
      iErrorStop = 380;
      throw iRc;
    };

    // save current record
    if(( iRc = MyTable->PutRecord()) != XB_NO_ERROR ){
      iErrorStop = 390;
      throw iRc;
    }
    // pack the table with no options
    if(( iRc = MyTable->Pack()) != XB_NO_ERROR ){
      iErrorStop = 400;
      throw iRc;
    }

    if(( iRc = MyTable->Commit()) != XB_NO_ERROR ){
      // here is where you would address any errors.
      // in this program, we simply abort and continue
      MyTable->Abort();
    }

    /* Close database and associated indexes */
    if(( iRc = MyTable->Close())   != XB_NO_ERROR ){
      iErrorStop = 410;
      throw iRc;
    }

  }
  catch( xbInt16 rc ){
    std::cout << "xb_ex_v3_upd_dbf error.  Error stop point = [" << iErrorStop << "] iRc = [" << rc << "]" << std::endl;
    std::cout << x.GetErrorMessage( rc ) << std::endl;
  }

  delete MyTable;

  #endif // XB_DBF3_SUPPORT
  return 0;
}
