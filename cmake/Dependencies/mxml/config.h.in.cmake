/*
 * Include necessary headers...
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>


/*
 * Version number...
 */

#define MXML_VERSION ${MXML_VERSION}


/*
 * Inline function support...
 */

#define inline


/*
 * Long long support...
 */

#cmakedefine HAVE_LONG_LONG


/*
 * Do we have the snprintf() and vsnprintf() functions?
 */

#cmakedefine HAVE_SNPRINTF
#cmakedefine HAVE_VSNPRINTF


/*
 * Do we have the strXXX() functions?
 */

#cmakedefine HAVE_STRDUP


/*
 * Do we have threading support?
 */

#cmakedefine HAVE_PTHREAD_H


/*
 * Define prototypes for string functions as needed...
 */

#  ifndef HAVE_STRDUP
extern char	*_mxml_strdup(const char *);
#    define strdup _mxml_strdup
#  endif /* !HAVE_STRDUP */

extern char	*_mxml_strdupf(const char *, ...);
extern char	*_mxml_vstrdupf(const char *, va_list);

#  ifndef HAVE_SNPRINTF
extern int	_mxml_snprintf(char *, size_t, const char *, ...);
#    define snprintf _mxml_snprintf
#  endif /* !HAVE_SNPRINTF */

#  ifndef HAVE_VSNPRINTF
extern int	_mxml_vsnprintf(char *, size_t, const char *, va_list);
#    define vsnprintf _mxml_vsnprintf
#  endif /* !HAVE_VSNPRINTF */

