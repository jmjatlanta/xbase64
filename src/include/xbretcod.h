/*  xbretcod.h

XBase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023,2024 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license.

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

#ifndef __XB_RETCODES_H__
#define __XB_RETCODES_H__

/***********************************************/
/* Return Codes and Error Messages             */

namespace xb{

  #define XB_NO_ERROR                     0       // general
  #define XB_NO_MEMORY                 -100       // general
  #define XB_INVALID_OPTION            -101       // general
  #define XB_INVALID_PARAMETER         -102       // general
  #define XB_DUP_TABLE_OR_ALIAS        -110       // table manager
  #define XB_INVALID_NODELINK          -120       // linklist
  #define XB_KEY_NOT_UNIQUE            -121       // linklist, index
  #define XB_MEMCPY_ERROR              -122       // general
  #define XB_ALREADY_DEFINED           -123       // general

  #define XB_FILE_EXISTS               -200       // file
  #define XB_ALREADY_OPEN              -201       // file
  #define XB_DBF_FILE_NOT_OPEN         -202       // file
  #define XB_FILE_NOT_FOUND            -203       // file
  #define XB_FILE_TYPE_NOT_SUPPORTED   -204       // file
  #define XB_RENAME_ERROR              -205       // file
  #define XB_INVALID_OBJECT            -206       // file, general
  #define XB_NOT_OPEN                  -207       // file
  #define XB_NOT_CLOSED                -208       // file
  #define XB_NOT_FOUND                 -209       // file, general
  #define XB_OPEN_ERROR                -210       // file
  #define XB_CLOSE_ERROR               -211       // file
  #define XB_SEEK_ERROR                -212       // file
  #define XB_READ_ERROR                -213       // file
  #define XB_WRITE_ERROR               -214       // file
  #define XB_EOF                       -215       // file
  #define XB_BOF                       -216       // file
  #define XB_INVALID_BLOCK_SIZE        -217       // file
  #define XB_INVALID_BLOCK_NO          -218       // file
  #define XB_INVALID_RECORD            -219       // file
  #define XB_DELETE_FAILED             -220       // file
  #define XB_INVALID_TABLE_NAME        -221       // file
  #define XB_EMPTY                     -222       // file
  #define XB_LIMIT_REACHED             -223       // file
  #define XB_BLOCKREAD_NOT_ENABLED     -224       // file
  #define XB_DIRECTORY_ERROR           -225       // file
  #define XB_SETVBUF_ERROR             -226       // file

  #define XB_INVALID_FIELD_TYPE        -300       // field
  #define XB_INVALID_FIELD_NO          -301       // field
  #define XB_INVALID_DATA              -302       // field
  #define XB_INVALID_FIELD_NAME        -303       // field
  #define XB_INVALID_MEMO_FIELD        -304       // field
  #define XB_INVALID_FIELD             -305       // field
  #define XB_INVALID_FIELD_LEN         -306       // field
  #define XB_INVALID_DATE              -307       // date field

  #define XB_INVALID_LOCK_OPTION       -400       // lock
  #define XB_LOCK_FAILED               -401       // lock
  #define XB_TABLE_NOT_LOCKED          -402       // lock - need table locked for operation

  #define XB_PARSE_ERROR               -500       // expression
  #define XB_INVALID_FUNCTION          -501       // expression
  #define XB_INVALID_PARM              -502       // expression
  #define XB_INCONSISTENT_PARM_LENS    -503       // expression
  #define XB_INCOMPATIBLE_OPERANDS     -504       // expression
  #define XB_UNBALANCED_PARENS         -505       // expression
  #define XB_UNBALANCED_QUOTES         -506       // expression
  #define XB_INVALID_EXPRESSION        -507       // expression

  #define XB_INVALID_KEYNO             -600       // index
  #define XB_INVALID_INDEX             -601       // index file error
  #define XB_INVALID_TAG               -602       // invalid index tag or invalid tag name, must be <= 10 bytes
  #define XB_INVALID_PAGE              -603       // invalid index page


  #define XB_SYNTAX_ERROR              -700       // sql syntax error


  #define XB_MAX_ERROR_NO              -999


///@cond DOXYOFF
struct XBDLLEXPORT xbErrorMessage{
  xbInt16    iErrorNo;
  const char *sErrorText;
};
///@endcond DOXYOFF

const xbErrorMessage xbErrorMessages[] = {
  { XB_NO_ERROR,                "No Error" },
  { XB_NO_MEMORY,               "No Memory" },
  { XB_INVALID_OPTION,          "Invalid Option" },
  { XB_INVALID_PARAMETER,       "Invalid Parameter" },
  { XB_DUP_TABLE_OR_ALIAS,      "Duplicate Alias/Table Name" },
  { XB_INVALID_NODELINK,        "Invalid Node Link" },
  { XB_KEY_NOT_UNIQUE,          "Key Not Unique" },
  { XB_MEMCPY_ERROR,            "Memory copy failure" },
  { XB_ALREADY_DEFINED,         "Object already defined" },
  { XB_FILE_EXISTS,             "File Already Exists" },
  { XB_ALREADY_OPEN,            "Database already open" },
  { XB_DBF_FILE_NOT_OPEN,       "DBF File Not Open" },
  { XB_FILE_NOT_FOUND,          "File not found" },
  { XB_FILE_TYPE_NOT_SUPPORTED, "Not an Xbase type database" },
  { XB_RENAME_ERROR,            "Unable to rename file" },
  { XB_INVALID_OBJECT,          "Invalid Object" },
  { XB_NOT_OPEN,                "Database not open" },
  { XB_NOT_FOUND,               "Not Found" },
  { XB_OPEN_ERROR,              "Open Error" },
  { XB_NOT_CLOSED,              "File(s) not closed" },
  { XB_CLOSE_ERROR,             "Close Error" },
  { XB_SEEK_ERROR,              "Seek Error" },
  { XB_READ_ERROR,              "Read Error" },
  { XB_WRITE_ERROR,             "Error writing to disk drive" },
  { XB_EOF,                     "End Of File" },
  { XB_BOF,                     "Beginning Of File" },
  { XB_INVALID_BLOCK_SIZE,      "Invalid Block Size" },
  { XB_INVALID_BLOCK_NO,        "Invalid Block Number" },
  { XB_INVALID_RECORD,          "Invalid Record Number" },
  { XB_DELETE_FAILED,           "Delete Failed" },
  { XB_INVALID_TABLE_NAME,      "Invalid Table Name" },
  { XB_EMPTY,                   "Empty Table or Index" },
  { XB_LIMIT_REACHED,           "Limit Reached" },
  { XB_BLOCKREAD_NOT_ENABLED,   "Block Read Mode is not enabled" },
  { XB_DIRECTORY_ERROR,         "Directory Read/Write error" },
  { XB_SETVBUF_ERROR,           "Error configuring file buffer options" },
  { XB_INVALID_FIELD_TYPE,      "Unknown Field Type" },
  { XB_INVALID_FIELD_NO,        "Invalid Field Number" },
  { XB_INVALID_DATA,            "Invalid Data" },
  { XB_INVALID_FIELD_NAME,      "Invalid Field Name" },
  { XB_INVALID_MEMO_FIELD,      "Not a Memo field" },
  { XB_INVALID_FIELD,           "Invalid Field" },
  { XB_INVALID_FIELD_LEN,       "Invalid Field Length" },
  { XB_INVALID_DATE,            "Invalid Date" },
  { XB_INVALID_LOCK_OPTION,     "Invalid Lock Option" },
  { XB_LOCK_FAILED,             "Lock Failed" },
  { XB_TABLE_NOT_LOCKED,        "Table Not Locked" },
  { XB_PARSE_ERROR,             "Parse Error" },
  { XB_INVALID_FUNCTION,        "Invalid or Undefined Function" },
  { XB_INVALID_PARM,            "Invalid Parm" },
  { XB_INCONSISTENT_PARM_LENS,  "Inconsistent parm lengths" },
  { XB_INCOMPATIBLE_OPERANDS,   "Incompatible operands" },
  { XB_UNBALANCED_PARENS,       "Unbalanced Parens" },
  { XB_UNBALANCED_QUOTES,       "Unbalanced Quotes" },
  { XB_INVALID_EXPRESSION,      "Invalid expression" },
  { XB_INVALID_KEYNO,           "Invalid Key Number" },
  { XB_INVALID_INDEX,           "Index File Error" },
  { XB_INVALID_TAG,             "Invalid index tag" },
  { XB_SYNTAX_ERROR,            "Invalid SQL Syntax" },
  { XB_MAX_ERROR_NO,            "End of Error List" }
};


}        /* namespace */
#endif   /* __XB_RETCODES_H__ */

