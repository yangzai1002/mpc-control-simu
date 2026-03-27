# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Edward Diener 2014.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.boost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
#ifndef MSGHANDLE_PREPROCESSOR_DETAIL_IS_EMPTY_HPP
#define MSGHANDLE_PREPROCESSOR_DETAIL_IS_EMPTY_HPP

#include <msghandle/preprocessor/punctuation/is_begin_parens.hpp>

#if MSGHANDLE_PP_VARIADICS_MSVC

# pragma warning(once:4002)

#define MSGHANDLE_PP_DETAIL_IS_EMPTY_IIF_0(t, b) b
#define MSGHANDLE_PP_DETAIL_IS_EMPTY_IIF_1(t, b) t

#else

#define MSGHANDLE_PP_DETAIL_IS_EMPTY_IIF_0(t, ...) __VA_ARGS__
#define MSGHANDLE_PP_DETAIL_IS_EMPTY_IIF_1(t, ...) t

#endif

#if MSGHANDLE_PP_VARIADICS_MSVC && _MSC_VER <= 1400

#define MSGHANDLE_PP_DETAIL_IS_EMPTY_PROCESS(param) \
	MSGHANDLE_PP_IS_BEGIN_PARENS \
    	( \
        MSGHANDLE_PP_DETAIL_IS_EMPTY_NON_FUNCTION_C param () \
        ) \
/**/

#else

#define MSGHANDLE_PP_DETAIL_IS_EMPTY_PROCESS(...) \
	MSGHANDLE_PP_IS_BEGIN_PARENS \
        ( \
        MSGHANDLE_PP_DETAIL_IS_EMPTY_NON_FUNCTION_C __VA_ARGS__ () \
        ) \
/**/

#endif

#define MSGHANDLE_PP_DETAIL_IS_EMPTY_PRIMITIVE_CAT(a, b) a ## b
#define MSGHANDLE_PP_DETAIL_IS_EMPTY_IIF(bit) MSGHANDLE_PP_DETAIL_IS_EMPTY_PRIMITIVE_CAT(MSGHANDLE_PP_DETAIL_IS_EMPTY_IIF_,bit)
#define MSGHANDLE_PP_DETAIL_IS_EMPTY_NON_FUNCTION_C(...) ()

#endif /* MSGHANDLE_PREPROCESSOR_DETAIL_IS_EMPTY_HPP */
