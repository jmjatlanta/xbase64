cmake_minimum_required(VERSION 3.2...5.0)

project (Xbase64)
set( xbase_VERSION_MAJOR 4 )
set( xbase_VERSION_MINOR 1 )
set( xbase_VERSION_PATCH 7 )

MESSAGE( "--- Xbase build for:          " ${CMAKE_SYSTEM_NAME} )
MESSAGE( "--- Compiler:                 " ${CMAKE_CXX_COMPILER_ID} )
MESSAGE( "--- Project Name:             " ${CMAKE_PROJECT_NAME} )
MESSAGE( "--- Install Prefix:           " ${CMAKE_INSTALL_PREFIX} )

#Set compiler options

IF( CMAKE_COMPILER_IS_GNUCXX )
  MESSAGE( "Setting GNU compiler options" )
  add_compile_options( -Wall -Wsign-conversion )
#  add_compile_options( -Wall -Wconversion -Wsign-conversion )
  SET( VERBOSE 1 )
ENDIF( CMAKE_COMPILER_IS_GNUCXX )

IF( BORLAND )
  MESSAGE ("BORLAND COMPILER" )
  #set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w-8022" )
  add_compile_options( -w-8022 )
ENDIF( BORLAND )

MESSAGE( "--- Checking integer sizes" )
INCLUDE(CheckTypeSize)
CHECK_TYPE_SIZE( "short int" SHORT_INT )
CHECK_TYPE_SIZE( int         INT )
CHECK_TYPE_SIZE( __int32     __INT32 )
CHECK_TYPE_SIZE( int32_t     INT32_T )
CHECK_TYPE_SIZE( long        LONG_INT )
CHECK_TYPE_SIZE( __int64     __INT64 )
CHECK_TYPE_SIZE( int64_t     INT64_T )
CHECK_TYPE_SIZE( "long long" LONG_LONG )
CHECK_TYPE_SIZE( u_int64_t   U_INT64_T )
CHECK_TYPE_SIZE( uint64_t    UINT64_T )
CHECK_TYPE_SIZE( "unsigned   __int64"   UNSIGNED__INT64 )
CHECK_TYPE_SIZE( "unsigned   long long" UNSIGNED_LONG_LONG )

CHECK_TYPE_SIZE( "double"  DOUBLE )
MESSAGE( "--- Double size = " ${DOUBLE} )


#determine how 16 bit integer defined on this platform
SET( iSize 0 )
IF( HAVE_SHORT_INT AND ${SHORT_INT} EQUAL 2 )
  SET( xbInt16  "short int" )
  SET( xbUInt16 "unsigned short int" )
  SET( iSize    ${SHORT_INT} )
  MESSAGE( "--- xbInt16  = [short int]" )
  MESSAGE( "--- xbUInt16 = [unsigned short int]" )

ENDIF( HAVE_SHORT_INT AND ${SHORT_INT} EQUAL 2 )
IF( NOT ${iSize} EQUAL 2 )
  MESSAGE( "FATAL_ERROR Could not detect 16 bit integer type" )
  MESSAGE( ${iSize} )
  RETURN()
ENDIF( NOT ${iSize} EQUAL 2 )

#determine how 32 bit integer defined on this platform
SET( iSize 0 )
IF( HAVE_INT AND ${INT} EQUAL 4 )
  SET( xbInt32  int )
  SET( xbUInt32 "unsigned int" )
  SET( iSize    ${INT} )
  MESSAGE( "--- xbInt32  = [int]" )
  MESSAGE( "--- xbUInt32 = [unsigned int]" )
ENDIF( HAVE_INT AND ${INT} EQUAL 4 )

IF(( NOT iSize EQUAL 4 ) AND HAVE___INT32 )
  MESSAGE( __int untested if block - see CMakeLists.txt)
  SET( xbInt32  __int32 )
  SET( xbUInt32 "unsigned __int32" )
  SET( iSize    ${__INT} )
  MESSAGE( "--- xbInt32  = [__int]" )
  MESSAGE( "--- xbUInt32 = [unsigned __int]" )
ENDIF(( NOT iSize EQUAL 4 ) AND HAVE___INT32 )

IF(( NOT iSize EQUAL 4 ) AND HAVE_INT32_T )
  SET( xbInt32  int32_t )
  SET( xbUInt32 "u_int32_t" )
  SET( iSize    ${INT32_T} )
  MESSAGE( "--- xbInt32  = [int32_t]" )
  MESSAGE( "--- xbUInt32 = [u_int32_t]" )
ENDIF(( NOT iSize EQUAL 4 ) AND HAVE_INT32_T )

IF( NOT ${iSize} EQUAL 4 )
  MESSAGE( "FATAL_ERROR Could not detect 32 bit integer type" )
  MESSAGE( ${iSize} )
  RETURN()
ENDIF( NOT ${iSize} EQUAL 4 )

#determine how 64 bit integer defined on this platform
SET( iSize 0 )
IF( HAVE___INT64 )
  SET( xbInt64  __int64 )
  SET( iSize    ${__INT64} )
  MESSAGE( "--- xbInt64  = [__int64]" )
ENDIF( HAVE___INT64 )

IF(( NOT iSize EQUAL 8 ) AND HAVE_INT64_T )
  SET( xbInt64  int64_t )
  SET( iSize    ${INT64_T} )
  MESSAGE( "--- xbInt64  = [int64_t]" )
ENDIF(( NOT iSize EQUAL 8 ) AND HAVE_INT64_T )

IF(( NOT iSize EQUAL 8 ) AND HAVE_LONG_LONG )
  SET( xbInt64  "long long" )
  SET( iSize    ${LONG_LONG} )
  MESSAGE( "--- xbInt64  = [long long]" )
ENDIF(( NOT iSize EQUAL 8 ) AND HAVE_LONG_LONG  )

IF( NOT ${iSize} EQUAL 8 )
  MESSAGE( "FATAL_ERROR Could not detect 64 bit integer type" )
  MESSAGE( ${iSize} )
  RETURN()
ENDIF( NOT ${iSize} EQUAL 8 )

#determine how 64 bit unsigned integer defined on this platform
SET( iSize 0 )
IF( HAVE_UNSIGNED__INT64 )
  SET( xbUInt64  "unsigned __int64" )
  SET( iSize    ${UNSIGNED__INT64} )
  MESSAGE( "--- xbUInt64 = [unsigned __int64]" )
ENDIF( HAVE_UNSIGNED__INT64 )

IF(( NOT iSize EQUAL 8 ) AND HAVE_UINT64_T )
  SET( xbUInt64 "uint64_t" )
  SET( iSize    ${UINT64_T} )
  MESSAGE( "--- xbUInt64 = [uint64_t]" )
ENDIF(( NOT iSize EQUAL 8 ) AND HAVE_UINT64_T  )

IF(( NOT iSize EQUAL 8 ) AND HAVE_U_INT64_T )
  SET( xbUInt64 u_int64_t )
  SET( iSize    ${U_INT64_T} )
  MESSAGE( "--- xbUInt64 = [u_int64_t]" )
ENDIF(( NOT iSize EQUAL 8 ) AND HAVE_U_INT64_T )

IF(( NOT iSize EQUAL 8 ) AND HAVE_UNSIGNED_LONG_LONG )
  SET( xbUInt64 "unsigned long long" )
  SET( iSize    ${UNSIGNED_LONG_LONG} )
  MESSAGE( "--- xbUInt64 = [unsigned long long]" )
ENDIF(( NOT iSize EQUAL 8 ) AND HAVE_UNSIGNED_LONG_LONG )

IF( NOT ${iSize} EQUAL 8 )
  MESSAGE( "FATAL_ERROR Could not detect 64 bit unsigned integer type" )
  MESSAGE( ${iSize} )
  RETURN()
ENDIF( NOT ${iSize} EQUAL 8 )

#Library options
option( BUILD_SHARED_LIBS     "Build shared libraries"                          ON)
option( XB_DBF3_SUPPORT       "Compile DBase File Format Version 3 support"     ON)
option( XB_DBF4_SUPPORT       "Compile DBase File Format Version 4 support"     ON)
option( XB_MEMO_SUPPORT       "Compile memo field support into library"         ON)
option( XB_NDX_SUPPORT        "Compile NDX Index support into library"          ON)
option( XB_MDX_SUPPORT        "Compile MDX Index support into library"          ON)
option( XB_LOCKING_SUPPORT    "Compile Multi User Locking support into library" ON)
option( XB_TDX_SUPPORT        "Compile temporary index support into library"    ON)

option( XB_LOGGING_SUPPORT    "Compile message logging support into library"    ON)
option( XB_LINKLIST_SUPPORT   "Compile Link List routines into library"         ON)
option( XB_FUNCTION_SUPPORT   "Compile XBase Function support into library"     ON)
option( XB_EXPRESSION_SUPPORT "Compile Expression support into library"         ON)
option( XB_INF_SUPPORT        "Compile NDX INF file support into library"       ON)
option( XB_FILTER_SUPPORT     "Compile Filter support into library"             ON)
option( XB_BLOCKREAD_SUPPORT  "Compile Block Read support into library"         ON)
option( XB_RELATE_SUPPORT     "Compile relation support into library"           ON)
option( XB_SQL_SUPPORT        "Compile SQL support into library - alpha"        ON)
option( XB_DEBUG_SUPPORT      "Compile debug logic into library"                ON)
option( XB_UTILS_SUPPORT      "Compile utility programs"                        ON)
option( XB_EXAMPLES_SUPPORT   "Compile example programs"                        ON)

Message( "--- BUILD_SHARED_LIBS     " ${BUILD_SHARED_LIBS})
Message( "--- XB_DBF3_SUPPORT       " ${XB_DBF3_SUPPORT})
Message( "--- XB_DBF4_SUPPORT       " ${XB_DBF4_SUPPORT})
Message( "--- XB_MEMO_SUPPORT       " ${XB_MEMO_SUPPORT})
Message( "--- XB_NDX_SUPPORT        " ${XB_NDX_SUPPORT})
Message( "--- XB_MDX_SUPPORT        " ${XB_MDX_SUPPORT})
Message( "--- XB_TDX_SUPPORT        " ${XB_TDX_SUPPORT})
Message( "--- XB_LOCKING_SUPPORT    " ${XB_LOCKING_SUPPORT})
Message( "--- XB_LINKLIST_SUPPORT   " ${XB_LINKLIST_SUPPORT})
Message( "--- XB_FUNCTION_SUPPORT   " ${XB_FUNCTION_SUPPORT})
Message( "--- XB_EXPRESSION_SUPPORT " ${XB_EXPRESSION_SUPPORT})
Message( "--- XB_FILTER_SUPPORT     " ${XB_FILTER_SUPPORT})
Message( "--- XB_DEBUG_SUPPORT      " ${XB_DEBUG_SUPPORT})
Message( "--- XB_UTILS_SUPPORT      " ${XB_UTILS_SUPPORT})
Message( "--- XB_EXAMPLES_SUPPORT   " ${XB_EXAMPLES_SUPPORT})
Message( "--- XB_LOGGING_SUPPORT    " ${XB_LOGGING_SUPPORT})
Message( "--- XB_SQL_SUPPORT        " ${XB_SQL_SUPPORT})
Message( "--- XB_INF_SUPPORT        " ${XB_INF_SUPPORT})
Message( "--- XB_RELATE_SUPPORT     " ${XB_RELATE_SUPPORT})
Message( "--- XB_BLOCKREAD_SUPPORT  " ${XB_BLOCKREAD_SUPPORT})

IF( XB_DEBUG_SUPPORT )
  Message( "--- Adding debug support" )
  set( CMAKE_BUILD_TYPE Debug )
IF( CMAKE_COMPILER_IS_GNUCXX )
  add_compile_options( -Og )
ENDIF( CMAKE_COMPILER_IS_GNUCXX )
ENDIF( XB_DEBUG_SUPPORT )

IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    MESSAGE( "--- 64 bit compiler detected" )
    set( XB_PLATFORM "x64" )
    set( XB_PLATFORM_64 1 )
    set( XB_LIBNAME  "xbase64" )

ELSE( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    MESSAGE( "--- 32 bit compiler detected" )
    set( XB_PLATFORM "x86" )
    set( XB_PLATFORM_32 1 )
    set( XB_LIBNAME  "xbase32" )

ENDIF( CMAKE_SIZEOF_VOID_P EQUAL 8 )


#Check for the existence of various header files
INCLUDE (CheckIncludeFiles)
CHECK_INCLUDE_FILES (ctype.h    HAVE_CTYPE_H)
CHECK_INCLUDE_FILES (dirent.h   HAVE_DIRENT_H)
CHECK_INCLUDE_FILES (fcntl.h    HAVE_FCNTL_H)
CHECK_INCLUDE_FILES (inttypes.h HAVE_INTTYPES_H)
CHECK_INCLUDE_FILES (pwd.h      HAVE_PWD_H)
CHECK_INCLUDE_FILES (stdarg.h   HAVE_STDARG_H)
CHECK_INCLUDE_FILES (stdargs.h  HAVE_STDARGS_H)
CHECK_INCLUDE_FILES (string.h   HAVE_STRING_H)
CHECK_INCLUDE_FILES (strings.h  HAVE_STRINGS_H)
CHECK_INCLUDE_FILES (sys/stat.h HAVE_STAT_H)
CHECK_INCLUDE_FILES (unistd.h   HAVE_UNISTD_H)
CHECK_INCLUDE_FILES (varargs.h  HAVE_VARARGS_H)
CHECK_INCLUDE_FILES (windows.h  HAVE_WINDOWS_H)

#Check for the existence of various function calls
INCLUDE( CheckFunctionExists )
CHECK_FUNCTION_EXISTS(_close         HAVE__CLOSE_F)
CHECK_FUNCTION_EXISTS(fcntl          HAVE_FCNTL_F)
CHECK_FUNCTION_EXISTS(_fdopen        HAVE__FDOPEN_F)
CHECK_FUNCTION_EXISTS(fopen_s        HAVE_FOPEN_S_F)
CHECK_FUNCTION_EXISTS(_fsopen        HAVE__FSOPEN_F)
CHECK_FUNCTION_EXISTS(_fileno        HAVE__FILENO_F)
CHECK_FUNCTION_EXISTS(fork           HAVE_FORK_F)
CHECK_FUNCTION_EXISTS(_fseeki64      HAVE__FSEEKI64_F)
CHECK_FUNCTION_EXISTS(fseeko         HAVE_FSEEKO_F)
CHECK_FUNCTION_EXISTS(ftruncate      HAVE_FTRUNCATE_F)
CHECK_FUNCTION_EXISTS(getenv_s       HAVE_GETENV_S_F)

CHECK_FUNCTION_EXISTS(_localtime64_s HAVE__LOCALTIME64_S_F)
CHECK_FUNCTION_EXISTS(LockFile       HAVE_LOCKFILE_F)
CHECK_FUNCTION_EXISTS(locking        HAVE_LOCKING_F)
CHECK_FUNCTION_EXISTS(_locking       HAVE__LOCKING_F)
CHECK_FUNCTION_EXISTS(_open          HAVE__OPEN_F)
CHECK_FUNCTION_EXISTS(SetEndOfFile   HAVE_SETENDOFFILE_F)

CHECK_FUNCTION_EXISTS(vsnprintf      HAVE_VSNPRINTF_F)
CHECK_FUNCTION_EXISTS(_vsnprintf_s   HAVE__VSNPRINTF_S_F)


CHECK_FUNCTION_EXISTS(getopt         HAVE_GETOPT_F)

IF( WIN32 )

# Use CHECK_SYMBOL_EXISTS for the Win32API instead of CHECK_FUNCTION_EXISTS
#  Can't detect inline functions in headers
#  Can't detect due to mismatch of calling conventions
#  Can't detect mangled names

INCLUDE( CheckSymbolExists )
CHECK_SYMBOL_EXISTS(CreateProcessW Windows.h HAVE_CREATEPROCESSW_F )
CHECK_SYMBOL_EXISTS(vsprintf_s     stdio.h   HAVE_VSPRINTF_S_F )

ENDIF ( WIN32 )

# IF( NOT HAVE_SENTENDOFFILE_F AND NOT HAVE_FTRUNCATE_F AND BORLAND )
#  # ----fix me----
#  # cmake CHECK_FUNCTION_EXISTS - could not find Borland 5.5 SetEndOfFile
#  Set( HAVE_SETENDOFFILE_F "HAVE_SETENDOFFILE_H" )
# ENDIF ( NOT HAVE_SENTENDOFFILE_F AND NOT HAVE_FTRUNCATE_F AND BORLAND )

IF( NOT HAVE_SETENDOFFILE_F AND NOT HAVE_FTRUNCATE_F AND WIN32 )
  # ----fix me----
  # cmake CHECK_FUNCTION_EXISTS - could not find Borland 5.5, or Windows VS 32 bit SetEndOfFile
  Set( HAVE_SETENDOFFILE_F "HAVE_SETENDOFFILE_F" )
ENDIF ( NOT HAVE_SETENDOFFILE_F AND NOT HAVE_FTRUNCATE_F AND WIN32 )

IF( XB_NDX_SUPPORT OR XB_MDX_SUPPORT )
  Set( XB_INDEX_SUPPORT "1" )
ENDIF( XB_NDX_SUPPORT OR XB_MDX_SUPPORT )


# Set directories
# PROJECT_PARENT_DIR is two nodes higher
#get_filename_component( PROJECT_PARENT_DIR ${PROJECT_BINARY_DIR} DIRECTORY )
#get_filename_component( PROJECT_PARENT_DIR ${PROJECT_PARENT_DIR} DIRECTORY )
# the problem with the above is that PROJECT_BINARY_DIR is wrong when using this project as a git submodule
cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH PROJECT_PARENT_DIR)
cmake_path(GET PROJECT_PARENT_DIR PARENT_PATH PROJECT_PARENT_DIR)

# MESSAGE( "PROJECT_PARENT_DIR = " ${PROJECT_PARENT_DIR} )
# MESSAGE( "PROJECT_BINARY_DIR = " ${PROJECT_BINARY_DIR} )


set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib )

set( PROJECT_SOURCE_DIR             ${PROJECT_PARENT_DIR}/src )
set( PROJECT_DATA_DIR               ${PROJECT_PARENT_DIR}/data )
set( PROJECT_LOG_DIR                ${PROJECT_PARENT_DIR}/logs )
set( PROJECT_TEMP_DIR               ${PROJECT_PARENT_DIR}/temp )
set( EXTRA_LIBS                     ${EXTRA_LIBS} ${XB_LIBNAME} )


# Undo bad decisions
IF( XB_PLATFORM_32 AND XB_LARGEFILE_SUPPORT )
    MESSAGE( "--- Can't build 64 bit environment with a 32 bit compiler" )
    MESSAGE( "--- XB_LARGEFILE_SUPPORT set to OFF" )
    set( XB_LARGEFILE_SUPPORT OFF CACHE BOOL "Use 64 bit large file support?" FORCE )
ENDIF ( XB_PLATFORM_32 AND XB_LARGEFILE_SUPPORT )

IF( NOT XB_DBF3_SUPPORT AND NOT XB_DBF4_SUPPORT )
    MESSAGE( "--- At lease one file format is required. Defaulting to Version 4" )
    set( XB_DBF4_SUPPORT ON CACHE BOOL "Compile DBase File Format Version 4 support" FORCE )
ENDIF( NOT XB_DBF3_SUPPORT AND NOT XB_DBF4_SUPPORT )

IF( XB_DBF4_SUPPORT AND XB_MEMO_SUPPORT AND NOT XB_LINKLIST_SUPPORT )
    MESSAGE( "--- Link List support required for DBF V4 Memo files. Setting to ON" )
    set( XB_LINKLIST_SUPPORT ON CACHE BOOL "Compile Link List routines into library" FORCE )
ENDIF( XB_DBF4_SUPPORT AND XB_MEMO_SUPPORT AND NOT XB_LINKLIST_SUPPORT )

IF( XB_LOCKING_SUPPORT AND NOT XB_LINKLIST_SUPPORT )
    MESSAGE( "--- Link List support required for locking functions. Setting to ON" )
    set( XB_LINKLIST_SUPPORT ON CACHE BOOL "Compile Link List routines into library" FORCE )
ENDIF( XB_LOCKING_SUPPORT AND NOT XB_LINKLIST_SUPPORT )

IF( XB_TDX_SUPPORT AND NOT XB_MDX_SUPPORT )
    MESSAGE( "--- MDX support required for Temporary indices. Setting to ON" )
    set( XB_MDX_SUPPORT ON CACHE BOOL "Compile MDX Index support into library" FORCE )
ENDIF( XB_TDX_SUPPORT AND NOT XB_MDX_SUPPORT )

IF( XB_RELATE_SUPPORT AND NOT XB_INDEX_SUPPORT )
    MESSAGE( "--- Index support required for RELATE logic. Setting to ON" )
    set( XB_MDX_SUPPORT ON CACHE BOOL "Compile MDX Index support into library" FORCE )
    set( XB_INDEX_SUPPORT "1" )
ENDIF( XB_RELATE_SUPPORT AND NOT XB_INDEX_SUPPORT )

IF( XB_RELATE_SUPPORT AND NOT XB_FILTER_SUPPORT )
    MESSAGE( "--- Index support required for RELATE logic. Setting to ON" )
    set( XB_FILTER_SUPPORT ON CACHE BOOL "Compile Filter support into library" FORCE )
ENDIF( XB_RELATE_SUPPORT AND NOT XB_FILTER_SUPPORT )

IF( XB_INDEX_SUPPORT AND NOT XB_EXPRESSION_SUPPORT )
    MESSAGE( "--- Expression required for indices. Setting to ON" )
    set( XB_EXPRESSION_SUPPORT ON CACHE BOOL "Compile Expression support into library" FORCE )
ENDIF( XB_INDEX_SUPPORT AND NOT XB_EXPRESSION_SUPPORT )

IF( XB_EXPRESSION_SUPPORT AND NOT XB_FUNCTION_SUPPORT )
    MESSAGE( "--- Function support required for expressions. Setting to ON" )
    set( XB_FUNCTION_SUPPORT ON CACHE BOOL "Compile XBase Function support into library" FORCE )
ENDIF( XB_EXPRESSION_SUPPORT AND NOT XB_FUNCTION_SUPPORT )

IF( XB_EXPRESSION_SUPPORT AND NOT XB_LINKLIST_SUPPORT )
    MESSAGE( "--- Linklist support required for expressions. Setting to ON" )
    set( XB_LINKLIST_SUPPORT ON CACHE BOOL "Compile Link List routines into library" FORCE )
ENDIF( XB_EXPRESSION_SUPPORT AND NOT XB_LINKLIST_SUPPORT )

IF( XB_SQL_SUPPORT AND NOT XB_MEMO_SUPPORT )
    MESSAGE( "--- Memo support required for SQL. Setting to ON" )
    set( XB_MEMO_SUPPORT ON CACHE BOOL "Compile memo field support into library" FORCE )
ENDIF( XB_SQL_SUPPORT AND NOT XB_MEMO_SUPPORT )

IF( XB_SQL_SUPPORT AND NOT XB_RELATE_SUPPORT )
    MESSAGE( "--- Relate support required for SQL. Setting to ON" )
    set( XB_RELATE_SUPPORT ON CACHE BOOL "Compile relate support into library" FORCE )
ENDIF( XB_SQL_SUPPORT AND NOT XB_RELATE_SUPPORT )

IF( XB_SQL_SUPPORT AND NOT XB_DBF4_SUPPORT )
    MESSAGE( "--- DBF4 support required for SQL. Setting to ON" )
    set( XB_DBF4_SUPPORT ON CACHE BOOL "Compile DBase File Format Version 4 support" FORCE )
ENDIF( XB_SQL_SUPPORT AND NOT XB_DBF4_SUPPORT )

IF( XB_FILTER_SUPPORT AND NOT XB_EXPRESSION_SUPPORT )
    MESSAGE( "--- Expression required for filters. Setting to ON" )
    set( XB_EXPRESSION_SUPPORT ON CACHE BOOL "Compile Expression support into library" FORCE )
ENDIF( XB_FILTER_SUPPORT AND NOT XB_EXPRESSION_SUPPORT )

IF( XB_SQL_SUPPORT AND NOT XB_BLOCKREAD_SUPPORT )
    MESSAGE( "--- BLOCKREAD support required for SQL. Setting to ON" )
    set( XB_BLOCKREAD_SUPPORT ON CACHE BOOL "Compile Block Read support into library" FORCE )
ENDIF( XB_SQL_SUPPORT AND NOT XB_BLOCKREAD_SUPPORT )

IF( XB_TDX_SUPPORT AND NOT XB_MDX_SUPPORT )
    MESSAGE( "--- MDX support required for Temporary indices. Setting to ON" )
    set( XB_MDX_SUPPORT ON CACHE BOOL "Compile temporary index support into library" FORCE )
ENDIF( XB_TDX_SUPPORT AND NOT XB_MDX_SUPPORT )



# Build configuration header file
configure_file (
    "${PROJECT_SOURCE_DIR}/include/xbconfig.h.in"
    "${PROJECT_BINARY_DIR}/include/xbconfig.h"
)

# Microsoft Windows compiler settings
IF( MSVC )
#  add_definitions( /D_CRT_SECURE_NO_WARNINGS)
  add_definitions( /EHsc )
ENDIF( MSVC )


# set up include directories
include_directories( "${PROJECT_BINARY_DIR}/include" )
include_directories( "${PROJECT_SOURCE_DIR}/include" )

#build the library
IF ( BUILD_SHARED_LIBS )
  set( LIB_TYPE SHARED )
ELSE( BUILD_SHARED_LIBS )
  set( LIB_TYPE STATIC )
ENDIF( BUILD_SHARED_LIBS )
MESSAGE( "--- Lib type " ${LIB_TYPE} )



set (SOURCES ${PROJECT_SOURCE_DIR}/core/xbstring.cpp
             ${PROJECT_SOURCE_DIR}/core/xbssv.cpp
             ${PROJECT_SOURCE_DIR}/core/xbdate.cpp
             ${PROJECT_SOURCE_DIR}/core/xbtblmgr.cpp
             ${PROJECT_SOURCE_DIR}/core/xbxbase.cpp
             ${PROJECT_SOURCE_DIR}/core/xbfile.cpp
             ${PROJECT_SOURCE_DIR}/core/xbdbf.cpp
             ${PROJECT_SOURCE_DIR}/core/xbfields.cpp
             ${PROJECT_SOURCE_DIR}/core/xblog.cpp)

IF( XB_MEMO_SUPPORT )
  set (SOURCES ${SOURCES}  ${PROJECT_SOURCE_DIR}/core/xbmemo.cpp )
ENDIF( XB_MEMO_SUPPORT )

IF( XB_DBF3_SUPPORT )
  set (SOURCES ${SOURCES}  ${PROJECT_SOURCE_DIR}/core/xbdbf3.cpp )
  IF( XB_MEMO_SUPPORT )
    set (SOURCES ${SOURCES}  ${PROJECT_SOURCE_DIR}/core/xbmemo3.cpp )
  ENDIF( XB_MEMO_SUPPORT )
ENDIF( XB_DBF3_SUPPORT )

IF( XB_DBF4_SUPPORT )
  set (SOURCES ${SOURCES}  ${PROJECT_SOURCE_DIR}/core/xbdbf4.cpp )
  IF( XB_MEMO_SUPPORT )
    set (SOURCES ${SOURCES}  ${PROJECT_SOURCE_DIR}/core/xbmemo4.cpp )
  ENDIF( XB_MEMO_SUPPORT )
ENDIF( XB_DBF4_SUPPORT )

IF( XB_FUNCTION_SUPPORT )
  set (SOURCES ${SOURCES} ${PROJECT_SOURCE_DIR}/core/xbfuncs.cpp )
ENDIF( XB_FUNCTION_SUPPORT )

IF( XB_EXPRESSION_SUPPORT )
  set (SOURCES ${SOURCES} ${PROJECT_SOURCE_DIR}/core/xbexpnode.cpp )
  set (SOURCES ${SOURCES} ${PROJECT_SOURCE_DIR}/core/xbexp.cpp  )
  set (SOURCES ${SOURCES} ${PROJECT_SOURCE_DIR}/core/xbuda.cpp     )
ENDIF( XB_EXPRESSION_SUPPORT )

IF( XB_INDEX_SUPPORT )
  set (SOURCES ${SOURCES} ${PROJECT_SOURCE_DIR}/core/xbbcd.cpp    )
  set (SOURCES ${SOURCES} ${PROJECT_SOURCE_DIR}/core/xbixbase.cpp )
  set (SOURCES ${SOURCES} ${PROJECT_SOURCE_DIR}/core/xbtag.cpp )
ENDIF( XB_INDEX_SUPPORT )

IF( XB_NDX_SUPPORT )
  set (SOURCES ${SOURCES} ${PROJECT_SOURCE_DIR}/core/xbixndx.cpp )
ENDIF( XB_NDX_SUPPORT )


IF( XB_MDX_SUPPORT )
 set (SOURCES ${SOURCES} ${PROJECT_SOURCE_DIR}/core/xbixmdx.cpp )
ENDIF( XB_MDX_SUPPORT )

IF( XB_TDX_SUPPORT )
 set (SOURCES ${SOURCES} ${PROJECT_SOURCE_DIR}/core/xbixtdx.cpp )
ENDIF( XB_TDX_SUPPORT )

IF( XB_RELATE_SUPPORT )
 set (SOURCES ${SOURCES} ${PROJECT_SOURCE_DIR}/core/xbrelate.cpp )
ENDIF( XB_RELATE_SUPPORT )


IF( XB_SQL_SUPPORT )
  set (SOURCES ${SOURCES}   ${PROJECT_SOURCE_DIR}/sql/xbalttbl.cpp )
  set (SOURCES ${SOURCES}   ${PROJECT_SOURCE_DIR}/sql/xbcrix.cpp   )
  set (SOURCES ${SOURCES}   ${PROJECT_SOURCE_DIR}/sql/xbcrtbl.cpp  )
  set (SOURCES ${SOURCES}   ${PROJECT_SOURCE_DIR}/sql/xbdelete.cpp )
  set (SOURCES ${SOURCES}   ${PROJECT_SOURCE_DIR}/sql/xbdrpix.cpp  )
  set (SOURCES ${SOURCES}   ${PROJECT_SOURCE_DIR}/sql/xbdrptbl.cpp )
  set (SOURCES ${SOURCES}   ${PROJECT_SOURCE_DIR}/sql/xbinsert.cpp )
  set (SOURCES ${SOURCES}   ${PROJECT_SOURCE_DIR}/sql/xbset.cpp    )
  set (SOURCES ${SOURCES}   ${PROJECT_SOURCE_DIR}/sql/xbsql.cpp    )
  set (SOURCES ${SOURCES}   ${PROJECT_SOURCE_DIR}/sql/xbstmt.cpp   )
  set (SOURCES ${SOURCES}   ${PROJECT_SOURCE_DIR}/sql/xbselect.cpp )
ENDIF( XB_SQL_SUPPORT )


IF( XB_FILTER_SUPPORT )
 set (SOURCES ${SOURCES} ${PROJECT_SOURCE_DIR}/core/xbfilter.cpp )
ENDIF( XB_FILTER_SUPPORT )

IF( XB_BLOCKREAD_SUPPORT )
 set (SOURCES ${SOURCES} ${PROJECT_SOURCE_DIR}/core/xbblockread.cpp )
ENDIF( XB_BLOCKREAD_SUPPORT )


include( GenerateExportHeader )
#add_library( xbase ${LIB_TYPE} ${SOURCES} )
add_library( ${XB_LIBNAME} ${LIB_TYPE} ${SOURCES} )
#add_library( ${CMAKE_PROJECT_NAME} ${LIB_TYPE} ${SOURCES} )


IF( WIN32 )
MESSAGE( "--- Generating Windows Library Headers" )

#GENERATE_EXPORT_HEADER( xbase
#        BASE_NAME xbase

GENERATE_EXPORT_HEADER( ${XB_LIBNAME}
        BASE_NAME ${XB_LIBNAME}
        EXPORT_MACRO_NAME xbase_EXPORT
        EXPORT_FILE_NAME xbase_Export.h
        STATIC_DEFINE xbase_BUILT_AS_STATIC
)
ENDIF( WIN32 )


#build test programs
enable_testing()

add_executable( xb_test_string                  ${PROJECT_SOURCE_DIR}/tests/xb_test_string.cpp )
target_link_libraries( xb_test_string           ${EXTRA_LIBS})
add_test( NAME xbString COMMAND xb_test_string QUIET )

add_executable( xb_test_date                    ${PROJECT_SOURCE_DIR}/tests/xb_test_date.cpp )
target_link_libraries( xb_test_date             ${EXTRA_LIBS})
add_test( NAME xbDate COMMAND xb_test_date QUIET )

add_executable( xb_test_tblmgr                  ${PROJECT_SOURCE_DIR}/tests/xb_test_tblmgr.cpp )
target_link_libraries( xb_test_tblmgr           ${EXTRA_LIBS})
add_test( NAME xbTblMgr COMMAND xb_test_tblmgr QUIET )

add_executable( xb_test_xbase                   ${PROJECT_SOURCE_DIR}/tests/xb_test_xbase.cpp )
target_link_libraries( xb_test_xbase            ${EXTRA_LIBS})
add_test( NAME xbXBase COMMAND xb_test_xbase QUIET )

add_executable( xb_test_file                    ${PROJECT_SOURCE_DIR}/tests/xb_test_file.cpp )
target_link_libraries( xb_test_file             ${EXTRA_LIBS})
add_test( NAME xbFile COMMAND xb_test_file QUIET )

add_executable( xb_test_log                     ${PROJECT_SOURCE_DIR}/tests/xb_test_log.cpp )
target_link_libraries( xb_test_log              ${EXTRA_LIBS})
add_test( NAME xbLog COMMAND xb_test_log QUIET )

IF( XB_LINKLIST_SUPPORT )
add_executable( xb_test_linklist                ${PROJECT_SOURCE_DIR}/tests/xb_test_linklist.cpp )
target_link_libraries( xb_test_linklist         ${EXTRA_LIBS})
add_test( NAME xbLinkList COMMAND xb_test_linklist QUIET )
ENDIF( XB_LINKLIST_SUPPORT )

IF( XB_DBF3_SUPPORT )
add_executable( xb_test_dbf_v3_nomemos          ${PROJECT_SOURCE_DIR}/tests/xb_test_dbf_v3_nomemos.cpp )
target_link_libraries( xb_test_dbf_v3_nomemos   ${EXTRA_LIBS})
add_test( NAME xbDbfV3NoMemos COMMAND xb_test_dbf_v3_nomemos QUIET )

IF( XB_MEMO_SUPPORT )
add_executable( xb_test_dbf_v3_memos            ${PROJECT_SOURCE_DIR}/tests/xb_test_dbf_v3_memos.cpp )
target_link_libraries( xb_test_dbf_v3_memos     ${EXTRA_LIBS})
add_test( NAME xbDbfV3Memos COMMAND xb_test_dbf_v3_memos QUIET )

ENDIF( XB_MEMO_SUPPORT )
ENDIF( XB_DBF3_SUPPORT )

IF( XB_DBF4_SUPPORT )
add_executable( xb_test_dbf_v4_nomemos          ${PROJECT_SOURCE_DIR}/tests/xb_test_dbf_v4_nomemos.cpp )
target_link_libraries( xb_test_dbf_v4_nomemos   ${EXTRA_LIBS})
add_test( NAME xbDbfV4NoMemos COMMAND xb_test_dbf_v4_nomemos QUIET )

IF( XB_MEMO_SUPPORT )
add_executable( xb_test_dbf_v4_memos            ${PROJECT_SOURCE_DIR}/tests/xb_test_dbf_v4_memos.cpp )
target_link_libraries( xb_test_dbf_v4_memos     ${EXTRA_LIBS})
add_test( NAME xbDbfV4Memos COMMAND xb_test_dbf_v4_memos QUIET )

ENDIF( XB_MEMO_SUPPORT )
ENDIF( XB_DBF4_SUPPORT )

IF( XB_FUNCTION_SUPPORT )
add_executable( xb_test_funcs                   ${PROJECT_SOURCE_DIR}/tests/xb_test_funcs.cpp )
target_link_libraries( xb_test_funcs            ${EXTRA_LIBS})
add_test( NAME xbFunctions COMMAND xb_test_funcs QUIET )
ENDIF( XB_FUNCTION_SUPPORT )

IF( XB_NDX_SUPPORT )
IF( XB_DBF3_SUPPORT )
add_executable( xb_test_ndx                     ${PROJECT_SOURCE_DIR}/tests/xb_test_ndx.cpp )
target_link_libraries( xb_test_ndx              ${EXTRA_LIBS})
add_test( NAME xbIxNdx COMMAND xb_test_ndx QUIET )
ENDIF( XB_DBF3_SUPPORT )
ENDIF( XB_NDX_SUPPORT )

IF( XB_RELATE_SUPPORT )
add_executable( xb_test_relate1                 ${PROJECT_SOURCE_DIR}/tests/xb_test_relate1.cpp )
target_link_libraries( xb_test_relate1          ${EXTRA_LIBS})
add_test( NAME xbRelate1 COMMAND xb_test_relate1 QUIET )

add_executable( xb_test_relate2                 ${PROJECT_SOURCE_DIR}/tests/xb_test_relate2.cpp )
target_link_libraries( xb_test_relate2          ${EXTRA_LIBS})
add_test( NAME xbRelate2 COMMAND xb_test_relate2 QUIET )
ENDIF( XB_RELATE_SUPPORT )

IF( XB_DBF4_SUPPORT )

IF( XB_MDX_SUPPORT )
add_executable( xb_test_mdx                     ${PROJECT_SOURCE_DIR}/tests/xb_test_mdx.cpp )
target_link_libraries( xb_test_mdx              ${EXTRA_LIBS})
add_test( NAME xbIxMdx COMMAND xb_test_mdx QUIET )
ENDIF( XB_MDX_SUPPORT )

IF( XB_TDX_SUPPORT )
add_executable( xb_test_tdx                     ${PROJECT_SOURCE_DIR}/tests/xb_test_tdx.cpp )
target_link_libraries( xb_test_tdx              ${EXTRA_LIBS})
add_test( NAME xbIxTdx COMMAND xb_test_tdx QUIET )
ENDIF( XB_TDX_SUPPORT )

ENDIF( XB_DBF4_SUPPORT )



IF( XB_INDEX_SUPPORT )
add_executable( xb_test_bcd                     ${PROJECT_SOURCE_DIR}/tests/xb_test_bcd.cpp )
target_link_libraries( xb_test_bcd              ${EXTRA_LIBS})
add_test( NAME xbBcd COMMAND xb_test_bcd QUIET )
ENDIF( XB_INDEX_SUPPORT )


IF( XB_EXPRESSION_SUPPORT )

add_executable( xb_test_expnode                 ${PROJECT_SOURCE_DIR}/tests/xb_test_expnode.cpp )
target_link_libraries( xb_test_expnode          ${EXTRA_LIBS})
add_test( NAME xbExpNode COMMAND xb_test_expnode QUIET )


add_executable( xb_test_expression              ${PROJECT_SOURCE_DIR}/tests/xb_test_expression.cpp )
target_link_libraries( xb_test_expression       ${EXTRA_LIBS})
add_test( NAME xbExpressions COMMAND xb_test_expression QUIET )

add_executable( xb_test_uda                     ${PROJECT_SOURCE_DIR}/tests/xb_test_uda.cpp )
target_link_libraries( xb_test_uda              ${EXTRA_LIBS})
add_test( NAME xbUda         COMMAND xb_test_uda        QUIET )

ENDIF( XB_EXPRESSION_SUPPORT )

IF( XB_SQL_SUPPORT )
add_executable( xb_test_sql                     ${PROJECT_SOURCE_DIR}/tests/xb_test_sql.cpp )
target_link_libraries( xb_test_sql              ${EXTRA_LIBS})
add_test( NAME xbSql COMMAND xb_test_sql QUIET )
ENDIF( XB_SQL_SUPPORT )

IF( XB_FILTER_SUPPORT )
add_executable( xb_test_filter                  ${PROJECT_SOURCE_DIR}/tests/xb_test_filter.cpp )
target_link_libraries( xb_test_filter           ${EXTRA_LIBS})
add_test( NAME xbFilter COMMAND xb_test_filter QUIET )
ENDIF( XB_FILTER_SUPPORT )

IF( XB_BLOCKREAD_SUPPORT )
add_executable( xb_test_blockread               ${PROJECT_SOURCE_DIR}/tests/xb_test_blockread.cpp )
target_link_libraries( xb_test_blockread        ${EXTRA_LIBS})
add_test( NAME xbBlockRead COMMAND xb_test_blockread QUIET )
ENDIF( XB_BLOCKREAD_SUPPORT )

IF( XB_LOCKING_SUPPORT )
add_executable( xb_test_lock                    ${PROJECT_SOURCE_DIR}/tests/xb_test_lock.cpp )
target_link_libraries( xb_test_lock             ${EXTRA_LIBS})
add_test( NAME xbLock COMMAND xb_test_lock QUIET )
IF( HAVE_CREATEPROCESSW_F )
add_executable( xb_test_lock2                   ${PROJECT_SOURCE_DIR}/tests/xb_test_lock2.cpp )
target_link_libraries( xb_test_lock2            ${EXTRA_LIBS})
ENDIF( HAVE_CREATEPROCESSW_F )
ENDIF( XB_LOCKING_SUPPORT )

#build example programs
IF( XB_EXAMPLES_SUPPORT )
  add_executable( xb_ex_string                    ${PROJECT_SOURCE_DIR}/examples/xb_ex_string.cpp )
  target_link_libraries( xb_ex_string             ${EXTRA_LIBS}) 

  add_executable( xb_ex_date                      ${PROJECT_SOURCE_DIR}/examples/xb_ex_date.cpp )
  target_link_libraries( xb_ex_date               ${EXTRA_LIBS})

  IF( XB_DBF3_SUPPORT )
  add_executable( xb_ex_v3_create_dbf             ${PROJECT_SOURCE_DIR}/examples/xb_ex_v3_create_dbf.cpp )
  target_link_libraries( xb_ex_v3_create_dbf      ${EXTRA_LIBS})

  add_executable( xb_ex_v3_upd_dbf                ${PROJECT_SOURCE_DIR}/examples/xb_ex_v3_upd_dbf.cpp )
  target_link_libraries( xb_ex_v3_upd_dbf         ${EXTRA_LIBS})
  ENDIF( XB_DBF3_SUPPORT )

  IF( XB_DBF4_SUPPORT )
  add_executable( xb_ex_v4_create_dbf             ${PROJECT_SOURCE_DIR}/examples/xb_ex_v4_create_dbf.cpp )
  target_link_libraries( xb_ex_v4_create_dbf      ${EXTRA_LIBS})
  ENDIF( XB_DBF4_SUPPORT )

  IF( XB_EXPRESSION_SUPPORT )
  add_executable( xb_ex_expression                ${PROJECT_SOURCE_DIR}/examples/xb_ex_expression.cpp )
  target_link_libraries( xb_ex_expression         ${EXTRA_LIBS})
  ENDIF( XB_EXPRESSION_SUPPORT )

  add_executable( xb_ex_v4_upd_dbf                ${PROJECT_SOURCE_DIR}/examples/xb_ex_v4_upd_dbf.cpp )
  target_link_libraries( xb_ex_v4_upd_dbf         ${EXTRA_LIBS})

  add_executable( xb_ex_ssv                       ${PROJECT_SOURCE_DIR}/examples/xb_ex_ssv.cpp )
  target_link_libraries( xb_ex_ssv                ${EXTRA_LIBS})

  IF( XB_LOGGING_SUPPORT )
  add_executable( xb_ex_log                       ${PROJECT_SOURCE_DIR}/examples/xb_ex_log.cpp )
  target_link_libraries( xb_ex_log                ${EXTRA_LIBS})
  ENDIF( XB_LOGGING_SUPPORT )

  IF( XB_RELATE_SUPPORT )
  add_executable( xb_ex_relation                  ${PROJECT_SOURCE_DIR}/examples/xb_ex_relation.cpp )
  target_link_libraries( xb_ex_relation           ${EXTRA_LIBS})
  ENDIF( XB_RELATE_SUPPORT )

  IF( XB_FILTER_SUPPORT )
  add_executable( xb_ex_filter                    ${PROJECT_SOURCE_DIR}/examples/xb_ex_filter.cpp )
  target_link_libraries( xb_ex_filter             ${EXTRA_LIBS})
  ENDIF( XB_FILTER_SUPPORT )


ENDIF( XB_EXAMPLES_SUPPORT )

#build utility programs
IF( XB_UTILS_SUPPORT )

  add_executable( xb_cfg_check                    ${PROJECT_SOURCE_DIR}/utils/xb_cfg_check.cpp )
  target_link_libraries( xb_cfg_check             ${EXTRA_LIBS})

  add_executable( xb_copydbf                      ${PROJECT_SOURCE_DIR}/utils/xb_copydbf.cpp )
  target_link_libraries( xb_copydbf               ${EXTRA_LIBS})

  add_executable( xb_dbfutil                      ${PROJECT_SOURCE_DIR}/utils/xb_dbfutil.cpp )
  target_link_libraries( xb_dbfutil               ${EXTRA_LIBS})

  add_executable( xb_deletall                     ${PROJECT_SOURCE_DIR}/utils/xb_deletall.cpp )
  target_link_libraries( xb_deletall              ${EXTRA_LIBS})

  IF( XB_LOGGING_SUPPORT )
  add_executable( xb_dumpdbt                      ${PROJECT_SOURCE_DIR}/utils/xb_dumpdbt.cpp )
  target_link_libraries( xb_dumpdbt               ${EXTRA_LIBS})
  ENDIF( XB_LOGGING_SUPPORT )

  add_executable( xb_dumprecs                     ${PROJECT_SOURCE_DIR}/utils/xb_dumprecs.cpp )
  target_link_libraries( xb_dumprecs              ${EXTRA_LIBS})

  IF( XB_SQL_SUPPORT )
  add_executable( xb_execsql                      ${PROJECT_SOURCE_DIR}/utils/xb_execsql.cpp )
  target_link_libraries( xb_execsql               ${EXTRA_LIBS})
  ENDIF( XB_SQL_SUPPORT )

  add_executable( xb_import                       ${PROJECT_SOURCE_DIR}/utils/xb_import.cpp )
  target_link_libraries( xb_import                ${EXTRA_LIBS})

  IF( XB_INDEX_SUPPORT )
  add_executable( xb_reindex                      ${PROJECT_SOURCE_DIR}/utils/xb_reindex.cpp )
  target_link_libraries( xb_reindex               ${EXTRA_LIBS})
  ENDIF( XB_INDEX_SUPPORT )

  add_executable( xb_tblinfo                      ${PROJECT_SOURCE_DIR}/utils/xb_tblinfo.cpp )
  target_link_libraries( xb_tblinfo               ${EXTRA_LIBS})

  add_executable( xb_pack                         ${PROJECT_SOURCE_DIR}/utils/xb_pack.cpp )
  target_link_libraries( xb_pack                  ${EXTRA_LIBS})

  add_executable( xb_undelall                     ${PROJECT_SOURCE_DIR}/utils/xb_undelall.cpp )
  target_link_libraries( xb_undelall              ${EXTRA_LIBS})

  install (TARGETS xb_cfg_check DESTINATION bin)
  install (TARGETS xb_copydbf   DESTINATION bin)
  install (TARGETS xb_dbfutil   DESTINATION bin)
  install (TARGETS xb_deletall  DESTINATION bin)
  install (TARGETS xb_dumpdbt   DESTINATION bin)
  install (TARGETS xb_dumprecs  DESTINATION bin)
  IF( XB_SQL_SUPPORT )
  install (TARGETS xb_execsql   DESTINATION bin)
  ENDIF( XB_SQL_SUPPORT )
  install (TARGETS xb_import    DESTINATION bin)
  install (TARGETS xb_pack      DESTINATION bin)
  IF( XB_INDEX_SUPPORT )
  install (TARGETS xb_reindex   DESTINATION bin)
  ENDIF( XB_INDEX_SUPPORT )
  install (TARGETS xb_tblinfo   DESTINATION bin)
  install (TARGETS xb_undelall  DESTINATION bin)



ENDIF( XB_UTILS_SUPPORT )

install (FILES ${PROJECT_BINARY_DIR}/include/xbconfig.h
               ${PROJECT_SOURCE_DIR}/include/xbase.h
               ${PROJECT_SOURCE_DIR}/include/xbbcd.h
               ${PROJECT_SOURCE_DIR}/include/xbblkread.h
               ${PROJECT_SOURCE_DIR}/include/xbdate.h
               ${PROJECT_SOURCE_DIR}/include/xbdbf.h
               ${PROJECT_SOURCE_DIR}/include/xbexp.h
               ${PROJECT_SOURCE_DIR}/include/xbexpnode.h
               ${PROJECT_SOURCE_DIR}/include/xbfile.h
               ${PROJECT_SOURCE_DIR}/include/xbfilter.h
               ${PROJECT_SOURCE_DIR}/include/xbindex.h
               ${PROJECT_SOURCE_DIR}/include/xblnklst.h
               ${PROJECT_SOURCE_DIR}/include/xblnklstord.h
               ${PROJECT_SOURCE_DIR}/include/xblnknod.h
               ${PROJECT_SOURCE_DIR}/include/xblog.h
               ${PROJECT_SOURCE_DIR}/include/xbmemo.h
               ${PROJECT_SOURCE_DIR}/include/xbretcod.h
               ${PROJECT_SOURCE_DIR}/include/xbsql.h
               ${PROJECT_SOURCE_DIR}/include/xbssv.h
               ${PROJECT_SOURCE_DIR}/include/xbstring.h
               ${PROJECT_SOURCE_DIR}/include/xbtag.h
               ${PROJECT_SOURCE_DIR}/include/xbtblmgr.h
               ${PROJECT_SOURCE_DIR}/include/xbtypes.h
               ${PROJECT_SOURCE_DIR}/include/xbuda.h
               ${PROJECT_SOURCE_DIR}/include/xbxbase.h
         DESTINATION include/${CMAKE_PROJECT_NAME})

IF( NOT WIN32 )
install (FILES ${PROJECT_PARENT_DIR}/docs/man/xb_cfg_check.1.gz
               ${PROJECT_PARENT_DIR}/docs/man/xb_copydbf.1.gz
               ${PROJECT_PARENT_DIR}/docs/man/xb_dbfutil.1.gz
               ${PROJECT_PARENT_DIR}/docs/man/xb_deletall.1.gz
               ${PROJECT_PARENT_DIR}/docs/man/xb_dumpdbt.1.gz
               ${PROJECT_PARENT_DIR}/docs/man/xb_dumprecs.1.gz
               ${PROJECT_PARENT_DIR}/docs/man/xb_execsql.1.gz
               ${PROJECT_PARENT_DIR}/docs/man/xb_import.1.gz
               ${PROJECT_PARENT_DIR}/docs/man/xb_pack.1.gz
               ${PROJECT_PARENT_DIR}/docs/man/xb_reindex.1.gz
               ${PROJECT_PARENT_DIR}/docs/man/xb_tblinfo.1.gz
               ${PROJECT_PARENT_DIR}/docs/man/xb_undelall.1.gz

         DESTINATION man/man1)

ENDIF( NOT WIN32 )


SET_TARGET_PROPERTIES( ${XB_LIBNAME} PROPERTIES VERSION 4.1.7 SOVERSION 0 )

IF( WIN32 )
  install (TARGETS ${XB_LIBNAME}     DESTINATION bin)
ELSE( WIN32 )
  IF( EXISTS /usr/local/lib64 )
    install (TARGETS ${XB_LIBNAME}     DESTINATION lib64/${CMAKE_PROJECT_NAME})
  ELSE( EXISTS /usr/local/lib64 )
    install (TARGETS ${XB_LIBNAME}     DESTINATION lib/${CMAKE_PROJECT_NAME})
  ENDIF( EXISTS /usr/local/lib64 )
ENDIF( WIN32 )
