# /* Copyright (C) 2001
#  * Housemarque Oy
#  * http://www.housemarque.com
#  *
#  * Distributed under the Boost Software License, Version 1.0. (See
#  * accompanying file LICENSE_1_0.txt or copy at
#  * http://www.boost.org/LICENSE_1_0.txt)
#  */
#
# /* Revised by Paul Mensonides (2002) */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef MSGHANDLE_PREPROCESSOR_ARITHMETIC_DIV_HPP
# define MSGHANDLE_PREPROCESSOR_ARITHMETIC_DIV_HPP
#
# include <msghandle/preprocessor/arithmetic/detail/div_base.hpp>
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/tuple/elem.hpp>
#
# /* MSGHANDLE_PP_DIV */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_DIV(x, y) MSGHANDLE_PP_TUPLE_ELEM(3, 0, MSGHANDLE_PP_DIV_BASE(x, y))
# else
#    define MSGHANDLE_PP_DIV(x, y) MSGHANDLE_PP_DIV_I(x, y)
#    define MSGHANDLE_PP_DIV_I(x, y) MSGHANDLE_PP_TUPLE_ELEM(3, 0, MSGHANDLE_PP_DIV_BASE(x, y))
# endif
#
# /* MSGHANDLE_PP_DIV_D */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_DIV_D(d, x, y) MSGHANDLE_PP_TUPLE_ELEM(3, 0, MSGHANDLE_PP_DIV_BASE_D(d, x, y))
# else
#    define MSGHANDLE_PP_DIV_D(d, x, y) MSGHANDLE_PP_DIV_D_I(d, x, y)
#    define MSGHANDLE_PP_DIV_D_I(d, x, y) MSGHANDLE_PP_TUPLE_ELEM(3, 0, MSGHANDLE_PP_DIV_BASE_D(d, x, y))
# endif
#
# endif
