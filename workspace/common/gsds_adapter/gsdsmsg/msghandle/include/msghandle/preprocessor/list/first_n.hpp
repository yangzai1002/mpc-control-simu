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
# ifndef MSGHANDLE_PREPROCESSOR_LIST_FIRST_N_HPP
# define MSGHANDLE_PREPROCESSOR_LIST_FIRST_N_HPP
#
# include <msghandle/preprocessor/arithmetic/dec.hpp>
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/control/while.hpp>
# include <msghandle/preprocessor/list/adt.hpp>
# include <msghandle/preprocessor/list/reverse.hpp>
# include <msghandle/preprocessor/tuple/elem.hpp>
# include <msghandle/preprocessor/tuple/rem.hpp>
#
# /* MSGHANDLE_PP_LIST_FIRST_N */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LIST_FIRST_N(count, list) MSGHANDLE_PP_LIST_REVERSE(MSGHANDLE_PP_TUPLE_ELEM(3, 2, MSGHANDLE_PP_WHILE(MSGHANDLE_PP_LIST_FIRST_N_P, MSGHANDLE_PP_LIST_FIRST_N_O, (count, list, MSGHANDLE_PP_NIL))))
# else
#    define MSGHANDLE_PP_LIST_FIRST_N(count, list) MSGHANDLE_PP_LIST_FIRST_N_I(count, list)
#    define MSGHANDLE_PP_LIST_FIRST_N_I(count, list) MSGHANDLE_PP_LIST_REVERSE(MSGHANDLE_PP_TUPLE_ELEM(3, 2, MSGHANDLE_PP_WHILE(MSGHANDLE_PP_LIST_FIRST_N_P, MSGHANDLE_PP_LIST_FIRST_N_O, (count, list, MSGHANDLE_PP_NIL))))
# endif
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LIST_FIRST_N_P(d, data) MSGHANDLE_PP_TUPLE_ELEM(3, 0, data)
# else
#    define MSGHANDLE_PP_LIST_FIRST_N_P(d, data) MSGHANDLE_PP_LIST_FIRST_N_P_I data
#    define MSGHANDLE_PP_LIST_FIRST_N_P_I(c, l, nl) c
# endif
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MWCC()
#    define MSGHANDLE_PP_LIST_FIRST_N_O(d, data) MSGHANDLE_PP_LIST_FIRST_N_O_D data
# else
#    define MSGHANDLE_PP_LIST_FIRST_N_O(d, data) MSGHANDLE_PP_LIST_FIRST_N_O_D(MSGHANDLE_PP_TUPLE_ELEM(3, 0, data), MSGHANDLE_PP_TUPLE_ELEM(3, 1, data), MSGHANDLE_PP_TUPLE_ELEM(3, 2, data))
# endif
#
# define MSGHANDLE_PP_LIST_FIRST_N_O_D(c, l, nl) (MSGHANDLE_PP_DEC(c), MSGHANDLE_PP_LIST_REST(l), (MSGHANDLE_PP_LIST_FIRST(l), nl))
#
# /* MSGHANDLE_PP_LIST_FIRST_N_D */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LIST_FIRST_N_D(d, count, list) MSGHANDLE_PP_LIST_REVERSE_D(d, MSGHANDLE_PP_TUPLE_ELEM(3, 2, MSGHANDLE_PP_WHILE_ ## d(MSGHANDLE_PP_LIST_FIRST_N_P, MSGHANDLE_PP_LIST_FIRST_N_O, (count, list, MSGHANDLE_PP_NIL))))
# else
#    define MSGHANDLE_PP_LIST_FIRST_N_D(d, count, list) MSGHANDLE_PP_LIST_FIRST_N_D_I(d, count, list)
#    define MSGHANDLE_PP_LIST_FIRST_N_D_I(d, count, list) MSGHANDLE_PP_LIST_REVERSE_D(d, MSGHANDLE_PP_TUPLE_ELEM(3, 2, MSGHANDLE_PP_WHILE_ ## d(MSGHANDLE_PP_LIST_FIRST_N_P, MSGHANDLE_PP_LIST_FIRST_N_O, (count, list, MSGHANDLE_PP_NIL))))
# endif
#
# endif
