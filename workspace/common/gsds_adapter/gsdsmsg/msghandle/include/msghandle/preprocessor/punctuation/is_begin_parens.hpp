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
# ifndef MSGHANDLE_PREPROCESSOR_IS_BEGIN_PARENS_HPP
# define MSGHANDLE_PREPROCESSOR_IS_BEGIN_PARENS_HPP

# include <msghandle/preprocessor/config/config.hpp>

#if MSGHANDLE_PP_VARIADICS

#include <msghandle/preprocessor/punctuation/detail/is_begin_parens.hpp>

#if MSGHANDLE_PP_VARIADICS_MSVC && _MSC_VER <= 1400

#define MSGHANDLE_PP_IS_BEGIN_PARENS(param) \
    MSGHANDLE_PP_DETAIL_IBP_SPLIT \
      ( \
      0, \
      MSGHANDLE_PP_DETAIL_IBP_CAT \
        ( \
        MSGHANDLE_PP_DETAIL_IBP_IS_VARIADIC_R_, \
        MSGHANDLE_PP_DETAIL_IBP_IS_VARIADIC_C param \
        ) \
      ) \
/**/

#else

#define MSGHANDLE_PP_IS_BEGIN_PARENS(...) \
    MSGHANDLE_PP_DETAIL_IBP_SPLIT \
      ( \
      0, \
      MSGHANDLE_PP_DETAIL_IBP_CAT \
        ( \
        MSGHANDLE_PP_DETAIL_IBP_IS_VARIADIC_R_, \
        MSGHANDLE_PP_DETAIL_IBP_IS_VARIADIC_C __VA_ARGS__ \
        ) \
      ) \
/**/

#endif /* MSGHANDLE_PP_VARIADICS_MSVC && _MSC_VER <= 1400 */
#endif /* MSGHANDLE_PP_VARIADICS */
#endif /* MSGHANDLE_PREPROCESSOR_IS_BEGIN_PARENS_HPP */
