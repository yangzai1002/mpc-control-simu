#ifndef MSGHANDLE_CPP_VERSION_HPP
#define MSGHANDLE_CPP_VERSION_HPP

#if defined(_MSC_VER)
#  if _MSC_VER < 1900
#    define MSGHANDLE_USE_CPP03
#  endif
#elif (__cplusplus < 201103L)
#  define MSGHANDLE_USE_CPP03
#endif

#if defined(_MSVC_LANG)
#  define MSGHANDLE_CPP_VERSION _MSVC_LANG
#else
#  define MSGHANDLE_CPP_VERSION __cplusplus
#endif

#endif // MSGHANDLE_CPP_VERSION_HPP
