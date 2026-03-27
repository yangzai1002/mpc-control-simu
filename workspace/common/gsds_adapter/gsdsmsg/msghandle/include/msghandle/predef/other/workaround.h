/*
Copyright Rene Rivera 2017
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_WORKAROUND_H
#define MSGHANDLE_PREDEF_WORKAROUND_H

/*`
[heading `MSGHANDLE_PREDEF_WORKAROUND`]

``
MSGHANDLE_PREDEF_WORKAROUND(symbol,comp,major,minor,patch)
``

Usage:

``
#if MSGHANDLE_PREDEF_WORKAROUND(MSGHANDLE_COMP_CLANG,<,3,0,0)
    // Workaround for old clang compilers..
#endif
``

Defines a comparison against two version numbers that depends on the definion
of `MSGHANDLE_STRICT_CONFIG`. When `MSGHANDLE_STRICT_CONFIG` is defined this will expand
to a value convertible to `false`. Which has the effect of disabling all code
conditionally guarded by `MSGHANDLE_PREDEF_WORKAROUND`. When `MSGHANDLE_STRICT_CONFIG`
is undefine this expand to test the given `symbol` version value with the
`comp` comparison against `MSGHANDLE_VERSION_NUMBER(major,minor,patch)`.
*/
#ifdef MSGHANDLE_STRICT_CONFIG
#   define MSGHANDLE_PREDEF_WORKAROUND(symbol, comp, major, minor, patch) (0)
#else
#   include <msghandle/predef/version_number.h>
#   define MSGHANDLE_PREDEF_WORKAROUND(symbol, comp, major, minor, patch) \
        ( (symbol) != (0) ) && \
        ( (symbol) comp (MSGHANDLE_VERSION_NUMBER( (major) , (minor) , (patch) )) )
#endif

/*`
[heading `MSGHANDLE_PREDEF_TESTED_AT`]

``
MSGHANDLE_PREDEF_TESTED_AT(symbol,major,minor,patch)
``

Usage:

``
#if MSGHANDLE_PREDEF_TESTED_AT(MSGHANDLE_COMP_CLANG,3,5,0)
    // Needed for clang, and last checked for 3.5.0.
#endif
``

Defines a comparison against two version numbers that depends on the definion
of `MSGHANDLE_STRICT_CONFIG` and `MSGHANDLE_DETECT_OUTDATED_WORKAROUNDS`.
When `MSGHANDLE_STRICT_CONFIG` is defined this will expand to a value convertible
to `false`. Which has the effect of disabling all code
conditionally guarded by `MSGHANDLE_PREDEF_TESTED_AT`. When `MSGHANDLE_STRICT_CONFIG`
is undefined this expand to either:

* A value convertible to `true` when `MSGHANDLE_DETECT_OUTDATED_WORKAROUNDS` is not
  defined.
* A value convertible `true` when the expansion of
  `MSGHANDLE_PREDEF_WORKAROUND(symbol, <=, major, minor, patch)` is `true` and
  `MSGHANDLE_DETECT_OUTDATED_WORKAROUNDS` is defined.
* A compile error when the expansion of
  `MSGHANDLE_PREDEF_WORKAROUND(symbol, >, major, minor, patch)` is true and
  `MSGHANDLE_DETECT_OUTDATED_WORKAROUNDS` is defined.
*/
#ifdef MSGHANDLE_STRICT_CONFIG
#   define MSGHANDLE_PREDEF_TESTED_AT(symbol, major, minor, patch) (0)
#else
#   ifdef MSGHANDLE_DETECT_OUTDATED_WORKAROUNDS
#       define MSGHANDLE_PREDEF_TESTED_AT(symbol, major, minor, patch) ( \
            MSGHANDLE_PREDEF_WORKAROUND(symbol, <=, major, minor, patch) \
            ? 1 \
            : (1%0) )
#   else
#       define MSGHANDLE_PREDEF_TESTED_AT(symbol, major, minor, patch) \
            ( (symbol) >= MSGHANDLE_VERSION_NUMBER_AVAILABLE )
#   endif
#endif

#endif
