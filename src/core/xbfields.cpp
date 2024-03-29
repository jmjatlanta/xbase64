/* xbfields.cpp

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

#include "xbase.h"

namespace xb{

/************************************************************************/
//! @brief Get xbDouble field for field name.
/*!
  \param sFieldName Name of field to retrieve.
  \param dFieldValue Output field value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/

xbInt16 xbDbf::GetDoubleField( const xbString &sFieldName, xbDouble &dFieldValue ) const {
  return GetDoubleField( GetFieldNo( sFieldName ), dFieldValue );
}

/************************************************************************/
//! @brief Get xbDouble field for field number.
/*!
  \param iFieldNo Number of field to retrieve.
  \param dFieldValue Output field value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/
xbInt16 xbDbf::GetDoubleField( xbInt16 iFieldNo, xbDouble &dFieldValue ) const {
  xbInt16 iRc = XB_NO_ERROR;
  char buf[21];
  memset( buf, 0x00, 21 );
  if(( iRc = GetRawField( iFieldNo, buf, 21, 0 )) >= XB_NO_ERROR ){
    dFieldValue = strtod( buf, NULL );
    return XB_NO_ERROR;
  } else
    return iRc;
}
/************************************************************************/
//! @brief Get xbDouble field for field number.
/*!
  \param iFieldNo Number of field to retrieve.
  \param dFieldValue Output field value.
  \param iRecBufSw 0 - Record buffer with any updates.<br>1 - Record buffer with original data.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/

xbInt16 xbDbf::GetDoubleField( xbInt16 iFieldNo, xbDouble &dFieldValue, xbInt16 iRecBufSw ) const {
  xbInt16 iRc = XB_NO_ERROR;
  char buf[21];
  memset( buf, 0x00, 21 );
  if(( iRc = GetRawField( iFieldNo, buf, 21, iRecBufSw )) >= XB_NO_ERROR ){
    dFieldValue = strtod( buf, NULL );
    return XB_NO_ERROR;
  } else
    return iRc;
}

/************************************************************************/
//! @brief Get xbDate field for field number.
/*!
  \param iFieldNo Number of field to retrieve.
  \param dtFieldValue Output field value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/

xbInt16 xbDbf::GetDateField( xbInt16 iFieldNo, xbDate &dtFieldValue ) const{
  xbString s;
  xbInt16 iRc;
  if(( iRc = GetField( iFieldNo, s )) != XB_NO_ERROR )
    return iRc;
  return dtFieldValue.Set( s );
  // return XB_NO_ERROR;
}

/************************************************************************/
//! @brief Get xbDate field for field name.
/*!
  \param sFieldName Name of field to retrieve.
  \param dtFieldValue Output field value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/

xbInt16 xbDbf::GetDateField( const xbString &sFieldName, xbDate &dtFieldValue ) const{
  xbString s;
  xbInt16 iRc;
  if(( iRc = GetField( sFieldName, s )) != XB_NO_ERROR )
    return iRc;
  return dtFieldValue.Set( s );
  // return XB_NO_ERROR;
}

/************************************************************************/
//! @brief Get xbString field for field name.
/*!
  \param sFieldName Name of field to retrieve
  \param sFieldValue Output field value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/

xbInt16 xbDbf::GetField( const xbString &sFieldName, xbString &sFieldValue ) const{
  return GetField( GetFieldNo( sFieldName ), sFieldValue, 0 );
}
/************************************************************************/
//! @brief Get field data for field number.
/*!
  \param iFieldNo Number of field to retrieve.
  \param sFieldValue Output field value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/

xbInt16 xbDbf::GetField( xbInt16 iFieldNo, xbString &sFieldValue ) const{
  return GetField( iFieldNo, sFieldValue, 0 );
}
/************************************************************************/
//! @brief Get field data for field number.
/*!
  \param iFieldNo Number of field to retrieve.
  \param sFieldValue Output field value.
  \param iRecBufSw 0 - Record buffer with any updates.<br>1 - Record buffer with original data.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/

xbInt16 xbDbf::GetField( xbInt16 iFieldNo, xbString &sFieldValue, xbInt16 iRecBufSw) const
{
  xbUInt32 iLen;
  if( iFieldNo < 0 || iFieldNo >= iNoOfFields ) {
    sFieldValue = "";
    return XB_INVALID_FIELD_NO;
  }
  iLen = SchemaPtr[iFieldNo].cFieldLen;
  sFieldValue = "";
  if( iRecBufSw )
    sFieldValue.Append( (char *) SchemaPtr[iFieldNo].pAddress2, iLen );   // original record buffer
  else
    sFieldValue.Append( (char *) SchemaPtr[iFieldNo].pAddress, iLen );    // current record buffer
  return XB_NO_ERROR;
}


/************************************************************************/
//! @brief Get decimal for field name.
/*!
  This routine retreives a field's decimal length.
  \param sFieldName Name of field to retrieve
  \param iFieldDecimal Output field value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/

xbInt16 xbDbf::GetFieldDecimal( const xbString &sFieldName, xbInt16 & iFieldDecimal ) const {
  return GetFieldDecimal( GetFieldNo( sFieldName ), iFieldDecimal );
}


/************************************************************************/
//! @brief Get decimal for field number.
/*!
  This routine retreives a field's decimal length.
  \param iFieldNo Number of field to retrieve
  \param iFieldDecimal Output field value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/

xbInt16 xbDbf::GetFieldDecimal( xbInt16 iFieldNo, xbInt16 & iFieldDecimal ) const {

  if( iFieldNo < 0 || iFieldNo >= iNoOfFields ) {
    return XB_INVALID_FIELD_NO;
  }
  iFieldDecimal = SchemaPtr[iFieldNo].cNoOfDecs;
  return XB_NO_ERROR;
}

/************************************************************************/
//! @brief Get field length for field name.
/*!

  This function retrieves a field's length.

  \param sFieldName Name of field to retrieve
  \param iFieldLen Output field length value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/

xbInt16 xbDbf::GetFieldLen( const xbString &sFieldName, xbInt16 &iFieldLen ) const {
  return GetFieldLen( GetFieldNo( sFieldName ), iFieldLen );
}


/************************************************************************/
//! @brief Get field length for field number.
/*!
  This function retrieves a field's length.

  \param iFieldNo Name of field to retrieve
  \param iFieldLen Output field length value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/

xbInt16 xbDbf::GetFieldLen( xbInt16 iFieldNo, xbInt16 &iFieldLen ) const {
  if( iFieldNo >= 0 && iFieldNo < iNoOfFields ){
    iFieldLen = SchemaPtr[iFieldNo].cFieldLen;
    return XB_NO_ERROR;
  } else
    return XB_INVALID_FIELD_NO;
}

/************************************************************************/
//! @brief  Get the field number for name.
/*! Returns the field number for the named field.

  All field get/put methods require either a field number or field name as
  one of the parameters.  Using the methods that take the field numbers will
  yield slightly better performance because the methods that take a name, have 
  to look up the number.

    \param sFieldName Name of field.
    \param iFieldNo Output field number for the given name.
    \returns Number of field named fldName.
*/

xbInt16 xbDbf::GetFieldNo( const xbString & sFieldName, xbInt16 &iFieldNo ) const
{
  int i;

  if( sFieldName.Len() > 10 )
    return XB_INVALID_FIELD_NAME;

  for( i = 0; i < iNoOfFields; i++ ){
    if( sFieldName == SchemaPtr[i].cFieldName ){
      iFieldNo = i;
      return XB_NO_ERROR;
    }
  }
  return XB_INVALID_FIELD_NAME;
}

/************************************************************************/
//! Get field ID number for a given field name.
/*! Returns the field number for the named field.

    \param sFieldName Name of field.
    \returns Number of field or XB_INVALID_FIELD_NAME.
*/

xbInt16 xbDbf::GetFieldNo( const xbString &sFieldName ) const {
  int i;

  if( sFieldName.Len() > 10 )
    return XB_INVALID_FIELD_NAME;

  for( i = 0; i < iNoOfFields; i++ ){
    if( sFieldName == SchemaPtr[i].cFieldName )
      return i;
  }
  return XB_INVALID_FIELD_NAME;
}

/************************************************************************/
//! Get field type for field number.
/*!
    \param iFieldNo Field number.
    \param cFieldType Output field type.
    \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/
xbInt16 xbDbf::GetFieldType( xbInt16 iFieldNo, char & cFieldType ) const{

  if( iFieldNo >= 0 && iFieldNo < iNoOfFields ){
    cFieldType = SchemaPtr[iFieldNo].cType;
    return XB_NO_ERROR;
  }
  else
    return XB_INVALID_FIELD_NO;
}

/************************************************************************/
//! Get field type for field name.
/*!
    \param sFieldName Field name.
    \param cFieldType Output field type.
    \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/

xbInt16 xbDbf::GetFieldType( const xbString &sFieldName, char & cFieldType ) const{
  return( GetFieldType( GetFieldNo( sFieldName ), cFieldType));
}

/************************************************************************/
//! @brief Returns the name of the specified field.
/*! Returns a pointer to the name for the field specified by iFieldNo.

    \param iFieldNo Number of field.
    \param sFieldName Output variable containing the field name.
    \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/
xbInt16 xbDbf::GetFieldName( xbInt16 iFieldNo, xbString &sFieldName ) const{
  if( iFieldNo >= 0 && iFieldNo < iNoOfFields ){
    sFieldName = SchemaPtr[iFieldNo].cFieldName;
    return XB_NO_ERROR;
  }
  else
    return XB_INVALID_FIELD_NO;
}

/************************************************************************/
//! @brief Get xbFloat field for field number.
/*!
  \param iFieldNo Number of field to retrieve.
  \param fFieldValue Output field value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/
xbInt16 xbDbf::GetFloatField( xbInt16 iFieldNo, xbFloat & fFieldValue ) const {

  char cFieldType;
  xbInt16 rc = GetFieldType( iFieldNo, cFieldType );
  if( rc < 0 )
    return rc;

  if( cFieldType != 'N' && cFieldType != 'F' )
    return XB_INVALID_FIELD_TYPE;

  xbString sTemp;
  rc = GetField( iFieldNo, sTemp, 0 );
  if( rc < 0 )
    return rc;

  fFieldValue = (xbFloat) atof( sTemp.Str());
  return XB_NO_ERROR;
}


/************************************************************************/
//! @brief Get xbFloat field for field name.
/*!
  \param sFieldName Number of field to retrieve.
  \param fFieldValue Output field value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/

xbInt16 xbDbf::GetFloatField( const xbString & sFieldName, xbFloat & fFieldValue ) const {
  return GetFloatField( GetFieldNo(sFieldName ), fFieldValue );
}

/************************************************************************/
//! @brief Get logical field for field number.
/*!
  \param iFieldNo Number of field to retrieve.
  \param sFieldValue Output field value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/
xbInt16 xbDbf::GetLogicalField( xbInt16 iFieldNo, xbString &sFieldValue ) const {

  char cFieldType;
  xbInt16 iRc = GetFieldType( iFieldNo, cFieldType );
  if( iRc < 0 )
    return iRc;
  else if( cFieldType != 'L' )
    return XB_INVALID_FIELD_TYPE;

  if(( iRc = GetField( iFieldNo, sFieldValue )) < XB_NO_ERROR )
    return iRc;
  else
    return XB_NO_ERROR;
}

/************************************************************************/
//! @brief Get logical field for field name.
/*!
  \param sFieldName Name of field to retrieve.
  \param sFieldValue Output field value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/

xbInt16 xbDbf::GetLogicalField( const xbString &sFieldName, xbString &sFieldValue ) const {
  return GetLogicalField( GetFieldNo( sFieldName ), sFieldValue );
}


/************************************************************************/
//! @brief Get logical field for field number.
/*!
  \param iFieldNo Number of field to retrieve.
  \param bFieldValue Output field value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/

xbInt16 xbDbf::GetLogicalField( xbInt16 iFieldNo, xbBool &bFieldValue ) const {
  return GetLogicalField( iFieldNo, bFieldValue, 0 );
}

/************************************************************************/
//! @brief Get logical field for field number.
/*!
  \param iFieldNo Number of field to retrieve.
  \param bFieldValue Output field value.
  \param iRecBufSw 0 - Record buffer with any updates.<br>1 - Record buffer with original data.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/

xbInt16 xbDbf::GetLogicalField( xbInt16 iFieldNo, xbBool &bFieldValue, xbInt16 iRecBufSw ) const {

  char cFieldType;
  xbInt16 iRc = GetFieldType( iFieldNo, cFieldType );
  if((iRc = GetFieldType( iFieldNo, cFieldType )) != XB_NO_ERROR )
    return iRc;

  if( cFieldType != 'L' )
    return XB_INVALID_FIELD_TYPE;

  xbString sFieldValue;
  if(( iRc = GetField( iFieldNo, sFieldValue, iRecBufSw )) < XB_NO_ERROR )
    return iRc;

  if( sFieldValue == 'T' || sFieldValue == 't' || sFieldValue == 'Y' || sFieldValue == 'y' )
    bFieldValue = xbTrue;
  else
    bFieldValue = xbFalse;

  return XB_NO_ERROR;
}

/************************************************************************/
//! @brief Get logical field for field name.
/*!
  \param sFieldName Name of field to retrieve.
  \param bFieldValue Output field value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/
xbInt16 xbDbf::GetLogicalField( const xbString &sFieldName, xbBool &bFieldValue ) const {
  return GetLogicalField( GetFieldNo( sFieldName ), bFieldValue );
}
/************************************************************************/
//! @brief Get long field for field number.
/*!
  \param iFieldNo Number of field to retrieve.
  \param lFieldValue Output field value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/
xbInt16 xbDbf::GetLongField( xbInt16 iFieldNo, xbInt32 & lFieldValue ) const {

  xbInt16  iRc = 0;
  xbInt16  iErrorStop = 0;
  char     cFieldType;
  xbString sTemp;

  try{

    if(( iRc = GetFieldType( iFieldNo, cFieldType )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw iRc;
    }

    if( cFieldType != 'N' && cFieldType != 'F' && cFieldType != 'M' ){
      iErrorStop = 110;
      iRc = XB_INVALID_FIELD_TYPE;
      throw iRc;
    }

    if(( iRc = GetField( iFieldNo, sTemp, 0 )) < 0 ){
      iErrorStop = 120;
      throw iRc;
    }
    sTemp.Trim();

    if( !sTemp.ValidNumericValue() ){
      iErrorStop = 130;
      iRc = XB_INVALID_DATA;
      throw iRc;
    }

    if( sTemp.Pos( '.' ) > 0){
      iErrorStop = 140;
      iRc = XB_INVALID_DATA;
      throw iRc;
    }

    lFieldValue = atol( sTemp.Str());
  }

  catch (xbInt16 iRc ){
    xbString sMsg;
    sMsg.Sprintf( "xbDbf::GetLongField() Exception Caught. Error Stop = [%d] rc = [%d] [%s]", iErrorStop, iRc, sTemp.Str() );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( iRc ));
  }

  return XB_NO_ERROR;
}

/************************************************************************/
#ifdef XB_MEMO_SUPPORT
//! @brief Get memo field for field number.
/*!
  \param iFieldNo Number of field to retrieve.
  \param sMemoValue Output field value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/
xbInt16 xbDbf::GetMemoField( xbInt16 iFieldNo, xbString &sMemoValue ){
  return Memo->GetMemoField( iFieldNo, sMemoValue );
}
/************************************************************************/
//! @brief Get memo field for field name.
/*!

  \param sFieldName Name of field to retrieve.
  \param sMemoValue Output field value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/
xbInt16 xbDbf::GetMemoField( const xbString & sFieldName, xbString & sMemoValue ){
 return Memo->GetMemoField( GetFieldNo( sFieldName ), sMemoValue );
}

/************************************************************************/
//! @brief Get the memo field count for this table.
/*!
  \returns Returns the number of memo fields in the table,
*/
xbInt16 xbDbf::GetMemoFieldCnt() const {
  return iMemoFieldCnt;
}
/************************************************************************/
//! @brief Get memo field length for field number.
/*!
  \param iFieldNo Number of field to retrieve. 
  \param ulMemoFieldLen Output memo field value length.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/

xbInt16 xbDbf::GetMemoFieldLen( xbInt16 iFieldNo, xbUInt32 & ulMemoFieldLen ){
  return Memo->GetMemoFieldLen( iFieldNo, ulMemoFieldLen );
}
/************************************************************************/
//! @brief Get memo field length for field name.
/*!
  \param sFieldName Name of field to retrieve.
  \param ulMemoFieldLen Output memo field value length.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/
xbInt16 xbDbf::GetMemoFieldLen( const xbString &sFieldName, xbUInt32 &ulMemoFieldLen ){
  return Memo->GetMemoFieldLen( GetFieldNo( sFieldName ), ulMemoFieldLen );
}

#endif // XB_MEMO_SUPPORT

/************************************************************************/
//! @brief Get field null status
/*!
  \param iFieldNo Number of field to retrieve.
  \param bIsNull Output field value.  If field is all spaces on record buffer, returns true.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/
xbInt16 xbDbf::GetNullSts( xbInt16 iFieldNo, xbBool &bIsNull ) const {
  return GetNullSts( iFieldNo, bIsNull, 0 );
}

/************************************************************************/
//! @brief Get field null status
/*!
  \param sFieldName Field Name of field to retrieve.
  \param bIsNull Output field value.  If field is all spaces on record buffer, returns true.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/

xbInt16 xbDbf::GetNullSts( const xbString &sFieldName, xbBool &bIsNull ) const {
  return GetNullSts( GetFieldNo( sFieldName ), bIsNull, 0 );
}

/************************************************************************/
//! @brief Get field null status
/*!
  \param iFieldNo Number of field to retrieve.
  \param bIsNull Output field value.  If field is all spaces on record buffer, returns true.
  \param iRecBufSw 0 - Record buffer with any updates.<br>1 - Record buffer with original data.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/

xbInt16 xbDbf::GetNullSts( xbInt16 iFieldNo, xbBool &bIsNull, xbInt16 iRecBufSw ) const
{
  if( iFieldNo < 0 || iFieldNo >= iNoOfFields ) {
    return XB_INVALID_FIELD_NO;
  }
  bIsNull = xbTrue;
  char *p;
  if( iRecBufSw )
    p = (char *) SchemaPtr[iFieldNo].pAddress2;
  else
    p = (char *) SchemaPtr[iFieldNo].pAddress;

  xbUInt32 ulLen = SchemaPtr[iFieldNo].cFieldLen;
  xbUInt32 ul   = 0;

  while( ul < ulLen && bIsNull ){
    if( *p++ != ' ' )
      bIsNull = xbFalse;
    else
      ul++;
  }
  return XB_NO_ERROR;
}

/************************************************************************/
//! @brief Get unsigned long field for field number.
/*!
  \param iFieldNo Number of field to retrieve.
  \param ulFieldValue Output field value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/
xbInt16 xbDbf::GetULongField( xbInt16 iFieldNo, xbUInt32 & ulFieldValue ) const {

  xbInt16 rc = 0;
  xbInt16 iErrorStop = 0;
  char cFieldType;

  try{

    if(( rc = GetFieldType( iFieldNo, cFieldType )) != XB_NO_ERROR ){
      iErrorStop = 100;
      throw rc;
    }
    if( cFieldType != 'N' && cFieldType != 'F' && cFieldType != 'M' ){
      iErrorStop = 110;
      rc = XB_INVALID_FIELD_TYPE;
      throw rc;
    }
    xbString sTemp;
    if(( rc = GetField( iFieldNo, sTemp, 0 )) != XB_NO_ERROR ){
      iErrorStop = 120;
      throw rc;
    }
    sTemp.Trim();
    if( !sTemp.ValidNumericValue() || ((int) sTemp.Pos( '.' ) > 0)){
      iErrorStop = 130;
      rc = XB_INVALID_DATA;
      throw rc;
    }
    ulFieldValue = strtoul( sTemp.Str(), NULL, 10 );
  }
  catch (xbInt16 rc ){
    xbString sMsg;
    sMsg.Sprintf( "xbDbf::GetULongField() Exception Caught. Error Stop = [%d] rc = [%d]", iErrorStop, rc );
    xbase->WriteLogMessage( sMsg.Str() );
    xbase->WriteLogMessage( GetErrorMessage( rc ));
  }
  return XB_NO_ERROR;
}

/************************************************************************/
//! @brief Get long field for field name.
/*!
  \param sFieldName Number of field to retrieve.
  \param lFieldValue Output field value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/

xbInt16 xbDbf::GetLongField( const xbString &sFieldName, xbInt32 &lFieldValue ) const {
  return GetLongField( GetFieldNo( sFieldName ), lFieldValue );
}

/************************************************************************/
//! @brief Get unsigned long field for field name.
/*!
  \param sFieldName Number of field to retrieve.
  \param ulFieldValue Output field value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/

xbInt16 xbDbf::GetULongField( const xbString &sFieldName, xbUInt32 &ulFieldValue ) const {
  return GetULongField( GetFieldNo( sFieldName ), ulFieldValue );
}

/************************************************************************/
//! @brief Get raw field data for field number.
/*!

  This is a protected method, used by other methods.  This method would be 
  subject to buffer overflows if made public.

  \param iFieldNo Number of field to retrieve.
  \param cBuf Pointer to buffer area provided by calling application program.
  \param ulBufSize Size of data to copy
  \param iRecBufSw 0 - Record buffer with any updates.<br>1 - Record buffer with original data.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/


xbInt16 xbDbf::GetRawField( xbInt16 iFieldNo, char *cBuf, xbUInt32 ulBufSize, xbInt16 iRecBufSw ) const
{
  if( iFieldNo < 0 || iFieldNo >= iNoOfFields ) {
    return XB_INVALID_FIELD_NO;
  }

  size_t stCopySize;
  if( ulBufSize > (size_t) (SchemaPtr[iFieldNo].cFieldLen ))
    stCopySize =  (size_t) (SchemaPtr[iFieldNo].cFieldLen );
  else
    stCopySize = ulBufSize - 1;

  if( iRecBufSw )
    memcpy( cBuf, SchemaPtr[iFieldNo].pAddress2, stCopySize );
  else
    memcpy( cBuf, SchemaPtr[iFieldNo].pAddress, stCopySize );

  cBuf[stCopySize] = 0x00;
  return XB_NO_ERROR;
}


/************************************************************************/
//! @brief Put double field for field name.
/*!
  \param sFieldName Name of field to update.
  \param dFieldValue Field value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/
xbInt16 xbDbf::PutDoubleField( const xbString &sFieldName, xbDouble dFieldValue ){
  return PutDoubleField( GetFieldNo( sFieldName ), dFieldValue );
}

/************************************************************************/
//! @brief Put double field for field number.
/*!
  \param iFieldNo Number of field to update.
  \param dFieldValue Field value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/

xbInt16 xbDbf::PutDoubleField( xbInt16 iFieldNo, xbDouble dFieldValue ){

  xbInt16  rc;
  xbString sDoubleFmt;
  xbString sDoubleFmt2;
  xbString sDoubleVal;
  xbInt16  iFieldLen;
  xbInt16  iNoOfDecs;

  if(( rc = GetFieldDecimal( iFieldNo, iNoOfDecs )) != XB_NO_ERROR )
    return rc;

  if(( rc = GetFieldLen( iFieldNo, iFieldLen )) != XB_NO_ERROR )
    return rc;

  sDoubleFmt.Sprintf( "%d.%df", iFieldLen, iNoOfDecs );
  sDoubleFmt2 = "%-";
  sDoubleFmt2 += sDoubleFmt;
  sDoubleVal.Sprintf( sDoubleFmt2.Str(), dFieldValue );
  sDoubleVal.Rtrim();
  return PutField( iFieldNo, sDoubleVal );
}

/************************************************************************/
//! @brief Put date field for field name.
/*!
  \param sFieldName Name of field to update.
  \param dtFieldValue Field value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/

xbInt16 xbDbf::PutDateField(const xbString &sFieldName, const xbDate &dtFieldValue ){
  return PutField( GetFieldNo( sFieldName ), dtFieldValue.Str() );
}

/************************************************************************/
//! @brief Put date field for field number.
/*!
  \param iFieldNo Number of field to update.
  \param dtFieldValue Field value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/
xbInt16 xbDbf::PutDateField( xbInt16 iFieldNo, const xbDate &dtFieldValue ){
  return PutField( iFieldNo, dtFieldValue.Str() );
}

/************************************************************************/
//! @brief Put field for field name.
/*!
  \param sFieldName Name of field to update.
  \param sFieldValue Field value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/
xbInt16 xbDbf::PutField(const xbString &sFieldName, const xbString &sFieldValue ) {
  return PutField( GetFieldNo( sFieldName ), sFieldValue );
}
/************************************************************************/
//! @brief Put field for field number.
/*!
  \param iFieldNo Number of field to update.
  \param sFieldValue Field value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/

xbInt16 xbDbf::PutField( xbInt16 iFieldNo, const xbString &sFieldValue ) {
  xbInt16 iLen;
  xbInt16 iDecPos;         /* Decimal Position */
  char * startpos;
  char * tp;              /* Target Pointer */

  if( iFieldNo < 0 || iFieldNo >= iNoOfFields )
    return XB_INVALID_FIELD_NO;

  if( SchemaPtr[iFieldNo].cType == 'L' && !sFieldValue.ValidLogicalValue())
    return XB_INVALID_DATA;

  else if( SchemaPtr[iFieldNo].cType == 'D' ){
    xbDate d;
    if( !d.DateIsValid( sFieldValue ))
      return XB_INVALID_DATA;
    iLen = 8;
  }
  else
  {
    iLen = (xbInt16) sFieldValue.Len();
    if( SchemaPtr[iFieldNo].cType == 'F' || SchemaPtr[iFieldNo].cType == 'N' || SchemaPtr[iFieldNo].cType == 'M' ){
      if( !sFieldValue.ValidNumericValue()) {
        return XB_INVALID_DATA;
    }
      else {
        iDecPos = (xbInt16) sFieldValue.Pos( "." );     // 0 is no decimal
        int mlod;    // max no of digits left of decimal point digit count 


        if( SchemaPtr[iFieldNo].cNoOfDecs > 0 )
          mlod = SchemaPtr[iFieldNo].cFieldLen - SchemaPtr[iFieldNo].cNoOfDecs - 1;
        else
          mlod = iLen;

        if( iDecPos == 0 ){                  // no decimal in incoming data

          // check digits to the left of the decimal
          if( SchemaPtr[iFieldNo].cNoOfDecs > 0 && iLen > mlod )  /* no decimal in incoming data */
            return XB_INVALID_DATA;

          else if( SchemaPtr[iFieldNo].cNoOfDecs == 0 && iLen > SchemaPtr[iFieldNo].cFieldLen )
            return XB_INVALID_DATA;

        }
        else                                // decimal in incoming data
        {
          if( (iDecPos-1) > mlod )           // too many digits to the left of dec in incoming data 
            return XB_INVALID_DATA;

          // check digits to the right of the decimal
          else if((iLen - iDecPos) > SchemaPtr[iFieldNo].cNoOfDecs )
            return XB_INVALID_DATA;
        }
      }
    }
  }

  // do all field edits before this point
  if( iDbfStatus != XB_UPDATED ){
    iDbfStatus = XB_UPDATED;
    memcpy( RecBuf2, RecBuf, uiRecordLen );    // save the original record bufer before making updates
  }

  memset( SchemaPtr[iFieldNo].pAddress, 0x20, SchemaPtr[iFieldNo].cFieldLen );

  if( iLen > SchemaPtr[iFieldNo].cFieldLen )
    iLen = SchemaPtr[iFieldNo].cFieldLen;

  if( SchemaPtr[iFieldNo].cType == 'F' || SchemaPtr[iFieldNo].cType == 'N' 
      || SchemaPtr[iFieldNo].cType == 'M') {

    xbInt16 iDecPos = (xbInt16) sFieldValue.Pos( "." );
    if( iDecPos == 0 ){
      iLen = (xbInt16) sFieldValue.Len();
      iDecPos = 0;
    }
    else{
      iLen = iDecPos - 1;
    }

    if( SchemaPtr[iFieldNo].cNoOfDecs > 0 ){
      tp = SchemaPtr[iFieldNo].pAddress;
      tp += SchemaPtr[iFieldNo].cFieldLen - SchemaPtr[iFieldNo].cNoOfDecs - 1;
      *tp++ = '.';

      if( iDecPos == 0 ){
        for( xbInt32 i = 0; i < SchemaPtr[iFieldNo].cNoOfDecs; i++ )
          *tp++ = '0';
      } else {
        xbInt32 j = iDecPos + 1;
        for( xbInt32 i = 0; i < SchemaPtr[iFieldNo].cNoOfDecs; i++, j++ ){
          if( j <= (xbInt32) sFieldValue.Len())
            *tp++ = sFieldValue[j];
          else
            *tp++ = '0';
        }
      }
      startpos= SchemaPtr[iFieldNo].pAddress +
                SchemaPtr[iFieldNo].cFieldLen -
                SchemaPtr[iFieldNo].cNoOfDecs - iLen - 1; 

    }
    else
      startpos=SchemaPtr[iFieldNo].pAddress+SchemaPtr[iFieldNo].cFieldLen-iLen; 
  }
  else
    startpos = SchemaPtr[iFieldNo].pAddress;

  memcpy( startpos, sFieldValue.Str(), (size_t) iLen );
  return 0;
}

/************************************************************************/
//! @brief Put float field for field number.
/*!
  \param iFieldNo Number of field to update.
  \param fFieldValue Field value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/
xbInt16 xbDbf::PutFloatField( xbInt16 iFieldNo, xbFloat fFieldValue ){

  xbInt16  rc;
  xbString sFloatFmt;
  xbString sFloatFmt2;
  xbString sFloatVal;
  xbInt16  iFieldLen;
  xbInt16  iNoOfDecs;

  if(( rc = GetFieldDecimal( iFieldNo, iNoOfDecs )) != XB_NO_ERROR )
    return rc;

  if(( rc = GetFieldLen( iFieldNo, iFieldLen )) != XB_NO_ERROR )
    return rc;

  sFloatFmt.Sprintf( "%d.%df", iFieldLen, iNoOfDecs );
  sFloatFmt2 = "%-";
  sFloatFmt2 += sFloatFmt;
  sFloatVal.Sprintf( sFloatFmt2.Str(), fFieldValue );
  sFloatVal.Rtrim();
  return PutField( iFieldNo, sFloatVal );
}

/************************************************************************/
//! @brief Put float field for field name.
/*!
  \param sFieldName Name of field to update.
  \param fFieldValue Field value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/

xbInt16 xbDbf::PutFloatField( const xbString &sFieldName, xbFloat fFieldValue ){
  return PutFloatField( GetFieldNo( sFieldName ), fFieldValue );
}

/************************************************************************/
//! @brief Put logical field for field number.
/*!
  \param iFieldNo Number of field to update.
  \param sFieldValue Field value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/

xbInt16 xbDbf::PutLogicalField( xbInt16 iFieldNo, const xbString &sFieldValue ) {
  return PutField( iFieldNo, sFieldValue );
}

/************************************************************************/
//! @brief Put logical field for field name.
/*!
  \param sFieldName Name of field to update.
  \param sFieldValue Field value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/
xbInt16 xbDbf::PutLogicalField( const xbString &sFieldName, const xbString &sFieldValue ) {
  return PutField( GetFieldNo( sFieldName ), sFieldValue );
}

/************************************************************************/
//! @brief Put logical field for field number.
/*!
  \param iFieldNo Number of field to update.
  \param bFieldValue Field value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/

xbInt16 xbDbf::PutLogicalField( xbInt16 iFieldNo, xbBool bFieldValue ) {

  if( bFieldValue )
    return PutField( iFieldNo, "T" );
  else
    return PutField( iFieldNo, "F" );
}

/************************************************************************/
//! @brief Put logical field for field name.
/*!
  \param sFieldName Name of field to update.
  \param bFieldValue Field value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/
xbInt16 xbDbf::PutLogicalField( const xbString &sFieldName, xbBool bFieldValue ) {
  return PutLogicalField( GetFieldNo( sFieldName ), bFieldValue );
}

/************************************************************************/
//! @brief Put long field for field number.
/*!
  \param iFieldNo Number of field to update.
  \param lFieldValue Field value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/

xbInt16 xbDbf::PutLongField( xbInt16 iFieldNo, xbInt32 lFieldValue ) {
  xbString sLong;
  sLong.Sprintf( "%ld", (xbInt32) lFieldValue );
  return PutField( iFieldNo, sLong.Str() );
}

/************************************************************************/
//! @brief Put long field for field name.
/*!
  \param sFieldName Name of field to update.
  \param lFieldValue Field value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/

xbInt16 xbDbf::PutLongField( const xbString &sFieldName, xbInt32 lFieldValue ) {
  return PutLongField( GetFieldNo( sFieldName ), lFieldValue );
}

/************************************************************************/
//! @brief Put unsigned long field for field number.
/*!
  \param iFieldNo Number of field to update.
  \param ulFieldValue Field value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/

xbInt16 xbDbf::PutULongField( xbInt16 iFieldNo, xbUInt32 ulFieldValue ) {
  xbString sLong;
  sLong.Sprintf( "%lu", (xbInt32) ulFieldValue );
  return PutField( iFieldNo, sLong.Str() );
}

/************************************************************************/
//! @brief Put unsigned long field for field name.
/*!
  \param sFieldName Name of field to update.
  \param ulFieldValue Field value.
  \returns The field routines return one of:<br>
    XB_NO_ERROR<br>XB_INVALID_DATA<br>XB_INVALID_FIELD_NO<br>XB_INVALID_FIELD_NAME<br>
    XB_INVALID_FIELD_TYPE<br>XB_INVALID_DATA
*/

xbInt16 xbDbf::PutULongField( const xbString &sFieldName, xbUInt32 ulFieldValue ) {
  return PutLongField( GetFieldNo( sFieldName ), (xbInt32) ulFieldValue );
}

/************************************************************************/
#ifdef XB_MEMO_SUPPORT
//! @brief Check if memo field exists for field name.
/*!
  \param sFieldName Name of field to check.
  \returns xbTrue Field exists.<br> xbFale Field does not exist.
*/

xbBool xbDbf::MemoFieldExists( const xbString &sFieldName ) const{
  return MemoFieldExists( GetFieldNo( sFieldName ));
}

/************************************************************************/
//! @brief Check if memo field exists for field number.
/*!
  \param iFieldNo Number of field to check.
  \returns xbTrue Field exists.<br> xbFale Field does not exist.
*/

xbBool xbDbf::MemoFieldExists( xbInt16 iFieldNo ) const{

  xbInt32 lFld = 0L;
  GetLongField( iFieldNo, lFld );
  if( lFld == 0L )
    return xbFalse;
  else
    return xbTrue;
}

#endif

}   /* namespace */

