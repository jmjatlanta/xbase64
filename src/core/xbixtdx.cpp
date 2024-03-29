/*  xbixtdx.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

This module handles temporary index logic 

*/

#include "xbase.h"



#ifdef XB_TDX_SUPPORT


namespace xb{

/************************************************************************/
xbIxTdx::xbIxTdx( xbDbf *dbf ) : xbIxMdx( dbf ) {

  // Init();  not needed, called in xbMdx
  sIxType = "TDX";
}

/************************************************************************/
xbIxTdx::~xbIxTdx() {

//  std::cout << "xbIxTdx::Destructor()\n";

}

/***********************************************************************/
xbInt16 xbIxTdx::Close(){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;


  try{
    if(( iRc = xbIxMdx::Close()) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }
    if(( iRc = xbRemove()) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxTdx::Close() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }
  return iRc;
}

/***********************************************************************/
//! @brief Create new tag.
/*!
  This routine creates a new tag.   When complete, sets the cur tag pointer to 
  the newly created tag.


  \param sName Tag Name, including .MDX suffix
  \param sKey Key Expression
  \param sFilter Filter expression.
  \param iDescending 
  \param iUnique xbtrue - Unique.<br>xbFalse - Not unique.
  \param iOverLay xbTrue - Overlay if file already exists.<br>xbFalse - Don't overlay.
  \param vpTag Output from method Pointer to vptag pointer.
  \returns <a href="xbretcod_8h.html">Return Codes</a>
*/


xbInt16  xbIxTdx::CreateTag( const xbString &sName, const xbString &sKey, const xbString &sFilter, xbInt16 iDescending, xbInt16 iUnique, xbInt16 iOverlay, void **vpTag ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;
  xbMdxTag *tte = NULL;

  // std::cout << "xbIxTdx::CreateTag()\n";
  // std::cout << "CreateTag() name=[" << sName.Str() << "] key=[" << sKey.Str() << "] sFilter=[" << sFilter.Str() << "]\n"; 
  // std::cout << "TagUseCnt = " << iTagUseCnt << std::endl;


  try{
    // verify room for new tag
    if( !( iTagUseCnt < 47 )){
      iErrorStop = 100;
      iRc = XB_LIMIT_REACHED;
      throw iRc;
    }

    // verify valid tag name
    xbString sWorker = sName;
    sWorker.Trim();
    if( sWorker.Len() > 10 ){
      iErrorStop = 110;
      iRc = XB_INVALID_TAG;
      throw iRc;
    }

    // verify tag not already defined
    if( iTagUseCnt > 0 ){
      if( GetTag( sWorker )){
        iErrorStop = 120;
        iRc = XB_INVALID_TAG;
        throw iRc;
      }
    }

    // allocate a tag structure here
    if(( tte = (xbMdxTag *) calloc( 1, (size_t) sizeof( xbMdxTag ))) == NULL ){
      iErrorStop = 130;
      iRc = XB_NO_MEMORY;
      throw iRc;
    }
    *vpTag = tte;
    tte->sTagName = new xbString( sWorker );

    //set up the key expression
    sWorker = sFilter;
    sWorker.Trim();
    if( sWorker.Len() > 0 ){
      if( sWorker.Len() == 0 || sWorker.Len() > 220 ){
        iRc = XB_INVALID_TAG;
        iErrorStop = 140;
        throw iRc;
      }
      tte->sFiltExp = new xbString( sWorker );
      tte->filter = new xbExp( dbf->GetXbasePtr());
      if(( iRc = tte->filter->ParseExpression( dbf, sWorker )) != XB_NO_ERROR ){
        iErrorStop = 150;
        throw iRc;
      }

      //  tte->filter->DumpTree( 1 );

      if((tte->filter->GetReturnType()) != 'L' ){
        iRc = XB_INVALID_TAG;
        iErrorStop = 160;
        throw iRc;
      }
      tte->cHasFilter = 0x01;
    }

    //set up the key expression
    sWorker = sKey;
    sWorker.Trim();
    if( sWorker.Len() == 0 || sWorker.Len() > 100 ){
      iRc = XB_INVALID_TAG;
      iErrorStop = 170;
      throw iRc;
    }
    tte->sKeyExp = new xbString( sWorker );
    tte->exp = new xbExp( dbf->GetXbasePtr());
    if(( iRc = tte->exp->ParseExpression( dbf, sWorker )) != XB_NO_ERROR ){
      iErrorStop = 180;
      throw iRc;
    }

    xbDate d;
    d.Sysdate();
    if( iTagUseCnt == 0 ){
      // first tag, new mdx file
      // create the file name

      // create temp file
      xbString sIxFileName;
      if(( iRc = CreateUniqueFileName( GetTempDirectory(), "TDX", sIxFileName )) != XB_NO_ERROR ){
        iErrorStop = 190;
        throw iRc;
      }

      // copy the file name to the class variable
      this->SetFileName( sIxFileName );
      if( FileExists() && !iOverlay ){
        iErrorStop = 200;
        iRc = XB_FILE_EXISTS;
        throw iRc;
      }

      // first tag, need to create the file
      if(( iRc = xbFopen( "w+b", dbf->GetShareMode())) != XB_NO_ERROR ){
        iErrorStop = 210;
        throw iRc;
      }
      cVersion         = 2;
      cCreateYY        = (char) d.YearOf() - 1900;
      cCreateMM        = (char) d.MonthOf();
      cCreateDD        = (char) d.DayOf( XB_FMT_MONTH );

      GetFileNamePart( sFileName );
      sFileName.ToUpperCase();

      SetBlockSize( (xbUInt32) dbf->GetCreateMdxBlockSize());
      iBlockFactor     = GetBlockSize() / 512;

      cProdIxFlag      = 0;              // MDX is 1
      cTagEntryCnt     = 48;
      iTagLen          = 32;
      ulPageCnt        = 4;
      ulFirstFreePage  = 0;
      ulNoOfBlockAvail = 0;
      cNextTag         = 1;
      c1B              = 0x1B;
      cUpdateYY        = cCreateYY;
      cUpdateMM        = cCreateMM;
      cUpdateDD        = cCreateDD;

      if(( iRc = WriteHeadBlock( 0 )) != XB_NO_ERROR ){
        iErrorStop = 220;
        throw iRc;
      }
    }

    // populate the tag table entry structure
    tte->ulTagHdrPageNo = ulPageCnt;
    ulPageCnt += (xbUInt32) iBlockFactor;
    tte->sTagName->strncpy( tte->cTagName, 10 );

    // cKeyFmt is always 0x10;
    // tested 2+ZIPCD  CITY+STATE  or just standalone field - always 0x10
    tte->cKeyFmt = 0x10;        //  = CalcTagKeyFmt( *tte->exp );

    switch( tte->exp->GetReturnType()){
      case XB_EXP_CHAR:
        tte->cKeyType    = 'C';
        tte->iKeyLen     = tte->exp->GetResultLen();
        tte->iSecKeyType = 0;
        break;

      case XB_EXP_NUMERIC:
        tte->cKeyType    = 'N';
        tte->iKeyLen     = 12;
        tte->iSecKeyType = 0;
        break;

      case XB_EXP_DATE:
        tte->cKeyType    = 'D';
        tte->iKeyLen     = 8;
        tte->iSecKeyType = 1;
        break;

      default:
        iErrorStop = 200;
        iRc = XB_INVALID_INDEX;
        throw iRc;
    }

    tte->cpKeyBuf  = (char *) malloc( (size_t) tte->iKeyLen + 1 );
    tte->cpKeyBuf2 = (char *) malloc( (size_t) tte->iKeyLen + 1 );

//    if( 0 ){
//      printf( "ulTagHdrPageNo=[%d] cTagName=[%-11s], cLeftChild=[%d] cRightChild=[%d] cParent=[%d] cKeyType=[%c]\n\n", 
//         tte->ulTagHdrPageNo, tte->cTagName, tte->cLeftChild, tte->cRightChild, tte->cParent, tte->cKeyType );
//    }

    // write the new tte entry here
    char tteBuf[21];
    memset( tteBuf, 0x00, 21 );

    ePutUInt32( &tteBuf[0], tte->ulTagHdrPageNo );
    for( xbUInt32 l = 0; l < tte->sTagName->Len() && l < 10; l++ ){
      tteBuf[l+4] = tte->sTagName->GetCharacter(l+1);
    }
    tteBuf[15] = tte->cKeyFmt;
    tteBuf[19] = 0x02;                // appears to always be a 0x02
    tteBuf[20] = tte->cKeyType;

    if(( iRc = xbFseek( (iTagUseCnt * 32) + 544, SEEK_SET )) != XB_NO_ERROR ){
      iErrorStop = 230;
      throw iRc;
    }
    if(( iRc = xbFwrite( tteBuf, 21, 1 )) != XB_NO_ERROR ){
      iErrorStop = 240;
      throw iRc;
    }


    // Begin Tag Header
    tte->ulRootPage   = ulPageCnt;
    tte->ulTagSize    = (xbUInt32) iBlockFactor;
    ulPageCnt        += 2;
    tte->cKeyFmt2     = 0x10;
    if( iDescending )
      tte->cKeyFmt2  += 0x08;
    if( iUnique ){
      tte->cKeyFmt2  += 0x40;
      tte->cUnique    = 0x01;
    }

    tte->cTag11       = 0x1B;    // always 0x1b ?
    tte->cSerialNo    = 0x01;    // version incremented with each tag update
    tte->ulLeftChild  = tte->ulRootPage;
    tte->ulRightChild = tte->ulRootPage;

    tte->cTagYY       = (char) d.YearOf() - 1900;
    tte->cTagMM       = (char) d.MonthOf();
    tte->cTagDD       = (char) d.DayOf( XB_FMT_MONTH );

    tte->cKeyType2    = tte->cKeyType;
    tte->iKeyItemLen  = tte->iKeyLen + 4;
    while(( tte->iKeyItemLen % 4 ) != 0 ) tte->iKeyItemLen++;

    tte->iKeysPerBlock = (xbInt16) (GetBlockSize() - 12) / tte->iKeyItemLen;

    //std::cout << "-------------- create tag info\n";
    //std::cout << "keylen=" << tte->iKeyLen << " iKeyItemLen = " << tte->iKeyItemLen << " keys per block calc = " << tte->iKeysPerBlock << "\n";

    tte->cKeyFmt3 = CalcTagKeyFmt( *tte->exp );

//  printf( "ulRootPage=[%d] cKeyFmt2=[%d] cKeyType2=[%d] iKeyLen=[%d]iKeysPerBlock=[%d]\n", tte->ulRootPage, tte->cKeyFmt2, tte->cKeyType2, tte->iKeyLen,  tte->iKeysPerBlock );
//  printf( "iSecKeyType=[%d] iKeyItemLen=[%d] cUnique=[%d] \n", tte->iSecKeyType, tte->iKeyItemLen, tte->cUnique );

    char *pBuf;
    if(( pBuf = (char *) calloc( 1, (size_t) GetBlockSize())) == NULL ){
      iErrorStop = 230;
      iRc = XB_NO_MEMORY;
      throw iRc;
    }
    char *wPtr;
    wPtr = pBuf;
    ePutUInt32( wPtr, tte->ulRootPage );

    wPtr  += 4;
    ePutUInt32( wPtr, tte->ulTagSize );

    wPtr  += 4;
    *wPtr  = tte->cKeyFmt2;

    wPtr++;
    *wPtr  = tte->cKeyType2;

    wPtr  += 2;
    *wPtr  = tte->cTag11;

    wPtr  += 1;
    ePutInt16( wPtr, tte->iKeyLen );

    wPtr += 2;
    ePutInt16( wPtr, tte->iKeysPerBlock );

    wPtr += 2;
    ePutInt16( wPtr, tte->iSecKeyType );

    wPtr += 2;
    ePutInt16( wPtr, tte->iKeyItemLen );

    wPtr += 2;
    *wPtr = tte->cSerialNo;

    wPtr += 3;
    *wPtr = tte->cUnique;

    wPtr++;
    for( xbUInt32 l = 0; l < tte->sKeyExp->Len(); l++ )
      *wPtr++ = tte->sKeyExp->GetCharacter(l+1);

    wPtr = pBuf;

    tte->cHasKeys = 0x00;
    pBuf[246] = tte->cHasKeys;

    wPtr += 248;
    ePutUInt32( wPtr, tte->ulLeftChild );
    wPtr += 4;
    ePutUInt32( wPtr, tte->ulRightChild );

    pBuf[257] = tte->cTagYY;
    pBuf[258] = tte->cTagMM;
    pBuf[259] = tte->cTagDD;
    pBuf[480] = tte->cKeyFmt3;

    if( sFilter.Len() > 0 ){
      pBuf[245] = tte->cHasFilter;
      wPtr = pBuf;
      wPtr += 762;
      for( xbUInt32 l = 0; l < sFilter.Len(); l++ )
        *wPtr++ = sFilter.GetCharacter(l+1);
    }

    if(( iRc = xbFseek( tte->ulTagHdrPageNo * 512, SEEK_SET )) != XB_NO_ERROR ){
      iErrorStop = 250;
      throw iRc;
    }

    if(( iRc = xbFwrite( pBuf, GetBlockSize(), 1 )) != XB_NO_ERROR ){
      iErrorStop = 260;
      throw iRc;
    }

    memset( pBuf, 0x00, GetBlockSize() );
    if(( iRc = xbFwrite( pBuf, GetBlockSize(), 1 )) != XB_NO_ERROR ){
      iErrorStop = 270;
      throw iRc;
    }

    iTagUseCnt++;
    cNextTag++;


    if(( iRc = WriteHeadBlock( 1 )) != XB_NO_ERROR ){
      iErrorStop = 280;
      throw iRc;
    }

    // add the new entry to the end of the list of tags
    if( mdxTagTbl == NULL ){
      mdxTagTbl = tte;
    } else {
      xbMdxTag *tteL = mdxTagTbl;
      while( tteL->next )
        tteL = tteL->next;
      tteL->next = tte;
    }

    /* update the btree pointers */
    CalcBtreePointers();
    char bBuf[3];
    xbMdxTag *tteWork = mdxTagTbl;

    if(( iRc = xbFseek( 560, SEEK_SET )) != XB_NO_ERROR ){
      iErrorStop = 290;
      throw iRc;
    }
    while( tteWork ){
      bBuf[0] = tteWork->cLeftChild;
      bBuf[1] = tteWork->cRightChild;
      bBuf[2] = tteWork->cParent;

      if(( iRc = xbFwrite( bBuf, 3, 1 )) != XB_NO_ERROR ){
        iErrorStop = 300;
        throw iRc;
      }
      if( tteWork->next ){
        if(( iRc = xbFseek( 29, SEEK_CUR )) != XB_NO_ERROR ){
          iErrorStop = 310;
          throw iRc;
        }
      }
      tteWork = tteWork->next;
    }
    free( pBuf );

  }

  catch (xbInt16 iRc ){
    if( tte ){
      if( tte->cpKeyBuf )
        free( tte->cpKeyBuf );
      if( tte->cpKeyBuf2 )
        free( tte->cpKeyBuf2 );
      if( tte->exp )
        delete tte->exp;
      if( tte->filter )
        delete tte->filter;
      if( tte->sKeyExp )
        delete tte->sKeyExp;
      if( tte->sFiltExp )
        delete tte->sFiltExp;
      if( tte->sTagName )
        delete tte->sTagName;
      free( tte );
    }
    xbString sMsg;
    sMsg.Sprintf( "xbIxTdx::CreateTag() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }

  return iRc;
};

/***********************************************************************/
//! @brief Delete a given tag
/*!
  \param vpTag Input tag ptr for tag to be deleted<br>
  \returns <a href="xbretcod_8h.html">Return Codes</a><br>
            1 = Deleted entire MDX file, only had one tag

*/

xbInt16 xbIxTdx::DeleteTag( void *vpTag ){

  xbInt16  iRc        = 0;
  xbInt16  iErrorStop = 0;
  xbMdxTag * mpTag    = (xbMdxTag *) vpTag;
  xbIxNode *n         = NULL;
  xbBool   bLoneTag   = xbFalse;

  try{

    if( !vpTag ){
      iErrorStop = 100;
      iRc = XB_INVALID_TAG;
      throw iRc;
    }

    // char cSaveHasFilter = mpTag->cHasFilter;
    // char cSaveKeyFmt3   = mpTag->cKeyFmt3;
    // xbString sSaveKey   = mpTag->sKeyExp->Str();

    if( iTagUseCnt == 1 ){
      // std::cout << "xbIxTdx::DeleteTag - one tag found, delete the mdx file\n";

      // close the mdx file
      if(( iRc = xbIxMdx::Close()) != XB_NO_ERROR ){
        iErrorStop = 110;
        throw iRc;
      }

      // delete the file
      xbRemove();

      // init variables - needed?
      // Init();
      // iRc > 0 defines this as the only tag in an MDX file, MDX file deleted.
      //         signals to the calling process to drop the MDX file from the
      //         list of updateable indices.
      bLoneTag = xbTrue;

    } else {

      //    harvest tag nodes

      if(( iRc = HarvestTagNodes( mpTag, xbTrue )) != XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }

      // remove an entry from tag table
      //   which tag is this?
      xbInt16  iTagNo  = 0;
      xbMdxTag *mp     = mdxTagTbl;
      xbMdxTag *mpPrev = NULL;
      while( mp && mp->ulTagHdrPageNo != mpTag->ulTagHdrPageNo ){
        iTagNo++;
        mpPrev = mp;
        mp = mp->next;
      }

      // remove it from the linked list of tags
      if( !mpPrev ){
        mdxTagTbl = mp->next;
      } else {
        mpPrev->next = mp->next;
      }
      if( mp ){
        if( mp->cpKeyBuf )  free(  mp->cpKeyBuf );
        if( mp->cpKeyBuf2 ) free(  mp->cpKeyBuf2 );
        if( mp->exp )       delete mp->exp;
        if( mp->filter )    delete mp->filter;
        if( mp->sKeyExp )   delete mp->sKeyExp;
        if( mp->sFiltExp )  delete mp->sFiltExp;
        if( mp->sTagName )  delete mp->sTagName;
        free( mp );
      }
      xbInt32 iTarg = iTagNo * 32;
      xbInt32 iSrc  = iTarg  + 32;
      xbInt32 iLen  = (iTagUseCnt - iTagNo) * 32;

      if(( iRc = xbFseek( 544, SEEK_SET )) != XB_NO_ERROR ){
        iErrorStop = 160;
        throw iRc;
      }
      char Buf[1536];  // 47 tags + 1 in case tag #47 is deleted
      memset( Buf, 0x00, 1536 );
      if(( iRc = xbFread( Buf, 1504, 1 )) != XB_NO_ERROR ){
        iErrorStop = 170;
        throw iRc;
      }
      char *pTrg = Buf;
      pTrg += iTarg;
      char *pSrc = Buf;
      pSrc += iSrc;
      for( xbInt32 i = 0; i < iLen; i++ )
        *pTrg++ = *pSrc++;

      if(( iRc = xbFseek( 544, SEEK_SET )) != XB_NO_ERROR ){
        iErrorStop = 180;
        throw iRc;
      }
      if(( iRc = xbFwrite( Buf, 1504, 1 )) != XB_NO_ERROR ){
        iErrorStop = 190;
        throw iRc;
      }

      iTagUseCnt--;
      if(( iRc = WriteHeadBlock( 1 )) != XB_NO_ERROR ){
        iErrorStop = 200;
        throw iRc;
      }

      // update the btree pointers
      CalcBtreePointers();
      char bBuf[3];
      xbMdxTag *tteWork = mdxTagTbl;

      if(( iRc = xbFseek( 560, SEEK_SET )) != XB_NO_ERROR ){
        iErrorStop = 210;
        throw iRc;
      }
      while( tteWork ){
        bBuf[0] = tteWork->cLeftChild;
        bBuf[1] = tteWork->cRightChild;
        bBuf[2] = tteWork->cParent;

        if(( iRc = xbFwrite( bBuf, 3, 1 )) != XB_NO_ERROR ){
          iErrorStop = 320;
          throw iRc;
        }
        if( tteWork->next ){
          if(( iRc = xbFseek( 29, SEEK_CUR )) != XB_NO_ERROR ){
            iErrorStop = 330;
            throw iRc;
          }
        }
        tteWork = tteWork->next;
      }
    }

  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbIxTdx::DeleteTag() Exception Caught. Error Stop = [%d] iRc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
    if( n )
      free( n );
  }
  if( bLoneTag && !iRc )
    return 1;
  else
    return iRc;
}

/************************************************************************/



/************************************************************************/
}   /* namespace */ 
#endif         /*  XB_TDX_SUPPORT */