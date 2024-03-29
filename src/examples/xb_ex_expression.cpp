/*  xb_ex_expression.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022 Gary A Kunkel

The xb64 software library is covered under 
the terms of the GPL Version 3, 2007 license.

Email Contact:

    xb64-devel@lists.sourceforge.net
    xb64-users@lists.sourceforge.net


   This example program demonstrates expression usage

*/


#include <xbase.h>

using namespace xb;



  xbSchema MyV4Record[] = 
  {
    { "CFLD1",  XB_CHAR_FLD,    20, 0 },
    { "CFLD2",  XB_CHAR_FLD,    10, 0 },
    { "NFLD1",  XB_NUMERIC_FLD,  5, 0 },
    { "DATE1",  XB_DATE_FLD,     8, 0 },
    { "",0,0,0 }
  };

//*********************************************************************************************************************************
void PrintResult( xbString *sExpression, xbExp *exp );
void PrintResult( xbString *sExpression, xbExp *exp ){

    // Determine the expression return type
    char cExpType = exp->GetReturnType();

    // Process the expression results, dependent on return type
    if( cExpType == XB_EXP_NUMERIC ){
      xbDouble dResult;
      exp->GetNumericResult( dResult );
      std::cout << "Numeric result from expression [" << sExpression->Str() << "] is [" << dResult << "]" << std::endl;

    } else if( cExpType == XB_EXP_DATE ){
      xbDate dt;
      exp->GetDateResult( dt );
      std::cout << "Date result from expression [" << sExpression->Str() << "] is [" << dt.Str() << "]" << std::endl;

    } else if( cExpType == XB_EXP_LOGICAL ){
      xbBool bResult;
      exp->GetBoolResult( bResult );
      std::cout << "Bool result from expression [" << sExpression->Str() << "] is [" << (bResult ? " True" : "False") << "]" << std::endl;

    } else if( cExpType == XB_EXP_CHAR ){
      xbString sResult;
      exp->GetStringResult( sResult );
      std::cout << "Char result from expression [" << sExpression->Str() << "] is [" << sResult.Str() << "]" << std::endl;
    }

}

//*********************************************************************************************************************************
//int main( int ac, char ** av ){

int main(){

  xbInt16 iRc        = 0;
  xbInt16 iErrorStop = 0;
  xbIx    *pIx       = NULL;
  void    *pTag      = NULL;

  xbXBase x;
  xbDbf *MyFile = new xbDbf4( &x );

  try{ 

    if(( iRc = MyFile->CreateTable( "EXPEXAMPL.DBF", "TestMdxX2", MyV4Record, XB_OVERLAY, XB_MULTI_USER )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }
    /*
     CreateTag( const xbString &sIxType, const xbString &sName, const xbString &sKey, const xbString &sFilter, 
             xbInt16 iDescending, xbInt16 iUnique, xbInt16 iOverLay, xbIx **xbIxOut, void **vpTagOut );
    */

    // the following index definition has two expressions   
    //  1) CFLD1+CFLD2      -- concat two char fields into an index key
    //  2) .NOT. DELETED()  -- don't include any deleted records in the index
    if(( iRc = MyFile->CreateTag( "MDX", "TAG1",  "CFLD1+CFLD2", ".NOT. DELETED()", 0, 0, XB_OVERLAY, &pIx, &pTag )) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }

    // add a record to the table
    if(( iRc = MyFile->BlankRecord()) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw iRc;
    }

    if(( iRc = MyFile->PutField( "CFLD1", "Some text" )) != XB_NO_ERROR ){
      iErrorStop = 130;
      throw iRc;
    }

    if(( iRc = MyFile->PutField( "CFLD2", "Other text" )) != XB_NO_ERROR ){
      iErrorStop = 140;
      throw iRc;
    }

    if(( iRc = MyFile->PutLongField( "NFLD1", 1000 )) != XB_NO_ERROR ){
      iErrorStop = 150;
      throw iRc;
    }

    xbDate dt;
    dt.Set( "19890209" );
    if(( iRc = MyFile->PutDateField( "DATE1", dt )) != XB_NO_ERROR ){
      iErrorStop = 160;
      throw iRc;
    }

    if(( iRc = MyFile->AppendRecord()) != XB_NO_ERROR ){
      iErrorStop = 170;
      throw iRc;
    }

    if(( iRc = MyFile->Commit()) != XB_NO_ERROR ){
      iErrorStop = 180;
      throw iRc;
    }


    // To use the XBase64 expression processing logic
    //  1)  Parse an expression with the xbExp::ParseExpression() method
    //  2)  Process the parsed expression with the xbExp::ProcessExpression() method
    //  3)  If needed, determine the expression return type with the xbExp::GetReturnType() method
    //  4)  Use the appriate methid to retrieve the expression value:
    //           xbExp::GetNumericResult()
    //           xbExp::GetDateResult()
    //           xbExp::GetLogicalResult()
    //           xbExp::GetStringResult()


    //  The expression only needs to be parsed once.  The ProcessExpression() method can be used
    //  zero, one or many times after it is initially parsed.

    // see docs/html/xbc5.html for expression documentation
    // see example below


    // Numeric expression example
    xbString sExpression = "NFLD1 * (2 + RECNO())";
    xbExp exp( &x );
    // Parse the expression
    if(( iRc = exp.ParseExpression( MyFile, sExpression )) != XB_NO_ERROR ){
      iErrorStop = 190;
      throw iRc;
    }
    // Process the parsed expression
    if(( iRc = exp.ProcessExpression()) != XB_NO_ERROR ){
      iErrorStop = 200;
      return -1;
    }
    PrintResult( &sExpression, &exp );

    // String expression example
    sExpression = "CFLD1+CFLD2+'{'+DTOS(DATE1)+'}'";
    xbExp exp2( &x );
    if(( iRc = exp2.ParseExpression( MyFile, sExpression )) != XB_NO_ERROR ){
      iErrorStop = 210;
      throw iRc;
    }

    // Process the parsed expression
    if(( iRc = exp2.ProcessExpression()) != XB_NO_ERROR ){
      iErrorStop = 220;
      return -1;
    }

    PrintResult( &sExpression, &exp2 );

    // Date example
    sExpression = "DATE() + 6";
    xbExp exp3( &x );
    if(( iRc = exp3.ParseExpression( MyFile, sExpression )) != XB_NO_ERROR ){
      iErrorStop = 230;
      throw iRc;
    }
    // Process the parsed expression
    if(( iRc = exp3.ProcessExpression()) != XB_NO_ERROR ){
      iErrorStop = 240;
      return -1;
    }
    PrintResult( &sExpression, &exp3 );

    // Logic example
    sExpression = "NFLD1 = 5";
    xbExp exp4( &x );
    if(( iRc = exp4.ParseExpression( MyFile, sExpression )) != XB_NO_ERROR ){
      iErrorStop = 250;
      throw iRc;
    }
    // Process the parsed expression
    if(( iRc = exp4.ProcessExpression()) != XB_NO_ERROR ){
      iErrorStop = 260;
      return -1;
    }
    PrintResult( &sExpression, &exp4 );

    // Cleanup
    MyFile->DeleteTable();
    delete MyFile;


  } catch (xbInt16 iRc ){

     std::cout << "Error in program xb_ex_expression at location " << iErrorStop << std::endl;
     std::cout << x.GetErrorMessage( iRc ) << std::endl;

  }




  return iRc;
}
