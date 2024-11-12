/*  xbrelate.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

#include "xbase.h"

#ifdef XB_RELATE_SUPPORT

namespace xb{

/************************************************************************/
xbRelate::xbRelate( xbXBase *x ) {
  this->xbase = x;
}
/************************************************************************/
xbRelate::~xbRelate() {

  xbase->RemoveRelFromRelList( this );

  // clear the memory in the node tree  
  xbLinkListNode<xbRelation *> *lln = GetLastTreeNode( NULL );
  xbRelation *pRel;

  if( lln ){
    xbBool bDone = xbFalse;

    while( lln != NULL && !bDone ){

      pRel = lln->GetKey();
      if( pRel->pParentExp )
        delete pRel->pParentExp;
      delete pRel->sParentExpression;

      if( pRel->sFilter )
        delete pRel->sFilter;

      if( pRel->pFilt )
        delete pRel->pFilt;

      if( pRel->pParent ){
        pRel->pParent->llChildren.RemoveFromEnd( pRel );
        free( pRel );

      } else {
        if( llQryTree.GetNodeCnt() > 0 ){
          llQryTree.RemoveFromEnd( pRel );
          free( pRel );
          bDone = xbTrue;
        }
      }
      lln = GetLastTreeNode( NULL );
    }
  }
  while( llQryList.GetNodeCnt() > 0 ){
    llQryList.RemoveFromFront( pRel );
  }
}
/************************************************************************/
xbInt16 xbRelate::AddRelation( xbDbf *dParent, const xbString &sParentExpression,
      xbDbf *dChild,  const xbString &sChildTagName, char cType,  const xbString &sFilter ){

  xbInt16    iRc              = XB_NO_ERROR;
  xbInt16    iErrorStop       = 0;
  try{
    xbIx * pIx;
    void * pTag;

    if(( iRc = dChild->GetTagPtrs( sChildTagName,  &pIx, &pTag )) != XB_NO_ERROR ) {
      iErrorStop = 100;
      throw iRc;
    }
    if(( iRc = AddRelation( dParent, sParentExpression, dChild, pIx, pTag, cType, sFilter )) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbRelate::AddRelation(1) Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
/************************************************************************/
xbInt16 xbRelate::AddRelation( xbDbf *dParent, const xbString &sParentExpression,
  xbDbf *dChild, xbIx * pChildIx, void *pChildTag, char cType, const xbString &sFilter ){

  xbInt16    iRc              = XB_NO_ERROR;
  xbInt16    iErrorStop       = 0;
  xbRelation *pChildRelation  = NULL;
  xbRelation *pParentRelation = NULL;

  try{

    if( cType != 'O' && cType != 'R' ){
      iErrorStop = 100;
      iRc = XB_INVALID_OPTION;
      throw iRc;
    }
    if( !dParent || !dChild ){
      iRc = XB_NOT_OPEN;
      iErrorStop = 110;
      throw iRc;
    }
    if( dParent->GetDbfStatus() != XB_OPEN || dChild->GetDbfStatus() != XB_OPEN ){
      iRc = XB_NOT_OPEN;
      iErrorStop = 120;
      throw iRc;
    }
    if( sParentExpression.Len() == 0 ){
      iRc = XB_INVALID_OPTION;
      iErrorStop = 130;
      throw iRc;
    }
    if( !pChildIx ){
      iRc = XB_INVALID_INDEX;
      iErrorStop = 140;
      throw iRc;
    }
    if( !pChildTag ){
      iRc = XB_INVALID_TAG;
      iErrorStop = 150;
      throw iRc;
    }
    //if( ChkTblSts( dChild )){
    if( xbase->CheckRelListForTblUse( dChild ) != XB_NO_ERROR ){
      iRc = XB_ALREADY_DEFINED;
      iErrorStop = 160;
      throw iRc;
    }
    // find the parent node to add it to
    xbLinkListNode<xbRelation *> *pNode = FindNodeForDbf( dParent );
    if( !pNode ){
      iErrorStop = 170;
      throw iRc;
    }
    pParentRelation = pNode->GetKey();
    // allocate an xbRelation structure
    if(( pChildRelation = (xbRelation *) calloc( 1, (size_t) sizeof( xbRelation ))) == NULL ){
      iRc = XB_NO_MEMORY;
      iErrorStop = 180;
      throw iRc;
    }
    // allocate / parse Parent Expression
    pChildRelation->pParentExp = new xbExp( xbase );
    if(( iRc = pChildRelation->pParentExp->ParseExpression( dParent, sParentExpression )) != XB_NO_ERROR ){
      iErrorStop = 190;
      throw iRc;
    }

    pChildRelation->dDbf              = dChild;
    pChildRelation->pIx               = pChildIx;
    pChildRelation->pTag              = pChildTag;
    pChildRelation->cType             = cType;
    pChildRelation->pParent           = pParentRelation;
    pChildRelation->pParentNp         = pNode;
    pChildRelation->sParentExpression = new xbString( sParentExpression );

    if( strlen( sFilter ) > 0 ){
      pChildRelation->sFilter = new xbString( sFilter );
      pChildRelation->pFilt = new xbFilter( dChild );
      if(( iRc = pChildRelation->pFilt->Set( sFilter, pChildIx, pChildTag )) != XB_NO_ERROR ){
        iErrorStop = 200;
        throw iRc;
      }
    }
    pParentRelation->llChildren.InsertAtEnd( pChildRelation );
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbRelate::AddRelation(2) Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
    if( pChildRelation && pChildRelation->pParentExp )
      delete pChildRelation->pParentExp;
    if( pChildRelation ){
      if( pChildRelation->sFilter )
        delete pChildRelation->sFilter;
      if( pChildRelation->pFilt )
        delete pChildRelation->pFilt;
      free( pChildRelation );
    }
  }
  return iRc;
}

/************************************************************************/
xbInt16 xbRelate::CheckTblSts( xbDbf *d ){
  // returns XB_ALREADY_DEFINED if the table has already been assigned to a relation in list

  if( llQryList.GetNodeCnt() == 0 )
    return XB_NO_ERROR;

  xbLinkListNode<xbRelation *> *pNode = llQryList.GetHeadNode();
  xbRelation *pRel = NULL;
  xbInt16 iRc = XB_NO_ERROR;
  while( pNode && iRc == XB_NO_ERROR  ){
    pRel = pNode->GetKey();
    if( pRel->dDbf == d ){ 
      iRc = XB_ALREADY_DEFINED;
    }
    pNode = pNode->GetNextNode();
  }
  return iRc;
}
/************************************************************************/
xbInt16 xbRelate::CompareKeys( xbRelation *pRel, xbString &s1, xbString &s2, xbDouble &d1, xbDouble &d2, xbDate &dt1, xbDate &dt2 ){

  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;
  try{
    char cParType = pRel->pParentExp->GetReturnType();
    if( cParType == XB_EXP_CHAR ){
      if( s1 != s2 )
        iRc = XB_NOT_FOUND;
    } else if( cParType == XB_EXP_NUMERIC ){
      if( d1 != d2 )
        iRc = XB_NOT_FOUND;
    } else if( cParType == XB_EXP_DATE ){
      if( dt1 != dt2 )
        iRc = XB_NOT_FOUND;
    } else {
      iErrorStop = 100;
      iRc = XB_INVALID_OBJECT;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbRelate::CompareKeys( xbString ) Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
/************************************************************************/
#ifdef XB_DEBUG_SUPPORT

xbInt16 xbRelate::CheckRecNo( xbInt16 iPo, xbInt32 lRec0, xbInt32 lRec1,
                    xbInt32 lRec2, xbInt32 lRec3, xbInt32 lRec4,
                    xbInt32 lRec5, xbInt32 lRec6, xbInt32 lRec7 ){

  // if empty list, but expecting data, return -1
  if( llQryList.GetNodeCnt() == 0 && lRec0 > -1 )
    return -1;

  xbInt16 iErrorCtr = 0;
  xbLinkListNode<xbRelation *> *pNode = llQryList.GetHeadNode();
  xbRelation * pRel = NULL;

  xbInt16 iLc = 0;
  xbInt32 lRecNo = -1;

  xbBool bDone = xbFalse;
  while( !bDone ){

    switch( iLc ){
     case 0:  lRecNo = lRec0; break;
     case 1:  lRecNo = lRec1; break;
     case 2:  lRecNo = lRec2; break;
     case 3:  lRecNo = lRec3; break;
     case 4:  lRecNo = lRec4; break;
     case 5:  lRecNo = lRec5; break;
     case 6:  lRecNo = lRec6; break;
     case 7:  lRecNo = lRec7; break;
     default: lRecNo = -1;    break;
    }

    if( iLc > 7 )
      bDone = xbTrue;

    else if( !pNode ){
      if( lRecNo > -1 ) 
        iErrorCtr--;
      bDone = xbTrue;

    } else {

      pRel = pNode->GetKey();
      xbString sTblName;
      xbString sMsg;

      if( lRecNo > -1 ){
        if( (xbInt32) pRel->dDbf->GetCurRecNo() != lRecNo ){
          if( iPo == 2 ){   // print option from error routine logic in test programs
            pRel->dDbf->GetFileNamePart( sTblName );
            sMsg.Sprintf( "Table: [%s] Node: [%d]  Expecting: [%d]  CurDbfRec: [%d]\n", 
              sTblName.Str(), iLc, lRecNo, pRel->dDbf->GetCurRecNo() );
        //    std::cout << sMsg.Str();
          }
          iErrorCtr--;
        }
      }
      pNode = pNode->GetNextNode();
    }
    iLc++;
  }
  return iErrorCtr;
}

void xbRelate::DumpRelationList(){

  std::cout << "DumpRelationList()"<< std::endl;

  xbLinkListNode<xbRelation *> *pNode = NULL;
  xbRelation * pRelation = NULL;
  //pNode = GetNextTreeNode( pNode );

  pNode = llQryList.GetHeadNode();

  xbString sMsg;
  xbString sFileName;
  xbString sParentFileName;
  xbString sTagName;

  xbInt16 iCtr = 1;

  sMsg.Sprintf( "Cnt\tType\tStatus\tRecPos\tCurRec\tParRec\tTable   \tTagName  \tParent   \tPar Expression\n" );
  std::cout << sMsg.Str();

  sMsg.Sprintf( "===\t====\t=====\t======\t======\t=======\t========\t==============\t========\t==============\n" );
  std::cout << sMsg.Str();

  while( pNode ){
    pRelation = pNode->GetKey();
    pRelation->dDbf->GetFileNamePart( sFileName );


    if( pRelation->cType == 'M' ){
      sParentFileName = "Root    ";
      sTagName = "N/A";

    } else {
      pRelation->pParent->dDbf->GetFileNamePart( sParentFileName );
      sTagName = pRelation->pIx->GetTagName( pRelation->pTag );
    }
    sTagName.PadRight( ' ', 10 );
    sMsg.Sprintf( "%d\t%c\t%d\t%d\t%d\t%d\t%-8s\t%-12s \t%-8s \t%s \n",
        iCtr++, pRelation->cType,
        pRelation->iStatus, pRelation->ulRecPos, 
        pRelation->dDbf->GetCurRecNo(),
        pRelation->ulSaveParRecNo,
        sFileName.Str(), sTagName.Str(), sParentFileName.Str(),
        pRelation->sParentExpression->Str());

    std::cout << sMsg.Str();

    if( pRelation->sFilter ){
      sMsg.Sprintf( "\tFilter: [%s]\n", pRelation->sFilter->Str() );
      std::cout << sMsg.Str();
    }
    pNode = pNode->GetNextNode();
  }
}

void xbRelate::DumpRelationNode( const xbString &sTitle, xbRelation *pRel ){

  xbString sDbfFileName;
  xbString sMdxFileName;
  xbString sTagName;

  if( pRel->pTag )
    sTagName = pRel->pIx->GetTagName( pRel->pTag );
  else
    sTagName = "No Tag";

  if( pRel->pIx )
    pRel->pIx->GetFileNamePart( sMdxFileName );
  else
    sMdxFileName = "No Mdx";

  if( pRel ){
    pRel->dDbf->GetFileNamePart( sDbfFileName );
    std::cout << sTitle << " DBF=[" << sDbfFileName << "] MDX=[" << sMdxFileName << "] Tag=[" << sTagName.Str() << "]" << std::endl;
    std::cout << "Type     [" << pRel->cType << "] Status [" << pRel->iStatus << "]\n";
    std::cout << "ulRecPos [" << pRel->iStatus << "] CurRecNo [" << pRel->dDbf->GetCurRecNo() << "] ulParRecNo [" << pRel->ulSaveParRecNo << "]\n";
  } else { 
    std::cout << sTitle << " - null node\n";
  }

}

void xbRelate::DumpRelationTree(){

  std::cout << "DumpRelationTree()"<< std::endl;

  xbLinkListNode<xbRelation *> *pNode = NULL;
  xbRelation * pRelation = NULL;
  pNode = GetNextTreeNode( pNode );

  xbString sMsg;
  xbString sFileName;
  xbString sParentFileName;
  xbString sTagName;

  xbInt16 iCtr = 1;

  sMsg.Sprintf( "Cnt\tType\tTable\tTagName  \tParent   \tPar Expression\n" );
  std::cout << sMsg.Str();

  sMsg.Sprintf( "===\t====\t=====\t========\t==========\t==============\n" );
  std::cout << sMsg.Str();

  while( pNode ){
    pRelation = pNode->GetKey();
    pRelation->dDbf->GetFileNamePart( sFileName );


    if( pRelation->cType == 'M' ){
      sParentFileName = "Root     ";
      sTagName = "N/A";
    } else {
      pRelation->pParent->dDbf->GetFileNamePart( sParentFileName );
      sTagName = pRelation->pIx->GetTagName( pRelation->pTag );
    }

    sTagName.PadRight( ' ', 10 );
    sMsg.Sprintf( "%d\t%c\t%s\t%s\t%-8s\t%s\n", iCtr++, pRelation->cType, sFileName.Str(), sTagName.Str(), sParentFileName.Str(), pRelation->sParentExpression->Str() );
    std::cout << sMsg.Str();

    if( pRelation->sFilter ){
      sMsg.Sprintf( "\tFilter: [%s]\n", pRelation->sFilter->Str() );
      std::cout << sMsg.Str();
    }

    pNode = GetNextTreeNode( pNode );
  }
}

#endif  // XB_DEBUG_SUPPORT
/************************************************************************/
xbLinkListNode<xbRelation *> *xbRelate::FindNodeForDbf( xbDbf * d ){

  xbBool     bFound     = xbFalse;

  xbLinkListNode<xbRelation *> *pNode = NULL;
  xbRelation *pRel = NULL;

  pNode = GetNextTreeNode( pNode );
  while( pNode && !bFound ){

    pRel = pNode->GetKey();

    if( d == pRel->dDbf )
      bFound = xbTrue;
    else
      pNode = GetNextTreeNode( pNode );
  }

  return pNode;
}

/************************************************************************/
xbInt16 xbRelate::GetFirstRecForNode( xbLinkListNode<xbRelation *> *lln ){

  xbInt16    iRc        = XB_NO_ERROR;
  xbInt16    iErrorStop = 0;
  xbRelation *pRel      = NULL;

  try{
    if( !lln ){
      iRc = XB_INVALID_OBJECT;
      iErrorStop = 100;
      throw iRc;
    }

    pRel = lln->GetKey();
    pRel->iStatus = XB_NO_ERROR;

    if( pRel->cType == 'M' ){     // master
      if( pRel->pFilt ){
        if(( iRc = pRel->pFilt->GetFirstRecord( XB_ACTIVE_RECS )) != XB_NO_ERROR ){
          iErrorStop = 110;
          throw iRc;
        }
      } else {
        if(( iRc = pRel->dDbf->GetFirstRecord( XB_ACTIVE_RECS )) != XB_NO_ERROR ){
          iErrorStop = 120;
          throw iRc;
        }
      }
      pRel->iStatus = iRc;

    } else {   // child relation

      xbString sKey;
      xbDouble dKey;
      xbDate   dtKey;
      if(( iRc = GetParentRelKeys( pRel, sKey, dKey, dtKey )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }

      pRel->ulSaveParRecNo = pRel->pParent->dDbf->GetCurRecNo();

      if(( iRc = RelKeySearch( pRel, 'F', sKey, dKey, dtKey )) != XB_NO_ERROR ){
        iErrorStop = 140;
        throw iRc;
      }
    }
    if( pRel->iStatus == 0 )
      pRel->ulRecPos = 1;
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    if( iRc != XB_NOT_FOUND && iRc != XB_EOF && iRc != XB_EMPTY ){
      sMsg.Sprintf( "xbRelate::GetFirstRecForNode() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, iRc );
      xbase->WriteLogMessage( sMsg.Str() );
      xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
    }
    if( pRel ){
      if(( iRc = pRel->dDbf->BlankRecord()) != XB_NO_ERROR ){
        sMsg.Sprintf( "xbRelate::GetFirstRecForNode() Exception Caught. Error Stop = [500] rc = [%d]", iRc );
        xbase->WriteLogMessage( sMsg.Str() );
        xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
      }
    }
  }
  return iRc;
}
/************************************************************************/
xbInt16 xbRelate::GetFirstRelRecord(){

  xbInt16    iRc        = XB_NO_ERROR;
  xbInt16    iErrorStop = 0;
  xbInt16    iMctr = 0;
  xbBool     bDone      = xbFalse;
  xbRelation *pRel      = NULL;
  char       cDir       = 'R';         // direction to proceed on the node list

  xbLinkListNode<xbRelation *> *lln;

  try{
    // if empty list, need to build it
    if( llQryList.GetNodeCnt() == 0 )
      InitQuery();

    // clear any previous positioning
    lln = llQryList.GetHeadNode();
    while( lln ){
      pRel = lln->GetKey();
      if( pRel->dDbf->GetCurRecNo() != 0 )
        pRel->dDbf->BlankRecord();
      pRel->ulSaveParRecNo = 0;
      pRel->iStatus  = 0;
      pRel->ulRecPos = 0;
      lln = lln->GetNextNode();
    }

    lln = llQryList.GetHeadNode();
    while( lln && !bDone ){
      pRel = lln->GetKey();
      // std::cout << "gfrr1 type = " << pRel->cType << " pRel->iStatus = " << pRel->iStatus << " rec no = " << pRel->dDbf->GetCurRecNo() << "\n";

      if( pRel->cType == 'M' && iMctr == 0 ){
        iMctr++;
        if(( iRc = GetFirstRecForNode( lln )) != XB_NO_ERROR ){
          iErrorStop = 100;
          throw iRc;
        }
      } else {
        if(( iRc = GetNextRecForNode( lln )) != XB_NO_ERROR ){
          iErrorStop = 110;
          throw iRc;
        }
      }

      if( pRel->iStatus == XB_NO_ERROR ){
        if( cDir == 'L' ){
          cDir = 'R';
        }

      } else {    // record not found for last node
        if( pRel->cType == 'M' )
          bDone = xbTrue;

        else if( cDir == 'R' && pRel->cType == 'R' )
          cDir = 'L';
      }
      if( !bDone ){
        if( cDir == 'L' )
           lln = lln->GetPrevNode();
        else
           lln = lln->GetNextNode();
      }
      // std::cout << "gfrr2 type  = " << pRel->cType << " pRel->iStatus = " << pRel->iStatus << " rec no = " << pRel->dDbf->GetCurRecNo() << "\n\n";
    }
  }
  catch (xbInt16 iRc ){
    if( iRc != XB_NOT_FOUND && iRc != XB_EMPTY && iRc != XB_EOF ){
      xbString sMsg;
      sMsg.Sprintf( "xbRelate::GetFirstRelRecord() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, iRc );
      xbase->WriteLogMessage( sMsg.Str() );
      xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
    }
  }
  return iRc;
}

/************************************************************************/
xbInt16 xbRelate::GetLastRecForNode( xbLinkListNode<xbRelation *> *lln ){

  xbInt16    iRc        = XB_NO_ERROR;
  xbInt16    iErrorStop = 0;
  xbRelation *pRel      = NULL;

  try{
    if( !lln ){
      iRc = XB_INVALID_OBJECT;
      iErrorStop = 100;
      throw iRc;
    }

    pRel = lln->GetKey();
    pRel->iStatus = XB_NO_ERROR;

    if( pRel->cType == 'M' ){     // master
      if( pRel->pFilt ){
        if(( iRc = pRel->pFilt->GetLastRecord( XB_ACTIVE_RECS )) != XB_NO_ERROR ){
          iErrorStop = 110;
          throw iRc;
        }
      } else {
        if(( iRc = pRel->dDbf->GetLastRecord( XB_ACTIVE_RECS )) != XB_NO_ERROR ){
          iErrorStop = 120;
          throw iRc;
        }
      }
      pRel->iStatus = iRc;

    } else {   // child relation

      xbString sKey;
      xbDouble dKey;
      xbDate   dtKey;
      if(( iRc = GetParentRelKeys( pRel, sKey, dKey, dtKey )) != XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }
      pRel->ulSaveParRecNo = pRel->pParent->dDbf->GetCurRecNo();

      if(( iRc = RelKeySearch( pRel, 'L', sKey, dKey, dtKey )) != XB_NO_ERROR ){
        iErrorStop = 140;
        throw iRc;
      }

    }
    if( pRel->iStatus == 0 )
      pRel->ulRecPos = -1;
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    if( iRc != XB_NOT_FOUND && iRc != XB_EOF && iRc != XB_EMPTY ){
      sMsg.Sprintf( "xbRelate::GetLastRecForNode() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, iRc );
      xbase->WriteLogMessage( sMsg.Str() );
      xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
    }
    if( pRel ){
      if(( iRc = pRel->dDbf->BlankRecord()) != XB_NO_ERROR ){
        sMsg.Sprintf( "xbRelate::GetLastRecForNode() Exception Caught. Error Stop = [500] rc = [%d]", iRc );
        xbase->WriteLogMessage( sMsg.Str() );
        xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
      }
    }
  }
  return iRc;
}
/************************************************************************/
xbInt16 xbRelate::GetNextRecForNode( xbLinkListNode<xbRelation *> *lln ){

  xbInt16    iRc        = XB_NO_ERROR;
  xbInt16    iErrorStop = 0;
  xbRelation *pRel      = NULL;
  xbString   sParentKey;
  xbDouble   dParentKey;
  xbDate     dtParentKey;
  xbString   sChildKey;
  xbDouble   dChildKey;
  xbDate     dtChildKey;

  // save the key
  // execute next record
  // if this key = prev key
  //      ok
  // else
  //   rc = not found
  //   clear rec buf

  // GetNextRecForNode return codes
  //  RC             pRel->iStatus
  //  ===========    =============
  //   0               0
  //   0             XB_NOT_FOUND, XB_BOF, XB_EOF, XB_EMPTY
  //  fatal error    XB_NOT_FOUND

  try{
    if( !lln ){
      iErrorStop = 100;
      iRc = XB_INVALID_OBJECT;
      throw iRc;
    }

    pRel = lln->GetKey();
    pRel->iStatus = XB_NO_ERROR;

    if( pRel->cType == 'M' ){     // master
      if( pRel->pFilt ) 
        iRc = pRel->pFilt->GetNextRecord( XB_ACTIVE_RECS );
      else
        iRc = pRel->dDbf->GetNextRecord( XB_ACTIVE_RECS );

      if( iRc != XB_NO_ERROR ){
        if( iRc == XB_EOF || iRc == XB_EMPTY ){  // error on fatal errors only
          pRel->iStatus = iRc;
          iRc           = 0;
        } else {
          pRel->iStatus = XB_NOT_FOUND;
          iErrorStop    = 110;
          throw iRc;
        }
      }

    } else {   // child relation

      if( pRel->pParent->dDbf->GetCurRecNo() == 0 ){
        pRel->iStatus = XB_NOT_FOUND;

      } else {

        xbBool bFirstKeyLookup = xbFalse;
        if( pRel->dDbf->GetCurRecNo() == 0 ){
          bFirstKeyLookup = xbTrue;
        } else if( pRel->pParent->dDbf->GetCurRecNo() != pRel->ulSaveParRecNo ){
          bFirstKeyLookup = xbTrue;
        }

        if(( iRc = GetParentRelKeys( pRel, sParentKey, dParentKey, dtParentKey )) != XB_NO_ERROR ){
          iErrorStop = 130;
          throw iRc;
        }

        if( bFirstKeyLookup ){

          pRel->ulSaveParRecNo = pRel->pParent->dDbf->GetCurRecNo();
          if(( iRc = RelKeySearch( pRel, 'F', sParentKey, dParentKey, dtParentKey )) != XB_NO_ERROR ){
            iErrorStop = 140;
            throw iRc;
          }

        } else {

          // else go to next record
          if( pRel->pFilt ){
            iRc = pRel->pFilt->GetNextRecordIx();
          } else {
            iRc = pRel->dDbf->GetNextKey( pRel->pIx, pRel->pTag );   // pulls next key and corresponding record
          }

          if( iRc != XB_NO_ERROR ){
            pRel->iStatus = XB_NOT_FOUND;
            if( iRc != XB_EOF && iRc != XB_EMPTY && iRc != XB_NOT_FOUND ){
              iErrorStop = 140;
              throw iRc;
            } else {
              iRc = 0;
            }
          }

          if( pRel->iStatus == XB_NO_ERROR ){

            // if this key doesn't match prev key, then return XB_NOT_FOUND
            if(( iRc = GetChildRelKeys( pRel, sChildKey, dChildKey, dtChildKey )) != XB_NO_ERROR ){
              iErrorStop = 150;
              throw iRc;
            }
            iRc = CompareKeys( pRel, sChildKey, sParentKey, dChildKey, dParentKey, dtChildKey, dtParentKey );

            if( iRc != XB_NO_ERROR ){
              if( iRc == XB_NOT_FOUND ){
                pRel->iStatus = XB_NOT_FOUND;
                iRc = XB_NO_ERROR;
              } else {
                iErrorStop = 160;
                throw iRc;
              }
            }
          }
        }
      }
      if( pRel->iStatus == 0 )
        pRel->ulRecPos++;

    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    if( pRel ) pRel->iStatus = XB_NOT_FOUND;
    sMsg.Sprintf( "xbRelate::GetNextRecForNode() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }

  if( iRc != XB_NO_ERROR || pRel->iStatus != XB_NO_ERROR ){
    if( pRel ){
      if(( iRc = pRel->dDbf->BlankRecord()) != XB_NO_ERROR ){
        xbString sMsg;
        sMsg.Sprintf( "xbRelate::GetNextRecForNode() Exception Caught. Error Stop = [500] rc = [%d]", iRc );
        xbase->WriteLogMessage( sMsg.Str() );
        xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
      }
    }
  }
  return iRc;
}

/************************************************************************/
xbInt16 xbRelate::GetPrevRecForNode( xbLinkListNode<xbRelation *> *lln ){

  xbInt16    iRc        = XB_NO_ERROR;
  xbInt16    iErrorStop = 0;
  xbRelation *pRel      = NULL;

  xbString   sParentKey;
  xbDouble   dParentKey;
  xbDate     dtParentKey;

  xbString   sChildKey;
  xbDouble   dChildKey;
  xbDate     dtChildKey;


  // save the key
  // execute prev record
  // if this key = prev key
  //      ok
  // else
  //   rc = not found
  //   clear rec buf

  // GetNextRecForNode return codes
  //  RC             pRel->iStatus
  //  ===========    =============
  //   0               0
  //   0             XB_NOT_FOUND, XB_BOF, XB_EOF, XB_EMPTY
  //  fatal error    XB_NOT_FOUND

  try{
    if( !lln ){
      iErrorStop = 100;
      iRc = XB_INVALID_OBJECT;
      throw iRc;
    }

    pRel = lln->GetKey();
    pRel->iStatus = XB_NO_ERROR;

    if( pRel->cType == 'M' ){     // master
      if( pRel->pFilt ) 
        iRc = pRel->pFilt->GetPrevRecord( XB_ACTIVE_RECS );
      else
        iRc = pRel->dDbf->GetPrevRecord( XB_ACTIVE_RECS );

      if( iRc != XB_NO_ERROR ){
        if( iRc == XB_BOF || iRc == XB_EMPTY ){  // error on fatal errors only
          pRel->iStatus = iRc;
          iRc           = 0;
        } else {
          pRel->iStatus = XB_NOT_FOUND;
          iErrorStop    = 110;
          throw iRc;
        }
      }

    } else {   // child relation

      if( pRel->pParent->dDbf->GetCurRecNo() == 0 ){
        pRel->iStatus = XB_NOT_FOUND;

      } else {

        xbBool bFirstKeyLookup = xbFalse;
        if( pRel->dDbf->GetCurRecNo() == 0 ){
          bFirstKeyLookup = xbTrue;
        } else if( pRel->pParent->dDbf->GetCurRecNo() != pRel->ulSaveParRecNo ){
          bFirstKeyLookup = xbTrue;
        }

        if(( iRc = GetParentRelKeys( pRel, sParentKey, dParentKey, dtParentKey )) != XB_NO_ERROR ){
          iErrorStop = 130;
          throw iRc;
        }

        if( bFirstKeyLookup ){

          // std::cout << "gprfn firstkey lookup [" << pRel->dDbf->GetFileName().Str() << "] pa key = [" << sParentKey << "]\n";

          pRel->ulSaveParRecNo = pRel->pParent->dDbf->GetCurRecNo();
          if(( iRc = RelKeySearch( pRel, 'L', sParentKey, dParentKey, dtParentKey )) != XB_NO_ERROR ){
            iErrorStop = 140;
            throw iRc;
          }

        } else {

          // else go to prev record

          if( pRel->pFilt ){
            iRc = pRel->pFilt->GetPrevRecordIx();
          } else {
            iRc = pRel->dDbf->GetPrevKey( pRel->pIx, pRel->pTag );   // pulls prev key and corresponding record
          }

          if( iRc != XB_NO_ERROR ){
            pRel->iStatus = XB_NOT_FOUND;
            if( iRc != XB_BOF && iRc != XB_EMPTY && iRc != XB_NOT_FOUND ){
              iErrorStop = 140;
              throw iRc;
            } else {
              iRc = 0;
            }
          }

          if( pRel->iStatus == XB_NO_ERROR ){

            // if this key doesn't match prev key, then return XB_NOT_FOUND
            if(( iRc = GetChildRelKeys( pRel, sChildKey, dChildKey, dtChildKey )) != XB_NO_ERROR ){
              iErrorStop = 150;
              throw iRc;
            }
            iRc = CompareKeys( pRel, sChildKey, sParentKey, dChildKey, dParentKey, dtChildKey, dtParentKey );

            if( iRc != XB_NO_ERROR ){
              if( iRc == XB_NOT_FOUND ){
                pRel->iStatus = XB_NOT_FOUND;
                iRc = XB_NO_ERROR;
              } else {
                iErrorStop = 160;
                throw iRc;
              }
            }
          }
        }
      }
      if( pRel->iStatus == 0 )
        pRel->ulRecPos++;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    if( pRel ) pRel->iStatus = XB_NOT_FOUND;
    sMsg.Sprintf( "xbRelate::GetPrevRecForNode() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }

  if( iRc != XB_NO_ERROR || pRel->iStatus != XB_NO_ERROR ){
    if( pRel ){
      if(( iRc = pRel->dDbf->BlankRecord()) != XB_NO_ERROR ){
        xbString sMsg;
        sMsg.Sprintf( "xbRelate::GetPrevRecForNode() Exception Caught. Error Stop = [500] rc = [%d]", iRc );
        xbase->WriteLogMessage( sMsg.Str() );
        xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
      }
    }
  }
  return iRc;
}
/************************************************************************/
xbInt16 xbRelate::GetChildRelKeys( xbRelation *pRel, xbString &s, xbDouble &d, xbDate &dt ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;

  try{

    if( !pRel ){
      iErrorStop = 100;
      iRc = XB_INVALID_OBJECT;
      throw iRc;
    }

    char cParType = pRel->pParentExp->GetReturnType();
    //std::cout << "gcrk child par type =[ " << cParType << "]\n";
    //std::cout << "gcrk name [" << pRel->dDbf->GetFileName().Str() << "]\n";
    //std::cout << "gcrk cur rec no [" << pRel->dDbf->GetCurRecNo() << "]\n";


    if( cParType == XB_EXP_CHAR ){
      if(( iRc = pRel->pIx->GetCurKeyVal( pRel->pTag, s )) !=XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      }
    } else if( cParType == XB_EXP_NUMERIC ){
      if(( iRc = pRel->pIx->GetCurKeyVal( pRel->pTag, d )) !=XB_NO_ERROR ){
        iErrorStop = 110;
        throw iRc;
      }
    } else if( cParType == XB_EXP_DATE ){
      if(( iRc = pRel->pIx->GetCurKeyVal( pRel->pTag, dt )) !=XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }
    } else {
      iErrorStop = 130;
      iRc = XB_INVALID_OBJECT;
      throw iRc;
    }

    // std::cout << "gcrk retrieved [" << s.Str() << "]\n";

  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbRelate::GetChildRelKeys() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
/************************************************************************/
xbInt16 xbRelate::GetParentRelKeys( xbRelation *pRel, xbString &s, xbDouble &d, xbDate &dt ){

  xbInt16 iRc = XB_NO_ERROR;
  xbInt16 iErrorStop = 0;

  try{

    if( !pRel ){
      iErrorStop = 100;
      iRc = XB_INVALID_OBJECT;
      throw iRc;
    }
    char cParType = pRel->pParentExp->GetReturnType();

    if(( iRc = pRel->pParentExp->ProcessExpression()) != XB_NO_ERROR ){
      iErrorStop = 110;
      throw iRc;
    }

    if( cParType == XB_EXP_CHAR ){
      if(( iRc = pRel->pParentExp->GetStringResult( s )) !=XB_NO_ERROR ){
        iErrorStop = 120;
        throw iRc;
      }
    } else if( cParType == XB_EXP_NUMERIC ){
      if(( iRc = pRel->pParentExp->GetNumericResult( d )) !=XB_NO_ERROR ){
        iErrorStop = 130;
        throw iRc;
      }
    } else if( cParType == XB_EXP_DATE ){
      if(( iRc = pRel->pParentExp->GetDateResult( dt )) !=XB_NO_ERROR ){
        iErrorStop = 140;
        throw iRc;
      }
    } else {
      iErrorStop = 140;
      iRc = XB_INVALID_OBJECT;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbRelate::GetParentRelKeys() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
/************************************************************************/
xbInt16 xbRelate::InitQuery(){

  // turn the tree into a straight linked list for the query routines

  xbInt16    iRc        = XB_NO_ERROR;
  xbInt16    iErrorStop = 0;

  try{
    // delete the llQryList linked list if it exists
    xbRelation *pRel;
    while( llQryList.GetNodeCnt() > 0 ){
      if(( iRc = llQryList.RemoveFromFront( pRel )) != XB_NO_ERROR ){
        iErrorStop = 100;
        throw iRc;
      }
    }
    // create straight linked list of all the nodes
    xbLinkListNode<xbRelation *> *llN = GetNextTreeNode( NULL );
    while( llN ){
      pRel = llN->GetKey();
      if(( iRc = llQryList.InsertAtEnd( pRel )) != XB_NO_ERROR ){
        iErrorStop = 110;
        throw iRc;
      }
      llN = GetNextTreeNode( llN );
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbRelate::InitQuery() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
/************************************************************************/
// This routine does a first key lookup or a last key lookup depending on cFL
//     for key value in one of sKey, dKey or dtKey depending on key type

// cFL = F    Do first key lookup
// CFL = L    Do last key lookup

// RelKeySearch return codes
//  RC             pRel->iStatus
//  ===========    =============
//   0               0
//   0             XB_NOT_FOUND
//  fatal error    XB_NOT_FOUND


xbInt16 xbRelate::RelKeySearch( xbRelation *pRel, const char cFL, xbString &sKey, xbDouble &dKey, xbDate &dtKey ){


  xbInt16 iRc = 0;
  xbInt16 iErrorStop = 0;

  try{

    char cParType = pRel->pParentExp->GetReturnType();
    pRel->iStatus = XB_NO_ERROR;

    if( cParType == 'M' ){
      iErrorStop = 100;
      iRc = XB_INVALID_OPTION;
      throw iRc;
    }

    if( cParType == XB_EXP_CHAR ){
      if( pRel->pFilt )
        iRc = pRel->pFilt->Find( sKey );
      else
        iRc = pRel->dDbf->Find( pRel->pIx, pRel->pTag, sKey );

    } else if( cParType == XB_EXP_NUMERIC ){
        if( pRel->pFilt )
          iRc = pRel->pFilt->Find( dKey );
        else
          iRc = pRel->dDbf->Find( pRel->pIx, pRel->pTag, dKey );

    } else if( cParType == XB_EXP_DATE ){
        if( pRel->pFilt )
          iRc = pRel->pFilt->Find( dtKey );
        else
          iRc = pRel->dDbf->Find( pRel->pIx, pRel->pTag, dtKey );

    } else {
        iErrorStop = 110;
        iRc = XB_INVALID_FIELD_TYPE;
        throw iRc;
    }

    if( iRc != XB_NO_ERROR ){
      pRel->iStatus = XB_NOT_FOUND;
      if( iRc == XB_NOT_FOUND || iRc == XB_BOF || iRc == XB_EOF || iRc == XB_EMPTY ){
        iRc = XB_NO_ERROR;
      } else {
        iErrorStop = 120;
        throw iRc;
      }
    }

    if( cFL == 'L' && pRel->iStatus == XB_NO_ERROR ){
      // find the last record for this key

      xbUInt32 ulLastGoodRecNo = pRel->dDbf->GetCurRecNo();
      xbBool bDone = xbFalse;
      xbString sThisKey;
      xbDouble dThisKey;
      xbDate   dtThisKey;

      while( !bDone ){
        // loop to next record until keys don't match

        if( pRel->pFilt )
          iRc = pRel->pFilt->GetNextRecordIx();
        else
          iRc = pRel->dDbf->GetNextKey( pRel->pIx, pRel->pTag );

        if( iRc == XB_EOF ){
          bDone = xbTrue;
          iRc = 0;

        } else if( iRc != XB_NO_ERROR ){
          iErrorStop = 100;
          throw iRc;

        } else {
 
          if(( iRc = GetChildRelKeys( pRel, sThisKey, dThisKey, dtThisKey )) != XB_NO_ERROR ){
            iErrorStop = 130;
            throw iRc;
          }

          if( cParType == XB_EXP_CHAR && sThisKey == sKey )
            ulLastGoodRecNo = pRel->dDbf->GetCurRecNo();
          else if( cParType == XB_EXP_NUMERIC && dThisKey == dKey )
            ulLastGoodRecNo = pRel->dDbf->GetCurRecNo();
          else if( cParType == XB_EXP_DATE && dtThisKey == dtKey )
            ulLastGoodRecNo = pRel->dDbf->GetCurRecNo();
          else
            bDone = xbTrue;
        }
      }
      if( pRel->dDbf->GetCurRecNo() != ulLastGoodRecNo ){
        // go back one
       if( pRel->pFilt )
          iRc = pRel->pFilt->GetPrevRecordIx();
        else
          iRc = pRel->dDbf->GetPrevKey( pRel->pIx, pRel->pTag );
      }
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    pRel->iStatus = XB_NOT_FOUND;
    sMsg.Sprintf( "xbRelate::RelKeySearch() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }

  if( iRc != XB_NO_ERROR || pRel->iStatus != XB_NO_ERROR ){
    if( pRel ){
      if( pRel->dDbf->GetCurRecNo() != 0 ){
        if(( iRc = pRel->dDbf->BlankRecord()) != XB_NO_ERROR ){
          xbString sMsg;
          sMsg.Sprintf( "xbRelate::RelKeySearch() Exception Caught. Error Stop = [500] rc = [%d]", iRc );
          xbase->WriteLogMessage( sMsg.Str() );
          xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
        }
      }
    }
  }
  return iRc;
}
/************************************************************************/
xbLinkListNode<xbRelation *> *xbRelate::GetLastTreeNode( xbLinkListNode<xbRelation *> *lln ){

  xbLinkListNode<xbRelation *> *llnWork = NULL;
  xbRelation *pRel;
  xbBool bDone = xbFalse;

  if( lln == NULL ){
    if( llQryTree.GetNodeCnt() > 0 )
      llnWork = llQryTree.GetHeadNode();
    else
      bDone = xbTrue;
  } else {
    llnWork = lln;
  }

  while( !bDone ){
  
    pRel = llnWork->GetKey();
    if( pRel->llChildren.GetNodeCnt() > 0 ){
      llnWork = pRel->llChildren.GetEndNode();
    } else {
      bDone = xbTrue;
    }
  }

  return llnWork;
}
/************************************************************************/
xbLinkListNode<xbRelation *> *xbRelate::GetNextTreeNode( xbLinkListNode<xbRelation *> *lln ){

  // if no tree exists, return null
  if( llQryTree.GetNodeCnt() == 0 )
    return NULL;

  // if( null input ) return first node
  if( lln == NULL )
    return llQryTree.GetHeadNode();

  // if( left child exists ) return left child
  xbRelation *pRel = lln->GetKey();

  if( pRel->llChildren.GetNodeCnt() > 0 )
    return pRel->llChildren.GetHeadNode();

  // if( right sibling exists ) return right sibling
  xbLinkListNode<xbRelation *> *llnRightSibling;
  if(( llnRightSibling = lln->GetNextNode()) != NULL )
    return llnRightSibling;

  // go up the tree looking for a right sibling
  xbLinkListNode<xbRelation *> *llnCur = lln;
  while( llnCur && ((llnRightSibling = llnCur->GetNextNode()) == NULL )){
    pRel = llnCur->GetKey();
    llnCur = pRel->pParentNp;
  }
  return llnRightSibling;
}

/************************************************************************/
xbInt16 xbRelate::GetLastRelRecord(){

  xbInt16    iRc        = XB_NO_ERROR;
  xbInt16    iErrorStop = 0;
  xbBool     bDone      = xbFalse;
  xbRelation *pRel      = NULL;
  char       cDir       = 'R';

  xbLinkListNode<xbRelation *> *lln;

  try{
    // if empty list, need to build it
    if( llQryList.GetNodeCnt() == 0 )
      InitQuery();

    // clear any previous positioning
    lln = llQryList.GetHeadNode();
    while( lln ){
      pRel = lln->GetKey();
      if( pRel->dDbf->GetCurRecNo() != 0 )
        pRel->dDbf->BlankRecord();
      pRel->ulSaveParRecNo = 0;
      pRel->iStatus  = 0;
      pRel->ulRecPos = 0;
      lln = lln->GetNextNode();
    }

    lln = llQryList.GetHeadNode();
    xbInt32 iMctr = 0;
    while( lln && !bDone ){
      pRel = lln->GetKey();

      if( pRel->cType == 'M' && iMctr == 0 ){
        iMctr++;
        if(( iRc = GetLastRecForNode( lln )) != XB_NO_ERROR ){
          iErrorStop = 100;
          throw iRc;
        }
      } else {
        if(( iRc = GetPrevRecForNode( lln )) != XB_NO_ERROR ){
          iErrorStop = 110;
          throw iRc;
        }
      }

      if( pRel->iStatus == XB_NO_ERROR ){
        if( cDir == 'L' ){
          cDir = 'R';
        }

      } else {

        if( pRel->cType == 'M' ){
          bDone = xbTrue;

        } else if( cDir == 'R' && pRel->cType == 'R' )
          cDir = 'L';

      }
      if( !bDone ){
        if( cDir == 'L' )
          lln = lln->GetPrevNode();
        else
          lln = lln->GetNextNode();
      }


    }
  }
  catch (xbInt16 iRc ){
    if( iRc != XB_NOT_FOUND && iRc != XB_EMPTY && iRc != XB_EOF ){
      xbString sMsg;
      sMsg.Sprintf( "xbRelate::GetLastRelRecord() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, iRc );
      xbase->WriteLogMessage( sMsg.Str() );
      xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
    }
  }
  return iRc;
}

/************************************************************************/
xbInt16 xbRelate::GetNextRelRecord(){
  return GetRelRecord( 'N' );
}
/************************************************************************/
xbInt16 xbRelate::GetPrevRelRecord(){
  return GetRelRecord( 'P' );
}
/************************************************************************/
xbInt16 xbRelate::GetRelRecord( const char cDirection ){

  // cDirection N - Get Next Rel Record
  //            P - Get Prev Rel Record

  xbInt16    iRc        = XB_NO_ERROR;
  xbInt16    iErrorStop = 0;
  xbBool     bDone      = xbFalse;
  xbRelation *pRel      = NULL;

  char       cDir       = 'L';  // direction to move on chain if XB_NOT_FOUND

  xbLinkListNode<xbRelation *> *lln;

  try{
    lln = llQryList.GetEndNode();

    while( lln && !bDone ){
      pRel = lln->GetKey();

      // GetNextRecForNode/GetPrevRecForNode return codes

      //  RC             pRel->iStatus
      //  ===========    =============
      //   0               0
      //   0             XB_NOT_FOUND, XB_BOF, XB_EOF, XB_EMPTY
      //  Fatal Error    XB_NOT_FOUND

      if( cDirection == 'P' ){
        if(( iRc = GetPrevRecForNode( lln )) != XB_NO_ERROR ){
          iErrorStop = 100;
          throw iRc;
        }
      } else {
        if(( iRc = GetNextRecForNode( lln )) != XB_NO_ERROR ){
          iErrorStop = 110;
          throw iRc;
        }
      }

      if( pRel->iStatus == 0 ){
        if( cDir == 'L' )
          cDir = 'R';

      } else {   // if( pRel->iStatus != XB_NO_ERROR ){

        if( pRel->cType == 'M' ){
          bDone = xbTrue;
          iRc = pRel->iStatus;

        } else if( cDir == 'R' && pRel->cType == 'R' ){
          cDir = 'L';
        }
      }

      if( !bDone ){
        if( cDir == 'L' )
          lln = lln->GetPrevNode();
        else
          lln = lln->GetNextNode();
      }
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbRelate::GetRelRecord() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str());
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));
  }
  return iRc;
}
/************************************************************************/
xbInt16 xbRelate::SetMaster( xbDbf * d, const xbString &sFilter ) {

  xbInt16    iRc        = XB_NO_ERROR;
  xbInt16    iErrorStop = 0;
  xbRelation *pRelation = NULL;

  try{

    if( llQryTree.GetNodeCnt() != 0 ){
      iRc = XB_ALREADY_DEFINED;
      iErrorStop = 100;
      throw iRc;
    }
    if( !d ){
      iRc = XB_NOT_OPEN;
      iErrorStop = 110;
      throw iRc;
    }
    if( d->GetDbfStatus() != XB_OPEN ){
      iRc = XB_NOT_OPEN;
      iErrorStop = 120;
      throw iRc;
    }
    if(( iRc = xbase->CheckRelListForTblUse( d )) != XB_NO_ERROR ){
      iErrorStop = 130;
      throw iRc;
    }
    // allocate an xbRelation structure
    if(( pRelation = (xbRelation *) calloc( 1, (size_t) sizeof( xbRelation ))) == NULL ){
      iRc = XB_NO_MEMORY;
      iErrorStop = 140;
      throw iRc;
    }
    pRelation->dDbf   = d;
    pRelation->cType  = 'M';          // main parent
    pRelation->sParentExpression = new xbString();

    if( strlen( sFilter ) > 0 ){
      pRelation->sFilter = new xbString( sFilter );
      pRelation->pFilt = new xbFilter( d );
      if(( iRc = pRelation->pFilt->Set( sFilter )) != XB_NO_ERROR ){
        iErrorStop = 150;
        throw iRc;
      }
    }
    llQryTree.InsertAtEnd( pRelation );

    if(( iRc = xbase->AddRelToRelList( this )) != XB_NO_ERROR ){
      iErrorStop  = 160;
      throw iRc;
    }
  }
  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbRelate::SetMaster() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, iRc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( xbase->GetErrorMessage( iRc ));

    if( pRelation ){
      if( pRelation->pFilt ){
        delete pRelation->pFilt;
        pRelation->pFilt = NULL;
        delete pRelation->sFilter;
        pRelation->sFilter = NULL;
      }
      free( pRelation );
    }
  }
  return iRc;
}
/************************************************************************/
}   /* namespace */ 
#endif         /*  XB_RELATE_SUPPORT */