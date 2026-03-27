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
# ifndef MSGHANDLE_PREPROCESSOR_COMPARISON_LESS_HPP
# define MSGHANDLE_PREPROCESSOR_COMPARISON_LESS_HPP
#
# include <msghandle/preprocessor/comparison/less_equal.hpp>
# include <msghandle/preprocessor/comparison/not_equal.hpp>
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/control/iif.hpp>
# include <msghandle/preprocessor/logical/bitand.hpp>
# include <msghandle/preprocessor/tuple/eat.hpp>
#
# /* MSGHANDLE_PP_LESS */
#
# if MSGHANDLE_PP_CONFIG_FLAGS() & (MSGHANDLE_PP_CONFIG_MWCC() | MSGHANDLE_PP_CONFIG_DMC())
#    define MSGHANDLE_PP_LESS(x, y) MSGHANDLE_PP_BITAND(MSGHANDLE_PP_NOT_EQUAL(x, y), MSGHANDLE_PP_LESS_EQUAL(x, y))
# elif ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LESS(x, y) MSGHANDLE_PP_IIF(MSGHANDLE_PP_NOT_EQUAL(x, y), MSGHANDLE_PP_LESS_EQUAL, 0 MSGHANDLE_PP_TUPLE_EAT_2)(x, y)
# else
#    define MSGHANDLE_PP_LESS(x, y) MSGHANDLE_PP_LESS_I(x, y)
#    define MSGHANDLE_PP_LESS_I(x, y) MSGHANDLE_PP_IIF(MSGHANDLE_PP_NOT_EQUAL(x, y), MSGHANDLE_PP_LESS_EQUAL, 0 MSGHANDLE_PP_TUPLE_EAT_2)(x, y)
# endif
#
# /* MSGHANDLE_PP_LESS_D */
#
# if MSGHANDLE_PP_CONFIG_FLAGS() & (MSGHANDLE_PP_CONFIG_MWCC() | MSGHANDLE_PP_CONFIG_DMC())
#    define MSGHANDLE_PP_LESS_D(d, x, y) MSGHANDLE_PP_BITAND(MSGHANDLE_PP_NOT_EQUAL(x, y), MSGHANDLE_PP_LESS_EQUAL_D(d, x, y))
# elif ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LESS_D(d, x, y) MSGHANDLE_PP_IIF(MSGHANDLE_PP_NOT_EQUAL(x, y), MSGHANDLE_PP_LESS_EQUAL_D, 0 MSGHANDLE_PP_TUPLE_EAT_3)(d, x, y)
# else
#    define MSGHANDLE_PP_LESS_D(d, x, y) MSGHANDLE_PP_LESS_D_I(d, x, y)
#    define MSGHANDLE_PP_LESS_D_I(d, x, y) MSGHANDLE_PP_IIF(MSGHANDLE_PP_NOT_EQUAL(x, y), MSGHANDLE_PP_LESS_EQUAL_D, 0 MSGHANDLE_PP_TUPLE_EAT_3)(d, x, y)
# endif
#
# endif
