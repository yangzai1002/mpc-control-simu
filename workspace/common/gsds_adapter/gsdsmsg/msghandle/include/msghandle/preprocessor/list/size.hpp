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
# ifndef MSGHANDLE_PREPROCESSOR_LIST_SIZE_HPP
# define MSGHANDLE_PREPROCESSOR_LIST_SIZE_HPP
#
# include <msghandle/preprocessor/arithmetic/inc.hpp>
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/control/while.hpp>
# include <msghandle/preprocessor/list/adt.hpp>
# include <msghandle/preprocessor/tuple/elem.hpp>
# include <msghandle/preprocessor/tuple/rem.hpp>
#
# /* MSGHANDLE_PP_LIST_SIZE */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LIST_SIZE(list) MSGHANDLE_PP_TUPLE_ELEM(2, 0, MSGHANDLE_PP_WHILE(MSGHANDLE_PP_LIST_SIZE_P, MSGHANDLE_PP_LIST_SIZE_O, (0, list)))
# else
#    define MSGHANDLE_PP_LIST_SIZE(list) MSGHANDLE_PP_LIST_SIZE_I(list)
#    define MSGHANDLE_PP_LIST_SIZE_I(list) MSGHANDLE_PP_TUPLE_ELEM(2, 0, MSGHANDLE_PP_WHILE(MSGHANDLE_PP_LIST_SIZE_P, MSGHANDLE_PP_LIST_SIZE_O, (0, list)))
# endif
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LIST_SIZE_P(d, rl) MSGHANDLE_PP_LIST_IS_CONS(MSGHANDLE_PP_TUPLE_ELEM(2, 1, rl))
# else
#    define MSGHANDLE_PP_LIST_SIZE_P(d, rl) MSGHANDLE_PP_LIST_SIZE_P_I(MSGHANDLE_PP_TUPLE_REM_2 rl)
#    define MSGHANDLE_PP_LIST_SIZE_P_I(im) MSGHANDLE_PP_LIST_SIZE_P_II(im)
#    define MSGHANDLE_PP_LIST_SIZE_P_II(r, l) MSGHANDLE_PP_LIST_IS_CONS(l)
# endif
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LIST_SIZE_O(d, rl) (MSGHANDLE_PP_INC(MSGHANDLE_PP_TUPLE_ELEM(2, 0, rl)), MSGHANDLE_PP_LIST_REST(MSGHANDLE_PP_TUPLE_ELEM(2, 1, rl)))
# else
#    define MSGHANDLE_PP_LIST_SIZE_O(d, rl) MSGHANDLE_PP_LIST_SIZE_O_I(MSGHANDLE_PP_TUPLE_REM_2 rl)
#    define MSGHANDLE_PP_LIST_SIZE_O_I(im) MSGHANDLE_PP_LIST_SIZE_O_II(im)
#    define MSGHANDLE_PP_LIST_SIZE_O_II(r, l) (MSGHANDLE_PP_INC(r), MSGHANDLE_PP_LIST_REST(l))
# endif
#
# /* MSGHANDLE_PP_LIST_SIZE_D */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LIST_SIZE_D(d, list) MSGHANDLE_PP_TUPLE_ELEM(2, 0, MSGHANDLE_PP_WHILE_ ## d(MSGHANDLE_PP_LIST_SIZE_P, MSGHANDLE_PP_LIST_SIZE_O, (0, list)))
# else
#    define MSGHANDLE_PP_LIST_SIZE_D(d, list) MSGHANDLE_PP_LIST_SIZE_D_I(d, list)
#    define MSGHANDLE_PP_LIST_SIZE_D_I(d, list) MSGHANDLE_PP_TUPLE_ELEM(2, 0, MSGHANDLE_PP_WHILE_ ## d(MSGHANDLE_PP_LIST_SIZE_P, MSGHANDLE_PP_LIST_SIZE_O, (0, list)))
# endif
#
# endif
