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
# ifndef MSGHANDLE_PREPROCESSOR_FACILITIES_IS_EMPTY_VARIADIC_HPP
# define MSGHANDLE_PREPROCESSOR_FACILITIES_IS_EMPTY_VARIADIC_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
#
# if MSGHANDLE_PP_VARIADICS
#
# include <msghandle/preprocessor/punctuation/is_begin_parens.hpp>
# include <msghandle/preprocessor/facilities/detail/is_empty.hpp>
#
#if MSGHANDLE_PP_VARIADICS_MSVC && _MSC_VER <= 1400
#
#define MSGHANDLE_PP_IS_EMPTY(param) \
    MSGHANDLE_PP_DETAIL_IS_EMPTY_IIF \
      ( \
      MSGHANDLE_PP_IS_BEGIN_PARENS \
        ( \
        param \
        ) \
      ) \
      ( \
      MSGHANDLE_PP_IS_EMPTY_ZERO, \
      MSGHANDLE_PP_DETAIL_IS_EMPTY_PROCESS \
      ) \
    (param) \
/**/
#define MSGHANDLE_PP_IS_EMPTY_ZERO(param) 0
# else
#define MSGHANDLE_PP_IS_EMPTY(...) \
    MSGHANDLE_PP_DETAIL_IS_EMPTY_IIF \
      ( \
      MSGHANDLE_PP_IS_BEGIN_PARENS \
        ( \
        __VA_ARGS__ \
        ) \
      ) \
      ( \
      MSGHANDLE_PP_IS_EMPTY_ZERO, \
      MSGHANDLE_PP_DETAIL_IS_EMPTY_PROCESS \
      ) \
    (__VA_ARGS__) \
/**/
#define MSGHANDLE_PP_IS_EMPTY_ZERO(...) 0
# endif /* MSGHANDLE_PP_VARIADICS_MSVC && _MSC_VER <= 1400 */
# endif /* MSGHANDLE_PP_VARIADICS */
# endif /* MSGHANDLE_PREPROCESSOR_FACILITIES_IS_EMPTY_VARIADIC_HPP */
