/*  xb_ex_v4_upd_dbf.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

    This example demonstrates how to open the DBase IV table created by xb_ex_v4_create_dbf
    and apply various updates to the table.

*/

#include <xbase.h>

using namespace xb;

int main()
{

#ifdef XB_DBF4_SUPPORT


  /* define the classes */
  xbXBase x;                                       /* initialize xbase             */
  x.SetDataDirectory( PROJECT_DATA_DIR );   /* where all the tables live    */
  x.EnableMsgLogging();
  x.WriteLogMessage( "Program [xb_ex_v4_upd_dbf] initializing..." );

  xbDbf *MyTable = new xbDbf4( &x );                            /* class for DBase V4 table */

  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;

  try{

    if(( iRc = MyTable->Open( "Address.DBF" )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }

    /* get the field numbers for all the fields in the table */

    xbInt16 iFld_FIRSTNAME = MyTable->GetFieldNo( "FIRSTNAME" );
    xbInt16 iFld_LASTNAME  = MyTable->GetFieldNo( "LASTNAME" );
    xbInt16 iFld_BIRTHDATE = MyTable->GetFieldNo( "BIRTHDATE" );
    xbInt16 iFld_AMOUNT1   = MyTable->GetFieldNo( "AMOUNT1" );
    xbInt16 iFld_FRIEND    = MyTable->GetFieldNo( "FRIEND?" );
    xbInt16 iFld_ZIPCD     = MyTable->GetFieldNo( "ZIPCD" );
    xbInt16 iFld_AMOUNT2   = MyTable->GetFieldNo( "AMOUNT2" );

    #ifdef XB_MEMO_FIELDS
    zbInt16 iFld_MEMO1     = MyTable->GetFieldNo( "MEMO1" );
    #endif


    #ifdef XB_MDX_SUPPPORT
    std::cout << "Current tag = [" << MyTable->GetCurTagName() << "]\n";
    #endif

    // Blank the record buffer
    if(( iRc = MyTable->BlankRecord()) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }

    // put field examples - using field numbers
    if(( iRc = MyTable->PutField( iFld_LASTNAME, "JONES" )) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw iRc;
    }
    // could also reference by field name (see below) but referencing by number
    // is a little bit faster because it doesn't need to look up the number for the field name
    // Alternative-->   if(( iRc = MyTable->PutField( "LASTNAME", "JONES" )) != XB_NO_ERROR ){

    if(( iRc = MyTable->PutField( iFld_FIRSTNAME, "JERRY" )) != XB_NO_ERROR ){
      iErrorStop = 130;
      throw iRc;
    }

    if(( iRc = MyTable->PutField( iFld_AMOUNT1, "12.35" )) != XB_NO_ERROR ){
      iErrorStop = 140;
      throw iRc;
    }

    if(( iRc = MyTable->PutLogicalField( iFld_FRIEND, "Y" )) != XB_NO_ERROR ){
      iErrorStop = 150;
      throw iRc;
    }

    if(( iRc = MyTable->PutField( iFld_BIRTHDATE, "19880209" )) != XB_NO_ERROR ){
      iErrorStop = 160;
      throw iRc;
    }

    if(( iRc = MyTable->PutLongField( iFld_ZIPCD, 12345 )) != XB_NO_ERROR ){
      iErrorStop = 170;
      throw iRc;
    }

    // Append the first record
    if(( iRc = MyTable->AppendRecord()) != XB_NO_ERROR ){
      iErrorStop = 180;
      throw iRc;
    }

    // Commit the updates
    if(( iRc = MyTable->Commit()) != XB_NO_ERROR ){
      iErrorStop = 190;
      throw iRc;
    }

    // Blank the record buffer
    if(( iRc = MyTable->BlankRecord()) != XB_NO_ERROR ){
      iErrorStop = 200;
      throw iRc;
    }

    // put field to the record buffer using field name (slightly less efficient than using field numbers) 
    if(( iRc = MyTable->PutField( "LASTNAME", "FUCKPUTIN" )) != XB_NO_ERROR ){
      iErrorStop = 210;
      throw iRc;
    }

    if(( iRc = MyTable->PutField( "FIRSTNAME", "ALBERT" )) != XB_NO_ERROR ){
      iErrorStop = 220;
      throw iRc;
    }

    if(( iRc = MyTable->PutDoubleField( "AMOUNT1", (xbDouble) 987.55 )) != XB_NO_ERROR ){
      iErrorStop = 230;
      throw iRc;
    }

    if(( iRc = MyTable->PutLogicalField( "FRIEND?", "N" )) != XB_NO_ERROR ){
      iErrorStop = 240;
      throw iRc;
    }

    if(( iRc = MyTable->PutLongField( "ZIPCD", 44256 )) != XB_NO_ERROR ){
      iErrorStop = 250;
      throw iRc;
    }

    xbFloat f = (xbFloat) 12345.67;
    std::cout << f << std::endl;

    if(( iRc = MyTable->PutFloatField( iFld_AMOUNT2, f )) != XB_NO_ERROR ){
      iErrorStop = 260;
      throw iRc;
    }
    xbDouble d = 76543.21;
    if(( iRc = MyTable->PutDoubleField( iFld_AMOUNT1, d )) != XB_NO_ERROR ){
      iErrorStop = 270;
      throw iRc;
    }

    // Append the second record
    if(( iRc = MyTable->AppendRecord()) != XB_NO_ERROR ){
      iErrorStop = 280;
      throw iRc;
    }

    // Commit the updates
    if(( iRc = MyTable->Commit()) != XB_NO_ERROR ){
      iErrorStop = 290;
      throw iRc;
    }


    // get a field with a field number
    xbString FirstName;
    if(( iRc = MyTable->GetField( iFld_FIRSTNAME, FirstName )) < 0 ){
      iErrorStop = 300;
      throw iRc;
    }
    std::cout << "First Name is [" << FirstName.Str() << "]" << std::endl;

    xbString LastName;
    if(( iRc = MyTable->GetField( "LASTNAME", LastName )) < 0 ){
      iErrorStop = 310;
      throw iRc;
    }
    std::cout << "Last Name is [" << LastName.Str() << "]" << std::endl;

    xbInt16 iNoOfDecimals;
    if(( iRc = MyTable->GetFieldDecimal( "AMOUNT2", iNoOfDecimals )) != XB_NO_ERROR ){
      iErrorStop = 320;
      throw iRc;
    }
    std::cout << "There are " << iNoOfDecimals << " decimals in the AMOUNT field" << std::endl;

    xbString FieldName;
    if(( iRc = MyTable->GetFieldName( 4, FieldName )) != XB_NO_ERROR ){
      iErrorStop = 330;
      throw iRc;
    }
    std::cout << "Field #4 name is " << FieldName.Str() << std::endl;

    xbString sFriend;
    if(( iRc = MyTable->GetLogicalField( "FRIEND?", sFriend )) < 0 ){
      iErrorStop = 340;
      throw iRc;
    }
    std::cout << "Switch value = [" << sFriend.Str() << "]" << std::endl;

    xbInt32 lZip = 0;
    if(( iRc = MyTable->GetLongField( "ZIPCODE", lZip )) < 0  ){
      iErrorStop = 350;
      throw iRc;
    }
    std::cout << "Long value = [" << lZip << "]" << std::endl;

    if(( iRc = MyTable->GetFloatField( iFld_AMOUNT2, f )) < 0 ){
      iErrorStop = 360;
      throw iRc;
    }
    printf( "Field NUMFLD1 %8.2f\n", f );


    if(( iRc = MyTable->GetDoubleField( iFld_AMOUNT1, d )) < 0 ){
      iErrorStop = 370;
      throw iRc;
    }
    printf( "Field NUMFLD2 %8.2f\n", d );

    // Initialize the record buffer in preparation for another record
    if(( iRc = MyTable->BlankRecord()) != XB_NO_ERROR ){
      iErrorStop = 380;
      throw iRc;
    }

    // Append another record (it will be blank)
    if(( iRc = MyTable->AppendRecord()) != XB_NO_ERROR ){
      iErrorStop = 390;
      throw iRc;
    };

    // mark current record for deletion 
    if(( iRc = MyTable->DeleteRecord()) != XB_NO_ERROR ){
      iErrorStop = 400;
      throw iRc;
    };

    // save current record
    if(( iRc = MyTable->PutRecord()) != XB_NO_ERROR ){
      iErrorStop = 410;
      throw iRc;
    }

    if(( iRc = MyTable->Commit()) != XB_NO_ERROR ){
      iErrorStop = 420;
      throw iRc;
    }

    // example code to loop through the table
	xbUInt32 ulRecCnt;
	if(( iRc = MyTable->GetRecordCnt( ulRecCnt )) != XB_NO_ERROR ){
	  iErrorStop = 430;
	  throw iRc;
	}
	
    for( xbUInt32 ul = 1; ul <= ulRecCnt; ul++ ){
      if(( iRc = MyTable->GetRecord( ul )) != XB_NO_ERROR ){
        iErrorStop = 440;
        throw iRc;
      }
      // do something with the record here
      std::cout << "Tuple = " << MyTable->GetCurRecNo() << std::endl;

    }

    /* Close database and associated indexes */
    if(( iRc = MyTable->Close())   != XB_NO_ERROR ){
      iErrorStop = 450;
      throw iRc;
    }
    delete MyTable;


  }
  catch( xbInt16 iRc ){
    std::cout << "xb_ex_v4_upd_dbf error.  Error stop point = [" << iErrorStop << "] iRc = [" << iRc << "]" << std::endl;
    std::cout << x.GetErrorMessage( iRc ) << std::endl;
  }

#endif    // XB_DBF4_SUPPORT
  return 0;
}
