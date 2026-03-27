# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2002.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.boost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef MSGHANDLE_PREPROCESSOR_DETAIL_IS_BINARY_HPP
# define MSGHANDLE_PREPROCESSOR_DETAIL_IS_BINARY_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/detail/check.hpp>
#
# /* MSGHANDLE_PP_IS_BINARY */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_IS_BINARY(x) MSGHANDLE_PP_CHECK(x, MSGHANDLE_PP_IS_BINARY_CHECK)
# else
#    define MSGHANDLE_PP_IS_BINARY(x) MSGHANDLE_PP_IS_BINARY_I(x)
#    define MSGHANDLE_PP_IS_BINARY_I(x) MSGHANDLE_PP_CHECK(x, MSGHANDLE_PP_IS_BINARY_CHECK)
# endif
#
# define MSGHANDLE_PP_IS_BINARY_CHECK(a, b) 1
# define MSGHANDLE_PP_CHECK_RESULT_MSGHANDLE_PP_IS_BINARY_CHECK 0, MSGHANDLE_PP_NIL
#
# endif
