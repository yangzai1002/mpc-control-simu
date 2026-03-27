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
# ifndef MSGHANDLE_PREPROCESSOR_ARITHMETIC_DETAIL_DIV_BASE_HPP
# define MSGHANDLE_PREPROCESSOR_ARITHMETIC_DETAIL_DIV_BASE_HPP
#
# include <msghandle/preprocessor/arithmetic/inc.hpp>
# include <msghandle/preprocessor/arithmetic/sub.hpp>
# include <msghandle/preprocessor/comparison/less_equal.hpp>
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/control/while.hpp>
# include <msghandle/preprocessor/tuple/elem.hpp>
# include <msghandle/preprocessor/tuple/rem.hpp>
#
# /* MSGHANDLE_PP_DIV_BASE */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_DIV_BASE(x, y) MSGHANDLE_PP_WHILE(MSGHANDLE_PP_DIV_BASE_P, MSGHANDLE_PP_DIV_BASE_O, (0, x, y))
# else
#    define MSGHANDLE_PP_DIV_BASE(x, y) MSGHANDLE_PP_DIV_BASE_I(x, y)
#    define MSGHANDLE_PP_DIV_BASE_I(x, y) MSGHANDLE_PP_WHILE(MSGHANDLE_PP_DIV_BASE_P, MSGHANDLE_PP_DIV_BASE_O, (0, x, y))
# endif
#
# if MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_STRICT()
#    define MSGHANDLE_PP_DIV_BASE_P(d, rxy) MSGHANDLE_PP_DIV_BASE_P_IM(d, MSGHANDLE_PP_TUPLE_REM_3 rxy)
#    define MSGHANDLE_PP_DIV_BASE_P_IM(d, im) MSGHANDLE_PP_DIV_BASE_P_I(d, im)
# else
#    define MSGHANDLE_PP_DIV_BASE_P(d, rxy) MSGHANDLE_PP_DIV_BASE_P_I(d, MSGHANDLE_PP_TUPLE_ELEM(3, 0, rxy), MSGHANDLE_PP_TUPLE_ELEM(3, 1, rxy), MSGHANDLE_PP_TUPLE_ELEM(3, 2, rxy))
# endif
#
# define MSGHANDLE_PP_DIV_BASE_P_I(d, r, x, y) MSGHANDLE_PP_LESS_EQUAL_D(d, y, x)
#
# if MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_STRICT()
#    define MSGHANDLE_PP_DIV_BASE_O(d, rxy) MSGHANDLE_PP_DIV_BASE_O_IM(d, MSGHANDLE_PP_TUPLE_REM_3 rxy)
#    define MSGHANDLE_PP_DIV_BASE_O_IM(d, im) MSGHANDLE_PP_DIV_BASE_O_I(d, im)
# else
#    define MSGHANDLE_PP_DIV_BASE_O(d, rxy) MSGHANDLE_PP_DIV_BASE_O_I(d, MSGHANDLE_PP_TUPLE_ELEM(3, 0, rxy), MSGHANDLE_PP_TUPLE_ELEM(3, 1, rxy), MSGHANDLE_PP_TUPLE_ELEM(3, 2, rxy))
# endif
#
# define MSGHANDLE_PP_DIV_BASE_O_I(d, r, x, y) (MSGHANDLE_PP_INC(r), MSGHANDLE_PP_SUB_D(d, x, y), y)
#
# /* MSGHANDLE_PP_DIV_BASE_D */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_DIV_BASE_D(d, x, y) MSGHANDLE_PP_WHILE_ ## d(MSGHANDLE_PP_DIV_BASE_P, MSGHANDLE_PP_DIV_BASE_O, (0, x, y))
# else
#    define MSGHANDLE_PP_DIV_BASE_D(d, x, y) MSGHANDLE_PP_DIV_BASE_D_I(d, x, y)
#    define MSGHANDLE_PP_DIV_BASE_D_I(d, x, y) MSGHANDLE_PP_WHILE_ ## d(MSGHANDLE_PP_DIV_BASE_P, MSGHANDLE_PP_DIV_BASE_O, (0, x, y))
# endif
#
# endif
