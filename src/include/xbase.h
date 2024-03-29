/*  xbase.h


Xbase64 Software Library

Copyright (c) 1997,2003,2014,2022,2023 Gary A Kunkel

The xb64 software library is covered under the terms of the GPL Version 3, 2007 license

Email Contact:

    XDB-devel@lists.sourceforge.net
    XDB-users@lists.sourceforge.net

*/

#include <xbconfig.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <math.h>

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

// #ifdef STDC_HEADERS
#ifdef HAVE_STDARGS_H
#include <stdargs.h>
#endif

#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif

#ifdef HAVE_PWD_H
#include <pwd.h>
#endif

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif

#ifdef HAVE_STAT_H
#include <sys/stat.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_VARARGS_H
#ifndef HAVE_STDARG_H
#include <varargs.h>
#endif
#endif

#ifdef HAVE_WINDOWS_H
#include <windows.h>
#include <io.h>
#endif

#include <iomanip>

#include <xbretcod.h>
#include <xbtypes.h>
#include <xbstring.h>
#include <xbssv.h>
#include <xbdate.h>

#include <xblnknod.h>
#include <xblnklst.h>
#include <xblnklstord.h>

#include <xbtblmgr.h>
#include <xbxbase.h>

//#include <xblnknod.h>
//#include <xblnklst.h>
//#include <xblnklstord.h>

#include <xbfile.h>
#include <xblog.h>
#include <xbmemo.h>
#include <xbbcd.h>
#include <xbuda.h>
#include <xbexpnode.h>
#include <xbexp.h>

#include <xbtag.h>
#include <xbblkread.h>
#include <xbdbf.h>             /* dbf base class   */
#include <xbindex.h>           /* index base class */
#include <xbfilter.h>
#include <xbrelate.h>
#include <xbsql.h>
